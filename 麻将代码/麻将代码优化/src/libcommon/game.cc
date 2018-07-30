#include "game.h"
#include "redis_client.h"

extern PHZ phz;
extern Log log;

Game::Game()
{
	_fd = 0;
	link_data_client = NULL;
	ev_reconnect_datasvr_tstamp = 0.3;
	ev_reconnect_datasvr_timer.data = this;
	ev_timer_init( &ev_reconnect_datasvr_timer, Game::reconnect_datasvr_cb, ev_reconnect_datasvr_tstamp, ev_reconnect_datasvr_tstamp );
	link_video_client = NULL;
	ev_reconnect_videosvr_timer.data = this;
	ev_reconnect_videosvr_tstamp = 0.3;
	ev_timer_init( &ev_reconnect_videosvr_timer, Game::reconnect_videosvr_cb, ev_reconnect_videosvr_tstamp, ev_reconnect_videosvr_tstamp );
	server_zid = phz.conf["tables"]["zid"].asInt();
	server_vid = phz.conf["tables"]["vid"].asInt();
	log.info("game svr zid[%d] vid[%d].\n", server_zid, server_vid );
}

Game::~Game()
{
	if( NULL != link_data_client ){
		delete link_data_client;
	}
	if( NULL != link_video_client ){
		delete link_video_client;
	}
	ev_timer_stop( phz.loop, &ev_reconnect_datasvr_timer );
	ev_timer_stop( phz.loop, &ev_reconnect_videosvr_timer);
}

void Game::start()
{
	connect_datasvr();
	connect_videosvr();
	RequestDataFromRedis();
	init_accept();
}

int Game::init_accept()
{
	log.info("Listening on %s:%d\n",
				phz.conf["game"]["host"].asString().c_str(),
				phz.conf["game"]["port"].asInt());
	struct sockaddr_in addr;

	_fd = socket(PF_INET, SOCK_STREAM, 0);
	if (_fd < 0) {
		log.error("File[%s] Line[%d]: socket failed: %s\n", __FILE__, __LINE__, strerror(errno));
	}
	addr.sin_family = AF_INET;
	addr.sin_port = htons(phz.conf["game"]["port"].asInt());
	addr.sin_addr.s_addr = inet_addr(phz.conf["game"]["host"].asString().c_str());
	if (addr.sin_addr.s_addr == INADDR_NONE) {
		log.error("game::init_accept Incorrect ip address!");
		close(_fd);
		_fd = -1;
		exit(1);
	}
	int on = 1;
	if (setsockopt(_fd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) < 0) {
		log.error("File[%s] Line[%d]: setsockopt failed: %s\n", __FILE__, __LINE__, strerror(errno));
		close(_fd);
		exit(1);
		return -1;
	}
	if (bind(_fd, (struct sockaddr *) &addr, sizeof(addr)) < 0) {
		log.error("File[%s] Line[%d]: bind failed: %s\n", __FILE__, __LINE__, strerror(errno));
		close(_fd);
		exit(1);
		return -1;
	}
	fcntl(_fd, F_SETFL, fcntl(_fd, F_GETFL, 0) | O_NONBLOCK);
	listen(_fd, 10000);
	_ev_accept.data = this;
	ev_io_init(&_ev_accept, Game::accept_cb, _fd, EV_READ);
	ev_io_start(phz.loop, &_ev_accept);
	log.info("listen ok\n");
	return 0;
}

int Game::connect_datasvr()
{
	log.info("datasvr management ip[%s] port[%d].\n",
				phz.conf["data-svr"]["host"].asString().c_str(),
				phz.conf["data-svr"]["port"].asInt() );
	struct sockaddr_in addr;
	int fd = socket(PF_INET, SOCK_STREAM, 0);
	if (fd < 0){
		log.error("File[%s] Line[%d]: socket failed: %s\n", __FILE__, __LINE__, strerror(errno));
		close(fd);
		exit(1);
	}
	addr.sin_family = AF_INET;
	addr.sin_port = htons(phz.conf["data-svr"]["port"].asInt());
	addr.sin_addr.s_addr = inet_addr(phz.conf["data-svr"]["host"].asString().c_str());
	if (addr.sin_addr.s_addr == INADDR_NONE){
		log.error("game::connect_datasvr Incorrect ip address!");
		close(fd);
		exit(1);
	}
	if( connect(fd, (struct sockaddr*)&addr, sizeof(addr)) < 0){
		close(fd);
		log.error("connect data svr error: %s(errno: %d)\n", strerror(errno), errno);
		exit(1);
	}
	fcntl(fd, F_SETFL, fcntl(fd, F_GETFL, 0) | O_NONBLOCK);
	link_data_client = new(std::nothrow) Client( fd, false, 1);
	if( NULL != link_data_client ){
		log.info("connect data svr fd[%d].\n", fd );
	}
	else{
		close(fd);
		log.error("new data svr client error.\n");
		exit(1);
	}
	return 0;

}

