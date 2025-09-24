// SPDX-License-Identifier: Apache-2.0
// Copyright 2018-2025 Gliim LLC. 
// Licensed under Apache License v2. See LICENSE file.
// On the web http://golf-lang.com/ - this file is part of Golf framework.

//
// Handling of fatal crashes, or controlled program aborts.
// The purpose is to create a backtrace file which contains the
// stack, together with source file name/line numbers, when a 
// crash occurs.
//
// For the source code/line number reporting to work,  -g and
// -rdynamic (most likely) must be used. Golf always does that automatically.
//
// This uses libbacktrace (see LICENSE in btrace directory in Golf source, as well License 
// in documentation for copyright and acknowledgement details).
//

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <stdbool.h>
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
#include "btrace/backtrace.h"

// *******************
// NO CALLS TO CODE OUTSIDE OF THIS MODULE MUST BE MADE!! This is a self-containing backtrace-providing module.
// *******************
 
// limitations on stack size, intended to be reasonably well sized
//
#define GG_MAX_MSG_LEN 2048 // max length of message with backtrace. It goes from deepest levels first, so it will catch the most important bits
#define GG_MAX_STACK_FRAMES 150

static char *expla = ""; // error explanation
// outputting data
static char gg_emsg[GG_MAX_MSG_LEN];  // message that's output
static gg_num gg_emsg_pos = 0; // current position where we're writing into the message
//

// these are generally set elsewhere for usage here, they provide
// additional information about where we were when crash happened
volatile gg_num gg_end_program=0; // when SIGTERM is received, set this to 1 to exit gracefully
extern gg_num gg_in_request; // 1 if we're processing request when signal happened, 0 if not
                             // determines how we handle the signal (we wait for request to end if it's in the request,
                             // otherwise quit)

// Functions
static void signal_handler(int sig);
static void set_signal_handler();
static char *btrace_path; // full path of backtrace, including file prefix bt.<PID>
static struct backtrace_state *bt_state; // state of backtrace viewer, created once
static void bt_handle_error (void *data, const char *msg, int error_number);
static int bt_handle_info (void *data, uintptr_t pc, const char *srcfile, int lnum, const char *function);
static void gg_add_msg (char *add, gg_num *pos, char *to, gg_num avail);
static void gg_itos (int n, char *s, gg_num avail);
char *curr_err; // current error message from the caller
static gg_num first_output = 1; // special output for the very first output of an error (meaning first in a list of lines 
                                // output for a single error), but must be reset with each error


// 
// Obtain backtrace, and write information to output file fname.
// We don't use signal unsafe functions, like printf, hence our own number-to-string conversion, plus formatted output
//
void gg_get_stack(char *err)
{
    // init stack just before getting it. We don't do it in signal handler, because we can call gg_get_stack() outside of it, say for
    // Golf own exceptions
    static bool binit = false;
    if (!binit)
    { // perform init just once
        // we don't use threads, so second param is 0. If this fails, it calls error handler and that's it.
        bt_state = backtrace_create_state(NULL, 0, bt_handle_error, NULL); // init can be called here, it's as gcc does it
        binit = true;
    }

    // init buffer for writing
    gg_emsg[0] = 0;
    gg_emsg_pos = 0;
    // get stack
    curr_err = err;
    //
    // backtrace_full will get a full stack backtrace. Second parameter is the number of frames to skip; we could skip this function
    // but we do filtering ourselves. NULL is the 'data', which is the first parameter in the handlers; we can pass something along here.
    backtrace_full(bt_state, 0, bt_handle_info, bt_handle_error, NULL);
    first_output = 1;
    //
    //
    //
    gg_gen_write (true, gg_emsg, gg_emsg_pos); // write to stderr - first write the stack

    // Construct the file name to output to
    char btrace_cur[GG_MAX_FILE_NAME+50]; // file name of the current stack trace (pid-1,2,3...) so one for each incident, easier to use
    gg_num btrace_pos = 0; // position in btrace_cur string
    static gg_num btrace_counter  = 0; // counter of stack trace incidents, 1,2,3...
    // get the counter as string
    char counter[30];
    gg_itos (btrace_counter, counter, sizeof(counter));
    // file name, generic trace $HOME/.golf/apps/<app>/app/trace/bt.PID-<incident>
    gg_add_msg (btrace_cur, &btrace_pos, btrace_path, sizeof(btrace_cur));
    gg_add_msg (btrace_cur, &btrace_pos, "-", sizeof(btrace_cur));
    gg_add_msg (btrace_cur, &btrace_pos, counter, sizeof(btrace_cur));
    btrace_counter++; // increase for the next one
    // open file, write, close it; this way info is passed on to OS for sure; file is owner read/write only
    int bwrite = open (btrace_cur, O_WRONLY|O_APPEND|O_CREAT, S_IRUSR+S_IWUSR);
    int written = write (bwrite, gg_emsg, gg_emsg_pos); 
    GG_UNUSED(written); // if it doesn't work, we do nothing as this isn't the functionality; it's a best-effort try to record
    close (bwrite);

}

