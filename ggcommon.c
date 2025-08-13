#include "ggcommon.h"

//
// Get requested type of directory into buffer dir, which is of size dir_size, types that can be requested are in ggcommon.h
// app can be NULL, it's the application name which if it's NULL then it's just user dir without app subdir; 
// user is the user name which if it's NULL then it's the current user
// User directory is $HOME/.golf in general
// Returns length written, or -1 if there's an issue
//
#define GG_GOLF ".golf"

GG_ALWAYS_INLINE inline size_t gg_dir (int type, char *dir, size_t dir_size, char *app, char *user)
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
    if (dir_len + 1 + sizeof(GG_GOLF) + 1 + app_len + 1 + 40 > dir_size) return -1;
    // copy home dir
    char *cdir = dir;
    memcpy (cdir, dir, dir_len);
    cdir += dir_len;
    // copy / after it
    *cdir = '/';
    cdir++;
    // copy .golf
    memcpy (cdir, GG_GOLF, sizeof(GG_GOLF)-1);
    cdir += sizeof(GG_GOLF)-1;
    if (type != GG_DIR_USER) // we already have user dir
    {
        // add apps
#define _GG_APPS "/apps"
        memcpy (cdir, _GG_APPS, sizeof(_GG_APPS)-1);
        cdir += sizeof(_GG_APPS)-1;
        if (type != GG_DIR_APPS)  // this is /apps
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
            if (type != GG_DIR_APPNAME) // this is /apps/<app>
            {
                if (type == GG_DIR_APP) // this is /apps/<app>/app
                {
#define _GG_APP "/app"
                    memcpy (cdir, _GG_APP, sizeof(_GG_APP)-1);
                    cdir += sizeof(_GG_APP)-1;
                }
                else if (type == GG_DIR_TRACENAME) // this is trace name template for a process
                {
#define _GG_TRACENAME "/trace/bt."
                    memcpy (cdir, _GG_TRACENAME, sizeof(_GG_TRACENAME)-1);
                    cdir += sizeof(_GG_TRACENAME)-1;
                    cdir += snprintf (cdir, dir_size-1-(cdir-dir)-25, "%ld", (long)getpid()); // assuming number length can't be bigger than 25 chars
                } 
                else if (type == GG_DIR_TRACE) // this is trace directory /apps/<app>/trace
                {
#define _GG_TRACE "/trace"
                    memcpy (cdir, _GG_TRACE, sizeof(_GG_TRACE)-1);
                    cdir += sizeof(_GG_TRACE)-1;
                } 
                else if (type == GG_DIR_DB) // this is db directory /apps/<app>/db
                {
#define _GG_DB "/db"
                    memcpy (cdir, _GG_DB, sizeof(_GG_DB)-1);
                    cdir += sizeof(_GG_DB)-1;
                } 
                else if (type == GG_DIR_TMP) // this is file storage directory /apps/<app>/file/t (temporary)
                {
#define _GG_TMP "/file/t"
                    memcpy (cdir, _GG_TMP, sizeof(_GG_TMP)-1);
                    cdir += sizeof(_GG_TMP)-1;
                } 
                else if (type == GG_DIR_FILE) // this is file storage directory /apps/<app>/file
                {
#define _GG_FILE "/file"
                    memcpy (cdir, _GG_FILE, sizeof(_GG_FILE)-1);
                    cdir += sizeof(_GG_FILE)-1;
                } 
                else if (type == GG_DIR_SOCK) // this is socket dir /apps/<app>/sock (needed so we can set sticky bit for a socket inside,
                                              // so it can't be renamed or deleted by anyone but owner)
                {
#define _GG_SOCK "/sock"
                    memcpy (cdir, _GG_SOCK, sizeof(_GG_SOCK)-1);
                    cdir += sizeof(_GG_SOCK)-1;
                } 
                else if (type == GG_DIR_SOCKFILE) // this is socket file /apps/<app>/.sock
                {
#define _GG_SOCKFILE "/sock/.sock"
                    memcpy (cdir, _GG_SOCKFILE, sizeof(_GG_SOCKFILE)-1);
                    cdir += sizeof(_GG_SOCKFILE)-1;
                } 
                else if (type == GG_DIR_LOCK) // this is server lock file /apps/<app>/.lock
                {
#define _GG_LOCK "/.lock"
                    memcpy (cdir, _GG_LOCK, sizeof(_GG_LOCK)-1);
                    cdir += sizeof(_GG_LOCK)-1;
                } 
                else if (type == GG_DIR_MEM) // this is socket memory /apps/<app>/.mem
                {
#define _GG_MEM "/.mem"
                    memcpy (cdir, _GG_MEM, sizeof(_GG_MEM)-1);
                    cdir += sizeof(_GG_MEM)-1;
                } 
                else if (type == GG_DIR_BLD) // build directory /apps/<app>/.bld
                {
#define _GG_BLD "/.bld"
                    memcpy (cdir, _GG_BLD, sizeof(_GG_BLD)-1);
                    cdir += sizeof(_GG_BLD)-1;
                } 
                else if (type == GG_DIR_MGRG1) // this is mgrg log file /apps/<app>/.mgrglog
                {
#define _GG_MGRG1 "/.mgrglog1"
                    memcpy (cdir, _GG_MGRG1, sizeof(_GG_MGRG1)-1);
                    cdir += sizeof(_GG_MGRG1)-1;
                } 
                else if (type == GG_DIR_MGRG) // this is mgrg log file /apps/<app>/.mgrglog
                {
#define _GG_MGRG "/.mgrglog"
                    memcpy (cdir, _GG_MGRG, sizeof(_GG_MGRG)-1);
                    cdir += sizeof(_GG_MGRG)-1;
                } 
            }
        }
    }
    // cap string
    *cdir = 0;
    return cdir-dir;
}


