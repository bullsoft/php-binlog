/**
 *  Pinyin.h
 *  Class that is exported to PHP space
 */

/**
 *  Include guard
 */
#pragma once

#include "binlog.h"
#include "MyContentHandler.h"

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
using binary_log::Binary_log;
using binary_log::system::create_transport;
using binary_log::system::Binary_log_driver;
using binary_log::system::Binlog_file_driver;

typedef std::map<long int, binary_log::Table_map_event *> Int2event_map;

/**
 *  Class definition
 */
class Master : public Php::Base
{
 private:
  Decoder *decode;
  Binary_log *binlog;
  string dsn;
  Binary_log_driver *drv;

  Int2event_map m_table_index;
  int m_table_id;
  map<int, string> tid2tname;

  string filename;
  long int position;

 public:

  Row_Fields_map row_fields_val;
  Row_map rows_val;

    /**
     *  Constructor
     */
    Master()
    {
      decode = new Decoder(1);
    }

    /**
     *  Destructor
     */
    ~Master() {
      if(binlog) delete binlog;
      if(drv) delete drv;
      if(decode) delete decode;

      Int2event_map::iterator it= m_table_index.begin();
      do {
        if (it->second != NULL) {
          delete it->second;
        }
      } while (++it != m_table_index.end());
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
      Binary_log_event *event;
      long event_start_pos;
      string event_type;
      string database_dot_table;

      std::pair<unsigned char *, size_t> buffer_buflen;
      map<int, string>::iterator tb_it;

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
          Table_map_event *table_map_event= static_cast<Table_map_event*>(event);
          database_dot_table = table_map_event->m_dbnam;
          database_dot_table.append(".");
          database_dot_table.append(table_map_event->m_tblnam);
          tid2tname[table_map_event->get_table_id()]= database_dot_table;
          process_event(table_map_event);

          return 0;

        } else {
          // It is a row event
          Rows_event *row_event= static_cast<Rows_event*>(event);
          tb_it= tid2tname.begin();
          tb_it= tid2tname.find(row_event->get_table_id());
          if (tb_it != tid2tname.end()) {
            database_dot_table= tb_it->second;
            if (row_event->get_flags() == Rows_event::STMT_END_F) {
              tid2tname.erase(tb_it);
            }
          }

          process_event(row_event);

          for(Row_map::iterator row_it = rows_val.begin();
              row_it != rows_val.end();
              ++ row_it)
            {
              for(Row_Fields_map::iterator filed_it = row_it->begin();
                  filed_it != row_it->end();
                  ++ filed_it)
                {
                  std::cout << filed_it->second << "\t";
                }
              std::cout << std::endl;
            }
          std::cout << std::endl;

        }

      }

      if (event->get_event_type() == binary_log::QUERY_EVENT) {
        binary_log::Query_event *qev = dynamic_cast<binary_log::Query_event *>(event);
        std::cout << "Query = "
                  << qev->query
                  << " DB = "
                  << qev->db
                  << std::endl;
      }

      delete event;
      event = NULL;

      return 0;

    }


    /**
     *  Cast to a string
     *  @return const char *
     */
    const char *__toString() const
    {
        return "MySQL Binlog for PHP. Made by BullSoft.org";
    }

    Binary_log_event* process_event(Table_map_event *tm)
    {
      m_table_id = tm->get_table_id();
      m_table_index.insert(Int2event_map::value_type(tm->get_table_id(), tm));
      return 0;
    }

    Binary_log_event* process_event(Rows_event *event)
    {
      rows_val.clear();
      Int2event_map::iterator ti_it= m_table_index.find(event->get_table_id());
      if (ti_it != m_table_index.end()) {
        Row_event_set rows(event, ti_it->second);
        Row_event_set::iterator row_it = rows.begin();
        do {
          row_fields_val.clear();
          binary_log::Row_of_fields fields = *row_it;
          binary_log::Row_of_fields::iterator field_it = fields.begin();
          binary_log::Converter converter;
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

      } //end if
      return event;
    }
};