//
// Convert safely (without using signal unsafe functions) number n to string s
// avail is the number of bytes available in s
static void gg_itos (int n, char *s, gg_num avail)
{
    bool neg = (n<0?true:false);
    if (neg) n = -n;
#define GG_MAX_DIGITS 25
    if (avail <= GG_MAX_DIGITS) {s[0] = 0; return;}
    int last = GG_MAX_DIGITS-2;
    do
    {
        int mod = n % 10;
        n = n / 10;
        s[last--] = '0'+mod;
        if (last == 0) { s[0] = 0; return; }
    } while (n>0);
    last++; // go back the first digit
    int sign_adj = 0;
    if (neg) {s[0] = '-'; sign_adj=1;}
    memmove (s+sign_adj, s+last, GG_MAX_DIGITS-2-last+1);
    s[sign_adj+GG_MAX_DIGITS-2-last+1] = 0;
}

//
// Add string 'add' to 'to', which has avail bytes left. Start with pos in add.
// If no room, ignore 'to' (this is a best effort task)
//
static void gg_add_msg (char *add, gg_num *pos, char *to, gg_num avail)
{
    gg_num lto = strlen (to);
    if (*pos + lto >= avail-1) return; // keep room for \0
    memcpy (add+*pos, to, lto);
    *pos+=lto;
    add[*pos] = 0;
}


//
// Error Handler for backtrace. We shouldn't really encounter this. Typically, 
// msg is something like 'no debug info found'. Or it could be a corrupted executable etc.
// error_number is errno for this.
//
static void bt_handle_error (void *data, const char *msg, int error_number) 
{
    GG_UNUSED(data);
    if (error_number == -1) 
    { // cannot determine error information
        return;
    }
    else 
    {
        // display what was the issue, why couldn't we get the stack backtrace?
        gg_add_msg (gg_emsg, &gg_emsg_pos, "Backtrace error ", sizeof(gg_emsg));
        char lnum[50];
        gg_itos (error_number, lnum, sizeof(lnum));
        gg_add_msg (gg_emsg, &gg_emsg_pos, (char*)lnum, sizeof(gg_emsg));
        gg_add_msg (gg_emsg, &gg_emsg_pos, ": ", sizeof(gg_emsg));
        gg_add_msg (gg_emsg, &gg_emsg_pos, (char*)msg, sizeof(gg_emsg));
        gg_add_msg (gg_emsg, &gg_emsg_pos, "\n", sizeof(gg_emsg));
        // this will be displayed in gg_get_stack() since this is called  from backtrace_full(), also written to a file there
  }
}


