#include "table_xiaohua.h"
#include <vector>

#include "../proto/proto.h"
#include "../zjh.h"
#include "../common/log.h"
#include "../game.h"
#include "../libnormalmahjong/mj_common2.h"
#include "../libnormalmahjong/game_logic.h"
#include "../table_helper.h"

using namespace  proto::game::hunanmj; 
Table_XiaoHua::Table_XiaoHua(int player)
{

	m_GAME_PLAYER = player;
	memset(&_xiaohua_conf, 0, sizeof(XiaoHua_conf));		
	m_ft2fn = &g_xiaohua_ft2fn;	
	m_conf.m_AnGangPoint = 4;
	m_conf.m_WanGangPoint = 2;
	m_conf.m_ZhiGangPoint = 2;
	m_kezi = 0;
	memset(m_buzhang, 0, sizeof(m_buzhang));
	m_HuWeiDa = false;
	m_hucount = -1; 
	m_curGangPlayerSid = INVALID_CHAIRID; 
	for (int i = 0; i < m_GAME_PLAYER; i++)
	{
		m_MultiFan[i].Clear();
	}
	
	memset(m_MultiHufanNums, 0, sizeof(m_MultiHufanNums)); 
	m_gang_hou_card.Clear();

	for (int i = 0; i < m_GAME_PLAYER; i++)
	{
		m_gang_hou_op[i].Clear();
		m_MultiFan[i].Clear();
		m_MultiHufanNums[i] = 0;
	}
	m_Gang_buzhang_res = 0;
	m_HuWeiDaCard = INVALID_MJ_CARD;
	m_HuWeiDaOp = TYPE_NULL;
	
	memset(m_RecvBaipaiMap, 0, sizeof(m_RecvBaipaiMap));  //默认为0   摆牌选择 
	memset(m_nRecvBaoTingPlayerMap, 0, sizeof(m_nRecvBaoTingPlayerMap));
	memset(m_nCanBaoTingArr, 0, sizeof(m_nCanBaoTingArr));
	memset(m_BaoTingPos, -1, sizeof(m_BaoTingPos));
	


}

void Table_XiaoHua::GameEndReset()
{

	Table::GameEndReset();
	m_HuWeiDa = false; 
	m_HuWeiDaOp = TYPE_NULL;
	m_HuWeiDaCard = INVALID_MJ_CARD;
	m_curGangPlayerSid = INVALID_CHAIRID;
	
	for (int i = 0; i < m_GAME_PLAYER; i++)
	{
		m_MultiFan[i].Clear();
	}
	memset(m_MultiHufanNums, 0, sizeof(m_MultiHufanNums));

	memset(m_buzhang, 0, sizeof(m_buzhang));
	m_gang_hou_card.Clear();

	for (int i = 0; i < m_GAME_PLAYER; i++)
	{
		m_gang_hou_op[i].Clear();
		m_MultiFan[i].Clear();
		m_MultiHufanNums[i] = 0;
	}
	memset(m_buzhang, 0, sizeof(m_buzhang));
	memset(m_RecvBaipaiMap, 0, sizeof(m_RecvBaipaiMap));  //默认为0   摆牌选择 
	memset(m_nRecvBaoTingPlayerMap, 0, sizeof(m_nRecvBaoTingPlayerMap));
	memset(m_nCanBaoTingArr, 0, sizeof(m_nCanBaoTingArr)); 
	memset(m_stUserOutCardAfterBaoTing, 0, sizeof(m_stUserOutCardAfterBaoTing));
	memset(m_BaoTingPos, -1, sizeof(m_BaoTingPos));

	m_Gang_buzhang_res = 0; 
}


void Table_XiaoHua::GangHouClear()
{
	m_curGangPlayerSid = INVALID_CHAIRID;
	memset(&m_gang_shai, 0, sizeof(m_gang_shai));
	m_gang_hou_card.Clear();
	for (int i = 0; i < m_GAME_PLAYER; i++)
	{
		m_gang_hou_op[i].Clear();
		m_MultiFan[i].Clear();
		m_MultiHufanNums[i] = 0;
	}
	m_Gang_buzhang_res = 0; 

	m_HuWeiDa = false;
	m_HuWeiDaCard = INVALID_MJ_CARD;
	m_HuWeiDaOp = TYPE_NULL;

}



Table_XiaoHua::~Table_XiaoHua()
{
	log_debug("delete xiaohua   ");
	
}

void  Table_XiaoHua::GameStart()
{
	Table::GameStart(); 
	Table::OnUserGrapCard(m_bBanker);	
}

int Table_XiaoHua::FixPengCard(const  HandCards *phc)
{
	if (phc->FixedCardsLen != 1)
		return 0;
	for (int i = 0; i < phc->FixedCardsLen;i++)
	{
		return phc->FixedCards[i].CardData;
	}
	return 0;
}

int Table_XiaoHua::Special_Hu_If_Remove_One_Next_Add( _uint8  chairId   ,  _uint8  add_last_card  )
{
	if (chairId< 0 || chairId  >= m_GAME_PLAYER)
		return  0; 

	HandCards* pHc = &m_HandCard[chairId];
	int  add_card[] =
	{
		0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09,//万	
		0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19,//条	
		0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29,//筒	
		0x31, 0x32, 0x33, 0x34, 0x41, 0x42, 0x43, 
	};
	int fix_c = FixPengCard(pHc);
	bool HandNoFixCard = true;
	for (int i = 0; i < pHc->ChangeableCardsLen; i++)
	{
		if (pHc->ChangeableCards[i] == fix_c )
			HandNoFixCard = false;		
	}

	bool can_hu = false;
	map<int, int> all_card_map;
	for (int r = 0; r < pHc->ChangeableCardsLen; r++)
		all_card_map[pHc->ChangeableCards[r]] = 1;

	for ( map<int, int>::iterator it = all_card_map.begin(); it != all_card_map.end(); ++it)
	{
		int ct = it->first;
		m_GameLogic.ChuPai(pHc, ct);               // remove ----111111111111111111111  
		for (int i = 0; i <  34   ; i++)
		{
			if (add_card[i] > 0x40  && _xiaohua_conf.man_tian_hua ==false ) continue; 
			if ( (add_card[i] ==   0x30+ m_bBanker +1) &&_xiaohua_conf.man_tian_hua == false  ) 
				continue; 

			if (_xiaohua_conf.man_tian_hua &&  add_card[i] > 0x30) 
				continue; 

			m_GameLogic.AddCard(pHc, add_card[i]);  //  add ---------------------22222222222222222
			if (m_GameLogic.cur_card_count(pHc, add_card[i]) > 4 )
			{
				m_GameLogic.ChuPai(pHc, add_card[i]);  // rmeove -----------------2222222222222222222
				continue; 
			}			
			if ( m_GameLogic.is_qi_dui(pHc) )
				can_hu = true;
			if (HandNoFixCard  &&  m_GameLogic.xiaohua_haohua_qi_dui(pHc,  add_card[i] ))
					can_hu = true;
							
			m_GameLogic.ChuPai(pHc, add_card[i]);  // rmeove ---------------------------2222222222222222
			if ( can_hu==true)
				break;
		}		
		m_GameLogic.AddCard(pHc, ct);       // add ---111111111111111111111111111111111111111


		if (can_hu)
		{
			log_debug("it [%x]" , it->first );
			return 1;
		}
		
		
	}

	return 0;
}

int Table_XiaoHua::Check_Hu(_uint8 ChairID, HandCards& pHc, MJ_fan_buffer& oFan , _uint8 add_last_card )
{
	if ( _xiaohua_conf.man_tian_hua &&  add_last_card > 0x30  &&  add_last_card != INVALID_MJ_CARD ) 
	{
		//log_error("add last care %x" , add_last_card ); 
		return 0;
	}
		
	if (IsHandHasHuaCard(&pHc) && _xiaohua_conf.man_tian_hua) //有花。。不能胡牌
	{
		//log_error("all hua ");
		return 0; 
	}
	if (_xiaohua_conf.man_tian_hua == false && HasFeng(ChairID))
	{
		return 0;
	}
		
	if (pHc.ChangeableCardsLen > 14)
	{
		log_error("cardlen %d", pHc.ChangeableCardsLen);
		return TYPE_NULL;
	}
	//取余3不等于2的手牌不做胡牌判断
	if (pHc.ChangeableCardsLen != 0 && pHc.ChangeableCardsLen % 3 != 2)
	{
		log_error("cardlen[%d]", pHc.ChangeableCardsLen);
		return TYPE_NULL;
	}
	oFan.Clear(); 	
	MJ_win_pattern pattern;
	if ( Is7Dui(ChairID, pHc, oFan))
	{
		oFan.Add(MJ_FAN_TYPE_QIDUI);
		log_info("3.七大对");		
	}	
	if (ChairID == m_bCurChairID  &&  m_GameLogic.xiaohua_haohua_qi_dui((const HandCards*)&pHc, add_last_card))
	{ 
		oFan.Add( MJ_FAN_HAO_HUA_QI_XIAO_DUI);
		log_info("hao hua qidui  [%x]", add_last_card );
	}
	if ( FormatCards(&pHc, pattern))  // 判断是否能胡牌
	{
		Check_Hu_Pattern(ChairID, pHc, oFan, pattern);
	}
	else
	{
		if (oFan.Size())
			AddFanFunXiaoHu(ChairID, pHc, oFan, pattern , false , 0   );
	}

	return oFan.IsNull() ? TYPE_NULL : TYPE_HU;

}


void Table_XiaoHua::DoMultiOperate(_uint8 ChairID, int iOpType, _uint8 bCard)
{	
	if (iOpType & TYPE_HU && m_bUserOperate[ChairID] & TYPE_HU && iOpType != TYPE_PASS)
	{
		for (_uint8 c = NextChair(ChairID); c != ChairID; c = NextChair(c))
		{
			if (m_buzhang[c].buzhang.Find(bCard))
				m_buzhang[c].buzhang.RemoveLast();
		}
	}
	
	if (m_HuWeiDa)
	{
		log_debug("into this !!");
		return DoMultiOperate_HuWeiDa(ChairID, iOpType, bCard);
	}
	Table::DoMultiOperate(ChairID, iOpType, bCard);

}


void Table_XiaoHua::DoMultiOperate_HuWeiDa(_uint8 ChairID, int iOpType, _uint8 bCard)
{
	m_bMultiDoneOpNum++;
	m_bMultiUserDoneOp[ChairID] = iOpType;
	m_bMultiUserDoneOpCard[ChairID] = bCard;
	log_info("uid:%d chaird:%d, curoptype:%x, optype:%x, now done OpNum:%d, totalOpnum:%d ",
		GetPlayerUid(ChairID), ChairID, m_bUserOperate[ChairID], iOpType, m_bMultiDoneOpNum, m_bMultiOpUserNum);

	if (iOpType == TYPE_HU)
	{
		m_bMulitFirstOperate = true;
		m_bMulitHuOpeCount++;
		if (m_conf.m_bYiPaoDuoXiang)
		{
			UserOperateHu(ChairID, m_bCurCard);
			DoMultiForceHu(ChairID, m_bCurCard);
		}
		m_HuWeiDaCard = INVALID_MJ_CARD;  
		m_HuWeiDaOp = TYPE_NULL;
	}
	else if (iOpType == TYPE_PASS)
	{
		if (m_bUserOperate[ChairID] & TYPE_HU)   //胡牌玩家选择了过牌
		{
			m_bMulitHuOpeCount++;
			log_info(" user uid:%d pass seat:%d, optype: %x ", GetPlayerUid(ChairID), ChairID, m_bUserOperate[ChairID]);
			m_PassHuPlayer.Add(ChairID);
		}
		UserOperatePass(ChairID, iOpType, bCard);
	}
	if (m_bMultiDoneOpNum >= m_bMultiOpUserNum)
	{
		if (m_bMulitFirstOperate)
		{
			if (!m_conf.m_bYiPaoDuoXiang)
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
			GameEnd();
		}
		else
		{
			Table::DoSimpleOperate(m_curGangPlayerSid, m_HuWeiDaOp, m_HuWeiDaCard);
			if (m_HuWeiDaOp & TYPE_BU_ZHANG)
				GangHouClear();
		}
		init_multi_data();
	}
}


