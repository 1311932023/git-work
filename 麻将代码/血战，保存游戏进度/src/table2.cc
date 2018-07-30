#include "proto/proto.h"
#include "table.h"
#include "zjh.h"
#include "common/log.h"
#include "game.h"


extern ZJH zjh;
extern Log log;

using namespace proto::game::henanmj;

#define MultiOpOr(X,Y)	(((X) & (Y) ? (Y) : 0) & (m_bMultiUserDoneOp[c] & (Y) ? (Y) :0))

void Table::UserOperateChi(_uint8 ChairID, int iOpType, _uint8 bCard)
{

	memset(m_bAddFanFlag, 0, sizeof(m_bAddFanFlag));
	memset(m_bUserOperate, 0, sizeof(m_bUserOperate));

	MJ_opinfo stChi;
	switch (iOpType)
	{
	case TYPE_LEFT_CHI:
	{
		//吃牌检查
		if (!(GetChi(ChairID, bCard, stChi) & TYPE_LEFT_CHI))
		{
			log_info("user_operate_chi ChairID:%d operate:0x%x，card: 0x%02x,but handcard has no cur operate \n", ChairID, iOpType, bCard);
			return;
		}
		m_GameLogic.ExecuteLeftChi(&m_HandCard[ChairID], m_bOutCardChairID, bCard);		
	}
	break;
	case TYPE_CENTER_CHI:
	{
		//吃牌检查
		if (!(GetChi(ChairID, bCard, stChi) & TYPE_CENTER_CHI))
		{
			log_info("user_operate_chi ChairID:%d operate:0x%x，card: 0x%02x,but handcard has no cur operate \n", ChairID, iOpType, bCard);
			return;
		}
		m_GameLogic.ExecuteCenterChi(&m_HandCard[ChairID], m_bOutCardChairID, bCard);
	}
	break;
	case TYPE_RIGHT_CHI:
	{
		//吃牌检查
		if (!(GetChi(ChairID, bCard, stChi) & TYPE_RIGHT_CHI))
		{
			log_info("user_operate_chi ChairID:%d operate:0x%x，card: 0x%02x,but handcard has no cur operate \n", ChairID, iOpType, bCard);
			return;
		}
		m_GameLogic.ExecuteRightChi(&m_HandCard[ChairID], m_bOutCardChairID, bCard);
	}
	break;
	default:
		break;
	}
	m_bGetCardCount[ChairID]++;

	m_stUserOutCard[m_bOutCardChairID].RemoveLast(bCard);	
	m_bCurOutCard = INVALID_MJ_CARD;

	//广播操作结果的消息
	BroadcastOperateResult(ChairID, iOpType, bCard);
	log_info("%s tid:%d uid:%d ChairID:%d, bCard:0x%02x \n", __FUNCTION__, tid, GetPlayerUid(ChairID), ChairID, bCard);

	SendTingInfo(ChairID);

	int score[GAME_PLAYER] = { 0 };
	save_video_operate(iOpType, ChairID, m_bOutCardChairID, bCard, score);

	UserAfterOperate(ChairID, iOpType, bCard);	

}

void Table::UserOperatePeng(_uint8 ChairID, _uint8 bCard)
{	
	log_info("%s tid:%d uid:%d ChairID:%d, bCard:0x%02x \n", __FUNCTION__, tid, GetPlayerUid(ChairID), ChairID, bCard);
	memset(m_bAddFanFlag, 0, sizeof(m_bAddFanFlag));	
	memset(m_bUserOperate, 0, sizeof(m_bUserOperate));

	BroadcastOperateResult(ChairID, TYPE_PENG, bCard);

	int fixedlen = m_HandCard[ChairID].FixedCardsLen;
	//if (fixedlen >= 4)  //17张牌可以碰5下
	if (fixedlen >= HAND_CARD_KAN_MAX)
	{
		log_error("玩家组合牌长度:%d大于正常的长度 \n", fixedlen);
		return;
	}
	m_GameLogic.ExecutePeng(&m_HandCard[ChairID], m_bOutCardChairID, bCard);
	m_bGetCardCount[ChairID]++;

	//从打出牌删掉这张牌	
	m_stUserOutCard[m_bOutCardChairID].RemoveLast(bCard);
	m_bCurOutCard = INVALID_MJ_CARD;
	//m_bOutCardCount[CardType(bCard)][CardNum(bCard)] += 2;


	//TODO 检查是否听牌
	SendTingInfo(ChairID);
	
	UserAfterOperate(ChairID, TYPE_PENG, bCard);

	int score[GAME_PLAYER] = { 0 };
	save_video_operate(TYPE_PENG, ChairID, m_bOutCardChairID, bCard, score);
}