void Game::start_reconnect()
{
	ev_timer_again( phz.loop, &ev_reconnect_datasvr_timer );
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
	if (fd < 0){
		log.error("File[%s] Line[%d]: socket failed: %s\n", __FILE__, __LINE__, strerror(errno));
		return -1;
	}
	addr.sin_family = AF_INET;
	addr.sin_port = htons(phz.conf["data-svr"]["port"].asInt());
	addr.sin_addr.s_addr = inet_addr(phz.conf["data-svr"]["host"].asString().c_str());
	if (addr.sin_addr.s_addr == INADDR_NONE)
	{
		log.error("game::reconnect_datasvr Incorrect ip address!");
		close(fd);
		fd = -1;
		return -1;
	}
	if( connect(fd, (struct sockaddr*)&addr, sizeof(addr)) < 0)
	{
		close(fd);
		log.error("reconnect datasvr error: %s(errno: %d)\n", strerror(errno), errno);
		return -1;
	}
	fcntl(fd, F_SETFL, fcntl(fd, F_GETFL, 0) | O_NONBLOCK);
	link_data_client = new (std::nothrow) Client( fd, false, 1);
	if( link_data_client ){
		log.info("reconnect datasvr link fd[%d].\n", fd );
		ev_timer_stop(phz.loop, &ev_reconnect_datasvr_timer );
	}
	return 0;
}

void Game::send_to_datasvr( const std::string &res )
{
	if( NULL != link_data_client ){
		link_data_client->send( res );
	}
	else{
		log.error("link_data client is NULL, send to datasvr failed \n");
	}
}


//连接录像服
int Game::connect_videosvr()
{
	if (phz.conf["video-svr"].isNull()){
		return -1;
	}
	log.info("videosvr management ip[%s] port[%d].\n",
				phz.conf["video-svr"]["host"].asString().c_str(),
				phz.conf["video-svr"]["port"].asInt() );
	struct sockaddr_in addr;
	int fd = socket(PF_INET, SOCK_STREAM, 0);
	if (fd < 0){
		log.error("File[%s] Line[%d]: socket failed: %s\n", __FILE__, __LINE__, strerror(errno));
		return -1;
	}
	addr.sin_family = AF_INET;
	addr.sin_port = htons(phz.conf["video-svr"]["port"].asInt());
	addr.sin_addr.s_addr = inet_addr(phz.conf["video-svr"]["host"].asString().c_str());
	if (addr.sin_addr.s_addr == INADDR_NONE){
		log.error("game::connect_videosvr Incorrect ip address!");
		close(fd);
		fd = -1;
		exit(1);
	}
	if( connect(fd, (struct sockaddr*)&addr, sizeof(addr)) < 0){
		close(fd);
		log.error("connect video svr error: %s(errno: %d)\n", strerror(errno), errno);
		fd = -1;
		exit(1);
	}
	fcntl(fd, F_SETFL, fcntl(fd, F_GETFL, 0) | O_NONBLOCK);
	link_video_client = new(std::nothrow) Client( fd, false, 1);
	if( NULL != link_video_client ){
		log.info("connect video svr fd[%d].\n", fd );
	}
	else{
		close(fd);
		log.error("new video svr client error.\n");
	}
	return 0;
}

void Game::start_videoreconnect()
{
	ev_timer_again( phz.loop, &ev_reconnect_videosvr_timer );
}

void Game::reconnect_videosvr_cb( struct ev_loop *loop, struct ev_timer *w, int revents )
{
	Game *p_game = (Game*)w->data;
	p_game->reconnect_videosvr();
}

