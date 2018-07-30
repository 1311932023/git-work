#ifndef _LANDLORD_H_
#define _LANDLORD_H_

#include <stdio.h>
#include <stdint.h>
#include <iostream>
#include <fstream>
#include <string>
#include <json/json.h>
#include "redis_client.h"
#include <sstream>
//#include "screenword.h"
//#include "common/library.h"

class Game;

class ZJH
{
public:
	std::string         conf_file;
	int                 is_daemonize;
	Json::Value         conf;
	Game                *game;
	struct ev_loop      *loop;
	//Library             *library;
	//ScreenWord          *p_sreenword;
 	RedisClient			*main_rc[20];
// 	RedisClient         *eventlog_rc;   // cfc add  20140102
	int					main_size;
	int                 pub_size;
//	RedisClient        *pub_rc[10];

private:
};

#endif    /* _LANDLORD_H_ */
