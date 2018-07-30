#include "table.h"

extern PHZ phz;
extern Log log;

bool CheckPlayID(const Json::Value &val)
{
	return !val["dataMap"].isNull() && !val["dataMap"]["innerWayList"].isNull();
}

int GetPlaySubID(const Json::Value &val, std::string playID, PhzPlayID& SubID) 
{
	int cnt = 0;
	SubID.Clear();
	if(!val["dataMap"]["innerWayList"][playID].isNull() && val["dataMap"]["innerWayList"][playID].isString()){
		std::string conf_id = val["dataMap"]["innerWayList"][playID].asString();
		char *temp;
		char tempstr[200];
		strcpy(tempstr, conf_id.c_str());
		temp = strtok(tempstr, ",");
		while (temp != NULL) {
			conf_id = temp;
			SubID.Add(atoi(conf_id.c_str()));
			++cnt;
			temp = strtok(NULL, ",");
		}
	}
	return cnt;
}

int decompress_str(std::string &src, std::string &des)
{
	//base64编码
	static unsigned char ubuf[MaxVideoBufLen] = { 0 };
	int unoutlen = MaxVideoBufLen;
	int iRet = base64_decode((const unsigned char *)src.c_str(), src.size(), (unsigned char *)ubuf, &unoutlen);
	if (iRet != CRYPT_OK)
	{
		log.error("base64_decode video svr is error iRet:%d .\n", iRet);
		return -1;
	}
	//zlib解压
	static unsigned char uzbuf[MaxVideoBufLen] = { 0 };
	uLong unzlen = MaxVideoBufLen;
	iRet = uncompress((Bytef*)uzbuf, (uLongf *)&unzlen, (const Bytef*)ubuf, (uLong)unoutlen);
	if (iRet != Z_OK)
	{
		log.error("uncompress video error iRet:%d .\n", iRet);
		return -1;
	}
	std::string packStr((char *)uzbuf, unzlen);
	des.clear();
	des = packStr;
	return 0;
}

int compress_str(std::string &src, std::string &des)
{
	//zlib压缩
	static unsigned char zbuf[MaxVideoBufLen] = { 0 };
	uLong nzlen = MaxVideoBufLen;
	int iRet = compress((Bytef*)zbuf, (uLongf *)&nzlen, (const Bytef*)src.c_str(), (uLong)src.size());
	if (iRet != Z_OK)
	{
		log.error("send video to svr compress error iRet:%d .\n", iRet);
		return -1;
	}
	//base64编码
	static unsigned char buf[MaxVideoBufLen] = { 0 };
	int noutlen = MaxVideoBufLen;
	iRet = base64_encode((const unsigned char *)zbuf, nzlen, (unsigned char *)buf, &noutlen);
	if (iRet != CRYPT_OK)
	{
		log.error("send video to svr base64_encode is error iRet:%d .\n", iRet);
		return -1;
	}
	std::string packStr((char *)buf, noutlen);
	des.clear();
	des = packStr;
	return 0;
}

void Table::start_timer_cb(struct ev_loop *loop, struct ev_timer *w, int revents)
{
	Table *table = (Table*)w->data;
	ev_timer_stop(phz.loop, &table->start_timer);
	table->m_cbCurTimer = 0;
	table->OnOperateStart();
}

void Table::first_getcard_timer_cb(struct ev_loop *loop, struct ev_timer *w, int revents)
{
	Table *table = (Table*)w->data;
	ev_timer_stop(phz.loop, &table->first_getcard_timer);
	table->m_cbCurTimer = 0;
	table->OnUserGetCard(table->m_BankerSeat);
}

void Table::preready_timer_cb(struct ev_loop *loop, struct ev_timer *w, int revents)
{
	Table *table = (Table*)w->data;
	ev_timer_stop(phz.loop, &table->preready_timer);
	table->m_cbCurTimer = 0;
	table->handler_preready();
}

void Table::dispatch_show_timer_cb(struct ev_loop *loop, struct ev_timer *w, int revents)
{
	Table *table = (Table*)w->data;
	ev_timer_stop(phz.loop, &table->dispatch_show_timer);
	table->m_cbCurTimer = 0;
	if(!table->m_bCardOperated){
		//table->UpdateAbandonCard();
	}
	table->m_CurSeat = (table->m_CurSeat + 1) % (table->m_GamePlayer);
	//table->DispatchCardData();
}

void Table::dispatch_delay_timer_cb(struct ev_loop *loop, struct ev_timer *w, int revents)
{
	Table *table = (Table*)w->data;
	ev_timer_stop(phz.loop, &table->dispatch_delay_timer);
	table->m_cbCurTimer = 0;/*
	bool bEstimate = table->OnEstimateUserRespond();
	if (!bEstimate){
		ev_timer_again(phz.loop, &table->dispatch_show_timer);
		table->m_cbCurTimer = 3;
	}
	table->SerializeTableData();*/
}

void Table::Send_Video_Data_Timer_cb(struct ev_loop *loop, struct ev_timer *w, int revents)
{
	Table *table = (Table*)w->data;
	log.info("Send_Video_Data_Timer_cb roomID:%s \n", table->tid.c_str());
	ev_timer_stop(phz.loop, &table->send_videodata_timer);
	table->m_cbCurTimer = 0;
	table->send_video_to_svr();
}

void Table::dissolve_room_timer_cb(struct ev_loop *loop, struct ev_timer *w, int revents)
{
	Table *table = (Table*)w->data;
	ev_timer_stop(phz.loop, &table->dissolve_room_timer);
	table->m_cbCurTimer = 0;
	std::map<int, Player*>::iterator it = table->voters_players.begin();
	for (; it != table->voters_players.end(); it++) {
		if (it->second->dissovle_state == 2) {
			it->second->dissovle_state = 1;
			table->check_dissovle_result();
		}
	}
}

void Table::game_restart_timer_cb(struct ev_loop *loop, struct ev_timer *w, int revents)
{
	Table *table = (Table*)w->data;
	ev_timer_stop(phz.loop, &table->game_restart_timer);
	table->game_start_req();
}


void Table::UserOutCard_Timer_cb(struct ev_loop *loop, struct ev_timer *w, int revents)
{
	Table *table = (Table*)w->data;
	if (table)
	{		
		ev_timer_stop(phz.loop, &table->m_Outcard_Timer);
		table->m_bOverTimeState[table->m_CurSeat] = OVER_TIME_OUTCARD;
		table->UserOvertime_State(table->m_CurSeat);		
	}
}

void Table::UserChooseOperate_Timer_cb(struct ev_loop *loop, struct ev_timer *w, int revents)
{
	Table *table = (Table*)w->data;
	if (table)
	{
		log.info("uid:%d Chairid:0 choose timer out...\n", table->GetPlayerUid(static_cast<_uint8>(0)));
		ev_timer_stop(phz.loop, &table->m_Operate_Timer);
		if (table->m_bMultiOpUserNum == 0)
		{
			log.info("chairid 0 operate timer is on, but it have not operate \n");
			return;
		}
		for(int i = 0;i < table->m_GamePlayer;i++)
		{
			if(table->m_Seats[i].m_bUserOperate != TYPE_NULL)
			{
				table->m_bOverTimeState[i] = OVER_TIME_OPERATE;
			}
		}
		table->UserOvertime_State(0);
	}
}


