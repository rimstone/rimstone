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
#include <sys/shm.h>
#include <ctype.h>
#include <dirent.h>
#include <time.h>
#include <pwd.h>
#include <grp.h>
#include <fcntl.h>
#include <stdbool.h>
#include <sys/prctl.h>

// must update this if updating in golf.h
typedef long gg_num; 


// actions for processes and manager
#define GG_STOP 1
#define GG_QUIT 2
#define GG_STOPONE 3
// display error
#define GG_FERR (errno == 0 ? "" : strerror(errno))
// assert not null (used for malloc)
#define GG_ANN(x) ((x) == NULL ? exit_error("Cannot allocate memory [%s]", GG_FERR) : 0)
#define GG_MAX_CLI_WAIT 10 // max second client will wait
#define GG_MAX_ARGS 64 // max args to mgrg in -a
#define GG_MAX_FILELEN 200 // max path+file length
// directories used by initialization -i
#define GG_RUNDIR "/var/lib/gg"
#define GG_RUNNAME GG_RUNDIR "/%s"
#define GG_APPDIR GG_RUNDIR "/%s/app"
#define GG_FILEDIR GG_APPDIR "/file"
#define GG_TMPFILEDIR GG_FILEDIR "/t"
#define GG_TRACEDIR GG_APPDIR "/trace"
#define GG_DBDIR GG_APPDIR "/db"
#define GG_BLDDIR GG_RUNDIR "/bld"
#define GG_BLDAPPDIR GG_RUNDIR "/bld/%s"
#define GG_SHNAME GG_RUNNAME "/mem"
#define GG_SOCKDIR GG_RUNNAME "/sock"
#define GG_VFLOGDIR GG_RUNNAME "/mgrglog"
#define GG_SOCKNAME GG_SOCKDIR "/sock"
#define GG_LOCKNAME GG_RUNNAME "/lock"
// client commands to server and back
#define GG_COMMAND 1
#define GG_DONE 2
#define GG_DONEBAD 3
#define GG_DONESTARTUP 4
#define GG_DONESTARTUP0 5
// locks for checking server the only one running for an app
#define GG_LOCK 1
#define GG_CHECKLOCK 2
//
// now that we know the server is (most likely) running, do the handshake so we get the confirmation the job is done
// number of tries correlate to the sleep interval. Total time to wait is (GG_MAX_CLI_WAIT * 1000), and since we're checking
// every mslp , total of (GG_MAX_CLI_WAIT*1000)/mslp checks. We do +1 to get at least one try in case the total is 0.
#define GG_TRIES ((gg_num)((GG_MAX_CLI_WAIT*1000)/mslp)+1)
//
// logging stuff
#define GG_MOUT 0
#define GG_MLOG 1
#define out_msg(...) log_msg0(GG_MOUT, __VA_ARGS__)
#define log_msg(...) log_msg0(GG_MLOG, __VA_ARGS__)

// Command from client to server
typedef struct s_shbuf {
    gg_num command;
    char data1[100];
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
static shbuf *shm; // shared memory for communication btw client and server
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
static pid_t whatisprocess(pid_t p);
static void log_msg0(gg_num dest, char *format, ...);
static void exit_error(char *format, ...);
static void getshm();
static void checkshm();
static void cli_getshm(char *comm, gg_num byserver);
static void usage(int ec);
static void start_child (char *command, gg_num pcount);
static void processup(char addone);
static void tokarg();
static void handlestop(gg_num sig);
static void sleepabit(gg_num milli);
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
    if (chown (ipath, uid, guid)!= 0) exit_error ("Cannot change the ownership of file [%s], [%s]", ipath, GG_FERR);
    if (chmod(ipath, mode) != 0) exit_error ("Cannot set permissions for file [%s]", GG_FERR);
}