void Table_XiaoHua::ConfInit_Remote(const Json::Value& val)
{
	Table::ConfInit_Remote(val);

	m_conf.m_bZimoHu       = true;	
	m_conf.m_bSupportChi    = false ;
	m_conf.m_bSupportZhuaniao = false;
	m_conf.m_bQiangGangHu = false;
	m_conf.m_bQiangAnGang = false;
	m_conf.m_bQiangZhiGang = false;
	m_conf.m_bYiPaoDuoXiang = false;
	_xiaohua_conf.is_3fen_di = true;
	_xiaohua_conf.man_tian_hua = true;

	m_CardPool.Set( CARD_POOL_NO_FLOWER, tid);
	m_CardPool.GameId = XIAOHUA_MAJIAGN_GAME_ID ;


	if (json_check_play_type_id(val))
	{
		MJ_SUB_ID subid;

		json_get_play_type_id(val, 3011, subid);
		if (subid.Find(3011001)) 
		{
			
			_xiaohua_conf.is_3fen_di = true;
			g_xiaohua_ft2fn.base = 3;
			log_debug(" 3fen");

		}
		if (subid.Find(3011002))
		{
			
			_xiaohua_conf.is_3fen_di = false;
			g_xiaohua_ft2fn.base = 5;
			log_debug(" 5fen ");
		}
		m_room_config += subid; 


		int  iCounts = json_get_play_type_id(val, 3010, subid);
		log_debug("icount %d  subid [%d]", iCounts, subid.Size());
		if (subid.Find(3010001))
		{
			m_GAME_PLAYER = 4;
			m_room_config.Add(subid[0]);
			log_debug("444444444 ");
		}
		else if (subid.Find(3010002))
		{
			m_GAME_PLAYER = 3;
			m_room_config.Add(subid[0]);
			log_debug("333");
		}
		else
		{
			m_GAME_PLAYER = 2;
			m_room_config.Add(3010003); 
			log_debug("2222222222222");
		}
		iCounts = json_get_play_type_id(val, 3012 , subid);
		if (subid.Find(3012001))
		{
			m_room_config.Add( 3012001 );
			_xiaohua_conf.man_tian_hua = true; 
		}
		else
		{
			m_room_config.Add( 3012002 ); 
			_xiaohua_conf.man_tian_hua = false;
		}
	}

}

bool Table_XiaoHua::EstimateUserRespond(_uint8 bOutChairID, _uint8 bOutCard)
{
	if (bOutCard< 0x30  &&  _xiaohua_conf.man_tian_hua )
	{
		Table::EstimateUserRespond(bOutChairID, bOutCard);
	}

	if (_xiaohua_conf.man_tian_hua == false) 
	{
				
		if (bOutCard< 0x30)
			Table::EstimateUserRespond(bOutChairID, bOutCard); 
		else if (bOutCard < 0x40 &&  bOutCard != (0x30+1+m_bBanker)) 
			Table::EstimateUserRespond(bOutChairID, bOutCard); 
		
	}
	return true;

}

void Table_XiaoHua::OnUserOutCard(_uint8 ChairID, _uint8 bOutCard)
{
	if (ChairID >= 0 && ChairID < m_GAME_PLAYER && m_buzhang[ChairID].bao_ting  && m_BaoTingPos[ChairID] < 0)
	{
		m_BaoTingPos[ChairID] = PlayerAllOutCard[ChairID].Size();
	}

	Table::OnUserOutCard(ChairID, bOutCard);	
	//log_debug("mantina hua %d   boutcard [%x ] ", _xiaohua_conf.man_tian_hua, bOutCard);
	if (bOutCard >= 0x30 && _xiaohua_conf.man_tian_hua )
	{		
		OnUserGrapCard(ChairID ,false ) ;
	}
	else if (_xiaohua_conf.man_tian_hua ==  false &&   (bOutCard > 0x40||   (bOutCard == (0x30+1+m_bBanker))  ))
	{
		OnUserGrapCard(ChairID,  false );
	}

}


//这个过程 就是。。打出去一张。  再加一张，，看能不能形成听牌 


int Table_XiaoHua::IsUserAterPengCanBaoTing(_uint8  ChairID )
{
	const  HandCards* pHc = &m_HandCard[ChairID];
	vector<int> all_card_vec;
	int remove_pos = 0;
	int all = pHc->ChangeableCardsLen; 
	if (all > 14)
		return  0;

	
	while (remove_pos < all  )
	{
		all_card_vec.clear();
		for (int i = 0; i < pHc->ChangeableCardsLen; i++)
		{
			_uint8  ct = pHc->ChangeableCards[i];
			if (ct > 0x30 && _xiaohua_conf.man_tian_hua )
				return 0;

			if (i != remove_pos)			
				all_card_vec.push_back( ct );			
		}
		
		int ret = hu_if_add( all_card_vec, 34 );	 	
		if (ret > 0)
		{
			int size = all_card_vec.size();
			for (int i = 0; i< size ; i++)
			{
				int ct = all_card_vec[i];
				if (_xiaohua_conf.man_tian_hua  && ct > 0x30)
					ret = 0;
				else if (!_xiaohua_conf.man_tian_hua && (ct > 0x40 || ct == 0x30 + 1 + m_bBanker))
					ret = 0;
			}
		}
		remove_pos++;
		if (ret > 0)
		{
			return 1;
		}	
	}
		
	return 0;

}


int Table_XiaoHua::CheckPlaySlefGrpCardCanOperateCode(_uint8 ChairID, _uint8 bCard, MJ_major& major, int& fan_num)
{
	int iOpType = 0;
	//log_debug("grp card [%x]   mantihua %d" , bCard  , _xiaohua_conf.man_tian_hua ); 


	if ((bCard < 0x30 || bCard == INVALID_MJ_CARD) && _xiaohua_conf.man_tian_hua)  // 人生。处处是坑。 。
		iOpType = Table::CheckPlaySlefGrpCardCanOperateCode(ChairID, bCard, major, fan_num);

	if (_xiaohua_conf.man_tian_hua == false )
		iOpType = Table::CheckPlaySlefGrpCardCanOperateCode(ChairID, bCard, major, fan_num);
	
	
	
	if (m_buzhang[ChairID].bao_ting == false)
	{   
		iOpType &= ~( TYPE_HU) ;  //  未报听。。不能胡牌。 
		//log_debug("nobaotint   cancel hu   uid[%d] bcard[%x]" ,  GetPlayerUid(ChairID) , bCard  );
	}
	if (   m_buzhang[ChairID].bao_ting == false ) 
	{
		HandCards* pHc = &m_HandCard[ChairID];
		queue<int> my_queue;		
		//log_debug("all card [%s]", CatHandCard(m_HandCard[ChairID], ChairID)); 
		bool  is_add = false;
		if (pHc->ChangeableCardsLen < 14)
		{
			m_GameLogic.AddCard(pHc, bCard);                    // add  -------11111111111111111111
			is_add = true;
		}
		

		for (int i = 0; i < pHc->ChangeableCardsLen;i++)		
			my_queue.push(pHc->ChangeableCards[i]);
				
		//log_debug("len [%d] hucard [%x]  iOpType[%x]", pHc->ChangeableCardsLen, bCard, iOpType );
		while (!my_queue.empty())
		{
			_uint8  a = my_queue.front();
			m_GameLogic.ChuPai(pHc, a);                    // add-2 -------------222222222222222
			if ( Table::check_player_can_bao_ting(ChairID) ) 
			{
				iOpType |= TYPE_BAO_TING;
			}
			my_queue.pop();
			m_GameLogic.AddCard(pHc, a);	            //  add -2 --------------222222222222222222      
			if (iOpType & TYPE_BAO_TING)
				break;
		}
		if (is_add)
			m_GameLogic.ChuPai(pHc, bCard);                /// end add22 ----------111111111111111111111111111111

		if ( (iOpType & TYPE_BAO_TING ) ==0 )
		{ 
			//log_debug("len [%d]", pHc->ChangeableCardsLen);
			
			m_GameLogic.AddCard(pHc, bCard);          //  add-3--------------------333333333333333333333333
			if ( Special_Hu_If_Remove_One_Next_Add(ChairID , bCard ))
			{
				iOpType |= TYPE_BAO_TING;
				log_debug("特殊牌型可以胡牌 ");
			}
			m_GameLogic.ChuPai(pHc, bCard);           // end--3--------------------333333333333333333333333333
		}		
		//log_debug("len [%d]", pHc->ChangeableCardsLen);
	}


	if (m_buzhang[ChairID].bao_ting)  // 报听 
	{
		iOpType &= ~(TYPE_CHI | TYPE_PENG);				
		if ( iOpType & TYPE_GANG ) 
		{
			m_BaoTingGrapCardCannotGangCard.clear();
			for (int i = 0; i < major.m_GetGangInfo.Size(); i++)
			{
				int ct = major.m_GetGangInfo[i].cbCard;
				if ( !IsBaoTingGrapCardCanGang(ChairID, ct) ) 
				{
					m_BaoTingGrapCardCannotGangCard.push_back(ct);
					log_debug("uid[%d] " ,ct);
				}				
			}
			
			if (major.m_GetGangInfo.Size() == (int)m_BaoTingGrapCardCannotGangCard.size())
			{
				iOpType &= ~(TYPE_GANG);
				log_debug("baoting quxiao gang uid[%d]", GetPlayerUid(ChairID));
			}			
		}
		iOpType &= ~( TYPE_CHI | TYPE_PENG | TYPE_BU_ZHANG );   // 报听修改
		log_debug("  取消吃碰     [%x]", iOpType);

	}
	
	return iOpType;
}



int Table_XiaoHua::CheckOutCardOtherPlayerCanOpCode(_uint8 ChairID, _uint8 bCard, MJ_major& major, int& fan_num)
{
	int iOpType = 0; 

	if (bCard < 0x30 && _xiaohua_conf.man_tian_hua)
		iOpType = Table::CheckOutCardOtherPlayerCanOpCode(ChairID, bCard, major, fan_num);

	if (_xiaohua_conf.man_tian_hua == false )
		iOpType = Table::CheckOutCardOtherPlayerCanOpCode(ChairID, bCard, major, fan_num);

	if (bCard>= 0x30 && _xiaohua_conf.man_tian_hua )
	{
		iOpType &=  ~(TYPE_PENG | TYPE_GANG | TYPE_HU | TYPE_CHI );
	}
	else
	{  
		iOpType &= ~(TYPE_CHI ); 
	}
	
	//log_debug("ioptype [%x]   mantianhua[%d]" , iOpType , _xiaohua_conf.man_tian_hua );

	if (m_buzhang[ChairID].bao_ting && bCard <0x30 && _xiaohua_conf.man_tian_hua  )
	{
		iOpType &=  ~(TYPE_PENG | TYPE_BU_ZHANG ); 		
		if(  (iOpType  & TYPE_GANG)  &&  (!IsNoPreOutCardJudgeGang(ChairID , bCard)) ) 			
		{
			iOpType &= ~(TYPE_GANG);
			log_debug("cancel gang ! uid[%d]" , GetPlayerUid(ChairID));
		}		
	} 
	else if (m_buzhang[ChairID].bao_ting  && _xiaohua_conf.man_tian_hua == false)
	{
		iOpType &= ~(TYPE_PENG | TYPE_BU_ZHANG);
		if ((iOpType  & TYPE_GANG) && (!IsNoPreOutCardJudgeGang(ChairID, bCard)))
		{
			iOpType &= ~(TYPE_GANG);
			log_debug("cancel gang ! uid[%d]", GetPlayerUid(ChairID));
		}

	}
	else
	{
		iOpType &= ~(TYPE_CHI | TYPE_HU | TYPE_BU_ZHANG ) ;

	}
	log_debug("ioptype [%x]", iOpType);

	return iOpType;
}