//
// This is called to make each line of stack trace by the backtrace. pc is program counter. filename is the source file.
// lnum is the line number in it. function is the function name.
// Returns 0 to continue tracing. We return 1 when too many stack traces are present. This is so we don't go overboard ourselves
// and crash here; we want to show something useful, but if program dies here, nothing may come out of it.
// srcfile has a full path
//
static int bt_handle_info (void *data, uintptr_t pc, const char *srcfile, int lnum, const char *function) 
{
    GG_UNUSED(data);
    GG_UNUSED(pc);
    static gg_num sframes = 0; // how many stack frames we process?

    if (srcfile != NULL && function != NULL && lnum != 0)  // sanity check
    {
        const char *filename = strrchr(srcfile, '/'); // find the first / in reverse
        if (filename != NULL) filename++; else filename = srcfile; // file name is now basename(), which we don't use for safety here
        // Do not output anything for functions here, in golfrt.c and for main/libc. This is just to remove info that isn't very useful.
        // Just these specific functions, others are fine (and needed); mostly because these are fixed and always there.
        if (!(
                (!strcmp (filename, "chandle.c"))
                || 
                (!strcmp (filename, "golfrt.c") && !strcmp (function, "gg_write_ereport")) 
                || 
                (!strcmp (filename, "golfrt.c") && !strcmp (function, "gg_dispatch_request")) 
                || 
                (!strncmp (function, "__libc_", 7)) 
                || 
                (!strcmp (function, "main")) 
          ))
        {
            if (first_output)
            { 
                // For the very first line, output error itself, timestamp, and intro that backtrace follow
                first_output = false;
                gg_add_msg (gg_emsg, &gg_emsg_pos, "\n--- ", sizeof(gg_emsg));
                gg_add_msg (gg_emsg, &gg_emsg_pos, curr_err, sizeof(gg_emsg));
                gg_add_msg (gg_emsg, &gg_emsg_pos, ". Stack backtrace of the error:\n", sizeof(gg_emsg));
            }
            // output backtrace line
            // this will be displayed in gg_get_stack() since this is called  from backtrace_full(), also written to a file there
            //
            bool is_golf = (strstr (filename,".golf") != NULL);
            // If this is golf source, put * in front, this makes it visually easier to see
            // otherwise just plain indent
            if (is_golf) gg_add_msg (gg_emsg, &gg_emsg_pos, "     *", sizeof(gg_emsg));
            else gg_add_msg (gg_emsg, &gg_emsg_pos, "      ", sizeof(gg_emsg));
            /// convert line num to string
            char lnum_str[50];
            gg_itos (lnum, lnum_str, sizeof(lnum_str));
            // display filename:line in func()
            gg_add_msg (gg_emsg, &gg_emsg_pos, (char*)filename, sizeof(gg_emsg));
            gg_add_msg (gg_emsg, &gg_emsg_pos, ":", sizeof(gg_emsg));
            gg_add_msg (gg_emsg, &gg_emsg_pos, (char*)lnum_str, sizeof(gg_emsg));
            gg_add_msg (gg_emsg, &gg_emsg_pos,  " in ", sizeof(gg_emsg));
            gg_add_msg (gg_emsg, &gg_emsg_pos, (char*)function, sizeof(gg_emsg));
            gg_add_msg (gg_emsg, &gg_emsg_pos,  "()", sizeof(gg_emsg));
            gg_add_msg (gg_emsg, &gg_emsg_pos, "\n", sizeof(gg_emsg));
            if (sframes++ > GG_MAX_STACK_FRAMES) return  1; // this means stop doing it, too many; meaning backtrace_full() will stop after this
        }
    }
    return 0; // continue to the next line, backtrace_full() uses this
}



// 
// Signal handler for signal sig. sig is signal number
// This way at run time we know which signal was caught. 
// NO GOLF MEMORY HANDLING HERE
// This will either end program (due to request to end it, or some fatal condition), or set the flag
// to end it once the current request completes, and that's all it can do.
//
static void signal_handler(int sig)
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

    expla="Unknown error"; // default if we can't figure it out

    switch(sig)
    {
        case SIGFPE:
            expla= "Caught SIGFPE: math exception, such as divide by zero, overflow or illegal instruction";
            break;
        case SIGILL:
            expla= "Caught SIGILL: illegal code";
            break;
        case SIGABRT:
        case SIGBUS:
        case SIGSEGV:
            if (sig == SIGABRT) expla= "Caught SIGABRT: usually caused by an abort() or assert()";
            if (sig == SIGBUS) expla= "Caught SIGBUS: bus error";
            if (sig == SIGSEGV) expla= "Caught SIGSEGV: segmentation fault";
            break;
        case SIGHUP:
            expla= "Caught SIGHUP: hang up";
            break;
        case SIGTERM:
            gg_end_program = 1;
            if (gg_in_request == 0) 
            {
                expla="Caught SIGTERM: request for graceful shutdown, shutting down now as Golf is not processing a request";
                break; // go down, print out stack and end it
            }
            else
            {
                expla= "Caught SIGTERM: request for graceful shutdown, Golf will shutdown once a request is processed";
            }
            return; // we go back to finish current request if SIGTERM was sent, and we're in the middle of it
    }

    // this means if the error is outside request processing, and it's command, still output to standard error
    // because we can do it; and we're exiting here anyway at this point
