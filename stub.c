// SPDX-License-Identifier: Apache-2.0
// Copyright 2019 Gliim LLC. 
// Licensed under Apache License v2. See LICENSE file.
// On the web http://golf-lang.com/ - this file is part of Golf framework.

// 
// Stubs for unused modules
//


#include "golf.h"


#ifdef GG_PCRE2
gg_num gg_regex(char *look_here, char *find_this, char *replace, char **res, gg_num utf8, gg_num case_insensitive, gg_num single_match, regex_t **cached) {
    GG_UNUSED(look_here);
    GG_UNUSED(find_this);
    GG_UNUSED(replace);
    GG_UNUSED(utf8);
    GG_UNUSED(case_insensitive);
    GG_UNUSED(single_match);
    GG_UNUSED(cached);
    GG_UNUSED(res);
    return -1;
}
void gg_regfree(regex_t *preg) {
    GG_UNUSED(preg);
}
#endif

#ifdef GG_CRYPTO
void gg_sec_load_algos(void) {}
int gg_RAND_bytes(unsigned char *buf, int num) {
    GG_UNUSED(buf);
    GG_UNUSED(num);
    return 0; // failure if used by accident
}
#endif

#ifdef GG_SERVICE
void gg_set_fcgi (gg_cli **callin, char *server, char *req_method, char *app_path, char *req, char *url_params, char *ctype, char *body, int clen, int timeout, char **env, bool simple_server, bool simple_url) {
    GG_UNUSED(simple_server);
    GG_UNUSED(simple_url);
    GG_UNUSED(callin);
    GG_UNUSED(server);
    GG_UNUSED(req_method);
    GG_UNUSED(app_path);
    GG_UNUSED(req);
    GG_UNUSED(url_params);
    GG_UNUSED(ctype);
    GG_UNUSED(body);
    GG_UNUSED(clen);
    GG_UNUSED(timeout);
    GG_UNUSED(env);
}
void gg_cli_delete (gg_cli *callin) {
    GG_UNUSED(callin);
}
gg_num gg_call_fcgi (gg_cli **req, gg_num threads, gg_num *finokay, gg_num *started) {
    GG_UNUSED(req);
    GG_UNUSED(finokay);
    GG_UNUSED(started);
    GG_UNUSED(threads);
    return 0;
}
#endif

#ifdef GG_CURL
void curl_global_cleanup(void) {}

CURLcode curl_global_init(long flags)
{
    GG_UNUSED(flags);
    return 0;
}
#endif

#ifdef GG_TREE
gg_num gg_tree_bal (gg_tree_node *tree) { GG_UNUSED(tree); return 1; }
void gg_tree_create_root (gg_tree *res, bool sorted) { GG_UNUSED(res); GG_UNUSED(sorted); }
gg_tree *gg_tree_create(char key_type, bool sorted, bool process) { GG_UNUSED(key_type); GG_UNUSED(sorted); GG_UNUSED(process); return NULL;}
void gg_tree_insert_f (gg_tree_cursor *lcurs, gg_tree *orig_tree, char *key, gg_num key_len, void *data) {GG_UNUSED(lcurs);GG_UNUSED(orig_tree);GG_UNUSED(key);GG_UNUSED(key_len);GG_UNUSED(data);}
void gg_tree_search_f (gg_tree_cursor *lcurs, gg_tree *orig_tree, char *key, gg_num key_len) {GG_UNUSED(lcurs);GG_UNUSED(orig_tree);GG_UNUSED(key);GG_UNUSED(key_len);}
void gg_tree_delete_f (gg_tree_cursor *lcurs, gg_tree *orig_tree, char *key) {GG_UNUSED(lcurs);GG_UNUSED(orig_tree);GG_UNUSED(key);}
void gg_tree_search_greater_equal_f (gg_tree_cursor *lcurs, gg_tree *orig_tree, bool equal, char *key, gg_num key_len) {GG_UNUSED(lcurs); GG_UNUSED(orig_tree); GG_UNUSED(equal);GG_UNUSED(key);GG_UNUSED(key_len);}
void gg_tree_search_lesser_equal_f (gg_tree_cursor *lcurs, gg_tree *orig_tree, bool equal, char *key, gg_num key_len) {GG_UNUSED(lcurs); GG_UNUSED(orig_tree); GG_UNUSED(equal); GG_UNUSED(key); GG_UNUSED(key_len);}
void gg_tree_max_f (gg_tree_cursor *lcurs, gg_tree *orig_tree) {GG_UNUSED(lcurs); GG_UNUSED(orig_tree); }
void gg_tree_min_f (gg_tree_cursor *lcurs, gg_tree *orig_tree) {GG_UNUSED(lcurs); GG_UNUSED(orig_tree);}
void gg_tree_purge_f (gg_tree *parent_tree) {GG_UNUSED(parent_tree); }

#endif



#ifdef GG_STUB_GENDB
void gg_check_transaction(gg_num check_mode)
{
    GG_UNUSED(check_mode);
}
void gg_end_all_db() {}
#endif


