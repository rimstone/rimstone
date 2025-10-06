// SPDX-License-Identifier: Apache-2.0
// Copyright 2018-2025 Gliim LLC. 
// Licensed under Apache License v2. See LICENSE file.
// On the web http://rimstone-lang.com/ - this file is part of RimStone framework.

// 
// mariadb native interface, used by db.c for making of db-specific libs
//
// All errors must go through rim_maria_error() and the sole point of entry is rim_maria_exec()
// in order to avoid disorderly rollbacks in case of errors
//

#include "rim.h"
static rim_num rim_maria_stmt_exec();
static int rim_maria_stmt_rows (char ***row, unsigned long **lens);
static void rim_maria_add_input(rim_num i, char *arg);
static void rim_maria_bind_input ();
static int rim_maria_prep_stmt(void **prep, char *stmt, rim_num num_of_args);

static char *cerror = NULL;


rim_num rim_maria_checkc()
{
    return (mysql_ping (RIM_CURR_DB.dbc->maria.con) == 0 ? 1 : 0);
}

char *rim_maria_errm(char *errm, rim_num errmsize, char *s, char *sname, rim_num lnum, char *er, char is_prep)
{
    if (is_prep == 0)
    {
        snprintf(errm,errmsize,"Error during query [%s], additional [%s] file [%s], line [%ld] : [%s]:%s", s, cerror==NULL?"":cerror, sname, lnum, er ,atol(er) == ER_PARSE_ERROR ?  "Problem with parsing SQL statement" : mysql_error(RIM_CURR_DB.dbc->maria.con));
        return errm;
    }
    else
    {
        snprintf(errm,errmsize,"Error during query [%s], additional [%s] file [%s], line [%ld] : [%s]:%s", s, cerror==NULL?"":cerror, sname, lnum, er ,atol(er) == ER_PARSE_ERROR ?  "Problem with parsing SQL statement" : mysql_stmt_error(RIM_CURR_DB.dbc->maria.stmt));
        return errm;
    }
}


char *rim_maria_error(char *s, char is_prep)
{
    RIM_UNUSED(s); // used in tracing only
    if (is_prep == 0)
    {
        static char errv[30];
        snprintf (errv, sizeof(errv), "%d", mysql_errno(RIM_CURR_DB.dbc->maria.con));
        return errv;
    }
    else
    {
        static char errv[30];
        snprintf (errv, sizeof(errv), "%d", mysql_stmt_errno(RIM_CURR_DB.dbc->maria.stmt));
        return errv;
    }
}


int rim_maria_rows (char ***row, unsigned long **lens, char is_prep)
{
    if (is_prep == 0)
    {
        RIM_CURR_DB.need_copy = 1;
        // fetch a row, field by field
        MYSQL_ROW row1; // this is char **, if that changes than code here must be refactored, or made to use
                      // that new structure
        row1 = mysql_fetch_row(RIM_CURR_DB.dbc->maria.res);
        *row = row1; // simple now, may not be in the future. In any case, the returning row variable must
                    // be an array of strings
        *lens = mysql_fetch_lengths (RIM_CURR_DB.dbc->maria.res);
    }
    else
    {
        RIM_CURR_DB.need_copy = 0;
        if (rim_maria_stmt_rows (row, lens)) return 1;
    }
    return 0;
}


