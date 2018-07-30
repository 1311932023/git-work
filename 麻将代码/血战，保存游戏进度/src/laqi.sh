rm log/majiang.log -f

./killProc

#./mjcommonTestYangdian  -f conf/

./xuezhanmatch -f conf/xuezhanmatch.conf -D 

ps  -ef|grep -v grep   | grep   xuezhanmatch 
