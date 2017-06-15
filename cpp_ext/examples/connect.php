<?php

$binlog = new MySqlBinlog("mysql://root:123456@localhost");
$binlog->connect();

while(true) {
    $d = $binlog->get_next_event();
    var_dump($d);
}