// SPDX-License-Identifier: Apache-2.0
// Copyright 2019 Gliim LLC. 
// Licensed under Apache License v2. See LICENSE file.
// On the web http://golf-lang.com/ - this file is part of Golf framework.

// 
// interface to Postgres
// DB connection is for the life of process - if it goes down, so does
// the process. 
//

#include "golf.h"

//
// Note: we ALWAYS use gg_get_db_connection () explicitly whenever the database
// connection is needed. The reason is that, if db connection drops and can be reestablished,
// we will reconnect. This happens transparently.
//


#define _GG_LOOK_S "'%s'"

// function prototypes
gg_num gg_handle_error (char *s, char **er, char **err_message, gg_num retry, char is_prep, char erract);
void gg_end_connection(gg_num close_db);
gg_num gg_retry_db();
gg_num gg_firstword(char *w, char *s);
void gg_arows(gg_num *arows, char is_prep);

int gg_stmt_cached = 0;


// Close database connection. This is when database connection was already detected lost, and we need to 
// match that and in no other situation. This is to end the connection regardless of which database it is.
// If close_db is 1, then close db-specific connection. In some cases, when connection fails in the first 
// place, the connection doesn't exist, so closing it is questionable.
// We NEVER close connection in our code, EXCEPT on shutdown of command line program.
void gg_end_connection(gg_num close_db) {
    GG_TRACE("");
    if (GG_CURR_DB.dbc != NULL) 
    {
        if (close_db == 1) 
        {
            if (GG_CURR_DB.db_type == GG_DB_POSTGRES)
            {
                gg_pg_close();
            }
            if (GG_CURR_DB.db_type == GG_DB_MARIADB)
            {
                gg_maria_close ();
            } 
            if (GG_CURR_DB.db_type == GG_DB_SQLITE)
            {
                gg_lite_close ();
            } 
        }

        free (GG_CURR_DB.dbc); // was created by malloc() and not gg_malloc, so free must be used
        GG_CURR_DB.dbc = NULL;
        GG_CURR_DB.is_begin_transaction = 0; // we are no longer in a transaction if
                    // connection closed
    }
}

// 
// Get database connection. Caches connection and connects ONE TIME only. 
// We use a db connection "for-ever", i.e. as long as possible.
// Returns connection data pointer, or NULL if failed to connect.
// 
gg_dbc *gg_get_db_connection (gg_num abort_if_bad)
{
    GG_TRACE("");

    // this index is used in GG_CURR_DB
    if (gg_get_config()->ctx.db->ind_current_db == -1) 
    {
        // this shouldn't happen, as all Golf db ops are meant to have db connection. A bug if it gets here.
        gg_report_error ("No active database in use, yet database operation attempted");
    }

    //
    // ...db.ind_current_db is ALWAYS passed to here as the database to use. The number is predetermined
    // at compile time and each database name has a fixed slot in GG_CTX.db[] array.
    //

    if (GG_CURR_DB.dbc != NULL) 
    {
        GG_TRACE ("using cached db connection");
        return GG_CURR_DB.dbc; 
            // already connected
    }
    
    // 
    // One process has ONLY one connection. We can reconnect ONLY if we were NOT in a transaction. 
    // 'Reconnecting' means 'has_connected' is 1, and we're trying to connect again.
    // We cannot reconnect if we are in a transaction, because the database would have automatically
    // rollbacked on disconnect (that happened prior to this) and it would be complicated for an application to recover from this.
    // If we try to connect a second time in this case, error out.
    //
    if (GG_CURR_DB.has_connected==1)
    {
        if (GG_CURR_DB.is_begin_transaction ==1) 
        {
            //
            // If we're in a transaction, we have to exit as we cannot recover from a rollback of a
            // transaction initiated by the application. If however, we are not in a transaction,
            // we can safely establish a new connection and let application continue with a new
            // transaction or a single-statement transaction.
            //
            gg_report_error ("The connection to database has been lost"); 
        }
        //
        // Here we are if we're not in a transaction
        //
    }


    if (GG_CURR_DB.db_type == GG_DB_POSTGRES)
    {
        if (gg_pg_connect (abort_if_bad) == NULL) return NULL;
    }
    else if (GG_CURR_DB.db_type == GG_DB_MARIADB)
    {
        if (gg_maria_connect (abort_if_bad) == NULL) return NULL;
    }
    else if (GG_CURR_DB.db_type == GG_DB_SQLITE)
    {
        if (gg_lite_connect (abort_if_bad) == NULL) return NULL;
    }
    else
    {
        gg_report_error ("Unknown database type [%ld]", GG_CURR_DB.db_type); 
    }

    GG_CURR_DB.has_connected = 1;
    // 
    // The following (due to above guard) is executed ONLY ONCE for the life of process
    //
    GG_CURR_DB.is_begin_transaction = 0; // we are no longer in a transaction if
                        // connection is being (re)opened, this is implicit rollback

    return GG_CURR_DB.dbc;
}

// We NEVER close connection in our code, EXCEPT on shutdown of command line program.
             
