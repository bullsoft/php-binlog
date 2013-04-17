PHP Binlog
==========

A PHP-Client for mysql replication listener API.

That's what we want to do.

Dependence
=========
* Boost (> 1.39)
* MySQL Replication Listerner Library
* PHP 5.3

Install
=========
/home/work/local/php/bin/phpize

./confingure --with-php-config=/home/work/local/php/php-config --with-mysqlbinlog=yes --with-boost=/home/work/boost --with-mysql-replication=/home/work/mysql-replication

make

make install

Example
==========
```php
$link = binlog_connect("mysql://root@127.0.0.1:3306");
// binlog_set_position($link, 4);                                       

while($event=binlog_wait_for_next_event($link)) {
    // it will block here                                               
    switch($event['type_code']) {
        case BINLOG_DELETE_ROWS_EVENT:
            // do what u want ...                                       
            break;
        case BINLOG_WRITE_ROWS_EVENT:
            // do what u want ...                                       
            break;
        case BINLOG_UPDATE_ROWS_EVENT:
            // do what u want ...                                       
            break;
    }
}
```


Reference
========
MySQL Replication Listener:

http://cdn.oreillystatic.com/en/assets/1/event/61/Binary%20log%20API_%20A%20Library%20for%20Change%20Data%20Capture%20using%20MySQL%20Presentation.pdf


