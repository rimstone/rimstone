// SPDX-License-Identifier: Apache-2.0
// Copyright 2018-2025 Gliim LLC. 
// Licensed under Apache License v2. See LICENSE file.
// On the web http://rimstone-lang.com/ - this file is part of RimStone framework.

// 
// sqlite native interface, used by db.c for making of db-specific libs
//
// All errors must go through rim_lite_error() and the sole point of entry is rim_lite_exec()
// in order to avoid disorderly rollbacks in case of errors
//

#include "rim.h"
static int rim_lite_stmt_rows (char ***row, unsigned long **lens);
static int rim_lite_add_input(rim_num i, char *arg);
static int rim_lite_prep_stmt(char is_prep, void **prep, char *stmt, rim_num num_of_args);
static int rim_lite_get_data ();

static char *cerror = NULL;
static rim_num qnumrows = 0;
static rim_num qnumfields = 0;
static rim_num qrownow = 0;
static char **qrows = NULL;
static unsigned long *qlens = NULL;
static rim_num qcolid = 0;

rim_num rim_lite_checkc()
{
    return 1; // sqlite is a library, thus always "up" as long as the process is up
}

char *rim_lite_errm(char *errm, rim_num errmsize, char *s, char *sname, rim_num lnum, char *er, char is_prep)
{
    RIM_UNUSED(is_prep);
    snprintf(errm,errmsize,"Error during query [%s], additional [%s] file [%s], line [%ld] : [%s]:%s", s, cerror==NULL?"":cerror, sname, lnum, er ,sqlite3_errmsg(RIM_CURR_DB.dbc->sqlite.con));
    return errm;
}


char *rim_lite_error(char *s, char is_prep)
{
    RIM_UNUSED(s); // used only for tracing
    RIM_UNUSED(is_prep);
    static char errv[30];
    int errc;
    snprintf (errv, sizeof(errv), "%d", errc = sqlite3_errcode(RIM_CURR_DB.dbc->sqlite.con));
    return errv;
}


int rim_lite_rows(char ***row, unsigned long **lens)
{
    RIM_CURR_DB.need_copy = 0;
    if (rim_lite_stmt_rows (row, lens)) return 1;
    return 0;
}


//
// Get a row when tuples returned. *row[] is an array of strings which represent fields (columns) and *lens is the
// array of lengths of those fields.
//
static int rim_lite_stmt_rows (char ***row, unsigned long **lens)
{
    if (qrows == NULL || qlens == NULL) 
    {
        cerror = "Cannot get row field and length data";
        return 1;
    }

// advance row when asked
    *row = &(qrows[qnumfields * qrownow]);
    *lens = &(qlens[qnumfields * qrownow]) ;
    qrownow++; 
    return 0;
}

static int rim_lite_get_data ()
{
    char *sname = "";
    rim_num lnum = 0;
    // get which file and line number is this going on at
    rim_location (&sname, &lnum, 0);

    rim_num i;

    // set qrows, qlens to NULL as rim_lite_get_data() is called only once for a query
    // below is freeing of these, and if not NULL, it would free bad data if no rows selected and there's an error (like
    // for example if inserting data and duplicate violation)
    qrows = NULL;
    qlens = NULL;
    rim_num cdata = 0; // index of fields within a row
    qnumrows = 0;
    int r;
    char firstr = 0;
    rim_num initr = 1; // initial number of rows
    while (1) 
    {
        if ((r = sqlite3_step(RIM_CURR_DB.dbc->sqlite.stmt)) == SQLITE_ROW)
        {
            // number of fields in a row
            if (firstr == 0)
            {
                firstr = 1;
                qnumfields = sqlite3_column_count(RIM_CURR_DB.dbc->sqlite.stmt);
                // get results (columns, lengths)
                qrows = (char **)rim_calloc (qnumfields * initr, sizeof(char*));
                qlens = (unsigned long *)rim_calloc (qnumfields * initr, sizeof(unsigned long));
            }
            if (qnumrows >= initr) 
            {
                initr = qnumrows + 1;
                qrows = (char **)rim_realloc (rim_mem_get_id(qrows), qnumfields * initr * sizeof(char*));
                qlens = (unsigned long *)rim_realloc (rim_mem_get_id(qlens), qnumfields * initr * sizeof(unsigned long));
            }
            //
            // Get a single row
            //
            // present row data for caller, wheter it's truncated or not. The only way it's not truncated is if all columns are NULL
            for (i = 0; i < qnumfields; i++) 
            {
                // use EMPTY_STRING if null, so freeing works
                char *res = (char*)sqlite3_column_text(RIM_CURR_DB.dbc->sqlite.stmt, i);
                (qlens)[cdata] = sqlite3_column_bytes(RIM_CURR_DB.dbc->sqlite.stmt, i);
                if (res == NULL)
                {
                    if (sqlite3_errcode(RIM_CURR_DB.dbc->sqlite.con) == SQLITE_NOMEM)
                    {
                        cerror = "Out of memory when obtaining query result";
                        return 1;
                    } 
                    else
                    {
                        (qrows)[cdata] = RIM_EMPTY_STRING;
                    }
                }
                else
                {
                    (qrows)[cdata] = rim_malloc ((qlens)[cdata] + 1);
                    memcpy ((qrows)[cdata], res, (qlens)[cdata]);
                    (qrows)[cdata][(qlens)[cdata]] = 0;
                }
                cdata++;
            }
            qnumrows++;
        }
        else 
        {
            if (r == SQLITE_DONE)  break;
            else
            {
                rim_num i;
                for (i = 0; i < cdata; i ++) 
                {
                    rim_free_int ((qrows)[i]);
                }
                if (qrows != NULL) rim_free_int (qrows);
                if (qlens != NULL) rim_free_int (qlens);
                return 1;
            } 
        }
    }

    qcolid = 0; // starting index for fetching column names

    return 0;
}


