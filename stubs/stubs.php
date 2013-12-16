<?php
define('BINLOG_UNKNOWN_EVENT', 0);
define('BINLOG_START_EVENT_V3', 1);
define('BINLOG_QUERY_EVENT', 2);
define('BINLOG_STOP_EVENT', 3);
define('BINLOG_ROTATE_EVENT', 4);
define('BINLOG_INTVAR_EVENT', 5);
define('BINLOG_LOAD_EVENT', 6);
define('BINLOG_SLAVE_EVENT', 7);
define('BINLOG_CREATE_FILE_EVENT', 8);
define('BINLOG_APPEND_BLOCK_EVENT', 9);
define('BINLOG_EXEC_LOAD_EVENT', 10);
define('BINLOG_DELETE_FILE_EVENT', 11);

define('BINLOG_NEW_LOAD_EVENT', 12);
define('BINLOG_RAND_EVENT', 13);
define('BINLOG_USER_VAR_EVENT', 14);
define('BINLOG_FORMAT_DESCRIPTION_EVENT', 15);
define('BINLOG_XID_EVENT', 16);
define('BINLOG_BEGIN_LOAD_QUERY_EVENT', 17);
define('BINLOG_EXECUTE_LOAD_QUERY_EVENT', 18);
define('BINLOG_TABLE_MAP_EVENT', 19);

define('BINLOG_PRE_GA_WRITE_ROWS_EVENT', 20);
define('BINLOG_PRE_GA_UPDATE_ROWS_EVENT', 21);
define('BINLOG_PRE_GA_DELETE_ROWS_EVENT', 22);

define('BINLOG_WRITE_ROWS_EVENT', 23);
define('BINLOG_UPDATE_ROWS_EVENT', 24);
define('BINLOG_DELETE_ROWS_EVENT', 25);

define('BINLOG_INCIDENT_EVENT', 26);

define('BINLOG_USER_DEFINED', 27);

/**
 * @param string $connect_str
 * @param int    $server_id
 *
 * @return resource
 */
function binlog_connect($connect_str, $server_id = 1)
{
}

/**
 * @param resource $link
 *
 * @return void
 *
 * @throws Exception
 */
function binlog_disconnect($link)
{
}

/**
 * @param resource        $link
 * @param string|string[] $db_watch_list
 * @param string|string[] $table_wild_watch_list
 *
 * @return null|array
 */
function binlog_wait_for_next_event($link, $db_watch_list = null, $table_wild_watch_list = null)
{
}

/**
 * @param resource $link
 * @param int      $position
 * @param string   $filename
 *
 * @return null|true|false
 */
function binlog_set_position($link, $position, $filename = null)
{
}

/**
 * @param resource $link
 * @param string   $filename
 *
 * @return null|int
 */
function binlog_getPosition($link, $filename = null)
{
}