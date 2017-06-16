/**
 *  MySqlBinlog.h
 *  Class that is exported to PHP space
 */

/**
 *  Include guard
 */
#pragma once

#include "binlog.h"
#include <iostream>
#include <ostream>
#include <iomanip>
#include <map>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <cstring>
#include <regex.h>
#include <algorithm>

#define MAX_BINLOG_SIZE 1073741824
#define MAX_BINLOG_POSITION MAX_BINLOG_SIZE/4

using namespace std;
using namespace binary_log;
using binary_log::Binary_log;
using binary_log::system::create_transport;
using binary_log::system::Binary_log_driver;
using binary_log::system::Binlog_file_driver;

typedef std::map<long int, binary_log::Table_map_event *> Int2event_map;
typedef std::pair<enum_field_types, std::string> Row_Fields_pair;
typedef std::vector<Row_Fields_pair> Row_Fields_map;
typedef std::vector<Row_Fields_map> Row_map;

const char* get_type_str(Log_event_type type)
{
  switch(type) {
  case binary_log::START_EVENT_V3:  return "Start_v3";
  case binary_log::STOP_EVENT:   return "Stop";
  case binary_log::QUERY_EVENT:  return "Query";
  case binary_log::ROTATE_EVENT: return "Rotate";
  case binary_log::INTVAR_EVENT: return "Intvar";
  case binary_log::LOAD_EVENT:   return "Load";
  case binary_log::NEW_LOAD_EVENT:   return "New_load";
  case binary_log::CREATE_FILE_EVENT: return "Create_file";
  case binary_log::APPEND_BLOCK_EVENT: return "Append_block";
  case binary_log::DELETE_FILE_EVENT: return "Delete_file";
  case binary_log::EXEC_LOAD_EVENT: return "Exec_load";
  case binary_log::RAND_EVENT: return "RAND";
  case binary_log::XID_EVENT: return "Xid";
  case binary_log::USER_VAR_EVENT: return "User var";
  case binary_log::FORMAT_DESCRIPTION_EVENT: return "Format_desc";
  case binary_log::TABLE_MAP_EVENT: return "Table_map";
  case binary_log::PRE_GA_WRITE_ROWS_EVENT: return "Write_rows_event_old";
  case binary_log::PRE_GA_UPDATE_ROWS_EVENT: return "Update_rows_event_old";
  case binary_log::PRE_GA_DELETE_ROWS_EVENT: return "Delete_rows_event_old";
  case binary_log::WRITE_ROWS_EVENT_V1: return "Write_rows_v1";
  case binary_log::UPDATE_ROWS_EVENT_V1: return "Update_rows_v1";
  case binary_log::DELETE_ROWS_EVENT_V1: return "Delete_rows_v1";
  case binary_log::BEGIN_LOAD_QUERY_EVENT: return "Begin_load_query";
  case binary_log::EXECUTE_LOAD_QUERY_EVENT: return "Execute_load_query";
  case binary_log::INCIDENT_EVENT: return "Incident";
  case binary_log::IGNORABLE_LOG_EVENT: return "Ignorable";
  case binary_log::ROWS_QUERY_LOG_EVENT: return "Rows_query";
  case binary_log::WRITE_ROWS_EVENT: return "Write_rows";
  case binary_log::UPDATE_ROWS_EVENT: return "Update_rows";
  case binary_log::DELETE_ROWS_EVENT: return "Delete_rows";
  case binary_log::GTID_LOG_EVENT: return "Gtid";
  case binary_log::ANONYMOUS_GTID_LOG_EVENT: return "Anonymous_Gtid";
  case binary_log::PREVIOUS_GTIDS_LOG_EVENT: return "Previous_gtids";
  case binary_log::HEARTBEAT_LOG_EVENT: return "Heartbeat";
  case binary_log::TRANSACTION_CONTEXT_EVENT: return "Transaction_context";
  case binary_log::VIEW_CHANGE_EVENT: return "View_change";
  //case binary_log::XA_PREPARE_LOG_EVENT: return "XA_prepare";
  default: return "Unknown";                            /* impossible */
  }
}


/**
 *  Class definition
 */
class Master : public Php::Base
{
 private:
  Decoder *decode;
  Binary_log *binlog;
  Binary_log_driver *drv;

  Int2event_map m_table_index;
  //map<int, string> tid2tname;
  map<int, vector<string>> tid2tmap;

  string dsn;
  string opt_filename;
  long int opt_position;


 public:

    /**
     *  Constructor
     */
    Master()
    {
      decode = new Decoder(0);
    }

    /**
     *  Destructor
     */
    ~Master() {
      if(binlog) {
        binlog->disconnect();
        delete binlog;
      }
      if(drv) delete drv;
      if(decode) delete decode;

      Int2event_map::iterator it= m_table_index.begin();
      do {
        if (it->second != NULL) {
          delete it->second;
        }
      } while (++it != m_table_index.end());

      //tid2tname.clear();
      tid2tmap.clear();
    }


    void __construct(Php::Parameters &params) {
      dsn = params[0].stringValue();
    }


    Php::Value connect(Php::Parameters &params) {
      drv = create_transport(dsn.c_str());
      binlog = new Binary_log(drv);
      int error_number= binlog->connect();

      if (const char* msg= str_error(error_number)) {
        cerr << msg << endl;
      }

      if (error_number != ERR_OK) {
          cerr << "Unable to setup connection" << endl;
          return 1;
      }

      return 0;
    }

