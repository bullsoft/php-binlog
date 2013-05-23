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

#include "php_mysqlbinlog.h"

#include <iostream>
#include <map>
#include <string>

using mysql::Binary_log;
using mysql::system::create_transport;
using mysql::system::get_event_type_str;
using mysql::User_var_event;

ZEND_DECLARE_MODULE_GLOBALS(mysqlbinlog)

/* True global resources - no need for thread safety here */
#define BINLOG_LINK_DESC "MySQL Binlog 连接句柄"

static int le_binloglink;

/* {{{ mysqlbinlog_functions[]
 *
 * Every user visible function must have an entry in mysqlbinlog_functions[].
 */
const zend_function_entry mysqlbinlog_functions[] = {
    PHP_FE(binlog_connect, NULL)
    PHP_FE(binlog_wait_for_next_event, NULL)
    PHP_FE(binlog_set_position, NULL)
    PHP_FE(binlog_get_position, NULL)
	PHP_FE_END	/* Must be the last line in mysqlbinlog_functions[] */
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
	PHP_MYSQLBINLOG_VERSION, /* Replace with version number for your extension */
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
    REGISTER_LONG_CONSTANT("BINLOG_UNKNOWN_EVENT"            , mysql::UNKNOWN_EVENT            , CONST_CS | CONST_PERSISTENT);
    REGISTER_LONG_CONSTANT("BINLOG_START_EVENT_V3"           , mysql::START_EVENT_V3           , CONST_CS | CONST_PERSISTENT);
    REGISTER_LONG_CONSTANT("BINLOG_QUERY_EVENT"              , mysql::QUERY_EVENT              , CONST_CS | CONST_PERSISTENT);
    REGISTER_LONG_CONSTANT("BINLOG_STOP_EVENT"               , mysql::STOP_EVENT               , CONST_CS | CONST_PERSISTENT);
    REGISTER_LONG_CONSTANT("BINLOG_ROTATE_EVENT"             , mysql::ROTATE_EVENT             , CONST_CS | CONST_PERSISTENT);
    REGISTER_LONG_CONSTANT("BINLOG_INTVAR_EVENT"             , mysql::INTVAR_EVENT             , CONST_CS | CONST_PERSISTENT);
    REGISTER_LONG_CONSTANT("BINLOG_LOAD_EVENT"               , mysql::LOAD_EVENT               , CONST_CS | CONST_PERSISTENT);
    REGISTER_LONG_CONSTANT("BINLOG_SLAVE_EVENT"              , mysql::SLAVE_EVENT              , CONST_CS | CONST_PERSISTENT);
    REGISTER_LONG_CONSTANT("BINLOG_CREATE_FILE_EVENT"        , mysql::CREATE_FILE_EVENT        , CONST_CS | CONST_PERSISTENT);
    REGISTER_LONG_CONSTANT("BINLOG_APPEND_BLOCK_EVENT"       , mysql::APPEND_BLOCK_EVENT       , CONST_CS | CONST_PERSISTENT);
    REGISTER_LONG_CONSTANT("BINLOG_EXEC_LOAD_EVENT"          , mysql::EXEC_LOAD_EVENT          , CONST_CS | CONST_PERSISTENT);
    REGISTER_LONG_CONSTANT("BINLOG_DELETE_FILE_EVENT"        , mysql::DELETE_FILE_EVENT        , CONST_CS | CONST_PERSISTENT);

    REGISTER_LONG_CONSTANT("BINLOG_NEW_LOAD_EVENT"           , mysql::NEW_LOAD_EVENT           , CONST_CS | CONST_PERSISTENT);
    REGISTER_LONG_CONSTANT("BINLOG_RAND_EVENT"               , mysql::RAND_EVENT               , CONST_CS | CONST_PERSISTENT);
    REGISTER_LONG_CONSTANT("BINLOG_USER_VAR_EVENT"           , mysql::USER_VAR_EVENT           , CONST_CS | CONST_PERSISTENT);
    REGISTER_LONG_CONSTANT("BINLOG_FORMAT_DESCRIPTION_EVENT" , mysql::FORMAT_DESCRIPTION_EVENT , CONST_CS | CONST_PERSISTENT);
    REGISTER_LONG_CONSTANT("BINLOG_XID_EVENT"                , mysql::XID_EVENT                , CONST_CS | CONST_PERSISTENT);
    REGISTER_LONG_CONSTANT("BINLOG_BEGIN_LOAD_QUERY_EVENT"   , mysql::BEGIN_LOAD_QUERY_EVENT   , CONST_CS | CONST_PERSISTENT);
    REGISTER_LONG_CONSTANT("BINLOG_EXECUTE_LOAD_QUERY_EVENT" , mysql::EXECUTE_LOAD_QUERY_EVENT , CONST_CS | CONST_PERSISTENT);
    REGISTER_LONG_CONSTANT("BINLOG_TABLE_MAP_EVENT"          , mysql::TABLE_MAP_EVENT          , CONST_CS | CONST_PERSISTENT);

    REGISTER_LONG_CONSTANT("BINLOG_PRE_GA_WRITE_ROWS_EVENT"  , mysql::PRE_GA_WRITE_ROWS_EVENT  , CONST_CS | CONST_PERSISTENT);
    REGISTER_LONG_CONSTANT("BINLOG_PRE_GA_UPDATE_ROWS_EVENT" , mysql::PRE_GA_UPDATE_ROWS_EVENT , CONST_CS | CONST_PERSISTENT);
    REGISTER_LONG_CONSTANT("BINLOG_PRE_GA_DELETE_ROWS_EVENT" , mysql::PRE_GA_DELETE_ROWS_EVENT , CONST_CS | CONST_PERSISTENT);

    REGISTER_LONG_CONSTANT("BINLOG_WRITE_ROWS_EVENT"         , mysql::WRITE_ROWS_EVENT         , CONST_CS | CONST_PERSISTENT);
    REGISTER_LONG_CONSTANT("BINLOG_UPDATE_ROWS_EVENT"        , mysql::UPDATE_ROWS_EVENT        , CONST_CS | CONST_PERSISTENT);
    REGISTER_LONG_CONSTANT("BINLOG_DELETE_ROWS_EVENT"        , mysql::DELETE_ROWS_EVENT        , CONST_CS | CONST_PERSISTENT);

    REGISTER_LONG_CONSTANT("BINLOG_INCIDENT_EVENT"           , mysql::INCIDENT_EVENT           , CONST_CS | CONST_PERSISTENT);

    REGISTER_LONG_CONSTANT("BINLOG_USER_DEFINED"             , mysql::USER_DEFINED             , CONST_CS | CONST_PERSISTENT);

    /* If you have INI entries, uncomment these lines 
    REGISTER_INI_ENTRIES();
    */
    le_binloglink = zend_register_list_destructors_ex(binlog_destruction_handler, NULL, BINLOG_LINK_DESC, module_number);
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
    php_info_print_table_row(2, "Author", "Roy Gu (guweigang@baidu.com, guweigang@outlook.com)");
    php_info_print_table_row(2, "Author", "Ideal (shangyuanchun@baidu.com, idealities@gmail.com)");    
    php_info_print_table_end();

    /* Remove comments if you have entries in php.ini
    DISPLAY_INI_ENTRIES();
    */
}
/* }}} */