int Table::OutCardEstimate(_uint8 ChairID, _uint8 bCard, MJ_major& major, int& fan_num)
{	
	int iOpType = TYPE_NULL;
	
	HandCards* pHc = &m_HandCard[ChairID];
	
	if (m_conf.m_bCheckCardNum && pHc->ChangeableCardsLen % 3 != 1)
	{
		log_error("%s tid:%d fatal paishu error uid:%d chairid:%d!\n", __FUNCTION__, tid, GetPlayerUid(ChairID), ChairID);
		log_error("%s cards(%d):%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x\n", __FUNCTION__, pHc->ChangeableCardsLen,
			pHc->ChangeableCards[0], pHc->ChangeableCards[1], pHc->ChangeableCards[2], pHc->ChangeableCards[3],
			pHc->ChangeableCards[4], pHc->ChangeableCards[5], pHc->ChangeableCards[6], pHc->ChangeableCards[7],
			pHc->ChangeableCards[8], pHc->ChangeableCards[9], pHc->ChangeableCards[10], pHc->ChangeableCards[11],
			pHc->ChangeableCards[12], pHc->ChangeableCards[13]);
		throw("card num is error\n");
		return TYPE_NULL;
	}

	iOpType |= GetChi(ChairID, bCard, major.m_Chi);
		
	iOpType |= GetPengGang(ChairID, bCard, major.m_GetPengGang);

	
	if (!m_conf.m_bZimoHu)
	{
		m_GameLogic.AddCard(pHc, bCard);
		
		iOpType |= Check_Hu(ChairID, *pHc, major.m_oFan);		
		if (CardPoolIsEmpty())
		{
			UserLastOutCard(ChairID, iOpType, major.m_oFan);
		}		
		
		if (IsQuanqiuren(*pHc, major.m_oFan, iOpType))
		{
		}		
		
		if (m_bGetCardCount[m_bBanker] == 1)
		{//庄家打出第一张牌
			for (_uint8 c = NextChair(m_bBanker); c != m_bBanker; c = NextChair(c))
			{
				if (m_bGetCardCount[c] != 0)
					goto BANKER_FIRST_OUTCARD;
			}
			UserFirstOutCard(ChairID, iOpType, major.m_oFan);			
		}
BANKER_FIRST_OUTCARD:
		if (iOpType & TYPE_HU)
		{
			if (m_conf.m_bGangShangPao)
			{
				if (!m_bRobWanGang && m_bAddFanFlag[m_bCurChairID]) //杠上炮
				{
					UserGangHouPao(ChairID, bCard, major.m_oFan);					
				}
			}
			if (m_conf.m_bQiangGangHu || m_conf.m_bQiangZhiGang || m_conf.m_bQiangAnGang)
			{
				if (m_bRobWanGang && m_bAddFanFlag[m_WangGangPlayer])  //抢杠胡
				{
					UserQiangGangHu(ChairID, bCard, major.m_oFan);
				}
				else if (m_bRobGangHuJudge)
				{
					UserQiangGangHu(ChairID, bCard, major.m_oFan);
				}
			}
			
			fan_num = Calc_Fan(ChairID, major.m_oFan);
		}
		m_GameLogic.ChuPai(pHc, bCard);
	}	
	else if (m_bRobGangHuJudge || (m_bRobWanGang  && m_bAddFanFlag[m_WangGangPlayer]))
	{
		m_GameLogic.AddCard(pHc, bCard);

		iOpType |= Check_Hu(ChairID, *pHc, major.m_oFan);

		if (iOpType & TYPE_HU)
		{
			UserQiangGangHu(ChairID, bCard, major.m_oFan);
			fan_num = Calc_Fan(ChairID, major.m_oFan);
		}
		m_GameLogic.ChuPai(pHc, bCard);
	}
	return iOpType;	
}

int Table::GetCardEstimate(_uint8 ChairID, _uint8 bCard, MJ_major& major, int& fan_num)
{
	int iOpType = TYPE_NULL;

	HandCards* pHc = &m_HandCard[ChairID];	
	m_GameLogic.AddCard(pHc, bCard);
	
	if (m_conf.m_bCheckCardNum && pHc->ChangeableCardsLen % 3 != 2)
	{
		log_error("%s tid:%d fatal paishu error uid:%d chairid:%d!\n", __FUNCTION__, tid, GetPlayerUid(ChairID), ChairID);
		log_error("%s cards(%d):%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x\n", __FUNCTION__, pHc->ChangeableCardsLen,
			pHc->ChangeableCards[0], pHc->ChangeableCards[1], pHc->ChangeableCards[2], pHc->ChangeableCards[3],
			pHc->ChangeableCards[4], pHc->ChangeableCards[5], pHc->ChangeableCards[6], pHc->ChangeableCards[7],
			pHc->ChangeableCards[8], pHc->ChangeableCards[9], pHc->ChangeableCards[10], pHc->ChangeableCards[11],
			pHc->ChangeableCards[12], pHc->ChangeableCards[13]);		
		throw("card num is error\n");
		return TYPE_NULL;
	}
		
	iOpType |= GetGangInfo(ChairID, major.m_GetGangInfo);

	iOpType |= Check_Hu(ChairID, *pHc, major.m_oFan);
	if (m_bGetCardCount[ChairID] == 1)
	{
		UserGetFirstCard(ChairID, iOpType, major.m_oFan);
	}		
	if (CardPoolIsEmpty())
	{
		UserGetLastCard(ChairID, iOpType, major.m_oFan);
	}		
	if (iOpType & TYPE_HU)
	{
 		if (m_bAddFanFlag[ChairID])   //杠上花
 		{
			UserGangShangHua(ChairID, bCard, major.m_oFan); 			
 		}

		fan_num = Calc_Fan(ChairID, major.m_oFan);
	}

	m_GameLogic.ChuPai(pHc, bCard);

	return iOpType;
}
void Table::InitBanker()
{
	if (m_nPlayingTimes == 0)
	{
		m_bBanker = 0;
		for (int i = 0; i < m_GAME_PLAYER; i++)
		{
			if (m_SeatPlayer[i].player != NULL)
			{
				log_info("%s room %d player uid:%d, room owner:%d\n", __FUNCTION__, tid, GetPlayerUid(i), m_room_owner_uid);
				if (m_SeatPlayer[i].player->uid == m_room_owner_uid)
				{
					m_bBanker = m_SeatPlayer[i].seatid;
					break;
				}
			}
		}
	}
	if (m_bBanker >= m_GAME_PLAYER)
	{
		m_bBanker = 0;
	}
	assert(m_bBanker < m_GAME_PLAYER);
}
void Table::TurnBanker()
{

	m_bBanker = NextChair(m_bBanker);

}

int Table::Check_Hu(_uint8 ChairID, HandCards& pHc, MJ_fan_buffer& oFan)
{

	if (pHc.ChangeableCardsLen > HAND_CARD_SIZE_MAX)
	{
		return TYPE_NULL;
	}
	//取余3不等于2的手牌不做胡牌判断
	if (pHc.ChangeableCardsLen != 0 && pHc.ChangeableCardsLen % 3 != 2)
	{
		return TYPE_NULL;
	}		
	MJ_win_pattern pattern;
	if (Is7Dui(ChairID, pHc, oFan))
	{
		return TYPE_HU;
	}	
	else if (Is13Yao(pHc, oFan, pattern))
	{		
		return TYPE_HU;
	}
	if (FormatCards(&pHc, pattern))
	{
		return Check_Hu_Pattern(ChairID, pHc, oFan, pattern);
	}
	
	return TYPE_NULL;
}
int Table::Check_Hu_Pattern(_uint8 ChairID, const HandCards& pHc, MJ_fan_buffer& oFan, const MJ_win_pattern& oPattern)
{
	oFan.Add(MJ_FAN_TYPE_PINGHU);
	return TYPE_HU;
}

