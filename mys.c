// SPDX-License-Identifier: Apache-2.0
// Copyright 2018-2025 Gliim LLC. 
// Licensed under Apache License v2. See LICENSE file.
// On the web http://golf-lang.com/ - this file is part of Golf framework.

// 
// mariadb native interface, used by db.c for making of db-specific libs
//
// All errors must go through gg_maria_error() and the sole point of entry is gg_maria_exec()
// in order to avoid disorderly rollbacks in case of errors
//

#include "golf.h"
static gg_num gg_maria_stmt_exec();
static int gg_maria_stmt_rows (char ***row, unsigned long **lens);
static gg_num gg_maria_stmt_exec();
static void gg_maria_add_input(gg_num i, char *arg);
static void gg_maria_bind_input ();
static int gg_maria_prep_stmt(void **prep, char *stmt, gg_num num_of_args);

static char *cerror = NULL;


gg_num gg_maria_checkc()
{
    GG_TRACE("");
    return (mysql_ping (GG_CURR_DB.dbc->maria.con) == 0 ? 1 : 0);
}

char *gg_maria_errm(char *errm, gg_num errmsize, char *s, char *sname, gg_num lnum, char *er, char is_prep)
{
    GG_TRACE("");
    if (is_prep == 0)
    {
        snprintf(errm,errmsize,"Error during query [%s], additional [%s] file [%s], line [%ld] : [%s]%s", s, cerror==NULL?"":cerror, sname, lnum, er ,atol(er) == ER_PARSE_ERROR ?  "Problem with parsing SQL statement" : mysql_error(GG_CURR_DB.dbc->maria.con));
        return errm;
    }
    else
    {
        snprintf(errm,errmsize,"Error during query [%s], additional [%s] file [%s], line [%ld] : [%s]%s", s, cerror==NULL?"":cerror, sname, lnum, er ,atol(er) == ER_PARSE_ERROR ?  "Problem with parsing SQL statement" : mysql_stmt_error(GG_CURR_DB.dbc->maria.stmt));
        return errm;
    }
}


char *gg_maria_error(char *s, char is_prep)
{
    GG_TRACE("");
    GG_UNUSED(s); // used in tracing only
    if (is_prep == 0)
    {
        static char errv[30];
        snprintf (errv, sizeof(errv), "%d", mysql_errno(GG_CURR_DB.dbc->maria.con));
        GG_TRACE ("Error in %s: %s error %s state %s", s, mysql_error(GG_CURR_DB.dbc->maria.con), errv, mysql_sqlstate(GG_CURR_DB.dbc->maria.con));
        return errv;
    }
    else
    {
        static char errv[30];
        snprintf (errv, sizeof(errv), "%d", mysql_stmt_errno(GG_CURR_DB.dbc->maria.stmt));
        GG_TRACE ("Error in %s: %s error %s state %s", s, mysql_stmt_error(GG_CURR_DB.dbc->maria.stmt), errv, mysql_stmt_sqlstate(GG_CURR_DB.dbc->maria.stmt));
        return errv;
    }
}


int gg_maria_rows (char ***row, unsigned long **lens, char is_prep)
{
    GG_TRACE("");
    if (is_prep == 0)
    {
        GG_CURR_DB.need_copy = 1;
        // fetch a row, field by field
        MYSQL_ROW row1; // this is char **, if that changes than code here must be refactored, or made to use
                      // that new structure
        row1 = mysql_fetch_row(GG_CURR_DB.dbc->maria.res);
        *row = row1; // simple now, may not be in the future. In any case, the returning row variable must
                    // be an array of strings
        *lens = mysql_fetch_lengths (GG_CURR_DB.dbc->maria.res);
    }
    else
    {
        GG_CURR_DB.need_copy = 0;
        if (gg_maria_stmt_rows (row, lens)) return 1;
    }
    return 0;
}


