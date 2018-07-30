#include "table.h"

extern PHZ phz;
extern Log log;

Table::Table():
m_RepertoryCard(CARD_POOL_NO_FLOWER, 0),
start_timer_stamp(1),
preready_timer_stamp(1),
dispatch_show_timer_stamp(0.9),
dispatch_delay_timer_stamp(0.01),
dissolve_room_tstamp(90),
send_videodata_timer_stamp(0.4),
game_restart_timer_stamp(3),
m_Outcard_Timer_stamp(10),
m_Operate_Timer_stamp(10),
m_Operate_Timer_stamp0(10),
m_Operate_Timer_stamp1(10),
m_Operate_Timer_stamp2(10),
m_Operate_Timer_stamp3(10),
first_getcard_timer_stamp(2)
{
	preready_timer.data = this;
	ev_timer_init(&preready_timer, Table::preready_timer_cb, preready_timer_stamp, preready_timer_stamp);
	
	dissolve_room_timer.data = this;
	ev_timer_init(&dissolve_room_timer, Table::dissolve_room_timer_cb, dissolve_room_tstamp, dissolve_room_tstamp);
	
	send_videodata_timer.data = this;
	ev_timer_init(&send_videodata_timer, Table::Send_Video_Data_Timer_cb, send_videodata_timer_stamp, send_videodata_timer_stamp);
	
	game_restart_timer.data = this;
	ev_timer_init(&game_restart_timer, Table::game_restart_timer_cb, game_restart_timer_stamp, game_restart_timer_stamp);

	m_Outcard_Timer.data = this;
	ev_timer_init(&m_Outcard_Timer, Table::UserOutCard_Timer_cb, m_Outcard_Timer_stamp, m_Outcard_Timer_stamp);

	m_Operate_Timer.data = this;
	ev_timer_init(&m_Operate_Timer, Table::UserChooseOperate_Timer_cb, m_Operate_Timer_stamp, m_Operate_Timer_stamp);

	m_Operate_Timer0.data = this;
	ev_timer_init(&m_Operate_Timer0, Table::UserChooseOperate_Timer_cb0, m_Operate_Timer_stamp0, m_Operate_Timer_stamp0);

	m_Operate_Timer1.data = this;
	ev_timer_init(&m_Operate_Timer1, Table::UserChooseOperate_Timer_cb1, m_Operate_Timer_stamp1, m_Operate_Timer_stamp1);

	m_Operate_Timer2.data = this;
	ev_timer_init(&m_Operate_Timer2, Table::UserChooseOperate_Timer_cb2, m_Operate_Timer_stamp2, m_Operate_Timer_stamp2);

	m_Operate_Timer3.data = this;
	ev_timer_init(&m_Operate_Timer3, Table::UserChooseOperate_Timer_cb3, m_Operate_Timer_stamp3, m_Operate_Timer_stamp3);

	start_timer.data = this;
	ev_timer_init(&start_timer, Table::start_timer_cb, start_timer_stamp, start_timer_stamp);

	dispatch_show_timer.data = this;
	ev_timer_init(&dispatch_show_timer, Table::dispatch_show_timer_cb, dispatch_show_timer_stamp, dispatch_show_timer_stamp);

	dispatch_delay_timer.data = this;
	ev_timer_init(&dispatch_delay_timer, Table::dispatch_delay_timer_cb, dispatch_delay_timer_stamp, dispatch_delay_timer_stamp);	 
 	
	first_getcard_timer.data = this;
	ev_timer_init(&first_getcard_timer, Table::first_getcard_timer_cb, first_getcard_timer_stamp, first_getcard_timer_stamp);
	
	m_room_config.Clear();

	m_Trusteeshit_Out_Delay_Timer_stamp = 1;
	m_Trusteeshit_Ope_Delay_Timer_stamp0 = 1;
	m_Trusteeshit_Ope_Delay_Timer_stamp1 = m_Trusteeshit_Ope_Delay_Timer_stamp0;
	m_Trusteeshit_Ope_Delay_Timer_stamp2 = m_Trusteeshit_Ope_Delay_Timer_stamp0;
	m_Trusteeshit_Ope_Delay_Timer_stamp3 = m_Trusteeshit_Ope_Delay_Timer_stamp0;

	m_Trusteeshit_Out_Delay_Timer.data = this;
    ev_timer_init(&m_Trusteeshit_Out_Delay_Timer, Table::Trusteeshit_Out_Timer_cb, m_Trusteeshit_Out_Delay_Timer_stamp, m_Trusteeshit_Out_Delay_Timer_stamp);	

    m_Trusteeshit_Ope_Delay_Timer0.data = this;
    ev_timer_init(&m_Trusteeshit_Ope_Delay_Timer0, Table::Trusteeshit_Ope_Timer_cb0, m_Trusteeshit_Ope_Delay_Timer_stamp0, m_Trusteeshit_Ope_Delay_Timer_stamp0);   

    m_Trusteeshit_Ope_Delay_Timer1.data = this;
    ev_timer_init(&m_Trusteeshit_Ope_Delay_Timer1, Table::Trusteeshit_Ope_Timer_cb1, m_Trusteeshit_Ope_Delay_Timer_stamp1, m_Trusteeshit_Ope_Delay_Timer_stamp1);   

    m_Trusteeshit_Ope_Delay_Timer2.data = this;
    ev_timer_init(&m_Trusteeshit_Ope_Delay_Timer2, Table::Trusteeshit_Ope_Timer_cb2, m_Trusteeshit_Ope_Delay_Timer_stamp2, m_Trusteeshit_Ope_Delay_Timer_stamp2);   
    
    m_Trusteeshit_Ope_Delay_Timer3.data = this;
    ev_timer_init(&m_Trusteeshit_Ope_Delay_Timer3, Table::Trusteeshit_Ope_Timer_cb3, m_Trusteeshit_Ope_Delay_Timer_stamp3, m_Trusteeshit_Ope_Delay_Timer_stamp3);       
}

Table::~Table()
{
	ev_timer_stop(phz.loop, &start_timer);	
 	ev_timer_stop(phz.loop, &first_getcard_timer);
	ev_timer_stop(phz.loop, &preready_timer);
	ev_timer_stop(phz.loop, &dispatch_show_timer);
	ev_timer_stop(phz.loop, &dispatch_delay_timer);
	ev_timer_stop(phz.loop, &dissolve_room_timer);
	ev_timer_stop(phz.loop, &send_videodata_timer);
	ev_timer_stop(phz.loop, &game_restart_timer);

	ev_timer_stop(phz.loop, &m_Outcard_Timer);
	ev_timer_stop(phz.loop, &m_Operate_Timer);
	ev_timer_stop(phz.loop, &m_Operate_Timer0);
	ev_timer_stop(phz.loop, &m_Operate_Timer1);
	ev_timer_stop(phz.loop, &m_Operate_Timer2);
	ev_timer_stop(phz.loop, &m_Operate_Timer3);

	ev_timer_stop(phz.loop, &m_Trusteeshit_Out_Delay_Timer);
	ev_timer_stop(phz.loop, &m_Trusteeshit_Ope_Delay_Timer0);
	ev_timer_stop(phz.loop, &m_Trusteeshit_Ope_Delay_Timer1);
	ev_timer_stop(phz.loop, &m_Trusteeshit_Ope_Delay_Timer2);
	ev_timer_stop(phz.loop, &m_Trusteeshit_Ope_Delay_Timer3);
}