void Table::UserChooseOperate_Timer_cb0(struct ev_loop *loop, struct ev_timer *w, int revents)
{
	Table *table = (Table*)w->data;
	if (table)
	{
		log.info("uid:%d Chairid:0 choose timer out...\n", table->GetPlayerUid(static_cast<_uint8>(0)));
		ev_timer_stop(phz.loop, &table->m_Operate_Timer0);
		if (table->m_Seats[0].m_bUserOperate == TYPE_NULL)
		{
			log.info("chairid 0 operate timer is on, but it have not operate \n");
			return;
		}
		table->m_bOverTimeState[0] = OVER_TIME_OPERATE;
		table->UserOvertime_State(0);		
	}
}

void Table::UserChooseOperate_Timer_cb1(struct ev_loop *loop, struct ev_timer *w, int revents)
{
	Table *table = (Table*)w->data;
	if (table)
	{
		log.info("uid:%d Chairid:1 choose timer out...\n", table->GetPlayerUid(static_cast<_uint8>(1)));
		ev_timer_stop(phz.loop, &table->m_Operate_Timer1);
		if (table->m_Seats[1].m_bUserOperate == TYPE_NULL)
		{
			log.info("chairid 1 operate timer is on, but it have not operate \n");
			return;
		}
		table->m_bOverTimeState[1] = OVER_TIME_OPERATE;
		table->UserOvertime_State(1);
	}
}

void Table::UserChooseOperate_Timer_cb2(struct ev_loop *loop, struct ev_timer *w, int revents)
{
	Table *table = (Table*)w->data;
	if (table)
	{
		log.info("uid:%d Chairid:2 choose timer out...\n", table->GetPlayerUid(static_cast<_uint8>(2)));
		ev_timer_stop(phz.loop, &table->m_Operate_Timer2);
		if (table->m_Seats[2].m_bUserOperate == TYPE_NULL)
		{
			log.info("chairid 2 operate timer is on, but it have not operate \n");
			return;
		}
		table->m_bOverTimeState[2] = OVER_TIME_OPERATE;
		table->UserOvertime_State(2);
	}
}

void Table::UserChooseOperate_Timer_cb3(struct ev_loop *loop, struct ev_timer *w, int revents)
{
	Table *table = (Table*)w->data;
	if (table)
	{
		log.info("uid:%d Chairid:3 choose timer out...\n", table->GetPlayerUid(static_cast<_uint8>(3)));
		ev_timer_stop(phz.loop, &table->m_Operate_Timer3);
		if (table->m_Seats[3].m_bUserOperate == TYPE_NULL)
		{
			log.info("chairid 3 operate timer is on, but it have not operate \n");
			return;
		}
		table->m_bOverTimeState[3] = OVER_TIME_OPERATE;
		table->UserOvertime_State(3);
	}	
}

void Table::Trusteeshit_Out_Timer_cb(struct ev_loop *loop, struct ev_timer *w, int revents)
{
    Table *table = (Table*)w->data;
	if (table)
	{
	    ev_timer_stop(phz.loop, &table->m_Trusteeshit_Out_Delay_Timer);
	    
        table->OnAutoUserOutCard(table->m_CurSeat, table->m_CurCard);
    }
}

void Table::Trusteeshit_Ope_Timer_cb0(struct ev_loop *loop, struct ev_timer *w, int revents)
{
	Table *table = (Table*)w->data;
	if (table)
	{
        ev_timer_stop(phz.loop, &table->m_Trusteeshit_Ope_Delay_Timer0);
        table->UserAutoOperate(0, table->m_CurCard);
	}
}

void Table::Trusteeshit_Ope_Timer_cb1(struct ev_loop *loop, struct ev_timer *w, int revents)
{
	Table *table = (Table*)w->data;
	if (table)
	{
        ev_timer_stop(phz.loop, &table->m_Trusteeshit_Ope_Delay_Timer1);
        table->UserAutoOperate(1, table->m_CurCard);
	}
}

void Table::Trusteeshit_Ope_Timer_cb2(struct ev_loop *loop, struct ev_timer *w, int revents)
{
	Table *table = (Table*)w->data;
	if (table)
	{
        ev_timer_stop(phz.loop, &table->m_Trusteeshit_Ope_Delay_Timer2);
        table->UserAutoOperate(2, table->m_CurCard);
	}
}

void Table::Trusteeshit_Ope_Timer_cb3(struct ev_loop *loop, struct ev_timer *w, int revents)
{
	Table *table = (Table*)w->data;
	if (table)
	{
        ev_timer_stop(phz.loop, &table->m_Trusteeshit_Ope_Delay_Timer3);
        table->UserAutoOperate(3, table->m_CurCard);
	}
}

int Table::broadcast(Player *p, const std::string &packet)
{
	Player *player;
	std::map<int, Player*>::iterator it;
	for (it = players.begin(); it != players.end(); it++) {
		player = it->second;
		if (player == p || player->client == NULL) {
			continue;
		}
		player->client->send(packet);
	}
	return 0;
}

int Table::unicast(Player *p, const std::string &packet)
{
	if (p->client) {
		return p->client->send(packet);
	}
	return -1;
}

int Table::sit_down(Player *player)
{
    //固定按进入的先后顺序坐
    if (true)
    {
        for (int i = 0; i < m_GamePlayer; i++)
		{
			if (!m_Seats[i].m_bOccupied)
			{
				m_Seats[i].m_bOccupied = true;
		        m_Seats[i].m_Player = player;

				log.info("sit_down uid:%d seatid[%d] tid:%s.\n",player->uid, player->seatid, tid.c_str());
				return i;
			}
		}
		return -1;
    }
    //随机坐
	std::vector<int> tmp;
	for (int i = 0; i < m_GamePlayer; i++) {
		if (!m_Seats[i].m_bOccupied) {
			tmp.push_back(i);
		}
	}
	int len = tmp.size();
	if (len > 0){
		int index = RANDOM(0, len - 1, player->uid);
		int i = tmp[index];
		m_Seats[i].m_bOccupied = true;
		m_Seats[i].m_Player = player;
		return i;
	}
	log.info("sit_down uid:%d seatid[%d] tid:%s.\n",player->uid, player->seatid, tid.c_str());
	return -1;
}

void Table::stand_up(Player *player)
{
	m_Seats[player->seatid].Clear();
}

int Table::del_player(Player *player)
{
	if (player == NULL){
		log.error("del player error player Null tid:%s .\n", tid.c_str());
		return -1;
	}
	if (players.find(player->uid) == players.end()){
		log.error("del player error player not find uid:%d tid:%s.\n", player->uid, tid.c_str());
		return -1;
	}
	player->stop_offline_timer();
	players.erase(player->uid);
	log.info("del player uid:%d tid[%d].\n", player->uid, tid.c_str());
	if (players.empty()){
		return 1;
	}
	return 0;
}

int Table::handler_login(Player *player)
{
	if (players.find(player->uid) != players.end()){
		log.error("handler login error player exsit uid:%d tid:%s .\n", player->uid, tid.c_str());
		return -1;
	}
	players[player->uid] = player;
	player->tid = tid;
	handler_login_succ_uc(player);	//单播玩家登陆消息
	handler_apply_uptable(player);	//登录后自动上桌	
	handler_table_info(player);		//单播桌子信息
	log.info("handler login succ uid:%d tid:%s size[%d] .\n", player->uid, tid.c_str(), players.size());
	return 0;
}

