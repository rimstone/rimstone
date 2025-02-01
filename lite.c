// SPDX-License-Identifier: Apache-2.0
// Copyright 2018 Gliim LLC. 
// Licensed under Apache License v2. See LICENSE file.
// On the web http://golf-lang.com/ - this file is part of Golf framework.

// 
// sqlite native interface, used by db.c for making of db-specific libs
//
// All errors must go through gg_lite_error() and the sole point of entry is gg_lite_exec()
// in order to avoid disorderly rollbacks in case of errors
//

#include "golf.h"
static int gg_lite_stmt_rows (char ***row, unsigned long **lens);
static int gg_lite_add_input(gg_num i, char *arg);
static int gg_lite_prep_stmt(char is_prep, void **prep, char *stmt, gg_num num_of_args);

static char *cerror = NULL;
static gg_num qnumrows = 0;
static gg_num qnumfields = 0;
static gg_num qrownow = 0;
static char **qrows = NULL;
static unsigned long *qlens = NULL;
static gg_num qcolid = 0;
static int gg_lite_get_data ();

gg_num gg_lite_checkc()
{
    GG_TRACE("");
    return 1; // sqlite is a library, thus always "up" as long as the process is up
}

char *gg_lite_errm(char *errm, gg_num errmsize, char *s, char *sname, gg_num lnum, char *er, char is_prep)
{
    GG_TRACE("");
    GG_UNUSED(is_prep);
    snprintf(errm,errmsize,"Error during query [%s], additional [%s] file [%s], line [%ld] : [%s]%s", s, cerror==NULL?"":cerror, sname, lnum, er ,atol(er) == ER_PARSE_ERROR ?  "Problem with parsing SQL statement" : sqlite3_errmsg(GG_CURR_DB.dbc->sqlite.con));
    return errm;
}


char *gg_lite_error(char *s, char is_prep)
{
    GG_TRACE("");
    GG_UNUSED(s); // used only for tracing
    GG_UNUSED(is_prep);
    static char errv[30];
    int errc;
    snprintf (errv, sizeof(errv), "%d", errc = sqlite3_errcode(GG_CURR_DB.dbc->sqlite.con));
    GG_TRACE ("Error in %s: %s error code %s", s, sqlite3_errstr(errc), errv);
    return errv;
}


int gg_lite_rows(char ***row, unsigned long **lens)
{
    GG_TRACE("");
    GG_CURR_DB.need_copy = 0;
    if (gg_lite_stmt_rows (row, lens)) return 1;
    return 0;
}