void Table::Reset()
{
	memset(m_stGang, 0, sizeof(m_stGang));
	memset(m_stHuScore, 0, sizeof(m_stHuScore));
	memset(m_bIsHu, 0, sizeof(m_bIsHu));
	memset(m_HuFanNums, 0, sizeof(m_HuFanNums));

	m_bMultiOpUserNum = 0;//引起有操作提示的人数	
	m_bMultiDoneOpNum = 0;//已经选择操作的玩家数
	m_bMulitFirstOperate = false;//优先级最高的玩家是否执行还是放弃，放弃为false
	m_bMultiOpHuNum = 0;
		
	m_bMulitHuOpeCount = 0;	
	m_RobWanGangCard = INVALID_MJ_CARD;
	m_RobOperateNum = 0;
	m_bRobHu = false;
	m_RobHuNum = 0;
	m_bRobWanGang = false;
	m_bRobGangHuJudge = false;
	m_WangGangPlayerOpType = TYPE_NULL;
	m_PassHuPlayer.Clear();
	m_WangGangPlayer = INVALID_CHAIRID;
	memset(&m_RobHuDoneOp, 0, sizeof(m_RobHuDoneOp));
	memset(m_bAddFanFlag, 0, sizeof(m_bAddFanFlag));
	memset(m_oFan, 0, sizeof(m_oFan));    

	m_State = GAME_FREE;
	for(_uint8 i = 0; i < GAME_PLAYER; ++i){
		m_Seats[i].Reset();
	}
	m_strVideo.clear();
	m_strInnings.clear();

	m_bFirstCard = false;
	m_bDispatch = true;
	m_bCardOperated = false;
	m_bOutCard = false;
	m_bCanHu = false;
	m_CurOutSeat = INVALID_CHAIR;
	m_CurOutCard = INVALID_CARD;	

	m_LaiziCard = INVALID_CARD;
	m_GuiVector.Clear();
	for(_uint8 i = 0; i < GAME_PLAYER; ++i){
		m_OperateRecord[i].Clear();
	}
}


void Table::Init(const Json::Value &val, const std::string &roomid)
{
	InitMember();
	InitConf(val, roomid);
	InitLogic();
}

//初始化逻辑类配置
void Table::InitLogic()
{
	//固有逻辑初始化
}

void Table::InitMember()
{    
    players.clear();
    m_PlayId = phz.conf["table"]["m_PlayId"].asInt();
    m_pTableInfo.Clear();
    
    is_dissolved = 0;
	dissovle_state = 0;
	dissolve_applyer = 0;
    voters_players.clear();
     
    m_GamePlayer = 0;
    m_bAlreadyResp = false;
    memset(m_bOverTimeState, 0, sizeof(m_bOverTimeState));	
	m_BankerSeat = INVALID_CHAIR;
	m_BankerCard = INVALID_CARD;
	m_CurSeat = INVALID_CHAIR;
	m_CurCard = INVALID_CARD;	
	m_CurTurn = 1;
	m_TotalTurn = 1;
	m_vecGains.clear();
	base_score = 1;
	m_cbCurTimer = 0;
	m_bGangCalcImme = false;
	m_last_Banker = INVALID_CHAIRID;
	m_MaxCount = 0;
	m_ft2fn = NULL;    
	 	 	
	m_bPiao = false;
	m_PiaoScore.clear();	
	
	for(_uint8 i = 0; i < GAME_PLAYER; ++i){
		m_Seats[i].m_SeatId = i;
		m_Seats[i].m_Table = this;
	}
	Reset();
}

//初始化配置
void Table::InitConf(const Json::Value &val, const std::string &roomid)
{
	tid = roomid;
	if (!val["baseGold"].isNull() && val["baseGold"].isNumeric()){
		base_money = val["baseGold"].asInt();
	}
	if (!val["standMoney"].isNull() && val["standMoney"].isNumeric()){
		stand_money = val["standMoney"].asInt();
	}
	if (!val["roomGolden"].isNull() && val["roomGolden"].isNumeric()){
		m_TakeIn = val["roomGolden"].asInt();
	}
	if (!val["roomStatus"].isNull() && val["roomStatus"].isNumeric()){
		m_RoomStatus = val["roomStatus"].asInt();
	}
	if (!val["roomUserId"].isNull() && val["roomUserId"].isNumeric()){
		m_RoomUserId = val["roomUserId"].asInt();
	}
	m_TotalTurn = val["dataMap"]["roomCount"].asInt();
}


int Table::HandlerUserOperate(int cmd, Player* player)
{
	switch (cmd)
	{
	case CLIENT_READY_REQ:
		handler_ready(player);
		break;
	case CLIENT_UPDATE_GPS_REQ:
		handler_update_gps(player);
		break;		
	case CLIENT_GET_GPS_REQ:
		handler_get_gps(player);
		break;
	case CLIENT_DISSOLVE_ROOM_REQ:
		handler_dissolve_room(player);		//解散房间请求
		break;
	case CLIENT_DISSOLVE_ACTION_REQ:
		handler_dissolve_action(player);	//操作
		break;
	case CLIENT_LOGOUT_REQ:
		phz.game->del_player(player);
		break;
	case CLIENT_TABLE_INFO_REQ:
		handler_table_info(player);
		break;	
	case CLIENT_SWITCH_STYLE_REQ:
		handler_switch_style(player);
		break;
	case CLIENT_CHAT_REQ:
		handler_chat(player);
		break;
	case CLIENT_FACE_REQ:
		handler_face(player);
		break;
	case CLIENT_EMOTION_REQ:
		handler_interaction_emotion(player);
		break;
	case CLIENT_UPTABLE_APPLY_REQ:
		handler_apply_uptable(player);
		break;
	case CLIENT_DOWNTABLE_REQ:
		handler_downtable(player);
		break;
	case CLIENT_MJ_OUT_CARD_REQ:			//玩家出牌消息
		OnUserOutCard(player);
		break;
	case CLIENT_MJ_OPERATE_CARD_REQ:		//玩家操作牌
		OnUserOperateCard(player);
		break;
	case CLIENT_MJ_PIAO_REQ:
		OnUserPiaoOperate(player);			//飘操作
		break;
	case CLIENT_MJ_FINAL_END_REQ:
		OnFinalEndReq(player);				//总结算请求
		break;
	case CLIENT_MJ_GAINS_REQ:
		OnUserGainsReq(player);				//结算详情
		break;
	default:
		log.error("user operate error invalid cmd[%d] uid:%d tid:%s .\n", cmd, player->uid, player->tid.c_str());
		return -1;
	}
	return 0;
}

void Table::InitBaseConf()
{

}



