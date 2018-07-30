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
#include "common/log.h"
#include "game.h"
#include "proto/proto.h"
#include "common/client.h"
#include "match.h"
#include "rj_match.h"
#include "rr_match.h"
#include "rr_waiwei_match.h"

#include "table.h"
#include "redis_client.h"
#include "common/client.h"

extern ZJH zjh;
extern Log log;

Match* last_match = NULL;
Table* last_table = NULL;
bool g_period_match = false;
bool g_room_card_score = false;

static void game_del_client(Client *client, bool del_oldclient)
{
	zjh.game->DelClient(client, del_oldclient);
}
static int game_dispatch_client(Client *client)
{
	try
	{
		return zjh.game->DispatchProto(client);
	}
	//try {}
	catch (...) {}
	return -1;
}
static int game_dispatch_server(Client *client)
{
	if (zjh.game->period_match)
		return zjh.game->DispatchJson2(client);
	else
		return zjh.game->DispatchJson(client);
}
static int game_send_filter(Client* client, std::list<Buffer*>& _write_q)
{		
	return 0;
}



Game::Game() :
	reconnect_datasvr_tstamp(0.3),
	del_match_tstamp(300)
{
	m_iGameSrv = -1;
	link_data_client = NULL;
	reconnect_datasvr_timer.data = this;
	ev_timer_init( &reconnect_datasvr_timer, Game::ReconnectDataSvrCB, reconnect_datasvr_tstamp, reconnect_datasvr_tstamp );
	del_match_timer.data = this;
	ev_timer_init(&del_match_timer, Game::DelMatchCB, del_match_tstamp, del_match_tstamp);		
	server_vid = zjh.conf["tables"]["vid"].asInt();
	ev_timer_start(zjh.loop, &del_match_timer);

	prize_timer.data = this;
	prize_tstamp = 6.0f;
	ev_timer_init(&prize_timer, Game::PrizeCB, prize_tstamp, prize_tstamp);

	period_match = false;
	qualifier_match = false;
}
Game::~Game()
{
	if( NULL != link_data_client )
	{
		delete link_data_client;
	}
	ev_timer_stop(zjh.loop, &reconnect_datasvr_timer);
	ev_timer_stop(zjh.loop, &del_match_timer);
	ev_timer_stop(zjh.loop, &prize_timer);
}

void Game::Start()
{
	static InitClient c(zjh.loop, game_del_client, game_dispatch_client, game_dispatch_server, game_send_filter);

	try
	{
		ConnectDatasvr();

		InitAccept(zjh.conf["game"]["host"].asString().c_str(), zjh.conf["game"]["port"].asInt(), m_iGameSrv, m_evGameSrv);

		Json::Value match_srv = zjh.conf["match-srv"];
		for (int i = 0; i < match_srv.size(); i++)
		{
			InitAccept(match_srv[i]["host"].asString().c_str(), match_srv[i]["port"].asInt(), m_iSrv[i], m_evSrv[i]);
		}
		for (int i =0; i < zjh.conf["match-client"]["host"].size(); i++)
		{
			m_MatchClient.insert(zjh.conf["match-client"]["host"][i].asString());
		}		
				
		int mode = zjh.conf["mode"].asInt();
		if (mode == 0)
		{
			period_match = false;
			qualifier_match = false;
		}
		else if (mode == 1)
		{
			period_match = true;
			g_period_match = true;
		}
		else if (mode == 2)
		{
			qualifier_match = true;
		}
		if (!zjh.conf["calc_score"].empty())
			g_room_card_score = (zjh.conf["calc_score"].asInt() == 1);

	}
	//try {}
	catch (...) 
	{}
}