int Table::Calc_Fan(_uint8 ChairID, MJ_fan_buffer& oFan)
{
	
	return 0;
}
int Table::Calc_Hu(_uint8 ChairID, _uint8 bCard, int Score[GAME_PLAYER])
{

	m_oFan[ChairID].Clear();	
	m_HuFanNums[ChairID] = 0;

	MJ_major major;
	if (m_bRobHu)
	{
		int iOpType = OutCardEstimate(ChairID, bCard, major, m_HuFanNums[ChairID]);
		if (iOpType & TYPE_HU)
		{
			m_oFan[ChairID] = major.m_oFan;
			Score[ChairID] += m_HuFanNums[ChairID] * m_BaseScore;
			Score[m_WangGangPlayer] -= m_HuFanNums[ChairID] * m_BaseScore;

			m_stHuScore[ChairID].stHu.Add(tagHuRecord(m_WangGangPlayer, bCard, HU_PAOHU, m_HuFanNums[ChairID]));
		}
		else
		{
			m_stHuScore[ChairID].stHu.Add(tagHuRecord(m_WangGangPlayer, bCard, HU_PAOHU, 0));
		}
	}
	else if (ChairID == m_bCurChairID)
	{//自摸
		int iOpType = GetCardEstimate(ChairID, INVALID_MJ_CARD, major, m_HuFanNums[ChairID]);
		if (iOpType & TYPE_HU)
		{			
			m_oFan[ChairID] = major.m_oFan;
			for (_uint8 c = NextChair(ChairID); c != ChairID; c = NextChair(c))
			{
				Score[ChairID] += m_HuFanNums[ChairID] * m_BaseScore;
				Score[c] -= m_HuFanNums[ChairID] * m_BaseScore;
			}
			m_stHuScore[ChairID].stHu.Add(tagHuRecord(ChairID, bCard, HU_ZIMO, m_HuFanNums[ChairID]));
		}		
		else
		{
			m_stHuScore[ChairID].stHu.Add(tagHuRecord(ChairID, bCard, HU_ZIMO,0));
		}
	}
	else
	{//放炮
		int iOpType = OutCardEstimate(ChairID, bCard, major, m_HuFanNums[ChairID]);
		if (iOpType & TYPE_HU)
		{
			m_oFan[ChairID] = major.m_oFan;
			Score[ChairID] += m_HuFanNums[ChairID] * m_BaseScore;			
			Score[m_bCurChairID] -= m_HuFanNums[ChairID] * m_BaseScore;

			m_stHuScore[ChairID].stHu.Add(tagHuRecord(m_bCurChairID, bCard, HU_PAOHU, m_HuFanNums[ChairID] ));
		}		
		else
		{
			m_stHuScore[ChairID].stHu.Add(tagHuRecord(m_bCurChairID, bCard, HU_PAOHU, 0));
		}
	}
	
	return 0;

}
void Table::UserGetFirstCard(_uint8 ChairID, int& iOpType, MJ_fan_buffer& oFan)
{
	if (iOpType & TYPE_HU)
	{
		if (ChairID == m_bBanker)
		{
			oFan.Add(MJ_FAN_TYPE_TIANHU);
		}		
	}	
}
void Table::UserFirstOutCard(_uint8 ChairID, int& iOpType, MJ_fan_buffer& oFan)
{
	if (iOpType & TYPE_HU && ChairID != m_bBanker)
	{
		oFan.Add(MJ_FAN_TYPE_DIHU);
	}
}

void Table::UserGetLastCard(_uint8 ChairID, int& iOpType, MJ_fan_buffer& oFan)
{
	if (iOpType & TYPE_HU)
	{
		oFan.Add(MJ_FAN_TYPE_HAIDILAOYUE);
	}

	//最后一张牌不能杠
	iOpType &= ~(TYPE_ANGANG | TYPE_WANGANG);	

}
void Table::UserLastOutCard(_uint8 ChairID, int& iOpType, MJ_fan_buffer& oFan)
{
	
	iOpType &= ~(TYPE_PENG | TYPE_CHI | TYPE_GANG);
}
void Table::UserGangShangHua(_uint8 ChairID, _uint8 bCard, MJ_fan_buffer& oFan)
{
	oFan.Add(MJ_FAN_TYPE_GANGSHANGHUA);
}
void Table::UserGangHouPao(_uint8 ChairID, _uint8 bCard, MJ_fan_buffer& oFan)
{
	oFan.Add(MJ_FAN_TYPE_GANGHOUPAO);
}
void Table::UserQiangGangHu(_uint8 ChairID, _uint8 bCard, MJ_fan_buffer& oFan)
{
	oFan.Add(MJ_FAN_TYPE_QIANGGANGHU);
}