int Game::reconnect_videosvr()
{
	struct sockaddr_in addr;
	int fd = socket(PF_INET, SOCK_STREAM, 0);
	if (fd < 0)
	{
		log.error("File[%s] Line[%d]: socket failed: %s\n", __FILE__, __LINE__, strerror(errno));
		return -1;
	}
	addr.sin_family = AF_INET;
	addr.sin_port = htons(phz.conf["video-svr"]["port"].asInt());
	addr.sin_addr.s_addr = inet_addr(phz.conf["video-svr"]["host"].asString().c_str());
	if (addr.sin_addr.s_addr == INADDR_NONE)
	{
		log.error("game::reconnect_videosvr Incorrect ip address!");
		close(fd);
		fd = -1;
		return -1;
	}
	if( connect(fd, (struct sockaddr*)&addr, sizeof(addr)) < 0)
	{
		close(fd);
		log.error("reconnect videosvr error: %s(errno: %d)\n", strerror(errno), errno);
		return -1;
	}
	fcntl(fd, F_SETFL, fcntl(fd, F_GETFL, 0) | O_NONBLOCK);
	link_video_client = new (std::nothrow) Client( fd, false, 1);
	if( link_video_client )
	{
		log.info("reconnect videosvr link fd[%d].\n", fd );
		ev_timer_stop(phz.loop, &ev_reconnect_videosvr_timer );
	}
	return 0;
}

void Game::send_to_videosvr(const std::string &res)
{
	if( NULL != link_video_client ){
		link_video_client->send( res );
	}
	else{
		log.error("link_data client is NULL, send to videosvr failed \n");
	}
}


void Game::del_all_client()
{
	std::map<int, Client*> temp_fd_client_map = fd_client_map;
	log.info("for disconnect to datasvr, delete all client fd_client_len[%d] temp_fd_client_map[%d]  .\n", fd_client_map.size(), temp_fd_client_map.size() );
	for(std::map<int, Client*>::iterator iter = temp_fd_client_map.begin(); iter != temp_fd_client_map.end(); iter++ ){
		Client::pre_destroy( iter->second );
	}
}

void Game::accept_cb(struct ev_loop *loop, struct ev_io *w, int revents)
{
	if (EV_ERROR & revents) {
		log.error("got invalid event\n");
		return;
	}
	struct sockaddr_in client_addr;
	socklen_t client_len = sizeof(client_addr);

	int fd = accept(w->fd, (struct sockaddr *) &client_addr, &client_len);
	if (fd < 0) {
		log.error("accept error[%s]\n", strerror(errno));
		return;
	}
	Game *game = (Game*) (w->data);
	if( NULL == game->link_data_client ){
		close(fd);
		log.error("accept client error for the link of datasvr disconnect.\n");
		return;
	}
	fcntl(fd, F_SETFL, fcntl(fd, F_GETFL, 0) | O_NONBLOCK);
	Client *client = new (std::nothrow) Client(fd);
	if (client != NULL){
		client->string_ip = inet_ntoa(client_addr.sin_addr);
		game->fd_client_map[fd] = client;
	}
	else{
		close(fd);
	}
}

int Game::request_login(Client *client)
{
	proto::game::ReqLogin stLogin;
	stLogin.ParseFromString(client->ppacket.body);
	log.debug("request login stLogin[%s] .\n", stLogin.DebugString().c_str());
	client->uid = stLogin.uid();
	client->skey = stLogin.skey();
	client->roomid = stLogin.roomid();
	log.info("request login uid[%d] roomid[%s]\n", client->uid, client->roomid.c_str());
	if (all_tables.find(client->roomid) != all_tables.end()){
		Table *table = all_tables[client->roomid];
		if (table->is_dissolved == 1) {
			log.info("request login error table dissolved uid[%d] roomid[%s].\n", client->uid, client->roomid.c_str() );
			dz_login_error_back(client, proto::game::ROOM_STATUS_ERR);
			Client::pre_destroy(client);
			return -1;
		}
		if (table->players.find(client->uid) != table->players.end()){
			Player* player = table->players[client->uid];
			if(player != NULL){
				//检验skey
				if(player->skey == client->skey) {
					Client *oldClient = player->client;
					if( oldClient && oldClient->fd != client->fd ){
						SendLogout(oldClient, client->uid, -1);
						oldClient->player = NULL;
						Client::pre_destroy(oldClient, true);
					}
					player->set_client(client);
					client->set_positon(POSITION_TABLE);
					player->stop_offline_timer();
					player->is_offline = 0;
					all_tables[player->tid]->handler_login_succ_uc(player);
					all_tables[player->tid]->handler_login_succ_bc(player);
					all_tables[player->tid]->handler_table_info(player);
					log.info("request login uid[%d] roomid[%s] reconnect ok .\n", player->uid, player->tid.c_str());
					return 0;
				}
			}
		}
	}
	if(uid_client_map.find(client->uid) != uid_client_map.end()){
		if( uid_client_map[client->uid]->fd != client->fd ){
			del_client(uid_client_map[client->uid]);
		}
	}
	uid_client_map[client->uid] = client;
	//json发送给平台验证
	Jpacket jpac;
	jpac.val["cmd"] = CLIENT_DZ_LOGIN_REQ;
	jpac.val["uid"] = client->uid;
	jpac.val["skey"] = client->skey;
	jpac.val["roomid"] = client->roomid;
	jpac.val["vid"] = server_vid;
	jpac.val["zid"] = server_zid;
	int isGetRoomInfo = 0;
	if(all_tables.find(client->roomid) == all_tables.end()){
		isGetRoomInfo = 1;
	}
	jpac.val["isGetRoomInfo"] = isGetRoomInfo;
	jpac.end();
	send_to_datasvr(jpac.tostring());
	return 0;
}

