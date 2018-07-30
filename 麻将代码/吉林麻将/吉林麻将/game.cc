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
#include <list>
#include <set>

#include "zjh.h"
#include "game.h"

#include "table.h"
#include "proto/proto.h"
#include "proto/gameproto.pb.h"
#include "common/log.h"
#include "common/client.h"
#include "common/player.h"
#include "robot_manager.h"
#include "libnormalmahjong/common.h"
#include "common/client_buffer.h"

extern ZJH zjh;


static void game_del_client(Client *client, bool del_oldclient)
{
	zjh.game->del_client(client, del_oldclient);
}
static int game_del_player(Player *player)
{
	return zjh.game->del_player(player);
}
int game_dispatch_client(Client *client)
{
	return zjh.game->dispatch_client(client);
}
int game_dispatch_server(Client *client)
{
	return zjh.game->dispatch_server(client);
}
static int GameEndScoreFilter_0(Buffer* buffer)
{
	try
	{
		Client_Buffer* h = (Client_Buffer*)buffer->dpos();

		char tmp[0x10000] = "";
		strncpy(tmp, h->json_data, h->json_head.length);
		tmp[sizeof(tmp) - 1] = '\0';

		Jpacket jp;
		int iRet = jp.parse(tmp, h->json_head.length);
		if (iRet != 0)
		{
			log_error("%s parse error! ", __FUNCTION__);
		}
		string r = jp.val["roomid"].asString();
		if (!r.empty())
		{
			int roomid = atoi(r.c_str());
			return roomid;
		}
	}	
	catch (...)
	{
	}
	return 0;	
}
static void GameEndScoreFilter(int iScore[GAME_PLAYER], Buffer* buffer)
{
	Client_Buffer* h = (Client_Buffer*)buffer->dpos();

	char tmp[0x10000] = "";
	strncpy(tmp, h->json_data, h->json_head.length);
	tmp[sizeof(tmp) - 1] = '\0';

	Jpacket jp;
	int iRet = jp.parse(tmp, h->json_head.length);
	if (iRet != 0)
	{
		log_error("%s parse error! ", __FUNCTION__);
	}
	for (unsigned int i = 0; i < jp.val["players"].size(); i++)
	{
		iScore[i] += jp.val["players"][i]["alter_money"].asInt();
	}
}
static void GameEndScoreRepackage(int iScore[GAME_PLAYER], Buffer* buffer, Jpacket& out)
{
	Client_Buffer* h = (Client_Buffer*)buffer->dpos();

	char tmp[0x10000] = "";
	strncpy(tmp, h->json_data, h->json_head.length);
	tmp[sizeof(tmp) - 1] = '\0';

	int iRet = out.parse(tmp, h->json_head.length);
	if (iRet != 0)
	{
		log_error("%s parse error! ", __FUNCTION__);
	}
	for (unsigned int i = 0; i < out.val["players"].size(); i++)
	{
		out.val["players"][i]["alter_money"] = iScore[i];
	}
	out.end();
}

int game_send_filter(Client* client, std::list<Buffer*>& _write_q)
{	
	if (client == zjh.game->link_data_client)
	{
		MJ_BUFFER < Buffer*, 0x500> target;
		for (std::list<Buffer*>::iterator it = _write_q.begin(); it != _write_q.end(); it++)
		{
			JsonHeaderEx* header = (JsonHeaderEx*)(*it)->pdata();
			if (header->cmd == SERVER_DZ_UPDATE_USER_INFO || header->cmd == SERVER_DZ_UPDATE_USER_INFO_LITTLE)
			{
				target.Add(*it);
			}
		}
		if (target.Size() > 1)
		{
			std::set<int> roomid_set;
			for (int i = 0; i < target.Size(); i++)
			{
				int roomid = GameEndScoreFilter_0(target[i]);
				roomid_set.insert(roomid);
			}
			if (roomid_set.size() > 1)
				return 0;

			int iScore[GAME_PLAYER] = { 0 };
			for (int i = 0; i < target.Size(); i++)
			{
				GameEndScoreFilter(iScore, target[i]);
			}
			Jpacket out;
			GameEndScoreRepackage(iScore, target.Last(), out);
			for (int i = 0; i < target.Size(); i++)
			{
				_write_q.remove(target[i]);
				delete target[i];
			}
			zjh.game->send_to_datasvr(out.tostring());
			log_info(" size %d, final score[%d,%d,%d,%d]! ", 
				 target.Size(), iScore[0], iScore[1], iScore[2], iScore[3]);
			return target.Size();
		}
	}
	return 0;
}

Game::Game()
{
	_fd = 0;
	link_data_client = NULL;
	link_video_client = NULL;
	ev_reconnect_datasvr_tstamp = 1;
	ev_reconnect_datasvr_timer.data = this;
	ev_timer_init( &ev_reconnect_datasvr_timer, Game::reconnect_datasvr_cb, ev_reconnect_datasvr_tstamp, ev_reconnect_datasvr_tstamp );
	
	server_zid = zjh.conf["tables"]["zid"].asInt();
	server_vid = zjh.conf["tables"]["vid"].asInt();
	log_info("game svr zid[%d] vid[%d] ", server_zid, server_vid );
	game_name = zjh.conf["game_name"].asString();
	game_number = zjh.conf["game_number"].asInt();
	
// 	ev_del_table_tstamp = 5 * 60;
// 	ev_del_table_timer.data = this;
// 	ev_timer_init( &ev_del_table_timer, Game::del_table_cb, ev_del_table_tstamp, 0 );
   
	link_video_client = NULL;
	ev_reconnect_videosvr_timer.data = this;
	ev_reconnect_videosvr_tstamp = 1;
	ev_timer_init( &ev_reconnect_videosvr_timer, Game::reconnect_videosvr_cb, ev_reconnect_videosvr_tstamp, ev_reconnect_videosvr_tstamp );
}

Game::~Game()
{
	if( NULL != link_data_client )
	{
		delete link_data_client;
		link_data_client = NULL;
	}
	if (NULL != link_video_client)
	{
		delete link_video_client;
		link_video_client = NULL;
	}

	ev_timer_stop( zjh.loop, &ev_reconnect_datasvr_timer );
	ev_timer_stop( zjh.loop, &ev_reconnect_videosvr_timer);
	
}

int Game::start()
{
	static InitPlayer init_player(zjh.loop, zjh.conf, game_del_player);
	static InitClient init_client(zjh.loop, game_del_client, game_dispatch_client, game_dispatch_server, game_send_filter);

	connect_datasvr();

	connect_videosvr();

	//init_table();

	init_accept();

	return 0;
}


void Game::table_config()
{
	
}

int Game::tabel_setup(Client *client)
{
	
	return 0;
}



