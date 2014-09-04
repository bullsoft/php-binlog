PHP Binlog
==========

_We have deployed it in produtive enviroment, and I posted an article for it (but in Chinese), http://guweigang.com/blog/2013/11/18/mysql-binlog-in-realtime/. Hope it will help you._
      
---------------------------------------

A PHP-Client for mysql replication listener API.

You can use it to connect to a MYSQL server which produces BINLOG, and get BINLOG events in real-time, just like a Async-Trigger.

You know, we always need to use different components to do a good job, serve a good service. Just like we use MYSQL to storage all our data, use REDIS as a cache server, and use LUCENE as a search engine. The differences between MYSQL and other components will affect the sorting result, and therefore affect user experience, especially for time-sensitive service.

Our group use PHP to develop web projects. So if we have a PHP extension to do the data trigger job between different components, everything will be easy. This is what we do and why we do.

Dependence
--------------------
* MySQL Replication Listener Library (https://github.com/BullSoft/mysql-replication-listener)
* PHP 5.3.X

Install
--------------------
First, install mysql-replication-listener.

$ cmake . -DCMAKE_INSTALL_PREFIX=/home/work/mysql-replication

$ make && make install

Then change to php-binlog ext directory,

$ /home/work/local/php/bin/phpize

$ ./confingure --with-php-config=/home/work/local/php/bin/php-config --with-mysql-binlog=/home/work/mysql-replication

$ make

$ make install

Example
----------------
注：Binlog为行格式

```php
$link = binlog_connect("mysql://root@127.0.0.1:3306");
// binlog_set_position($link, 4);                           

while($event=binlog_wait_for_next_event($link)) {
    // it will block here                                   
    switch($event['type_code']) {
        case BINLOG_DELETE_ROWS_EVENT:
            var_dump($event);
            // do what u want ...                           
            break;
        case BINLOG_WRITE_ROWS_EVENT:
            var_dump($event);
            // do what u want ...                           
            break;
        case BINLOG_UPDATE_ROWS_EVENT:
            var_dump($event);
            // do what u want ...                           
            break;
        default:
            // var_dump($event);                            
            break;
    }
}
```

### Update_rows

```sql
update `type` set type_id = 22 WHERE id in (58, 59);
```
```php
array(5) {
  'type_code' =>
  int(24)
  'type_str' =>
  string(11) "Update_rows"
  'db_name' =>
  string(5) "cloud"
  'table_name' =>
  string(4) "type"
  'rows' =>
  array(4) {
    [0] =>
    array(5) {
      [0] =>
      string(2) "58"
      [1] =>
      string(8) "adsfasdf"
      [2] =>
      string(4) "asdf"
      [3] =>
      string(2) "22"
      [4] =>
      string(1) "0"
    }
    [1] =>
    array(5) {
      [0] =>
      string(2) "58"
      [1] =>
      string(8) "adsfasdf"
      [2] =>
      string(4) "asdf"
      [3] =>
      string(1) "4"
      [4] =>
      string(1) "0"
    }
    [2] =>
    array(5) {
      [0] =>
      string(2) "59"
      [1] =>
      string(8) "adsfasdf"
      [2] =>
      string(4) "asdf"
      [3] =>
      string(2) "22"
      [4] =>
      string(1) "0"
    }
    [3] =>
    array(5) {
      [0] =>
      string(2) "59"
      [1] =>
      string(8) "adsfasdf"
      [2] =>
      string(4) "asdf"
      [3] =>
      string(1) "4"
      [4] =>
      string(1) "0"
    }
  }
}
```
### Delete_rows

```sql
delete from `type` WHERE id in (58, 59);
```

```php
array(5) {
  'type_code' =>
  int(25)
  'type_str' =>
  string(11) "Delete_rows"
  'db_name' =>
  string(5) "cloud"
  'table_name' =>
  string(4) "type"
  'rows' =>
  array(2) {
    [0] =>
    array(5) {
      [0] =>
      string(2) "58"
      [1] =>
      string(8) "adsfasdf"
      [2] =>
      string(4) "asdf"
      [3] =>
      string(2) "22"
      [4] =>
      string(1) "0"
    }
    [1] =>
    array(5) {
      [0] =>
      string(2) "59"
      [1] =>
      string(8) "adsfasdf"
      [2] =>
      string(4) "asdf"
      [3] =>
      string(2) "22"
      [4] =>
      string(1) "0"
    }
  }
}
```
### Write_rows

```sql
insert into type values (Null, "Hello, World", "Best world", 4, 0), (NULL, "你好，世界", "世界很美好", 3, 5);
```

```php
array(5) {
  'type_code' =>
  int(23)
  'type_str' =>
  string(10) "Write_rows"
  'db_name' =>
  string(5) "cloud"
  'table_name' =>
  string(4) "type"
  'rows' =>
  array(2) {
    [0] =>
    array(5) {
      [0] =>
      string(2) "95"
      [1] =>
      string(12) "Hello, World"
      [2] =>
      string(10) "Best world"
      [3] =>
      string(1) "4"
      [4] =>
      string(1) "0"
    }
    [1] =>
    array(5) {
      [0] =>
      string(2) "96"
      [1] =>
      string(15) "你好，世界"
      [2] =>
      string(15) "世界很美好"
      [3] =>
      string(1) "3"
      [4] =>
      string(1) "5"
    }
  }
}
```

Reference
--------------------
MySQL Replication Listener:

http://cdn.oreillystatic.com/en/assets/1/event/61/Binary%20log%20API_%20A%20Library%20for%20Change%20Data%20Capture%20using%20MySQL%20Presentation.pdf

http://dev.mysql.com/doc/internals/en/index.html

http://dev.mysql.com/doc/refman/5.1/en/mysqlbinlog-row-events.html

http://dev.mysql.com/doc/internals/en/replication-protocol.html


[![endorse](https://api.coderwall.com/cnwggu/endorsecount.png)](https://coderwall.com/cnwggu)



Feedback
-------------------------

Issues and contributions are welcome.

Thank you!