int Game::login_succ_back( Json::Value &val )
{
	int uid = 0;
	if( !val["uid"].isNull() && val["uid"].isNumeric() )
	{
		uid = val["uid"].asInt();
	}
	std::map<int,Client*>::iterator iter = uid_client_map.find( uid );
	if( iter == uid_client_map.end() )
	{
		log.error("the player uid[%d] socket is not in map.\n", uid );
		return 0;
	}
	Client *client = iter->second;
	uid_client_map.erase(uid);
	if (!val["roomStatus"].isNull() && val["roomStatus"].isNumeric() )
	{
		int room_status = val["roomStatus"].asInt();
		if( room_status >= 2 )//房间已经结束或者解散
		{
			log.error("login succ back roomid[%s] status[%d] over or dissovled .\n", client->roomid.c_str(), room_status);
			dz_login_error_back(client, proto::game::ROOM_STATUS_ERR);
			Client::pre_destroy(client);
			return -1;
		}
	}
	if( all_tables.find( client->roomid ) != all_tables.end() )
	{
		Table* table = all_tables[client->roomid];
		if( table->players.find( client->uid) != table->players.end() )
		{
			Player* player = table->players[client->uid];
			if( player )
			{
				Client *oldClient = player->client;
				if( oldClient && oldClient->fd != client->fd )
				{
					SendLogout(oldClient, client->uid, -1);
					oldClient->player = NULL;
					Client::pre_destroy(oldClient, true);
				}
				player->set_client(client);
				client->set_positon(POSITION_TABLE);
				player->stop_offline_timer();
				player->is_offline = 0;
				all_tables[player->tid]->handler_login_succ_uc(player);
				all_tables[player->tid]->handler_login_succ_bc(player);
				all_tables[player->tid]->handler_table_info(player);
				if(!val["skey"].isNull() && val["skey"].isString()) //更新skey
				{
					player->skey = val["skey"].asString();
				}
				log.info("login succ back uid[%d] roomid[%s] reconnect ok .\n", player->uid, player->tid.c_str());
				return 0;
			}
		}
	}
	Player *player = new (std::nothrow) Player();
	if (player == NULL)
	{
		log.error("login succ back new player err");
		return 0;
	}
	int ret = player->init( val );
	if( ret < 0 )
	{
		dz_login_error_back(client, proto::game::USER_NO_MONEY_LOGIN_ERR);
		Client::pre_destroy(client);
	}
	player->set_client( client );
	log.info("login succ back one player[%d] new success.\n", uid );
	handler_login_table( client, val );
	return 0;
}

int Game::dz_login_error_back( Client *client, int code)
{
	proto::game::AckLoginError stLoginEr;
	stLoginEr.set_uid(client->uid);
	stLoginEr.set_code(code);
	Ppacket ppack;
	stLoginEr.SerializeToString(&ppack.body);
	ppack.pack(SERVER_DZ_LOGIN_ERR_RES);
	client->send(ppack.data);
	log.debug("dz login error uid[%d] stLoginEr[%s] .\n", client->uid, stLoginEr.DebugString().c_str());
	log.info("dz login error uid[%d] code[%d].\n", client->uid, code);
	return 0;
}

