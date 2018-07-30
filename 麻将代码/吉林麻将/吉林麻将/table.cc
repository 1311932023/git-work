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
#include <algorithm>
#include <assert.h>
#include <sstream> 
#include <sys/time.h>

#include "game.h"
#include "common/log.h"

#include "table.h"
#include "common/client.h"
#include "common/player.h"
#include "proto/proto.h"
#include "card.h"
#include "common/base64.h"



extern ZJH zjh;

static Fantype_to_FanNum g_ft2fn;

Table::Table() : m_CardPool(CARD_POOL_NO_FLOWER, 0)
{

	preready_timer_stamp = zjh.conf["timers"]["preready_timer_stamp"].asInt();	
	m_bDelayTime = zjh.conf["timers"]["delay_timer_stamp"].asInt();

	
	m_Outcard_Timer_stamp = zjh.conf["timers"]["outcard_timer_stamp"].asInt();
	m_Operate_Timer_stamp0 = zjh.conf["timers"]["operate_timer_stamp"].asInt();
	m_Operate_Timer_stamp1 = m_Operate_Timer_stamp0;
	m_Operate_Timer_stamp2 = m_Operate_Timer_stamp0;
	m_Operate_Timer_stamp3 = m_Operate_Timer_stamp0;
    m_Disbandtable_Timer_stamp= 180;
	m_Disbandtable_Timer_stamp = zjh.conf["timers"]["disbandtable_timer_stamp"].asInt();
	m_Disbandtable_Timer_stamp1 = 120;
	m_Disbandtable_Timer_stamp1 = zjh.conf["timers"]["disbandtable_timer_stamp1"].asInt();
	m_bDisbandCount = 2;

	//m_bDisbandCount = zjh.conf["tables"]["disbandtable_times"].asInt();

	log_debug("preready_timer_stamp[%d]  m_bDelayTime[%d] m_Outcard_Timer_stamp[%d] \
		m_Operate_Timer_stamp0[%d] m_Disbandtable_Timer_stamp[%d] m_Disbandtable_Timer_stamp1[%d]", 
		preready_timer_stamp, 
		m_bDelayTime,
		m_Outcard_Timer_stamp 
		, m_Operate_Timer_stamp0 
		, m_Disbandtable_Timer_stamp 
		, m_Disbandtable_Timer_stamp1 
		);

	m_RandomSeat = false;
	if (!zjh.conf["tables"]["RandomSeat"].isNull())
	{ 
		m_RandomSeat = zjh.conf["tables"]["RandomSeat"].asInt();
	}

	m_Outcard_Timer.data = this;
	ev_timer_init(&m_Outcard_Timer, Table::UserOutCard_Timer_cb, m_Outcard_Timer_stamp+ m_bDelayTime, m_Outcard_Timer_stamp+ m_bDelayTime);

	m_Operate_Timer0.data = this;
	ev_timer_init(&m_Operate_Timer0, Table::UserChooseOperate_Timer_cb0, m_Operate_Timer_stamp0+ m_bDelayTime, m_Operate_Timer_stamp0+ m_bDelayTime);

	m_Operate_Timer1.data = this;
	ev_timer_init(&m_Operate_Timer1, Table::UserChooseOperate_Timer_cb1, m_Operate_Timer_stamp1+ m_bDelayTime, m_Operate_Timer_stamp1+ m_bDelayTime);

	m_Operate_Timer2.data = this;
	ev_timer_init(&m_Operate_Timer2, Table::UserChooseOperate_Timer_cb2, m_Operate_Timer_stamp2+ m_bDelayTime, m_Operate_Timer_stamp2+ m_bDelayTime);

	m_Operate_Timer3.data = this;
	ev_timer_init(&m_Operate_Timer3, Table::UserChooseOperate_Timer_cb3, m_Operate_Timer_stamp3+ m_bDelayTime, m_Operate_Timer_stamp3+ m_bDelayTime);
    
	m_Disbandtable_Timer0.data = this;
	ev_timer_init(&m_Disbandtable_Timer0, Table::Disband_table_Timer_cb0, m_Disbandtable_Timer_stamp, m_Disbandtable_Timer_stamp);		

	send_videodata_timer.data = this;
	send_videodata_timer_stamp = 0.4;
	ev_timer_init(&send_videodata_timer, Table::Send_Video_Data_Timer_cb, send_videodata_timer_stamp, send_videodata_timer_stamp);

	m_RecvDisbandTable_Timer.data = this;
	m_RecvDisbandTable_Timer_stamp = 5;
	ev_timer_init(&m_RecvDisbandTable_Timer, Table::Recv_DisbandTable_Timer_cb, m_RecvDisbandTable_Timer_stamp, m_RecvDisbandTable_Timer_stamp);

	m_Disband_ResUser = 0;
	init_member_variable();
	m_bGameState = GAME_FREE;//初始化状态
	m_bBanker = INVALID_CHAIRID;
	m_bIsDisbandTable = false;
	m_bDeleteTable = false;
	m_nPlayingTimes = 0;
	m_nAllRound = 8;
	m_nCurRound = 0; 

	memset(m_DisbandChoose, 0, sizeof(m_DisbandChoose));
	memset(m_IsOffline, false, sizeof(m_IsOffline));
	memset(PlayerAllOutCard, 0, sizeof(PlayerAllOutCard));
	memset(m_bUserOperate, 0, sizeof(m_bUserOperate));
	memset(m_CanBaiPaiArr, 0, sizeof(m_CanBaiPaiArr));
	memset(m_PlayGrapCard, 0, sizeof(m_PlayGrapCard));
	m_room_config.Clear();

	m_GAME_PLAYER = GAME_PLAYER;

	for (int i = 0; i < GAME_PLAYER; i++)
	{
		m_Online_Beat[i] = false;
	}	
	m_Offline_timer.data = this;
	m_Offline_timer_stamp = NODATA_TIMEOUT;
	ev_timer_init( &m_Offline_timer, Table::Check_Offline_Timer_cb, m_Offline_timer_stamp, m_Offline_timer_stamp);
	ev_timer_start(zjh.loop, &m_Offline_timer);

	req_start_timer_stamp = 3.0f;
	req_start_timer.data = this;
	ev_timer_init(&req_start_timer, Table::RequestGameStart_Timer_cb, req_start_timer_stamp, req_start_timer_stamp);

	m_ft2fn = &g_ft2fn;
	m_BaseScore = 1;  

	m_bGangCalcImme = false;

	m_last_Banker = INVALID_CHAIRID;
	m_nGameRunState = 0;
	m_canBankCanOpCard = true;

}

Table::~Table()
{

	log_debug("delete  !!!   ");
	
	ev_timer_stop(zjh.loop, &m_Outcard_Timer);
	ev_timer_stop(zjh.loop, &m_Operate_Timer0);
	ev_timer_stop(zjh.loop, &m_Operate_Timer1);
	ev_timer_stop(zjh.loop, &m_Operate_Timer2);
	ev_timer_stop(zjh.loop, &m_Operate_Timer3);   //66 
	//ev_timer_stop(zjh.loop, &m_CurTimer); 

	TIMER_STOP(m_Disbandtable_Timer0);	
	m_RecvDisbandTable_Timer.data = NULL; 
	TIMER_STOP(m_RecvDisbandTable_Timer);
	m_Offline_timer.data = NULL;

	TIMER_STOP(m_Offline_timer);	//9 s
	req_start_timer.data = NULL;

	TIMER_STOP(req_start_timer); //10 
	send_videodata_timer.data = NULL;
	ev_timer_stop(zjh.loop, &send_videodata_timer);   //  11 
	

	

}

void Table::init_member_variable()
{
	vid = 0;
	zid = 0;

	lose_exp = 0;
	win_exp = 0;

}

void Table::init(const Json::Value &val, const Json::Value &valCfg, Player *player)
{
	tid = 0;
	int my_vid = val["vid"].asInt();
	int my_zid = val["zid"].asInt();

	tid = atoi(player->tid_str.c_str());	
	vid = ( vid > 0 ) ? vid : my_vid;
	zid = ( zid > 0 ) ? zid : my_zid;		
	
	srand(tid);		
   
	ConfInit_Remote(valCfg);		
		
	log_info(" player tid:%s uid:%d ", player->tid_str.c_str(), player->uid);
	
	if (!val["resuser"].isNull())
	{
		m_Disband_ResUser = val["resuser"].asInt();
	}
}



void Table::ConfInit_Remote(const Json::Value& val)
{
	m_room_config.Clear();

	if (!val["baseGold"].isNull())
	{
		m_BaseScore = val["baseGold"].asInt();
		log_debug("m_BaseScore [%d]", m_BaseScore );
	}
	if (!val["standMoney"].isNull())
	{
		stand_money = val["standMoney"].asInt();
	}
	if (!val["baseGold"].isNull())
	{
		base_money = val["baseGold"].asInt();
	}
	if (!val["standMoney"].isNull())
	{
		stand_money = val["standMoney"].asInt();
	}
	if (!val["roomGolden"].isNull())
	{
		roomGold = val["roomGolden"].asInt();
	}
	if (!val["roomUserId"].isNull())
	{
		m_room_owner_uid = val["roomUserId"].asInt();
	}
	if (!val["roomStatus"].isNull())
	{
		m_room_status = val["roomStatus"].asInt();
	}	
}

int Table::handler_login(Player *player)
{
	if (player == NULL)
	{
		log_error("handler_login error, player is NULL \n");
		return -1;
	}
	
	if (!HasSeat(player))
	{	
		//不让多余的人登录
		log_error("%s tid:%d uid:%d seat is full force logout!\n", __FUNCTION__, tid, GetPlayerUid(player));
		zjh.game->dz_login_error_back(player->uid,  100);
		force_logout_room_req(player);		
		//logout_room_req(player);
		del_player(player);
		return -1;
	}
	log_info("%s tid:%d uid:%d client:0x%x usertype:%d ", __FUNCTION__, tid, player->uid, player->client, player->usertype);
	bool bFind = false;//是否已经是桌子上的玩家
	std::map<int, Player *>::iterator iter = m_mapTable_Player.find(player->uid);
	
	if (iter != m_mapTable_Player.end())
	{
		log_error("%s uid:%d can not sitdown because find in this table\n", __FUNCTION__, player->uid);
		bFind = true;
	}		

	if (!bFind)
	{
		player->tid_str = to_char(tid);
		m_mapTable_Player.insert(pair<int, Player *>(player->uid, player));
		m_bCurPlayers = m_mapTable_Player.size();

		handler_login_succ_uc(player);
		handler_table_info(player);	
		handler_login_succ_bc(player);
		Send_Game_Scene(player);

		if (player->seatid < 0 || player->seatid >= GAME_PLAYER)		
		{
			handler_apply_uptable(player);
		}
		return 0;
	}

	return -1;
}

int Table::Sit_Down(Player *player)
{
	if (m_RandomSeat)
	{
		MJ_BUFFER<_uint8, GAME_PLAYER> s;
		for (int i = 0; i < m_GAME_PLAYER; i++)
		{
			if (m_SeatPlayer[i].state == Seat::E_Seat_Empty)
			{
				s.Add(i);
			}
		}
		if (s.IsNull())
		{
			return -1;
		}

		srand(time(0) + tid);
		_uint8 i = s[rand() % s.Size()];
		m_SeatPlayer[i].action = ACTION_SIDOWN;
		m_SeatPlayer[i].state = Seat::E_Seat_Occupy;
		m_SeatPlayer[i].player = player;
		m_SeatPlayer[i].seatid = i;
		m_SeatPlayer[i].uid = player->uid;
		player->status = SIT_DOWN;

		log_info("%s tid:%d player uid:%d chairid:%d success ", __FUNCTION__, tid, player->uid, i);
		return i;

	}
	else
	{
		for (int i = 0; i < m_GAME_PLAYER; i++)
		{
			if (m_SeatPlayer[i].state == Seat::E_Seat_Empty)
			{
				m_SeatPlayer[i].action = ACTION_SIDOWN;
				m_SeatPlayer[i].state = Seat::E_Seat_Occupy;
				m_SeatPlayer[i].player = player;
				m_SeatPlayer[i].seatid = i;
				m_SeatPlayer[i].uid = player->uid;
				player->status = SIT_DOWN;

				log_info("%s tid:%d player uid:%d chairid:%d success ", __FUNCTION__, tid, player->uid, i);
				return i;
			}
		}
	}
	return -1;
}
bool Table::Re_Sit_Down(Player* player)
{
	if (player == NULL)
		return false;
	for (int i = 0; i < m_GAME_PLAYER; i++)
	{
		if (m_SeatPlayer[i].player == NULL && m_SeatPlayer[i].uid == player->uid)
		{
			m_SeatPlayer[i].action = ACTION_SIDOWN;
			m_SeatPlayer[i].state = Seat::E_Seat_Occupy;
			m_SeatPlayer[i].player = player;
			m_SeatPlayer[i].seatid = i;
			m_SeatPlayer[i].uid = player->uid;
			player->status = SIT_DOWN;
			player->seatid = i;

			log_info("%s tid:%d player uid:%d chairid:%d success ", __FUNCTION__, tid, player->uid, i);
			return true;
		}
	}
	return false;
}
void Table::Stand_Up(Player *player)
{
	if (player->seatid >= m_GAME_PLAYER || player->seatid <0)
	{
		log_error("%s uid:%d stand up but seatid:%d is wrong \n ", __FUNCTION__, player->uid, player->seatid);
		return;
	}
	log_info("%s uid:%d chairid:%d success ", __FUNCTION__, player->uid, player->seatid);
	player->status = STAND_UP;
	m_SeatPlayer[player->seatid].clear();
	m_bBanker = INVALID_CHAIRID;
}
void Table::Player_Leave(Player *player)
{
	if (player->seatid >= m_GAME_PLAYER || player->seatid < 0)
	{
		log_error("%s uid:%d leave but seatid:%d is wrong \n ", __FUNCTION__, player->uid, player->seatid);
		return;
	}
	log_info("%s uid:%d chairid:%d success\n", __FUNCTION__, player->uid, player->seatid);
	player->status = STAND_UP;
	m_SeatPlayer[player->seatid].leave();
	
}

int Table::del_player(Player *player)
{
	if (player == NULL)
	{
		log_error("del_player, but player is NULL \n");
		return -1;
	}
	log_info("%s tid:%d uid:%d player nums:%d \n", __FUNCTION__, tid, player->uid, m_mapTable_Player.size());
	std::map<int, Player *>::iterator iter = m_mapTable_Player.find(player->uid);
	if (iter == m_mapTable_Player.end())
	{
		log_error("can not find uid:%d \n", player->uid);
		return -1;
	}
	if (m_bGameState != GAME_FREE)
	{
		if (player->seatid >= 0 && player->seatid < m_GAME_PLAYER)
		{
            m_IsOffline[player->seatid] = true;//记录断线情况
//			UserOnlineState_bc(player->seatid);
		}	
	}
	else
	{
		if (player->seatid >= 0 && player->seatid < m_GAME_PLAYER)
		{
			m_IsOffline[player->seatid] = false;//记录断线情况
			if (m_bIsDisbandTable)
			{   
				//游戏没开始时玩家掉线或者退出时 默认不解散
                RecvDisbandTableChoose(player->seatid, proto::game::DISBAND_NO);
			}
		}	
	}
	m_mapTable_Player.erase(player->uid);
	m_bCurPlayers = m_mapTable_Player.size();	
	if (m_room_status == 0)
	{
		Stand_Up(player);
	}
	else
	{
		Player_Leave(player);
	}
	
	
	if (m_bCurPlayers == 0)
	{		
		return 1;
	}

	return 0;	
}