#ifdef GG_COMMAND
    gg_finished_output = 0;
#endif


    gg_get_stack(expla);
     // NOTE: this is in signal handler, only serious Golf bugs basically come here. We just dump stack and exit. Doing more than
     // that can cause more trouble. We cannot realistically send server status back to the client. Client will know just by the fact
     // that the connection broke. 
     //
     // A report-error (whether it's a statement, or if Golf called it because of memory safety guard or like that), will NEVER be calling here
     // That goes to  v1.c (in do_printf) or _gg_report_error (if memory safety issue or status etc). Those will continue processing (if service).
     //

    // exit and quit, cannot continue
    GG_FATAL ("Exiting because of the signal: [%s]", expla);
}
 

// 
// Set each signal handler, this must be called asap in the program
//
static void set_signal_handler()
{
    struct sigaction psa;
    memset (&psa, 0, sizeof (psa));
    psa.sa_handler = signal_handler;
    sigemptyset(&psa.sa_mask);
    psa.sa_flags = SA_RESTART;
    // We do not set psa.sa_flags to SA_RESTART because vf -d process management depends on
    // properly interrupting the read() and such
    if (sigaction(SIGABRT, &psa, NULL) == -1) GG_FATAL ("Cannot set ABRT signal handler");
    if (sigaction(SIGFPE,  &psa, NULL) == -1) GG_FATAL ("Cannot set FPE signal handler");
    if (sigaction(SIGILL,  &psa, NULL) == -1) GG_FATAL ("Cannot set ILL signal handler");
    if (sigaction(SIGSEGV, &psa, NULL) == -1) GG_FATAL ("Cannot set SEGV signal handler");
    if (sigaction(SIGFPE, &psa, NULL) == -1) GG_FATAL ("Cannot set SEGFPE signal handler");
    if (sigaction(SIGBUS, &psa, NULL) == -1) GG_FATAL ("Cannot set BUS signal handler");
    if (sigaction(SIGTERM, &psa, NULL) == -1) GG_FATAL ("Cannot set TERM signal handler");
    if (sigaction(SIGHUP, &psa, NULL) == -1) GG_FATAL ("Cannot set HUP signal handler");
    //
    // ignore these
    //
    struct sigaction ipsa;
    memset (&ipsa, 0, sizeof (ipsa));
    sigemptyset(&ipsa.sa_mask);
    ipsa.sa_handler = SIG_IGN;
    if (sigaction(SIGINT, &ipsa, NULL) == -1) GG_FATAL ("Cannot ignore SIGINT"); // ctrl c
    if (sigaction(SIGPIPE, &ipsa, NULL) == -1) GG_FATAL ("Cannot ignore SIGPIPE"); // broken pipe
    if (sigaction(SIGUSR1, &ipsa, NULL) == -1) GG_FATAL ("Cannot ignore SIGUSR1"); // no meaning in golf yet
    if (sigaction(SIGUSR2, &ipsa, NULL) == -1) GG_FATAL ("Cannot ignore SIGUSR2"); // no meaning in golf yet
    // CANNOT ignore SIGCHLD because we DO need status from them
    // DO NOT do anything with SIGALRM - curl uses it for timeouts
}



//
// This is what's called by generated GOLF program at the beginning
// to enable catchng signals and dumping human-readable stack in backtrace file
// btrace is a partial file name (with path), i.e. /$HOME/.golf/apps/<app>/app/trace/bt.<PID> and we add -1, -2.. to it
// when writing out incidents
//
void gg_set_crash_handler(char *btrace)
{
    expla="";
    btrace_path = btrace;
    // set signal handling
    set_signal_handler();

}