//
// Get a row when tuples returned. *row[] is an array of strings which represent fields (columns) and *lens is the
// array of lengths of those fields.
//
static int rim_maria_stmt_rows (char ***row, unsigned long **lens)
{
    char *sname = "";
    rim_num lnum = 0;
    // get which file and line number is this going on at
    rim_location (&sname, &lnum, 0);

    // number of fields in a row
    rim_num nf = mysql_num_fields(RIM_CURR_DB.dbc->maria.res);
    rim_num i;

    // get result binding array
    RIM_CURR_DB.dbc->maria.bindout = (MYSQL_BIND*)rim_calloc(nf, sizeof(MYSQL_BIND));

    // get results (columns, lengths)
    *row = (char **)rim_calloc (nf, sizeof(char*));
    *lens = (unsigned long *)rim_calloc (nf, sizeof(unsigned long));

    // rlen is the array of actual lengths of an array of fields in a row, obtained from mariadb fetching
    unsigned long *rlen = (unsigned long*)rim_calloc (nf, sizeof(unsigned long));
    // array of whether fields are NULL
    my_bool *isnull = (char*)rim_calloc (nf, sizeof(char));
    // dummy storage for first pass of fetching
    char dummy[2]; // just in case give it storage
    // set all cols to strings, unknown length

    // set binding for results, we want all as strings. Length is 0 so we can obtain actual lengths in second pass.
    for (i = 0; i < nf; i++) 
    {
        RIM_CURR_DB.dbc->maria.bindout[i].buffer_type = MYSQL_TYPE_STRING;
        RIM_CURR_DB.dbc->maria.bindout[i].buffer = &(dummy[0]);
        RIM_CURR_DB.dbc->maria.bindout[i].buffer_length = 0;
        RIM_CURR_DB.dbc->maria.bindout[i].is_null = &(isnull[i]);
        RIM_CURR_DB.dbc->maria.bindout[i].length = &(rlen[i]);
    }

    // bind results
    if (mysql_stmt_bind_result(RIM_CURR_DB.dbc->maria.stmt, RIM_CURR_DB.dbc->maria.bindout) != 0)
    {
        cerror = "Cannot initialize statement";
        return 1;
    }
    rim_num cdata = 0; // index of fields within a row

    //
    // Get a single row
    //
    // set empty for each row, because we don't know the size of its columns and each must be separately allocated
    for (i = 0; i < nf; i++) 
    {
        RIM_CURR_DB.dbc->maria.bindout[i].buffer = &(dummy[0]);
        RIM_CURR_DB.dbc->maria.bindout[i].buffer_length = 0;
    }
    // fetch a row
    int res = mysql_stmt_fetch(RIM_CURR_DB.dbc->maria.stmt);
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
            RIM_CURR_DB.dbc->maria.bindout[i].buffer_length = rlen[i]+1;
            RIM_CURR_DB.dbc->maria.bindout[i].buffer = rim_malloc (rlen[i] + 1 + 1); // just in case extra byte of storage
        }
        // now fetch columns based on actual lengths for each field
        for (i = 0; i < nf; i++) 
        {
            if (mysql_stmt_fetch_column(RIM_CURR_DB.dbc->maria.stmt, &(RIM_CURR_DB.dbc->maria.bindout[i]), i, 0) != 0)
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
        (*row)[cdata] = RIM_CURR_DB.dbc->maria.bindout[i].buffer == NULL ? RIM_EMPTY_STRING : RIM_CURR_DB.dbc->maria.bindout[i].buffer;
        (*lens)[cdata] = *(RIM_CURR_DB.dbc->maria.bindout[i].length);
        cdata++;
    }

    // free allocations
    rim_free_int (RIM_CURR_DB.dbc->maria.bindout);
    rim_free_int (rlen);
    rim_free_int (isnull);
    return 0;
}


rim_num rim_maria_nrows(char is_prep)
{
    if (is_prep == 0)
    {
        // works since we use buffered (store) method to get results
        return mysql_num_rows (RIM_CURR_DB.dbc->maria.res);
    }
    else
    {
        return mysql_stmt_num_rows (RIM_CURR_DB.dbc->maria.stmt);
    }
}

void rim_maria_free()
{
    if (RIM_CURR_DB.dbc->maria.res != NULL) mysql_free_result(RIM_CURR_DB.dbc->maria.res);
    RIM_CURR_DB.dbc->maria.res = NULL;
}

char *rim_maria_fieldname()
{
    return mysql_fetch_field(RIM_CURR_DB.dbc->maria.res)->name;
}

rim_num rim_maria_nfield()
{
    return mysql_num_fields(RIM_CURR_DB.dbc->maria.res);
}

int rim_maria_use(char is_prep)
{
    if (is_prep == 0)
    {
        RIM_CURR_DB.dbc->maria.res = mysql_use_result(RIM_CURR_DB.dbc->maria.con);
        if (RIM_CURR_DB.dbc->maria.res == NULL)
        {
            cerror = "Error storing obtained data";
            return 1;
        }
    }
    else 
    {
        RIM_CURR_DB.dbc->maria.res = mysql_stmt_result_metadata(RIM_CURR_DB.dbc->maria.stmt);
        if (RIM_CURR_DB.dbc->maria.res == NULL)
        {
            cerror = "Error storing obtained data (1)";
            return 1;
        }
    }
    return 0;
}

