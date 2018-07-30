#!/bin/bash

cd common
make clean && make -j 
cd ..

cd libnormalmahjong
make clean && make -j 
cd ..

cd proto
#make clean;  make  -j  
sh   mk.sh  
cd ..

make clean && make -j 
