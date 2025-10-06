// SPDX-License-Identifier: Apache-2.0
// Copyright 2018-2025 Gliim LLC. 
// Licensed under Apache License v2. See LICENSE file.
// On the web http://rimstone-lang.com/ - this file is part of RimStone framework.

// 
// interface to Postgres
// DB connection is for the life of process - if it goes down, so does
// the process. 
//

#include "rim.h"

//
// Note: we ALWAYS use rim_get_db_connection () explicitly whenever the database
// connection is needed. The reason is that, if db connection drops and can be reestablished,
// we will reconnect. This happens transparently.
//


#define _RIM_LOOK_S "'%s'"

// function prototypes
static rim_num rim_handle_error (char *s, char **er, char **err_message, rim_num retry, char is_prep, char erract);
void rim_end_connection(rim_num close_db);
static rim_num rim_retry_db();
static rim_num rim_firstword(char *w, char *s);
static void rim_arows(rim_num *arows, char is_prep);

int rim_stmt_cached = 0;


// Close database connection. This is when database connection was already detected lost, and we need to 
// match that and in no other situation. This is to end the connection regardless of which database it is.
// If close_db is 1, then close db-specific connection. In some cases, when connection fails in the first 
// place, the connection doesn't exist, so closing it is questionable.
// We NEVER close connection in our code, EXCEPT on shutdown of command line program.
void rim_end_connection(rim_num close_db) {
    if (RIM_CURR_DB.dbc != NULL) 
    {
        if (close_db == 1) 
        {
            if (RIM_CURR_DB.db_type == RIM_DB_POSTGRES)
            {
                rim_pg_close();
            }
            if (RIM_CURR_DB.db_type == RIM_DB_MARIADB)
            {
                rim_maria_close ();
            } 
            if (RIM_CURR_DB.db_type == RIM_DB_SQLITE)
            {
                rim_lite_close ();
            } 
        }

        free (RIM_CURR_DB.dbc); // was created by malloc() and not rim_malloc, so free must be used
        RIM_CURR_DB.dbc = NULL;
        RIM_CURR_DB.is_begin_transaction = 0; // we are no longer in a transaction if
                    // connection closed
    }
}

// 
// Get database connection. Caches connection and connects ONE TIME only. 
// We use a db connection "for-ever", i.e. as long as possible.
// Returns connection data pointer, or NULL if failed to connect.
// 
rim_dbc *rim_get_db_connection (rim_num abort_if_bad)
{

    // this index is used in RIM_CURR_DB
    if (rim_get_config()->ctx.db->ind_current_db == -1) 
    {
        // this shouldn't happen, as all RimStone db ops are meant to have db connection. A bug if it gets here.
        rim_report_error ("No active database in use, yet database operation attempted");
    }

    //
    // ...db.ind_current_db is ALWAYS passed to here as the database to use. The number is predetermined
    // at compile time and each database name has a fixed slot in RIM_CTX.db[] array.
    //

    if (RIM_CURR_DB.dbc != NULL) 
    {
        return RIM_CURR_DB.dbc; 
            // already connected
    }
    
    // 
    // One process has ONLY one connection. We can reconnect ONLY if we were NOT in a transaction. 
    // 'Reconnecting' means 'has_connected' is 1, and we're trying to connect again.
    // We cannot reconnect if we are in a transaction, because the database would have automatically
    // rollbacked on disconnect (that happened prior to this) and it would be complicated for an application to recover from this.
    // If we try to connect a second time in this case, error out.
    //
    if (RIM_CURR_DB.has_connected==1)
    {
        if (RIM_CURR_DB.is_begin_transaction ==1) 
        {
            //
            // If we're in a transaction, we have to exit as we cannot recover from a rollback of a
            // transaction initiated by the application. If however, we are not in a transaction,
            // we can safely establish a new connection and let application continue with a new
            // transaction or a single-statement transaction.
            //
            rim_report_error ("The connection to database has been lost"); 
        }
        //
        // Here we are if we're not in a transaction
        //
    }


    if (RIM_CURR_DB.db_type == RIM_DB_POSTGRES)
    {
        if (rim_pg_connect (abort_if_bad) == NULL) return NULL;
    }
    else if (RIM_CURR_DB.db_type == RIM_DB_MARIADB)
    {
        if (rim_maria_connect (abort_if_bad) == NULL) return NULL;
    }
    else if (RIM_CURR_DB.db_type == RIM_DB_SQLITE)
    {
        if (rim_lite_connect (abort_if_bad) == NULL) return NULL;
    }
    else
    {
        rim_report_error ("Unknown database type [%ld]", RIM_CURR_DB.db_type); 
    }

    RIM_CURR_DB.has_connected = 1;
    // 
    // The following (due to above guard) is executed ONLY ONCE for the life of process
    //
    RIM_CURR_DB.is_begin_transaction = 0; // we are no longer in a transaction if
                        // connection is being (re)opened, this is implicit rollback

    return RIM_CURR_DB.dbc;
}

// We NEVER close connection in our code, EXCEPT on shutdown of command line program.
             