//
// Initialize directory ipath, with mode 'mode', to be owned by user group uid/guid
//
static void initdir (char *ipath, gg_num mode, uid_t uid, gid_t guid) {
    log_msg ("Creating directory [%s]", ipath);
    if (mkdir (ipath, mode) != 0) if (errno != EEXIST) exit_error ("Cannot create directory [%s], [%s]", ipath, GG_FERR); 
    if (chown (ipath, uid, guid)!= 0) exit_error ("Cannot change the ownership of directory [%s], [%s]", ipath, GG_FERR);
    if (chmod(ipath, mode) != 0) exit_error ("Cannot set permissions for directory [%s]", GG_FERR);
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
            if ((pwd = getpwuid (sbuff.st_uid)) == NULL) exit_error ("Cannot find user who owns [%s], user id [%ld], [%s]", ipath, sbuff.st_uid, GG_FERR);
            // must allocate new buffer as getpwuid results overwrite the old ones
            GG_ANN (userown = strdup(pwd->pw_name));
            if ((pwd1 = getpwuid (uid)) == NULL) exit_error ("Cannot find user [%ld], [%s]", ipath, uid, GG_FERR);
            GG_ANN (usernew= strdup(pwd->pw_name));
            exit_error ("Directory [%s] is already owned by another user [%s], current user [%s]", ipath, userown, usernew);
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
    if (setuid(0) == 0 || seteuid(0) == 0) exit_error ("Program can never run as root");

    char ipath[GG_MAX_FILELEN];
    snprintf (ipath, sizeof(ipath), GG_RUNNAME "/app", golfapp);
    if (chdir(ipath) != 0) exit_error ("Cannot set current directory to [%s], [%s]", ipath, GG_FERR);
}


//
// returns 1 if there is another server running, 0 if not. If op is GG_CHECKLOCK, then do not keep a lock (client).
// If it is GG_LOCK, keep a lock (only server does this).
// A file is locked to achieve one-app-one-server. Return 0 if can lock, 1 otherwise.
//
static gg_num srvhere(gg_num op) {
    char lpath[GG_MAX_FILELEN];
    if (lfd != -1) return 1; // this is when calling srvhere again in the same process like in sending okay_started/okay_running
    snprintf (lpath, sizeof(lpath), GG_LOCKNAME, golfapp);
    if (lockfile (lpath, &lfd) == 0) {
        log_msg("Another server is running, lock file failed");
        return 1;
    }
    if (op == GG_CHECKLOCK) {close (lfd); lfd = -1;}
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
// Lock a file fname, get lock file descriptor lf. 
// returns 0 if lock failed, meaning another process is locking
// and 1 if okay, meaning this is the server process
//
static gg_num lockfile (char *fname, gg_num *lf) {
    struct flock lock;
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
        if (carg >= GG_MAX_ARGS - 1) exit_error ("Too many arguments specified for service processes [%ld]", carg);
    }
    if (instring == 1) exit_error ("Unterminated string in arguments specified for service process, at [%ld]", begstring);
    xarg[carg] = NULL;
}

//
// Log message in printf-style to stdout, which can be redirected by caller. 
// If dest is GG_MLOG, then if showinfo is 1, messages still go through even if logging is off.
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
//
static void exit_error(char *format, ...) {
    time_t t;
    time(&t);
    fprintf(stderr, "%ld: %s: Error: ", (gg_num)getpid(), strtok (ctime(&t), "\n"));
    va_list args;
    va_start (args, format);
    vfprintf(stderr, format, args);
    va_end (args);
    fputs ("\n", stderr);
    fflush (stderr);
    exit(-1);
}

//
// What is the status of the process with PID p? Returns p if running, -p if running but either stopped or continued,
// -1 if process dead, 0 if cannot determine
//
static pid_t whatisprocess(pid_t p) {
    int pstat = 0; // waitpid may not set anything, so it may be random (bad)
    gg_num pid = waitpid (p, &pstat, WNOHANG);
    if (pid == 0) { // pid 0 means status not changed, child is running
        return p;
    }
    else if (pid == p) { // status has changed
        if (!WIFEXITED(pstat) && !WIFSIGNALED(pstat)) { // was not killed, must have been stopped or continued, but still up
            return -p;
        }
        else return -1; // exited or terminated by a signal
    } 
    else if (pid == -1 || errno == ECHILD) {
        return -1;// else child may have exited already or been killed, or there is a process with this PID that is not a child
    } else {
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
            pid_t p = whatisprocess (plist[i]);
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
        log_msg ("Terminating process manager (me)");
        close (sockfd);
    }
}