void Table::Proto_Game_OpNotify(_uint8 ChairID, _uint8 CurChairID, _uint8 bCard, proto::game::AckOpNotify& stNotify)
{
	
	stNotify.Clear();
	stNotify.set_chairid(ChairID);
	//非提示玩家,下面的协议字段为空
	if (CurChairID == ChairID)
	{
		stNotify.set_optype(m_bUserOperate[ChairID]);
		stNotify.set_card(bCard);		
	}
	if (m_bUserOperate[ChairID] & TYPE_HU)
	{
		if (m_bRobGangHuJudge)
		{
			stNotify.set_hu_type(1);
		}
	}

}
void Table::Proto_Game_OpNotifyAfterPengChi(_uint8 ChairID, _uint8 CurChairID, const MJ_opinfo& gang, proto::game::AckOpNotify& stNotify)
{
	stNotify.Clear();
	stNotify.set_chairid(ChairID);
	//非提示玩家,下面的协议字段为空
	if (CurChairID == ChairID)
	{
		stNotify.set_optype(m_bUserOperate[ChairID]);
		stNotify.set_card(0);
		for (int i = 0; i < gang.Size(); i++)
		{
			proto::game::tagGangInfo* ganginfo = stNotify.add_gang();
			ganginfo->set_card(gang[i].cbCard);
			ganginfo->set_state(gang[i].nState);
		}		
	}
}
void Table::Proto_Game_UserGetCard(_uint8 ChairID, _uint8 CurChairID, _uint8 bCard, proto::game::AckUserGetCard& stUsergetcard)
{
	//_uint8 tempChairid = m_SeatPlayer[ChairID].player->seatid;
	stUsergetcard.Clear();

	stUsergetcard.set_chairid(CurChairID);
	stUsergetcard.set_leftcardnum(m_CardPool.Count());

	//如果是摸牌用户，则打包摸牌、听牌等数据，非摸牌用户则不发送这些数据
	if (ChairID == CurChairID)
	{
		stUsergetcard.set_card(bCard);

		if (m_bUserOperate[CurChairID] & TYPE_GANG_EXT)
		{
			MJ_opinfo stGang;			
			GetGangInfo(CurChairID, stGang);

			for (int i = 0; i < stGang.Size(); i++)
			{
				//log_info("玩家还没胡牌,此时暗杠牌有:0x%02x \n", stGang[i].cbCard);
				log_info("%s uid:%d chairID:%d angang:0x%02x\n", __FUNCTION__, GetPlayerUid(ChairID), ChairID, stGang[i].cbCard);
				proto::game::tagGangInfo *pstGang = stUsergetcard.add_ganginfo();
				pstGang->set_card(stGang[i].cbCard);
				pstGang->set_state(stGang[i].nState);
			}			
		}
		//听牌判断 TODO
		tagListenInfos stListenInfo[14];
		_uint8 bListenLen = 0;		
		//胡牌不用判断听牌，自动胡牌不用判断听牌
		if (!(m_bUserOperate[ChairID] & TYPE_HU))
		{
			m_bUserOperate[ChairID] |= GetListeningInfo(ChairID, stListenInfo, bListenLen);
		}
		stUsergetcard.set_optype(m_bUserOperate[CurChairID]);
		if (m_bUserOperate[CurChairID] & TYPE_LISTEN)
		{
			if (m_send_listen_info)
			{
				//打包听牌信息
				for (int j = 0; j < bListenLen; j++)
				{
					PackageTingInfo(ChairID, stListenInfo, j, stUsergetcard.add_listeninfo());

				}
			}
		}
	}
}

void Table::Proto_Game_UserOutCard(_uint8 ChairID, _uint8 bCard, proto::game::AckUserOutCard& stUseroutcard)
{
	stUseroutcard.Clear();
	stUseroutcard.set_chairid(ChairID);
	stUseroutcard.set_card(bCard);
	for (int i = 0; i < m_GAME_PLAYER; i++)
	{
		proto::game::tagShowOutCard *pstShow = stUseroutcard.add_showcard();
		for (int j = 0; j < m_stUserOutCard[i].Size(); j++)
		{
			pstShow->add_outcard(m_stUserOutCard[i][j]);
		}
		pstShow->set_outcardlen(m_stUserOutCard[i].Size());
	}
}
#ifndef __REFINE__
void Table::Proto_Game_OperateGang(_uint8 ChairID, _uint8 bCard, int iOpType, int iGangScore[GAME_PLAYER], proto::game::AckBcOpResult& stOpresult)
{
	stOpresult.set_chairid(ChairID);
	stOpresult.set_outcardchairid(m_bOutCardChairID);
	stOpresult.set_optype(iOpType);
	stOpresult.set_card(bCard);
	for (int i = 0; i < m_GAME_PLAYER; i++)
	{
		stOpresult.add_score(iGangScore[i]);		
	}

	if (m_bGangCalcImme)
	{
		Update_Score(iGangScore);		
		
	}
	for (int i = 0; i < m_GAME_PLAYER; i++)
	{
		switch (iOpType)
		{
		case TYPE_ZHIGANG: m_SeatPlayer[i].player->m_zhigang += iGangScore[i]; break;
		case TYPE_WANGANG: m_SeatPlayer[i].player->m_wangang += iGangScore[i]; break;
		case TYPE_ANGANG: m_SeatPlayer[i].player->m_angang += iGangScore[i]; break;
		}
	}	
}
#endif
void Table::SendTingInfo(_uint8 ChairID)
{
	tagListenInfos stListenInfo[14];
	_uint8 bListenLen = 0;	
	//胡牌不用判断听牌，自动胡牌不用判断听牌
	if (GetListeningInfo(ChairID, stListenInfo, bListenLen))
	{
		m_bUserOperate[ChairID] |= TYPE_LISTEN;
	}
	
	// 发送听牌协议
	proto::game::AckListenNotify stListen;
	if (m_bUserOperate[ChairID] & TYPE_LISTEN)
	{
		log_info("%s tid:%d uid:%d chairid:%d ListenLen:%d \n", __FUNCTION__, tid, GetPlayerUid(ChairID), ChairID, bListenLen);
		stListen.set_optype(TYPE_LISTEN);
		if (m_send_listen_info)
		{
			//打包听牌信息
			for (int j = 0; j < bListenLen; j++)
			{
				PackageTingInfo(ChairID, stListenInfo, j, stListen.add_listeninfo());
			}
		}
	}
	else
	{
		stListen.set_optype(TYPE_NULL);
		log_info("%s uid:%d chairid:%d no listen\n", __FUNCTION__, GetPlayerUid(ChairID), ChairID);
	}
	unicast(SERVER_LISTEN_NOTIFY, ChairID, stListen);
	
}

