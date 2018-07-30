#include "table_pingxiangmj.h"
#include "../phz.h"
#include "../libcommon/log.h"


extern PHZ phz;
extern Log log;

Table_pingxiangmj::Table_pingxiangmj()
{

}

Table_pingxiangmj::~Table_pingxiangmj()
{

}

void Table_pingxiangmj::InitConf(const Json::Value &val, const std::string &roomid)
{
	Table::InitConf(val, roomid);

	m_RepertoryCard.Set(CARD_POOL_NO_FLOWER);

	m_conf.m_bSupportChi = true;
	m_conf.m_bZimoHu = false;
	m_conf.m_bYiPaoDuoXiang = true;
	m_conf.m_bQiangGangHu = true;
	m_conf.m_bGangShangPao = false;	
	m_conf.m_bHuangGang = true;
	m_conf.m_bForceHuQiangGangHu = false;
	m_conf.m_bFourceHuMutilHu = false;	
	m_bGangCalcImme = true;
	
	m_conf.m_bHu7dui = true;
	m_conf.m_bHu13yao = true;
	m_conf.m_AnGangPoint = 2;
	m_conf.m_ZhiGangPoint = 3;
	m_conf.m_WanGangPoint = 1;
	base_score = 1; 

	if (json_check_play_type_id(val))
	{
		MJ_SUB_ID SubID;
		//添加玩法id，录像使用
		json_get_play_type_id(val, 1000, SubID);
		if (SubID.Find(1000001))
		{
			m_conf_pxmj.iGameMode = PLAY_MODE_PINGXIANGZZ;
		}
		else if (SubID.Find(1000002))
		{
			m_conf_pxmj.iGameMode = PLAY_MODE_PINGXIANGHZ;
		}
		else if (SubID.Find(1000003))
		{
			m_conf_pxmj.iGameMode = PLAY_MODE_PINGXIANG258;
		}
		m_room_config += SubID;
        
		json_get_play_type_id(val, 1001, SubID);
		if (SubID.Find(1001001))
		{
			m_GamePlayer = GAME_PLAYER;
		}
		else if (SubID.Find(1001002))
		{
			m_GamePlayer = (GAME_PLAYER - 1);
		}
		m_room_config += SubID;

        json_get_play_type_id(val, 1002, SubID);
		if(SubID.Find(1002001)){
			m_bPiao = true;
			//可飘分数初始化
			for (int i = 0; i <= 10; ++i)
			{
			    m_PiaoScore.insert(i);
			}
		}
		if(SubID.Find(1002002))
		{
		    m_conf_pxmj.bAutoHu = true;
		}
		if(SubID.Find(1002003))
		{
		    m_conf.m_bSupportZhuaniao = true;			
		}
		if(SubID.Find(1002004))
		{
		    m_conf.m_bSupportChi = false;
		}
		if(SubID.Find(1002005))
		{
		    m_conf.m_bZimoHu = true;	
		}
		if(SubID.Find(1002006))
		{
		    m_conf_pxmj.bLouhuCanHu = true;	
		}
		if(SubID.Find(1002007))
		{
		    m_conf_pxmj.bBankerScore = true;	
		}
		m_room_config += SubID;
		
		json_get_play_type_id(val, 1003, SubID);
		if (SubID.Find(1003001))
		{
		    m_conf.m_bSupportZhuaniao = true;	//抓鸟
			m_conf.m_bMojiJiangji = true;
			m_conf.m_ZhuaniaoNum = 2;
		}
		else if (SubID.Find(1003002))
		{
		    m_conf.m_bSupportZhuaniao = true;	//抓鸟
			m_conf.m_bMojiJiangji = true;
			m_conf.m_ZhuaniaoNum = 4;
		}
		else if (SubID.Find(1003003))
		{
		    m_conf.m_bSupportZhuaniao = true;	//抓鸟
			m_conf.m_bMojiJiangji = true;
			m_conf.m_ZhuaniaoNum = 6;
		}
		else if (SubID.Find(1003004))
		{
		    m_conf.m_bSupportZhuaniao = true;	//抓鸟
			m_conf.m_bMojiJiangji = true;
			m_conf.m_ZhuaniaoNum = 8;
		}		
		m_room_config += SubID;

		json_get_play_type_id(val, 1004, SubID);
		if (SubID.Find(1004001))
		{
			m_conf_pxmj.bNoneNiaoAsAll = true;
		}
		if (SubID.Find(1004002))
		{
			m_conf_pxmj.bAllNiaoDb = true;
		}
		m_room_config += SubID;

		json_get_play_type_id(val, 1005, SubID);
		if (SubID.Find(1005001))
		{
			m_conf_pxmj.iNiaoScore = 1;
		}
		else if (SubID.Find(1005002))
		{
			m_conf_pxmj.iNiaoScore = 2;
		}
		else if (SubID.Find(1005003))
		{
			m_conf_pxmj.iNiaoScore = ZHONG_NIAO_DOUBLE;
		}		
		m_room_config += SubID; 

		json_get_play_type_id(val, 1006, SubID);
		if (SubID.Find(1006001))
		{
			m_conf_pxmj.iHongZhongNum = 4;
		}
		else if (SubID.Find(1006002))
		{
			m_conf_pxmj.iHongZhongNum = 8;
		}	
		m_room_config += SubID;

		json_get_play_type_id(val, 1007, SubID);
		if (SubID.Find(1007001))
		{
			m_conf_pxmj.bHasFeng = true;
		}
		else if (SubID.Find(1007002))
		{
			m_conf_pxmj.bHasFeng = false;
		}	
		m_room_config += SubID;
		
		json_get_play_type_id(val, 1008, SubID);
		if (SubID.Find(1008001))
		{
			m_conf_pxmj.bQingYiSeChi = true;
		}
		m_room_config += SubID;
	}

	if (m_conf_pxmj.bHasFeng)
    {
        m_RepertoryCard.Set(CARD_POOL_NO_FLOWER);	
    }
    else if (m_conf_pxmj.iHongZhongNum > 0)
    {
        if (4 == m_conf_pxmj.iHongZhongNum)
        {
            m_RepertoryCard.Set(CARD_POOL_WTTHONGZHONG);	//万筒条 + 4红中
        }
        else
        {
            m_RepertoryCard.Set(CARD_POOL_WTT_HONGZHONG_8);	//万筒条 + 8红中
        }
    }
    else
    {
        m_RepertoryCard.Set(CARD_POOL_NO_WIND_DRAGON_FLOWER);	//万筒条
    }

	m_ft2fn = &m_pingxiangmj_Fantype_to_FanNum;

	log.debug("%s tid:%s m_GamePlayer:%d, m_bYiPaoDuoXiang:%d, m_bZimoHu:%d, m_bSupportZhuaniao:%d, m_ZhuaniaoNum:%d, m_bSupportPiao:%d, m_TotalTurn:%d, bAutoHu:%d, m_bSupportChi:%d, bNoneNiaoAsAll:%d, bAllNiaoDb:%d, iNiaoScore:%d, bQingYiSeChi:%d \n", 
	        __FUNCTION__, tid.c_str(), m_GamePlayer, m_conf.m_bYiPaoDuoXiang, m_conf.m_bZimoHu, m_conf.m_bSupportZhuaniao, 
	        m_conf.m_ZhuaniaoNum, m_conf.m_bSupportPiao, m_TotalTurn, m_conf_pxmj.bAutoHu, m_conf.m_bSupportChi, 
	        m_conf_pxmj.bNoneNiaoAsAll, m_conf_pxmj.bAllNiaoDb, m_conf_pxmj.iNiaoScore, m_conf_pxmj.bQingYiSeChi);
}

