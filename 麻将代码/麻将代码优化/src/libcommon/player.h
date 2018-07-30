#ifndef _PLAYER_H_
#define _PLAYER_H_

#include <json/json.h>
#include <ev.h>
#include <ev++.h>
#include "log.h"
#include "client.h"
#include "game.h"
#include "../phz.h"
#include "gameproto.pb.h"
class Client;

class Player
{		
	public:
		std::string			tid;
		int					seatid;
		int                 uid;
		std::string			skey;
		std::string			name;
		int					sex;
		std::string			avatar;
		int					usertype;
		int					exp;
		int					rmb;
		long				money;
		double              latitude;
    	double              longitude;
    	std::string         city;
    	int                 gps_state;  //0:未更新  1:已更新
		Client              *client;
		int 				logout_type;
		int					dissolve_cnt;
		int					dissovle_state;//0--反对解散 1--同意解散 2--未操作
		int					is_offline;
	private:
		ev_timer			_offline_timer;
		ev_tstamp			_offline_timeout;

		ev_timer			_del_player_timer;
		ev_tstamp			_del_player_tstamp;

	public:
		Player();
		virtual ~Player();
		void set_client(Client *c);
		//初始化和重置
		int init( Json::Value &val);
		void reset();
		void clear();
		//掉线玩家定时器
		void start_offline_timer();
		void stop_offline_timer();
		static void offline_timeout(struct ev_loop *loop, ev_timer *w, int revents);
		//删除玩家定时器
		void start_del_player_timer();
		void stop_del_player_timer();
		static void del_player_timer_cb(struct ev_loop *loop, ev_timer *w, int revents);
		int InitData(const proto::game::tagPlayers &pUserInfo);
		void update_info( long alter_money, int alter_total_board, int alter_total_win, int alter_exp );
	
};

#endif
