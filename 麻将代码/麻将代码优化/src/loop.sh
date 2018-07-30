#!/bin/bash	

CMD=$1

while(true)
do
	killall -w $CMD
	echo "kill $CMD"
	sleep 2
	./$CMD -f conf/$CMD.conf -D
	echo "restart $CMD"
	sleep 15
done