//
// Get shared memory for this app, and attach to it. Client-server communication.
//
static void getshm() {
    char kname[100];
    key_t shmkey;
    int shmid;
    snprintf (kname, sizeof(kname), GG_SHNAME, golfapp);
    // The shared memory segment is created and never destroyed, since daemons can come and go
    if ((shmkey = ftok (kname, 'V')) == (key_t)-1) exit_error ("Cannot create ID for shmem [%s]", GG_FERR);
    shmid = shmget (shmkey, sizeof(shbuf), IPC_CREAT | 0600);
    if (shmid < 0) exit_error ("Cannot create shared memory [%s]", GG_FERR);
    shm = (shbuf*) shmat (shmid, NULL, 0);
    if (shm == (void*)-1) exit_error ("Cannot get shared memory [%s]", GG_FERR);
    shm->command = GG_DONE;
}

//
// Check in server if there's client command.
//
static void checkshm() {
    if (shm->command == GG_COMMAND) {
        log_msg ("Received command [%s]", shm->data1);
        // we don't send a response, just make sure we don't do it again
        if (!strcmp (shm->data1, "restart")) {
            // restart stops, then immediately restarts
            temp_no_restart = 0;
            handlestop (GG_STOP);            
            processup(0);
            shm->command = GG_DONE;
        } else if (!strcmp (shm->data1, "start")) {
            temp_no_restart = 0;
            processup(0);
            shm->command = GG_DONE;
        } else if (!strcmp (shm->data1, "status")) {
            strcpy (shm->data1, "okay");
            shm->command = GG_DONE;
        } else if (!strcmp (shm->data1, "okay_started")) {
            shm->command = GG_DONESTARTUP;
        } else if (!strcmp (shm->data1, "okay_running")) {
            shm->command = GG_DONESTARTUP0;
        } else if (!strcmp (shm->data1, "stop")) {
            handlestop (GG_STOP);            
            temp_no_restart = 1;
            shm->command = GG_DONE;
        } else if (!strcmp (shm->data1, "quit")) {
            handlestop (GG_QUIT);            
            shm->command = GG_DONE;
            exit(0);
        }
        else
        {
            // bad command
            log_msg ("Unknown command [%s]", shm->data1);
            shm->command = GG_DONEBAD;
        }
    }
}

