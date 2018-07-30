#include "table.h"
#include "log.h"

extern PHZ phz;
extern Log log;
//重启定时器
void Table::AgainTimer()
{
	switch (m_cbCurTimer){
	case 1:
		ev_timer_again(phz.loop, &start_timer);
		break;
	case 2:
		ev_timer_again(phz.loop, &preready_timer);
		break;
	case 3:
		ev_timer_again(phz.loop, &dispatch_show_timer);
		break;
	case 4:
		ev_timer_again(phz.loop, &dispatch_delay_timer);
		break;
	case 5:
		ev_timer_again(phz.loop, &dissolve_room_timer);
		break;
	case 6:
		ev_timer_again(phz.loop, &send_videodata_timer);
		break;
	case 7:
		ev_timer_again(phz.loop, &m_Outcard_Timer);
		break;
	case 8:
		ev_timer_again(phz.loop, &m_Operate_Timer);
		break;
	case 9:
		ev_timer_again(phz.loop, &first_getcard_timer);
		break;
	default:
		break;
	}
}

//保存不变的牌桌信息
void Table::InitProtoTable()
{
	m_pTableInfo.Clear();
	m_pTableInfo.set_take_in(m_TakeIn);
	m_pTableInfo.set_play_id(m_PlayId);
	m_pTableInfo.set_room_user_id(m_RoomUserId);
	m_pTableInfo.set_stand_money(stand_money);
	m_pTableInfo.set_base_money(base_money);
	m_pTableInfo.set_game_player(m_GamePlayer);
	m_pTableInfo.set_tid(tid);
	m_pTableInfo.set_is_piao(m_bPiao);
	if(m_bPiao){
		for(std::set<int>::iterator it =  m_PiaoScore.begin(); it != m_PiaoScore.end(); ++it){
			m_pTableInfo.add_piao_count(*it);
		}
	}
	m_pTableInfo.set_max_count(m_MaxCount);
}