/* this function is called when request shutdown */
/* {{{ binlog_destruction_handler
 */
void binlog_destruction_handler(zend_rsrc_list_entry *rsrc TSRMLS_DC)
{
    Binary_log *bp = (Binary_log *)rsrc->ptr;
    if (bp) {
        delete bp;
    }
}
/* }}} */

PHP_FUNCTION(binlog_connect)
{
    char *arg = NULL;
    int  arg_len;
    Binary_log *bp;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &arg, &arg_len) == FAILURE) {
        RETURN_NULL();
    }
    bp = new Binary_log (create_transport(arg));
    if(bp->connect()) {
        php_error_docref(NULL TSRMLS_CC, E_WARNING, "Connect to mysql failed: %s", arg);
        RETURN_FALSE;
    }

    ZEND_REGISTER_RESOURCE(return_value, bp, le_binloglink);
}

PHP_FUNCTION(binlog_wait_for_next_event)
{
    zval *link; int id = -1;
    char *db = NULL, *tbl = NULL;
    int db_len, tbl_len;
    Binary_log *bp;
    Binary_log_event *event;
    
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r|ss", &link, &db, &db_len, &tbl, &tbl_len) == FAILURE) {
        RETURN_NULL();
    }

    ZEND_FETCH_RESOURCE(bp, Binary_log *, &link, id, BINLOG_LINK_DESC, le_binloglink);

    if (!bp) {
        php_error_docref(NULL TSRMLS_CC, E_WARNING, "Wrong resource handler passed to binlog_wait_for_next_event().");
        RETURN_NULL();
    }

    int result = bp->wait_for_next_event(&event);
    
    if (result == ERR_EOF) RETURN_NULL();

    // array initial
    array_init(return_value);
    
    add_assoc_long(return_value, "type_code", event->get_event_type());
    add_assoc_string(return_value, "type_str", (char *)get_event_type_str(event->get_event_type()), 1);
    
    mysql::Log_event_type event_type = event->get_event_type();
    
    switch (event_type) {
        case mysql::QUERY_EVENT:
        {
            const mysql::Query_event *qev= static_cast<const mysql::Query_event *>(event);
            add_assoc_string(return_value, "query", (char *)(qev->query).c_str(), 1);
            add_assoc_string(return_value, "db_name", (char *)(qev->db_name).c_str(), 1);
        }
        break;
        case mysql::ROTATE_EVENT:
        {
            mysql::Rotate_event *rot= static_cast<mysql::Rotate_event *>(event);
            add_assoc_string(return_value, "filename", (char *)rot->binlog_file.c_str(), 1);
            add_assoc_long(return_value, "position", rot->binlog_pos);
        }
        break;
        case mysql::TABLE_MAP_EVENT:
        {
            MYSQLBINLOG_G(tmev) = static_cast<mysql::Table_map_event *>(event);
            
            add_assoc_string(return_value, "db_name", (char *) MYSQLBINLOG_G(tmev)->db_name.c_str(), 1);            
            add_assoc_long(return_value, "table_id", MYSQLBINLOG_G(tmev)->table_id);
            add_assoc_string(return_value, "table_name", (char *) MYSQLBINLOG_G(tmev)->table_name.c_str(), 1);
        }
        break;
        case mysql::WRITE_ROWS_EVENT:
        case mysql::UPDATE_ROWS_EVENT:
        case mysql::DELETE_ROWS_EVENT:
        {
            add_assoc_string(return_value, "db_name", (char *) MYSQLBINLOG_G(tmev)->db_name.c_str(), 1);            
            add_assoc_string(return_value, "table_name", (char *) MYSQLBINLOG_G(tmev)->table_name.c_str(), 1);

            if(db != NULL && strcmp(db, (char *) MYSQLBINLOG_G(tmev)->db_name.c_str())) {
                break;
            }
            
            if(tbl != NULL && strcmp(tbl, (char *) MYSQLBINLOG_G(tmev)->table_name.c_str())) {
                break;
            }
            
            zval *mysql_rows = NULL;
            MAKE_STD_ZVAL(mysql_rows);
            array_init(mysql_rows);
            
            mysql::Row_event *rev= static_cast<mysql::Row_event *>(event);
            
            mysql::Row_event_set rows(rev, MYSQLBINLOG_G(tmev));
            mysql::Row_event_set::iterator itor = rows.begin();
            
            int i=0;
            do {
                mysql::Row_of_fields fields = *itor;
                zval *mysql_row = NULL;
                MAKE_STD_ZVAL(mysql_row);
                array_init(mysql_row);
                
                if (event_type == mysql::WRITE_ROWS_EVENT) {
                    proc_event(fields, mysql_row);
                } else if(event_type == mysql::UPDATE_ROWS_EVENT) {
                    itor++;
                    mysql::Row_of_fields new_fields = *itor;
                    zval *mysql_new_row;
                    MAKE_STD_ZVAL(mysql_new_row);
                    array_init(mysql_new_row);
                    // old data
                    proc_event(fields, mysql_row);
                    // new data
                    proc_event(new_fields, mysql_new_row);
                    // add new row to zval
                    add_index_zval(mysql_rows, i++, mysql_new_row);
                    
                } else if(event_type == mysql::DELETE_ROWS_EVENT) {
                    proc_event(fields, mysql_row);
                }
                add_index_zval(mysql_rows, i++, mysql_row);
            } while (++itor != rows.end());

            // ensure this is the right place to delete table map event.
            delete MYSQLBINLOG_G(tmev);
            MYSQLBINLOG_G(tmev) = NULL;
            add_assoc_zval(return_value, "rows", mysql_rows);
		}
		break;
    }
    if (event_type != mysql::TABLE_MAP_EVENT) {
        delete event;
    }
}