// 
// Begin transaction. 
// Returns 1 if okay, 0 if failed. It may just stop and report error, depending on on-error settings (both
// erract is RIM_ON_ERROR_CONTINUE/EXIT for statement-specific on-error continue/exit or RIM_OKAY if db-level on-error is in effect.
// err is the error code of any error, errt is error text - both are NULL if none requested.
//
rim_num rim_begin_transaction(char *t, char erract, char **err, char **errt)
{

    char *er;
    rim_num rows;
    char *errm="";
    char start[512];
    if (RIM_CURR_DB.db_type == RIM_DB_SQLITE)
    {
        snprintf (start, sizeof(start), "begin %s", t);
    } 
    else 
    {   
        snprintf (start, sizeof(start), "start transaction %s", t);
    }
// last param is RIM_ON_ERROR_CONTINUE/EXIT for statement-specific on-error continue/exit or RIM_OKAY if db-level on-error is in effect.
    if (rim_execute_SQL (start, &rows, &er, &errm, 0, 0, 0, NULL, 0, NULL, erract) == NULL)
    {
        RIM_CURR_DB.is_begin_transaction = 0;
        return 0;
    }
    if (err != NULL) *err = er; else rim_free_int((void*)er);
    if (errt != NULL) *errt = errm; else rim_free_int((void*)errm);
    RIM_CURR_DB.is_begin_transaction = 1;
    return 1;
}

// 
// Close connection for all dbs and release resources. Done only just before exit.
//
void rim_end_all_db()
{
    rim_num i;
    for (i = 0; i < rim_get_config()->ctx.tot_dbs; i++)
    {
        rim_get_config()->ctx.db->ind_current_db = i;
        rim_end_connection (1);
    }
}

// 
// Check if any transaction is opened, for any db. If so, rollback them. If check_mode is 1, return.
// If check_mode is anything but 1, error out.
// If transaction is not opened, return regardless.
//
void rim_check_transaction(rim_num check_mode)
{
    rim_num savedb = rim_get_config()->ctx.db->ind_current_db;
    rim_num i;
    rim_num err = 0;
    rim_num errdb = -1;
    for (i = 0; i < rim_get_config()->ctx.tot_dbs; i++)
    {
        if (rim_get_config()->ctx.db->conn[i].is_begin_transaction == 1) 
        {
            errdb = i;
            rim_get_config()->ctx.db->ind_current_db = i;
            rim_rollback("", RIM_OKAY, NULL, NULL);
            rim_get_config()->ctx.db->ind_current_db = savedb;
            err =1;
        }
    }
    if (err == 1)
    {
        if (check_mode==1) return;
        rim_report_error ("Started transaction database [%s], but was never committed or rollbacked", rim_get_config()->ctx.db->conn[errdb].db_name);
    }
}

// 
// Commit transaction. 
// Returns 1 if okay, 0 if failed. It may just stop and report error, depending on on-error settings (both
// erract is RIM_ON_ERROR_CONTINUE/EXIT for statement-specific on-error continue/exit or RIM_OKAY if db-level on-error is in effect.
// err is the error code of any error, errt is error text - both are NULL if none requested.
//
rim_num rim_commit(char *t, char erract, char **err, char **errt)
{

    RIM_CURR_DB.is_begin_transaction = 0;

    char *er;
    rim_num rows;
    char *errm="";
    char comm[512];
    snprintf (comm, sizeof(comm), "commit %s", t);
// last param is RIM_ON_ERROR_CONTINUE/EXIT for statement-specific on-error continue/exit or RIM_OKAY if db-level on-error is in effect.
    if (rim_execute_SQL (comm, &rows, &er, &errm, 0, 0, 0, NULL, 0, NULL, erract) == NULL)
    {
        return 0;
    }
    if (err != NULL) *err = er; else rim_free_int((void*)er);
    if (errt != NULL) *errt = errm; else rim_free_int((void*)errm);
    return 1;
}

// 
// Rollbacks the transaction. 
// Returns 1 if okay, 0 if failed. It may just stop and report error, depending on on-error settings (both
// global and per-statement).
// erract is RIM_ON_ERROR_CONTINUE/EXIT for statement-specific on-error continue/exit or RIM_OKAY if db-level on-error is in effect.
// err is the error code of any error, errt is error text - both are NULL if none requested.
//
rim_num rim_rollback(char *t, char erract, char **err, char **errt)
{

    RIM_CURR_DB.is_begin_transaction = 0;

    char *er;
    rim_num rows;
    char *errm="";
    char rollb[512];
    snprintf (rollb, sizeof(rollb), "rollback %s", t);
// last param is RIM_ON_ERROR_CONTINUE/EXIT for statement-specific on-error continue/exit or RIM_OKAY if db-level on-error is in effect.
    if (rim_execute_SQL (rollb, &rows, &er, &errm, 0, 0, 0, NULL, 0, NULL, erract) == NULL)
    {
        return 0;
    }
    if (err != NULL) *err = er; else rim_free_int((void*)er);
    if (errt != NULL) *errt = errm; else rim_free_int((void*)errm);
    return 1;
}

//
// Return 1 if the first word of 's' is 'w', otherwise 0. If 's' is quoted, ignore quotes.
// Also, ignore leading spaces.
//
static rim_num rim_firstword(char *w, char *s)
{
    rim_num l = strlen (w);
    char *mbeg = s;
    // avoid spaces and quotes in the beginning (could be spaces, quote, spaces again)
    while (isspace(*mbeg) || *mbeg == '"') mbeg++; // right now, always quoted, get passed the quote
    if (!strncasecmp (mbeg, (w), l) && (mbeg[l] == 0 || mbeg[l] == '"' || isspace(mbeg[l]))) return 1; else return 0;
}

//
// Get the number of rows affected in arows
//
static void rim_arows(rim_num *arows, char is_prep)
{
    if (arows!= NULL)
    {
        if (RIM_CURR_DB.db_type == RIM_DB_POSTGRES)
        {
            *arows = rim_pg_affected();
        }
        else if (RIM_CURR_DB.db_type == RIM_DB_MARIADB)
        {
            *arows = rim_maria_affected(is_prep);
        }
        else if (RIM_CURR_DB.db_type == RIM_DB_SQLITE)
        {
            *arows = rim_lite_affected(is_prep);
        }
        else
        {
            rim_report_error ("Unknown database type [%ld]", RIM_CURR_DB.db_type); 
        }
    }
}