void Table_XiaoHua::OnUserGrapCard(_uint8 ChairID, bool bGetLast)
{
	m_BaoTingGrapCardCannotGangCard.clear();
	Table::OnUserGrapCard(ChairID, bGetLast);


}



void Table_XiaoHua::UserAfterOperate(_uint8 ChairID, int iOpType, _uint8 bCard)
{
	m_bCurChairID = ChairID;
	// log_info(" uid:%d chairid:%d operate: %x card: %x ", GetPlayerUid(ChairID), ChairID, iOpType, bCard);

	m_bUserOperate[ChairID] = TYPE_NULL;
	MJ_opinfo gang;
	m_bUserOperate[ChairID] = GetGangInfo(m_bCurChairID, gang);
	if (m_buzhang[ChairID].bao_ting == false)
	{
		int ret  = (IsUserAterPengCanBaoTing(ChairID) || Special_Hu_If_Remove_One_Next_Add(ChairID));				  		
		if (ret > 0)
			m_bUserOperate[ChairID] |= TYPE_BAO_TING;
		log_debug("op [%x] ret[%d] " , m_bUserOperate[ChairID] , ret );
	}
	
	//    没哟 刚。也没有报听 
	if (gang.Size() == 0   && (m_bUserOperate[ChairID] & TYPE_BAO_TING) == 0 ) // 
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


void Table_XiaoHua::UserOperatePeng(_uint8 ChairID, _uint8 bCard)
{
	
	if (m_curGangPlayerSid != INVALID_CHAIRID)
	{
		GangHouClear();
	}
	Table::UserOperatePeng(ChairID, bCard);

}


void Table_XiaoHua::Proto_Game_OpNotifyAfterPengChi(_uint8 ChairID, _uint8 CurChairID, const MJ_opinfo& gang, proto::game::AckOpNotify& stNotify)
{
	stNotify.Clear();
	stNotify.set_chairid(ChairID);
	log_debug("chairid [%d]  CurChairID[%d]  op[%x] " , ChairID , CurChairID  , m_bUserOperate[ChairID] );

	if (ChairID == CurChairID)
	{	
		//非打牌玩家
		int iOpType = m_bUserOperate[ChairID];
		log_debug("ioptype [%x]" , iOpType );
		if (m_bUserOperate[ChairID] & TYPE_GANG)
		{
			// m_bUserOperate[ChairID] |= TYPE_BU_ZHANG;
			iOpType = m_bUserOperate[ChairID] & ~TYPE_GANG;  

		}
		if (iOpType  & TYPE_LISTEN ) // add  
		{
			iOpType |= TYPE_BAO_TING;
		}
		stNotify.set_card(0);
		stNotify.set_optype(iOpType);
	}
}




void Table_XiaoHua::DoSimpleOperate(_uint8 ChairID, int iOpType, _uint8 bCard)
{
	
	if ( (iOpType == TYPE_BAO_TING)   &&   (m_bUserOperate[ChairID]  &  TYPE_BAO_TING) )
	{
		m_buzhang[ChairID].bao_ting = true;
		m_buzhang[ChairID].is_first_bao_ting = false;
		log_debug(" bao Ting uid[%d] ", GetPlayerUid(ChairID ) );
		proto::game::hunanmj::serMsgBaojiao  pbAll;
		pbAll.set_uid(GetPlayerUid(ChairID));
		pbAll.set_chairid(ChairID);
		broadcast(SERVER_BAO_TING_SUCC, pbAll);
		m_nRecvBaoTingPlayerMap[ChairID] = TYPE_BAO_TING;
		m_bUserOperate[ChairID] &= (~TYPE_GANG);  

		return;

	}
	if ( (iOpType  & TYPE_PASS)  &&  (m_bUserOperate[ChairID] & TYPE_BAO_TING))
	{
		m_nRecvBaoTingPlayerMap[ChairID] = TYPE_PASS;
	}


	m_nGameRunState = GAME_QISHOU_BAOTING_OVER;  // 报听结束 !!
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

			if (m_bCurChairID == INVALID_CHAIRID)
			{
				OnUserGrapCard(m_bBanker , false );
			}
			else
			{
				m_bCurChairID = NextChair(m_bCurChairID);
				OnUserGrapCard(m_bCurChairID , false );
			}
		}
		break;
	case TYPE_HU:
		{
			int ret = UserOperateHu(ChairID, 0xFF);
			if(ret>=0)
				GameEnd();							
		}
		
		break;
	default:
		break;
	}
}

bool Table_XiaoHua::is_gang_shang_kai_hua(_uint8  chairID, bool zimo)
{
	if (chairID >= 0 && chairID < m_GAME_PLAYER)
	{
		//log_debug("ChairID[%d]   m_bAddFanFlag[%d]", chairID, m_bAddFanFlag[chairID]);
		if (m_bAddFanFlag[chairID] == 1 && zimo  )
		{
			return true;
		}
	}
	return false;
}


bool Table_XiaoHua::IsQuanqiuren(const HandCards& pHc)
{
	return m_GameLogic.IsDanDiao(pHc);
	
}


int Table_XiaoHua::AddFanFunXiaoHu(_uint8 ChairID, const HandCards& pHc,
									MJ_fan_buffer& oFan, const MJ_win_pattern& oPattern, bool  IsAddJiangDui , _uint8  add_last  )
{	
	//、、加番牌型。  2杠上开花 1
	// 2条 碰碰胡   胡牌的牌型。  可以碰/杠/补，此时任意对子都可以做将，不再限定258
	bool duiduihu = false;
	bool qinyise = false;
	bool IsBigHu = oFan.Size() > 0;
	
	//int nFan = 3;
	bool isPengpenghu = false;
	if (  m_GameLogic.IsPengPengHu(&pHc, oPattern.kan, oPattern.jiang))
	{
		duiduihu = true;
		oFan.Add(MJ_FAN_TYPE_PENGPENGHU);
		
		IsBigHu = true;
		isPengpenghu = true;
		log_debug(" 5.碰碰胡   10-15");
	}
	// 3 清一色。任意牌将  胡牌的牌型。  可以碰/杠/补，此时任意对子都可以做将，不再限定258 
	// 可以组成 3*n  + 2  的 组合 。

	if (m_GameLogic.IsQingYiSe(&pHc))   //  
	{
		qinyise = true;
		oFan.Add(MJ_FAN_TYPE_QINGYISE);		
		IsBigHu = true;
		log_info("yise " );
	}
	if (duiduihu  && qinyise)
	{
		oFan.Add(MJ_FAN_DUI_DUI_QING);
		oFan.Remove(MJ_FAN_TYPE_PENGPENGHU);
		oFan.Remove(MJ_FAN_TYPE_QINGYISE);
		IsBigHu = true;
		log_info(" duiduiqing 30-50 ");
	}
	//  4  海底捞月。 加番牌型。 15.海底捞月 海底捞月 19 
	if (oFan.Size() &&  (m_GameLogic.hai_di_lao_yue(m_CardPool.IsEmpty() , 0, ChairID == m_bCurChairID, 0)) )
	{
		oFan.Add(MJ_FAN_TYPE_HAIDILAOYUE);
		//nFan += 8;
		IsBigHu = true;
		log_info(" 15.haidilaoyu 3-5");
	}
	//丫子
	if ((!oFan.Find(MJ_FAN_TYPE_QIDUI)) && (check_ya_zi(ChairID, m_bCurCard, oPattern)))
	{
		log_debug(" 丫子  6-9 ");
		oFan.Add(MJ_FAN_YA_ZI);
	}

	//、、加番牌型。 全求人 。。11.单吊
	if ((!oFan.Find(MJ_FAN_YA_ZI)) && dandiao(oPattern, m_bCurCard , ChairID )) 
	{
		oFan.Add( MJ_FAN_TYPE_QUANQIUREN);		
		IsBigHu = true;
		log_info(" dandiao  6-9  ");
	}
	//13.推倒胡 13 
	if ( duiduihu== false  &&  (TuiDaoHu(ChairID  ,oPattern, m_bCurCard)) )
	{
		oFan.Add(MJ_FAN_TUIDAO_HU);
		log_debug("tuidaohu  ");
	}

	//、、加番牌型。  2杠上开花
	if (oFan.Size() && is_gang_shang_kai_hua(ChairID, m_bCurChairID == ChairID))
	{
		oFan.Add(MJ_FAN_TYPE_GANGSHANGHUA);
		IsBigHu = true;		
		log_info(" gagnhua ");
	}

	
	//对倒胡 12
	 
	if( isPengpenghu == false 
		&&  (!( oFan.Find(MJ_FAN_TYPE_QIDUI)|| oFan.Find(MJ_FAN_HAO_HUA_QI_XIAO_DUI) ))
		&& DuiDaoHu(&pHc , oPattern , m_bCurCard ))
	{
		oFan.Add(MJ_FAN_DUI_DAO_HU);
		log_debug("9.对倒胡  7-10 ");
	}
	//火叉  16
	if ( HuoCha(oPattern, (HandCards*)&pHc))
	{
		oFan.Add(MJ_FAN_HUO_CHA);
		log_debug("16.火叉  3-5 ");
	}
	//无花 
	if (NoHuaGuo(ChairID))
	{
		oFan.Add(MJ_FAN_WU_HUA_GUO);
		log_debug("11 wuhuaguo   10-20 "); 
	}
	int sixi = SixiCount(ChairID);
	if (sixi > 0)
	{
		oFan.Add(MJ_FAN_SI_XI_XIAO_HUA_MAJIANG);
		log_debug("12.四喜  6-10 ");
	}
	// 大决  修改。  没有七对 。 豪华七大对，不与大绝叠
	//if (  (!(oFan.Find(MJ_FAN_HAO_HUA_QI_XIAO_DUI) || oFan.Find(MJ_FAN_TYPE_QIDUI)))  &&  dajue(ChairID, m_bCurCard) )
	if ((!(oFan.Find(MJ_FAN_HAO_HUA_QI_XIAO_DUI))) && dajue(ChairID, m_bCurCard))
	{
		oFan.Add(MJ_FAN_DA_JUE);		
		log_debug("10.大绝   10-20 ");
	}

	if ( m_GameLogic.yi_tiao_long(&pHc)>=0  && yi_tiao_long(oPattern) )  //yi_tiao_long(oPattern))
	{
		oFan.Add(MJ_FAN_YI_TIAO_LONG);
		log_debug("1--9");   
	} 
	if ( zao_zao_hu(isPengpenghu, &pHc))
	{
		oFan.Add(MJ_FAN_ZAOZAOHU);
		oFan.Remove(MJ_FAN_TYPE_PENGPENGHU); 
		oFan.Remove(MJ_FAN_DUI_DAO_HU);
		log_debug("zaozaohu  30-50");
	}
	//21.幺幺胡 不和碰碰胡叠加 
	if (  Is119(ChairID))
	{
		oFan.Add(MJ_FAN_ALL_19);
		oFan.Remove(  MJ_FAN_TYPE_PENGPENGHU); 
		log_debug("119");
	}
	// 混一色：胡牌时牌型只包含风和某一种花色 
	//log_debug("mantianhua %d  fen[%x]", _xiaohua_conf.man_tian_hua, 0x30 + m_bBanker + 1);

	if (_xiaohua_conf.man_tian_hua == false && HunyiSe(ChairID))
	{
		oFan.Add(MJ_FAN_TYPE_HUNYISE);
		log_debug("hun yise  !!");
	}
	//log_debug("over ");
	return 0; 
}