int Table::handler_login_succ_uc(Player *player)
{
	proto::game::AckLoginSuccessUc stLoginuc;
	stLoginuc.set_seatid(player->seatid);
	stLoginuc.set_uid(player->uid);
	stLoginuc.set_money(player->money);
	if (player->name != ""){
		stLoginuc.set_name(player->name);
	}
	if (player->avatar != ""){
		stLoginuc.set_avatar(player->avatar);
	}
	stLoginuc.set_sex(player->sex);
	stLoginuc.set_exp(player->exp);
	stLoginuc.set_rmb(player->rmb);
	Ppacket ppack;
	stLoginuc.SerializeToString(&ppack.body);
	ppack.pack(SERVER_LOGIN_SUCC_UC);
	unicast(player, ppack.data);
	log.debug("%s AckLoginSuccessUc:%s \n", __FUNCTION__, stLoginuc.DebugString().c_str());
	log.info("handler login succ uc uid:%d tid:%s .\n", player->uid, tid.c_str());
	return 0;
}

int Table::handler_login_succ_bc(Player *player)
{
	proto::game::AckLoginSuccessBc stLoginbc;
	proto::game::tagPlayers* pstPlayer = stLoginbc.mutable_player();
	pstPlayer->set_seatid(player->seatid);
	pstPlayer->set_uid(player->uid);
	pstPlayer->set_ready(m_Seats[player->seatid].m_bReady);
	if (player->name.size() != 0){
	    pstPlayer->set_name(player->name);
	}
	if (player->avatar.size() != 0){
	    pstPlayer->set_avatar(player->avatar);
	}
	pstPlayer->set_sex(player->sex);
	pstPlayer->set_money(player->money);
	
//	stLoginbc.set_seatid(player->seatid);
//	stLoginbc.set_uid(player->uid);
//	stLoginbc.set_money(player->money);
//	if (player->name != ""){
//		stLoginbc.set_name(player->name.c_str());
//	}
//	if (player->avatar.size() != 0){
//		stLoginbc.set_avatar(player->avatar.c_str());
//	}
//	stLoginbc.set_sex(player->sex);

	Ppacket ppack;
	stLoginbc.SerializeToString(&ppack.body);
	ppack.pack(SERVER_LOGIN_SUCC_BC);
	broadcast(player, ppack.data);
	log.info("handler login succ bc uid:%d tid:%s .\n", player->uid, tid.c_str());
	return 0;
}

int Table::handler_preready()
{
	//游戏初始化
	Reset();	
	for (std::map<int, Player*>::iterator iter = players.begin(); iter != players.end(); iter++){
		iter->second->reset();
	}
	log.info("handler preready tid:%s .\n", tid.c_str());
	SerializeTableData();
	return 0;
}

int Table::handler_logout(Player *player)
{
	if (m_State != GAME_FREE || m_RoomStatus != 0 ){
		for (int i = 0; i < m_GamePlayer; ++i){
			if (m_Seats[i].m_Uid == player->uid){
				log.error("handler logout error player in seat playing. uid:%d m_State[%d] tid:%s.\n", player->uid, m_State, tid.c_str());
				return -1;
			}
		}
	}
	handler_downtable(player);
	//广播退出房间协议
	proto::game::AckLogoutBc stLogout;
	stLogout.set_seatid(player->seatid);
	stLogout.set_uid(player->uid);
	stLogout.set_type(player->logout_type);
	Ppacket ppack;
	stLogout.SerializeToString(&ppack.body);
	ppack.pack(SERVER_LOGOUT_SUCC_BC);
	unicast(player, ppack.data);
	log.info("handler logout uid:%d seatid[%d] money[%ld] tid:%s.\n", player->uid, player->seatid, player->money, tid.c_str());
	return 0;
}

int Table::handler_chat(Player *player)
{
	proto::game::AckChat stChat;
	stChat.ParseFromString(player->client->ppacket.body);
	proto::game::AckChat stChatRes;
	stChatRes.set_seatid(player->seatid);
	stChatRes.set_text(stChat.text());
	Ppacket ppack;
	stChatRes.SerializeToString(&ppack.body);
	ppack.pack(SERVER_CHAT_BC);
	broadcast(NULL, ppack.data);
	log.debug("handler chat stChat[%s] stChatRes[%s] .\n", stChat.DebugString().c_str(), stChatRes.DebugString().c_str());
	return 0;
}

int Table::handler_face(Player *player)
{
	proto::game::AckFace stFace;
	stFace.ParseFromString(player->client->ppacket.body);
	proto::game::AckFace stFaceRes;
	stFaceRes.set_seatid(player->seatid);
	stFaceRes.set_faceid(stFace.faceid());
	Ppacket ppack;
	stFaceRes.SerializeToString(&ppack.body);
	ppack.pack(SERVER_FACE_BC);
	broadcast(NULL, ppack.data);
	return 0;
}

int Table::handler_interaction_emotion(Player *player)
{
	proto::game::ReqEmotion stEmotion;
	stEmotion.ParseFromString(player->client->ppacket.body);
	int target_seatid = stEmotion.target_seatid();
	int itype = stEmotion.type();
	proto::game::AckEmotionBC stEmotionBC;
	stEmotionBC.Clear();
	stEmotionBC.set_seatid(player->seatid);
	stEmotionBC.set_money(player->money);
	stEmotionBC.set_target_seatid(target_seatid);
	stEmotionBC.set_type(itype);
	Ppacket ppack;
	stEmotionBC.SerializeToString(&ppack.body);
	ppack.pack(SERVER_EMOTION_BC);
	broadcast(NULL, ppack.data);
	log.info("handler interaction emotion uid:%d type[%d] tid:%s.\n ", player->uid, itype, tid.c_str());
	return 0;
}

