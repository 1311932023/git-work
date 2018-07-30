#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include <errno.h>

#include "log.h"
#include "../proto/proto.h"

#include "player.h"
#include "client.h"

extern Log log;

struct ev_loop* (Player::loop) = NULL;
GAME_DEL_PLAYER (Player::del_player) = NULL;
Json::Value (Player::conf);

InitPlayer::InitPlayer(struct ev_loop *zjh_loop, Json::Value& zjh_conf, GAME_DEL_PLAYER game_del_player)
{	
	Player::loop = zjh_loop;
	Player::del_player = game_del_player;
	Player::conf = zjh_conf;

}
Player::Player()
	: _offline_timeout(60 * 20)
{
	_offline_timer.data = this;
	ev_timer_init(&_offline_timer, Player::offline_timeout, _offline_timeout, 0);	
		
	seatid = -1;
	uid = 0;
	sex = 0;
	exp = 0;
	rmb = 0;
	money = 0;
	coin = 0;	
	pcount = 0;	
	vlevel = 0;
	client = NULL;	
	logout_type = 0;	
	down_tag = 0;	
	usertype = 0;	
    status = 0;
	DisBandCount = 0;	
	useless = -1;
}

Player::~Player()
{  
	if (useless == 0)
	{
		log_error("%s delete multi times uid:%d\n", __FUNCTION__, uid);
	}
	_offline_timer.data = NULL;
	ev_timer_stop(loop, &_offline_timer);
	if (client)
	{
		client->player = NULL;	
	}  
	useless = 0;
}



int Player::init(const Json::Value &val)
{
	
	reset();

	uid = val["uid"].asInt();
	skey = val["skey"].asString().empty() ? "" : val["skey"].asString();
	name = val["name"].asString().empty() ? "" : val["name"].asString();
	sex = val["sex"].asInt();
	avatar = val["avatar"].asString().empty() ? "" : val["avatar"].asString();
	exp = val["exp"].asInt();

	money = val["money"].asDouble();	
	vlevel = val["vlevel"].asInt();
	usertype = val["user_type"].asInt();
    rmb = val["rmb"].asInt();
	if (conf["tables"]["max_money"].asInt() == 0)
	{
		if (money < conf["tables"]["min_money"].asInt()) 
		{
			log_error("player init uid[%d] money[%d] is not fit.\n", uid, money);
			return -1;
		}
	} 
	else 
	{
		if (money < conf["tables"]["min_money"].asInt() || money >= conf["tables"]["max_money"].asInt()) 
		{
			log_error("player init uid[%d] money[%d] is not fit.\n", uid, money);
			return -1;
		}
	}

	tid_str = to_char(-1);	
	status = STAND_UP;
	return 0;
}

void Player::reset()
{
	logout_type = 0;
	//time_cnt = 0;
	stop_offline_timer();
}

void Player::clear()
{
	
}


void Player::set_client(Client *c)
{
	client = c;	
	client->player = this;
}


void Player::start_offline_timer()
{
	ev_timer_start(loop, &_offline_timer);
}

void Player::stop_offline_timer()
{
	ev_timer_stop(loop, &_offline_timer);
}

void Player::offline_timeout(struct ev_loop *loop, ev_timer *w, int revents)
{	
	Player* self = (Player*)w->data;
	if (self != NULL)
	{
		log_warn("%s del_player %d\n", __FUNCTION__, self->uid);
		ev_timer_stop(loop, &self->_offline_timer);
		del_player(self);		
	}
}
void Player::update_info( long alter_money, int alter_total_board, int alter_total_win, int alter_exp )
{
	long old_money = money;
	money += alter_money;
	if( money < 0 )
	{
		money = 0;
	}	
	alter_exp += alter_exp;
	log_info("%s uid:%d old_money:%d alter_money:%d money:%d\n", __FUNCTION__, uid, old_money, alter_money, money );
}
