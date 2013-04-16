php-binlog
==========

A php-client for mysql replication listener API

That's what we want to do.

Dependence
=========
* Boost (> 1.39)
* stdc++
* MySQL Replication Listerner Library


Install
=========
/home/work/local/php/bin/phpize

./confingure --with-php-config=/home/work/local/php/php-config --with-mysqlbinlog=yes --with-boost=/home/work/boost --with-mysql-replication=/home/work/mysql-replication

make

make install



Reference
========
MySQL Replication Listener:

http://cdn.oreillystatic.com/en/assets/1/event/61/Binary%20log%20API_%20A%20Library%20for%20Change%20Data%20Capture%20using%20MySQL%20Presentation.pdf


