// SPDX-License-Identifier: Apache-2.0
// Copyright 2018-2025 Gliim LLC. 
// Licensed under Apache License v2. See LICENSE file.
// On the web http://golf-lang.com/ - this file is part of Golf framework. 

// 
// Golf SERVICE process manager. 
//

#define _GNU_SOURCE         

#include <unistd.h> 
#include <getopt.h> 
#include <stdio.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/un.h>
#include <sys/socket.h>
#include <errno.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/file.h>
#include <sys/mman.h>
#include <ctype.h>
#include <dirent.h>
#include <time.h>
#include <pwd.h>
#include <grp.h>
#include <fcntl.h>
#include <stdbool.h>
#include <sys/prctl.h>
#include <stdatomic.h>
#include "ggcommon.h"


// POSIX shared memory used by server
#define GG_SHMEM "/golfmem"


// display error
#define GG_FERR (errno == 0 ? "" : strerror(errno))

// assert not null (used for malloc)
#define GG_ANN(x) ((x) == NULL ? exit_error(-1, "Cannot allocate memory [%s]", GG_FERR) : 0)


#define GG_MAX_ARGS 64 // max args to mgrg in -a

#define GG_MAX_FILELEN 200 // max path+file length

// client commands to server and back
#define GG_OKAY 0
#define GG_START 1
#define GG_STOP 2
#define GG_RESTART 3
#define GG_QUIT 4
#define GG_OKAY_STARTED 5
#define GG_OKAY_RUNNING 6
#define GG_STATUS 7
#define GG_STOPONE 8
// codes for processes and manager communication
#define GG_COMMAND 9
#define GG_DONE 10
#define GG_DONEBAD 11
#define GG_DONESTARTUP 12
#define GG_DONESTARTUP0 13

// invalid exit status of child process (used to tell us we don't know what happened)
#define GG_BAD_EXIT 1000

// locks for checking server the only one running for an app
#define GG_LOCK 1 // lock it
#define GG_CHECKLOCK 2 // check if locked
#define GG_MAX_CLI_WAIT 10 // max second client will wait to hear back from server

//
// now that we know the server is (most likely) running, do the handshake so we get the confirmation the job is done
// number of tries correlate to the sleep interval. Total time to wait is (GG_MAX_CLI_WAIT * 1000), and since we're checking
// every mslp (400 millisecs) , total of (GG_MAX_CLI_WAIT*1000)/mslp checks. We do +1 to get at least one try in case the total is 0.
#define GG_TRIES ((gg_num)((GG_MAX_CLI_WAIT*1000)/mslp)+1)

//
// logging stuff
// both output to stdout, while log_msg is for resident process, and out_msg outputs to whatever is connected to stdout
#define GG_MOUT 0
#define GG_MLOG 1
#define out_msg(...) log_msg0(GG_MOUT, __VA_ARGS__) 
#define log_msg(...) log_msg0(GG_MLOG, __VA_ARGS__)

// Command from client to server, all updates to command are atomic.
// This is the fastest way to synchronize reads and writes without using semaphores, mutexes etc. It's like there's a memory barrier at atomic write
// that assures all that is written before it will be properly available after the barrier at atomic read. A neat C11 feature.
typedef struct s_shbuf {
    volatile _Atomic gg_num command;
    volatile _Atomic gg_num data1;
} shbuf;

// Help for -h
static char *usage_message =
    "Usage:\n\
    \n\
    mgrg <options> <app_name>\n\
    \n\
    <options> can be\n\
    -i            initialize directory structure\n\
    -u <user>     OS user who owns the application\n\
    -m <msg>      send message to server (start, restart, stop, quit, status)\n\
    -c <command>  program to execute as service\n\
    -f            run in foreground and log to standard output\n\
    -p <port>     TCP/IP listening port for workers\n\
    -x            use Unix domain sockets (local connections)\n\
    -n            do not restart dead workers\n\
    -g            do not restart workers when executable changes\n\
    -l <backlog>  the size of listening backlog for incoming connections\n\
    -w <num_wrk>  number of workers if not adaptive (-d)\n\
    -d            adaptive load mode: number of workers determined dynamically\n\
    --max-worker\n\
        <max_wrk> minimum number of workers if adaptive (-d) (0 minimum)\n\
    --min-worker\n\
        <min_wrk> maximum number of if adaptive (-d)\n\
    -t <reltime>  seconds before reducing number of workers, if adaptive (-d)\n\
    -r <prxy_grp> primary group of proxy server (Unix domain sockets only)\n\
    -a <args>     list of arguments for workers (quoted, single quotes inside)\n\
    -s <slp>      milliseconds between commands and managing workers\n\
    -e            display verbose messages\n\
    -v            display version, copyright and license\n\
    -h            this help\n\
    \n\
Type 'man mgrg' for more help\n";

static gg_num maxblog = 400; // listening back log
static gg_num maxproc = 0;  // max-worker
static pid_t *plist; // list of process IDs started by mgrg instance
static gg_num sockfd; // socket passed down to forked child
static gg_num num_process; // abs max # of processes, plist is sized on it
static FILE *logfile = NULL; // this is mgrg.log
static pid_t sid = 0; // session id of the child group
static char *golfapp = ""; // app name
static gg_num num_to_start_min = 5; // min-worker
static gg_num adapt = 0; // adaptive mode
static gg_num tspike = 30; // number of secs to die down for client srvc process if no demand 
static gg_num port = 0; // port (tcp)
static char *command = ""; // command (srvc) to execute as child proces
static struct timespec *commtime = NULL;  // timestamp of command executable
static char *run_user = "";  // user running server
static char *proxy_grp = ""; // group of reverse proxy
static uid_t run_user_id = -1; // running user id
static gid_t proxy_grp_id = -1; // reverse proxy group id
static gid_t run_user_grp_id = -1; // running user group id
static shbuf *golfmem; // shared memory for communication btw client and server
static char *client_msg = "";  // message from client -m
static gg_num temp_no_restart = 0;  // by default, process stopped is restarted. 
                                 // if temp_no_restart is 1, then it won't be, if 0, it will be.
static gg_num modreload = 1; // reload if exec modified
static bool silent = false;
static char *parg = ""; // input args -a
static char *xarg[GG_MAX_ARGS]; // list of parsed args -a
static gg_num mslp=400; /*millisecs to sleep*/ 
static gg_num lfd = -1; // locked file descriptor
static gg_num unix_sock = 0; // tcp vs unix sock
static gg_num initit=0; // initialize dirs
static gg_num islogging = 0; // logging or not
static gg_num showinfo = 0;  // show info -e
static gg_num dnr = 0;  // do not restart if 1
static gg_num exenotfound = 0; // print exe not found just once
static gg_num fg = 0; // if 1, run in foreground

// functions prototypes
static pid_t whatisprocess(pid_t p, int *st);
static void log_msg0(gg_num dest, char *format, ...);
static void exit_error(int ec, char *format, ...);
static void wait_server_end() ;
static void getshm();
static void checkshm(bool is_running_server);
static void cli_getshm(char *comm, gg_num byserver);
static void usage(int ec);
static void start_child (char *command, gg_num pcount);
static void processup(char addone);
static void tokarg();
static void handlestop(gg_num sig);
static void sleepabit(gg_num milli);
static void unlockfile (gg_num lf);
static gg_num testlockfile (char *fname) ;
static gg_num lockfile (char *fname, gg_num *lf);
static gg_num srvhere(gg_num op);
static void runasuser();
static void owned (char *ipath, uid_t uid);
static void initdir (char *ipath, gg_num mode, uid_t uid, gid_t guid);
static gg_num checkmod();
static void msg_notfound(char *err);
static gg_num timeq(struct timespec *a, struct timespec *b);
static gg_num connwait();
static gg_num totprocess();

//
// check if there are input connections from outside to create new srvc process
// existing srvc processes wait for requests too. If mgrg gets to wait on an input connection
// then there are too many queued up
// Return  1 if connection is waiting to be accepted, 0 otherwise
//
static gg_num connwait()
{
    fd_set set;
    struct timeval timeout;
    FD_ZERO (&set);
    FD_SET (sockfd, &set);
    timeout.tv_sec = 0;
    timeout.tv_usec = 0;
    if (select (FD_SETSIZE, &set, NULL, NULL, &timeout) == 1)
    {
        if (FD_ISSET(sockfd, &set))
        {
            return 1;
        }
    }
    return 0;
}

//
// Check if two timestamps equal (a and b), return 1 if they are, 0 otherwise
//
static gg_num timeq(struct timespec *a, struct timespec *b) {
    if (a->tv_sec == b->tv_sec) {
        if (a->tv_nsec != b->tv_nsec) return 0;
    } else return 0;
    return 1;
}