void Table::SendTingInfo_Simple(_uint8 ChairID)
{
	tagListenInfos stListenInfo[14];
	_uint8 bListenLen = 0;
	//胡牌不用判断听牌，自动胡牌不用判断听牌
	if (IsListening(ChairID, stListenInfo, bListenLen))
	{
		m_bUserOperate[ChairID] |= TYPE_LISTEN;
	}

	// 发送听牌协议
	proto::game::AckListenNotify stListen;
	if (m_bUserOperate[ChairID] & TYPE_LISTEN)
	{
		log_info("%s uid:%d chairid:%d ListenLen:%d \n", __FUNCTION__, GetPlayerUid(ChairID), ChairID, bListenLen);
		stListen.set_optype(TYPE_LISTEN);

		PackageTingInfo(ChairID, stListenInfo, 0, stListen.add_listeninfo());
		
	}	
	unicast(SERVER_LISTEN_NOTIFY, ChairID, stListen);
}
void Table::UserAfterOperate(_uint8 ChairID, int iOpType, _uint8 bCard)
{
	m_bCurChairID = ChairID;
	log_info("%s tid:%d uid:%d chairid:%d operate:0x%x card:0x%02x\n", __FUNCTION__, tid, GetPlayerUid(ChairID), ChairID, iOpType, bCard);

	m_bUserOperate[ChairID] = TYPE_NULL;
	MJ_opinfo gang;
	m_bUserOperate[ChairID] = GetGangInfo(m_bCurChairID, gang);
	
	if (gang.Size() == 0)
	{
		set_out_timer(m_bCurChairID);
	}
	else
	{
		proto::game::AckOpNotify stNotify;		
		for (int i = 0; i < m_GAME_PLAYER; i++)
		{
			Proto_Game_OpNotifyAfterPengChi(ChairID, i, gang, stNotify);		
			unicast(SERVER_OPERATE_NOTIFY, i, stNotify);
		}
		set_operator_timer(ChairID);
	}	
	m_trustee.get()->UserAfterOperate(ChairID);
}

