﻿#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <unistd.h>
#include <errno.h>

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <ev.h>

#include "zjh.h"
#include "game.h"
#include "common/daemonize.h"
#include "common/log.h"
#include "libnormalmahjong/acp.h"
#include "libnormalmahjong/game_logic.h"
#include "redis_cache.h"


static int parse_arg(int argc, char **argv);
static int init_conf();
static void dump_conf();
static int set_rlimit(int n);
static int init_redis();

ZJH zjh;
Log log;

int main(int argc, char **argv)
{
	int ret;
	srand(time(NULL));	
	ret = parse_arg(argc, argv);
	if (ret < 0) {
		log_fatal("File: %s Func: %s Line: %d => parse_arg.\n", __FILE__, __FUNCTION__, __LINE__);
		exit(1);
	}
	ret = init_conf();
	if (ret < 0) {
		log_fatal("File: %s Func: %s Line: %d => init_conf.\n", __FILE__, __FUNCTION__, __LINE__);
		exit(1);
	}
	/*ת������*/
	dump_conf();

	if (zjh.is_daemonize == 1)
		daemonize();
	signal(SIGPIPE, SIG_IGN);

	ret = single_instance_running(zjh.conf.get("pid_file", "conf/normalmahjong.pid").asString().c_str());
	if (ret < 0) {
		log_fatal("File: %s Func: %s Line: %d => single_instance_running.\n", __FILE__, __FUNCTION__, __LINE__);
		exit(1);
    }
	Log::Instance().start("./log/majiang.log", 5, 0,  1, 999999, 10 );
	log_info("--start--"); 

	//log.start(zjh.conf["log"].get("log_file", "log/normalmahjong.log").asString(),
	//		zjh.conf["log"].get("level", 5).asInt(),
	//		zjh.conf["log"].get("console", 0).asInt(),
	//		zjh.conf["log"].get("rotate", 1).asInt(),
	//		zjh.conf["log"].get("max_size", 1073741824).asInt(),
	//		zjh.conf["log"].get("max_file", 10).asInt());
	
	set_rlimit(10240);
// 	ret = acp_req(zjh.conf, &RegInit);
// 	log_error("the from fuction ret[%d]\n", ret );
// 	if (ret != 0) {
// 		exit(0);
// 	}
	Library *library = new (std::nothrow) Library();
	ret = library->init("conf/library.json");
	if (ret < 0) {
		log_fatal("init card library failed.\n");
		exit(1);
	}
	zjh.library = library;

	if (zjh.conf["open_redis"].asInt())
	{
		ret = init_redis();
		if (ret < 0) //connect redis
		{
			log_fatal("init redis failed\n");
			exit(1);
		}
	}

	ScreenWord * screenword = new(std::nothrow) ScreenWord;
	ret = screenword->init("conf/screenword.conf");
	if( ret < 0 )	
	{
		log_fatal("init screenword failed .\n");
		exit(1);
	}
	zjh.p_sreenword = screenword;

	struct ev_loop *loop = ev_default_loop(0);
	zjh.loop = loop;
	zjh.game = new (std::nothrow) Game();
	zjh.game->start();

	if ( zjh.conf["open_redis"].asInt() )
	{
		CRedisCache::GetInstance().StartClearTime(zjh.conf["timers"]["redis_call_ptime"].asInt());
	}	

	ev_loop(loop, 0);
	return 0;
}

static int parse_arg(int argc, char **argv)
{
	int flag = 0;
	int oc; /* option chacb. */
	char ic; /* invalid chacb. */

	/*�Ƿ�Ϊ�ػ�����*/
	zjh.is_daemonize = 0;
	while ((oc = getopt(argc, argv, "Df:")) != -1) {
		switch (oc) {
		case 'D':
			zjh.is_daemonize = 1;
			break;
		case 'f':
			flag = 1;
			/*optargָ���������������ļ�����*/
			zjh.conf_file = string(optarg);
			break;
		case '?':
			ic = (char) optopt;
			printf("invalid \'%c\'\n", ic);
			break;
		case ':':
			printf("lack option arg\n");
			break;
		}
	}

	if (flag == 0)
		return -1;

	return 0;
}

static int init_conf()
{
	std::ifstream in(zjh.conf_file.c_str(), std::ifstream::binary);
	if (!in) {
		std::cout << "init file no found." << endl;
		return -1;
	}

	Json::Reader reader;
	/*��json�����ļ���������*/
	bool ret = reader.parse(in, zjh.conf);
	if (!ret) {
		in.close();
		std::cout << "init file parser." << endl;
		return -1;
	}

	in.close();
	return 0;
}