//
// Check if command executable timestamp changed. If it did, return 1, otherwise 0
//
static gg_num checkmod() {
    if (commtime == NULL) return 0;
    struct stat sbuff;
    if (stat(command, &sbuff) != 0) {  msg_notfound(GG_FERR); return 0;} // command could be temporarily deleted
    if (timeq(commtime, &(sbuff.st_mtim))) return 0; else return 1;
}

//
// Set file with mode 'mode', and to be owned by user group uid/guid, if it exists
// If it doesn't don't do anything
//
static void initfile (char *ipath, gg_num mode, uid_t uid, gid_t guid) {
    log_msg ("Setting privileges on file [%s]", ipath);
    struct stat sbuff;
    if (stat(ipath, &sbuff) != 0) return;
    // We check if this isn't local install; if so, we will try chown;
    // We'll check if current user/group matches what we want to change to, and if not, emit error; otherwise continue since
    // nothing's changing
    if (chown (ipath, uid, guid)!= 0) exit_error (-1, "Cannot change the ownership of file [%s], [%s]", ipath, GG_FERR);
    if (chmod(ipath, mode) != 0) exit_error (-1, "Cannot set permissions for file [%s]", GG_FERR);
}


//
// Initialize directory ipath, with mode 'mode', to be owned by user group uid/guid
//
static void initdir (char *ipath, gg_num mode, uid_t uid, gid_t guid) {
    log_msg ("Creating directory [%s]", ipath);
    if (mkdir (ipath, mode) != 0) if (errno != EEXIST) exit_error (-1, "Cannot create directory [%s], [%s]", ipath, GG_FERR); 
    // We check if this isn't local install; if so, we will try chown;
    // We'll check if current user/group matches what we want to change to, and if not, emit error; otherwise continue since
    // nothing's changing
    if (chown (ipath, uid, guid)!= 0) exit_error (-1, "Cannot change the ownership of directory [%s], [%s]", ipath, GG_FERR);
    if (chmod(ipath, mode) != 0) exit_error (-1, "Cannot set permissions for directory [%s]", GG_FERR);
}

//
// Make sure file/dir ipath is owned by uid, if not exit
//
static void owned (char *ipath, uid_t uid) {
    struct passwd* pwd;
    struct passwd* pwd1;
    struct stat sbuff;
    char *userown;
    char *usernew;
    if (stat(ipath, &sbuff) == 0) {
        if (sbuff.st_uid != uid) {
            if ((pwd = getpwuid (sbuff.st_uid)) == NULL) exit_error (-1,"Cannot find user who owns [%s], user id [%ld], [%s]", ipath, sbuff.st_uid, GG_FERR);
            // must allocate new buffer as getpwuid results overwrite the old ones
            GG_ANN (userown = strdup(pwd->pw_name));
            if ((pwd1 = getpwuid (uid)) == NULL) exit_error (-1,"Cannot find user [%ld], [%s]", ipath, uid, GG_FERR);
            GG_ANN (usernew= strdup(pwd->pw_name));
            exit_error (-1,"Directory [%s] is already owned by another user [%s], current user [%s]", ipath, userown, usernew);
        }
    }
}

//
// Make sure app isn't run as root, and it's home directory is accessible
//
static void runasuser() {
    // Make sure root isn't here - it can't be, but double check. 
    // Final test is to try and make us a root. If succeeded, there was some bug, but in any case, this is the end of it
    // It will never run as root, bug or no bug.
    if (setuid(0) == 0 || seteuid(0) == 0) exit_error (-1,"Program can never run as root");

    char ipath[GG_MAX_OS_UDIR_LEN];
    gg_dir (GG_DIR_APP, ipath, sizeof(ipath), golfapp, NULL);
    if (chdir(ipath) != 0) exit_error (-1,"Cannot set current directory to [%s], [%s]", ipath, GG_FERR);
}


//
// returns 1 if there is another server running, 0 if not. If op is GG_CHECKLOCK, then do not lock (client), just check.
// If it is GG_LOCK, lock the file (only server does this).
// A file is locked to achieve one-app-one-server.
//
static gg_num srvhere(gg_num op) {
    char lpath[GG_MAX_OS_UDIR_LEN];
    gg_dir (GG_DIR_LOCK, lpath, sizeof(lpath), golfapp, NULL);
    if (op == GG_CHECKLOCK) 
    {
        if (testlockfile (lpath) == 1) return 1; else return 0;
    }
    if (lockfile (lpath, &lfd) == 0) {
        log_msg("Another server is running, lock file failed");
        return 1;
    }
    return 0;
}

//
// Sleep milliseconds milli. We don't have signals here, so no need for remaining time to be considered.
//
static void sleepabit(gg_num milli) {
   struct timespec slp;
   slp.tv_sec = milli / 1000;
   slp.tv_nsec = (milli % 1000) * 1000000;
   nanosleep(&slp, NULL);
}

//
// Test lock on a file fname. Returns 0 if not locked or error, 1 if locked
//
static gg_num testlockfile (char *fname) 
{
    struct flock lock;
    memset(&lock, 0, sizeof(lock));
    int fd = open(fname, O_RDWR);
    if (fd  == -1) return 0;
    lock.l_type = F_WRLCK; 
    lock.l_start = 0;      
    lock.l_len = 0;       
    lock.l_whence = SEEK_SET; 

    if (fcntl(fd, F_GETLK, &lock) == -1) { //error
        log_msg("Test lock file [%s] failed [%s]", fname, GG_FERR);
        close(fd);
        return 0;
    } else if (lock.l_type != F_UNLCK) { //locked
        log_msg("Test lock file [%s] locked by [%d]", fname, lock.l_pid);
        close(fd);
        return 1;
    } else { // not locked
        close(fd);
        return 0;
    }
}

//
// UnLock a file id lf. We don't really check for return here, it is what it is, as it's a best effor endeavour.
//
static void unlockfile (gg_num lf) {
    struct flock lock;
    memset(&lock, 0, sizeof(lock));
    //
    lock.l_type = F_UNLCK;
    lock.l_whence = SEEK_SET;
    lock.l_start = 0;
    lock.l_len = 0;
    //
    int unl = fcntl(lf, F_SETLK, &lock);
    log_msg("UnLock file result [%d], [%s]", unl, GG_FERR);
}

//
// Lock a file fname, get lock file descriptor lf. 
// returns 0 if lock failed, meaning another process is locking
// and 1 if okay, meaning this is the server process
//
static gg_num lockfile (char *fname, gg_num *lf) {
    struct flock lock;
    memset(&lock, 0, sizeof(lock));
    *lf = open(fname, O_RDWR | O_CREAT, 0600);
    if (*lf  == -1) return 0;

    lock.l_type = F_WRLCK;
    lock.l_whence = SEEK_SET;
    lock.l_start = 0;
    lock.l_len = 0;
    if (fcntl(*lf, F_SETLK, &lock) == -1) {
        log_msg("Lock file [%s] failed [%s]", fname, GG_FERR);
        
        /* Lock failed. Close file and report locking failure. */
        close(*lf);
        return 0;
    }
    log_msg("Lock file [%s] okay", fname);
    return 1;
}

//
// Tokenize input args from parg, store to xarg[]. Single quoted is okay to keep an arg whole.
//
static void tokarg() {
    gg_num c = 0;
    gg_num instring = 0;
    gg_num begstring = 0;
    gg_num quote;
    gg_num carg = 0;
    GG_ANN (xarg[carg++] = strdup (command)); // avoid const issue
    while (isspace(parg[c])) c++; // get rid of leading spaces
    gg_num begarg = c;
    while (1) {
        if (instring == 0 && (parg[c] == '"' || parg[c] == '\'')) { instring = 1; quote = parg[c]; begstring = ++c; }
        else if (instring && parg[c] == quote) { 
            instring = 0; 
            xarg[carg++] = parg + begstring;
            parg[c++] = 0;
            while (isspace(parg[c])) c++; // get rid of leading spaces
            begarg = c;
        } else if (instring == 0 && isspace (parg[c])) {
            xarg[carg++] = parg + begarg;
            parg[c++] = 0;
            while (isspace(parg[c])) c++; // get rid of leading spaces
            begarg = c;
        } else if (parg[c] == 0) {
            if (instring == 0 && parg[begarg] != 0) xarg[carg++] = parg + begarg;
            break;
        } else c++;
        if (carg >= GG_MAX_ARGS - 1) exit_error (-1,"Too many arguments specified for service processes [%ld]", carg);
    }
    if (instring == 1) exit_error (-1,"Unterminated string in arguments specified for service process, at [%ld]", begstring);
    xarg[carg] = NULL;
}

