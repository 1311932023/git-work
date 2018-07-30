#include "table.h"
#include "zjh.h"
#include "proto/proto.h"
#include "common/log.h"
#include "match.h"

extern ZJH zjh;
extern Log log;

using namespace proto::game::hunanmj;

void Table::Proto_GameScene(_uint8 ChairID, Player* player, proto::game::AckGameScene& stGameScene)
{

	//log.info("%s begin to uid:%d chairid:%d gamestate:%d \n", __FUNCTION__, player->uid, ChairID, m_bGameState);
	stGameScene.set_outcardtime(m_Outcard_Timer_stamp);
	stGameScene.set_operatetime(m_Operate_Timer_stamp[0]);
	stGameScene.set_delaytime(m_bDelayTime);
	stGameScene.set_basescore(m_BaseScore);
	if (m_bDeleteTable)
	{
		stGameScene.set_scenestatus(10);
	}
	else
	{
		stGameScene.set_scenestatus(m_bGameState);
	}
	stGameScene.set_curoutcarduser(m_bOutCardChairID);
	stGameScene.set_curoutcard(m_bCurOutCard);
	//stGameScene.set_optype(m_bUserOperate[ChairID]);
	stGameScene.set_optype(0);
	stGameScene.set_bankerchairid(m_bBanker);
	_uint8 bIsMoPai = (ChairID == m_bCurChairID) ? 1 : 0;//当前摸牌玩家
	stGameScene.set_iscurgetcard(bIsMoPai);
	stGameScene.set_card(m_bCurCard);//摸到的牌
// 	_uint8 bAutoHu = 0;		
// 	stGameScene.set_isautohu(bAutoHu);
	stGameScene.set_isautohu(m_trustee.get()->isAutoOut(ChairID) ? 1 : 0);
	ev_tstamp nleftTime = 0;
	if (m_bUserOperate[ChairID] == TYPE_NULL || m_bUserOperate[ChairID] == TYPE_LISTEN)
	{
		nleftTime = get_operate_remain_time(TYPE_NULL, true);
		if (nleftTime > m_Outcard_Timer_stamp)
		{
			nleftTime = m_Outcard_Timer_stamp;
		}

	}
	else
	{
		nleftTime = get_operate_remain_time(TYPE_NULL, true);
		if (nleftTime > m_Operate_Timer_stamp[0])
		{
			nleftTime = m_Operate_Timer_stamp[0];
		}

	}
	stGameScene.set_lefttime(nleftTime);
	log.info("%s uid:%d chairid:%d left timer:%d m_bUserOperate:0x%x\n", __FUNCTION__, GetPlayerUid(player), ChairID, stGameScene.lefttime(), m_bUserOperate[ChairID]);

	stGameScene.set_stand_money(stand_money);
	stGameScene.set_poolcardleftnums(m_CardPool.Count());
	stGameScene.set_preready_timer(preready_timer_stamp);

	stGameScene.set_isdisbandroom(m_bIsDisbandTable);
	proto::game::AckDisbandApply *pDisInfo = stGameScene.mutable_disbandinfo();
	pDisInfo->set_applychairid(m_ApplyDisbandChairid);
	int leftchoosetime = Disband_timer_remaintime(ChairID);
	pDisInfo->set_waittime(leftchoosetime);
	for (int i = 0; i < m_GAME_PLAYER; i++)
	{
		pDisInfo->add_onlinechairid(i);
	}

	proto::game::HandCards *pstHc = stGameScene.mutable_usercard();
	Fill_HandCard(ChairID, pstHc);

	for (int i = 0; i < m_GAME_PLAYER; i++)
	{
		stGameScene.add_handcardnums(m_HandCard[i].ChangeableCardsLen);
		stGameScene.add_totalscore(m_stHuScore[i].iTotalScore + m_stGang[i].iTotalScore);
		stGameScene.add_offlinestate(m_IsOffline[i]);

		proto::game::AckDisbandChooseBc *pDisChoose = stGameScene.add_userdisbandstate();
		pDisChoose->set_chairid(i);
		pDisChoose->set_choosestate(m_DisbandChoose[i]);
		proto::game::AckUserOnlineState *pOnlinestate = stGameScene.add_useronlinestate();
		pOnlinestate->set_chairid(i);
		pOnlinestate->set_state(m_IsOffline[i]);
		proto::game::AckUserOvertimeState *pOvertime = stGameScene.add_userovertime();
		pOvertime->set_chairid(i);
		pOvertime->set_state(m_bOverTimeState[i]);
		//打出牌
		proto::game::tagShowOutCard *pstShow = stGameScene.add_showcard();
		for (int j = 0; j < m_stUserOutCard[i].Size(); j++)
		{
			pstShow->add_outcard(m_stUserOutCard[i][j]);
		}
		pstShow->set_outcardlen(m_stUserOutCard[i].Size());
		//碰杠牌
		proto::game::tagGroupCard *pstGr = stGameScene.add_groupcard();
		for (int j = 0; j < m_HandCard[i].FixedCardsLen && j < 4; j++)
		{
			proto::game::FixedCard *pstFix = pstGr->add_fixedcard();
			Fill_FixedCard(i, j, pstFix);
		}
		//胡牌信息
		proto::game::tagUserHuInfo *pstUserHu = stGameScene.add_huinfo();
		for (int j = 0; j < m_stHuScore[i].stHu.Size(); j++)
		{
			proto::game::tagHu *pstHu = pstUserHu->add_hu();
			pstHu->set_hucard(m_stHuScore[i].stHu[j].bHuCard);
			pstHu->set_pashuchiarid(m_stHuScore[i].stHu[j].bPasHuChairID);
		}
	}

	if (!(m_bMultiOpUserNum != 0 && m_bMultiUserDoneOp[ChairID] != TYPE_NULL))
	{	
		Proto_GameScene_Operate(ChairID, player, stGameScene);
	}	

	AckCustomiedInfo* info = stGameScene.mutable_hunanscene();
	if (info != NULL)
	{
		info->set_gametype(proto::game::hunanmj::HUNAN_CHANGSHA);
		info->set_ishavepiao(0);
		info->set_jiangma(proto::game::hunanmj::JIANGMA);//抓鸟
		info->set_laizi(0);//没有癞子
		info->set_piaotime(15);
		for (int i = 0; i < m_room_config.Size(); i++)
		{
			info->add_roomconfigid(m_room_config[i]);
		}
	}
}
void Table::Proto_GameScene_Operate(_uint8 ChairID, Player* player, proto::game::AckGameScene& stGameScene)
{
	stGameScene.set_optype(m_bUserOperate[ChairID]);		
	if (m_bUserOperate[ChairID] & TYPE_ANGANG || m_bUserOperate[ChairID] & TYPE_WANGANG)
	{
		MJ_opinfo stGang;
		GetGangInfo(ChairID, stGang);
		for (int len = 0; len < stGang.Size(); len++)
		{
			proto::game::tagGangInfo *pstGang = stGameScene.add_ganginfo();
			pstGang->set_card(stGang[len].cbCard);
			pstGang->set_state(stGang[len].nState);
		}
	}
	if ((m_bUserOperate[ChairID] & TYPE_LISTEN) && !(m_bUserOperate[ChairID] & TYPE_HU))
	{
		//打包听牌信息
		tagListenInfos stListenInfo[14];
		_uint8 bListenLen = 0;
		//胡牌不用判断听牌，自动胡牌不用判断听牌
		if (!(m_bUserOperate[ChairID] & TYPE_HU))
		{
			m_bUserOperate[ChairID] |= GetListeningInfo(ChairID, stListenInfo, bListenLen);
		}
		
		if (m_send_listen_info)
		{
			for (int j = 0; j < bListenLen; j++)
			{
				PackageTingInfo(ChairID, stListenInfo, j, stGameScene.add_listeninfo());
			}
		}
	}
	if (m_bUserOperate[ChairID] & TYPE_HU)
	{
		if (m_bRobGangHuJudge || m_bRobWanGang)
		{
			AckCustomiedInfo* info = stGameScene.mutable_hunanscene();
			info->set_hu_type(1);
		}
	}
}
void Table::Proto_GameScene_lookon(_uint8 ChairID, Player* player, proto::game::AckGameScene& stGameScene)
{

	stGameScene.set_outcardtime(m_Outcard_Timer_stamp);
	stGameScene.set_operatetime(m_Operate_Timer_stamp[0]);
	stGameScene.set_delaytime(m_bDelayTime);
	stGameScene.set_basescore(m_BaseScore);
	stGameScene.set_scenestatus(m_bGameState);
	stGameScene.set_curoutcarduser(m_bOutCardChairID);
	stGameScene.set_curoutcard(m_bCurOutCard);
	stGameScene.set_bankerchairid(m_bBanker);

	stGameScene.set_stand_money(stand_money);
	
	ev_tstamp nleftTime = get_operate_remain_time(TYPE_NULL, true);
	if (nleftTime > m_Operate_Timer_stamp[0])
	{
		nleftTime = m_Operate_Timer_stamp[0];
	}
		
	stGameScene.set_lefttime(nleftTime);
	stGameScene.set_poolcardleftnums(m_CardPool.Count());
	stGameScene.set_preready_timer(preready_timer_stamp);

	for (int i = 0; i < m_GAME_PLAYER; i++)
	{
		stGameScene.add_handcardnums(m_HandCard[i].ChangeableCardsLen);
		stGameScene.add_offlinestate(m_IsOffline[i]);
		//打出牌
		proto::game::tagShowOutCard *pstShow = stGameScene.add_showcard();
		for (int j = 0; j < m_stUserOutCard[i].Size(); j++)
		{
			pstShow->add_outcard(m_stUserOutCard[i][j]);
		}
		pstShow->set_outcardlen(m_stUserOutCard[i].Size());
		//碰杠牌
		proto::game::tagGroupCard *pstGr = stGameScene.add_groupcard();
		for (int j = 0; j < m_HandCard[i].FixedCardsLen && j < 4; j++)
		{
			proto::game::FixedCard *pstFix = pstGr->add_fixedcard();
			Fill_FixedCard(i, j, pstFix);
		}
		//胡牌信息
		proto::game::tagUserHuInfo *pstUserHu = stGameScene.add_huinfo();
		for (int j = 0; j < m_stHuScore[i].stHu.Size(); j++)
		{
			proto::game::tagHu *pstHu = pstUserHu->add_hu();
			pstHu->set_hucard(m_stHuScore[i].stHu[j].bHuCard);
			pstHu->set_pashuchiarid(m_stHuScore[i].stHu[j].bPasHuChairID);
		}
	}
	AckCustomiedInfo* info = stGameScene.mutable_hunanscene();
	if (info != NULL)
	{
		info->set_gametype(proto::game::hunanmj::HUNAN_CHANGSHA);
		info->set_ishavepiao(0);
		info->set_jiangma(proto::game::hunanmj::JIANGMA);//抓鸟
		info->set_laizi(0);//没有癞子
		info->set_piaotime(15);
		for (int i = 0; i < m_room_config.Size(); i++)
		{
			info->add_roomconfigid(m_room_config[i]);
		}
	}
}