// 
// Execute SQL. 
// 's' is the SQL, 'rows' is the output number of rows returned (if not NULL, in which case it's nothing).
// 'er' is the error code (if there was an error), and err_messsage is allocated and filled with error message, if there was an error.
// If user_check is 1, then make sure begin/start/commit/rollback isn't allowed (and any other user restrictions).
// prep is ptr for prepared stmt. is_prep is 1 if prep stmt.
// Returns  NULL if there is an error, non-NULL if it is okay.
// erract is RIM_ON_ERROR_CONTINUE/EXIT for statement-specific on-error continue/exit or RIM_OKAY if db-level on-error is in effect.
// returns_tuple is 1 if the SQL returns tuple, such as SELECT. It could be also SQL liks INSERT..RETURNING() so not
// necessarily DML vs SELECT.
//
rim_dbc *rim_execute_SQL (char *s,  rim_num *arows, char **er, char **err_message, rim_num returns_tuple, rim_num user_check, char is_prep, void **prep, rim_num paramcount, char **params, char erract)
{

    // get location in source code (if set, RIM automatically does this)
    char *sname = "";
    rim_num lnum = 0; 
    rim_location (&sname, &lnum, 0);
    if (s[0] == 0)
    {
        rim_report_error ( "Query cannot be empty, reading file [%s] at line [%ld]", sname, lnum);
    }

    // s (from the end-user) cannot be BEGIN/START, nor COMMIT/ROLLBACK - if it were, we couldn't control reconnection.
    // If connection to db got lost in the middle of transaction, and we weren't aware, whatever
    // happened so far would be either rolled back or committed (depending on the db setting)
    // and the rest would also be independently rolled back or committed - because transaction
    // is tied to a *connection* and we have two of those here.
    // Note that s from RimStone (as in from rim_begin_transaction) is OKAY
    if (user_check == 1)
    {
        if (rim_firstword("BEGIN", s) || rim_firstword("START", s) || rim_firstword("COMMIT", s) || rim_firstword("ROLLBACK", s))
        {
            rim_report_error ( "Use RimStone begin-transaction, commit-transaction or rollback-transaction instead of direct database SQL for these statements, reading file [%s] at line [%ld]", sname, lnum);
        }
    }

    rim_dbc *dbc = rim_get_db_connection (1);
    

    // init error code/message
    char *oker =  rim_strdup("0"); // success for error in query-result#...,error...
    *er = oker;
    *err_message = RIM_EMPTY_STRING;

    rim_num execres = 0;
    if (RIM_CURR_DB.db_type == RIM_DB_POSTGRES)
    {   
        execres = rim_pg_exec (s, returns_tuple, is_prep, prep, paramcount, params);
    }
    else if (RIM_CURR_DB.db_type == RIM_DB_MARIADB)
    {   
        execres = rim_maria_exec (s, is_prep, prep, paramcount, params);
    }
    else if (RIM_CURR_DB.db_type == RIM_DB_SQLITE)
    {   
        execres = rim_lite_exec (s, is_prep, prep, paramcount, params);
    }
    else
    {
        rim_report_error ("Unknown database type [%ld]", RIM_CURR_DB.db_type); 
    }

    if (execres)
    {
        //
        // Error in execution. If we're not in a transaction, try to reconnect ONCE.
        // If we ARE in a transaction, we CANNOT reconnect.
        // By transaction, I mean usage of RimStone begin-transaction - no other kind is allowed.
        //
        if (RIM_CURR_DB.is_begin_transaction == 0)
        {
            //
            // While handling error, if error is unrecoverable (or on-error is set to exit), rim_handle_error
            // will stop the program. If error can be reported back to the application,
            // then  we will set arows to 0 and return 0.
            //
            // Since we're NOT in transaction here, try to reconnect ONCE if the error is 'lost
            // connection' or such. The 4th parameter of 1 means that only in case of such errors,
            // try to reconnect. If the error is not lost connection, then we will report it
            // back by setting arows to 0 and returning 0.
            //
            if (rim_handle_error (s, er, err_message, 1, is_prep, erract) == 0)
            {
                //
                // This means there was an error which may be anything or 'lost connection' (er is "-1"). Return to application
                //
                if (arows!= NULL) *arows = 0;
                return NULL;
            }
            else
            {
                //
                // This means connection was lost and was successfully reestablished. Try executing
                // statement again, but get NEW connection first. Remember, we were NOT in a transaction
                // to begin with, so we can actually do retry here.
                //
                if (RIM_CURR_DB.db_type == RIM_DB_POSTGRES)
                {   
                    execres = rim_pg_exec (s, returns_tuple, is_prep, prep, paramcount, params);
                }
                else if(RIM_CURR_DB.db_type == RIM_DB_MARIADB)
                {   
                    execres = rim_maria_exec (s, is_prep, prep, paramcount, params);
                }
                else if(RIM_CURR_DB.db_type == RIM_DB_SQLITE)
                {   
                    execres = rim_lite_exec (s, is_prep, prep, paramcount, params);
                }
                else
                {
                    rim_report_error ("Unknown database type [%ld]", RIM_CURR_DB.db_type); 
                }

                if (execres)
                {
                    //
                    // There was still an error, and this time we will NOT try reconnecting. 
                    // If case of any error, including lost connection,report to application.
                    // We don't check return value, because IF rim_handle_error returns, it has to be 0 in this case,
                    // because retry is set to 0.
                    //
                    rim_handle_error (s, er, err_message, 0, is_prep, erract);
                    if (arows!= NULL) *arows = 0;
                    return NULL;
                }
                else
                {
                    // 
                    // SQL statement executed correctly after reconnecting. Just proceed
                    //
                    *er = oker; // clear previous error, make it okay
                }
            }
        }
        else
        {
            //
            // This is if we're in a transaction. Do NOT try to reconnect - if we lost connection, consider it an error.
            // Otherwise, report error back to the application.
            // We don't check return value, because IF rim_handle_error returns, it has to be 0.
            //
            rim_handle_error (s, er, err_message, 0, is_prep, erract);
            if (arows!= NULL)  *arows = 0;
            return NULL;
        }
    }

    //
    // Getting rows-affected Not done here for SELECT (see rim_select_table)
    //
    rim_arows(arows, is_prep);

    return dbc;
}