void proc_event(mysql::Row_of_fields &fields, zval *mysql_fields)
{
  mysql::Converter converter;
  mysql::Row_of_fields::iterator itor = fields.begin();
  int i = 0;
  do {
      mysql::system::enum_field_types type = itor->type();
      if (itor->is_null()) {
          add_index_long(mysql_fields, i++, 0);
      } else if (type == mysql::system::MYSQL_TYPE_FLOAT) {
          add_index_double(mysql_fields, i++, itor->as_float());
      } else if (type == mysql::system::MYSQL_TYPE_DOUBLE) {
          add_index_double(mysql_fields, i++, itor->as_double());
      } else {
          std::string out;
          converter.to(out, *itor);
          add_index_string(mysql_fields, i++, (char *)out.c_str(), 1);
      }
  } while(++itor != fields.end());
}

PHP_FUNCTION(binlog_set_position)
{
    zval *link, *file = NULL;
    int result, id = -1; long position;
    Binary_log *bp;
    
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rl|z!", &link, &position, &file) == FAILURE) {
        RETURN_NULL();
    }

    ZEND_FETCH_RESOURCE(bp, Binary_log *, &link, id, BINLOG_LINK_DESC, le_binloglink);

    if (!bp) {
        php_error_docref(NULL TSRMLS_CC, E_WARNING, "Wrong resource handler passed to binlog_set_position().");
        RETURN_NULL();
    }

    if (!file) {
        result = bp->set_position(position);
    } else if (Z_TYPE_P(file) == IS_STRING) {
        result = bp->set_position(Z_STRVAL_P(file), position);
    } else {
        php_error_docref(NULL TSRMLS_CC, E_WARNING, "filename must be a string");
        RETURN_FALSE;
    }
    switch(result) {
        case ERR_OK:
            RETURN_TRUE;
            break;
        case ERR_EOF:
            RETURN_NULL();
            break;
       default:
           RETURN_FALSE;
           break;
    }
}

PHP_FUNCTION(binlog_get_position)
{
    zval *link, *file = NULL;
    int id = -1;
    Binary_log *bp;
    
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r|z", &link, &file) == FAILURE) {
        RETURN_NULL();
    }

    ZEND_FETCH_RESOURCE(bp, Binary_log *, &link, id, BINLOG_LINK_DESC, le_binloglink);

    if (!bp) {
        php_error_docref(NULL TSRMLS_CC, E_WARNING, "Wrong resource handler passed to binlog_get_position().");
        RETURN_NULL();
    }

    std::string filename;
    
    if (!file || Z_TYPE_P(file) == IS_NULL) {
        RETURN_LONG(bp->get_position());
    } else if (Z_TYPE_P(file) == IS_STRING) {
        filename.assign(Z_STRVAL_P(file));
        RETURN_LONG(bp->get_position(filename));
    } else {
        php_error_docref(NULL TSRMLS_CC, E_WARNING, "filename must be a string");
        RETURN_FALSE;
    }
}

/*
 * Local variables:
 * c-basic-offset: 4
 * End:
 * vim600: et sw=4 ts=4
 * vim<600: et sw=4 ts=4
 */