//
// Log message in printf-style to stdout, which can be redirected by caller. 
// If dest is GG_MLOG, then if showinfo is 1, messages still go through even if logging is off.
// islogging is turned on *only* in resident mgrg process.
//
static void log_msg0(gg_num dest, char *format, ...) {
    time_t t;
    time(&t);
    if (islogging) printf("%ld: %s: ", (gg_num)getpid(), strtok (ctime(&t), "\n"));
    if (islogging == 0 && dest == GG_MLOG && showinfo == 0) return;
    va_list args;
    va_start (args, format);
    vprintf(format, args);
    puts ("");
    va_end (args);
    fflush (stdout);
}

//
// Display error message to stderr, which may be redirected in printf-style
// ec is the exit code
//
static void exit_error(int ec, char *format, ...) {
    time_t t;
    time(&t);
    fprintf(stderr, "%ld: %s: Error: ", (gg_num)getpid(), strtok (ctime(&t), "\n"));
    va_list args;
    va_start (args, format);
    vfprintf(stderr, format, args);
    va_end (args);
    fputs ("\n", stderr);
    fflush (stderr);
    exit(ec);
}

//
// What is the status of the child process with PID p? Returns p if running, -p if running but either stopped or continued,
// -1 if process dead, 0 if cannot determine
// if process exited, then st is its exit status; st can be NULL; if child actually exited, then st is the status, if not 
// (such as if it was terminated, or otherwise we can't get it), then st is GG_BAD_EXIT (which is 1000, not a valid exit status, thus signalling it never exited)
// st is set only if process does not exist any more, so if it does exist, it's GG_BAD_EXIT
//
static pid_t whatisprocess(pid_t p, int *st) {
    int pstat = 0; // waitpid may not set anything, so it may be random (bad)
    gg_num pid = waitpid (p, &pstat, WNOHANG);
    if (st != NULL) *st = GG_BAD_EXIT;
    if (pid == 0) { // pid 0 means status not changed, child is running
        return p;
    }
    else if (pid == p) { // status has changed
        if (!WIFEXITED(pstat) && !WIFSIGNALED(pstat)) { // was not killed, must have been stopped or continued, but still up
            return -p;
        }
        else 
        {
            if (st != NULL) { 
                if (WIFEXITED(pstat)) 
                    *st = WEXITSTATUS(pstat); 
                else 
                    *st = GG_BAD_EXIT; 
            }
            return -1; // exited or terminated by a signal
        }
    } 
    else if (pid == -1 || errno == ECHILD) {
        return -1;// else child may have exited already or been killed, or there is a process with this PID that is not a child
    } else {
        if (st != NULL) *st = GG_BAD_EXIT;
        return 0; // this can be only bad option; but it isn't (WNOHANG). Should never happen.
    }
}


//
// Stop processes. If sig is GG_QUIT, all srvc processes and mgrg stop (total quit). If GG_STOP, mgrg continues running,
// while srvc processes are stopped. If GG_STOPONE, stop one srvc process only (for adaptive downsizing).
//
static void handlestop(gg_num sig) {
    if (sig == GG_QUIT || sig == GG_STOP || sig == GG_STOPONE ) {
        gg_num i;
        if (sig == GG_QUIT) log_msg("Terminating service processes and the process manager (me)");
        if (sig == GG_STOP) log_msg("Terminating service processes only");
        if (sig == GG_STOPONE) log_msg("Terminating single service processes only");
        gg_num pkilled = 0;
        for (i = 0; i < num_process; i++) {
            if (plist[i] == -1) continue; // no process in this slot
            pid_t p = whatisprocess (plist[i], NULL);
            if (p == plist[i]) { // means status not changed, child is running
                log_msg("Sending SIGTERM to [%ld]", plist[i]);
                kill (plist[i], SIGTERM);
                pkilled = 1;
            }
            else if (p == -plist[i]) { // process up but stopped or continuted
                log_msg("Sending SIGTERM to [%ld]", plist[i]);
                kill (plist[i], SIGTERM);
                pkilled = 1;
            } // else child may have exited already or been killed
            else if (p == -1) {
                log_msg ("Child either died or PID is not a child [%s]", GG_FERR);
                pkilled = 1;  // process either died or is no longer a child of this one
            } else {
                log_msg("Unrecognized return from waitpid [%ld]",plist[i]);
                continue; // do not set -1 for process ID below, only if terminated, this shouldn't happen, just continue
            }
            plist[i] = -1; // because new process could take its place under the same PID (any process really)
            if (sig == GG_STOPONE && pkilled == 1) break;
        }
        //sleep(1);
    }
    if (sig == GG_QUIT) { // kill self if term signal
        munmap(golfmem, sizeof(shbuf));
        // We do not unlink shared memory as there's really no need for it, and in edge cases of timing, it can cause issues
        //shm_unlink (GG_SHMEM);
        if (lfd != -1) { unlockfile(lfd); close (lfd); }
        close (sockfd);
        log_msg ("Terminating process manager (me)");
        exit(0);
    }
}

//
// Get shared memory for this app, and attach to it. Client-server communication.
//
static void getshm() {
    int ggmemid;
    // The shared memory segment is created and never destroyed, since daemons can come and go
    char shmem[NAME_MAX];
    snprintf (shmem, sizeof(shmem), "%s_%s", GG_SHMEM, golfapp);
    ggmemid = shm_open (shmem, O_CREAT|O_RDWR, 0600);
    if (ggmemid == -1) exit_error (-1,"Cannot create shared memory [%s]", GG_FERR);
    if (ftruncate (ggmemid, sizeof (shbuf)) == -1) exit_error (-1,"Cannot set shared memory size [%s]", GG_FERR);
    golfmem = mmap (NULL, sizeof(shbuf), PROT_READ|PROT_WRITE, MAP_SHARED, ggmemid, 0);
    if (golfmem == MAP_FAILED) exit_error (-1,"Cannot get shared memory [%s]", GG_FERR);
    atomic_store_explicit (&(golfmem->command) , GG_DONE, memory_order_seq_cst);
}

//
// Check in server if there's client command.
// is_running_server is true if this is the established resident process here (not the fork of a client trying to become one); 
// that's true only in the resident process itself (which is what
// is_running_server conveys).
//
static void checkshm(bool is_running_server) {
    gg_num command = atomic_load_explicit(&(golfmem->command), memory_order_seq_cst);
    gg_num data1 = atomic_load_explicit(&(golfmem->data1), memory_order_seq_cst);
    if (command == GG_COMMAND) {
        log_msg ("Received command [%ld]", data1);
        // we don't send a response, just make sure we don't do it again
        if (data1 == GG_RESTART) {
            // restart stops, then immediately restarts
            temp_no_restart = 0;
            handlestop (GG_STOP);            
            log_msg ("Before process up restart");
            processup(0);
            log_msg ("After process up restart");
            atomic_store_explicit (&(golfmem->command) , GG_DONE, memory_order_seq_cst);
        } else if (data1 == GG_START) {
            temp_no_restart = 0;
            log_msg ("Before process up");
            processup(0);
            log_msg ("After process up");
            atomic_store_explicit (&(golfmem->command) , GG_DONE, memory_order_seq_cst);
        } else if (data1 == GG_STATUS) {
            atomic_store_explicit (&(golfmem->data1) , GG_OKAY, memory_order_seq_cst);
            atomic_store_explicit (&(golfmem->command) , GG_DONE, memory_order_seq_cst);
        } else if (data1 == GG_OKAY_STARTED) {
            if (is_running_server) return; // this is client trying to start the server and it's child should answer this; not the running server
            atomic_store_explicit (&(golfmem->command) , GG_DONESTARTUP, memory_order_seq_cst);
        } else if (data1 == GG_OKAY_RUNNING) {
            if (is_running_server) return; // this is client trying to start the server and it's child should answer this; not the running server
            atomic_store_explicit (&(golfmem->command) , GG_DONESTARTUP0, memory_order_seq_cst);
        } else if (data1 == GG_STOP) {
            handlestop (GG_STOP);            
            temp_no_restart = 1;
            atomic_store_explicit (&(golfmem->command) , GG_DONE, memory_order_seq_cst);
        } else if (data1 == GG_QUIT) {
            if (is_running_server) // this has to be here and not in else if above, b/c then the below Unknown command would run, messing up response
            {
                atomic_store_explicit (&(golfmem->command) , GG_DONE, memory_order_seq_cst); // this must come BEFORE handling quit as that will delete memory!
                handlestop (GG_QUIT); // exits, no return
            }
        }
        else
        {
            // bad command
            log_msg ("Unknown command [%ld]", data1);
            atomic_store_explicit (&(golfmem->command) , GG_DONEBAD, memory_order_seq_cst);
        }
    }
}