int Game::login_error_back(int uid, int code)
{
	std::map<int, Client*>::iterator iter = uid_client_map.find(uid);
	if (iter == uid_client_map.end()) 
	{
		log.error("login error back uid[%d] is not in socket map\n", uid);
		return 0;
	}
	Client *client = iter->second;
	dz_login_error_back(client, code);
	//广播退出房间协议,防止客户端断线后又发登录请求，一直死循环
	SendLogout(client, client->uid, -1);
	Client::pre_destroy(client);
	return 0;
}

void Game::del_client(Client *client, bool del_oldclient)
{
	if (client == NULL){
		log.error("del client error client NULL .\n");
		return;
	}
	if (client == link_data_client){
		del_all_client();
		delete link_data_client;
		link_data_client = NULL;
		start_reconnect();
		return;
	}
	if (client == link_video_client){
		delete link_video_client;
		link_video_client = NULL;
		start_videoreconnect();
		return; 
	}
	//顶号问题
	if (del_oldclient || client->cmd_type == 1 ){
		log.info("delete old client, fd:%d \n", client->fd);
		fd_client_map.erase(client->fd);
		delete client;
		client = NULL;
		return;
	}
	if (fd_client_map.find(client->fd) == fd_client_map.end()){
		log.error("del client free client err[miss] fd[%d].\n", client->fd );
		return;
	}
	fd_client_map.erase(client->fd);
	if(uid_client_map.find( client->uid ) != uid_client_map.end()){
		uid_client_map.erase( client->uid );
	}
	// todo
	if (client->player){
		Player *player = client->player;
		if (client->position == POSITION_WAIT){
			del_player(player);
		}
		else{
			player->client = NULL;
			player->start_offline_timer();
		}
	}
	log.info("del client fd[%d] uid[%d].\n", client->fd, client->uid);
	std::string room_id = client->roomid;
	delete client;
	client = NULL;
	if (table_no_players(room_id)) {
		Jpacket pac;
		pac.val["cmd"] = SERVER_ALL_PLAYER_DOWN;
		pac.val["roomid"] = room_id;
		pac.val["vid"] = server_vid;
		pac.end();
		send_to_datasvr(pac.tostring());
	}	
}

int Game::table_no_players(std::string _roomid)
{
	if (all_tables.find(_roomid) == all_tables.end()) {
		log.error("table no players room[%s] is not find\n", _roomid.c_str());
		return 2;
	}
	Table *temp_table = all_tables[_roomid];
	std::map<int, Player*>::iterator it = temp_table->players.begin();
	for (; it != temp_table->players.end(); it++) {
		if (it->second->client != NULL) {
			return 0;
		}
	}
	return 1;
}

//平台返回
int Game::dispatch_json(Client *client)
{
	client->cmd_type = 0;
	int cmd = client->packet.sefe_check();
	if (cmd < 0)
	{
		log.error("the cmd format is error.\n");
		return -1;
	}
	log.info("dispatch_json client fd[%d] uid[%d] recv cmd[%d]\n", client->fd, client->uid, cmd);
	if( cmd == SERVER_DZ_LOGIN_ERR_RES ){
		int uid = 0;
		if( !(client->packet.val["uid"].isNull()) && client->packet.val["uid"].isNumeric() ){
			uid = client->packet.val["uid"].asInt();
		}
		int code = 0;
		if( !(client->packet.val["code"].isNull()) && client->packet.val["code"].isNumeric() ){
			code = client->packet.val["code"].asInt();
		}
		login_error_back(uid, code);
		return 0;
	}
	else if( cmd == SERVER_DZ_LOGIN_SUCC_RES ){
		login_succ_back( client->packet.tojson() );
		return 0;
	}
	else if( cmd == SERVER_DZ_GAME_START_RES ){
		game_start_res( client->packet );
		return 0;
	}
	else if (cmd == SERVER_DZ_DISSOLVE_ROOM_RES) {
		game_start_res( client->packet );
		return 0;
	}
	//请求删除房间
	else if( cmd == SERVER_DELETE_ROOM_REQ ) {
		std::string roomid;
		if( !(client->packet.val["roomid"].isNull()) && client->packet.val["roomid"].isString() ){
			roomid = client->packet.val["roomid"].asString();
		}
		DeleteRoomReq(roomid);
		return 0;
	}
	else if( cmd == SERVER_VIDEO_DATA_ERR_RESP ){
		int roomID = 0;
		if( !(client->packet.val["roomid"].isNull()) && client->packet.val["roomid"].isNumeric() ){
			roomID = client->packet.val["roomid"].asInt();
		}
		log.info("save vedio data resp roomID:%d .\n", roomID);
		return 0;
	}
	return 0;
}

