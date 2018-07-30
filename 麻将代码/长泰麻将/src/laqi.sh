rm log/majiang.log

killall  changtaimajiang  

#./changtaimajiang  -f conf/

./changtaimajiang -f conf/changtaimajiang.conf -D 

ps  -ef|grep -v grep   | grep   changtaimajiang 