// 
// Begin transaction. 
// Returns 1 if okay, 0 if failed. It may just stop and report error, depending on on-error settings (both
// erract is GG_ON_ERROR_CONTINUE/EXIT for statement-specific on-error continue/exit or GG_OKAY if db-level on-error is in effect.
// err is the error code of any error, errt is error text - both are NULL if none requested.
//
gg_num gg_begin_transaction(char *t, char erract, char **err, char **errt)
{
    GG_TRACE("");

    char *er;
    gg_num rows;
    char *errm="";
    char start[512];
    if (GG_CURR_DB.db_type == GG_DB_SQLITE)
    {
        snprintf (start, sizeof(start), "begin %s", t);
    } 
    else 
    {   
        snprintf (start, sizeof(start), "start transaction %s", t);
    }
// last param is GG_ON_ERROR_CONTINUE/EXIT for statement-specific on-error continue/exit or GG_OKAY if db-level on-error is in effect.
    if (gg_execute_SQL (start, &rows, &er, &errm, 0, 0, 0, NULL, 0, NULL, erract) == NULL)
    {
        GG_CURR_DB.is_begin_transaction = 0;
        return 0;
    }
    if (err != NULL) *err = er; else gg_free((void*)er);
    if (errt != NULL) *errt = errm; else gg_free((void*)errm);
    GG_CURR_DB.is_begin_transaction = 1;
    return 1;
}

// 
// Close connection for all dbs and release resources. Done only just before exit.
//
void gg_end_all_db()
{
    GG_TRACE("");
    gg_num i;
    for (i = 0; i < gg_get_config()->ctx.tot_dbs; i++)
    {
        gg_get_config()->ctx.db->ind_current_db = i;
        gg_end_connection (1);
    }
}

// 
// Check if any transaction is opened, for any db. If so, rollback them. If check_mode is 1, return.
// If check_mode is anything but 1, error out.
// If transaction is not opened, return regardless.
//
void gg_check_transaction(gg_num check_mode)
{
    GG_TRACE("");
    gg_num savedb = gg_get_config()->ctx.db->ind_current_db;
    gg_num i;
    gg_num err = 0;
    gg_num errdb = -1;
    for (i = 0; i < gg_get_config()->ctx.tot_dbs; i++)
    {
        if (gg_get_config()->ctx.db->conn[i].is_begin_transaction == 1) 
        {
            GG_TRACE("Transaction rollbacked (check mode %ld)", check_mode);
            errdb = i;
            gg_get_config()->ctx.db->ind_current_db = i;
            gg_rollback("", GG_OKAY, NULL, NULL);
            gg_get_config()->ctx.db->ind_current_db = savedb;
            err =1;
        }
    }
    if (err == 1)
    {
        if (check_mode==1) return;
        gg_report_error ("Started transaction database [%s], but was never committed or rollbacked", gg_get_config()->ctx.db->conn[errdb].db_name);
    }
}

// 
// Commit transaction. 
// Returns 1 if okay, 0 if failed. It may just stop and report error, depending on on-error settings (both
// erract is GG_ON_ERROR_CONTINUE/EXIT for statement-specific on-error continue/exit or GG_OKAY if db-level on-error is in effect.
// err is the error code of any error, errt is error text - both are NULL if none requested.
//
gg_num gg_commit(char *t, char erract, char **err, char **errt)
{
    GG_TRACE("");

    GG_CURR_DB.is_begin_transaction = 0;

    char *er;
    gg_num rows;
    char *errm="";
    char comm[512];
    snprintf (comm, sizeof(comm), "commit %s", t);
// last param is GG_ON_ERROR_CONTINUE/EXIT for statement-specific on-error continue/exit or GG_OKAY if db-level on-error is in effect.
    if (gg_execute_SQL (comm, &rows, &er, &errm, 0, 0, 0, NULL, 0, NULL, erract) == NULL)
    {
        return 0;
    }
    if (err != NULL) *err = er; else gg_free((void*)er);
    if (errt != NULL) *errt = errm; else gg_free((void*)errm);
    return 1;
}

// 
// Rollbacks the transaction. 
// Returns 1 if okay, 0 if failed. It may just stop and report error, depending on on-error settings (both
// global and per-statement).
// erract is GG_ON_ERROR_CONTINUE/EXIT for statement-specific on-error continue/exit or GG_OKAY if db-level on-error is in effect.
// err is the error code of any error, errt is error text - both are NULL if none requested.
//
gg_num gg_rollback(char *t, char erract, char **err, char **errt)
{
    GG_TRACE("");

    GG_CURR_DB.is_begin_transaction = 0;

    char *er;
    gg_num rows;
    char *errm="";
    char rollb[512];
    snprintf (rollb, sizeof(rollb), "rollback %s", t);
// last param is GG_ON_ERROR_CONTINUE/EXIT for statement-specific on-error continue/exit or GG_OKAY if db-level on-error is in effect.
    if (gg_execute_SQL (rollb, &rows, &er, &errm, 0, 0, 0, NULL, 0, NULL, erract) == NULL)
    {
        return 0;
    }
    if (err != NULL) *err = er; else gg_free((void*)er);
    if (errt != NULL) *errt = errm; else gg_free((void*)errm);
    return 1;
}

//
// Return 1 if the first word of 's' is 'w', otherwise 0. If 's' is quoted, ignore quotes.
// Also, ignore leading spaces.
//
gg_num gg_firstword(char *w, char *s)
{
    GG_TRACE("");
    gg_num l = strlen (w);
    char *mbeg = s;
    // avoid spaces and quotes in the beginning (could be spaces, quote, spaces again)
    while (isspace(*mbeg) || *mbeg == '"') mbeg++; // right now, always quoted, get passed the quote
    if (!strncasecmp (mbeg, (w), l) && (mbeg[l] == 0 || mbeg[l] == '"' || isspace(mbeg[l]))) return 1; else return 0;
}