int Table::handler_dissolve_room(Player *player)
{
	if (dissovle_state == 1 || player->seatid < 0 || player->seatid >= m_GamePlayer || is_dissolved == 1)
	{
		log.error("apply dissovling room error uid:%d seatid[%d] dissovle_state[%d] is_dissolved[%d] tid:%s .\n", player->uid, player->seatid, dissovle_state, is_dissolved, tid.c_str());
		return -1;
	}
	if (m_RoomStatus != 1) //房间不是进行中，就直接解散 
	{
		proto::game::AckDissolveResult stResult;
		stResult.set_result(1);
		Ppacket ppack;
		stResult.SerializeToString(&ppack.body);
		ppack.pack(SERVER_DISSOLVE_ROOM_RESULT_BC);
		broadcast(NULL, ppack.data);
		//发送给平台
		Jpacket datapac;
		datapac.val["cmd"] = SERVER_DZ_DISSOLVE_ROOM_REQ;
		datapac.val["roomid"] = tid;
		datapac.val["innings"] = m_strInnings;
		datapac.end();
		phz.game->send_to_datasvr(datapac.tostring());
		dissovle_state = 0;
		for (_uint8 i = 0; i < m_GamePlayer; ++i){
			if (m_Seats[i].m_Player != NULL) m_Seats[i].m_Player->dissovle_state = 2;
		}
		log.info("handler dissolve room player[%d] apply dissolve room[%s] \n", player->uid, tid.c_str());
		return 0;
	}

	if (player->dissolve_cnt >= 2){
		proto::game::AckDissolveErr stErr;
		stErr.set_code(101);
		Ppacket ppack;
		stErr.SerializeToString(&ppack.body);
		ppack.pack(SERVER_APPLY_DISSOLVE_ROOM_ERR_UC);
		unicast(player, ppack.data);
		return 0;
	}
	player->dissolve_cnt++;
	dissovle_state = 1;
	dissolve_applyer = player->uid;
	voters_players.clear();

	log.info("handler dissolve room player[%d] apply dissolve room[%s] \n", player->uid, tid.c_str());

	proto::game::AckDissolveSucc stSucc;
	stSucc.set_uid(player->uid);
	stSucc.set_remain_time(dissolve_room_tstamp);
	for (_uint8 j = 0; j < m_GamePlayer; ++j){
		if (m_Seats[j].m_Player == NULL) continue;
		if (player->uid == m_Seats[j].m_Uid)
			continue;
		stSucc.add_voters_uid(m_Seats[j].m_Uid);
		voters_players[m_Seats[j].m_Uid] = m_Seats[j].m_Player;
	}
	Ppacket ppack;
	stSucc.SerializeToString(&ppack.body);
	ppack.pack(SERVER_APPLY_DISSOLVE_ROOM_SUCC_BC);
	broadcast(NULL, ppack.data);
	log.debug("%s AckDissolveSucc:%s .\n", __FUNCTION__, stSucc.DebugString().c_str());
	ev_timer_again(phz.loop, &dissolve_room_timer);
	m_cbCurTimer = 5;
	check_dissovle_result();
	SerializeTableData();
	return 0;
}

int Table::handler_dissolve_action(Player *player)
{
	if (dissovle_state != 1) {
		log.error("dissovle_state[%d] is error\n", dissovle_state);
		return -1;
	}
	if (player->dissovle_state != 2) {
		log.error("player[%d] vote error\n", player->uid);
		return -1;
	}
	proto::game::ReqDissolveAct stAct;
	stAct.ParseFromString(player->client->ppacket.body);
	int uid = stAct.uid();
	int act = stAct.action();
	if (voters_players.find(uid) == voters_players.end()){
		log.error("voters_uid not cantain uid:%d\n", uid);
		return -1;
	}
	log.info("handler dissovle action player[%d] act[%d] room[%s] \n", player->uid, act, tid.c_str());
	player->dissovle_state = act;
	proto::game::AckDissolveActSucc stActSucc;
	stActSucc.set_uid(uid);
	stActSucc.set_action(act);
	Ppacket ppack;
	stActSucc.SerializeToString(&ppack.body);
	ppack.pack(SERVER_DISSOLVE_ACTION_SUCC_BC);
	broadcast(NULL, ppack.data);
	check_dissovle_result();
	SerializeTableData();
	return 0;
}

void Table::check_dissovle_result()
{
	unsigned int _cnt_yes = 0;
	unsigned int _cnt_no = 0;
	std::map<int, Player*>::iterator it = voters_players.begin();
	for (; it != voters_players.end(); it++){
		if (it->second->dissovle_state == 0) {
			_cnt_no++;
		}
		else if (it->second->dissovle_state == 1) {
			_cnt_yes++;
		}
	}
	if (_cnt_yes + _cnt_no == voters_players.size() || _cnt_no > 0){
		ev_timer_stop(phz.loop, &dissolve_room_timer);
		m_cbCurTimer = 0;
		proto::game::AckDissolveResult stResult;
		stResult.set_result(_cnt_yes == voters_players.size() ? 1 : 0);
		Ppacket ppack;
		stResult.SerializeToString(&ppack.body);
		ppack.pack(SERVER_DISSOLVE_ROOM_RESULT_BC);
		broadcast(NULL, ppack.data);
		if (_cnt_yes == voters_players.size()){
			//发送录像
			if (m_State == GAME_PLAYING){
				tagGain stGain;
				for( _uint8 i = 0; i < GAME_PLAYER; ++i ){
					stGain.Gain[i] = 0;
				}
				m_vecGains.push_back(stGain);
				ev_timer_again(phz.loop, &send_videodata_timer);
				m_cbCurTimer = 6;
			}
			
			Jpacket pac;
			pac.val["cmd"] = SERVER_DZ_DISSOLVE_ROOM_REQ;
			pac.val["roomid"] = tid;
			pac.val["innings"] = m_strInnings;
			pac.end();
			phz.game->send_to_datasvr(pac.tostring());
		}
		dissovle_state = 0;
		for (_uint8 i = 0; i < m_GamePlayer; ++i){
			if (m_Seats[i].m_Player != NULL) m_Seats[i].m_Player->dissovle_state = 2;
		}
	}
}

void Table::uptable_error_uc(Player *player, int code)
{
	if (NULL == player)
	{
		log.info("uptable error uc error player is null tid:%s .\n", tid.c_str());
		return;
	}
	proto::game::AckUpTableErrorUc stUpTable;
	stUpTable.set_uid(player->uid);
	stUpTable.set_money(player->money);
	stUpTable.set_code(code);
	Ppacket ppack;
	stUpTable.SerializeToString(&ppack.body);
	ppack.pack(SERVER_UPTABLE_ERR_UC);
	unicast(player, ppack.data);
}

int Table::handler_apply_uptable(Player *player)
{
	handler_uptable(player);
	return 0;
}

int Table::handler_uptable(Player *player)
{
	for (int i = 0; i < m_GamePlayer; ++i){
		if (m_Seats[i].m_Uid == player->uid){
			log.error("handler uptable error uid:%d already in seat tid:%s.\n", player->uid, tid.c_str());
			uptable_error_uc(player, 102);
			return -1;
		}
	}
	if (player->money < stand_money){
		log.error("handler uptable error uid:%d money[%ld] too less stand_money[%d] tid:%s.\n", player->uid, player->money, stand_money, tid.c_str());
		uptable_error_uc(player, 104);
		return -1;
	}
	int SeatNum = 0;
	for (int i = 0; i < m_GamePlayer; i++){
		if (m_Seats[i].m_Player != NULL)
			SeatNum++;
	}
	if (SeatNum >= m_GamePlayer){
		log.error("handler uptable error uid:%d SeatNum[%d] tid:%s .\n", player->uid, SeatNum, tid.c_str());
		uptable_error_uc(player, 105);
		return -1;
	}
	player->stop_offline_timer();
	player->seatid = sit_down(player);
	if (player->seatid < 0){
		log.error("handler uptable error uid:%d seatid[%d] tid:%s .\n", player->uid, player->seatid, tid.c_str());
		uptable_error_uc(player, 105);
		return -1;
	}
	Seat &seat = m_Seats[player->seatid];
	seat.m_Uid = player->uid;
	proto::game::AckUpTableSuccess stUpTable;
//	stUpTable.set_uid(player->uid);
//	stUpTable.set_seatid(player->seatid);
//	stUpTable.set_ready(m_Seats[player->seatid].m_bReady);
//	if (player->name.size() != 0){
//		stUpTable.set_name(player->name.c_str());
//	}
//	stUpTable.set_sex(player->sex);
//	if (player->avatar.size() != 0){
//		stUpTable.set_avatar(player->avatar.c_str());
//	}
//	stUpTable.set_money(player->money);
	
	proto::game::tagPlayers* pstPlayer = stUpTable.mutable_player();
	pstPlayer->set_seatid(player->seatid);
	pstPlayer->set_uid(player->uid);
	pstPlayer->set_ready(m_Seats[player->seatid].m_bReady);
	if (player->name.size() != 0){
	    pstPlayer->set_name(player->name);
	}
	if (player->avatar.size() != 0){
	    pstPlayer->set_avatar(player->avatar);
	}
	pstPlayer->set_sex(player->sex);
	pstPlayer->set_money(player->money);
	
	Ppacket ppack;
	stUpTable.SerializeToString(&ppack.body);
	ppack.pack(SERVER_UPTABLE_SUCC_BC);
	broadcast(NULL, ppack.data);
	log.debug("%s AckUpTableSuccess:%s .\n", __FUNCTION__, stUpTable.DebugString().c_str());
	log.info("handler uptable succ uid:%d seaitd[%d] money[%ld] tid:%s .\n", player->uid, player->seatid, player->money, tid.c_str());
	return 0;
}