int Game::init_accept()
{
	log_info("Listening on %s:%d ",
			zjh.conf["game"]["host"].asString().c_str(),
			zjh.conf["game"]["port"].asInt());

	struct sockaddr_in addr;

	_fd = socket(PF_INET, SOCK_STREAM, 0);
	if (_fd < 0) {
		log_error("File[%s] Line[%d]: socket failed: %s ", __FILE__, __LINE__, strerror(errno));
	}

	addr.sin_family = AF_INET;
	addr.sin_port = htons(zjh.conf["game"]["port"].asInt());
	addr.sin_addr.s_addr = inet_addr(zjh.conf["game"]["host"].asString().c_str());
	if (addr.sin_addr.s_addr == INADDR_NONE) {
		log_error("game::init_accept Incorrect ip address!");
		close(_fd);
		_fd = -1;
		abort();
		//exit(1);
	}

	int on = 1;
	if (setsockopt(_fd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) < 0) {
		log_error("File[%s] Line[%d]: setsockopt failed: %s ", __FILE__, __LINE__, strerror(errno));
		close(_fd);
		abort();
		//exit(1);
		return -1;
	}

	if (bind(_fd, (struct sockaddr *) &addr, sizeof(addr)) < 0) {
		log_error("File[%s] Line[%d]: bind failed: %s ", __FILE__, __LINE__, strerror(errno));
		close(_fd);
		abort();
		//exit(1);
		return -1;
	}

	fcntl(_fd, F_SETFL, fcntl(_fd, F_GETFL, 0) | O_NONBLOCK);
	listen(_fd, 10000);

	_ev_accept.data = this;
	ev_io_init(&_ev_accept, Game::accept_cb, _fd, EV_READ);
	
	ev_io_start(zjh.loop, &_ev_accept);

	log_info("listen ok ");
	return 0;
}

int Game::connect_datasvr()
{
	log_info("datasvr management ip[%s] port[%d] ",
				zjh.conf["data-svr"]["host"].asString().c_str(),
				zjh.conf["data-svr"]["port"].asInt() );

	struct sockaddr_in addr;
	int fd = socket(PF_INET, SOCK_STREAM, 0);	
	if (fd < 0) 
	{
		log_error("File[%s] Line[%d]: socket failed: %s ", __FILE__, __LINE__, strerror(errno));
		return -1;
	}
	addr.sin_family = AF_INET;
	addr.sin_port = htons(zjh.conf["data-svr"]["port"].asInt());
	addr.sin_addr.s_addr = inet_addr(zjh.conf["data-svr"]["host"].asString().c_str());
	if (addr.sin_addr.s_addr == INADDR_NONE) 
	{
		log_error("game::connect_datasvr Incorrect ip address!");
		close(fd);
		fd = -1;
		exit(1);
	}
	if( connect(fd, (struct sockaddr*)&addr, sizeof(addr)) < 0)
	{
		close(fd);
		log_error("connect data svr error: %s(errno: %d)\n", strerror(errno), errno);
		fd = -1;
		abort(); 
		exit(1);
	}
	fcntl(fd, F_SETFL, fcntl(fd, F_GETFL, 0) | O_NONBLOCK);
	link_data_client = new(std::nothrow) Client( fd, false, 1);
	if( NULL != link_data_client )
	{
		log_info("connect data svr fd[%d].\n", fd );
//		fd_client_map[link_data_client->fd] = link_data_client;
		register_server();
	}
	else
	{
		close(fd);
		log_error("new data svr client error.\n");
		exit(1);
	}
	return 0;

}
void Game::register_server()
{
	Jpacket packet;
	packet.val["cmd"] = SERVER_REGISTER_REQ;
	packet.val["vid"] = server_zid;
	packet.end();
	send_to_datasvr( packet.tostring() );
}

void Game::start_reconnect()
{
	ev_timer_again( zjh.loop, &ev_reconnect_datasvr_timer );
	log_info("%s to datasvr! ", __FUNCTION__);
}

void Game::reconnect_datasvr_cb( struct ev_loop *loop, struct ev_timer *w, int revents )
{
 	Game *p_game = (Game*)w->data;
 	p_game->reconnect_datasvr();
}

int Game::reconnect_datasvr()
{
	struct sockaddr_in addr;
	int fd = socket(PF_INET, SOCK_STREAM, 0);
	if (fd < 0) 
	{
		log_error("File[%s] Line[%d]: socket failed: %s\n", __FILE__, __LINE__, strerror(errno));
		return -1;
	}
	addr.sin_family = AF_INET;
	addr.sin_port = htons(zjh.conf["data-svr"]["port"].asInt());
	addr.sin_addr.s_addr = inet_addr(zjh.conf["data-svr"]["host"].asString().c_str());
	if (addr.sin_addr.s_addr == INADDR_NONE) 
	{
		log_error("game::reconnect_datasvr Incorrect ip address!");
		close(fd);
		fd = -1;
		return -1;
	}
	if( connect(fd, (struct sockaddr*)&addr, sizeof(addr)) < 0)
	{
		close(fd);
		log_error("reconnect datasvr error: %s(errno: %d)\n", strerror(errno), errno);
		return -1;
	}
	fcntl(fd, F_SETFL, fcntl(fd, F_GETFL, 0) | O_NONBLOCK);
	link_data_client = new (std::nothrow) Client( fd, false, 1);
	if( link_data_client )
	{
		log_info("reconnect datasvr link fd[%d].\n", fd );
		//fd_client_map[link_data_client->fd] = link_data_client;
		ev_timer_stop(zjh.loop, &ev_reconnect_datasvr_timer );
		register_server();
	}
	return 0;
}

void Game::send_to_datasvr(const std::string &res )
{
	if( NULL != link_data_client )
	{
		link_data_client->send_as_json( res);		
	}
	else
	{
		log_error("link_data client is NULL, send to datasvr failed \n");
	}
}
void Game::del_all_client()
{		
	std::map<int, Client*> temp_fd_client_map = fd_client_map;
	log_info("%s delete all client fd_client_len:%d\n", __FUNCTION__, fd_client_map.size());
	for (std::map<int, Client*>::iterator iter = temp_fd_client_map.begin(); iter != temp_fd_client_map.end(); iter++)
	{
		Client::pre_destroy(iter->second);
	}
}

void Game::accept_cb(struct ev_loop *loop, struct ev_io *w, int revents)
{
	//log_error("Enter accept_cb \n");
	if (EV_ERROR & revents) {
		log_error("got invalid event\n");
		return;
	}
	
	struct sockaddr_in client_addr;
	socklen_t client_len = sizeof(client_addr);

	int fd = accept(w->fd, (struct sockaddr *) &client_addr, &client_len);
	if (fd < 0) {
		log_error("accept error[%s]\n", strerror(errno));
		return;
	}

	Game *game = (Game*) (w->data);

	fcntl(fd, F_SETFL, fcntl(fd, F_GETFL, 0) | O_NONBLOCK);

	Client *client = new (std::nothrow) Client(fd, true, 0);
	if (client) 
	{		
		client->string_ip = inet_ntoa( client_addr.sin_addr );
		game->fd_client_map[fd] = client;
		
		log_info("%s Client connect ip:%s port:%d ", __FUNCTION__, client->string_ip.c_str(), client_addr.sin_port);
	} 
	else
	{
		close(fd);
	}
}

