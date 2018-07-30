#ifndef _GAME_H_
#define _GAME_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <ev++.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>

#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <set>
#include "proto/loginproto.pb.h"
#include "common/jpacket.h"

class Client;
class Player;
class Table;


class Game
{
public:
	Client *link_data_client;
	Client *link_video_client;
	std::string game_name;
	int game_number;

	std::map<std::string, Table*>						all_tables;		//主键roomid，通过roomid找桌子
	std::map<int, Client*>								uid_client_map;			//
	std::map<int, Client*>								fd_client_map;
	std::map<std::string, std::vector<Client*> >		tid_client_map;//桌子未初始化时预登陆客户端



private:
	ev_io _ev_accept;
	int _fd;	

	ev_timer	ev_reconnect_datasvr_timer;
	ev_tstamp   ev_reconnect_datasvr_tstamp;

	ev_timer    ev_delall_client_timer;
	ev_tstamp   ev_delall_client_tstamp;

	int server_vid;
	int server_zid;

public:
	Game();
	virtual ~Game();
	int start();
	int connect_datasvr();
	void register_server();
	static void reconnect_datasvr_cb(struct ev_loop *loop, struct ev_timer *w, int revents);
	//static void del_table_cb(struct ev_loop *loop, struct ev_timer *w, int revents);

	void del_table();
	void start_reconnect();
	int reconnect_datasvr();
	void send_to_datasvr(const std::string& res);
	void del_all_client();

	//玩家请求登陆当前游戏
	int request_login(Client *client);
	int login_succ_back(Json::Value& val);

	int login_error_back(int uid, Json::Value& val, int code);
	static void accept_cb(struct ev_loop *loop, struct ev_io *w, int revents);
	void del_client(Client *client, bool del_oldclient = false);
	//处理客户端发送的消息
	int dispatch_client(Client *client);
	int dispatch_server(Client *client);

//	int safe_check(Client *client, int cmd);
	int handler_login_table(int uid, Client* client, Json::Value& val);
	int login_table(int uid, Player *player, Json::Value &val);
	int handle_logout_table(int tid);
	//用户进出桌子消息发送data_server
	void inform_datasvr_user_vid(int uid, int inorout);

	int del_player(Player *player);
	int change_table(Player *player);

	//void init_buff_head(char* buff, int CmdID, int BodySize);
	int check_skey(Client *client);
	int check_zid(Client *client);

	int tabel_setup(Client *client);
	void table_config();

	//开始游戏
	void game_start_res(Jpacket& packet);
	//玩家请求进入对战桌面
	int dz_request_login(Client *client);
	// 对战模式登录成功返回 
	int dz_login_succ_back(Json::Value& val);
	int dz_login_error_back(int uid, int code);
	int dz_login_reconnect(Client* client, Player* player);
	//将玩家请求信息发送data_server
	int send_request_login(Client* client);

	void game_user_offline(Player *player);
	
	void robot_success(Player *player);

//	void delete_table(const std::string& roomID);
	//是否可以删除房间查询

	bool DeleteRoomReq(const std::string& roomid);
	//发送登出房间协议给客户端
	
	void SendLogout(Client *client, int uid, int code);
	//发送登录失败
	
	void SendLoginFail(Client *client, int uid, int code);
	
	int request_room_conf(Client *client, const std::string& roomid);
	
	int handler_room_conf_res(Json::Value &val);
	
	void player_offline_timeout(Player *player);
	//登出 服务器
	
	void logout_room_res(const Json::Value& val);
	//获取所有连接客户端的数量
	
	void sys_get_user_num(Client *client);
public:
	ev_timer	ev_reconnect_videosvr_timer;
	ev_tstamp   ev_reconnect_videosvr_tstamp;

	int connect_videosvr();
	void start_videoreconnect();
	int reconnect_videosvr();		
	void send_to_videosvr(const std::string &res);
	static void reconnect_videosvr_cb(struct ev_loop *loop, struct ev_timer *w, int revents);
private:
	int init_table();

	int init_accept();

};

#endif
