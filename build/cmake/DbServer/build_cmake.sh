#!/bin/sh


cmake ../../../DbServer
make

cp bin/DbServer ../../../bin/linux/db