//保存变动的牌桌信息
void Table::SaveProtoTable()
{
	InitProtoTable();
	m_pTableInfo.set_state(m_State);
	m_pTableInfo.set_current_num(m_CurTurn);
	m_pTableInfo.set_all_num(m_TotalTurn);
	m_pTableInfo.set_banker_user(m_BankerSeat);
	m_pTableInfo.set_current_user(m_CurSeat);
	m_pTableInfo.set_current_card(m_CurCard);
	m_pTableInfo.set_current_outcard_user(m_CurOutSeat);
	m_pTableInfo.set_current_outcard(m_CurOutCard);
	m_pTableInfo.set_is_out_card(m_bOutCard);
	m_pTableInfo.set_is_dispatch(m_bDispatch);
	m_pTableInfo.set_card_operated(m_bCardOperated);
	m_pTableInfo.set_is_first_card(m_bFirstCard);
	m_pTableInfo.set_room_status(m_RoomStatus);
	m_pTableInfo.set_innings(m_strInnings);
	m_pTableInfo.set_video_str(m_strVideo);
	m_pTableInfo.set_b_can_hu(m_bCanHu);
	m_pTableInfo.set_cur_timer(m_cbCurTimer);
	m_pTableInfo.set_dissovle_state(dissovle_state);
	m_pTableInfo.set_is_dissolved(is_dissolved);
    //dissolve room recover
    proto::game::tagDissolveRoomInfo* pstDissolveInfo = m_pTableInfo.mutable_dissolve_room_info();
	if (dissovle_state == 1){
		pstDissolveInfo->set_uid(dissolve_applyer);
		for (std::map<int, Player*>::iterator it = voters_players.begin(); it != voters_players.end(); it++){
			pstDissolveInfo->add_voters_uid(it->first);
		}
	}	
	for (int i = 0; i < m_room_config.Size(); i++)
	{
		m_pTableInfo.add_roomconfigid(m_room_config[i]);
	}
	for (int i = 0; i < m_Start_Shai.Num; i++)
	{
		m_pTableInfo.add_shai_set(m_Start_Shai.Shai[i]);
	}
	m_pTableInfo.set_base_score(base_score);
	m_pTableInfo.set_bgangcalcimme(m_bGangCalcImme);
	m_pTableInfo.set_laizicard(m_LaiziCard);
	m_pTableInfo.clear_gains();
	for(std::vector<tagGain>::iterator iter = m_vecGains.begin(); iter != m_vecGains.end(); ++iter){
		proto::game::tagGain* pstGain = m_pTableInfo.add_gains();
		for(_uint8 i = 0; i < GAME_PLAYER; ++i){
			pstGain->add_score((*iter).Gain[i]);
		}
	}

    //多人操作info
    m_pTableInfo.set_first_operate(m_bMulitFirstOperate);
	
	proto::game::LowLevelOperate *pLowLevelOperate = m_pTableInfo.mutable_recordmultiop();
	pLowLevelOperate->set_op_type(m_bRecordMultiOp.iOpType);
	pLowLevelOperate->set_chair_id(m_bRecordMultiOp.ChairID);
	pLowLevelOperate->set_b_card(m_bRecordMultiOp.bCard);		

    m_pTableInfo.set_done_op_num(m_bMultiDoneOpNum);
    m_pTableInfo.set_op_user_num(m_bMultiOpUserNum);
    m_pTableInfo.set_hu_ope_count(m_bMulitHuOpeCount);
    m_pTableInfo.set_op_hu_num(m_bMultiOpHuNum);

    for (int i = 0; i < m_PassHuPlayer.Size(); i++)
	{
		m_pTableInfo.add_pass_hu_player(m_PassHuPlayer[i]);
	}	

	//抢杠胡
	m_pTableInfo.set_rob_operate_num(m_RobOperateNum);
	m_pTableInfo.set_rob_hu_num(m_RobHuNum);
	m_pTableInfo.set_wang_gang_player(m_WangGangPlayer);
	m_pTableInfo.set_wanggangplayeroptype(m_WangGangPlayerOpType);
	for (int i = 0; i < m_GamePlayer; i++)
	{
		m_pTableInfo.add_rob_hu_done_op(m_RobHuDoneOp[i]);
		m_pTableInfo.add_add_fan_flag(m_bAddFanFlag[i]);
	}	
	
	m_pTableInfo.set_rob_wangangcard(m_RobWanGangCard);
	m_pTableInfo.set_rob_wangang(m_bRobWanGang);
	m_pTableInfo.set_rob_hu(m_bRobHu);
	m_pTableInfo.set_robgang_huhudge(m_bRobGangHuJudge);

	//胡牌信息	
	for (int i = 0; i < m_GamePlayer; i++)
	{
        m_pTableInfo.add_is_hu(m_bIsHu[i]);
        m_pTableInfo.add_hu_fan_nums(m_HuFanNums[i]);

        proto::game::tagGangScoreInfoRecover *pstGangInfo = m_pTableInfo.add_gang_info();
        pstGangInfo->set_total_score(m_stGang[i].iTotalScore);
        for (int j = 0; j < m_stGang[i].Gang.Size(); j++)
        {
            proto::game::tagGangScoreRecover *pstGangScore = pstGangInfo->add_gang_score();
            pstGangScore->set_card(m_stGang[i].Gang[j].Card);
            pstGangScore->set_chairid(m_stGang[i].Gang[j].ChairID);
            pstGangScore->set_type(m_stGang[i].Gang[j].iType);
        }
	
		proto::game::tagUserHuInfo *pstUserHu = m_pTableInfo.add_huinfo();
		for (int j = 0; j < m_stHuScore[i].stHu.Size(); j++)
		{
			proto::game::tagHu *pstHu = pstUserHu->add_hu();
			pstHu->set_hucard(m_stHuScore[i].stHu[j].bHuCard);
			pstHu->set_pashuchiarid(m_stHuScore[i].stHu[j].bPasHuChairID);
		}

		proto::game::tagFanInfoRecover *pstFanInfo = m_pTableInfo.add_fan_info();
		for (int j = 0; j < m_oFan[i].Size(); j++)
		{
		    pstFanInfo->add_fan(m_oFan[i][j]);
		}
    }  

	proto::game::CardPool *pCardPool = m_pTableInfo.mutable_all_cardpool();
	log.debug("%s: m_RepertoryCard.MaxCount():%d, m_RepertoryCard.m_Current:%d, m_Last:%d \n", 
	        __FUNCTION__, m_RepertoryCard.MaxCount(), m_RepertoryCard.Current(), m_RepertoryCard.Last());
	for(int i = 0;i < m_RepertoryCard.MaxCount();i++)
	{
		pCardPool->add_all_card(m_RepertoryCard.GetAllCard(i));
	}
	pCardPool->set_seedid(m_RepertoryCard.SeedID());
	pCardPool->set_mode(m_RepertoryCard.Mode());
	pCardPool->set_maxcount(m_RepertoryCard.MaxCount());
	pCardPool->set_count(m_RepertoryCard.Count());
	pCardPool->set_current(m_RepertoryCard.CurCard());
	pCardPool->set_last(m_RepertoryCard.Last());
	
    m_pTableInfo.set_m_bsupportchi(m_conf.m_bSupportChi);
    m_pTableInfo.set_m_bsupportwind(m_conf.m_bSupportWind);
    m_pTableInfo.set_m_bsupportting(m_conf.m_bSupportTing);
    m_pTableInfo.set_m_bzimohu(m_conf.m_bZimoHu);
    m_pTableInfo.set_m_byipaoduoxiang(m_conf.m_bYiPaoDuoXiang);
    m_pTableInfo.set_m_bqiangganghu(m_conf.m_bQiangGangHu);
    m_pTableInfo.set_m_bqiangzhigang(m_conf.m_bQiangZhiGang);
    m_pTableInfo.set_m_bqiangangang(m_conf.m_bQiangAnGang);
    m_pTableInfo.set_m_bgangshangpao(m_conf.m_bGangShangPao);
    m_pTableInfo.set_m_bhuanggang(m_conf.m_bHuangGang);
    m_pTableInfo.set_m_bzhuangxian(m_conf.m_bZhuangXian);
    m_pTableInfo.set_m_bforcehuqiangganghu(m_conf.m_bForceHuQiangGangHu);
    m_pTableInfo.set_m_bfourcehumutilhu(m_conf.m_bFourceHuMutilHu);
    m_pTableInfo.set_m_angangpoint(m_conf.m_AnGangPoint);
    m_pTableInfo.set_m_wangangpoint(m_conf.m_WanGangPoint);
    m_pTableInfo.set_m_zhigangpoint(m_conf.m_ZhiGangPoint);
    m_pTableInfo.set_m_bhu7dui(m_conf.m_bHu7dui);
    m_pTableInfo.set_m_bhu13yao(m_conf.m_bHu13yao);
    m_pTableInfo.set_m_bnosupport_quanqiuren(m_conf.m_bNoSupport_Quanqiuren);

    for (int i = 0; i < m_GamePlayer; ++i)
    {
        proto::game::tagOperateRecord *pOperateRecord = m_pTableInfo.add_operate_record();
        for (int j = 0; j < m_OperateRecord[i].m_Chi.Size(); ++j)
        {
            proto::game::tagOpInfo *pOpInfo = pOperateRecord->add_opinfo_chi();
            pOpInfo->set_card(m_OperateRecord[i].m_Chi[j].cbCard);
            pOpInfo->set_state(m_OperateRecord[i].m_Chi[j].nState);
        }
        for (int k = 0; k < m_OperateRecord[i].m_GetGangInfo.Size(); ++k)
        {
            proto::game::tagOpInfo *pOpInfo = pOperateRecord->add_opinfo_gang();
            pOpInfo->set_card(m_OperateRecord[i].m_GetGangInfo[k].cbCard);
            pOpInfo->set_state(m_OperateRecord[i].m_GetGangInfo[k].nState);
        }
        for (int l = 0; l < m_OperateRecord[i].m_GetPengGang.Size(); ++l)
        {
            proto::game::tagOpInfo *pOpInfo = pOperateRecord->add_opinfo_penggang();
            pOpInfo->set_card(m_OperateRecord[i].m_GetPengGang[l].cbCard);
            pOpInfo->set_state(m_OperateRecord[i].m_GetPengGang[l].nState);
        }
    }
	
	SaveProtoPlayers();
}