//机器人请求登录
int Game::request_login( Client *client )
{
	//assert(client->m_Buffer != NULL);
	if (client == NULL || client->m_Buffer == NULL)
	{
		log_error("client null or buffer null !");
		return -1;
	}
		
	proto::login::ReqLogin stLogin;
	stLogin.ParseFromArray(client->m_Buffer->proto_data, client->m_Buffer->proto_head.length);
	int uid = stLogin.uid();	
	std::ostringstream oss;
	oss << uid << "_7xlpdndm219dhy8g1xld3ald16ezpw1y";
	std::string   skey;
	skey.clear();
	skey = oss.str();

	std::ostringstream os1;
	os1 << stLogin.roomid();
	std::string   strroomid;
	strroomid.clear();
	strroomid = os1.str();

	client->uid = uid;
	client->skey = skey;
	client->tid_str = strroomid;
	log_info("%s tid:%s uid:%d zid:%d skey:%s ",__FUNCTION__, client->tid_str.c_str(), uid, server_zid, client->skey.c_str());
	if (uid == 0)
	{
		return -1;
	}
	
	
	if(  uid_client_map.find( uid ) != uid_client_map.end() )
	{
		if( uid_client_map[uid]->fd != client->fd )
		{
			log_info("%s mapSession tid:%s uid:%d", __FUNCTION__, strroomid.c_str(), uid);
			del_client(uid_client_map[uid]);
		}
	}
	uid_client_map[uid] = client;

	//请求玩家信息
	log_info("begin req player info uid=%d ", uid);
    
	Jpacket packet;
	packet.val["cmd"] = CLIENT_DZ_LOGIN_REQ;
	packet.val["uid"] = uid;
	packet.val["skey"] = skey;
	packet.val["deviceId"] = client->deviceid;
	packet.val["vid"] = server_vid;
	packet.val["roomId"] = client->tid_str;
	packet.end();
	send_to_datasvr( packet.tostring() );
	return 0;
}

void Game::inform_datasvr_user_vid( int uid, int inorout )
{
	Jpacket packet_vid;
	packet_vid.val["cmd"] = SERVER_INFORM_USER_VID_TO_DATASV;
	packet_vid.val["uid"] = uid;
	packet_vid.val["inorout"] = inorout;
	packet_vid.end();
	send_to_datasvr( packet_vid.tostring() );
}

int Game::login_succ_back(Json::Value &val )
{
	int uid = val["uid"].asInt();
	int usertype = val["usertype"].asInt();
	log_debug("%s success, uid:%d usertype:%d  ", __FUNCTION__, uid, usertype);

	std::map<int,Client*>::iterator iter = uid_client_map.find(uid);
	if( iter == uid_client_map.end() )
	{
		log_error("the player uid[%d] socket is not in map. ", uid);
		return 0;
	}
	Client *client = iter->second;	
	if ( all_tables.find(client->tid_str) != all_tables.end())
	{
		Table* table = all_tables[client->tid_str];
		if (table->m_mapTable_Player.find(client->uid) != table->m_mapTable_Player.end())
		{
			if (table->CheckPlayerSkey(val, client))
			{
				Player* player = table->m_mapTable_Player[client->uid];
				if (player != NULL)
				{
					dz_login_reconnect(client, player);
					return 0;
				}
			}
			log_error("%s rebind tid:%s uid:%d get info ok ", __FUNCTION__, client->tid_str.c_str(), uid);
			del_client(client);
			return 0;
		}
	}
	if (client->player != NULL)
	{
		log_error("%s player is ready new uid:%d", __FUNCTION__, client->uid);
		return 0;
	}


	Player *player = new (std::nothrow) Player();
	if (player == NULL) 
	{
		log_error("new player err");
		return 0;
	}
	int ret = player->init( val );
	if( 0 != ret )
	{
		Jpacket packet;
		packet.val["cmd"] = SERVER_LOGIN_ERR_UC;
		packet.val["code"] = 508;
		packet.end();
		client->send_as_json( packet.tostring() );
		Client::pre_destroy( client );
	//	del_player( player );
		return 0;
	}
	player->tid_str = client->tid_str;
	player->set_client(client);
	log_info(" new player uid:%d success. save point client: %x ",  uid, client);
	//如果是机器人，加到机器人队列中
// 	if (usertype == 4 )
// 	{
// 		player->usertype = usertype;
// 		RobotManager::GetInstance()->AddRobotToList(player);
// 	}
// 	else
	{
		handler_login_table(uid, client, val);
		robot_success(player);
	}

	inform_datasvr_user_vid( uid, 1 );	
	return 0;
}

int Game::login_error_back( int uid, Json::Value &val ,int code)
{	
	log_debug("login_error_back, uid:%d \n", uid);
	std::map<int,Client*>::iterator iter = uid_client_map.find( uid );
	if( iter == uid_client_map.end() )
	{
		log_error("the player uid[%d] socket is not in map.\n", uid );
		return 0;
	}
	Client *user_client = iter->second;	
	proto::login::AckLoginError stLoginEr;
	stLoginEr.set_uid(uid);
	stLoginEr.set_code(code);
	user_client->unicast(SERVER_LOGIN_ERR_UC, stLoginEr);
	Client::pre_destroy( user_client );
	return 0;
}

// void Game::init_buff_head(char* buff, int CmdID, int BodySize) 
// {
// 	ProtoHeader stHead = {0};
// 	stHead.CmdID = CmdID;
// 	stHead.Len = BodySize;
// 	PackHeader(&stHead, buff);
// 
// }

/*
int Game::check_skey(Client *client)
{
	if (client->uid < 10000) {
		return 0;
	}
#if 1
	int i = client->uid % zjh.main_size;
	int ret = zjh.main_rc[i]->command("hget hu:%d skey", client->uid);
	if (ret < 0) {
		log_error("check skey error, because get player infomation error.\n");
		return -1;
	}

	log_debug("skey [%s] [%s]\n", client->skey.c_str(), zjh.main_rc[i]->reply->str);
	if (zjh.main_rc[i]->reply->str && client->skey.compare(zjh.main_rc[i]->reply->str) != 0) {
		log_error("check skey error, client[%s] server[%s].\n", client->skey.c_str(), zjh.main_rc[i]->reply->str);
		return -1;
	}
#endif	
	return 0;
}


int Game::check_zid(Client *client)
{
	if (client->uid < 10000) {
		return 0;
	}
#if 1
	int my_zid = zjh.conf["tables"]["zid"].asInt();
	int i = client->uid % zjh.main_size;
	int ret = zjh.main_rc[i]->command("hget hu:%d zid", client->uid);
	if (ret < 0) {
		log_error("check zid error, uid[%d] because get player zid error from redis.\n", client->uid);
		return -1;
	}
	log_debug("zid [%d] [%s]\n", my_zid, zjh.main_rc[i]->reply->str);
	if (zjh.main_rc[i]->reply->str && my_zid != atoi(zjh.main_rc[i]->reply->str)) {
		client->zid = atoi(zjh.main_rc[i]->reply->str);
		log_error("check zid error, uid[%d] client[%d] server[%s].\n", client->uid, my_zid, zjh.main_rc[i]->reply->str);

		ret = zjh.main_rc[i]->command("hget hu:%d vid", client->uid);
		if (ret < 0) {
			log_error("get vid error, uid[%d] because get player vid error from redis.\n", client->uid);
			return -1;
		}
		if (zjh.main_rc[i]->reply->str) {
			client->vid = atoi(zjh.main_rc[i]->reply->str);
			log_error("get vid error, uid[%d] vid[%d].\n", client->uid, client->vid);
		}

		return -1;
	}

#endif

	return 0;
}
*/

