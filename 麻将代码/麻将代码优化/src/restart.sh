#!/bin/bash

CMD=$1

killall -w $CMD
#rm log/*
./$CMD -f conf/$CMD.conf -D