//保存玩家信息即座位信息
void Table::SaveProtoPlayers()
{
	m_pTableInfo.clear_players();
	for(_uint8 i = 0; i < m_GamePlayer; ++i){
		proto::game::tagPlayers *stPlayer = m_pTableInfo.add_players();
		Player *player = m_Seats[i].m_Player;
		if(player == NULL) continue;
		stPlayer->set_uid(m_Seats[i].m_Uid);
		stPlayer->set_ready(m_Seats[i].m_bReady);
		stPlayer->set_tid(player->tid);
		stPlayer->set_seatid(player->seatid);
		stPlayer->set_offline(player->is_offline);
		stPlayer->set_skey(player->skey);
		stPlayer->set_name(player->name);
		stPlayer->set_sex(player->sex);
		stPlayer->set_avatar(player->avatar);
		stPlayer->set_money(player->money);
		stPlayer->set_dissolve_cnt(player->dissolve_cnt);
		stPlayer->set_dissovle_state(player->dissovle_state);
		stPlayer->set_all_piao_score(m_Seats[i].m_SettleInfo.m_wAllPiaoScore);
		stPlayer->set_zimo_count(m_Seats[i].m_SettleInfo.m_ZimoCnt);
		stPlayer->set_hu_count(m_Seats[i].m_SettleInfo.m_HuCnt);
		stPlayer->set_dian_pao_count(m_Seats[i].m_SettleInfo.m_DianPaoCnt);
		stPlayer->set_best_score(m_Seats[i].m_SettleInfo.m_wBestScore);
		stPlayer->set_total_score(m_Seats[i].m_SettleInfo.m_wTotalScore);
		stPlayer->set_win_count(m_Seats[i].m_SettleInfo.m_WinCnt);
		stPlayer->set_lose_count(m_Seats[i].m_SettleInfo.m_LoseCnt);
		proto::game::HandCards *pstHc = stPlayer->mutable_hand_cards();
		//添加手牌
		Fill_HandCard(i, pstHc);
		for (_uint8 j = 0; j < m_Seats[i].m_bOutRecord.Size(); ++j){
			stPlayer->add_out_record(m_Seats[i].m_bOutRecord[j]);
		}
		stPlayer->set_user_operate(m_Seats[i].m_bUserOperate);
		stPlayer->set_piao(m_Seats[i].m_UserPiaoScore);
		stPlayer->set_operate_type(m_Seats[i].m_bMultiUserDoneOp);
		stPlayer->set_operate_card(m_Seats[i].m_bMultiUserDoneOpCard);
		stPlayer->set_out_cnt(m_Seats[i].m_OutCardCnt);
		stPlayer->set_b_resp(m_Seats[i].m_bResponse);
		stPlayer->set_getcardcount(m_Seats[i].m_bGetCardCount);
		proto::game::AckUserOvertimeState* pAckOverTimeState = stPlayer->mutable_user_overtime();
    	pAckOverTimeState->Clear();
    	pAckOverTimeState->set_chairid(i);
    	pAckOverTimeState->set_state(m_bOverTimeState[i]);
	}
}

