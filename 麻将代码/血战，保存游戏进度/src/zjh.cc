#include <stdio.h>
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
#include "game_serialize.hpp"
#include "common/client.h"
#include "common/daemonize.h"
#include "common/log.h"
#include "libnormalmahjong/game_logic.h"


static int parse_arg(int argc, char **argv);
static int init_conf();
static int set_rlimit(int n);
static int init_redis();

ZJH zjh;
Log log;

const Game* game = zjh.game;
int main(int argc, char **argv)
{
	int ret;
	srand(time(NULL));	
	zjh.deleteArchive = false;
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

	if (zjh.is_daemonize == 1)
		daemonize();
	signal(SIGPIPE, SIG_IGN);

	ret = single_instance_running(zjh.conf.get("pid_file", "conf/normalmahjong.pid").asString().c_str());
	if (ret < 0) {
		log_fatal("File: %s Func: %s Line: %d => single_instance_running.\n", __FILE__, __FUNCTION__, __LINE__);
		exit(1);
    }
	Log::Instance().start("./log/majiang.log", 5, 0, 1, 999999, 10);
	log_info("--start--");

	/*log.start(zjh.conf["log"].get("log_file", "log/normalmahjong.log").asString(),
			zjh.conf["log"].get("level", 5).asInt(),
			zjh.conf["log"].get("console", 0).asInt(),
			zjh.conf["log"].get("rotate", 1).asInt(),
			zjh.conf["log"].get("max_size", 1073741824).asInt(),
			zjh.conf["log"].get("max_file", 10).asInt());*/
	
	set_rlimit(10240);
// 	ret = acp_req(zjh.conf, &RegInit);
// 	log_error("the from fuction ret[%d]\n", ret );
// 	if (ret != 0) {
// 		exit(0);
// 	}
// 	Library *library = new (std::nothrow) Library();
// 	ret = library->init("conf/library.json");
// 	if (ret < 0) {
// 		log_fatal("init card library failed.\n");
// 		exit(1);
// 	}
// 	zjh.library = library;

	if (zjh.conf["main-db"].size() != 0)
	{
		ret = init_redis();
		if (ret < 0) //connect redis
		{
			log_fatal("init redis failed\n");
			exit(1);
		}
	}

// 	ScreenWord * screenword = new(std::nothrow) ScreenWord;
// 	ret = screenword->init("conf/screenword.conf");
// 	if( ret < 0 )	
// 	{
// 		log_fatal("init screenword failed .\n");
// 		exit(1);
// 	}
// 	zjh.p_sreenword = screenword;



	struct ev_loop *loop = ev_default_loop(0);
	zjh.loop = loop;
	

	{
		/*std::string filename = "demofile.txt";*/
		/*std::ifstream ifs;*/
		/*ifs.open(filename.c_str());*/

		if (zjh.deleteArchive)
		{
			zjh.main_rc[0]->command("del archiveStr:%d", 1);
		}

		std::string* archiveStr = new std::string();
		int ret = zjh.main_rc[0]->command("get archiveStr:%d", 1);
		if (ret < 0 || zjh.main_rc[0]->reply->str == NULL) {
			printf("new (std::nothrow) Game()\n");
			zjh.game = new (std::nothrow) Game();
		}
		else {
			printf("ia >> zjh\n");
			*archiveStr = zjh.main_rc[0]->reply->str;
			std::istringstream* archiveStream = new std::istringstream(*archiveStr);
			assert(archiveStream->good());
			boost::archive::text_iarchive* ia = new boost::archive::text_iarchive(*archiveStream);
			(*ia) >> BOOST_SERIALIZATION_NVP(zjh);
			zjh.game->restart();
		}


		zjh.main_rc[0]->command("del archiveStr:%d", 1);

		/*if (ifs)
		{
			{
				printf("ia >> zjh\n");
				boost::archive::binary_iarchive ia(ifs);
				ia >> zjh;
			}
			ifs.close();
			zjh.game->restart();
		}
		else {
			printf("new (std::nothrow) Game()\n");
			zjh.game = new (std::nothrow) Game();
		}*/
		game = zjh.game;
	}
	
	zjh.game->Start();

// 	if ( zjh.conf["open_redis"].asInt() )
// 	{
// 		CRedisCache::GetInstance().StartClearTime(zjh.conf["timers"]["redis_call_ptime"].asInt());
// 	}	


	ev_loop(loop, 0);
	return 0;
}

static int parse_arg(int argc, char **argv)
{
	int flag = 0;
	int oc; /* option chacb. */
	char ic; /* invalid chacb. */

	zjh.is_daemonize = 0;
	while ((oc = getopt(argc, argv, "Dyf:")) != -1) {
		switch (oc) {
		case 'D':
			zjh.is_daemonize = 1;
			break;
		case 'f':
			flag = 1;
			zjh.conf_file = string(optarg);
			break;
		case 'y':
			zjh.deleteArchive = true;
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
	bool ret = reader.parse(in, zjh.conf);
	if (!ret) {
		in.close();
		std::cout << "init file parser." << endl;
		return -1;
	}

	in.close();
	return 0;
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
    return 0;
}
