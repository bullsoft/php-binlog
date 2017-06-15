#include <phpcpp.h>
#include "master.h"
#include <cstdlib>

/**
 *  tell the compiler that the get_module is a pure C function
 */
extern "C" {

    /**
     *  Function that is called by PHP right after the PHP process
     *  has started, and that returns an address of an internal PHP
     *  strucure with all the details and features of your extension
     *
     *  @return void*   a pointer to an address that is understood by PHP
     */
    PHPCPP_EXPORT void *get_module()
    {
        // static(!) Php::Extension object that should stay in memory
        // for the entire duration of the process (that's why it's static)
        static Php::Extension extension("mysqlbinlog", "1.0");

        // we have to class - master
        Php::Class<Master> master("MySqlBinlog");

        master.method<&Master::__construct>("__construct", {
            Php::ByVal("dsn", Php::Type::String),
            Php::ByVal("filename", Php::Type::String, false),
            Php::ByVal("position", Php::Type::Numeric, false)
          });

        master.method<&Master::connect>("connect");
        master.method<&Master::get_next_event>("get_next_event");

        master.property("UNKNOWN_EVENT"             , Log_event_type::UNKNOWN_EVENT            , Php::Const);
        master.property("START_EVENT_V3"            , Log_event_type::START_EVENT_V3           , Php::Const);
        master.property("QUERY_EVENT"               , Log_event_type::QUERY_EVENT              , Php::Const);
        master.property("STOP_EVENT"                , Log_event_type::STOP_EVENT               , Php::Const);
        master.property("ROTATE_EVENT"              , Log_event_type::ROTATE_EVENT             , Php::Const);
        master.property("INTVAR_EVENT"              , Log_event_type::INTVAR_EVENT             , Php::Const);
        master.property("LOAD_EVENT"                , Log_event_type::LOAD_EVENT               , Php::Const);
        master.property("SLAVE_EVENT"               , Log_event_type::SLAVE_EVENT              , Php::Const);
        master.property("CREATE_FILE_EVENT"         , Log_event_type::CREATE_FILE_EVENT        , Php::Const);
        master.property("APPEND_BLOCK_EVENT"        , Log_event_type::APPEND_BLOCK_EVENT       , Php::Const);
        master.property("EXEC_LOAD_EVENT"           , Log_event_type::EXEC_LOAD_EVENT          , Php::Const);
        master.property("DELETE_FILE_EVENT"         , Log_event_type::DELETE_FILE_EVENT        , Php::Const);
        master.property("NEW_LOAD_EVENT"            , Log_event_type::NEW_LOAD_EVENT           , Php::Const);
        master.property("RAND_EVENT"                , Log_event_type::RAND_EVENT               , Php::Const);
        master.property("USER_VAR_EVENT"            , Log_event_type::USER_VAR_EVENT           , Php::Const);
        master.property("FORMAT_DESCRIPTION_EVENT"  , Log_event_type::FORMAT_DESCRIPTION_EVENT , Php::Const);
        master.property("XID_EVENT"                 , Log_event_type::XID_EVENT                , Php::Const);
        master.property("BEGIN_LOAD_QUERY_EVENT"    , Log_event_type::BEGIN_LOAD_QUERY_EVENT   , Php::Const);
        master.property("EXECUTE_LOAD_QUERY_EVENT"  , Log_event_type::EXECUTE_LOAD_QUERY_EVENT , Php::Const);
        master.property("TABLE_MAP_EVENT"           , Log_event_type::TABLE_MAP_EVENT          , Php::Const);
        master.property("PRE_GA_WRITE_ROWS_EVENT"   , Log_event_type::PRE_GA_WRITE_ROWS_EVENT  , Php::Const);
        master.property("PRE_GA_UPDATE_ROWS_EVENT"  , Log_event_type::PRE_GA_UPDATE_ROWS_EVENT , Php::Const);
        master.property("PRE_GA_DELETE_ROWS_EVENT"  , Log_event_type::PRE_GA_DELETE_ROWS_EVENT , Php::Const);
        master.property("WRITE_ROWS_EVENT_V1"       , Log_event_type::WRITE_ROWS_EVENT_V1      , Php::Const);
        master.property("UPDATE_ROWS_EVENT_V1"      , Log_event_type::UPDATE_ROWS_EVENT_V1     , Php::Const);
        master.property("DELETE_ROWS_EVENT_V1"      , Log_event_type::DELETE_ROWS_EVENT_V1     , Php::Const);
        master.property("INCIDENT_EVENT"            , Log_event_type::INCIDENT_EVENT           , Php::Const);
        master.property("HEARTBEAT_LOG_EVENT"       , Log_event_type::HEARTBEAT_LOG_EVENT      , Php::Const);
        master.property("IGNORABLE_LOG_EVENT"       , Log_event_type::IGNORABLE_LOG_EVENT      , Php::Const);
        master.property("ROWS_QUERY_LOG_EVENT"      , Log_event_type::ROWS_QUERY_LOG_EVENT     , Php::Const);
        master.property("WRITE_ROWS_EVENT"          , Log_event_type::WRITE_ROWS_EVENT         , Php::Const);
        master.property("UPDATE_ROWS_EVENT"         , Log_event_type::UPDATE_ROWS_EVENT        , Php::Const);
        master.property("DELETE_ROWS_EVENT"         , Log_event_type::DELETE_ROWS_EVENT        , Php::Const);
        master.property("GTID_LOG_EVENT"            , Log_event_type::GTID_LOG_EVENT           , Php::Const);
        master.property("ANONYMOUS_GTID_LOG_EVENT"  , Log_event_type::ANONYMOUS_GTID_LOG_EVENT , Php::Const);
        master.property("PREVIOUS_GTIDS_LOG_EVENT"  , Log_event_type::PREVIOUS_GTIDS_LOG_EVENT , Php::Const);
        master.property("TRANSACTION_CONTEXT_EVENT" , Log_event_type::TRANSACTION_CONTEXT_EVENT, Php::Const);
        master.property("VIEW_CHANGE_EVENT"         , Log_event_type::VIEW_CHANGE_EVENT        , Php::Const);

        // add all classes to the extension
        extension.add(master);

        extension.onStartup([]() {
          });

        extension.onShutdown([]() {
          });

        extension.onRequest([]() {
          });

        extension.onIdle([]() {
          });


        // return the extension
        return extension;
    }
}
