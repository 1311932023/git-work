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
#include "daemonize.h"
#include "phz.h"
#include "game.h"
#include "log.h"
#include "redis_client.h"

static int parse_arg(int argc, char **argv);
static int init_conf();
static int set_rlimit(int n);
static int init_redis();

PHZ phz;
Log log;

int main(int argc, char **argv)
{
	int ret;
	ret = parse_arg(argc, argv);
	if (ret < 0) {
		log.fatal("File: %s Func: %s Line: %d => parse_arg.\n",	__FILE__, __FUNCTION__, __LINE__);
		exit(1);
	}
	ret = init_conf();
	if (ret < 0) {
		log.fatal("File: %s Func: %s Line: %d => init_conf.\n",	__FILE__, __FUNCTION__, __LINE__);
		exit(1);
	}

	if (phz.is_daemonize == 1)
		daemonize();
	signal(SIGPIPE, SIG_IGN);

	ret = single_instance_running(phz.conf.get("pid_file", "conf/yuanjiangzpghz.pid").asString().c_str());
	if (ret < 0) {
		log.fatal("File: %s Func: %s Line: %d => single_instance_running.\n", __FILE__, __FUNCTION__, __LINE__);
		exit(1);
	}
	log.start(phz.conf["log"].get("log_file", "log/yuanjiangzpghz.log").asString(),
				phz.conf["log"].get("level", 4).asInt(),
				phz.conf["log"].get("console", 0).asInt(),
				phz.conf["log"].get("rotate", 1).asInt(),
				phz.conf["log"].get("max_size", 1073741824).asInt(),
				phz.conf["log"].get("max_file", 50).asInt());

	set_rlimit(10240);
	ret = init_redis();
	if (ret < 0) { 
		log.fatal("init redis failed\n");
		exit(1);
	}
	struct ev_loop *loop = ev_default_loop(0);
	phz.loop = loop;
	//phz.m_GameLogic = GameLogic::CreateLocationLogic(phz.conf["table"]["logic"].asString());
	phz.game = new (std::nothrow) Game();
	phz.game->start();
	ev_loop(loop, 0);
	return 0;
}

static int parse_arg(int argc, char **argv)
{
	int flag = 0;
	int oc; /* option chacb. */
	char ic; /* invalid chacb. */

	phz.is_daemonize = 0;
	while((oc = getopt(argc, argv, "Df:")) != -1) {
		switch(oc) {
			case 'D':
				phz.is_daemonize = 1;
				break;
			case 'f':
				flag = 1;
				phz.conf_file = string(optarg);
				break;
			case '?':
				ic = (char)optopt;
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
	std::ifstream in(phz.conf_file.c_str(), std::ifstream::binary);
	if (!in) {
		std::cout << "init file no found." << endl;
		return -1;
	}

	Json::Reader reader;
	bool ret = reader.parse(in, phz.conf);
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
		log.error("File: %s Func: %s Line: %d => setrlimit %s.\n",
							__FILE__, __FUNCTION__, __LINE__, strerror(errno));
		return -1;
	}
	return 0;
}

static int init_redis()
{
	int ret;
	phz.phz_rc = new RedisClient();
	ret = phz.phz_rc->init(phz.conf["redis-db"]["host"].asString(),
			phz.conf["redis-db"]["port"].asInt(), 1000, phz.conf["redis-db"]["pass"].asString());
	if (ret < 0) {
		log.error("phz db redis error.\n");
		return -1;
	}
	return 0;
}