int Table::handler_downtable(Player *player)
{
	bool bFind = false;
	for (int i = 0; i < m_GamePlayer; ++i){
		if (m_Seats[i].m_Uid == player->uid){
			bFind = true;
			break;
		}
	}
	if (!bFind){
		log.error("handler downtable error uid:%d not in seat tid:%s.\n", player->uid, tid.c_str());
		return -1;
	}
	if (m_State != GAME_FREE || m_RoomStatus != 0 ){
		return -1;
	}
	if (player->seatid == m_BankerSeat)
		m_BankerSeat = INVALID_CHAIR;
	proto::game::AckDownTableSuccess stDownTableSuccess;
	stDownTableSuccess.set_uid(player->uid);
	stDownTableSuccess.set_seatid(player->seatid);
	stDownTableSuccess.set_money(player->money);
	Ppacket ppack;
	stDownTableSuccess.SerializeToString(&ppack.body);
	ppack.pack(SERVER_DOWNTABLE_SUCC_BC);
	broadcast(NULL, ppack.data);
	stand_up(player);
	player->clear();
	log.info("handler downtable succ uid:%d money[%ld] tid:%s.\n", player->uid, player->money, tid.c_str());
	return 0;
}

void Table::update_user_info_to_datasvr(int UserScore[GAME_PLAYER])
{
	Jpacket packet_data;
	packet_data.val["cmd"] = SERVER_DZ_UPDATE_USER_INFO;
	for (int i = 0; i < m_GamePlayer; i++)
	{
		packet_data.val["players"][i]["uid"] = m_Seats[i].m_Uid;
		packet_data.val["players"][i]["alter_money"] = UserScore[i];
		packet_data.val["players"][i]["al_board"] = 1;
		if (UserScore[i] > 0) {
			packet_data.val["players"][i]["al_win"] = 1;
			packet_data.val["players"][i]["alter_exp"] = 0;
		}
		else {
			packet_data.val["players"][i]["al_win"] = 0;
			packet_data.val["players"][i]["alter_exp"] = 0;
		}
	}
	packet_data.val["roomid"] = tid;
	packet_data.val["innings"] = m_strInnings;
	packet_data.end();
	phz.game->send_to_datasvr(packet_data.tostring());
}

void Table::UpdateUserInfoToUser(Player *player)
{
	if (player == NULL)
	{
		log.error("update user info to user error, player is NULL \n");
		return;
	}
	log.info("%s uid:%d chairid:%d money:%d\n", __FUNCTION__, player->uid, player->seatid, player->money);
	proto::game::AckUpdateUserInfo stAck;
	stAck.set_chairid(player->seatid);
	stAck.set_uid(player->uid);
	stAck.set_money(player->money);
	stAck.set_rmb(player->rmb);

    Ppacket ppack;
	stAck.SerializeToString(&ppack.body);
	ppack.pack(SERVER_UPDATE_USERINFO);
	broadcast(NULL, ppack.data);

    save_video_data(SERVER_UPDATE_USERINFO, stAck.SerializeAsString());
}

int Table::game_start_req()
{
	Jpacket req_packet;
	req_packet.val["cmd"] = SERVER_DZ_GAME_START_REQ;
	req_packet.val["roomid"] = tid;
	req_packet.val["innings"] = m_strInnings;
	req_packet.end();
	phz.game->send_to_datasvr(req_packet.tostring());
	log.info("game start req roomid[%s] .\n", tid.c_str());
	return 0;
}

int Table::game_start_res(const Jpacket &packet)
{
	int can_start = 0;
	log.info("game start res succ roomid[%s] can_start[%d] .\n", tid.c_str(), can_start);
	if (!packet.val["can_start"].isNull() && packet.val["can_start"].isNumeric()){
		can_start = packet.val["can_start"].asInt();
		m_CurTurn = packet.val["currentCount"].asInt();
	}
	if (0 == can_start){
		int code = 0;
		if (!packet.val["code"].isNull() && packet.val["code"].isNumeric()){
			code = packet.val["code"].asInt();
		}
		proto::game::AckGameStartFail stGameStartFail;
		stGameStartFail.set_roomid(tid);
		stGameStartFail.set_can_start(0);
		stGameStartFail.set_code(code);
		int ntime = (int)time(NULL);
		stGameStartFail.set_ts(ntime);
		Ppacket ppack;
		stGameStartFail.SerializeToString(&ppack.body);
		ppack.pack(SERVER_DZ_GAME_START_RES);
		broadcast(NULL, ppack.data);
		advance_game_end();
		log.error("game start res fail roomid[%s] can_start[%d] code[%d] .\n", tid.c_str(), can_start, code);
		return -1;
	}
	ev_timer_stop(phz.loop, &game_restart_timer);
	if (m_bAlreadyResp == true){
		log.error("game start res tid:%s m_bAlreadyResp[%d] .\n", tid.c_str(), m_bAlreadyResp);
		return -1;
	}
	m_bAlreadyResp = true;
	int cReadyNum = 0;
	for (int i = 0; i < m_GamePlayer; ++i){
		if (m_Seats[i].m_bReady)
			cReadyNum++;
	}
	if (cReadyNum == m_GamePlayer){
		//开始游戏
		proto::game::AckPrereadyBC stPre;
		stPre.set_roomid(tid);
		Ppacket ppack;
		stPre.SerializeToString(&ppack.body);
		ppack.pack(SERVER_GAME_PREREADY_BC);
		broadcast(NULL, ppack.data);
		m_RoomStatus = 1;
		//保存录像
	    save_video_data(SERVER_TABLE_INFO_UC, m_pTableInfo.SerializeAsString());
		return 0;
	}
	//不能开赛，发0结算给平台
	_tint32 UserScore[GAME_PLAYER] = { 0 };
	update_user_info_to_datasvr(UserScore);
	return -1;	
}

