#!/bin/bash

CMD=$1

scp -P 12800 $CMD wanbeilog@114.55.110.124:/home/wanbeilog/$CMD
