// SPDX-License-Identifier: Apache-2.0
// Copyright 2019 Gliim LLC. 
// Licensed under Apache License v2. See LICENSE file.
// On the web http://golf-lang.com/ - this file is part of Golf framework.

//
// Handling of fatal crashes, or controlled program aborts.
// The purpose is to create a backtrace file which contains the
// stack, together with source file name/line numbers, when a 
// crash occurs.
//
// For the source code/line number reporting to work,  -g and
// -rdynamic (most likely) must be used. 
//

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <assert.h>
#include <execinfo.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <assert.h>
#include <unistd.h>
#include <time.h>
#include <link.h>
#include <sys/resource.h>
#include "golf.h"

// *******************
// NO CALLS TO CODE OUTSIDE OF THIS MODULE MUST BE MADE AND NO GG_TRACE()!!
// *******************
// Otherwise, those calls' tracing would always place
// the last 'traced' (i.e. visited) location, right there
// and not in the place where it happened
// Meaning, as a crash-handling code, all of it is right here.
 
// limitations on stack size, intended to be reasonably well sized
//
#define MAX_STACK_FRAMES 512
#define MAX_EXPL_LEN 1024


// Describes the ELF segments of shared libraries used, in particular the name and address range for each
// so that at run time we find out for each stack element where exactly it comes from - and determine
// the exact source code location. We assume program has less than MAX_SO shared objects used
#define MAX_SO 100

// these are generally set elsewhere for usage here, they provide
// additional information about where we were when crash happened
volatile gg_num gg_end_program=0; // when SIGTERM is received, set this to 1 to exit gracefully
extern gg_num gg_in_request;

// Static variables to be used in the case of a crash
static void *stack_dump[MAX_STACK_FRAMES]; // stack frame`
static char timestr[100];
static char expla[MAX_EXPL_LEN + 1];
static char backtrace_file[600];
static char backtrace_start[sizeof(backtrace_file)+1500];
static gg_so_info so[MAX_SO]; // info on all shared libraries linked with this program
static gg_num total_so = 0; // total number of shared libraries we found on startup

// function prototypes
gg_num addr2line(void const * const addr, char *fname);
void posix_print_stack_trace();
void signal_handler(int sig);
void set_signal_handler();
void gg_get_time_crash (char *outstr, gg_num outstrLen);
int modinfo(struct dl_phdr_info *info, size_t size, void *data);


// 
// Resolve symbol name and source location given the path to the executable 
// and an address 
// addr is program address for which to find line #.
// fname is file name where to write.
// Returns exit code from addr2line
// This function is called multiple times (once for each line
//      on backtrace), so we use >> to add to output
//
gg_num addr2line(void const * const addr, char *fname)
{
    char addr2line_cmd[512] = {0};
    assert (fname);
    assert (addr);
    gg_num it;

    //
    // Go through all shared objects and find out where is the address on the stack located.
    // This allows us to find base address and shared object name, and thus the source code location.
    //
    for (it = 0; it < total_so; it++)
    {
       //
       // Is address in question between the start and end address of shared object?
       //
       if (so[it].mod_addr<=addr && addr<=so[it].mod_end) 
       {
           break;
       }
    }


    if (it == total_so)
    {
        //
        // This should NEVER happen, we couldn't find the address in any shared object!! We just default to first one
        // even if it won't really work
        //
        it = 0;
    }

    if (strstr (so[it].mod_name, "linux-vdso.so.1") == NULL)
    {

        // get line information for an address, and put it into a backtrace file
        // that has timestamp and process number. We do this by finding the actual RELATIVE
        // address of the fault address (addr) and the load address of this executable module (mod_addr)
        snprintf(addr2line_cmd, sizeof(addr2line_cmd), "addr2line -f -e %s 0x%lx |grep -v \"??\" >> %s", so[it].mod_name, (unsigned long)(addr-so[it].mod_addr+so[it].mod_offset),
            fname);
    
        // execute addr2line, which is a Linux utility that does this 
        return system(addr2line_cmd);
    }
    else
        return 0;
}

// 
// Get stack trace for current execution, then abort program.
//
void posix_print_stack_trace()
{
    gg_get_stack(backtrace_file);
    gg_report_error ("Program received a signal, see backtrace file");
}