//游戏开始
void Table::GameStart()
{
	m_State = GAME_PLAYING;
	//确定庄家 首局房主为庄 若为机器人开房随机当庄
	if (m_BankerSeat == INVALID_CHAIR){
		bool IsFind = false;
		for (_uint8 i = 0; i < m_GamePlayer; ++i){
			if (m_Seats[i].m_Player != NULL && m_Seats[i].m_Player->uid == m_RoomUserId){
				m_BankerSeat = i;
				IsFind = true;
				break;
			}
		}
		if (!IsFind) m_BankerSeat = rand() % m_GamePlayer;
	}	

	log.fatal("%s tid:%s round:%d m_strInnings:%s Banker uid:%d chairid:%d player[%d, %d, %d, %d]\n", 
	        __FUNCTION__, tid.c_str(), m_CurTurn, m_strInnings.c_str(), m_Seats[m_BankerSeat].m_Player->uid, m_BankerSeat,
		    m_Seats[0].m_Uid, m_Seats[1].m_Uid, m_Seats[2].m_Uid, m_Seats[3].m_Uid);
	
	//麻将的不同牌库
	//如果开启好牌	
	if (false)
	{
		for (int i = 0; i < m_GamePlayer; ++i)
		{
			if (m_Seats[i].m_Player->uid == m_RoomUserId)
				m_BankerSeat = i;
		}
		m_BankerSeat = 0;
    	m_RepertoryCard.FixCard(1);
    	
    	m_RepertoryCard[m_RepertoryCard.Count() - 3] = 0x14;
    	for (int i = 1; i < 3; ++i)
    	{
    		m_RepertoryCard[m_RepertoryCard.Count() - i] = 0x41;
    	}
	}
	else
	{
		m_RepertoryCard.InitPool();
	}

	m_bFirstCard = true;

	m_Start_Shai = m_GameLogic.GetShaizi(2);
	
	//发送游戏开始协议
	proto::game::AckGameStart stGamestart;
	stGamestart.set_chairid(m_BankerSeat);
	stGamestart.set_ischangbanker(m_last_Banker == INVALID_CHAIRID ? 0 :
		m_last_Banker == m_BankerSeat ? 0 : 1);
	if (m_Start_Shai.Num > 0)
	{
		stGamestart.add_touziinfo(m_Start_Shai.Shai[0]);
		stGamestart.add_touziinfo(m_Start_Shai.Shai[1]);
	}

	Ppacket ppack;
	stGamestart.SerializeToString(&ppack.body);
	ppack.pack(SERVER_GAME_START_BC);
	broadcast(NULL, ppack.data);
    log.debug("%s AckGameStart:%s .\n", __FUNCTION__, stGamestart.DebugString().c_str());	

	SaveGameStartVideo(); 
	
	//广播游戏开始
	SendGameStart();
	ev_timer_again(phz.loop, &start_timer);     
	m_cbCurTimer = 1;

    //发送赖子信息
    SendLaiziCard();
    
	SerializeTableData();

//	//保存tableinfo，跳过开局阶段，相当于断线重连，直接从发完牌开始显示录像
//	save_video_data(SERVER_TABLE_INFO_UC, m_pTableInfo.SerializeAsString());
}

void Table::SendGameStart()
{
	//给每个人产牌
	for (int i = 0; i < m_GamePlayer; i++)
	{
		m_Seats[i].m_HandCard.ChangeableCardsLen = HAND_CARD_SIZE_MAX - 1;
		m_RepertoryCard.GetCard(m_Seats[i].m_HandCard.ChangeableCards, m_Seats[i].m_HandCard.ChangeableCardsLen);
	}
	//给客户端发牌
	proto::game::AckFirstHandCard stFirsthandcard;	
	for (int i = 0; i < m_GamePlayer; i++)
	{
	    stFirsthandcard.Clear();
    	stFirsthandcard.set_banker(m_BankerSeat);
    	stFirsthandcard.set_current_num(m_CurTurn);
    	stFirsthandcard.set_total_num(m_TotalTurn);
    	stFirsthandcard.set_left_card_count(m_RepertoryCard.Count());
	
		proto::game::HandCards *pstHc = stFirsthandcard.mutable_usercard();
		pstHc->set_changeablecardslen(m_Seats[i].m_HandCard.ChangeableCardsLen);

		for (int j=0; j<m_Seats[i].m_HandCard.ChangeableCardsLen; j++)
		{
			pstHc->add_changeablecards(m_Seats[i].m_HandCard.ChangeableCards[j]);		
		}

		for (int k = 0; k < m_GamePlayer; k++)
		{
			stFirsthandcard.add_handcardnums(m_Seats[k].m_HandCard.ChangeableCardsLen);
		}		
		Ppacket ppack;
		stFirsthandcard.SerializeToString(&ppack.body);
		ppack.pack(SERVER_FIRST_HANDCARD);
		unicast(m_Seats[i].m_Player, ppack.data);

//		log.debug("%s tid:%s, AckFirstHandCard:%s \n", __FUNCTION__, stFirsthandcard.DebugString().c_str());
		log.info("%s first uid:%d chairid:%d [%s]\n", __FUNCTION__, m_Seats[i].m_Uid, i, CatHandCard(m_Seats[i].m_HandCard, i));
	}

	//玩家手牌排序
	for (int i = 0; i < m_GamePlayer; i++)
	{
		m_GameLogic.SortCard(m_Seats[i].m_HandCard.ChangeableCards, m_Seats[i].m_HandCard.ChangeableCardsLen);
	}
	
	save_video_firsthand_card();
}

void Table::save_video_firsthand_card()
{
	proto::game::VideoFirstHandCard FirstHandCard;
	for (int i = 0; i < m_GamePlayer; i++)
	{
		proto::game::HandCards *pHandCard = FirstHandCard.add_usercard();
		if (pHandCard)
		{
			pHandCard->set_changeablecardslen(m_Seats[i].m_HandCard.ChangeableCardsLen);
			for (int j = 0; j < m_Seats[i].m_HandCard.ChangeableCardsLen; j++)
			{
				pHandCard->add_changeablecards(m_Seats[i].m_HandCard.ChangeableCards[j]);
			}
		}
	}

	std::string str = FirstHandCard.SerializeAsString();
	save_video_data(SERVER_VIDEO_FIRST_HANDCARD, str);
}


//发送赖子信息
void Table::SendLaiziCard()
{    
    m_LaiziCard = INVALID_MJ_CARD;

	proto::game::AckLaiziCard stAck;
	stAck.set_laizicard(m_LaiziCard);
	stAck.set_mocard(m_LaiziCard);

    Ppacket ppack;
	stAck.SerializeToString(&ppack.body);
	ppack.pack(SERVER_LAIZI_INFO);
	broadcast(NULL, ppack.data);
	log.debug("%s AckLaiziCard:%s\n", __FUNCTION__, stAck.DebugString().c_str());

	save_video_data(SERVER_LAIZI_INFO, stAck.SerializeAsString());
}

//发完牌后的第一手操作
void Table::OnOperateStart()
{
	m_CurSeat = m_BankerSeat;
	m_CurCard = m_BankerCard;

	ev_timer_again(phz.loop, &first_getcard_timer);
	m_cbCurTimer = 9;
}

//摸牌
void Table::OnUserGetCard(_uint8 ChairID, bool bGetLast)
{
	//房间是否已经被解散
	if (is_dissolved)
	{
		return;
	}

	if (CardPoolIsEmpty())
	{
		GameEnd();
		return;
	}
	m_CurSeat = ChairID;

	_uint8 bGetCard = bGetLast? m_RepertoryCard.GetLast() : m_RepertoryCard.GetOneCard();

	if (m_Seats[ChairID].m_Player == NULL)
	{
		//如果当前玩家不存在
		return;
	}
	m_Seats[ChairID].m_bGetCardCount++;

	m_CurCard = bGetCard;
	MJ_major major;
	//摸牌判断
	m_Seats[ChairID].m_bUserOperate = GetCardEstimate(ChairID, bGetCard,  major, m_HuFanNums[ChairID]);
	m_OperateRecord[ChairID] = major;

	m_GameLogic.AddCard(&m_Seats[ChairID].m_HandCard, bGetCard);

	log.info("OnUserGetCard card data seatid[%d] uid:%d card[0x%02x] tid:%s. \n", m_CurSeat, m_Seats[m_CurSeat].m_Uid, bGetCard, tid.c_str());
	
	SaveGetCard(ChairID, bGetCard, m_RepertoryCard.Count(), bGetLast);
	//广播玩家摸牌
	
	proto::game::AckUserGetCard stUsergetcard;
	for (int i = 0; i < m_GamePlayer; i++)
	{
		if (m_Seats[i].m_Player == NULL)
		{
			log.error("OnUserGetCard chairid:%d  error, one player is NULL\n", ChairID);
			continue;
		}
		//发送出牌和操作提示
		Proto_Game_UserGetCard(i, ChairID, bGetCard, stUsergetcard);
		stUsergetcard.set_isgetlast(bGetLast ? 1 : 0);
		Ppacket ppack;
		stUsergetcard.SerializeToString(&ppack.body);
		ppack.pack(SERVER_USER_GETCARD);
		unicast(m_Seats[i].m_Player, ppack.data);
		//摸牌后，发送听牌信息
		if (i == ChairID)
		{
            SendTingInfo(ChairID);
		}
	}
	
	if (m_Seats[ChairID].m_bUserOperate == TYPE_NULL || m_Seats[ChairID].m_bUserOperate == TYPE_LISTEN)
	{
		//出牌
		set_out_timer(ChairID);
	}
	else
	{
		//操作
		set_operator_timer(ChairID);
	}

	SerializeTableData();
}