//
// Client issuing command to server. 'comm' is command.
// This is called by client to see if server has a response.
// Also called by server to let the client know it has started (okay_started as comm) or running (okay_running).
// If called by server, then byserver is 1, otherwise 0
//
static void cli_getshm(char *comm, gg_num byserver) {
    char kname[GG_MAX_FILELEN];
    key_t shmkey;
    int shmid;

    // okay_started/okay_running is internal command, not for the message list
    if (strcmp(comm, "start") && strcmp(comm, "stop") && strcmp(comm, "restart") && strcmp(comm, "quit") && strcmp(comm, "status") && strcmp(comm, "okay_started") && strcmp(comm, "okay_running")) {
        exit_error ("Unrecognized command [%s]. Available commands are start, stop, restart and quit", comm);
    }

    snprintf (kname, sizeof(kname), GG_SHNAME, golfapp);
    if ((shmkey = ftok (kname, 'V')) == (key_t)-1) exit_error ("Cannot create ID for shmem [%s]", GG_FERR);
    shmid = shmget (shmkey, sizeof(shbuf), 0600);
    // don't display error message if the intention was to quit
    if (shmid < 0) {
        if (strcmp (comm, "quit")) exit_error ("There is no active mgrg server named [%s]", golfapp);
        else exit(0); // quit achieved without error message
    }
    shm = (shbuf*) shmat (shmid, NULL, 0);
    if (shm == (void*)-1) exit_error ("Cannot get shared memory [%s]", GG_FERR);

    // check if server here - if only one processes attached to shared memory (this one, so no server) if the file lock not done
    struct shmid_ds shmstat;
    shmctl (shmid, IPC_STAT, &shmstat);
    if (shmstat.shm_nattch == 1 || srvhere(GG_CHECKLOCK) == 0) {
        if (strcmp (comm, "quit")) exit_error ("Server is not running [%ld]", shmstat.shm_nattch);
        else exit(0); // quit achieved without error message
    }

    char *servererr = "Server either too busy, experiencing problems, or down";
    gg_num tries = GG_TRIES;
    // no need to check if server is processing something else if this is server
    if (byserver == 0) {
        // check to see if server processing previous command. This is obviousy prone to race conditions.
        // but mgrg client is an administrative one; there is not supposed to be more than one person doing this. It is
        // not an end-user tool. If someone is issuing many calls simultaneously logged as the same OS user, you may have bigger problems.
        tries = GG_TRIES;
        while (tries-- >= 0) {
            if (shm->command == GG_COMMAND) {
                sleepabit (mslp); // normally not GG_COMMAND, so null virtually always
            } else break;
        }
        if (shm->command == GG_COMMAND) exit_error ("Cannot contact server: %s", servererr);
    }
    // Now issue the actual command, know the server is in a good state (most likely)
    snprintf (shm->data1, sizeof (shm->data1), "%s", comm);
    shm->command = GG_COMMAND;
    tries = GG_TRIES;
    while (tries-- >= 0) {
        if (shm->command != GG_COMMAND) {
            if (shm->command == GG_DONEBAD) exit_error ("Unknown command [%s]", comm);
            // if asked for status, print what server responded. Use stdout, as no dup2 took place.
            if (!strcmp (comm, "status")) out_msg ("%s\n", shm->data1);
            return;
        }
        sleepabit (mslp);
    }
    
    // this is now failure, we got nothing from server, either it died, or was never there
    gg_num ccom = shm->command;
    shm->command = GG_DONE; // always reset so that previous command doesn't stay in memory if previous server went away
                            // for example if previous server didn't exist or timed out, GG_COMMAND may stay and confuse the
                            // following client

    // do not say any error happened if it was "quit", that's the exception. "quit" on an already quit server shouldn't cause an error.
    // another exception is "okay_started/okay_running", 
    // this is server telling client it started. If client dies before getting this (which is 
    // true in if below), there is no reason for server to die too.
    if (ccom == GG_COMMAND) {
        if (strcmp (comm, "quit") && strcmp (comm, "okay_started") && strcmp (comm, "okay_running")) exit_error ("Request [%s] sent, but cannot get response from %s: %s", comm, byserver==1 ? "client":"server", servererr);
    }
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
    if (access(command, X_OK) != 0) { log_msg ("File not accessible to start [%s]", GG_FERR); return;}
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
            exit_error ("Cannot set parent death signal [%s]", GG_FERR);
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
            exit_error ("Cannot dup socket to 0 [%s]", GG_FERR);
        }
        if (dup (sockfd) != 1) {
            exit_error ("Cannot dup socket to 1 [%s]", GG_FERR);
        }
        if (dup (sockfd) != 2) {
            exit_error ("Cannot dup socket to 2 [%s]", GG_FERR);
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
        if (curr_env >= tot_env) exit_error (em);
        //
        // Execute service process
        //
        if (execvpe(command, xarg, gg_env)) exit(-1);
    } else if (fres == -1) {
        // ERROR
        exit_error("Cannot fork child process [%s]", GG_FERR);
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
        pid_t p = whatisprocess (plist[i]);
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
            pid_t p = whatisprocess (plist[i]);
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
                    if (num_to_start_min < 0 || num_to_start_min >= SOMAXCONN - 10) exit_error ("Minimum number of workers must be between 0 and %ld", SOMAXCONN -10);
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
                break;
            case 't':
                tspike = atol (optarg);
                if (tspike < 5 || tspike >= 86400) exit_error ("Timeout for releasing processes must be between 5 and 86400");
                tspikeset = 1;
                break;
            case 'l':
                maxblog = atol (optarg);
                if (maxblog < 10 || maxblog >= SOMAXCONN) exit_error ("Listening backlog size must be between 10 and %ld", SOMAXCONN-1);
                break;
            case 'd':
                adapt = 1;
                break;
            case 'c':
                GG_ANN (command = strdup (optarg));
                break;
            case 's':
                mslp = atoi(optarg);
                if (mslp < 100 || mslp > 5000) exit_error ("sleep (in milliseconds) must be between 100 and 5000");
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
                    exit_error ("Number of workers must be between 1 and %ld", SOMAXCONN);
                }
                maxproc = num_to_start_min; 
                numworkset = 1;
                break;
            case '?':
                exit_error("Unrecognized option [-%c]", optopt);
                break;
            default:
                exit_error("Unrecognized option");
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
    if (minmaxset == 1 && adapt == 0) exit_error ("--min-worker and --max-worker can only be used with -d (adaptive load)");
    if (adapt == 1 && numworkset == 1) exit_error ("You can use either -d or -w option but not both");
    if (adapt == 0 && tspikeset == 1) exit_error ("You can only use -t option with -d option");
    if (adapt == 0 && maxproc == 0) maxproc = 3;
    if (adapt == 1 && maxproc == 0) maxproc = 20;
    if (maxproc < 1 || maxproc >= SOMAXCONN) exit_error ("Maximum number of workers must be between 1 and %ld", SOMAXCONN);
    if (minmaxset == 1 && maxproc<=num_to_start_min) exit_error ("Maximum number of workers must be at least one above minimum number of workers");

    // Create and initialize list of srvc child processes
    GG_ANN (plist = (pid_t*)calloc(maxproc, sizeof(pid_t)));
    gg_num i;
    for (i = 0; i < maxproc; i++) plist[i] = -1; // set as inactive processes
    
    // get app name
    if (optind <= argc - 1) GG_ANN (golfapp = strdup (argv[optind++]));
    else {
        if (golfapp[0] == 0) exit_error ("Application name must be specified as an argument [%ld,%ld]", optind, argc-1);
    }
    gg_num l;
    if ((l = strlen (golfapp)) > 30) exit_error ("Application name [%s] too long", golfapp);
    for (i = 0; i < l; i++) 
    {
        if (isalnum(golfapp[i])) continue;
        if (golfapp[i]=='_') continue;
        if (golfapp[i] == '-') { continue; }
        exit_error ("Application name can be comprised only of underscore, hyphen, digits and characters, found [%s]", golfapp);
    }
    if (!isalpha(golfapp[0])) exit_error ("Application name must start with alphabetic character, found [%s]", golfapp);

    // Command line checks
    char ipath[GG_MAX_FILELEN];
    if (optind != argc) exit_error ("Extra unwanted arguments on the command line [%s]", argv[optind]);
    if (unix_sock == 1 && port != 0) exit_error ("Cannot use both unix socket and TCP socket. Use one or the other");


    //
    // get user and group id that is desired. 
    // -u is used only with -i to setup dirs
    //
    struct passwd* pwd;
    if (run_user[0] == 0) {
        if (initit == 1) exit_error ("You must specify the user who owns the application, in order to initialize it");
        if ((pwd = getpwuid (geteuid())) == NULL) exit_error ("Cannot find current user [%s]", GG_FERR);
    } else {
        if (initit == 0) exit_error ("User (-u) can be specified only when initializing (-i)");
        if ((pwd = getpwnam(run_user)) == NULL) exit_error ("Cannot find user [%s], [%s]", run_user, GG_FERR);
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
        // This is mgrg -i setup, the only time we need root privs.
        //
        if (seteuid (0) != 0) exit_error ("To perform this operation, you must run as root");

        // create ID file for application that states application name. Used by gg to work
        // without having to specify application name
        snprintf (ipath, sizeof(ipath), ".vappname");
        FILE *f = fopen (ipath, "w+");
        if (f != NULL) {
            fprintf(f, "%s", golfapp);
            fclose(f);
            initfile (ipath, 0700, run_user_id, run_user_grp_id);
        } // don't do anything if can't write (no error), just specify name in gg

        //
        // create /var/lib/gg/bld is created by install (in Makefile)
        // create /var/lib/gg/bld/<appname>
        //
        snprintf (ipath, sizeof(ipath), GG_BLDAPPDIR, golfapp);
        owned (ipath, run_user_id); // make sure no one else already took this
        initdir (ipath, 02700, run_user_id, run_user_grp_id);

        //
        // create /var/lib/gg/<appname> - this is also created by Golf in setup_hello() - MUST MATCH IT!!
        // reason being that depending on what is done first (i.e. mgrg used alone without without Golf, or Golf using mgrg)
        //
        snprintf (ipath, sizeof(ipath), GG_RUNNAME, golfapp);
        owned (ipath, run_user_id); // make sure no one else already took this
        initdir (ipath, 0755, run_user_id, run_user_grp_id);
        //
        // create /var/lib/gg/<appname>/mgrglog
        //
        snprintf (ipath, sizeof(ipath), GG_VFLOGDIR, golfapp);
        initdir (ipath, 0700, run_user_id, run_user_grp_id);
        //
        // create /var/lib/gg/<appname>/app 
        //
        snprintf (ipath, sizeof(ipath), GG_APPDIR, golfapp);
        initdir (ipath, 0700, run_user_id, run_user_grp_id);
        //
        // create /var/lib/gg/<appname>/app/file
        //
        snprintf (ipath, sizeof(ipath), GG_FILEDIR, golfapp);
        initdir (ipath, 0700, run_user_id, run_user_grp_id);
        //
        // create /var/lib/gg/<appname>/app/file/t
        //
        snprintf (ipath, sizeof(ipath), GG_TMPFILEDIR, golfapp);
        initdir (ipath, 0700, run_user_id, run_user_grp_id);
        //
        // create /var/lib/gg/<appname>/app/trace 
        //
        snprintf (ipath, sizeof(ipath), GG_TRACEDIR, golfapp);
        initdir (ipath, 0700, run_user_id, run_user_grp_id);
        //
        // create /var/lib/gg/<appname>/app/db
        //
        snprintf (ipath, sizeof(ipath), GG_DBDIR, golfapp);
        initdir (ipath, 0700, run_user_id, run_user_grp_id);
        //
        // create /var/lib/gg/<appname>/sock (with group ownership of reverse proxy)
        //
        // get proxy group id. If specified make group sticky bit on sock directory so socket
        // is always created with it. Based on this, when socket is created, we will set perms to 0660,
        // otherwise 0666, we reset here even if they already exist in case we change mode (say we allowed
        // socket to everyone, now just to group)
        if (proxy_grp[0] != 0) {
            struct group *grp;
            if ((grp = getgrnam(proxy_grp)) == NULL) exit_error ("Unknown group [%s], [%s]", proxy_grp, GG_FERR);
            proxy_grp_id = grp->gr_gid;
            snprintf (ipath, sizeof(ipath), GG_SOCKDIR, golfapp);
            initdir (ipath, 02750, run_user_id, proxy_grp_id);
            snprintf (ipath, sizeof(ipath), GG_SOCKNAME, golfapp);
            initfile (ipath, 02660, run_user_id, proxy_grp_id);
        } else {
            // allow anyone to create connection
            snprintf (ipath, sizeof(ipath), GG_SOCKDIR, golfapp);
            initdir (ipath, 02755, run_user_id, run_user_id);
            snprintf (ipath, sizeof(ipath), GG_SOCKNAME, golfapp);
            initfile (ipath, 02666, run_user_id, run_user_id);
        }
        //
        //
        // end of golf setup_hello() match
        //
        //
        //
        // END ROOT
        //
        snprintf (ipath, sizeof(ipath), GG_SHNAME, golfapp);
        // do not recreate file, as that leaves current server's memory in a limbo - will never receive anything
        if (stat(ipath, &sbuff) != 0) {
            gg_num f;
            if ((f = open (ipath, O_CREAT | O_RDWR | O_TRUNC, 0700)) == -1) exit_error ("Cannot create mem file [%s] [%s]", ipath, GG_FERR); else close(f);
        }
        if (chown (ipath, run_user_id, run_user_grp_id)!= 0) exit_error ("Cannot change the ownership/group of shmem key file [%s], [%s]", ipath, GG_FERR);
        if (chmod(ipath, 0700) != 0) exit_error ("Cannot set permissions for shmem key file [%s]", GG_FERR);
        exit (0);
        //
        // End of init
        //
    }

    runasuser(); // make sure mgrg application NEVER runs as root

    // check if this app initialized
    snprintf (ipath, sizeof(ipath), GG_APPDIR, golfapp);
    if (stat(ipath, &sbuff) != 0) exit_error ("Directory [%s] does not exist or cannot be accessed", ipath); 
    snprintf (ipath, sizeof(ipath), GG_RUNNAME "/mem", golfapp);
    if (stat(ipath, &sbuff) != 0) exit_error ("Directory [%s] does not exist or cannot be accessed", ipath); 

    // This is if there is -m <command>, run mgrg as client, and exit right away
    // this does not require root privilege
    if (client_msg[0] != 0) {
        cli_getshm (client_msg, 0);
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

    // use ./command for something in current directory or a full path
    if (command[0] == 0) {
        // if no command, assume standard Golf format - in golf's bld directory
        GG_ANN (command = (char*)malloc (GG_MAX_FILELEN));
        snprintf (command, GG_MAX_FILELEN, GG_BLDAPPDIR "/%s.srvc", golfapp, golfapp);
    } else {
        char *s = strchr (command, '/');
        if (s == NULL) {
            // if command without path, place the file in golf's bld directory
            char *nc;
            GG_ANN (nc = (char*)malloc (GG_MAX_FILELEN));
            snprintf (nc, GG_MAX_FILELEN, GG_BLDAPPDIR "/%s", golfapp, command);
            command = nc; // old command is lost, but no big deal; freeing would work but any change in logic might cause sigseg later
        }
    }


    tokarg(); // get -a args for srvc processes

    signal(SIGCHLD, SIG_IGN); // avoid defunct processes when children killed - we do not expect return value
    signal(SIGPIPE, SIG_IGN); // ignore broken pipe


// check if command exists and is executable
    if (access(command, X_OK) != 0) {
        exit_error ("Command [%s] does not exist, is not executable, or you have no permissions to execute it", command);
    }

    gg_num init_start;
    init_start = num_to_start_min; // min srvc processes to start

    // log file defined outside fork as it must be visible to parent AND child
    char logn[GG_MAX_FILELEN];
    snprintf (logn, sizeof(logn), GG_VFLOGDIR "/log", golfapp);
    
    gg_num deadres; // used for first round of forking, which isn't relevant
    // if foreground mode, do not fork
    if (fg == 0) deadres=fork(); else deadres = 0;
    if (deadres == 0) {
        // THIS IS THE RESIDENT SERVER PROCESS (child of original)

        // create new session so that terminating current logged-on session will not send terminating signals to here
        // not as important for Golf, as it handles all signals, in general a good idea
        if (fg == 0) { // create session only if running in background
            if ((sid = setsid()) < 0) {
                exit_error ("Cannot create new session group [%s]", GG_FERR);
            }
        }
    

        // up to this point, all output is stdout, stderr. Now it's log file, but just for current invocation (does not grow)
        if (fg == 0) { // redirect stdin,out,err only if background mode, otherwise print to console
            logfile = fopen (logn, "a+");
            if (logfile == NULL) exit_error ("Cannot open log file [%s]", GG_FERR);
            // close stdin,stdout,stderror, and redirect them to /var/log/golf
            close(0);
            close(1);
            close(2);
            if (dup (fileno(logfile)) != 0) { // logfileile to stdin? we will never read; must have it so children can use it to dup socket!
                exit_error ("Cannot dup logfile to 0 [%s]", GG_FERR);
            } 
            if (dup (fileno(logfile)) != 1) {
                exit_error ("Cannot dup logfile to 1 [%s]", GG_FERR);
            }
            if (dup (fileno(logfile)) != 2) {
                exit_error ("Cannot dup logfile to 2 [%s]", GG_FERR);
            }
        }
        islogging=1;

        // Locking here to make sure only one resident server process remains, since this lock
        // doesn't propogate to children
        if (srvhere(GG_LOCK) == 1) {
            // if server already running, communicate to front end that it is okay, then exit
            if (fg == 0) cli_getshm ("okay_running", 1); // nobody to communicate with unless forked first
            exit_error ("The Golf process manager for application [%s] is already running", golfapp);
        }

        log_msg ("Golf Service Manager v%s starting", GG_PKGVERSION);
        log_msg ("Command [%s], port [%ld], workers [%ld], user [%s], name [%s], sleep [%ld], minmaxset [%ld], min-workers [%ld], max-workers [%ld]", command, port, num_to_start_min, run_user, golfapp, mslp, minmaxset, num_to_start_min, maxproc);

         
        // Binding must be here, because it has to be after locking checked in srvhere. This is especially important
        // for unix sockets, where next bind overwrites the previous, and all processes that listened to the previous are just...hanging
        if (unix_sock == 1) sockfd = socket(AF_UNIX, SOCK_STREAM, 0);
        else sockfd = socket(AF_INET, SOCK_STREAM, 0);
        if (sockfd == -1) {
            exit_error("Cannot create socket [%s]", GG_FERR);
        } 

        if (unix_sock == 0) {
            // reuse address, port; get load balancing; only the original user can take advantage of reusing port
            if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEPORT, &(int){1}, sizeof(int)) < 0) {
                exit_error ("Cannot set socket option (1) [%s]", GG_FERR);
            }
        }

        gg_num rv;
        if (unix_sock == 1) {
            struct sockaddr_un servaddr;
            memset(&servaddr, 0, sizeof(struct sockaddr_un));
            servaddr.sun_family = AF_UNIX; 
            snprintf (servaddr.sun_path, sizeof (servaddr.sun_path), GG_SOCKNAME, golfapp);
            if (unlink(servaddr.sun_path) != 0) if (errno != ENOENT) exit_error ("Cannot unlink unix domain socket file [%s]", GG_FERR);
            if (bind(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr)) != 0) exit_error ("Cannot bind unix domain socket [%s], [%s]", servaddr.sun_path, GG_FERR);
            struct stat sbuff;
            snprintf (ipath, sizeof(ipath), GG_SOCKDIR, golfapp);
            if (stat(ipath, &sbuff) != 0) exit_error("Socket directory not found [%s], [%s]", ipath, GG_FERR);
            // check if directory perms are 0750, if so make it 0660 for the socket. If 0755, make it 0666
            if ((sbuff.st_mode & 05) == 05 ) {
                if (chmod(servaddr.sun_path, 0666) != 0) exit_error ("Cannot set permissions for unix domain socket [%s]", GG_FERR);
            } else {
                if (chmod(servaddr.sun_path, 0660) != 0) exit_error ("Cannot set permissions for unix domain socket [%s]", GG_FERR);
            }
        } else {
            struct sockaddr_in servaddr;
            bzero(&servaddr, sizeof(servaddr));
            servaddr.sin_family = AF_INET;
            servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
            servaddr.sin_port = htons(port);
            if (bind(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr)) != 0) exit_error("Cannot bind socket [%s]", GG_FERR);
        }
        
        if ((rv = listen(sockfd, maxblog)) != 0) {
            exit_error("Cannot listen on socket [%s]", GG_FERR);
        }
        if (fg == 0) cli_getshm ("okay_started", 1); // nobody to communicate with unless forked first
        log_msg ("Golf Service Manager v%s successfully started", GG_PKGVERSION);

    } else if (deadres == -1) {
        exit_error ("Could not start Golf Service Manager for [%s], [%s]", golfapp, GG_FERR);
    } else {
        // ORIGINAL PARENT FROM COMMAND LINE OR A SCRIPT, deadres is the PID of the child process (the resident process)
        // get success message from child (our resident process)
        // THIS NEVER HAPPENS IN FOREGROUND MODE, as there is no fork (parent and child), but only one process
        gg_num tries = GG_TRIES;
        while (tries-- >= 0) {
            // DO NOT check if child process has died, because if the server is already running, it will exit, i.e. die
            // and before it dies, it will send okay_started message

            // check for client commands
            checkshm(); 
            if (shm->command == GG_DONESTARTUP) {out_msg ("Golf Service Manager [%s] for [%s] successfully started", GG_PKGVERSION, golfapp); exit(0);}
            else if (shm->command == GG_DONESTARTUP0) {out_msg ("Golf Service Manager [%s] for [%s] is already running. If you want to restart it, stop it first (-m quit), then start it", GG_PKGVERSION, golfapp); exit(1);}
            sleepabit (mslp);
        }
        exit_error ("Could not start Golf Service Manager for [%s],[%ld],[1], log file [%s]", golfapp, tries, logn);
    }
    gg_num pcount = 0;
    num_process = maxproc;
    while (--init_start >= 0) {
        start_child (command, pcount++);
    }

    
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
        checkshm(); 
    }

    // never gets here
    return 0;
}


