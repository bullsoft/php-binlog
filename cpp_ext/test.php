<?php
dl('mysqlbinlog.so');

echo ReflectionExtension::export("mysqlbinlog");