//设置出牌时钟
void Table::set_out_timer(_uint8 ChairID, bool bDelayTimer)
{
	log.info("start player uid:%d chairid:%d out card timer  \n", GetPlayerUid(ChairID), ChairID);
	
	ev_timer_stop(phz.loop, &m_Outcard_Timer);
	ev_timer_again(phz.loop, &m_Outcard_Timer);

	m_CurTimer = m_Outcard_Timer;
	m_cbCurTimer = 7;
}

//关闭操作时钟
void Table::stop_operator_timer(_uint8 ChairID)
{
	switch (ChairID)
	{
		case 0:
		ev_timer_stop(phz.loop, &m_Operate_Timer0);
		break;
		case 1:
		ev_timer_stop(phz.loop, &m_Operate_Timer1);
		break;
		case 2:
		ev_timer_stop(phz.loop, &m_Operate_Timer2);
		break;
		case 3:
		ev_timer_stop(phz.loop, &m_Operate_Timer3);
		break;
		default:
		break;
		//ev_timer_stop(phz.loop, &m_Operate_Timer);
	}
}

//设置操作时钟
void Table::set_operator_timer(_uint8 ChairID, bool bDelayTimer)
{
	//操作时钟
	log.info("operate timer ChairID:%d \n", ChairID);
	switch (ChairID)
	{
		case 0:
		{			
			if (!bDelayTimer)
			{
				ev_timer_stop(phz.loop, &m_Operate_Timer0);
				ev_timer_set(&m_Operate_Timer0, m_Operate_Timer_stamp0, m_Operate_Timer_stamp0);
				ev_timer_again(phz.loop, &m_Operate_Timer0);
			}
			else
			{   
				ev_timer_stop(phz.loop, &m_Operate_Timer0);
				ev_timer_again(phz.loop, &m_Operate_Timer0);
			}

			m_CurTimer = m_Operate_Timer0;
			m_cbCurTimer = 8;
		}
		break;
		case 1:
		{			
			if (!bDelayTimer)
			{
				ev_timer_stop(phz.loop, &m_Operate_Timer1);
				ev_timer_set(&m_Operate_Timer1, m_Operate_Timer_stamp1, m_Operate_Timer_stamp1);
				ev_timer_again(phz.loop, &m_Operate_Timer1);
			}
			else
			{   
				ev_timer_stop(phz.loop, &m_Operate_Timer1);
				ev_timer_again(phz.loop, &m_Operate_Timer1);
			}
			m_CurTimer = m_Operate_Timer1;
			m_cbCurTimer = 8;
		}
		break;
		case 2:
		{			
			if (!bDelayTimer)
			{
				ev_timer_stop(phz.loop, &m_Operate_Timer2);
				ev_timer_set(&m_Operate_Timer2, m_Operate_Timer_stamp2, m_Operate_Timer_stamp2);
				ev_timer_again(phz.loop, &m_Operate_Timer2);
			}
			else
			{   
				ev_timer_stop(phz.loop, &m_Operate_Timer2);
				ev_timer_again(phz.loop, &m_Operate_Timer2);
			}
			m_CurTimer = m_Operate_Timer2;
			m_cbCurTimer = 8;
		}
		break;
		case 3:
		{			
			if (!bDelayTimer)
			{
				ev_timer_stop(phz.loop, &m_Operate_Timer3);
				ev_timer_set(&m_Operate_Timer3, m_Operate_Timer_stamp3, m_Operate_Timer_stamp3);
				ev_timer_again(phz.loop, &m_Operate_Timer3);
			}
			else
			{   
				ev_timer_stop(phz.loop, &m_Operate_Timer3);
				ev_timer_again(phz.loop, &m_Operate_Timer3);
			}
			m_CurTimer = m_Operate_Timer3;
			m_cbCurTimer = 8;
		}
		break;
		default:
		break;
	}
}

//获取倒计时剩余时间
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
			nLeftTime = ev_timer_remaining(phz.loop, &m_CurTimer);
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
			nLeftTime = ev_timer_remaining(phz.loop, &m_CurTimer);
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

void Table::set_trusteeship_out_timer(_uint8 ChairID)
{
    ev_timer_again(phz.loop, &m_Trusteeshit_Out_Delay_Timer);
    m_CurTimer = m_Trusteeshit_Out_Delay_Timer;    
}

void Table::set_trusteeship_operator_timer(_uint8 ChairID)
{
    //操作时钟
	switch (ChairID)
	{
		case 0:
		{			
			ev_timer_again(phz.loop, &m_Trusteeshit_Ope_Delay_Timer0);
			m_CurTimer = m_Trusteeshit_Ope_Delay_Timer0;
		}
		break;
		case 1:
		{			
			ev_timer_again(phz.loop, &m_Trusteeshit_Ope_Delay_Timer1);
			m_CurTimer = m_Trusteeshit_Ope_Delay_Timer1;
		}
		break;
		case 2:
		{			
			ev_timer_again(phz.loop, &m_Trusteeshit_Ope_Delay_Timer2);
			m_CurTimer = m_Trusteeshit_Ope_Delay_Timer2;
		}
		break;
		case 3:
		{			
			ev_timer_again(phz.loop, &m_Trusteeshit_Ope_Delay_Timer3);
			m_CurTimer = m_Trusteeshit_Ope_Delay_Timer3;
		}
		break;
		default:
		break;
	}
}


//用户出牌
bool Table::OnUserOutCard(Player *player)
{
	if (is_dissolved){
		log.error("Room Dissovled is_dissolved:%d tid:%s .\n", is_dissolved, tid.c_str());
		return false;
	}
	proto::game::ReqOutCard stReqOutCard;
	stReqOutCard.ParseFromString(player->client->ppacket.body);
	log.debug("%s tid:%s uid:%d stReqOutCard:%s .\n", __FUNCTION__, tid.c_str(), player->uid, stReqOutCard.DebugString().c_str());
	_uint8 cbCardData = stReqOutCard.out_card();	//用户出的牌数据
	_uint8 wChairID = player->seatid;
	log.debug("%s tid:%s uid:%d cbCardData:0x%02x wChairID:%d.\n", __FUNCTION__ , tid.c_str(), player->uid, cbCardData, wChairID);

	if (wChairID != m_CurSeat){
		log.error("%s wChairID:%d m_wCurrentUser:%d error .\n",
			__FUNCTION__, wChairID, m_CurSeat, m_bOutCard);
		OperateError(player->seatid, proto::game::E_OUTCARD_SEAT_ERROR);
		return false;
	}

	if (CheckUserOutCard(wChairID, cbCardData))
	{
		// 删出牌时钟	
		ev_timer_stop(phz.loop, &m_Outcard_Timer);

		m_CurSeat = wChairID;
		m_CurOutSeat = wChairID;
		m_GameLogic.ChuPai(&m_Seats[wChairID].m_HandCard, cbCardData);
		m_CurCard = cbCardData;
		m_CurOutCard = cbCardData;
		m_bOverTimeState[wChairID] = 0;
		
		m_Seats[wChairID].m_bOutRecord.Add(cbCardData);
		
		//广播玩家出牌	
		proto::game::AckUserOutCard stUseroutcard;
		stUseroutcard.Clear();
		stUseroutcard.set_chairid(wChairID);
		stUseroutcard.set_card(cbCardData);
		for (int i = 0; i < m_GamePlayer; i++)
		{
			proto::game::tagShowOutCard *pstShow = stUseroutcard.add_showcard();
			for (int j=0; j<m_Seats[i].m_bOutRecord.Size(); j++)
			{
				pstShow->add_outcard(m_Seats[i].m_bOutRecord[j]);
			}
			pstShow->set_outcardlen(m_Seats[i].m_bOutRecord.Size());
		}

		Ppacket ppack;
		stUseroutcard.SerializeToString(&ppack.body);
		ppack.pack(SERVER_USER_OUTCARD);
		broadcast(NULL, ppack.data);

		//保存出牌信息
		SaveOutCardVideo(wChairID,cbCardData);
		//出牌后其他玩家操作判断
		EstimateUserRespond(m_CurOutSeat, m_CurOutCard);
	}

	SerializeTableData();	
	return true;
}