void Table::DoMultiOperate(_uint8 ChairID, int iOpType, _uint8 bCard)
{
	m_bMultiDoneOpNum++;
	m_bMultiUserDoneOp[ChairID] = iOpType;
	m_bMultiUserDoneOpCard[ChairID] = bCard;
	log_info("%s tid:%d uid:%d chaird:%d, curoptype:0x%x, optype:0x%x, now done OpNum:%d, totalOpnum:%d \n", __FUNCTION__, tid,
		GetPlayerUid(ChairID), ChairID, m_bUserOperate[ChairID], iOpType, m_bMultiDoneOpNum, m_bMultiOpUserNum);
	if (iOpType == TYPE_HU)
	{
		m_bMulitFirstOperate = true;		
		m_bMulitHuOpeCount++;
		
		for (_uint8 c = NextChair(ChairID); c != ChairID; c = NextChair(c))
		{
			if (c == m_bCurChairID)
				continue;
			if (m_bUserOperate[c] & TYPE_HU)
				continue;
			if (m_bUserOperate[c] == TYPE_NULL)
				continue;
			if (m_bMultiUserDoneOp[c] == TYPE_NULL)
			{
				m_bMultiDoneOpNum++;
				m_bMultiUserDoneOp[c] = TYPE_PASS;
				log_info("%s force user uid:%d pass seat:%d!\n", __FUNCTION__, GetPlayerUid(c), c);
				UserOperatePass(c, TYPE_PASS, bCard);
			}
		}
		if (m_conf.m_bYiPaoDuoXiang)
		{
			m_louhu[ChairID].Unlock();
			UserOperateHu(ChairID, m_bCurCard);

			if (m_conf.m_bFourceHuMutilHu)
			{
				DoMultiForceHu(ChairID, m_bCurCard);
			}
		}

	}
	else
	{
		if (iOpType == TYPE_PASS)
		{
			if (m_bUserOperate[ChairID] & TYPE_HU)   //胡牌玩家选择了过牌
			{
				m_bMulitHuOpeCount++;
				log_info("%s user uid:%d pass seat:%d, optype:0x%x!\n", __FUNCTION__, GetPlayerUid(ChairID), ChairID, m_bUserOperate[ChairID]);
				m_PassHuPlayer.Add(ChairID);
			}
			UserOperatePass(ChairID, iOpType, bCard);
		}
		else
		{
			if (iOpType != TYPE_NULL)
			{				
				assert(iOpType != TYPE_PASS);
				assert(m_bRecordMultiOp.iOpType != TYPE_PASS);
				if (
					(m_bRecordMultiOp.iOpType == TYPE_NULL && (iOpType & (TYPE_CHI | TYPE_PENG | TYPE_GANG_EXT | TYPE_HU))) ||
					((iOpType & TYPE_GANG_EXT) && (m_bRecordMultiOp.iOpType & (TYPE_CHI | TYPE_PENG| TYPE_CAI_GANG_X))) ||
					((iOpType & TYPE_PENG) && (m_bRecordMultiOp.iOpType & (TYPE_CHI| TYPE_CAI_GANG_X))) 					
					)					
				{
					m_bRecordMultiOp.iOpType = iOpType;
					m_bRecordMultiOp.ChairID = ChairID;
					m_bRecordMultiOp.bCard = bCard;
				}			
				assert(m_bRecordMultiOp.iOpType != TYPE_NULL);
			}
		}
	}
	if (m_bMultiDoneOpNum >= m_bMultiOpUserNum)
	{
		if (m_bMulitFirstOperate)
		{			
			if (!m_conf.m_bYiPaoDuoXiang)
			{
				for (_uint8 c = NextChair(m_bCurChairID); c != m_bCurChairID;c=NextChair(c))
				{
					if (m_bMultiUserDoneOp[c] == TYPE_HU)
					{
						UserOperateHu(c, m_bCurCard);
						break;
					}					
				}				
			}
			GameEnd();
			init_multi_data();
		}
		else
		{
			assert(m_bRecordMultiOp.iOpType != TYPE_PASS);			
			if (m_conf.m_bMultiOpSpecail)
			{
				m_bMultiOpUserNum = 0;
				m_bMultiOpHuNum = 0;
				if (m_bRecordMultiOp.iOpType != TYPE_NULL)
				{
					MultiOperateOver(m_bRecordMultiOp.ChairID, m_bRecordMultiOp.iOpType, m_bRecordMultiOp.bCard);
				}
				else
				{
					MultiOperateOver(ChairID, iOpType, bCard);

					OnUserGetCard(NextChair(m_bCurChairID));
				}
				m_bMulitFirstOperate = false;//数据初始化						
				m_bMultiDoneOpNum = 0;
				m_bMulitHuOpeCount = 0;
				m_bRecordMultiOp.iOpType = TYPE_NULL;
				m_bRecordMultiOp.ChairID = 0xFF;
				m_bRecordMultiOp.bCard = 0xFF;
				memset(m_bMultiUserDoneOp, 0, sizeof(m_bMultiUserDoneOp));
				memset(m_bMultiUserDoneOpCard, 0, sizeof(m_bMultiUserDoneOpCard));
				m_PassHuPlayer.Clear();
			}
			else
			{
				if (m_bRecordMultiOp.iOpType != TYPE_NULL)
				{
					MultiOperateOver(m_bRecordMultiOp.ChairID, m_bRecordMultiOp.iOpType, m_bRecordMultiOp.bCard);
				}
				else
				{
					MultiOperateOver(ChairID, iOpType, bCard);

					OnUserGetCard(NextChair(m_bCurChairID));
				}
			}
			if (!m_conf.m_bMultiOpSpecail)
			{
				init_multi_data();
			}
		}		
	}
	else//优先级操作了，优先级低的就忽略了
	{
		if (m_bMulitFirstOperate)
		{
			if (m_bMulitHuOpeCount != m_bMultiOpHuNum)
			{
				return;
			}
			/*未到达*/
			if (m_conf.m_bYiPaoDuoXiang)
			{//一炮多响,其他玩家自动胡
				
			}
			else
			{
				for (_uint8 c = NextChair(m_bCurChairID); c != m_bCurChairID; c = NextChair(c))
				{
					if (m_bMultiUserDoneOp[c] == TYPE_HU)
					{
						UserOperateHu(c, m_bCurCard);
						break;
					}
				}
				
			}
			init_multi_data();
			GameEnd();
		}
		else
		{
			bool bOver = true;			
			for (_uint8 c = NextChair(m_bCurChairID); c != m_bCurChairID; c = NextChair(c))
			{
				if (m_bMultiUserDoneOp[c] != TYPE_NULL)
					continue;
				int tmpOpType = m_bUserOperate[c];
				if (tmpOpType == TYPE_NULL)
					continue;				
				if (
					(m_bRecordMultiOp.iOpType == TYPE_NULL && (tmpOpType & (TYPE_CHI | TYPE_PENG | TYPE_GANG_EXT | TYPE_HU | TYPE_CAI_GANG | TYPE_CHUAI_GANG))) ||
					(tmpOpType & TYPE_HU) ||		
					(tmpOpType & (TYPE_GANG_EXT | TYPE_CHUAI_GANG | TYPE_CHUAI_CAI_GANG) && (m_bRecordMultiOp.iOpType & (TYPE_CHI | TYPE_PENG | TYPE_GANG_EXT | TYPE_CAI_GANG | TYPE_CHUAI_GANG))) ||
					(tmpOpType & TYPE_PENG && (m_bRecordMultiOp.iOpType & (TYPE_CHI | TYPE_PENG | TYPE_CAI_GANG )))
					)
				{
					bOver = false;
					break;
				}
			}
			if (bOver)   //优先级高的玩家操作后，立即停止多操作
			{
				for (int i = 0; i < m_GAME_PLAYER; i++)  //其他低操作玩家服务器主动发过操作
				{
					if (i == m_bRecordMultiOp.ChairID)
					{
						continue;
					}
					if (m_bUserOperate[i] != TYPE_NULL && m_bMultiUserDoneOp[i] == TYPE_NULL)
					{
						UserOperatePass(i, TYPE_PASS, bCard);
					}

				}
				if (m_conf.m_bMultiOpSpecail)
				{
					m_bMultiOpUserNum = 0;
					m_bMultiOpHuNum = 0;
					MultiOperateOver(m_bRecordMultiOp.ChairID, m_bRecordMultiOp.iOpType, m_bRecordMultiOp.bCard);
					m_bMulitFirstOperate = false;//数据初始化						
					m_bMultiDoneOpNum = 0;
					m_bMulitHuOpeCount = 0;
					m_bRecordMultiOp.iOpType = TYPE_NULL;
					m_bRecordMultiOp.ChairID = 0xFF;
					m_bRecordMultiOp.bCard = 0xFF;
					memset(m_bMultiUserDoneOp, 0, sizeof(m_bMultiUserDoneOp));
					memset(m_bMultiUserDoneOpCard, 0, sizeof(m_bMultiUserDoneOpCard));
					m_PassHuPlayer.Clear();
				}
				else
				{
					MultiOperateOver(m_bRecordMultiOp.ChairID, m_bRecordMultiOp.iOpType, m_bRecordMultiOp.bCard);
					init_multi_data();
				}
			}

		}
	}
}

void Table::init_multi_data()
{
	m_bMulitFirstOperate = false;//数据初始化
	m_bRecordMultiOp.iOpType = TYPE_NULL;
	m_bRecordMultiOp.ChairID = 0xFF;
	m_bRecordMultiOp.bCard = 0xFF;
	m_bMultiDoneOpNum = 0;
	m_bMultiOpUserNum = 0;
	m_bMulitHuOpeCount = 0;
	m_bMultiOpHuNum = 0;
	memset(m_bMultiUserDoneOp, 0, sizeof(m_bMultiUserDoneOp));
	memset(m_bMultiUserDoneOpCard, 0, sizeof(m_bMultiUserDoneOpCard));
	m_PassHuPlayer.Clear();
}

