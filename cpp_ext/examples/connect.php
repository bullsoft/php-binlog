<?php

$binlog = new MySqlBinlog("mysql://root:123456@localhost");
$binlog->connect();

while(true) {
    $binlog->get_next_event();
}