int Table_XiaoHua::Check_Hu_Pattern(_uint8 ChairID, const HandCards& pHc, MJ_fan_buffer& oFan, const MJ_win_pattern& oPattern)
{
		
	//  门清要是自己莫得。 
	//log_debug("charid[%d] sid[%d] uid[%d] m_bCurCard[%x]", m_bCurChairID, ChairID, GetPlayerUid(ChairID), m_bCurCard);
	bool IsAddJiangDui = false;
	//log_debug("all %s   _xiaohua_conf.is_3fen_di [%d] allhua[%d]", CatPattern(oPattern), _xiaohua_conf.is_3fen_di, _xiaohua_conf.man_tian_hua  );

	
	oFan.Add(MJ_FAN_TYPE_JIANGDUI);
	IsAddJiangDui = true;
	//log_debug(" 平胡  3-5");	
	int ret =  AddFanFunXiaoHu(ChairID, pHc, oFan, oPattern , IsAddJiangDui ,0 ); 

	if ( ret  && IsAddJiangDui)
	{
		oFan.Remove(MJ_FAN_TYPE_JIANGDUI);		
		//log_debug("bighu nfan ");
	}
	//log_debug("ofan[%d]  ", oFan.Size()  );	
	return  0;

}


//摸到的四花个数   打出去的牌。。是花牌的。

int Table_XiaoHua::SixiCount(_uint8  sid  )
{
	int sixi_count = 0;  
	int cur_count = 0; 
	int  all_card_count = PlayerAllOutCard[sid].Size();
	for (int i = 0; i < all_card_count;i++)
	{
		cur_count = 1;
		_uint8  pre_card = PlayerAllOutCard[sid][i];
		if (pre_card < 0x30 && _xiaohua_conf.man_tian_hua)
			continue;  
		if (_xiaohua_conf.man_tian_hua ==false  &&  (!( pre_card> 0x40 || ( pre_card == 0x30+1 +m_bBanker) )) ) 
			continue; 

		for (int j = i + 1; j < all_card_count; j++)
		{
			_uint8  next_card = PlayerAllOutCard[sid][j];
			if (next_card == pre_card)
			{
				cur_count++;
				if (cur_count >=4)
				{
					sixi_count++;
					break;
				}
			}
		}
	}
	return sixi_count;
}


int Table_XiaoHua::playerOutHuaCount(int sid ) 
{
	int hua_count = 0;	
	int  all =  PlayerAllOutCard[sid].Size();
	for (int i = 0; i < all; i++)
	{
		_uint8  ct = PlayerAllOutCard[sid][i];
		if (ct >  0x30 && _xiaohua_conf.man_tian_hua)
			hua_count++;
		else if (_xiaohua_conf.man_tian_hua == false &&  (ct > 0x40 ||  (ct == (0x30 + 1 + m_bBanker))) )
			hua_count++; 
	}
	return   hua_count;

}

//m_bGetCardCount
//15.无花

int Table_XiaoHua::NoHuaGuo( _uint8   sid )
{		 
	if (sid < 0 || sid >= m_GAME_PLAYER)
		return  0;

	int all = PlayerAllOutCard[sid].Size();
	int pos = m_BaoTingPos[sid]; 
	for (int i = 0; i < all;i++)
	{
		if (i>= pos)
			break; 

		_uint8  ct = PlayerAllOutCard[sid][i];
		if (ct >= 0x30 && _xiaohua_conf.man_tian_hua )
			return 0 ;
		if (_xiaohua_conf.man_tian_hua == false  && (ct > 0x40 || ct == 0x30 + 1 + m_bBanker))
			return 0; 
	}
	return true;  
}



int Table_XiaoHua::Is119(_uint8  sid)
{
	if (sid < 0 || sid>=  m_GAME_PLAYER)
	{
		log_error("sid errr %d" ,sid );
		return 0;
	}
	const  HandCards* pHc = &m_HandCard[sid];
	for (int i = 0; i < pHc->ChangeableCardsLen; i++)
	{
		int ct = pHc->ChangeableCards[i];
		int face = get_face(ct);
		if (face>1 && face < 9)
			return false;
	}

	for (int i = 0; i < pHc->FixedCardsLen; i++)
	{
		int ct = pHc->FixedCards[i].CardData;
		int face = get_face(ct);
		if (face>1 && face < 9)
			return false;
	}
	return 1;

}

//10.大绝

int Table_XiaoHua::dajue(_uint8   sid , _uint8  huPai )
{
	if (sid < 0 || sid>= m_GAME_PLAYER)
	{
		log_error("sid errr %d", sid);
		return 0;
	}
	const HandCards* pc = &m_HandCard[sid];
	if (m_GameLogic.cur_card_count(  pc , huPai) >= 4)
		return  1;

	for (int i = 0; i < m_GAME_PLAYER;i++)
	{		
		const  HandCards  *pHc = &m_HandCard[i];
		if (m_GameLogic.dajue(pHc , huPai))
		{
			return 1;
		}
	}
	return  0;
}



int Table_XiaoHua::DuiDaoHu(const  HandCards   *phc  , const MJ_win_pattern  &pattern , _uint8  hu_pai  )
{
	int  in_hand = 0;
	for (int i = 0; i < phc->ChangeableCardsLen;i++)
	{
		if (hu_pai == phc->ChangeableCards[i])
			in_hand++;
	}
	if (in_hand < 3)
		return 0;
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 1;j++)
		{
			if (pattern.kan[i][0] != pattern.kan[i][1])
				break;
			if (hu_pai == pattern.kan[i][0])
			{
				return  1;
			}
		}
	}
	return 0;
}

int Table_XiaoHua::dandiao(const MJ_win_pattern  &pattern , _uint8 hupai  , _uint8  ChairID)
{
	if (ChairID < 0 || ChairID >= m_GAME_PLAYER)
		return 0;
	if (hupai <= 0 || hupai> 0x43)
		return  0; 

		
	HandCards* pHc = &m_HandCard[ChairID];
	vector<int> all_han_card;
	for (int i = 0; i < pHc->ChangeableCardsLen; i++)
	{
		all_han_card.push_back(pHc->ChangeableCards[i]);
	}	
	for (vector<int>::iterator it = all_han_card.begin(); it != all_han_card.end();it++)
	{
		if (*it == hupai)
		{
			all_han_card.erase(it);
			break; 
		}
	}
	vector<int> tingvec;
	

	get_all_ting_card_vec( tingvec ,  all_han_card );  
	if (tingvec.size() > 1)
	{
		
		return 0;
	}
	

	int  all_di_pai[] =
	{
		0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09,//万	
		0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19,//条	
		0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29,//筒	
		0x31, 0x32, 0x33, 0x34, 0x41, 0x42, 0x43,
	}; 
	int canhuqidui = 0;
	bool is_add = false;
	bool is_del_suc = false;
	if (m_GameLogic.ChuPai(pHc, hupai)) 
		is_del_suc = true;
	
	
	for (int i = 0; i < 34; i++)
	{
		is_add = false;
		canhuqidui = 0;
		if (pHc->ChangeableCardsLen< 13)
		{
			m_GameLogic.AddCard(pHc, all_di_pai[i]); 
			is_add = true;
		}
		
		if (m_GameLogic.is_qi_dui(pHc))
			canhuqidui = 1;
		if (canhuqidui == 1)
		{
			vector<int>::iterator it = find(tingvec.begin(), tingvec.end(), all_di_pai[i]);
			if (it == tingvec.end())
			{
				tingvec.push_back(all_di_pai[i]);				
			}
		}
				
		if (is_add)
			m_GameLogic.ChuPai(pHc, all_di_pai[i]);
		if (  tingvec.size()> 1)
		{
			if (is_del_suc)
				m_GameLogic.AddCard(pHc, hupai);
			return 0;
		}						
	}
	if (is_del_suc)
		m_GameLogic.AddCard(pHc, hupai);
	

	return 1;


}

//听牌时胡一副顺子的首尾，胡牌的张数大于等于2张，3花，例如23条胡14条，45678万胡369万，3花（5花）

int Table_XiaoHua::TuiDaoHu(_uint8 ChairID ,  const  MJ_win_pattern &pattern, _uint8   hupai)
{	
	if (ChairID < 0 || ChairID >= m_GAME_PLAYER)
		return 0;

	const HandCards* pHc = &m_HandCard[ChairID];
	vector<int> all_han_card;
	log_debug("uid[%d] sid[%d] hu_card[%x] len[%d]", GetPlayerUid(ChairID), ChairID, hupai , pHc->ChangeableCardsLen );
	for (int i = 0; i < pHc->ChangeableCardsLen; i++)
	{
		all_han_card.push_back(pHc->ChangeableCards[i]);
	}
	for (vector<int>::iterator it = all_han_card.begin(); it != all_han_card.end();) 
	{
		if (*it == hupai)
		{
			it =  all_han_card.erase(it);
			break;
		}
		else
			it++;
	}
	int hu_count = CanhuCount(all_han_card ,34 );
	if (hu_count <= 1)
		return false;

	for (int i = 0; i < 4;i++)
	{
		for (int j = 0; j < 2 ;j++)
		{
			_uint8 pre_card = pattern.kan[i][j];
			_uint8 next_card = pattern.kan[i][j + 1];
			_uint8 last_card = pattern.kan[i][j+2]; 
			if (pre_card == next_card)
				break;
			else
			{
				if (pre_card == hupai || last_card== hupai)
				{
					return 1;
				}
				if (next_card == hupai)
				{
					return 0;
				}
			}

		}
	}
	return  0;

}

//丫子

int Table_XiaoHua::check_ya_zi(_uint8  ChairID, _uint8  hu_card , const  MJ_win_pattern &pattern )
{	
	if (ChairID <0 || ChairID >= m_GAME_PLAYER)
	{
		log_error("error %d " ,ChairID );
		return 0;
	}		
	const  HandCards* pHc = &m_HandCard[ChairID];
	vector<int> all_han_card;
	log_debug("uid[%d] sid[%d] hu_card[%x]  len[%d]" , GetPlayerUid(ChairID) , ChairID , hu_card , pHc->ChangeableCardsLen );
	int all = pHc->ChangeableCardsLen;
	if (all> 14)
	{
		all = 14;
		log_error("error ");
	}
	
	for (int i = 0; i <all ;i++)
	{		
		all_han_card.push_back( pHc->ChangeableCards[i]);
	}
	for (vector<int>::iterator it = all_han_card.begin(); it != all_han_card.end();)
	{
		if (*it == hu_card)
		{
			it = all_han_card.erase(it);
			break;
		}
		else
			it++;
	}
	//log_debug("into this ");
	int hu_count  = CanhuCount(all_han_card ,34 );
	if (hu_count > 1)
		return 0;
	//log_debug("into this222 ");
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 1; j++)
		{
			if (pattern.kan[i][j] == pattern.kan[i][j + 1])
			{
				continue;
			}
			int  first_a = pattern.kan[i][j];
			int  sec_b = pattern.kan[i][j+1];
			int  third_c = pattern.kan[i][j+2];
			if (hu_card == sec_b)
			{
				return 1;
			}
			else if (CardNum(first_a) == 1 &&  hu_card == third_c) 
			{
				log_debug(" a b c [%d] [%d] [%d]", first_a, sec_b, third_c);
				return 1;
			}
			else if (CardNum(third_c) == 9 && hu_card == first_a)
			{
				log_debug(" a b c [%d] [%d] [%d]", first_a, sec_b, third_c);
				return 1;
			}
		}
	}
	//log_debug("into this222 ");

	return  0;

}