void Game::del_client(Client *client, bool del_oldclient)
{
	if ( client  == NULL) 
		return  ; 
	if (client == link_data_client)
	{
		//会出现崩溃 所以注释此行
		//del_all_client();
		log_info("into  this");
		delete link_data_client;
		link_data_client = NULL;
		start_reconnect();
		return;
	}
	if (client == link_video_client)
	{
		delete link_video_client;
		link_video_client = NULL;
		start_videoreconnect();
		return; 
	}
	if (del_oldclient)
	{
		log_info("%s delete old client fd:%d uid:%d ", __FUNCTION__, client->fd, client->uid);
		fd_client_map.erase(client->fd); 		
		if (client->uid != -1 )
			uid_client_map.erase(client->uid);
		delete client;
		client = NULL;
		return;
	}
	if ( fd_client_map.find(client->fd) == fd_client_map.end() )
	{
		log_error("%s del client free fd[%d]. ", __FUNCTION__, client->fd );
		return;
	}	

	fd_client_map.erase(client->fd);
	if (client->uid != -1)
	{
		if (uid_client_map.find(client->uid) != uid_client_map.end())
		{
			log_info("  uid:%d erase uid_client_map ",  client->uid);
			uid_client_map.erase(client->uid); 
		}
	}

	if (client->player)
	{
		Player *player = client->player;		
		player->start_offline_timer();
		if (client->player->status !=  PLAYING && (player->seatid < 0 || player->seatid >= GAME_PLAYER))
		{  	
			del_player(player);
		}
		else
		{   
			player->client = NULL;	
			game_user_offline(player);
		}
	}
	else
	{
		int uid = client->uid;
		if (all_tables.find(client->tid_str) != all_tables.end())
		{
			Table* table = all_tables[client->tid_str];
			if (table != NULL && uid != -1)
			{
				if (table->m_mapTable_Player.find(uid) != table->m_mapTable_Player.end())
				{
					Player* player = table->m_mapTable_Player[uid];
					if (player != NULL)
					{
						player->start_offline_timer();
						if (player->status != PLAYING && (player->seatid < 0 || player->seatid >= GAME_PLAYER))
						{
							del_player(player);
						}
						else
						{
							player->client = NULL;
							game_user_offline(player);
							log_info("%s player is null offline tid:%s uid:%d ", __FUNCTION__, client->tid_str.c_str(), uid);
						}						
					}
				}
			}
		}
		log_info("%s player is null end tid:%s uid:%d ", __FUNCTION__, client->tid_str.c_str(), client->uid);
	}

	if (client)
	{
		log_info("%s uid:%d del client fd:%d 0x%x ", __FUNCTION__, client->uid, client->fd, client);
		delete client;
		client = NULL;	
	}
}

int Game::dispatch_client(Client *client)
{
	//用protobuf 解包
	if (client == NULL )
		return 0;
	if (client->m_Buffer == NULL)
		return 0;  

	int cmd = client->m_Buffer->proto_head.cmdID;

	if ( cmd == CLIENT_LOGIN_REQ ) 
	{
		return request_login( client );
	}
	else if (cmd == CLIENT_DZ_LOGIN_REQ)
	{		
		return dz_request_login(client);
	}
	else if (cmd == CLINET_HEART_BEAT_REQ)
	{		
		proto::login::AckHeatBeat stAck;
		stAck.set_state(0);		
		client->unicast(SERVER_HEART_BEAT_RESP, stAck);
		client->Heart_Beat();
		if (client->player != NULL)
		{
			if (all_tables.find(client->tid_str) != all_tables.end())
			{
				all_tables[client->tid_str]->Online_Beat(client->player);
			}
		}
		return 0;
	}
	else if( link_data_client && client->fd == link_data_client->fd )
	{
		log_error("%s error cmd:%d link_data_clinet is null, client->fd == link_data_client->fd\n", __FUNCTION__, cmd );
		return 0;
	}
   
	Player *player = client->player;
	if (NULL == player)
	{		
		return -1;
	}
	if (all_tables.find(player->tid_str) == all_tables.end())
	{
		log_error("%s tid:%s uid:%d is not in table ", __FUNCTION__, player->tid_str.c_str(), player->uid);
		return -1;
	}
	//log_info("%s cmd:%d player tid:%s uid:%d\n", __FUNCTION__, cmd, player->tid_str.c_str(), player->uid);
	switch (cmd)
   	{
		case CLIENT_READY_REQ:
			all_tables[player->tid_str]->handler_ready(player);
			break;
		case CLIENT_INFO_REQ:
			all_tables[player->tid_str]->handler_info(player);
			break;
		case CLIENT_LOGOUT_REQ:			
			del_player(player);
			break;
		case CLIENT_CHAT_REQ:
			all_tables[player->tid_str]->handler_chat(client);
			break;
		case CLIENT_EMOTION_REQ:
			all_tables[player->tid_str]->handler_interaction_emotion(client, player);
			break;		
		case CLIENT_TABLE_INFO_REQ:
			all_tables[player->tid_str]->handler_table_info(player);
			break;	
		case CLIENT_UPTABLE_APPLY_REQ:
			all_tables[player->tid_str]->handler_apply_uptable(player);
			break;		
		case CLIENT_DOWNTABLE_REQ:
			all_tables[player->tid_str]->handler_downtable(player);
			break;
		case CLIENT_DAEMONIZE_REQ:
			all_tables[player->tid_str]->handler_daemonize( player );
			break;
		case CLIENT_ALL_PLAYERS_REQ:
			all_tables[player->tid_str]->handler_all_players_info( player );
			break;
		//case CLIENT_MJ_BAI_PAI_SEL:
			//all_tables[player->tid_str]->recv_client_baipai_sel(client, player);   // 
			//break;
		
		case CLIENT_MJ_OUT_CARD:
			all_tables[player->tid_str]->recv_client_outcard(client, player);   // 出牌 
			break;
		//case CLIENT_MJ_NINGXIANG_FENG_DONG:
			//all_tables[player->tid_str]->recv_client_outcard(client, player);   // 出牌 
			//break;
		case CLIENT_MJ_OPERATE_CARD:
			all_tables[player->tid_str]->recv_client_opeater_req(client, player);   // 吃碰杠 
			break;
		case  CLIENT_MJ_JILIN_GAGN:
			all_tables[player->tid_str]->recv_jilin_majiang_gang_req(client, player);
			break;
		case  CLIENT_MJ_NINGXIANG_FENG_DONG:
			all_tables[player->tid_str]->recv_ningxiang_fengdong_op(client, player);
			break;
		case CLIENT_MJ_USER_ACTION:
			{
				proto::game::ReqAction stAction;
				stAction.ParseFromArray(client->m_Buffer->proto_data, client->m_Buffer->proto_head.length);
				int action = stAction.action();
				if (action == proto::game::ACTION_CHANGE_TABLE)
				{
					//change_table(player);
				}
				else
				{
					all_tables[player->tid_str]->recv_client_action(client, player);   // 准备走这里
				}	
			}
			break;		
		case CLIENT_MJ_HOME_RETURN:
			{
				log_info("Recv_Home_Return req tid:%s uid:%d\n", player->tid_str.c_str(), player->uid);
				all_tables[player->tid_str]->handler_table_info(player);
				all_tables[player->tid_str]->Send_Game_Scene(player);
			}
			break;
		case CLIENT_MJ_AUTOHU:
			{   
				all_tables[player->tid_str]->RecvClientAutoHu(client, player);
				log_info("recv auto hu tid:%s uid:%d\n", player->tid_str.c_str(), player->uid);
			}
			break;
		case CLIENT_DISBAND_ROOM:
			{
				all_tables[player->tid_str]->RecvDisbandTableApply(player);
			}
			break;
		case CLIENT_DISBAND_CHOOSE:
			{
				proto::game::ReqChooseDisband stChoose;
				stChoose.ParseFromArray(client->m_Buffer->proto_data, client->m_Buffer->proto_head.length);
				int choosestype = stChoose.choosestate();
				all_tables[player->tid_str]->RecvDisbandTableChoose(player->seatid, choosestype);
			}
			break;
		case CLIENT_HOME_OUT:
			{
				all_tables[player->tid_str]->User_Home_Out(player);
			}
			break;
// 		case CLIENT_MJ_CHOOSE_PIAO:
// 			{
// 				all_tables[player->tid_str]->RecvClientChoosePiao(client, player);
// 			}
// 			break;		
		default:
			return all_tables[player->tid_str]->dispatch_client(cmd, client, player);			
	}	
	return 0;
}