void Table_pingxiangmj::RecoverConfig()
{
	if (m_room_config.Find(1000001))
	{
		m_conf_pxmj.iGameMode = PLAY_MODE_PINGXIANGZZ;
	}
	else if (m_room_config.Find(1000002))
	{
		m_conf_pxmj.iGameMode = PLAY_MODE_PINGXIANGHZ;
	}
	else if (m_room_config.Find(1000003))
	{
		m_conf_pxmj.iGameMode = PLAY_MODE_PINGXIANG258;
	}    

	if(m_room_config.Find(1002002))
	{
	    m_conf_pxmj.bAutoHu = true;
	}
	if(m_room_config.Find(1002003))
	{
	    m_conf.m_bSupportZhuaniao = true;			
	}
	if(m_room_config.Find(1002004))
	{
	    m_conf.m_bSupportChi = true;
	}
	if(m_room_config.Find(1002005))
	{
	    m_conf.m_bZimoHu = true;	
	}
	if(m_room_config.Find(1002006))
	{
	    m_conf_pxmj.bLouhuCanHu = true;	
	}
	if(m_room_config.Find(1002007))
	{
	    m_conf_pxmj.bBankerScore = true;	
	}

	if (m_room_config.Find(1003001))
	{
	    m_conf.m_bSupportZhuaniao = true;	//抓鸟
		m_conf.m_bMojiJiangji = true;
		m_conf.m_ZhuaniaoNum = 2;
	}
	else if (m_room_config.Find(1003002))
	{
	    m_conf.m_bSupportZhuaniao = true;	//抓鸟
		m_conf.m_bMojiJiangji = true;
		m_conf.m_ZhuaniaoNum = 4;
	}
	else if (m_room_config.Find(1003003))
	{
	    m_conf.m_bSupportZhuaniao = true;	//抓鸟
		m_conf.m_bMojiJiangji = true;
		m_conf.m_ZhuaniaoNum = 6;
	}
	else if (m_room_config.Find(1003004))
	{
	    m_conf.m_bSupportZhuaniao = true;	//抓鸟
		m_conf.m_bMojiJiangji = true;
		m_conf.m_ZhuaniaoNum = 8;
	}		

	if (m_room_config.Find(1004001))
	{
		m_conf_pxmj.bNoneNiaoAsAll = true;
	}
	if (m_room_config.Find(1004002))
	{
		m_conf_pxmj.bAllNiaoDb = true;
	}

	if (m_room_config.Find(1005001))
	{
		m_conf_pxmj.iNiaoScore = 1;
	}
	else if (m_room_config.Find(1005002))
	{
		m_conf_pxmj.iNiaoScore = 2;
	}
	else if (m_room_config.Find(1005003))
	{
		m_conf_pxmj.iNiaoScore = ZHONG_NIAO_DOUBLE;
	}		

	if (m_room_config.Find(1006001))
	{
		m_conf_pxmj.iHongZhongNum = 4;
	}
	else if (m_room_config.Find(1006002))
	{
		m_conf_pxmj.iHongZhongNum = 8;
	}	

	if (m_room_config.Find(1007001))
	{
		m_conf_pxmj.bHasFeng = true;
	}
	else if (m_room_config.Find(1007002))
	{
		m_conf_pxmj.bHasFeng = false;
	}	
	if (m_room_config.Find(1008001))
	{
		m_conf_pxmj.bQingYiSeChi = true;
	}
	
    m_ft2fn = &m_pingxiangmj_Fantype_to_FanNum;
}

void Table_pingxiangmj::SaveProtoTable()
{
    Table::SaveProtoTable();

    for (_uint8 i = 0; i < m_GamePlayer; ++i){
        proto::game::tagLouGangInfo *pLouGangInfo = m_pTableInfo.add_m_lou_gang();
        for (_uint8 j = 0; j < m_lou_gang[i].Size(); ++j){
            pLouGangInfo->add_card(m_lou_gang[i][j]);
        }        
    }

    for (_uint8 i = 0; i < m_GamePlayer; ++i){
        m_pTableInfo.add_m_lou_hu(m_bLouhu[i]);
    }

    for (_uint8 i = 0; i < m_GamePlayer; ++i){
        m_pTableInfo.add_m_chicard(m_bChiCard[i]);
    }
}

void Table_pingxiangmj::ParseProtoTable()
{
    Table::ParseProtoTable();

    for (_uint8 i = 0; i < m_pTableInfo.m_lou_gang_size(); ++i){
        m_lou_gang[i].Clear();
		const proto::game::tagLouGangInfo& stLouGangInfo = m_pTableInfo.m_lou_gang(i);
		for (_uint8 j = 0; j < stLouGangInfo.card_size(); ++j){
		    m_lou_gang[i].Add(stLouGangInfo.card(j));
		}		
	}

	for (_uint8 i = 0; i < m_pTableInfo.m_lou_hu_size(); ++i){
        m_bLouhu[i] = m_pTableInfo.m_lou_hu(i);			
	}

	for (_uint8 i = 0; i < m_pTableInfo.m_chicard_size(); ++i){
        m_bChiCard[i] = m_pTableInfo.m_chicard(i);			
	}
}

void Table_pingxiangmj::Reset()
{
    Table::Reset();

    memset(m_lou_gang, 0, sizeof(m_lou_gang));	
	memset(m_bLouhu, false, sizeof(m_bLouhu));	
		 
    for(_uint8 i = 0; i < GAME_PLAYER; ++i){
		m_bChiCard[i] = INVALID_MJ_CARD;
	}
}

void Table_pingxiangmj::save_video_notify(proto::game::AckOpNotify &stNotify)
{
    std::string str = stNotify.SerializeAsString();
	save_video_data(SERVER_OPERATE_NOTIFY, str);
}

void Table_pingxiangmj::save_user_get_card(proto::game::AckUserGetCard &stUsergetcard)
{
    std::string str = stUsergetcard.SerializeAsString();
	save_video_data(SERVER_USER_GETCARD, str);
}

void Table_pingxiangmj::save_video_game_end(proto::game::pingxiangmj::AckGameEndPingXiang& stGameEndPingXiang)
{
    std::string str = stGameEndPingXiang.SerializeAsString();
	save_video_data(SERVER_GAME_END_BC, str);
}

int Table_pingxiangmj::game_start_res(const Jpacket &packet)
{
	int ret = Table::game_start_res(packet);
	if(ret == 0){
		if(m_bPiao){
			StartPiao();
		}
		else{		    
			GameStart();
		}
	}
	return ret;
}

void Table_pingxiangmj::GameStart()
{
	Table::GameStart();		
}