#ifndef __REFINE__
void Table::MultiOperateOver(_uint8 ChairID, int iOpType, _uint8 bCard)
{
	if (iOpType == TYPE_PASS)
	{
		m_bAddFanFlag[m_bOutCardChairID] = false;
		return;
	}

	for (_uint8 c = NextChair(m_bCurChairID); c != ChairID;c = NextChair(c))
	{
		if (m_bMultiUserDoneOp[c] == TYPE_PASS)
			continue;
		if (MultiOpOr(iOpType, TYPE_GANG_EXT))		
		{
			ChairID = c;
			iOpType = m_bMultiUserDoneOp[c];
			bCard = m_bMultiUserDoneOpCard[c];
			break;
		}
		else if (MultiOpOr(iOpType, TYPE_PENG))
		{
			ChairID = c;
			iOpType = m_bMultiUserDoneOp[c];
			bCard = m_bMultiUserDoneOpCard[c];
			break;
		}
		else if (MultiOpOr(iOpType, TYPE_CHI))
		{
			ChairID = c;
			iOpType = m_bMultiUserDoneOp[c];
			bCard = m_bMultiUserDoneOpCard[c];
			break;
		}
	}
	switch (iOpType)
	{
	case TYPE_LEFT_CHI:
	case TYPE_CENTER_CHI:
	case TYPE_RIGHT_CHI:	
		UserOperateChi(ChairID, iOpType, bCard);
		break;
	case TYPE_PENG:	
		UserOperatePeng(ChairID, bCard);
		break;	
	case TYPE_WANGANG:
	case TYPE_ANGANG:		
	case TYPE_ZHIGANG:
	case TYPE_DATOUZI_AN:
	case TYPE_DATOUZI_WAN:
	case TYPE_DATOUZI_ZHI:
		if (m_conf.m_bQiangZhiGang && RobGangHu(ChairID, bCard, iOpType))
			return;
		UserOperateGang(ChairID, iOpType, bCard);

		OnUserGetCard(ChairID, true);
		break;	
	default:
		break;
	}
}
#endif
int Table::GetChi(_uint8 ChairID, _uint8 bCard, MJ_opinfo& ChiInfo)
{
	if (m_conf.m_bSupportChi)
	{
		if (NextChair(m_bCurChairID) == ChairID)
		{
			return m_GameLogic.GetChi(&m_HandCard[ChairID], bCard, ChiInfo);
		}
	}
	return TYPE_NULL;	
}
int Table::GetGangInfo(_uint8 ChairID, MJ_opinfo& GangCardInfo)
{
	return m_GameLogic.GetGangInfo(&m_HandCard[ChairID], GangCardInfo);
}
int Table::GetPengGang(_uint8 ChairID, _uint8 cbCard, MJ_opinfo& GangCardInfo)
{
	return m_GameLogic.GetPengGang(&m_HandCard[ChairID], cbCard, GangCardInfo);	
}