void Game::AcceptCB(struct ev_loop *loop, struct ev_io *w, int revents)
{
	if (EV_ERROR & revents) {
		log.error("got invalid event\n");
		return;
	}
	struct sockaddr_in client_addr;
	socklen_t client_len = sizeof(client_addr);
	int fd = accept(w->fd, (struct sockaddr *) &client_addr, &client_len);
	if (fd < 0) {
		log.error("%s:%d:%s accept error[%s]\n",__FILE__,__LINE__, __FUNCTION__,strerror(errno));
		return;
	}
	Game *game = (Game*) (w->data);
	if( NULL == game->link_data_client ){
		close(fd);
		log.error("%s:%d:%s accept client error for the link of datasvr disconnect.\n", __FILE__, __LINE__, __FUNCTION__);
		return;
	}
	fcntl(fd, F_SETFL, fcntl(fd, F_GETFL, 0) | O_NONBLOCK);
	Client *client = NULL;
	std::string cli_ip(inet_ntoa( client_addr.sin_addr ));		
	if (game->m_MatchClient.find(cli_ip) != game->m_MatchClient.end())
	{
		client = new (std::nothrow) Client(fd, false, E_PB_JSON);
	}
	else
	{
		client = new (std::nothrow) Client(fd, false, E_PB_PROTO);
	}
	log.info("%s:%d:%s fd:%d  client.type:%d ip:%s port:%d\n", __FILE__,__LINE__,__FUNCTION__, fd, client->clientType, inet_ntoa(client_addr.sin_addr), htons(client_addr.sin_port));
		
	if (client){
		client->string_ip = cli_ip;
		game->fd_client_map[fd] = client;
	}
	else{
		close(fd);
	}
}

int Game::ConnectDatasvr()
{
	log.info("%s:%d:%s datasvr management ip[%s] port[%d].\n",__FILE__,__LINE__,__FUNCTION__,
				zjh.conf["data-svr"]["host"].asString().c_str(),
				zjh.conf["data-svr"]["port"].asInt() );
	struct sockaddr_in addr;
	int fd = socket(PF_INET, SOCK_STREAM, 0);
	if (fd < 0)	{
		log.error("File[%s] Line[%d]: socket failed: %s\n", __FILE__, __LINE__, strerror(errno));
		return -1;
	}
	addr.sin_family = AF_INET;
	addr.sin_port = htons(zjh.conf["data-svr"]["port"].asInt());
	addr.sin_addr.s_addr = inet_addr(zjh.conf["data-svr"]["host"].asString().c_str());
	if (addr.sin_addr.s_addr == INADDR_NONE){
		log.error("game::connect_datasvr Incorrect ip address!");
		close(fd);
		fd = -1;
		exit(1);
	}
	if( connect(fd, (struct sockaddr*)&addr, sizeof(addr)) < 0){
		close(fd);
		log.error("connect data svr error: %s(errno: %d)\n", strerror(errno), errno);
		fd = -1;
		exit(1);
	}
	fcntl(fd, F_SETFL, fcntl(fd, F_GETFL, 0) | O_NONBLOCK);
	link_data_client = new(std::nothrow) Client( fd, false, E_PB_JSON);
	if( NULL != link_data_client ){
		log.info("%s:%d:%s connect data svr fd[%d].\n", __FILE__, __LINE__, __FUNCTION__, fd);
		fd_client_map[link_data_client->fd] = link_data_client;
		//link_data_client->mGameId = 101;
	}
	else{
		close(fd);
		log.error("new data svr client error.\n");
		exit(1);
	}
	RegisterSvr();
	return 0;
}

void Game::StartReconnect()
{
	ev_timer_again( zjh.loop, &reconnect_datasvr_timer );
}

void Game::ReconnectDataSvrCB( struct ev_loop *loop, struct ev_timer *w, int revents )
{
	Game *p_game = (Game*)w->data;
	p_game->ReconnectDataSvr();
}