int Table_XiaoHua::Calc_Hu(_uint8 ChairID, _uint8 bCard, int Score[GAME_PLAYER])
{
	
	return Table::Calc_Hu(ChairID, bCard, Score);	


}


void Table_XiaoHua::Calc_Hu_Player_Base_Score(_uint8 ChairID, int iBaseScore[GAME_PLAYER])
{
	if (ChairID < 0 || ChairID>= m_GAME_PLAYER)
	{
		log_error("error  !");
		return;
	}
	if (!m_bIsHu[ChairID])
		return;
	

	MJ_fan_buffer OutFan;
	int iScore = 3; 
	if (_xiaohua_conf.is_3fen_di == false)
		iScore = 5; 

	int out_hua = playerOutHuaCount(ChairID);
	iScore += out_hua; 

	int sixi = SixiCount(ChairID);
	m_sixi = sixi;
	if (sixi > 1)
	{
		iScore += (sixi - 1) * 6;
		log_debug("iscore [%d] sixi[%d]", iScore , sixi );  // 四喜的个数 。
	}
	const  HandCards* pHc = &m_HandCard[ChairID];
	vector<int> all_han_card;
	for (int i = 0; i < pHc->ChangeableCardsLen;i++)	
		all_han_card.push_back(pHc->ChangeableCards[i]);
	

	m_kezi  = GetMaxKeCount(all_han_card);
	iScore += m_kezi;
	log_debug("kezi base[%d]", m_kezi);

	bool IsBigHu = false;
	log_debug("iscore [%d]  outhua[%d] m_kezi[%d]", iScore, out_hua, m_kezi );

	int iFanCount = 0;
	log_debug("m_curGangPlayerSid[%x]  m_BaseScore[%d] ", m_curGangPlayerSid, m_BaseScore);

	if (m_curGangPlayerSid == INVALID_CHAIRID)
	{
		if (m_ft2fn->FanFilter(m_oFan[ChairID], OutFan))   //  大胡过滤 
		{
			iFanCount = OutFan.Size();
			IsBigHu = true;
			log_debug("ifancount%d", iFanCount);
		}
		else
		{
			iFanCount = m_oFan[ChairID].Size();     // 小胡一个 
		}
		log_debug("sid[%d] ifancount[%d]", ChairID, iFanCount);
	}
	else
	{
		if (m_ft2fn->FanFilter( m_MultiFan[ChairID], OutFan))
		{
			iFanCount = OutFan.Size();
			IsBigHu = true;
			log_debug("ifancount %d", iFanCount);
		}
		else
			iFanCount = m_MultiFan[ChairID].Size();

		log_debug("sid[%d] ifancount[%d]", ChairID, iFanCount);

	}

	for (int i = 0; i < m_stHuScore[ChairID].stHu.Size(); i++)
	{
		if (IsBigHu)
			iScore += m_stHuScore[ChairID].stHu[i].iFanNum;
		log_debug("iscore [%d]  chaidid[%d]   ifanum[%d]", iScore, ChairID, m_stHuScore[ChairID].stHu[i].iFanNum );
	}
	for (int i = 0; i < m_stGang[ChairID].Gang.Size(); i++)
	{
		const tagGangScore& t = m_stGang[ChairID].Gang[i];
		if (t.iType  & TYPE_ZHIGANG || t.iType & TYPE_WANGANG )
			iScore += 2;
		else
			iScore += 4;
		log_debug("add gang  score[%d] %x \n" , iScore ,  t.iType);
	}

	
	log_debug("socre [%d]  m_BaseScore ", iScore, m_BaseScore ); 	
	iBaseScore[ChairID] = iScore * 1   ;
	for (_uint8 c = NextChair(ChairID); c != ChairID; c = NextChair(c))
	{
		int tmpScore = iScore * 1   ;
		iBaseScore[c] = tmpScore;		
		log_debug("c[%d] iBaseScore[%d]", c, iBaseScore[c]);
	}
}

void Table_XiaoHua::TurnBanker()
{
	MJ_user_buffer UserID;
	int iCount = MultiHu(UserID);
	if (iCount == 0)
		m_bBanker = m_bBanker;
	else if (iCount == 1 && UserID[0] == m_bBanker)
		m_bBanker = m_bBanker;
	else
		m_bBanker = NextChair(m_bBanker);


}

bool Table_XiaoHua::Is7Dui(_uint8 ChairID, HandCards& pHc, MJ_fan_buffer& oFan)
{
	if (   m_GameLogic.is_qi_dui(&pHc))
	{
		oFan.Add(MJ_FAN_TYPE_QIDUI);
		log_debug("into table  qidui ");
		
		return true;
		
	}
	return  false;

}


//火叉  胡牌时，手上有同花色的三连对，加上比三连对略大且同花色的一副顺子

int   Table_XiaoHua::HuoCha(const MJ_win_pattern  &pattern  ,  HandCards *pHc  )
{
	if( pHc->ChangeableCardsLen <  11 )
		return 0; 
	map<int, int> card_map; 
	int all_array[3] = { 0 };
	for (int i = 0; i < pHc->ChangeableCardsLen; i++)
	{
		int ct = pHc->ChangeableCards[i];	
		int suit = get_suits(ct);
		card_map[ct]++;
		if (suit >= 0 && suit < 3 )
		{			
			all_array[suit]++;
		}		
	}
	bool  IsMoreThan9 = false;
	for (int i = 0; i < 3; i++)
	{
		if(all_array[i] < 9 )
			continue; 
		IsMoreThan9 = true;
	}
	if (card_map.size() < 6 || IsMoreThan9 == false )
		return 0;

	map<int, int>::iterator it = card_map.begin();
	for (; it != card_map.end();++it)
	{
		if (it->second < 2) continue;  		
		int next1 = it->first + 1;    //  33  44  55  6 7 8  
		int next2 = it->first + 2;
		if (card_map[next2] >= 2 && card_map[next1] >= 2)
		{
			
			for (int i = 0; i < 4; i++)
			{
				for (int j = 0; j < 1; j++)
				{
					int    a = pattern.kan[i][j];
					int    b = pattern.kan[i][j + 1];
					int    c = pattern.kan[i][j + 2];
					if (a == b && a != 0)
						break;
					if (a== 0 || b == 0 || a== 0xFF || a> 0x30  )
						break; 
					if (a == next2 + 1)
					{
						int  arr[14] = { 0 };
						for (int k = 0; k < pHc->ChangeableCardsLen && k<= 13 ; k++)
							arr[k] = pHc->ChangeableCards[k];
						int arr_remove[14] = { 0 };
						arr_remove[0] = it->first;
						arr_remove[1] = it->first;
						arr_remove[2] = it->first + 1;
						arr_remove[3] = it->first + 1;
						arr_remove[4] = it->first + 2;
						arr_remove[5] = it->first + 2;
						arr_remove[6] = a;
						arr_remove[7] = a + 1;
						arr_remove[8] = a + 2;
						for (int r = 0; r <=  8; r++)  // == 8 
						{
							for (int t = 0; t < 14; t++)
							{
								if (arr_remove[r] == arr[t] && arr[t] > 0  && arr[t] <= 0x43 )
								{
									arr[t] = 0;
									break;
								}
							}
						} 
						int   CheckHuArr[14] = { 0 };
						int all = 0;
						for (int point  = 0; point  < 14 && all<= 13  ; point++)
						{
							if (arr[point] > 0  )
							{
								CheckHuArr[all] = arr[point];								
								all++;
							}
							if (arr[point] > 0x30 && _xiaohua_conf.man_tian_hua )
							{
								log_error("error ");
								return 0;
							}

						}
						if (all % 3 != 2)
							return 0;
						//log_debug("a[%d] == next2[%d] all [%d]", a, next2 , all );
						return  ( pHc->ChangeableCardsLen == 9 || le_check_hu(CheckHuArr, all) );
						//if (ret)
							//return 1;
					}
					else if (c==   (it->first -1)  &&   get_face(it->first)> 3  )
					{

						int  arr[14] = { 0 };
						for (int k = 0; k < pHc->ChangeableCardsLen && k<= 13 ; k++)
							arr[k] = pHc->ChangeableCards[k];
						int arr_remove[14] = { 0 };
						arr_remove[0] = it->first;
						arr_remove[1] = it->first;
						arr_remove[2] = it->first + 1;
						arr_remove[3] = it->first + 1;
						arr_remove[4] = it->first + 2;
						arr_remove[5] = it->first + 2;
						arr_remove[6] = it->first - 1;
						arr_remove[7] = it->first - 2;
						arr_remove[8] = it->first - 3; 
						for (int r = 0; r <= 8; r++)  // == 8 
						{
							for (int t = 0; t < 14; t++)
							{
								if (t<=13 && arr_remove[r] == arr[t] && arr[t] > 0 && arr[t] <= 0x43)
								{
									arr[t] = 0;
									break;
								}
							}
						}
						int   CheckHuArr[14] = { 0 };
						int all = 0;
						for (int point = 0; point < 14 && all <= 13; point++)
						{
							if (arr[point] > 0)
								CheckHuArr[all++] = arr[point];
							
							if (arr[point] > 0x30 && _xiaohua_conf.man_tian_hua)
							{
								log_error("error ");
								return 0;
							}

						}
						if (all % 3 != 2)
							return 0; 						
						return  (pHc->ChangeableCardsLen == 9 || le_check_hu(CheckHuArr, all) );						
					}
				}
			}
		}					
	}
	return  0;
}


void Table_XiaoHua::UserOperateGang(_uint8 ChairID, int iOpType, _uint8 bCard)
{
	
	if (m_curGangPlayerSid != INVALID_CHAIRID)
	{
		switch (iOpType)
		{
		case TYPE_WANGANG:
		case TYPE_ANGANG:
			m_stUserOutCard[ChairID].RemoveLast(bCard);
			break;
		default:
			break; 
		}
		GangHouClear();
	}
	Table::UserOperateGang(ChairID, iOpType, bCard); 
}

void Table_XiaoHua::Proto_Game_OperateGang(_uint8 ChairID, _uint8 bCard, int iOpType, int iGangScore[GAME_PLAYER], proto::game::AckBcOpResult& stOpresult)
{
	Table::Proto_Game_OperateGang(ChairID, bCard, iOpType, iGangScore, stOpresult);

}


void Table_XiaoHua::Proto_GameEndInfo_GangInfo(_uint8 ChairID, proto::game::AckGameEnd& stGameend)
{
	if (!m_bIsHu[ChairID])
		return;
	int zhigang = 0; 
	int angang = 0;
	proto::game::tagGameEndInfo* pGameendinfo = stGameend.mutable_endinfo(ChairID);
	for (int i = 0; i < m_stGang[ChairID].Gang.Size(); i++)
	{
		const tagGangScore& t = m_stGang[ChairID].Gang[i];
		proto::game::tagGameEndGang *pGang = pGameendinfo->add_gang();
		pGang->set_gangcard(t.Card);
		log_debug("gang-card[%x] type[%x] " , t.Card , t.iType  );		
		pGang->set_gangtype(t.iType);
		if (t.iType & TYPE_ANGANG)
			angang++;
		else
			zhigang++;
		
	}
	for (int i = 0; i < m_GAME_PLAYER; i++)//添加点杠信息
	{
		for (int j = 0; j < m_stGang[i].Gang.Size(); j++)
		{
			const tagGangScore& t = m_stGang[i].Gang[j];
			if (t.iType == TYPE_ZHIGANG && t.ChairID == ChairID)
			{
				if (!m_buzhang[i].buzhang.Find(t.Card))
				{
					proto::game::tagGameEndGang* pGang = pGameendinfo->add_gang();
					pGang->set_gangtype(64);//点杠
					pGang->set_gangcard(t.Card);
				}
			}
		}
	}
	stGameend.set_zhigangscore(zhigang * 2); 
	stGameend.set_angangscore(angang * 4);  

	stGameend.set_gangscore(m_stGang[ChairID].iTotalScore);
	log_debug("%d  zhigang[%d] angang[%d]", m_stGang[ChairID].iTotalScore , zhigang , angang  );

}