//
// Get a row when tuples returned. *row[] is an array of strings which represent fields (columns) and *lens is the
// array of lengths of those fields.
//
int gg_maria_stmt_rows (char ***row, unsigned long **lens)
{
    GG_TRACE("");
    char *sname = "";
    gg_num lnum = 0;
    // get which file and line number is this going on at
    gg_location (&sname, &lnum, 0);

    // number of fields in a row
    gg_num nf = mysql_num_fields(GG_CURR_DB.dbc->maria.res);
    gg_num i;

    // get result binding array
    GG_CURR_DB.dbc->maria.bindout = (MYSQL_BIND*)gg_calloc(nf, sizeof(MYSQL_BIND));

    // get results (columns, lengths)
    *row = (char **)gg_calloc (nf, sizeof(char*));
    *lens = (unsigned long *)gg_calloc (nf, sizeof(unsigned long));

    // rlen is the array of actual lengths of an array of fields in a row, obtained from mariadb fetching
    unsigned long *rlen = (unsigned long*)gg_calloc (nf, sizeof(unsigned long));
    // array of whether fields are NULL
    my_bool *isnull = (char*)gg_calloc (nf, sizeof(char));
    // dummy storage for first pass of fetching
    char dummy[2]; // just in case give it storage
    // set all cols to strings, unknown length

    // set binding for results, we want all as strings. Length is 0 so we can obtain actual lengths in second pass.
    for (i = 0; i < nf; i++) 
    {
        GG_CURR_DB.dbc->maria.bindout[i].buffer_type = MYSQL_TYPE_STRING;
        GG_CURR_DB.dbc->maria.bindout[i].buffer = &(dummy[0]);
        GG_CURR_DB.dbc->maria.bindout[i].buffer_length = 0;
        GG_CURR_DB.dbc->maria.bindout[i].is_null = &(isnull[i]);
        GG_CURR_DB.dbc->maria.bindout[i].length = &(rlen[i]);
    }

    // bind results
    if (mysql_stmt_bind_result(GG_CURR_DB.dbc->maria.stmt, GG_CURR_DB.dbc->maria.bindout) != 0)
    {
        cerror = "Cannot initialize statement";
        return 1;
    }
    gg_num cdata = 0; // index of fields within a row

    //
    // Get a single row
    //
    // set empty for each row, because we don't know the size of its columns and each must be separately allocated
    for (i = 0; i < nf; i++) 
    {
        GG_CURR_DB.dbc->maria.bindout[i].buffer = &(dummy[0]);
        GG_CURR_DB.dbc->maria.bindout[i].buffer_length = 0;
    }
    // fetch a row
    int res = mysql_stmt_fetch(GG_CURR_DB.dbc->maria.stmt);
    // row is either okay or truncated, if not, error
    if (res != 0 && res != MYSQL_DATA_TRUNCATED) 
    {
        cerror = "Cannot fetch row";
        return 1;
    }
    if (res == MYSQL_DATA_TRUNCATED) 
    {
        // if truncated, now we can get actual lengths and allocate them
        for (i = 0; i < nf; i++) 
        {
            GG_CURR_DB.dbc->maria.bindout[i].buffer_length = rlen[i]+1;
            GG_CURR_DB.dbc->maria.bindout[i].buffer = gg_malloc (rlen[i] + 1 + 1); // just in case extra byte of storage
        }
        // now fetch columns based on actual lengths for each field
        for (i = 0; i < nf; i++) 
        {
            if (mysql_stmt_fetch_column(GG_CURR_DB.dbc->maria.stmt, &(GG_CURR_DB.dbc->maria.bindout[i]), i, 0) != 0)
            {
                cerror = "Cannot fetch data for column";
                return 1;
            }
        }
    }
    // present row data for caller, wheter it's truncated or not. The only way it's not truncated is if all columns are NULL
    for (i = 0; i < nf; i++) 
    {
        // use EMPTY_STRING if null, so freeing works
        (*row)[cdata] = GG_CURR_DB.dbc->maria.bindout[i].buffer == NULL ? GG_EMPTY_STRING : GG_CURR_DB.dbc->maria.bindout[i].buffer;
        (*lens)[cdata] = *(GG_CURR_DB.dbc->maria.bindout[i].length);
        cdata++;
    }

    // free allocations
    gg_free_int (GG_CURR_DB.dbc->maria.bindout);
    gg_free_int (rlen);
    gg_free_int (isnull);
    return 0;
}


