dnl $Id$
dnl config.m4 for extension mysql_binlog

dnl Comments in this file start with the string 'dnl'.
dnl Remove where necessary. This file will not work
dnl without editing.

PHP_ARG_ENABLE(mysql-binlog, whether to enable mysql binlog,
[  --enable-mysql-binlog             Enable mysql binlog for php])

dnl If your extension references something external, use with:

PHP_ARG_WITH(mysql-replication, for mysql replication api support,
dnl Make sure that the comment is aligned:
[  --with-mysql-replication=DIR             Include mysql replication api support])

PHP_ARG_WITH(boost, for boost support,
[  --with-boost=DIR                  Include boost support])

if test "$PHP_MYSQL_BINLOG" != "no"; then
  dnl Write more examples of tests here...

  dnl # --with-mysql_binlog -> check with-path
  SEARCH_PATH="/usr/local /usr /local /opt"
  SEARCH_FOR="/include/binlog_api.h"
  dnl check $PHP_MYSQL_REPLICATION first
  if test -r $PHP_MYSQL_REPLICATION/$SEARCH_FOR; then
    MYSQL_BINLOG_DIR=$PHP_MYSQL_REPLICATION
  dnl else search default path list
  else
    AC_MSG_CHECKING([for mysql_binlog files in default path])
    for i in $SEARCH_PATH ; do
      if test -r $i/$SEARCH_FOR; then
        MYSQL_BINLOG_DIR=$i
        AC_MSG_RESULT(found in $i)
      fi
    done
  fi
  dnl
  if test -z "$MYSQL_BINLOG_DIR"; then
    AC_MSG_RESULT([not found])
    AC_MSG_ERROR([Please reinstall the mysql_binlog distribution])
  fi

  dnl # --with-mysql_binlog -> add include path
  
  PHP_ADD_INCLUDE($MYSQL_BINLOG_DIR/include)
  PHP_ADD_INCLUDE($PHP_BOOST/include)

  PHP_REQUIRE_CXX()

  PHP_SUBST(MYSQL_BINLOG_SHARED_LIBADD)
  PHP_ADD_LIBRARY(stdc++, 1, MYSQL_BINLOG_SHARED_LIBADD)
  PHP_ADD_LIBRARY(replication, 1, $MYSQL_BINLOG_DIR/lib)
  PHP_NEW_EXTENSION(mysql_binlog, mysql_binlog.cpp, $ext_shared)
fi