void Table::handler_ready(Player* player)
{
	if (player->seatid < 0 || player->seatid >= m_GamePlayer){
		log.error("handler ready error seatid[%d] wrong .\n", player->seatid);
		return;
	}
	if (m_Seats[player->seatid].m_bReady){
		log.error("handler ready error seatid[%d] already ready[ .\n", player->seatid);
		return;
	}
	m_Seats[player->seatid].m_bReady = true;
	proto::game::AckUserReady stReady;
	stReady.set_seatid(player->seatid);
	stReady.set_uid(player->uid);
	Ppacket ppack;
	stReady.SerializeToString(&ppack.body);
	ppack.pack(SERVER_READY_SUCC_BC);
	broadcast(NULL, ppack.data);
	log.info("handler ready succ uid:%d seatid[%d] tid:%s .\n", player->uid, player->seatid, tid.c_str());
	//请求开始游戏
	_uint8 cReadyNum = 0;
	for (int i = 0; i < m_GamePlayer; i++){
		if (m_Seats[i].m_bReady)
			cReadyNum++;
	}
	if (m_State == GAME_FREE && cReadyNum == m_GamePlayer){
		m_bAlreadyResp = false;
		std::ostringstream oss;
		oss << tid << "_" << m_CurTurn;
		m_strInnings = oss.str();
		game_start_req();
		ev_timer_again(phz.loop, &game_restart_timer);
	}
}

void Table::handler_update_gps(Player* player)
{
    proto::game::UpdateGPSReq stUpdateGPS;
	stUpdateGPS.ParseFromString(player->client->ppacket.body);

	player->latitude = stUpdateGPS.latitude();
	player->longitude = stUpdateGPS.longitude();
	player->city = stUpdateGPS.city();
	player->gps_state = GPS_STATE_UPDATED;

	proto::game::UpdateGPSAck stUpdateGPSAck;
	Proto_Update_Gps(stUpdateGPSAck);

    Ppacket ppack;
	stUpdateGPSAck.SerializeToString(&ppack.body);
	ppack.pack(SERVER_UPDATE_GPS_BC);
	broadcast(NULL, ppack.data);
}

void Table::handler_get_gps(Player* player)
{
    proto::game::UpdateGPSAck stUpdateGPSAck;
	Proto_Update_Gps(stUpdateGPSAck);

    Ppacket ppack;
	stUpdateGPSAck.SerializeToString(&ppack.body);
	ppack.pack(SERVER_UPDATE_GPS_BC);
	unicast(player, ppack.data);
}

bool Table::CheckReqValid(Player *player)
{
	if (is_dissolved){
		log.error("Room Dissovled is_dissolved[%d] tid:%s .\n", is_dissolved, tid.c_str());
		OperateError(player->seatid, proto::game::E_OPER_DISSOLVE_STATE_ERROR);
		return false;
	}
	if (player->seatid < 0 || player->seatid >= m_GamePlayer){
		log.error("on user operate card error wChairID[%d] wrong.\n", player->seatid);
		OperateError(player->seatid, proto::game::E_OPER_SEAT_ERROR);
		return false;
	}
	return true;
}

void Table::save_video_data(_tint32 cmd, const std::string &str)
{
	ostringstream oss;
	oss << cmd << ":S:" << str << ":E:";
	m_strVideo += oss.str();
}

void Table::send_video_to_svr()
{
	//base64编码
	static unsigned char buf[MaxVideoBufLen] = { 0 };
	int noutlen = MaxVideoBufLen;
	int iRet = base64_encode((const unsigned char *)m_strVideo.c_str(), m_strVideo.size(), (unsigned char *)buf, &noutlen);
	if (iRet != CRYPT_OK){
		log.error("send video to svr base64_encode is error iRet:%d .\n", iRet);
		return;
	}
	Jpacket packet_data;
	packet_data.val["cmd"] = SERVER_VIDEO_DATA_SAVE;
	packet_data.val["roomid"] = tid;
	std::string packStr((char *)buf, noutlen);
	packet_data.val["content"] = packStr;
	packet_data.val["innings"] = m_strInnings;
	packet_data.end();
	phz.game->send_to_videosvr(packet_data.tostring());
	log.info("send video to svr m_strVideo.size[%d] noutlen[%d] tid:%s.\n", m_strVideo.size(), noutlen, tid.c_str());
}

//开始飘
void Table::StartPiao()
{
	m_State = GAME_PIAO;
	proto::game::AckStartPiao stStartPiao;
	stStartPiao.set_state(m_State);
	stStartPiao.set_banker(m_BankerSeat);
	Ppacket ppack;
	stStartPiao.SerializeToString(&ppack.body);
	ppack.pack(SERVER_PHZ_START_PIAO);
	broadcast(NULL, ppack.data);
	save_video_data(SERVER_PHZ_START_PIAO, stStartPiao.SerializeAsString());
	SerializeTableData();
}

void Table::PiaoError(Player *player, int code)
{
	proto::game::AckPiaoErr stPiaoErr;
	stPiaoErr.set_uid(player->uid);
	stPiaoErr.set_seatid(player->seatid);
	stPiaoErr.set_code(code);
	Ppacket ppack;
	stPiaoErr.SerializeToString(&ppack.body);
	ppack.pack(SERVER_PHZ_PIAO_ERR);
	unicast(player, ppack.data);
}

int Table::OnUserPiaoOperate(Player* player)
{
	if (m_State != GAME_PIAO){
		log.error("on user piao operate error tid:%s uid:%d m_State[%d]  .\n", tid.c_str(), player->uid, m_State);
		PiaoError(player, 101);
		OperateError(player->seatid, proto::game::E_PIAO_STATE_ERROR);
		return -1;
	}
	_tint32 wChairID = player->seatid;
	if (wChairID < 0 || wChairID >= m_GamePlayer){
		log.error("on user piao operate error tid:%s uid:%d seatid[%d] wrong .\n", tid.c_str(), player->uid, wChairID);
		PiaoError(player, 102);
		OperateError(player->seatid, proto::game::E_PIAO_SEAT_ERROR);
		return -1;
	}
	if (m_Seats[wChairID].m_UserPiaoScore >= 0){
		log.error("on user piao operate error tid:%s uid:%d m_UserPiaoScore[%d] already piao .\n", tid.c_str(), player->uid, m_Seats[wChairID].m_UserPiaoScore);
		PiaoError(player, 103);
		OperateError(player->seatid, proto::game::E_PIAO_REPEAT_ERROR);
		return -1;
	}
	proto::game::ReqPiao stReqPiao;
	stReqPiao.ParseFromString(player->client->ppacket.body);
	_uint8 my_count = stReqPiao.cout();
	if (my_count < 0 || (my_count > 0 && m_PiaoScore.find(my_count) == m_PiaoScore.end())){
		log.error("on user piao operate tid:%s uid:%d my_count[%d] wrong or not find .\n", tid.c_str(), player->uid, my_count);
		PiaoError(player, 104);
		OperateError(player->seatid, proto::game::E_PIAO_NUM_ERROR);
		return -1;
	}
	m_Seats[wChairID].m_UserPiaoScore = my_count;
	proto::game::AckPiaoSucc stPiaoSucc;
	stPiaoSucc.set_uid(player->uid);
	stPiaoSucc.set_seatid(player->seatid);
	stPiaoSucc.set_name(player->name);
	stPiaoSucc.set_avatar(player->avatar);
	stPiaoSucc.set_sex(player->sex);
	stPiaoSucc.set_piao(m_Seats[wChairID].m_UserPiaoScore);
	Ppacket ppack;
	stPiaoSucc.SerializeToString(&ppack.body);
	ppack.pack(SERVER_PHZ_PIAO_SUCC);
	broadcast(NULL, ppack.data);
	log.debug("%s tid:%s uid:%d AckPiaoSucc:%s .\n", __FUNCTION__, tid.c_str(), player->uid, stPiaoSucc.DebugString().c_str());
	log.info("%s tid:%s uid:%d m_State:%d seatid:%d my_count:%d.\n ", 
	            __FUNCTION__, tid.c_str(), player->uid, m_State, player->seatid, my_count);    

	std::string str = stPiaoSucc.SerializeAsString();
	save_video_data(SERVER_PHZ_PIAO_SUCC, str);
	
	_uint8 cbPiaoCount = 0;
	for (_uint8 i = 0; i < m_GamePlayer; ++i){
		if (m_Seats[i].m_UserPiaoScore >= 0) cbPiaoCount++;
	}
	if (cbPiaoCount == m_GamePlayer){
		GameStart();
	}
	SerializeTableData();
	return 0;
}


