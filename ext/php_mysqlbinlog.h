/*
  +----------------------------------------------------------------------+
  | PHP Version 5                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2012 The PHP Group                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_01.txt                                  |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Author:                                                              |
  +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifndef PHP_MYSQLBINLOG_H
#define PHP_MYSQLBINLOG_H

extern zend_module_entry mysqlbinlog_module_entry;
#define phpext_mysqlbinlog_ptr &mysqlbinlog_module_entry

extern "C" {
#ifdef ZTS
#include "TSRM.h"
#endif
}

#define PHP_MYSQLBINLOG_VERSION "0.1.0"

#ifdef PHP_WIN32
#	define PHP_MYSQLBINLOG_API __declspec(dllexport)
#elif defined(__GNUC__) && __GNUC__ >= 4
#	define PHP_MYSQLBINLOG_API __attribute__ ((visibility("default")))
#else
#	define PHP_MYSQLBINLOG_API
#endif

#ifdef ZTS
#include "TSRM.h"
#endif

#ifndef PHP_FE_END
#define PHP_FE_END { NULL, NULL, NULL, 0, 0 }
#endif

#ifndef ZEND_MOD_END
#define ZEND_MOD_END { NULL, NULL, NULL, 0 }
#endif

PHP_MINIT_FUNCTION(mysqlbinlog);
PHP_MSHUTDOWN_FUNCTION(mysqlbinlog);
PHP_RINIT_FUNCTION(mysqlbinlog);
PHP_RSHUTDOWN_FUNCTION(mysqlbinlog);
PHP_MINFO_FUNCTION(mysqlbinlog);

#include <binlog_api.h>

void binlog_destruction_handler(zend_rsrc_list_entry *rsrc TSRMLS_DC);
void proc_event(mysql::Row_of_fields &fields, zval *mysql_fields);
bool in_watch_wild_tables(char* tbl, int tbl_len, std::string tbl_given TSRMLS_DC);

PHP_FUNCTION(binlog_connect);
PHP_FUNCTION(binlog_disconnect);
PHP_FUNCTION(binlog_wait_for_next_event);
PHP_FUNCTION(binlog_set_position);
PHP_FUNCTION(binlog_get_position);

typedef mysql::Table_map_event *tbl_map_evt;

ZEND_BEGIN_MODULE_GLOBALS(mysqlbinlog)
    tbl_map_evt tmev;
ZEND_END_MODULE_GLOBALS(mysqlbinlog)

/* In every utility function you add that needs to use variables 
   in php_mysqlbinlog_globals, call TSRMLS_FETCH(); after declaring other 
   variables used by that function, or better yet, pass in TSRMLS_CC
   after the last function argument and declare your utility function
   with TSRMLS_DC after the last declared argument.  Always refer to
   the globals in your function as MYSQLBINLOG_G(variable).  You are 
   encouraged to rename these macros something shorter, see
   examples in any other php module directory.
*/

#ifdef ZTS
#define MYSQLBINLOG_G(v) TSRMG(mysqlbinlog_globals_id, zend_mysqlbinlog_globals *, v)
#else
#define MYSQLBINLOG_G(v) (mysqlbinlog_globals.v)
#endif

#endif	/* PHP_MYSQLBINLOG_H */


/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