// when error is encountered, retry to connect (assuming we're not in a transaction, which is checked in the caller) 
// if ok then return 1, otherwise return 0. 
static rim_num rim_retry_db()
{
    //
    // Retry connection
    //
    //
    // First we must reset connection we have already. It's lost so we're not calling mysql_close().
    // Without this, an attempt to retry connection would not work, and would just return the old cached bad
    // connection
    //
    rim_end_connection(1);
    //
    // Retry it
    //
    if (rim_get_db_connection(1) != NULL)
    {
        //
        // Retry was instructed and successful
        //
        return 1;
    }
    else
    {
        return 0;
    }
}

//
// Handle error of execution of SQL. 's' is the statement. 'con' is the db connection.
// 'er' is the output error, and its text is in output variable err_message which can be NULL in which case none is output.
// 'retry' has any meaning ONLY if connection was actually lost. Only if 'retry' is 1 AND this is a lost connection, try to 
// reconnect ONCE; if retry is 0, or couldn't reconnect, then return to application with -1 error.
// erract is RIM_ON_ERROR_CONTINUE/EXIT for statement-specific on-error continue/exit or RIM_OKAY if db-level on-error is in effect.
// If this is something other than lost connection, then handle it by returning 0. 
// erract is RIM_ON_ERROR_CONTINUE/EXIT for statement-specific on-error continue/exit or RIM_OKAY if db-level on-error is in effect.
// Returns 1 if reconnect was successful (meaning retry had to be 1 for this to happen), 0 in any other case.
// If on-error is set to exit, then stop program in any case but successfull reconnection.
//
static rim_num rim_handle_error (char *s, char **er, char **err_message, rim_num retry, char is_prep, char erract)
{
    // This static is fine - it is used only within a single request, i.e. it doesn't span multiple request.
    // Errm is set HERE and used right after it - it doesn't go beyond a request.
    static char errm[8192];

    // get location in source code (if set, RIM automatically does this)
    char *sname = "";
    rim_num lnum = 0;
    rim_location (&sname, &lnum, 0);

    *er = RIM_EMPTY_STRING;
    if (err_message!=NULL) *err_message=RIM_EMPTY_STRING;

    rim_num connlost = 0;
    if (RIM_CURR_DB.db_type == RIM_DB_POSTGRES)
    {
        *er = rim_strdup (rim_pg_error(s));
        if (err_message!=NULL) *err_message=rim_strdup(rim_pg_errm(errm, sizeof(errm), s, sname, lnum, *er));
        if (rim_pg_checkc() != 1) 
        {
            // do not call rim_end_connection as that will free the 'con' structure
            // just signal the connection is bad and the code below will take care of it
            connlost = 1;
        }
    }
    else if (RIM_CURR_DB.db_type == RIM_DB_MARIADB)
    {
        *er = rim_strdup (rim_maria_error(s, is_prep));
        //
        // With MariaDB, rim_maria_checkc() which calls mysql_ping() *will* reset error message, so we have to get it first.
        // This isn't the case with the PostgreSQL and SQLite though, so those are fine regardless
        //
        if (err_message!=NULL) *err_message=rim_strdup(rim_maria_errm(errm, sizeof(errm), s, sname, lnum, *er, is_prep));
        if (rim_maria_checkc() != 1) 
        {
            // do not call rim_end_connection as that will free the 'con' structure
            // just signal the connection is bad and the code below will take care of it
            connlost = 1;
        }
    }
    else if (RIM_CURR_DB.db_type == RIM_DB_SQLITE)
    {
        *er = rim_strdup (rim_lite_error(s, is_prep));
        if (err_message!=NULL) *err_message=rim_strdup(rim_lite_errm(errm, sizeof(errm), s, sname, lnum, *er, is_prep));
        if (rim_lite_checkc() != 1) 
        {
            // do not call rim_end_connection as that will free the 'con' structure
            // just signal the connection is bad and the code below will take care of it
            connlost = 1;
        }
    }
    else
    {
        rim_report_error ("Unknown database type [%ld]", RIM_CURR_DB.db_type); 
    }


    if (connlost == 1)
    {
        if (retry == 1)
        {
            if (rim_retry_db () == 1) return 1; // success in retrying
        }
        // This is only if connection was lost after reconnecting successfully once (so retry is 0, 
        // because we only attempt to reconnect once). In practicality, this is very very unlikely to happen.
        // So -1/lost connection, really never happens. If we cannot reconnect, rimstone will error out the request.
        *er = rim_strdup ("-1"); // connection lost
        char * lostm = "Database connection lost or cannot be (re)established";
        if (err_message!=NULL) *err_message=rim_strdup(lostm); 
    }

    // regardless of what went wrong, report error if on-error set to exit, or return 0 for application to handle
    char reperr = 0;
    // statement-level on-error overrides db level. 
    if (RIM_CURR_DB.exit_on_error == 1) 
    {
        reperr = 1; // db level
        if (erract == RIM_ON_ERROR_CONTINUE) reperr = 0; // override with stmt level
    }
    else if (erract == RIM_ON_ERROR_EXIT) reperr = 1; // override with stmt level
    if (reperr == 1) rim_report_error ("Database operation failed: [%s], error code [%s]", err_message != NULL ? *err_message:errm, *er);
    return 0;
}

