#ifndef _PLAYER_H_
#define _PLAYER_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <ev++.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <errno.h>
#include <iostream>
#include <fstream>
#include <string>

#include <json/json.h>
#include <ev.h>

class Client;
class Player;

typedef int (*GAME_DEL_PLAYER)(Player *player);

class InitPlayer
{
public:
	InitPlayer(struct ev_loop *zjh_loop, Json::Value& zjh_conf, GAME_DEL_PLAYER game_del_player);
};

class Player
{		
public:
	Client              *client;	
	std::string	        tid_str;	
	std::string			skey;
	std::string			name;	
	std::string			avatar;	
	int					seatid;	
	int                 uid;
	int					sex;
	int					exp;
	int					rmb;
	int					money;
	int					coin;	
	int					pcount;	
	int					vlevel;	
	int                 status;		
	int 				logout_type;	
	int             down_tag;   // game end down table	
	int             usertype;	
    int             DisBandCount;           //解散桌子次数
private:
    ev_timer       _offline_timer;
    ev_tstamp      _offline_timeout;
	int useless;
public:
	Player();
	~Player();
	void set_client(Client *c);
	int init(const Json::Value &val);
	
	void reset();
	void clear();

	void start_offline_timer();
	void stop_offline_timer();
	static void offline_timeout(struct ev_loop *loop, ev_timer *w, int revents);

	void update_info( long alter_money, int alter_total_board, int alter_total_win, int alter_exp );

private:
	static struct ev_loop *loop;	
	static Json::Value conf;
	static GAME_DEL_PLAYER del_player;
	
	friend class InitPlayer;
};

#endif