//发送赖子信息
void Table_pingxiangmj::SendLaiziCard()
{    
    if (PLAY_MODE_PINGXIANGHZ == m_conf_pxmj.iGameMode)
    {
        m_LaiziCard = MJ_HONG_ZHONG;
    }
    else
    {
        m_LaiziCard = INVALID_MJ_CARD;
    }

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

bool Table_pingxiangmj::CheckFixedCardsType(_uint8 ChairID, _uint8 bCard)
{
    _uint8 cardType = CardType(bCard);    
    for (int i = 0; i < m_Seats[ChairID].m_HandCard.FixedCardsLen; ++i)
    {
        if (CardType(m_Seats[ChairID].m_HandCard.FixedCards[i].CardData) != cardType)
        {
            return false;
        }
    }
    return true;
}


bool Table_pingxiangmj::CheckUserOperator(_uint8 ChairID, int iOpType, _uint8 bCard)
{
    if (m_bMultiOpUserNum > 1)
    {
        if (m_Seats[ChairID].m_bMultiUserDoneOp != 0)
        {
            log.error("%s: player already operated, ChairID:%d, m_Seats[ChairID].m_bMultiUserDoneOp:%d, iOpType:%d \n", 
                        __FUNCTION__, ChairID, m_Seats[ChairID].m_bMultiUserDoneOp, iOpType);
            return false;                        
        }
    }

	return Table::CheckUserOperator(ChairID, iOpType, bCard);
}

void Table_pingxiangmj::UserOperatePass(_uint8 ChairID, int iOpType, _uint8 bCard)
{
	log.info("%s uid:%d ChairID:%d, m_Seats[ChairID].m_bUserOperate:0x%x, iOpType:0x%x, bCard:0x%02x \n", 
	        __FUNCTION__, GetPlayerUid(ChairID), ChairID, m_Seats[ChairID].m_bUserOperate, iOpType, bCard);
	BroadcastOperateResult(ChairID, iOpType, bCard);

    //漏胡
    if (m_Seats[ChairID].m_bUserOperate & TYPE_HU)
    {
        m_bLouhu[ChairID] = true;
        //防止重连之后又可以点胡
        m_Seats[ChairID].m_bUserOperate &= ~(TYPE_HU);
    }
    
    //弯杠选择pass，算漏杠
	if (m_Seats[ChairID].m_bUserOperate & TYPE_WANGANG && !m_lou_gang[ChairID].Find(bCard))
	{
	    m_lou_gang[ChairID].Add(bCard);
	}
	
	if (ChairID != m_CurSeat)
	{
		m_bAddFanFlag[ChairID] = false;
		m_Seats[ChairID].m_bUserOperate = TYPE_NULL;
	} 
}

void Table_pingxiangmj::UserOperateChi(_uint8 ChairID, int iOpType, _uint8 bCard)
{
    //能胡的牌，吃后算漏胡
	if (m_Seats[ChairID].m_bUserOperate & TYPE_HU)
	{
        m_bLouhu[ChairID] = true; 
	}
	
	memset(m_bAddFanFlag, 0, sizeof(m_bAddFanFlag));
	for (int i = 0; i < m_GamePlayer; i++)
    {
        m_Seats[i].m_bUserOperate = NULL;
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
	log.info("%s uid:%d ChairID:%d, bCard:0x%02x \n", __FUNCTION__, GetPlayerUid(ChairID), ChairID, bCard);

	SendTingInfo(ChairID);

	//记录清一色吃牌
	if (PLAY_MODE_PINGXIANG258 == m_conf_pxmj.iGameMode && m_conf_pxmj.bQingYiSeChi && INVALID_MJ_CARD == m_bChiCard[ChairID])
	{
        m_bChiCard[ChairID] = bCard;
	}

	int score[GAME_PLAYER] = { 0 };
	SaveOperateVideo(iOpType, ChairID, m_CurOutSeat, bCard, score);

	UserAfterOperate(ChairID, iOpType, bCard);	
}


void Table_pingxiangmj::UserOperateGang(_uint8 ChairID, int iOpType, _uint8 bCard)
{
    //能胡的牌，杠后算漏胡
	if (m_Seats[ChairID].m_bUserOperate & TYPE_HU)
	{
        m_bLouhu[ChairID] = true; 
	}
	
	memset(m_bAddFanFlag, 0, sizeof(m_bAddFanFlag));
    for (int i = 0; i < m_GamePlayer; i++)
    {
        m_Seats[i].m_bUserOperate = NULL;
    }

	log.info("%s uid:%d ChairID:%d, iOpType:0x%x, bCard:0x%02x \n", __FUNCTION__, GetPlayerUid(ChairID), ChairID, iOpType, bCard);
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

    Ppacket ppack;
	stOpresult.SerializeToString(&ppack.body);
	ppack.pack(SERVER_OPERATE_RESULT);
	broadcast(NULL, ppack.data);
	
	log.debug("%s:AckBcOpResult:%s .\n", __FUNCTION__, stOpresult.DebugString().c_str());

	m_Seats[ChairID].m_bUserOperate = TYPE_NULL;

	save_video_data(SERVER_OPERATE_RESULT, stOpresult.SerializeAsString());
}


bool Table_pingxiangmj::CheckCardGang(_uint8 ChairID, _uint8 bCard)
{    
    for (int i = 0; i < m_Seats[ChairID].m_HandCard.FixedCardsLen; i++)
	{
		if (m_Seats[ChairID].m_HandCard.FixedCards[i].state == TYPE_PENG && m_Seats[ChairID].m_HandCard.FixedCards[i].CardData == bCard)
		{
			for (int j = 0; j < m_Seats[ChairID].m_HandCard.ChangeableCardsLen; j++)
			{
				if (m_Seats[ChairID].m_HandCard.FixedCards[i].CardData == m_Seats[ChairID].m_HandCard.ChangeableCards[j])
				{					
					return true;
				}
			}
		}
	}
	return false;
}

void Table_pingxiangmj::UserOperatePeng(_uint8 ChairID, _uint8 bCard)
{	
	log.info("%s uid:%d ChairID:%d, bCard:0x%02x \n", __FUNCTION__, GetPlayerUid(ChairID), ChairID, bCard);
	//能胡的牌，碰后算漏胡
	if (m_Seats[ChairID].m_bUserOperate & TYPE_HU)
	{
        m_bLouhu[ChairID] = true; 
	}
	
	memset(m_bAddFanFlag, 0, sizeof(m_bAddFanFlag));	
    for (int i = 0; i < m_GamePlayer; i++)
    {
        m_Seats[i].m_bUserOperate = NULL;
    }

	BroadcastOperateResult(ChairID, TYPE_PENG, bCard);

	int fixedlen = m_Seats[ChairID].m_HandCard.FixedCardsLen;
	//if (fixedlen >= 4)  //17张牌可以碰5下
	if (fixedlen >= HAND_CARD_KAN_MAX)
	{
		log.error("玩家组合牌长度:%d大于正常的长度 \n", fixedlen);
		return;
	}
	m_GameLogic.ExecutePeng(&m_Seats[ChairID].m_HandCard, m_CurOutSeat, bCard);
	m_Seats[ChairID].m_bGetCardCount++;

	//从打出牌删掉这张牌	
	m_Seats[m_CurOutSeat].m_bOutRecord.RemoveLast(bCard);

	//m_bOutCardCount[CardType(bCard)][CardNum(bCard)] += 2;


	//TODO 检查是否听牌
	SendTingInfo(ChairID);

	if (CheckCardGang(ChairID, bCard) && !m_lou_gang[ChairID].Find(bCard))
    {
	    m_lou_gang[ChairID].Add(bCard);
	}
	
	UserAfterOperate(ChairID, TYPE_PENG, bCard);

	int score[GAME_PLAYER] = { 0 };
	SaveOperateVideo(TYPE_PENG, ChairID, m_CurOutSeat, bCard, score);
}


void Table_pingxiangmj::UserAfterOperate(_uint8 ChairID, int iOpType, _uint8 bCard)
{
	m_CurSeat = ChairID;
	log.info("%s uid:%d chairid:%d operate:0x%x card:0x%02x m_lou_gang.size:%d \n", __FUNCTION__, 
	            GetPlayerUid(ChairID), ChairID, iOpType, bCard, m_lou_gang[m_CurSeat].Size());

	m_Seats[ChairID].m_bUserOperate = TYPE_NULL;
	MJ_opinfo gang;	
	//赖子不能吃碰杠
	m_Seats[ChairID].m_bUserOperate = GetGangInfo(m_CurSeat, gang, m_lou_gang[m_CurSeat], m_LaiziCard);

	//清一色吃牌检测
    m_Seats[ChairID].m_bUserOperate &= ~(TYPE_GANG);
	if (PLAY_MODE_PINGXIANG258 == m_conf_pxmj.iGameMode && m_conf_pxmj.bQingYiSeChi && m_bChiCard[ChairID] != INVALID_MJ_CARD)
    {
        for (int i = 0; i < gang.Size();)
        {
            if (!CheckFixedCardsType(ChairID, gang[i].cbCard))
            {
                gang.Remove(gang[i]);
                continue;
            }
            m_Seats[ChairID].m_bUserOperate |= gang[i].nState;
            ++i;
        }        
    }
	
	//  if (gang.Size() == 0)
    if (TYPE_NULL == m_Seats[ChairID].m_bUserOperate)
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
    		log.debug("%s:AckOpNotify:%s .\n", __FUNCTION__, stNotify.DebugString().c_str());
    		
			if (i == ChairID)
			{
			    save_video_notify(stNotify);
			}
		}
		//发送听牌信息
		SendTingInfo(ChairID);
		
		set_operator_timer(ChairID);
	}	
}

int Table_pingxiangmj::GetPengGang(_uint8 ChairID, _uint8 cbCard, MJ_opinfo& GangCardInfo, _uint8 laizi_card)
{
    if (PLAY_MODE_PINGXIANG258 == m_conf_pxmj.iGameMode && m_conf_pxmj.bQingYiSeChi && m_bChiCard[ChairID] != INVALID_MJ_CARD)
    {
        if (!CheckFixedCardsType(ChairID, cbCard))
        {
            return TYPE_NULL;
        }
    }
    return m_GameLogic.GetPengGang(&m_Seats[ChairID].m_HandCard, cbCard, GangCardInfo, laizi_card);	
}
int Table_pingxiangmj::GetChi(_uint8 ChairID, _uint8 bCard, MJ_opinfo& ChiInfo)
{
	if (m_conf.m_bSupportChi)
	{
		return m_GameLogic.GetChi(&m_Seats[ChairID].m_HandCard, bCard, ChiInfo);
	}
	else if (PLAY_MODE_PINGXIANG258 == m_conf_pxmj.iGameMode && m_conf_pxmj.bQingYiSeChi)
	{
        if (CheckFixedCardsType(ChairID, bCard))
        {
            return m_GameLogic.GetChi(&m_Seats[ChairID].m_HandCard, bCard, ChiInfo);
        }
	}
	return TYPE_NULL;	
}