///解析从服务器 发过来的 配置信息
 
int Game::dispatch_server(Client *client)
{	
	int cmd = client->packet.sefe_check();
	if (cmd < 0) {
		log_error("the cmd format is error.\n");
		return -1;
	}

	switch (cmd)
	{	
	case SERVER_DISBAND_ROOM_RESP:
		{
			std::string roomid = client->packet.val["roomid"].asString();
			log_info("SERVER_DISBAND_ROOM_RESP tid:%s \n", roomid.c_str());
			if (all_tables.find(roomid) != all_tables.end())
			{
				Table* table = all_tables[roomid];
				log_info("SERVER_DISBAND_ROOM_RESP tid:%s, player=[%d,%d,%d,%d]\n", roomid.c_str(),
					table->GetPlayerUid(0), table->GetPlayerUid(1), table->GetPlayerUid(2), table->GetPlayerUid(3));
				all_tables[roomid]->Disband_table_resp(client->packet);
			}
		}
		break;
	case SERVER_LOGIN_ERR_UC:
		{
			int val = client->packet.val["uid"].asInt();
			int code = client->packet.val["code"].asInt();
			client->packet.end();
			login_error_back(val, client->packet.tojson(), code);
		}
		break;
	case SERVER_LOGIN_SUCC_UC:
		login_succ_back(client->packet.tojson());
		break;
	case SERVER_ROOM_CONFIG_RES:
		handler_room_conf_res(client->packet.tojson());
		break;
	case SERVER_LOGOUT_ROOM_RESP:
		logout_room_res(client->packet.tojson());
		break;
	case SYS_GET_USER_NUM:	
		sys_get_user_num(client);
		break;
	case SERVER_DZ_LOGIN_SUCC_RES:
	{
		dz_login_succ_back(client->packet.val);
	}
	break;
	case SERVER_DZ_LOGIN_ERR_RES:
	{
		int val = client->packet.val["uid"].asInt();
		int code = client->packet.val["code"].asInt();
		client->packet.end();
		dz_login_error_back(val, code);
	}
	break;
	case SERVER_DZ_GAME_START_RES:
	{
		game_start_res(client->packet);
	}
	break;
	case SERVER_DZ_UPDATE_USER_INFO:
	{
		int nstatus = 0;
		if (!client->packet.val["status"].isNull() && client->packet.val["status"].isInt())
		{
			nstatus = client->packet.val["status"].asInt();
		}
		std::string roomid;
		if (!client->packet.val["roomid"].isNull() && client->packet.val["roomid"].isString())
		{
			roomid = client->packet.val["roomid"].asString();
		}
		log_info("SERVER_DZ_UPDATE_USER_INFO_RES, STATUS:%d ROOMID:%s\n", nstatus, roomid.c_str());
	}
	break;
	case SERVER_VIDEO_DATA_ERR_RESP:
	{
		int roomID = client->packet.val["roomid"].asInt();
		int ndatatime = client->packet.val["datetime"].asInt();
		int status = client->packet.val["status"].asInt();
		int nNum = client->packet.val["num"].asInt();
		log_debug("save vedio data resp roomID:%d, num:%d, status:%d, datetime:%d ", roomID, nNum, status, ndatatime);
	}
	break;
	case SERVER_DELETE_ROOM_REQ:
	{
		std::string roomid;
		if (!client->packet.val["roomid"].isNull() && client->packet.val["roomid"].isString())
		{
			roomid = client->packet.val["roomid"].asString();
			DeleteRoomReq(roomid);
		}
	}break;
	case SERVER_ALL_PLAYER_DOWN:
	case SERVER_DELETE_ROOM_RESP:
		break;	
	case SERVER_FORCE_LOGOUT_ROOM_RESP:
	{
		log_info("force logout user");
	}
	break;
	
	default:
		log_error("invalid command[%d]\n", cmd);
		return 0;
	}

	return 0;
}





int Game::handler_login_table(int uid, Client* client, Json::Value &val)
{
	if (client == NULL)
	{
		log_error("handler_login_table client is null \n");
		return -1;
	}
	Player *player = client->player;
	if (player == NULL)
	{
		log_error("handler_login_table player is null \n");
		return -1;
	}
	if (client->position == POSITION_TABLE) {
		log_error("%s uid:%d have been in table\n", __FUNCTION__, player->uid);
		return -1;
	}

	int ret = 0;
	ret = login_table(uid, player, val);
	if (ret == 0)
		return 0;
	else if (ret == -2)
		return -2;

	log_error("handler login table no seat.");
	return -1;
}

int Game::login_table(int uid, Player *player, Json::Value &val)
{
	if (player == NULL)
	{
		log_error("%s player is null ", __FUNCTION__);
		return -1;
	}
	Client* client = player->client;
	if (client == NULL)
	{
		log_error("%s client is null ", __FUNCTION__);
		return -1;
	}	
	std::string roomid = client->tid_str;
	if( !roomid.empty() )
	{
		if( all_tables.find( roomid ) == all_tables.end() )
		{			
			Table* table = Table::CreateTable(zjh.conf["tables"], val);
			if (table == NULL)
			{
				log_error("table is null !");
				return  -1; 
			}
			table->init(zjh.conf["tables"], val, player);
			all_tables[roomid] = table;
		}
		client->set_positon( POSITION_TABLE );
		all_tables[roomid]->handler_login( player );
// 		if( del_tables_map.find( roomid ) != del_tables_map.end() )
// 		{
// 			del_tables_map.erase( roomid );
// 			if( 0 == del_tables_map.size() )
// 			{
// 				ev_timer_stop( zjh.loop, &ev_del_table_timer );
// 			}
// 		}
		log_info( " succ tid:%s uid:%d ", roomid.c_str(), player->uid);
		return 0;
	}

	log_error( " succ uid:%d roomid is NULL ",  player->uid );
	return -1;
}

int Game::handle_logout_table(int tid)
{	

	return -1;

}

