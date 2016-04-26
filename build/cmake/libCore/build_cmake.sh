#!/bin/sh


cmake ../../../libCore
make
cp lib/libMisc.a ../../../Lib/Misc_lib/