int Game::ReconnectDataSvr()
{
	struct sockaddr_in addr;
	int fd = socket(PF_INET, SOCK_STREAM, 0);
	if (fd < 0){
		log.error("File[%s] Line[%d]:%s socket failed: %s\n", __FILE__, __LINE__,__FUNCTION__, strerror(errno));
		return -1;
	}
	addr.sin_family = AF_INET;
	addr.sin_port = htons(zjh.conf["data-svr"]["port"].asInt());
	addr.sin_addr.s_addr = inet_addr(zjh.conf["data-svr"]["host"].asString().c_str());
	if (addr.sin_addr.s_addr == INADDR_NONE){
		log.error("game::reconnect_datasvr Incorrect ip address!");
		close(fd);
		fd = -1;
		return -1;
	}
	if( connect(fd, (struct sockaddr*)&addr, sizeof(addr)) < 0){
		close(fd);
		log.error("reconnect datasvr error: %s(errno: %d)\n", strerror(errno), errno);
		return -1;
	}
	fcntl(fd, F_SETFL, fcntl(fd, F_GETFL, 0) | O_NONBLOCK);
	link_data_client = new (std::nothrow) Client( fd, false, E_PB_JSON);
	if( link_data_client ){
		log.info("%s:%d:%s reconnect datasvr link fd[%d].\n", __FILE__, __LINE__, __FUNCTION__, fd );
		//link_data_client->mGameId = 101;
		fd_client_map[link_data_client->fd] = link_data_client;
		ev_timer_stop(zjh.loop, &reconnect_datasvr_timer );
		RegisterSvr();
	}
	return 0;
}
void Game::SendToDataSvr( const std::string &res )
{
	if( NULL != link_data_client ){
		link_data_client->send_as_json( res );
	}
	else {
		log.error("link data svr is null\n");
	}
}

void Game::DelClient(Client *client, bool del_oldclient)
{
	//顶号问题
	if ( link_data_client && client->fd != link_data_client->fd && (del_oldclient /*|| client->cmd_type == 1*/)){
		log.info("%s:%d:%s delete old client, fd:%d \n", __FILE__, __LINE__, __FUNCTION__, client->fd);
		fd_client_map.erase(client->fd);
		delete client;
		client = NULL;
		return;
	}
	if ( fd_client_map.find(client->fd) == fd_client_map.end() ){
		log.error("%s:%d:%s del client free client err[miss] fd[%d].\n", __FILE__, __LINE__, __FUNCTION__, client->fd );
		if (uid_client_map.find(client->uid) != uid_client_map.end()) {
			if (uid_client_map[client->uid]->fd == client->fd)
			{
				uid_client_map.erase(client->fd);
				log.error("%s:%d:%s free client err[miss] fd[%d].\n", __FILE__, __LINE__,__FUNCTION__, client->fd);
			}
		}
		return;
	}
	fd_client_map.erase(client->fd);
	if( uid_client_map.find( client->uid ) != uid_client_map.end() ){
		uid_client_map.erase( client->uid );
	}
	if (client->uid == -1)
	{
		for (std::map<int, Client*>::iterator it = uid_client_map.begin(); it != uid_client_map.end(); ++it)
		{
			if (it->second == client)
			{
				uid_client_map.erase(it);
				break;
			}
		}
	}
	int i = client->fd;
	int flag = 0;
	if( link_data_client ){
		flag = ( i == link_data_client->fd) ? 1:0;
	}
	if (client->player) {
		client->player->client = NULL;
		client->player = NULL;
	}
	log.info("%s:%d:%s del client fd:%d\n", __FILE__,__LINE__,__FUNCTION__, client->fd);
	delete client;
	if( flag == 1){
		link_data_client = NULL;
		DelAllClient();
		StartReconnect();
	}
	client = NULL;
}

void Game::DelAllClient()
{
	std::map<int, Client*> temp_mFdClientMap = fd_client_map;
	log.info("%s:%d:%s for disconnect to datasvr, delete all client fd_client_len[%d] temp_mFdClientMap[%d]  .\n",__FILE__,__LINE__,__FUNCTION__, fd_client_map.size(), temp_mFdClientMap.size() );
	for(std::map<int, Client*>::iterator iter = temp_mFdClientMap.begin(); iter != temp_mFdClientMap.end(); iter++ ){
		Client::pre_destroy( iter->second );
	}
}