int Game::del_player(Player *player)
{   
	log_info("Game::del_player ");
	if( NULL == player )
	{
		log_info("player is NULL ,return -1...\n");
		return -1;
	}
	int ret = 0;
	int uid = player->uid;
	if (all_tables.find(player->tid_str) != all_tables.end()) 
	{
		ret = all_tables[player->tid_str]->handler_logout(player);
		if (ret < 0) {
			log_error("%s table handler logout ", __FUNCTION__);
			return -1;
		}
		if( NULL == player )
		{
			log_info("the function handler_logout have delete player.\n");
			return -1;
		}
		ret = all_tables[player->tid_str]->del_player(player);
		if (ret < 0) {
			log_error("%s table del player ", __FUNCTION__);
			return -1;
		}
		if (1 == ret)
		{   
			//all_tables[player->tid]->send_talbe_no_user();
			std::map<std::string, Table*>::iterator iter2 = all_tables.find(player->tid_str);
			if( iter2 != all_tables.end() )
			{				
				Table *table = iter2->second;
				all_tables.erase( iter2 );
				if (table)
				{
					delete table;
					table = NULL;
				}				
				log_info("%s delete table! tid:%s left tables:%d ", __FUNCTION__, player->tid_str.c_str(), all_tables.size());
				Jpacket packet;
				packet.val["cmd"] = SERVER_ALL_PLAYER_DOWN;
				packet.val["vid"] = server_vid;
				packet.val["roomid"] = player->tid_str;
				packet.end();
				send_to_datasvr( packet.tostring() );

			}
		}
	}

	if (player)
	{

		if (player->client !=NULL)
		{
			std::map<std::string, std::vector<Client*> >::iterator it = tid_client_map.find(player->tid_str);
			if (it != tid_client_map.end()) {
				for (std::vector<Client*>::iterator iter = it->second.begin(); iter != it->second.end();)
				{
					if ((*iter) == player->client)
					{    
						iter = it->second.erase(iter);
						break;
					}
					else
					{
						iter++;
					}
				}
			}
		}
		log_info("%s delete player! tid:%s uid:%d player:%p ", __FUNCTION__, player->tid_str.c_str(), uid, player);
		delete player;
		player = NULL;
	}
	inform_datasvr_user_vid( uid, 0 );
	return 0;
}

int Game::change_table(Player *player)
{
	return 0;
}

void Game::game_start_res( Jpacket &packet )
{
	std::string roomid = packet.val["roomid"].asString();
	if( all_tables.find( roomid ) != all_tables.end() )
	{
		all_tables[roomid]->game_start_res( packet );
	}
}

int Game::dz_request_login( Client *client )
{
	if (client == NULL || client->m_Buffer == NULL)
		return 0;

	proto::login::ReqLogin stLogin;
	stLogin.ParseFromArray(client->m_Buffer->proto_data, client->m_Buffer->proto_head.length);

	int uid = stLogin.uid();
	client->uid = uid;
	if (stLogin.has_skey())	
		client->skey = stLogin.skey();
	
	client->tid_str = to_char(stLogin.roomid());
	if (stLogin.has_deviceid())	
		client->deviceid = stLogin.deviceid();
	
	if (uid == 0 || uid == -1)
	{
		log_error("%s tid:%s uid:%d zid:%d skey:%s ", __FUNCTION__, client->tid_str.c_str(), uid, server_zid, stLogin.skey().c_str());
		return -1;
	}
	log_info("tid:%s uid:%d zid:%d skey:%s ",  client->tid_str.c_str(), uid, server_zid, stLogin.skey().c_str());
	
	//断线重连
	if (all_tables.find(client->tid_str) != all_tables.end())
	{
		map<std::string, Table*>::iterator iter = all_tables.find(client->tid_str);
		Table *table = iter->second;
		if (table)
		{
			if (table->m_mapTable_Player.find(client->uid) != table->m_mapTable_Player.end())
			{
				Player* player = table->m_mapTable_Player[client->uid];
				if (player)
				{  
					Client *oldClient = player->client;										
					if (oldClient == client)
					{
						log_info( "%s old client %x = client %x\n", __FUNCTION__, oldClient, client);						
						client->Heart_Beat();
						return 0;
					}
					else if (oldClient && oldClient->fd != client->fd)
					{
						log_error(" tid:%s uid:%d pre_destroy old_client:%d ", client->tid_str.c_str(), client->uid, oldClient->fd);
						SendLogout(oldClient, client->uid, -1);
						oldClient->player = NULL;
						Client::pre_destroy(oldClient, true);
					}
					player->set_client(client);
					client->set_positon(POSITION_TABLE);
					uid_client_map[uid] = client;

					if (player->skey != client->skey)
					{						
						return send_request_login(client);
					}				
					return dz_login_reconnect(client, player);
				}     
			}	
		}	
	}

	if( uid_client_map.find( uid ) != uid_client_map.end() )
	{
		if( uid_client_map[uid]->fd != client->fd )
		{
			log_info("tid:%s uid:%d client player delete uid_client_map", client->tid_str.c_str(), client->uid);
			del_client( uid_client_map[ uid ] );			
		}
	}
	
	client->player = NULL;
	uid_client_map[uid] = client;

	//请求玩家信息
	//log_info("%s mapSession uid:%d\n", __FUNCTION__, uid);
	
	return send_request_login(client);
}
int Game::send_request_login(Client* client)
{
	int uid = client->uid;

	Jpacket packet;

	packet.val["cmd"] = CLIENT_DZ_LOGIN_REQ;
	packet.val["uid"] = uid;
	packet.val["skey"] = client->skey;
	packet.val["deviceId"] = client->deviceid;
	packet.val["vid"] = server_vid;
	packet.val["roomId"] = client->tid_str;
	packet.end();
	send_to_datasvr(packet.tostring());

	return 0;
}
int Game::dz_login_reconnect(Client* client, Player* player)
{
	int uid = client->uid;	
	all_tables[client->tid_str]->Reconnect(player);                   //断线重连，更新桌子保存player指针	  
	all_tables[client->tid_str]->handler_login_succ_uc(player);
	all_tables[client->tid_str]->handler_table_info(player);

	all_tables[client->tid_str]->Send_Game_Scene(player);

	if (player->seatid < 0 || player->seatid >= GAME_PLAYER)
	{
		all_tables[client->tid_str]->handler_apply_uptable(player);
	}
	player->stop_offline_timer();
	inform_datasvr_user_vid(uid, 1);
	client->Set_Beat_Time(NODATA_TIMEOUT);

	if (uid_client_map.find(uid) != uid_client_map.end())
	{
		log_error("%s uid_client_map has uid:%d already!\n", __FUNCTION__, uid);
	}
	//uid_client_map[uid] = client;
	log_info("%s tid:%s uid:%d success ", __FUNCTION__, client->tid_str.c_str(), client->uid);
	return 0;
}

int Game::dz_login_succ_back( Json::Value &val )
{
	int uid = val["uid"].asInt();
	int usertype = val["usertype"].asInt();
	log_info("%s login success, uid:%d usertype:%d \n", __FUNCTION__, uid, usertype);
	std::map<int,Client*>::iterator iter = uid_client_map.find( uid );
	if( iter == uid_client_map.end() )
	{
		log_error("%s the player uid:%d socket is not in map.\n", __FUNCTION__, uid );
		return 0;
	}
	Client *client = iter->second;
	//uid_client_map.erase( uid );
	if( all_tables.find( client->tid_str ) != all_tables.end() )
	{
		Table* table = all_tables[client->tid_str];
		if (table->m_mapTable_Player.find(client->uid) != table->m_mapTable_Player.end())
		{			
			if (table->CheckPlayerSkey(val, client))
			{
				Player* player = table->m_mapTable_Player[client->uid];
				if (player != NULL)
				{
					dz_login_reconnect(client, player);
					return 0;
				}
			}
			log_error("%s rebind tid:%s uid:%d get info ok ", __FUNCTION__, client->tid_str.c_str(), uid);
			del_client(client);
			return 0;
		}		
	}
	if (client->player != NULL)
	{
        log_error("%s player is ready new uid:%d", __FUNCTION__, client->uid);
		return 0;
	}

	
	Player *player = new (std::nothrow) Player();
	if (player == NULL) 
	{
		log_error("new player err");
		return 0;
	}
	int ret = player->init( val );
	player->tid_str = client->tid_str;
	player->set_client( client );
	//client->player = player;
	if( 0 != ret )
	{
		Jpacket packet;
		packet.val["cmd"] = SERVER_LOGIN_ERR_UC;
		packet.val["code"] = 508;
		packet.end();
		client->send_as_json(packet.tostring());
		Client::pre_destroy( client );
		//	del_player( player );
		return 0;
	}

	//log_info("%s new success tid:%s uid:%d  player:%p\n", __FUNCTION__, player->tid_str.c_str(), player->uid, player);	
    client->Set_Beat_Time(NODATA_TIMEOUT);
	//inform_datasvr_user_vid( uid, 1 );	
	if( all_tables.find( client->tid_str ) != all_tables.end() )
	{
		handler_login_table(uid, client, val);
	}
	else
	{    
		request_room_conf(client, client->tid_str);
	}
	return 0;
}