#ifdef GG_STUB_POSTGRES
void gg_pg_close() {}
gg_dbc *gg_pg_connect (gg_num abort_if_bad)
{
    GG_UNUSED(abort_if_bad);
    return NULL;
}
gg_num gg_pg_exec(char *s, gg_num returns_tuple,  char is_prep, void **prep, gg_num paramcount, char **params)
{
    GG_UNUSED(s);
    GG_UNUSED(returns_tuple);
    GG_UNUSED(is_prep);
    GG_UNUSED(prep);
    GG_UNUSED(paramcount);
    GG_UNUSED(params);
    return 0;
}
gg_num gg_pg_affected() {return 0;}
gg_num gg_pg_nfield() {return 0;}
char *gg_pg_fieldname(gg_num fnum)
{
    GG_UNUSED(fnum);
    return NULL;
}
void gg_pg_free() {}
gg_num gg_pg_nrows() {return 0;}
void gg_pg_rows(char ***row, gg_num num_fields, gg_num nrow, unsigned long **lens)
{
    GG_UNUSED(row);
    GG_UNUSED(num_fields);
    GG_UNUSED(nrow);
    GG_UNUSED(lens);
}
char *gg_pg_error(char *s) 
{
    GG_UNUSED(s);
    return NULL;
}
char *gg_pg_errm(char *errm, gg_num errmsize, char *s, char *sname, gg_num lnum, char *er)
{
    GG_UNUSED(errm);
    GG_UNUSED(errmsize);
    GG_UNUSED(s);
    GG_UNUSED(sname);
    GG_UNUSED(lnum);
    GG_UNUSED(er);
    return NULL;
}
gg_num gg_pg_checkc() { return 0;}
void gg_pg_close_stmt (void *st)
{
    GG_UNUSED(st);
}
int gg_pg_escape(char *from, char *to, gg_num *len)
{
    GG_UNUSED(from);
    GG_UNUSED(to);
    GG_UNUSED(len);
    return 0;
}
#endif


#ifdef GG_STUB_MARIADB
void gg_maria_close () {}
gg_dbc *gg_maria_connect (gg_num abort_if_bad)
{
    GG_UNUSED(abort_if_bad);
    return NULL;
}
gg_num gg_maria_exec(char *s,  char is_prep, void **prep, gg_num paramcount, char **params)
{
    GG_UNUSED(s);
    GG_UNUSED(is_prep);
    GG_UNUSED(prep);
    GG_UNUSED(paramcount);
    GG_UNUSED(params);
    return 0;
}
gg_num gg_maria_affected(char is_prep) 
{
    GG_UNUSED(is_prep);
    return 0;
}
int gg_maria_use(char is_prep)
{
    GG_UNUSED(is_prep);
    return 0;
}
int gg_maria_store(char is_prep)
{
    GG_UNUSED(is_prep);
    return 0;
}
gg_num gg_maria_nfield() {return 0;}
char *gg_maria_fieldname() { return NULL; }
void gg_maria_free() {}
gg_num gg_maria_nrows(char is_prep) 
{
    GG_UNUSED(is_prep);
    return 0;
}
int gg_maria_rows (char ***row, unsigned long **lens, char is_prep)
{
    GG_UNUSED(row);
    GG_UNUSED(lens);
    GG_UNUSED(is_prep);
    return 0;
}
char *gg_maria_error(char *s, char is_prep) 
{
    GG_UNUSED(s);
    GG_UNUSED(is_prep);
    return NULL;
}
char *gg_maria_errm(char *errm, gg_num errmsize, char *s, char *sname, gg_num lnum, char *er, char is_prep)
{
    GG_UNUSED(errm);
    GG_UNUSED(errmsize);
    GG_UNUSED(s);
    GG_UNUSED(sname);
    GG_UNUSED(lnum);
    GG_UNUSED(er);
    GG_UNUSED(is_prep);
    return NULL;
}
gg_num gg_maria_checkc() { return 0;}
void gg_maria_close_stmt (void *st)
{
    GG_UNUSED(st);
}
int gg_maria_escape(char *from, char *to, gg_num *len)
{
    GG_UNUSED(from);
    GG_UNUSED(to);
    GG_UNUSED(len);
    return 0;
}
#endif


#ifdef GG_STUB_SQLITE
void gg_lite_close () {}
gg_dbc *gg_lite_connect (gg_num abort_if_bad)
{
    GG_UNUSED(abort_if_bad);
    return NULL;
}
gg_num gg_lite_exec(char *s,  char is_prep, void **prep, gg_num paramcount, char **params)
{
    GG_UNUSED(s);
    GG_UNUSED(is_prep);
    GG_UNUSED(prep);
    GG_UNUSED(paramcount);
    GG_UNUSED(params);
    return 0;
}
gg_num gg_lite_affected(char is_prep) 
{
    GG_UNUSED(is_prep);
    return 0;
}
int gg_lite_use(char is_prep)
{
    GG_UNUSED(is_prep);
    return 0;
}
int gg_lite_store(char is_prep)
{
    GG_UNUSED(is_prep);
    return 0;
}
gg_num gg_lite_nfield() {return 0;}
char *gg_lite_fieldname() { return NULL; }
void gg_lite_free(char is_prep) {GG_UNUSED(is_prep);}
gg_num gg_lite_nrows()
{
    return 0;
}
int gg_lite_rows (char ***row, unsigned long **lens)
{
    GG_UNUSED(row);
    GG_UNUSED(lens);
    return 0;
}
char *gg_lite_error(char *s, char is_prep) 
{
    GG_UNUSED(s);
    GG_UNUSED(is_prep);
    return NULL;
}
char *gg_lite_errm(char *errm, gg_num errmsize, char *s, char *sname, gg_num lnum, char *er, char is_prep)
{
    GG_UNUSED(errm);
    GG_UNUSED(errmsize);
    GG_UNUSED(s);
    GG_UNUSED(sname);
    GG_UNUSED(lnum);
    GG_UNUSED(er);
    GG_UNUSED(is_prep);
    return NULL;
}
gg_num gg_lite_checkc() { return 0;}
void gg_lite_close_stmt (void *st)
{
    GG_UNUSED(st);
}
int gg_lite_escape(char *from, char *to, gg_num *len)
{
    GG_UNUSED(from);
    GG_UNUSED(to);
    GG_UNUSED(len);
    return 0;
}
#endif