rim_num rim_lite_nrows()
{
    return qnumrows;
}

void rim_lite_free(char is_prep)
{
    if (is_prep == 0) rim_lite_close_stmt (RIM_CURR_DB.dbc->sqlite.stmt);
    return;
}

char *rim_lite_fieldname()
{
    return (char*)sqlite3_column_name(RIM_CURR_DB.dbc->sqlite.stmt, qcolid++);

}

rim_num rim_lite_nfield()
{
    return qnumfields;
}

int rim_lite_use(char is_prep)
{
    RIM_UNUSED(is_prep);
    return 0;
}

int rim_lite_store(char is_prep)
{
    RIM_UNUSED(is_prep);
    return 0;
}


void rim_lite_close ()
{
    sqlite3_close(RIM_CURR_DB.dbc->sqlite.con);
}


rim_dbc *rim_lite_connect (rim_num abort_if_bad)
{
    // reset all prepared statements
    rim_db_prep (NULL);


    // allocate connection to database. Must be done here to account for the actual generated code, which
    // dictates the sizeof(rim_dbc)
    // This must be malloc and NOT rim_malloc since we want to reuse connection across connections.
    // Otherwise rim_malloc would be automatically freed when the request is done, and the next 
    // request would use an invalid pointer. Also must use free to free it, not rim_free.
    if ((RIM_CURR_DB.dbc = rim_malloc0 (sizeof (rim_dbc))) == NULL) RIM_FATAL ("Cannot allocate memory for database connection [%m]");

    char db_config_name[150];
    snprintf (db_config_name, sizeof(db_config_name), "%s/%s", rim_get_config()->app.dbconf_dir, RIM_CURR_DB.db_name);
    char *cinfo;
    if (rim_read_file (db_config_name, &cinfo, 0, 0, NULL) < 0)
    {
        char em[300];
        snprintf (em, sizeof(em), "Cannot read database configuration file [%s]", db_config_name);
        if (abort_if_bad == 1) rim_report_error ("%s", em);
        rim_end_connection (0); // without it, we would think connection exists
        return NULL; // just for compiler, never gets here
    }
    // clean up config file, must be just file name
    rim_num lpath = (rim_num)strlen (cinfo);
    char *ts = rim_trim_ptr (cinfo, &lpath);
    if (strstr (ts, "\n")) 
    {
        char em[300];
        snprintf (em, sizeof(em), "Database file [%s] cannot have new line", ts);
        if (abort_if_bad == 1) rim_report_error ("%s", em);
        rim_free_int(cinfo);
        return NULL;
    }
    // Replace tilde with home directory, only if it's the first character
    char *exp_ts = NULL;
    if (ts[0] == '~')
    {
        struct passwd *pw = getpwuid(getuid());
        char *home = pw->pw_dir;
        if (home != NULL) // don't do anything if home not found
        {
            rim_num hlen = strlen(home);
            rim_num ns = hlen+lpath+10; // +10 indeed just +1, for good measure only
            exp_ts = rim_malloc (ns);
            memcpy (exp_ts, home, hlen);
            exp_ts[hlen]='/';
            memcpy (exp_ts+hlen+1, ts+1, lpath-1); // skip ~ when copying (ts+1)
            exp_ts[hlen+lpath-1+1]=0; // +1 for /
        } else exp_ts = ts;
    } else exp_ts = ts;
    // make connection to database
    if (sqlite3_open(exp_ts, &(RIM_CURR_DB.dbc->sqlite.con)) != SQLITE_OK) 
    {
        char em[300];
        snprintf (em, sizeof(em), "Cannot cannot open database [%s]", ts);
        if (abort_if_bad == 1) rim_report_error ("%s", em);
        rim_end_connection (0); // without it, we would think connection exists
        rim_free_int(cinfo);
        return NULL;
    }
    if (exp_ts != ts) rim_free_int (exp_ts);
    rim_free_int(cinfo);

    return RIM_CURR_DB.dbc;
}