//检查玩家出牌是否在手上
bool Table::CheckUserOutCard(_uint8 ChairID, _uint8 cbCard)
{
	bool bIsHave = false;
	// 检查此张牌是否在用户手牌里
	_uint8 bChangeLens = m_Seats[ChairID].m_HandCard.ChangeableCardsLen;
	for (_uint8 i = 0; i < bChangeLens; i++)
	{
		if (m_Seats[ChairID].m_HandCard.ChangeableCards[i] == cbCard)
		{
			bIsHave = true;
			break;
		}
	}
	if (!bIsHave)
	{
		log.error("%s uid:%d seatid:%d outcard error has no card:0x%02x  \n",  __FUNCTION__, ChairID, GetPlayerUid(ChairID), cbCard);
		for (_uint8 i = 0; i < bChangeLens; i++)
		{
			log.info("card[%d]:0x%02x \n", i, m_Seats[ChairID].m_HandCard.ChangeableCards[i]);
		}
		OperateError(ChairID, proto::game::E_OUTCARD_CARD_NOT_IN_HAND_ERROR);
	}

	return bIsHave;
}


//用户打牌后的判断
bool Table::EstimateUserRespond(_uint8 bOutChairID, _uint8 bOutCard)
{
	m_bMultiOpUserNum = 0;
	m_bMultiOpHuNum = 0;
	bool bIsChangeUser = true;//是否轮到下一玩家摸牌

	for (int i = 0; i < m_GamePlayer; i++)
	{
		m_Seats[i].m_bUserOperate = TYPE_NULL;
		if (i == bOutChairID) 
		{
			m_Seats[i].m_bUserOperate = TYPE_NULL;
			continue;
		}
		//牌型判断			
		MJ_major major;
		m_Seats[i].m_bUserOperate = OutCardEstimate(i, bOutCard, major, m_HuFanNums[i]);
		m_OperateRecord[i] = major;

		if (m_Seats[i].m_bUserOperate != TYPE_NULL)
		{
			if (m_Seats[i].m_bUserOperate & TYPE_HU)
			{
				m_bMultiOpHuNum++;				
			}
			m_bMultiOpUserNum++;
			bIsChangeUser = false;
			//准备向四个玩家发送操作提示

			for (int j = 0; j < m_GamePlayer; j++)
			{
				proto::game::AckOpNotify stNotify;		
				Proto_Game_OpNotify(i, j, bOutCard, stNotify);
				
				Ppacket ppack;
				stNotify.SerializeToString(&ppack.body);
				ppack.pack(SERVER_OPERATE_NOTIFY);
				unicast(m_Seats[j].m_Player, ppack.data);
				log.debug("%s:AckOpNotify:%s .\n", __FUNCTION__, stNotify.DebugString().c_str());
				if (i == j)
				{
					//保存操作提示录像
					SaveOperateNotifyVideo();
				}
			}
		}
		if (m_Seats[i].m_bUserOperate != 0)
		{
			log.info("%s tid:%s uid:%d chairid:%d Operate:%d, bOutCard:%d totalop:%d \n", 
			__FUNCTION__, tid.c_str(), GetPlayerUid(i), i, m_Seats[i].m_bUserOperate, m_CurOutCard, m_bMultiOpUserNum);
		}		
	}	
	
	if (bIsChangeUser)
	{		
		m_CurSeat = NextChair(bOutChairID);
		memset(m_bAddFanFlag, 0, sizeof(m_bAddFanFlag));
		OnUserGetCard(m_CurSeat);
	}
	else 
	{
		//开启操作时钟 TODO		
		for (_uint8 c = NextChair(bOutChairID); c != bOutChairID; c = NextChair(c))
		{			
			if (m_Seats[c].m_bUserOperate != TYPE_NULL)
			{
				set_operator_timer(c);
			}
		}
	}
	return bIsChangeUser;
}

void Table::UserOperatePass(_uint8 ChairID, int iOpType, _uint8 bCard)
{
	log.info("%s tid:%s uid:%d ChairID:%d, iOpType:%d, bCard:%d \n", __FUNCTION__, tid.c_str(), GetPlayerUid(ChairID), ChairID, iOpType, bCard);
	BroadcastOperateResult(ChairID, iOpType, bCard);
	if (ChairID != m_CurSeat)
	{
		m_bAddFanFlag[ChairID] = false;
		m_Seats[ChairID].m_bUserOperate = TYPE_NULL;
	}
	if (ChairID == m_CurSeat)
	{
		m_Seats[ChairID].m_bUserOperate &= ~(TYPE_CHI_PENG_GANG | TYPE_HU );
	}
}

void Table::UserOperateHu(_uint8 ChairID, _uint8 bHuCard)
{
	log.info("%s tid:%s uid:%d ChairID:%d hu FanNum:%d \n", __FUNCTION__, tid.c_str(), GetPlayerUid(ChairID), ChairID, m_HuFanNums[ChairID]);
	_uint8 bCard = 0xFF;
	if (bHuCard == 0xFF)
	{
		bCard = m_CurCard;
	}
	else
	{
		bCard = bHuCard;
	}
	
	BroadcastOperateResult(ChairID, TYPE_HU, bCard);
	if (!(m_Seats[ChairID].m_bUserOperate & TYPE_HU))
	{
		log.error("hu error, hu chairid:%d, m_bUserOperate[%d]:%d \n", ChairID, ChairID,m_Seats[ChairID].m_bUserOperate);
		return ;
	}	
	
	m_bIsHu[ChairID] = true;	
	int userscore[GAME_PLAYER] = { 0 };
	Calc_Hu(ChairID, bCard, userscore);	

	for (int i = 0; i < m_GamePlayer; i++)
	{
		m_stHuScore[i].iTotalScore += userscore[i];
	}
	//如果没有保存胡牌信息，重新获取
	
	proto::game::AckUserHu stHu;
	stHu.set_huchairid(ChairID);
	if (m_CurSeat == ChairID)
	{
		stHu.set_pashuchairid(ChairID);
	}
	else
	{
		stHu.set_pashuchairid(m_CurSeat);
	}
	for (int i = 0; i < m_GamePlayer; i++)
	{
		stHu.add_score(userscore[i]);		
	}

	Ppacket ppack;
	stHu.SerializeToString(&ppack.body);
	ppack.pack(SERVER_USER_HU);
	broadcast(NULL, ppack.data);

	m_Seats[ChairID].m_bUserOperate = TYPE_NULL;
	
	int nscore[GAME_PLAYER] = {0};
	SaveOperateVideo(TYPE_HU, ChairID, stHu.pashuchairid(), bCard, nscore);
	//保存录像
	save_video_hu(ChairID, stHu.pashuchairid(), userscore);
}