int Table::handler_table_info(Player *player)
{
    if (player == NULL)
	{
		log.error("handler_table_info player is NULL \n");
		return - 1;
	}
	
	m_pTableInfo.Clear();
	m_pTableInfo.set_seatid(player->seatid);
	m_pTableInfo.set_state(m_State);
	m_pTableInfo.set_outcard_time_stamp(m_Outcard_Timer_stamp);
	m_pTableInfo.set_operate_timer_stamp(m_Operate_Timer_stamp);
//	m_pTableInfo.set_operate_timer_stamp(15);    
	m_pTableInfo.set_left_card_count(m_RepertoryCard.Count());
	m_pTableInfo.set_banker_user(m_BankerSeat);
	m_pTableInfo.set_current_user(m_CurSeat);
	m_pTableInfo.set_current_card(m_CurCard);
	m_pTableInfo.set_current_outcard_user(m_CurOutSeat);
	m_pTableInfo.set_current_outcard(m_CurOutCard);
	m_pTableInfo.set_is_dispatch(m_bDispatch);
	m_pTableInfo.set_card_operated(m_bCardOperated);
	m_pTableInfo.set_is_out_card(m_bOutCard);
	m_pTableInfo.set_is_first_card(m_bFirstCard);
	m_pTableInfo.set_take_in(m_TakeIn);
	m_pTableInfo.set_play_id(m_PlayId);
	m_pTableInfo.set_room_user_id(m_RoomUserId);
	m_pTableInfo.set_current_num(m_CurTurn);
	m_pTableInfo.set_all_num(m_TotalTurn);
	m_pTableInfo.set_base_money(base_money);
	m_pTableInfo.set_stand_money(stand_money);
	m_pTableInfo.set_game_player(m_GamePlayer);
	m_pTableInfo.set_gcrdfrmlstnum(m_RepertoryCard.GetCardCount_FromBack());
	for (int i = 0; i < m_Start_Shai.Num; i++)
	{
		m_pTableInfo.add_shai_set(m_Start_Shai.Shai[i]);
	}
	//添加手牌
	proto::game::HandCards *pstHcTableInfo = m_pTableInfo.mutable_hand_cards();    	
	Fill_HandCard(player->seatid, pstHcTableInfo);
	
	if (m_bPiao){
		for (std::set<int>::iterator it = m_PiaoScore.begin(); it != m_PiaoScore.end(); ++it)
			m_pTableInfo.add_piao_count(*it);
	}
	_tint32 wChairID = player->seatid;
	if (wChairID >= 0 && wChairID < m_GamePlayer){
		m_pTableInfo.set_user_action(m_Seats[wChairID].m_bUserOperate);
		m_pTableInfo.set_response(m_Seats[wChairID].m_bResponse);
		//AddHandCard(wChairID, m_pTableInfo);
		//AddListenCard(wChairID, m_pTableInfo);
		//if (m_Seats[wChairID].m_bUserOperate & ACK_CHI){
		//	AddChiCard(wChairID, m_pTableInfo);
		//}
	}
	proto::game::tagDissolveRoomInfo* pstDissolveInfo = m_pTableInfo.mutable_dissolve_room_info();
	pstDissolveInfo->set_state(dissovle_state);
	if (dissovle_state == 1){
		pstDissolveInfo->set_uid(dissolve_applyer);
		for (std::map<int, Player*>::iterator it = voters_players.begin(); it != voters_players.end(); it++){
			pstDissolveInfo->add_voters_uid(it->first);
			proto::game::tagPlayers* pstDissPlayers = pstDissolveInfo->add_players();
			pstDissPlayers->set_uid(it->second->uid);
			pstDissPlayers->set_action(it->second->dissovle_state);
		}
		int wRemainTime = (int)ev_timer_remaining(phz.loop, &dissolve_room_timer);
		pstDissolveInfo->set_remain_time(wRemainTime);
	}
	for (_uint8 c = 0; c < m_GamePlayer; ++c){
		Player* p = m_Seats[c].m_Player;
		if (p == NULL) continue;
		proto::game::tagPlayers* pstTablePlayers = m_pTableInfo.add_players();
		pstTablePlayers->set_uid(p->uid);
		pstTablePlayers->set_seatid(p->seatid);
		pstTablePlayers->set_ready(m_Seats[c].m_bReady);
		pstTablePlayers->set_money(p->money);
		pstTablePlayers->set_name(p->name);
		pstTablePlayers->set_sex(p->sex);
		pstTablePlayers->set_avatar(p->avatar);
		//AddAbandCard(c, *pstTablePlayers);
		if (m_bPiao){
			pstTablePlayers->set_piao(m_Seats[c].m_UserPiaoScore);
		}
		pstTablePlayers->set_offline(p->is_offline);
		//添加手牌
		proto::game::HandCards *pstHc = pstTablePlayers->mutable_hand_cards();    	
    	Fill_HandCard(c, pstHc);

    	pstTablePlayers->set_hand_count(m_Seats[c].m_HandCard.ChangeableCardsLen);
    	pstTablePlayers->set_total_score(m_Seats[c].m_SettleInfo.m_wTotalScore);
    	//超时状态
    	proto::game::AckUserOvertimeState* pAckOverTimeState = pstTablePlayers->mutable_user_overtime();
    	pAckOverTimeState->Clear();
    	pAckOverTimeState->set_chairid(c);
    	pAckOverTimeState->set_state(m_bOverTimeState[c]);
        //打出过的牌
    	for (_uint8 j = 0; j < m_Seats[c].m_bOutRecord.Size(); ++j){
			pstTablePlayers->add_out_record(m_Seats[c].m_bOutRecord[j]);
		}

        //以下信息挪到tableinfo中，同时tagplayers中保留以备之后使用
		m_pTableInfo.add_hand_count(m_Seats[c].m_HandCard.ChangeableCardsLen);
		
		proto::game::tagShowOutCard *pstShow = m_pTableInfo.add_showcard();
		for (int j = 0; j < m_Seats[c].m_bOutRecord.Size(); j++)
		{
			pstShow->add_outcard(m_Seats[c].m_bOutRecord[j]);
		}
		pstShow->set_outcardlen(m_Seats[c].m_bOutRecord.Size());

		//胡牌信息
		proto::game::tagUserHuInfo *pstUserHuTableInfo = m_pTableInfo.add_huinfo();
		for (int j = 0; j < m_stHuScore[c].stHu.Size(); j++)
		{
			proto::game::tagHu *pstHu = pstUserHuTableInfo->add_hu();
			pstHu->set_hucard(m_stHuScore[c].stHu[j].bHuCard);
			pstHu->set_pashuchiarid(m_stHuScore[c].stHu[j].bPasHuChairID);
		}

		proto::game::tagGroupCard *pstGr = m_pTableInfo.add_groupcard();
		for (int j = 0; j < m_Seats[c].m_HandCard.FixedCardsLen && j < 4; j++)
		{
			proto::game::FixedCard *pstFix = pstGr->add_fixedcard();
			Fill_FixedCard(c, j, pstFix);
		}
	}

	for (int i = 0; i < m_room_config.Size(); i++)
	{
		m_pTableInfo.add_roomconfigid(m_room_config[i]);
	}

	m_pTableInfo.set_base_score(base_score);
	m_pTableInfo.set_laizicard(m_LaiziCard);

	m_pTableInfo.set_left_time(get_operate_remain_time(m_Seats[wChairID].m_bUserOperate, true));
	m_pTableInfo.set_preready_timer(preready_timer_stamp);		
    //弯暗杠信息
	if (m_Seats[wChairID].m_bUserOperate & TYPE_ANGANG || m_Seats[wChairID].m_bUserOperate & TYPE_WANGANG)
	{
		MJ_opinfo stGang;
		GetGangInfo(wChairID, stGang);
		for (int len = 0; len < stGang.Size(); len++)
		{
			proto::game::tagGangInfo *pstGang = m_pTableInfo.add_ganginfo();
			pstGang->set_card(stGang[len].cbCard);
			pstGang->set_state(stGang[len].nState);
		}
	}
    //听牌信息
    if (m_Seats[wChairID].m_bUserOperate & TYPE_LISTEN)
	{
		tagListenInfos stListenInfo[14];
		_uint8 bListenLen = 0;

		GetListeningInfo(wChairID, stListenInfo, bListenLen);

		for (int j = 0; j < bListenLen; j++)
		{
			PackageTingInfo(wChairID, stListenInfo, j, m_pTableInfo.add_listeninfo());
		}
	}
	else
	{
        tagListenInfos stListenInfo[14];
		_uint8 bListenLen = 0;

		GetListeningInfoNoOutCard(wChairID, stListenInfo, bListenLen);

		for (int j = 0; j < bListenLen; j++)
		{
			PackageTingInfo(wChairID, stListenInfo, j, m_pTableInfo.add_listeninfo());
		}
	}
	
	return 0;
}

