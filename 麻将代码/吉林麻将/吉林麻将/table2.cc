#include "proto/proto.h"
#include "table.h"
#include "zjh.h"
#include "common/log.h"
#include "libnormalmahjong/common_mahjong_logic.h"
#include "game.h"


extern ZJH zjh;


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
			log_info("user_operate_chi ChairID:%d operate:0x%x，card: 0x%02x,but handcard has no cur operate ", ChairID, iOpType, bCard);
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
			log_info("user_operate_chi ChairID:%d operate:0x%x，card: 0x%02x,but handcard has no cur operate ", ChairID, iOpType, bCard);
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
			log_info("user_operate_chi ChairID:%d operate:0x%x，card: 0x%02x,but handcard has no cur operate ", ChairID, iOpType, bCard);
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
	
	//广播操作结果的消息
	BroadcastOperateResult(ChairID, iOpType, bCard);
	log_info(" uid[%d] ChairID[%d]  bCard: %x  ",  GetPlayerUid(ChairID), ChairID, bCard);

	SendTingInfo(ChairID);

	int score[GAME_PLAYER] = { 0 };
	save_video_operate(iOpType, ChairID, m_bOutCardChairID, bCard, score);

	UserAfterOperate(ChairID, iOpType, bCard);	

}

void Table::UserOperatePeng(_uint8 ChairID, _uint8 bCard)
{	
	log_info("uid[%d] ChairID[%d] bCard [%x] ",  GetPlayerUid(ChairID), ChairID, bCard);

	memset(m_bAddFanFlag, 0, sizeof(m_bAddFanFlag));	
	memset(m_bUserOperate, 0, sizeof(m_bUserOperate));

	BroadcastOperateResult(ChairID, TYPE_PENG, bCard);

	int fixedlen = m_HandCard[ChairID].FixedCardsLen;
	
	if (fixedlen >= HAND_CARD_KAN_MAX)
	{
		log_error("玩家组合牌长度:%d大于正常的长度  ", fixedlen);
		return;
	}
	if(m_bOutCardChairID>=0 && m_bOutCardChairID< m_GAME_PLAYER  && ChairID>= 0 && ChairID< m_GAME_PLAYER)
	{
		m_GameLogic.ExecutePeng(&m_HandCard[ChairID], m_bOutCardChairID, bCard);
		m_bGetCardCount[ChairID]++;

		//从打出牌删掉这张牌	
		//if(m_stUserOutCard[m_bOutCardChairID].Size() )   // core  bug !!!!!
		m_stUserOutCard[m_bOutCardChairID].RemoveLast(bCard);
		//TODO 检查是否听牌
		SendTingInfo(ChairID);

		UserAfterOperate(ChairID, TYPE_PENG, bCard);

		int score[GAME_PLAYER] = { 0 };
		save_video_operate(TYPE_PENG, ChairID, m_bOutCardChairID, bCard, score);
	}
	else
	{
		log_error("outid chairid %d %d" , m_bOutCardChairID , ChairID );
	}

}

