// SPDX-License-Identifier: Apache-2.0
// Copyright 2018-2025 Gliim LLC. 
// Licensed under Apache License v2. See LICENSE file.
// On the web http://rimstone-lang.com/ - this file is part of RimStone framework.

// 
// postgres native interface, used by db.c for making of db-specific libs
//

#include "rim.h"
static char *rim_pg_stmt(void **prep);
static int rim_pg_prep_stmt(void **prep, char *stmt, rim_num num_of_args);
static char *cerror = NULL;

rim_num rim_pg_checkc()
{
    return (PQstatus(RIM_CURR_DB.dbc->pg.con) != CONNECTION_OK ? 0 : 1);
}

char *rim_pg_errm(char *errm, rim_num errmsize, char *s, char *sname, rim_num lnum, char *er)
{
    char *detail = PQresultErrorField(RIM_CURR_DB.dbc->pg.res, PG_DIAG_MESSAGE_DETAIL);
    snprintf(errm,errmsize,"Error during query [%s], additional [%s], detail [%s], file [%s], line [%ld] : [%s]:%s", s, cerror==NULL?"":cerror, detail==NULL?"":detail, sname, lnum, er ,PQerrorMessage(RIM_CURR_DB.dbc->pg.con));
    return errm;
}

char *rim_pg_error(char *s)
{
    RIM_UNUSED(s); // used for tracing only
    char *local_error = PQresultErrorField(RIM_CURR_DB.dbc->pg.res, PG_DIAG_SQLSTATE);
    return local_error == NULL ? "" : local_error;
}

void rim_pg_rows(char ***row, rim_num num_fields, rim_num nrow, unsigned long **lens)
{
    RIM_CURR_DB.need_copy = 1;
    *row = (char**)rim_malloc(num_fields*sizeof(char*));
    rim_num i;
    for(i = 0; i < num_fields; i++) (*row)[i] = PQgetvalue (RIM_CURR_DB.dbc->pg.res, nrow, i);
    *lens = (unsigned long*)rim_malloc(num_fields*sizeof(unsigned long));
    for(i = 0; i < num_fields; i++) (*lens)[i] = PQgetlength (RIM_CURR_DB.dbc->pg.res, nrow, i);
}


rim_num rim_pg_nrows()
{
    return PQntuples(RIM_CURR_DB.dbc->pg.res);
}

void rim_pg_free()
{
    if (RIM_CURR_DB.dbc->pg.res != NULL) PQclear (RIM_CURR_DB.dbc->pg.res);
    RIM_CURR_DB.dbc->pg.res = NULL;
}

char *rim_pg_fieldname(rim_num fnum)
{
    return PQfname(RIM_CURR_DB.dbc->pg.res, fnum);
}

rim_num rim_pg_nfield()
{
    return PQnfields(RIM_CURR_DB.dbc->pg.res);
}

void rim_pg_close()
{
    PQclear (RIM_CURR_DB.dbc->pg.res);
    PQfinish (RIM_CURR_DB.dbc->pg.con);
}


rim_dbc *rim_pg_connect (rim_num abort_if_bad)
{
    // reset all prepared statements
    rim_db_prep (NULL);


    char db_config_name[150];
    // allocate connection to database. Must be done here to account for the actual generated code, which
    // dictates the sizeof(rim_dbc)
    // This must be malloc and NOT rim_malloc since we want to reuse connection across connections.
    // Otherwise rim_malloc would be automatically freed when the request is done, and the next 
    // request would use an invalid pointer.  Also must use free to free it, not rim_free.
    if ((RIM_CURR_DB.dbc = rim_malloc0 (sizeof (rim_dbc))) == NULL) RIM_FATAL ("Cannot allocate memory for database connection [%m]");

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
    // make connection to database
    RIM_CURR_DB.dbc->pg.con = PQconnectdb(cinfo);
    RIM_CURR_DB.dbc->pg.res = NULL;
    rim_free_int (cinfo);

    if (PQstatus (RIM_CURR_DB.dbc->pg.con) != CONNECTION_OK)
    {
        char em[300];
        snprintf (em, sizeof(em), "Cannot initialize database connection [%s]", PQerrorMessage(RIM_CURR_DB.dbc->pg.con));
        if (abort_if_bad == 1) rim_report_error ("%s", em);
        rim_end_connection (0); // without it, we would think connection exists
        return NULL;
    }
    return RIM_CURR_DB.dbc;
}


