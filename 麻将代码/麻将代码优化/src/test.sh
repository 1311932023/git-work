#!/bin/bash
count=0
while [ true ];do
	echo $count
	count=$(($count+1))
	./stop.sh
	sleep 1
	./start.sh
	sleep 10
done
