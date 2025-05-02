// SPDX-License-Identifier: Apache-2.0
// Copyright 2018-2025 Gliim LLC. 
// Licensed under Apache License v2. See LICENSE file.
// On the web http://golf-lang.com/ - this file is part of Golf framework.

// 
// postgres native interface, used by db.c for making of db-specific libs
//

#include "golf.h"
static char *gg_pg_stmt(void **prep);
static int gg_pg_prep_stmt(void **prep, char *stmt, gg_num num_of_args);
static char *cerror = NULL;

gg_num gg_pg_checkc()
{
    GG_TRACE("");
    return (PQstatus(GG_CURR_DB.dbc->pg.con) != CONNECTION_OK ? 0 : 1);
}

char *gg_pg_errm(char *errm, gg_num errmsize, char *s, char *sname, gg_num lnum, char *er)
{
    GG_TRACE("");
    char *detail = PQresultErrorField(GG_CURR_DB.dbc->pg.res, PG_DIAG_MESSAGE_DETAIL);
    snprintf(errm,errmsize,"Error during query [%s], additional [%s], detail [%s], file [%s], line [%ld] : [%s]%s", s, cerror==NULL?"":cerror, detail==NULL?"":detail, sname, lnum, er ,PQerrorMessage(GG_CURR_DB.dbc->pg.con));
    return errm;
}

char *gg_pg_error(char *s)
{
    GG_TRACE("");
    GG_UNUSED(s); // used for tracing only
    char *local_error = PQresultErrorField(GG_CURR_DB.dbc->pg.res, PG_DIAG_SQLSTATE);
    GG_TRACE ("Error in %s: [%s] error [%d] state [%s]", s, PQerrorMessage(GG_CURR_DB.dbc->pg.con), PQresultStatus(GG_CURR_DB.dbc->pg.res), local_error == NULL ? "": local_error);
    return local_error == NULL ? "" : local_error;
}

void gg_pg_rows(char ***row, gg_num num_fields, gg_num nrow, unsigned long **lens)
{
    GG_TRACE("");
    GG_CURR_DB.need_copy = 1;
    *row = (char**)gg_malloc(num_fields*sizeof(char*));
    gg_num i;
    for(i = 0; i < num_fields; i++) (*row)[i] = PQgetvalue (GG_CURR_DB.dbc->pg.res, nrow, i);
    *lens = (unsigned long*)gg_malloc(num_fields*sizeof(unsigned long));
    for(i = 0; i < num_fields; i++) (*lens)[i] = PQgetlength (GG_CURR_DB.dbc->pg.res, nrow, i);
}


gg_num gg_pg_nrows()
{
    GG_TRACE("");
    return PQntuples(GG_CURR_DB.dbc->pg.res);
}

void gg_pg_free()
{
    GG_TRACE("");
    if (GG_CURR_DB.dbc->pg.res != NULL) PQclear (GG_CURR_DB.dbc->pg.res);
    GG_CURR_DB.dbc->pg.res = NULL;
}

char *gg_pg_fieldname(gg_num fnum)
{
    GG_TRACE("");
    return PQfname(GG_CURR_DB.dbc->pg.res, fnum);
}

gg_num gg_pg_nfield()
{
    GG_TRACE("");
    return PQnfields(GG_CURR_DB.dbc->pg.res);
}

void gg_pg_close()
{
    GG_TRACE("");
    PQclear (GG_CURR_DB.dbc->pg.res);
    PQfinish (GG_CURR_DB.dbc->pg.con);
}


gg_dbc *gg_pg_connect (gg_num abort_if_bad)
{
    GG_TRACE("");
    // reset all prepared statements
    gg_db_prep (NULL);


    char db_config_name[150];
    // allocate connection to database. Must be done here to account for the actual generated code, which
    // dictates the sizeof(gg_dbc)
    // This must be malloc and NOT gg_malloc since we want to reuse connection across connections.
    // Otherwise gg_malloc would be automatically freed when the request is done, and the next 
    // request would use an invalid pointer.  Also must use free to free it, not gg_free.
    if ((GG_CURR_DB.dbc = malloc (sizeof (gg_dbc))) == NULL) GG_FATAL ("Cannot allocate memory for database connection [%m]");

    snprintf (db_config_name, sizeof(db_config_name), "%s/%s", gg_get_config()->app.dbconf_dir, GG_CURR_DB.db_name);
    GG_TRACE ("Using db config file [%s]", db_config_name);
    char *cinfo;
    if (gg_read_file (db_config_name, &cinfo, 0, 0, NULL) < 0)
    {
        char em[300];
        snprintf (em, sizeof(em), "Cannot read database configuration file [%s]", db_config_name);
        GG_TRACE ("%s", em);
        if (abort_if_bad == 1) gg_report_error ("%s", em);
        gg_end_connection (0); // without it, we would think connection exists
        return NULL; // just for compiler, never gets here
    }
    // make connection to database
    GG_CURR_DB.dbc->pg.con = PQconnectdb(cinfo);
    GG_CURR_DB.dbc->pg.res = NULL;
    gg_free_int (cinfo);

    if (PQstatus (GG_CURR_DB.dbc->pg.con) != CONNECTION_OK)
    {
        char em[300];
        snprintf (em, sizeof(em), "Cannot initialize database connection [%s]", PQerrorMessage(GG_CURR_DB.dbc->pg.con));
        GG_TRACE ("%s", em);
        if (abort_if_bad == 1) gg_report_error ("%s", em);
        gg_end_connection (0); // without it, we would think connection exists
        return NULL;
    }
    return GG_CURR_DB.dbc;
}