bool Table::CheckUserOperator(_uint8 ChairID, int iOpType, _uint8 bCard)
{
	if ((iOpType != TYPE_NULL && !(m_Seats[ChairID].m_bUserOperate & iOpType) && iOpType != TYPE_PASS)
		|| (iOpType == TYPE_PASS && m_Seats[ChairID].m_bUserOperate == TYPE_NULL))
	{
		log.error("%s error uid:%d chairid:%d iOpType:0x%x m_bUserOperate:0x%x\n", __FUNCTION__, GetPlayerUid(ChairID), ChairID, iOpType,m_Seats[ChairID].m_bUserOperate);
		OperateError(ChairID, proto::game::E_OPER_CODE_ERROR);
		return false;
	}
	if (Is_ChiPengGang(iOpType) && (bCard == 0x0 || bCard == 0xff))
	{
		log.error("%s error uid:%d chairid:%d iOpType:0x%x card 0x%02x!\n", __FUNCTION__, GetPlayerUid(ChairID), ChairID, iOpType, bCard);
        OperateError(ChairID, proto::game::E_OPER_CARD_ERROR);
		return false;
	}
	if ((iOpType != TYPE_PASS) && ( iOpType & (TYPE_CHI | TYPE_PENG)) && (bCard != m_CurOutCard))
	{
		log.error("%s error uid:%d chairid:%d iOpType:0x%x card 0x%02x outcard:0x%02x!\n", __FUNCTION__, GetPlayerUid(ChairID), ChairID, iOpType, bCard, m_CurOutCard);
        OperateError(ChairID, proto::game::E_OPER_CARD_ERROR);
		return false;
	}
	if (m_Seats[ChairID].m_bUserOperate == TYPE_NULL)
	{
		log.error("%s error uid:%d chairid:%d iOpType:0x%x m_bUserOperate:0x%x\n", __FUNCTION__, GetPlayerUid(ChairID), ChairID, iOpType, m_Seats[ChairID].m_bUserOperate);
        OperateError(ChairID, proto::game::E_OPER_CODE_ERROR);
		return false;
	}
	if (m_bRobWanGang && ChairID == m_WangGangPlayer)
	{
		log.error("%s rob gang error uid:%d chairid:%d iOpType:0x%x m_bUserOperate:0x%x, m_bRobWanGang:%d, m_WangGangPlayer:%d \n", __FUNCTION__, GetPlayerUid(ChairID), ChairID, iOpType, m_Seats[ChairID].m_bUserOperate, m_bRobWanGang, m_WangGangPlayer);
        OperateError(ChairID, proto::game::E_OPER_REPEAT_ERROR);
		return false;
	}
	if (iOpType != TYPE_PASS && iOpType & (TYPE_CHI | TYPE_PENG | TYPE_GANG))
	{
    	//遍历操作记录，检查操作是否合法
        for (int i = 0; i < m_OperateRecord[ChairID].m_Chi.Size(); ++i)
        {
            if (bCard == m_OperateRecord[ChairID].m_Chi[i].cbCard && iOpType == m_OperateRecord[ChairID].m_Chi[i].nState)
            {
                m_OperateRecord[ChairID].Clear();
                return true;
            }
        }
    	for (int i = 0; i < m_OperateRecord[ChairID].m_GetGangInfo.Size(); ++i)
        {
            if (bCard == m_OperateRecord[ChairID].m_GetGangInfo[i].cbCard && iOpType == m_OperateRecord[ChairID].m_GetGangInfo[i].nState)
            {
                m_OperateRecord[ChairID].Clear();
                return true;
            }
        }
        for (int i = 0; i < m_OperateRecord[ChairID].m_GetPengGang.Size(); ++i)
        {
            if (bCard == m_OperateRecord[ChairID].m_GetPengGang[i].cbCard && iOpType == m_OperateRecord[ChairID].m_GetPengGang[i].nState)
            {
                m_OperateRecord[ChairID].Clear();
                return true;
            }
        }
    	
    	log.error("%s error uid:%d chairid:%d iOpType:0x%x m_bUserOperate:0x%x, bCard:0x%x, m_CurCard:0x%x \n", __FUNCTION__, GetPlayerUid(ChairID), ChairID, iOpType, m_Seats[ChairID].m_bUserOperate, bCard, m_CurCard);
        OperateError(ChairID, proto::game::E_OPER_CARD_ERROR);
        return false;
    }
    
    return true;
}


//用户操作
bool Table::OnUserOperateCard(Player* player)
{
	if(!CheckReqValid(player)){
		log.info("OnUserChangeCard error \n");
		return false;
	}
	_uint8 wChairID = player->seatid;
	proto::game::ReqOperateCard stReqOp;
	stReqOp.ParseFromString(player->client->ppacket.body);
	log.debug("%s tid:%s uid:%d stReqOp:%s .\n", __FUNCTION__, tid.c_str(), player->uid, stReqOp.DebugString().c_str());
	int cbOperateCode = stReqOp.operate_type();	//动作代码
	_uint8 cbCard = stReqOp.operate_card();			//操作的牌
	if(!CheckUserOperator(wChairID, cbOperateCode, cbCard)){
		log.error("%s error wChairID:%d m_bResponse:%d m_bUserOperate:0x%02x cbOperateCode:0x%02x tid:%s .\n",
			 __FUNCTION__, wChairID, m_Seats[wChairID].m_bResponse, m_Seats[wChairID].m_bUserOperate, cbOperateCode, tid.c_str());
		return false;
	}
	//记录玩家操作
	SaveOperateResVideo(wChairID,cbOperateCode);
	//关闭操作计时
	stop_operator_timer(wChairID);
	//如果是多操作
	if (m_bMultiOpUserNum > 1)
	{
		DoMultiOperate(wChairID, cbOperateCode, cbCard);
	}
	else 
	{   
		if (m_bRobWanGang)  //抢杠胡操作优先级
		{
			RobGangPriority(wChairID, cbCard, cbOperateCode);
			SerializeTableData();
			return true;
		}
		DoSimpleOperate(wChairID, cbOperateCode, cbCard);
		//判断是下一玩家摸牌，或者当前玩家出牌		
	}
	SerializeTableData();
	return true;
}

void Table::DoSimpleOperate(_uint8 ChairID, int iOpType, _uint8 bCard)
{
	log.info("DoSimpleOperate uid:%d chairid:%d iOptype:0x%x bCard:0x%x \n", GetPlayerUid(ChairID), ChairID, iOpType, bCard);
	
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
			OnUserGetCard(ChairID, true);
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
		if (m_CurSeat == ChairID)
		{
				//开启出牌时钟
			set_out_timer(ChairID);
		}
		else
		{				
			m_bAddFanFlag[m_CurOutSeat] = false;
			if ( m_CurSeat == INVALID_CHAIRID)
			{
				OnUserGetCard(m_BankerSeat);
			}
			else
			{
				m_CurSeat = NextChair(m_CurSeat);
				OnUserGetCard(m_CurSeat);
			}
		}
		break;
		case TYPE_HU:
		UserOperateHu(ChairID);

		GameEnd();
		break;
		default:
		break;
	}	
}