void Table::Score_GameEndDraw(HuScoreSet& score, proto::game::AckGameEnd& stGameend)
{
	if (m_conf.m_bHuangGang)
	{
		memset(m_stGang, 0, sizeof(m_stGang));
	}	
}
void Table::Score_GameEndSucc(HuScoreSet& score, proto::game::AckGameEnd& stGameend)
{

	int iTotalScore[GAME_PLAYER] = { 0 };
	int iBaseScore[GAME_PLAYER][GAME_PLAYER];
	int iScore[GAME_PLAYER][GAME_PLAYER];
	memset(iBaseScore, 0, sizeof(iBaseScore));
	memset(iScore, 0, sizeof(iScore));
	for (int i = 0; i < m_GAME_PLAYER; i++)
	{
		Calc_BaseScore(i, iBaseScore[i]);
	}
	for (int i = 0; i < m_GAME_PLAYER; i++)
	{		
		Calc_HuScore(i, iBaseScore[i], iScore[i]);		
	}
	if (m_conf.m_bCalcAllNiao)
	{
		for (int i = 0; i < m_GAME_PLAYER; i++)
		{
			for (_uint8 j = 0; j < m_niao.Size(); j++)
			{
				Calc_ZhongNiao(i, m_niao[j], iBaseScore[i], iScore[i]);
			}
		}
	}
	else
	{
		for (int i = 0; i < m_GAME_PLAYER; i++)
		{
			Calc_ZhongNiao(i, INVALID_MJ_CARD, iBaseScore[i], iScore[i]);
		}
	}
	for (int i = 0; i < m_GAME_PLAYER; i++)
	{
		Calc_FengDing(i, iScore[i]);
	}
	for (int i = 0; i < m_GAME_PLAYER; i++)
	{
		Calc_Score(iTotalScore, iScore[i]);
	}
	for (int i = 0; i < m_GAME_PLAYER; i++)
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
void Table::Calc_ZhongNiao(_uint8 ChairID, _uint8 ShaiNum, int iBaseScore[GAME_PLAYER], int iScore[GAME_PLAYER])
{

}
void Table::Calc_FengDing(_uint8 ChairID, int iScore[GAME_PLAYER])
{

}
#ifndef __REFINE__
void Table::Calc_UserGangScore(_uint8 ChairID, int iOpType, _uint8 bCard, int iScore[GAME_PLAYER])
{

}
#endif
void Table::Fill_HandCard(_uint8 ChairID, proto::game::HandCards* pHandcard)
{
	pHandcard->set_changeablecardslen(m_HandCard[ChairID].ChangeableCardsLen);
	for (int j = 0; j < m_HandCard[ChairID].ChangeableCardsLen; j++)
	{
		pHandcard->add_changeablecards(m_HandCard[ChairID].ChangeableCards[j]);		
	}
	for (int j = 0; j < m_HandCard[ChairID].FixedCardsLen; j++)
	{
		proto::game::FixedCard *pFixedcard = pHandcard->add_stfixedcards();
		Fill_FixedCard(ChairID, j, pFixedcard);
	}
	pHandcard->set_fixedcardslen(m_HandCard[ChairID].FixedCardsLen);
}
void Table::Fill_FixedCard(_uint8 ChairID, _uint8 Pos, proto::game::FixedCard* pstFix)
{
	_uint8 cbCard = 0xFF;
	if (m_HandCard[ChairID].FixedCards[Pos].state == TYPE_LEFT_CHI)
	{
		cbCard = m_HandCard[ChairID].FixedCards[Pos].c[0];
	}
	else if (m_HandCard[ChairID].FixedCards[Pos].state == TYPE_CENTER_CHI)
	{
		cbCard = m_HandCard[ChairID].FixedCards[Pos].c[1];
	}
	else if (m_HandCard[ChairID].FixedCards[Pos].state == TYPE_RIGHT_CHI)
	{
		cbCard = m_HandCard[ChairID].FixedCards[Pos].c[2];
	}
	else
	{
		cbCard = m_HandCard[ChairID].FixedCards[Pos].CardData;
	}
	pstFix->set_carddata(cbCard);
	pstFix->set_state(m_HandCard[ChairID].FixedCards[Pos].state);
	pstFix->set_chairid(m_HandCard[ChairID].FixedCards[Pos].chairID);
	//log.info("%s uid:%d ChairID:%d \n", __FUNCTION__, GetPlayerUid(ChairID), ChairID);
}

int Table::MultiHu(MJ_user_buffer& id)const
{
	id.Clear();
	for (int i = 0; i < m_GAME_PLAYER; i++)
	{
		if (m_bIsHu[i])
		{
			id.Add(i);
		}
	}
	return id.Size();
}

void Table::Update_Score(int iScore[GAME_PLAYER])
{
	tagUserScore userscore[GAME_PLAYER];

	for (int i = 0; i < m_GAME_PLAYER; i++)
	{
		if (m_SeatPlayer[i].player != NULL)
		{
			userscore[i].uid = m_SeatPlayer[i].player->uid;
			userscore[i].score = iScore[i];

			m_SeatPlayer[i].player->update_info(iScore[i], 0, 0, 0);

			UpdateUserInfoToUser(m_SeatPlayer[i].player);
			log.fatal("%s uid:%d chairid:%d score:%d\n", __FUNCTION__, m_SeatPlayer[i].player->uid, i, iScore[i]);
		}
	}
	update_user_info_to_datasvr_dz(userscore, 1102, SERVER_DZ_UPDATE_USER_INFO_LITTLE);
	
}

void Table::Recv_DisbandTable_Timer_cb(struct ev_loop *loop, struct ev_timer *w, int revents)
{
	Table* table = (Table*)w->data;
	if (table != NULL)
	{		
		log.info("%s tid:%d, send disband message again\n", __FUNCTION__, table->tid);
		table->Send_DisbandTableMessage();
	}
}

void Table::Send_DisbandTableMessage()
{
	//发送解散命令给后台
	Jpacket packet;
	packet.val["cmd"] = SERVER_DISBAND_ROOM;
	packet.val["vid"] = vid;

	char strroomid[64] = { 0 };
	sprintf(strroomid, "%d", tid);
	packet.val["roomid"] = strroomid;

	char strbuf[50] = { 0 };
	sprintf(strbuf, "%d_%04d", tid, m_nPlayingTimes);
	packet.val["innings"] = strbuf;
	packet.val["backCardInFirstCount"] = 1;
	packet.end();

	log.info("%s disband table:%d times:%s \n", __FUNCTION__, tid, strbuf);	
	zjh.game->send_to_datasvr(packet.tostring());
	
	m_bDeleteTable = true;

	TIMER_AGAIN(m_RecvDisbandTable_Timer, m_RecvDisbandTable_Timer_stamp);
}
int Table::logout_room_req(Player *player)
{	
	// 如果在桌上先让其下桌
	if (find(player->uid)) {
		if (handler_downtable(player) == -1)
		{
			log.error("%s error tid:%d uid:%d seatid:%d state:%d\n", __FUNCTION__, tid, player->uid, player->seatid, player->status);			
		}
	}

	// 通知业务平台玩家退出房间
	Jpacket packet;
	packet.val["cmd"] = SERVER_LOGOUT_ROOM_REQ;
	packet.val["uid"] = player->uid;
	packet.val["vid"] = zid;
	packet.val["roomid"] = tid;
	packet.end();
	zjh.game->send_to_datasvr(packet.tostring());

	log.info("%s tid:%d uid:%d seatid:%d money:%d\n", __FUNCTION__, tid, player->uid, player->seatid, player->money);

	return 0;
}
int Table::force_logout_room_req(Player *player)
{
	if (player == NULL)
	{
		return -1;
	}	
	// 通知业务平台玩家退出房间
	Jpacket packet;
	packet.val["cmd"] = SERVER_FORCE_LOGOUT_ROOM_REQ;
	packet.val["uid"] = player->uid;
	packet.val["vid"] = zid;
	packet.val["roomid"] = tid;
	packet.end();
	zjh.game->send_to_datasvr(packet.tostring());

	log.info("%s tid:%d uid:%d seatid:%d money:%d\n", __FUNCTION__, tid, player->uid, player->seatid, player->money);
	return 0;
}
void Table::GameBreakOff()
{

	proto::game::AckGameEnd stGameend;
	HuScoreSet hu_score;
	Score_GameEndDraw(hu_score, stGameend);

	log.info("%s tid:%d score[%d,%d,%d,%d]\n", __FUNCTION__, tid, hu_score[0].iTotleScore, hu_score[1].iTotleScore, hu_score[2].iTotleScore, hu_score[3].iTotleScore);

	int iScore[GAME_PLAYER] = { 0 };
	for (_uint8 i = 0; i < m_GAME_PLAYER; i ++)
	{
		iScore[i] = hu_score[i].iTotleScore;
	}
	Update_Score(iScore);

	MJ_user_buffer UserID;	
	CalcRecordInfo(UserID, iScore);

}
bool Table::HasSeat(const Player *player)const
{	
	if (player == NULL)
	{
		return false;
	}
	for (int i = 0; i < m_GAME_PLAYER; i++)
	{
		if (m_SeatPlayer[i].player == player)
		{
			return true;
		}
	}
	for (int i = 0; i < m_GAME_PLAYER; i++)
	{
		if (m_SeatPlayer[i].player == NULL)
		{
			return true;
		}
	}
	return false;
}
bool Table::find(_uint8 uid)
{
	for (int i = 0; i < m_GAME_PLAYER; i++)
	{
		if (m_SeatPlayer[i].state != Seat::E_Seat_Empty && m_SeatPlayer[i].uid == uid)
		{
			return true;
		}
	}
	return false;
}
_uint8 Table::SeatPlayerCount()const
{
	_uint8 iSeatCount = 0;
	for (int i = 0; i < m_GAME_PLAYER; i++)
	{
		if (m_SeatPlayer[i].state != Seat::E_Seat_Empty)
		{
			iSeatCount++;
		}
	}
	return iSeatCount;
}
void Table::User_Home_Out(Player * player)
{
	log.info("%s tid:%d uid:%d\n", __FUNCTION__, tid, GetPlayerUid(player));
}

void Table::Check_All_Offline()
{
	if (check_all_table_user_offline())
	{
		send_talbe_no_user();
	}
}

void Table::Check_Offline_Timer_cb(struct ev_loop *loop, struct ev_timer *w, int revents)
{
	Table* table = (Table*)w->data;
	if (table != NULL)
	{
		table->Check_Offline_Beat();
	}
	
}
void Table::Check_Offline_Beat()
{	
	if (m_bDeleteTable)
		return;
	for (int i = 0; i < m_GAME_PLAYER; i++)
	{
		if (m_SeatPlayer[i].state == Seat::E_Seat_Empty)
		{
			continue;
		}
		if (m_SeatPlayer[i].player != NULL && m_SeatPlayer[i].player->m_isRobot)
		{
			continue;
		}
		if (!m_Online_Beat[i])
		{
			User_Offline(m_SeatPlayer[i].player);
		}
		m_Online_Beat[i] = false;
	}
}
void Table::Online_Beat(Player* player)
{
	if (player->seatid >= 0 && player->seatid < m_GAME_PLAYER)
	{
		if (!m_Online_Beat[player->seatid])
		{
			User_Offline(player, false);
		}		
		m_Online_Beat[player->seatid] = true;				
	}	
}

bool Table::Is_GenZhuangMa4X(_uint8 ChairID, _uint8 niao)
{
	_uint8 dis = DisChair(m_bBanker, ChairID);
	_uint8 niao_pos =  niao % GAME_PLAYER;		
	return dis == niao_pos;
}

bool Table::Is_GenZhuangByPlayerNum(_uint8 ChairID, _uint8 niao)
{
	_uint8 dis = DisChair(m_bBanker, ChairID);
	assert(m_GAME_PLAYER > 0);
	_uint8 niao_pos = niao % m_GAME_PLAYER;
	return dis == niao_pos;
}

bool Table::Is_HuPaiMa4X(_uint8 HuId, _uint8 ChairID, _uint8 niao)
{
	_uint8 dis = DisChair(HuId, ChairID);
	_uint8 niao_pos = niao % GAME_PLAYER;	
	return dis == niao_pos;
}

void Table::PackageTingInfo(_uint8 ChairID, tagListenInfos stListenInfo[14], _uint8 iPos, proto::game::tagListenInfo* listeninfo)
{
	listeninfo->set_outcard(stListenInfo[iPos].bOutCard);
	for (int k = 0; k < stListenInfo[iPos].bHuLen; k++)
	{
		proto::game::tagHuInfo *pstHuinfo = listeninfo->add_huinfo();
		_uint8 bListenCard = stListenInfo[iPos].HuInfo[k].bCard;
		pstHuinfo->set_card(bListenCard);
		pstHuinfo->set_hufannum(stListenInfo[iPos].HuInfo[k].bFanShu);
		int nCardLeftNum = Get_Card_Left_Num(ChairID, bListenCard);
		pstHuinfo->set_leftnum(nCardLeftNum);
	}
}

void Table::DoMultiForceHu(_uint8 ChairID, _uint8 bCard)
{
	m_bRecordMultiOp.iOpType = TYPE_HU;
	m_bRecordMultiOp.ChairID = ChairID;
	m_bRecordMultiOp.bCard = bCard;
	m_bMultiDoneOpNum = m_bMultiOpUserNum;

	for (_uint8 c = NextChair(ChairID); c != ChairID; c = NextChair(c))
	{
		if (c == m_bCurChairID)
			continue;
		if (m_bUserOperate[c] & TYPE_HU || m_PassHuPlayer.Find(c))
		{
			m_louhu[c].Unlock();

			m_bMultiUserDoneOp[c] = TYPE_HU;
			m_bMultiUserDoneOpCard[c] = bCard;

			m_bUserOperate[c] |= TYPE_HU;
			UserOperateHu(c, bCard);
			stop_operator_timer(c);
		}
	}
}
void Table::RobGangForceHu(_uint8 ChairID, _uint8 bCard)
{
	m_RobOperateNum = m_RobHuNum;

	for (_uint8 c = NextChair(ChairID); c != ChairID; c=NextChair(c))
	{
		if (c == m_WangGangPlayer)
			continue;
		if (m_bUserOperate[c] & TYPE_HU || m_PassHuPlayer.Find(c))
		{
			m_louhu[c].Unlock();

			m_bUserOperate[c] |= TYPE_HU;
			UserOperateHu(c, bCard);
			stop_operator_timer(c);
		}		
	}	
}


void Table::CalcRecordInfo(const MJ_user_buffer& UserID, int iTotalScore[GAME_PLAYER])
{

}

_uint8 SortNode(SGSI_SortNode n[GAME_PLAYER], _uint8 game_player)
{
	SGSI_SortNode tmp;
	for (int i = 0; i < (game_player - 1); i++)
	{
		for (int j = 0; j < (game_player - 1); j++)
		{
			if (n[j].iTotalScore < n[j + 1].iTotalScore)
			{
				tmp = n[j];
				n[j] = n[j + 1];
				n[j + 1] = tmp;
			}
		}
	}
	if (n[0].iTotalScore == 0)
		return 0;
	int iMax = n[0].iTotalScore;
	_uint8 iCounts = 1;
	for (int i = 1; i < game_player; i++)
	{
		if (iMax == n[i].iTotalScore)
			iCounts++;
	}
	return iCounts;
}
void Table::Send_GameEnd_ScoreInfo(Player* player, proto::game::hunanmj::GameEndRecord &ger)
{ 		
	
}

void Table::RedisUpdateScoreToServer(int iTotalScore[GAME_PLAYER])
{

}
void Table::Send_GameRound_ScoreInfo(Player* player)
{

}
void Table::RedisUpdateScoreToLocal()
{
}

void Table::InitPlayAccont()
{


}
void Table::RequestGameStart_Timer_cb(struct ev_loop *loop, struct ev_timer *w, int revents)
{
	Table* table = (Table*)w->data;
	if (table != NULL)
	{
		table->game_start_req();
	}
}

void Table::RoundOver_Timer_cb(struct ev_loop *loop, struct ev_timer *w, int revents)
{
	Table* table = (Table*)w->data;
	if (table != NULL)
	{
		TIMER_STOP(table->m_RoundOver_timer);
		try
		{
			table->match->RoundOver(table);
		}
		//try {}
		catch (...) {}
	}
}
void Table::NextRound_Timer_cb(struct ev_loop *loop, struct ev_timer *w, int revents)
{
	Table* table = (Table*)w->data;
	if (table != NULL)
	{
		TIMER_STOP(table->m_NextRound_timer);

		int tmp_all_round = table->m_CurRound - table->match->InitRoundNumber() + 2;
		if (tmp_all_round > table->match->m_AllRound)			
		{
			return;
		}					
		table->m_CurRound++;
		table->match->TableGameStart(table);
		log.info("%s:%d:%s  roundinfo round:%d\n",__FILE__,__LINE__, __FUNCTION__, table->m_CurRound);
	}
}
