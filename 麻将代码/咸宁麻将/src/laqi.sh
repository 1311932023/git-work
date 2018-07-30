rm log/majiang.log

killall  xianningmajiang  

#./xianningmajiang  -f conf/

./xianningmajiang -f conf/xianningmajiang.conf -D 

ps  -ef|grep -v grep   | grep   xianningmajiang 
