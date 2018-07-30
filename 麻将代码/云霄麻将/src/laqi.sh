rm log/majiang.log

killall  yunxiaomajiang  

#./yunxiaomajiang  -f conf/

./yunxiaomajiang -f conf/yunxiaomj.conf -D 

ps  -ef|grep -v grep   | grep   yunxiaomajiang 