//
// Get the number of rows affected in arows
//
void gg_arows(gg_num *arows, char is_prep)
{
    GG_TRACE("");
    if (arows!= NULL)
    {
        if (GG_CURR_DB.db_type == GG_DB_POSTGRES)
        {
            *arows = gg_pg_affected();
        }
        else if (GG_CURR_DB.db_type == GG_DB_MARIADB)
        {
            *arows = gg_maria_affected(is_prep);
        }
        else if (GG_CURR_DB.db_type == GG_DB_SQLITE)
        {
            *arows = gg_lite_affected(is_prep);
        }
        else
        {
            gg_report_error ("Unknown database type [%ld]", GG_CURR_DB.db_type); 
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
// erract is GG_ON_ERROR_CONTINUE/EXIT for statement-specific on-error continue/exit or GG_OKAY if db-level on-error is in effect.
// returns_tuple is 1 if the SQL returns tuple, such as SELECT. It could be also SQL liks INSERT..RETURNING() so not
// necessarily DML vs SELECT.
//
gg_dbc *gg_execute_SQL (char *s,  gg_num *arows, char **er, char **err_message, gg_num returns_tuple, gg_num user_check, char is_prep, void **prep, gg_num paramcount, char **params, char erract)
{
    GG_TRACE("");
    assert (s);
    assert (er);

    // get location in source code (if set, GLIIMLY automatically does this)
    char *sname = "";
    gg_num lnum = 0; 
    gg_location (&sname, &lnum, 0);
    if (s[0] == 0)
    {
        gg_report_error ( "Query cannot be empty, reading file [%s] at line [%ld]", sname, lnum);
    }

    // s (from the end-user) cannot be BEGIN/START, nor COMMIT/ROLLBACK - if it were, we couldn't control reconnection.
    // If connection to db got lost in the middle of transaction, and we weren't aware, whatever
    // happened so far would be either rolled back or committed (depending on the db setting)
    // and the rest would also be independently rolled back or committed - because transaction
    // is tied to a *connection* and we have two of those here.
    // Note that s from Golf (as in from gg_begin_transaction) is OKAY
    if (user_check == 1)
    {
        if (gg_firstword("BEGIN", s) || gg_firstword("START", s) || gg_firstword("COMMIT", s) || gg_firstword("ROLLBACK", s))
        {
            gg_report_error ( "Use Golf begin-transaction, commit-transaction or rollback-transaction instead of direct database SQL for these statements, reading file [%s] at line [%ld]", sname, lnum);
        }
    }

    gg_dbc *dbc = gg_get_db_connection (1);
    
    GG_TRACE ("Query executing: [%s]", s);

    // init error code/message
    char *oker =  gg_strdup("0"); // success for error in query-result#...,error...
    *er = oker;
    *err_message = GG_EMPTY_STRING;

    gg_num execres = 0;
    if (GG_CURR_DB.db_type == GG_DB_POSTGRES)
    {   
        execres = gg_pg_exec (s, returns_tuple, is_prep, prep, paramcount, params);
    }
    else if (GG_CURR_DB.db_type == GG_DB_MARIADB)
    {   
        execres = gg_maria_exec (s, is_prep, prep, paramcount, params);
    }
    else if (GG_CURR_DB.db_type == GG_DB_SQLITE)
    {   
        execres = gg_lite_exec (s, is_prep, prep, paramcount, params);
    }
    else
    {
        gg_report_error ("Unknown database type [%ld]", GG_CURR_DB.db_type); 
    }

    if (execres)
    {
        //
        // Error in execution. If we're not in a transaction, try to reconnect ONCE.
        // If we ARE in a transaction, we CANNOT reconnect.
        // By transaction, I mean usage of Golf begin-transaction - no other kind is allowed.
        //
        if (GG_CURR_DB.is_begin_transaction == 0)
        {
            //
            // While handling error, if error is unrecoverable (or on-error is set to exit), gg_handle_error
            // will stop the program. If error can be reported back to the application,
            // then  we will set arows to 0 and return 0.
            //
            // Since we're NOT in transaction here, try to reconnect ONCE if the error is 'lost
            // connection' or such. The 4th parameter of 1 means that only in case of such errors,
            // try to reconnect. If the error is not lost connection, then we will report it
            // back by setting arows to 0 and returning 0.
            //
            if (gg_handle_error (s, er, err_message, 1, is_prep, erract) == 0)
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
                if (GG_CURR_DB.db_type == GG_DB_POSTGRES)
                {   
                    execres = gg_pg_exec (s, returns_tuple, is_prep, prep, paramcount, params);
                }
                else if(GG_CURR_DB.db_type == GG_DB_MARIADB)
                {   
                    execres = gg_maria_exec (s, is_prep, prep, paramcount, params);
                }
                else if(GG_CURR_DB.db_type == GG_DB_SQLITE)
                {   
                    execres = gg_lite_exec (s, is_prep, prep, paramcount, params);
                }
                else
                {
                    gg_report_error ("Unknown database type [%ld]", GG_CURR_DB.db_type); 
                }

                if (execres)
                {
                    //
                    // There was still an error, and this time we will NOT try reconnecting. 
                    // If case of any error, including lost connection,report to application.
                    // We don't check return value, because IF gg_handle_error returns, it has to be 0 in this case,
                    // because retry is set to 0.
                    //
                    gg_handle_error (s, er, err_message, 0, is_prep, erract);
                    if (arows!= NULL) *arows = 0;
                    return NULL;
                }
                else
                {
                    // 
                    // SQL statement executed correctly after reconnecting. Just proceed
                    //
                    GG_TRACE("SQL statement executed OKAY after reconnecting to database.");
                    *er = oker; // clear previous error, make it okay
                }
            }
        }
        else
        {
            //
            // This is if we're in a transaction. Do NOT try to reconnect - if we lost connection, consider it an error.
            // Otherwise, report error back to the application.
            // We don't check return value, because IF gg_handle_error returns, it has to be 0.
            //
            gg_handle_error (s, er, err_message, 0, is_prep, erract);
            if (arows!= NULL)  *arows = 0;
            return NULL;
        }
    }

    //
    // Getting rows-affected Not done here for SELECT (see gg_select_table)
    //
    gg_arows(arows, is_prep);
    GG_TRACE("Query OK, affected rows [%ld] - incorrect for SELECT, see further for that.", arows==NULL?0:*arows);

    return dbc;
}

// when error is encountered, retry to connect (assuming we're not in a transaction, which is checked in the caller) 
// if ok then return 1, otherwise return 0. 
gg_num gg_retry_db()
{
    GG_TRACE("");
    //
    // Retry connection
    //
    //
    // First we must reset connection we have already. It's lost so we're not calling mysql_close().
    // Without this, an attempt to retry connection would not work, and would just return the old cached bad
    // connection
    //
    gg_end_connection(1);
    //
    // Retry it
    //
    if (gg_get_db_connection(1) != NULL)
    {
        //
        // Retry was instructed and successful
        //
        GG_TRACE("Reconnecting to database OKAY");
        return 1;
    }
    else
    {
        GG_TRACE("Could not reconnect to database");
        return 0;
    }
}

//
// Handle error of execution of SQL. 's' is the statement. 'con' is the db connection.
// 'er' is the output error, and its text is in output variable err_message which can be NULL in which case none is output.
// 'retry' has any meaning ONLY if connection was actually lost. Only if 'retry' is 1 AND this is a lost connection, try to 
// reconnect ONCE; if retry is 0, or couldn't reconnect, then return to application with -1 error.
// erract is GG_ON_ERROR_CONTINUE/EXIT for statement-specific on-error continue/exit or GG_OKAY if db-level on-error is in effect.
// If this is something other than lost connection, then handle it by returning 0. 
// erract is GG_ON_ERROR_CONTINUE/EXIT for statement-specific on-error continue/exit or GG_OKAY if db-level on-error is in effect.
// Returns 1 if reconnect was successful (meaning retry had to be 1 for this to happen), 0 in any other case.
// If on-error is set to exit, then stop program in any case but successfull reconnection.
//
gg_num gg_handle_error (char *s, char **er, char **err_message, gg_num retry, char is_prep, char erract)
{
    GG_TRACE("");
    // This static is fine - it is used only within a single request, i.e. it doesn't span multiple request.
    // Errm is set HERE and used right after it - it doesn't go beyond a request.
    static char errm[8192];
    assert (s != NULL);
    assert (er != NULL);

    *er = GG_EMPTY_STRING;
    if (err_message!=NULL) *err_message=GG_EMPTY_STRING;

    gg_num connlost = 0;
    char * local_error = "";
    if (GG_CURR_DB.db_type == GG_DB_POSTGRES)
    {
        local_error = gg_pg_error(s);
        if (gg_pg_checkc() != 1) 
        {
            // do not call gg_end_connection as that will free the 'con' structure
            // just signal the connection is bad and the code below will take care of it
            connlost = 1;
        }
    }
    else if (GG_CURR_DB.db_type == GG_DB_MARIADB)
    {
        local_error = gg_maria_error(s, is_prep);
        if (gg_maria_checkc() != 1) 
        {
            // do not call gg_end_connection as that will free the 'con' structure
            // just signal the connection is bad and the code below will take care of it
            connlost = 1;
        }
    }
    else if (GG_CURR_DB.db_type == GG_DB_SQLITE)
    {
        local_error = gg_lite_error(s, is_prep);
        if (gg_lite_checkc() != 1) 
        {
            // do not call gg_end_connection as that will free the 'con' structure
            // just signal the connection is bad and the code below will take care of it
            connlost = 1;
        }
    }
    else
    {
        gg_report_error ("Unknown database type [%ld]", GG_CURR_DB.db_type); 
    }

    // get location in source code (if set, GLIIMLY automatically does this)
    char *sname = "";
    gg_num lnum = 0;
    gg_location (&sname, &lnum, 0);

    if (connlost == 1)
    {
        if (retry == 1)
        {
            if (gg_retry_db () == 1) return 1; // success in retrying
        }
        // This is only if connection was lost after reconnecting successfully once (so retry is 0, 
        // because we only attempt to reconnect once). In practicality, this is very very unlikely to happen.
        // So -1/lost connection, really never happens. If we cannot reconnect, golf will error out the request.
        *er = gg_strdup ("-1"); // connection lost
        char * lostm = "Database connection lost or cannot be (re)established";
        if (err_message!=NULL) *err_message=gg_strdup(lostm); else GG_TRACE ("%s", lostm);
    }
    else 
    {
        // Probably a fatal error, application must handle
        *er = gg_strdup (local_error);
        if (GG_CURR_DB.db_type == GG_DB_POSTGRES)
        {
            if (err_message!=NULL) *err_message=gg_strdup(gg_pg_errm(errm, sizeof(errm), s, sname, lnum, *er));
            else GG_TRACE("%s", errm);
        }
        else if (GG_CURR_DB.db_type == GG_DB_MARIADB)
        {
            if (err_message!=NULL) *err_message=gg_strdup(gg_maria_errm(errm, sizeof(errm), s, sname, lnum, *er, is_prep));
            else GG_TRACE("%s", errm);
        }
        else if (GG_CURR_DB.db_type == GG_DB_SQLITE)
        {
            if (err_message!=NULL) *err_message=gg_strdup(gg_lite_errm(errm, sizeof(errm), s, sname, lnum, *er, is_prep));
            else GG_TRACE("%s", errm);
        }
        else
        {
            gg_report_error ("Unknown database type [%ld]", GG_CURR_DB.db_type); 
        }
        GG_TRACE("%s", errm);
    }

    // regardless of what went wrong, report error if on-error set to exit, or return 0 for application to handle
    char reperr = 0;
    // statement-level on-error overrides db level. 
    if (GG_CURR_DB.exit_on_error == 1) 
    {
        reperr = 1; // db level
        if (erract == GG_ON_ERROR_CONTINUE) reperr = 0; // override with stmt level
    }
    else if (erract == GG_ON_ERROR_EXIT) reperr = 1; // override with stmt level
    if (reperr == 1) gg_report_error ("Database operation failed: [%s], error code [%s]", err_message != NULL ? *err_message:errm, *er);
    return 0;
}

//
// Set location of SQL statement. Before it's executed, we set file name (fname),
// line number (lnum) and 'set' is 1. If there was an error, we can use this information
// to point back to the exact line where problem happened.
//
void gg_location (char **fname, gg_num *lnum, gg_num set)
{
    GG_TRACE("");
    // this static variables are fine, they are used only within a single request. 
    // Before SQL is executed, gg_location is called, and if there is an error, we would
    // return that value - meaning these values are ALWAYS set in the process and THEN used
    static char *fname_loc = "";
    static gg_num lnum_loc = 0;

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
// column names with GG_MAX_QUERY_OUTPUTS being the max size of the array, 'data' (if not NULL) is 
// the buffer allocated by the function that contains all data from the select. 
// er is any error code, errm is any error text
// erract is GG_ON_ERROR_CONTINUE/EXIT for statement-specific on-error continue/exit or GG_OKAY if db-level on-error is in effect.
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
void gg_select_table (char *s,
                  gg_num *arow, 
                  gg_num *nrow, 
                  gg_num *ncol, 
                  char ***col_names,
                  char ***data, 
                  gg_num **dlen, 
                  char **er,
                  char **errm,
                  char is_prep,
                  void **prep, 
                  gg_num paramcount, 
                  char **params,
                  char erract)
                  
{
    GG_TRACE("");
    assert (nrow);
    assert (ncol);
    assert (s);
    assert (er);
    assert (errm);

    char *sname = "";
    gg_num lnum = 0;
    // get location in end-user source code where this is called from, for error reporting purposes
    gg_location (&sname, &lnum, 0);

    if (errm!=NULL) *errm=GG_EMPTY_STRING; // by default error is empty

    // *nrow must be set to 0 if no results, because getting results relies on it. If it's undefined, program will crash
    // trying to get data for undefined number of rows (data that does not even exist).


    // execute SELECT, reconnect if necessary - do not try to get affected rows, we will get it down here
    // because for SELECT, it's not available until after the data is obtained
    if (gg_execute_SQL (s, NULL, er, errm, 1, 1, is_prep, prep, paramcount, params, erract) == NULL)
    {
        *nrow = 0;
        GG_TRACE ("Cannot perform select, error [%s], error summary: [%s], line [%ld], file [%s]", *er, *errm, lnum,sname);
        return;
    }
                
    if (GG_CURR_DB.db_type == GG_DB_MARIADB)
    {
        if (data != NULL)
        {
            // get all result data
            if (gg_maria_store(is_prep))
            {
                gg_handle_error (s, er, errm, 0, is_prep, erract);
                GG_TRACE ("Cannot store select results, error [%s], error summary: [%s], line [%ld], file [%s]", *er, *errm, lnum,sname);
                return;
            }
        }
        else
        {
            // no need to fetch the result yet, since we're getting column names only
            if (gg_maria_use(is_prep))
            {
                gg_handle_error (s, er, errm, 0, is_prep, erract);
                GG_TRACE ("Cannot use select results, error [%s], error summary: [%s], line [%ld], file [%s]", *er, *errm, lnum,sname);
                return;
            }
        }
    }

    // get number of columns
    gg_num num_fields = 0;
    if (GG_CURR_DB.db_type == GG_DB_MARIADB)
    {
        num_fields = gg_maria_nfield();
    }
    else if (GG_CURR_DB.db_type == GG_DB_POSTGRES)
    {
        num_fields = gg_pg_nfield();
    }
    else if (GG_CURR_DB.db_type == GG_DB_SQLITE)
    {
        num_fields = gg_lite_nfield();
    }
    else
    {
        gg_report_error ("Unknown database type [%ld]", GG_CURR_DB.db_type); 
    }

    // get affected rows, which is for non-SELECT mostly, and only available for SELECT-like statement *after* the data is obtained
    // so the value obtained in gg_execute_SQL is not correct for such
    gg_arows(arow, is_prep);


    // number of columns is needed whether we get column names only or result data
    *ncol = num_fields;

    if (col_names != NULL) 
    {
        //
        // allocate column names too, this is for unknown-output only
        //
        *col_names = (char**)gg_calloc (num_fields, sizeof(char*));

        //
        // Get column names to be either used when requested
        //
        gg_num field_index = 0;
        for (field_index = 0; field_index < num_fields; field_index++)
        {
            if (GG_CURR_DB.db_type == GG_DB_MARIADB)
            {
                (*col_names)[field_index] = gg_strdup(gg_maria_fieldname(is_prep));
            }
            else if (GG_CURR_DB.db_type == GG_DB_POSTGRES)
            {
                (*col_names)[field_index] = gg_strdup(gg_pg_fieldname(field_index));
            }
            else if (GG_CURR_DB.db_type == GG_DB_SQLITE)
            {
                (*col_names)[field_index] = gg_strdup(gg_lite_fieldname(field_index));
            }
            else
            {
                gg_report_error ("Unknown database type [%ld]", GG_CURR_DB.db_type); 
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
    if (GG_CURR_DB.db_type == GG_DB_MARIADB)
    {
        *nrow = gg_maria_nrows(is_prep);
    }
    else if (GG_CURR_DB.db_type == GG_DB_POSTGRES)
    {
        *nrow = gg_pg_nrows();
    }
    else if (GG_CURR_DB.db_type == GG_DB_SQLITE)
    {
        *nrow = gg_lite_nrows();
    }
    else
    {
        gg_report_error ("Unknown database type [%ld]", GG_CURR_DB.db_type); 
    }



// get this many rows to start with, increment by this afterwards
#define GG_INITIAL_QUERY_BATCH 200

    gg_num query_batch = GG_INITIAL_QUERY_BATCH;

    // allocate the array where data is
    *data = gg_calloc(query_batch*num_fields, sizeof(char**));
    *dlen = gg_calloc(query_batch*num_fields, sizeof(gg_num*));

    gg_num i;
    gg_num r;


    for (r = 0; r < *nrow; r++) 
    {
        char **row;
        unsigned long *lens;

        if (GG_CURR_DB.db_type == GG_DB_MARIADB)
        {
            if (gg_maria_rows(&row, &lens, is_prep ))
            {
                gg_handle_error (s, er, errm, 0, is_prep, erract);
                GG_TRACE ("Cannot get result rows, error [%s], error summary: [%s], line [%ld], file [%s]", *er, *errm, lnum,sname);
                return;
            }
        }
        else if (GG_CURR_DB.db_type == GG_DB_POSTGRES)
        {
            gg_pg_rows(&row, num_fields, r, &lens);
        }
        else if (GG_CURR_DB.db_type == GG_DB_SQLITE)
        {
            if (gg_lite_rows(&row, &lens) != 0)
            {
                gg_handle_error (s, er, errm, 0, is_prep, erract);
                GG_TRACE ("Cannot get result rows, error [%s], error summary: [%s], line [%ld], file [%s]", *er, *errm, lnum,sname);
                return;
            }
        }
        else
        {
            gg_report_error ("Unknown database type [%ld]", GG_CURR_DB.db_type); 
        }


        // check if buffer is full, if so, allocate another chunk for as long as we need it
        if (r>=query_batch)
        {
            query_batch+=GG_INITIAL_QUERY_BATCH;
            *data = gg_realloc(gg_mem_get_id(*data), query_batch*num_fields*sizeof(char**));
            *dlen = gg_realloc(gg_mem_get_id(*dlen), query_batch*num_fields*sizeof(gg_num*));
        }

        for(i = 0; i < num_fields; i++)
        {
            // calculate position in data
            gg_num cpos = r * num_fields + i;

            // allocate memory for each column. We don't use row directly even though it'd be a bit
            // faster because memory allocation isn't gg_* -
            // we would have to free the result (mysql_free_result()) at the end, which would be more complex
            // than automatically cleaning up gg_* allocated memory. mysql_free_result() implementation could change
            // and we don't know exactly what's in it. However, we could 'register' result variable as mysql and
            // free it at the end. This is TODO for a speedup in obtaining results.
            //
            // in some cases Golf allocates storage, such as prepared for mariadb, so no need to gg_malloc and copy
            //
            if (GG_CURR_DB.need_copy == 1)
            {
                //
                // length for allocated query results is exactly the usage
                //
                (*data)[cpos] = gg_malloc (lens[i]+1);
                // copy data
                memcpy ((*data)[cpos], row[i] ? row[i] : "", lens[i]);
            }
            else
            {
                (*data)[cpos] = row[i]; // row[i] is already gg_malloc-ed
            }
            (*data)[cpos][lens[i]] = 0; // end with zero in any case, even if binary
                                        // wont' hurt
            (*dlen)[cpos] = lens[i];
        }
    }

    GG_TRACE("SELECT retrieved [%ld] rows", *nrow);
}





//
// Manage caching of prepared statements. When invoked with prep as NULL, it will set all 
// prepared statements to NULL. This is only done when connection is lost.
// When is not-NULL, it is the address of void * that equals to pointer to prepared statements.
//
void gg_db_prep(void **prep)
{
    GG_TRACE("");
// list of prepared statement pointers. Must keep it to reset them when connection resets
// as prepared statements are session-only
    typedef struct s_spreps 
    {
        void **p; // ptr to prep stmt
        char db; // which database vendor it belongs to (mariadb, postgres..)
    } spreps;
    static spreps *allpreps = NULL; // all prepared statements
    static gg_num totpreps = 0; // total number of prep stmts
    static gg_num curpreps = 0; // current prep statement
#define GG_ADD_PREPS 1 /* advance by 1*/
    char *sname = "";
    gg_num lnum = 0;
    gg_location (&sname, &lnum, 0);

    if (prep == NULL)
    {
        // reset all, connection reset
        gg_num i;
        for (i = 0; i <curpreps; i++)
        {
            if (allpreps[i].db == GG_CURR_DB.db_type) // reset all for the current type of db
            {
                GG_TRACE ("closing prepared statement");
                if (GG_CURR_DB.db_type == GG_DB_MARIADB) 
                {
                    gg_maria_close_stmt (*(allpreps[i].p));
                }
                else if (GG_CURR_DB.db_type == GG_DB_SQLITE) 
                {
                    gg_lite_close_stmt (*(allpreps[i].p));
                }
                else if (GG_CURR_DB.db_type == GG_DB_POSTGRES) 
                {
                    gg_pg_close_stmt (*(allpreps[i].p));
                }
                else
                {
                    gg_report_error ("Unknown database type [%ld]", GG_CURR_DB.db_type); 
                }
                *(allpreps[i].p) = NULL;
            }
        }
        return;
    }

    if (allpreps == NULL) allpreps = (spreps*)malloc (sizeof(spreps)*(totpreps=GG_ADD_PREPS)); // first allocating
    else
    {
        if (curpreps >= totpreps) // if not enough, allocate more
        {
            totpreps += GG_ADD_PREPS;
            allpreps = (spreps*)realloc (allpreps, sizeof(spreps)*totpreps);
        }
    }
    if (allpreps == NULL)
    {
        gg_report_error ("Out of memory for prepared statement list, line [%ld], file [%s]", lnum, sname);
    }
    // add prepared statement of given type
    allpreps[curpreps].p = prep;
    allpreps[curpreps].db = GG_CURR_DB.db_type;
    curpreps++;
}


//
// Convert '%s' into ? (mariadb), $N (postgres) etc. for true prepared statements and return string
// that is newly allocated, or returns t itself if nothing to be done
//
char *gg_db_prep_text(char *t)
{
    GG_TRACE("");
    char *f;

    gg_num tot = gg_count_substring (t, _GG_LOOK_S, sizeof(_GG_LOOK_S)-1, 0);
    if (tot == 0) return t; // nothing to do if no params

    gg_num len = (gg_num)strlen (t);
    char *n;
    if (GG_CURR_DB.db_type == GG_DB_MARIADB || GG_CURR_DB.db_type == GG_DB_SQLITE) n = (char*)gg_malloc (len+1); // less for mariadb
    else if (GG_CURR_DB.db_type == GG_DB_POSTGRES) n = (char*)gg_malloc (len+ tot*6+1); // allocate as if all are $65535
    else n=gg_malloc(1); // satisfy compiler
    char *orig = n;

    gg_num p = 1;
    while (1) 
    {
        f = strstr (t, "'%s'");
        if (f != NULL)
        {
            memcpy (n, t, f-t);
            if (GG_CURR_DB.db_type == GG_DB_MARIADB || GG_CURR_DB.db_type == GG_DB_SQLITE)
            {
                n+=f-t;
                *n='?';
                n++;
            }
            else if (GG_CURR_DB.db_type == GG_DB_POSTGRES)
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
int gg_db_escape(char *from, char *to, gg_num *len)
{
    GG_TRACE("");
    // must get connection b/c escaping requires it
    if (GG_CURR_DB.dbc == NULL) gg_get_db_connection (1);
    if (GG_CURR_DB.dbc == NULL) gg_report_error ("Cannot get database connection");

    if (GG_CURR_DB.db_type == GG_DB_MARIADB) return gg_maria_escape (from, to, len);
    else if (GG_CURR_DB.db_type == GG_DB_POSTGRES) return gg_pg_escape (from, to, len);
    else if (GG_CURR_DB.db_type == GG_DB_SQLITE) return gg_lite_escape (from, to, len);
    else
    {
        gg_report_error ("Unknown database type [%ld]", GG_CURR_DB.db_type); 
    }
    return 0;
}



// 
// Run-time construction of SQL statement. 'dest' is the output SQL text, destSize is the size of
// memory currently available for SQL text in 'dest'. 'num_of_args' is the number of input arguments that follow
// 'format' which is a string that can only contain '%s' as a format placeholder. The input arguments following
// 'format' can be NULL or otherwise (if NULL it's empty string).
// In dynamic queries, the SQL text is a run-time variable, so we don't know the number of '%s' in there ahead of time (i.e. # of inp. params).
// This function makes sure that SQL injection cannot happen by handling single quotes and slashes within strings. 
// We will also adjust buffer destSize to what's needed, this avoids allocating fixed buffers and allows for unlimited query size. 'dest' is always a golf string so we will adjust it here.
// The method is not one of binding but of constructing a full SQL text with data in it, but under sterile conditions not conducive to SQL injection.
// We increase destSize to match what's needed and dest changes too. The caller code will free dest after SQL executes, so if in a loop,
// it will allocate again and so on.
//
void gg_make_SQL (char **dest, gg_num num_of_args, char *format, ...) 
{
    GG_TRACE("");
    assert (format);
    
    // Double quotes are allowed in format because ANSI_QUOTES  is set at the session's beginning.
    // So double quotes can NOT be used for string literals, ONLY single quotes.

    gg_num num_of_percents;
    gg_num count_percents;
    count_percents = gg_count_substring (format, _GG_LOOK_S, sizeof(_GG_LOOK_S)-1, 1);
    num_of_percents = count_percents; // used in a countdown later in this code

    
    // For the format, there has to be an even number of single quotes. An uneven number suggests
    // faulty query. For example, insert ... values 'xyz', 'aaa  - in this case the last string
    // never ends
    gg_num count_single_quote;
    count_single_quote = gg_count_substring (format, "'", 1, 1);
    if (count_single_quote % 2 != 0)
    {
        gg_report_error ("Incorrect number of single quotes, must be an even number, query [%s]", format);
    }

    // Format must be smaller than the destination size to begin with
    gg_num flen = strlen (format);
    gg_num destSize;
    *dest = (char*)gg_malloc (destSize = flen + 1);
    memcpy (*dest, format, flen + 1);

    char *curr_sql_arg = (char*)gg_malloc(1); // current argument is constantly re-allocated for args and dealloced at the end

    // All %s must be quoted, otherwise in select ... where id=%s, it could be made to be
    // select ... where id=2;drop table x; if input parameters is id=2;drop table x;
#define _GG_LOOK_S "'%s'"
    gg_num numPlaceWithQuotes = gg_count_substring (format, _GG_LOOK_S, sizeof(_GG_LOOK_S)-1, 1);
    if (numPlaceWithQuotes != count_percents)
    {
        gg_report_error ("All arguments in SQL statement must be quoted, including numbers, format [%s], number of arguments [%ld]", format, count_percents);
    }

    // num_of_args isn't the same as count_percents. num_of_args is the total number of input params, some of which may be NULL
    // count_percentss is the number of '%s' in SQL statement, which must be equal to # of non-NULL input params (among num_of_args params)

    va_list vl;
    va_start (vl, format);

    gg_num i;
    char *curr = *dest;
    char *curr_input;
    for (i = 0; i < num_of_args; i++)
    {
        curr_input = va_arg (vl, char *);

        if (curr_input == NULL) // such as with dynamic params if add-query-input is used
        {
            continue;
        }

        num_of_percents--;
        // make sure number of non-NULL params isn't greater than what's expected at run-time
        if (num_of_percents < 0)
        {
            gg_report_error ("Too many non-NULL input parameters in input parameter list for SQL statement [%s], expected [%ld] non-NULL run-time arguments", format, count_percents);
        }
        gg_num l_curr_input = strlen (curr_input);
        gg_num l_curr_sql_arg = 2 * l_curr_input + 1;
        curr_sql_arg = (char*)gg_realloc (gg_mem_get_id(curr_sql_arg), l_curr_sql_arg); // worse case scenario, all back-slashes and single-quotes
        // Escape: single quote and escape backslash in an single quote string
        // Some parameters might not be quoted, and we will catch that as an erro

        if (gg_db_escape (curr_input, curr_sql_arg, &l_curr_input) != 0)
        {
            va_end (vl);
            gg_report_error ("Argument #%ld cannot be escaped as input parameter [%s], argument [%.100s]", i, format, curr_sql_arg);
        }

        // substitute input parameters, one by one
        // but first trim each value if so by [no-]trim-query-input
        // We worked with a copy of data here, so no need to put anything back (like the zero character at the end we may put)
        char *val = curr_sql_arg;

        // trim right
        gg_num len_of = l_curr_input; // length is from gg_db_escape
        if (len_of != 0)
        {
            while (len_of!=0 && isspace(*(val+len_of-1))) len_of--;
            *(val+len_of)=0;
        }
        // trim left
        while (*val!=0 && isspace(*val)) val++;

        gg_num curr_offset = curr - *dest;
        *dest = (char*)gg_realloc (gg_mem_get_id(*dest), destSize += (len_of + 1)); // increase destSize to account for val size
        curr = *dest + curr_offset; // make curr (current within dest) remain properly within it since we
                                   // just realloced dest
        if (gg_replace_string (curr, destSize - 1 - (curr - *dest + 1), "%s", val, 0, &curr, 1) == -1)
        {
            va_end (vl);
            gg_report_error ("SQL too large, format [%s], argument [%.100s]", format, curr_sql_arg);
        }
    }

    gg_free (curr_sql_arg);

    // make sure number of non-NULL input params isn't lesser than what's expected at run-time
    // num_of_args is # of non-NULL input params in va_arg. # of actual params in va_arg can be different than num_of_percents
    // because va_arg can contain NULLs for dynamic params when add-query-input is used
    if (num_of_percents != 0)
    {
        gg_report_error ("Too few non-NULL input parameters in input parameter list for SQL statement [%s], expected [%ld] non-NULL run-time arguments", format, num_of_percents);
    }

    va_end (vl);
    GG_TRACE ("final statement:[%s]", *dest);
}


//
// Frees result of the query that just executed
// is_prep is 1 if this was prepared statement
//
void gg_db_free_result (char is_prep)
{
    GG_TRACE("");
    if (GG_CURR_DB.db_type == GG_DB_POSTGRES)
    {
        gg_pg_free();
    }
    else if (GG_CURR_DB.db_type == GG_DB_MARIADB)
    {
        gg_maria_free();
    }
    else if (GG_CURR_DB.db_type == GG_DB_SQLITE)
    {
        gg_lite_free(is_prep);
    }
    else
    {
        gg_report_error ("Unknown database type [%ld]", GG_CURR_DB.db_type); 
    }
}