// 
// Obtain backtrace, and write information to output file fname.
// Obtain each stack item and process it to obtain file name and line number.
// Print out along side other information for debugging and post-mortem.
// Do not call this more than once, because certain failures may loop back here
// and go into infinite loop, destroying all useful information.
//
void gg_get_stack(char *fname)
{
#ifndef DEBUG
    return; // do not bog down production with stack printout, only if debugging mode
#endif

    //
    // This static variable is okay because if we're here, the program WILL end right here in this module.
    // So after it restarts, this static variable will re-initialize.
    //
    static gg_num was_here = 0;
    gg_num i = 0;
    gg_num trace_size = 0;
    char **dump_msg = (char **)NULL;
    gg_num rs;

    if (was_here == 1)
        return;

    was_here = 1;
    GG_UNUSED(rs);

    // set a hook for module loading, so we go through all modules loaded
    // and then figure out the one we're in and get the info needed for
    // source code/line number resolution. 
    dl_iterate_phdr(&modinfo, NULL);


    // get stack and symbols
    trace_size = backtrace(stack_dump, MAX_STACK_FRAMES);
    dump_msg = backtrace_symbols(stack_dump, trace_size);
 
    snprintf(backtrace_start, sizeof(backtrace_start), "echo 'START STACK DUMP ***********' >> %s", fname);
    rs = system (backtrace_start);

    gg_get_time_crash (timestr, sizeof(timestr)-1);
    snprintf(backtrace_start, sizeof(backtrace_start), "echo '%d: %s: %s' >> %s", getpid(), timestr, expla, backtrace_file);
    rs = system (backtrace_start);

    // get source and line number for each stack line item
    for (i = 0; i < trace_size; ++i)
    {
        // try to display what we can
        // we don't check for return value because some lines with ??
        // are filtered out (we only look for source line in the module we originate from)

        // divide stack entries
        snprintf(backtrace_start, sizeof(backtrace_start), "echo '-----' >> %s",  fname);
        rs = system (backtrace_start);

        // display source file/line number
        addr2line(stack_dump[i], fname);
        snprintf(backtrace_start, sizeof(backtrace_start), "echo '%s' >> %s", dump_msg[i], fname);
        rs = system (backtrace_start);
    }
    snprintf(backtrace_start, sizeof(backtrace_start), "echo 'END STACK DUMP ***********' >> %s", fname);
    rs = system (backtrace_start);

    // skip freeing to avoid potential issues SIGKILL
    //if (dump_msg) { free(dump_msg); } 

}

// 
// Signal handler for signal sig. sig is signal number
// This way at run time we know which signal was caught. We also core dump for 
// more information.
// NO GLIIMLY MEMORY HANDLING HERE
// This will either end program (due to request to end it, or some fatal condition), or set the flag
// to end it once the current request completes, and that's all it can do.
//
void signal_handler(int sig)
{

    // this code MUST REMAIN SPARSE and use only basic ops, as it handles jumping 
    // here, unless this is SIGTERM, this is fatal and the process EXITS

    // make sure no surprises with longjumps, disable them right away. gg_done_err_setjmp is used
    // for report-error to go to the next request, and gg_done_setjmp for exit-service. Neither should
    // potentially do these, because this is fatal and must exit, so we allow no jumps elsewhere that would
    // prevent fatal exit.
    gg_done_err_setjmp = 0;
    gg_done_setjmp = 0;


    // set to make sure gg_report_error does not exit, but lets this function go through the end to report
    // on what's really happening
    gg_in_fatal_exit = 1;


    switch(sig)
    {
        case SIGFPE:
            gg_strncpy(expla, "Caught SIGFPE: math exception, such as divide by zero\n",
                MAX_EXPL_LEN - 1);
            break;
        case SIGILL:
            gg_strncpy(expla, "Caught SIGILL: illegal code\n",  MAX_EXPL_LEN - 1);
            break;
        case SIGABRT:
        case SIGBUS:
        case SIGSEGV:
            if (sig == SIGABRT) gg_strncpy(expla, "Caught SIGABRT: usually caused by an abort() or assert()\n", MAX_EXPL_LEN - 1);
            if (sig == SIGBUS) gg_strncpy(expla, "Caught SIGBUS: bus error\n",  MAX_EXPL_LEN - 1);
            if (sig == SIGSEGV) gg_strncpy(expla, "Caught SIGSEGV: segmentation fault\n",  MAX_EXPL_LEN - 1);
            break;
        case SIGHUP:
            gg_strncpy(expla, "Caught SIGHUP: hang up\n",  MAX_EXPL_LEN - 1);
            break;
        case SIGTERM:
            gg_end_program = 1;
            if (gg_in_request == 0) 
            {
                gg_strncpy(expla, "Caught SIGTERM: request for graceful shutdown, shutting down now as I am not processing a request\n",  MAX_EXPL_LEN - 1);
                // since we're not processing request, req is undefined and will make gg_report_error() crash (we will
                // inevitably call it as part of quitting).
                // so we make it NULL, since gg_report_error() guards against that.
                gg_get_config()->ctx.req = NULL;
                break;
            }
            else
            {
                gg_strncpy(expla, "Caught SIGTERM: request for graceful shutdown, will shutdown once a request is processed\n",  MAX_EXPL_LEN - 1);
            }
            return;
        default:
            // this really should not happen since we handled all the signals we trapped, just in case
            snprintf(expla, sizeof(expla), "Caught something not handled, signal [%d]\n", sig);
            break;
    }

    snprintf(backtrace_start, sizeof(backtrace_start), "echo '***\n***\n***\n' >> %s", backtrace_file);
    gg_num rs = system (backtrace_start);
    GG_UNUSED(rs);

    // 
    // Printout stack trace
    //
    posix_print_stack_trace();


    GG_FATAL ("Exiting because of the signal: [%s]", expla);
}
 