int Table::SerializeTableData()
{
	SaveProtoTable();
	//压缩保存
	std::string data_str = "";
	m_pTableInfo.SerializeToString(&data_str);
	log.info("Serialize Table Data succ tid:%s size[%d].\n", tid.c_str(), data_str.size());
	_tint32 iRet = compress_str(data_str, data_str);
	if (iRet < 0){
		log.error("Serialize Table Data compress fail iRet[%d].\n", iRet);
		return -1;
	}
	phz.game->SaveDataToRedis(tid, data_str);
	return 0;
}

int Table::ParseTableData(std::string &data_str)
{
	//解压
	_tint32 iRet = 0;
	iRet = decompress_str(data_str, data_str);
	if (iRet < 0){
		log.error("Parse Table Data decompress fail iRet[%d] data size[%d].\n", iRet, data_str.size());
		return -1;
	}
	m_pTableInfo.ParseFromString(data_str);
	ParseProtoTable();
	InitLogic();
	//重启定时器
	AgainTimer();
	log.info("Parse Table Data Parse succ tid:%s.\n", tid.c_str());
	return 0;
}

void Table::ParseProtoTable()
{
	tid = m_pTableInfo.tid();
	m_TakeIn = m_pTableInfo.take_in();
	m_PlayId = m_pTableInfo.play_id();
	m_RoomUserId = m_pTableInfo.room_user_id();
	stand_money = m_pTableInfo.stand_money();
	base_money = m_pTableInfo.base_money();
	m_GamePlayer = m_pTableInfo.game_player();
	m_bPiao = m_pTableInfo.is_piao();
	if(m_bPiao){
		for(int i = 0; i < m_pTableInfo.piao_count_size(); ++i){
			m_PiaoScore.insert(m_pTableInfo.piao_count(i));
		}
	}
	m_MaxCount = m_pTableInfo.max_count();

	m_State = (State)m_pTableInfo.state();
	m_CurTurn = m_pTableInfo.current_num();
	m_TotalTurn = m_pTableInfo.all_num();
	m_BankerSeat = m_pTableInfo.banker_user();
	m_CurSeat = m_pTableInfo.current_user();
	m_CurCard = m_pTableInfo.current_card();
	m_CurOutSeat = m_pTableInfo.current_outcard_user();
	m_CurOutCard = m_pTableInfo.current_outcard();
	m_bOutCard = m_pTableInfo.is_out_card();
	m_bDispatch = m_pTableInfo.is_dispatch();
	m_bCardOperated = m_pTableInfo.card_operated();
	m_bFirstCard = m_pTableInfo.is_first_card();
	m_bGangCalcImme = m_pTableInfo.bgangcalcimme();
	base_score = m_pTableInfo.base_score();
	for (_uint8 i = 0; i < m_pTableInfo.roomconfigid_size(); ++i){
		m_room_config.Add(m_pTableInfo.roomconfigid(i));
	}	
	for (_uint8 i = 0; i < m_pTableInfo.shai_set_size(); ++i){
		m_Start_Shai.Shai[i] = m_pTableInfo.shai_set(i);		
	}	
	m_Start_Shai.Num = m_pTableInfo.shai_set_size();
	m_LaiziCard = m_pTableInfo.laizicard();
	//牌库恢复还需要处理
	//CardPool NowCardPool;
	_uint8 AllCard[POOL_CARD_MAX];
	const proto::game::CardPool &stCardPool = m_pTableInfo.all_cardpool();
	_uint8 seedid;
	E_POOL_MODE mode;
	_uint8 maxcount;
	_uint8 count;
	_uint8 current;
	_uint8 last;
	seedid = stCardPool.seedid();
	mode = (E_POOL_MODE)stCardPool.mode();
	maxcount = stCardPool.maxcount();
	count = stCardPool.count();
	current = stCardPool.current();
	last = stCardPool.last();
	for(_uint8 i = 0; i < stCardPool.all_card_size(); ++i)
	{
		AllCard[i] = stCardPool.all_card(i);
	}
	m_RepertoryCard.PushCardPool(AllCard, stCardPool.all_card_size(), seedid,mode,maxcount,count,current,last);
	
    //多人操作	
	m_bMulitFirstOperate = m_pTableInfo.first_operate();

	const proto::game::LowLevelOperate &recordmultiop = m_pTableInfo.recordmultiop();
	m_bRecordMultiOp.iOpType = recordmultiop.op_type();
	m_bRecordMultiOp.ChairID = recordmultiop.chair_id();
	m_bRecordMultiOp.bCard = recordmultiop.b_card();
	
	m_bMultiDoneOpNum = m_pTableInfo.done_op_num();
	m_bMultiOpUserNum = m_pTableInfo.op_user_num();
	m_bMulitHuOpeCount = m_pTableInfo.hu_ope_count();
	m_bMultiOpHuNum = m_pTableInfo.op_hu_num();

	for(_uint8 i = 0; i < m_pTableInfo.pass_hu_player_size(); ++i)
	{
		m_PassHuPlayer.Add(m_pTableInfo.pass_hu_player(i));
	}	

	//抢杠胡
	m_RobOperateNum = m_pTableInfo.rob_operate_num();
	m_RobHuNum = m_pTableInfo.rob_hu_num();
	m_WangGangPlayer = m_pTableInfo.wang_gang_player();
	m_WangGangPlayerOpType = m_pTableInfo.wanggangplayeroptype();
	for(_uint8 i = 0; i < m_pTableInfo.rob_hu_done_op_size(); ++i)
	{
		m_RobHuDoneOp[i] = m_pTableInfo.rob_hu_done_op(i);
	}
	for(_uint8 i = 0; i < m_pTableInfo.add_fan_flag_size(); ++i)
	{
		m_bAddFanFlag[i] = m_pTableInfo.add_fan_flag(i);
	}
	m_RobWanGangCard = m_pTableInfo.rob_wangangcard();
	m_bRobWanGang = m_pTableInfo.rob_wangang();
	m_bRobHu = m_pTableInfo.rob_hu();
	m_bRobGangHuJudge = m_pTableInfo.robgang_huhudge();

	//胡牌信息
    for(_uint8 i = 0; i < m_pTableInfo.is_hu_size(); ++i)
	{
	    m_bIsHu[i] = m_pTableInfo.is_hu(i);
	}
	for(_uint8 i = 0; i < m_pTableInfo.hu_fan_nums_size(); ++i)
	{
	    m_HuFanNums[i] = m_pTableInfo.hu_fan_nums(i);
	}
	for(_uint8 i = 0; i < m_pTableInfo.gang_info_size(); ++i)
	{	    
	    const proto::game::tagGangScoreInfoRecover& stGangScoreInfo = m_pTableInfo.gang_info(i);
	    m_stGang[i].iTotalScore = stGangScoreInfo.total_score();
	    for (_uint8 j = 0; j < stGangScoreInfo.gang_score_size(); ++j)
	    {
	        const proto::game::tagGangScoreRecover& stGangScore = stGangScoreInfo.gang_score(j);
	        m_stGang[i].Gang.Add(tagGangScore(stGangScore.card(), stGangScore.chairid(), stGangScore.type()));	
	    }
	}    
	for(_uint8 i = 0; i < m_pTableInfo.huinfo_size(); ++i)
	{
		const proto::game::tagUserHuInfo& stUserHu = m_pTableInfo.huinfo(i);
		
		for (_uint8 j = 0; j < stUserHu.hu_size(); ++j){
		    const proto::game::tagHu& stTagHu = stUserHu.hu(i);
		    tagHuRecord huRecord;
		    huRecord.bHuCard = stTagHu.hucard();
		    huRecord.bPasHuChairID = stTagHu.pashuchiarid();
		    m_stHuScore[i].stHu.Add(huRecord);
		}
	}
	for(_uint8 i = 0; i < m_pTableInfo.fan_info_size(); ++i)
	{
	    const proto::game::tagFanInfoRecover& stFanInfo = m_pTableInfo.fan_info(i);

	    for (_uint8 j = 0; j < stFanInfo.fan_size(); ++j)
	    {
	        m_oFan[i].Add(stFanInfo.fan(j));
	    }
	}
	
	m_RoomStatus = m_pTableInfo.room_status();
	m_strInnings = m_pTableInfo.innings();
	m_strVideo = m_pTableInfo.video_str();
	m_bCanHu = m_pTableInfo.b_can_hu();
	m_cbCurTimer = m_pTableInfo.cur_timer();
	dissovle_state = m_pTableInfo.dissovle_state();
	is_dissolved = m_pTableInfo.is_dissolved();   	
	for(_uint8 i = 0; i < m_pTableInfo.gains_size(); ++i){
		tagGain stGain;
		const proto::game::tagGain &stProtoGain = m_pTableInfo.gains(i);
		for(_uint8 j = 0; j < stProtoGain.score_size(); ++j){
			stGain.Gain[j] = stProtoGain.score(j);
		}
		m_vecGains.push_back(stGain);
	}
	//优先级这里还需要处理
	for(_uint8 i = 0; i < m_pTableInfo.players_size() && i < GAME_PLAYER; ++i){
		const proto::game::tagPlayers& stPlayer = m_pTableInfo.players(i);
		m_Seats[i].m_Uid = stPlayer.uid();
		m_Seats[i].m_bOccupied = 1;
		m_Seats[i].m_bReady = stPlayer.ready();
		Player *player = NULL;
		if (players.find(stPlayer.uid()) == players.end()){
			player = new (std::nothrow) Player();
		}
		else{
			player = players[stPlayer.uid()];
		}
		player->InitData(stPlayer);
		m_Seats[i].m_Player = player;
		m_Seats[i].m_Table = this;
		players[player->uid] = player;
		m_Seats[i].m_SeatId = i;		
		m_Seats[i].m_SettleInfo.m_wAllPiaoScore = stPlayer.all_piao_score();
		m_Seats[i].m_SettleInfo.m_ZimoCnt = stPlayer.zimo_count();
		m_Seats[i].m_SettleInfo.m_HuCnt = stPlayer.hu_count();
		m_Seats[i].m_SettleInfo.m_DianPaoCnt = stPlayer.dian_pao_count();
		m_Seats[i].m_SettleInfo.m_wBestScore = stPlayer.best_score();
		m_Seats[i].m_SettleInfo.m_wTotalScore = stPlayer.total_score();
		m_Seats[i].m_SettleInfo.m_WinCnt = stPlayer.win_count();
		m_Seats[i].m_SettleInfo.m_LoseCnt = stPlayer.lose_count();
		for (_uint8 j = 0; j < stPlayer.out_record_size(); ++j){
			m_Seats[i].m_bOutRecord.Add(stPlayer.out_record(j));
		}

		const proto::game::HandCards& pstHc = stPlayer.hand_cards();
		m_Seats[i].m_HandCard.ChangeableCardsLen = pstHc.changeablecardslen();
		for (int j = 0; j < m_Seats[i].m_HandCard.ChangeableCardsLen; j++)
		{
			//手牌
			m_Seats[i].m_HandCard.ChangeableCards[j] = pstHc.changeablecards(j);
		}
		
		m_Seats[i].m_HandCard.FixedCardsLen = pstHc.fixedcardslen();
		for(_uint8 j = 0; j < m_Seats[i].m_HandCard.FixedCardsLen; ++j){
			_uint8 cbCard = 0xFF;
			proto::game::FixedCard pFixedcard = pstHc.stfixedcards(j);
			//组合牌
			cbCard = pFixedcard.carddata();
			m_Seats[i].m_HandCard.FixedCards[j].state = pFixedcard.state();
			m_Seats[i].m_HandCard.FixedCards[j].chairID = pFixedcard.chairid();

        	if (m_Seats[i].m_HandCard.FixedCards[j].state == TYPE_LEFT_CHI)
        	{
        		m_Seats[i].m_HandCard.FixedCards[j].c[0] = cbCard;
        		m_Seats[i].m_HandCard.FixedCards[j].c[1] = cbCard + 1;
        		m_Seats[i].m_HandCard.FixedCards[j].c[2] = cbCard + 2;
        	}
        	else if (m_Seats[i].m_HandCard.FixedCards[j].state == TYPE_CENTER_CHI)
        	{
        	    m_Seats[i].m_HandCard.FixedCards[j].c[0] = cbCard - 1;
        		m_Seats[i].m_HandCard.FixedCards[j].c[1] = cbCard;
        		m_Seats[i].m_HandCard.FixedCards[j].c[2] = cbCard + 1;
        	}
        	else if (m_Seats[i].m_HandCard.FixedCards[j].state == TYPE_RIGHT_CHI)
        	{
        	    m_Seats[i].m_HandCard.FixedCards[j].c[0] = cbCard - 2;
        	    m_Seats[i].m_HandCard.FixedCards[j].c[1] = cbCard - 1;
        		m_Seats[i].m_HandCard.FixedCards[j].c[2] = cbCard;
        	}
        	else
        	{
        		m_Seats[i].m_HandCard.FixedCards[j].CardData = cbCard;
        	}
		}
		m_Seats[i].m_UserPiaoScore = stPlayer.piao();
		m_Seats[i].m_bResponse = stPlayer.b_resp();
		m_Seats[i].m_bMultiUserDoneOp = stPlayer.operate_type();
		m_Seats[i].m_bMultiUserDoneOpCard = stPlayer.operate_card();
		m_Seats[i].m_bUserOperate = stPlayer.user_operate();
		m_Seats[i].m_OutCardCnt = stPlayer.out_cnt();
		m_Seats[i].m_bGetCardCount = stPlayer.getcardcount();
		const proto::game::AckUserOvertimeState& stAckOverTimeState = stPlayer.user_overtime();
    	m_bOverTimeState[i] = stAckOverTimeState.state();
	}	
	//dissolve room recover
    const proto::game::tagDissolveRoomInfo& stDissolveRoomInfo = m_pTableInfo.dissolve_room_info();
    dissolve_applyer = stDissolveRoomInfo.uid();
	for (int i = 0; i < stDissolveRoomInfo.voters_uid_size(); ++i)
	{
	    int uid = stDissolveRoomInfo.voters_uid(i);
        if (voters_players.find(uid) == voters_players.end() && players.find(uid) != players.end())
        {
            voters_players[uid] = players[uid];            
        }
	}	
	m_conf.m_bSupportChi = m_pTableInfo.m_bsupportchi();
	m_conf.m_bSupportWind = m_pTableInfo.m_bsupportwind();
	m_conf.m_bSupportTing = m_pTableInfo.m_bsupportting();
	m_conf.m_bZimoHu = m_pTableInfo.m_bzimohu();
	m_conf.m_bYiPaoDuoXiang = m_pTableInfo.m_byipaoduoxiang();
	m_conf.m_bQiangGangHu = m_pTableInfo.m_bqiangganghu();
	m_conf.m_bQiangZhiGang = m_pTableInfo.m_bqiangzhigang();
	m_conf.m_bQiangAnGang = m_pTableInfo.m_bqiangangang();
	m_conf.m_bGangShangPao = m_pTableInfo.m_bgangshangpao();
	m_conf.m_bHuangGang = m_pTableInfo.m_bhuanggang();
	m_conf.m_bZhuangXian = m_pTableInfo.m_bzhuangxian();
	m_conf.m_bForceHuQiangGangHu = m_pTableInfo.m_bforcehuqiangganghu();
	m_conf.m_bFourceHuMutilHu = m_pTableInfo.m_bfourcehumutilhu();
	m_conf.m_AnGangPoint = m_pTableInfo.m_angangpoint();
	m_conf.m_WanGangPoint = m_pTableInfo.m_wangangpoint();
	m_conf.m_ZhiGangPoint = m_pTableInfo.m_zhigangpoint();
	m_conf.m_bHu7dui = m_pTableInfo.m_bhu7dui();
	m_conf.m_bHu13yao = m_pTableInfo.m_bhu13yao();
	m_conf.m_bNoSupport_Quanqiuren = m_pTableInfo.m_bnosupport_quanqiuren();

    for (int i = 0; i < m_pTableInfo.operate_record_size(); ++i)
    {
        const proto::game::tagOperateRecord& stOperateRecord = m_pTableInfo.operate_record(i);
        for (int j = 0; j < stOperateRecord.opinfo_chi_size(); ++j)
        {
            const proto::game::tagOpInfo& stOpInfo = stOperateRecord.opinfo_chi(j);
            m_OperateRecord[i].m_Chi.Add(OpState(stOpInfo.card(), stOpInfo.state()));
        }
        for (int k = 0; k < stOperateRecord.opinfo_gang_size(); ++k)
        {
            const proto::game::tagOpInfo& stOpInfo = stOperateRecord.opinfo_gang(k);
            m_OperateRecord[i].m_GetGangInfo.Add(OpState(stOpInfo.card(), stOpInfo.state()));
        }
        for (int l = 0; l < stOperateRecord.opinfo_penggang_size(); ++l)
        {
            const proto::game::tagOpInfo& stOpInfo = stOperateRecord.opinfo_penggang(l);
            m_OperateRecord[i].m_GetPengGang.Add(OpState(stOpInfo.card(), stOpInfo.state()));
        }
    }

	//还原配置项
	RecoverConfig();
}