gg_num gg_maria_nrows(char is_prep)
{
    GG_TRACE("");
    if (is_prep == 0)
    {
        // works since we use buffered (store) method to get results
        return mysql_num_rows (GG_CURR_DB.dbc->maria.res);
    }
    else
    {
        return mysql_stmt_num_rows (GG_CURR_DB.dbc->maria.stmt);
    }
}

void gg_maria_free()
{
    GG_TRACE("");
    if (GG_CURR_DB.dbc->maria.res != NULL) mysql_free_result(GG_CURR_DB.dbc->maria.res);
    GG_CURR_DB.dbc->maria.res = NULL;
}

char *gg_maria_fieldname()
{
    GG_TRACE("");
    return mysql_fetch_field(GG_CURR_DB.dbc->maria.res)->name;
}

gg_num gg_maria_nfield()
{
    GG_TRACE("");
    return mysql_num_fields(GG_CURR_DB.dbc->maria.res);
}

int gg_maria_use(char is_prep)
{
    GG_TRACE("");
    if (is_prep == 0)
    {
        GG_CURR_DB.dbc->maria.res = mysql_use_result(GG_CURR_DB.dbc->maria.con);
        if (GG_CURR_DB.dbc->maria.res == NULL)
        {
            cerror = "Error storing obtained data";
            return 1;
        }
    }
    else 
    {
        GG_CURR_DB.dbc->maria.res = mysql_stmt_result_metadata(GG_CURR_DB.dbc->maria.stmt);
        if (GG_CURR_DB.dbc->maria.res == NULL)
        {
            cerror = "Error storing obtained data (1)";
            return 1;
        }
    }
    return 0;
}

int gg_maria_store(char is_prep)
{
    GG_TRACE("");
    if (is_prep == 0)
    {
        GG_CURR_DB.dbc->maria.res = mysql_store_result(GG_CURR_DB.dbc->maria.con);
        if (GG_CURR_DB.dbc->maria.res == NULL)
        {
            cerror = "Error storing obtained data (2)";
            return 1;
        }
    }
    else
    {
        gg_maria_use (is_prep); // get metadata first
        my_bool update= 1;
        // get lengths
        mysql_stmt_attr_set(GG_CURR_DB.dbc->maria.stmt, STMT_ATTR_UPDATE_MAX_LENGTH, &update);
        // get result to copy to client
        if (mysql_stmt_store_result (GG_CURR_DB.dbc->maria.stmt) != 0)
        {
            cerror = "Error storing obtained data (3)";
            return 1;
        }
    }
    return 0;
}


void gg_maria_close ()
{
    GG_TRACE("");
    mysql_close (GG_CURR_DB.dbc->maria.con);
}