// 
// Set each signal handler, this must be called asap in the program
//
void set_signal_handler()
{
    struct sigaction psa;
    memset (&psa, 0, sizeof (psa));
    psa.sa_handler = signal_handler;
    // We do not set psa.sa_flags to SA_RESTART because vf -d process management depends on
    // properly interrupting the read() and such
    if (sigaction(SIGABRT, &psa, NULL) == -1) GG_FATAL ("Cannot set ABRT signal handler");
    if (sigaction(SIGFPE,  &psa, NULL) == -1) GG_FATAL ("Cannot set FPE signal handler");
    if (sigaction(SIGILL,  &psa, NULL) == -1) GG_FATAL ("Cannot set ILL signal handler");
    if (sigaction(SIGSEGV, &psa, NULL) == -1) GG_FATAL ("Cannot set SEGV signal handler");
    if (sigaction(SIGBUS, &psa, NULL) == -1) GG_FATAL ("Cannot set BUS signal handler");
    if (sigaction(SIGTERM, &psa, NULL) == -1) GG_FATAL ("Cannot set TERM signal handler");
    if (sigaction(SIGHUP, &psa, NULL) == -1) GG_FATAL ("Cannot set HUP signal handler");
    // ignore these
    signal(SIGPIPE, SIG_IGN); // ignore broken pipe
    signal(SIGINT, SIG_IGN); // ignore ctrl c and such
    signal(SIGUSR1, SIG_IGN); // ignore as it has no meaning for Golf
    signal(SIGUSR2, SIG_IGN); // ignore as it has no meaning for Golf
    // CANNOT ignore SIGCHLD because we DO need status from them
    // DO NOT do anything with SIGALRM - curl uses it for timeouts
}



// 
// Obtain load start address for current executable module. This must be 
// deducted from an address obtained by backtrace.. in order to have proper
// source/line number information available.
// Returns 0.
//
int modinfo(struct dl_phdr_info *info, size_t size, void *data)
{
    // set as unused as API is broader than what we need
    GG_UNUSED(size);
    GG_UNUSED(data);


    gg_num i;

    // go through a list of segments loaded for this module and pick one we're in
    // and make sure we get the loading address
    for (i = 0; i < info->dlpi_phnum; i++) 
    {
        // get start load address of module - look at all that are executable (PF_X) - this
        // covers not just shared libs but the main executable too
        if (info->dlpi_phdr[i].p_type == PT_LOAD && (info->dlpi_phdr[i].p_flags & PF_X))
        {
            // this is global module address we will use in addr2line as a base to deduct
            so[total_so].mod_addr = (void *) (info->dlpi_addr + info->dlpi_phdr[i].p_vaddr);

            // the offset is in the file, but for addr2line we need that
            so[total_so].mod_offset = info->dlpi_phdr[i].p_offset;

            // get ending module, we use it here to find out if we're in the range of addresses
            // for this module, and if we are, this is the base module for our code
            so[total_so].mod_end = so[total_so].mod_addr + info->dlpi_phdr[i].p_memsz-1;

            // get module name, we will be using it in addr2line
            if (info->dlpi_name == NULL || info->dlpi_name[0] == 0)
            {
                // this is main program
                if (readlink("/proc/self/exe", so[total_so].mod_name, sizeof(so[total_so].mod_name)-1) == -1)
                {
                    continue; // do not increment total_so, go to the next one, should not happen
                }
            }
            else
            {
                snprintf(so[total_so].mod_name,sizeof(so[total_so].mod_name),"%s", info->dlpi_name);
            }
            total_so++;
            if (total_so >= MAX_SO)
            {
                break; // in this case there's more shared object(plus main) than we can handle, we will break
                       // and try to find the addresses, but some are missing. The only solution to this problem
                       // is to increase MAX_SO. Normally 100 should be more than enough though.
            }
        }
    }
    return 0;
}
 
//
// This is what's called by generated GLIIMLY program at the beginning
// to enable catchng signals and dumping human-readable stack in backtrace file
// 'dir' is the tracing directory where to write trace file
//
void gg_set_crash_handler(char *dir)
{
    // build backtrace file name to be used througout here
    snprintf(backtrace_file, sizeof(backtrace_file), "%s/backtrace", dir);

    expla[0] = 0;

    // set signal handling
    set_signal_handler();

}

// 
// Get time without tracing, self contained here
// This is on purpose, so no outside calls are made
// see explanation at the top of this file
// This uses localtime on the server
// 'outstr' is the output time, and 'outstrLen' is the 
// length of this buffer.
//
void gg_get_time_crash (char *outstr, gg_num outstrLen)
{
    time_t t;
    struct tm *tmp;

    t = time(NULL);
    tmp = localtime(&t);
    if (tmp == NULL) 
    {
        outstr[0] = 0;
        return; 
    }

    if (strftime(outstr, outstrLen, "%F-%H-%M-%S", tmp) == 0) 
    {
        outstr[0] = 0;
    }
}


//
// Return total number of shared libraries loaded.
// Output variable sos is the list of shared libraries loaded, which we can print out if needed.
//
gg_num gg_total_so(gg_so_info **sos)
{
    *sos =  so;
    return total_so;
}