void Table_XiaoHua::Proto_Game_OpNotify(_uint8 ChairID, _uint8 CurChairID, _uint8 bCard, proto::game::AckOpNotify& stNotify)
{
	stNotify.Clear();
	stNotify.set_chairid(ChairID);

	if (ChairID == CurChairID)
	{
		//非打牌玩家
		int iOpType = m_bUserOperate[ChairID];
		if (m_bUserOperate[ChairID] & TYPE_GANG)
		{
			//m_bUserOperate[ChairID] |= TYPE_BU_ZHANG;

			iOpType = m_bUserOperate[ChairID];

			proto::game::hunanmj::tagBuZhang* bu = stNotify.mutable_bu();
			proto::game::hunanmj::tagBuinfo* buinfo = bu->add_bu();
			buinfo->set_card(bCard);
			//buinfo->set_state(TYPE_BU_ZHANG);
			buinfo->set_state( TYPE_GANG );
			if ( IsListening(ChairID, bCard))
			{
				proto::game::tagGangInfo* ganginfo = stNotify.add_gang();
				ganginfo->set_card(bCard);
				ganginfo->set_state(TYPE_ZHIGANG);

			}
			if (stNotify.gang_size() == 0)
			{
				//iOpType &= ~TYPE_GANG;
			}
		}
		stNotify.set_card(bCard);
		stNotify.set_optype(iOpType);
		log_debug("uid[%d] op[%x]" ,  GetPlayerUid(ChairID) , iOpType);

	}
	stNotify.set_outcardid(m_bOutCardChairID );

}


bool Table_XiaoHua::CheckUserOutCard(_uint8 ChairID, _uint8 cbCard)
{
	
	if (m_curGangPlayerSid != INVALID_CHAIRID)
	{
		log_debug("curGangSid[%d] ", m_curGangPlayerSid);
		return true;
	}
	if (!Table::CheckUserOutCard(ChairID, cbCard))
		return false;

	//if (m_buzhang[ChairID].bao_ting)
	//{
	//	if (m_bCurCard == cbCard)
	//	{
	//		return true;
	//	}
	//	log_error("baoting");
	//	return false;
	//}
	
	return true;
}



void Table_XiaoHua::Proto_Game_UserGetCard(_uint8 ChairID, _uint8 MoChairID, _uint8 bCard, proto::game::AckUserGetCard& stUsergetcard)
{

	Table::Proto_Game_UserGetCard(ChairID, MoChairID, bCard, stUsergetcard);
	if (ChairID == MoChairID)
	{
		//摸牌玩家
		if (m_bUserOperate[ChairID] & TYPE_GANG)
		{
			stUsergetcard.clear_ganginfo();
			//m_bUserOperate[ChairID] |= TYPE_BU_ZHANG;
			int iOpType = m_bUserOperate[ChairID];

			MJ_opinfo stGang;
			m_GameLogic.GetGangInfo(&m_HandCard[MoChairID], stGang);
			proto::game::hunanmj::tagBuZhang* bu = stUsergetcard.mutable_bu();
			for (int i = 0; i < stGang.Size(); i++)
			{
				if (m_buzhang[ChairID].bao_ting && stGang[i].cbCard != bCard)
					continue;
				proto::game::hunanmj::tagBuinfo* buinfo = bu->add_bu();
				buinfo->set_card(stGang[i].cbCard);				
				buinfo->set_state(TYPE_GANG);  //buinfo->set_state(TYPE_BU_ZHANG);
				if (1)  /// IsListening(ChairID, stGang[i].cbCard) )
				{
					proto::game::tagGangInfo* ganginfo = stUsergetcard.add_ganginfo();
					ganginfo->set_card(stGang[i].cbCard);
					ganginfo->set_state(stGang[i].nState);
				}
			}
			if (stUsergetcard.ganginfo_size() == 0)
			{
				//iOpType &= ~(TYPE_GANG);
			}
			else
			{
				//iOpType &= ~(TYPE_GANG);
				for (int i = 0; i < stUsergetcard.ganginfo_size(); i++)
				{
					proto::game::tagGangInfo* t = stUsergetcard.mutable_ganginfo(i);
					iOpType |= t->state();
				}
			}
			stUsergetcard.set_optype(iOpType);
		}
	}

}

void Table_XiaoHua::Proto_GameEndInfo(const MJ_user_buffer& UserID, proto::game::AckGameEnd& stGameend)
{

	//Table_changsha::Proto_GameEndInfo(UserID, stGameend);
	MJ_user_buffer   huUserId;
	MultiHu(  huUserId ); 
	
	log_debug("hucount [%d]" , huUserId.Size() ); 
	for (int i = 0; i < UserID.Size(); i++)
	{
		log_debug("huid [%d]" , UserID[i] );
	}

	Table::Proto_GameEndInfo(UserID, stGameend);

	tagHuNanGameEnd* end = stGameend.mutable_hunangameendinfo();
	end->set_leftcardnum(m_CardPool.Count());

	FanInfo* info[GAME_PLAYER] = { 0 };
	for (_uint8 i = 0; i < m_GAME_PLAYER; i++)
	{
		info[i] = end->add_faninfo();
	}	

	MJ_fan_buffer tmpFan[GAME_PLAYER] = m_oFan;
	int tmpHuFanNums[GAME_PLAYER] = { 0 };
	if (m_curGangPlayerSid != INVALID_CHAIRID)
	{
		for (int i = 0; i < m_GAME_PLAYER; i++)
		{
			proto::game::tagGameEndInfo* endinfo = stGameend.mutable_endinfo(i);
			endinfo->set_hucard(0);
		}
		for (int i = 0; i < m_GAME_PLAYER; i++)
		{
			tmpFan[i] = m_MultiFan[i];
		}
		for (int i = 0; i < m_GAME_PLAYER; i++)
		{
			if (m_bIsHu[i])
			{
				proto::game::hunanmj::tagMultiHuInfo* multi_hu_info = end->add_huinfo();
				multi_hu_info->set_chairid(i);
				for (int j = 0; j < m_stHuScore[i].stHu.Size(); j++)
				{
					multi_hu_info->add_hucard(m_stHuScore[i].stHu[j].bHuCard);
				}
			}
		}
		Calc_Score(tmpHuFanNums, m_MultiHufanNums);
	}
	else
	{
		Calc_Score(tmpHuFanNums, m_HuFanNums);
	}

	if ( huUserId.Size() == 1)
	{
		//一个人胡
		_uint8 win_seat = huUserId[0];
		log_debug("hucount[%lu] win_id[%d]", m_stHuScore[win_seat].stHu.Size(), win_seat);
		if (m_stHuScore[win_seat].stHu.Size() == 0)
		{
			log_error("Error!!");
			return;
		}

		if (m_stHuScore[win_seat].stHu[0].bType == HU_ZIMO)
		{
			//自摸胡
			info[win_seat]->set_chairid(win_seat);
			info[win_seat]->set_specialtype(HU_ZIMO); 
			// add
			bool  bao_ting_bei_zimo = false;
			for (_uint8 j = NextChair(win_seat); j != win_seat; j = NextChair(j))
			{
				if (m_buzhang[j].bao_ting  && m_buzhang[j].is_first_bao_ting)
				{
					bao_ting_bei_zimo = true;
					break;
				}
			}
			// end add 

			if (m_stHuScore[win_seat].stHu.Size() == 1)
			{
				for (int i = 0; i < tmpFan[win_seat].Size(); i++)
				{
					AddFanInfo* pfan = info[win_seat]->add_addfan();
					pfan->set_addtype(tmpFan[win_seat][i]);
					pfan->set_addnum(m_ft2fn->GetFan(tmpFan[win_seat][i]));

					log_debug("uid[%d] sid[%d]  win_fan[%d] ", GetPlayerUid(win_seat), win_seat, tmpFan[win_seat][i]);
				}
			}
			else
			{
				MJ_fan_buffer  outFan;
				m_ft2fn->FanFilter(tmpFan[win_seat], outFan);
				for (int i = 0; i < outFan.Size(); i++)
				{
					AddFanInfo* pfan = info[win_seat]->add_addfan();
					pfan->set_addtype(outFan[i]);
					pfan->set_addnum(m_ft2fn->GetFan(outFan[i]));
					log_debug("uid[%d] sid[%d]  win_fan[%d] ", GetPlayerUid(win_seat), win_seat, outFan[i]);

				}
				int add_c = m_stHuScore[win_seat].stHu.Size() - 1;
				for (int i = 0; i < add_c; i++)
				{
					AddFanInfo* pfan = info[win_seat]->add_addfan();
					pfan->set_addtype(MJ_FAN_TYPE_GANGSHANGHUA);
					pfan->set_addnum(8);
					log_debug("addc[%d]", add_c);
				}
			}

			for (_uint8 c = NextChair(win_seat); c != win_seat; c = NextChair(c))
			{
				info[c]->set_chairid(c);
				info[c]->set_specialtype(HU_BEIZIMO);
				info[c]->set_fannum(0);
				if (m_buzhang[c].bao_ting  && m_buzhang[c].is_first_bao_ting)
				{
					AddFanInfo *pOtherFan = info[c]->add_addfan();
					pOtherFan->set_addtype(MF_FAN_BAOTING_BEI_ZI_MO);
					pOtherFan->set_addnum(8);
					log_debug("uid[%d] sid[%d]", GetPlayerUid(c), c);
				}
			}
			info[win_seat]->set_fannum(tmpHuFanNums[win_seat]);
		}
		else if (m_stHuScore[win_seat].stHu.Size() && m_stHuScore[win_seat].stHu[0].bType == HU_PAOHU)
		{
			//点炮			
			info[win_seat]->set_chairid(win_seat);
			info[win_seat]->set_specialtype(HU_PAOHU);
			_uint8 iPaoID = m_stHuScore[win_seat].stHu[0].bPasHuChairID;
			log_debug("ipaoid [%d]", iPaoID);

			if (m_stHuScore[win_seat].stHu.Size() == 1)
			{
				for (int i = 0; i < tmpFan[win_seat].Size(); i++)
				{
					AddFanInfo* pfan = info[win_seat]->add_addfan();
					pfan->set_addtype(tmpFan[win_seat][i]);
					log_debug("out type[%d]", tmpFan[win_seat][i]);
					pfan->set_addnum(m_ft2fn->GetFan(tmpFan[win_seat][i]));
				}
			}
			else
			{
				MJ_fan_buffer  outFan;
				m_ft2fn->FanFilter(tmpFan[win_seat], outFan);
				for (int i = 0; i < outFan.Size(); i++)
				{
					AddFanInfo* pfan = info[win_seat]->add_addfan();
					pfan->set_addtype(outFan[i]);
					pfan->set_addnum(m_ft2fn->GetFan(outFan[i]));
					log_debug("out fan [%d]", outFan[i]);
				}
				int add_c = m_stHuScore[win_seat].stHu.Size() - 1;
				for (int i = 0; i < add_c; i++)
				{
					AddFanInfo* pfan = info[win_seat]->add_addfan();
					pfan->set_addtype(MJ_FAN_TYPE_GANGHOUPAO);
					pfan->set_addnum(8);
					log_debug("add type [%d]", MJ_FAN_TYPE_GANGHOUPAO);
				}

			}


			for (_uint8 c = NextChair(win_seat); c != win_seat; c = NextChair(c))
			{
				info[c]->set_chairid(c);
				info[c]->set_specialtype(HU_INVALID);
				info[c]->set_fannum(0);
			}
			info[win_seat]->set_fannum(tmpHuFanNums[win_seat]);
			info[m_stHuScore[win_seat].stHu[0].bPasHuChairID]->set_specialtype(HU_DIANPAO);
		}
	}
	else if (huUserId.Size() > 1)
	{
		//一炮多响
		for (_uint8 i = 0; i < m_GAME_PLAYER; i++)
		{
			if (huUserId.Find(i))
			{
				info[i]->set_chairid(i);
				info[i]->set_specialtype(HU_PAOHU);
				log_debug("m_stHuScore[i].stHu.Size() [%lu] uid[%d]", m_stHuScore[i].stHu.Size(), GetPlayerUid(i));
				if (m_stHuScore[i].stHu.Size() == 0)
				{
					log_error("error!!");
					continue;
				}
				_uint8 iPaoID = m_stHuScore[i].stHu[0].bPasHuChairID;
				log_debug("ipaoid [%d]", iPaoID);
				if (m_stHuScore[i].stHu.Size() == 1)
				{
					for (int j = 0; j < tmpFan[i].Size(); j++)
					{
						AddFanInfo* pfan = info[i]->add_addfan();
						pfan->set_addtype(tmpFan[i][j]);
						pfan->set_addnum(m_ft2fn->GetFan(tmpFan[i][j]));
						log_debug("add fan type[%d]", tmpFan[i][j]);
					}
				}
				else
				{
					MJ_fan_buffer  outfan;
					m_ft2fn->FanFilter(tmpFan[i], outfan);
					for (int j = 0; j < outfan.Size(); j++)
					{
						AddFanInfo* pfan = info[i]->add_addfan();
						pfan->set_addtype(outfan[j]);
						pfan->set_addnum(m_ft2fn->GetFan(outfan[j]));
						log_debug("add fan type[%d]", outfan[j]);
					}
					int add_c = m_stHuScore[i].stHu.Size() - 1;
					log_debug("addc [%d]", add_c);
					for (int st = 0; st < add_c; st++)
					{
						AddFanInfo* pfan = info[i]->add_addfan();
						pfan->set_addtype(MJ_FAN_TYPE_GANGHOUPAO);
						pfan->set_addnum(8);
						log_debug("add fan 8");
					}
				}
				info[i]->set_fannum(tmpHuFanNums[i]);
			}
			else
			{
				info[i]->set_chairid(i);
				info[i]->set_specialtype(HU_INVALID);
				info[i]->set_fannum(0);

			}

		}
		if (m_stHuScore[huUserId[0]].stHu.Size() == 0)
		{
			log_error("error !!!");
			return;
		}
		_uint8 dianpaoID = m_stHuScore[huUserId[0]].stHu[0].bPasHuChairID;
		info[dianpaoID]->set_specialtype(HU_DIANPAO);
	}
	else
	{
		for (int i = 0; i < m_GAME_PLAYER; i++)
		{
			info[i]->set_chairid(i);
			info[i]->set_specialtype(HU_INVALID);
			info[i]->set_fannum(0);
		}
	}
	// end 


	proto::game::XiaoHua  *pHua = stGameend.mutable_hua();
	pHua->set_hua_fen(1);  
	int huId = -1;
	for (int i = 0; i < m_GAME_PLAYER;i++)
	{
		if (m_bIsHu[i])
		{
			pHua->set_chairid(i);
			huId = i; 
			break;
		}
	}
	if (huId != -1)
	{
		int all = PlayerAllOutCard[huId].Size();
		for (int i = 0; i < all; i++)
		{
			_uint8  ct = PlayerAllOutCard[huId][i];
			//log_debug("%x", ct);
			if (  _xiaohua_conf.man_tian_hua  && ct > 0x30 )
			{
				pHua->add_hua_arr(ct);		
				//log_debug("%x", ct);  
			}
			else if( _xiaohua_conf.man_tian_hua == false && ( ct > 0x40 || ct ==  (0x30+1 + m_bBanker) ))
			{
				pHua->add_hua_arr(ct); 
				//log_debug("%x", ct);
			}
		}  
	}
	pHua->set_base(_xiaohua_conf.is_3fen_di == true? 3: 5  );
	pHua->set_si_xi(m_sixi);
	pHua->set_kezi(m_kezi);

	log_debug("play_times[%d]  kezi [%d] sixi[%d] base[%d]  ",  m_nPlayingTimes ,   m_kezi, m_sixi, _xiaohua_conf.is_3fen_di);
	
	
}