gg_dbc *gg_maria_connect (gg_num abort_if_bad)
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

    GG_CURR_DB.dbc->maria.con = mysql_init(NULL);
    GG_CURR_DB.dbc->maria.res = NULL;
    GG_CURR_DB.dbc->maria.bind = NULL;
    GG_CURR_DB.dbc->maria.bindout = NULL;
   
    if (GG_CURR_DB.dbc->maria.con == NULL) 
    {
        char *em = "Cannot initialize database connection";
        GG_TRACE ("%s", em);
        if (abort_if_bad == 1) gg_report_error ("%s", em);
        gg_end_connection (0);
        return NULL; 
    }  

    char db_config_name[150];
    snprintf (db_config_name, sizeof(db_config_name), "%s/%s", gg_get_config()->app.dbconf_dir, GG_CURR_DB.db_name);
    GG_TRACE ("Using db config file [%s]", db_config_name);
    mysql_optionsv (GG_CURR_DB.dbc->maria.con, MYSQL_READ_DEFAULT_FILE, db_config_name);

    GG_TRACE ("Logging in to database, config [%s]", db_config_name);
    if (mysql_real_connect(GG_CURR_DB.dbc->maria.con, NULL, NULL, NULL, 
                   NULL, 0, NULL, 0) == NULL) 
    {
        char em[300];
        snprintf (em, sizeof(em), "Error in logging in to database: error [%s], using config file [%s]", mysql_error(GG_CURR_DB.dbc->maria.con), db_config_name);
        GG_TRACE ("%s", em);
        if (abort_if_bad == 1) gg_report_error ("%s",em);
        gg_end_connection (0);
        return NULL;
    }    
    //
    // These are the most common settings. ANSI_QUOTES means that single quotes
    // are used for string and defense against SQL injection depends on this (could be done
    // either way or for both, but the odds are ansi_quotes is used anyway). UTF8 is used for
    // web communication.
    // So in short, do NOT change either one of these settings!
    //
    // Removed character set, since that can be set in the connection file! No need to hardcode.
    /*if (mysql_set_character_set(GG_CURR_DB.dbc->maria.con, "utf8mb4"))
    {
        char *em = "Cannot set character set to utf8mb4";
        GG_TRACE ("%s", em);
        gg_end_connection (1);
        if (abort_if_bad == 1) gg_report_error ("%s", em);
        return NULL;
    }*/

    if (mysql_query(GG_CURR_DB.dbc->maria.con, "set session sql_mode=ansi_quotes")) 
    {
        char *em = "Cannot set sql_mode to ansi_quotes";
        GG_TRACE ("%s", em);
        gg_end_connection (1);
        if (abort_if_bad == 1) gg_report_error ("%s", em);
        return NULL;
    }
    return GG_CURR_DB.dbc;
}

gg_num gg_maria_exec(char *s, char is_prep, void **prep, gg_num paramcount, char **params)
{   
    GG_TRACE("");
    gg_stmt_cached = 0;
    if (is_prep == 0)
    {
        return (mysql_query(GG_CURR_DB.dbc->maria.con, s));
    }
    else
    {
        if (gg_maria_prep_stmt (prep, s, paramcount)) return 1;
        gg_num i;
        for (i = 0; i < paramcount; i++)
        {
            gg_maria_add_input(i, params[i]);
        }
        gg_maria_bind_input ();
        return gg_maria_stmt_exec();
    }
}


//
// deallocate prepared statement (only when connection lost), st is void * to prep stmt.
//
void gg_maria_close_stmt (void *st)
{
    GG_TRACE("");
    if (st == NULL) return; // statement has not been prepared yet, so cannot deallocate
    if (GG_CURR_DB.dbc != NULL) 
    {
        // we only close statements when connection is lost, so we don't care about return code
        // only that memory is deallocated
        mysql_stmt_close((MYSQL_STMT *)st);
    }
}

//
// Prepare statement with source test stmt and num_of_args input parameters
// prep is the address of void * that points to prepared statement. This is a static
// void *, which survives requests (otherwise prepared statements wouldn't be very useful,
// actually would decrease performance), however prep is set to NULL when connection is 
// reestablished (typically if db server recycles), which is generally rare.
//
int gg_maria_prep_stmt(void **prep, char *stmt, gg_num num_of_args)
{
    GG_TRACE("");
    char *sname = "";
    gg_num lnum = 0;
    gg_location (&sname, &lnum, 0);

    // reuse prepared statement from prep
    if (*prep != NULL) 
    {
        gg_stmt_cached = 1;
        GG_TRACE ("reusing prepared statement");
        GG_CURR_DB.dbc->maria.stmt = (MYSQL_STMT*)*prep;
    }
    else
    {
        GG_TRACE ("creating prepared statement");
        // if prep is NULL, create prepared statement
        char *origs = stmt; // original stmt
        stmt = gg_db_prep_text(stmt); // make ? instead of '%s'
        // init prep stmt
        if ((GG_CURR_DB.dbc->maria.stmt = mysql_stmt_init (GG_CURR_DB.dbc->maria.con)) == NULL)
        {
            cerror = "Cannot initialize statement";
            return 1;
        }
        // prepare it
        if (mysql_stmt_prepare(GG_CURR_DB.dbc->maria.stmt, stmt, strlen(stmt)) != 0)
        {
            cerror = "Cannot prepare statement";
            return 1;
        }
        if (stmt != origs) gg_free_int (stmt); // release statement if actually allocated
        // save it for reuse for as long as the process lives (minus reconnects to db, but those are rare)
        *prep = GG_CURR_DB.dbc->maria.stmt;
    }
    GG_CURR_DB.num_inp = num_of_args;
    // check param count correct before binding as the statement and what golf says may be different, in which case
    // mariadb would access memory that doesn't exist if golf say there's less than really is
    gg_num count = mysql_stmt_param_count(GG_CURR_DB.dbc->maria.stmt);
    if (count != GG_CURR_DB.num_inp)
    {
        cerror = "Wrong number of input parameters";
        return 1;
    }

    if (GG_CURR_DB.num_inp != 0)
    {
        // allocate binds
        GG_CURR_DB.dbc->maria.bind = (MYSQL_BIND*)gg_calloc(num_of_args, sizeof(MYSQL_BIND));
    }
    return 0;
}