void Table::RecoverConfig()
{
}

void Table::SaveGameStartVideo()
{
	proto::game::AckGameStart stGamestart;
	stGamestart.set_chairid(m_BankerSeat);
	if (m_Start_Shai.Num > 0)
	{
		stGamestart.add_touziinfo(m_Start_Shai.Shai[0]);
		stGamestart.add_touziinfo(m_Start_Shai.Shai[1]);
	}
	save_video_data(SERVER_GAME_START_BC, stGamestart.SerializeAsString());
}

void Table::SaveOutCardVideo(_uint8 wChairID, _uint8 cbCardData)
{
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

	save_video_data(SERVER_USER_OUTCARD, stUseroutcard.SerializeAsString());
}

void Table::UserOvertime_State(_uint8 ChairID)
{
	proto::game::AckUserOvertimeState stAck;
	stAck.Clear();
	stAck.set_chairid(ChairID);
	stAck.set_state(m_bOverTimeState[ChairID]);
	
	Ppacket ppack;
	stAck.SerializeToString(&ppack.body);
	ppack.pack(SERVER_OVERTIME_USER);
	broadcast(NULL, ppack.data);
}

void Table::SaveGetCard(_uint8 chairID, _uint8 cbCard, int nLeftCardNum, bool bGetLast)
{
	proto::game::AckUserGetCard getCard;
	getCard.Clear();
	getCard.set_card(cbCard);
	getCard.set_chairid(chairID);
	getCard.set_leftcardnum(nLeftCardNum);
	getCard.set_optype(m_Seats[chairID].m_bUserOperate & (~TYPE_LISTEN));   //不发送listen操作
	getCard.set_isgetlast(bGetLast ? 1 : 0);
	std::string str = getCard.SerializeAsString();
	save_video_data(SERVER_USER_GETCARD, str);
}