rim_num rim_pg_exec(char *s, rim_num returns_tuple, char is_prep, void **prep, rim_num paramcount, char **params)
{
    rim_stmt_cached = 0;
    if (is_prep == 0)
    {
        return (PQresultStatus(RIM_CURR_DB.dbc->pg.res = PQexec(RIM_CURR_DB.dbc->pg.con, s)) != (returns_tuple == 1 ? PGRES_TUPLES_OK : PGRES_COMMAND_OK));
    }
    else
    {
        if (rim_pg_prep_stmt(prep, s, paramcount)) return 1;
        rim_num num = PQresultStatus (RIM_CURR_DB.dbc->pg.res = PQexecPrepared(RIM_CURR_DB.dbc->pg.con, RIM_CURR_DB.dbc->pg.name, paramcount, (const char * const *)params, NULL,NULL,0)) != (returns_tuple == 1 ? PGRES_TUPLES_OK : PGRES_COMMAND_OK);

        return num;
    }
}

rim_num rim_pg_affected()
{
    return atol(PQcmdTuples (RIM_CURR_DB.dbc->pg.res));
}


//
// deallocate prepared statement (only when connection lost), st is void * to prep stmt.
// the function will attempt to deallocate at server too; this is for future use, when we would deallocate in situation when connection is still up
//
void rim_pg_close_stmt (void *st)
{
    if (st == NULL) return; // statement has not been prepared yet, so cannot deallocate
    if (RIM_CURR_DB.dbc != NULL) 
    {
        // we only close statements when connection is lost, so we don't care about return code
        // only that memory is deallocated
        char dstm[60];
        snprintf(dstm, sizeof(dstm), "DEALLOCATE %s", (char*)st); // statement name is st
        PQexec(RIM_CURR_DB.dbc->pg.con, dstm);
    }
    free (st); // deallocate name itself, was strdup-ed
}

static char *rim_pg_stmt(void **prep)
{
#define PGSTMTNAMEL 30
    static char tmp[PGSTMTNAMEL];
    snprintf (tmp, PGSTMTNAMEL, "%p", prep);// name is void *, which is unique, easy to get
    rim_num i;
    if (!isalpha(tmp[0])) tmp[0] = 'A';
    for (i = 0; i < (rim_num)strlen (tmp); i++)
    {
        if (!isalnum(tmp[i])) tmp[i] = 'A';
    }
    return tmp;
}

//
// Prepare statement with source text stmt and num_of_args input parameters. 
// prep is the address of void * that points to prepared statement. This is a static
// void *, which survives requests (otherwise prepared statements wouldn't be very useful,
// actually would decrease performance), however prep is set to NULL when connection is 
// reestablished (typically if db server recycles), which is generally rare.
//
static int rim_pg_prep_stmt(void **prep, char *stmt, rim_num num_of_args)
{
    char *sname = "";
    rim_num lnum = 0;
    rim_location (&sname, &lnum, 0);

    // reuse prepared statement from prep
    if (*prep != NULL) 
    {
        rim_stmt_cached = 1;
        RIM_CURR_DB.dbc->pg.name = (char*)*prep;
    }
    else 
    {
        // if prep is NULL, create prepared statement
        // must duplicate as rim_pg_stmt returns static char*
        // must be strdup to survive request change (rimstone memory is gone, so no rim_strdup)
        if ((RIM_CURR_DB.dbc->pg.name = strdup(rim_pg_stmt(prep))) == NULL)
        {
            cerror = "Out of memory for prepared statement";
            return 1;
        }

        // if prep is NULL, create prepared statement
        char *origs = stmt; // original stmt
        stmt = rim_db_prep_text(stmt); // make $N instead of '%s' newly alloc'd, or return original stmt if nothing to do
        // prepare statement
        PGresult *res = PQprepare(RIM_CURR_DB.dbc->pg.con, RIM_CURR_DB.dbc->pg.name, stmt, num_of_args, NULL);
        if (PQresultStatus(res) != PGRES_COMMAND_OK)
        {
            cerror = "Cannot prepare statement";
            return 1;
        }
        if (stmt != origs) rim_free_int (stmt); // release statement if actually allocated
        *prep = RIM_CURR_DB.dbc->pg.name; // the name of prepared statements, lives for the life of the process
                                        // minus reconnects, which are rare
    }
    return 0;
}


//
// Escape string for inclusion in quoted params. Postgres uses single quotes only, which cannot be changed.
// memory for to must be 2*len+1. *len is the length of encoded string, not counting zero byte
//
int rim_pg_escape(char *from, char *to, rim_num *len)
{
    int err;
    *len = (rim_num)PQescapeStringConn (RIM_CURR_DB.dbc->pg.con, to, from, (size_t) *len, &err);
    if (err != 0) return 1;
    return 0;
}