//
// Set location of SQL statement. Before it's executed, we set file name (fname),
// line number (lnum) and 'set' is 1. If there was an error, we can use this information
// to point back to the exact line where problem happened.
//
void rim_location (char **fname, rim_num *lnum, rim_num set)
{
    // this static variables are fine, they are used only within a single request. 
    // Before SQL is executed, rim_location is called, and if there is an error, we would
    // return that value - meaning these values are ALWAYS set in the process and THEN used
    static char *fname_loc = "";
    static rim_num lnum_loc = 0;

    if (set == 1)
    {
        fname_loc = *fname;
        lnum_loc = *lnum;
    }
    else
    {
        *fname = fname_loc;
        *lnum = lnum_loc;
    }
} 

// 
// Select SQL. 's' is the text of the SQL and it must start with 'select'. 
//
// Outputs are 'nrow' (the number of rows in the result), 'arow' (affected rows),
// 'ncol' (the number of columns in the result), 'col_names' is a pointer to an array (allocated here) that contains all
// column names with RIM_MAX_QUERY_OUTPUTS being the max size of the array, 'data' (if not NULL) is 
// the buffer allocated by the function that contains all data from the select. 
// er is any error code, errm is any error text
// erract is RIM_ON_ERROR_CONTINUE/EXIT for statement-specific on-error continue/exit or RIM_OKAY if db-level on-error is in effect.
// is_prep is 1 if this is prepared stmt, prep is ptr for prep stmt
// dlen is the length of cols in rows, similar to data
//
// 'data' can be NULL, but col_name cannot be. 
// Each column is present (NULL is the same as empty, or "", we do NOT make a distinction).
// 'data' is an array of pointers, whereby each array entry points to a single column. For example, if a 
// query SELECT X,Y FROM T; selects 3 rows, then data[0]={pointer to  X in 1st row}, data[1]={pointer to
// Y in 1st row}, data[2]={pointer to X in 2nd row}, data[3]={pointer to Y in 2nd row}, data[4]={pointer to
// X in 3rd row}, data[5]={pointer to Y in 3rd row}.
//
// Note that in SQL like INSERT ... RETURNING () we can have both nrow and arow (it's like DML + SELECT in one).
//
void rim_select_table (char *s,
                  rim_num *arow, 
                  rim_num *nrow, 
                  rim_num *ncol, 
                  char ***col_names,
                  char ***data, 
                  rim_num **dlen, 
                  char **er,
                  char **errm,
                  char is_prep,
                  void **prep, 
                  rim_num paramcount, 
                  char **params,
                  char erract)
                  
