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
#include <map>
#include <set>
#include <json/json.h>
#include "common/jpacket.h"

class Client;
class Match;
class Player;

const char* _4float(float f);

//Game类功能：连接上级服务器，并处理client数据
class Game
{
public:
	std::map<int, Client*> 			fd_client_map;
	std::map<int, Client*>			uid_client_map;
	std::map<int, Match*>			all_match_map;		//赛事管理
	Client 							*link_data_client;	//大厅
	int 							server_vid;			//服务器玩法id

	std::map<int, Match*>			del_match_map;

protected:
	int 							m_iGameSrv;
	ev_io 							m_evGameSrv;
	int								m_iSrv[4];	
	ev_io							m_evSrv[4];	
	std::set<std::string>			m_MatchClient;

	ev_timer 						reconnect_datasvr_timer;
	ev_tstamp 						reconnect_datasvr_tstamp;

	ev_tstamp 						del_match_tstamp;
	ev_timer 						del_match_timer;

	Json::Value	prize_json;
	ev_tstamp prize_tstamp;
	ev_timer prize_timer;
public:
	bool period_match;//创建时段赛
	bool qualifier_match;
public:
	Game();
	virtual ~Game();
	void Start();
	void DelClient(Client *client, bool del_oldclient = false);	
	//连接上级服务器
	int ConnectDatasvr();
	//与上级服务器断开后重连机制
	static void ReconnectDataSvrCB(struct ev_loop *loop, struct ev_timer *w, int revents);
	static void AcceptCB(struct ev_loop *loop, struct ev_io *w, int revents);
	static void DelMatchCB(struct ev_loop *loop, struct ev_timer *w, int revents);
	static void PrizeCB(struct ev_loop *loop, struct ev_timer *w, int revents);
	void StartReconnect();
	int ReconnectDataSvr();
	void SendToDataSvr(const std::string &res);	
	void RegisterSvr();
	//void RegisterSvrRes(Json::Value &val);	
	void DelAllClient();
	int DispatchJson(Client *client);
	int DispatchProto(Client *client);
	int JoinMatchReq(Client* client);
	int JoinMatch(int mid, int uid, Client *client);
	int MatchStatusAck(Json::Value &val);
	int MatchStart(Json::Value &val);
	int HandlerPrize(Json::Value &val);
	int HandlerMatchPause(Json::Value &val);
	static void SendLogout(Client *client, int uid, int code);
	int MatchEndRes(int mid);
	int MatchEndRes(Json::Value &val) {
		int matchid = val["mid"].asInt();
		return MatchEndRes(matchid);
	}
	
	void send_to_datasvr(const std::string& res) { return SendToDataSvr(res); }
	void send_to_videosvr(const std::string& res) { }	

	int InitAccept(const char* ip, int port, int fd, ev_io& );	
	
	int DispatchJson2(Client* client);	
	int HandlerMatchClose2(Json::Value &val);
	int DelPlayer2(Player *player, int code);
	void RegisterSvrRes2(Json::Value &val);
	void QuitMatchReq2(Client *client);

	void JoinMatchSuccAck2(Client* org_client, Json::Value &val);
	int JoinMatch2(int matchid, Json::Value &val);
};

#endif