//
// When client sends a quit message to server, wait until it's no longer there. Otherwise, the following startup will fail,
// because the server is still running.
//
static void wait_server_end() 
{
    gg_num tries = GG_TRIES;
    while (tries-- >= 0) 
    {
        if (srvhere(GG_CHECKLOCK) != 1) {
            break;
        }
        sleepabit (mslp);
    }
}

//
// Client issuing command to server. 'comm' is command.
// This is called by client to see if server has a response.
// Also called by server to let the client know it has started (okay_started as comm) or running (okay_running).
// If called by server, then byserver is 1, otherwise 0
// There is another process by server after it forks(), those two communicate like "client" and "server"
//
static void cli_getshm(char *comm, gg_num byserver) {

    // okay_started/okay_running is internal command, not for the message list
    if (strcmp(comm, "start") && strcmp(comm, "stop") && strcmp(comm, "restart") && strcmp(comm, "quit") && strcmp(comm, "status") && strcmp(comm, "okay_started") && strcmp(comm, "okay_running")) {
        exit_error (-1,"Unrecognized command [%s]. Available commands are start, stop, restart and quit", comm);
    }

    if (byserver == 0)
    {
        int ggmemid;
        char shmem[NAME_MAX];
        snprintf (shmem, sizeof(shmem), "%s_%s", GG_SHMEM, golfapp);
        ggmemid = shm_open (shmem, O_RDWR, 0600);
        if (ggmemid == -1) 
        {
            // note that server process can exit or be killed, and shared memory will still be there; that's why we use locked files
            if (strcmp (comm, "quit")) exit_error (-1,"There is no active mgrg server named [%s]", golfapp);
            else exit(0); // quit achieved without error message
        }
        golfmem = mmap (NULL, sizeof(shbuf), PROT_READ|PROT_WRITE, MAP_SHARED, ggmemid, 0);
        if (golfmem == MAP_FAILED) exit_error (-1,"Cannot get shared memory [%s]", GG_FERR);
    }




    // check if server here if stopping, status. For restart, it doesn't matter.For quitting don't say anything.
    // if we checked this for server, then lock would be removed with srvhere(GG_CHECKLOCK)
    if (byserver == 0 && srvhere(GG_CHECKLOCK) == 0) {
        if (!strcmp (comm, "stop") ||  !strcmp (comm, "status")) exit_error (-1,"Server is not running");
        else if (!strcmp (comm, "quit")) exit(0); // for quit just exit normally
    }

    char *servererr = "Server either too busy, experiencing problems, or down";
    gg_num tries = GG_TRIES;
    // no need to check if server is processing something else if this is server
    if (byserver == 0) {
        log_msg ("Waiting for server before [%s]", comm);
        // check to see if server processing previous command. This is obviousy prone to race conditions.
        // but mgrg client is an administrative one; there is not supposed to be more than one person doing this. It is
        // not an end-user tool. If someone is issuing many calls simultaneously logged as the same OS user, you may have bigger problems.
        tries = GG_TRIES;
        while (tries-- >= 0) {
            gg_num command = atomic_load_explicit(&(golfmem->command), memory_order_seq_cst);
            if (command == GG_COMMAND) {
                sleepabit (mslp); // normally not GG_COMMAND, so null virtually always
            } else break;
        }
        gg_num command = atomic_load_explicit(&(golfmem->command), memory_order_seq_cst);
        if (command == GG_COMMAND) exit_error (-1,"Cannot contact server: %s", servererr);
    }
    // Now issue the actual command, know the server is in a good state (most likely)
if (!strcmp (comm, "start")) 
            atomic_store_explicit (&(golfmem->data1) , GG_START, memory_order_seq_cst);
else if (!strcmp (comm, "stop")) 
            atomic_store_explicit (&(golfmem->data1) , GG_STOP, memory_order_seq_cst);
else if (!strcmp (comm, "restart")) 
            atomic_store_explicit (&(golfmem->data1) , GG_RESTART, memory_order_seq_cst);
else if (!strcmp (comm, "quit")) 
            atomic_store_explicit (&(golfmem->data1) , GG_QUIT, memory_order_seq_cst);
else if (!strcmp (comm, "status")) 
            atomic_store_explicit (&(golfmem->data1) , GG_STATUS, memory_order_seq_cst);
else if (!strcmp (comm, "okay_started")) 
            atomic_store_explicit (&(golfmem->data1) , GG_OKAY_STARTED, memory_order_seq_cst);
else if (!strcmp (comm, "okay_running")) 
            atomic_store_explicit (&(golfmem->data1) , GG_OKAY_RUNNING, memory_order_seq_cst);
else exit_error (-1, "Unknown client command [%s]", comm);

    atomic_store_explicit (&(golfmem->command) , GG_COMMAND, memory_order_seq_cst);
    log_msg ("Sent command [%s], by server [%ld]", comm, byserver);
    tries = GG_TRIES;
    gg_num command;
    gg_num data1;
    while (tries-- >= 0) {
        command = atomic_load_explicit(&(golfmem->command), memory_order_seq_cst);
        data1 = atomic_load_explicit(&(golfmem->data1), memory_order_seq_cst);
        if (command != GG_COMMAND) {
            if (command == GG_DONEBAD) exit_error (-1,"Unknown command [%s]", comm);
            // if asked for status, print what server responded. Use stdout, as no dup2 took place.
            if (!strcmp (comm, "status")) out_msg ("%s\n", data1==GG_OKAY?"okay":"not running");
            log_msg ("Got server response on [%s], by server [%ld] command [%ld]", comm, byserver, command);
            // for quit wait until it's actually gone; this is because a message from a killed server cannot be reliable sent back; because
            // the server and its shared memory are gone!
            // only wait if not server (i.e. client)
            if (byserver == 0 && !strcmp (comm, "quit")) wait_server_end();
            return;
        } 
        sleepabit (mslp);
    }
    log_msg ("Couldn't get reply from sending to server the command [%s], shared memory status command [%ld], by server [%ld]", comm, command, byserver);
    
    // this is now failure, we got nothing from server, either it died, or was never there
    gg_num ccom = atomic_load_explicit(&(golfmem->command), memory_order_seq_cst);
    atomic_store_explicit (&(golfmem->command) , GG_DONE, memory_order_seq_cst); // always reset so that previous command doesn't stay in memory if previous server went away
                            // for example if previous server didn't exist or timed out, GG_COMMAND may stay and confuse the
                            // following client

    // do not say any error happened if it was "quit", that's the exception. "quit" on an already quit server shouldn't cause an error.
    // another exception is "okay_started/okay_running", 
    // this is server telling client it started. If client dies before getting this (which is 
    // true in if below), there is no reason for server to die too.
    if (ccom == GG_COMMAND) {
        if (strcmp (comm, "quit") && strcmp (comm, "okay_started") && strcmp (comm, "okay_running")) exit_error (-1,"Request [%s] sent, but cannot get response from %s: %s", comm, byserver==1 ? "client":"server", servererr);
    }
    // for quit wait until it's actually gone; this is because a message from a killed server cannot be reliable sent back; because
    // the server and its shared memory are gone! This bit here is if getting response back (above) failed.
    // only wait if not server (i.e. client)
    if (byserver == 0 && !strcmp (comm, "quit")) wait_server_end();
}

//
// Display usage 
//
static void usage(int ec)
{
    fprintf(stderr, "%s", usage_message);
    fflush (stderr);

    exit(ec);
}

//
// Error out as 'not found', show err. Used if command (srvc) not found
//
static void msg_notfound(char *err) {
    if (exenotfound == 0) {
        exenotfound = 1;
        log_msg ("Cannot find command [%s], [%s]", command, err);
    }
}