//
// Get a row when tuples returned. *row[] is an array of strings which represent fields (columns) and *lens is the
// array of lengths of those fields.
//
int gg_lite_stmt_rows (char ***row, unsigned long **lens)
{
    GG_TRACE("");
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

int gg_lite_get_data ()
{
    GG_TRACE("");
    char *sname = "";
    gg_num lnum = 0;
    // get which file and line number is this going on at
    gg_location (&sname, &lnum, 0);

    gg_num i;

    // set qrows, qlens to NULL as gg_lite_get_data() is called only once for a query
    // below is freeing of these, and if not NULL, it would free bad data if no rows selected and there's an error (like
    // for example if inserting data and duplicate violation)
    qrows = NULL;
    qlens = NULL;
    gg_num cdata = 0; // index of fields within a row
    qnumrows = 0;
    int r;
    char firstr = 0;
    gg_num initr = 1; // initial number of rows
    while (1) 
    {
        if ((r = sqlite3_step(GG_CURR_DB.dbc->sqlite.stmt)) == SQLITE_ROW)
        {
            // number of fields in a row
            if (firstr == 0)
            {
                firstr = 1;
                qnumfields = sqlite3_column_count(GG_CURR_DB.dbc->sqlite.stmt);
                // get results (columns, lengths)
                qrows = (char **)gg_calloc (qnumfields * initr, sizeof(char*));
                qlens = (unsigned long *)gg_calloc (qnumfields * initr, sizeof(unsigned long));
            }
            if (qnumrows >= initr) 
            {
                initr = qnumrows + 1;
                qrows = (char **)gg_realloc (gg_mem_get_id(qrows), qnumfields * initr * sizeof(char*));
                qlens = (unsigned long *)gg_realloc (gg_mem_get_id(qlens), qnumfields * initr * sizeof(unsigned long));
            }
            //
            // Get a single row
            //
            // present row data for caller, wheter it's truncated or not. The only way it's not truncated is if all columns are NULL
            for (i = 0; i < qnumfields; i++) 
            {
                // use EMPTY_STRING if null, so freeing works
                char *res = (char*)sqlite3_column_text(GG_CURR_DB.dbc->sqlite.stmt, i);
                (qlens)[cdata] = sqlite3_column_bytes(GG_CURR_DB.dbc->sqlite.stmt, i);
                if (res == NULL)
                {
                    if (sqlite3_errcode(GG_CURR_DB.dbc->sqlite.con) == SQLITE_NOMEM)
                    {
                        cerror = "Out of memory when obtaining query result";
                        return 1;
                    } 
                    else
                    {
                        (qrows)[cdata] = GG_EMPTY_STRING;
                    }
                }
                else
                {
                    (qrows)[cdata] = gg_malloc ((qlens)[cdata] + 1);
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
                gg_num i;
                for (i = 0; i < cdata; i ++) 
                {
                    gg_free ((qrows)[i]);
                }
                if (qrows != NULL) gg_free (qrows);
                if (qlens != NULL) gg_free (qlens);
                return 1;
            } 
        }
    }

    qcolid = 0; // starting index for fetching column names

    return 0;
}


gg_num gg_lite_nrows()
{
    GG_TRACE("");
    return qnumrows;
}

void gg_lite_free(char is_prep)
{
    GG_TRACE("");
    if (is_prep == 0) gg_lite_close_stmt (GG_CURR_DB.dbc->sqlite.stmt);
    return;
}

char *gg_lite_fieldname()
{
    GG_TRACE("");
    return (char*)sqlite3_column_name(GG_CURR_DB.dbc->sqlite.stmt, qcolid++);

}

gg_num gg_lite_nfield()
{
    GG_TRACE("");
    return qnumfields;
}

int gg_lite_use(char is_prep)
{
    GG_TRACE("");
    GG_UNUSED(is_prep);
    return 0;
}

int gg_lite_store(char is_prep)
{
    GG_TRACE("");
    GG_UNUSED(is_prep);
    return 0;
}


void gg_lite_close ()
{
    GG_TRACE("");
    sqlite3_close(GG_CURR_DB.dbc->sqlite.con);
}


gg_dbc *gg_lite_connect (gg_num abort_if_bad)
{
    GG_TRACE("");
    // reset all prepared statements
    gg_db_prep (NULL);


    // allocate connection to database. Must be done here to account for the actual generated code, which
    // dictates the sizeof(gg_dbc)
    // This must be malloc and NOT gg_malloc since we want to reuse connection across connections.
    // Otherwise gg_malloc would be automatically freed when the request is done, and the next 
    // request would use an invalid pointer. Also must use free to free it, not gg_free.
    if ((GG_CURR_DB.dbc = malloc (sizeof (gg_dbc))) == NULL) GG_FATAL ("Cannot allocate memory for database connection [%m]");

    char db_config_name[150];
    snprintf (db_config_name, sizeof(db_config_name), "%s/%s", gg_get_config()->app.dbconf_dir, GG_CURR_DB.db_name);
    GG_TRACE ("Using db config file [%s]", db_config_name);
    char *cinfo;
    if (gg_read_file (db_config_name, &cinfo, 0, 0) < 0)
    {
        char em[300];
        snprintf (em, sizeof(em), "Cannot read database configuration file [%s]", db_config_name);
        GG_TRACE ("%s", em);
        if (abort_if_bad == 1) gg_report_error ("%s", em);
        gg_end_connection (0); // without it, we would think connection exists
        return NULL; // just for compiler, never gets here
    }
    // clean up config file, must be just file name
    gg_num l = (gg_num)strlen (cinfo);
    char *ts = gg_trim_ptr (cinfo, &l);
    if (strstr (ts, "\n")) 
    {
        char em[300];
        snprintf (em, sizeof(em), "Database file [%s] cannot have new line", ts);
        GG_TRACE ("%s", em);
        if (abort_if_bad == 1) gg_report_error ("%s", em);
        gg_free(cinfo);
        return NULL;
    }
    // make connection to database
    if (sqlite3_open(ts, &(GG_CURR_DB.dbc->sqlite.con)) != SQLITE_OK) 
    {
        char em[300];
        snprintf (em, sizeof(em), "Cannot cannot open database [%s]", ts);
        GG_TRACE ("%s", em);
        if (abort_if_bad == 1) gg_report_error ("%s", em);
        gg_end_connection (0); // without it, we would think connection exists
        gg_free(cinfo);
        return NULL;
    }
    gg_free(cinfo);

    return GG_CURR_DB.dbc;
}

gg_num gg_lite_exec(char *s, char is_prep, void **prep, gg_num paramcount, char **params)
{   
    GG_TRACE("");
    if (gg_lite_prep_stmt (is_prep, prep, s, paramcount)) return 1;
    gg_num i;
    for (i = 0; i < paramcount; i++)
    {
        if (gg_lite_add_input(i, params[i]) != 0) return 1;
    }
    if (gg_lite_get_data () != 0) {
        return 1;
    }
    qrownow = 0; // row number currently served
    // sqlite must be reset to be able to reuse statement
    sqlite3_reset(GG_CURR_DB.dbc->sqlite.stmt);
    sqlite3_clear_bindings(GG_CURR_DB.dbc->sqlite.stmt);
    return 0;
}


//
// deallocate prepared statement (only when connection lost), st is void * to prep stmt.
//
void gg_lite_close_stmt (void *st)
{
    GG_TRACE("");
    if (st == NULL) return; // statement has not been prepared yet, so cannot deallocate
    if (GG_CURR_DB.dbc != NULL) 
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
int gg_lite_prep_stmt(char is_prep, void **prep, char *stmt, gg_num num_of_args)
{
    GG_TRACE("");
    char *sname = "";
    gg_num lnum = 0;
    gg_location (&sname, &lnum, 0);

    // reuse prepared statement from prep
    // some statements (begin transaction, exec-query <dynamic query> and such) must be re-prepared
    // because they are *always* purely dynamic statements
    if (is_prep ==1 && prep != NULL && *prep != NULL) 
    {
        gg_stmt_cached = 1;
        GG_TRACE ("reusing prepared statement");
        GG_CURR_DB.dbc->sqlite.stmt = (sqlite3_stmt*)*prep;
    }
    else
    {
        GG_TRACE ("creating prepared statement");
        // if prep is NULL, create prepared statement
        char *origs = stmt; // original stmt
        stmt = gg_db_prep_text(stmt); // make ? instead of '%s'
        // prep stmt
        if (sqlite3_prepare_v2(GG_CURR_DB.dbc->sqlite.con, stmt, -1, &(GG_CURR_DB.dbc->sqlite.stmt), 0) != SQLITE_OK) 
        {
            cerror = "Cannot prepare statement";
            return 1;
        }
        if (stmt != origs) gg_free (stmt); // release statement if actually allocated
        // save it for reuse for as long as the process lives (minus reconnects to db, but those are rare)
        if (prep != NULL) *prep = GG_CURR_DB.dbc->sqlite.stmt;
    }
    GG_CURR_DB.num_inp = num_of_args;
    // check param count correct before binding as the statement and what golf says may be different, in which case
    // mariadb would access memory that doesn't exist if golf say there's less than really is
    gg_num count = sqlite3_bind_parameter_count(GG_CURR_DB.dbc->sqlite.stmt);
    if (count != GG_CURR_DB.num_inp)
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
int gg_lite_add_input(gg_num i, char *arg)
{
    GG_TRACE("");
    if (sqlite3_bind_text(GG_CURR_DB.dbc->sqlite.stmt,i+1,arg,-1,SQLITE_STATIC) != SQLITE_OK) 
    {
        cerror = "Cannot bind input parameter";
        return 1;
    }
    return 0;
}


gg_num gg_lite_affected(char is_prep) 
{
    GG_TRACE("");
    GG_UNUSED(is_prep);
    return (gg_num) sqlite3_changes(GG_CURR_DB.dbc->sqlite.con);
}


//
// Escape string for inclusion in quoted params. MariaDB may use different quotes, but we force ansi quotes.
// Even if we didn't, the string would be properly escaped.
// memory for to must be 2*len+1. *len  is the actual encoded length without zero byte counted.
// Returns 0 for okay, 1 otherwise.
//
int gg_lite_escape(char *from, char *to, gg_num *len)
{
    GG_TRACE("");
    memcpy (to, from, *len + 1);
    if (gg_replace_string (to, 2* *len+1, "\\", "\\\\", 1, NULL, 1) == -1) return 1;
    if ((*len = gg_replace_string (to, 2* *len+1, "'", "''", 1, NULL, 1)) == -1) return 1;
    return 0;
}


