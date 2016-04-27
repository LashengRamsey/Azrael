#!/bin/sh


cmake ../../../libCore
make
cp lib/libCore.a ../../../lib/linux