{

    char *sname = "";
    rim_num lnum = 0;
    // get location in end-user source code where this is called from, for error reporting purposes
    rim_location (&sname, &lnum, 0);

    if (errm!=NULL) *errm=RIM_EMPTY_STRING; // by default error is empty

    // *nrow must be set to 0 if no results, because getting results relies on it. If it's undefined, program will crash
    // trying to get data for undefined number of rows (data that does not even exist).


    // execute SELECT, reconnect if necessary - do not try to get affected rows, we will get it down here
    // because for SELECT, it's not available until after the data is obtained
    if (rim_execute_SQL (s, NULL, er, errm, 1, 1, is_prep, prep, paramcount, params, erract) == NULL)
    {
        *nrow = 0;
        return;
    }
                
    if (RIM_CURR_DB.db_type == RIM_DB_MARIADB)
    {
        if (data != NULL)
        {
            // get all result data
            if (rim_maria_store(is_prep))
            {
                rim_handle_error (s, er, errm, 0, is_prep, erract);
                return;
            }
        }
        else
        {
            // no need to fetch the result yet, since we're getting column names only
            if (rim_maria_use(is_prep))
            {
                rim_handle_error (s, er, errm, 0, is_prep, erract);
                return;
            }
        }
    }

    // get number of columns
    rim_num num_fields = 0;
    if (RIM_CURR_DB.db_type == RIM_DB_MARIADB)
    {
        num_fields = rim_maria_nfield();
    }
    else if (RIM_CURR_DB.db_type == RIM_DB_POSTGRES)
    {
        num_fields = rim_pg_nfield();
    }
    else if (RIM_CURR_DB.db_type == RIM_DB_SQLITE)
    {
        num_fields = rim_lite_nfield();
    }
    else
    {
        rim_report_error ("Unknown database type [%ld]", RIM_CURR_DB.db_type); 
    }

    // get affected rows, which is for non-SELECT mostly, and only available for SELECT-like statement *after* the data is obtained
    // so the value obtained in rim_execute_SQL is not correct for such
    rim_arows(arow, is_prep);


    // number of columns is needed whether we get column names only or result data
    *ncol = num_fields;

    if (col_names != NULL) 
    {
        //
        // allocate column names too, this is for unknown-output only
        //
        *col_names = (char**)rim_calloc (num_fields, sizeof(char*));

        //
        // Get column names to be either used when requested
        //
        rim_num field_index = 0;
        for (field_index = 0; field_index < num_fields; field_index++)
        {
            if (RIM_CURR_DB.db_type == RIM_DB_MARIADB)
            {
                (*col_names)[field_index] = rim_strdup(rim_maria_fieldname());
            }
            else if (RIM_CURR_DB.db_type == RIM_DB_POSTGRES)
            {
                (*col_names)[field_index] = rim_strdup(rim_pg_fieldname(field_index));
            }
            else if (RIM_CURR_DB.db_type == RIM_DB_SQLITE)
            {
                (*col_names)[field_index] = rim_strdup(rim_lite_fieldname());
            }
            else
            {
                rim_report_error ("Unknown database type [%ld]", RIM_CURR_DB.db_type); 
            }
        }
    }

    if (data == NULL)
    {
        *nrow = 0; // no data, zero rows
        // clean the result and return in case we want column names ONLY
        return;
    }

    //
    // this is getting actual data
    //
    if (RIM_CURR_DB.db_type == RIM_DB_MARIADB)
    {
        *nrow = rim_maria_nrows(is_prep);
    }
    else if (RIM_CURR_DB.db_type == RIM_DB_POSTGRES)
    {
        *nrow = rim_pg_nrows();
    }
    else if (RIM_CURR_DB.db_type == RIM_DB_SQLITE)
    {
        *nrow = rim_lite_nrows();
    }
    else
    {
        rim_report_error ("Unknown database type [%ld]", RIM_CURR_DB.db_type); 
    }



// get this many rows to start with, increment by this afterwards
#define RIM_INITIAL_QUERY_BATCH 200

    rim_num query_batch = RIM_INITIAL_QUERY_BATCH;

    // allocate the array where data is
    *data = rim_calloc(query_batch*num_fields, sizeof(char**));
    *dlen = rim_calloc(query_batch*num_fields, sizeof(rim_num*));

    rim_num i;
    rim_num r;


    for (r = 0; r < *nrow; r++) 
    {
        char **row;
        unsigned long *lens;

        if (RIM_CURR_DB.db_type == RIM_DB_MARIADB)
        {
            if (rim_maria_rows(&row, &lens, is_prep ))
            {
                rim_handle_error (s, er, errm, 0, is_prep, erract);
                return;
            }
        }
        else if (RIM_CURR_DB.db_type == RIM_DB_POSTGRES)
        {
            rim_pg_rows(&row, num_fields, r, &lens);
        }
        else if (RIM_CURR_DB.db_type == RIM_DB_SQLITE)
        {
            if (rim_lite_rows(&row, &lens) != 0)
            {
                rim_handle_error (s, er, errm, 0, is_prep, erract);
                return;
            }
        }
        else
        {
            rim_report_error ("Unknown database type [%ld]", RIM_CURR_DB.db_type); 
        }


        // check if buffer is full, if so, allocate another chunk for as long as we need it
        if (r>=query_batch)
        {
            query_batch+=RIM_INITIAL_QUERY_BATCH;
            *data = rim_realloc(rim_mem_get_id(*data), query_batch*num_fields*sizeof(char**));
            *dlen = rim_realloc(rim_mem_get_id(*dlen), query_batch*num_fields*sizeof(rim_num*));
        }

        for(i = 0; i < num_fields; i++)
        {
            // calculate position in data
            rim_num cpos = r * num_fields + i;

            // allocate memory for each column. We don't use row directly even though it'd be a bit
            // faster because memory allocation isn't rim_* -
            // we would have to free the result (mysql_free_result()) at the end, which would be more complex
            // than automatically cleaning up rim_* allocated memory. mysql_free_result() implementation could change
            // and we don't know exactly what's in it. However, we could 'register' result variable as mysql and
            // free it at the end. This is TODO for a speedup in obtaining results.
            //
            // in some cases RimStone allocates storage, such as prepared for mariadb, so no need to rim_malloc and copy
            //
            if (RIM_CURR_DB.need_copy == 1)
            {
                //
                // length for allocated query results is exactly the usage
                //
                (*data)[cpos] = rim_malloc (lens[i]+1);
                // copy data
                memcpy ((*data)[cpos], row[i] ? row[i] : "", lens[i]);
            }
            else
            {
                (*data)[cpos] = row[i]; // row[i] is already rim_malloc-ed
            }
            (*data)[cpos][lens[i]] = 0; // end with zero in any case, even if binary
                                        // wont' hurt
            (*dlen)[cpos] = lens[i];
        }
    }

}





//
// Manage caching of prepared statements. When invoked with prep as NULL, it will set all 
// prepared statements to NULL. This is only done when connection is lost.
// When is not-NULL, it is the address of void * that equals to pointer to prepared statements.
//
void rim_db_prep(void **prep)
{
// list of prepared statement pointers. Must keep it to reset them when connection resets
// as prepared statements are session-only
    typedef struct s_spreps 
    {
        void **p; // ptr to prep stmt
        char db; // which database vendor it belongs to (mariadb, postgres..)
    } spreps;
    static spreps *allpreps = NULL; // all prepared statements
    static rim_num totpreps = 0; // total number of prep stmts
    static rim_num curpreps = 0; // current prep statement
#define RIM_ADD_PREPS 1 /* advance by 1*/
    char *sname = "";
    rim_num lnum = 0;
    rim_location (&sname, &lnum, 0);

    if (prep == NULL)
    {
        // reset all, connection reset
        rim_num i;
        for (i = 0; i <curpreps; i++)
        {
            if (allpreps[i].db == RIM_CURR_DB.db_type) // reset all for the current type of db
            {
                if (RIM_CURR_DB.db_type == RIM_DB_MARIADB) 
                {
                    rim_maria_close_stmt (*(allpreps[i].p));
                }
                else if (RIM_CURR_DB.db_type == RIM_DB_SQLITE) 
                {
                    rim_lite_close_stmt (*(allpreps[i].p));
                }
                else if (RIM_CURR_DB.db_type == RIM_DB_POSTGRES) 
                {
                    rim_pg_close_stmt (*(allpreps[i].p));
                }
                else
                {
                    rim_report_error ("Unknown database type [%ld]", RIM_CURR_DB.db_type); 
                }
                *(allpreps[i].p) = NULL;
            }
        }
        return;
    }

    if (allpreps == NULL) allpreps = (spreps*)rim_malloc0 (sizeof(spreps)*(totpreps=RIM_ADD_PREPS)); // first allocating
    else
    {
        if (curpreps >= totpreps) // if not enough, allocate more
        {
            totpreps += RIM_ADD_PREPS;
            allpreps = (spreps*)realloc (allpreps, sizeof(spreps)*totpreps);
        }
    }
    // add prepared statement of given type
    allpreps[curpreps].p = prep;
    allpreps[curpreps].db = RIM_CURR_DB.db_type;
    curpreps++;
}