int Game::dispatch_proto(Client *client)
{
	int cmd = client->ppacket.safe_check();
	//排除心跳
	if( cmd != CLINET_HEART_BEAT_REQ && cmd != SERVER_HEART_BEAT_RES ) {
		log.info("dispatch_proto client fd[%d] uid[%d] recv cmd[%d]\n", client->fd, client->uid, cmd );
	}
	if (cmd < 0) {
		log.error("the cmd format is error.\n");
		return -1;
	}
	if( client == link_data_client || client == link_video_client ){
		return 0;
	}
	if(cmd == CLINET_HEART_BEAT_REQ){
		Ppacket ppack;
		ppack.pack(SERVER_HEART_BEAT_RES);
		client->send(ppack.data);
		return 0;
	}
	if ( cmd == CLIENT_DZ_LOGIN_REQ ){
		return request_login( client );
	}
	if (safe_check(client, cmd) < 0) {
		return -1;
	}
	Player *player = client->player;
	return all_tables[player->tid]->HandlerUserOperate(cmd, player);
}

int Game::safe_check(Client *client, int cmd)
{
	Player *player = client->player;
	if (player == NULL) {
		log.error("safe check client player is NULL.\n");
		return -1;
	}
	if (all_tables.find(player->tid) == all_tables.end()){
		log.error("safe_check uid[%d] is not in tid[%s]\n", player->uid, player->tid.c_str());
		return -1;
	}
	return 0;
}

int Game::handler_login_table(Client *client, Json::Value &val)
{
	if (client == NULL) 
	{
		log.error("handler login table error client NULL\n");
		return -1;
	}
	Player *player = client->player;
	if (player == NULL) 
	{
		log.error("handler login table error player NULL\n", client->fd);
		return -1;
	}
	if (client->position == POSITION_TABLE) 
	{
		log.error("handler login table uid[%d] have been in table\n", player->uid);
		return -1;
	}
	return login_table(client, val);
}

int Game::login_table(Client *client, Json::Value &val)
{
	if (client == NULL){
		log.error("login table client is null\n");
		return -1;
	}
	Player *player = client->player;
	std::string roomid = client->roomid;
	if( !roomid.empty() ){
		if( all_tables.find( roomid ) == all_tables.end() ){
			Table *table = Table::CreateLocationTable(phz.conf["table"]["location"].asString());
			if(table == NULL){
				log.error("new %s error\n", phz.conf["table"]["location"].asString().c_str());
				return -1;
			}
			table->Init(val, roomid);
			all_tables[roomid] = table;
			log.info("login table total tables[%d].\n", all_tables.size() );
		}
		client->set_positon( POSITION_TABLE );
		all_tables[roomid]->handler_login( player );
		return 0;
	}
	log.error( "login table error uid[%d] roomid is NULL.\n", player->uid );
	return -1;
}

int Game::del_player(Player *player)
{
	int ret = 0;
	int uid = player->uid;
	std::string strtid = player->tid;
	if (all_tables.find(player->tid) != all_tables.end()) {
		player->logout_type = 1;
		ret = all_tables[player->tid]->handler_logout(player);
		if (ret < 0) {
			log.error("del player handler logout error uid[%d].\n", player->uid);
			return -1;
		}
		ret = all_tables[player->tid]->del_player(player);
		if (ret < 0) {
			log.error("del player del player error uid[%d] .\n", player->uid);
			return -1;
		}
	}
	if (player->client) {
		Client *client = player->client;
		client->position = POSITION_WAIT;
		Client::pre_destroy(client);
	}
	log.info("del player[%p] uid[%d]\n", player, player->uid);
	delete player;
	player = NULL;
	//通知业务平台玩家退出房间
	Jpacket packet;
	packet.val["cmd"] = SERVER_LOGOUT_ROOM_REQ;
	packet.val["uid"] = uid;
	packet.val["vid"] = server_zid;
	packet.val["roomid"] = strtid;
	packet.end();
	send_to_datasvr(packet.tostring());
	return 0;
}