    Php::Value get_next_event(Php::Parameters &params) {
      Php::Value array; // return it
      Binary_log_event *event;
      long event_start_pos;
      //string event_type;
      //string database_dot_table;
      //map<int, string>::iterator tb_it;
      map<int, vector<string>>::iterator tb_it;

      std::pair<unsigned char *, size_t> buffer_buflen;
      int error_number= drv->get_next_event(&buffer_buflen);

      if (error_number == ERR_OK) {
          const char *error= NULL;
          if (!(event= decode->decode_event((char*)buffer_buflen.first, buffer_buflen.second, &error, 1))) {
              cerr << error << endl;
              throw Php::Exception(string(error));
          }
      } else {
          const char* msg=  str_error(error_number);
          cerr << msg << endl;
          throw Php::Exception(string(msg));
      }

      if (event->get_event_type() == binary_log::INCIDENT_EVENT ||
          (event->get_event_type() == binary_log::ROTATE_EVENT &&
           event->header()->log_pos == 0 ||
           event->header()->log_pos == 0)) {
        /*
          If the event type is an Incident event or a Rotate event
          which occurs when a server starts on a fresh binlog file,
          the event will not be written in the binlog file.
        */
        event_start_pos = 0;

      } else {
        event_start_pos = (event->header()->log_pos) - (event->header())->data_written;
      }

      array["position"] = static_cast<int64_t>(event_start_pos);
      array["type_code"] = event->get_event_type();
      array["type_str"] = get_type_str(event->get_event_type());

      if (event_start_pos >= MAX_BINLOG_POSITION) {
        throw Php::Exception("position exceed max binlog position");
      }

      if (
          event->get_event_type() == binary_log::TABLE_MAP_EVENT ||
          event->get_event_type() == binary_log::PRE_GA_WRITE_ROWS_EVENT ||
          event->get_event_type() == binary_log::PRE_GA_UPDATE_ROWS_EVENT ||
          event->get_event_type() == binary_log::PRE_GA_DELETE_ROWS_EVENT ||
          event->get_event_type() == binary_log::WRITE_ROWS_EVENT ||
          event->get_event_type() == binary_log::WRITE_ROWS_EVENT_V1 ||
          event->get_event_type() == binary_log::UPDATE_ROWS_EVENT ||
          event->get_event_type() == binary_log::UPDATE_ROWS_EVENT_V1 ||
          event->get_event_type() == binary_log::DELETE_ROWS_EVENT ||
          event->get_event_type() == binary_log::DELETE_ROWS_EVENT_V1
          ) {
        if (event->get_event_type() == binary_log::TABLE_MAP_EVENT) {
          // It is a table map event
          Table_map_event *table_map_event= static_cast<Table_map_event*>(event);
          process_event(table_map_event);

          vector<string> dt(2);
          dt[0] = table_map_event->m_dbnam;
          dt[1] = table_map_event->m_tblnam;
          tid2tmap[table_map_event->get_table_id()] = dt;

          array["db_name"] = dt[0];
          array["table_name"] = dt[1];
          return array;

        } else {
          // It is a row event
          Rows_event *row_event= static_cast<Rows_event*>(event);
          Row_map rows_val = process_event(row_event);

          tb_it= tid2tmap.begin();
          tb_it= tid2tmap.find(row_event->get_table_id());
          if (tb_it != tid2tmap.end()) {
            array["db_name"] = tb_it->second[0];
            array["table_name"] = tb_it->second[1];
            if (row_event->get_flags() == Rows_event::STMT_END_F) {
              std::cout << "IN STMT_END_F" << std::endl;
              tid2tmap.erase(tb_it);
              if(m_table_index[tb_it->first] != NULL) {
                delete m_table_index[tb_it->first];
              }
              m_table_index.erase(tb_it->first);
            }
          }

          int j = 0;
          for(Row_map::iterator row_it = rows_val.begin();
              row_it != rows_val.end();
              ++ row_it)
            {
              Php::Array row;
              int i = 0;
              for(Row_Fields_map::iterator filed_it = row_it->begin();
                  filed_it != row_it->end();
                  ++ filed_it) {
                row[i++] = filed_it->second;
              }
              array["rows"][j++] = row;
            }
        }

      }

      if (event->get_event_type() == binary_log::QUERY_EVENT) {
        Query_event *qev = dynamic_cast<Query_event *>(event);
        array["query"]  = qev->query;
        array["db_name"] = qev->db;
        /* std::cout << "Query = " */
        /*           << qev->query */
        /*           << " DB = " */
        /*           << qev->db */
        /*           << std::endl; */
      }

      delete event;
      event = NULL;

      return array;

    }


    /**
     *  Cast to a string
     *  @return const char *
     */
    const char *__toString() const
    {
        return "MySQL Binlog for PHP. Made by BullSoft.org";
    }

    void process_event(Table_map_event *tm)
    {
      m_table_index.insert(Int2event_map::value_type(tm->get_table_id(), tm));
    }

    Row_map process_event(Rows_event *event)
    {
      Row_map rows_val;
      Int2event_map::iterator ti_it= m_table_index.find(event->get_table_id());
      if (ti_it != m_table_index.end()) {
        Row_event_set rows(event, ti_it->second);
        Row_event_set::iterator row_it = rows.begin();
        do {
          Row_Fields_map row_fields_val;
          Row_of_fields fields = *row_it;
          Row_of_fields::iterator field_it = fields.begin();
          Converter converter;
          if (field_it != fields.end()) {
            do {
              std::string str;
              converter.to(str, *field_it);
              Row_Fields_pair field;
              field.first = (*field_it).type();
              field.second = str;
              row_fields_val.push_back(field);
            } while (++field_it != fields.end());
          }
          rows_val.push_back(row_fields_val);
        } while (++row_it != rows.end());

      }
      return rows_val;
    }
};

