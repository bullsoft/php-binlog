dnl $Id$
dnl config.m4 for extension mysqlbinlog

dnl Comments in this file start with the string 'dnl'.
dnl Remove where necessary. This file will not work
dnl without editing.

dnl If your extension references something external, use with:

PHP_ARG_WITH(mysqlbinlog, for mysqlbinlog support,
dnl Make sure that the comment is aligned:
[  --with-mysqlbinlog             Include mysqlbinlog support])

PHP_ARG_WITH(mysql-replication, for mysql replication api support,
[  --with-mysql-replication=DIR             Include mysql replication api support])

PHP_ARG_WITH(boost, for boost support,
[  --with-boost=DIR                  Include boost support])


if test "$PHP_MYSQLBINLOG" != "no"; then
  dnl Write more examples of tests here...

  dnl # --with-mysqlbinlog -> check with-path
  SEARCH_PATH="/usr/local /usr"     # you might want to change this
  SEARCH_FOR_REPLICATION="/include/binlog_api.h"  
  SEARCH_FOR_BOOST="/lib/libboost_system.so"
    
  if test -r $PHP_MYSQL_REPLICATION/$SEARCH_FOR_REPLICATION; then
    MYSQL_REPLICATION_DIR=$PHP_MYSQL_REPLICATION
  dnl else search default path list
  else
    AC_MSG_CHECKING([for mysql-replication-listener libraries in default path])
    for i in $SEARCH_PATH ; do
      if test -r $i/$SEARCH_FOR_REPLICATION; then
        MYSQL_REPLICATION_DIR=$i
        AC_MSG_RESULT(found in $i)
      fi
    done
  fi
  dnl test replication dir
  if test -z "$MYSQL_REPLICATION_DIR"; then
    AC_MSG_RESULT([not found])
    AC_MSG_ERROR([Please check --with-mysql-replication option])
  fi

  
  if test -r $PHP_BOOST/$SEARCH_FOR_BOOST; then
    BOOST_DIR=$PHP_BOOST
  dnl else search default path list
  else
    AC_MSG_CHECKING([for boost libraries in default path])
    for i in $SEARCH_PATH ; do
      if test -r $i/$SEARCH_FOR_BOOST; then
        BOOST_DIR=$i
        AC_MSG_RESULT(found in $i)
      fi
    done
  fi
  
  dnl test boost dir
  if test -z "$BOOST_DIR"; then
    AC_MSG_RESULT([not found])
    AC_MSG_ERROR([Please check --with-boost option])
  fi
  
  dnl # --enable-mysql-binlog -> add include path
  
  PHP_ADD_INCLUDE($MYSQL_REPLICATION_DIR/include)
  PHP_ADD_INCLUDE($BOOST_DIR/include)

  PHP_REQUIRE_CXX()

  PHP_SUBST(MYSQLBINLOG_SHARED_LIBADD)
  PHP_ADD_LIBRARY(stdc++, 1, MYSQLBINLOG_SHARED_LIBADD)

  PHP_ADD_LIBPATH($MYSQL_REPLICATION_DIR"/lib")
  PHP_ADD_LIBRARY(replication, 1, MYSQLBINLOG_SHARED_LIBADD)

  PHP_ADD_LIBPATH($BOOST_DIR"/lib")
  PHP_ADD_LIBRARY(boost_system, 1, MYSQLBINLOG_SHARED_LIBADD)  
  PHP_NEW_EXTENSION(mysqlbinlog, mysqlbinlog.cpp, $ext_shared)
fi
