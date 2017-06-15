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
