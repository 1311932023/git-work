#!/bin/bash

CMD=$"pingxiangmajiang"

./$CMD -f ./conf/$CMD.conf -D
ps -ef|grep $CMD