int Table::CheckOutCardOtherPlayerCanOpCode(_uint8 ChairID, _uint8 bCard, MJ_major& major, int& fan_num)
{	
	
	if (major.m_oFan.Size() != 0)
	{
		log_error("majof fan[%lu]" , major.m_oFan.Size() );
		return -1;
	}

	int iOpType = TYPE_NULL;
	if (ChairID >= GAME_PLAYER)
	{
		log_error("chair_id[%d]",  ChairID );
		return -1; 
	}
	
	HandCards* pHc = &m_HandCard[ChairID];		
	iOpType |= GetChi(ChairID, bCard, major.m_Chi);
		
	iOpType |= GetPengGang(ChairID, bCard, major.m_GetPengGang);	
	//assert(pHc->ChangeableCardsLen < HAND_CARD_SIZE_MAX);
	if (pHc->ChangeableCardsLen >= HAND_CARD_SIZE_MAX)
	{
		log_error("carlen[%d] uid[%d] %s  card[%x] " , 
			pHc->ChangeableCardsLen , GetPlayerUid(ChairID) ,  CatHandCard(*pHc , ChairID ) , bCard ); 
		return -1; 
	}

	if (!m_conf.m_bZimoHu)   //只能自摸操作。。
	{
		m_GameLogic.AddCard(pHc, bCard);
		
		iOpType |= Check_Hu(ChairID, *pHc, major.m_oFan , bCard );		
		if (CardPoolIsEmpty())
		{
			UserLastOutCard(ChairID, iOpType, major.m_oFan);
		}		
		
		if ( IsQuanqiuren(*pHc, major.m_oFan, iOpType))
		{
		}		
		
		if (m_bGetCardCount[m_bBanker] == 1)
		{	
			//庄家打出第一张牌
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
	else if ( m_bRobGangHuJudge || ( m_bRobWanGang  && m_bAddFanFlag[m_WangGangPlayer]))
	{
		m_GameLogic.AddCard(pHc, bCard);

		iOpType |= Check_Hu(ChairID, *pHc, major.m_oFan , bCard);

		if (iOpType & TYPE_HU)
		{
			UserQiangGangHu(ChairID, bCard, major.m_oFan);
			fan_num = Calc_Fan(ChairID, major.m_oFan);
		}
		m_GameLogic.ChuPai(pHc, bCard);
	}
	return iOpType;	
}

 //起手闲家， 是否能报叫 或者是报听 ！！！ 
//思路就是加一张。 判断。。 再把这张减出去。。一定要记得减出去。。要不就是问题了

int Table::check_player_can_bao_ting(_uint8  ChairID , int end_pos  )
{
	if (ChairID < 0 || ChairID >= m_GAME_PLAYER)
		return 0;
	end_pos = std::min(34, end_pos);

	HandCards* pHc = &m_HandCard[ChairID];	
	static int  all_card[] =
	{
		0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09,//万	
		0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19,//条	
		0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29,//筒	
		0x31 , 0x32 , 0x33,0x34, 0x41,0x42,0x43 , 
	};
	for (int i = 0; i < end_pos  ; i++) 
	{ 
		int iOpType = TYPE_NULL;
		int   bCard = all_card[i];
		m_GameLogic.AddCard(pHc, bCard);
		if (m_GameLogic.cur_card_count(pHc, bCard) > 4)
		{
			m_GameLogic.ChuPai(pHc , bCard);
			log_debug("max 4ge ..");
			continue; 
		}		
		MJ_major  major;
		iOpType |= Check_Hu(ChairID, *pHc, major.m_oFan , bCard );
		m_GameLogic.ChuPai(pHc, bCard);
		if (iOpType  & TYPE_HU)
		{
			log_debug("card[%x]" ,  bCard  );
			return 1;
		}
	}
	return 0;
 
}




//摸牌判断  玩家自己摸牌。 自摸可以进行的操作进行判断 。。

int Table::CheckPlaySlefGrpCardCanOperateCode(_uint8 ChairID, _uint8 bCard, MJ_major& major, int& fan_num)
{
	if (ChairID >= m_GAME_PLAYER)
		return 0;
	if (major.m_oFan.Size() != 0)
	{
		log_error("error");
		return 0;
	}		

	int iOpType = TYPE_NULL;
	HandCards* pHc = &m_HandCard[ChairID];	
	if (pHc->ChangeableCardsLen >= 14)
	{
		log_error("eror %d %d " , ChairID , GetPlayerUid(ChairID));
		//return  0;  // bug!!
	}

	int add_res =  m_GameLogic.AddCard(pHc, bCard); 
	log_debug("uid[%d] sid[%d]  len_card[%d]  bcard[%x]" , GetPlayerUid(ChairID) , ChairID , pHc->ChangeableCardsLen , bCard  );

	
	if (pHc->ChangeableCardsLen % 3 != 2)  // 3 *n +2 模式 。
	{
		log_error(" fatal paishu error uid:%d chairid:%d! %s ",  GetPlayerUid(ChairID), ChairID  ,  CatHandCard(*pHc , ChairID)); 
		if(add_res>=0 )
			m_GameLogic.ChuPai(pHc, bCard);
		return TYPE_NULL;
	}
	
	if (pHc->ChangeableCardsLen >  HAND_CARD_SIZE_MAX)
	{
		log_error("handcard[%d]" ,pHc->ChangeableCardsLen );   //手上的牌 
		if(add_res >= 0 )
			m_GameLogic.ChuPai(pHc, bCard);  //  这些都是容错出来。。
		return  0;
	}
	
		
	 iOpType |= GetGangInfo(  ChairID, major.m_GetGangInfo);

	iOpType |= Check_Hu(ChairID, *pHc, major.m_oFan , bCard );
	if (m_bGetCardCount[ChairID] == 1) // && ChairID == m_bBanker )
	{
		UserGetFirstCard(ChairID, iOpType, major.m_oFan);
	}		
	if (CardPoolIsEmpty())
	{
		UserGetLastCard(ChairID, iOpType, major.m_oFan);
	}		
	if (iOpType & TYPE_HU)
	{
 		if ( m_bAddFanFlag[ChairID])   //杠上花
 		{
			UserGangShangHua(ChairID, bCard, major.m_oFan); 			
 		}
		fan_num = Calc_Fan(ChairID, major.m_oFan);
	}
	if(add_res>= 0 )
		m_GameLogic.ChuPai(pHc, bCard);   //  13  

	return iOpType;
}


void Table::InitBanker()
{
	if ( m_nPlayingTimes == 0)
	{
		m_bBanker = 0;
		for (int i = 0; i < m_GAME_PLAYER; i++)
		{
			if ( m_SeatPlayer[i].player != NULL)
			{
				log_info("room %d player uid:%d, room owner:%d ",  tid, GetPlayerUid(i), m_room_owner_uid);
				if ( m_SeatPlayer[i].player->uid == m_room_owner_uid)
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
	//assert(m_bBanker < m_GAME_PLAYER);
}

//一局结束后。。计算下 庄家 

void Table::TurnBanker()
{

	m_bBanker = NextChair(m_bBanker);

}

//哦这个函数是判断是否可以胡牌的。   
//但是操蛋的是 把牌型番数。也加进去了。我也很无语


int Table::Check_Hu(_uint8 ChairID, HandCards& pHc, MJ_fan_buffer& oFan , _uint8 add_last_card )
{

	if (pHc.ChangeableCardsLen > 14)
	{
		log_error("cardlen %d", pHc.ChangeableCardsLen );
		return TYPE_NULL;
	}
	
	
	//取余3不等于2的手牌不做胡牌判断
	if (pHc.ChangeableCardsLen != 0 && pHc.ChangeableCardsLen % 3 != 2)
	{
		log_error("cardlen[%d]", pHc.ChangeableCardsLen);
		return TYPE_NULL;
	}		
	MJ_win_pattern pattern;

	if ( Is7Dui(ChairID, pHc, oFan))
	{
		oFan.Add(MJ_FAN_TYPE_QIDUI);
		log_info("qidui");
		//return TYPE_HU;
	}	
	if ( m_GameLogic.is_jiang_jiang_hu(&pHc))
	{
		oFan.Add(MJ_FAN_CHANGSHA_JIANGJIANGHU);
		log_info("jiang jiang hu !!");
	}
	// 清一色。。加番。。
	if ( oFan.Size() && m_GameLogic.IsQingYiSe(&pHc))  
	{
		oFan.Add(MJ_FAN_TYPE_QINGYISE);
		log_debug("add qing yise ");
	}
	

	if (FormatCards(&pHc, pattern))  // 判断是否能胡牌
	{
		 Check_Hu_Pattern(ChairID, pHc, oFan, pattern);
	}
	
	return oFan.IsNull() ? TYPE_NULL : TYPE_HU;


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
		int iOpType = CheckOutCardOtherPlayerCanOpCode(ChairID, bCard, major, m_HuFanNums[ChairID]);
		if (iOpType & TYPE_HU)
		{
			m_oFan[ChairID] = major.m_oFan  ;			
			Score[ChairID]          += m_HuFanNums[ChairID] * m_BaseScore;
			Score[m_WangGangPlayer] -= m_HuFanNums[ChairID] * m_BaseScore;
			m_stHuScore[ChairID].stHu.Add( tagHuRecord(m_WangGangPlayer, bCard, HU_PAOHU, m_HuFanNums[ChairID]));			
			log_debug("paohu   hucard  %x " , bCard );
		}
		log_debug("iop [%x]" , iOpType);
	}
	else if (ChairID == m_bCurChairID)
	{	
		//自摸
		int iOpType = CheckPlaySlefGrpCardCanOperateCode(ChairID, INVALID_MJ_CARD, major, m_HuFanNums[ChairID]); 				
		if (iOpType & TYPE_HU )
		{			
			m_oFan[ChairID] = major.m_oFan;
			for (_uint8 c = NextChair(ChairID); c != ChairID; c = NextChair(c))
			{
				Score[ChairID] +=    m_HuFanNums[ChairID] * m_BaseScore;
				Score[c]       -=    m_HuFanNums[ChairID] * m_BaseScore;
			}
			m_stHuScore[ChairID].stHu.Add(tagHuRecord(ChairID, bCard, HU_ZIMO, m_HuFanNums[ChairID]));			
			log_debug("zimo iop [%x]   hucard %x", iOpType ,  bCard );
		}	
		else
		{
			log_error("iop %x" , iOpType);
		}
	}
	else
	{
		//放炮
		int iOpType = CheckOutCardOtherPlayerCanOpCode(ChairID, bCard, major, m_HuFanNums[ChairID]);
		//if (iOpType == -1 && bCard< 0x43 )
		if (0 )
		{			
			if (m_HandCard[ChairID].ChangeableCardsLen % 3 == 0 )
				m_GameLogic.AddCard(&m_HandCard[ChairID], bCard);
			else  if(m_HandCard[ChairID].ChangeableCardsLen %3 == 2)
				m_GameLogic.ChuPai( &m_HandCard[ChairID], bCard);   

			iOpType = CheckOutCardOtherPlayerCanOpCode(ChairID, bCard, major, m_HuFanNums[ChairID]);  
			log_error("error !!! type %x  bcard %x   pai %s " , iOpType , bCard ,  CatHandCard(m_HandCard[ChairID] , ChairID ) );
		}

		if (iOpType & TYPE_HU)
		{
			m_oFan[ChairID]      = major.m_oFan;
			Score[ChairID]       += m_HuFanNums[ChairID] * m_BaseScore;			
			Score[m_bCurChairID] -= m_HuFanNums[ChairID] * m_BaseScore;
			m_stHuScore[ChairID].stHu.Add(tagHuRecord(m_bCurChairID, bCard, HU_PAOHU, m_HuFanNums[ChairID] ));			
			log_debug("fangpaohu    hucard %x " , bCard   );
		}		
		else
		{
			log_error("iop [%x]", iOpType);
		}
		
	}

	//DealNingxiangHuBroad(ChairID, major.m_oFan  );

	
	return 0;

}
void Table::UserGetFirstCard(_uint8 ChairID, int& iOpType, MJ_fan_buffer& oFan)
{
	if (iOpType & TYPE_HU)
	{
		if (ChairID == m_bBanker)
		{
			//oFan.Add( MJ_FAN_TYPE_TIANHU);
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
	oFan.Add( MJ_FAN_TYPE_QIANGGANGHU);
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
void Table::Proto_Game_UserGetCard(_uint8 ChairID, _uint8 MoChairID, _uint8 bCard, proto::game::AckUserGetCard& stUsergetcard)
{
	
	stUsergetcard.Clear();
	stUsergetcard.set_chairid(MoChairID);
	stUsergetcard.set_leftcardnum(m_CardPool.Count());

	//如果是摸牌用户，则打包摸牌、听牌等数据，非摸牌用户则不发送这些数据
	if (ChairID == MoChairID)
	{
		stUsergetcard.set_card(bCard);
		if (m_bUserOperate[MoChairID] & TYPE_ANGANG || m_bUserOperate[MoChairID] & TYPE_WANGANG)
		{
			MJ_opinfo stGang;			
			GetGangInfo(MoChairID, stGang);
			for (int i = 0; i < stGang.Size(); i++)
			{
				
				log_info(" uid:%d chairID:%d angang: %x", GetPlayerUid(ChairID), ChairID, stGang[i].cbCard);
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
		stUsergetcard.set_optype( m_bUserOperate[MoChairID]);
		if (m_bUserOperate[MoChairID] & TYPE_LISTEN)
		{
			//打包听牌信息  这个sb 代码。。 额。。  稀乱的。 
			for (int j = 0; j < bListenLen; j++)
			{
				// log_debug("can_out_card   [%x]" , stListenInfo[j].bOutCard  );
				PackageTingInfo(ChairID, stListenInfo, j, stUsergetcard.add_listeninfo());		
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
		for ( int j = 0; j < m_stUserOutCard[i].Size(); j++)
		{
			pstShow->add_outcard(m_stUserOutCard[i][j]);
		}
		pstShow->set_outcardlen(m_stUserOutCard[i].Size());
	}
}

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
}

void Table::SendTingInfo(_uint8 ChairID)
{
	tagListenInfos stListenInfo[14];
	_uint8  bListenLen = 0;	
	//胡牌不用判断听牌，自动胡牌不用判断听牌
	if ( GetListeningInfo(ChairID, stListenInfo, bListenLen))
	{
		m_bUserOperate[ChairID] |= TYPE_LISTEN;
	}


	// 发送听牌协议
	proto::game::AckListenNotify stListen;
	if (m_bUserOperate[ChairID] & TYPE_LISTEN)
	{
		// log_info(" uid:%d chairid:%d ListenLen:%d " , GetPlayerUid(ChairID), ChairID, bListenLen);
		stListen.set_optype(TYPE_LISTEN);
		//打包听牌信息
		for (int j = 0; j < bListenLen; j++)
		{
			log_debug("peng out %x   blen %d" , stListenInfo[j].bOutCard  , bListenLen  );  
			PackageTingInfo(ChairID, stListenInfo, j, stListen.add_listeninfo());
		}
	}
	else
	{
		stListen.set_optype(TYPE_NULL);
		log_info("  uid:%d chairid:%d no listen ",  GetPlayerUid(ChairID), ChairID);
	}
	unicast(SERVER_LISTEN_NOTIFY, ChairID, stListen); 

	log_debug("ting listen   %s  ", stListen.ShortDebugString().c_str());
	
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
		log_info(" uid:%d chairid:%d ListenLen:%d ",  GetPlayerUid(ChairID), ChairID, bListenLen);
		stListen.set_optype(TYPE_LISTEN);

		PackageTingInfo(ChairID, stListenInfo, 0, stListen.add_listeninfo());
		
	}	
	unicast(SERVER_LISTEN_NOTIFY, ChairID, stListen);
}


void Table::UserAfterOperate(_uint8 ChairID, int iOpType, _uint8 bCard)
{
	m_bCurChairID = ChairID;
	log_info(" uid:%d chairid:%d operate: %x card: %x ",  GetPlayerUid(ChairID), ChairID, iOpType, bCard);

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
}

void Table::DoMultiOperate(_uint8 ChairID, int iOpType, _uint8 bCard)
{
	m_bMultiDoneOpNum++;
	m_bMultiUserDoneOp[ChairID] = iOpType;
	m_bMultiUserDoneOpCard[ChairID] = bCard;

	log_info(" uid[%d] chaird[%d] curoptype[%x]  optype:%x, now done OpNum:%d, totalOpnum:%d  m_bCurChairID %d ", 
			GetPlayerUid(ChairID), ChairID, m_bUserOperate[ChairID], 
		 iOpType, m_bMultiDoneOpNum, m_bMultiOpUserNum, m_bCurChairID
		); 

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
				log_info("  force user uid:%d pass seat:%d! ",   GetPlayerUid(c), c);
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
		//log_debug("here  this place !!   ");
		if (iOpType == TYPE_PASS)
		{
			if (m_bUserOperate[ChairID] & TYPE_HU)   //胡牌玩家选择了过牌
			{
				m_bMulitHuOpeCount++;
				log_info(" user uid:%d pass seat:%d, optype:0x%x! ", 
					 GetPlayerUid(ChairID), ChairID, m_bUserOperate[ChairID]);
				m_PassHuPlayer.Add(ChairID);
			}
			UserOperatePass(ChairID, iOpType, bCard);  
			//log_debug("pass over !! ");
		}
		else
		{
			if (iOpType != TYPE_NULL)
			{				
				//assert(iOpType != TYPE_PASS);
				//assert(m_bRecordMultiOp.iOpType != TYPE_PASS);
				if (iOpType == TYPE_PASS || m_bRecordMultiOp.iOpType == TYPE_PASS)
				{
					log_error("error  into this  !!"); 
					return;
				} 
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
				//assert(m_bRecordMultiOp.iOpType != TYPE_NULL);
			}
		}
	}
	// log_debug(" %d %d ", m_bMultiDoneOpNum, m_bMultiOpUserNum );  // 1  

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
		}
		else
		{
			//assert(m_bRecordMultiOp.iOpType != TYPE_PASS);
			if (m_bRecordMultiOp.iOpType == TYPE_PASS)
			{
				log_error("error !!!"); 
				return;
			}
			 // log_debug("m_bRecordMultiOp.iOpType    %x", m_bRecordMultiOp.iOpType  );
			if (m_bRecordMultiOp.iOpType != TYPE_NULL)
			{				
				MultiOperateOver(m_bRecordMultiOp.ChairID, m_bRecordMultiOp.iOpType, m_bRecordMultiOp.bCard);								
			}
			else
			{
				MultiOperateOver(ChairID, iOpType, bCard);				
				
				OnUserGrapCard(NextChair(m_bCurChairID));
			}
		}
		init_multi_data();
	}
	else//优先级操作了，优先级低的就忽略了
	{
		// log_debug("  m_bMulitFirstOperate  %d " , m_bMulitFirstOperate );
		if (m_bMulitFirstOperate)
		{
			if (m_bMulitHuOpeCount != m_bMultiOpHuNum)
			{
				log_info("return this place !");
				return;
			}
			/*未到达*/
			if (m_conf.m_bYiPaoDuoXiang)
			{
				//一炮多响,其他玩家自动胡				
			}
			else
			{
				int size = 0;
				for (_uint8 c = NextChair(m_bCurChairID); c != m_bCurChairID; c = NextChair(c))
				{
					if(size++> 19) 
						break; 
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
			//log_debug("into this place !!! ");
			bool bOver = true;	
			int  time = 0;
			for (_uint8 c = NextChair(m_bCurChairID); c != m_bCurChairID; c = NextChair(c))
			{				
				if (time++> 10 )
				{
					log_error("error time %d" , time); 
					return;
				}
				
				if (m_bMultiUserDoneOp[c] != TYPE_NULL)
					continue;
				int tmpOpType = m_bUserOperate[c];
				if (tmpOpType == TYPE_NULL)
					continue;
				//assert(tmpOpType != TYPE_PASS);
				if (tmpOpType == TYPE_PASS)
				{
					log_error("erro %x" , tmpOpType );
					return; 
				}
				
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
			// log_debug("this   over %d" , bOver) ;
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
				MultiOperateOver(m_bRecordMultiOp.ChairID, m_bRecordMultiOp.iOpType, m_bRecordMultiOp.bCard); 

				init_multi_data(); 
				//log_debug("over thisi place 1!!!!!!!   ");
			}
		}
	}
//	log_debug("over !! ");

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

void Table::MultiOperateOver(_uint8 ChairID, int iOpType, _uint8 bCard)
{		
	if (iOpType == TYPE_PASS || ChairID>= m_GAME_PLAYER  )
	{
		log_error("erroe !!");
		return;
	}
	  // log_debug("into place !!  sid[%d]  type[%x]  card[%x]   m_bCurChairID[%d]  ChairID[%d]", ChairID, iOpType, bCard, m_bCurChairID, ChairID);
	int  s_time = 0;
	for (_uint8 c = NextChair(m_bCurChairID); c != ChairID;    c = NextChair(c))
	{
		//log_debug("into this place !! c  %d   m_bMultiUserDoneOp [%x]", c, m_bMultiUserDoneOp[c]  );
		if (s_time++ > 10)
		{
			log_error("error %d " , s_time ) ;
			break;
		}
		

		if (m_bMultiUserDoneOp[c] == TYPE_PASS)
			continue;
		if (MultiOpOr(iOpType, TYPE_GANG))		
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
		if (m_conf.m_bQiangZhiGang && RobGangHu(ChairID, bCard, iOpType))
			return;
		UserOperateGang(ChairID, iOpType, bCard);

		OnUserGrapCard(ChairID, true);
		break;	
	default:
		break;
	}
}
int Table::GetChi(_uint8 ChairID, _uint8 bCard, MJ_opinfo& ChiInfo)
{
	if (m_conf.m_bSupportChi)
	{
		return m_GameLogic.GetChi(&m_HandCard[ChairID], bCard, ChiInfo);
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
	if (  m_GameLogic.is_qi_dui(&pHc))
	{		
		oFan.Add(MJ_FAN_TYPE_QIDUI);
		
		log_debug("into table  qidui "); 
		
		_uint8 hao_hua_qidui = 0;
		hao_hua_qidui = m_GameLogic.hao_hua_qi_dui(&pHc);

		log_debug("haohua [%d]", hao_hua_qidui);

		if (hao_hua_qidui == 1)
			oFan.Add(MJ_FAN_HAO_HUA_QI_XIAO_DUI);
		if (hao_hua_qidui == 2)
			oFan.Add(MJ_FAN_HAO_HUA_DOUBLE_QI_XIAO_DUI);
		if (hao_hua_qidui == 3)
			oFan.Add(MJ_FAN_HAO_HUA_THREE_QI_XIAO_DUI);

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
	std::map<int, Player*>::iterator it;
	if ((it = m_mapTable_Player.find(client->uid)) != m_mapTable_Player.end())
	{
		Player* player = it->second;
		if (player == NULL)
			return false;
		if (!val["skey"].isNull() && val["skey"].asString() == client->skey)
		{
			log_info("%s reconnect skey %s success! ", __FUNCTION__, client->skey.c_str());
			player->skey = val["skey"].asString();
			return true;
		}
	}
	return false;
}

int Table::dispatch_client(int cmd, Client *client, Player *player)
{
	if (client == NULL || player == NULL)
	{
		log_error("%s:%d\t%s roomid:%d client has disbanded \n",__FILE__,__LINE__,__FUNCTION__ , tid);
		return -1;
	}
	switch (cmd)
	{
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

//
//void Table::SendPlayerFirstCardOp(_uint8 ChairID)
//{
//	MJ_major major;
//	m_bUserOperate[ChairID] = CheckPlaySlefGrpCardCanOperateCode(ChairID, INVALID_MJ_CARD, major, m_HuFanNums[ChairID]);
//	if (m_bUserOperate[ChairID] != TYPE_NULL && m_bUserOperate[ChairID] != TYPE_LISTEN)
//	{
//		for (int i = 0; i < m_GAME_PLAYER; i++)
//		{
//			proto::game::AckOpNotify stNotify;
//			stNotify.Clear();
//
//			stNotify.set_chairid(ChairID);
//			if (ChairID == i)
//			{//非打牌玩家
//				int iOpType = m_bUserOperate[ChairID];
//				if (m_bUserOperate[ChairID] & TYPE_GANG)
//				{
//					for (int i = 0; i < major.m_GetGangInfo.Size(); i++)
//					{
//						log_info("%s uid:%d chairID:%d angang:0x%02x\n", __FUNCTION__, GetPlayerUid(ChairID), ChairID, major.m_GetGangInfo[i].cbCard);
//						proto::game::tagGangInfo *pstGang = stNotify.add_gang();
//						pstGang->set_card(major.m_GetGangInfo[i].cbCard);
//						pstGang->set_state(major.m_GetGangInfo[i].nState);
//					}
//
//				}
//				stNotify.set_card(m_bCurCard);
//				stNotify.set_optype(iOpType);
//			}
//			unicast( SERVER_OPERATE_NOTIFY, i, stNotify);
//		}
//	}
//	if (m_conf.m_bQiShou_14_zhang)
//	{
//		if (m_bUserOperate[ChairID] == TYPE_NULL || m_bUserOperate[ChairID] == TYPE_LISTEN)
//		{
//			set_out_timer(ChairID);
//		}
//		else
//		{
//			set_operator_timer(ChairID);
//		}
//	}
//}

void Table::GetFirstHandCard_NoOp(_uint8 ChairdID)
{
	_uint8 bCard = m_CardPool.GetOneCard();
	//	log_debug("get fist card  chairid[%] uid[%d]  card[%x] " , ChairdID ,  GetPlayerUid(ChairdID)  , bCard );

	// m_nGameRunState = GAME_QISHOU_BAOTING_OVER; 

	m_bCurChairID = ChairdID;
	m_bGetCardCount[ChairdID]++;
	m_bCurCard = bCard;

	m_GameLogic.AddCard(&m_HandCard[ChairdID], bCard);

	save_user_get_card(ChairdID, bCard, m_CardPool.Count());

	//广播庄家获得手牌
	proto::game::AckUserGetCard stUsergetcard;
	stUsergetcard.set_chairid(ChairdID);
	stUsergetcard.set_leftcardnum(m_CardPool.Count());
	for (_uint8 c = NextChair(ChairdID); c != ChairdID; c = NextChair(c))
	{
		unicast(SERVER_USER_GETCARD, c, stUsergetcard);
	}
	stUsergetcard.set_card(bCard);
	stUsergetcard.set_optype(TYPE_NULL);
	unicast(SERVER_USER_GETCARD, ChairdID, stUsergetcard);

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