int Game::dz_login_error_back(int uid, int code)
{	
	log_debug("%s uid:%d ", __FUNCTION__, uid);
	std::map<int,Client*>::iterator iter = uid_client_map.find( uid );
	if( iter == uid_client_map.end() )
	{
		log_error("%s the player uid:%d socket is not in map.\n", __FUNCTION__, uid);
		return 0;
	}
	Client *user_client = iter->second;	
	proto::login::AckLoginError stLoginEr;
	stLoginEr.set_uid(uid);
	stLoginEr.set_code(code);	
	user_client->unicast(SERVER_LOGIN_ERR_UC, stLoginEr);
	//广播退出房间协议,防止客户端断线后又发登录请求，一直死循环
	SendLogout(user_client, uid, code);
	Client::pre_destroy( user_client );
	return 0;
}

int Game::connect_videosvr()
{   
	if (zjh.conf["video-svr"].isNull())
	{
		return -1;
	}
	log_info("videosvr management ip[%s] port[%d] ",
		zjh.conf["video-svr"]["host"].asString().c_str(),
		zjh.conf["video-svr"]["port"].asInt() ); 

	struct sockaddr_in addr;
	int fd = socket(PF_INET, SOCK_STREAM, 0);	
	if (fd < 0) 
	{
		log_error("File[%s] Line[%d]: socket failed: %s ", __FILE__, __LINE__, strerror(errno));
		return -1;
	}
	addr.sin_family = AF_INET;
	addr.sin_port = htons(zjh.conf["video-svr"]["port"].asInt());
	addr.sin_addr.s_addr = inet_addr(zjh.conf["video-svr"]["host"].asString().c_str());
	if (addr.sin_addr.s_addr == INADDR_NONE) 
	{
		log_error("game::connect_videosvr Incorrect ip address!");
		close(fd);
		fd = -1;
		exit(1);
	}
	if( connect(fd, (struct sockaddr*)&addr, sizeof(addr)) < 0)
	{
		close(fd);
		log_error("connect video svr error: %s(errno: %d) ", strerror(errno), errno);
		fd = -1;
		exit(1);
	}
	fcntl(fd, F_SETFL, fcntl(fd, F_GETFL, 0) | O_NONBLOCK);
	link_video_client = new(std::nothrow) Client( fd, false, 1);
	if( NULL != link_video_client )
	{
		log_info("connect video svr fd[%d]. ", fd );
		//		fd_client_map[link_data_client->fd] = link_data_client;
		//register_server();
	}
	else
	{
		close(fd);
		log_error("new video svr client error.\n");
		//exit(1);
	}
	return 0;
}

int Game::reconnect_videosvr()
{
	struct sockaddr_in addr;
	int fd = socket(PF_INET, SOCK_STREAM, 0);
	if (fd < 0) 
	{
		log_error("File[%s] Line[%d]: socket failed: %s ", __FILE__, __LINE__, strerror(errno));
		return -1;
	}
	addr.sin_family = AF_INET;
	addr.sin_port = htons(zjh.conf["video-svr"]["port"].asInt());
	addr.sin_addr.s_addr = inet_addr(zjh.conf["video-svr"]["host"].asString().c_str());
	if (addr.sin_addr.s_addr == INADDR_NONE) 
	{
		log_error("game::reconnect_videosvr Incorrect ip address!");
		close(fd);
		fd = -1;
		return -1;
	}
	if( connect(fd, (struct sockaddr*)&addr, sizeof(addr)) < 0)
	{
		close(fd);
		log_error("reconnect videosvr error: %s(errno: %d) ", strerror(errno), errno);
		return -1;
	}
	fcntl(fd, F_SETFL, fcntl(fd, F_GETFL, 0) | O_NONBLOCK);
	link_video_client = new (std::nothrow) Client( fd, false, 1);
	if( link_video_client )
	{
		log_info("%s videosvr link fd[%d]. ", __FUNCTION__, fd);
		//fd_client_map[link_data_client->fd] = link_data_client;
		ev_timer_stop(zjh.loop, &ev_reconnect_videosvr_timer );
		//register_server();
	}
	log_info("  reconnect! " );
	return 0;
}

void Game::send_to_videosvr(const std::string &res)
{
	if( NULL != link_video_client )
	{
		link_video_client->send_as_json(res);
	}
	else
	{
		log_error("link_data client is NULL, send to videosvr failed \n");
	}
}

void Game::start_videoreconnect()
{
	ev_timer_again( zjh.loop, &ev_reconnect_videosvr_timer );
}
void Game::reconnect_videosvr_cb( struct ev_loop *loop, struct ev_timer *w, int revents )
{
	Game *p_game = (Game*)w->data;
	p_game->reconnect_videosvr();
}

void Game::game_user_offline(Player *player)
{
	if (player == NULL)
	{
		return;
	}
	if (all_tables.find(player->tid_str) != all_tables.end()) 
	{		
		//玩家一分钟后掉线		 
		//all_tables[player->tid_str]->User_Offline(player);
		Table* table = all_tables[player->tid_str];
		if (player->seatid >= 0 && player->seatid < table->m_GAME_PLAYER)
		{
			table->m_IsOffline[player->seatid] = true;
		}
		table->Check_All_Offline();
	}
}

void Game::robot_success(Player *player)
{
	if (NULL == player)
	{
		log_error("robot_success error player is null");
		return;
	}

	proto::login::AckLoginSuccessUc stLoginuc;
	stLoginuc.set_seatid(player->seatid);
	stLoginuc.set_uid(player->uid);
	stLoginuc.set_money(player->money);
	if (player->name != "")
	{
		stLoginuc.set_name(player->name.c_str());
	}
	if (player->avatar != "")
	{
		stLoginuc.set_avatar(player->avatar.c_str());
	}
	stLoginuc.set_sex(player->sex);
	stLoginuc.set_coin(player->coin);
	stLoginuc.set_exp(player->exp);
	stLoginuc.set_rmb(player->rmb);
	stLoginuc.set_vlevel(player->vlevel);

	player->client->unicast(SERVER_LOGIN_SUCC_UC, stLoginuc);
}