//
// Convert '%s' into ? (mariadb), $N (postgres) etc. for true prepared statements and return string
// that is newly allocated, or returns t itself if nothing to be done
//
char *rim_db_prep_text(char *t)
{
    char *f;

    rim_num tot = rim_count_substring (t, _RIM_LOOK_S, sizeof(_RIM_LOOK_S)-1, 0);
    if (tot == 0) return t; // nothing to do if no params

    rim_num len = (rim_num)strlen (t);
    char *n;
    if (RIM_CURR_DB.db_type == RIM_DB_MARIADB || RIM_CURR_DB.db_type == RIM_DB_SQLITE) n = (char*)rim_malloc (len+1); // less for mariadb
    else if (RIM_CURR_DB.db_type == RIM_DB_POSTGRES) n = (char*)rim_malloc (len+ tot*6+1); // allocate as if all are $65535
    else n=rim_malloc(1); // satisfy compiler
    char *orig = n;

    rim_num p = 1;
    while (1) 
    {
        f = strstr (t, "'%s'");
        if (f != NULL)
        {
            memcpy (n, t, f-t);
            if (RIM_CURR_DB.db_type == RIM_DB_MARIADB || RIM_CURR_DB.db_type == RIM_DB_SQLITE)
            {
                n+=f-t;
                *n='?';
                n++;
            }
            else if (RIM_CURR_DB.db_type == RIM_DB_POSTGRES)
            {
                n+=f-t;
                n+=sprintf (n, "$%d", (int)p);
            }
            *n = 0;
            t = f+4;
        } else 
        {
            strcpy (n, t);
            break;
        }
        p++;
        if (p > 65535) break; // no more than that is supported by PG
    }
    return orig;
}


//
// Escape string for inclusion in quoted params. 
// memory for to must be 2*len+1. *len  is the actual encoded length without zero byte counted.
// Returns 0 for okay, 1 otherwise.
//
int rim_db_escape(char *from, char *to, rim_num *len)
{
    // must get connection b/c escaping requires it
    if (RIM_CURR_DB.dbc == NULL) rim_get_db_connection (1);
    if (RIM_CURR_DB.dbc == NULL) rim_report_error ("Cannot get database connection");

    if (RIM_CURR_DB.db_type == RIM_DB_MARIADB) return rim_maria_escape (from, to, len);
    else if (RIM_CURR_DB.db_type == RIM_DB_POSTGRES) return rim_pg_escape (from, to, len);
    else if (RIM_CURR_DB.db_type == RIM_DB_SQLITE) return rim_lite_escape (from, to, len);
    else
    {
        rim_report_error ("Unknown database type [%ld]", RIM_CURR_DB.db_type); 
    }
    return 0;
}