bool Table_pingxiangmj::RobGangHu(_uint8 ChairID, _uint8 CardData, int iOpType)
{
	if (iOpType & TYPE_WANGANG && !m_conf.m_bQiangGangHu)
		return false;
	if (iOpType & TYPE_ZHIGANG && !m_conf.m_bQiangZhiGang)
		return false;
	if (iOpType & TYPE_ANGANG && !m_conf.m_bQiangAnGang)
		return false;

    bool bDidAutoHu = false;
	_uint8 cbNum = 0;
	for (int i = 0; i < m_GamePlayer; i++)
	{
		if (i == ChairID || i == m_CurSeat)
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
		    if (m_conf_pxmj.bAutoHu)
		    {
		        m_RobHuNum = cbNum;
        		m_bRobWanGang = true;
        		m_WangGangPlayer = ChairID;
        		m_WangGangPlayerOpType = iOpType;
        		m_RobWanGangCard = CardData;

        		bDidAutoHu = true;
            	cbNum++;
            	m_bRobHu = true;
            	m_bAddFanFlag[ChairID] = true; 
        		
                log.info("%s qiang gang quto hu, uid:%d chairid:%d Operate:0x%x \n", __FUNCTION__, GetPlayerUid(i), i, m_Seats[i].m_bUserOperate);
            	UserOperateHu(i, CardData);            	
		    }
		    else
		    {
    			const HandCards& hc = m_Seats[ChairID].m_HandCard;
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
                		log.debug("%s:AckOpNotify:%s .\n", __FUNCTION__, stNotify.DebugString().c_str());
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
                		log.debug("%s:AckOpNotify:%s .\n", __FUNCTION__, stNotify.DebugString().c_str());
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
                		log.debug("%s:AckOpNotify:%s .\n", __FUNCTION__, stNotify.DebugString().c_str());
    				}
    				cbNum++;
    			}
    		}
		}
	}

	if (m_conf_pxmj.bAutoHu && bDidAutoHu)
    {
        if (iOpType & (TYPE_ANGANG | TYPE_WANGANG))
        {
		    m_GameLogic.RemoveCard(&m_Seats[ChairID].m_HandCard, CardData, 1);
		}
		
    	GameEnd();

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
    else
    {
    	//开启操作时钟 TODO
    	for (int i = 0; i < m_GamePlayer; i++)
    	{
    		if (i == ChairID)
    		{
    			continue;
    		}
    		log.info("RobGangHu uid:%d chaird:%d out card, so that chairid:%d have operate:0x%x\n", GetPlayerUid(ChairID),ChairID, i, m_Seats[i].m_bUserOperate);
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
    		//log.info("robot ganghu, and hu user nums:%d \n", m_RobHuNum);
    	}  
    }

	return (cbNum > 0);
}


int Table_pingxiangmj::GetCardEstimate(_uint8 ChairID, _uint8 bCard, MJ_major& major, int& fan_num)
{
	assert(major.m_oFan.Size() == 0);

	int iOpType = TYPE_NULL;

	HandCards* pHc = &m_Seats[ChairID].m_HandCard;	
	m_GameLogic.AddCard(pHc, bCard);

	//assert(pHc->ChangeableCardsLen % 3 == 2);
	if (pHc->ChangeableCardsLen % 3 != 2)
	{
		log.error("%s fatal paishu error uid:%d chairid:%d!\n", __FUNCTION__, GetPlayerUid(ChairID), ChairID);
		m_GameLogic.ChuPai(pHc, bCard);
		return TYPE_NULL;
	}
	assert(pHc->ChangeableCardsLen <= HAND_CARD_SIZE_MAX);

	//赖子不能吃碰杠
	iOpType |= GetGangInfo(ChairID, major.m_GetGangInfo, m_lou_gang[ChairID], m_LaiziCard);
	

    //漏胡不能再胡
    if (!m_bLouhu[ChairID])
    {
	    iOpType |= Check_Hu(ChairID, *pHc, major.m_oFan);
	}
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

	//清一色吃检测    
	if (PLAY_MODE_PINGXIANG258 == m_conf_pxmj.iGameMode && m_conf_pxmj.bQingYiSeChi && m_bChiCard[ChairID] != INVALID_MJ_CARD)
    {
        iOpType &= (~TYPE_GANG);
        for (int i = 0; i < major.m_GetGangInfo.Size();)
        {
            if (!CheckFixedCardsType(ChairID, major.m_GetGangInfo[i].cbCard))
            {
                major.m_GetGangInfo.Remove(major.m_GetGangInfo[i]);
                continue;
            }
            iOpType |= major.m_GetGangInfo[i].nState;
            ++i;
        }        
    }
	return iOpType;
}


void Table_pingxiangmj::OnUserGetCard(_uint8 ChairID, bool bGetLast)
{
	if (is_dissolved)
	{
		return;
	}

	if (CardPoolIsEmpty())
	{
		GameEnd();
		return;
	}

    //摸牌，漏胡作废
    if (m_conf_pxmj.bLouhuCanHu)
    {
        m_bLouhu[ChairID] = false;
    }
	
	for (int i = 0; i < m_GamePlayer; i++)
	{
	    m_Seats[i].m_bUserOperate = NULL;
	}
	m_CurSeat = ChairID;

	_uint8 bGetCard = bGetLast? m_RepertoryCard.GetLast() : m_RepertoryCard.GetOneCard();

	m_Seats[ChairID].m_bGetCardCount++;

	m_CurCard = bGetCard;
	MJ_major major;
	//摸牌判断
	m_Seats[ChairID].m_bUserOperate = GetCardEstimate(ChairID, bGetCard,  major, m_HuFanNums[ChairID]);
	m_OperateRecord[ChairID] = major;

	m_GameLogic.AddCard(&m_Seats[ChairID].m_HandCard, bGetCard);

	log.info("%s tid:%s uid:%d chairid:%d getcard:0x%02x optype:0x%x cardpoollen:%d \n", __FUNCTION__, tid.c_str(), GetPlayerUid(ChairID), ChairID, bGetCard, m_Seats[ChairID].m_bUserOperate, m_RepertoryCard.Count());

	//广播玩家摸牌
	proto::game::AckUserGetCard stUsergetcard;
	for (int i = 0; i < m_GamePlayer; i++)
	{
		if (m_Seats[i].m_Player == NULL)
		{
			log.error("OnUserGetCard chairid:%d  error, one player is NULL\n", ChairID);
			continue;
		}

		Proto_Game_UserGetCard(i, ChairID, bGetCard, stUsergetcard);
		stUsergetcard.set_isgetlast(bGetLast ? 1 : 0);
		
		Ppacket ppack;
    	stUsergetcard.SerializeToString(&ppack.body);
    	ppack.pack(SERVER_USER_GETCARD);
    	unicast(m_Seats[i].m_Player, ppack.data);
    	
		log.debug("AckUserGetCard:%s\n", stUsergetcard.DebugString().c_str());

        //摸牌后，发送听牌信息
		if (i == ChairID)
		{
            SendTingInfo(ChairID);
            save_user_get_card(stUsergetcard);
		}
	}

	//见牌胡
	if (m_conf_pxmj.bAutoHu)
	{
        if (m_Seats[ChairID].m_bUserOperate & TYPE_HU)
        {
            UserOperateHu(ChairID);

			GameEnd();    
			return;
        }
	}

	if (m_Seats[ChairID].m_bUserOperate == TYPE_NULL || m_Seats[ChairID].m_bUserOperate == TYPE_LISTEN)
	{
	    if (CardPoolIsEmpty())
	    {
	        GameEnd();
		    return;
	    }
	    else
	    {
	        set_out_timer(ChairID);
	    }		
	}
	else
	{
		set_operator_timer(ChairID);
	}

	SerializeTableData();
}

