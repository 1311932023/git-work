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
#include <sstream>

#include "zjh.h"
#include "game.h"
#include "proto/proto.h"
#include "common/log.h"
#include "common/client.h"
#include "player.h"
#include "table.h"
#include "match.h"
#include "redis_client.h"
#include "sd_match.h"

extern ZJH zjh;
extern Log log;
extern Match* last_match;

const char* _4float(float f)
{
	static char number[100];
	sprintf(number, "%.4f", f);
	return number;
}
int Game::DispatchJson2(Client *client)
{
	int cmd = client->packet.safe_check();
	if (cmd < 0) {
		log_error("packet safe check error\n");
		return -1;
	}

	Json::Value &val = client->packet.tojson();
	log_info("%s client fd:%d cmd:%d\n", __FUNCTION__, client->fd, cmd);
	int matchid = 0;
	try {
		matchid = val["mid"].asInt();
		if (all_match_map.find(matchid) != all_match_map.end())
			last_match = all_match_map[matchid];
	}
	catch (...) {
		log_error("%s catch error\n", __FUNCTION__);
		return -1;
	}
	try
	{
		switch (cmd)
		{
		case CLIENT_QUIT_MATCH_REQ:
			QuitMatchReq2(client);
			return 0;
		case SERVER_JOIN_MATCH_SUCC:
			JoinMatchSuccAck2(client->packet.tojson());
			return 0;
		case SERVER_LAST_RANK_RES:
			MatchEndRes(client->packet.tojson());
			return 0;
		case SERVER_REGISTER_RES:
			RegisterSvrRes2(client->packet.tojson());
			return 0;
		case SERVER_MATCH_CLOSE_RES:
			HandlerMatchClose2(client->packet.tojson());
			return 0;
		default:
			break;
		}
	}
	catch(...)
	{ }
	return 0;
}



//平台通知赛事关闭
int Game::HandlerMatchClose2(Json::Value &val)
{
	int mid = val["mid"].asInt();
	if (all_match_map.find(mid) == all_match_map.end()){
		log_error("match[%d] close error\n", mid);
		return -1;
	}
	Match *match = all_match_map[mid];
	match->m_State = MATCH_OVER;	
	if (match->all_tables.size() == match->m_TurnOverTables.size()) {
		match->MatchOver();
	}
	return 0;
}
void Game::QuitMatchReq2(Client *client)
{
	Json::Value &val = client->packet.tojson();
	int uid = val["uid"].asInt();
	//int mid = val["mid"].asInt();

	SendLogout(client, uid, 106);
	return;
}

int Game::DelPlayer2(Player *player, int code)
{
	if (player == NULL) {
		return -1;
	}
	if (player->client) {
		Client *client = player->client;
		client->player = NULL;
		SendLogout(client, player->uid, code);
		//Client::pre_destroy(client);
	}
	if (player->table) {
		Table *table = player->table;
		table->handler_downtable(player);
		log_info("%s del tid:%d table:%p\n", __FUNCTION__, table->tid, table);	
	}
	
	log_debug("%s player:%p uid:%d\n", __FUNCTION__, player, player->uid);
	delete player;
	player = NULL;
	return 0;
}



void Game::RegisterSvrRes2(Json::Value &val)
{
	if (val["gameId"].asInt() != server_vid || val["status"].asInt() != 200) {
		log_error("register gamesvr error[%d] code[%s]\n", val["status"].asInt(), val["statusMsg"].asString().c_str());
		RegisterSvr();
	}
}

//平台验证进入比赛成功返回
void Game::JoinMatchSuccAck2(Json::Value &val)
{
	int tmpCode = val["status"].asInt();
	int tmpMid = val["mid"].asInt();
	int tmpUid = val["uid"].asInt();
	//验证登录失败
	if (tmpCode == 0) {
		map<int, Client*>::iterator iter = fd_client_map.find(tmpUid);
		if (iter == fd_client_map.end()) {
			log_error("the player uid:%d socket is not in map.\n", tmpUid);
			return;
		}
		Client *client = iter->second;
		SendLogout(client, tmpUid, 104);
		Client::pre_destroy(client);
		return;
	}
	map<int, Client*>::iterator iter = uid_client_map.find(tmpUid);
	if (iter == uid_client_map.end()) {
		log_error("the player uid:%d socket is not in map.\n", tmpUid);
		return;
	}
	//获取redis数据
	int ret = zjh.main_rc[0]->command("get minfo:%d:%d", tmpMid, tmpUid);
	if (ret < 0) {
		log_error("get minfo[%d] error\n", tmpMid);
		return;
	}
	if (zjh.main_rc[0]->reply->str == NULL) {
		log_error("get minfo[%d] str error\n", tmpMid);
		return;
	}
	std::string strminfo = zjh.main_rc[0]->reply->str;
	Json::Reader reader;
	Json::Value mval;
	if (reader.parse(strminfo, mval) < 0) {
		log_error("parse minfo[%d] error\n", tmpMid);
		return;
	}
	log_debug("get redis minfo[%s]\n", mval.toStyledString().c_str());
	zjh.main_rc[0]->command("del minfo:%d:%d", tmpMid, tmpUid);
	Client *client = iter->second;
	uid_client_map.erase(tmpUid);
	
	int mid = mval["minfo"]["mid"].asInt();
	int uid = mval["uinfo"]["uid"].asInt();	
	Player *player = NULL;
	if (all_match_map.find(mid) != all_match_map.end())
	{
		Match* match = all_match_map[mid];
		if (match->all_players.find(uid) != match->all_players.end())
		{
			player = match->all_players[uid];
		}
	}	
	if (player == NULL)
	{
		player = new (std::nothrow) Player();
		if (player == NULL) {
			log_error("new player err");
			return;
		}
		ret = player->Init(mval["uinfo"]);
		if (ret < 0) {
			log_error("init player error\n");
			delete player;
			return;
		}
		log_info("%s one player uid:%d tid:%d new succ\n", __FUNCTION__, tmpUid, player->tid);
	}
	else
	{
		player->ResetMoney();
	}
	
	ret = JoinMatch2(mid, mval);
	if (ret < 0) {
		log_error("%s player:%d join match error\n", __FUNCTION__, tmpUid);		
		Client::pre_destroy(client);		
		return;
	}
	player->SetClient(client);	
	player->m_isRobot = false;

	if (all_match_map.find(mid) != all_match_map.end())
	{
		Match* match = all_match_map[mid];
		if (player->tid == 0)
		{
			player->tid = ++(match->m_tmpTid);	
			player->tmp_playerid = player->tid;
		}
	}
	
	Json::Value table_val = all_match_map[mid]->GetTableConfigure(mval["rinfo"]);
	
	ret = all_match_map[mid]->HandlerJoinMatch(mid, player, table_val);
	if (ret < 0){
		log_error("%s player:%d join match %d error\n", __FUNCTION__, tmpUid);
		Client::pre_destroy(client);
	}		
}

int Game::JoinMatch2(int matchid, Json::Value &val)
{	
	if (all_match_map.find(matchid) == all_match_map.end()) {
		Match *match = new(std::nothrow)SDMatch();
		if (match == NULL) {
			log_error("new match error\n");
			return -1;
		}
		all_match_map[matchid] = match;
		int ret = match->Init2(val);
		if (ret < 0) {
			log_error("init match error\n");
			delete match;
			all_match_map.erase(matchid);
			return -1;
		}
	}
	return 0;
}