rim_num rim_lite_exec(char *s, char is_prep, void **prep, rim_num paramcount, char **params)
{   
    if (rim_lite_prep_stmt (is_prep, prep, s, paramcount)) return 1;
    rim_num i;
    for (i = 0; i < paramcount; i++)
    {
        if (rim_lite_add_input(i, params[i]) != 0) return 1;
    }
    if (rim_lite_get_data () != 0) {
        return 1;
    }
    qrownow = 0; // row number currently served
    // sqlite must be reset to be able to reuse statement
    sqlite3_reset(RIM_CURR_DB.dbc->sqlite.stmt);
    sqlite3_clear_bindings(RIM_CURR_DB.dbc->sqlite.stmt);
    return 0;
}


//
// deallocate prepared statement (only when connection lost), st is void * to prep stmt.
//
void rim_lite_close_stmt (void *st)
{
    if (st == NULL) return; // statement has not been prepared yet, so cannot deallocate
    if (RIM_CURR_DB.dbc != NULL) 
    {
        // we only close statements when connection is lost, so we don't care about return code
        // only that memory is deallocated
        sqlite3_finalize((sqlite3_stmt *)st);
    }
}

//
// Prepare statement with source test stmt and num_of_args input parameters
// prep is the address of void * that points to prepared statement. This is a static
// void *, which survives requests (otherwise prepared statements wouldn't be very useful,
// actually would decrease performance), however prep is set to NULL when connection is 
// reestablished (typically if db server recycles), which is generally rare.
// is_prep is 0 if not prepare, 1 if it is. We need this because prep is !=NULL even when prepared
// is not used, resulting in the same SQL executed over and over
//
static int rim_lite_prep_stmt(char is_prep, void **prep, char *stmt, rim_num num_of_args)
{
    char *sname = "";
    rim_num lnum = 0;
    rim_location (&sname, &lnum, 0);

    // reuse prepared statement from prep
    // some statements (begin transaction, exec-query <dynamic query> and such) must be re-prepared
    // because they are *always* purely dynamic statements
    if (is_prep ==1 && prep != NULL && *prep != NULL) 
    {
        rim_stmt_cached = 1;
        RIM_CURR_DB.dbc->sqlite.stmt = (sqlite3_stmt*)*prep;
    }
    else
    {
        // if prep is NULL, create prepared statement
        char *origs = stmt; // original stmt
        stmt = rim_db_prep_text(stmt); // make ? instead of '%s'
        // prep stmt
        if (sqlite3_prepare_v2(RIM_CURR_DB.dbc->sqlite.con, stmt, -1, &(RIM_CURR_DB.dbc->sqlite.stmt), 0) != SQLITE_OK) 
        {
            cerror = "Cannot prepare statement";
            return 1;
        }
        if (stmt != origs) rim_free_int (stmt); // release statement if actually allocated
        // save it for reuse for as long as the process lives (minus reconnects to db, but those are rare)
        if (prep != NULL) *prep = RIM_CURR_DB.dbc->sqlite.stmt;
    }
    RIM_CURR_DB.num_inp = num_of_args;
    // check param count correct before binding as the statement and what rimstone says may be different, in which case
    // mariadb would access memory that doesn't exist if rimstone say there's less than really is
    rim_num count = sqlite3_bind_parameter_count(RIM_CURR_DB.dbc->sqlite.stmt);
    if (count != RIM_CURR_DB.num_inp)
    {
        cerror = "Wrong number of input parameters";
        return 1;
    }

    return 0;
}


//
// Add input parameter to SQL about to be executed. 'i' is the index of parameter
// (which must be 0,1,2.. for each invocation), arg is the argument.
// return 0 on okay, 1 on error
//
static int rim_lite_add_input(rim_num i, char *arg)
{
    if (sqlite3_bind_text(RIM_CURR_DB.dbc->sqlite.stmt,i+1,arg,-1,SQLITE_STATIC) != SQLITE_OK) 
    {
        cerror = "Cannot bind input parameter";
        return 1;
    }
    return 0;
}


rim_num rim_lite_affected(char is_prep) 
{
    RIM_UNUSED(is_prep);
    return (rim_num) sqlite3_changes(RIM_CURR_DB.dbc->sqlite.con);
}


//
// Escape string for inclusion in quoted params. MariaDB may use different quotes, but we force ansi quotes.
// Even if we didn't, the string would be properly escaped.
// memory for to must be 2*len+1. *len  is the actual encoded length without zero byte counted.
// Returns 0 for okay, 1 otherwise.
//
int rim_lite_escape(char *from, char *to, rim_num *len)
{
    memcpy (to, from, *len + 1);
    if (rim_replace_string (to, 2* *len+1, "\\", "\\\\", 1, NULL, 1) == -1) return 1;
    if ((*len = rim_replace_string (to, 2* *len+1, "'", "''", 1, NULL, 1)) == -1) return 1;
    return 0;
}