//广播操作
void Table::BroadcastOperateResult(_uint8 bOpChairID, int iOpType, _uint8 bOpCard)
{
    //广播发送操作结果	
	proto::game::AckBcOpResult stOpresult;
	stOpresult.set_chairid(bOpChairID);
	stOpresult.set_outcardchairid(m_CurSeat);
	stOpresult.set_optype(iOpType);
	stOpresult.set_card(bOpCard);

	SetRobGangHu_Flag(bOpChairID, iOpType, stOpresult);
		
	Ppacket ppack;
	stOpresult.SerializeToString(&ppack.body);
	ppack.pack(SERVER_OPERATE_RESULT);
	broadcast(NULL, ppack.data);
}

void Table::ResetAction()
{
	for(_uint8 i = 0; i < m_GamePlayer; ++i){
		m_Seats[i].ResetAction();
	}
}

void Table::GameEnd()
{
	//摸完最后一张牌才算是结束
	MJ_user_buffer UserID;
	_uint8 bDraw = (MultiHu(UserID) == 0) ? 1 : 0;//结束状态，1表示流局，0表示胡牌结束	
	
	//单发结算协议	
	proto::game::AckGameEnd stGameend;
	HuScoreSet hu_score;
	
	if (bDraw)
	{
		Score_GameEndDraw(hu_score, stGameend);
	}
	else
	{
		Score_GameEndSucc(hu_score, stGameend);
	}
	stGameend.set_endstate(bDraw);

	Proto_GameEndInfo_AddFan(UserID, stGameend);

	int UserScore[GAME_PLAYER] = {0};
	for (int i = 0; i < m_GamePlayer; i++)
	{
		proto::game::HandCards *pHandcard = stGameend.add_usercard();
		Fill_HandCard(i, pHandcard);

		Proto_GameEndInfo_GangInfo(i, stGameend);

		UserScore[i] = hu_score[i].iTotleScore;

		stGameend.add_score(UserScore[i]);
	}

	log.fatal("%s tid:%s bDraw:%d score[%d,%d,%d,%d]\n", __FUNCTION__, tid.c_str(), bDraw, UserScore[0], UserScore[1], UserScore[2], UserScore[3]);

	tagUserScore userscore[GAME_PLAYER];
	for (int i = 0; i < m_GamePlayer; i++)
	{
		if (m_Seats[i].m_Player != NULL)
		{  
			userscore[i].uid = m_Seats[i].m_Player->uid;
			userscore[i].score = UserScore[i] ;
			if (UserScore[i]  > 0)
			{
				m_Seats[i].m_Player->update_info(UserScore[i], 1, 1, 0);
			}
			else
			{
				m_Seats[i].m_Player->update_info(UserScore[i], 1, 0, 0);
			}
			stGameend.add_money(m_Seats[i].m_Player->money); 
		}
	}


	Ppacket ppack;
	stGameend.SerializeToString(&ppack.body);
	ppack.pack(SERVER_GAME_END_BC);
	broadcast(NULL, ppack.data);
	log.debug("%s AckGameEnd:%s .\n", __FUNCTION__, stGameend.DebugString().c_str());
	//保存录像
	save_video_data(SERVER_GAME_END_BC, stGameend.SerializeAsString());
	//更新数据
	update_user_info_to_datasvr(UserScore);
	//保存到统计信息与战绩信息
	CalcRecordInfo(UserID, hu_score);
	//发送录像
	ev_timer_again(phz.loop, &send_videodata_timer);
	GameEndReset();
	m_cbCurTimer = 6;
	m_State = GAME_FREE;
	TurnBanker();
	ev_timer_again(phz.loop, &preready_timer);
	m_cbCurTimer = 2;
	SerializeTableData();
}

void Table::CalcRecordInfo(const MJ_user_buffer& UserID, HuScoreSet& hu_score)
{
	if (UserID.Size() == 1)
	{
		_uint8 ChairID = UserID[0];
		m_Seats[ChairID].m_SettleInfo.m_HuCnt++;
		if (m_stHuScore[ChairID].stHu[0].bType == HU_ZIMO)
		{
			m_Seats[ChairID].m_SettleInfo.m_ZimoCnt++;
		}
		else
		{	    
			_uint8 PaoID = m_stHuScore[ChairID].stHu[0].bPasHuChairID;
			m_Seats[PaoID].m_SettleInfo.m_DianPaoCnt++;
		}

	}
	else if (UserID.Size() > 1)
	{
		for (int i = 0; i < UserID.Size(); i++)
		{
			_uint8 ChairID = UserID[i];
			if (m_stHuScore[ChairID].stHu[0].bType == HU_PAOHU)
			{
				m_Seats[ChairID].m_SettleInfo.m_HuCnt++;

				_uint8 PaoID = m_stHuScore[ChairID].stHu[0].bPasHuChairID;
				m_Seats[PaoID].m_SettleInfo.m_DianPaoCnt++;
			}
		}
	}	
	for (int i = 0; i < m_GamePlayer; i++)
	{
		if (m_Seats[i].m_Player != NULL)
		{
		    if (hu_score[i].iTotleScore > 0)
		    {
		        m_Seats[i].m_SettleInfo.m_WinCnt++;
		    }
		    else
		    {
		        m_Seats[i].m_SettleInfo.m_LoseCnt++;
		    }
			m_Seats[i].m_SettleInfo.m_wTotalScore += hu_score[i].iTotleScore;
			if (m_Seats[i].m_SettleInfo.m_wBestScore < hu_score[i].iTotleScore)
			{
                m_Seats[i].m_SettleInfo.m_wBestScore = hu_score[i].iTotleScore;
			}
		}
	}	

    //战绩
	tagGain stGain;
	for( _uint8 i = 0; i < GAME_PLAYER; ++i ){
		stGain.Gain[i] = hu_score[i].iTotleScore;
	}
	m_vecGains.push_back(stGain);

//	for (int i = 0; i < m_GamePlayer; i++)
//	{
//		if (iTotalScore[i] > 0)
//		{
//			m_Seats[i].m_SettleInfo.m_wincount++;
//		}
//	}		
	
//	for (int i = 0; i < m_GamePlayer; i++)
//	{
//		m_totalrecordinfo.gang_times[i] += m_stGang[i].Gang.Size();
//	}
//	for (int i = 0; i < m_GamePlayer; i++)
//	{
//		for (int j = 0; j < m_stGang[i].Gang.Size(); j++)
//		{
//			if (m_stGang[i].Gang[j].iType == TYPE_ZHIGANG)
//			{
//				m_totalrecordinfo.dian_gang_times[m_stGang[i].Gang[j].ChairID]++;
//			}
//		}
//	}
}


void Table::TurnBanker()
{
	m_BankerSeat = NextChair(m_BankerSeat);
}

void Table::GameEndReset()
{  

	memset(m_stGang, 0, sizeof(m_stGang));
	memset(m_stHuScore, 0, sizeof(m_stHuScore));
	memset(m_bIsHu, 0, sizeof(m_bIsHu));

	m_State = GAME_FREE;

	m_bRobWanGang = false;
	m_bRobGangHuJudge = false;
	m_WangGangPlayerOpType = TYPE_NULL;
	m_PassHuPlayer.Clear();
	for (int i = 0; i < m_GamePlayer; i++)
	{
		m_Seats[i].m_bUserOperate = 0;
		if (m_Seats[i].m_Player == NULL)
		{
			continue;
		}
	}
	log.info("%s player uid:[%d,%d,%d,%d] game reset\n", __FUNCTION__,
		GetPlayerUid(0), GetPlayerUid(1), GetPlayerUid(2), GetPlayerUid(3));
	ev_timer_stop(phz.loop, &m_Outcard_Timer);
	ev_timer_stop(phz.loop, &m_Operate_Timer0);
	ev_timer_stop(phz.loop, &m_Operate_Timer1);
	ev_timer_stop(phz.loop, &m_Operate_Timer2);
	ev_timer_stop(phz.loop, &m_Operate_Timer3);

	ev_timer_stop(phz.loop, &m_Trusteeshit_Out_Delay_Timer);
	ev_timer_stop(phz.loop, &m_Trusteeshit_Ope_Delay_Timer0);
	ev_timer_stop(phz.loop, &m_Trusteeshit_Ope_Delay_Timer1);
	ev_timer_stop(phz.loop, &m_Trusteeshit_Ope_Delay_Timer2);
	ev_timer_stop(phz.loop, &m_Trusteeshit_Ope_Delay_Timer3);
}