// 
// Run-time construction of SQL statement. 'dest' is the output SQL text, 
// 'num_of_args' is the number of input arguments that follow.
// 'format' which is a string that can only contain '%s' as a format placeholder. The number of inputs
// must match the number of '%s'. This function detects and prevents SQL injection.
// In dynamic queries, the SQL text is a run-time variable, so we don't know the number of '%s' in there ahead of time (i.e. # of inp. params).
// This function makes sure that SQL injection cannot happen by handling single quotes and slashes within strings. 
// The method is not one of binding but of constructing a full SQL text with data in it, but under sterile conditions not conducive to SQL injection.
// The caller code will free dest after SQL executes, so if in a loop,
// it will allocate again and so on.
//
void rim_make_SQL (char **dest, rim_num num_of_args, char *format, ...) 
{
    
    // Double quotes are allowed in format because ANSI_QUOTES  is set at the session's beginning.
    // So double quotes can NOT be used for string literals, ONLY single quotes.
    rim_num num_of_percents;
    rim_num count_percents;
    count_percents = rim_count_substring (format, _RIM_LOOK_S, sizeof(_RIM_LOOK_S)-1, 1);
    num_of_percents = count_percents; // used in a countdown later in this code

    
    // For the format, there has to be an even number of single quotes. An uneven number means
    // faulty query. For example, insert ... values 'xyz', 'aaa  - in this case the last string
    // never ends
    rim_num count_single_quote;
    count_single_quote = rim_count_substring (format, "'", 1, 1);
    if (count_single_quote % 2 != 0)
    {
        rim_report_error ("Unbalanced single quotes in query [%s]", format);
    }

    // Format must be smaller than the destination size to begin with
    rim_num flen = rim_mem_get_len (format);

    // All %s must be quoted, otherwise in select ... where id=%s, it could be made to be
    // select ... where id=2;drop table x; if input parameters is id=2;drop table x;
    rim_num numPlaceWithQuotes = rim_count_substring (format, _RIM_LOOK_S, sizeof(_RIM_LOOK_S)-1, 1);
    if (numPlaceWithQuotes != count_percents)
    {
        rim_report_error ("All arguments in SQL statement must be quoted, including numbers, format [%s], number of arguments [%ld]", format, count_percents);
    }

    // num_of_args isn't the same as count_percents. num_of_args is the total number of input params, some of which may be NULL
    // count_percents is the number of '%s' in SQL statement, which must be equal to # of non-NULL input params (among num_of_args params)

    va_list vl;
    // First, find out the maximum length of all input params
    // and also the maximum length of any of the params
    va_start (vl, format);
    rim_num max_inlen = 0;
    rim_num max_arglen = 0;
    rim_num i;
    char *curr_input;

    for (i = 0; i < num_of_args; i++)
    {
        curr_input = va_arg (vl, char *);
        rim_num alen;
        max_inlen += 2*(alen=rim_mem_get_len (curr_input)); // twice for worst case of each character escaped
        if (alen >= max_arglen) max_arglen = alen;
    }
    max_arglen = 2*max_arglen + 1; // to allow for maximum 
    va_end (vl);

    // restart and rewind variadic arg list
    va_start (vl, format);
    // First, allocate the result (*dest), its length will be adjusted below
    *dest = (char*)rim_malloc (flen + 1 + max_inlen); // includes max possible length, this is sufficient as it doesn't even
                                                      // account for %s which is replaced (so -2 bytes for each param when replacing)
    char sarg[2048]; // use this for argument if max_arglen less than 2K
    char *curr_sql_arg;
    if (max_arglen <= (rim_num)sizeof (sarg)) curr_sql_arg = sarg;
    else curr_sql_arg = (char*)rim_malloc(max_arglen); // allocate max possible argument length, this way done only once
    
    char *curr = *dest;
    char *pholder = NULL;
    char *cformat = format;
    // Add parameters, and copy only format as we go along, ensuring the minimum possible memory copy to produce the final SQL
    for (i = 0; i < num_of_args; i++)
    {
        curr_input = va_arg (vl, char *);

        if (curr_input == NULL) // parameter expected but not found
        {
            rim_report_error ("Input parameters expected in SQL statement [%s] at position [%ld], but not found", format, i+1);
        }

        num_of_percents--;
        // make sure number of non-NULL params isn't greater than what's expected at run-time
        if (num_of_percents < 0)
        {
            rim_report_error ("Too many input parameters in input parameter list for SQL statement [%s], expected [%ld] input parameters", format, count_percents);
        }
        rim_num l_curr_input = rim_mem_get_len (curr_input);

        // Escape: single quote and escape backslash in an single quote string
        // Some parameters might not be quoted, and we will catch that as an error
        // curr_sql_arg is already allocated to hold the max escaped string, so it can never run out of space
        if (rim_db_escape (curr_input, curr_sql_arg, &l_curr_input) != 0)
        {
            va_end (vl);
            rim_report_error ("Argument #%ld cannot be escaped as input parameter [%s], argument [%.100s]", i+1, format, curr_sql_arg);
        }

        // substitute input parameters, one by one
        // but first trim each value
        // We worked with a copy of data here, so no need to put anything back (like the zero character at the end we may put)
        char *val = curr_sql_arg;
        // trim right
        rim_num len_of = l_curr_input; // length is from rim_db_escape
        if (len_of != 0)
        {
            while (len_of!=0 && isspace(*(val+len_of-1))) len_of--;
            *(val+len_of)=0;
        }
        // trim left, val is now trimmed value!
        while (*val!=0 && isspace(*val)) val++;

        pholder = strstr (cformat, "'%s'"); // find the next '%s', cformat is zero delimited like all rimstone strings (including binary ones)
        if (pholder == NULL)
        {
            va_end (vl);
            rim_report_error ("SQL not properly formatted (could not find '%%s' placeholder), text [%s], argument [%.100s]", format, curr_sql_arg);
        }
        memcpy (curr, cformat, pholder - cformat + 1); // + 1 to copy '  so we can start copyig input data
        curr += pholder - cformat + 1; // one passed '
        cformat = pholder + 4; // 3 is strlen("'%s'"), position to one passed trailing ' in '%s'
        memcpy (curr, val, len_of); // copy data
        curr += len_of; // advance data
        *curr = '\''; // add trailing '
        curr++; // get passed ', since cformat is passed it too
    }
    // copy remainder of cformat after last '%s'
    rim_num done_len;
    memcpy (curr, cformat, done_len = (flen - (cformat -format)));  // done_len here is the length of final piece of format
                                                                    // after last '%s'
    curr += done_len;
    *curr = 0; // make sure null terminated, this is the end of *dest
    done_len = curr - *dest; // done_len here is final length of *dest
    *dest = rim_realloc (rim_mem_get_id (*dest), done_len+1); // realloc mem so we don't use more than needed
    rim_mem_set_len (*dest, done_len+1); // set length to be correct

    if (curr_sql_arg != sarg) rim_free_int (curr_sql_arg); // free up argument if we actually alloc'd it

    // make sure number of non-NULL input params isn't lesser than what's expected at run-time
    // num_of_args is # of non-NULL input params in va_arg. # of actual params in va_arg can be different than num_of_percents
    // because va_arg can contain NULLs for dynamic params when add-query-input is used
    if (num_of_percents != 0)
    {
        rim_report_error ("Too few non-NULL input parameters in input parameter list for SQL statement [%s], expected [%ld] non-NULL run-time arguments", format, num_of_percents);
    }

    va_end (vl);
}


//
// Frees result of the query that just executed
// is_prep is 1 if this was prepared statement
//
void rim_db_free_result (char is_prep)
{
    if (RIM_CURR_DB.db_type == RIM_DB_POSTGRES)
    {
        rim_pg_free();
    }
    else if (RIM_CURR_DB.db_type == RIM_DB_MARIADB)
    {
        rim_maria_free();
    }
    else if (RIM_CURR_DB.db_type == RIM_DB_SQLITE)
    {
        rim_lite_free(is_prep);
    }
    else
    {
        rim_report_error ("Unknown database type [%ld]", RIM_CURR_DB.db_type); 
    }
}


