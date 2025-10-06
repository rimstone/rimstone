#include "rimcommon.h"

//
// Get requested type of directory into buffer dir, which is of size dir_size, types that can be requested are in rimcommon.h
// app can be NULL, it's the application name which if it's NULL then it's just user dir without app subdir; 
// user is the user name which if it's NULL then it's the current user
// User directory is $HOME/.rimstone in general
// Returns length written, or -1 if there's an issue
//
#define RIM_RIM ".rimstone"

RIM_ALWAYS_INLINE inline size_t rim_dir (int type, char *dir, size_t dir_size, char *app, char *user)
{
    struct passwd* pwd;
    // We use REAL UID, and not effective uid. That's because we will ignore any sudo or 'sudo -u'. All the directories MUST be
    // tied to REAL UID, and thus not effective one. It would be a huge mess otherwise.
    if (user == NULL) { if ((pwd = getpwuid (getuid())) == NULL) return -1; }
    else { if ((pwd = getpwnam (user)) == NULL) return -1; }

    strncpy (dir, pwd->pw_dir, dir_size-1);
    (dir)[dir_size-1] = 0;

    // get lengths
    size_t dir_len = strlen(dir);
    size_t app_len = app == NULL ? 0: strlen(app);
    // calculate if it'll hold
    // +1 for / in between dir and app, +1 for null + 40 (more than enough for a few more / and path like app/trace, apps/file, the longer is
    // apps/app/sock/.sock which is 18 or apps/trace/bt.xxxxxxxxx...xx which is 35 max)
    if (dir_len + 1 + sizeof(RIM_RIM) + 1 + app_len + 1 + 40 > dir_size) return -1;
    // copy home dir
    char *cdir = dir;
    memcpy (cdir, dir, dir_len);
    cdir += dir_len;
    // copy / after it
    *cdir = '/';
    cdir++;
    // copy .rimstone
    memcpy (cdir, RIM_RIM, sizeof(RIM_RIM)-1);
    cdir += sizeof(RIM_RIM)-1;
    if (type != RIM_DIR_USER) // we already have user dir
    {
        // add apps
#define _RIM_APPS "/apps"
        memcpy (cdir, _RIM_APPS, sizeof(_RIM_APPS)-1);
        cdir += sizeof(_RIM_APPS)-1;
        if (type != RIM_DIR_APPS)  // this is /apps
        {
            // add app name
            if (app != NULL)
            {
                // copy / after it
                *cdir = '/';
                cdir++;
                memcpy (cdir, app, app_len);
                cdir += app_len;
            } else return -1;
            if (type != RIM_DIR_APPNAME) // this is /apps/<app>
            {
                if (type == RIM_DIR_APP) // this is /apps/<app>/app
                {
#define _RIM_APP "/app"
                    memcpy (cdir, _RIM_APP, sizeof(_RIM_APP)-1);
                    cdir += sizeof(_RIM_APP)-1;
                }
                else if (type == RIM_DIR_TRACENAME) // this is trace name template for a process
                {
#define _RIM_TRACENAME "/trace/bt."
                    memcpy (cdir, _RIM_TRACENAME, sizeof(_RIM_TRACENAME)-1);
                    cdir += sizeof(_RIM_TRACENAME)-1;
                    cdir += snprintf (cdir, dir_size-1-(cdir-dir)-25, "%ld", (long)getpid()); // assuming number length can't be bigger than 25 chars
                } 
                else if (type == RIM_DIR_TRACE) // this is trace directory /apps/<app>/trace
                {
#define _RIM_TRACE "/trace"
                    memcpy (cdir, _RIM_TRACE, sizeof(_RIM_TRACE)-1);
                    cdir += sizeof(_RIM_TRACE)-1;
                } 
                else if (type == RIM_DIR_DB) // this is db directory /apps/<app>/db
                {
#define _RIM_DB "/db"
                    memcpy (cdir, _RIM_DB, sizeof(_RIM_DB)-1);
                    cdir += sizeof(_RIM_DB)-1;
                } 
                else if (type == RIM_DIR_TMP) // this is file storage directory /apps/<app>/file/t (temporary)
                {
#define _RIM_TMP "/file/t"
                    memcpy (cdir, _RIM_TMP, sizeof(_RIM_TMP)-1);
                    cdir += sizeof(_RIM_TMP)-1;
                } 
                else if (type == RIM_DIR_FILE) // this is file storage directory /apps/<app>/file
                {
#define _RIM_FILE "/file"
                    memcpy (cdir, _RIM_FILE, sizeof(_RIM_FILE)-1);
                    cdir += sizeof(_RIM_FILE)-1;
                } 
                else if (type == RIM_DIR_SOCK) // this is socket dir /apps/<app>/sock (needed so we can set sticky bit for a socket inside,
                                              // so it can't be renamed or deleted by anyone but owner)
                {
#define _RIM_SOCK "/sock"
                    memcpy (cdir, _RIM_SOCK, sizeof(_RIM_SOCK)-1);
                    cdir += sizeof(_RIM_SOCK)-1;
                } 
                else if (type == RIM_DIR_SOCKFILE) // this is socket file /apps/<app>/.sock
                {
#define _RIM_SOCKFILE "/sock/.sock"
                    memcpy (cdir, _RIM_SOCKFILE, sizeof(_RIM_SOCKFILE)-1);
                    cdir += sizeof(_RIM_SOCKFILE)-1;
                } 
                else if (type == RIM_DIR_LOCK) // this is server lock file /apps/<app>/.lock
                {
#define _RIM_LOCK "/.lock"
                    memcpy (cdir, _RIM_LOCK, sizeof(_RIM_LOCK)-1);
                    cdir += sizeof(_RIM_LOCK)-1;
                } 
                else if (type == RIM_DIR_MEM) // this is socket memory /apps/<app>/.mem
                {
#define _RIM_MEM "/.mem"
                    memcpy (cdir, _RIM_MEM, sizeof(_RIM_MEM)-1);
                    cdir += sizeof(_RIM_MEM)-1;
                } 
                else if (type == RIM_DIR_BLD) // build directory /apps/<app>/.bld
                {
#define _RIM_BLD "/.bld"
                    memcpy (cdir, _RIM_BLD, sizeof(_RIM_BLD)-1);
                    cdir += sizeof(_RIM_BLD)-1;
                } 
                else if (type == RIM_DIR_ART) // this is .mrimart log file /apps/<app>/.mrimart
                {
#define _RIM_ART "/.mrimart"
                    memcpy (cdir, _RIM_ART, sizeof(_RIM_ART)-1);
                    cdir += sizeof(_RIM_ART)-1;
                } 
                else if (type == RIM_DIR_MGRG) // this is mrim log file /apps/<app>/.mrimlog
                {
#define _RIM_MGRG "/.mrimlog"
                    memcpy (cdir, _RIM_MGRG, sizeof(_RIM_MGRG)-1);
                    cdir += sizeof(_RIM_MGRG)-1;
                } 
            }
        }
    }
    // cap string
    *cdir = 0;
    return cdir-dir;
}


