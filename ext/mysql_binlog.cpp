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

extern "C" {
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
}

#include "php_mysql_binlog.h"

/* If you declare any globals in php_mysql_binlog.h uncomment this:
ZEND_DECLARE_MODULE_GLOBALS(mysql_binlog)
*/

const zend_function_entry mysql_binlog_functions[] = {
	//PHP_FE(confirm_mysql_binlog_compiled,	NULL)		/* For testing, remove later. */
	{NULL, NULL, NULL}
};

/*
 * mysql replication client
 *
 */
zend_class_entry *client_ce;


const zend_function_entry client_methods[] = {
	PHP_ME(BinlogClient, __construct, NULL, ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
	{NULL, NULL, NULL}
};

/* {{{ mysql_binlog_module_entry
 */
zend_module_entry mysql_binlog_module_entry = {
#if ZEND_MODULE_API_NO >= 20010901
	STANDARD_MODULE_HEADER,
#endif
	"mysql_binlog",
	mysql_binlog_functions,
	PHP_MINIT(mysql_binlog),
	PHP_MSHUTDOWN(mysql_binlog),
	PHP_RINIT(mysql_binlog),		/* Replace with NULL if there's nothing to do at request start */
	PHP_RSHUTDOWN(mysql_binlog),	/* Replace with NULL if there's nothing to do at request end */
	PHP_MINFO(mysql_binlog),
#if ZEND_MODULE_API_NO >= 20010901
	PHP_MYSQL_BINLOG_VERSION,
#endif
	STANDARD_MODULE_PROPERTIES
};
/* }}} */

#ifdef COMPILE_DL_MYSQL_BINLOG
extern "C" {
ZEND_GET_MODULE(mysql_binlog)
}
#endif

/* {{{ PHP_INI
 */
/* Remove comments and fill if you need to have entries in php.ini
PHP_INI_BEGIN()
    STD_PHP_INI_ENTRY("mysql_binlog.global_value",      "42", PHP_INI_ALL, OnUpdateLong, global_value, zend_mysql_binlog_globals, mysql_binlog_globals)
    STD_PHP_INI_ENTRY("mysql_binlog.global_string", "foobar", PHP_INI_ALL, OnUpdateString, global_string, zend_mysql_binlog_globals, mysql_binlog_globals)
PHP_INI_END()
*/
/* }}} */

/* {{{ php_mysql_binlog_init_globals
 */
/* Uncomment this function if you have INI entries
static void php_mysql_binlog_init_globals(zend_mysql_binlog_globals *mysql_binlog_globals)
{
	mysql_binlog_globals->global_value = 0;
	mysql_binlog_globals->global_string = NULL;
}
*/
/* }}} */

/* {{{ PHP_MINIT_FUNCTION
 */
PHP_MINIT_FUNCTION(mysql_binlog)
{
	/* If you have INI entries, uncomment these lines 
	REGISTER_INI_ENTRIES();
	*/
	zend_class_entry ce;
	INIT_CLASS_ENTRY(ce, "Client", client_methods);
	client_ce = zend_register_internal_class(&ce TSRMLS_CC);
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MSHUTDOWN_FUNCTION
 */
PHP_MSHUTDOWN_FUNCTION(mysql_binlog)
{
	/* uncomment this line if you have INI entries
	UNREGISTER_INI_ENTRIES();
	*/
	return SUCCESS;
}
/* }}} */

/* Remove if there's nothing to do at request start */
/* {{{ PHP_RINIT_FUNCTION
 */
PHP_RINIT_FUNCTION(mysql_binlog)
{
	return SUCCESS;
}
/* }}} */

/* Remove if there's nothing to do at request end */
/* {{{ PHP_RSHUTDOWN_FUNCTION
 */
PHP_RSHUTDOWN_FUNCTION(mysql_binlog)
{
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MINFO_FUNCTION
 */
PHP_MINFO_FUNCTION(mysql_binlog)
{
	php_info_print_table_start();
	php_info_print_table_header(2, "mysql_binlog support", "enabled");
	php_info_print_table_row(2, "Version", PHP_MYSQL_BINLOG_VERSION);
	php_info_print_table_end();

	/* Remove comments if you have entries in php.ini
	DISPLAY_INI_ENTRIES();
	*/
}
/* }}} */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4
 * vim<600: noet sw=4 ts=4
 */