int rim_maria_store(char is_prep)
{
    if (is_prep == 0)
    {
        RIM_CURR_DB.dbc->maria.res = mysql_store_result(RIM_CURR_DB.dbc->maria.con);
        if (RIM_CURR_DB.dbc->maria.res == NULL)
        {
            cerror = "Error storing obtained data (2)";
            return 1;
        }
    }
    else
    {
        rim_maria_use (is_prep); // get metadata first
        my_bool update= 1;
        // get lengths
        mysql_stmt_attr_set(RIM_CURR_DB.dbc->maria.stmt, STMT_ATTR_UPDATE_MAX_LENGTH, &update);
        // get result to copy to client
        if (mysql_stmt_store_result (RIM_CURR_DB.dbc->maria.stmt) != 0)
        {
            cerror = "Error storing obtained data (3)";
            return 1;
        }
    }
    return 0;
}


void rim_maria_close ()
{
    mysql_close (RIM_CURR_DB.dbc->maria.con);
}


rim_dbc *rim_maria_connect (rim_num abort_if_bad)
{
    // reset all prepared statements
    rim_db_prep (NULL);

    // allocate connection to database. Must be done here to account for the actual generated code, which
    // dictates the sizeof(rim_dbc)
    // This must be malloc and NOT rim_malloc since we want to reuse connection across connections.
    // Otherwise rim_malloc would be automatically freed when the request is done, and the next 
    // request would use an invalid pointer. Also must use free to free it, not rim_free.
    if ((RIM_CURR_DB.dbc = rim_malloc0 (sizeof (rim_dbc))) == NULL) RIM_FATAL ("Cannot allocate memory for database connection [%m]");

    RIM_CURR_DB.dbc->maria.con = mysql_init(NULL);
    RIM_CURR_DB.dbc->maria.res = NULL;
    RIM_CURR_DB.dbc->maria.bind = NULL;
    RIM_CURR_DB.dbc->maria.bindout = NULL;
   
    if (RIM_CURR_DB.dbc->maria.con == NULL) 
    {
        char *em = "Cannot initialize database connection";
        if (abort_if_bad == 1) rim_report_error ("%s", em);
        rim_end_connection (0);
        return NULL; 
    }  

    char db_config_name[150];
    snprintf (db_config_name, sizeof(db_config_name), "%s/%s", rim_get_config()->app.dbconf_dir, RIM_CURR_DB.db_name);
    mysql_optionsv (RIM_CURR_DB.dbc->maria.con, MYSQL_READ_DEFAULT_FILE, db_config_name);

    if (mysql_real_connect(RIM_CURR_DB.dbc->maria.con, NULL, NULL, NULL, 
                   NULL, 0, NULL, 0) == NULL) 
    {
        char em[300];
        snprintf (em, sizeof(em), "Error in logging in to database: error [%s], using config file [%s]", mysql_error(RIM_CURR_DB.dbc->maria.con), db_config_name);
        if (abort_if_bad == 1) rim_report_error ("%s",em);
        rim_end_connection (0);
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
    /*if (mysql_set_character_set(RIM_CURR_DB.dbc->maria.con, "utf8mb4"))
    {
        char *em = "Cannot set character set to utf8mb4";
        rim_end_connection (1);
        if (abort_if_bad == 1) rim_report_error ("%s", em);
        return NULL;
    }*/

    if (mysql_query(RIM_CURR_DB.dbc->maria.con, "set session sql_mode=ansi_quotes")) 
    {
        char *em = "Cannot set sql_mode to ansi_quotes";
        rim_end_connection (1);
        if (abort_if_bad == 1) rim_report_error ("%s", em);
        return NULL;
    }
    return RIM_CURR_DB.dbc;
}

rim_num rim_maria_exec(char *s, char is_prep, void **prep, rim_num paramcount, char **params)
{   
    rim_stmt_cached = 0;
    if (is_prep == 0)
    {
        return (mysql_query(RIM_CURR_DB.dbc->maria.con, s));
    }
    else
    {
        if (rim_maria_prep_stmt (prep, s, paramcount)) return 1;
        rim_num i;
        for (i = 0; i < paramcount; i++)
        {
            rim_maria_add_input(i, params[i]);
        }
        rim_maria_bind_input ();
        return rim_maria_stmt_exec();
    }
}


//
// deallocate prepared statement (only when connection lost), st is void * to prep stmt.
//
void rim_maria_close_stmt (void *st)
{
    if (st == NULL) return; // statement has not been prepared yet, so cannot deallocate
    if (RIM_CURR_DB.dbc != NULL) 
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
static int rim_maria_prep_stmt(void **prep, char *stmt, rim_num num_of_args)
{
    char *sname = "";
    rim_num lnum = 0;
    rim_location (&sname, &lnum, 0);

    // reuse prepared statement from prep
    if (*prep != NULL) 
    {
        rim_stmt_cached = 1;
        RIM_CURR_DB.dbc->maria.stmt = (MYSQL_STMT*)*prep;
    }
    else
    {
        // if prep is NULL, create prepared statement
        char *origs = stmt; // original stmt
        stmt = rim_db_prep_text(stmt); // make ? instead of '%s'
        // init prep stmt
        if ((RIM_CURR_DB.dbc->maria.stmt = mysql_stmt_init (RIM_CURR_DB.dbc->maria.con)) == NULL)
        {
            cerror = "Cannot initialize statement";
            return 1;
        }
        // prepare it
        if (mysql_stmt_prepare(RIM_CURR_DB.dbc->maria.stmt, stmt, strlen(stmt)) != 0)
        {
            cerror = "Cannot prepare statement";
            return 1;
        }
        if (stmt != origs) rim_free_int (stmt); // release statement if actually allocated
        // save it for reuse for as long as the process lives (minus reconnects to db, but those are rare)
        *prep = RIM_CURR_DB.dbc->maria.stmt;
    }
    RIM_CURR_DB.num_inp = num_of_args;
    // check param count correct before binding as the statement and what rimstone says may be different, in which case
    // mariadb would access memory that doesn't exist if rimstone say there's less than really is
    rim_num count = mysql_stmt_param_count(RIM_CURR_DB.dbc->maria.stmt);
    if (count != RIM_CURR_DB.num_inp)
    {
        cerror = "Wrong number of input parameters";
        return 1;
    }

    if (RIM_CURR_DB.num_inp != 0)
    {
        // allocate binds
        RIM_CURR_DB.dbc->maria.bind = (MYSQL_BIND*)rim_calloc(num_of_args, sizeof(MYSQL_BIND));
    }
    return 0;
}


//
// Add input parameter to SQL about to be executed. 'i' is the index of parameter
// (which must be 0,1,2.. for each invocation), arg is the argument.
//
static void rim_maria_add_input(rim_num i, char *arg)
{
    // all inputs are converted to string
    RIM_CURR_DB.dbc->maria.bind[i].buffer_type = MYSQL_TYPE_STRING;
    // if null, it's empty
    RIM_CURR_DB.dbc->maria.bind[i].buffer = (char*)(arg == NULL ? "":arg);
    RIM_CURR_DB.dbc->maria.bind[i].buffer_length = strlen(arg);
    RIM_CURR_DB.dbc->maria.bind[i].length = NULL; // this is for array binding
    // never null, it's RimStone's simplification
    RIM_CURR_DB.dbc->maria.bind[i].is_null = 0;
}

//
// Statement that has been prepared and input params specified, is now bound
//
static void rim_maria_bind_input ()
{
    if (RIM_CURR_DB.num_inp != 0) 
    {
        mysql_stmt_bind_param (RIM_CURR_DB.dbc->maria.stmt, RIM_CURR_DB.dbc->maria.bind);
    }
}




rim_num rim_maria_affected(char is_prep) 
{
    if (is_prep == 0)
    {
        return (rim_num) mysql_affected_rows (RIM_CURR_DB.dbc->maria.con);
    }
    else
    {
        return (rim_num) mysql_stmt_affected_rows (RIM_CURR_DB.dbc->maria.stmt);
    }
}


//
// Execute statement that's prepared. Input binding is removed.
//
static rim_num rim_maria_stmt_exec()
{
    rim_num res = mysql_stmt_execute (RIM_CURR_DB.dbc->maria.stmt);
    // freeing bind struct must be here and not rim_maria_free because rim_maria_free is not
    // called for non-tuple queries (such as INSERT), but this one is always called. If not like this,
    // bind ptr would remain and double free would be attempted
    if (RIM_CURR_DB.dbc->maria.bind != NULL)
    {
        rim_free_int (RIM_CURR_DB.dbc->maria.bind);
        RIM_CURR_DB.dbc->maria.bind = NULL;
    }
    return res;
}

//
// Escape string for inclusion in quoted params. MariaDB may use different quotes, but we force ansi quotes.
// Even if we didn't, the string would be properly escaped.
// memory for to must be 2*len+1. *len  is the actual encoded length without zero byte counted.
// Returns 0 for okay, 1 otherwise.
//
int rim_maria_escape(char *from, char *to, rim_num *len)
{
    *len = (rim_num)mysql_real_escape_string(RIM_CURR_DB.dbc->maria.con, to, from, (unsigned long) *len);
    if ((unsigned long)*len == (unsigned long) -1) return 1;
    return 0;
}


