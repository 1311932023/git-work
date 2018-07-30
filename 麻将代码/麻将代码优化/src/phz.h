#ifndef _PHZ_H_
#define _PHZ_H_

#include <string>
#include <json/json.h>
#include <ev.h>

class RedisClient;
class Game;
class GameLogic;

class PHZ
{
public:
	std::string         conf_file;
	int                 is_daemonize;
	Json::Value         conf;
	Game                *game;
	struct ev_loop      *loop;
	RedisClient     	*phz_rc;
	GameLogic 			*m_GameLogic;
};

#endif