void Table_pingxiangmj::Proto_Game_UserGetCard(_uint8 ChairID, _uint8 MoChairID, _uint8 bCard, proto::game::AckUserGetCard& stUsergetcard)
{
	//_uint8 tempChairid = m_SeatPlayer[ChairID].player->seatid;
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
			//赖子不能吃碰杠	
			GetGangInfo(MoChairID, stGang, m_lou_gang[MoChairID], m_LaiziCard);

			//清一色吃检测            
        	if (PLAY_MODE_PINGXIANG258 == m_conf_pxmj.iGameMode && m_conf_pxmj.bQingYiSeChi && m_bChiCard[ChairID] != INVALID_MJ_CARD)
            {
                m_Seats[ChairID].m_bUserOperate &= (~TYPE_GANG);
                for (int i = 0; i < stGang.Size();)
                {
                    if (!CheckFixedCardsType(ChairID, stGang[i].cbCard))
                    {
                        stGang.Remove(stGang[i]);
                        continue;
                    }
                    m_Seats[ChairID].m_bUserOperate |= stGang[i].nState;
                    ++i;
                }        
            }

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
		stUsergetcard.set_optype(m_Seats[MoChairID].m_bUserOperate);
		//听牌消息统一发送到AckListenNotify
//		if (m_Seats[MoChairID].m_bUserOperate & TYPE_LISTEN)
//		{
//			//打包听牌信息
//			for (int j = 0; j < bListenLen; j++)
//			{
//				PackageTingInfo(ChairID, stListenInfo, j, stUsergetcard.add_listeninfo());

//			}
//		}
		log.debug("%s AckUserGetCard:%s .\n", __FUNCTION__, stUsergetcard.DebugString().c_str());
	}
}

//用户打牌后的判断
bool Table_pingxiangmj::EstimateUserRespond(_uint8 bOutChairID, _uint8 bOutCard)
{
	m_bMultiOpUserNum = 0;
	m_bMultiOpHuNum = 0;
	bool bIsChangeUser = true;//是否轮到下一玩家摸牌
	bool bDidAutoHu = false;    //是否有自动胡操作

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

		if (m_conf_pxmj.bAutoHu && (m_Seats[i].m_bUserOperate & TYPE_HU))
		{		    
            log.info("%s dian pao auto hu, tid:%s uid:%d chairid:%d Operate:0x%x \n", __FUNCTION__, tid.c_str(), GetPlayerUid(i), i, m_Seats[i].m_bUserOperate);
            UserOperateHu(i, bOutCard);
            bDidAutoHu = true;
		}
        else
        {
    		if (m_Seats[i].m_bUserOperate != TYPE_NULL)
    		{
    			if (m_Seats[i].m_bUserOperate & TYPE_HU)
    			{
    				//m_bAddFanFlag[i] = true;
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

    				if (j == i)
    				{
    				    save_video_notify(stNotify);
    				}
    			}
    		}
        }
		if (m_Seats[i].m_bUserOperate != 0)
		{
			log.info("%s tid:%s uid:%d chairid:%d Operate:0x%x, totalop:%d \n", __FUNCTION__, tid.c_str(), GetPlayerUid(i), i, m_Seats[i].m_bUserOperate, m_bMultiOpUserNum);
		}
	}

	if (m_conf_pxmj.bAutoHu && bDidAutoHu)
	{
	    GameEnd();
	}
    else
    {
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
    }

	return bIsChangeUser;
}