void Game::game_start_res( Jpacket &packet )
{
	std::string roomid;
	if( !(packet.val["roomid"].isNull()) && packet.val["roomid"].isString() ){
		roomid = packet.val["roomid"].asString();
	}
	if( all_tables.find( roomid ) != all_tables.end() ){
		all_tables[roomid]->game_start_res( packet );
	}
}

bool Game::DeleteRoomReq(std::string roomid)
{
	int ret = table_no_players(roomid);
	Jpacket packet;
	packet.val["cmd"] = SERVER_DELETE_ROOM_RESP;
	packet.val["vid"] = server_vid;
	packet.val["roomid"] = roomid;
	packet.val["type"] = ret > 0 ? 1 : 0;
	packet.end();
	send_to_datasvr(packet.tostring());
	if (ret == 1){
		std::map<std::string, Table*>::iterator it = all_tables.find(roomid);
		if(it != all_tables.end()){
		    DelDataFromRedis(roomid);
			Table *tm = it->second;
			if(tm != NULL){
				std::map<int, Player*>::iterator it_p = tm->players.begin();
				for(; it_p != tm->players.end(); ++it_p){
					Player *p = it_p->second;
					if(p != NULL){
						log.info("tid[%s] delete player[%d]\n", roomid.c_str(), p->uid);
						delete p;
						p = NULL;
					}
				}
			}
			all_tables.erase(roomid);
			delete tm;
			tm = NULL;			
			log.info("delete tid:%s, now total tables:%d \n", roomid.c_str(), all_tables.size());
		}
	}
	return true;
}

void Game::SendLogout(Client *client, int uid, int code)
{
	//广播退出房间协议,防止客户端断线后又发登录请求，一直死循环
	proto::game::AckLogoutBc stLogout;
	stLogout.set_uid(uid);
	stLogout.set_seatid(-1);
	stLogout.set_type(code);
	Ppacket ppack;
	stLogout.SerializeToString(&ppack.body);
	ppack.pack(SERVER_LOGOUT_SUCC_BC);
	client->send(ppack.data);
}

int Game::SaveDataToRedis(std::string &tid, std::string &data_str)
{
	int ret = phz.phz_rc->command("hset %s:%d %s %s", phz.conf["table"]["location"].asString().c_str(), phz.conf["game"]["port"].asInt(), tid.c_str(), data_str.c_str());
	if (ret < 0) {
		log.error("table[%s] send data to redis error\n", tid.c_str());
		return -1;
	}
	log.info("table[%s] send data to redis succ\n", tid.c_str());
	return 0;
}

int Game::RequestDataFromRedis()
{
	int ret = phz.phz_rc->command("hgetall %s:%d", phz.conf["table"]["location"].asString().c_str(), phz.conf["game"]["port"].asInt());
	if (ret < 0) {
		log.error("get all table data from redis error.\n");
		return -1;
	}
	if (phz.phz_rc->is_array_return_ok() < 0) {
		log.error("get all table data error.\n");
		return -1;
	}
	size_t i = 1;
	while (i < phz.phz_rc->reply->elements) {
		string tid = phz.phz_rc->reply->element[i-1]->str;
		string data_str = phz.phz_rc->reply->element[i]->str;
		log.info("request data from redis tid[%s] data size[%d] .\n", tid.c_str(), data_str.size());
		if(all_tables.find(tid) == all_tables.end()){
			Table *table = Table::CreateLocationTable(phz.conf["table"]["location"].asString());
			if(table == NULL){
				log.error("new %s error\n", phz.conf["table"]["location"].asString().c_str());
				return -1;
			}
			ret = table->ParseTableData(data_str);
			if(ret < 0){
				delete table;
				table = NULL;
				log.error("request data from redis Parse error tid[%s].\n", tid.c_str());
			}
			else{
				all_tables[tid] = table;
				log.info("redis recover table[%s]\n", tid.c_str());
			}
		}
		i += 2;
	}
	return 0;
}

int Game::DelDataFromRedis(std::string &tid)
{
	int ret = phz.phz_rc->command("hdel %s:%d %s ", phz.conf["table"]["location"].asString().c_str(), phz.conf["game"]["port"].asInt(), tid.c_str());
	if (ret < 0) {
		log.error("del redis table[%s] data error\n", tid.c_str());
		return -1;
	}
	log.info("del redis table[%s] data succ\n", tid.c_str());
	return 0;
}