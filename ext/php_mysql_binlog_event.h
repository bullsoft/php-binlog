#ifndef PHP_MYSQL_BINLOG_EVENT_H
#define PHP_MYSQL_BINLOG_EVENT_H

#include "php_mysql_binlog.h"

extern VALUE php_cBinlogEvent;

namespace php {
namespace binlog {

struct Event {
  mysql::Log_event_header *m_event_header;

  static VALUE get_marker(VALUE self);
  static VALUE get_timestamp(VALUE self);
  static VALUE get_type_code(VALUE self);
  static VALUE get_server_id(VALUE self);
  static VALUE get_event_length(VALUE self);
  static VALUE get_next_position(VALUE self);
  static VALUE get_flags(VALUE self);
  static VALUE get_event_type(VALUE self);
  static void init(VALUE clazz);
};

struct QueryEvent : public Event {
  mysql::Query_event *m_event;

  static void free(QueryEvent *p);
  static VALUE alloc(VALUE klass);
  static void set_event(VALUE self, mysql::Binary_log_event *event);
  static void init();

  static VALUE get_thread_id(VALUE self);
  static VALUE get_exec_time(VALUE self);
  static VALUE get_error_code(VALUE self);
  static VALUE get_variables(VALUE self);
  static VALUE get_db_name(VALUE self);
  static VALUE get_query(VALUE self);
};

struct RotateEvent : public Event {
  mysql::Rotate_event *m_event;

  static void free(RotateEvent *p);
  static VALUE alloc(VALUE klass);
  static void set_event(VALUE self, mysql::Binary_log_event *event);
  static void init();

  static VALUE get_binlog_file(VALUE self);
  static VALUE get_binlog_pos(VALUE self);
};

struct FormatEvent : public Event {
  mysql::Format_event *m_event;

  static void free(FormatEvent *p);
  static VALUE alloc(VALUE klass);
  static void set_event(VALUE self, mysql::Binary_log_event *event);
  static void init();

  static VALUE get_binlog_version(VALUE self);
  static VALUE get_master_version(VALUE self);
  static VALUE get_created_ts(VALUE self);
  static VALUE get_log_header_len(VALUE self);
};

struct UserVarEvent : public Event {
  mysql::User_var_event *m_event;

  static void free(UserVarEvent *p);
  static VALUE alloc(VALUE klass);
  static void set_event(VALUE self, mysql::Binary_log_event *event);
  static void init();

  static VALUE get_name(VALUE self);
  static VALUE get_is_null(VALUE self);
  static VALUE get_type(VALUE self);
  static VALUE get_charset(VALUE self);
  static VALUE get_value(VALUE self);
};

struct TableMapEvent : public Event {
  mysql::Table_map_event *m_event;

  static void free(TableMapEvent *p);
  static VALUE alloc(VALUE klass);
  static void set_event(VALUE self, mysql::Binary_log_event *event);
  static void init();

  static VALUE get_table_id(VALUE self);
  static VALUE get_flags(VALUE self);
  static VALUE get_db_name(VALUE self);
  static VALUE get_table_name(VALUE self);
  static VALUE get_columns(VALUE self);
  static VALUE get_column_types(VALUE self);
  static VALUE get_metadata(VALUE self);
  static VALUE get_null_bits(VALUE self);
};

struct RowEvent : public Event {
  mysql::Row_event *m_event;
  VALUE m_table_map;

  static void free(RowEvent *p);
  static void mark(RowEvent *p);
  static VALUE alloc(VALUE klass);
  static void set_event(VALUE self, mysql::Binary_log_event *event, VALUE table_map);
  static void init();

  static VALUE get_table_id(VALUE self);
  static VALUE get_db_name(VALUE self);
  static VALUE get_table_name(VALUE self);
  static VALUE get_column_types(VALUE self);
  static VALUE get_flags(VALUE self);
  static VALUE get_columns_len(VALUE self);
  static VALUE get_null_bits_len(VALUE self);
  static VALUE get_columns_before_image(VALUE self);
  static VALUE get_used_columns(VALUE self);
  static VALUE get_row(VALUE self);
  static VALUE get_rows(VALUE self);

private:
  static void proc0(mysql::Row_of_fields &fields, VALUE rb_fields);
  static VALUE proc_insert(mysql::Row_of_fields &fields);
  static VALUE proc_update(mysql::Row_of_fields &old_fields, mysql::Row_of_fields &new_fields);
  static VALUE proc_delete(mysql::Row_of_fields &fields);
};

struct IntVarEvent : public Event {
  mysql::Int_var_event *m_event;

  static void free(IntVarEvent *p);
  static VALUE alloc(VALUE klass);
  static void set_event(VALUE self, mysql::Binary_log_event *event);
  static void init();

  static VALUE get_type(VALUE self);
  static VALUE get_value(VALUE self);
};

struct IncidentEvent : public Event {
  mysql::Incident_event *m_event;

  static void free(IncidentEvent *p);
  static VALUE alloc(VALUE klass);
  static void set_event(VALUE self, mysql::Binary_log_event *event);
  static void init();

  static VALUE get_type(VALUE self);
  static VALUE get_message(VALUE self);
};

struct XidEvent : public Event {
  mysql::Xid *m_event;

  static void free(XidEvent *p);
  static VALUE alloc(VALUE klass);
  static void set_event(VALUE self, mysql::Binary_log_event *event);
  static void init();

  static VALUE get_xid_id(VALUE self);
};

struct UnimplementedEvent : public Event {
  mysql::Binary_log_event *m_event;

  static void free(QueryEvent *p);
  static VALUE alloc(VALUE klass);
  static void set_event(VALUE self, mysql::Binary_log_event *event);
  static void init();
};

} // namespace binlog
} // namespace php

#endif // PHP_BINLOG_EVENT_H
