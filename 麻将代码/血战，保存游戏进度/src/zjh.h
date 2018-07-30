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
#include "common/boostHeader.hpp"

class Game;

class ZJH
{
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive & ar, const unsigned int /* file_version */);

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
	bool				deleteArchive;

	ZJH()
	{
		game = NULL;
	}
};


#include "zjh.hpp"

#endif    /* _LANDLORD_H_ */
