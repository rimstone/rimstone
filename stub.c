// SPDX-License-Identifier: Apache-2.0
// Copyright 2018-2025 Gliim LLC. 
// Licensed under Apache License v2. See LICENSE file.
// On the web http://rimstone-lang.com/ - this file is part of RimStone framework.

// 
// Stubs for unused modules
//


#include "rim.h"


#ifdef RIM_PCRE2
rim_num rim_regex(char *look_here, char *find_this, char *replace, char **res, rim_num utf, rim_num case_insensitive, rim_num single_match, regex_t **cached) {
    RIM_UNUSED(look_here);
    RIM_UNUSED(find_this);
    RIM_UNUSED(replace);
    RIM_UNUSED(utf);
    RIM_UNUSED(case_insensitive);
    RIM_UNUSED(single_match);
    RIM_UNUSED(cached);
    RIM_UNUSED(res);
    return -1;
}
void rim_regfree(regex_t *preg) {
    RIM_UNUSED(preg);
}
#endif

#ifdef RIM_CRYPTO
void rim_sec_load_algos(void) {}
int rim_RAND_bytes(unsigned char *buf, int num) {
    RIM_UNUSED(buf);
    RIM_UNUSED(num);
    return 0; // failure if used by accident
}
#endif

#ifdef RIM_SERVICE
void rim_set_fcgi (rim_cli **callin, char *server, char *req_method, char *app_path, char *req, char *url_params, char *ctype, char *body, int clen, int timeout, char **env, bool simple_server, bool simple_url) {
    RIM_UNUSED(simple_server);
    RIM_UNUSED(simple_url);
    RIM_UNUSED(callin);
    RIM_UNUSED(server);
    RIM_UNUSED(req_method);
    RIM_UNUSED(app_path);
    RIM_UNUSED(req);
    RIM_UNUSED(url_params);
    RIM_UNUSED(ctype);
    RIM_UNUSED(body);
    RIM_UNUSED(clen);
    RIM_UNUSED(timeout);
    RIM_UNUSED(env);
}
void rim_cli_delete (rim_cli *callin) {
    RIM_UNUSED(callin);
}
rim_num rim_call_fcgi (rim_cli **req, rim_num threads, rim_num *finokay, rim_num *started) {
    RIM_UNUSED(req);
    RIM_UNUSED(finokay);
    RIM_UNUSED(started);
    RIM_UNUSED(threads);
    return 0;
}
#endif

#ifdef RIM_TREE
rim_num rim_tree_bal (rim_tree_node *tree) { RIM_UNUSED(tree); return 1; }
void rim_tree_create_root (rim_tree *res, bool sorted) { RIM_UNUSED(res); RIM_UNUSED(sorted); }
rim_tree *rim_tree_create(char key_type, bool sorted, unsigned char process) { RIM_UNUSED(key_type); RIM_UNUSED(sorted); RIM_UNUSED(process); return NULL;}
void rim_tree_insert_f (rim_tree_cursor *lcurs, rim_tree *orig_tree, char *key, rim_num key_len, void *data) {RIM_UNUSED(lcurs);RIM_UNUSED(orig_tree);RIM_UNUSED(key);RIM_UNUSED(key_len);RIM_UNUSED(data);}
void rim_tree_search_f (rim_tree_cursor *lcurs, rim_tree *orig_tree, char *key, rim_num key_len) {RIM_UNUSED(lcurs);RIM_UNUSED(orig_tree);RIM_UNUSED(key);RIM_UNUSED(key_len);}
void rim_tree_delete_f (rim_tree_cursor *lcurs, rim_tree *orig_tree, char *key) {RIM_UNUSED(lcurs);RIM_UNUSED(orig_tree);RIM_UNUSED(key);}
void rim_tree_search_greater_equal_f (rim_tree_cursor *lcurs, rim_tree *orig_tree, bool equal, char *key, rim_num key_len) {RIM_UNUSED(lcurs); RIM_UNUSED(orig_tree); RIM_UNUSED(equal);RIM_UNUSED(key);RIM_UNUSED(key_len);}
void rim_tree_search_lesser_equal_f (rim_tree_cursor *lcurs, rim_tree *orig_tree, bool equal, char *key, rim_num key_len) {RIM_UNUSED(lcurs); RIM_UNUSED(orig_tree); RIM_UNUSED(equal); RIM_UNUSED(key); RIM_UNUSED(key_len);}
void rim_tree_max_f (rim_tree_cursor *lcurs, rim_tree *orig_tree) {RIM_UNUSED(lcurs); RIM_UNUSED(orig_tree); }
void rim_tree_min_f (rim_tree_cursor *lcurs, rim_tree *orig_tree) {RIM_UNUSED(lcurs); RIM_UNUSED(orig_tree);}
void rim_tree_purge_f (rim_tree *parent_tree) {RIM_UNUSED(parent_tree); }

#endif



#ifdef RIM_STUB_GENDB
void rim_check_transaction(rim_num check_mode)
{
    RIM_UNUSED(check_mode);
}
void rim_end_all_db() {}
#endif