int Game::DispatchJson(Client *client)
{	
	int cmd = client->packet.safe_check();
	if (cmd < 0){
		log.error("the cmd format is error.\n");
		return -1;
	}
	Json::Value &val = client->packet.tojson();
	//log.info("%s client fd:%d cmd:%d\n", __FUNCTION__, client->fd, cmd);
	int matchid = 0;
	try
	{
		matchid = val["mid"].asInt();
		if (all_match_map.find(matchid) != all_match_map.end())
			last_match = all_match_map[matchid];
	}
	//try {}
	catch (...){
		log.error("%s catch error\n", __FUNCTION__);
		return -1;
	}
	switch(cmd){
		case SERVER_MATCH_STATUS_ACK:
			MatchStatusAck(val);
			break;
		case SERVER_MATCH_START_ACK:
			MatchStart(val);
			break;
		case SERVER_MATCH_PRIZE_REQ:
			//HandlerPrize(val);
			prize_json = val;
			TIMER_AGAIN(prize_timer, prize_tstamp);
			break;
		case SERVER_MATCH_PAUSE_REQ:
			HandlerMatchPause(val);
			break;
		case SERVER_MATCH_SAVE_ACK:
		case SERVER_MATCH_RANK_INFO_ACK:
			MatchEndRes(val["mid"].asInt());
			break;
		case SERVER_REGISTER_ACK:
		case SERVER_TURN_MP_INFO_ACK:
		case SERVER_TURN_RANK_INFO_ACK:
		case SERVER_TURN_MOVE_INFO_ACK:
		case SERVER_MATCH_EXCEPTION_ACK:
		case SERVER_USER_OVER_ACK:
		case SERVER_USER_EXCEPTION_ACK:
			log.info("%s:%d:%s reciev data svr res cmd:%d\n", __FILE__, __LINE__, __FUNCTION__, cmd);
			return 0;
	}
	return 0;
}

int Game::DispatchProto(Client *client)
{	
	int cmd = client->m_Buffer->proto_head.cmdID;

	if (cmd < 0) {
		log.error("the cmd format is error.\n");
		return -1;
	}
	if (cmd == CLINET_HEART_BEAT_REQ)
	{
		proto::game::tagUserState state;
		state.set_chairid(-1);
		state.set_state(0);
		client->unicast(SERVER_HEART_BEAT_RES, state);
		client->Heart_Beat();

		Player* player = client->player;
		if (player != NULL && player->table != NULL)
		{
			player->table->Online_Beat(player);
		}
		return 0;
	}
	if (cmd == CLIENT_JOIN_MATCH_REQ) {
		JoinMatchReq(client);
		return 0;
	}
	//客户端通信
	Player *player = client->player;
	if (player == NULL) {
		log.error("%s safe check cmd:%d\n", __FUNCTION__, cmd);
		return -1;
	}
	if (client->player != NULL)
	{
		log.info("%s:%d:%s mid:%d uid:%d cmd:%d\n",__FILE__,__LINE__, __FUNCTION__, player->matchid, player->uid, cmd);
	}
	
	if (all_match_map.find(player->matchid) == all_match_map.end()){
				
		log.error("%s safe_check uid:%d is not in match:%d\n", __FUNCTION__, player->uid, player->matchid);
		return -1;
	}
	Match *match = all_match_map[player->matchid];
	last_match = match;	
	switch(cmd){
		case CLIENT_RANK_REQ:
			match->HandlerRankInfoReq(player);
			return 0;
		case CLIENT_RANKLIST_REQ:
			match->HandlerRankListReq(player);
			return 0;
		case CLIENT_RANK_REQ_2:
			match->HandlerRankListReq2(player);
			return 0;
	}
	Table *table = player->table;
	if(table == NULL){
		log.error("safe_check uid[%d] table is null\n", player->uid);
		return -1;
	}
	last_table = table;
	if (player->client == NULL)
	{
		player->client = client;
	}
	if (cmd == CLIENT_QUIT_MATCH_REQ)
	{
		QuitMatchReq2(player->client);
		return 0;
	}
	return table->HandlerUserOperate(cmd, player);	
}

