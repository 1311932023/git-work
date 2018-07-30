#!/bin/bash

CMD=$1

cd protocol
make
cd ..
cd libphz
make clean && make lib
cd ..
cd libcommon
make clean && make lib
cd ..
cd zlib
make clean && make lib
cd ..
make clean && make
killall -w $CMD
#rm log/*
./$CMD -f conf/$CMD.conf -D
