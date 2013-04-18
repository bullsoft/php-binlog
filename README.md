PHP Binlog
==========

A PHP-Client for mysql replication listener API.

That's what we want to do.

Dependence
=========
* Boost (> 1.39)
* MySQL Replication Listerner Library
* PHP 5.3.X

Install
=========
/home/work/local/php/bin/phpize

./confingure --with-php-config=/home/work/local/php/bin/php-config --with-mysqlbinlog=yes --with-boost=/home/work/boost --with-mysql-replication=/home/work/mysql-replication

make

make install

Example
==========
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
=======================================
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
========
MySQL Replication Listener:

http://cdn.oreillystatic.com/en/assets/1/event/61/Binary%20log%20API_%20A%20Library%20for%20Change%20Data%20Capture%20using%20MySQL%20Presentation.pdf
http://dev.mysql.com/doc/internals/en/index.html