//用户请求进入比赛 若未收到平台通知赛事开赛 则进入失败
int Game::JoinMatchReq(Client* client)
{
	proto::game::ReqLogin pReqLogin;	
	if (!client->parse_proto(pReqLogin))
	{	
		log.error("pReqLogin parse error\n");
		return -1;
	}
	int tmpUid = pReqLogin.uid();
	int tmpMid = pReqLogin.mid();
	//如果该赛事已经生成 表示重连
	if (all_match_map.find(tmpMid) != all_match_map.end()) {
		return JoinMatch(tmpMid, tmpUid, client);
	}
	else if(uid_client_map.find(tmpUid) != uid_client_map.end()){
		if( uid_client_map[tmpUid]->fd != client->fd ){
			DelClient(uid_client_map[tmpUid]);
		}
	}
	uid_client_map[tmpUid] = client;
	client->uid = tmpUid;
	if (period_match)
	{
		Jpacket packet;
		packet.val["cmd"] = SERVER_JOIN_MATCH_REQ;
		packet.val["mid"] = tmpMid;
		packet.val["uid"] = tmpUid;
		packet.val["datetime"] = (int)time(NULL);		
		packet.end();
		//赛事不存在 向服务器请求赛事信息
		SendToDataSvr(packet.tostring());
	}
	else
	{
		Jpacket packet;
		packet.val["cmd"] = SERVER_MATCH_STATUS_REQ;
		packet.val["mid"] = tmpMid;
		packet.val["uid"] = tmpUid;
		packet.val["datetime"] = (int)time(NULL);
		packet.end();
		SendToDataSvr(packet.tostring());
	}
	return 0;
}

int Game::JoinMatch(int matchid, int uid, Client *client)
{
	if (del_match_map.find(matchid) != del_match_map.end())
	{
		log.error("%s:%d:%s find deleted match:%d\n", __FILE__, __LINE__, __FUNCTION__,  matchid);
		return -1;
	}
	if(all_match_map.find(matchid) == all_match_map.end()){
		log.error("%s:%d:%s can't find match:%d\n", __FILE__, __LINE__, __FUNCTION__, matchid);
		return -1;
	}
	Match *match = all_match_map[matchid];
	if (period_match)
	{
		Jpacket packet;
		packet.val["cmd"] = SERVER_JOIN_MATCH_REQ;
		packet.val["mid"] = matchid;
		packet.val["uid"] = uid;
		packet.val["datetime"] = (int)time(NULL);
		packet.end();
		//赛事不存在 向服务器请求赛事信息
		SendToDataSvr(packet.tostring());
		uid_client_map[uid] = client;
		return 0;
	}
	else if (match->all_players.find(uid) != match->all_players.end()) {
		Player* player = match->all_players[uid];
		Client* oldClient = player->client;
		player->SetClient(client);
		Table* table = 	match->all_tables[player->tid];
		player->m_isRobot = false;
		table->PlayerLogin(player, true);
		table->m_trustee.get()->TriggerTimer(player->seatid);

		if (oldClient != NULL && oldClient->fd != client->fd) {
			log.info("%s:%d:%s uid:%d new fd:%d old fd:%d\n", __FILE__, __LINE__, __FUNCTION__, player->uid, player->client->fd, oldClient->fd);
			SendLogout(oldClient, client->uid, 99);
			oldClient->player = NULL;
			Client::pre_destroy(oldClient, true);
		}
		log.info("%s:%d:%s mid:%d uid:%d\n", __FILE__, __LINE__, __FUNCTION__,  matchid, uid);
		if (table->m_bGameState == GAME_FREE && table->m_CurRound >= table->match->AllRound()&& table->m_cbInitTableState==true)
		{
			if (match->m_CurTurn >= match->m_AllTurn)
				match->HandlerRankInfoReq(player);
			else
				match->HandlerTurnOverInfo(player, match->m_bInterval);
		}
		return 0;
	}	
	else
	{
		Jpacket packet;
		packet.val["cmd"] = SERVER_USER_EXCEPTION_REQ;
		packet.val["mid"] = matchid;
		packet.val["uid"] = uid;
		packet.val["datetime"] = (int)time(NULL);
		packet.end();
		SendToDataSvr(packet.tostring());
		SendLogout(client, uid, 103);
		return 0;
	}
}