void Table::SaveOperateNotifyVideo()
{
	proto::game::OperateNotify stOpNotify;
	for (_uint8 i = 0; i < m_GamePlayer; ++i){
		proto::game::tagPlayers* pstPlayers = stOpNotify.add_players();
		pstPlayers->set_seatid(i);
		pstPlayers->set_operate_type(m_Seats[i].m_bUserOperate);
	}
	save_video_data(SERVER_OPERATE_NOTIFY, stOpNotify.SerializeAsString());
}

void Table::save_operate_notify(const proto::game::AckOpNotify& stNotify)
{
	std::string str = stNotify.SerializeAsString();
	save_video_data(SERVER_OPERATE_NOTIFY, str);		
}

void Table::SaveOperateVideo(_uint8 wChairID, int cbOpCode, _uint8 cbCardData)
{
	proto::game::AckBcOpResult stOpresult;
	stOpresult.set_chairid(wChairID);
	stOpresult.set_outcardchairid(m_CurOutSeat);
	stOpresult.set_optype(cbOpCode);
	stOpresult.set_card(cbCardData);
	for (int i = 0; i < m_GamePlayer; i++)
	{
		//stOpresult.add_score(score[i]);
	}
	std::string str = stOpresult.SerializeAsString();
	save_video_data(SERVER_OPERATE_CARD_RES, stOpresult.SerializeAsString());
}
void Table::SaveOperateVideo(int iOpType, _uint8 chairID, _uint8 paschairID, _uint8 cbCard, int score[GAME_PLAYER])
{
	proto::game::AckBcOpResult stOpresult;
	stOpresult.set_chairid(chairID);
	stOpresult.set_outcardchairid(paschairID);
	stOpresult.set_optype(iOpType);
	stOpresult.set_card(cbCard);
	for (int i = 0; i < m_GamePlayer; i++)
	{
		stOpresult.add_score(score[i]);
	}
	std::string str = stOpresult.SerializeAsString();
	save_video_data(SERVER_OPERATE_RESULT, stOpresult.SerializeAsString());
}

