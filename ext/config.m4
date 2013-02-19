dnl $Id$
dnl config.m4 for extension mysql_binlog

PHP_ARG_ENABLE(mysql-binlog, whether to enable mysql binlog,
[  --enable-mysql-binlog             Enable mysql binlog for php])

PHP_ARG_WITH(mysql-replication, for mysql replication api support,
[  --with-mysql-replication=DIR             Include mysql replication api support])

PHP_ARG_WITH(boost, for boost support,
[  --with-boost=DIR                  Include boost support])

if test "$PHP_MYSQL_BINLOG" != "no"; then
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

  dnl # --enable-mysql-binlog -> add include path
  
  PHP_ADD_INCLUDE($MYSQL_BINLOG_DIR/include)
  PHP_ADD_INCLUDE($PHP_BOOST/include)

  PHP_REQUIRE_CXX()

  PHP_SUBST(MYSQL_BINLOG_SHARED_LIBADD)
  PHP_ADD_LIBRARY(stdc++, 1, MYSQL_BINLOG_SHARED_LIBADD)
  PHP_ADD_LIBRARY(replication, 1, $MYSQL_BINLOG_DIR/lib)
  PHP_NEW_EXTENSION(mysql_binlog, mysql_binlog.cpp, $ext_shared)
fi