gg_num gg_pg_exec(char *s, gg_num returns_tuple, char is_prep, void **prep, gg_num paramcount, char **params)
{
    GG_TRACE("");
    gg_stmt_cached = 0;
    if (is_prep == 0)
    {
        return (PQresultStatus(GG_CURR_DB.dbc->pg.res = PQexec(GG_CURR_DB.dbc->pg.con, s)) != (returns_tuple == 1 ? PGRES_TUPLES_OK : PGRES_COMMAND_OK));
    }
    else
    {
        if (gg_pg_prep_stmt(prep, s, paramcount)) return 1;
        gg_num num = PQresultStatus (GG_CURR_DB.dbc->pg.res = PQexecPrepared(GG_CURR_DB.dbc->pg.con, GG_CURR_DB.dbc->pg.name, paramcount, (const char * const *)params, NULL,NULL,0)) != (returns_tuple == 1 ? PGRES_TUPLES_OK : PGRES_COMMAND_OK);

        return num;
    }
}

gg_num gg_pg_affected()
{
    GG_TRACE("");
    return atol(PQcmdTuples (GG_CURR_DB.dbc->pg.res));
}


//
// deallocate prepared statement (only when connection lost), st is void * to prep stmt.
// the function will attempt to deallocate at server too; this is for future use, when we would deallocate in situation when connection is still up
//
void gg_pg_close_stmt (void *st)
{
    GG_TRACE("");
    if (st == NULL) return; // statement has not been prepared yet, so cannot deallocate
    if (GG_CURR_DB.dbc != NULL) 
    {
        // we only close statements when connection is lost, so we don't care about return code
        // only that memory is deallocated
        char dstm[60];
        snprintf(dstm, sizeof(dstm), "DEALLOCATE %s", (char*)st); // statement name is st
        PQexec(GG_CURR_DB.dbc->pg.con, dstm);
    }
    free (st); // deallocate name itself, was strdup-ed
}

char *gg_pg_stmt(void **prep)
{
    GG_TRACE("");
#define PGSTMTNAMEL 30
    static char tmp[PGSTMTNAMEL];
    snprintf (tmp, PGSTMTNAMEL, "%p", prep);// name is void *, which is unique, easy to get
    gg_num i;
    if (!isalpha(tmp[0])) tmp[0] = 'A';
    for (i = 0; i < (gg_num)strlen (tmp); i++)
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
int gg_pg_prep_stmt(void **prep, char *stmt, gg_num num_of_args)
{
    GG_TRACE("");
    char *sname = "";
    gg_num lnum = 0;
    gg_location (&sname, &lnum, 0);

    // reuse prepared statement from prep
    if (*prep != NULL) 
    {
        gg_stmt_cached = 1;
        GG_CURR_DB.dbc->pg.name = (char*)*prep;
    }
    else 
    {
        // if prep is NULL, create prepared statement
        // must duplicate as gg_pg_stmt returns static char*
        // must be strdup to survive request change (golf memory is gone, so no gg_strdup)
        if ((GG_CURR_DB.dbc->pg.name = strdup(gg_pg_stmt(prep))) == NULL)
        {
            cerror = "Out of memory for prepared statement";
            return 1;
        }

        // if prep is NULL, create prepared statement
        char *origs = stmt; // original stmt
        stmt = gg_db_prep_text(stmt); // make $N instead of '%s' newly alloc'd, or return original stmt if nothing to do
        // prepare statement
        PGresult *res = PQprepare(GG_CURR_DB.dbc->pg.con, GG_CURR_DB.dbc->pg.name, stmt, num_of_args, NULL);
        if (PQresultStatus(res) != PGRES_COMMAND_OK)
        {
            cerror = "Cannot prepare statement";
            return 1;
        }
        if (stmt != origs) gg_free_int (stmt); // release statement if actually allocated
        *prep = GG_CURR_DB.dbc->pg.name; // the name of prepared statements, lives for the life of the process
                                        // minus reconnects, which are rare
    }
    return 0;
}


//
// Escape string for inclusion in quoted params. Postgres uses single quotes only, which cannot be changed.
// memory for to must be 2*len+1. *len is the length of encoded string, not counting zero byte
//
int gg_pg_escape(char *from, char *to, gg_num *len)
{
    GG_TRACE("");
    int err;
    *len = (gg_num)PQescapeStringConn (GG_CURR_DB.dbc->pg.con, to, from, (size_t) *len, &err);
    if (err != 0) return 1;
    return 0;
}