void Table::SaveOperateResVideo(_uint8 wChairID, int cbOpCode)
{
	proto::game::AckOperateRes stOpRes;
	stOpRes.set_seatid(wChairID);
	stOpRes.set_operate_type(cbOpCode);
	save_video_data(SERVER_OPERATE_RES, stOpRes.SerializeAsString());
}

void Table::SaveTableInfoVideo()
{
	proto::game::TableInfoVideo stTableInfo;
	stTableInfo.set_banker(m_BankerSeat);
	stTableInfo.set_take_in(m_TakeIn);
	stTableInfo.set_play_id(m_PlayId);
	stTableInfo.set_room_user_id(m_RoomUserId);
	stTableInfo.set_game_player(m_GamePlayer);
	stTableInfo.set_current_num(m_CurTurn);
	stTableInfo.set_all_num(m_TotalTurn);
	for (int c = 0; c < m_GamePlayer; ++c){
		Player *p = m_Seats[c].m_Player;
		if (p == NULL) continue;
		proto::game::tagPlayers* pstTablePlayers = stTableInfo.add_players();
		pstTablePlayers->set_uid(p->uid);
		pstTablePlayers->set_seatid(p->seatid);
		pstTablePlayers->set_ready(m_Seats[c].m_bReady);
		pstTablePlayers->set_money(p->money);
		pstTablePlayers->set_name(p->name);
		pstTablePlayers->set_sex(p->sex);
		pstTablePlayers->set_avatar(p->avatar);
		if (m_bPiao){
			pstTablePlayers->set_piao(m_Seats[c].m_UserPiaoScore);
		}
	}
	save_video_data(SERVER_TABLE_INFO_UC, stTableInfo.SerializeAsString());
}

void Table::save_video_hu(_uint8 chairID, _uint8 paschairID, int score[GAME_PLAYER])
{
	proto::game::AckUserHu stHu;
	stHu.set_huchairid(chairID);
	stHu.set_pashuchairid(paschairID);
	for (int i = 0; i < m_GamePlayer; i++)
	{
		stHu.add_score(score[i]);
	}
	std::string str = stHu.SerializeAsString();
	save_video_data(SERVER_USER_HU, str);
}