//通天


int Table_XiaoHua::yi_tiao_long(const  MJ_win_pattern  &pattern   )
{
	int my_color[10] = { 0 };
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			if (pattern.kan[i][0] == pattern.kan[i][1])
			{
				break;
			}
			if (pattern.kan[i][j] == 0)
				continue;
			int cur_color = CardNum(pattern.kan[i][j]);
			if(cur_color>0 && cur_color< 10)
				my_color[cur_color] = 1; 
		}
	}
	int all = 0;
	for (int i = 0; i < 10; i++)
	{
		all += (my_color[i] == 1);
	}
	return all == 9;
}


int Table_XiaoHua::zao_zao_hu(bool  pengpenghu, const  HandCards *pHc)
{
	if (pengpenghu == false)
		return 0;
	
	for (int i = 0; i < pHc->FixedCardsLen; i++)
	{
		if( pHc->FixedCards[i].state & TYPE_PENG)
		{
			return false;
		} 
	}
	return true;
	
}

int  Table_XiaoHua::IsHandHasHuaCard(const  HandCards  *pHc)
{
	
	for (int i = 0; i < pHc->ChangeableCardsLen;i++)
	{
		if (pHc->ChangeableCards[i] > 0x30)
			return 1;
	}
	return 0;
}

int Table_XiaoHua::IsBaoTingGrapCardCanGang(_uint8  chairID, _uint8  add_card)
{
	if (add_card > 0x30 && _xiaohua_conf.man_tian_hua )
		return 0; 
	if (chairID< 0 || chairID>= m_GAME_PLAYER )
	{
		log_error("error !");
		return 0;
	}
	const  HandCards* pHc = &m_HandCard[chairID];
	vector<int> all_hand_vec;
	for (int i = 0; i < pHc->ChangeableCardsLen; i++)
	{
		if (pHc->ChangeableCards[i] > 0x30 && _xiaohua_conf.man_tian_hua )
			return 0;

		if (add_card != pHc->ChangeableCards[i])
			all_hand_vec.push_back(pHc->ChangeableCards[i]);
	}
	vector<int> ting;
	get_all_ting_card_vec(ting, all_hand_vec);

	return ting.size() >  0; 
}

//非上家出牌，或者是自己摸牌。 判断是不是可以杠

int Table_XiaoHua::IsNoPreOutCardJudgeGang(_uint8 chairID, _uint8  add_card)
{
	if (add_card > 0x30 && _xiaohua_conf.man_tian_hua )
		return 0; 
	if (chairID< 0 || chairID>= m_GAME_PLAYER)
		return 0;

	const  HandCards* pHc = &m_HandCard[chairID];	
	vector<int> all_hand_vec;
	for (int i = 0; i < pHc->ChangeableCardsLen; i++)
	{
		if (pHc->ChangeableCards[i] > 0x30 && _xiaohua_conf.man_tian_hua )
			return 0;

		if (add_card != pHc->ChangeableCards[i])					
			all_hand_vec.push_back(pHc->ChangeableCards[i]);		
	}
	

	vector<int> TingVec;
	get_all_ting_card_vec(TingVec, all_hand_vec);
	

	return TingVec.size() > 0; 

}
//


void Table_XiaoHua::Score_GameEndSucc(HuScoreSet& score, proto::game::AckGameEnd& stGameend)
{
	Table::Score_GameEndSucc(score, stGameend);   // 非流局

	for (int i = 0; i < m_GAME_PLAYER; i++)
	{
		//只有在每局的最后才将m_HandCard的杠牌状态转换成 TYPE_BU_ZHANG
		for (int j = 0; j < m_HandCard[i].FixedCardsLen; j++)
		{
			if (m_HandCard[i].FixedCards[j].state & TYPE_GANG &&
				m_buzhang[i].buzhang.Find(m_HandCard[i].FixedCards[j].CardData))
			{
				m_HandCard[i].FixedCards[j].state = TYPE_BU_ZHANG;
			}
		}
	}

}

void Table_XiaoHua::Calc_HuScore(_uint8 ChairID, int iBaseScore[GAME_PLAYER], int iScore[GAME_PLAYER])
{
	if (m_bIsHu[ChairID])
	{
		if (m_bCurChairID == ChairID)  //自摸
		{
			for (_uint8 c = NextChair(ChairID); c != ChairID; c = NextChair(c))
			{
				iScore[ChairID] += iBaseScore[c];
				iScore[c] -= iBaseScore[c];
			}
		}
		else     // 放炮 
		{
			if (m_stHuScore[ChairID].stHu.Size() == 0)  // modify  
			{
				log_error("error");
				return;
			}
			_uint8 iPaoID = m_stHuScore[ChairID].stHu[0].bPasHuChairID;
			iScore[ChairID] += iBaseScore[iPaoID];
			iScore[iPaoID] -= iBaseScore[iPaoID];

		}
		log_info("uid:%d chairid:%d, base score [%d,%d,%d,%d] ",
			GetPlayerUid(ChairID), ChairID, iScore[0], iScore[1], iScore[2], iScore[3]);
	}
}


int Table_XiaoHua::Calc_Fan(_uint8 ChairID, MJ_fan_buffer& oFan)
{
	int iTotalFan = 0;
	MJ_fan_buffer oFilterFan;
	if (m_ft2fn->FanFilter(oFan, oFilterFan))
	{
		//计算番之前把如果过滤番，如果有大胡就过滤所有小胡
		for (int i = 0; i < oFilterFan.Size(); i++)
		{
			iTotalFan += m_ft2fn->GetFan(oFilterFan[i]);
			log_debug("iTotal [%d]  GetFan[%d]", iTotalFan, m_ft2fn->GetFan(oFilterFan[i]));
		}
		oFan = oFilterFan;
		return iTotalFan;
	}

	for (int i = 0; i < oFan.Size(); i++)
	{
		iTotalFan += m_ft2fn->GetFan(oFan[i]);
	}
	return iTotalFan;
}