bool Table::FormatCards(HandCards *pHc, MJ_win_pattern& pattern)
{
	return m_GameLogic.FormatCards(pHc, pattern.kan, pattern.jiang);
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

bool Table::IsListening(_uint8 ChairID, tagListenInfos stListenInfo[14], _uint8 &bListenLen)
{	
	HandCards tmp = m_HandCard[ChairID];
	memset(stListenInfo, 0, sizeof(tagListenInfos) * 14);
	bListenLen = 0;
	Check_Listening(&tmp, ChairID, 0, stListenInfo, bListenLen);

	return bListenLen != 0;
}

void Table::SetRobGangHu_Flag(_uint8 ChairID, int iOpType, proto::game::AckBcOpResult& stOpresult)
{
	if (Is_HuOperator(iOpType) && m_bRobWanGang && m_bAddFanFlag[m_WangGangPlayer])
	{
		stOpresult.set_isqiangganghu(1);
	}
}
bool Table::CardPoolIsEmpty()
{
	return m_CardPool.IsEmpty();
}

bool Table::CheckPlayerSkey(const Json::Value& val, const Client* client)
{
	return false;
}

int Table::dispatch_client(int cmd, Client *client, Player *player)
{
	if (client == NULL || player == NULL)
	{
		log_error("%s tid:%d client has disbanded \n", __FUNCTION__ , tid);
		return -1;
	}
	//log_debug("matchid<%d>, tid<%d>, uid<%d>, cmd<%d>", matchid, tid, client->uid, cmd);
	if (m_bDeleteTable)
	{
		log_error("%s tid:%d table has deleted cmd:%d uid:%d \n", __FUNCTION__, tid, cmd, GetPlayerUid(player));
		return 0;
	}
	if (client->m_Buffer == NULL)
		return -1;

	switch (cmd)
	{
	case CLIENT_CHAT_REQ:
		handler_chat(client);
		return 0;
	case CLIENT_EMOTION_REQ:
		handler_interaction_emotion(client, player);
		return 0;
	case CLIENT_TABLE_INFO_REQ:
		handler_table_info(player);
		return 0;
	case CLIENT_UPTABLE_APPLY_REQ:
		handler_uptable(player, false);
		return 0;
	case CLIENT_DOWNTABLE_REQ:
		handler_downtable(player);
		return 0;
	case CLIENT_MJ_OUT_CARD:
		recv_client_outcard(client, player);
		return 0;
	case CLIENT_MJ_OPERATE_CARD:
		recv_client_opeatereuslt(client, player);
		return 0;
	case CLIENT_MJ_USER_ACTION:
		do {
			proto::game::ReqAction stAction;
			stAction.ParseFromArray(client->m_Buffer->proto_data, client->m_Buffer->proto_head.length);
			int action = stAction.action();
			if (action == proto::game::ACTION_CHANGE_TABLE)
			{
			}
			else
			{
				recv_client_action(client, player);
			}
		} while (false);
		return 0;
	case CLIENT_MJ_HOME_RETURN:
		log_info("Recv_Home_Return req tid:%d uid:%d\n", player->tid, player->uid);
		handler_table_info(player);
		Send_Game_Scene(player);
		return 0;
	case CLIENT_MJ_AUTOHU:
		RecvClientAutoHu(client, player);
		log_info("recv auto hu tid:%d uid:%d\n", player->tid, player->uid);
		return 0;
	case CLIENT_DISBAND_ROOM:
		RecvDisbandTableApply(player);
		return 0;
	case CLIENT_DISBAND_CHOOSE:
		do {
			proto::game::ReqChooseDisband stChoose;
			stChoose.ParseFromArray(client->m_Buffer->proto_data, client->m_Buffer->proto_head.length);
			int choosestype = stChoose.choosestate();
			RecvDisbandTableChoose(player->seatid, choosestype);
		} while (false);
		return 0;
	case CLIENT_HOME_OUT:
		User_Home_Out(player);
		return 0;
	case CLIENT_GAME_END_RECORD_REG:
	{	
		proto::game::hunanmj::GameEndRecord ger;		
		for (int i = 0; i < m_GAME_PLAYER; i++)
		{
			ger.add_userrecoreinfo();
		}
		Send_GameEnd_ScoreInfo(player, ger);
		unicast(SERVER_GAME_RECORD, player, ger);
		return 0;
	}
	case CLIENT_REQ_ROUND_SCORE:
		Send_GameRound_ScoreInfo(player);
		return 0;
	default:
		break;
	}
	log_error("%s invalid command[%d], tid:%d uid:%d\n", __FUNCTION__, cmd, tid, player->uid);
	return -1;
}


void Table::SendPlayerFirstCardOp(_uint8 ChairID)
{
	MJ_major major;
	m_bUserOperate[ChairID] = GetCardEstimate(ChairID, INVALID_MJ_CARD, major, m_HuFanNums[ChairID]);
	if (m_bUserOperate[ChairID] != TYPE_NULL && m_bUserOperate[ChairID] != TYPE_LISTEN)
	{
		for (int i = 0; i < m_GAME_PLAYER; i++)
		{
			proto::game::AckOpNotify stNotify;
			stNotify.Clear();

			stNotify.set_chairid(ChairID);
			if (ChairID == i)
			{//非打牌玩家
				int iOpType = m_bUserOperate[ChairID];
				if (m_bUserOperate[ChairID] & TYPE_GANG)
				{
					for (int i = 0; i < major.m_GetGangInfo.Size(); i++)
					{
						log_info("%s uid:%d chairID:%d angang:0x%02x\n", __FUNCTION__, GetPlayerUid(ChairID), ChairID, major.m_GetGangInfo[i].cbCard);
						proto::game::tagGangInfo *pstGang = stNotify.add_gang();
						pstGang->set_card(major.m_GetGangInfo[i].cbCard);
						pstGang->set_state(major.m_GetGangInfo[i].nState);
					}

				}
				stNotify.set_card(m_bCurCard);
				stNotify.set_optype(iOpType);
			}
			unicast(SERVER_OPERATE_NOTIFY, i, stNotify);
		}
	}
	if (m_conf.m_bQiShou_14_zhang)
	{
		if (m_bUserOperate[ChairID] == TYPE_NULL || m_bUserOperate[ChairID] == TYPE_LISTEN)
		{
			set_out_timer(ChairID);
		}
		else
		{
			set_operator_timer(ChairID);
		}
	}
}

void Table::GetFirstHandCard_NoOp(_uint8 ChairID, bool bGetLast)
{
	_uint8 bCard = bGetLast? m_CardPool.GetLast() : m_CardPool.GetOneCard();

	m_bCurChairID = ChairID;
	m_bGetCardCount[ChairID]++;
	m_bCurCard = bCard;

	m_GameLogic.AddCard(&m_HandCard[ChairID], bCard);

	save_user_get_card(ChairID, bCard, m_CardPool.Count());

	//广播庄家获得手牌
	proto::game::AckUserGetCard stUsergetcard;
	stUsergetcard.set_chairid(ChairID);
	stUsergetcard.set_leftcardnum(m_CardPool.Count());
	for (_uint8 c = NextChair(ChairID); c != ChairID; c = NextChair(c))
	{
		unicast(SERVER_USER_GETCARD, c, stUsergetcard);
	}
	stUsergetcard.set_card(bCard);
	stUsergetcard.set_optype(TYPE_NULL);

	unicast(SERVER_USER_GETCARD, ChairID, stUsergetcard);

	log_info("%s tid:%d uid:%d chairid:%d getcard:0x%02x optype:0x%x cardpoollen:%d left:%d\n", __FUNCTION__, tid, GetPlayerUid(ChairID), ChairID, bCard, m_bUserOperate[ChairID], m_CardPool.Count(), m_HandCard[ChairID].ChangeableCardsLen);
}

int Table::Check_DaJiao(_uint8 ChairID)
{
	HandCards tempHandCard = m_HandCard[ChairID];
	
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
	_uint8 CurCard_bk = m_bCurCard;
	int iMaxFan = 0;
	for (_uint8 i = 0; i < _relativity.Size(); ++i)
	{
		m_GameLogic.AddCard(&tempHandCard, _relativity[i]);
		m_bCurCard = _relativity[i];
		MJ_fan_buffer fan;
		
		if (Check_Hu(ChairID, tempHandCard, fan) == TYPE_HU)
		{
			int tmpFan = Calc_Fan(ChairID, fan);
			if (iMaxFan < tmpFan)
			{
				iMaxFan = tmpFan;
				m_oFan[ChairID] = fan;
			}
		}
		m_GameLogic.ChuPai(&tempHandCard, _relativity[i]);
	}
	m_HuFanNums[ChairID] = iMaxFan;
	m_bCurCard = CurCard_bk;
	return iMaxFan;
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

bool Table::hasQuePai(_uint8 ChairID, _uint8 & queCard)
{
	return false;
}
