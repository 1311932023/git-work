#ifndef _GAME_H_
#define _GAME_H_

#include <string.h>
#include <stdint.h>
#include <ev++.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <stdarg.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include <sstream>
#include <iostream>
#include <fstream>
#include <string>
#include <map>

#include "../phz.h"
#include "log.h"
#include "proto.h"
#include "client.h"
#include "player.h"
#include "loginproto.pb.h"
//#include "gameproto.pb.h"
#include "common.h"
#include "table.h"

class Client;
class Player;
class Table;

class Game
{
	public:
		Client		*link_data_client;
		Client		*link_video_client;
		std::map<std::string, Table*>	all_tables;
		std::map<int, Client*>			uid_client_map;
		std::map<int, Client*>			fd_client_map;
		int			server_vid;
		int			server_zid;
	private:
		ev_io		_ev_accept;
		int			_fd;

		ev_timer	ev_reconnect_datasvr_timer;
		ev_tstamp   ev_reconnect_datasvr_tstamp;

		ev_timer	ev_reconnect_videosvr_timer;
		ev_tstamp   ev_reconnect_videosvr_tstamp;

	private:
		int init_accept();
		static void accept_cb(struct ev_loop *loop, struct ev_io *w, int revents);
		//连接数据服
		int connect_datasvr();
		void start_reconnect();
		static void reconnect_datasvr_cb(struct ev_loop *loop, struct ev_timer *w, int revents);
		int reconnect_datasvr();
		//连接录像服	
		int connect_videosvr();
		void start_videoreconnect();
		int reconnect_videosvr();
		static void reconnect_videosvr_cb(struct ev_loop *loop, struct ev_timer *w, int revents);
	public:
		Game();
		virtual ~Game();
		void start();
		//断开所有客户端
		void del_all_client();
		//登录
		int request_login( Client *client );
		int login_succ_back( Json::Value &val );
		int login_error_back(int uid, int code );
		int dz_login_error_back( Client *client, int code);
		//断开客户端
		void del_client(Client *client, bool del_oldclient=false);
		//删除玩家
		int del_player(Player *player);
		//平台协议
		int dispatch_json(Client *client);
		//客户端协议
		int dispatch_proto(Client *client);
		//安全检测
		int safe_check(Client *client, int cmd);
		//登录桌子
		int handler_login_table(Client *client, Json::Value &val);
		int login_table( Client *client, Json::Value &val);
		int login_table(Client *client, std::map<int, Table*> &a, std::map<int, Table*> &b);
		//游戏开始返回
		void game_start_res( Jpacket &packet );
		//发送登出房间
		void SendLogout(Client *client, int uid, int code);
		int table_no_players(std::string _roomid);
		//发送数据给平台
		void send_to_datasvr( const std::string &res );
		void send_to_videosvr(const std::string &res);
		//后台请求删除房间
		bool DeleteRoomReq(std::string roomid);
		//防崩存入
		int SaveDataToRedis(std::string &tid, std::string &data_str);
		//防崩取出
		int RequestDataFromRedis();
		//防崩删除
		int DelDataFromRedis(std::string &tid);
};

#endif