bool Table_XiaoHua::IsListening(_uint8 ChairID, _uint8 bRemoveCard)
{
	HandCards tempHandCard = m_HandCard[ChairID];
	if ( bRemoveCard != INVALID_MJ_CARD)
	{
		MJ_opinfo opinfo;
		m_GameLogic.GetGangInfo(&tempHandCard, opinfo);
		for (int i = 0; i < opinfo.Size(); i++)
		{
			if (opinfo[i].cbCard == bRemoveCard)
			{
				m_GameLogic.ExecuteGang(&tempHandCard, ChairID, bRemoveCard, opinfo[i].nState);
			}
		}
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
	bool is_add = false;
	bool is_true = false;
	for (_uint8 i = 0; i < _relativity.Size(); ++i)
	{
		is_add = false; 
		is_true = false;
		if (tempHandCard.ChangeableCardsLen < 14)
		{
			m_GameLogic.AddCard(&tempHandCard, _relativity[i]);
			is_add = true;
		}
		else
			log_error("len [%d]" ,  tempHandCard.ChangeableCardsLen );

		MJ_fan_buffer fan; 
		if (Check_Hu(ChairID, tempHandCard, fan, _relativity[i]) == TYPE_HU)
		{
			log_info(" uid:%d chairdid:%d Remove:%x ", GetPlayerUid(ChairID), ChairID, bRemoveCard);			
			is_true = true;  			
		}
		if (is_add )
			m_GameLogic.ChuPai(&tempHandCard, _relativity[i]);

		if (is_true)
			return  true;
		
	}
	return false;
}



void Table_XiaoHua::Proto_GameScene(_uint8 ChairID, Player* player, proto::game::AckGameScene& stGameScene)
{
	Table::Proto_GameScene(ChairID, player, stGameScene);
	AckCustomiedInfo* info = stGameScene.mutable_hunanscene();
	if (info != NULL)
	{
		info->set_gametype(proto::game::hunanmj::HUNAN_CHANGSHA);
		info->set_ishavepiao(0);
		info->set_jiangma(proto::game::hunanmj::ZHUANIAO);//抓鸟
		info->set_laizi(0);//没有癞子

		for (int i = 0; i < m_GAME_PLAYER; i++)
		{
			info->add_jinzhihuanpai((m_buzhang[i].bao_ting ) ? 1 : 0);
		}
		if (stGameScene.has_usercard())
		{
			proto::game::HandCards* pstGr = stGameScene.mutable_usercard();
			if (pstGr != NULL)
			{
				for (int j = 0; j < m_HandCard[ChairID].FixedCardsLen; j++)
				{
					proto::game::FixedCard* pstFix = pstGr->mutable_stfixedcards(j);
					if (pstFix == NULL)	continue;
					if ((pstFix->state() & TYPE_GANG) &&
						(m_buzhang[ChairID].buzhang.Find(pstFix->carddata())))
					{
						pstFix->set_state(TYPE_BU_ZHANG);
					}
				}
			}
		}
		for (int i = 0; i < m_GAME_PLAYER; i++)
		{
			//修改杠牌信息 为补张信息
			if (i >= stGameScene.groupcard_size())	break;
			proto::game::tagGroupCard* pstGr = stGameScene.mutable_groupcard(i);
			if (pstGr != NULL)
			{
				for (int j = 0; j < m_HandCard[i].FixedCardsLen; j++)
				{
					proto::game::FixedCard* pstFix = pstGr->mutable_fixedcard(j);
					if (pstFix == NULL)	continue;
					if ((pstFix->state() & TYPE_GANG) &&
						(m_buzhang[i].buzhang.Find(pstFix->carddata())))
					{
						pstFix->set_state(TYPE_BU_ZHANG);
					}
				}
			}
		}

	}

}


void Table_XiaoHua::Proto_GameScene_Operate(_uint8 ChairID, Player* player, proto::game::AckGameScene& stGameScene)
{
	Table::Proto_GameScene_Operate(ChairID, player, stGameScene);
	if (m_bMultiOpUserNum != 0 && m_bMultiUserDoneOp[ChairID] != TYPE_NULL)
	{
		log_error("%d %d %d", m_bMultiOpUserNum, m_bMultiUserDoneOp[ChairID], ChairID);
		return;
	}
	if (m_nCanBaoTingArr[ChairID] != 0 && m_nRecvBaoTingPlayerMap[ChairID] == 0)
	{
		stGameScene.set_optype(m_bUserOperate[ChairID] | TYPE_BAO_TING);
		log_debug("baoting %x", TYPE_BAO_TING);
	}
	else
	{
		if (m_nRecvBaoTingPlayerMap[ChairID] & TYPE_BAO_TING || (m_nRecvBaoTingPlayerMap[ChairID] & TYPE_PASS))   // 报听修改。  
			stGameScene.set_optype(m_bUserOperate[ChairID] & (~TYPE_BAO_TING));
		else
			stGameScene.set_optype(m_bUserOperate[ChairID]);

	}

	AckCustomiedInfo* info = stGameScene.mutable_hunanscene();
	if (info == NULL)	 return;

	if (m_HuWeiDaCard != INVALID_MJ_CARD && m_HuWeiDaOp != TYPE_NULL && m_curGangPlayerSid == ChairID)
	{
		stGameScene.clear_ganginfo();
		stGameScene.clear_optype();
	}
	else if (m_curGangPlayerSid != INVALID_CHAIRID)
	{
		stGameScene.clear_ganginfo();
		info->clear_bu();
		stGameScene.clear_optype();

		tagGang_Zhuapai* zhuapai = info->mutable_zhuapai();
		zhuapai->set_chairid(ChairID);
		zhuapai->set_leftcardnum(m_CardPool.Count());
		for (int i = 0; i < m_gang_hou_card.Size(); i++)
		{
			zhuapai->add_card(m_gang_hou_card[i]);

			if (m_bUserOperate[ChairID] != TYPE_NULL)
			{
				GangAckOpNotify* pNotify = zhuapai->add_opnotify();
				pNotify->set_chairid(ChairID);
				pNotify->set_card(m_gang_hou_card[i]);
				pNotify->set_optype(m_gang_hou_op[ChairID][i]);
				if (m_curGangPlayerSid == ChairID)
				{
					if (m_gang_hou_op[ChairID][i] & TYPE_GANG)
					{
						MJ_opinfo stGang;
						m_GameLogic.AddCard(&m_HandCard[ChairID], m_gang_hou_card[i]);
						m_GameLogic.GetGangInfo(&m_HandCard[ChairID], stGang);

						proto::game::hunanmj::tagBuZhang* bu = pNotify->mutable_bu();
						for (int j = 0; j < stGang.Size(); j++)
						{
							if (m_buzhang[ChairID].bao_ting && !m_gang_hou_card.Find(stGang[j].cbCard))
								continue;
							if (IsListening(ChairID, stGang[j].cbCard))
							{
								proto::game::hunanmj::tagBuinfo* buinfo = bu->add_bu();
								buinfo->set_card(stGang[j].cbCard);
								buinfo->set_state(TYPE_BU_ZHANG);

								proto::game::hunanmj::tagGangInfo* ganginfo = pNotify->add_gang();
								ganginfo->set_card(stGang[j].cbCard);
								ganginfo->set_state(stGang[j].nState);
							}
						}
						m_GameLogic.ChuPai(&m_HandCard[ChairID], m_gang_hou_card[i]);
						log_info(" player uid[%d] chairID[%d]  gangplayer[%d] card[%x] optype[%x]", GetPlayerUid(ChairID), ChairID, m_curGangPlayerSid, m_gang_hou_card[i], m_gang_hou_op[ChairID][i]);
					}
				}
				else if (m_gang_hou_op[ChairID][i] & (TYPE_GANG | TYPE_BU_ZHANG))
				{

					proto::game::hunanmj::tagBuZhang* bu = pNotify->mutable_bu();
					proto::game::hunanmj::tagBuinfo* buinfo = bu->add_bu();
					buinfo->set_card(m_gang_hou_card[i]);
					buinfo->set_state(TYPE_BU_ZHANG);

					proto::game::hunanmj::tagGangInfo* ganginfo = pNotify->add_gang();
					ganginfo->set_card(m_gang_hou_card[i]);
					ganginfo->set_state(TYPE_ZHIGANG);

					log_info(" player uid:%d chairID:%d gang_player:%d, card:%x, optype:%x ",
						GetPlayerUid(ChairID), ChairID, m_curGangPlayerSid,
						m_gang_hou_card[i], m_gang_hou_op[ChairID][i]);
				}

			}
		}
	}
	else
	{
		//增加补张信息
		log_debug("chonglian ..!!");
		 if( m_bUserOperate[ChairID] & TYPE_BU_ZHANG)
		{
			stGameScene.clear_ganginfo();

			MJ_opinfo stGang;
			if (m_bCurChairID == ChairID)
			{
				m_GameLogic.GetGangInfo(&m_HandCard[ChairID], stGang);
				proto::game::hunanmj::tagBuZhang* bu = info->mutable_bu();
				for (int i = 0; i < stGang.Size(); i++)
				{
					if (m_buzhang[ChairID].bao_ting && stGang[i].cbCard != m_bCurCard)
						continue;

					proto::game::hunanmj::tagBuinfo* buinfo = bu->add_bu();
					buinfo->set_card(stGang[i].cbCard);
					buinfo->set_state( TYPE_GANG );  // modify  

					if (IsListening(ChairID, stGang[i].cbCard))
					{
						proto::game::tagGangInfo* ganginfo = stGameScene.add_ganginfo();
						ganginfo->set_card(stGang[i].cbCard);
						ganginfo->set_state(stGang[i].nState);
					}
				}
			}
			else
			{
				proto::game::hunanmj::tagBuZhang* bu = info->mutable_bu();
				proto::game::hunanmj::tagBuinfo* buinfo = bu->add_bu();
				buinfo->set_card(m_bCurOutCard);
				buinfo->set_state( TYPE_GANG );

				if (IsListening(ChairID, m_bCurOutCard))
				{
					proto::game::tagGangInfo* ganginfo = stGameScene.add_ganginfo();
					ganginfo->set_card(m_bCurOutCard);
					ganginfo->set_state(TYPE_ZHIGANG);
				}
			}

			int iOpType = m_bUserOperate[ChairID];
			if (stGameScene.ganginfo_size() == 0)
			{
				iOpType &= ~(TYPE_GANG);
			}
			stGameScene.set_optype(iOpType);
		}
	}


}

void Table_XiaoHua::Proto_GameScene_lookon(_uint8 ChairID, Player* player, proto::game::AckGameScene& stGameScene)
{
	Table::Proto_GameScene_lookon(ChairID, player, stGameScene);
	AckCustomiedInfo* info = stGameScene.mutable_hunanscene();
	if (info != NULL)
	{
		info->set_gametype(proto::game::hunanmj::HUNAN_CHANGSHA);
		info->set_ishavepiao(0);
		info->set_jiangma(proto::game::hunanmj::ZHUANIAO);
		info->set_laizi(0);
	}
}

int  Table_XiaoHua::UserOperateHu(_uint8 ChairID, _uint8 bHuCard /* = 0xFF */)
{
	int  ret = Table::UserOperateHu(ChairID, bHuCard);
	return ret;

}

int Table_XiaoHua::HasFeng(_uint8  ChairId)
{
	if (ChairId < 0 || ChairId >= m_GAME_PLAYER)
		return 0;

	const  HandCards* pHc = &m_HandCard[ChairId];
	int   feng = 0x30 + m_bBanker + 1;
	for(int i = 0; i < pHc->ChangeableCardsLen;i++)
	{
		int ct = pHc->ChangeableCards[i];
		if (_xiaohua_conf.man_tian_hua)
		{
			if (ct> 0x30)
				return 1;
		}			
		else
		{
			if (ct> 0x40)
				return 1;
			if (ct > 0x30 && ct < 0x40 && ct == feng)
				return 1;
		}

	}


	return 0;
}

int Table_XiaoHua::HunyiSe(_uint8  ChairId)
{	 
	if (ChairId < 0 || ChairId >= m_GAME_PLAYER)
		return 0;

	const  HandCards* pHc = &m_HandCard[ChairId]; 
	int   color[5] = { 0 };
	int   feng = 0x30 + m_bBanker + 1; 
	int   has_feng = 0;

	for (int i = 0; i < pHc->ChangeableCardsLen;i++ )
	{
		int t = pHc->ChangeableCards[i]; 
		if (t > 0x40)
			return 0;

		if (t < 0x30)
		{
			int  suit = get_suits(t );
			if (suit >= 0 && suit < 3)
				color[suit]++;
		}
		else
		{
			has_feng = true;
			if (t < 0x40 && t == feng)
				return 0;
		} 		
	}
	for (int i = 0; i < pHc->FixedCardsLen;i++)
	{
		int t = pHc->FixedCards[i].CardData;
		if (t> 0x40)
			return 0; 
		if (t < 0x30)
		{
			int  suit = get_suits(t );
			if (suit >= 0 && suit < 3)
				color[suit]++;
		}
		else
		{
			has_feng = true;
			if (t < 0x40 && t == feng)
				return 0;
		}
	}
	int  all_s = 0;
	for (int i = 0; i < 3;i++)
	{
		if (color[i])
			all_s++;
	}
	if (all_s == 1 && has_feng)
		return 1;
	return 0;

}