int Game::MatchStatusAck(Json::Value &val)
{
	int tmpMid = val["mid"].asInt();
	int tmpUid = val["uid"].asInt();
	std::map<int, Client*>::iterator iter = uid_client_map.find(tmpUid);
	if( iter == uid_client_map.end() ){
		log.error("the player uid[%d] socket is not in map.\n", tmpUid);
		return 0;
	}
	Client *client = iter->second;
	uid_client_map.erase(tmpUid);
	int status = val["status"].asInt();
	if(status != 200){
		SendLogout(client, tmpUid, 103);
		return -1;
	}
	else{
		
		if(JoinMatch(tmpMid, tmpUid, client) == -1){
			int matchType = val["type"].asInt();
			if(matchType == 2){
				SendLogout(client, tmpUid, 101);	
				Jpacket packet_end;
				packet_end.val["cmd"] = SERVER_MATCH_EXCEPTION_REQ;
				packet_end.val["mid"] = tmpMid;
				packet_end.val["datetime"] = (int)time(NULL);
				packet_end.end();
				SendToDataSvr(packet_end.tostring());
			}
			else if(matchType == 3){
				SendLogout(client, tmpUid, 102);
			}
			else {
				SendLogout(client, tmpUid, 101);
			}
		}		
	}
	return 0;
}

int Game::MatchStart(Json::Value &val)
{
	int tmpMid = val["mid"].asInt();
	int ret = zjh.main_rc[0]->command("get minfo:%d", tmpMid);
	if(ret < 0){
		log.error("%s get minfo:%d error\n", __FUNCTION__, tmpMid);
		return -1;
	}
	if (zjh.main_rc[0]->reply->str == NULL)
	{
		log.error("%s mid:%d replay str is null\n", __FUNCTION__, tmpMid);
		return -1;
	}
	std::string strminfo = zjh.main_rc[0]->reply->str;
	Json::Reader reader;
	Json::Value mval;
	if(reader.parse(strminfo, mval) < 0){
		log.error("%s parse minfo[%d] error\n", __FUNCTION__, tmpMid);
		return -1;
	}	
	//log.debug("%s get redis minfo %s\n", __FUNCTION__, mval.toStyledString().c_str());
	zjh.main_rc[0]->command("del minfo:%d", tmpMid);
	if (all_match_map.find(tmpMid) == all_match_map.end()) {
		Match *match = NULL;
		
		if (qualifier_match && (mval["minfo"]["fightMode"].asInt() == 1))
		{
			match = new (std::nothrow) RRWW_Match();			
		}
		else if (period_match && (mval["minfo"]["fightMode"].asInt() == 0))
		{
			match = new(std::nothrow) RJMatch();
		}
		else if (mval["minfo"]["fightMode"].asInt() == 0) {
			match = new(std::nothrow) RJMatch();
		}
		else if (mval["minfo"]["fightMode"].asInt() == 1) {
			match = new(std::nothrow) RRMatch();
		}
		
		if (match == NULL) {
			log.error("new match error\n");
			return -1;
		}
		int ret = match->Init(mval);
		if (ret < 0) {
			log.error("init match error\n");
			delete match;
			match = NULL;
			return -1;
		}
		all_match_map[tmpMid] = match;
		Jpacket packet;
		packet.val["cmd"] = SERVER_MATCH_START_RES;
		packet.val["mid"] = tmpMid;
		packet.val["datatime"] = (int)time(NULL);
		packet.end();
		SendToDataSvr(packet.tostring());
	}
	else{
		log.error("%s match:%d already exsit\n", __FUNCTION__, tmpMid);
	}
	return 0;
}

