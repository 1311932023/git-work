#include "table.h"
#include "table.h"

extern PHZ phz;
extern Log log;

#define MultiOpOr(X,Y)	(((X) & (Y) ? (Y) : 0) & (m_Seats[c].m_bMultiUserDoneOp & (Y) ? (Y) :0))


//--------------------------玩家操作或操作判断-----------------------------------
void Table::UserOperateChi(_uint8 ChairID, int iOpType, _uint8 bCard)
{

	memset(m_bAddFanFlag, 0, sizeof(m_bAddFanFlag));
	for (int i = 0; i < m_GamePlayer; i++)
	{
		m_Seats[i].m_bUserOperate = 0;
	}

	MJ_opinfo stChi;
	switch (iOpType)
	{
	case TYPE_LEFT_CHI:
	{
		//吃牌检查
		if (!(GetChi(ChairID, bCard, stChi) & TYPE_LEFT_CHI))
		{
			log.info("user_operate_chi ChairID:%d operate:0x%x，card: 0x%02x,but handcard has no cur operate \n", ChairID, iOpType, bCard);
			return;
		}
		m_GameLogic.ExecuteLeftChi(&m_Seats[ChairID].m_HandCard, m_CurOutSeat, bCard);		
	}
	break;
	case TYPE_CENTER_CHI:
	{
		//吃牌检查
		if (!(GetChi(ChairID, bCard, stChi) & TYPE_CENTER_CHI))
		{
			log.info("user_operate_chi ChairID:%d operate:0x%x，card: 0x%02x,but handcard has no cur operate \n", ChairID, iOpType, bCard);
			return;
		}
		m_GameLogic.ExecuteCenterChi(&m_Seats[ChairID].m_HandCard, m_CurOutSeat, bCard);
	}
	break;
	case TYPE_RIGHT_CHI:
	{
		//吃牌检查
		if (!(GetChi(ChairID, bCard, stChi) & TYPE_RIGHT_CHI))
		{
			log.info("user_operate_chi ChairID:%d operate:0x%x，card: 0x%02x,but handcard has no cur operate \n", ChairID, iOpType, bCard);
			return;
		}
		m_GameLogic.ExecuteRightChi(&m_Seats[ChairID].m_HandCard, m_CurOutSeat, bCard);
	}
	break;
	default:
		break;
	}
	m_Seats[ChairID].m_bGetCardCount++;

	m_Seats[m_CurOutSeat].m_bOutRecord.RemoveLast(bCard);
	
	//广播操作结果的消息
	BroadcastOperateResult(ChairID, iOpType, bCard);
	log.info(" tid:%s uid:%d ChairID:%d, bCard:%d \n", tid.c_str(), GetPlayerUid(ChairID), ChairID, bCard);

	SendTingInfo(ChairID);

	int score[GAME_PLAYER] = { 0 };
	SaveOperateVideo(iOpType, ChairID, m_CurOutSeat, bCard, score);

	UserAfterOperate(ChairID, iOpType, bCard);	

}

void Table::UserOperatePeng(_uint8 ChairID, _uint8 bCard)
{	
	log.info(" tid:%s uid:%d ChairID:%d, bCard:%d \n",  tid.c_str(), GetPlayerUid(ChairID), ChairID, bCard);
	memset(m_bAddFanFlag, 0, sizeof(m_bAddFanFlag));
	
	for (int i = 0; i < m_GamePlayer; i++)
	{
		m_Seats[i].m_bUserOperate = 0;
	}
	BroadcastOperateResult(ChairID, TYPE_PENG, bCard);

	int fixedlen = m_Seats[ChairID].m_HandCard.FixedCardsLen;
	if (fixedlen >= HAND_CARD_KAN_MAX)
	{
		log.error("玩家组合牌长度:%d大于正常的长度 \n", fixedlen);
		return;
	}
	m_GameLogic.ExecutePeng(&m_Seats[ChairID].m_HandCard, m_CurOutSeat, bCard);
	m_Seats[ChairID].m_bGetCardCount++;
	//从打出牌删掉这张牌	
	m_Seats[m_CurOutSeat].m_bOutRecord.RemoveLast(bCard);

	//TODO 检查是否听牌
	SendTingInfo(ChairID);
	
	UserAfterOperate(ChairID, TYPE_PENG, bCard);

	int score[GAME_PLAYER] = { 0 };
	SaveOperateVideo(TYPE_PENG, ChairID, m_CurOutSeat, bCard, score);
}

void Table::UserOperateGang(_uint8 ChairID, int iOpType, _uint8 bCard)
{
	memset(m_bAddFanFlag, 0, sizeof(m_bAddFanFlag));
	for (int i = 0; i < m_GamePlayer; i++)
	{
		m_Seats[i].m_bUserOperate = 0;
	}

	log.info(" tid:%s uid:%d ChairID:%d, iOpType:%d, bCard:%d \n", tid.c_str(), GetPlayerUid(ChairID), ChairID, iOpType, bCard);
	//直杠 2*底分，弯杠1*底分，暗杠2*底分*3
	int iGangScore[GAME_PLAYER] = {0};
	_uint8 bTempChairID = INVALID_CHAIRID;
	m_bAddFanFlag[ChairID] = true;
	switch (iOpType)
	{
		case TYPE_ANGANG:
		{
			for (_uint8 c = NextChair(ChairID); c != ChairID; c = NextChair(c))
			{
				iGangScore[ChairID] += base_score * m_conf.m_AnGangPoint;
				iGangScore[c] -= base_score * m_conf.m_AnGangPoint;
			}
			m_stGang[ChairID].Gang.Add(tagGangScore(bCard, ChairID, TYPE_ANGANG));				
			bTempChairID = ChairID;
		}
		break;
		case TYPE_WANGANG:
		{	

			for (_uint8 c = NextChair(ChairID); c != ChairID; c = NextChair(c))
			{
				iGangScore[ChairID] += base_score * m_conf.m_WanGangPoint;
				iGangScore[c] -=  base_score * m_conf.m_WanGangPoint;					
			}
			m_stGang[ChairID].Gang.Add(tagGangScore(bCard, ChairID, TYPE_WANGANG));
			bTempChairID = ChairID;				
		}
		break;
		case TYPE_ZHIGANG:
		{
			iGangScore[ChairID] +=  base_score * m_conf.m_ZhiGangPoint;
			iGangScore[m_CurOutSeat] -=  base_score *  m_conf.m_ZhiGangPoint;

			m_stGang[ChairID].Gang.Add(tagGangScore(bCard, m_CurOutSeat, TYPE_ZHIGANG));

			bTempChairID = m_CurOutSeat;

			m_Seats[ChairID].m_bGetCardCount++;
			//从打出牌删掉这张牌
			m_Seats[m_CurOutSeat].m_bOutRecord.RemoveLast(bCard);
		}
		break;
		default:
		log.error("UserOperateGang, wrong optype:%d \n", iOpType);
		break;
	}
	

	m_GameLogic.ExecuteGang(&m_Seats[ChairID].m_HandCard, bTempChairID, bCard, iOpType);

	Calc_Score(m_stGang, iGangScore);

	proto::game::AckBcOpResult stOpresult;

	Proto_Game_OperateGang(ChairID, bCard, iOpType, iGangScore, stOpresult);

	save_video_data(SERVER_OPERATE_RESULT, stOpresult.SerializeAsString());
	Ppacket ppack;
	stOpresult.SerializeToString(&ppack.body);
	ppack.pack(SERVER_OPERATE_RESULT);
	broadcast(NULL, ppack.data);

	m_Seats[ChairID].m_bUserOperate = TYPE_NULL;

	
}

