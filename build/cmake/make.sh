#!/bin/sh

make
cp lib/libMisc.a ../../Lib/Misc_lib/
cp -rf ../../../Net/include ../../../Lib/Misc_lib/