//
// Add input parameter to SQL about to be executed. 'i' is the index of parameter
// (which must be 0,1,2.. for each invocation), arg is the argument.
//
void gg_maria_add_input(gg_num i, char *arg)
{
    GG_TRACE("");
    // all inputs are converted to string
    GG_CURR_DB.dbc->maria.bind[i].buffer_type = MYSQL_TYPE_STRING;
    // if null, it's empty
    GG_CURR_DB.dbc->maria.bind[i].buffer = (char*)(arg == NULL ? "":arg);
    GG_CURR_DB.dbc->maria.bind[i].buffer_length = strlen(arg);
    GG_CURR_DB.dbc->maria.bind[i].length = NULL; // this is for array binding
    // never null, it's Golf's simplification
    GG_CURR_DB.dbc->maria.bind[i].is_null = 0;
}

//
// Statement that has been prepared and input params specified, is now bound
//
void gg_maria_bind_input ()
{
    GG_TRACE("");
    if (GG_CURR_DB.num_inp != 0) 
    {
        mysql_stmt_bind_param (GG_CURR_DB.dbc->maria.stmt, GG_CURR_DB.dbc->maria.bind);
    }
}




gg_num gg_maria_affected(char is_prep) 
{
    GG_TRACE("");
    if (is_prep == 0)
    {
        return (gg_num) mysql_affected_rows (GG_CURR_DB.dbc->maria.con);
    }
    else
    {
        return (gg_num) mysql_stmt_affected_rows (GG_CURR_DB.dbc->maria.stmt);
    }
}


//
// Execute statement that's prepared. Input binding is removed.
//
gg_num gg_maria_stmt_exec()
{
    GG_TRACE("");
    gg_num res = mysql_stmt_execute (GG_CURR_DB.dbc->maria.stmt);
    // freeing bind struct must be here and not gg_maria_free because gg_maria_free is not
    // called for non-tuple queries (such as INSERT), but this one is always called. If not like this,
    // bind ptr would remain and double free would be attempted
    if (GG_CURR_DB.dbc->maria.bind != NULL)
    {
        gg_free_int (GG_CURR_DB.dbc->maria.bind);
        GG_CURR_DB.dbc->maria.bind = NULL;
    }
    return res;
}

//
// Escape string for inclusion in quoted params. MariaDB may use different quotes, but we force ansi quotes.
// Even if we didn't, the string would be properly escaped.
// memory for to must be 2*len+1. *len  is the actual encoded length without zero byte counted.
// Returns 0 for okay, 1 otherwise.
//
int gg_maria_escape(char *from, char *to, gg_num *len)
{
    GG_TRACE("");
    *len = (gg_num)mysql_real_escape_string(GG_CURR_DB.dbc->maria.con, to, from, (unsigned long) *len);
    if ((unsigned long)*len == (unsigned long) -1) return 1;
    return 0;
}