//
// Start a child srvc process. sockfd is listening socket created in server,
// command is the executable, pcount is the index in plist[] list of processes where
// the newly created process will be registered at.
//
static void start_child (char *command, gg_num pcount) {
    gg_num fres;
    struct stat sbuff;
    // check if command exists and save its modification time - used to restart if changed
    if (stat(command, &sbuff) != 0) { msg_notfound(GG_FERR); return;}
    // wait for access, if executable is being objcopied or otherwise locked
    // wait for 5 secs
    gg_num retries = 50;
    while (1) { // access may be temporarily denied, wait 5 seconds
                // in 0.1 sec increments until done or fails
        retries --;
        if (access(command, X_OK) != 0) {
            if (retries <= 0) { log_msg ("File not accessible to start [%s]", GG_FERR); return;}
            sleepabit (100);
            continue;
        } else break;
    }
    //
    if (commtime == NULL) GG_ANN (commtime = (struct timespec*)malloc (sizeof(struct timespec)));
    commtime->tv_sec =  sbuff.st_mtim.tv_sec;
    commtime->tv_nsec =  sbuff.st_mtim.tv_nsec;

    exenotfound = 0; // execution okay, if not found again, allow to log message 

    pid_t ppid = getpid(); // get process id, used below to set death signal to child if parent dead
    //
    // Fork for a final child, to prevent daemon processes (Golf SERVICE programs) from ever being able to acquire controlling terminal
    fres = fork();
    if (fres == 0) {
        // FINAL CHILD, CANNOT USE LOG_MSG ANYMORE!!!!

        // If parent dies without being able to tell children about it, kill children immediately. Otherwise,
        // children are likely sitting in accept() call and they will continue to take the next call before SIGTERM
        // would catch up with them. This may be problematic if a new version is installed. This KILL signal is used
        // only if parent is basically killed (not with -m stop), so the whole set of processes goes down with it, as
        // that was likely the intention.
        int res = prctl(PR_SET_PDEATHSIG, SIGKILL);
        // The prctl will fail if parent died between the fork() above and prctl(). Here we check if parent is still
        // around. If not, then child dies right away too.
        if (res == -1 || ppid != getppid()) {
            exit_error (-1,"Cannot set parent death signal [%s]", GG_FERR);
        }


        // umask to 0. For golf, it doesn't really matter because it sets own umask. In general, a good idea
        umask (0);

        //restore ignored signals in parent to default in child
        signal(SIGCHLD, SIG_DFL); 
        signal(SIGPIPE, SIG_DFL);
        // make process ID of child separate from parent, so signals do not propagate automatically. New pgid is the same as child PID.
        //setpgid (getpid(), 0);
        // make file descriptor 0 (stdin) but points to socket
        close(0);
        close(1);
        close(2);
        if (dup (sockfd) != 0) {
            exit_error (-1,"Cannot dup socket to 0 [%s]", GG_FERR);
        }
        if (dup (sockfd) != 1) {
            exit_error (-1,"Cannot dup socket to 1 [%s]", GG_FERR);
        }
        if (dup (sockfd) != 2) {
            exit_error (-1,"Cannot dup socket to 2 [%s]", GG_FERR);
        }

        if (lfd != -1) close(lfd); // close lock file (since lock doesn't propogate to child anyway)

        // execvpe can easily fail when command's time stamp changes and the process is to be restarted. Typically it's
        // permission denied, and it happens because we want to restart process when exec changes. But exec doesn't change instantly,
        // and while it's changing, there are typically 5-10 failures here if execvp here runs right after exec timestamp
        // change detected. We must exit, and that's fine, since this process will be restarted with feature that brings up
        // dead processes. We also add a bit of sleep when change detected, after checkmod().
        // Note that we put environment in gg_env - we intentionally clean up environment so service programs do not get confused with
        // possibly random environment variables. TODO: add environment variables to this list through startup mgrg process (somehow, not
        // necessarily via actual environment), maybe a file with A=B, or a string 'X=Y Z=W' possibly this.
        //
        // Setup environment
        char **gg_env;
        char *em = "Too many environment variables";
        gg_num tot_env = (1)+1; // for 1 env var plus one NULL at the end
        gg_env = (char**)malloc(sizeof(char*) * tot_env);
        char o_sil[100];
        snprintf (o_sil, sizeof(o_sil), "GG_SILENT_HEADER=%s", silent?"yes":"no");
        gg_num curr_env = 0;
        gg_env[curr_env++] = o_sil;
        gg_env[curr_env] = NULL;
        if (curr_env >= tot_env) exit_error (-1,em);
        //
        // Execute service process
        //
        if (execvpe(command, xarg, gg_env)) exit(-1);
    } else if (fres == -1) {
        // ERROR
        exit_error(-1,"Cannot fork child process [%s]", GG_FERR);
    } else {
        // PARENT (NEW PARENT REMAINING)
        //

        plist[pcount] = fres;
        log_msg ("Started service process, PID [%ld]", fres);
    }
}

//
// Return how many srvc processes are actually running. Used for adaptive to determine if to start more
// or trim some.
//
static gg_num totprocess() {
    gg_num i;
    gg_num tot = 0;
    for (i = 0; i < num_process; i++) {
        if (plist[i] == -1) { 
            continue;
        }
        pid_t p = whatisprocess (plist[i], NULL);
        if (p == -1)  {
            plist[i] = -1; // set process as dead
            continue;
        }
        tot++;
    }
    return tot;
}

//
// Create new srvc processes that are needed. If adaptive, it will start as many as needed to keep the minimum.
// Also if adaptive, and if addone is 1, it will start a new srvc process. If not adaptive, it will keep number of
// processes and num_process. Existing processes are examined (how many are running), so depending on the above,
// new processes are started (or not), to keep up with the command-line instructions (such as -w, --min-worker, 
// -max-worker).
//
static void processup(char addone) {
    gg_num i;
    gg_num tot = 0;
    // first determine how many processes are running
    if (adapt == 1) {
        tot = totprocess();
    } else tot = 0; // for adapt == 1, it doesn't matter what tot is

    // then restart according to mode we're in
    for (i = 0; i < num_process; i++) {
        if (plist[i] == -1) { 
            if (adapt == 0 || (adapt == 1 && (num_to_start_min > tot || addone == 1)))  {
                log_msg ("Re-starting child because we shut it down previously");
                start_child(command, i); 
                tot++;
                if (addone == 1) break; // needed to add just one, done
                continue; 
            }
        }
        else {
            pid_t p = whatisprocess (plist[i], NULL);
            if (p == -1) {
                pid_t pid = plist[i];
                plist[i] = -1; // set process as dead
                if (adapt == 0 || (adapt == 1 && (num_to_start_min > tot || addone == 1)))  {
                    log_msg ("Re-starting child because PID [%ld] detected down", pid);
                    start_child (command, i);
                    tot++;
                    if (addone == 1) break; // needed to add just one, done
                }
            }
        }
    }
    //sleep(1);
}