int Table_pingxiangmj::OutCardEstimate(_uint8 ChairID, _uint8 bCard, MJ_major& major, int& fan_num)
{
	assert(major.m_oFan.Size() == 0);

	int iOpType = TYPE_NULL;

	//赖子不能吃碰杠胡(任何操作)
	if (bCard == m_LaiziCard)
	{
        return iOpType;
	}

	HandCards* pHc = &m_Seats[ChairID].m_HandCard;

    if (ChairID == NextChair(m_CurSeat))//只能吃上家的牌
    {
	    iOpType |= GetChi(ChairID, bCard, major.m_Chi);
    }
    
    //赖子不能吃碰杠
	iOpType |= GetPengGang(ChairID, bCard, major.m_GetPengGang, m_LaiziCard);	

	assert(pHc->ChangeableCardsLen < HAND_CARD_SIZE_MAX);

    //可接炮，未漏胡
	if (!m_conf.m_bZimoHu && !m_bLouhu[ChairID])
	{
		m_GameLogic.AddCard(pHc, bCard);

		iOpType |= Check_Hu(ChairID, *pHc, major.m_oFan);
		if (CardPoolIsEmpty())
		{
			UserLastOutCard(ChairID, iOpType, major.m_oFan);
		}		
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

int Table_pingxiangmj::Calc_Fan(_uint8 ChairID, MJ_fan_buffer& oFan)
{
	int iTotalFan = 1;
	if (PLAY_MODE_PINGXIANG258 == m_conf_pxmj.iGameMode)
	{
    	for (int i = 0; i < oFan.Size(); i++)
    	{
    		if (2 == m_ft2fn->GetFan(oFan[i]))
    		{
                iTotalFan = 2;
                break;
    		}
    	}
    }
	return iTotalFan;
}

int Table_pingxiangmj::Calc_Hu(_uint8 ChairID, _uint8 bCard, int Score[GAME_PLAYER])
{
	assert(Score[0] == 0);
	assert(Score[1] == 0);
	assert(Score[2] == 0);
	assert(Score[3] == 0);

	m_oFan[ChairID].Clear();	
	m_HuFanNums[ChairID] = 0;

	int bankerScore[GAME_PLAYER] = {0};
	if (m_conf_pxmj.bBankerScore)
	{
    	for (int i = 0; i < m_GamePlayer; ++i)
    	{
            bankerScore[i] = (i == m_BankerSeat) ? 1 : 0;
    	}
    }

	MJ_major major;
	if (m_bRobHu)
	{
		int iOpType = OutCardEstimate(ChairID, bCard, major, m_HuFanNums[ChairID]);
		if (iOpType & TYPE_HU)
		{
			m_oFan[ChairID] = major.m_oFan;
			Score[ChairID] += (m_HuFanNums[ChairID] * base_score) + bankerScore[ChairID] + bankerScore[m_WangGangPlayer];
			Score[m_WangGangPlayer] -= (m_HuFanNums[ChairID] * base_score) + bankerScore[ChairID] + bankerScore[m_WangGangPlayer];

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
				Score[ChairID] += (m_HuFanNums[ChairID] * base_score * 2) + bankerScore[ChairID] + bankerScore[c];
				Score[c] -= (m_HuFanNums[ChairID] * base_score * 2) + bankerScore[ChairID] + bankerScore[c];
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
			Score[ChairID] += (m_HuFanNums[ChairID] * base_score) + bankerScore[ChairID] + bankerScore[m_CurSeat];			
			Score[m_CurSeat] -= (m_HuFanNums[ChairID] * base_score) + bankerScore[ChairID] + bankerScore[m_CurSeat];

			m_stHuScore[ChairID].stHu.Add(tagHuRecord(m_CurSeat, bCard, HU_PAOHU, m_HuFanNums[ChairID] ));
		}		
	}

	log.debug("%s: ChairID:%d, base_score:%d, m_HuFanNums[0]:%d, m_HuFanNums[1]:%d, m_HuFanNums[2]:%d, m_HuFanNums[3]:%d, bankerScore[0]:%d, bankerScore[1]:%d, bankerScore[2]:%d, bankerScore[3]:%d, Score[0]:%d, Score[1]:%d, Score[2]:%d, Score[3]:%d \n",
            __FUNCTION__, ChairID, base_score, m_HuFanNums[0], m_HuFanNums[1], m_HuFanNums[2], m_HuFanNums[3], 
            bankerScore[0], bankerScore[1], bankerScore[2], bankerScore[3], Score[0], Score[1], Score[2], Score[3]);
            
	return 0;

}


void Table_pingxiangmj::Score_GameEndSucc(HuScoreSet& score, proto::game::pingxiangmj::AckGameEndPingXiang& stGameEndPingXiang)
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

	//抓鸟
	//一炮多响时，只摸一次鸟，所有人按同一套摸鸟牌算分
	bool bMoNiao = false;
	int iNiaoScore = 0;
	for (int i = 0; i < m_GamePlayer; i++)
	{
		Calc_ZhongNiao(i, INVALID_MJ_CARD, iBaseScore[i], iScore[i], stGameEndPingXiang, bMoNiao, iNiaoScore);
	}
    //飘分
    for (int i = 0; i < m_GamePlayer; i++)
	{		
		Calc_Piao(i, iScore[i]);		
	}
	
	for (int i = 0; i < m_GamePlayer; i++)
	{
		Calc_Score(iTotalScore, iScore[i]);
	}

	for (int i = 0; i < m_GamePlayer; i++)
	{
//		score[i].iHuScore = m_stHuScore[i].iTotalScore;
//		iTotalScore[i] += m_stHuScore[i].iTotalScore;
//		score[i].iTotleScore = iTotalScore[i];
//		log.debug("%s iTotalScore:%d .\n", __FUNCTION__, iTotalScore[i]);
		
        score[i].iHuScore = m_stHuScore[i].iTotalScore;
        iTotalScore[i] += m_stHuScore[i].iTotalScore;

        score[i].iGangScore = m_stGang[i].iTotalScore;
        iTotalScore[i] += m_stGang[i].iTotalScore;

        score[i].iTotleScore = iTotalScore[i];
        log.debug("%s score[i].iHuScore:%d, iGangScore:%d, iTotalScore:%d .\n", __FUNCTION__, score[i].iHuScore, score[i].iGangScore, iTotalScore[i]);
	}	
}

void Table_pingxiangmj::Calc_BaseScore(_uint8 ChairID, int iBaseScore[GAME_PLAYER])
{

}

void Table_pingxiangmj::Calc_HuScore(_uint8 ChairID, int iBaseScore[GAME_PLAYER], int iScore[GAME_PLAYER])
{

}

void Table_pingxiangmj::Calc_GangScore(int iGangScore[GAME_PLAYER])
{
    for (int i = 0; i < m_GamePlayer; i++)
    {
        iGangScore[i] = m_stGang[i].iTotalScore;
    }

	log.info("%s tid:%s iGangScore:[%d,%d,%d,%d] .\n",
		__FUNCTION__, tid.c_str(), iGangScore[0], iGangScore[1], iGangScore[2], iGangScore[3]);
}

void Table_pingxiangmj::Calc_ZhongNiao(_uint8 ChairID, _uint8 ShaiNum, int iBaseScore[GAME_PLAYER], int iScore[GAME_PLAYER], 
                                            proto::game::pingxiangmj::AckGameEndPingXiang& stGameEndPingXiang,
                                            bool& bMoNiao, int& iNiaoScoreStore)
{
	if (m_bIsHu[ChairID] && m_conf.m_bSupportZhuaniao)
	{
	    if (!bMoNiao)
	    {
    	    proto::game::pingxiangmj::tagJiangma* pZhuaNiao = stGameEndPingXiang.add_zhuaniao_info();
    	    pZhuaNiao->set_chairid(ChairID);
    	    pZhuaNiao->set_cardtype(m_conf.m_bCalcAllNiao ? 1 : 0);
    	    
    	    int iNiaoNum = 0;
            if (m_conf.m_bCalcAllNiao)
    	    {
    //	        if (m_RepertoryCard.IsEmpty())
    //	        {
    //                return;
    //	        }
    //	        _uint8 bGetCard = m_RepertoryCard.GetOneCard();
    //	        if (MJ_HONG_ZHONG == bGetCard)
    //	        {
    //                iNiaoNum = NIAO_NUM_HONG_ZHONG;
    //	        }
    //	        else if (1 == CardNum(bGetCard))
    //	        {
    //                iNiaoNum = m_conf_pxmj.iOneNiaoNum;
    //	        }
    //	        else
    //	        {
    //                iNiaoNum = CardNum(bGetCard);
    //	        }

    //	        pZhuaNiao->add_card(bGetCard);
    //	        pZhuaNiao->add_zhongchairid(ChairID);
    //            log.info("%s bGetCard:%d iNiaoNum:%d \n", __FUNCTION__,bGetCard,iNiaoNum);
    	    }
    	    else 
    	    {
                MJ_BUFFER<int, 3> zhongNiao_List;
                zhongNiao_List.Clear();

                zhongNiao_List.Add(1);
                zhongNiao_List.Add(5);
                zhongNiao_List.Add(9);            
    	    
    	        int iMoNiaoNum = 0;
    	        iMoNiaoNum = m_RepertoryCard.Count() < m_conf.m_ZhuaniaoNum ? m_RepertoryCard.Count() : m_conf.m_ZhuaniaoNum;

    	        _uint8 *pMoNiao = new _uint8[iMoNiaoNum];
    	        m_RepertoryCard.GetCard(pMoNiao, iMoNiaoNum);
    	        for (int i = 0; i < iMoNiaoNum; i++)
    	        {
    	            pZhuaNiao->add_card(pMoNiao[i]);
                    if (MJ_HONG_ZHONG == pMoNiao[i] || (CardType(pMoNiao[i]) < MJ_TYPE_FENG && zhongNiao_List.Find(CardNum(pMoNiao[i]))))
                    {
                        iNiaoNum++;
                        pZhuaNiao->add_zhongchairid(ChairID);
                    }        
                    else
                    {
                        pZhuaNiao->add_zhongchairid(INVALID_CHAIRID);
                    }
    	        }
                //不中等于全中
    	        if (m_conf_pxmj.bNoneNiaoAsAll && 0 == iNiaoNum)
    	        {
    	            iNiaoNum = m_conf.m_ZhuaniaoNum;
    	        }
    	        log.info("%s iMoNiaoNum:%d iNiaoNum:%d .\n", __FUNCTION__,iMoNiaoNum,iNiaoNum);
    	        delete []pMoNiao;
    	        pMoNiao = NULL;
    	    }
    	    pZhuaNiao->set_num(iNiaoNum);
    	    //计算鸟分
    	    int iNiaoScore = 0;
    	    if (ZHONG_NIAO_DOUBLE == m_conf_pxmj.iNiaoScore)
    	    {
    	        iNiaoScore = pow(iNiaoNum);
    	    }
            else
            {
                iNiaoScore = iNiaoNum * m_conf_pxmj.iNiaoScore;
            }
        	//全中分数翻倍
            if (m_conf_pxmj.bAllNiaoDb && iNiaoNum == m_conf.m_ZhuaniaoNum)
            {
                iNiaoScore *= 2;
            }

            //储存摸鸟分
            iNiaoScoreStore = iNiaoScore;
            bMoNiao = true;
        }
	    
        //结算摸鸟分
        if (m_bRobHu)
	    {
	        iScore[ChairID] += iNiaoScoreStore * base_score;
        	iScore[m_WangGangPlayer] -= iNiaoScoreStore * base_score;
	    }
	    else if (ChairID == m_CurSeat)
	    {
	        for (_uint8 c = NextChair(ChairID); c != ChairID; c = NextChair(c))
        	{
        	    iScore[ChairID] += iNiaoScoreStore * base_score;
        		iScore[c] -= iNiaoScoreStore * base_score;
        	}
	    }
	    else
	    {
	        iScore[ChairID] += iNiaoScoreStore * base_score;
        	iScore[m_CurSeat] -= iNiaoScoreStore * base_score;
	    }
    	log.info("%s tid:%s ChairID:%d uid:%d, iNiaoScoreStore:%d, iScore[]:%d,%d,%d,%d .\n",
		__FUNCTION__, tid.c_str(), ChairID, GetPlayerUid(ChairID), iNiaoScoreStore, iScore[0], iScore[1], iScore[2], iScore[3]);
	}
}

void Table_pingxiangmj::Calc_Piao(_uint8 ChairID, int iScore[GAME_PLAYER])
{
	if (m_bIsHu[ChairID] && m_bPiao)
	{
		if (m_stHuScore[ChairID].stHu[0].bType == HU_ZIMO)
		{
			for (_uint8 c = NextChair(ChairID); c != ChairID; c = NextChair(c))
			{
				iScore[ChairID] += m_Seats[ChairID].m_UserPiaoScore + m_Seats[c].m_UserPiaoScore;
				iScore[c] -= m_Seats[ChairID].m_UserPiaoScore + m_Seats[c].m_UserPiaoScore;
			}
		}
		else
		{
			_uint8 iPaoId = m_stHuScore[ChairID].stHu[0].bPasHuChairID;
			iScore[ChairID] += m_Seats[ChairID].m_UserPiaoScore + m_Seats[iPaoId].m_UserPiaoScore;
			iScore[iPaoId] -= m_Seats[ChairID].m_UserPiaoScore + m_Seats[iPaoId].m_UserPiaoScore;
		}
	}

	log.info("%s tid:%s ChairID:%d m_bIsHu:%d piao:[%d,%d,%d,%d] score:[%d,%d,%d,%d] .\n ",
	__FUNCTION__, tid.c_str(), ChairID, m_bIsHu[ChairID], m_Seats[0].m_UserPiaoScore, m_Seats[1].m_UserPiaoScore, 
	m_Seats[2].m_UserPiaoScore, m_Seats[3].m_UserPiaoScore, iScore[0], iScore[1], iScore[2], iScore[3]);
}

bool Table_pingxiangmj::FormatCards(HandCards *pHc, MJ_win_pattern& pattern)
{
	bool isFourGui = false;
	std::map<_uint8, _uint8> guiCardCountMap;
	int iLaiziNum = removeAllGuiCard(pHc, guiCardCountMap, isFourGui);
    
	bool r = false;
	if (iLaiziNum == 0)
	{
		r = m_GameLogic.FormatCards(pHc, pattern.kan, pattern.jiang);
	}
	else
	{
		r = m_GameLogic.FormatCards_Laizi(pHc->ChangeableCards, pHc->ChangeableCardsLen, iLaiziNum);
	}

	addGuiCard(pHc, guiCardCountMap);

	return r;
}

void Table_pingxiangmj::TurnBanker()
{
	MJ_user_buffer UserID;
	_uint8 bDraw = (MultiHu(UserID) == 0) ? 1 : 0;//?áê?×′ì?￡?1±íê?á÷??￡?0±íê?oú???áê?
	if (bDraw)
	{
		m_BankerSeat = m_CurOutSeat;
	}
	else
	{
		if (UserID.Size() == 1)
		{
			m_BankerSeat = UserID[0];
		}
		else
		{
			m_BankerSeat = m_stHuScore[UserID[0]].stHu[0].bPasHuChairID;
		}
	}
}

int Table_pingxiangmj::GetListeningInfo(_uint8 ChairID, tagListenInfos stListenInfo[14], _uint8& bListenLen)
{
	HandCards tempHandCard = m_Seats[ChairID].m_HandCard;

	_uint8 bCard = 0xFF;

	memset(stListenInfo, 0, sizeof(stListenInfo));
	for (int i = 0; i<tempHandCard.ChangeableCardsLen; i++)
	{
		//·à?1ê???óDóD???￠?ì×?ê±￡?á?D????àí?μ???×??à′?μ??D??oú??
		if (bCard == tempHandCard.ChangeableCards[i])
		{
			continue;
		}
		bCard = tempHandCard.ChangeableCards[i];
		Check_Listening_LaiZi(&tempHandCard, ChairID, bCard, stListenInfo, bListenLen);
	}
	return (bListenLen != 0 ? TYPE_LISTEN : TYPE_NULL);
}

int Table_pingxiangmj::handler_table_info(Player *player)
{
    Table::handler_table_info(player); 
    
    Ppacket ppack;
	m_pTableInfo.SerializeToString(&ppack.body);
	ppack.pack(SERVER_TABLE_INFO_UC);
	unicast(player, ppack.data);

	log.debug("%s AckTableInfo:%s .\n", __FUNCTION__, m_pTableInfo.DebugString().c_str());
    
    return 0;
}

bool Table_pingxiangmj::Is7Dui(_uint8 ChairID, HandCards& pHc, MJ_fan_buffer& oFan)
{
	if (m_conf.m_bHu7dui)
	{
		bool r = false;

		bool isFourGui = false;
		std::map<_uint8, _uint8> guiCardCountMap;
		int iLaiziNum = removeAllGuiCard(&pHc, guiCardCountMap, isFourGui);

		_uint8 CardDataNum[MJ_TYPE_COMMON][10] = { { 0 }, { 0 }, { 0 }, { 0 }, { 0 } };
		m_GameLogic.GetCardCount(&pHc, CardDataNum);

		if (m_GameLogic.L_IsQiDui(&pHc, INVALID_MJ_CARD, CardDataNum, iLaiziNum))
		{
			oFan.Add(MJ_FAN_TYPE_QIDUI);
			r = true;
		}

		addGuiCard(&pHc, guiCardCountMap);

		return r;

	}
	return false;
}

bool Table_pingxiangmj::Is13Yao(HandCards& pHc, _uint8 c, _uint8 CardDataNum[MJ_TYPE_COMMON][10], int nHunNum, MJ_fan_buffer& oFan)
{
    if (m_conf.m_bHu13yao && m_GameLogic.L_Is13Yao(&pHc, c, CardDataNum, nHunNum))
	{
		oFan.Add(MJ_FAN_TYPE_13YAO);
		return true;
	}
	return false;
}

int Table_pingxiangmj::Check_Hu_Pattern(_uint8 ChairID, HandCards& pHc, MJ_fan_buffer& oFan, MJ_win_pattern& oPattern)
{    
    HandCards tmpHc = pHc;
    int iOpType = TYPE_HU;
    
	bool isFourGui = false;
	std::map<_uint8, _uint8> guiCardCountMap;
	int iLaiziNum = removeAllGuiCard(&pHc, guiCardCountMap, isFourGui);

    if (m_GameLogic.IsQingYiSe(&pHc))
    {
        oFan.Add(MJ_FAN_TYPE_QINGYISE);
    }

    _uint8 CardDataNum[MJ_TYPE_COMMON][10] = { { 0 }, { 0 }, { 0 }, { 0 }, { 0 } };
    m_GameLogic.GetCardCount(&pHc, CardDataNum);

    if (PLAY_MODE_PINGXIANGZZ == m_conf_pxmj.iGameMode)
    {
        if (m_GameLogic.L_IsPengPengHu(&pHc, INVALID_MJ_CARD, CardDataNum, iLaiziNum))
        {
            oFan.Add(MJ_FAN_TYPE_PENGPENGHU);
        }
    }
    else if (PLAY_MODE_PINGXIANG258 == m_conf_pxmj.iGameMode)
    {
        if (m_GameLogic.L_IsPengPengHu(&pHc, INVALID_MJ_CARD, CardDataNum, iLaiziNum))
        {
            if (oFan.Find(MJ_FAN_TYPE_LUANJIANGHU))
            {
                oFan.Remove(MJ_FAN_TYPE_LUANJIANGHU);
                oFan.Add(MJ_FAN_TYPE_JIANGJIANGHU);            
            }
            else
            {
                oFan.Add(MJ_FAN_TYPE_PENGPENGHU);
            }
        }     

        if (m_GameLogic.IsYiTiaoLong_PX(&pHc, oPattern.kan, oPattern.jiang))
        {
            oFan.Add(MJ_FAN_TYPE_YITIAOLONG);
        }    

    	if (!oFan.Find(MJ_FAN_TYPE_LUANJIANGHU) && !oFan.Find(MJ_FAN_TYPE_JIANGJIANGHU) && !oFan.Find(MJ_FAN_TYPE_QINGYISE) 
    	    && !oFan.Find(MJ_FAN_TYPE_13LUAN) && !oFan.Find(MJ_FAN_TYPE_QIDUI) && !oFan.Find(MJ_FAN_TYPE_13FENG))
    	{
        	//258作将
        	if (CardType(oPattern.jiang) >= MJ_TYPE_FENG 
        	    || (CardNum(oPattern.jiang) != 2 && CardNum(oPattern.jiang) != 5 && CardNum(oPattern.jiang) != 8))
        	{
                iOpType = TYPE_NULL;
        	}
        }

        //清一色吃牌后，只能胡清一色
        if (PLAY_MODE_PINGXIANG258 == m_conf_pxmj.iGameMode && m_conf_pxmj.bQingYiSeChi && m_bChiCard[ChairID] != INVALID_MJ_CARD && !oFan.Find(MJ_FAN_TYPE_QINGYISE))
        {
            iOpType = TYPE_NULL;
        }
    }

	addGuiCard(&pHc, guiCardCountMap);

    if (0 == oFan.Size())
    {
        oFan.Add(MJ_FAN_TYPE_PINGHU);
    }

	//自摸
	if (ChairID == m_CurSeat)
	{
	    oFan.Add(MJ_FAN_TYPE_ZIMO);
	}

	return iOpType;
}
int Table_pingxiangmj::Check_Hu(_uint8 ChairID, HandCards& pHc, MJ_fan_buffer& oFan)
{
	if (pHc.ChangeableCardsLen > 14)
	{
		return TYPE_NULL;
	}
	//
	if (pHc.ChangeableCardsLen != 0 && pHc.ChangeableCardsLen % 3 != 2)
	{
		return TYPE_NULL;
	}    

    int iHuType = TYPE_NULL;
    //七对可以和其他番叠加
    if (Is7Dui(ChairID, pHc, oFan))
    {
        iHuType = TYPE_HU;
    }

    if (PLAY_MODE_PINGXIANG258 == m_conf_pxmj.iGameMode)
    {
        if (m_GameLogic.Is13Luan(&pHc))
        {
            oFan.Add(MJ_FAN_TYPE_13LUAN);
            iHuType = TYPE_HU;
        }

        if (m_GameLogic.IsLuanJiangHu(&pHc))
        {
            oFan.Add(MJ_FAN_TYPE_LUANJIANGHU);
            iHuType = TYPE_HU;
        }

        if (m_GameLogic.Is13Feng(&pHc))
        {
            oFan.Add(MJ_FAN_TYPE_13FENG);
            iHuType = TYPE_HU;
        }   
    }

    MJ_win_pattern pattern;
    if (FormatCards(&pHc, pattern) || TYPE_HU == iHuType)
	{
		return Check_Hu_Pattern(ChairID, pHc, oFan, pattern);
	}

	return iHuType;
}

int Table_pingxiangmj::filterNoCardTingGroup(_uint8 ChairID,tagListenInfos stListenInfo[14], tagListenInfos tempListenInfo[14], _uint8 bListenLen)
{
	MJ_BUFFER<_uint8, POOL_CARD_TYPE_MAX> noCardList;   //??óDμ???
	MJ_BUFFER<_uint8, POOL_CARD_TYPE_MAX> haveCardList; //óDμ???
	int nRealLen = 0;
	for (int i = 0; i < bListenLen&&i<14; i++)
	{
		tagHuInfos(&tempHuInfo)[18] = tempListenInfo[i].HuInfo;
		for (int j = 0; j < tempListenInfo[i].bHuLen;j++)
		{
			_uint8 tempCard = tempHuInfo[j].bCard;
			if (haveCardList.Find(tempCard))
			{
				stListenInfo[nRealLen].bOutCard = tempListenInfo[i].bOutCard;//òa′ò3?μ???
				stListenInfo[nRealLen].HuInfo[stListenInfo[nRealLen].bHuLen].bCard = tempCard;//ìyμ???
				stListenInfo[nRealLen].HuInfo[stListenInfo[nRealLen].bHuLen].bFanShu = tempHuInfo[j].bFanShu;
				stListenInfo[nRealLen].bHuLen++;
			}
			else if (!noCardList.Find(tempCard))
			{
				int nCardLeftNum = Get_Card_Left_Num(ChairID, tempCard);
				if (nCardLeftNum == 0)
				{
					noCardList.Add(tempCard);
				}
				else
				{
					haveCardList.Add(tempCard);
					stListenInfo[nRealLen].bOutCard = tempListenInfo[i].bOutCard;//òa′ò3?μ???
					stListenInfo[nRealLen].HuInfo[stListenInfo[nRealLen].bHuLen].bCard = tempCard;//ìyμ???
					stListenInfo[nRealLen].HuInfo[stListenInfo[nRealLen].bHuLen].bFanShu = tempHuInfo[j].bFanShu;
					stListenInfo[nRealLen].bHuLen++;
				}
			}
		}
		if (stListenInfo[nRealLen].bHuLen > 0)
		{
			nRealLen++;
		}
	}
	return nRealLen;
}

void Table_pingxiangmj::GameEnd()
{
	//摸完最后一张牌才算是结束
	MJ_user_buffer UserID;
	_uint8 bDraw = (MultiHu(UserID) == 0) ? 1 : 0;//结束状态，1表示流局，0表示胡牌结束

	//单发结算协议
	proto::game::pingxiangmj::AckGameEndPingXiang stGameEndPingXiang;	
	
	proto::game::AckGameEnd* pGameEnd = stGameEndPingXiang.mutable_gameend();
	for (int i = 0; i < GAME_PLAYER; ++i)
	{
    	pGameEnd->add_piao_score(m_Seats[i].m_UserPiaoScore);
	}
	pGameEnd->set_cur_game_num(m_CurTurn + 1);
	pGameEnd->set_room_game_nums(m_TotalTurn);
	
	HuScoreSet hu_score;

	if (bDraw)
	{
		Score_GameEndDraw(hu_score, *pGameEnd);
	}
	else
	{
		Score_GameEndSucc(hu_score, stGameEndPingXiang);
	}
	pGameEnd->set_endstate(bDraw);

	log.fatal("%s tid:%s bDraw:%d score[%d,%d,%d,%d]\n", __FUNCTION__, tid.c_str(), bDraw, hu_score[0].iTotleScore, hu_score[1].iTotleScore, hu_score[2].iTotleScore, hu_score[3].iTotleScore);

    Proto_GameEndInfo_AddFan(UserID, *pGameEnd);

    int UserScore[GAME_PLAYER] = { 0 };//四个玩家输赢总分
	memset(UserScore, 0, sizeof(UserScore));
	for (int i = 0; i < m_GamePlayer; i++)
	{
	    //即时结算杠分，update_info在杠操作时已调用过一次，结算时去掉杠分以免重复计算
	    if (m_bGangCalcImme)
		{
            UserScore[i] = hu_score[i].iTotleScore - hu_score[i].iGangScore;
        }
        else
        {
		    UserScore[i] = hu_score[i].iTotleScore;
		}
		
		proto::game::HandCards *pHandcard = pGameEnd->add_usercard();
		Fill_HandCard(i, pHandcard);

		Proto_GameEndInfo_GangInfo(i, *pGameEnd);

        pGameEnd->add_score(hu_score[i].iTotleScore);
	}

	for (int i = 0; i < m_GamePlayer; i++)
	{
		if (m_Seats[i].m_Player != NULL)
		{
			if (hu_score[i].iTotleScore  > 0){
				m_Seats[i].m_Player->update_info(UserScore[i], 1, 1, 0);
			}
			else{
				m_Seats[i].m_Player->update_info(UserScore[i], 1, 0, 0);
			}
			pGameEnd->add_money(m_Seats[i].m_Player->money); 
		}
	}

    //更新用户数据
    update_user_info_to_datasvr(UserScore);
    //保存到统计信息与战绩信息
    CalcRecordInfo(UserID, hu_score);

    //累计总分
    for (int i = 0; i < m_GamePlayer; i++)
	{
	    pGameEnd->add_total_score(m_Seats[i].m_SettleInfo.m_wTotalScore);
	}

	Ppacket ppack;
	stGameEndPingXiang.SerializeToString(&ppack.body);
	ppack.pack(SERVER_GAME_END_BC);
	broadcast(NULL, ppack.data);
	log.debug("%s tid:%s, AckGameEndPingXiang:%s .\n", __FUNCTION__, tid.c_str(), stGameEndPingXiang.DebugString().c_str());
	
 	//保存录像
	save_video_game_end(stGameEndPingXiang);
    
    //发送录像
    ev_timer_again(phz.loop, &send_videodata_timer);    

	m_last_Banker = m_BankerSeat;
	//轮庄
	TurnBanker();
	//初始化数据
	GameEndReset();
	m_cbCurTimer = 6;
	m_State = GAME_FREE;
	ev_timer_again(phz.loop, &preready_timer);
	m_cbCurTimer = 2;
	SerializeTableData();
}


