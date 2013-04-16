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

#include <binlog_api.h>
#include "php_mysqlbinlog.h"

#include <iostream>
#include <map>
#include <string>

using mysql::Binary_log;
using mysql::system::create_transport;
using mysql::system::get_event_type_str;
using mysql::User_var_event;


/* If you declare any globals in php_mysqlbinlog.h uncomment this:
ZEND_DECLARE_MODULE_GLOBALS(mysqlbinlog)
*/

/* True global resources - no need for thread safety here */
#define BINLOG_LINK_DESC "MySQL Binlog连接句柄"

static int le_binloglink;

/* {{{ mysqlbinlog_functions[]
 *
 * Every user visible function must have an entry in mysqlbinlog_functions[].
 */
const zend_function_entry mysqlbinlog_functions[] = {
	PHP_FE(confirm_mysqlbinlog_compiled, NULL)		/* For testing, remove later. */
    PHP_FE(binlog_connect, NULL)
    PHP_FE(binlog_wait_for_next_event, NULL)
    // PHP_FE(binlog_set_position, NULL)
    // PHP_FE(binlog_get_position, NULL)
	PHP_FE_END
};
/* }}} */

/* {{{ mysqlbinlog_module_entry
 */
zend_module_entry mysqlbinlog_module_entry = {
#if ZEND_MODULE_API_NO >= 20010901
	STANDARD_MODULE_HEADER,
#endif
	"mysqlbinlog",
	mysqlbinlog_functions,
	PHP_MINIT(mysqlbinlog),
	PHP_MSHUTDOWN(mysqlbinlog),
	PHP_RINIT(mysqlbinlog),		/* Replace with NULL if there's nothing to do at request start */
	PHP_RSHUTDOWN(mysqlbinlog),	/* Replace with NULL if there's nothing to do at request end */
	PHP_MINFO(mysqlbinlog),
#if ZEND_MODULE_API_NO >= 20010901
	"0.1", /* Replace with version number for your extension */
#endif
	STANDARD_MODULE_PROPERTIES
};
/* }}} */

#ifdef COMPILE_DL_MYSQLBINLOG
extern "C" {
ZEND_GET_MODULE(mysqlbinlog)
}
#endif

/* {{{ PHP_INI
 */
/* Remove comments and fill if you need to have entries in php.ini
PHP_INI_BEGIN()
    STD_PHP_INI_ENTRY("mysqlbinlog.global_value",      "42", PHP_INI_ALL, OnUpdateLong, global_value, zend_mysqlbinlog_globals, mysqlbinlog_globals)
    STD_PHP_INI_ENTRY("mysqlbinlog.global_string", "foobar", PHP_INI_ALL, OnUpdateString, global_string, zend_mysqlbinlog_globals, mysqlbinlog_globals)
PHP_INI_END()
*/
/* }}} */

/* {{{ php_mysqlbinlog_init_globals
 */
/* Uncomment this function if you have INI entries
static void php_mysqlbinlog_init_globals(zend_mysqlbinlog_globals *mysqlbinlog_globals)
{
	mysqlbinlog_globals->global_value = 0;
	mysqlbinlog_globals->global_string = NULL;
}
*/
/* }}} */

/* {{{ PHP_MINIT_FUNCTION
 */
PHP_MINIT_FUNCTION(mysqlbinlog)
{
	/* If you have INI entries, uncomment these lines 
	REGISTER_INI_ENTRIES();
	*/
    le_binloglink = zend_register_list_destructors_ex(NULL, NULL, BINLOG_LINK_DESC, module_number);
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MSHUTDOWN_FUNCTION
 */
PHP_MSHUTDOWN_FUNCTION(mysqlbinlog)
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
PHP_RINIT_FUNCTION(mysqlbinlog)
{
	return SUCCESS;
}
/* }}} */

/* Remove if there's nothing to do at request end */
/* {{{ PHP_RSHUTDOWN_FUNCTION
 */
PHP_RSHUTDOWN_FUNCTION(mysqlbinlog)
{
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MINFO_FUNCTION
 */
PHP_MINFO_FUNCTION(mysqlbinlog)
{
	php_info_print_table_start();
	php_info_print_table_header(2, "mysqlbinlog support", "enabled");
	php_info_print_table_row(2, "Version", PHP_MYSQLBINLOG_VERSION);    
	php_info_print_table_end();

	/* Remove comments if you have entries in php.ini
	DISPLAY_INI_ENTRIES();
	*/
}
/* }}} */


/* Remove the following function when you have succesfully modified config.m4
   so that your module can be compiled into PHP, it exists only for testing
   purposes. */

/* Every user-visible function in PHP should document itself in the source */
/* {{{ proto string confirm_mysqlbinlog_compiled(string arg)
   Return a string to confirm that the module is compiled in */

PHP_FUNCTION(confirm_mysqlbinlog_compiled)
{
	char *arg = NULL;
	int arg_len, len;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &arg, &arg_len) == FAILURE) {
		return;
	}
    Binary_log binlog(create_transport(arg));
    binlog.connect();

    while (true) {
        Binary_log_event *event;
        int result = binlog.wait_for_next_event(&event);
        if (result == ERR_EOF)
            break;
        switch (event->get_event_type()) {
            case QUERY_EVENT:
                std::cout << static_cast<Query_event*>(event)->query
                          << static_cast<Query_event*>(event)->header()->timestamp
                          << std::endl;
                break;
        }
    }
}
/* }}} */

PHP_FUNCTION(binlog_connect)
{
	char *arg = NULL;
	int arg_len, len;
    Binary_log *bp;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &arg, &arg_len) == FAILURE) {
		RETURN_NULL();
	}
    bp = new Binary_log (create_transport(arg));
    bp->connect();

    ZEND_REGISTER_RESOURCE(return_value, bp, le_binloglink);
}

PHP_FUNCTION(binlog_wait_for_next_event)
{
    zval *link;
    int id = -1;
    Binary_log *bp;
    Binary_log_event *event;
    
    
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r", &link) == FAILURE) {
		RETURN_NULL();
	}

    ZEND_FETCH_RESOURCE(bp, Binary_log *, &link, id, BINLOG_LINK_DESC, le_binloglink);

    int result = bp->wait_for_next_event(&event);
    
    if (result == ERR_EOF) RETURN_NULL();

    array_init(return_value);
    
    add_assoc_long(return_value, "type", event->get_event_type());
    
    switch (event->get_event_type()) {
        case QUERY_EVENT:
            add_assoc_string(return_value, "query", (char *)(static_cast<Query_event*>(event)->query).data(), 1);
            // std::cout << static_cast<Query_event*>(event)->query
            //           << static_cast<Query_event*>(event)->header()->timestamp
            //           << std::endl;
            break;
    }
    
	//php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unable to bind to server: %s");
}

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4
 * vim<600: noet sw=4 ts=4
 */