int Table::handler_login_succ_uc(Player *player)
{  
	if (NULL == player)
	{
		log_error("handler_login_succ_uc error player is null");
		return -1;
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

	unicast(SERVER_LOGIN_SUCC_UC, player, stLoginuc);
	//log_info("%s send to client uid:%d \n", __FUNCTION__, player->uid);
	return 0;	
}

// SERVER_LOGIN_SUCC_BC
int Table::handler_login_succ_bc(Player *player)
{
	proto::login::AckLoginSuccessBc stLoginbc;
	stLoginbc.set_seatid(player->seatid);
	stLoginbc.set_uid(player->uid);
	stLoginbc.set_money(player->money);
	if (player->name != "")
	{
		stLoginbc.set_name(player->name.c_str());
	}
	if (player->avatar.size() != 0)
	{
		stLoginbc.set_avatar(player->avatar.c_str());
	}
	
	stLoginbc.set_sex(player->sex);

	broadcast(SERVER_LOGIN_SUCC_BC, stLoginbc);
	return 0;
}

int Table::handler_table_info(Player *player)
{
	if (player == NULL)
	{
		log_error("handler_table_info player is NULL ");
		return - 1;
	}
	
	proto::login::AckTableInfo stTableinfo;
	stTableinfo.set_seatid(player->seatid);
	stTableinfo.set_roomgolden(roomGold);

	for (int i = 0; i < m_GAME_PLAYER; i++)
	{
		if ( m_SeatPlayer[i].player)
		{
			proto::login::tagPlayers *paddPlayer = stTableinfo.add_players();
			paddPlayer->set_uid(m_SeatPlayer[i].player->uid);
			paddPlayer->set_seatid(m_SeatPlayer[i].player->seatid);
			_uint8 bReady = (m_SeatPlayer[i].action == ACTION_READY ? 1:0);
			paddPlayer->set_ready(bReady);
			paddPlayer->set_money(m_SeatPlayer[i].player->money);
			if (m_SeatPlayer[i].player->name != "")
			{
				paddPlayer->set_name(m_SeatPlayer[i].player->name.c_str());
			}
			if (m_SeatPlayer[i].player->avatar != "")
			{
				paddPlayer->set_avatar(m_SeatPlayer[i].player->avatar.c_str());
			}
			paddPlayer->set_sex(m_SeatPlayer[i].player->sex);
		}	
	}

	stTableinfo.set_cardpoolnums(m_CardPool.MaxCount());
	if (m_Start_Shai.Num > 0)
	{
		stTableinfo.add_touziinfo(m_Start_Shai.Shai[0] + 1);
		stTableinfo.add_touziinfo(m_Start_Shai.Shai[1] + 1);
	}
	stTableinfo.set_gcrdfrmlstnum( m_CardPool.GetCardCount_FromBack());

	unicast(SERVER_TABLE_INFO_UC, player, stTableinfo);
	
	log_info(" send to client uid:%d ", player->uid);

	return 0;
}

int Table::handler_ready(Player *player)
{  
	log_info("%s ", __FUNCTION__);
	//clean_table_user();
	return 0;
}

////////////玩家主动退出去 

int Table::handler_logout(Player *player)
{   
	if (m_bGameState != GAME_FREE || m_room_status != 0)
	{
		for (int i = 0; i < m_GAME_PLAYER; i++)
		{
			if (m_SeatPlayer[i].player!=NULL && m_SeatPlayer[i].player->uid == player->uid)
			{
				log_info("%s error uid:%d is in playing ", __FUNCTION__, player->uid);				
				if (player->seatid >= 0 && player->seatid < m_GAME_PLAYER)
				{
					m_IsOffline[player->seatid] = true;//记录断线情况
//					UserOnlineState_bc(player->seatid);
				}	
				return -1;
			}	
		}	
	}

	logout_room_req(player);

	log_info("%s tid:%d uid:%d seatid:%d money:%d ", __FUNCTION__, tid, player->uid, player->seatid, player->money);

	//handler_downtable( player );
	//广播退出房间协议
	proto::login::AckLogoutBc stLogout;
	stLogout.set_seatid(player->seatid);
	stLogout.set_uid(player->uid);
	stLogout.set_type(player->logout_type);	

	broadcast(SERVER_LOGOUT_SUCC_BC, stLogout);
	return 0;
}

//聊天 玩家 

int Table::handler_chat(Client *client)
{	

	proto::login::AckChatBC  chardata;

	if (chardata.ParseFromArray(client->m_Buffer->proto_data, client->m_Buffer->proto_head.length))
	{
		broadcast(SERVER_CHAT_BC, chardata);
	}
	else
	{
		log_error("%s tid:%d  ackchatbc parse error \n", __FUNCTION__, tid);
	}
	return 0;
}

int Table::handler_interaction_emotion(Client *client, Player *player)
{
	if (client == NULL || player == NULL)
	{
		return -1;
	}
		
	
	proto::login::ReqEmotion stEmotion;
	stEmotion.ParseFromArray(client->m_Buffer->proto_data, client->m_Buffer->proto_head.length);
	int target_seatid = stEmotion.target_seatid();
	int target_seat_num = stEmotion.target_seat_num();
	int itype = stEmotion.type();
	
	log_info("%s target uid:%d chairid:%d seatnum:%d type:%d ", 
		__FUNCTION__, GetPlayerUid(target_seatid), target_seatid, target_seat_num, itype);

	proto::login::AckEmotionBC stEmotionBC;
	stEmotionBC.Clear();
	stEmotionBC.set_seatid(player->seatid);
	stEmotionBC.set_money(player->money);
	stEmotionBC.set_target_seatid(target_seatid);
	stEmotionBC.set_target_seat_num(target_seat_num);
	stEmotionBC.set_type(itype);

	broadcast(SERVER_EMOTION_BC, stEmotionBC);
	return 0;
	
}

void Table::handler_daemonize( Player *player )
{
	
}

int Table::handler_info(Player *player)
{
	UpdateUserInfoToUser(player);
	return 0;
}

void Table::update_user_info_to_datasvr_dz(tagUserScore userscore[GAME_PLAYER], int tally_type, int cmd_type, int alter_type)
{
	Jpacket packet_data;
	packet_data.val["cmd"] = cmd_type;

	for (int i=0; i<m_GAME_PLAYER; i++)
	{
		packet_data.val["players"][i]["uid"] = userscore[i].uid;
		packet_data.val["players"][i]["alter_money"] = userscore[i].score;
		packet_data.val["players"][i]["al_board"] = 1;
		if( userscore[i].score > 0 ){
			packet_data.val["players"][i]["al_win"] = 1;
			packet_data.val["players"][i]["alter_exp"] = win_exp;
		}else{
			packet_data.val["players"][i]["al_win"] = 0;
			packet_data.val["players"][i]["alter_exp"] = lose_exp;
		}

	}	
	
	packet_data.val["vid"] = vid;
	packet_data.val["roomid"] = to_char(tid);
	
	char strbuf[50] = {0}; 
	sprintf(strbuf,"%d_%04d",tid, m_nPlayingTimes);
	std::string strTimes = strbuf;
	
	packet_data.val["innings"]= strTimes; 
	packet_data.end();
	
	zjh.game->send_to_datasvr(packet_data.tostring());
	int play_min = min( m_totalrecordinfo.round_score.MaxSize()   ,  m_nPlayingTimes); 

	 //while (  m_totalrecordinfo.round_score.Size() < m_nPlayingTimes ) 
	while (m_totalrecordinfo.round_score.Size() < play_min )
	{
		m_totalrecordinfo.round_score.Add(::RoundScore(0, 0, 0, 0));
	}

	if (m_nPlayingTimes > 0) 
	{
		for (int i = 0; i < m_GAME_PLAYER; i++)
		{
			// m_totalrecordinfo.round_score[m_nPlayingTimes - 1].score[i] += userscore[i].score;
			m_totalrecordinfo.round_score[play_min - 1].score[i] += userscore[i].score;
			log_debug("last fen [%d]" , userscore[i].score );
		}
	}
	
}

void Table::handler_all_players_info( Player *player )
{

}

void Table::InitData()
{
	m_bGameState = GAME_FREE;
	m_bCurChairID = INVALID_CHAIRID;//当前出牌或摸牌的椅子ID
	//m_bCurGrapId = INVALID_CHAIRID;

	m_bMultiOpUserNum = 0;//引起有操作提示的人数	
	m_bGrapId = 0  ;
	
	m_bMultiDoneOpNum = 0;//已经选择操作的玩家数
	m_bMulitFirstOperate = false;//优先级最高的玩家是否执行还是放弃，放弃为false	
	
	m_bCurCard = INVALID_MJ_CARD;	
    m_bMulitHuOpeCount = 0;	
	m_RobWanGangCard = INVALID_MJ_CARD;
	m_RobOperateNum = 0;
	m_bRobHu = false;
	m_RobHuNum = 0;
	m_bRobWanGang = false;
	m_bRobGangHuJudge = false;
	m_WangGangPlayerOpType = TYPE_NULL;
	m_bOutCardChairID = INVALID_CHAIRID;
	m_bCurOutCard = INVALID_MJ_CARD;
	m_bMultiOpHuNum = 0; 		
    m_videostr.clear();


	memset(m_bMultiUserDoneOp, 0, sizeof(m_bMultiUserDoneOp));//用户选择的操作
	memset(m_bMultiUserDoneOpCard, 0, sizeof(m_bMultiUserDoneOpCard));
    
	memset(m_HandCard, 0, sizeof(m_HandCard));
	memset(m_stUserOutCard, 0, sizeof(m_stUserOutCard));
	
	memset(m_bIsHu, false, sizeof(m_bIsHu));
	
	memset(m_HuFanNums, 0, sizeof(m_HuFanNums));
	
	memset(m_bGetCardCount, 0, sizeof(m_bGetCardCount));
	memset(m_bAddFanFlag, 0, sizeof(m_bAddFanFlag));
	
	memset(m_bAutoHu, 0, sizeof(m_bAutoHu));
	memset(m_RobHuDoneOp, 0, sizeof(m_RobHuDoneOp));
	memset(m_bIsTrustee, false, sizeof(m_bIsTrustee));
		
	//初始化杠的椅子id
	
	memset(m_stGang, 0, sizeof(m_stGang));
	memset( m_stHuScore, 0, sizeof(m_stHuScore));
	memset( m_oFan, 0, sizeof(m_oFan));
	memset( m_bBaoTingOutCount, 0, sizeof(m_bBaoTingOutCount)); 
	
    reset_disbandcount();
		
}

void Table::Send_Game_Scene(Player *player)
{
	if (player == NULL)
	{
		log_error("Send_GameScene, player is NULL \n");
		return;
	}
	int ChairID = player->seatid;	    
	if (ChairID < 0 || ChairID >= m_GAME_PLAYER)
	{
		Send_lookon_GameScene(player);
		return;
	}

	proto::game::AckGameScene stGameScene;		
	Proto_GameScene(ChairID, player, stGameScene);	
	unicast(SERVER_GAME_SCENE, player, stGameScene);
	
	Reconnect(player->seatid); 

	log_info(" over to uid:%d chairid:%d \n",  player->uid, player->seatid);	
}


//游戏开始 

int Table::game_start_req()
{   	
	m_room_status = 1;

	char strbuf[50] = {0};
	sprintf(strbuf,"%d_%04d",tid, m_nPlayingTimes);
	std::string strTimes = strbuf;
	
	char strroomid[64] = {0};
	sprintf(strroomid, "%d", tid);
	
	Jpacket req_packet;
	req_packet.val["cmd"] = SERVER_DZ_GAME_START_REQ; 
	req_packet.val["vid"] = zid;
	req_packet.val["roomid"] = strroomid;
	req_packet.val["innings"] = strTimes;
	req_packet.end();
	
	zjh.game->send_to_datasvr(req_packet.tostring());
	log_info( "  tid:%d ",  tid);
	return 0;

}
//游戏开始返回

int Table::game_start_res( Jpacket &packet )
{
	TIMER_STOP(req_start_timer);

	int can_start = packet.val["can_start"].asInt();

	m_nAllRound = packet.val["count"].asInt();

	//int play_times = packet.val["currentCount"].asInt();
	//m_nPlayingTimes = play_times; 
	// log_debug("playtimes [%d]" , play_times );

	InitPlayAccont();  

	m_account_info.SetRound(m_nPlayingTimes);
	RedisUpdateScoreToLocal();

	char strbuf[50] = {0};
	sprintf(strbuf,"%d_%04d",tid, m_nPlayingTimes);
	std::string strTimes = strbuf;
	if( 0 == can_start )
	{
		int code = packet.val["code"].asInt();
		proto::game::AckGameStartFail stGameStartFail;
		stGameStartFail.set_vid(vid);
		char strroomid[128] = {0};
		sprintf(strroomid, "%d", tid);
		stGameStartFail.set_roomid(strroomid);
		stGameStartFail.set_can_start(0);
		stGameStartFail.set_code(code);
		int ntime = (int)time( NULL );
		stGameStartFail.set_ts(ntime);

		broadcast(SERVER_DZ_GAME_START_RES, stGameStartFail);

		//不满足开局条件，踢掉所有人
		//del_all_player();	
		//zjh.game->del_all_client();
		log_error( " fail tid:%d can_start[%d] code[%d] nTimes:%s. " , tid, can_start, code ,strTimes.c_str());
		return -1;
	}
	//ev_timer_again(zjh.loop, &ready_timer);
	int nReadNum = 0;
	for (int i = 0; i < m_GAME_PLAYER; i++)
	{
		if (m_SeatPlayer[i].action == ACTION_READY)
		{
            nReadNum++;
		}
	}
	log_info("succ tid:%d can_start[%d] ready[%d]  allround[%d]" , tid, can_start, nReadNum  ,m_nAllRound );

	if (nReadNum == m_GAME_PLAYER && m_bGameState == GAME_FREE)
	{
		GameStart();
	}	
	return 0;
}
int Table::handler_apply_uptable(Player *player)
{   
	if (NULL == player)
	{
        log_error("handler_apply_uptable player is null\n");
		return  -1;
	}
	handler_uptable(player);
	return 0;
}

void Table::uptable_error_uc(Player *player, int code)
{  
	if (NULL == player)
	{   
		log_error("uptable_error_uc error player is null");
		return ;
	}
	proto::login::AckUpTableErrorUc stUpTable;
	stUpTable.set_uid(player->uid);
	stUpTable.set_money(player->money);
	stUpTable.set_code(code);

	unicast(SERVER_UPTABLE_ERR_UC, player, stUpTable);
}

int Table::handler_uptable(Player *player)
{  
	if (NULL == player)
	{
		log_error("handler_uptable player is null ");
		return  -1;
	}
	for (int i = 0; i < m_GAME_PLAYER; i++)
	{
        if (NULL != m_SeatPlayer[i].player)
        {
			if (m_SeatPlayer[i].player->uid == player->uid)
			{
				log_error(" error tid:%d uid:%d already in seat ", tid, player->uid);
				uptable_error_uc(player, 102);
				return -1;  
			}		
        }    
	}
	if (player->money < stand_money) {
		log_error(" error uid:%d money:%d too less stand_money:%d.", player->uid, player->money, stand_money);
		uptable_error_uc(player, 104);
		return -1;
	}

	_uint8 iSeatCount = SeatPlayerCount();
	int seatid = -1 ;
	if (iSeatCount < m_GAME_PLAYER) {
		seatid = Sit_Down(player);
	}
	else if (Re_Sit_Down(player))
	{
		seatid = player->seatid;
	}
	else
    {   
		log_error(" error uid:%d seatnum:%d ",  player->uid, iSeatCount);
		uptable_error_uc(player, 105);
		return -1;
    }
	
	player->seatid = seatid;
	proto::login::AckUpTableSuccess stUpTable;
	stUpTable.set_uid(player->uid);
	stUpTable.set_seatid(player->seatid);
	if (player->name.size() !=0)
	{
        stUpTable.set_name(player->name.c_str());
	}	
	stUpTable.set_sex(player->sex );   //性别问题 
	if (player->avatar.size() != 0)
	{
        stUpTable.set_avatar(player->avatar.c_str());
	}
	
	stUpTable.set_money(player->money);
	stUpTable.set_coin(player->coin);
	stUpTable.set_exp(player->exp);
	stUpTable.set_pcount(player->pcount);
	stUpTable.set_vlevel(player->vlevel);
// 	if (player->ps.size() != 0)
// 	{
//         stUpTable.set_ps(player->ps.c_str());
// 	}	
	broadcast(SERVER_UPTABLE_SUCC_BC, stUpTable);

	log_info("succ uid:%d chairid:%d money:%d ",  player->uid, player->seatid, player->money);
	return 0;
}
int Table::handler_downtable( Player *player )
{  
	if (player == NULL)
	{
		log_error("handler_downtable error player is null. ");
		return -1;
	}
	bool bFindUser = false;
	for (int i = 0; i < m_GAME_PLAYER; i++)
    {
        if (m_SeatPlayer[i].player != NULL)
        {
            if (m_SeatPlayer[i].player->uid == player->uid)
            {
				bFindUser = true;
            }           
        }      
    }
    if (!bFindUser)
    {
		log_error("  tid:%d error uid:%d not in seat.\n",  tid, player->uid);
		for (int i = 0; i < m_GAME_PLAYER; i++)
		{
			if (m_SeatPlayer[i].player != NULL)
			{
				log_info(" uid:%d, seatid:%d, roomid:%d ", 
					m_SeatPlayer[i].player ? m_SeatPlayer[i].player->uid : 0, m_SeatPlayer[i].seatid, tid);
			}
		}
		return -1;
    }
	if (m_bGameState != GAME_FREE || m_room_status != 0)
	{
         //player->down_tag = (player->down_tag == 0) ? 1:0;
		 proto::login::AckAheadDownTable stHeadDownTable;
		 stHeadDownTable.set_uid(player->uid);
		 stHeadDownTable.set_seatid(player->seatid);
		 stHeadDownTable.set_down_tag(player->down_tag);

		unicast(SERVER_AHEAD_DOWNTABLE_UC, player, stHeadDownTable);
		log_info(" round down tid:%d uid:%d money:%d state [%d]",  tid, player->uid, player->money, m_bGameState);
		return 0;
	}
	proto::login::AckDownTableSuccess stDownTableSuccess;
	stDownTableSuccess.set_uid(player->uid);
	stDownTableSuccess.set_seatid(player->seatid);
	stDownTableSuccess.set_money(player->money);
	player->down_tag = 0;

	broadcast(SERVER_DOWNTABLE_SUCC_BC, stDownTableSuccess);

	if (m_room_status == 0)
	{
		Stand_Up(player);
	}
	else
	{
		Player_Leave(player);
	}

	player->clear();
	log_info(" succ tid:%d uid:%d money:%d players:%d ",  tid, player->uid, player->money, m_mapTable_Player.size());
	
	return 0;
} 

//游戏开始基类的

void Table::GameStart()
{  
	if (m_bGameState != GAME_FREE)
	{
		log_error("game start error, now tid:%d gamestate is playing... ", tid);
		return;
	}	

	//初始化数据，庄家椅子id不能再被初始化
	InitData();	
	m_Start_Shai = m_GameLogic.GetShaizi(2);  //复用  摇骰子摸牌 。。
	InitBanker();   // 选庄家 ！！！

	log_info("tid:%d round:%d Banker uid:%d chairid:%d player[%d, %d, %d, %d] " ,
		tid, m_nPlayingTimes, GetPlayerUid(m_bBanker), m_bBanker,
		GetPlayerUid(0), GetPlayerUid(1), GetPlayerUid(2), GetPlayerUid(3));

	save_video_palyer_info();
	save_game_scene_info();

	
    //发送游戏开始协议
	proto::game::AckGameStart stGamestart;
	stGamestart.set_chairid(m_bBanker);
	stGamestart.set_ischangbanker(m_last_Banker == INVALID_CHAIRID ? 0 : m_last_Banker == m_bBanker ? 0 : 1);

	if (m_Start_Shai.Num > 0)
	{
		stGamestart.add_touziinfo( m_Start_Shai.Shai[0] + 1);  //  筛子打出来的是 0-5  所以这个地方。要+1 
		stGamestart.add_touziinfo( m_Start_Shai.Shai[1] + 1);
	}

	broadcast(SERVER_GAME_START, stGamestart);
    save_video_gamestar();
    //获得牌池牌
    
	m_bGameState = GAME_PLAYING;
	//如果开启好牌
	if ( zjh.conf["tables"]["fix"].asInt())
    {			 
		//m_CardPool.FixCard( 1); 		
		m_CardPool.FixCard_ReadJson();
    }
    else  
    {	
		m_CardPool.InitPool();		
    }

	for (int i = 0; i < m_GAME_PLAYER; i++)
	{
		if (m_SeatPlayer[i].player != NULL)
		{
			m_SeatPlayer[i].player->status = GAME_PLAYING;
		}
	}
	// 发牌 
	Send_Hand_Card();

}

//玩家抓牌，， 基类的   chairid  .. 抓牌这id 。  bGetlast ..是否是从最后一张摸牌。。一般。。杠是从最后摸牌。 

void Table::OnUserGrapCard(_uint8 ChairID, bool bGetLast)
{
	if (ChairID< 0 || ChairID>= m_GAME_PLAYER)
	{
		log_error("error %d" , ChairID); 
		return;
	}
	if (m_bDeleteTable)
	{
		log_error("m_bDeleteTable msg !!");   // 解散桌子 ！！！
		return;
	}
		
	if (CardPoolIsEmpty())  // 牌摸完了。！！ 流 局。。
	{
		log_info("liuju over !!");
		GameEnd();
		return;
	}
	memset(&m_bUserOperate, 0, sizeof(m_bUserOperate));
	m_bCurChairID = ChairID;
	m_bGrapId = ChairID;   // 重连的时候 用到。 

	_uint8 bGetCard = bGetLast? m_CardPool.GetLast() : m_CardPool.GetOneCard();

	m_bGetCardCount[ChairID]++;
	m_PlayGrapCard[ChairID] = bGetCard;  

	m_bCurCard = bGetCard;
	MJ_major major;

	//  自己摸牌。 判断可以进行的操作 
	m_bUserOperate[ChairID] = CheckPlaySlefGrpCardCanOperateCode(ChairID, bGetCard,  major, m_HuFanNums[ChairID]);

	m_GameLogic.AddCard(&m_HandCard[ChairID], bGetCard);

	log_info("tid[%d] uid[%d]  sid[%d]   grap_card[%x]  optype[%x]  cardpoollen:%d ",
		tid, GetPlayerUid(ChairID), ChairID, bGetCard, m_bUserOperate[ChairID], m_CardPool.Count());

	save_user_get_card(ChairID, bGetCard, m_CardPool.Count());
	
	//广播玩家摸牌	
	proto::game::AckUserGetCard stUsergetcard;
	for (int i = 0; i < m_GAME_PLAYER; i++)
	{
		if (m_SeatPlayer[i].player == NULL)
		{
			log_error("OnUserGrapCard chairid:%d  error, one player is NULL ", ChairID);
			continue;
		}
		Proto_Game_UserGetCard(i, ChairID, bGetCard, stUsergetcard);
		stUsergetcard.set_optype2(bGetLast ? 1 : 0);
		unicast(SERVER_USER_GETCARD, i, stUsergetcard);	 
		//log_debug("short_info  %s  ", stUsergetcard.ShortDebugString().c_str() );

		proto::game::AckOpNotify stVideo;  // add  
		stVideo.set_chairid(i);
		stVideo.set_optype(m_bUserOperate[i]);
		std::string str = stVideo.SerializeAsString();
		save_video_data(SERVER_OPERATE_NOTIFY, str);  
		
	}
	
	if (m_bUserOperate[ChairID] == TYPE_NULL || m_bUserOperate[ChairID] == TYPE_LISTEN)
	{		
		set_out_timer(ChairID);
	}
	else
	{	
		set_operator_timer(ChairID);
	}
}

//////玩家出牌

void Table::OnUserOutCard(_uint8 ChairID, _uint8 bCard)
{
	if (ChairID < 0 || ChairID >= m_GAME_PLAYER)
	{
		log_error("error chairid[%d]" , ChairID);
		return;
	}	
    // 删出牌时钟	
	ev_timer_stop(zjh.loop, &m_Outcard_Timer);
	log_info("tid:%d uid:%d chairid:%d outcard[%x] ",  tid, GetPlayerUid(ChairID), ChairID, bCard);

    m_bCurChairID = ChairID;
	m_bOutCardChairID = ChairID;
	m_GameLogic.ChuPai(&m_HandCard[ChairID], bCard);
	m_bCurCard = bCard;
	m_bCurOutCard = bCard;
	m_bOverTimeState[ChairID] = 0;
	
	m_stUserOutCard[ChairID].Add(bCard);
	PlayerAllOutCard[ChairID].Add(bCard);
	if ( m_nRecvBaoTingPlayerMap[ChairID] == TYPE_BAO_TING)
		m_bBaoTingOutCount[ChairID]++; 
	
	
    //m_bOutCardCount[CardType(bCard)][CardNum(bCard)] += 1;
    //广播玩家出牌	
	proto::game::AckUserOutCard stUseroutcard;

	Proto_Game_UserOutCard(ChairID, bCard, stUseroutcard);

	broadcast(SERVER_USER_OUTCARD, stUseroutcard);

	save_user_out_card(ChairID, bCard);

}

//////检查玩家出牌 

bool Table::CheckUserOutCard(_uint8 ChairID, _uint8 cbCard)
{
	bool bIsHave = false;
	if(ChairID != m_bCurChairID || cbCard == INVALID_MJ_CARD) 
	{
		log_error(" error cur_ChairID[%d] recv out uid[%d] chairID[%d] card[%x] ", 
			m_bCurChairID, GetPlayerUid(ChairID), ChairID, cbCard);
		Proto_OutCardErr(ChairID, cbCard, 3 ); 
		return bIsHave;
	}

	// 检查此张牌是否在用户手牌里
	_uint8 bChangeLens = m_HandCard[ChairID].ChangeableCardsLen;
	for (_uint8 i = 0; i < bChangeLens; i++)
	{
		if (m_HandCard[ChairID].ChangeableCards[i] == cbCard)
		{
			bIsHave = true;
			break;
		}
	}
	if (!bIsHave)
	{
		log_error("uid:%d seatid:%d outcard error has no card[%x] ",  ChairID, GetPlayerUid(ChairID), cbCard);
		for (_uint8 i = 0; i < bChangeLens; i++)
		{
			log_info("card[%d] [%x] ", i, m_HandCard[ChairID].ChangeableCards[i]);
		}
	}

	return bIsHave;
}

// //出牌后其他玩家操作判断

bool Table::EstimateUserRespond(_uint8 bOutChairID, _uint8 bOutCard)
{
	m_bMultiOpUserNum = 0;    //可以操作的人数
	m_bMultiOpHuNum = 0;
	bool bIsChangeUser = true;//是否轮到下一玩家摸牌
    
	memset(m_bUserOperate, TYPE_NULL, sizeof(m_bUserOperate));
	for (int i = 0; i < m_GAME_PLAYER; i++)
	{
		if (i == bOutChairID)  // 出牌的玩家。。
		{
			m_bUserOperate[i] = TYPE_NULL;
			continue;
		}
		//牌型判断	一个人打牌。后，其他人。可以执行的操作 ！！ 
		MJ_major major;		
		m_bUserOperate[i] = CheckOutCardOtherPlayerCanOpCode(i, bOutCard, major, m_HuFanNums[i]);
			
		if (m_bUserOperate[i] != TYPE_NULL)
		{
			if (m_bUserOperate[i] & TYPE_HU )
			{
				//m_bAddFanFlag[i] = true;
				m_bMultiOpHuNum++;				
			}
			m_bMultiOpUserNum++;
			bIsChangeUser = false;
			//准备向四个玩家发送操作提示

			for (int j = 0; j < m_GAME_PLAYER; j++)
			{
				proto::game::AckOpNotify stNotify;		
				Proto_Game_OpNotify(i, j, bOutCard, stNotify);
				unicast(SERVER_OPERATE_NOTIFY, j, stNotify);	 
				
				proto::game::AckOpNotify stVideo;
				stVideo.set_chairid(j); 
				stVideo.set_optype(m_bUserOperate[j]); 
				std::string str = stVideo.SerializeAsString();
				save_video_data(SERVER_OPERATE_NOTIFY, str);  

			}
		}
		if (m_bUserOperate[i] != 0)
		{
			log_info(" uid[%d] chairid[%d] Operate[%x]  totalop[%d] ",  GetPlayerUid(i), i, m_bUserOperate[i], m_bMultiOpUserNum);
		}		
	}	
	
	if (bIsChangeUser)
	{		
		log_debug("bcurchairid[%d]  bOutChairID[%d]", m_bCurChairID, bOutChairID);

		m_bCurChairID = NextChair(bOutChairID);
		memset(m_bAddFanFlag, 0, sizeof(m_bAddFanFlag));
		OnUserGrapCard(m_bCurChairID);
	}
	else 
	{
		//开启操作时钟 TODO		
		for (_uint8 c = NextChair(bOutChairID); c != bOutChairID; c = NextChair(c))
		{			
 			if (m_bUserOperate[c] != TYPE_NULL)
 			{
 				set_operator_timer(c);
 			}			
		}
	}

	return bIsChangeUser;
}

int Table::deal_hu(_uint8  ChairID)
{
	if(  ChairID >= m_GAME_PLAYER)
		return 0;
	int ret = UserOperateHu(ChairID);
	if (ret < 0)
	{
		log_error("ret %d" ,ret);
	}
	else
		GameEnd();

	return 0;
}


////用户吃碰刚。。操作的


void Table::DoSimpleOperate(_uint8 ChairID, int iOpType, _uint8 bCard)
{	
	int outall_ = PlayerAllOutCard[0].Size() + PlayerAllOutCard[1].Size() + PlayerAllOutCard[2].Size() + PlayerAllOutCard[3].Size();
	log_info("uid[%d] sid[%d]  iOptype[%x]  outall_[%d] m_nGameRunState[%d] ", 
		GetPlayerUid(ChairID), ChairID, iOpType, outall_, m_nGameRunState);

	if (outall_ == 0 && m_nGameRunState == 0  && (iOpType & TYPE_PASS || iOpType & TYPE_HU ))
		return OnRecvQishouHuBaiPai_Operate(ChairID, iOpType);      // 摆牌操作 	

	if (outall_ == 0 && m_nGameRunState == GAME_QISHOU_BAOTING_SEL && (iOpType & TYPE_BAO_TING || iOpType & TYPE_PASS ))
		return  recv_client_baoting_sel(ChairID , iOpType );
	
	m_nGameRunState = GAME_QISHOU_BAOTING_OVER;            // 报听结束 !!
	switch (iOpType)
	{
		case TYPE_PENG:
			UserOperatePeng(ChairID, bCard);
			break;
		case TYPE_WANGANG:
			if (m_conf.m_bQiangGangHu && RobGangHu(ChairID, bCard, iOpType))			
				return;
		case TYPE_ANGANG:		
			if (m_conf.m_bQiangAnGang && RobGangHu(ChairID, bCard, iOpType))
				return;
		case TYPE_ZHIGANG:
			if (m_conf.m_bQiangZhiGang && RobGangHu(ChairID, bCard, iOpType))
				return;
			do
			{ 
                UserOperateGang(ChairID, iOpType, bCard);
				OnUserGrapCard(ChairID, true);

			} while (0);
			break;
		case TYPE_LEFT_CHI:
		case TYPE_CENTER_CHI:
		case TYPE_RIGHT_CHI:
			UserOperateChi(ChairID, iOpType, bCard);			
			break;
		

		case TYPE_PASS:
			UserOperatePass(ChairID, iOpType, bCard);
			//如果自摸，则轮到他出牌
			if (m_bCurChairID == ChairID)
			{
				//开启出牌时钟
				set_out_timer(ChairID);

			}
			else
			{				

				if ( m_bCurChairID == INVALID_CHAIRID)
				{
					OnUserGrapCard(m_bBanker);
				}
				else
				{
					m_bCurChairID = NextChair(m_bCurChairID);
					OnUserGrapCard(m_bCurChairID);
				}
			}
			break;
		case TYPE_HU:		
			deal_hu(ChairID);
			break;		
		default:
			break;
	}	
}

//////检查用户操作是否合法

bool Table::CheckUserOperator(_uint8 ChairID, int iOpType, _uint8 bCard)
{

	if (ChairID >= m_GAME_PLAYER)
	{
		log_error(" error chairid:%d iOpType: %x ",  ChairID, iOpType);
		return false;
	}
	int outall_ = PlayerAllOutCard[0].Size() + PlayerAllOutCard[1].Size() + PlayerAllOutCard[2].Size() + PlayerAllOutCard[3].Size();

	if (outall_ == 0  && ( (iOpType & TYPE_PASS) ||  (iOpType& TYPE_HU) || (iOpType & TYPE_BAO_TING )) )
	{
		log_debug("qisho hu all_out[%d] opcode[%x]" , outall_ ,  iOpType );
		return true;  // 特殊处理 
	}
	if ((iOpType != TYPE_NULL && !(m_bUserOperate[ChairID] & iOpType) && iOpType != TYPE_PASS)
		|| (iOpType == TYPE_PASS && m_bUserOperate[ChairID] == TYPE_NULL))
	{
		log_error("%s error uid:%d chairid:%d iOpType: %x m_bUserOperate: %x ",
			__FUNCTION__, GetPlayerUid(ChairID), ChairID, iOpType, m_bUserOperate[ChairID]);
		return false;
	}
	if (Is_ChiPengGang(iOpType) && (bCard == 0x0 || bCard == 0xff))
	{
		log_error("%s error uid:%d chairid:%d iOpType[%x] card[%x] ",
			__FUNCTION__, GetPlayerUid(ChairID), ChairID, iOpType, bCard);
		return false;
	}
	if ((iOpType != TYPE_PASS) && ( iOpType & (TYPE_CHI | TYPE_PENG)) && (bCard != m_bCurOutCard))
	{
		log_error("%s error uid:%d chairid:%d iOpType:0x%x card %x outcard: %x ", 
			__FUNCTION__, GetPlayerUid(ChairID), ChairID, iOpType, bCard, m_bCurOutCard);
		return false;
	}
	if (m_bUserOperate[ChairID] == TYPE_NULL)
	{
		log_error("%s error uid:%d chairid:%d iOpType:0x%x m_bUserOperate:0x%x\n",
			__FUNCTION__, GetPlayerUid(ChairID), ChairID, iOpType, m_bUserOperate[ChairID]);
		return false;
	}
	if (m_bRobWanGang && ChairID == m_WangGangPlayer)
	{
		log_error("%s rob gang error uid:%d chairid:%d iOpType:0x%x m_bUserOperate:0x%x\n", 
			__FUNCTION__, GetPlayerUid(ChairID), ChairID, iOpType, m_bUserOperate[ChairID]);
		return false;
	}
	return true;
}

void Table::OnRecvUserOperate(_uint8 ChairID, int iOpType, int iCard)
{
	_uint8 bCard = (_uint8)iCard;
	log_debug("op_sid[%d] uid[%d] optype[%x] card[%x]  m_bUserOperate[%x] " ,
		ChairID , GetPlayerUid(ChairID) ,iOpType , bCard , m_bUserOperate[ChairID]);

	if ( !CheckUserOperator(ChairID, iOpType, bCard)) // 检查操作是否合法 ！！
	{
		log_error("error !!  sid  type  card %d %x %x" , ChairID , iOpType , bCard );
		return;
	}
	
	stop_operator_timer(ChairID);     // 停止定时器 
	m_bOverTimeState[ChairID] = 0;  // 0表示没有超时，1表示出牌超时，2表示操作超时	
    //如果是多操作
    if (m_bMultiOpUserNum > 1)   // 多个人，，可以操作如 A 吃 b 碰  c hu 
    {
		DoMultiOperate(ChairID, iOpType, bCard);
    }
    else 
    {   
		if (m_bRobWanGang)  //抢杠胡操作优先级
		{
			RobGangPriority(ChairID, bCard, iOpType);
			return ;
		}
		DoSimpleOperate(ChairID, iOpType, bCard);
		//判断是下一玩家摸牌，或者当前玩家出牌		
    }
//	log_debug("deal   over !!!! "); 

}

void Table::BroadcastOperateResult(_uint8 bOpChairID, int iOpType, _uint8 bOpCard)
{
    //广播发送操作结果	
	proto::game::AckBcOpResult stOpresult;
	stOpresult.set_chairid(bOpChairID);
	stOpresult.set_outcardchairid(m_bCurChairID);
	stOpresult.set_optype(iOpType);
	stOpresult.set_card(bOpCard);

	SetRobGangHu_Flag(bOpChairID, iOpType, stOpresult);
	
	broadcast(SERVER_OPERATE_RESULT, stOpresult);
	//log_info("%s uid:%d chairid:%d outchairid:%d optype:0x%x card:0x%02x",  
	// log_debug("over !!");

	// __FUNCTION__, GetPlayerUid(bOpChairID), bOpChairID, m_bCurChairID, iOpType, bOpCard);
    
}


int  Table::UserOperateGang(_uint8 ChairID, int iOpType, _uint8 bCard)
{
	if (ChairID < 0 || ChairID >= m_GAME_PLAYER)
		return   0; 
	memset(m_bAddFanFlag, 0, sizeof(m_bAddFanFlag));
	memset(m_bUserOperate, 0, sizeof(m_bUserOperate));
	log_info(" uid[%d] ChairID[%d]  iOpType[%x]  bCard[%x]  gang", GetPlayerUid(ChairID), ChairID, iOpType, bCard);	

	int iGangScore[GAME_PLAYER] = {0};
	_uint8 bTempChairID = INVALID_CHAIRID;
	m_bAddFanFlag[ChairID] = true;
	if (m_stGang[ChairID].Gang.Size() >= 4)
	{
		log_error("gang !!");
		return 0;
	}
		 

	switch (iOpType)
	{
		case TYPE_ANGANG:
			{
				for (_uint8 c = NextChair(ChairID); c != ChairID; c = NextChair(c))
				{
					// 这个地方分数是 0  
					iGangScore[ChairID] += m_BaseScore * m_conf.m_AnGangPoint;
					iGangScore[c]       -= m_BaseScore * m_conf.m_AnGangPoint;
					log_debug("chairid[%d] c[%d] point[%d]" , ChairID , c  , m_conf.m_AnGangPoint);    // 暗杠
				}
				m_stGang[ChairID].Gang.Add(tagGangScore(bCard, ChairID, TYPE_ANGANG));				
				bTempChairID = ChairID;
			}
			break;
		case TYPE_WANGANG:
			{	
				
				for (_uint8 c = NextChair(ChairID); c != ChairID; c = NextChair(c))
				{
					iGangScore[ChairID] += m_BaseScore * m_conf.m_WanGangPoint;
					iGangScore[c] -=  m_BaseScore * m_conf.m_WanGangPoint;				
					log_debug("chairid[%d] c[%d] point[%d]", ChairID, c, m_conf.m_WanGangPoint);  //  补杠
				}
				m_stGang[ChairID].Gang.Add(tagGangScore(bCard, ChairID, TYPE_WANGANG));
				bTempChairID = ChairID;				
			}
			break;
		case TYPE_ZHIGANG:
			{
				iGangScore[ChairID]           +=  m_BaseScore * m_conf.m_ZhiGangPoint;
				iGangScore[m_bOutCardChairID] -=  m_BaseScore *  m_conf.m_ZhiGangPoint;                			
				m_stGang[ChairID].Gang.Add(tagGangScore(bCard, m_bOutCardChairID, TYPE_ZHIGANG));
								
				bTempChairID = m_bOutCardChairID;
				m_PlayGrapCard[ChairID] = 0;   // 直杠 。

				m_bGetCardCount[ChairID]++;
				//从打出牌删掉这张牌
				m_stUserOutCard[m_bOutCardChairID].RemoveLast(bCard);
			}
			break;
		default:
			log_error("UserOperateGang, wrong optype:%d \n", iOpType);
			break;
	}
	

	m_GameLogic.ExecuteGang(&m_HandCard[ChairID], bTempChairID, bCard, iOpType);
		
	Calc_Score(m_stGang, iGangScore);

	proto::game::AckBcOpResult stOpresult;
	Proto_Game_OperateGang(ChairID, bCard, iOpType, iGangScore, stOpresult);

	broadcast(SERVER_OPERATE_RESULT, stOpresult);

	
	m_bUserOperate[ChairID] = TYPE_NULL;
	SaveVideoData(SERVER_OPERATE_RESULT, stOpresult);
	return  0;


}

void Table::UserOperatePass(_uint8 ChairID, int iOpType, _uint8 bCard)
{
	log_info(" uid:%d ChairID:%d, iOpType:%x, bCard: %x ",  GetPlayerUid(ChairID), ChairID, iOpType, bCard);

	BroadcastOperateResult( ChairID, iOpType, bCard);
	if (ChairID != m_bCurChairID)
	{
		m_bAddFanFlag[ChairID] = false; 		
		m_bUserOperate[ChairID] = TYPE_NULL; 
	}
//	log_debug("over  this 1111111111111111");
}

int  Table::UserOperateHu(_uint8 ChairID, _uint8 bHuCard)
{
	log_info(" uid:%d ChairID:%d hu FanNum:%d   bHuCard[%x]",  GetPlayerUid(ChairID), ChairID, m_HuFanNums[ChairID] , bHuCard );
	_uint8 bCard = 0xFF;
	if (bHuCard == 0xFF)
	{
		bCard = m_bCurCard;
	}
	else
	{
		bCard = bHuCard;
	}
	
	BroadcastOperateResult(ChairID, TYPE_HU, bCard);  
	if (!(m_bUserOperate[ChairID] & TYPE_HU))
	{
		log_error("hu error, hu chairid:%d, m_bUserOperate[%d]:%d  ", ChairID, ChairID, m_bUserOperate[ChairID]);
		return -1;
	}	
	
	m_bIsHu[ChairID] = true;	
	int userscore[GAME_PLAYER] = { 0 };

	Calc_Hu(ChairID, bCard, userscore);	

	log_debug("calc_hu over !!");
		
	Calc_Score( m_stHuScore, userscore);
	//log_debug();
	//如果没有保存胡牌信息，重新获取
	
	proto::game::AckUserHu stHu;
	stHu.set_huchairid(ChairID);
	if (m_bCurChairID == ChairID)
	{
		stHu.set_pashuchairid(ChairID);
	}
	else
	{
		stHu.set_pashuchairid(m_bCurChairID);
	}
	for (int i = 0; i < m_GAME_PLAYER; i++)
	{
		stHu.add_score(userscore[i]);		
	}

	broadcast(SERVER_USER_HU, stHu);

	m_bUserOperate[ChairID] = TYPE_NULL;
	
	int nscore[GAME_PLAYER] = {0};
	save_video_operate(TYPE_HU, ChairID, stHu.pashuchairid(), bCard, nscore);
	save_video_hu(ChairID, stHu.pashuchairid(), userscore);
	return  0;

}

int   Table::GetNextBanker()
{
	_uint8  pre_bank = m_bBanker;
	MJ_user_buffer UserID;
	int iCount = MultiHu(UserID);
	if (iCount <= 0)
		return  m_bBanker; 
	if (iCount == 1 && m_bIsHu[m_bBanker])		
		return m_bBanker; 

	else if (iCount > 1)
	{
		int s = 0;
		for (_uint8 c = NextChair(m_bOutCardChairID); c != m_bOutCardChairID ; c = NextChair(c))
		{
			if (s++ > 10)
				return   0; 
			if (m_bIsHu[c])
			{
				if (c == m_bBanker)
					return  c;
				else
					return  NextChair(m_bBanker);				
			}
		}
	}

	else
		return  NextChair(m_bBanker); 

	log_debug("pre next %d %d " , pre_bank , m_bBanker );
	return    m_bBanker; 
}

void Table::GameEnd()
{	
	//摸完最后一张牌才算是结束
	MJ_user_buffer UserID;
	_uint8 is_liu_ju = (MultiHu(UserID) == 0) ? 1 : 0; //结束状态，1表示流局，0表示胡牌结束	 判断胡牌的人数 ！！
	
	//单发结算协议	
	proto::game::AckGameEnd stGameend;
	HuScoreSet hu_score;
	
	if (is_liu_ju)
		Score_GameEndDraw( hu_score, stGameend);	
	else	
		Score_GameEndSucc( hu_score, stGameend);
	
	
	log_info("tid[%d] round[%d] score_ [0] %d,  [1] %d, [2] %d [3] [%d]  liuju[%d]  hupai_playe_size[%d] ",  
		tid, m_nPlayingTimes, hu_score[0].iTotleScore, 
		hu_score[1].iTotleScore, hu_score[2].iTotleScore, hu_score[3].iTotleScore, is_liu_ju, UserID.Size());

	int UserScore[GAME_PLAYER] = { 0 };					//四个玩家输赢总分
	memset(UserScore, 0, sizeof(UserScore));
	for (int i = 0; i < m_GAME_PLAYER; i++)
	{
		stGameend.add_score(hu_score[i].iTotleScore);   // 输赢总分。  		
		proto::game::HandCards *pHandcard = stGameend.add_usercard();   //手牌 信息
		Fill_HandCard(i, pHandcard); 
		log_info("sid[%d]  uid[%d] iTotleScore[%d] pai[%s]", 
			i, GetPlayerUid(i), hu_score[i].iTotleScore ,   CatHandCard( m_HandCard[i] , i) );  // quan bu shou pai !! 

		UserScore[i] = stGameend.score(i);
		if (m_SeatPlayer[i].player != NULL)
		{
			if (stGameend.score(i) < 0)
			{				
				if (m_SeatPlayer[i].player->money + stGameend.score(i) < 0)
				{					
					UserScore[i] = -m_SeatPlayer[i].player->money;					
				}
			}
		}		
	}

	tagUserScore userscore[GAME_PLAYER];
	for (int i = 0; i < m_GAME_PLAYER; i++)
	{
		if (m_SeatPlayer[i].player != NULL)
		{  
			userscore[i].uid = m_SeatPlayer[i].player->uid;
			userscore[i].score = UserScore[i] ;
			if (UserScore[i]  > 0)
			{
				m_SeatPlayer[i].player->update_info(UserScore[i], 1,1, win_exp);
			}
			else
			{
				m_SeatPlayer[i].player->update_info(UserScore[i],1 , 0, lose_exp);
			}
			UpdateUserInfoToUser(m_SeatPlayer[i].player);
		}
	}

	Proto_GameEndInfo(UserID, stGameend);	

	stGameend.set_endstate(is_liu_ju );	
	for (int i = 0; i < m_GAME_PLAYER; i++)
	{
		Proto_GameEndInfo_GangInfo(i, stGameend);
	}		
	// DealGuiLinJieSuan(stGameend);   // 桂林麻将。add  

	for (int i = 0; i < m_GAME_PLAYER; i++)
	{
		stGameend.set_huscore(m_stHuScore[i].iTotalScore);
		stGameend.clear_score();
		for (int j = 0; j < m_GAME_PLAYER; j++)
		{
			stGameend.add_score(UserScore[j]);			
		}
		unicast(SERVER_GAME_END, i, stGameend);
		//log_debug("short_info  %s  ", stGameend.ShortDebugString().c_str());  
		log_info("uid[%d] sid[%d] score[%d] ",  GetPlayerUid(i), i, UserScore[i]);
	}
	
	
 	//发送录像
	save_video_game_end(stGameend);
	int pre_bank = m_bBanker;

	if ( pre_bank ==  GetNextBanker() )
		update_user_info_to_datasvr_dz(userscore, 1102, SERVER_DZ_UPDATE_USER_INFO_LITTLE);
	else
		update_user_info_to_datasvr_dz(userscore, 1102); 
	
	ev_timer_again(zjh.loop, &send_videodata_timer);

	CalcRecordInfo(UserID, UserScore);

	RedisUpdateScoreToServer(UserScore);  // 保存分数到redis 

	m_last_Banker = m_bBanker;  // 上一局。做庄的人是谁 ！！

	TurnBanker();

	//初始化数据
	GameEndReset();

}

void Table::GameEndReset()
{  

	memset(m_stGang, 0, sizeof(m_stGang));
	memset(m_stHuScore, 0, sizeof(m_stHuScore));
	memset(m_bIsHu, 0, sizeof(m_bIsHu));
	memset(m_nCanBaoTingArr, 0, sizeof(m_nCanBaoTingArr));
	memset(PlayerAllOutCard, 0, sizeof(PlayerAllOutCard));	
	memset( m_CanBaiPaiArr, 0, sizeof(m_CanBaiPaiArr));		
	m_canBankCanOpCard = true;

	memset( m_PlayGrapCard, 0, sizeof(m_PlayGrapCard)); 

	for (int i = 0; i < m_GAME_PLAYER; i++)
	{
		m_louhu[i].Unlock();
	}
	for (int i = 0; i < m_GAME_PLAYER; i++)
	{
		m_Peng[i].Clear();
	}
	m_niao.Clear();
	m_zhong_niao.Clear();
	m_DaNiaoID = INVALID_CHAIRID;
	m_bGameState = GAME_FREE;

	m_bRobWanGang = false;
	m_bRobGangHuJudge = false;
	m_WangGangPlayerOpType = TYPE_NULL;
	m_PassHuPlayer.Clear();
	for (int i = 0; i < m_GAME_PLAYER; i++)
	{
        if (m_SeatPlayer[i].player == NULL)
        {
			continue;
        }
		m_SeatPlayer[i].player->status =  SIT_DOWN;		
		{
		    m_SeatPlayer[i].action = ACTION_SIDOWN;
		}
	}

	log_info(" player uid:[%d,%d,%d,%d] game reset ",
		GetPlayerUid(0), GetPlayerUid(1), GetPlayerUid(2), GetPlayerUid(3));
	
	memset(&m_HandCard, 0, sizeof(m_HandCard));
	ev_timer_stop(zjh.loop, &m_Outcard_Timer);
	ev_timer_stop(zjh.loop, &m_Operate_Timer0);
	ev_timer_stop(zjh.loop, &m_Operate_Timer1);
	ev_timer_stop(zjh.loop, &m_Operate_Timer2);
	ev_timer_stop(zjh.loop, &m_Operate_Timer3);

	m_nGameRunState = 0; 
	m_nAllRound = 0;
	m_nCurRound = 0; 
	memset(m_bUserOperate, 0, sizeof(m_bUserOperate));

}


void Table::UserOutCard_Timer_cb(struct ev_loop *loop, struct ev_timer *w, int revents)
{
	Table *table = (Table*)w->data;
	if (table)
	{		
		ev_timer_stop(zjh.loop, &table->m_Outcard_Timer);
		table->m_bOverTimeState[table->m_bCurChairID] = proto::game::OVER_TIME_OUTCARD;
		table->UserOvertime_State(table->m_bCurChairID);		
		table->m_totalrecordinfo.timerout_op[table->m_bCurChairID]++;
	}
}



void Table::UserChooseOperate_Timer_cb0(struct ev_loop *loop, struct ev_timer *w, int revents)
{
	Table *table = (Table*)w->data;
	if (table)
	{
		log_info("uid:%d Chairid:0 choose timer out... ", table->GetPlayerUid(static_cast<_uint8>(0)));
		ev_timer_stop(zjh.loop, &table->m_Operate_Timer0);
		if (table->m_bUserOperate[0] == TYPE_NULL)
		{
			log_info("chairid 0 operate timer is on, but it have not operate \n");
			return;
		}
		table->m_bOverTimeState[0] = proto::game::OVER_TIME_OPERATE;
		table->UserOvertime_State(0);		
		table->m_totalrecordinfo.timerout_op[0]++;
	}
}

void Table::UserChooseOperate_Timer_cb1(struct ev_loop *loop, struct ev_timer *w, int revents)
{
	Table *table = (Table*)w->data;
	if (table)
	{
		log_info("uid:%d Chairid:1 choose timer out... ", table->GetPlayerUid(static_cast<_uint8>(1)));
		ev_timer_stop(zjh.loop, &table->m_Operate_Timer1);
		if (table->m_bUserOperate[1] == TYPE_NULL)
		{
			log_info("chairid 1 operate timer is on, but it have not operate \n");
			return;
		}
		table->m_bOverTimeState[1] = proto::game::OVER_TIME_OPERATE;
		table->UserOvertime_State(1);		
		table->m_totalrecordinfo.timerout_op[1]++;
	}
}

void Table::UserChooseOperate_Timer_cb2(struct ev_loop *loop, struct ev_timer *w, int revents)
{
	Table *table = (Table*)w->data;
	if (table)
	{
		log_info("uid:%d Chairid:2 choose timer out...\n", table->GetPlayerUid(static_cast<_uint8>(2)));
		ev_timer_stop(zjh.loop, &table->m_Operate_Timer2);
		if (table->m_bUserOperate[2] == TYPE_NULL)
		{
			log_info("chairid 2 operate timer is on, but it have not operate \n");
			return;
		}
		table->m_bOverTimeState[2] = proto::game::OVER_TIME_OPERATE;
		table->UserOvertime_State(2);		
		table->m_totalrecordinfo.timerout_op[2]++;
	}
}

void Table::UserChooseOperate_Timer_cb3(struct ev_loop *loop, struct ev_timer *w, int revents)
{
	Table *table = (Table*)w->data;
	if (table)
	{
		log_info("uid:%d Chairid:3 choose timer out...\n", table->GetPlayerUid(static_cast<_uint8>(3)));
		ev_timer_stop(zjh.loop, &table->m_Operate_Timer3);
		if (table->m_bUserOperate[3] == TYPE_NULL)
		{
			log_info("chairid 3 operate timer is on, but it have not operate \n");
			return;
		}
		table->m_bOverTimeState[3] = proto::game::OVER_TIME_OPERATE;
		table->UserOvertime_State(3);		
		table->m_totalrecordinfo.timerout_op[3]++;
	}	
}

// 用户准备 消息 

void Table::User_Ready(_uint8 ChairID, int action)
{
	if (m_SeatPlayer[ChairID].player == NULL)
	{
		return;
	}
	if (m_SeatPlayer[ChairID].action == proto::game::ACTION_READY)
	{
		return;
	}
	m_SeatPlayer[ChairID].action = proto::game::ACTION_READY;
	m_SeatPlayer[ChairID].player->status = READY;
	_uint8 bReadyNum = 0;

	proto::game::AckUserReady stReady;
	for (int i = 0; i < m_GAME_PLAYER; i++)
	{
		int nready = m_SeatPlayer[i].action == proto::game::ACTION_READY ? 1 : 0;
		stReady.add_ready(nready);
		if (nready)
		{
			bReadyNum++;
		}
	}
	log_info("uid[%d] sid[%d] r_num[%d] all[%d] times[%d]", GetPlayerUid(ChairID), ChairID, bReadyNum, m_GAME_PLAYER, m_nPlayingTimes);

	broadcast(SERVER_USER_READY, stReady);
	if (m_nCurRound >= m_nAllRound && m_nAllRound> 0 )
	{
		proto::game::AckGameStartFail stGameStartFail;
		stGameStartFail.set_vid(vid);
		char strroomid[128] = { 0 };
		sprintf(strroomid, "%d", tid);
		stGameStartFail.set_roomid(strroomid);
		stGameStartFail.set_can_start(0);
		stGameStartFail.set_code( 20105 );
		int ntime = (int)time(NULL);
		stGameStartFail.set_ts(ntime);
		unicast(SERVER_DZ_GAME_START_RES, ChairID ,   stGameStartFail); 

		return;
	}

	if (bReadyNum == m_GAME_PLAYER)
	{
		m_nPlayingTimes++;
		game_start_req();
		TIMER_AGAIN(req_start_timer, req_start_timer_stamp);
	}	
}

void Table::recv_client_action(Client *client, Player *player)
{
	if (client == NULL)
	{
		log_info("RecvClientAction *client is NULL \n");
		return;
	}
	if (player == NULL)
	{
		log_info("RecvClientAction  player is NULL \n");
		return;
	}
	_uint8 ChairID = player->seatid;
	if (ChairID >= m_GAME_PLAYER || ChairID < 0)
	{
		log_error("RecvClientAction, but chaird:%d is wrong  charid[%d] ", player->seatid , ChairID );
		return;
	}
	proto::game::ReqAction stAction;
	stAction.ParseFromArray(client->m_Buffer->proto_data, client->m_Buffer->proto_head.length);
	int action = stAction.action();
	switch (action)
	{
		case proto::game::ACTION_READY:
		{
			User_Ready(ChairID, action);
		}
		break;
		default:
			break;
	}
}

//报听选择。

void Table::recv_client_baoting_sel(_uint8 sid   , int op_code  )
{

	Player  *player = GetPlayer(sid);
	Client  *client = player->client;
	if (client == NULL || player == NULL)
	{
		log_error("client player null");
		return;
	}

	if (player->seatid >= m_GAME_PLAYER || player->seatid < 0)
	{
		log_error(" but uid:%d chaird:%d is wrong ", player->uid, player->seatid);
		return;
	}
	
	if (op_code & TYPE_BAO_TING )
	{
		proto::game::hunanmj::serMsgBaojiao  pbAll;
		pbAll.set_uid(player->uid);
		pbAll.set_chairid(player->seatid);
		broadcast( SERVER_BAO_TING_SUCC, pbAll);
		log_debug("baoting  uid[%d]" , player->uid);
	}
	OnRecvBaoTingOp( sid, op_code);

}


void Table::Proto_OutCardErr(_uint8  chairID , _uint8  card  , int err_code )
{
	proto::game::OutCardErr   Ack;
	Ack.set_card(card); 
	Ack.set_chairid(chairID); 
	Ack.set_err_code(err_code); 
	unicast(SER_CMD_OUT_CARD_ERR , chairID ,  Ack);

	
}
//客户端命令。出牌

void Table::recv_client_outcard(Client *client, Player *player)
{
	if (player->seatid >= m_GAME_PLAYER || player->seatid < 0)
	{
		log_error(" but uid:%d chaird:%d is wrong ",   player->uid, player->seatid);
		return;
	}
	int bOutChairID = player->seatid;
	proto::game::ReqOutCard stOutCard;
	stOutCard.ParseFromArray(client->m_Buffer->proto_data, client->m_Buffer->proto_head.length);	
	_uint8 bOutCard = stOutCard.card();

	log_info(" uid[%d] sid[%d] outcard[%x] ",  player->uid, bOutChairID, bOutCard);

	if (m_bDeleteTable || m_bIsDisbandTable )
	{
		log_error("%d %d ", m_bDeleteTable, m_bIsDisbandTable); 
		Proto_OutCardErr( bOutChairID , bOutCard ,  3) ;
		return;
	} 

	//检查玩家打出的牌
	if ( CheckUserOutCard( bOutChairID, bOutCard))
	{
		OnUserOutCard(bOutChairID, bOutCard);
		//出牌后其他玩家操作判断
		EstimateUserRespond(bOutChairID, bOutCard);		

	}	
}


void Table::recv_jilin_majiang_gang_req(Client *client, Player *player)
{
	if (player->seatid >= m_GAME_PLAYER || player->seatid < 0)
	{
		log_error("recv but chaird:%d is wrong", player->seatid);
		return;
	}
	for (int i = 0; i < m_GAME_PLAYER; i++)
	{
		if (m_SeatPlayer[i].player == NULL)
			continue;
		if (m_SeatPlayer[i].player == player && m_SeatPlayer[i].player->client == client)
		{
			goto __FUNC_BEGIN__;
		}
	}
	log_error("%s  uid:%d is wrong \n", __FUNCTION__, client->uid);
	return;
__FUNC_BEGIN__:

	int chairId  = player->seatid; 
	proto::game::jilin::JilinOperateReq   req;
	req.ParseFromArray(client->m_Buffer->proto_data, client->m_Buffer->proto_head.length);
	int  optype = req.type();
	int  card_arr[14] = { 0 };
	int  pos = 0; 
	int  gang_id = 0;
	for (int i = 0; i < req.card_list_size() ; i++)
	{
		card_arr[pos++] = req.card_list(i);
		if (pos>6)
		{
			log_error("error");
			break;
		}
	}
	if (optype == 2)
		gang_id = req.gang_id();


	ProtoJilinGangOperate(chairId, optype, card_arr, pos, gang_id);



}


void Table::recv_ningxiang_fengdong_op(Client *client, Player *player)
{
	if (player->seatid >= m_GAME_PLAYER || player->seatid < 0)
	{
		log_error("recv but chaird:%d is wrong", player->seatid);
		return;
	}
	for (int i = 0; i < m_GAME_PLAYER; i++)
	{
		if (m_SeatPlayer[i].player == NULL)
			continue;
		if (m_SeatPlayer[i].player == player && m_SeatPlayer[i].player->client == client)
		{
			goto __FUNC_BEGIN__;
		}
	}
	log_error("%s  uid:%d is wrong \n", __FUNCTION__, client->uid);
	return;
__FUNC_BEGIN__:

	int chairId = player->seatid;
	proto::game::ningxiang::fengdongreq    req;
	req.ParseFromArray(client->m_Buffer->proto_data, client->m_Buffer->proto_head.length);
	
	ProtoNingXiangFengDong(chairId );   
	

}


//客户端。吃碰刚 操作请求

void Table::recv_client_opeater_req(Client *client, Player *player)
{
	if (player->seatid >= m_GAME_PLAYER || player->seatid < 0)
	{
		log_error("recv but chaird:%d is wrong", player->seatid);
		return;
	}
	for (int i = 0; i < m_GAME_PLAYER; i++)
	{
		if (m_SeatPlayer[i].player == NULL)
			continue;
		if (m_SeatPlayer[i].player == player && m_SeatPlayer[i].player->client == client)
		{
			goto __FUNC_BEGIN__;
		}
	}
	log_error( "%s  uid:%d is wrong \n", __FUNCTION__, client->uid);
	return;
__FUNC_BEGIN__:

	int chairid = player->seatid;
	proto::game::ReqOperateResult stOperate;
	stOperate.ParseFromArray(client->m_Buffer->proto_data, client->m_Buffer->proto_head.length);
	int iOpType = stOperate.type();
	int bCard = stOperate.card();
	
    if (m_bDeleteTable || m_bIsDisbandTable)
    {
		log_error("error %d %d " , m_bDeleteTable  , m_bIsDisbandTable );
		return;
    }
	OnRecvUserOperate(chairid, iOpType, bCard);

}

void Table::RecvClientAutoHu(Client *client, Player *player)
{
	if (client == NULL)
	{
		log_error("user trustee but, client is NULL \n");
		return;
	}

	proto::game::ReqIsAuto stReq;
	stReq.ParseFromArray(client->m_Buffer->proto_data, client->m_Buffer->proto_head.length);

	if (player == NULL)
	{
		log_error("user trustee but, client->player is NULL \n");
		return;
	}

	_uint8 ChairID = player->seatid;
	m_bAutoHu[ChairID] = stReq.isauto() == 0 ? false : true;
	log_info("%s uid:%d chairid:%d autoHu:%d ", __FUNCTION__, player->uid, player->seatid, m_bAutoHu[ChairID]);
}

bool Table::IsListening(_uint8 ChairID, _uint8 bRemoveCard)
{	
	HandCards tempHandCard = m_HandCard[ChairID];
	if (bRemoveCard != INVALID_MJ_CARD)
	{
		m_GameLogic.RemoveCard(&tempHandCard, bRemoveCard);
	}	
	else if (tempHandCard.ChangeableCardsLen % 3 == 2)
	{
		tagListenInfos stListenInfo[14];
		_uint8 bListenLen = 0;
		int iOpType = Table::GetListeningInfo(ChairID, stListenInfo, bListenLen);
		return (iOpType & TYPE_LISTEN);
	}
	_uint8 last_check = INVALID_MJ_CARD;
	MJ_BUFFER<_uint8, POOL_CARD_TYPE_MAX> _relativity;
	for (int i = 0; i < tempHandCard.ChangeableCardsLen; ++i)
	{
		if (last_check == tempHandCard.ChangeableCards[i])
		{
			continue;
		}
		last_check = tempHandCard.ChangeableCards[i];
		GetHandCardRelativity(last_check, _relativity);
	}

	for (_uint8 i = 0; i < _relativity.Size(); ++i)
	{
		m_GameLogic.AddCard(&tempHandCard, _relativity[i]);
		MJ_fan_buffer fan;
		if ( Check_Hu(ChairID, tempHandCard, fan , _relativity[i] ) == TYPE_HU)
		{
			log_info("  uid:%d chairdid:%d Remove: %x " , GetPlayerUid(ChairID), ChairID, bRemoveCard);
			return true;
		}
		m_GameLogic.ChuPai(&tempHandCard, _relativity[i]);
	}
	return false;
}

void Table::Reconnect(Player *player)
{
	if (player == NULL)
	{
		log_error("error");
		return;
	}
	int uid = player->uid;
	log_info("  uid:%d chairid:%d ",  uid, player->seatid);
	std::map<int, Player *>::iterator iter = m_mapTable_Player.find(uid);
	if (iter != m_mapTable_Player.end())
	{		
		iter->second = player;
		
	}
	if (player->seatid < m_GAME_PLAYER && player->seatid >= 0)
	{
		m_SeatPlayer[player->seatid].player = player;
	}
	User_Offline(player, false);
	
}

void Table::UpdateUserInfoToUser(Player *player)
{
	if (player == NULL)
	{
		log_error("update user info to user error, player is NULL \n");
		return;
	}
	log_info(" uid[%d] chairid[%d] money[%d] ",  player->uid, player->seatid, player->money);	
	proto::game::AckUpdateUserInfo stAck;
	stAck.set_chairid(player->seatid);
	stAck.set_uid(player->uid);
	stAck.set_money(player->money);
	stAck.set_rmb(player->rmb);
	broadcast(SERVER_UPDATE_USERINFO, stAck);

	SaveVideoData(SERVER_UPDATE_USERINFO, stAck);
}

void Table::RecvUserTrustee(Client *client)
{
	if (client == NULL)
	{
		log_error("user trustee but, client is NULL \n");
		return;
	}
	
	proto::game::ReqIsAuto stReq;
	stReq.ParseFromArray(client->m_Buffer->proto_data, client->m_Buffer->proto_head.length);
	
	if (client->player == NULL)
	{
		log_error("user trustee but, client->player is NULL \n");
		return;
	}
	
	_uint8 ChairID = client->player->seatid;
	m_bIsTrustee[ChairID] = stReq.isauto() == 0 ? false : true;

	proto::game::AckIsAuto stAck;
	stAck.set_chairid(client->player->seatid);
	stAck.set_isauto(m_bIsTrustee[ChairID]);

	broadcast(SERVER_TRUSTEE, stAck);
	log_info("%s uid:%d chairid:%d trustee:%d ", __FUNCTION__, client->uid, client->player->seatid, m_bIsTrustee[ChairID]);
}

int  Table::Get_Card_Left_Num(_uint8 ChairID, _uint8 cbCard)
{
	int nNum = 0;

	nNum = m_CardPool.Find(cbCard);

	for (int k = 0; k < m_GAME_PLAYER; k++)
	{   
        if (k == ChairID)
        {
			continue;
        }
        
		for (int j = 0; j < m_HandCard[k].ChangeableCardsLen; j++)
		{
			if (cbCard == m_HandCard[k].ChangeableCards[j])
			{
				nNum++;
			}
		}
		if (m_CardPool.GameId == GUILIN_MAJIANG_GAME_ID)
		{
			for (int j = 0; j < m_HandCard[k].FixedCardsLen; j++)
			{
				if (m_HandCard[k].FixedCards[j].state & TYPE_ANGANG && (cbCard == m_HandCard[k].FixedCards[j].CardData))
					nNum += 4;  
			}
		}
	}
	return nNum;
}

// 抢杠胡。在这个地方。判断可以的操作

bool Table::RobGangHu(_uint8 ChairID, _uint8 CardData, int iOpType)
{  
	if (iOpType & TYPE_WANGANG && !m_conf.m_bQiangGangHu)
		return false;
	if (iOpType & TYPE_ZHIGANG && !m_conf.m_bQiangZhiGang)
		return false;
	if (iOpType & TYPE_ANGANG && !m_conf.m_bQiangAnGang)
		return false;	

	_uint8 cbNum = 0;
	for (int i = 0; i < m_GAME_PLAYER; i++)
	{    
		if (i == ChairID)
		{
			continue;
		}		
		MJ_major major;
		m_bRobGangHuJudge = true;
		m_WangGangPlayerOpType = iOpType;
		m_bUserOperate[i] = CheckOutCardOtherPlayerCanOpCode(i, CardData, major, m_HuFanNums[i]);  
		m_bRobGangHuJudge = false;
		m_WangGangPlayerOpType = TYPE_NULL;
		m_bUserOperate[i] &= TYPE_HU;
		if (m_bUserOperate[i] & TYPE_HU)
		{   
			const HandCards& hc = m_HandCard[ChairID];
			MJ_opinfo opinfo;
			
			if (iOpType & TYPE_WANGANG && m_GameLogic.GetWanGang(&hc, opinfo, CardData))
			{
				for (int c = 0; c < m_GAME_PLAYER; c++)
				{
					proto::game::AckOpNotify stNotify;
					Proto_Game_OpNotify(i, c, CardData, stNotify);
					unicast(SERVER_OPERATE_NOTIFY, c, stNotify);

				}
				cbNum++;
			}
			else if (iOpType & TYPE_ZHIGANG && m_GameLogic.GetZhiGang(&hc, opinfo, CardData))
			{
				for (int c = 0; c < m_GAME_PLAYER; c++)
				{
					proto::game::AckOpNotify stNotify;
					Proto_Game_OpNotify(i, c, CardData, stNotify);
					unicast(SERVER_OPERATE_NOTIFY, c, stNotify);

				}
				cbNum++;
			}
			else if (iOpType & TYPE_ANGANG && m_GameLogic.GetAnGang(&hc, opinfo, CardData))
			{
				for (int c = 0; c < m_GAME_PLAYER; c++)
				{
					proto::game::AckOpNotify stNotify;
					Proto_Game_OpNotify(i, c, CardData, stNotify);
					unicast(SERVER_OPERATE_NOTIFY, c, stNotify);

				}
				cbNum++;
			}					
		}
	}
	//开启操作时钟 TODO
	for (int i = 0; i < m_GAME_PLAYER; i++)
	{
		if (i == ChairID)
		{
			continue;
		}
		
		log_info("RobGangHu uid:%d chaird:%d out card, so that chairid:%d have operate: %x ",
			GetPlayerUid(ChairID),ChairID, i, m_bUserOperate[i]);			

		if (m_bUserOperate[i] & TYPE_HU)
		{
			set_operator_timer(i);
		}

	}
	if (cbNum > 0)
	{
		m_RobHuNum = cbNum;
		m_bRobWanGang = true;
		log_debug("m_bRobWanGang [%d]", m_bRobWanGang);

		m_WangGangPlayer = ChairID;
		m_WangGangPlayerOpType = iOpType;
		m_RobWanGangCard = CardData;
		//log_info("robot ganghu, and hu user nums:%d \n", m_RobHuNum);
	}

	return (cbNum > 0);
}

//抢杠胡操作优先级
void Table::RobGangPriority(_uint8 ChairID, _uint8 CardData, int OperateType)
{
	stop_operator_timer(ChairID);

	m_RobOperateNum++;
	m_RobHuDoneOp[ChairID] = OperateType;
	if (TYPE_HU == OperateType)
	{
		m_bRobHu = true;		
		m_bAddFanFlag[m_WangGangPlayer] = true;	
		
		if (m_conf.m_bYiPaoDuoXiang)
		{
			m_bIsHu[ChairID] = true;
			UserOperateHu(ChairID, m_RobWanGangCard);

			if ( m_conf.m_bForceHuQiangGangHu )
			{
				RobGangForceHu(ChairID, m_RobWanGangCard);
			}
			
		}
	}
	else if (OperateType == TYPE_PASS)
	{
		UserOperatePass(ChairID, OperateType, CardData);
		m_PassHuPlayer.Add(ChairID);
	}
	
	log_info(" uid:%d chairid:%d OperateNum:%d RobHuNum:%d ",  GetPlayerUid(ChairID), ChairID, m_RobOperateNum, m_RobHuNum);
	if (m_RobOperateNum == m_RobHuNum)  //所有胡玩家都已经操作完毕
	{
		if (m_bRobHu)   //如果有人胡牌
		{
			if (!m_conf.m_bYiPaoDuoXiang)
			{
				for (_uint8 c = NextChair(m_bCurChairID); c != m_bCurChairID; c = NextChair(c))
				{
					if (m_bUserOperate[c] & TYPE_HU)
					{
						m_bIsHu[c] = true;
						UserOperateHu(c, m_RobWanGangCard);
						break;
					}
				}
			}
			if (m_WangGangPlayerOpType & (TYPE_ANGANG | TYPE_WANGANG))
				m_GameLogic.RemoveCard(&m_HandCard[m_WangGangPlayer], m_RobWanGangCard, 1);			
			GameEnd();
		}
		else
		{
			UserOperateGang(m_WangGangPlayer, m_WangGangPlayerOpType, m_RobWanGangCard);
			OnUserGrapCard(m_WangGangPlayer);
		}

	}
	if (m_RobOperateNum == m_RobHuNum)
	{
		m_bRobWanGang = false;
		m_bRobHu = false;
		m_RobOperateNum = 0;
		m_RobHuNum = 0;
		
		m_WangGangPlayer = INVALID_CHAIRID;
		m_WangGangPlayerOpType = TYPE_NULL;
		m_RobWanGangCard = INVALID_MJ_CARD;
		memset(&m_RobHuDoneOp, 0, sizeof(m_RobHuDoneOp));
		m_PassHuPlayer.Clear();

	}
}



//bool Table::is_all_ready()
//{
//	for (int i = 0; i<m_GAME_PLAYER; i++)
//	{
//		//判断已经坐下的玩家是否都已经准备
//		if (m_SeatPlayer[i].state == Seat::E_Seat_Occupy)
//		{
//			if (m_SeatPlayer[i].action != ACTION_READY)
//			{
//				return false;
//			}			
//		}
//	}
//	return true;
//}
 
void Table::clean_table_user()
{
	std::vector<Player*> temp_del_vec;
	for (int i = 0; i < m_GAME_PLAYER; i++)
	{   
		Player *player = m_SeatPlayer[i].player;
		if(player != NULL &&(NULL == player->client || 
			player->money < stand_money || 1 == player->down_tag))
		{  
			log_info("%s uid:%d client:%p money:%d down_tag:%d ", __FUNCTION__, player->uid, player->client, player->money, player->down_tag);
			temp_del_vec.push_back( player );
		}
	}
	
	for( std::vector<Player*>::iterator iter = temp_del_vec.begin(); iter != temp_del_vec.end(); iter++ )
	{
		handler_downtable( *iter );
		//m_bBanker = INVALID_CHAIR;
	}
	log_info("%s clean user num:%d\n", __FUNCTION__, temp_del_vec.size());
	temp_del_vec.clear();
	for( std::map<int, Player*>::iterator iter = m_mapTable_Player.begin(); iter != m_mapTable_Player.end(); iter++ )
	{
		if( NULL == iter->second->client )
		{   
			log_info("%s uid:%d client is null ", __FUNCTION__, iter->second->uid);
			temp_del_vec.push_back( iter->second );
		}
		iter->second->reset();
	}
	for( std::vector<Player*>::iterator iter = temp_del_vec.begin(); iter != temp_del_vec.end(); iter++ )
	{
		log_info("%s del_player %d\n", __FUNCTION__, (*iter)->uid);
		zjh.game->del_player( *iter );
	}
}

void Table::set_operator_timer(_uint8 ChairID, bool bDelayTimer)
{
	//操作时钟
	switch (ChairID)
	{
	case 0:
		{			
			if (!bDelayTimer)
			{
				ev_timer_stop(zjh.loop, &m_Operate_Timer0);
				ev_timer_set(&m_Operate_Timer0, m_Operate_Timer_stamp0+2, m_Operate_Timer_stamp0+2);
				ev_timer_again(zjh.loop, &m_Operate_Timer0);
			}
			else
			{   
				ev_timer_stop(zjh.loop, &m_Operate_Timer0);
				ev_timer_set(&m_Operate_Timer0, m_bDelayTime, m_bDelayTime);
				ev_timer_again(zjh.loop, &m_Operate_Timer0);
				log_info("operate timer m_bDelayTime:%d \n", m_bDelayTime);
			}

			m_CurTimer = m_Operate_Timer0;
		}
		break;
	case 1:
		{			
			if (!bDelayTimer)
			{
				ev_timer_stop(zjh.loop, &m_Operate_Timer1);
				ev_timer_set(&m_Operate_Timer1, m_Operate_Timer_stamp1+2, m_Operate_Timer_stamp1+2);
				ev_timer_again(zjh.loop, &m_Operate_Timer1);
			}
			else
			{   
				ev_timer_stop(zjh.loop, &m_Operate_Timer1);
				ev_timer_set(&m_Operate_Timer1, m_bDelayTime, m_bDelayTime);
				ev_timer_again(zjh.loop, &m_Operate_Timer1);
				log_info("operate timer m_bDelayTime:%d \n", m_bDelayTime);
			}
			m_CurTimer = m_Operate_Timer1;
		}
		break;
	case 2:
		{			
			if (!bDelayTimer)
			{
				ev_timer_stop(zjh.loop, &m_Operate_Timer2);
				ev_timer_set(&m_Operate_Timer2, m_Operate_Timer_stamp2+2, m_Operate_Timer_stamp2+2);
				ev_timer_again(zjh.loop, &m_Operate_Timer2);
			}
			else
			{   
				ev_timer_stop(zjh.loop, &m_Operate_Timer2);
				ev_timer_set(&m_Operate_Timer2, m_bDelayTime, m_bDelayTime);
				ev_timer_again(zjh.loop, &m_Operate_Timer2);
				log_info("operate timer m_bDelayTime:%d \n", m_bDelayTime);
			}
			m_CurTimer = m_Operate_Timer2;
		}
		break;
	case 3:
		{			
			if (!bDelayTimer)
			{
				ev_timer_stop(zjh.loop, &m_Operate_Timer3);
				ev_timer_set(&m_Operate_Timer3, m_Operate_Timer_stamp3+2, m_Operate_Timer_stamp3+2);
				ev_timer_again(zjh.loop, &m_Operate_Timer3);
			}
			else
			{   
				ev_timer_stop(zjh.loop, &m_Operate_Timer3);
				ev_timer_set(&m_Operate_Timer3, m_bDelayTime, m_bDelayTime);
				ev_timer_again(zjh.loop, &m_Operate_Timer3);
				log_info("operate timer m_bDelayTime:%d \n", m_bDelayTime);
			}
			m_CurTimer = m_Operate_Timer3;
		}
		break;
	default:
		break;
	}
}


void Table::stop_operator_timer(_uint8 ChairID)
{
	switch (ChairID)
	{
	case 0:
		ev_timer_stop(zjh.loop, &m_Operate_Timer0);
		break;
	case 1:
		ev_timer_stop(zjh.loop, &m_Operate_Timer1);
		break;
	case 2:
		ev_timer_stop(zjh.loop, &m_Operate_Timer2);
		break;
	case 3:
		ev_timer_stop(zjh.loop, &m_Operate_Timer3);
		break;
	default:
		break;
	}
}

void Table::set_out_timer(_uint8 ChairID, bool bDelayTimer)
{
	//log_info("start player uid:%d chairid:%d out card timer  \n", GetPlayerUid(ChairID), ChairID);
	if (!bDelayTimer)
	{
		ev_timer_stop(zjh.loop, &m_Outcard_Timer);
		ev_timer_set(&m_Outcard_Timer, m_Outcard_Timer_stamp+2 , m_Outcard_Timer_stamp+2);
		ev_timer_again(zjh.loop, &m_Outcard_Timer);
	}
	else
	{   
		ev_timer_stop(zjh.loop, &m_Outcard_Timer);
		ev_timer_set(&m_Outcard_Timer, m_bDelayTime, m_bDelayTime);
		ev_timer_again(zjh.loop, &m_Outcard_Timer);
		log_info("outcard timer m_bDelayTime:%d  ", m_bDelayTime);
	}
	m_CurTimer = m_Outcard_Timer;
}

void Table::Broadcast_UpdateTimer(_tint8 ChairID)
{
	//广播给客户端超时后的时钟的还剩余多少秒数
	proto::game::AckUpdateTimer stUpdateTimer;
	stUpdateTimer.set_chairid(ChairID);
	int blefttime = ev_timer_remaining(zjh.loop, &m_CurTimer);
	stUpdateTimer.set_lefttime(blefttime);

	broadcast(SERVER_UPDATE_TIMER, stUpdateTimer);

	log_info("%s uid:%d chairid:%d lefttime:%d  ",
		__FUNCTION__, GetPlayerUid(ChairID), stUpdateTimer.chairid(), stUpdateTimer.lefttime());
	
}

 ////////获取操作剩余时间。一般重连会用到

int Table::get_operate_remain_time(int iOpType, bool bReconnnect)
{   
	int nLeftTime = m_Outcard_Timer_stamp;
	if (iOpType == TYPE_NULL)
	{
		if (!bReconnnect)
		{
			nLeftTime = m_Outcard_Timer_stamp;
		}
		else
		{
			nLeftTime = ev_timer_remaining(zjh.loop, &m_CurTimer);
			if (nLeftTime >= m_Outcard_Timer_stamp)
			{
				nLeftTime = m_Outcard_Timer_stamp;
			}
			if (nLeftTime < 0)
			{
				nLeftTime = 0;
			}
			
		}
	}
	else
	{
        if (!bReconnnect)
        {
			nLeftTime = m_Operate_Timer_stamp0;
        }
        else
		{
			nLeftTime = ev_timer_remaining(zjh.loop, &m_CurTimer);
			if (nLeftTime >= m_Operate_Timer_stamp0)
			{
				nLeftTime = m_Operate_Timer_stamp0;
			}
			if (nLeftTime < 0)
			{
				nLeftTime = 0;
			}	
		}
	}
	return nLeftTime;
}


//手牌为14张牌的时候的判断听牌
int Table::GetListeningInfo(_uint8 ChairID, tagListenInfos stListenInfo[14], _uint8& bListenLen)
{
	HandCards &tempHandCard = m_HandCard[ChairID];

	_uint8 bCard = 0xFF;
	memset(stListenInfo, 0, sizeof(tagListenInfos) * 14);
	bListenLen = 0;	
	//log_debug("all_hand_card[%d] cards[%s]" , tempHandCard.ChangeableCardsLen ,  CatHandCard( tempHandCard , ChairID) );
	
	for (int i=0; i< tempHandCard.ChangeableCardsLen; i++)
	{   
		//防止手牌有有对、刻字时，连续对相同的牌做多次的判断胡牌
		//log_debug("bcard %x temp[%x]", bCard , tempHandCard.ChangeableCards[i]); 

		if (bCard == tempHandCard.ChangeableCards[i])
		{
			continue; 
		} 
		bCard = tempHandCard.ChangeableCards[i];
		Check_Listening(&tempHandCard, ChairID, bCard, stListenInfo, bListenLen); 	
	
	
	}
	
	return (bListenLen != 0 ? TYPE_LISTEN : TYPE_NULL);
}


int   Table::GetZhongfabaCount(HandCards  *phc)
{
	int al = 0;
	for (int i = 0; i < phc->ChangeableCardsLen; i++)
	{
		if (phc->ChangeableCards[i] > 0x40)
			al++;
	}
	return al;

}



//
//bool Table::Check_Listening_zhongfabai(HandCards *pHc, _uint8 ChairID, _uint8 bCard, tagListenInfos stListenInfo[14], _uint8 &bListenLen)
//{
//	bool bTrue = false;
//	if (GetZhongfabaCount(pHc) >= 3)
//	{
//		map<int, int>all_map;
//		MJ_fan_buffer fan_buffer;
//		for (int i = 0; i < pHc->ChangeableCardsLen; i++)
//			all_map[pHc->ChangeableCards[i]]++;
//		for (map<int, int>::iterator it = all_map.begin(); it != all_map.end(); ++it)
//		{
//			int  yes = false;
//			int  ret = m_GameLogic.ChuPai(pHc, it->first);
//			for (int j = 1; j <= 3; j++)
//			{
//				_uint8  cur_add_pai = 0x40 + j;
//				int add_rest = m_GameLogic.AddCard(pHc, cur_add_pai); 				
//				if (Check_Hu(ChairID, *pHc, fan_buffer, cur_add_pai) == TYPE_HU)
//				{
//					if (stListenInfo[bListenLen].bHuLen < 18 && bListenLen < 10 )
//					{
//						//log_debug("add True 1111111111 %d" , bListenLen);
//						stListenInfo[bListenLen].bOutCard = it->first;  
//						stListenInfo[bListenLen].HuInfo[stListenInfo[bListenLen].bHuLen].bCard = cur_add_pai;     //_relativity[i];//听的牌
//						stListenInfo[bListenLen].HuInfo[stListenInfo[bListenLen].bHuLen].bFanShu = 1 ;   //Calc_Fan(ChairID, fan_buffer);
//						bTrue = true;				
//						yes = true; 
//						stListenInfo[bListenLen].bHuLen++;						
//					}
//				}
//				if (add_rest == 0  )
//					m_GameLogic.ChuPai(pHc, cur_add_pai);
//			}
//			if (ret)
//				m_GameLogic.AddCard(pHc, it->first); 
//			if(yes)
//				bListenLen++; 
//		}
//	}
//	
//	return bTrue;
//}


void Table::GetHandCardRelativity(_uint8 bCard, MJ_BUFFER<_uint8, POOL_CARD_TYPE_MAX>& _relativity)
{
	_uint8 _start, _end;
	if ((CardType(bCard) == MJ_TYPE_FENG) || (CardType(bCard) == MJ_TYPE_JIAN))
	{
		//字牌
		_start = _end = bCard;
	}
	else
	{
		//玩条同
		if (CardNum(bCard) == 1)
		{
			_start = bCard;
			_end = bCard + 1;
		}
		else if (CardNum(bCard) == 9)
		{
			_start = bCard - 1;
			_end = bCard;
		}
		else
		{
			_start = bCard - 1;
			_end = bCard + 1;
		}
	}
	
	for (_uint8 i = _start; i <= _end; ++i)
	{
		if (_relativity.Size() >= 2)
		{
			if ((_relativity[_relativity.Size() - 1] != i) && (_relativity[_relativity.Size() - 2] != i))
			{
				_relativity.Add(i);
			}
		}
		else if (_relativity.Size() == 1)
		{
			if (_relativity[0] != i)
			{
				_relativity.Add(i);
			}
		}
		else
		{
			_relativity.Add(i);
		}
			
	}
}

void Table::GetAllHandCardRelativity(HandCards *pHc, MJ_BUFFER<_uint8, POOL_CARD_TYPE_MAX>& _relativity)
{
	_uint8 last_check = INVALID_MJ_CARD;
	
	for (int i = 0; i < pHc->ChangeableCardsLen; ++i)
	{
		if (last_check == pHc->ChangeableCards[i])
		{
			continue;
		}
		last_check = pHc->ChangeableCards[i];
		GetHandCardRelativity(last_check, _relativity);
	}
}


bool  Table::check_listening_guilin_xilan(HandCards *pHc, _uint8 ChairID, _uint8 bCard, tagListenInfos stListenInfo[14], _uint8 &bListenLen )
{
	if (m_CardPool.GameId != GUILIN_MAJIANG_GAME_ID)
	{		
		return false;
	}  		
	bool bTrue = false;
	if (ChairID< 0 || ChairID >= m_GAME_PLAYER)
		return 0;
	if (pHc->FixedCardsLen)
		return 0; 
	map<int, int> all_map; 
	int feng = 0;
	for (int i = 0; i < pHc->ChangeableCardsLen; i++)
	{
		int ct = pHc->ChangeableCards[i];
		if (ct> 0x30)
			feng++;
		all_map[ct]++;
		if (all_map[ct]>= 3  )
			return 0;
	}  
	if(feng < 4 ) // modify ..bug .!!
		return 0;
	 
	int  all_card[] =
	{
		0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09,//万	
		0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19,//条	
		0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29,//筒	
		0x31, 0x32, 0x33, 0x34, 0x41, 0x42, 0x43,
	};  	
	m_GameLogic.ChuPai(pHc, bCard); 
	

	bool is_add = false;   
	int   hu_count = 0;
	for (int i = 0; i < 34 ;  i++)
	{   
		is_add = false;
		if (all_map[all_card[i]])
			continue;
		if (pHc->ChangeableCardsLen <= 13)
		{
			m_GameLogic.AddCard(pHc, all_card[i]);
			is_add = true;
		} 		
		int ret = check_guilin_listen_xilun(ChairID);		
		if (ret) 
		{

			stListenInfo[bListenLen].bOutCard = bCard;  //要打出的牌       			
			bTrue = true;
			log_debug("ret %d  all_card[%x]  blisten[%d]", ret, all_card[i] , bListenLen );
			if ( bListenLen< 13 && hu_count< 12 ) 
			{
				hu_count++;
				stListenInfo[bListenLen].HuInfo[stListenInfo[bListenLen].bHuLen].bCard = all_card[i];   //听的牌				
				log_debug("ret %d  all_card[%x]" , ret , all_card[i]);
				stListenInfo[bListenLen].HuInfo[stListenInfo[bListenLen].bHuLen].bFanShu = 5;  
				stListenInfo[bListenLen].bHuLen++;
					
			}						
		} 		
		if (is_add)
		{
			m_GameLogic.ChuPai(pHc, all_card[i]);
			is_add = false;
		}
		
	} 

	if (pHc->ChangeableCardsLen<= 13)
	   m_GameLogic.AddCard(pHc, bCard);  

	if (bTrue && bListenLen< 13)
	{
		bListenLen++;
	}

	return  bTrue;

}

bool Table::Check_Listening_haohuaqidui(HandCards *pHc, _uint8 ChairID, _uint8 bCard, tagListenInfos stListenInfo[14], _uint8 &bListenLen)
{
	if (m_CardPool.GameId != XIAOHUA_MAJIAGN_GAME_ID)
		return false;
	bool bTrue = false;
	if (ChairID <0 || ChairID> m_GAME_PLAYER)
	{
		log_error("error !");
		return false;
	}
	
	int Fix = pHc->FixedCardsLen;
	if (Fix != 1)
		return false;
	if ((pHc->FixedCards[0].state  & TYPE_PENG) == 0)
		return false;
	
	m_GameLogic.ChuPai(pHc, bCard);
	//log_debug("out card [%x]  len [%d]   all[%s]", bCard, pHc->ChangeableCardsLen, CatHandCard(*pHc, ChairID));

	if (m_GameLogic.xiaohua_haohua_ting(pHc) && bListenLen  <=13 )
	{
		stListenInfo[bListenLen].bOutCard = bCard;//要打出的牌       
		stListenInfo[bListenLen].HuInfo[stListenInfo[bListenLen].bHuLen].bCard  =  pHc->FixedCards[0].CardData  ;  //听的牌
		stListenInfo[bListenLen].HuInfo[stListenInfo[bListenLen].bHuLen].bFanShu = 20;  // Calc_Fan(ChairID, fan_buffer);
		bTrue = true;
		stListenInfo[bListenLen].bHuLen++;
		bListenLen++; 
		log_debug("true %x" ,bCard );

	}
	m_GameLogic.AddCard(pHc, bCard);
	
	return bTrue;
}

int Table::Get_Hand_All_Card_Count(const  HandCards  &phc  , _uint8  cur_card ) 
{
	int size = 0;
	for (int i = 0; i < phc.ChangeableCardsLen; i++)
	{
		if (phc.ChangeableCards[i] == cur_card)
			size++;
	}
	return  size;
}


bool Table::Check_Listening(HandCards *pHc, _uint8 ChairID, _uint8 bCard, tagListenInfos stListenInfo[14], _uint8 &bListenLen)
{
	if (bListenLen >= 14)
	{
		log_error("%s chaochu zuida ting pai shu! ", __FUNCTION__);
		return false;
	}
	HandCards &tempHandCard = *pHc; 
	m_GameLogic.ChuPai(&tempHandCard, bCard);
	bool bTrue = false;	
	MJ_BUFFER<_uint8, POOL_CARD_TYPE_MAX> _relativity;
	GetAllHandCardRelativity(&tempHandCard, _relativity);
	_relativity.Add(0x41);
	_relativity.Add(0x42 );
	_relativity.Add(0x43);
	map<int, int> zhongfa_map;

	int ret = 0;
	for (_uint8 i = 0; i< _relativity.Size(); i++)
	{
		
		ret =   m_GameLogic.AddCard(&tempHandCard, _relativity[i]); 
		MJ_fan_buffer fan_buffer;		
		if ( Get_Hand_All_Card_Count(tempHandCard , _relativity[i]) <= 4 
			  && Check_Hu(ChairID, tempHandCard, fan_buffer , _relativity[i] ) == TYPE_HU)
		{			
			
			if (stListenInfo[bListenLen].bHuLen <18 )
			{					
				if (  zhongfa_map[_relativity[i]] ==  1 )
				{
					if (ret == 0)
						m_GameLogic.ChuPai(&tempHandCard, _relativity[i]);
					continue;   
				}
					
				stListenInfo[bListenLen].bOutCard = bCard;//要打出的牌       
				stListenInfo[bListenLen].HuInfo[stListenInfo[bListenLen].bHuLen].bCard = _relativity[i]; //听的牌
				stListenInfo[bListenLen].HuInfo[stListenInfo[bListenLen].bHuLen].bFanShu = Calc_Fan(ChairID, fan_buffer);
				bTrue = true; 
				zhongfa_map[_relativity[i]] = 1;   /// 两个红中问题。  
				stListenInfo[bListenLen].bHuLen++;
			}				
		}
		if(ret == 0 )		
			m_GameLogic.ChuPai(&tempHandCard, _relativity[i]);
	}
	

	m_GameLogic.AddCard(&tempHandCard, bCard);
	if (bTrue)
	{
		
		if (bListenLen < 14)
		{
			bListenLen++;
		}		
	}
	return bTrue;
	
}


void Table::Send_lookon_GameScene(Player *player)
{
	if (player == NULL)
	{
		log_error("Send_lookon_GameScene, player is NULL ");
		return;
	}
	int ChairID = player->seatid;	
	log_info(" begin uid %d chairid:%d now gamestate:%d  ",  player->uid, ChairID, m_bGameState);
	proto::game::AckGameScene stGameScene;
	Proto_GameScene_lookon(ChairID, player, stGameScene);
	unicast(SERVER_GAME_SCENE, player, stGameScene);
	//log_info("%s over uid:%d chairid:%d ", __FUNCTION__, player->uid, player->seatid);
}

void Table::User_Offline(Player * player, bool offline)
{   
	if (NULL == player)
	{
		return;
	}	
	if (player->seatid >= 0 && player->seatid < m_GAME_PLAYER)
	{
		m_IsOffline[player->seatid] = offline;
		UserOnlineState_bc(player->seatid);
	}
	if(rand()% 40 == 0 )
		log_info(" tid:%d uid:%d is  online_state[%d] ",  tid, GetPlayerUid(player),  offline );    

}

//玩家在线状态的改变
void Table::UserOnlineState_bc(_uint8 ChairID)
{
	proto::game::AckUserOnlineState stAck;
	stAck.Clear();
	stAck.set_chairid(ChairID);
	_uint8 state = m_IsOffline[ChairID] ? 1 : 0;
	stAck.set_state(state);
		
	broadcast(SERVER_USER_OFFLINE_BC, stAck);
}
void Table::UserOvertime_State(_uint8 ChairID)
{
	proto::game::AckUserOvertimeState stAck;
	stAck.Clear();
	stAck.set_chairid(ChairID);
	stAck.set_state(m_bOverTimeState[ChairID]);
	
	broadcast(SERVER_OVERTIME_USER, stAck);
}

char* Table::CatHandCard(const HandCards& pHc, _uint8 ChairID)
{
	static char retBuf[0x1000];
	retBuf[0] = '\0';
	for (int i = 0; i < pHc.ChangeableCardsLen; i++)
	{
		char tmp[0x10] = { 0 };
		sprintf(tmp, "%02x,", pHc.ChangeableCards[i]);
		strcat(retBuf, tmp);
	}
	return retBuf;
}

 // 给玩家发牌。。 

void Table::Send_Hand_Card()
{  
	for (int i = 0; i < m_GAME_PLAYER; i++)
	{

		m_HandCard[i].ChangeableCardsLen = HAND_CARD_SIZE_MAX - 1;
		m_CardPool.GetCard(m_HandCard[i].ChangeableCards, m_HandCard[i].ChangeableCardsLen);
	}
	if (m_conf.m_bQiShou_14_zhang)  // 给庄家发第14 张。 这个默认。是false  也没有配置。。修改这个地方的值。 不用看！！
	{
		_uint8 bCard = m_CardPool.GetOneCard();
		m_GameLogic.AddCard(&m_HandCard[m_bBanker], bCard);

		m_bCurChairID = m_bBanker;
		m_bGetCardCount[m_bBanker]++;
		m_bCurCard = bCard;
	}
	//给客户端 几个玩家  发牌 13 张。！
	proto::game::AckFirstHandCard stFirsthandcard;
	for (int i = 0; i < m_GAME_PLAYER; i++)
	{
		
		stFirsthandcard.Clear();
		proto::game::HandCards *pstHc = stFirsthandcard.mutable_usercard();
		pstHc->set_changeablecardslen( m_HandCard[i].ChangeableCardsLen);
		
		for (int j=0; j<m_HandCard[i].ChangeableCardsLen; j++)
		{
			pstHc->add_changeablecards(m_HandCard[i].ChangeableCards[j]);
			
		}

		for (int k = 0; k < m_GAME_PLAYER; k++)
		{
			stFirsthandcard.add_handcardnums(m_HandCard[k].ChangeableCardsLen);
		}		
		unicast(SERVER_FIRST_HANDCARD, i, stFirsthandcard);			
		log_info("m_playertimes[%d] uid[%d] sid[%d]  all_card[%s] " , 
			m_nPlayingTimes , GetPlayerUid(i), i, CatHandCard(m_HandCard[i], i));
			
	}

	for (int i = 0; i < m_GAME_PLAYER; i++)
	{
		m_GameLogic.SortCard(m_HandCard[i].ChangeableCards, m_HandCard[i].ChangeableCardsLen);
	}
	save_video_firsthand_card();
	
}

void Table::Proto_GameEndInfo(const MJ_user_buffer& UserID, proto::game::AckGameEnd& stGameend)
{
	//增加结算显示	
	proto::game::tagGameEndInfo *pGameendinfo[GAME_PLAYER] = { 0 };
	for (_uint8 i = 0; i < m_GAME_PLAYER; i++)
	{
		pGameendinfo[i] = stGameend.add_endinfo();
	}		
	stGameend.set_hufantype(0);
	
	if (UserID.Size() == 1)
	{   //一个人胡
		_uint8 win_seat = UserID[0];
		//assert( m_stHuScore[win_seat].stHu.Size() != 0);
		if (m_stHuScore[win_seat].stHu.Size() == 0)
		{
			log_error("error 0  "); 
			return;
		}
		
		if ( m_stHuScore[win_seat].stHu[0].bType == HU_ZIMO)
		{	
			//自摸胡
			pGameendinfo[win_seat]->set_endtype(HU_ZIMO);
			pGameendinfo[win_seat]->set_hucard( m_stHuScore[win_seat].stHu[0].bHuCard);			
			log_debug("hucard %x  ", m_stHuScore[win_seat].stHu[0].bHuCard );

			for (_uint8 c = NextChair(win_seat); c != win_seat; c = NextChair(c))
			{
				pGameendinfo[c]->set_endtype( HU_BEIZIMO);
				pGameendinfo[c]->set_hucard(0);
			}
		}
		else if (m_stHuScore[win_seat].stHu[0].bType == HU_PAOHU)
		{//点炮
			pGameendinfo[win_seat]->set_endtype(HU_PAOHU);
			pGameendinfo[win_seat]->set_hucard(m_stHuScore[win_seat].stHu[0].bHuCard);
			log_debug("hucard %x", m_stHuScore[win_seat].stHu[0].bHuCard );

			for (_uint8 c = NextChair(win_seat); c != win_seat; c = NextChair(c))
			{
				pGameendinfo[c]->set_endtype(HU_INVALID);
				pGameendinfo[c]->set_hucard(0);
			}
			pGameendinfo[m_stHuScore[win_seat].stHu[0].bPasHuChairID]->set_endtype(HU_DIANPAO);			

		}
	}
	else if (UserID.Size() > 1)
	{	
		//一炮多响
		//if (m_stHuScore[UserID[i]].stHu.Size() == 0)   // 容错处理 。
			//return; 

		for (int i = 0; i < UserID.Size();i++)
		{
			if (m_stHuScore[UserID[i]].stHu.Size() == 0 )   // bug modify .!!!
				continue;  

			pGameendinfo[UserID[i]]->set_endtype(HU_PAOHU);
			pGameendinfo[UserID[i]]->set_hucard( m_stHuScore[UserID[i]].stHu[0].bHuCard); 
			log_debug("hucard %x ", m_stHuScore[UserID[i]].stHu[0].bHuCard);
			
		}
		for (_uint8 i = 0; i < m_GAME_PLAYER; i++)
		{
			if (UserID.Find(i))
			{
				if (m_stHuScore[i].stHu.Size() == 0)      //  bug modify !!!!!
					continue;

				pGameendinfo[i]->set_endtype(HU_PAOHU);
				pGameendinfo[i]->set_hucard(m_stHuScore[i].stHu[0].bHuCard);
				log_debug("hucard %x", m_stHuScore[i].stHu[0].bHuCard );
			}
			else
			{
				pGameendinfo[i]->set_endtype(HU_INVALID);
				pGameendinfo[i]->set_hucard(0);
			}
		}
		
		if (m_stHuScore[UserID[0]].stHu.Size())         // bug modify !!!!!
		{
			_uint8 dianpaoID = m_stHuScore[UserID[0]].stHu[0].bPasHuChairID;
			pGameendinfo[dianpaoID]->set_endtype(HU_DIANPAO); 
		}

	
	}
	else
	{
		for (int i = 0; i < m_GAME_PLAYER; i++)
		{
			pGameendinfo[i]->set_endtype(HU_INVALID);
			pGameendinfo[i]->set_hucard(0);
		}
	}
	for (_uint8 i = 0; i < m_GAME_PLAYER; i++)
	{
		proto::game::tagGameEndInfo* endinfo = stGameend.mutable_endinfo(i);
		endinfo->set_totalgangscore(m_stGang[i].iTotalScore);
	}
	
}

void Table::Proto_GameEndInfo_GangInfo(_uint8 ChairID, proto::game::AckGameEnd& stGameend)
{
	proto::game::tagGameEndInfo* pGameendinfo = stGameend.mutable_endinfo(ChairID);

	for (int i = 0; i < m_stGang[ChairID].Gang.Size(); i++)
	{
		const tagGangScore& t = m_stGang[ChairID].Gang[i];
		proto::game::tagGameEndGang *pGang = pGameendinfo->add_gang();
		pGang->set_gangtype(t.iType);
		pGang->set_gangcard(t.Card);

	}
	for (int i = 0; i < m_GAME_PLAYER; i++)//添加点杠信息
	{
		for (int j = 0; j < m_stGang[i].Gang.Size(); j++)
		{
			const tagGangScore& t = m_stGang[i].Gang[j];
			if (t.iType == TYPE_ZHIGANG && t.ChairID == ChairID)
			{
				proto::game::tagGameEndGang* pGang = pGameendinfo->add_gang();
				pGang->set_gangtype(64);//点杠
				pGang->set_gangcard(t.Card);

			}
		}
	}

	stGameend.set_gangscore(m_stGang[ChairID].iTotalScore);
}

void Table::save_video_palyer_info()
{
	proto::login::AckTableInfo stTableinfo;
	stTableinfo.Clear();
	stTableinfo.set_seatid(-1);
	stTableinfo.set_roomgolden(roomGold);

	for (int i = 0; i < m_GAME_PLAYER; i++)
	{
		if (m_SeatPlayer[i].player)
		{   
			//log_info("save_video_palyer_info uid:%d chairID %d\n", GetPlayerUid(i), i);
			proto::login::tagPlayers *paddPlayer = stTableinfo.add_players();
			paddPlayer->set_uid(m_SeatPlayer[i].player->uid);
			paddPlayer->set_seatid(m_SeatPlayer[i].player->seatid);
			_uint8 bReady = (m_SeatPlayer[i].action == proto::game::ACTION_READY ? 1:0);
			paddPlayer->set_ready(bReady);
			paddPlayer->set_money(m_SeatPlayer[i].player->money);
			if (m_SeatPlayer[i].player->name != "")
			{
				paddPlayer->set_name(m_SeatPlayer[i].player->name.c_str());
			}
			if (m_SeatPlayer[i].player->avatar != "")
			{
				paddPlayer->set_avatar(m_SeatPlayer[i].player->avatar.c_str());
			}			
			paddPlayer->set_sex(m_SeatPlayer[i].player->sex);
		}	
	}
	stTableinfo.set_cardpoolnums(m_CardPool.MaxCount());
	if (m_Start_Shai.Num > 0)
	{
		stTableinfo.add_touziinfo(m_Start_Shai.Shai[0] + 1);
		stTableinfo.add_touziinfo(m_Start_Shai.Shai[1] + 1);
	}
	stTableinfo.set_gcrdfrmlstnum(m_CardPool.GetCardCount_FromBack());

	std::string str = stTableinfo.SerializeAsString();
	save_video_data(SERVER_TABLE_INFO_UC, str);
}

void Table::save_game_scene_info()
{
	proto::game::AckGameScene stGameScene;

	stGameScene.set_outcardtime(m_Outcard_Timer_stamp);
	stGameScene.set_operatetime(m_Operate_Timer_stamp0);
	stGameScene.set_delaytime(m_bDelayTime);
	stGameScene.set_basescore(m_BaseScore);
	stGameScene.set_scenestatus(m_bGameState);
	stGameScene.set_curoutcarduser(m_bOutCardChairID);
	stGameScene.set_curoutcard(m_bCurOutCard);
	stGameScene.set_bankerchairid(m_bBanker);
	stGameScene.set_stand_money(stand_money);

	Proto_GameScene_lookon(INVALID_CHAIRID, NULL, stGameScene);

	std::string str = stGameScene.SerializeAsString();
	save_video_data(SERVER_GAME_SCENE, str);
}

void Table::save_video_gamestar()
{
	proto::game::AckGameStart stGamestart;
	stGamestart.set_chairid(m_bBanker);
	
	stGamestart.set_ischangbanker(m_last_Banker == INVALID_CHAIRID ? 0 :
		m_last_Banker == m_bBanker ? 0 : 1);

	if (m_Start_Shai.Num > 0)
	{
		stGamestart.add_touziinfo(m_Start_Shai.Shai[0] + 1);
		stGamestart.add_touziinfo(m_Start_Shai.Shai[1] + 1);
	}
	std::string str = stGamestart.SerializeAsString();
	save_video_data(SERVER_GAME_START, str);

}

void Table::save_video_firsthand_card()
{  
	proto::game::VideoFirstHandCard FirstHandCard;
	for (int i = 0; i < m_GAME_PLAYER; i++)
	{
		proto::game::HandCards *pHandCard = FirstHandCard.add_usercard();
		if (pHandCard)
		{
			pHandCard->set_changeablecardslen(m_HandCard[i].ChangeableCardsLen);
			for (int j = 0; j < m_HandCard[i].ChangeableCardsLen; j++)
			{
				pHandCard->add_changeablecards(m_HandCard[i].ChangeableCards[j]);
			}
		}
	}
	
	std::string str = FirstHandCard.SerializeAsString();
	save_video_data(SERVER_VIDEO_FIRST_HANDCARD, str);
}

void Table::save_user_get_card(_uint8 chairID, _uint8 cbCard, int nLeftCardNum)
{
	proto::game::AckUserGetCard getCard;
	getCard.Clear();
	getCard.set_card(cbCard);
	getCard.set_chairid(chairID);
    getCard.set_leftcardnum(nLeftCardNum);
	getCard.set_optype(m_bUserOperate[chairID] );

	//log_info("save_user_get_card uid:%d chairID:%d card:0x%02x \n", GetPlayerUid(chairID), chairID, cbCard);
	std::string str = getCard.SerializeAsString();
	save_video_data(SERVER_USER_GETCARD , str);


}

void Table::save_user_out_card(_uint8 chairID, _uint8 cbCard)
{
	proto::game::AckUserOutCard stUseroutcard;
	stUseroutcard.Clear();
	stUseroutcard.set_chairid(chairID);
	stUseroutcard.set_card(cbCard);
    
	std::string str = stUseroutcard.SerializeAsString();
	save_video_data(SERVER_USER_OUTCARD, str);
}

void Table::save_video_hu(_uint8 chairID, _uint8 paschairID, int score[GAME_PLAYER])
{
	proto::game::AckUserHu stHu;
	stHu.set_huchairid(chairID);
	stHu.set_pashuchairid(paschairID);
	for (int i = 0; i < m_GAME_PLAYER; i++)
	{
		stHu.add_score(score[i]);
	}
	std::string str = stHu.SerializeAsString();
	save_video_data(SERVER_USER_HU, str);
}

void Table::save_video_operate(int iOpType, _uint8 chairID, _uint8 paschairID, _uint8 cbCard, int score[GAME_PLAYER])
{  
	proto::game::AckBcOpResult stOpresult;
	stOpresult.set_chairid(chairID);
	stOpresult.set_outcardchairid(paschairID);
	stOpresult.set_optype(iOpType);
	stOpresult.set_card(cbCard);
	for (int i = 0; i < m_GAME_PLAYER; i++)
	{
		stOpresult.add_score(score[i]);
	}
	std::string str = stOpresult.SerializeAsString();
	save_video_data(SERVER_OPERATE_RESULT, str);
}


void Table::save_video_game_end(const proto::game::AckGameEnd& stGameEnd)
{
	std::string str = stGameEnd.SerializeAsString();
	save_video_data(SERVER_GAME_END, str);
}

void Table::save_video_data(int cmd, const std::string& str, int nlen)
{   
	std::string strcmd;
	char b[0x40] = {0};
	snprintf(b, sizeof(b)-1 ,"%d:S:", cmd);
	//std::string aaa(b,strlen(b));
	strcmd = b;
	m_videostr += strcmd; 
    m_videostr += str;
	m_videostr.append(":E:");
	
}

void Table::Send_Video_Data_Timer_cb(struct ev_loop *loop, struct ev_timer *w, int revents)
{   
	Table *table = (Table*) w->data;
	//log_info("%s tid:%d ", __FUNCTION__, table->tid);
	if (table != NULL)
	{
		ev_timer_stop(zjh.loop, &table->send_videodata_timer);
		table->send_video_to_svr();
	}
}

void Table::send_video_to_svr()
{
	//m_videostr.clear();
	//return; 
	if (m_videostr.size() == 0)
	{
		return;
	}

	unsigned char buf[MaxVideoBufLen] ={0};
	int noutlen = MaxVideoBufLen;	
	int iRet = base64_encode((const unsigned char *)m_videostr.c_str(),m_videostr.size(),(unsigned char *)buf, &noutlen);
	if (iRet != CRYPT_OK)
	{
		log_error("send_video_to_svr base64_encode is error code:%d", iRet);
		return;
	}
	
	Jpacket packet_data;
	packet_data.val["cmd"] = SERVER_VIDEO_DATA_SAVE;
	packet_data.val["roomid"] = tid;
	std::string packStr((char *)buf, noutlen);
	packet_data.val["content"] = packStr;

	char strbuf[50] = { 0 };
	sprintf(strbuf, "%d_%04d", tid, m_nPlayingTimes);
	std::string strTimes = strbuf;

	packet_data.val["innings"] = strTimes;
	packet_data.end();
	zjh.game->send_to_videosvr(packet_data.tostring());	

	m_videostr.clear();
}


//重新设置每个人。解散房间的次数 

void Table::reset_disbandcount()
{
	for( std::map<int, Player*>::iterator iter = m_mapTable_Player.begin(); iter != m_mapTable_Player.end(); iter++ )
	{
		if( iter->second )
		{   
			iter->second->DisBandCount = 0;
		}
	}
}

bool Table::check_roomowner_disband_table(int uid, int roomstatus)
{
	if (m_bGameState == GAME_FREE && roomstatus == 0)
	{
		proto::game::AckDisbandResult stAckResult;		
		stAckResult.set_result(1);
		broadcast(SERVER_DISBAND_RESULT, stAckResult);

		m_bIsDisbandTable = false;

		//发送解散命令给后台
		Send_DisbandTableMessage();

		memset(m_DisbandChoose, proto::game::DISBAND_WAIT, sizeof(m_DisbandChoose));//初始化玩家选择
		return true;
	}
	return false;
}

void Table::send_talbe_no_user()
{
	Jpacket req_packet;
	req_packet.val["cmd"] = SERVER_ALL_PLAYER_DOWN;
	req_packet.val["vid"] = vid;

	char strroomid[64] = { 0 };
	sprintf(strroomid, "%d", tid);

	req_packet.val["roomid"] = strroomid;
	req_packet.end();
	
	zjh.game->send_to_datasvr(req_packet.tostring());
}

 bool Table::check_all_table_user_offline()
{
	for (int i = 0; i < m_GAME_PLAYER; i++)
	{
		if (!m_IsOffline[i])
		{
			return false;
		}
	}
	return true;
}