static void dump_conf()
{
	std::cout << "pid_file: "
        << zjh.conf.get("pid_file", "conf/bankerdn.pid").asString() << endl;
	
	std::cout << "log:log_file: "
        << zjh.conf["log"].get("log_file", "log/bankerdn.log").asString()
        << endl;
	std::cout << "log:level: "
        << zjh.conf["log"].get("level", 4).asInt() << endl;
	std::cout << "log:console: "
        << zjh.conf["log"].get("console", 0).asInt() << endl;
	std::cout << "log:rotate: "
        << zjh.conf["log"].get("rotate", 1).asInt() << endl;
	std::cout << "log:max_size: "
        << zjh.conf["log"].get("max_size", 1073741824).asInt() << endl;
	std::cout << "log:max_file: "
        << zjh.conf["log"].get("max_file", 50).asInt() << endl;

	std::cout << "game:host: "
        << zjh.conf["game"]["host"].asString() << endl;
	std::cout << "game:port: "
        << zjh.conf["game"]["port"].asInt() << endl;
	
	
	std::cout << "tables:begin: "
        << zjh.conf["tables"]["begin"].asInt() << endl;
	std::cout << "tables:end: "
        << zjh.conf["tables"]["end"].asInt() << endl;
	std::cout << "tables:min_money: "
        << zjh.conf["tables"]["min_money"].asInt() << endl;
	std::cout << "tables:max_money: "
        << zjh.conf["tables"]["max_money"].asInt() << endl;
	std::cout << "tables:base_money: "
        << zjh.conf["tables"]["base_money"].asInt() << endl;
	std::cout << "tables:min_round: "
        << zjh.conf["tables"]["min_round"].asInt() << endl;
	std::cout << "tables:max_round: "
        << zjh.conf["tables"]["max_round"].asInt() << endl;
	std::cout << "tables:vid: "
        << zjh.conf["tables"]["vid"].asInt() << endl;
	std::cout << "tables:zid: "
        << zjh.conf["tables"]["zid"].asInt() << endl;
	std::cout << "tables:type: "
        << zjh.conf["tables"]["type"].asInt() << endl;
	std::cout << "tables:play_number: "
		<< zjh.conf["tables"]["play_num"].asInt()<<endl;
	std::cout << "tables:first_charge: "
		<< zjh.conf["tables"]["first_charge"].asInt()<<endl;
	std::cout<<"table:wai_fee: "
		<<atof( zjh.conf["tables"]["wai_fee"].asString().c_str() )<<endl;
	std::cout<<"table:restitution_num: "
		<< zjh.conf["tables"]["restitution_num"].asInt() <<endl;
	std::cout<<"table:ban_cardstype_ratio: "
		<< zjh.conf["tables"]["ban_cardstype_ratio"].asInt()<<endl;
	std::cout << "tables:chose_bet_flag: "
		<< zjh.conf["tables"]["chose_bet_flag"].asInt()<<endl;
	std::cout << "tables:multi_flag: "
		<< zjh.conf["tables"]["multi_flag"].asInt()<<endl;

	unsigned int peak_size = zjh.conf["tables"]["uptable_peak"].size();
	std::cout <<"tables:uptable_peak: "<< peak_size<<endl;
	for( unsigned int i = 0; i < peak_size; i++ )
	{
		int start = zjh.conf["tables"]["uptable_peak"][i]["start_hour"].asInt();
		int end = zjh.conf["tables"]["uptable_peak"][i]["end_hour"].asInt();
		int limit =  zjh.conf["tables"]["uptable_peak"][i]["limit_money"].asInt();
		int stand = zjh.conf["tables"]["uptable_peak"][i]["stand_money"].asInt();
		std::cout <<"peak: "<< i << " start_hour: "<<start
			<<" end_hour: "<< end << " limit_money: "<<limit <<" stand_money: "<< stand <<endl;
	}
}

static int set_rlimit(int n)
{
	struct rlimit rt;
	rt.rlim_max = rt.rlim_cur = n;
	if (setrlimit(RLIMIT_NOFILE, &rt) == -1) {
		log_error("File: %s Func: %s Line: %d => setrlimit %s.\n", __FILE__, __FUNCTION__, __LINE__, strerror(errno));
		return -1;
	}

	return 0;
}


static int init_redis()
{
	int ret;
	
	zjh.main_size = zjh.conf["main-db"].size();
	for (int i = 0; i < zjh.main_size; i++) {
		zjh.main_rc[i] = new RedisClient();
		ret = zjh.main_rc[i]->init(zjh.conf["main-db"][i]["host"].asString(),
				zjh.conf["main-db"][i]["port"].asInt(), 1000, zjh.conf["main-db"][i]["pass"].asString());
		if (ret < 0) {
			log_error("main db redis error\n");
			return -1;
		}
	}

//     zjh.eventlog_rc = new RedisClient();
//     ret = zjh.eventlog_rc->init(zjh.conf["eventlog-db"]["host"].asString(),
//     		zjh.conf["eventlog-db"]["port"].asInt(), 1000, zjh.conf["eventlog-db"]["pass"].asString());
//     if (ret < 0) {
//     	log_error("eventlog db redis error.\n");
//     	return -1;
//     }
// 
//     // init pub
//     zjh.pub_size = zjh.conf["pub-db"].size();
//     for (int i = 0; i < zjh.pub_size; ++i) {
//     	zjh.pub_rc[i] = new RedisClient();
// 		ret = zjh.pub_rc[i]->init(zjh.conf["pub-db"][i]["host"].asString(),
// 				zjh.conf["pub-db"][i]["port"].asInt(), 1000, zjh.conf["pub-db"][i]["pass"].asString());
// 		if (ret < 0) {
// 			log_error("pub_rc db redis error.\n");
// 			return -1;
// 		}
// 	}
	
    return 0;
}