//
// Main server program
//
int main(int argc, char **argv)
{
    //
    // If a program is devel (not release), give ptracer privilege to anyone who otherwise has privileges on it
    // This isn't something that *must* succeed, we will not stop or issue message if it doesn't as it may cause malfunctioning otherwise
    // We would check if -1 if we ever  do that.
#ifdef GG_DEVEL
    prctl(PR_SET_PTRACER, PR_SET_PTRACER_ANY);
#endif

    gg_num c;

    //
    // Process command-line options
    //
    struct option opts[] = {
        {"min-worker",   required_argument, 0, 0},
        {"max-worker",  required_argument, 0, 0},
        {0, 0, 0, 0}
    };
    int oind;
    gg_num minmaxset = 0;
    gg_num numworkset = 0;
    gg_num tspikeset = 0;


    while ((c = getopt_long(argc, argv, "ft:l:dgnu:eir:xs:a:m:vc:p:w:hz", opts, &oind)) != -1) {
        switch (c) {
            case -1: break;
            case 0:
                if (!strcmp (opts[oind].name, "min-worker")) {
                    num_to_start_min = atol (optarg);
                    if (num_to_start_min < 0 || num_to_start_min >= SOMAXCONN - 10) exit_error (-1,"Minimum number of workers must be between 0 and %ld", SOMAXCONN -10);
                } else if (!strcmp (opts[oind].name, "max-worker")) {
                    maxproc = atol (optarg);
                }
                minmaxset = 1;
                break;
            case 'v':
                out_msg ("Golf Service Manager version [%s].", GG_PKGVERSION);
                exit (0);
                break;
            case 'f':
                fg = 1;
                break;
            case 'u':
                GG_ANN (run_user = strdup (optarg));
                break;
            case 'n':
                dnr = 1;
                break;
            case 'z':
                silent = true;
                break;
            case 'g':
                modreload = 0;
                break;
            case 'i':
                initit = 1;
                break;
            case 'e':
                showinfo = 1;
                break;
            case 'x':
                unix_sock = 1;
                break;
            case 'a':
                GG_ANN (parg = strdup (optarg));
                break;
            case 'm':
                GG_ANN (client_msg = strdup (optarg));
                break;
            case 'h':
                usage(0);
                break;
            case 'r':
                GG_ANN (proxy_grp = strdup (optarg));
                struct group *grp;
                if ((grp = getgrnam(proxy_grp)) == NULL) 
                {
                    exit_error (-1,"Unknown group [%s], [%s]", proxy_grp, GG_FERR);
                }
                proxy_grp_id = grp->gr_gid;
                break;
            case 't':
                tspike = atol (optarg);
                if (tspike < 5 || tspike >= 86400) exit_error (-1,"Timeout for releasing processes must be between 5 and 86400");
                tspikeset = 1;
                break;
            case 'l':
                maxblog = atol (optarg);
                if (maxblog < 10 || maxblog >= SOMAXCONN) exit_error (-1,"Listening backlog size must be between 10 and %ld", SOMAXCONN-1);
                break;
            case 'd':
                adapt = 1;
                break;
            case 'c':
                GG_ANN (command = strdup (optarg));
                break;
            case 's':
                mslp = atoi(optarg);
                if (mslp < 100 || mslp > 5000) exit_error (-1,"sleep (in milliseconds) must be between 100 and 5000");
                break;
            case 'p':
                port = atoi(optarg);
                if (! port) {
                    usage(-1);
                }
                break;
            case 'w':
                num_to_start_min = atoi(optarg);
                if (num_to_start_min < 1 || num_to_start_min >= SOMAXCONN) {
                    exit_error (-1,"Number of workers must be between 1 and %ld", SOMAXCONN);
                }
                maxproc = num_to_start_min; 
                numworkset = 1;
                break;
            case '?':
                exit_error(-1,"Unrecognized option [-%c]", optopt);
                break;
            default:
                exit_error(-1,"Unrecognized option");
                break;
        }
    }


    //
    // Check sanity of options, override if necessary
    //
    if (unix_sock == 0 && port == 0) unix_sock = 1;
    struct stat sbuff;
    // -d is default
    if (adapt == 0 && numworkset == 0) adapt = 1;

    if (adapt == 1) dnr = 0; // do not restart is not used in non-adaptive mode only, and must be set to 0 for adaptive
    if (minmaxset == 1 && adapt == 0) exit_error (-1,"--min-worker and --max-worker can only be used with -d (adaptive load)");
    if (adapt == 1 && numworkset == 1) exit_error (-1,"You can use either -d or -w option but not both");
    if (adapt == 0 && tspikeset == 1) exit_error (-1,"You can only use -t option with -d option");
    if (adapt == 0 && maxproc == 0) maxproc = 3;
    if (adapt == 1 && maxproc == 0) maxproc = 20;
    if (maxproc < 1 || maxproc >= SOMAXCONN) exit_error (-1,"Maximum number of workers must be between 1 and %ld", SOMAXCONN);
    if (minmaxset == 1 && maxproc<=num_to_start_min) exit_error (-1,"Maximum number of workers must be at least one above minimum number of workers");

    // Create and initialize list of srvc child processes
    GG_ANN (plist = (pid_t*)calloc(maxproc, sizeof(pid_t)));
    gg_num i;
    for (i = 0; i < maxproc; i++) plist[i] = -1; // set as inactive processes
    
    // get app name
    if (optind <= argc - 1) GG_ANN (golfapp = strdup (argv[optind++]));
    else {
        if (golfapp[0] == 0) exit_error (-1,"Application name must be specified as an argument [%ld,%ld]", optind, argc-1);
    }
    gg_num l;
    if ((l = strlen (golfapp)) > 30) exit_error (-1,"Application name [%s] too long", golfapp);
    for (i = 0; i < l; i++) 
    {
        if (isalnum(golfapp[i])) continue;
        if (golfapp[i]=='_') continue;
        if (golfapp[i] == '-') { continue; }
        exit_error (-1,"Application name can be comprised only of underscore, hyphen, digits and characters, found [%s]", golfapp);
    }
    if (!isalpha(golfapp[0])) exit_error (-1,"Application name must start with alphabetic character, found [%s]", golfapp);

    // Command line checks
    char ipath[GG_MAX_OS_UDIR_LEN];
    if (optind != argc) exit_error (-1,"Extra unwanted arguments on the command line [%s]", argv[optind]);
    if (unix_sock == 1 && port != 0) exit_error (-1,"Cannot use both unix socket and TCP socket. Use one or the other");


    //
    // get user and group id that is desired. 
    // -u is used only with -i to setup dirs
    //
    struct passwd* pwd;
    if (run_user[0] == 0) {
        if (initit == 1) exit_error (-1,"You must specify the user who owns the application, in order to initialize it");
        if ((pwd = getpwuid (geteuid())) == NULL) exit_error (-1,"Cannot find current user [%s]", GG_FERR);
    } else {
        if (initit == 0) exit_error (-1,"User (-u) can be specified only when initializing (-i)");
        if ((pwd = getpwnam(run_user)) == NULL) exit_error (-1,"Cannot find user [%s], [%s]", run_user, GG_FERR);
        free (run_user); // it was allocated by strdup in options processing, will be allocated again, just below
    }
    GG_ANN (run_user = strdup (pwd->pw_name));
    run_user_id = pwd->pw_uid;
    run_user_grp_id = pwd->pw_gid; // group of current user

    // Check if -i options, and if it is, initialize app. Every Golf app must have this done.
    // Recommended for all apps.
    if (initit == 1) {
        //
        // BEGIN ROOT - this section is the only time mgrg is allowed as root
        // This is mgrg -i setup, the only time we may need root privs. Most of the time we don't
        // This is for when we need to set socket privs to be a group of another user
        //if (seteuid (0) == 0) exit_error (-1,"You cannot run as root");


        // create ID file for application that states application name. Used by gg to work
        // without having to specify application name
        char *vfile = ".vappname";
        FILE *f = fopen (vfile, "w+");
        if (f != NULL) {
            fprintf(f, "%s", golfapp);
            fclose(f);
            initfile (vfile, 0700, run_user_id, run_user_grp_id);
        } // don't do anything if can't write (no error), just specify name in gg

        //
        // NOTE all gg_dir has run_user as the last argument, if not, all dirs would be /root when we run sudo mgrg
        // and we do that when we need to set socket to group ID of some other user (like apache GUID etc)
        //
        //
        // create .golf/apps dir, 711 so socket can pass through
        //
        gg_dir (GG_DIR_APPS, ipath, sizeof(ipath), NULL, run_user);
        initdir (ipath, 0711, run_user_id, run_user_grp_id);
        //
        // create .golf/apps/<app>
        //
        gg_dir (GG_DIR_APPNAME, ipath, sizeof(ipath), golfapp, run_user);
        owned (ipath, run_user_id); // make sure no one else already took this
        initdir (ipath, 0711, run_user_id, run_user_grp_id);
        //
        // create .golf/apps/<app>/app dir
        //
        gg_dir (GG_DIR_APP, ipath, sizeof(ipath), golfapp, run_user);
        owned (ipath, run_user_id); // make sure no one else already took this
        initdir (ipath, 0700, run_user_id, run_user_grp_id);
        //
        // Make creation artifact, used to recreate the application
        // Must be aftere the above GG_DIR_APP is created or the dir may not exist for the artifact here
        //
        char artname[GG_MAX_OS_UDIR_LEN];
        gg_dir (GG_DIR_ART, artname, sizeof(artname), golfapp, run_user);
        FILE *art = NULL; // artifact of creation
        if ((art = fopen (artname, "w+")) == NULL) exit_error (-1,"Cannot open creation artifact file [%s]", artname);
        // add if sudo
        if (geteuid() == 0) { if (fputs ("sudo ", art) == EOF) exit_error (-1,"Cannot write creation artifact file [%s]", artname); }
        // add all params ([0] is the program name)
        for (int i = 0; i < argc; i++) { if (fputs (argv[i], art) == EOF ||  fputs (" ", art) == EOF) exit_error (-1,"Cannot write creation artifact file [%s]", artname);; }
        fclose (art);
        //
        // create ./golf/apps/<app>/.bld dir
        //
        gg_dir (GG_DIR_BLD, ipath, sizeof(ipath), golfapp, run_user);
        owned (ipath, run_user_id); // make sure no one else already took this
        initdir (ipath, 0700, run_user_id, run_user_grp_id);
        //
        // create .golf/apps/<app>/file
        //
        gg_dir (GG_DIR_FILE, ipath, sizeof(ipath), golfapp, run_user);
        initdir (ipath, 0700, run_user_id, run_user_grp_id);
        //
        // create .golf/apps/<app>/file/t
        //
        gg_dir (GG_DIR_TMP, ipath, sizeof(ipath), golfapp, run_user);
        initdir (ipath, 0700, run_user_id, run_user_grp_id);
        //
        // create .golf/apps/<app>/trace
        //
        gg_dir (GG_DIR_TRACE, ipath, sizeof(ipath), golfapp, run_user);
        initdir (ipath, 0700, run_user_id, run_user_grp_id);
        //
        // create .golf/apps/<app>/db
        //
        gg_dir (GG_DIR_DB, ipath, sizeof(ipath), golfapp, run_user);
        initdir (ipath, 0700, run_user_id, run_user_grp_id);
        //
        // create .golf/apps/<app>/.sock (with group ownership of reverse proxy)
        //
        // socket directory with sticky bit (01) so no one can delete or rename socket inside, even if they have write privileges
        // also SETGUID bit (02) so the privileges of this directory are passed down to socket when it's actually created in bind()
        // so this is them 01+02=03
        // when we create dir, we use 5 and not 6, so no one else can create files here, but when we bind() socket, we set to 6 (so it can be written by
        // remote process). Very precise to balance security vs utility.
        if (proxy_grp[0] != 0) {
            struct group *grp;
            if ((grp = getgrnam(proxy_grp)) == NULL) exit_error (-1,"Unknown group [%s], [%s]", proxy_grp, GG_FERR);
            proxy_grp_id = grp->gr_gid;
            //
            gg_dir (GG_DIR_SOCK, ipath, sizeof(ipath), golfapp, run_user);
            // 03760 = stick+setguid+all for user+rx for group+none for other
            initdir (ipath, 03750, run_user_id, proxy_grp_id);
        } else {
            // allow anyone to create connection
            gg_dir (GG_DIR_SOCK, ipath, sizeof(ipath), golfapp, run_user);
            // 03755 = stick+setguid+all for user+rx for group+rx for other
            initdir (ipath, 03755, run_user_id, proxy_grp_id);
        }
        gg_dir (GG_DIR_MEM, ipath, sizeof(ipath), golfapp, run_user);
        // do not recreate file, as that leaves current server's memory in a limbo - will never receive anything
        if (stat(ipath, &sbuff) != 0) {
            gg_num f;
            if ((f = open (ipath, O_CREAT | O_RDWR | O_TRUNC, 0700)) == -1) exit_error (-1,"Cannot create mem file [%s] [%s]", ipath, GG_FERR); else close(f);
        }
        if (chown (ipath, run_user_id, run_user_grp_id)!= 0) exit_error (-1,"Cannot change the ownership/group of shmem key file [%s], [%s]", ipath, GG_FERR);
        if (chmod(ipath, 0700) != 0) exit_error (-1,"Cannot set permissions for shmem key file [%s]", GG_FERR);
        exit (0);
        //
        // END of what may be ROOT
        //
        //
        // End of init
        //
    }

    runasuser(); // make sure mgrg application NEVER runs as root

    // check if this app initialized
    gg_dir (GG_DIR_APP, ipath, sizeof(ipath), golfapp, NULL);
    if (stat(ipath, &sbuff) != 0) exit_error (-1,"Directory [%s] does not exist or cannot be accessed", ipath); 
    gg_dir (GG_DIR_MEM, ipath, sizeof(ipath), golfapp, NULL);
    if (stat(ipath, &sbuff) != 0) exit_error (-1,"File [%s] does not exist or cannot be accessed", ipath); 

    // This is if there is -m <command>, run mgrg as client, and exit right away
    // this does not require root privilege
    if (client_msg[0] != 0) {
        cli_getshm (client_msg, 0);
        munmap(golfmem, sizeof(shbuf));
        exit(0);
    }

    //
    // This is now server only
    //
    // get shared memory for client commands
    getshm();

    // check sanity
    if ((unix_sock == 0 && port == 0) || golfapp[0] == 0) {
        usage(-1);
    }

    gg_dir (GG_DIR_BLD, ipath, sizeof(ipath), golfapp, NULL);
    // use ./command for something in current directory or a full path
    if (command[0] == 0) {
        // if no command, assume standard Golf format - in golf's bld directory
        GG_ANN (command = (char*)malloc (GG_MAX_FILELEN+10)); // +10 for gcc to not complain about below snprintf
        snprintf (command, GG_MAX_FILELEN+10,  "%s/%s.srvc", ipath, golfapp);
    } else {
        char *s = strchr (command, '/');
        if (s == NULL) {
            // if command without path, place the file in golf's bld directory
            char *nc;
            GG_ANN (nc = (char*)malloc (GG_MAX_FILELEN + 10)); // +10 for gcc about below snprintf
            snprintf (nc, GG_MAX_FILELEN+10, "%s/%s", ipath,  command);
            command = nc; // old command is lost, but no big deal; freeing would work but any change in logic might cause sigseg later
        }
    }


    tokarg(); // get -a args for srvc processes

    signal(SIGCHLD, SIG_IGN); // avoid defunct processes when children killed - we do not expect return value
    signal(SIGPIPE, SIG_IGN); // ignore broken pipe


// check if command exists and is executable
    if (access(command, X_OK) != 0) {
        exit_error (-1,"Command [%s] does not exist, is not executable, or you have no permissions to execute it", command);
    }

    gg_num init_start;
    init_start = num_to_start_min; // min srvc processes to start

    // log file defined outside fork as it must be visible to parent AND child
    char logn[GG_MAX_OS_UDIR_LEN];
    gg_dir (GG_DIR_MGRG, logn, sizeof(logn), golfapp, NULL);

    atomic_store_explicit (&(golfmem->command) , GG_DONE, memory_order_seq_cst); // always reset so that previous command doesn't stay in memory if previous server went away
    
    static gg_num opid;
    opid =(gg_num)getpid();
    pid_t deadres; // used for first round of forking, which isn't relevant
    // if foreground mode, do not fork
    if (fg == 0) deadres=fork(); else deadres = 0;
    signal(SIGCHLD, SIG_DFL);  // we need this so that we can waitpid on a child (the resident process below). This is needed when we check
                               // if it's done, such as when server is already running, and we exit with status 2 which we want to collect.
                               // Since we will wait on it first (see below), it won't become a zombie
    atomic_store_explicit (&(golfmem->command) , GG_OKAY, memory_order_seq_cst); // clear up command flag, so any leftover is not confusing the processes below
    if (deadres == 0) {
// so we can attach to process with gdb without sudo
#ifdef GG_DEVEL
        prctl(PR_SET_PTRACER, PR_SET_PTRACER_ANY);
#endif
        // THIS IS THE RESIDENT SERVER PROCESS (child of original)

        // create new session so that terminating current logged-on session will not send terminating signals to here
        // not as important for Golf, as it handles all signals, in general a good idea
        if (fg == 0) { // create session only if running in background
            if ((sid = setsid()) < 0) {
                exit_error (-1,"Cannot create new session group [%s]", GG_FERR);
            }
        }
    

        // up to this point, all output is stdout, stderr. Now it's log file, but just for current invocation (does not grow)
        if (fg == 0) { // redirect stdin,out,err only if background mode, otherwise print to console
            logfile = fopen (logn, "a+");
            if (logfile == NULL) exit_error (-1,"Cannot open log file [%s]", GG_FERR);
            // close stdin,stdout,stderror, and redirect them to /var/log/golf
            close(0);
            close(1);
            close(2);
            if (dup (fileno(logfile)) != 0) { // logfileile to stdin? we will never read; must have it so children can use it to dup socket!
                exit_error (-1,"Cannot dup logfile to 0 [%s]", GG_FERR);
            } 
            if (dup (fileno(logfile)) != 1) {
                exit_error (-1,"Cannot dup logfile to 1 [%s]", GG_FERR);
            }
            if (dup (fileno(logfile)) != 2) {
                exit_error (-1,"Cannot dup logfile to 2 [%s]", GG_FERR);
            }
            initfile (logn, 0700, run_user_id, proxy_grp_id);
        }
        islogging=1;
        log_msg ("New process forked with [%ld] parent pid", opid);

        // Locking here to make sure only one resident server process remains, since this lock
        // doesn't propogate to children
        if (srvhere(GG_LOCK) == 1) {
            // if server already running, communicate to front end that it is okay, then exit
            if (fg == 0) cli_getshm ("okay_running", 1); // nobody to communicate with unless forked first
            exit_error (2,"The Golf process manager for application [%s] is already running", golfapp);
        }

        log_msg ("Golf Service Manager v%s starting", GG_PKGVERSION);
        log_msg ("Command [%s], port [%ld], workers [%ld], user [%s], name [%s], sleep [%ld], minmaxset [%ld], min-workers [%ld], max-workers [%ld]", command, port, num_to_start_min, run_user, golfapp, mslp, minmaxset, num_to_start_min, maxproc);

         
        // Binding must be here, because it has to be after locking checked in srvhere. This is especially important
        // for unix sockets, where next bind overwrites the previous, and all processes that listened to the previous are just...hanging
        if (unix_sock == 1) sockfd = socket(AF_UNIX, SOCK_STREAM, 0);
        else sockfd = socket(AF_INET, SOCK_STREAM, 0);
        if (sockfd == -1) {
            exit_error (-1,"Cannot create socket [%s]", GG_FERR);
        } 

        if (unix_sock == 0) {
            // reuse address, port; get load balancing; only the original user can take advantage of reusing port
            if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEPORT, &(int){1}, sizeof(int)) < 0) {
                exit_error (-1,"Cannot set socket option (1) [%s]", GG_FERR);
            }
        }

        gg_num rv;
        log_msg ("Before binding socket");
        if (unix_sock == 1) {
            struct sockaddr_un servaddr;
            memset(&servaddr, 0, sizeof(struct sockaddr_un));
            servaddr.sun_family = AF_UNIX; 
            // get sock file path, put it into structure to bind socket from
            char ipath[GG_MAX_OS_UDIR_LEN];
            gg_dir (GG_DIR_SOCKFILE, ipath, sizeof(ipath), golfapp, NULL);
            strncpy (servaddr.sun_path, ipath, sizeof(servaddr.sun_path)); 
            servaddr.sun_path[sizeof(servaddr.sun_path)-1] = 0;
            //
            if (unlink(servaddr.sun_path) != 0) if (errno != ENOENT) exit_error (-1,"Cannot unlink unix domain socket file [%s]", GG_FERR);
            if (bind(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr)) != 0) exit_error (-1,"Cannot bind unix domain socket [%s], [%s]", servaddr.sun_path, GG_FERR);
            if (proxy_grp[0] != 0) {
                // create .golf/apps/<app>/.sock
                initfile (servaddr.sun_path, 0660, run_user_id, proxy_grp_id);
            } else {
                // allow anyone to create connection
                // create .golf/apps/<app>/.sock
                initfile (servaddr.sun_path, 0666, run_user_id, proxy_grp_id);
            }
        } else {
            struct sockaddr_in servaddr;
            bzero(&servaddr, sizeof(servaddr));
            servaddr.sin_family = AF_INET;
            servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
            servaddr.sin_port = htons(port);
            if (bind(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr)) != 0) exit_error (-1,"Cannot bind socket [%s]", GG_FERR);
        }
        log_msg ("After binding socket [%ld]", opid);
        
        if ((rv = listen(sockfd, maxblog)) != 0) {
            exit_error (-1,"Cannot listen on socket [%s]", GG_FERR);
        }
        // Start the actual worker processes, we will say it started only after that's done. This we can do after this whole if()
        // which checks both forked processes, since listen() will queue up requests for processes and they won't be lost. However,
        // starting children before replying with okay_started may speed up original response time if client requests immediately
        // follow starting the server.
        // We also use init_start in the other forked process (the client), so if starting childred ever goes after this whole if()
        // then we must make a copy of init_start into another variable for this process. We can use this variable now in both processes
        // since the other forked process gets a copy, so us doing --init_start won't affect it.
        gg_num pcount = 0;
        num_process = maxproc;
        signal(SIGCHLD, SIG_IGN); // avoid defunct processes when children killed - we do not expect return value
                                  // this is for actual Golf server processes
        while (--init_start >= 0) {
            start_child (command, pcount++);
        }
        if (fg == 0) cli_getshm ("okay_started", 1); // nobody to communicate with unless forked first
        log_msg ("Golf Service Manager v%s successfully started", GG_PKGVERSION);
        // if we could not get this reply from the original below (say it died), then we place it there to simulate success, because
        // it may impact the following command (such as quit). We're clearly continuing, as the server has indeed started.
        atomic_store_explicit (&(golfmem->command) , GG_DONESTARTUP, memory_order_seq_cst);
        //
    } else if (deadres == -1) {
        exit_error (-1,"Could not start Golf Service Manager for [%s], [%s]", golfapp, GG_FERR);
    } else {
        #define GG_SRV_RUNNING "Golf Service Manager [%s] for [%s] is already running. If you want to restart it, stop it first (-m quit), then start it"
#define GG_SRV_STARTED "Golf Service Manager [%s] for [%s] successfully started"
#define GG_SRV_NOSTART "Could not start Golf Service Manager for [%s],[%ld],[1], log file [%s]"
        // ORIGINAL PARENT FROM COMMAND LINE OR A SCRIPT, deadres is the PID of the child process (the resident process)
        // get success message from child (our resident process)
        // THIS NEVER HAPPENS IN FOREGROUND MODE, as there is no fork (parent and child), but only one process
        // Try longer tries here, because this is a startup, and we know the PID of the process we're waiting for (and it's a child)
        gg_num tries = 3*GG_TRIES;
        while (tries-- >= 0) {
            int st;
            // wait on a child (i.e. resident process above) first so it never becomes a zombie
            pid_t p = whatisprocess (deadres, &st);
            if (!(p == deadres || p == -deadres))  // if resident process started above is no longer alive
            {
                if (st == 2) 
                {
                    // this is when server is already running
                    // it will be either this or the GG_SRV_RUNNING below, so this is to make sure we get that message
                    atomic_load_explicit(&(golfmem->command), memory_order_seq_cst);
                    checkshm(false);  // reset server memory
                    out_msg (GG_SRV_RUNNING, GG_PKGVERSION, golfapp); 
                    exit(1);
                } 
                else 
                {
                    // resident server process no longer alive
                    atomic_load_explicit(&(golfmem->command), memory_order_seq_cst);
                    checkshm(false);  // reset server memory
                    exit_error (-1,GG_SRV_NOSTART, golfapp, tries, logn);
                }
            }
            // Check for confirmation from server that it started, or running
            // check for client commands (client being the other process created in fork above) as a reply
            gg_num command = atomic_load_explicit(&(golfmem->command), memory_order_seq_cst);
            checkshm(false); // this must be AFTER getting golfmem->command, as that's what's checked 
            // This says ONLY that a manager daemon has started. The actual processes start asynchronously after this!!!
            if (command == GG_DONESTARTUP) {out_msg (GG_SRV_STARTED, GG_PKGVERSION, golfapp); exit(0);}
            else if (command == GG_DONESTARTUP0) {out_msg (GG_SRV_RUNNING, GG_PKGVERSION, golfapp); exit(1);}
            sleepabit (mslp);
        }
        // time out on starting, didn't get any message from server
        exit_error (-1,GG_SRV_NOSTART, golfapp, tries, logn);
    }

    // This is now resident server process checking for messages, forever or until user quits it
    
    gg_num nspike = 0;

    // monitor processes, exit is possible with quit command from mgrg client (-m option)
    while (1) {
        if (adapt == 1 && temp_no_restart != 1) {
            // adaptive mode, check if to start another srvc process
            // check if there's a request for connection, but no one is taking it, even after
            // a bit of sleep
            if (connwait() == 1) {
                nspike = 0;
                sleepabit (mslp/4);
                if (connwait() == 1)  {
                    sleepabit (mslp/4);
                    if (connwait() == 1)  {
                        if (totprocess() < num_process-1) {
                            processup(1);
                        }
                    }
                }
            } else {
                // adaptive mode, check if to stop a srvc process. Wait to see there are no connections pending
                // and using nspike variable come back here many times to amount to a tspike period of seconds of
                // no connections beyond what can be handled.
                sleepabit (mslp/4);
                if (connwait() == 0)  {
                    sleepabit (mslp/4);
                    if (connwait() == 0)  {
                        nspike++;
                        if (nspike >= (tspike * 1000)/ (mslp/2)) {
                            if (totprocess() > num_to_start_min) {
                                // kill a child since there is no extra traffic, but only if there is more than num_to_start_min processes running
                                handlestop(GG_STOPONE);
                            }
                            nspike = 0;
                        }
                    }
                }
            }
            processup(0);
        } else sleepabit (mslp);
        // check for terminated processes/restart. Don't do it if temporary pause caused by issuing stop.
        // Must issue start to get it going again. Do not do it if DNR (Do Not Restart) was specified.
        if (!(temp_no_restart == 1 || dnr == 1)) {
            // check if executable modification time changed; if so and -g not used; stop the process, then restart
            // but do this check ONLY if restarts are allowed. If stop issued, don't do it. If do-not-restart enabled, do not
            // do it - that's the external if() 
            // use sleepabit to sleep 1.4secs because file may be changing, and while it's changing the permission on it is denied
            if (modreload == 1 && checkmod() == 1) { sleepabit(500); handlestop (GG_STOP); }
            processup(0);
        }
        // check for client commands
        checkshm(true); 
    }

    // never gets here
    return 0;
}