void Table::Fill_HandCard(_uint8 ChairID, proto::game::HandCards* pHandcard)
{
	pHandcard->set_changeablecardslen(m_Seats[ChairID].m_HandCard.ChangeableCardsLen);
	for (int j = 0; j < m_Seats[ChairID].m_HandCard.ChangeableCardsLen; j++)
	{
		pHandcard->add_changeablecards(m_Seats[ChairID].m_HandCard.ChangeableCards[j]);		
	}
	for (int j = 0; j < m_Seats[ChairID].m_HandCard.FixedCardsLen; j++)
	{
		proto::game::FixedCard *pFixedcard = pHandcard->add_stfixedcards();
		Fill_FixedCard(ChairID, j, pFixedcard);
	}
	pHandcard->set_fixedcardslen(m_Seats[ChairID].m_HandCard.FixedCardsLen);
}

void Table::Fill_FixedCard(_uint8 ChairID, _uint8 Pos, proto::game::FixedCard* pstFix)
{
	_uint8 cbCard = 0xFF;
	if (m_Seats[ChairID].m_HandCard.FixedCards[Pos].state == TYPE_LEFT_CHI)
	{
		cbCard = m_Seats[ChairID].m_HandCard.FixedCards[Pos].c[0];
	}
	else if (m_Seats[ChairID].m_HandCard.FixedCards[Pos].state == TYPE_CENTER_CHI)
	{
		cbCard = m_Seats[ChairID].m_HandCard.FixedCards[Pos].c[1];
	}
	else if (m_Seats[ChairID].m_HandCard.FixedCards[Pos].state == TYPE_RIGHT_CHI)
	{
		cbCard = m_Seats[ChairID].m_HandCard.FixedCards[Pos].c[2];
	}
	else
	{
		cbCard = m_Seats[ChairID].m_HandCard.FixedCards[Pos].CardData;
	}
	pstFix->set_carddata(cbCard);
	pstFix->set_state(m_Seats[ChairID].m_HandCard.FixedCards[Pos].state);
	pstFix->set_chairid(m_Seats[ChairID].m_HandCard.FixedCards[Pos].chairID);
}

void Table::Score_GameEndDraw(HuScoreSet& score, proto::game::AckGameEnd& stGameEnd)
{
	if (m_conf.m_bHuangGang)
	{
		memset(m_stGang, 0, sizeof(m_stGang));
	}
}

void Table::Score_GameEndSucc(HuScoreSet& score, proto::game::AckGameEnd& stGameEnd)
{

	int iTotalScore[GAME_PLAYER] = { 0 };
	int iBaseScore[GAME_PLAYER][GAME_PLAYER];
	int iScore[GAME_PLAYER][GAME_PLAYER];
	memset(iBaseScore, 0, sizeof(iBaseScore));
	memset(iScore, 0, sizeof(iScore));
	for (int i = 0; i < m_GamePlayer; i++)
	{
		Calc_BaseScore(i, iBaseScore[i]);
	}
	for (int i = 0; i < m_GamePlayer; i++)
	{		
		Calc_HuScore(i, iBaseScore[i], iScore[i]);		
	}

	for (int i = 0; i < m_GamePlayer; i++)
	{
		//Calc_Score(iTotalScore, iScore[i]);
		for (int j = 0; j < m_GamePlayer; j++)
		{
			m_stHuScore[j].iTotalScore += iScore[i][j];
		}
	}
	for (int i = 0; i < m_GamePlayer; i++)
	{
		score[i].iTotleScore = iTotalScore[i];
	}
}

void Table::Calc_BaseScore(_uint8 ChairID, int iBaseScore[GAME_PLAYER])
{

}

void Table::Calc_HuScore(_uint8 ChairID, int iBaseScore[GAME_PLAYER], int iScore[GAME_PLAYER])
{

}

int Table::GetPlayerUid(int ChairID)
{
	return GetPlayerUid((_uint8)ChairID);
}

int Table::GetPlayerUid(_uint8 ChairID)
{		
	if ((ChairID < m_GamePlayer) && (m_Seats[ChairID].m_Player != NULL))
	{
		return m_Seats[ChairID].m_Player->uid;
	}				
	return 0;
}
int Table::GetPlayerUid(const Player* player)
{
	if (player != NULL)
	{
		return player->uid;
	}
	return 0;
}

int Table::pow(int n)
{
    int res = 1;
    for (int i = 0; i < n; ++i)
    {
        res *= 2;
    }
    return res;
}

char* Table::CatHandCard(const HandCards& pHc, _uint8 ChairID)
{
	static char retBuf[0x100];
	retBuf[0] = '\0';
	for (int i = 0; i < pHc.ChangeableCardsLen; i++)
	{
		char tmp[0x10] = { 0 };
		sprintf(tmp, "%02x,", pHc.ChangeableCards[i]);
		strcat(retBuf, tmp);
	}
	return retBuf;
}

int Table::MultiHu(MJ_user_buffer& id)const
{
	id.Clear();
	for (int i = 0; i < m_GamePlayer; i++)
	{
		if (m_bIsHu[i])
		{
			id.Add(i);
		}
	}
	return id.Size();
}

bool Table::IsQuanqiuren(const HandCards& pHc, MJ_fan_buffer& oFan, int& iOpType)
{
	if (!m_conf.m_bNoSupport_Quanqiuren &&	m_GameLogic.IsQuanqiuren(&pHc))
	{
		oFan.Add(MJ_FAN_TYPE_QUANQIUREN);
		iOpType |= TYPE_HU;
		return true;
	}
	return false;
}

bool Table::Is7Dui(_uint8 ChairID, HandCards& pHc, MJ_fan_buffer& oFan)
{
	if (m_conf.m_bHu7dui && m_GameLogic.Is7Dui(&pHc))
	{		
		oFan.Add(MJ_FAN_TYPE_QIDUI);
		return true;
	}
	return false;
}

bool Table::Is13Yao(HandCards& pHc, MJ_fan_buffer& oFan, MJ_win_pattern& pattern)
{
	if (m_conf.m_bHu13yao && m_GameLogic.Is13Yao(&pHc, INVALID_MJ_CARD, pattern.kan, pattern.jiang))
	{
		oFan.Add(MJ_FAN_TYPE_13YAO);
		return true;
	}
	return false;
}

int  Table::Get_Card_Left_Num(_uint8 ChairID, _uint8 cbCard)
{
	int nNum = 0;

	nNum = m_RepertoryCard.Find(cbCard);

	for (int k = 0; k < m_GamePlayer; k++)
	{   
		if (k == ChairID)
		{
			continue;
		}

		for (int j = 0; j < m_Seats[k].m_HandCard.ChangeableCardsLen; j++)
		{
			if (cbCard == m_Seats[k].m_HandCard.ChangeableCards[j])
			{
				nNum++;
			}
		}
	}
	return nNum;
}