int Game::HandlerPrize(Json::Value &val)
{
	int mid = val["mid"].asInt();
	if(all_match_map.find(mid) == all_match_map.end()){
		log.error("%s cannot find mid[%d]\n", __FUNCTION__, mid);
		return -1;
	}
	int isBreak = val["isBreak"].asInt();
	int ret = zjh.main_rc[0]->command("get prize_info:%d", mid);
	if(ret < 0){
		log.error("%s get prize_info[%d] error\n", __FUNCTION__, mid);
		return -1;
	}
	std::string strminfo = zjh.main_rc[0]->reply->str;
	Json::Reader reader;
	Json::Value mval;
	if(reader.parse(strminfo, mval) < 0){
		log.error("%s parse prize_info[%d] error\n", __FUNCTION__, mid);
		return -1;
	}
//	log.debug("%s get redis prize_info:%s\n", __FUNCTION__, mval.toStyledString().c_str());
	zjh.main_rc[0]->command("del prize_info:%d", mid);
	proto::game::MatchPrizeAck pMatchPrizeAck;
	pMatchPrizeAck.set_isbreak(isBreak);	
	Match *match = all_match_map[mid];
	for(size_t i = 0; i < mval.size(); ++i){
		int uid = mval[i]["userId"].asInt();
		if(match->all_players.find(uid) == match->all_players.end()){
			log.error("%s cannot find player[%d]\n", __FUNCTION__, uid);
			continue;
		}
		pMatchPrizeAck.set_content(mval[i]["content"].asString());
 		if(match->all_players[uid]->client){
			match->all_players[uid]->client->unicast(SERVER_MATCH_PRIZE_UC, pMatchPrizeAck);
 		}
	}
	return 0;
}

int Game::HandlerMatchPause(Json::Value &val)
{
	int mid = val["mid"].asInt();
	if(all_match_map.find(mid) == all_match_map.end()){
		log.error("HandlerMatchPause error not has mid[%d]\n", mid);
		return 0;
	}
	Match *match = all_match_map[mid];
	int ret = match->MatchPause(val["type"].asInt());
	int type = val["type"].asInt();
	Jpacket packet;
	packet.val["cmd"] = SERVER_MATCH_PAUSE_ACK;
	packet.val["datetime"] = (int)time(NULL);
	packet.val["type"] = type;
	packet.val["mid"] = mid;
	packet.val["status"] = ret;
	packet.end();
	packet.tostring();
	SendToDataSvr(packet.tostring());
	return 0;
}

void Game::RegisterSvr()
{
	Jpacket packet;
	packet.val["cmd"] = SERVER_REGISTER_REQ;
	packet.val["gameId"] = server_vid;
	packet.val["datetime"] = (int)time(NULL);
	packet.end();
	SendToDataSvr(packet.tostring());
}

// void Game::RegisterSvrRes(Json::Value &val)
// {
// 	if (val["gameId"].asInt() != server_vid || val["status"].asInt() != 200) {
// 		log.error("register gamesvr error[%d] code[%s]\n", val["status"].asInt(), val["statusMsg"].asString().c_str());
// 	}
// }

void Game::SendLogout(Client *client, int uid, int code)
{
	//广播退出房间协议,防止客户端断线后又发登录请求，一直死循环
	proto::game::AckLogoutBc stLogout;
	stLogout.set_uid(uid);
	stLogout.set_seatid(-1);
	stLogout.set_type(code);
	client->unicast(SERVER_LOGOUT_SUCC_BC, stLogout);

	log.info("%s:%d:%s uid:%d code:%d\n", __FILE__, __LINE__, __FUNCTION__, uid, code);
}

