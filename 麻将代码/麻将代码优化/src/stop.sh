#!/bin/bash

CMD=$"pingxiangmajiang"

kill -9 `ps -ef|grep $CMD|grep -v 'grep'|awk 'NR==1{print $2}'`