void Table::handler_switch_style(Player *player)
{
    proto::game::AckSwitchStyle stSwitchStyle;
    stSwitchStyle.set_seatid(player->seatid);
    
    Ppacket ppack;
	stSwitchStyle.SerializeToString(&ppack.body);
	ppack.pack(SERVER_SWITCH_STYLE_UC);
	unicast(player, ppack.data);

	log.debug("%s AckSwitchStyle:%s .\n", __FUNCTION__, stSwitchStyle.DebugString().c_str());
}

void Table::OnFinalEndReq(Player* player)
{
	proto::game::AckFinalEnd stFEnd;
	stFEnd.set_roomid(tid);
	stFEnd.set_room_user_id(m_RoomUserId);
	stFEnd.set_ts((int)time(NULL));
	for (_uint8 i = 0; i < m_GamePlayer; ++i){
		std::map<int, Player*>::iterator iter = players.find(m_Seats[i].m_Uid);
		if (iter != players.end() && iter->second){
			proto::game::tagPlayers* pstPlayer = stFEnd.add_players();
			pstPlayer->set_seatid(i);
			pstPlayer->set_uid(iter->second->uid);
			pstPlayer->set_name(iter->second->name);
			pstPlayer->set_avatar(iter->second->avatar);
			pstPlayer->set_sex(iter->second->sex);
			pstPlayer->set_hu_count(m_Seats[i].m_SettleInfo.m_HuCnt);
			
			pstPlayer->set_dian_pao_count(m_Seats[i].m_SettleInfo.m_DianPaoCnt);
			pstPlayer->set_best_score(m_Seats[i].m_SettleInfo.m_wBestScore);
			pstPlayer->set_total_score(m_Seats[i].m_SettleInfo.m_wTotalScore);
			pstPlayer->set_all_piao_score(m_Seats[i].m_SettleInfo.m_wAllPiaoScore);
			pstPlayer->set_zimo_count(m_Seats[i].m_SettleInfo.m_ZimoCnt);
			pstPlayer->set_win_count(m_Seats[i].m_SettleInfo.m_WinCnt);
			pstPlayer->set_lose_count(m_Seats[i].m_SettleInfo.m_LoseCnt);
		}
	}
	Ppacket ppack;
	stFEnd.SerializeToString(&ppack.body);
	ppack.pack(SERVER_PHZ_FINAL_END_RES);
	unicast(player, ppack.data);
	log.debug("%s tid:%s, AckFinalEnd:%s .\n", __FUNCTION__, tid.c_str(), stFEnd.DebugString().c_str());
}


bool Table::OnUserGainsReq(Player* player)
{
	_tint32 wChairID = player->seatid;
	if(wChairID < 0 || wChairID >= GAME_PLAYER){
		log.error("On user gains req error uid:%d wChairID[%d] tid:%s .\n", player->uid, wChairID, tid.c_str());
		return false;
	}
	proto::game::AckGains stGains;
	Player *p = NULL;
	for(_uint8 i = 0; i < GAME_PLAYER; ++i){
		if(m_Seats[i].m_Player == NULL) continue;
		p = m_Seats[i].m_Player;
		proto::game::tagPlayers* pstPlayer = stGains.add_players();
		pstPlayer->set_seatid(p->seatid);
		pstPlayer->set_uid(p->uid);
		pstPlayer->set_name(p->name);
		pstPlayer->set_avatar(p->avatar);
		pstPlayer->set_sex(p->sex);
		pstPlayer->set_money(p->money);
		_tint32 wTotalGains = 0;
		std::vector<tagGain>::iterator iter = m_vecGains.begin();
		for(; iter != m_vecGains.end(); ++iter){
			pstPlayer->add_gains((*iter).Gain[i]);
			wTotalGains += (*iter).Gain[i];
		}		
		pstPlayer->set_total_gains(wTotalGains);
	}
	Ppacket ppack;
	stGains.SerializeToString(&ppack.body);
	ppack.pack(SERVER_PHZ_GAINS_RES);
	unicast(player, ppack.data);
	return true;
}

int Table::advance_game_end()
{
	log.debug("advance game_end\n");
	m_State = GAME_END;
	ev_timer_stop(phz.loop, &start_timer);
	ev_timer_stop(phz.loop, &first_getcard_timer);	
	ev_timer_stop(phz.loop, &preready_timer);
	ev_timer_stop(phz.loop, &dispatch_show_timer);
	ev_timer_stop(phz.loop, &dissolve_room_timer);
	is_dissolved = 1;
	m_cbCurTimer = 0;
	return 0;
}

void Table::handler_offline(Player *player)
{
	proto::game::AckOffline stOffline;
	stOffline.set_uid(player->uid);
	stOffline.set_seatid(player->seatid);
	Ppacket ppack;
	stOffline.SerializeToString(&ppack.body);
	ppack.pack(SERVER_PLAYER_OFFLINE_BC);
	broadcast(NULL, ppack.data);
}