int Game::MatchEndRes(int mid)
{
	if (all_match_map.find(mid) == all_match_map.end()) {
		log.error("%s match:%d end not exsit\n", __FUNCTION__, mid);
		return -1;
	}
	del_match_map[mid] = all_match_map[mid];
	//ev_timer_start(zjh.loop, &del_match_timer);
	log.info("%s:%d:%s mid:%d\n", __FILE__, __LINE__, __FUNCTION__, mid);
	return 1;
}

void Game::DelMatchCB(struct ev_loop *loop, struct ev_timer *w, int revents)
{
	Game *game = (Game*)w->data;
	//ev_timer_stop(zjh.loop, &game->del_match_timer);
	for (std::map<int, Match*>::iterator it = game->del_match_map.begin(); it != game->del_match_map.end();) {
		Match *match = it->second;
		int mid = match->matchid;
		if (g_period_match)
		{
			if ((match->all_tables.size() > match->m_TurnOverTables.size()) && match->m_State != MATCH_OVER)
			{
				++it;
				continue;
			}
		}
		if (!match->m_can_delete)
		{
			match->m_can_delete = true;
			++it;
			continue;
		}		
		for(std::map<int, Player*>::iterator it_t = match->all_players.begin(); it_t != match->all_players.end(); ++it_t){
			Player *player = it_t->second;
			log.info("%s match:%d player:%d\n", __FUNCTION__, mid, player->uid);						
			game->DelPlayer2(player, 106);			
		}		
		for (std::map<int, Table*>::iterator it_t = match->all_tables.begin(); it_t != match->all_tables.end(); ++it_t) {
			Table *table = it_t->second;
			log.info("%s match:%d table:%d\n", __FUNCTION__, mid, table->tid);
			delete table;
		}
		for (std::set<Player*>::iterator it_t =  match->all_robots.begin(); it_t != match->all_robots.end(); ++it_t)
		{
			delete *it_t;
		}
		game->all_match_map.erase(it->first);
		delete match;		

		game->del_match_map.erase(it++);
		log.info("%s del match:%d succ\n", __FUNCTION__, mid);
	}
	//game->del_match_map.clear();
	log.info("%s:%d:%s timerout\n", __FILE__, __LINE__, __FUNCTION__);
}

int Game::InitAccept(const char* ip, int port, int fd, ev_io& acpt)
{
	log.info("Listening on %s:%d\n", ip, port);

	struct sockaddr_in addr;
	fd = socket(PF_INET, SOCK_STREAM, 0);
	if (fd < 0) {
		log.error("File[%s] Line[%d]: socket failed: %s\n", __FILE__, __LINE__, strerror(errno));
	}
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	addr.sin_addr.s_addr = inet_addr(ip);
	if (addr.sin_addr.s_addr == INADDR_NONE) {
		log.error("game::init_accept Incorrect ip address!");
		close(fd);
		fd = -1;
		exit(1);
	}
	int on = 1;
	if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) < 0) {
		log.error("File[%s] Line[%d]: setsockopt failed: %s\n", __FILE__, __LINE__, strerror(errno));
		close(fd);
		exit(1);
		return -1;
	}
	if (bind(fd, (struct sockaddr *) &addr, sizeof(addr)) < 0) {
		log.error("File[%s] Line[%d]: bind failed: %s\n", __FILE__, __LINE__, strerror(errno));
		close(fd);
		exit(1);
		return -1;
	}
	fcntl(fd, F_SETFL, fcntl(fd, F_GETFL, 0) | O_NONBLOCK);
	listen(fd, 10000);
	acpt.data = this;
	ev_io_init(&acpt, Game::AcceptCB, fd, EV_READ);
	ev_io_start(zjh.loop, &acpt);
	
	log.info("%s:%d:%s listen ok\n", __FILE__, __LINE__, __FUNCTION__);
	return 0;
}
void Game::PrizeCB(struct ev_loop *loop, struct ev_timer *w, int revents)
{
	Game *game = (Game*)w->data;
	if (game != NULL)
	{
		ev_timer_stop(zjh.loop, &game->prize_timer);
		game->HandlerPrize(game->prize_json);
		game->prize_json.clear();
	}
}