#ifdef RIM_STUB_POSTGRES
void rim_pg_close() {}
rim_dbc *rim_pg_connect (rim_num abort_if_bad)
{
    RIM_UNUSED(abort_if_bad);
    return NULL;
}
rim_num rim_pg_exec(char *s, rim_num returns_tuple,  char is_prep, void **prep, rim_num paramcount, char **params)
{
    RIM_UNUSED(s);
    RIM_UNUSED(returns_tuple);
    RIM_UNUSED(is_prep);
    RIM_UNUSED(prep);
    RIM_UNUSED(paramcount);
    RIM_UNUSED(params);
    return 0;
}
rim_num rim_pg_affected() {return 0;}
rim_num rim_pg_nfield() {return 0;}
char *rim_pg_fieldname(rim_num fnum)
{
    RIM_UNUSED(fnum);
    return NULL;
}
void rim_pg_free() {}
rim_num rim_pg_nrows() {return 0;}
void rim_pg_rows(char ***row, rim_num num_fields, rim_num nrow, unsigned long **lens)
{
    RIM_UNUSED(row);
    RIM_UNUSED(num_fields);
    RIM_UNUSED(nrow);
    RIM_UNUSED(lens);
}
char *rim_pg_error(char *s) 
{
    RIM_UNUSED(s);
    return NULL;
}
char *rim_pg_errm(char *errm, rim_num errmsize, char *s, char *sname, rim_num lnum, char *er)
{
    RIM_UNUSED(errm);
    RIM_UNUSED(errmsize);
    RIM_UNUSED(s);
    RIM_UNUSED(sname);
    RIM_UNUSED(lnum);
    RIM_UNUSED(er);
    return NULL;
}
rim_num rim_pg_checkc() { return 0;}
void rim_pg_close_stmt (void *st)
{
    RIM_UNUSED(st);
}
int rim_pg_escape(char *from, char *to, rim_num *len)
{
    RIM_UNUSED(from);
    RIM_UNUSED(to);
    RIM_UNUSED(len);
    return 0;
}
#endif


#ifdef RIM_STUB_MARIADB
void rim_maria_close () {}
rim_dbc *rim_maria_connect (rim_num abort_if_bad)
{
    RIM_UNUSED(abort_if_bad);
    return NULL;
}
rim_num rim_maria_exec(char *s,  char is_prep, void **prep, rim_num paramcount, char **params)
{
    RIM_UNUSED(s);
    RIM_UNUSED(is_prep);
    RIM_UNUSED(prep);
    RIM_UNUSED(paramcount);
    RIM_UNUSED(params);
    return 0;
}
rim_num rim_maria_affected(char is_prep) 
{
    RIM_UNUSED(is_prep);
    return 0;
}
int rim_maria_use(char is_prep)
{
    RIM_UNUSED(is_prep);
    return 0;
}
int rim_maria_store(char is_prep)
{
    RIM_UNUSED(is_prep);
    return 0;
}
rim_num rim_maria_nfield() {return 0;}
char *rim_maria_fieldname() { return NULL; }
void rim_maria_free() {}
rim_num rim_maria_nrows(char is_prep) 
{
    RIM_UNUSED(is_prep);
    return 0;
}
int rim_maria_rows (char ***row, unsigned long **lens, char is_prep)
{
    RIM_UNUSED(row);
    RIM_UNUSED(lens);
    RIM_UNUSED(is_prep);
    return 0;
}
char *rim_maria_error(char *s, char is_prep) 
{
    RIM_UNUSED(s);
    RIM_UNUSED(is_prep);
    return NULL;
}
char *rim_maria_errm(char *errm, rim_num errmsize, char *s, char *sname, rim_num lnum, char *er, char is_prep)
{
    RIM_UNUSED(errm);
    RIM_UNUSED(errmsize);
    RIM_UNUSED(s);
    RIM_UNUSED(sname);
    RIM_UNUSED(lnum);
    RIM_UNUSED(er);
    RIM_UNUSED(is_prep);
    return NULL;
}
rim_num rim_maria_checkc() { return 0;}
void rim_maria_close_stmt (void *st)
{
    RIM_UNUSED(st);
}
int rim_maria_escape(char *from, char *to, rim_num *len)
{
    RIM_UNUSED(from);
    RIM_UNUSED(to);
    RIM_UNUSED(len);
    return 0;
}
#endif


#ifdef RIM_STUB_SQLITE
void rim_lite_close () {}
rim_dbc *rim_lite_connect (rim_num abort_if_bad)
{
    RIM_UNUSED(abort_if_bad);
    return NULL;
}
rim_num rim_lite_exec(char *s,  char is_prep, void **prep, rim_num paramcount, char **params)
{
    RIM_UNUSED(s);
    RIM_UNUSED(is_prep);
    RIM_UNUSED(prep);
    RIM_UNUSED(paramcount);
    RIM_UNUSED(params);
    return 0;
}
rim_num rim_lite_affected(char is_prep) 
{
    RIM_UNUSED(is_prep);
    return 0;
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
rim_num rim_lite_nfield() {return 0;}
char *rim_lite_fieldname() { return NULL; }
void rim_lite_free(char is_prep) {RIM_UNUSED(is_prep);}
rim_num rim_lite_nrows()
{
    return 0;
}
int rim_lite_rows (char ***row, unsigned long **lens)
{
    RIM_UNUSED(row);
    RIM_UNUSED(lens);
    return 0;
}
char *rim_lite_error(char *s, char is_prep) 
{
    RIM_UNUSED(s);
    RIM_UNUSED(is_prep);
    return NULL;
}
char *rim_lite_errm(char *errm, rim_num errmsize, char *s, char *sname, rim_num lnum, char *er, char is_prep)
{
    RIM_UNUSED(errm);
    RIM_UNUSED(errmsize);
    RIM_UNUSED(s);
    RIM_UNUSED(sname);
    RIM_UNUSED(lnum);
    RIM_UNUSED(er);
    RIM_UNUSED(is_prep);
    return NULL;
}
rim_num rim_lite_checkc() { return 0;}
void rim_lite_close_stmt (void *st)
{
    RIM_UNUSED(st);
}
int rim_lite_escape(char *from, char *to, rim_num *len)
{
    RIM_UNUSED(from);
    RIM_UNUSED(to);
    RIM_UNUSED(len);
    return 0;
}
#endif