// void Game::delete_table(const std::string& roomID)
// {
// 	std::map<std::string, Table*>::iterator iter2 = all_tables.find(roomID);
// 	if( iter2 != all_tables.end() )
// 	{	
// 		Table *table = iter2->second;
// 		all_tables.erase( iter2 );
// 		if (table)
// 		{
// 			delete table;
// 			table = NULL;
// 		}
// 		log_info("delete_table tid:%s  now delete it, now total tables:%d \n", roomID.c_str(), all_tables.size());		
// 	}
// }

bool Game::DeleteRoomReq(const string& roomid)
{
	bool bIsAll = true;
	map<string, Table *>::iterator iter = all_tables.find(roomid);
	std::vector<Player*> temp_del_vec;
	if (iter != all_tables.end())
	{
		Table *table = iter->second;
		if (table)
		{
			//检测是否所有玩家都已经断线
			map<int, Player *>::iterator iter = table->m_mapTable_Player.begin();
			for (; iter != table->m_mapTable_Player.end(); iter++)
			{
				Player *player = iter->second;
				if (player)
				{
					if (player->client != NULL)
					{
						bIsAll = false;
						break;
					}
					temp_del_vec.push_back(player);
				}
			}
		}
	}
	log_info(" delete tid:%s user num:%d ",  roomid.c_str(), temp_del_vec.size());

	Jpacket packet;
	packet.val["cmd"] = SERVER_DELETE_ROOM_RESP;
	packet.val["vid"] = zjh.game->server_vid;
	packet.val["roomid"] = roomid;
	if (bIsAll)
	{
		packet.val["type"] = 1;
		if (iter != all_tables.end())
		{
			Table *table = iter->second;
			all_tables.erase(roomid);
			if (table)
			{				
				delete table;
				table = NULL;
			}

			log_info("  delete table! tid:%s. left tables:%d ",   roomid.c_str(), all_tables.size());
			for( std::vector<Player*>::iterator iter2 = temp_del_vec.begin(); iter2 != temp_del_vec.end(); iter2++ )
			{
				Player *player = *iter2;
				if (player)
				{
					log_info("delete player tid[%s] uid[%d] ",   roomid.c_str(), player->uid );
					delete player;
					player = NULL;
					
				}
			}
		}
	}
	else
	{
		packet.val["type"] = 0;
	}
	packet.end();
	zjh.game->send_to_datasvr(packet.tostring());
	return true;
}


void Game::sys_get_user_num(Client *client)
{
	int iUserCount = fd_client_map.size();
	Jpacket packet;
	packet.val["cmd"] = SYS_GET_USER_NUM;
	packet.val["usernum"] = iUserCount;
	packet.val["gameid"] = zjh.conf["sever-id"]["src_svr_id"].asInt();
	packet.end();

	send_to_datasvr(packet.tostring());    
	log_info("  num:%d " , iUserCount);
}


int Game::request_room_conf(Client *client, const std::string& roomid)
{
	if (tid_client_map.find(roomid) == tid_client_map.end()) {
		Jpacket pac;
		pac.val["cmd"] = SERVER_ROOM_CONFIG_REQ;
		pac.val["roomId"] = roomid;
		pac.end();
		send_to_datasvr(pac.tostring());
	}
	std::vector<Client*> temp_client =tid_client_map[roomid];
	bool bFind = false;
	for (std::vector<Client*>::iterator it_c = temp_client.begin(); it_c != temp_client.end(); it_c++) 
	{
		if ((*it_c) == client)
		{
			bFind = true;
		}
	}
	if (!bFind)
	{
		tid_client_map[roomid].push_back(client);
	}
	return 0;
}

int Game::handler_room_conf_res(Json::Value &val)
{
	std::string roomid = val["roomId"].asString();
	int status = val["status"].asInt();
	std::map<std::string, std::vector<Client*> >::iterator it = tid_client_map.find(roomid);
	if (it != tid_client_map.end()) 
	{
		//获取配置信息失败
		if (status != 200)
		{
			log_error("%s room[%s] conf res error   ", __FUNCTION__, roomid.c_str());
			std::vector<Client*> temp_del_client = tid_client_map[roomid];
			for (std::vector<Client*>::iterator it_c = temp_del_client.begin(); it_c != temp_del_client.end(); it_c++) {
				SendLoginFail(*it_c, (*it_c)->uid, 2);
				SendLogout(*it_c, (*it_c)->uid, -1);
				//(*it_c)->player = NULL;
				del_client(*it_c);
			}
			tid_client_map.erase(roomid); 
			return -1;
		}
		std::vector<Client*>::iterator it_v = it->second.begin();
		while (it_v != it->second.end()) 
		{
			int ret = handler_login_table( (*it_v)->uid, *it_v, val);
			if( 0 == ret )
			{
				(*it_v)->Set_Beat_Time(NODATA_TIMEOUT);
				inform_datasvr_user_vid(  (*it_v)->uid , 1 );
			}
			else {
				//(*it_v)->player = NULL;
				del_client(*it_v);
			}
			it_v = it->second.erase(it_v);
		}
		tid_client_map.erase(roomid);
		return 0;
	}
	return -1;
}
void Game::SendLoginFail(Client *client, int uid, int code)
{
	proto::login::AckLoginError stLoginEr;
	stLoginEr.set_uid(uid);
	stLoginEr.set_code(code);
	client->unicast(SERVER_LOGIN_ERR_UC, stLoginEr);
	Client::pre_destroy( client );
}

void Game::SendLogout(Client *client, int uid, int code)
{
	//广播退出房间协议,防止客户端断线后又发登录请求，一直死循环	
	proto::login::AckLogoutBc stLogout;
	stLogout.set_seatid(-1);
	stLogout.set_uid(uid);
	stLogout.set_type(code);
	log_info("%s uid:%d loging error code:%d logout out room ", __FUNCTION__, uid, code);	
	client->unicast(SERVER_LOGOUT_SUCC_BC, stLogout);
}



void Game::player_offline_timeout(Player *player)
{
	if (NULL == player)
	{   
		log_error("player_offline_timeout is error");
		return;
	}
	log_info("%s tid:%d player is timeout uid:%d ", __FUNCTION__, player->tid_str.c_str(), player->uid);
	if (all_tables.find(player->tid_str) != all_tables.end())
	{
		map<std::string, Table*>::iterator iter = all_tables.find(player->tid_str);
		Table *table = iter->second;
		if (table)
		{
			if (table->m_bDeleteTable || (player->seatid < 0 || player->seatid >= GAME_PLAYER))  //如果桌子已经解散
			{
				del_player(player);
			}
		}
	}
	else
	{
		log_info(" del_player %d ",  player ? player->uid : 0);
	    del_player(player);
	}
		
}

void Game::logout_room_res(const Json::Value& val)
{
	std::string roomid = val["roomid"].asString();
	if (roomid.empty())
	{
		log_error("logout_room_res: roomid is null.\n");
		return;
	}
		
	if (all_tables.find(roomid) != all_tables.end())
	{
		int uid = val["uid"].asInt();
		int status = val["status"].asInt();
		int is_success = val["is_success"].asInt();
		log_debug("%s tid:%s uid:%d status:%d is_success:%d ", __FUNCTION__, roomid.c_str(), uid, status, is_success);

		if (status == 200 && is_success == 1)
		{
			Table* table = all_tables[roomid];
			if (table->m_mapTable_Player.find(uid) != table->m_mapTable_Player.end())
			{
				Player* player = table->m_mapTable_Player[uid];
				del_player(player);
			}			
		}
	}
}