int Table::OutCardEstimate(_uint8 ChairID, _uint8 bCard, MJ_major& major, int& fan_num)
{	
	int iOpType = TYPE_NULL;
	
	HandCards* pHc = &m_Seats[ChairID].m_HandCard;
		
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
		
		
		if (m_Seats[m_CurSeat].m_bGetCardCount == 1)
		{//庄家打出第一张牌
			for (_uint8 c = NextChair(m_CurSeat); c != m_CurSeat; c = NextChair(c))
			{
				if (m_Seats[c].m_bGetCardCount != 0)
					goto BANKER_FIRST_OUTCARD;
			}
			UserFirstOutCard(ChairID, iOpType, major.m_oFan);			
		}
BANKER_FIRST_OUTCARD:
		if (iOpType & TYPE_HU)
		{
			if (m_conf.m_bGangShangPao)
			{
				if (!m_bRobWanGang && m_bAddFanFlag[m_CurSeat]) //杠上炮
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

	HandCards* pHc = &m_Seats[ChairID].m_HandCard;	
	m_GameLogic.AddCard(pHc, bCard);

	if (pHc->ChangeableCardsLen % 3 != 2)
	{
		log.error("tid:%s fatal paishu error uid:%d chairid:%d!\n", tid.c_str(), GetPlayerUid(ChairID), ChairID);
		m_GameLogic.ChuPai(pHc, bCard);
		return TYPE_NULL;
	}
		
	iOpType |= GetGangInfo(ChairID, major.m_GetGangInfo);

	iOpType |= Check_Hu(ChairID, *pHc, major.m_oFan);
	if (m_Seats[ChairID].m_bGetCardCount == 1)
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
	if (m_CurTurn == 1)
	{
		m_CurSeat = rand() % m_GamePlayer;
		for (int i = 0; i < m_GamePlayer; i++)
		{
			if (m_Seats[i].m_Player != NULL)
			{
				log.info("room %d player uid:%d, room owner:%d\n", tid.c_str(), GetPlayerUid(i), m_RoomUserId);
				if (m_Seats[i].m_Player->uid == m_RoomUserId)
				{
					m_CurSeat = m_Seats[i].m_SeatId;
					break;
				}
			}
		}
	}
	if (m_CurSeat >= m_GamePlayer)
	{
		m_CurSeat = rand() % m_GamePlayer;
	}
}


int Table::Check_Hu(_uint8 ChairID, HandCards& pHc, MJ_fan_buffer& oFan)
{

	if (pHc.ChangeableCardsLen > 14)
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
			Score[ChairID] += m_HuFanNums[ChairID] * base_score;
			Score[m_WangGangPlayer] -= m_HuFanNums[ChairID] * base_score;

			m_stHuScore[ChairID].stHu.Add(tagHuRecord(m_WangGangPlayer, bCard, HU_PAOHU, m_HuFanNums[ChairID]));
		}
	}
	else if (ChairID == m_CurSeat)
	{//自摸
		int iOpType = GetCardEstimate(ChairID, INVALID_MJ_CARD, major, m_HuFanNums[ChairID]);
		if (iOpType & TYPE_HU)
		{			
			m_oFan[ChairID] = major.m_oFan;
			for (_uint8 c = NextChair(ChairID); c != ChairID; c = NextChair(c))
			{
				Score[ChairID] += m_HuFanNums[ChairID] * base_score;
				Score[c] -= m_HuFanNums[ChairID] * base_score;
			}
			m_stHuScore[ChairID].stHu.Add(tagHuRecord(ChairID, bCard, HU_ZIMO, m_HuFanNums[ChairID]));
		}		
	}
	else
	{//放炮
		int iOpType = OutCardEstimate(ChairID, bCard, major, m_HuFanNums[ChairID]);
		if (iOpType & TYPE_HU)
		{
			m_oFan[ChairID] = major.m_oFan;
			Score[ChairID] += m_HuFanNums[ChairID] * base_score;		
			Score[m_CurSeat] -= m_HuFanNums[ChairID] * base_score;

			m_stHuScore[ChairID].stHu.Add(tagHuRecord(m_CurSeat, bCard, HU_PAOHU, m_HuFanNums[ChairID] ));
		}		
	}
	
	return 0;

}
void Table::UserGetFirstCard(_uint8 ChairID, int& iOpType, MJ_fan_buffer& oFan)
{
	if (iOpType & TYPE_HU)
	{
		if (ChairID == m_CurSeat)
		{
			oFan.Add(MJ_FAN_TYPE_TIANHU);
		}		
	}	
}
void Table::UserFirstOutCard(_uint8 ChairID, int& iOpType, MJ_fan_buffer& oFan)
{
	if (iOpType & TYPE_HU && ChairID != m_BankerSeat)
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
void Table::OperateError(_uint8 ChairID, int code)
{
	proto::game::AckOperErrUc p_opr_err;
	p_opr_err.set_code(code);
	Ppacket ppack;
	p_opr_err.SerializeToString(&ppack.body);
	ppack.pack(SERVER_OPER_ERROR_UC);
	unicast(m_Seats[ChairID].m_Player, ppack.data);
	log.debug("%s:AckOperErrUc:%s .\n", __FUNCTION__, p_opr_err.DebugString().c_str());
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


//-------------------------------------客户端通讯------------------------------
void Table::Proto_GameEndInfo_GangInfo(_uint8 ChairID, proto::game::AckGameEnd& stGameend)
{
	proto::game::tagEndGangInfo* pEndGanginfo = stGameend.add_endganginfo();

	for (int i = 0; i < m_stGang[ChairID].Gang.Size(); i++)
	{
		const tagGangScore& t = m_stGang[ChairID].Gang[i];

		proto::game::tagEndGang *pGang = pEndGanginfo->add_gang();
		pGang->set_gangtype(t.iType);
		pGang->set_gangcard(t.Card);

	}
	for (int i = 0; i < m_GamePlayer; i++)//添加点杠信息
	{
		for (int j = 0; j < m_stGang[i].Gang.Size(); j++)
		{
			const tagGangScore& t = m_stGang[i].Gang[j];
			if (t.iType == TYPE_ZHIGANG && t.ChairID == ChairID)
			{
				proto::game::tagEndGang* pGang = pEndGanginfo->add_gang();
				pGang->set_gangtype(64);//点杠
				pGang->set_gangcard(t.Card);

			}
		}
	}
	pEndGanginfo->set_totalgangscore(m_stGang[ChairID].iTotalScore);
}

void Table::Proto_Game_OpNotify(_uint8 ChairID, _uint8 CurChairID, _uint8 bCard, proto::game::AckOpNotify& stNotify)
{
	stNotify.Clear();
	stNotify.set_chairid(ChairID);
	//非提示玩家,下面的协议字段为空
	if (CurChairID == ChairID)
	{
		stNotify.set_optype(m_Seats[ChairID].m_bUserOperate & (~TYPE_LISTEN));   //不发送listen操作
		stNotify.set_card(bCard);		
	}
}

void Table::Proto_Game_OpNotifyAfterPengChi(_uint8 ChairID, _uint8 CurChairID, const MJ_opinfo& gang, proto::game::AckOpNotify& stNotify)
{
	stNotify.Clear();
	stNotify.set_chairid(ChairID);
	//非提示玩家,下面的协议字段为空
	if (CurChairID == ChairID)
	{
		stNotify.set_optype(m_Seats[ChairID].m_bUserOperate & (~TYPE_LISTEN));  //不发送listen操作
		stNotify.set_card(0);
		for (int i = 0; i < gang.Size(); i++)
		{
			proto::game::tagGangInfo* ganginfo = stNotify.add_gang();
			ganginfo->set_card(gang[i].cbCard);
			ganginfo->set_state(gang[i].nState);
		}		
	}
}
void Table::Proto_Game_UserGetCard(_uint8 ChairID, _uint8 MoChairID, _uint8 bCard, proto::game::AckUserGetCard& stUsergetcard)
{
	stUsergetcard.Clear();
	stUsergetcard.set_chairid(MoChairID);
	stUsergetcard.set_leftcardnum(m_RepertoryCard.Count());

	//如果是摸牌用户，则打包摸牌、听牌等数据，非摸牌用户则不发送这些数据
	if (ChairID == MoChairID)
	{
		stUsergetcard.set_card(bCard);
		
		if (m_Seats[MoChairID].m_bUserOperate & TYPE_ANGANG || m_Seats[MoChairID].m_bUserOperate & TYPE_WANGANG)
		{
			MJ_opinfo stGang;			
			GetGangInfo(MoChairID, stGang);

			for (int i = 0; i < stGang.Size(); i++)
			{
				//log.info("玩家还没胡牌,此时暗杠牌有:0x%02x \n", stGang[i].cbCard);
				log.info("%s uid:%d chairID:%d angang:0x%02x\n", __FUNCTION__, GetPlayerUid(ChairID), ChairID, stGang[i].cbCard);
				proto::game::tagGangInfo *pstGang = stUsergetcard.add_ganginfo();
				pstGang->set_card(stGang[i].cbCard);
				pstGang->set_state(stGang[i].nState);
			}			
		}
		//听牌判断 TODO
		tagListenInfos stListenInfo[14];
		_uint8 bListenLen = 0;		
		//胡牌不用判断听牌，自动胡牌不用判断听牌
		if (!(m_Seats[ChairID].m_bUserOperate & TYPE_HU))
		{
			m_Seats[ChairID].m_bUserOperate |= GetListeningInfo(ChairID, stListenInfo, bListenLen);
		}
		stUsergetcard.set_optype(m_Seats[MoChairID].m_bUserOperate & (~TYPE_LISTEN));   //不发送listen操作
		//听牌消息统一发送到AckListenNotify
//		if (m_Seats[MoChairID].m_bUserOperate & TYPE_LISTEN)
//		{
//			//打包听牌信息
//			for (int j = 0; j < bListenLen; j++)
//			{
//				PackageTingInfo(ChairID, stListenInfo, j, stUsergetcard.add_listeninfo());
//			}
//		}
	}
}

void Table::Proto_GameEndInfo(const MJ_user_buffer& UserID, proto::game::AckGameEnd& stGameend)
{
	return ;
}
void Table::Proto_GameScene_lookon(_uint8 ChairID, Player* player, proto::game::AckGameScene& stGameScene)
{/*
	Table::Proto_GameScene_lookon(ChairID, player, stGameScene);
	AckCustomiedInfo* info = stGameScene.mutable_hunanscene();
	if (info != NULL)
	{
		info->set_gametype(proto::game::hunanmj::HUNAN_ZHUANZHUAN);
		info->set_ishavepiao(0);
		info->set_jiangma(proto::game::hunanmj::JIANGMA);//???
		info->set_laizi(0);//??????
	}*/
	return ;
}

void Table::Proto_GameEndInfo_AddFan(const MJ_user_buffer& UserID, proto::game::AckGameEnd& stGameend)
{
	proto::game::FanInfo* info[GAME_PLAYER];
	for (_uint8 i = 0; i < m_GamePlayer; i++)
	{
		info[i] = stGameend.add_faninfo();
	}	

	proto::game::AddFanInfo* pfan = NULL;	
	if (UserID.Size() == 1)
	{//一个人胡
		_uint8 win_seat = UserID[0];

		if (m_stHuScore[win_seat].stHu[0].bType == HU_ZIMO)
		{//自摸胡
			info[win_seat]->set_chairid(win_seat);
			info[win_seat]->set_specialtype(HU_ZIMO);
			info[win_seat]->set_fannum(m_HuFanNums[win_seat]);
			info[win_seat]->set_hucard(m_stHuScore[win_seat].stHu[0].bHuCard);

			for (int i = 0; i < m_oFan[win_seat].Size(); i++)
			{
				pfan = info[win_seat]->add_addfan();
				pfan->set_addtype(m_oFan[win_seat][i]);
				pfan->set_addnum(m_ft2fn->GetFan(m_oFan[win_seat][i]));
			}
			for (_uint8 c = NextChair(win_seat); c != win_seat; c = NextChair(c))
			{
				info[c]->set_chairid(c);
				info[c]->set_specialtype(HU_BEIZIMO);
				info[c]->set_fannum(0);
				info[c]->set_hucard(0);
				//info[c]->set_fantype(0);				
			}
		}
		else if (m_stHuScore[win_seat].stHu[0].bType == HU_PAOHU)
		{//点炮 或者抢杠胡
			info[win_seat]->set_chairid(win_seat);
			info[win_seat]->set_specialtype(m_bRobHu ? HU_QIANG_GANG : HU_PAOHU);
			info[win_seat]->set_fannum(m_HuFanNums[win_seat]);
			info[win_seat]->set_hucard(m_stHuScore[win_seat].stHu[0].bHuCard);
			//info[win_seat]->set_fantype(m_HuFanType[win_seat]);

			for (int i = 0; i < m_oFan[win_seat].Size(); i++)
			{
				pfan = info[win_seat]->add_addfan();
				pfan->set_addtype(m_oFan[win_seat][i]);
				pfan->set_addnum(m_ft2fn->GetFan(m_oFan[win_seat][i]));
			}

			for (_uint8 c = NextChair(win_seat); c != win_seat; c = NextChair(c))
			{
				info[c]->set_chairid(c);
				info[c]->set_specialtype(HU_INVALID);
				info[c]->set_fannum(0);
				info[c]->set_hucard(0);
				//info[c]->set_fantype(0);

			}
			info[m_stHuScore[win_seat].stHu[0].bPasHuChairID]->set_specialtype(m_bRobHu ? HU_BEI_QIANG_GANG : HU_DIANPAO);
		}
	}
	else if (UserID.Size() > 1)
	{//一炮多响
		for (_uint8 i = 0; i < m_GamePlayer; i++)
		{
			if (UserID.Find(i))
			{

				info[i]->set_chairid(i);
				info[i]->set_specialtype(m_bRobHu ? HU_QIANG_GANG : HU_PAOHU);
				info[i]->set_fannum(m_HuFanNums[i]);
				info[i]->set_hucard(m_stHuScore[i].stHu[0].bHuCard);

				for (int j = 0; j < m_oFan[i].Size(); j++)
				{
					pfan = info[i]->add_addfan();
					pfan->set_addtype(m_oFan[i][j]);
					pfan->set_addnum(m_ft2fn->GetFan(m_oFan[i][j]));
				}
			}
			else
			{
				info[i]->set_chairid(i);
				info[i]->set_specialtype(HU_INVALID);
				info[i]->set_fannum(0);
				info[i]->set_hucard(0);
				//info[i]->set_fantype(0);
			}

		}
		_uint8 dianpaoID = m_stHuScore[UserID[0]].stHu[0].bPasHuChairID;
		info[dianpaoID]->set_specialtype(m_bRobHu ? HU_BEI_QIANG_GANG : HU_DIANPAO);
	}
	else
	{
		for (int i = 0; i < m_GamePlayer; i++)
		{
			info[i]->set_chairid(i);
			info[i]->set_specialtype(HU_INVALID);
			info[i]->set_fannum(0);
			info[i]->set_hucard(0);
		}
	}
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

void Table::Proto_Game_OperateGang(_uint8 ChairID, _uint8 bCard, int iOpType, int iGangScore[GAME_PLAYER], proto::game::AckBcOpResult& stOpresult)
{
	stOpresult.set_chairid(ChairID);
	stOpresult.set_outcardchairid(m_CurOutSeat);
	stOpresult.set_optype(iOpType);
	stOpresult.set_card(bCard);
	for (int i = 0; i < m_GamePlayer; i++)
	{
		stOpresult.add_score(iGangScore[i]);
		//记录杠的总分
		//m_stGang[i].nTotalScore += nGangScore[i];
		//log.info("chairid:%d , this gangscore:%d, totalscore:%d \n", i, iGangScore[i], m_stGang[i].iTotalScore);
	}

	if (m_bGangCalcImme)
	{
		Update_Score(iGangScore);
	}
}

void Table::Proto_Update_Gps(proto::game::UpdateGPSAck& stUpdateGPSAck)
{
	for (int i = 0; i < m_GamePlayer; ++i)
	{
	    if (m_Seats[i].m_Player)
		{
			proto::game::tagPlayers *pstPlayer = stUpdateGPSAck.add_players();
    		pstPlayer->set_seatid(m_Seats[i].m_Player->seatid);
    		pstPlayer->set_uid(m_Seats[i].m_Player->uid);
    		pstPlayer->set_name(m_Seats[i].m_Player->name);
    		pstPlayer->set_avatar(m_Seats[i].m_Player->avatar);
    		pstPlayer->set_sex(m_Seats[i].m_Player->sex);
    		pstPlayer->set_money(m_Seats[i].m_Player->money);

			pstPlayer->set_latitude(m_Seats[i].m_Player->latitude);
			pstPlayer->set_longitude(m_Seats[i].m_Player->longitude);
			if (m_Seats[i].m_Player->name != "")
			{
			    pstPlayer->set_city(m_Seats[i].m_Player->city.c_str());
			}
			pstPlayer->set_gps_state(m_Seats[i].m_Player->gps_state);
		}        
	}
}

void Table::Update_Score(int iScore[GAME_PLAYER])
{
	tagUserScore userscore[GAME_PLAYER];

	for (int i = 0; i < m_GamePlayer; i++)
	{
		if (m_Seats[i].m_Player != NULL)
		{
			userscore[i].uid = m_Seats[i].m_Player->uid;
			userscore[i].score = iScore[i];

			m_Seats[i].m_Player->update_info(iScore[i], 0, 0, 0);

            UpdateUserInfoToUser(m_Seats[i].m_Player);
			log.fatal("%s uid:%d chairid:%d score:%d\n", __FUNCTION__, m_Seats[i].m_Player->uid, i, iScore[i]);
		}
	}
	//update_user_info_to_datasvr_dz(userscore, 1102, SERVER_DZ_UPDATE_USER_INFO_LITTLE);
}

void Table::SendTingInfo(_uint8 ChairID)
{
	tagListenInfos stListenInfo[14];
	_uint8 bListenLen = 0;	
	//胡牌不用判断听牌，自动胡牌不用判断听牌
	if (GetListeningInfo(ChairID, stListenInfo, bListenLen))
	{
		m_Seats[ChairID].m_bUserOperate |= TYPE_LISTEN;
	}
	
	// 发送听牌协议
	proto::game::AckListenNotify stListen;
	if (m_Seats[ChairID].m_bUserOperate & TYPE_LISTEN)
	{
		log.info("%s uid:%d chairid:%d ListenLen:%d \n", __FUNCTION__, GetPlayerUid(ChairID), ChairID, bListenLen);
		stListen.set_optype(TYPE_LISTEN);
		//打包听牌信息
		for (int j = 0; j < bListenLen; j++)
		{
			PackageTingInfo(ChairID, stListenInfo, j, stListen.add_listeninfo());
		}
	}
	else
	{
		stListen.set_optype(TYPE_NULL);
		log.info("%s uid:%d chairid:%d no listen\n", __FUNCTION__, GetPlayerUid(ChairID), ChairID);
	}
	Ppacket ppack;
	stListen.SerializeToString(&ppack.body);
	ppack.pack(SERVER_LISTEN_NOTIFY);
	unicast(m_Seats[ChairID].m_Player, ppack.data);
	log.debug("%s AckListenNotify:%s .\n", __FUNCTION__, stListen.DebugString().c_str());
}
/*
void Table::SendTingInfo_Simple(_uint8 ChairID)
{
	tagListenInfos stListenInfo[14];
	_uint8 bListenLen = 0;
	//胡牌不用判断听牌，自动胡牌不用判断听牌
	if (IsListening(ChairID, stListenInfo, bListenLen))
	{
		m_Seats[ChairID].m_bUserOperate |= TYPE_LISTEN;
	}

	// 发送听牌协议
	proto::game::AckListenNotify stListen;
	if (m_Seats[ChairID].m_bUserOperate & TYPE_LISTEN)
	{
		log.info("%s uid:%d chairid:%d ListenLen:%d \n", __FUNCTION__, GetPlayerUid(ChairID), ChairID, bListenLen);
		stListen.set_optype(TYPE_LISTEN);

		PackageTingInfo(ChairID, stListenInfo, 0, stListen.add_listeninfo());
		
	}
	unicast(SERVER_LISTEN_NOTIFY, ChairID, stListen);
}*/
//----------------------------------玩家单多操作处理-------------------------------------
void Table::UserAfterOperate(_uint8 ChairID, int iOpType, _uint8 bCard)
{
	m_CurSeat = ChairID;
	log.info("%s uid:%d chairid:%d operate:0x%x card:0x%02x\n", __FUNCTION__, GetPlayerUid(ChairID), ChairID, iOpType, bCard);

	m_Seats[ChairID].m_bUserOperate = TYPE_NULL;
	MJ_opinfo gang;
	m_Seats[ChairID].m_bUserOperate = GetGangInfo(m_CurSeat, gang);
	
	if (gang.Size() == 0)
	{
		set_out_timer(m_CurSeat);
	}
	else
	{
		proto::game::AckOpNotify stNotify;		
		for (int i = 0; i < m_GamePlayer; i++)
		{
			Proto_Game_OpNotifyAfterPengChi(ChairID, i, gang, stNotify);
			Ppacket ppack;
			stNotify.SerializeToString(&ppack.body);
			ppack.pack(SERVER_OPERATE_NOTIFY);
			unicast(m_Seats[i].m_Player, ppack.data);
			//unicast(SERVER_OPERATE_NOTIFY, i, stNotify);
			log.debug("%s:AckOpNotify:%s .\n", __FUNCTION__, stNotify.DebugString().c_str());

			if (i == ChairID) save_operate_notify(stNotify);
		}
		//发送听牌信息
		SendTingInfo(ChairID);
		
		set_operator_timer(ChairID);
	}
}

void Table::DoMultiOperate(_uint8 ChairID, int iOpType, _uint8 bCard)
{
	m_bMultiDoneOpNum++;
	m_Seats[ChairID].m_bMultiUserDoneOp = iOpType;
	m_Seats[ChairID].m_bMultiUserDoneOpCard = bCard;
	log.info("%s uid:%d chaird:%d, curoptype:0x%x, optype:0x%x, now done OpNum:%d, totalOpnum:%d \n", __FUNCTION__,
		GetPlayerUid(ChairID), ChairID, m_Seats[ChairID].m_bUserOperate, iOpType, m_bMultiDoneOpNum, m_bMultiOpUserNum);
	if (iOpType == TYPE_HU)
	{
		m_bMulitFirstOperate = true;		
		m_bMulitHuOpeCount++;
		
		for (_uint8 c = NextChair(ChairID); c != ChairID; c = NextChair(c))
		{
			if (c == m_CurSeat)
				continue;
			if (m_Seats[c].m_bUserOperate & TYPE_HU)
				continue;
			if (m_Seats[c].m_bUserOperate == TYPE_NULL)
				continue;
			if (m_Seats[c].m_bMultiUserDoneOp == TYPE_NULL)
			{
				m_bMultiDoneOpNum++;
				m_Seats[c].m_bMultiUserDoneOp = TYPE_PASS;
				log.info("%s force user uid:%d pass seat:%d!\n", __FUNCTION__, GetPlayerUid(c), c);
				UserOperatePass(c, TYPE_PASS, bCard);
			}
		}
		if (m_conf.m_bYiPaoDuoXiang)
		{
			UserOperateHu(ChairID, m_CurCard);

			if (m_conf.m_bFourceHuMutilHu)
			{
				DoMultiForceHu(ChairID, m_CurCard);
			}
		}

	}
	else
	{
		if (iOpType == TYPE_PASS)
		{
			if (m_Seats[ChairID].m_bUserOperate & TYPE_HU)   //胡牌玩家选择了过牌
			{
				m_bMulitHuOpeCount++;
				log.info("%s user uid:%d pass seat:%d, optype:0x%x!\n", __FUNCTION__, GetPlayerUid(ChairID), ChairID,m_Seats[ChairID].m_bUserOperate);
				m_PassHuPlayer.Add(ChairID);
			}
			UserOperatePass(ChairID, iOpType, bCard);
		}
		else
		{
			if (iOpType != TYPE_NULL)
			{				
				if (
					(m_bRecordMultiOp.iOpType == TYPE_NULL && (iOpType & (TYPE_CHI | TYPE_PENG | TYPE_GANG | TYPE_HU))) ||
					((iOpType & TYPE_GANG) && (m_bRecordMultiOp.iOpType & (TYPE_CHI | TYPE_PENG))) ||
					((iOpType & TYPE_PENG) && (m_bRecordMultiOp.iOpType & (TYPE_CHI))) 					
					)					
				{
					m_bRecordMultiOp.iOpType = iOpType;
					m_bRecordMultiOp.ChairID = ChairID;
					m_bRecordMultiOp.bCard = bCard;
				}			
			}
		}
	}
	if (m_bMultiDoneOpNum >= m_bMultiOpUserNum)
	{
		if (m_bMulitFirstOperate)
		{			
			if (!m_conf.m_bYiPaoDuoXiang)
			{
				for (_uint8 c = NextChair(m_CurSeat); c != m_CurSeat;c=NextChair(c))
				{
					if (m_Seats[c].m_bMultiUserDoneOp == TYPE_HU)
					{
						UserOperateHu(c, m_CurCard);
						break;
					}					
				}
			}
			GameEnd();
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
				
				OnUserGetCard(NextChair(m_CurSeat));
			}
		}
		init_multi_data();
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
				for (_uint8 c = NextChair(m_CurSeat); c != m_CurSeat; c = NextChair(c))
				{
					if (m_Seats[c].m_bMultiUserDoneOp == TYPE_HU)
					{
						UserOperateHu(c, m_CurCard);
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
			for (_uint8 c = NextChair(m_CurSeat); c != m_CurSeat; c = NextChair(c))
			{
				if (m_Seats[c].m_bMultiUserDoneOp != TYPE_NULL)
					continue;
				int tmpOpType = m_Seats[c].m_bUserOperate;
				if (tmpOpType == TYPE_NULL)
					continue;
				if (
					(m_bRecordMultiOp.iOpType == TYPE_NULL && (tmpOpType & (TYPE_CHI | TYPE_PENG | TYPE_GANG | TYPE_HU))) ||
					(tmpOpType & TYPE_HU) ||		
					(tmpOpType & TYPE_GANG && (m_bRecordMultiOp.iOpType & (TYPE_CHI | TYPE_PENG | TYPE_GANG))) ||
					(tmpOpType & TYPE_PENG && (m_bRecordMultiOp.iOpType & (TYPE_CHI | TYPE_PENG))) 					
					)
				{
					bOver = false;
					break;
				}
			}
			if (bOver)   //优先级高的玩家操作后，立即停止多操作
			{
				for (int i = 0; i < m_GamePlayer; i++)  //其他低操作玩家服务器主动发过操作
				{
					if (i == m_bRecordMultiOp.ChairID)
					{
						continue;
					}
					if (m_Seats[i].m_bUserOperate != TYPE_NULL &&m_Seats[i].m_bMultiUserDoneOp == TYPE_NULL)
					{
						UserOperatePass(i, TYPE_PASS, bCard);
					}

				}
				MultiOperateOver(m_bRecordMultiOp.ChairID, m_bRecordMultiOp.iOpType, m_bRecordMultiOp.bCard);
				init_multi_data();
			}
		}
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
		if (c == m_CurSeat)
			continue;
		if (m_Seats[c].m_bUserOperate & TYPE_HU || m_PassHuPlayer.Find(c))
		{
			m_Seats[c].m_bMultiUserDoneOp = TYPE_HU;
			m_Seats[c].m_bMultiUserDoneOpCard = bCard;

			m_Seats[c].m_bUserOperate |= TYPE_HU;
			UserOperateHu(c, bCard);
			stop_operator_timer(c);
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
	for (_uint8 c = 0; c < m_GamePlayer; ++c)
	{
		m_Seats[c].m_bMultiUserDoneOp = 0;
		m_Seats[c].m_bMultiUserDoneOpCard = 0;
	}
	m_PassHuPlayer.Clear();
}

void Table::MultiOperateOver(_uint8 ChairID, int iOpType, _uint8 bCard)
{
	if (iOpType == TYPE_PASS)
	{
		m_bAddFanFlag[m_CurOutSeat] = false;
		return;
	}

	for (_uint8 c = NextChair(m_CurSeat); c != ChairID;c = NextChair(c))
	{
		if (m_Seats[c].m_bMultiUserDoneOp == TYPE_PASS)
			continue;
		if (MultiOpOr(iOpType, TYPE_GANG))		
		{
			ChairID = c;
			iOpType = m_Seats[c].m_bMultiUserDoneOp;
			bCard = m_Seats[c].m_bMultiUserDoneOpCard;
			break;
		}
		else if (MultiOpOr(iOpType, TYPE_PENG))
		{
			ChairID = c;
			iOpType = m_Seats[c].m_bMultiUserDoneOp;
			bCard = m_Seats[c].m_bMultiUserDoneOpCard;
			break;
		}
		else if (MultiOpOr(iOpType, TYPE_CHI))
		{
			ChairID = c;
			iOpType = m_Seats[c].m_bMultiUserDoneOp;
			bCard = m_Seats[c].m_bMultiUserDoneOpCard;
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
		if (m_conf.m_bQiangZhiGang && RobGangHu(ChairID, bCard, iOpType))
			return;
		UserOperateGang(ChairID, iOpType, bCard);

		OnUserGetCard(ChairID, true);
		break;	
	default:
		break;
	}
}


bool Table::RobGangHu(_uint8 ChairID, _uint8 CardData, int iOpType)
{  
	if (iOpType & TYPE_WANGANG && !m_conf.m_bQiangGangHu)
		return false;
	if (iOpType & TYPE_ZHIGANG && !m_conf.m_bQiangZhiGang)
		return false;
	if (iOpType & TYPE_ANGANG && !m_conf.m_bQiangAnGang)
		return false;	

	_uint8 cbNum = 0;
	for (int i = 0; i < m_GamePlayer; i++)
	{    
		if (i == ChairID)
		{
			continue;
		}		
		MJ_major major;
		m_bRobGangHuJudge = true;
		m_WangGangPlayerOpType = iOpType;
		m_Seats[i].m_bUserOperate = OutCardEstimate(i, CardData, major, m_HuFanNums[i]);		
		m_bRobGangHuJudge = false;
		m_WangGangPlayerOpType = TYPE_NULL;
		m_Seats[i].m_bUserOperate &= TYPE_HU;
		if (m_Seats[i].m_bUserOperate & TYPE_HU)
		{   
			const HandCards& hc = m_Seats[i].m_HandCard;
			MJ_opinfo opinfo;
			
			if (iOpType & TYPE_WANGANG && m_GameLogic.GetWanGang(&hc, opinfo, CardData))
			{
				for (int c = 0; c < m_GamePlayer; c++)
				{
					proto::game::AckOpNotify stNotify;
					Proto_Game_OpNotify(i, c, CardData, stNotify);
					Ppacket ppack;
					stNotify.SerializeToString(&ppack.body);
					ppack.pack(SERVER_OPERATE_NOTIFY);
					unicast(m_Seats[c].m_Player, ppack.data);
					if (i == c) save_operate_notify(stNotify);		
				}
				cbNum++;
			}
			else if (iOpType & TYPE_ZHIGANG && m_GameLogic.GetZhiGang(&hc, opinfo, CardData))
			{
				for (int c = 0; c < m_GamePlayer; c++)
				{
					proto::game::AckOpNotify stNotify;
					Proto_Game_OpNotify(i, c, CardData, stNotify);
					Ppacket ppack;
					stNotify.SerializeToString(&ppack.body);
					ppack.pack(SERVER_OPERATE_NOTIFY);
					unicast(m_Seats[c].m_Player, ppack.data);

					if (i == c) save_operate_notify(stNotify);	
				}
				cbNum++;
			}
			else if (iOpType & TYPE_ANGANG && m_GameLogic.GetAnGang(&hc, opinfo, CardData))
			{
				for (int c = 0; c < m_GamePlayer; c++)
				{
					proto::game::AckOpNotify stNotify;
					Proto_Game_OpNotify(i, c, CardData, stNotify);
					Ppacket ppack;
					stNotify.SerializeToString(&ppack.body);
					ppack.pack(SERVER_OPERATE_NOTIFY);
					unicast(m_Seats[c].m_Player, ppack.data);

					if (i == c) save_operate_notify(stNotify);
				}
				cbNum++;
			}					
		}
	}
	//开启操作时钟 TODO
	for (int i = 0; i < m_GamePlayer; i++)
	{
		if (i == ChairID)
		{
			continue;
		}
		log.info("RobGangHu uid:%d chaird:%d out card, so that chairid:%d have operate:0x%x\n", GetPlayerUid(ChairID),ChairID, i,m_Seats[i].m_bUserOperate);			
		if (m_Seats[i].m_bUserOperate & TYPE_HU)
		{
			set_operator_timer(i);
		}
	}
	if (cbNum > 0)
	{
		m_RobHuNum = cbNum;
		m_bRobWanGang = true;
		m_WangGangPlayer = ChairID;
		m_WangGangPlayerOpType = iOpType;
		m_RobWanGangCard = CardData;
		log.info("robot ganghu, and hu user nums:%d \n", m_RobHuNum);
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
		}
	}
	else if (OperateType == TYPE_PASS)
	{
		UserOperatePass(ChairID, OperateType, CardData);
		m_PassHuPlayer.Add(ChairID);
	}
	
	log.info("%s uid:%d chairid:%d OperateNum:%d RobHuNum:%d \n", __FUNCTION__, GetPlayerUid(ChairID), ChairID, m_RobOperateNum, m_RobHuNum);
	if (m_RobOperateNum == m_RobHuNum)  //所有胡玩家都已经操作完毕
	{
		if (m_bRobHu)   //如果有人胡牌
		{
			if (!m_conf.m_bYiPaoDuoXiang)
			{
				for (_uint8 c = NextChair(m_CurSeat); c != m_CurSeat; c = NextChair(c))
				{
					if (m_Seats[c].m_bUserOperate & TYPE_HU)
					{
						m_bIsHu[c] = true;
						UserOperateHu(c, m_RobWanGangCard);
						break;
					}
				}
			}
			if (m_WangGangPlayerOpType & (TYPE_ANGANG | TYPE_WANGANG))
				m_GameLogic.RemoveCard(&m_Seats[m_WangGangPlayer].m_HandCard, m_RobWanGangCard, 1);			
			GameEnd();
		}
		else
		{
			UserOperateGang(m_WangGangPlayer, m_WangGangPlayerOpType, m_RobWanGangCard);
			OnUserGetCard(m_WangGangPlayer, true);
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

void Table::OnAutoUserOutCard(_uint8 ChairID, _uint8 bCard)
{
   	    
}

void Table::UserAutoOperate(_uint8 ChairID, _uint8 bCard)
{
    
}


int Table::GetChi(_uint8 ChairID, _uint8 bCard, MJ_opinfo& ChiInfo)
{
	if (m_conf.m_bSupportChi)
	{
		return m_GameLogic.GetChi(&m_Seats[ChairID].m_HandCard, bCard, ChiInfo);
	}
	return TYPE_NULL;	
}
int Table::GetGangInfo(_uint8 ChairID, MJ_opinfo& GangCardInfo)
{
	return m_GameLogic.GetGangInfo(&m_Seats[ChairID].m_HandCard, GangCardInfo);
}
int Table::GetGangInfo(_uint8 ChairID, MJ_opinfo& GangCardInfo, MJ_gang& lou_gang, _uint8 laizi_card)
{
    return m_GameLogic.GetGangInfo(&m_Seats[ChairID].m_HandCard, GangCardInfo, lou_gang, laizi_card);
}
int Table::GetPengGang(_uint8 ChairID, _uint8 cbCard, MJ_opinfo& GangCardInfo)
{
	return m_GameLogic.GetPengGang(&m_Seats[ChairID].m_HandCard, cbCard, GangCardInfo);	
}
int Table::GetPengGang(_uint8 ChairID, _uint8 cbCard, MJ_opinfo& GangCardInfo, _uint8 laizi_card)
{
    return m_GameLogic.GetPengGang(&m_Seats[ChairID].m_HandCard, cbCard, GangCardInfo, laizi_card);	
}

bool Table::FormatCards(HandCards *pHc, MJ_win_pattern& pattern)
{
	return m_GameLogic.FormatCards(pHc, pattern.kan, pattern.jiang);
}


bool Table::IsListening(_uint8 ChairID, tagListenInfos stListenInfo[14], _uint8 &bListenLen)
{	
	HandCards tmp = m_Seats[ChairID].m_HandCard;
	memset(stListenInfo, 0, sizeof(tagListenInfos) * 14);
	bListenLen = 0;
	Check_Listening(&tmp, ChairID, 0, stListenInfo, bListenLen);

	return bListenLen != 0;
}

bool Table::IsListening(_uint8 ChairID, _uint8 bRemoveCard)
{	
	HandCards tempHandCard = m_Seats[ChairID].m_HandCard;
	if (bRemoveCard != INVALID_MJ_CARD)
	{
		m_GameLogic.RemoveCard(&tempHandCard, bRemoveCard);
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
		if (Check_Hu(ChairID, tempHandCard, fan) == TYPE_HU)
		{
			log.info("%s uid:%d chairdid:%d Remove:0x%02x\n", __FUNCTION__, GetPlayerUid(ChairID), ChairID, bRemoveCard);
			return true;
		}
		m_GameLogic.ChuPai(&tempHandCard, _relativity[i]);
	}
	return false;
}


int Table::GetListeningInfoNoOutCard(_uint8 ChairID, tagListenInfos stListenInfo[14], _uint8& bListenLen)
{
    HandCards tempHandCard = m_Seats[ChairID].m_HandCard;

    memset(stListenInfo, 0, sizeof(tagListenInfos) * 14);
	bListenLen = 0;
	Check_Listening(&tempHandCard, ChairID, INVALID_MJ_CARD, stListenInfo, bListenLen);

	return (bListenLen != 0 ? TYPE_LISTEN : TYPE_NULL);
}


//手牌为14张牌的时候的判断听牌
int Table::GetListeningInfo(_uint8 ChairID, tagListenInfos stListenInfo[14], _uint8& bListenLen)
{
	HandCards tempHandCard = m_Seats[ChairID].m_HandCard;

	_uint8 bCard = 0xFF;
	memset(stListenInfo, 0, sizeof(stListenInfo));
	bListenLen = 0;	
	
	for (int i=0; i<tempHandCard.ChangeableCardsLen; i++)
	{   
		//防止手牌有有对、刻字时，连续对相同的牌做多次的判断胡牌
		if (bCard == tempHandCard.ChangeableCards[i])
		{
			continue;
		} 
		bCard = tempHandCard.ChangeableCards[i];
		Check_Listening(&tempHandCard, ChairID, bCard, stListenInfo, bListenLen);
	}

	return (bListenLen != 0 ? TYPE_LISTEN : TYPE_NULL);
}

void Table::Check_Listening(HandCards *pHc, _uint8 ChairID, _uint8 bCard, tagListenInfos stListenInfo[14], _uint8 &bListenLen)
{
	HandCards tempHandCard = *pHc;

	m_GameLogic.ChuPai(&tempHandCard, bCard);
	bool bTrue = false;

	MJ_BUFFER<_uint8, POOL_CARD_TYPE_MAX> _relativity;
	GetAllHandCardRelativity(&tempHandCard, _relativity);
	//获取关联的所有牌

	for (_uint8 i = 0; i< _relativity.Size(); i++)
	{
		//将每张相关联的牌都测试一遍是否能听牌 不能就排除这张牌
		m_GameLogic.AddCard(&tempHandCard, _relativity[i]);
		MJ_fan_buffer fan_buffer;
		if (Check_Hu(ChairID, tempHandCard, fan_buffer) == TYPE_HU)
		{			
			if (stListenInfo[bListenLen].bHuLen <18)
			{				
				stListenInfo[bListenLen].bOutCard = bCard;//要打出的牌       
				stListenInfo[bListenLen].HuInfo[stListenInfo[bListenLen].bHuLen].bCard = _relativity[i];//听的牌
				stListenInfo[bListenLen].HuInfo[stListenInfo[bListenLen].bHuLen].bFanShu = Calc_Fan(ChairID, fan_buffer);
				bTrue = true;
				stListenInfo[bListenLen].bHuLen++;
			}	
			//log.info("Check_Listening chairid:%d OutCard:0x%02x, listenLen:%d hulen:%d \n", ChairID, bCard, bListenLen+ 1, stListenInfo[bListenLen].bHuLen);
		}
		m_GameLogic.ChuPai(&tempHandCard, _relativity[i]);
	}
	m_GameLogic.AddCard(&tempHandCard, bCard);
	if (bTrue)
	{ 	
		//log.info("%s uid:%d chairid:%d outcard:0x%02x hulen:%d\n", __FUNCTION__, GetPlayerUid(ChairID), ChairID, bCard, stListenInfo[bListenLen].bHuLen);
		if (bListenLen < 14)
		{
			bListenLen++;
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

int Table::removeAllGuiCard(HandCards *pHc, std::map<_uint8, _uint8>&guiCardCountMap, bool& isFourGui)
{
	int iLaiziNum = 0;
	for (int i = 0; i < m_GuiVector.Size(); i++)
	{
		_uint8 guiCard = m_GuiVector[i];
		while (m_GameLogic.ChuPai(pHc, guiCard))
		{
			iLaiziNum++;
			if (guiCardCountMap.find(guiCard) != guiCardCountMap.end())
			{
				guiCardCountMap[guiCard] += 1;
				if (guiCardCountMap[guiCard] >= 4)
					isFourGui = true;
			}
			else
			{
				guiCardCountMap[guiCard] = 1;
			}
		}
	}	
	//唯一赖子，不使用m_GuiVector时，简化
	while (m_GameLogic.ChuPai(pHc, m_LaiziCard))
	{
		iLaiziNum++;
		if (guiCardCountMap.find(m_LaiziCard) != guiCardCountMap.end())
		{
			guiCardCountMap[m_LaiziCard] += 1;
			if (guiCardCountMap[m_LaiziCard] >= 4)
				isFourGui = true;
		}
		else
		{
			guiCardCountMap[m_LaiziCard] = 1;
		}
	}
	
	return iLaiziNum;
}

void Table::addGuiCard(HandCards *pHc, std::map<_uint8, _uint8>&guiCardCountMap)
{
	for (std::map<_uint8, _uint8>::iterator itor = guiCardCountMap.begin(); itor != guiCardCountMap.end(); itor++)
	{
		for (int i = 0; i < itor->second; i++)
		{
			m_GameLogic.AddCard(pHc, itor->first);
		}
	}
}

void Table::Check_Listening_LaiZi(HandCards *pHc, _uint8 ChairID, _uint8 bCard, tagListenInfos stListenInfo[14], _uint8 &bListenLen)
{
	HandCards tempHandCard = *pHc;

	m_GameLogic.ChuPai(&tempHandCard, bCard);
	bool bTrue = false;

	_uint8 last_check = INVALID_MJ_CARD;
	MJ_BUFFER<_uint8, POOL_CARD_TYPE_MAX> _relativity;

	//
	bool isFourGui = false;
	std::map<_uint8, _uint8> guiCardCountMap;
	int iLaiziNum = removeAllGuiCard(&tempHandCard, guiCardCountMap, isFourGui);
	for (int i = 0; i < tempHandCard.ChangeableCardsLen; ++i)
	{
		if (last_check == tempHandCard.ChangeableCards[i])
		{
			continue;
		}
		last_check = tempHandCard.ChangeableCards[i];
		if (iLaiziNum == 0)
			GetHandCardRelativity(last_check, _relativity);
		else
		{
			GetHandCardRelativity_LaiZi(last_check, _relativity);
			break;
		}
	}
	if (0 == tempHandCard.ChangeableCardsLen && iLaiziNum > 0)
	{
        GetHandCardRelativity_LaiZi(last_check, _relativity);
	}
	
	//
	addGuiCard(&tempHandCard, guiCardCountMap);

	for (_uint8 i = 0; i< _relativity.Size(); i++)
	{
		m_GameLogic.AddCard(&tempHandCard, _relativity[i]);
		//听牌判断，临时改变当前牌为_relativity[i]
		_uint8 bCurCard = m_CurCard;
		m_CurCard = _relativity[i];
		
		MJ_fan_buffer fan_buffer;
		if (Check_Hu(ChairID, tempHandCard, fan_buffer) == TYPE_HU)
		{
			if (stListenInfo[bListenLen].bHuLen <9)
			{
				stListenInfo[bListenLen].bOutCard = bCard;//
				stListenInfo[bListenLen].HuInfo[stListenInfo[bListenLen].bHuLen].bCard = _relativity[i];//
				stListenInfo[bListenLen].HuInfo[stListenInfo[bListenLen].bHuLen].bFanShu = Calc_Fan(ChairID, fan_buffer);
				bTrue = true;
				stListenInfo[bListenLen].bHuLen++;
			}
		}
		m_GameLogic.ChuPai(&tempHandCard, _relativity[i]);
		//还原当前牌
		m_CurCard = bCurCard;
	}
	if (bTrue)
	{
		//log.info("%s uid:%d chairid:%d outcard:0x%02x hulen:%d\n", __FUNCTION__, GetPlayerUid(ChairID), ChairID, bCard, stListenInfo[bListenLen].bHuLen);
		if (bListenLen < 14)
		{
			bListenLen++;
		}
	}
}

void Table::GetHandCardRelativity_LaiZi(_uint8 bCard, MJ_BUFFER<_uint8, POOL_CARD_TYPE_MAX>& _relativity)
{
	_uint8 g_Card_Wan[] =
    {
    	0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09,//万	
    };
    _uint8 g_Card_Tiao[] =
    {
    	0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19,//条	
    };
    _uint8 g_Card_Tong[] =
    {
    	0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29,//筒	
    };
    _uint8 g_Card_Zi[] =
    {
    	0x31, 0x32, 0x33, 0x34, 0x41, 0x42, 0x43,
    };
    
    for (int i = 0; i < 9; ++i)
    {
        _relativity.Add(g_Card_Wan[i]);
        _relativity.Add(g_Card_Tiao[i]);
        _relativity.Add(g_Card_Tong[i]);
    }

    for (int i = 0; i < 7; ++i)
    {
        _relativity.Add(g_Card_Zi[i]);
    }
}


/*
void Table::SendTingInfo(_uint8 ChairID)
{
	tagListenInfos stListenInfo[14];
	_uint8 bListenLen = 0;
	//胡牌不用判断听牌，自动胡牌不用判断听牌
	if (GetListeningInfo(ChairID, stListenInfo, bListenLen))
	{
		m_Seats[ChairID].m_bUserOperate |= TYPE_LISTEN;
	}
	
	// 发送听牌协议
	proto::game::AckListenNotify stListen;
	if (m_Seats[ChairID].m_bUserOperate & TYPE_LISTEN)
	{
		log.info("%s uid:%d chairid:%d ListenLen:%d \n", __FUNCTION__, GetPlayerUid(ChairID), ChairID, bListenLen);
		stListen.set_optype(TYPE_LISTEN);
		//打包听牌信息
		for (int j = 0; j < bListenLen; j++)
		{
			PackageTingInfo(ChairID, stListenInfo, j, stListen.add_listeninfo());
		}
	}
	else
	{
		stListen.set_optype(TYPE_NULL);
		log.info("%s uid:%d chairid:%d no listen\n", __FUNCTION__, GetPlayerUid(ChairID), ChairID);
	}
	Ppacket ppack;
	stListen.SerializeToString(&ppack.body);
	ppack.pack(SERVER_LISTEN_NOTIFY);
	unicast(m_Seats[ChairID].m_Player, ppack.data);
	
}*/

void Table::SetRobGangHu_Flag(_uint8 ChairID, int iOpType, proto::game::AckBcOpResult& stOpresult)
{
	if (Is_HuOperator(iOpType) && m_bRobWanGang && m_bAddFanFlag[m_WangGangPlayer])
	{
		stOpresult.set_isqiangganghu(1);
	}
}

bool Table::CardPoolIsEmpty()
{
	return m_RepertoryCard.IsEmpty();
}



