#include "table_jilin.h"

#include <vector>
#include "../proto/proto.h"
#include "../zjh.h"
#include "../common/log.h"
#include "../game.h"
#include "../libnormalmahjong/mj_common2.h"
#include "../libnormalmahjong/game_logic.h"
#include "../table_helper.h"

using namespace  proto::game::hunanmj; 


//吉林
//////听牌玩家摸牌之前判断是否通宝，如果没通宝，判断是否换宝，如果换宝，换宝之后再判断是否通宝，然后重复循环，直到通宝胡牌本局结束，或者通宝没胡又无需换宝，再摸牌

/**********************************************************
  1:边胡，，夹胡，，同事存在， 2：0分 ， 补蛋不可以抢。  3：换宝 ，时间修改。 
  吉林：
  1. 换宝时机更改，改为听牌玩家摸牌前对宝后换宝
  2. 现在0分玩家补杠不能抢碰的bug
  3. 67889胡7 边胡被算作夹胡的bug
  4. 未报听玩家或报听当回合点炮，如果点的是7对改为不包赔 
***********************************************************/


Table_jilin::Table_jilin(int player)
{

	m_GAME_PLAYER = player;
	memset(&g_jilin_conf, 0, sizeof(jilin_conf));
	memset( &m_JilinGangInfo, 0, sizeof(JiGangInfo)); 
	for (int i = 0; i < m_GAME_PLAYER; i++)
	{
		m_JilinGangInfo[i].len = 0; 		
	}
	memset(m_nGiveUphu, 0, sizeof(m_nGiveUphu));
	m_ft2fn = &g_xiaohua_ft2fn;	 
	memset(m_QiangGangHu, 0, sizeof(m_QiangGangHu));
	
	memset( m_jilin_baoting, 0, sizeof(m_jilin_baoting));
	m_bIsTongBaoTime = false; 
	memset(m_fenResult, 0, sizeof(m_fenResult));
	m_gangID =  1; 
	memset(m_nqiangArr, -1, sizeof(m_nqiangArr));
	memset(m_TrueWinLost, 0, sizeof(m_TrueWinLost));
	m_bTongBao = false;
	memset(m_RecvBaipaiMap, 0, sizeof(m_RecvBaipaiMap));  //默认为0   摆牌选择 
	memset(m_nRecvBaoTingPlayerMap, 0, sizeof(m_nRecvBaoTingPlayerMap));
	memset(m_nCanBaoTingArr, 0, sizeof(m_nCanBaoTingArr));
	memset(m_nPlayerTongBao, 0, sizeof(m_nPlayerTongBao));
	memset(m_nXianBayi, 0, sizeof(m_nXianBayi));
	m_nRecvPlaerOp.clear();
	m_Jilinchi.clear();
	//memset(m_SiLou, 0, sizeof(m_SiLou));
	memset(m_NoQuickBaoTongBaoGrapPaiCount, -1, sizeof(m_NoQuickBaoTongBaoGrapPaiCount));
	memset(m_WinlostArr, 0, sizeof(m_WinlostArr));
	bIsCanQiangGangOrPeng = false;
	m_bHasTianhu = false;
	memset(m_outCardCountAfterBaoTing, 0, sizeof(m_outCardCountAfterBaoTing));
	m_nPlayerTingmap.clear();
	m_addlastCard = 0;
	m_baoPai = 0;
	memset( m_GangMoney, 0, sizeof(m_GangMoney));
	memset(m_nGangShangHua, 0, sizeof(m_nGangShangHua));
	m_allTingPosInfoMap.clear();

}

void Table_jilin::GameEndReset()
{

	Table::GameEndReset();
	memset(m_nGangShangHua, 0, sizeof(m_nGangShangHua)); 
	memset( &m_JilinGangInfo, 0, sizeof(JiGangInfo));  
	for (int i = 0; i < m_GAME_PLAYER; i++)
		m_JilinGangInfo[i].clear(); 
	bIsCanQiangGangOrPeng = false;
	m_Jilinchi.clear();
	memset(m_TrueWinLost, 0, sizeof(m_TrueWinLost));
	m_bHasTianhu = false;	
	m_baoPai = 0;
	m_nPlayerTingmap.clear();
	m_addlastCard = 0;
	m_bIsTongBaoTime = false;
	memset(m_nPlayerTongBao, 0, sizeof(m_nPlayerTongBao)); 
	m_bTongBao = false;
	memset( m_jilin_baoting, 0, sizeof(m_jilin_baoting));
	memset( m_nqiangArr, -1, sizeof(m_nqiangArr));
	memset( m_nGiveUphu, 0, sizeof(m_nGiveUphu));
	memset( m_outCardCountAfterBaoTing, 0, sizeof(m_outCardCountAfterBaoTing));
	memset( m_nXianBayi, 0, sizeof(m_nXianBayi));
	memset( m_RecvBaipaiMap, 0, sizeof(m_RecvBaipaiMap));  //默认为0   摆牌选择 
	memset( m_nRecvBaoTingPlayerMap, 0, sizeof(m_nRecvBaoTingPlayerMap));
	memset( m_nCanBaoTingArr, 0, sizeof(m_nCanBaoTingArr)); 
	memset( m_fenResult, 0, sizeof(m_fenResult)); 
	m_allTingPosInfoMap.clear();
	memset(m_NoQuickBaoTongBaoGrapPaiCount, -1, sizeof(m_NoQuickBaoTongBaoGrapPaiCount));

	memset( m_WinlostArr, 0, sizeof(m_WinlostArr));
	memset( m_QiangGangHu, 0, sizeof(m_QiangGangHu)); 
	memset( m_GangMoney, 0, sizeof(m_GangMoney)); 
	m_nRecvPlaerOp.clear();
	
}

Table_jilin::~Table_jilin()
{
	log_debug("delete jilin   !! ");
	
}

void  Table_jilin::GameStart()
{
	Table::GameStart(); 
	Table::OnUserGrapCard(m_bBanker);	
	MJ_fan_buffer  fan;
	if (m_bBanker >= 0 && m_bBanker < m_GAME_PLAYER  &&  Check_Hu(m_bBanker, m_HandCard[m_bBanker], fan, m_bCurCard))
	{
		m_bHasTianhu = true;
		m_bUserOperate[m_bBanker] |= TYPE_HU;
		proto::game::AckOpNotify  stNotify;
		stNotify.set_chairid(m_bBanker);
		stNotify.set_optype( m_bUserOperate[m_bBanker]); 
		stNotify.set_card(m_bCurCard); 
		unicast(SERVER_OPERATE_NOTIFY, m_bBanker, stNotify); 
		log_debug("has hu !! mbanker ");

	}
}



int  Table_jilin::HuIfPreRemoveOneCardNextAddOne(_uint8 ChairID ,  const HandCards *phc, MJ_fan_buffer  & oFan, _uint8  hu_pai , bool  jian_cha )
{
	if (ChairID >= m_GAME_PLAYER)
		return 0; 
	if (phc->ChangeableCardsLen > 14 || phc->ChangeableCardsLen % 3 != 2)
	{
		log_error("error %d   pai[%s]  " , phc->ChangeableCardsLen  , CatHandCard(*phc , ChairID ) );
		return 0;
	}
	
	
	oFan.Clear();
	if (jian_cha == false && !ThreeColorOrQingYiSe(ChairID, phc))
		return  0;
	if (jian_cha == false && !Card_19(ChairID, phc))
		return 0;
	if (m_GameLogic.is_qi_dui(phc))
		return  1;

	if ( Ting_Zhongfabai(phc->ChangeableCards, phc->ChangeableCardsLen, hu_pai))
		return  1;
	if( jian_cha == false && check_hu_kezi(ChairID) == 0)
		return  0;  	

	MJ_win_pattern pattern;
	if (!FormatCards( (HandCards*)phc, pattern))
		return 0;
	if ( check_jia_hu(ChairID, hu_pai, pattern))
		return 1;
	if ( check_bian_hu(ChairID, hu_pai, pattern))
		return 1;

	if (m_GameLogic.IsPengPengHu(phc, pattern.kan, pattern.jiang))
		return 1;
	return 0;
	
}


int Table_jilin::GangTingJudget(_uint8 ChairID, HandCards *phc, MJ_fan_buffer oFan, _uint8 hu_pai )
{ 
	if (ChairID >= m_GAME_PLAYER)
		return 0; 
	if (phc->ChangeableCardsLen > 14 || phc->ChangeableCardsLen % 3 != 2)
		return 0;
	if (m_GameLogic.is_qi_dui(phc))
		return  1;
	oFan.Clear();
	
	MJ_win_pattern pattern;
	if (Ting_Zhongfabai(phc->ChangeableCards, phc->ChangeableCardsLen, hu_pai))
		return  1;

	if (!FormatCards((HandCards*)phc, pattern))
		return 0;

	//if (!jilin_check_hu(phc->ChangeableCards, phc->ChangeableCardsLen))
		//return 0;

	if (check_jia_hu(ChairID, hu_pai, pattern , false ,  true))
		return 1;
	if ( check_bian_hu(ChairID, hu_pai, pattern ,false , true ) )
		return 1;

	if (m_GameLogic.IsPengPengHu(phc, pattern.kan, pattern.jiang))
		return 1;

	//if (is_all_kezi() && phc_allkezi(&phc))
		//return 1;

	return 0;

}

int Table_jilin::Jilin_qidui(const  HandCards  *p ,  bool  IsTongBaoMoBao)
{
	if (p->ChangeableCardsLen != 14)
		return 0;
	map<int, int> all_map; 
	bool  is_first = true;
	for (int i = 0; i < p->ChangeableCardsLen; i++)
	{		
		int ct = p->ChangeableCards[i]; 
		if (IsTongBaoMoBao )
		{
			if (ct == m_baoPai && ct > 0 && is_first)
			{
				is_first = false;
				continue; 
			}
		}
		all_map[ct]++; 
	}
	int dui = 0;
	for (map<int, int>::iterator it = all_map.begin(); it != all_map.end(); ++it)
	{
		if (all_map[it->first] == 2 || all_map[it->first] == 4)
			dui += all_map[it->first] / 2; 
		else
		{
			if (IsTongBaoMoBao)
			{
				dui += (all_map[it->first] + 1) / 2;
				IsTongBaoMoBao = false;
			}
		}
	}
	return dui == 7;  

}

int Table_jilin::Check_Hu(_uint8 ChairID, HandCards& pHc, MJ_fan_buffer& oFan , _uint8 add_last_card )
{
	if (ChairID >= m_GAME_PLAYER)
		return  0; 
	if (pHc.ChangeableCardsLen > 14)
	{
		log_error("cardlen %d", pHc.ChangeableCardsLen);
		return TYPE_NULL;
	}
	//取余3不等于2的手牌不做胡牌判断
	if (pHc.ChangeableCardsLen != 0 && pHc.ChangeableCardsLen % 3 != 2)
	{
		log_error("cardlen[%d] pai[%s] sid[%d]", pHc.ChangeableCardsLen , CatHandCard(pHc ,ChairID) , ChairID );
		return TYPE_NULL;
	}
	oFan.Clear(); 	
	MJ_win_pattern pattern;	
	 
	if (m_jilin_baoting[ChairID].bao_ting  &&  m_PlayGrapCard[ChairID]== m_baoPai && ChairID == m_bGrapId )
	{		 
		oFan.Add(MJ_FAN_TYPE_MO_BAO);
		log_debug("mobao   bao %x  sid[%d]" ,m_baoPai  , ChairID  );
	}
	if ( m_bIsTongBaoTime  && m_nPlayerTongBao[ChairID])
	{
		oFan.Add(MJ_FAN_TYPE_TONG_BAO);
		log_debug("tongbao !! %x   %x", add_last_card, m_baoPai);
	}
	if (oFan.Size() == 0 && !ThreeColorOrQingYiSe(ChairID, &pHc))
	{
		//log_debug("no 3se ");
		return  0;
	}
	if(m_GameLogic.is_qi_dui(&pHc)||  Jilin_qidui(&pHc ,  (oFan.Find(MJ_FAN_TYPE_TONG_BAO)|| oFan.Find(MJ_FAN_TYPE_MO_BAO)) ) )
	{
		oFan.Add(MJ_FAN_TYPE_QIDUI); 		
	}	

	if (oFan.Size() == 0 && !Card_19(ChairID, &pHc))
	{
		// log_debug("no 19 ");
		return 0;
	}
	bool zhongfabai_canhu  = 0;
	if (Ting_Zhongfabai(pHc.ChangeableCards, pHc.ChangeableCardsLen, add_last_card))
	{
		zhongfabai_canhu = 1; 
		log_debug("canhu zhongfabai %x  %s  %x ", zhongfabai_canhu , CatHandCard(pHc, ChairID) , add_last_card)
			
	}
	
	if (oFan.Size() == 0 && zhongfabai_canhu == 0  && check_hu_kezi(ChairID)== 0 )
	{
		  //log_debug("hu nokezi  !!");
		return  0;
	}
	//bool  can_hu = jilin_check_hu(pHc.ChangeableCards, pHc.ChangeableCardsLen); 
	// if(zhongfabai_canhu   ||  can_hu  ) 
	
	if( zhongfabai_canhu  ||   FormatCards(&pHc, pattern))  // 判断是否能胡牌	
	{		
		AddFanJilinHu(ChairID, pHc, oFan, pattern ,  0 ,  add_last_card );
	}
	else
	{
		if (oFan.Size())
			AddFanJilinHu(ChairID, pHc, oFan, pattern, 0   , 0);
	}
	

	return oFan.IsNull() ? TYPE_NULL : TYPE_HU;

}

int  Table_jilin::Ting_Zhongfabai( const  _uint8  *parr, _uint8  len  , _uint8  add_last )
{
	if ( add_last> 0x40  &&  len<= 14  )
	{
		int arr[20] = { 0 }; 
		int all = len; 
		int  color[5] = { 0 };
		for (int i = 0; i < len; i++)
		{
			arr[i] = parr[i]; 
			if (parr[i] > 0x40)
			{
				
				int face = get_face( parr[i]);
				if (face >= 1 && face <= 3)
					color[face] = 1;
			}			
		}
		if (color[1] + color[2] + color[3] != 3)					
			return 0;			
		return   ( Can_hu(arr, all) && zhongfabai_count() == 1 );
	}
	return  0; 
}

int Table_jilin::check_hu_kezi(_uint8 ChairID)
{
	if (  ChairID >= m_GAME_PLAYER)
		return 0;
	if ( m_JilinGangInfo[ChairID].len)
		return  1;  	
	for (int i = 0; i < (int)m_Jilinchi.size(); i++)
	{
		if (m_Jilinchi[i].chi_id == ChairID)
			return 1;
	}
	const  HandCards *phc = &m_HandCard[ChairID]; 
	for (int i = 0; i < phc->ChangeableCardsLen; i++)
	{
		if (phc->ChangeableCards[i]> 0x30)
			return 1;
	}

	for (int i = 0; i < phc->FixedCardsLen; i++)
	{
		if (phc->FixedCards[i].state  & TYPE_PENG ||  (phc->FixedCards[i].state  & TYPE_GANG) )
			return 1;
	} 	
	vector<int> all_hand_card;
	for (int i = 0; i < phc->ChangeableCardsLen; i++)
	{
		all_hand_card.push_back(phc->ChangeableCards[i]); 	
	}
	
	return  hu_has_ke( all_hand_card);  	 

}



void Table_jilin::DoMultiOperate(_uint8 ChairID, int iOpType, _uint8 bCard)
{	
	if (  ChairID >= m_GAME_PLAYER)
	{
		log_error("error !!! sid %d" , ChairID );
		return; 
	}
	
	if (iOpType & TYPE_HU && m_bUserOperate[ChairID] & TYPE_HU && iOpType != TYPE_PASS)
	{
		for (_uint8 c = NextChair(ChairID); c != ChairID; c = NextChair(c))
		{
			if ( m_jilin_baoting[c].buzhang.Find(bCard))
				m_jilin_baoting[c].buzhang.RemoveLast();
		}
	}
	
	Table::DoMultiOperate(ChairID, iOpType, bCard);

}



int Table_jilin::GetPlayerWinlostMoney(_uint8 ChairID)
{
	if (  ChairID >= m_GAME_PLAYER)
		return 0;
	int ct = 0;
	Player *p = GetPlayer(ChairID);
	if (p)
	{
		if (p->money < 10000)
			ct = p->money - 10000;
		else
			ct = p->money - 10000;
	}
	else
		return  0;

	 // log_debug("uid[%d] i[%d] ct[%d]" , p->uid , ChairID , ct  );

	return ct;
}

void Table_jilin::ConfInit_Remote(const Json::Value& val)
{
	Table::ConfInit_Remote(val);
	m_conf.m_bZimoHu       = false ;	
	m_conf.m_bSupportChi    =  true  ;
	m_conf.m_bSupportZhuaniao = false;
	m_conf.m_bQiangGangHu =  true ;
	m_conf.m_bQiangAnGang = false;
	m_conf.m_bQiangZhiGang = false;
	m_conf.m_bYiPaoDuoXiang = false;
	m_conf.m_bFourceHuMutilHu = false;
	m_conf.m_bCalcAllNiao = false; 
	m_CardPool.Set( CARD_POOL_NO_FLOWER, tid);
	m_CardPool.GameId = JI_LIN_MAJIANG_GAME_ID;  //   5070  
	g_jilin_conf.feng_ding = 16		;

	if (json_check_play_type_id(val))
	{
		MJ_SUB_ID subid;
		json_get_play_type_id(val, 5070, subid); 
		if (subid.Find(5070001))
		{
			m_GAME_PLAYER = 4;
			m_room_config.Add(5070001);
		}
		else if (subid.Find(5070002))  
		{
			m_GAME_PLAYER = 3;
			m_room_config.Add(5070002);
		}
		else
		{
			m_GAME_PLAYER = 2;
			m_room_config.Add(5070003);
		}

		json_get_play_type_id(val, 5071, subid);
		if (subid.Find(5071001))
		{
			g_jilin_conf.yao_jiu_dan  = true;
			m_room_config.Add( 5071001);  
		}
		
		if (subid.Find(5071002))
		{
			g_jilin_conf.quick_bao = true;
			m_room_config.Add( 5071002);
		}
		
		if (subid.Find(5071003))
		{
			g_jilin_conf.xiaojifeidan = true;
			m_room_config.Add(5071003);
		}		
		if (subid.Find(5071004))
		{
			//g_jilin_conf.si_lou = true;
			m_room_config.Add( 5071004 );
		}
		
		json_get_play_type_id(val, 5072 , subid);
		if (subid.Find(5072001))
		{
			g_jilin_conf.xian_ba_yi = true;
			m_room_config.Add( 5072001);
		}
		else
		{
			g_jilin_conf.xian_ba_yi = false;
			m_room_config.Add( 5072002);
		}
		
		json_get_play_type_id(val, 5073, subid);
		if (subid.Find(5073001))
		{
			g_jilin_conf.guo = 100;
			m_room_config.Add(subid[0]);
		}
		else if (subid.Find(5073002))
		{
			g_jilin_conf.guo = 200;
			m_room_config.Add(subid[0]);
		}
		else if (subid.Find(5073003))
		{
			g_jilin_conf.guo = 500;
			m_room_config.Add(subid[0]);
		}
		else
		{
			g_jilin_conf.guo = 99999;
			m_room_config.Add( 5073004);
		}
		

		json_get_play_type_id(val, 5074 , subid);
		if (subid.Find(5074001) || subid.Find(5074003) )
		{
			g_jilin_conf.feng_ding = 16;
			m_room_config.Add(subid[0]);
		}
		else if (subid.Find(5074002) || subid.Find(5074004))
		{
			g_jilin_conf.feng_ding = 32;
			m_room_config.Add(subid[0]);
		}
		else if (subid.Find(5074005) || subid.Find(5074007))
		{
			g_jilin_conf.feng_ding = 40;
			m_room_config.Add(subid[0]);
		}
		else if (subid.Find(5074009))
		{
			g_jilin_conf.feng_ding = 16;  
			m_room_config.Add(5074009 );
			log_debug(" 16 ");
		}
		else if (subid.Find(5074010))
		{
			g_jilin_conf.feng_ding = 32;  
			m_room_config.Add(5074010 );
			log_debug("32"); 
		}
		else if (subid.Find(5074011) || subid.Find(5074012))
		{
			g_jilin_conf.feng_ding = 64; 
			m_room_config.Add(subid[0]);
			log_debug("64 !!fengding 1!! ");
		}
		else
		{
			if (subid.Find(5074006))
				m_room_config.Add( 5074006 );
			else
				m_room_config.Add(5074008); 
			g_jilin_conf.feng_ding = 80; 
			
		}	

		json_get_play_type_id(val, 5071 , subid);
		if (subid.Find(5071005))
		{
			g_jilin_conf.yao_jiu_three_se = true;
			m_room_config.Add(5071005 );
			log_debug("19 3se!!!");
		}
		if (subid.Find( 5071006 )) 
		{
			g_jilin_conf.yaoji_wanneng = true; 
			m_room_config.Add(5071006); 
			log_debug("yaoji wan!!! ");
		}
		log_debug("fengding [%d] guo[%d]" , g_jilin_conf.feng_ding , g_jilin_conf.guo  );
	}

}


int Table_jilin::OneColor(int sid, const  HandCards *phc ,  bool  is_tongmo_bao  )
{
	if (sid<  0 || sid >= m_GAME_PLAYER)
		return  0;

	int color[10] = { 0 };
	bool is_first = true; 
	for (int i = 0; i < phc->ChangeableCardsLen; i++)
	{
		int  suit  = get_suits(phc->ChangeableCards[i]);
		if (is_tongmo_bao)
		{
			if (is_first &&  phc->ChangeableCards[i] == m_baoPai &&   m_baoPai> 0 )
			{
				is_first = false;
				continue; 
			}
		}
		if ( suit >= 0 && suit < 5){
			color[suit] = 1;			
		}
		
	}
	for (int i = 0; i < phc->FixedCardsLen; i++)
	{
		if (phc->FixedCards[i].state  & TYPE_PENG || phc->FixedCards[i].state & TYPE_GANG)
		{
			int ct = get_suits(phc->FixedCards[i].CardData);
			if (ct >= 0 && ct < 5)
			{
				log_debug("ct %x", ct);
				color[ct] = 1;
			}
				
		}
		else if (phc->FixedCards[i].state & TYPE_CHI)
		{
			int a = phc->FixedCards[i].c[0];
			int suit = get_suits(a);
			if (suit >= 0 && suit < 5)
			{				
				color[suit] = 1; 
			 //	log_debug("ct %x", a );
			}
				
		}
	}
	JiGangInfo   &info = m_JilinGangInfo[sid];
	for (int i = 0; i < info.len; i++) 
	{
		gang_card  &p = info.g_arr[i];
		for (int r = 0; r < XUAN_FENG_GANG_ARR_LEN; r++)
		{
			if (p.arr[r] == 0 || p.arr[r] >= 0x30   ) continue;    //  //int ct = p.arr[r];			  								
			if (!p.yaojiudan)
				break;			 			
				
		}
	} 

	if( (color[0] + color[1] + color[2])  == 1)
		return   1;  
	//log_debug("no 3 se !! %d %d %d   " , color[0] , color[1] , color[2]);   


	return 0;

}


int Table_jilin::ThreeColorOrQingYiSe(int sid , const  HandCards *phc)
{
	if (sid< 0 || sid>=  m_GAME_PLAYER)
		return  0;

	int color[10 ] = { 0 };
	for (int i = 0; i < phc->ChangeableCardsLen;i++)
	{
		int ct = get_suits(phc->ChangeableCards[i]);
		if (ct >= 0 && ct < 5)
			color[ct] = 1;
	} 
	for (int i = 0; i < phc->FixedCardsLen; i++)
	{
		if (phc->FixedCards[i].state  & TYPE_PENG || phc->FixedCards[i].state & TYPE_GANG)
		{
			int ct = get_suits(phc->FixedCards[i].CardData );
			if (ct >= 0 && ct < 5)
				color[ct] = 1;   
		}
		else if (phc->FixedCards[i].state & TYPE_CHI)
		{
			int a = phc->FixedCards[i].c[0];
			int suit = get_suits(a);
			if (suit >= 0 && suit < 5)
				color[suit] = 1;
		}
	}
	bool  three_se = false;
	JiGangInfo   &info = m_JilinGangInfo[sid]; 
	int  len = min(info.len, 20); 
	for (int i = 0; i <   len   ;   i++)
	{
		gang_card  &p = info.g_arr[i];
		for (int r = 0; r < XUAN_FENG_GANG_ARR_LEN; r++)
		{ 			  
			if (p.arr[r] == 0 || p.arr[r] >= 0x30  ) continue;  	
			if (g_jilin_conf.yao_jiu_three_se && p.yaojiudan)
				three_se = true; 
			if(!p.yaojiudan)
				break;								
		}
	} 	

	if ((color[0] + color[1] == 0) || (color[0] + color[2] == 0) || (color[1] + color[2] == 0))
		return  1;
	if( three_se ||   (color[0] && color[1] && color[2]) )
		return   3; 

	//log_debug("no 3 se !! %d %d %d  " , color[0] , color[1] , color[2]); 
	return 0;
	
}



int Table_jilin::Card_19( int sid  ,  const  HandCards  *phc)
{	
	if (sid< 0 || sid>= m_GAME_PLAYER)
		return 0;

	for (int i = 0; i < phc->ChangeableCardsLen; i++)
	{		
		if (phc->ChangeableCards[i] > 0x30)
			return 1; 
		int face = get_face(phc->ChangeableCards[i]); 
		if (face == 9 || face == 1)
			return  1;		
	}
	for (int i = 0; i < phc->FixedCardsLen; i++)
	{
		if (TYPE_PENG & phc->FixedCards[i].state || (TYPE_GANG & phc->FixedCards[i].state))
		{
			if (phc->FixedCards[i].CardData > 0x30 )
				return 1; 
			int ct = phc->FixedCards[i].CardData;
			if (get_face(ct)==1  || get_face(ct) == 9)
				return  1;  
		}
		if (phc->FixedCards[i].state & TYPE_CHI)
		{
			int a = phc->FixedCards[i].c[0];
			int b = phc->FixedCards[i].c[1];
			int c = phc->FixedCards[i].c[2];
			if (get_face(a) == 1 || get_face(a) == 9)
				return 1;
			if (get_face(b) == 1 || get_face(b) == 9)
				return 1;
			if (get_face(c) == 1 || get_face(c) == 9)
				return 1;
		}
	}
	JiGangInfo   &info = m_JilinGangInfo[sid];
	for (int i = 0; i < info.len; i++)
	{
		gang_card  &p = info.g_arr[i];
		for (int r = 0; r <   XUAN_FENG_GANG_ARR_LEN ; r++)
		{
			if (p.arr[r] == 0) break ;
			int ct = p.arr[r];
			if (ct> 0x30)
				return  1;
			if (get_face(ct) == 1 || get_face(ct) == 9)
				return 1;
		}
	}
	vector<ZhongfabaiChi>::iterator  it = m_Jilinchi.begin();
	for (; it != m_Jilinchi.end(); ++it)
	{
		if (it->chi_id == sid)
			return 1;
	}
	// log_debug("no 19 !!");

	return 0;
	

}

void Table_jilin::InitBanker()
{
	if (m_nPlayingTimes <=  1  )
	{
		m_bBanker = 0; 
	}
	if (m_bBanker >= m_GAME_PLAYER)
	{
		m_bBanker = 0;
	}
	
}

int Table_jilin::GetGangPeng_Card()
{
	for (int i = 0; i < m_GAME_PLAYER; i++)
	{
		if (m_nqiangArr[i][2] >= 0 && m_nqiangArr[i][2] < m_GAME_PLAYER)
		{
			// log_debug("card  %x", m_nqiangArr[i][0]);
			return m_nqiangArr[i][0];
		}

	}
	return  0; 
}

int  Table_jilin::GetGangPeng_Id()
{
	for (int i = 0; i < m_GAME_PLAYER; i++)
	{
		if ( m_nqiangArr[i][2] >= 0 && m_nqiangArr[i][2] < m_GAME_PLAYER)
		{
			//log_debug("%d" ,m_nqiangArr[i][2]); 
			return m_nqiangArr[i][2];
		}
		
	}
	log_error("error !!!");
	return 0; 
}

void Table_jilin::ProtoQiangJIlinGang(_uint8  qiang  , _uint8 bei_qiang , _uint8  card ) 
{

	for (int i = 0; i < m_GAME_PLAYER; i++)
	{
		if ( m_nqiangArr[i][2] >= 0 && m_nqiangArr[i][2] < m_GAME_PLAYER ) 
		{
			bei_qiang =  m_nqiangArr[i][2];
			card      =  m_nqiangArr[i][0]; 
			 // log_debug("qiang111111111111 [%x]  beiqiang[%x]  card [%x]", qiang, bei_qiang, card);
		}		
	}
	 //	log_debug("qiang [%x]  beiqiang[%x]  card [%x]" , qiang , bei_qiang , card ); 
	proto::game::jilin::NotifyQiangDanMsg    Ack;
	Ack.set_qiang_gang_chairid(qiang); 
	Ack.set_bei_qiang_chairid(bei_qiang); 
	Ack.set_card(card); 
	m_GameLogic.ChuPai(&m_HandCard[bei_qiang], card); 
	broadcast(SER_CMD_JILING_QIANG_GANG_MSG, Ack);
}


void Table_jilin::OnRecvUserOperate(_uint8 ChairID, int iOpType, int iCard)
{
	if (  ChairID >= m_GAME_PLAYER)
		return; 
	//log_debug("chairid %d    op[%x]  card[%x]" , ChairID , iOpType , iCard ); 

	if ( m_bIsTongBaoTime )
	{
		if (iOpType == TYPE_HU )
		{
			m_bIsTongBaoTime = 0;
			Table::OnRecvUserOperate(ChairID, iOpType, iCard); 
			return;
		}
		if (iOpType== TYPE_PASS)
		{
			m_nGiveUphu[ChairID] = 1;
			int can_hu = 0, op_ = 0;
			for (int i = 0; i < m_GAME_PLAYER; i++)
			{				
				if (m_bUserOperate[i] & TYPE_HU)
					can_hu++;
				if ( m_nGiveUphu[i])
					op_++;
			}
			if (can_hu== op_)
			{
				if (m_bOutCardChairID < 0 || m_bOutCardChairID >= m_GAME_PLAYER)
					return; 
				m_bUserOperate[m_bOutCardChairID] |= TYPE_HU; 
				m_GameLogic.AddCard(&m_HandCard[m_bOutCardChairID], m_baoPai); 
				Table::OnRecvUserOperate(m_bOutCardChairID, TYPE_HU, m_baoPai);
				return;
			}
		}
		return;

	}
	if (m_bHasTianhu)
	{
		if (iOpType == TYPE_PASS )
		{
			Table::OnRecvUserOperate(ChairID, iOpType, iCard);
			m_bHasTianhu = false; 
		}
		else if (iOpType == TYPE_HU)
		{
			m_addlastCard = iCard;
			Table::OnRecvUserOperate(ChairID, iOpType, iCard);
			
		}
		else
		{
			m_bHasTianhu = false;
			Table::OnRecvUserOperate(ChairID, iOpType, iCard);
		}			
		m_bHasTianhu = false;
		return;
	}

	if ( bIsCanQiangGangOrPeng )
	{
		m_nqiangArr[ChairID][1] = iOpType;
		  
		int  CurGang_Id = GetGangPeng_Id();   // 当前执行。碰杠。。 的人。 。！！！
		int Pre_No_Max_Op = -1;
		
		for (int right = NextChair(CurGang_Id); right != CurGang_Id; right = NextChair(right))
		{
			if (right == ChairID)  break;
			if (iOpType == TYPE_HU)
			{
				if (m_nqiangArr[right][0] > 0 && m_nqiangArr[right][1] == -1 && m_bUserOperate[right] & TYPE_HU)
				{
					Pre_No_Max_Op = TYPE_HU;
				}
			}
			else if (iOpType == TYPE_ZHIGANG || iOpType == TYPE_PENG)
			{
				if (m_nqiangArr[right][0] > 0 && m_nqiangArr[right][1] == -1 && (m_bUserOperate[right] & TYPE_HU))
					Pre_No_Max_Op = TYPE_HU;
			}
		}
		if (Pre_No_Max_Op == TYPE_HU  && iOpType == TYPE_HU)
		{
			log_debug("has max op hu !!");
			return;
		}
		else if (iOpType == TYPE_HU  && Pre_No_Max_Op == -1)
		{
			m_QiangGangHu[ChairID] = 1;
			
			Table::UserOperateHu(ChairID, GetGangPeng_Card());
			GameEnd();
			return;
		}
		else if (Pre_No_Max_Op == TYPE_HU && (iOpType == TYPE_ZHIGANG || iOpType == TYPE_PENG))
		{
			log_debug("has max op hu !!");
			return;
		}
		else if (Pre_No_Max_Op == -1 && (iOpType == TYPE_ZHIGANG || iOpType == TYPE_PENG))
		{
			ProtoQiangJIlinGang(ChairID, m_bGrapId, m_nqiangArr[m_bGrapId][2]);
			m_bCurOutCard = GetGangPeng_Card();  // 客户端卡住的问题。 
			m_bOutCardChairID = m_bGrapId;
			m_bCurChairID = CurGang_Id;				 // 碰的箭头不对。   
			m_bUserOperate[ChairID] |= iOpType;
			if (CurGang_Id >= 0 && CurGang_Id < m_GAME_PLAYER && m_jilin_baoting[CurGang_Id].bao_ting)
			{
				m_outCardCountAfterBaoTing[CurGang_Id] += 10;
				log_debug("no bao  outid[%x]", CurGang_Id);
			}
			Table::OnRecvUserOperate(ChairID, m_nqiangArr[ChairID][1], GetGangPeng_Card());
			bIsCanQiangGangOrPeng = false;
			log_debug("into this  2222222222  [%d] CurGang_Id[%d]", m_bOutCardChairID, CurGang_Id );
			return;
		}
		int  all_pass = 1, all_hu_pass = 1;
		for (int i = 0; i < m_GAME_PLAYER; i++)
		{
			if (i == CurGang_Id)  continue;
			if (m_nqiangArr[i][0] > 0 && m_nqiangArr[i][1] != TYPE_PASS)
				all_pass = 0;
			if (m_nqiangArr[i][0] > 0 && (m_bUserOperate[i] & TYPE_HU) && m_nqiangArr[i][1] != TYPE_PASS)
				all_hu_pass = 0;
		}

		for (int right = NextChair(CurGang_Id); right != CurGang_Id; right = NextChair(right))
		{
			if (right< 0 || right >= m_GAME_PLAYER)
			{
				log_error("error !! rith %d" , right );
				break;   // rongcuo chuli ..!!!! 
			}			
			if (iOpType == TYPE_PASS)
			{
				if (m_nqiangArr[right][0] > 0 && m_nqiangArr[right][1] == TYPE_HU)
				{
					m_QiangGangHu[right] = 1;
					Table::UserOperateHu(right, GetGangPeng_Card());
					GameEnd();
					return;
				}
				if (m_nqiangArr[right][0] && all_hu_pass == 1
					&& (m_nqiangArr[right][1] == TYPE_ZHIGANG || m_nqiangArr[right][1] == TYPE_PENG)
					)
				{
					ProtoQiangJIlinGang(right, m_bGrapId, m_nqiangArr[m_bGrapId][2]);
					m_bCurChairID = right;
					// m_bCurOutCard = GetGangPeng_Card();    // 客户端卡住的问题。 
					m_bOutCardChairID = m_bGrapId;
					m_bCurOutCard = iCard;
					if (CurGang_Id >= 0 && CurGang_Id < m_GAME_PLAYER && m_jilin_baoting[CurGang_Id].bao_ting)
					{
						m_outCardCountAfterBaoTing[CurGang_Id] += 10;
						log_debug("no bao  outid[%x] [%d]", CurGang_Id, CurGang_Id  );
					}
					Table::OnRecvUserOperate(right, m_nqiangArr[right][1], GetGangPeng_Card());
					bIsCanQiangGangOrPeng = false;
					log_debug("into this  555555555555   right %d ", right);
					return;
				}				
			}
		} 
		if ( all_pass  == 0 )
		{
			log_debug("waiti to op  %d" );  
			for (int i = 0; i < m_GAME_PLAYER; i++)
				log_debug("i [%d]  %d %d  " , i ,  m_nqiangArr[i][0] , m_nqiangArr[i][1] ) ;
			return;
		}

		if (all_pass == 1)
		{
			bIsCanQiangGangOrPeng = false;
			if (CurGang_Id >= 0 && CurGang_Id < m_GAME_PLAYER)
			{
				log_debug("into this  5555555555555 ");
				int op_sid = 0, card = 0, gangid = 0;
				for (int i = 0; i < m_GAME_PLAYER; i++)
				{
					if (m_nqiangArr[i][2] >= 0)
					{
						op_sid = m_nqiangArr[i][2];
						card = m_nqiangArr[i][0];
						gangid = m_nqiangArr[i][1];
					}
				}
				Proto_Jilin_bu(op_sid, card, gangid);
				OnUserGrapCard(op_sid, false);
				return;
			}
			else
			{
				log_error("error %d", m_bCurChairID);
			}
		}	
		bIsCanQiangGangOrPeng = false;
		return;
		
		
	}
	


	Table::OnRecvUserOperate(ChairID, iOpType, iCard);
}


bool Table_jilin::RobGangHu(_uint8 ChairID, _uint8 CardData, int iOpType)
{
	// log_debug("roboto this place !!!! ");
	if (ChairID< 0 || ChairID>= m_GAME_PLAYER)
	{
		log_error("error %d" , ChairID);
		return false;
	}
	if (iOpType & TYPE_WANGANG && !m_conf.m_bQiangGangHu)
		return false;
	if (iOpType & TYPE_ANGANG && !m_conf.m_bQiangAnGang)
		return false;
	if (iOpType & TYPE_ZHIGANG && !m_conf.m_bQiangZhiGang)
		return false;

	
	bool  can_qiang = true;
	if(!LingGuoHu(ChairID) )
	{
		log_debug("cannot qianggang hu !!");
		can_qiang = false; 
	}
	


	_uint8 cbNum = 0;
	for (int i = 0; i < m_GAME_PLAYER; i++)
	{
		if (i == ChairID)
		{
			continue;
		}
		if(can_qiang == false )  continue;   
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

	for (int i =  NextChair(ChairID );  i != ChairID ;  i = NextChair(i))
	{
		if(i< 0 || i>= m_GAME_PLAYER)  break; 
	 	if (i == ChairID)  continue;   
		if (m_bUserOperate[i] & TYPE_HU )
		{
			// if(m_HandCard[i].ChangeableCardsLen %) 
			m_QiangGangHu[i] = 1;
			m_bOutCardChairID = m_bGrapId;  
			log_debug("into this palce   len %d  pai %s    card %x", m_HandCard[i].ChangeableCardsLen, CatHandCard(m_HandCard[i], i)  , CardData );
			
			Table::UserOperateHu(i ,  CardData  );
			GameEnd(); 
			return true; 
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
			GetPlayerUid(ChairID), ChairID, i, m_bUserOperate[i]);

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



bool Table_jilin::EstimateUserJilinGangRespond(_uint8  GangId , _uint8 bOutCard , int gang_id )
{
	if (GangId < 0 || GangId >= m_GAME_PLAYER)
	{
		log_error("gangid %d" , GangId );
		return false;
	}
	bIsCanQiangGangOrPeng = false ;   //是否轮到下一玩家摸牌
	memset(m_bUserOperate, TYPE_NULL, sizeof(m_bUserOperate)); 
	memset( m_nqiangArr, -1 , sizeof(m_nqiangArr));
	bool  can_hu = true; 	
	if (!LingGuoHu(GangId))
	{
		can_hu = false;
		log_debug("cancel %x   m_bOutCardChairID[%x]    sid [%d ] ",  1  , GangId, 0); 		 
	}

	
	for (int i = 0; i < m_GAME_PLAYER; i++)
	{
		if (i == GangId)  // 出牌的玩家。。
		{
			m_bUserOperate[i] = TYPE_NULL;
			m_nqiangArr[i][0] = bOutCard;  
			m_nqiangArr[i][1] = gang_id; 
			m_nqiangArr[i][2] = GangId; 
			//log_debug("init2222   m_nqiangArr   %d %d %d  %d  " , m_nqiangArr[i][0] , m_nqiangArr[i][1] , m_nqiangArr[i][2] , i );
			continue;
		}
		// if(can_hu == false) 
			// break;  
		//牌型判断	一个人打牌。后，其他人。可以执行的操作 ！！ 
		MJ_major major;
		m_bUserOperate[i] = CheckOutCardOtherPlayerCanOpCode(i, bOutCard, major, m_HuFanNums[i]);
		if (can_hu == false)
			m_bUserOperate[i] &= (~TYPE_HU );   // bug !!!!
		if( (m_bUserOperate[i] &  TYPE_HU)   ||  (m_bUserOperate[i] &  TYPE_ZHIGANG) ||  (m_bUserOperate[i] & TYPE_PENG)  )
		{
			bIsCanQiangGangOrPeng = true ;
			m_nqiangArr[i][0] = 1;   // 可以抢杠的人。 

			proto::game::AckOpNotify stNotify; 
			stNotify.set_chairid(i);
			stNotify.set_card(bOutCard);
			stNotify.set_optype(m_bUserOperate[i]);
			if (m_bUserOperate[i] & TYPE_ZHIGANG)
			{
				proto::game::tagGangInfo* ganginfo = stNotify.add_gang();
				ganginfo->set_card(bOutCard);
				ganginfo->set_state(TYPE_ZHIGANG);
			}
			unicast(SERVER_OPERATE_NOTIFY, i , stNotify); 
		}
		if (m_bUserOperate[i] != 0)
		{
			log_info(" uid[%d] chairid[%d] Operate[%x]  totalop[%d] ", GetPlayerUid(i), i, m_bUserOperate[i], m_bMultiOpUserNum);
		}
	}
	

	for( int   j = NextChair(GangId); j != GangId; j = NextChair(j))
	{
		if(j<0 || j >= m_GAME_PLAYER)  break; 
		if(can_hu ==false)
			break; 
		if (m_bUserOperate[j]  & TYPE_HU) //   &&  !g_jilin_conf.si_lou)
		{
			m_QiangGangHu[j] = 1; 
			m_bCurChairID = GangId ;
			m_bOutCardChairID = m_bGrapId;
			m_bCurOutCard = bOutCard ; 
			// log_debug("auto hu   %d  outcard  %x   len %d  %s  " ,  
				//j  , bOutCard  , m_HandCard[j].ChangeableCardsLen  ,   CatHandCard(m_HandCard[j] , j ) ) ; 
					 
			Table::UserOperateHu(j ,  bOutCard); 
			GameEnd();
			return true;
		}
	}

	if ( bIsCanQiangGangOrPeng == false )
	{
		log_debug("bcurchairid[%d]  bOutChairID[%d]", m_bCurChairID,  GangId);				
	}
	else
	{
		//开启操作时钟 TODO		
		for (_uint8 c = NextChair(GangId); c != GangId; c = NextChair(c))
		{
			if (m_bUserOperate[c] != TYPE_NULL)
			{
				set_operator_timer(c);
			}
		}
	}
	return bIsCanQiangGangOrPeng ;
}

//int   winlost = GetPlayerWinlostMoney(m_bOutCardChairID);   // m_bCurChairID
//// 如果玩家A为0锅，那么A报听前，其他人不能胡他打出的点炮，A报听后点炮其他玩家可以胡 
//if (winlost < 0 && abs(winlost) >= g_jilin_conf.guo
//	&& m_bOutCardChairID < m_GAME_PLAYER &&  !m_jilin_baoting[m_bOutCardChairID].bao_ting)
//{ 

int  Table_jilin::LingGuoHu(_uint8  ChairID )
{
	if (ChairID >= m_GAME_PLAYER)
		return 0; 
	int   winlost = GetPlayerWinlostMoney(ChairID); 
	if ( winlost < 0 && abs(winlost) >= g_jilin_conf.guo
		&& (!m_jilin_baoting[ChairID].bao_ting || (m_jilin_baoting[ChairID].bao_ting && m_outCardCountAfterBaoTing[ChairID] <= 1))
		)
	{
		return  0;
	}
	return  1;
}

bool Table_jilin::EstimateUserRespond(_uint8 bOutChairID, _uint8 bOutCard)
{	
	if (bOutChairID >= m_GAME_PLAYER )
	{
		log_error("error %d   ", bOutChairID   );
		return false;
	}
	if (m_bGameState != GAME_PLAYING)
	{
		log_error("error !! %d", m_bGameState);
		return true; 
	} 
	if (!g_jilin_conf.si_lou)  // 死搂：默认不选，勾选时，点炮可选择胡或过，
	{
		MJ_major major; 		
		int  op_code[4] = { 0 };
		bool  canhu = true; 
		if (LingGuoHu(bOutChairID) == 0 )
		{
			log_debug("cannot hu !!! ");
			canhu = false;
		}
		map<_uint8, _uint8> tingmap;
		for (int i = 0; i < m_GAME_PLAYER; i++)
		{
			if (canhu == false) break; 
			if (i == bOutChairID) continue; 
			tingmap.clear();
			GetAllTingCard(tingmap, i);
			if (tingmap[bOutCard] )
			{
				op_code[i] = TYPE_HU;
			}			
		}
		int s = 0;
		for (int i = NextChair(bOutChairID); i != bOutChairID ; i = NextChair(i))
		{
			if (s++ > 10)  
				return true;
			if (i == bOutChairID ) continue; 			
			if (i >= 0 && i < m_GAME_PLAYER  && (op_code[i] & TYPE_HU))
			{
				log_debug("into this place !!! chairid %d", i);
				m_bUserOperate[i] |= TYPE_HU;   // bug !! 
				//Table::OnRecvUserOperate(i, TYPE_HU, m_bCurCard);
				Table::UserOperateHu(i, bOutCard);   // 胡放炮。。 ！！！
				GameEnd();   
				return  true;
			}
		}
	}

	if ( m_bIsTongBaoTime)   //  通宝时间段 。。 
	{
		for (int i = 0; i < m_GAME_PLAYER; i++)
		{
			if (i == m_bOutCardChairID)  continue;
			if ((m_bUserOperate[i] & TYPE_HU))
				return true;
		}
		if ( m_bOutCardChairID< m_GAME_PLAYER  && m_baoPai> 0)
		{
			m_bUserOperate[m_bOutCardChairID] |= TYPE_HU;
			m_bCurChairID = m_bOutCardChairID;    // 通宝看成，， 自摸，， m_bCurChairID
			m_bCurCard = m_baoPai;

			if (m_HandCard[m_bOutCardChairID].ChangeableCardsLen % 3 == 1)
				m_GameLogic.AddCard(&m_HandCard[m_bOutCardChairID], m_baoPai);


			m_nPlayerTongBao[m_bOutCardChairID] = 1;
			log_debug("into this place tongbao !! %d %x ", m_bOutCardChairID, m_baoPai);
			//Table::OnRecvUserOperate(m_bOutCardChairID, TYPE_HU, m_baoPai);  

			Table::UserOperateHu(m_bOutCardChairID, m_baoPai);
			GameEnd();
			return true;
		}
		else
		{
			log_error("error %d", m_bOutCardChairID);
		}
	}

	Table::EstimateUserRespond(bOutChairID, bOutCard);  	
	return true;

}

int Table_jilin::ProtoBaoSwap(_uint8  ChairID )
{
	int out = 0;	 	
	if (m_baoPai)
	{		
		for (int i = 0; i < (int)m_Jilinchi.size(); i++)
		{
			if (m_baoPai> 0x40)
				out++; 
		}  
		for (int  i = 0; i < m_GAME_PLAYER; i++)
		{
			int all = m_stUserOutCard[i].Size();
			for (int j = 0; j < all; j++)
			{
				if (m_baoPai == m_stUserOutCard[i][j])
					out++;
			}
			const  HandCards *phc = &m_HandCard[i];
			for (int t = 0; t < phc->FixedCardsLen; t++)
			{
				if ((phc->FixedCards[t].state & TYPE_PENG) && phc->FixedCards[t].CardData == m_baoPai)
				{
					out += 3;
				}
				else if (phc->FixedCards[t].state  & TYPE_CHI)
				{
					int a = phc->FixedCards[t].c[0];
					int b = phc->FixedCards[t].c[1];
					int c = phc->FixedCards[t].c[2];
					if (a == m_baoPai || b == m_baoPai || c == m_baoPai)
						out++;
				}
			}
			JiGangInfo &ganginfo = m_JilinGangInfo[i];
			for (int st = 0; st < ganginfo.len; st++)
			{
				gang_card  &g = ganginfo.g_arr[st];
				for (int r = 0; r < XUAN_FENG_GANG_ARR_LEN; r++)
				{
					if (g.arr[r] == 0)  break;
					if (g.arr[r] == m_baoPai)
						out += g.bu[r];
				}
			}			
			if (out >= 3)
				break;
		}		
	}
	
	//log_debug("out[%d]", out ); 
	if (out >= 3  )
	{
		int last_card = m_baoPai;
		proto::game::jilin::BaoPaiMsg   ack; 				
		m_baoPai = m_CardPool.GetLast()  ;   //  // m_baoPai = m_CardPool.GetOneCard(); 
		ack.set_card(last_card );
		ack.set_new_card(m_baoPai);  
		ack.set_type(2) ;  
		log_debug("old[%x]  baopai [%x]" ,  last_card , m_baoPai ); 
		broadcast(SER_CMD_JILIN_BAOPAI_MSG, ack); 		
		std::string str = ack.SerializeAsString();
		save_video_data(SER_CMD_JILIN_BAOPAI_MSG, str); 		
		if (m_CardPool.Count()<=13  )
		{
			// GameEnd();
			return 0; 
		}
	}
	return 0;
}

int Table_jilin::OutCardSwapBaoPai_NoQuickFun(_uint8   ChairID )
{
	if (ChairID >= m_GAME_PLAYER)
		return 0;

	if (m_baoPai != 0 && m_jilin_baoting[ChairID].bao_ting)
	{
		//if (g_jilin_conf.quick_bao)   // 快宝：默认勾选，勾选时，报听的同时就能胡通宝，不勾时，要等到下次轮到自己抓牌时才能胡通宝
		{
			map<_uint8, _uint8> curmap;
			GetAllTingCard(curmap, ChairID);
			for (map<_uint8, _uint8>::iterator it = curmap.begin(); it != curmap.end(); ++it)
			{
				// log_debug("first %x  bao %x", it->first, m_baoPai);
				if (it->first == m_baoPai || (m_baoPai == 0x11 && g_jilin_conf.yaoji_wanneng))
				{
					m_bIsTongBaoTime = true;
					m_nPlayerTongBao[ChairID] = 1;
					return 1;
					// log_debug("into   tongbao time !! ");
				}
			}
		}
	}
	return  0;

}

int  Table_jilin::OutCardSwapBaoPaiFunction(_uint8   ChairID )
{
	if (ChairID >= m_GAME_PLAYER)
		return 0; 

	if (m_baoPai != 0 && m_jilin_baoting[ChairID].bao_ting)
	{
		if (g_jilin_conf.quick_bao)   // 快宝：默认勾选，勾选时，报听的同时就能胡通宝，不勾时，要等到下次轮到自己抓牌时才能胡通宝
		{
			map<_uint8, _uint8> curmap;
			GetAllTingCard(curmap, ChairID);
			for (map<_uint8, _uint8>::iterator it = curmap.begin(); it != curmap.end(); ++it)
			{
				// log_debug("first %x  bao %x", it->first, m_baoPai);
				if (it->first == m_baoPai || (m_baoPai == 0x11 && g_jilin_conf.yaoji_wanneng))
				{
					m_bIsTongBaoTime = true;
					m_nPlayerTongBao[ChairID] = 1; 
					return 1;
					log_debug("into   tongbao time !! ");
				}
			}
		}
	}
	return  0;
}

void Table_jilin::HuanBao(_uint8  ChairID)
{
	int iCount = 0;
	int pre_bao = m_baoPai;
	while (m_CardPool.Count() > 13)
	{
		if (iCount++ > 100)
			break;
		pre_bao = m_baoPai;
		ProtoBaoSwap(ChairID);
		if (pre_bao != m_baoPai)
		{
			if (OutCardSwapBaoPaiFunction(ChairID))
				return;
		}
		else
			break;
	}

}


void Table_jilin::OnUserOutCard(_uint8 ChairID, _uint8 bOutCard)
{
	if(  ChairID >= m_GAME_PLAYER)
	{
		log_error("chairid error %d" , ChairID);
		return;
	}
	m_nRecvPlaerOp.clear();
	m_nGangShangHua[ChairID] = 0;  
	m_TingHouCanOutPaiMap.clear(); 

	Table::OnUserOutCard(ChairID, bOutCard);	 
	memset(m_PlayGrapCard, 0, sizeof(m_PlayGrapCard)); 
	OutCardSwapBaoPaiFunction(ChairID); 

	
	
	if ( m_jilin_baoting[ChairID].bao_ting)
	{
		m_outCardCountAfterBaoTing[ChairID]++;
	}
	if(!g_jilin_conf.quick_bao)
	{
		
		// //ProtoBaoSwap(ChairID);  //    宝牌问题。 。。。问题。。 			
		if (m_jilin_baoting[ChairID].bao_ting  &&   m_outCardCountAfterBaoTing[ChairID] > 1) 
		{
			//HuanBao(ChairID); 
			if (OutCardSwapBaoPai_NoQuickFun(ChairID))
				return; 
			int iCount = 0;
			int pre_bao = m_baoPai;
			while (m_CardPool.Count() > 13)
			{
				if (iCount++ > 100)
					break;
				pre_bao = m_baoPai;
				ProtoBaoSwap(ChairID);
				if (pre_bao != m_baoPai)
				{
					if (OutCardSwapBaoPai_NoQuickFun(ChairID))
						return;
				}
				else
					break;
			}
		}
	}
	else
	{
		if (m_bIsTongBaoTime == false && m_jilin_baoting[ChairID].bao_ting  )
		{			
			HuanBao(ChairID);
		}
		
	}
	if (m_jilin_baoting[ChairID].bao_ting)
	{
		m_allTingPosInfoMap[ChairID].out_size++;
		m_allTingPosInfoMap[ChairID].outcard = bOutCard;
		m_allTingPosInfoMap[ChairID].pos = m_CardPool.Count(); 
	}
	
	

}


void Table_jilin::GameEnd()
{
	
	MJ_user_buffer UserID;
	MultiHu(UserID); 
	if (UserID.Size() > 1  )   // 只能一个人。胡牌。 
	{
		int paoid = m_bCurChairID; 
		int  hu_id = -1; 
		for (int i = 0; i < m_GAME_PLAYER; i++)
		{
			int  ct = (paoid + i) % m_GAME_PLAYER;
			if (m_bIsHu[ct]  && hu_id == -1)
			{ 
				hu_id = ct;
			}
			else if (m_bIsHu[ct] && hu_id)
			{
				m_bIsHu[ct] = false;
			}

		}
	}
	Table::GameEnd(); 
	


	int all = 0;
	for (int i = 0; i < m_GAME_PLAYER; i++)
	{
		int  win_lost = GetPlayerWinlostMoney(i); 
		if (win_lost < 0 && abs(win_lost) >= g_jilin_conf.guo)
			all++;
		if (all>=3)
		{
			log_debug("over !!");
			SendTableDisBandOff(); 
			return;
		}
	} 
}

int  Table_jilin::BuGangAfterBaoTing(_uint8   ChairID , _uint8  grap_card  )
{
	if (  ChairID >= m_GAME_PLAYER)
		return 0;
	
	int  g_face = get_face(grap_card); 
	if (m_JilinGangInfo[ChairID].len == 0)
		return 0;
	JiGangInfo   &pGangInfo = m_JilinGangInfo[ChairID];
	for (int i = 0; i < pGangInfo.len; i++)
	{
		gang_card   &pCurInfo = pGangInfo.g_arr[i];
		for (int j = 0; j < XUAN_FENG_GANG_ARR_LEN; j++)
		{
			int ct = pCurInfo.arr[j]; 
			 //		log_debug("ct  %x %x  " , ct  , grap_card );
			if (ct == 0x11 || ct == 0 || grap_card == 0x11  ) continue;
			if (ct>0x40 && grap_card> 0x40)
				return  TYPE_JILIN_GANG; 
			else if (ct > 0x30 && ct < 0x40 && grap_card> 0x30 && grap_card < 0x40)
				return  TYPE_JILIN_GANG;
			else if (ct < 0x30 && get_face(ct) == 1 && (g_face == 1)  &&  grap_card < 0x30  && g_jilin_conf.yao_jiu_dan)
				return   TYPE_JILIN_GANG;
			else if (ct < 0x30 && get_face(ct) == 9 && (g_face == 9) && grap_card < 0x30 && g_jilin_conf.yao_jiu_dan)
				return   TYPE_JILIN_GANG;
		}
		if ( grap_card == 0x11  && g_jilin_conf.xiaojifeidan)
			return   TYPE_JILIN_GANG; 
	}
	return 0; 
}


int Table_jilin::CheckJilinGangOp(_uint8  ChairID  )
{
	// 特殊钢。。操作。 。
	if (ChairID >= 0 && ChairID < m_GAME_PLAYER)
	{
		int arr[5][10];
		memset(arr, 0, sizeof(arr));
		int  yaoji = 0;
		const HandCards* phc = &m_HandCard[ChairID]; 
		for (int i = 0; i < phc->ChangeableCardsLen; i++)
		{
			int suit = get_suits(phc->ChangeableCards[i]);
			int face = get_face(phc->ChangeableCards[i]);
			if (suit >= 0 && suit <= 4 && face >= 0 && face <= 9)
			{
				arr[suit][face] = 1;
				arr[suit][0]++;
				yaoji += (phc->ChangeableCards[i] == 0x11);
			}
		}
		int ct = arr[3][1] + arr[3][2] + arr[3][3] + arr[3][4];
		if( (ct >= 3) || (ct==2 && yaoji>=1 && g_jilin_conf.xiaojifeidan ) || (ct==1 && yaoji>=2 && g_jilin_conf.xiaojifeidan ) )
			return 1;
		ct = arr[4][1] + arr[4][2] + arr[4][3];
		if(ct >= 3 || (ct >= 2 && yaoji >= 1   && g_jilin_conf.xiaojifeidan ) || (ct >= 1 && yaoji >= 2 && g_jilin_conf.xiaojifeidan ))
			return 1;
		ct = arr[0][1] + arr[2][1];
		if((ct == 2 && yaoji >= 1 && g_jilin_conf.xiaojifeidan && g_jilin_conf.yao_jiu_dan  ) 
			|| (ct >= 1 && yaoji >= 2 && g_jilin_conf.xiaojifeidan  && g_jilin_conf.yao_jiu_dan )
		   )
			return 1;
		ct = arr[0][9] + arr[2][9] + arr[1][9]; 
		if( (ct == 3 && g_jilin_conf.yao_jiu_dan )  || (ct == 2 && yaoji >= 1 && g_jilin_conf.yao_jiu_dan && g_jilin_conf.xiaojifeidan  ) || 
			(ct >= 1 && yaoji >= 2 && g_jilin_conf.yao_jiu_dan && g_jilin_conf.xiaojifeidan ) 
		  )
			return 1;
		if (m_JilinGangInfo[ChairID].len == 0)
			return 0; 

		JiGangInfo   &pGangInfo = m_JilinGangInfo[ChairID];
		for (int i = 0; i < pGangInfo.len; i++)
		{
			gang_card   &pCurInfo = pGangInfo.g_arr[i];
			for (int j = 0; j < XUAN_FENG_GANG_ARR_LEN; j++)
			{
				int ct = pCurInfo.arr[j];
				if(ct == 0x11 || ct == 0 ) continue; 
				if (ct>0x40 && arr[4][0])
					return 1; 
				else if (ct>0x30 && ct < 0x40 && arr[3][0])
					return 1;
				else if(ct < 0x30 && get_face(ct) == 1 && (arr[0][1] || arr[2][1]) &&  g_jilin_conf.yao_jiu_dan ) 
					return 1;
				else if(ct < 0x30 && get_face(ct) == 9 && (arr[0][9] || arr[2][9] || arr[1][9] ) && g_jilin_conf.yao_jiu_dan )
					return  1; 
			}
			if (yaoji && g_jilin_conf.xiaojifeidan )
				return  1;
		}

	}
	return  0;

}

int Table_jilin::GangJudgeAfterTing(_uint8  ChairID, MJ_opinfo  &major, map<int, int>  &CanGangAllMap)
{
	if (ChairID < 0 || ChairID >= m_GAME_PLAYER)
		return 0;
	int  canhu = 0 ;
	 //if (major.m_GetGangInfo.Size() ==1)  
	for (int rt = 0; rt <  major.Size() ; rt++ )
	{
		canhu = 0; 
		int  remove = 0;
		_uint8  curGangCard = major[rt].cbCard;   
		if ( m_PlayGrapCard[ChairID] != curGangCard)
			continue;

		for (int i = 0; i < 4; i++)
		{
			bool ret =  m_GameLogic.ChuPai(&m_HandCard[ChairID], curGangCard);
			remove += (ret == true);  			
		}
		//log_debug("remve %d  mopai %x", remove , curGangCard );

		static   int   NeedCard[] =
		{
			0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09,//万	
			0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19,//条	
			0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29,//筒	
			0x31, 0x32, 0x33, 0x34, 0x41, 0x42, 0x43,
		};
		
		MJ_fan_buffer  fan;
		for (int i = 0; i < 34;i++)
		{
			m_GameLogic.AddCard(&m_HandCard[ChairID], NeedCard[i]); 
			if( GangTingJudget(ChairID, &m_HandCard[ChairID], fan, NeedCard[i]))
				canhu = 1 ;
			m_GameLogic.ChuPai(&m_HandCard[ChairID], NeedCard[i]);  
			if (canhu)
				break;
		}
		for (int i = 0; i < remove ; i++)
			m_GameLogic.AddCard(&m_HandCard[ChairID], curGangCard);  
		if (canhu)
		{
			CanGangAllMap[curGangCard] = 1 ;
			log_debug("can gang %x  remove %x" , curGangCard  , remove );
		}
		
	}
	return canhu; 

}

void Table_jilin::GetAllTingCard(map<_uint8, _uint8>  &  ting_map   ,  _uint8 ChairID)
{
	if (ChairID < 0 || ChairID >= m_GAME_PLAYER)
		return; 
	if(!m_jilin_baoting[ChairID].bao_ting)
		return;
	HandCards* pHc = &m_HandCard[ChairID];	
	MJ_fan_buffer  fan; 
	static   int  test[] =
	{
		0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09,//万	
		0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19,//条	
		0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29,//筒	
		0x31, 0x32, 0x33, 0x34, 0x41, 0x42, 0x43,
	};
	if (pHc->ChangeableCardsLen %3 == 2)
	{
		log_error("error %s ChairID [%d]" , CatHandCard(*pHc,ChairID) , ChairID );
		return;
	}
	int ret = 0;
	//log_debug("pai %s  len %d" , CatHandCard(*pHc ,ChairID) , pHc->ChangeableCardsLen );
	for (int i = 0; i < 34; i++)
	{
		m_GameLogic.AddCard(pHc, test[i]);
		ret = HuIfPreRemoveOneCardNextAddOne(ChairID, pHc, fan, test[i]);
		m_GameLogic.ChuPai(pHc, test[i]);
		if (ret)
		{
			ting_map[test[i]] = 1;
			log_debug("add ting card [%x]", test[i]);
		}
		
	}

}

int Table_jilin::IsCanGangAfterTing(int sid  , int   bcard )
{
	if (sid < 0 || sid >= m_GAME_PLAYER || bcard<=0 || bcard> 0x43 )
		return 0;
	HandCards  *phc = &m_HandCard[sid];
	static   int  can_hu_card[] =
	{
		0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09,//万	
		0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19,//条	
		0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29,//筒	
		0x31, 0x32, 0x33, 0x34, 0x41, 0x42, 0x43,
	};
	int remove = 0;
	for (int i = 0; i < phc->ChangeableCardsLen;){
		if ( phc->ChangeableCards[i] == bcard && m_GameLogic.ChuPai(phc, bcard))
			remove++;
		else
			i++;
	}	
	int ret = 0;
	MJ_fan_buffer  fan; 
	for (int i = 0; i < 34; i++)
	{
		m_GameLogic.AddCard(phc, can_hu_card[i]);
		if (GetCountSize(phc, can_hu_card[i]) > 4){
			m_GameLogic.ChuPai(phc, can_hu_card[i]);
		}
		else{
			ret = HuIfPreRemoveOneCardNextAddOne(sid, phc, fan, can_hu_card[i] ,  true );
			m_GameLogic.ChuPai(phc, can_hu_card[i]);
		}		
		if(ret> 0 )
			break; 
	}
	for (int i = 0; i < remove; i++)
		m_GameLogic.AddCard(phc, bcard); 
	return ret; 
}

int   Table_jilin::TingJudge(_uint8  ChairID )
{
	if ( ChairID >= m_GAME_PLAYER)
		return 0;
	map<int, int> all_map;
	HandCards* pHc = &m_HandCard[ChairID];
	for (int i = 0; i < pHc->ChangeableCardsLen; i++)
		all_map[pHc->ChangeableCards[i]] = 1;

	MJ_fan_buffer  fan;
	static   int  all_card[] =
	{
		0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09,//万	
		0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19,//条	
		0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29,//筒	
		0x31, 0x32, 0x33, 0x34, 0x41, 0x42, 0x43,
	};

	int ret = 0;
	//log_debug("pai %s", CatHandCard(*pHc, ChairID)); 

	map<int, int>::iterator it = all_map.begin();
	for (; it != all_map.end(); ++it)
	{
		m_GameLogic.ChuPai(pHc,  it->first ); 
		for (int i = 0; i < 34;  i++)
		{
			m_GameLogic.AddCard(pHc, all_card[i]);
			if (GetCountSize(pHc, all_card[i])> 4)
			{
				m_GameLogic.ChuPai(pHc, all_card[i]);				
			}
			else
			{
				ret = HuIfPreRemoveOneCardNextAddOne(ChairID, pHc, fan, all_card[i]);
				m_GameLogic.ChuPai(pHc, all_card[i]);
			}
			if(ret> 0 )
				break;
		}
		m_GameLogic.AddCard(pHc, it->first );
		if (ret > 0)
			return TYPE_BAO_TING; 
	}
	return 0;

	
}

int Table_jilin::CheckPlaySlefGrpCardCanOperateCode(_uint8 ChairID, _uint8 bCard, MJ_major& major, int& fan_num)
{
	int iOpType = 0;	
	iOpType = Table::CheckPlaySlefGrpCardCanOperateCode(ChairID, bCard, major, fan_num); 
	if ( m_jilin_baoting[ChairID].bao_ting &&  bCard == m_baoPai)
	{
		iOpType |= TYPE_HU;
		log_debug("mo dao bao pai ");
	}	
	//log_debug(" sid[%d] bcard[%x]  code[%x]   pai[%s]", GetPlayerUid(ChairID), bCard, iOpType  ,   CatHandCard(m_HandCard[ChairID] ,  ChairID ) );

	if( !m_jilin_baoting[ChairID].bao_ting )
	{   
		iOpType &= ~( TYPE_HU) ;  //  未报听。。不能胡牌。 //  
		HandCards* pHc = &m_HandCard[ChairID]; 
		bool is_add_ = false;
		bool add_one_can_hu = false;
		if (pHc->ChangeableCardsLen< 14 && bCard>0 && bCard <= 0x43)
		{
			is_add_ = true;
			m_GameLogic.AddCard(pHc, bCard);			
			map<int, int> all_map;
			for (int i = 0; i < pHc->ChangeableCardsLen; i++)
				all_map[pHc->ChangeableCards[i]] = 1; 				
			MJ_fan_buffer  fan;   			
			static   int  all_canhu_pai[] =
			{
				0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09,//万	
				0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19,//条	
				0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29,//筒	
				0x31, 0x32, 0x33, 0x34, 0x41, 0x42, 0x43,
			};			
			map<int, int>::iterator it = all_map.begin(); 
			int ret = 0;
			for (; it != all_map.end(); ++it )
			{								
				m_GameLogic.ChuPai(pHc,  it->first );
				for (int i = 0; i < 34; i++)
				{
					m_GameLogic.AddCard(pHc, all_canhu_pai[i]);
					if (GetCountSize(pHc, all_canhu_pai[i]) > 4)
					{
						m_GameLogic.ChuPai(pHc, all_canhu_pai[i]);
						continue; 
					}
					ret = HuIfPreRemoveOneCardNextAddOne(  ChairID  ,pHc, fan , all_canhu_pai[i] );  					
					add_one_can_hu = (ret > 0 );
					// log_debug("ret[%d]  pai %x   it-first %x" , ret , all_canhu_pai[i] , it->first  );

					m_GameLogic.ChuPai(pHc, all_canhu_pai[i]);
					if (add_one_can_hu == true )
						break;
				}
				m_GameLogic.AddCard(pHc, it->first ); 				
				if (add_one_can_hu == 1 )
					break;
			}
		} 					
		if (add_one_can_hu)
			iOpType |= TYPE_BAO_TING;  
		if ( CheckJilinGangOp(ChairID))
			iOpType |= TYPE_JILIN_GANG;


		if (is_add_)
			m_GameLogic.ChuPai(pHc, bCard);

	} 
	else
	{
		
		 iOpType &=  ~(TYPE_BAO_TING | TYPE_CHI  | TYPE_PENG   );  
		 iOpType |= BuGangAfterBaoTing(ChairID , m_PlayGrapCard[ChairID] );
		 if (iOpType & TYPE_GANG)
		 {
			 map<int, int>  allmap;
			 GangJudgeAfterTing(ChairID, major.m_GetGangInfo  , allmap ); 
			 if(allmap.size()== 0 )
			 {
				 iOpType &= ~(TYPE_GANG); 
				 log_debug(" ting cancel gang !!");
			 }
			 else
			 {
				 major.m_GetGangInfo.Clear();
				 for (map<int, int>::iterator it = allmap.begin(); it!=allmap.end(); ++it)
				 {
					 major.m_GetGangInfo.Add(OpState( it->first , get_card_size( &m_HandCard[ChairID],it->first)) );
					 log_debug("cangang %x" , it->first);  
				 }
			 }
		 }
	}
	if (m_bHasTianhu)
		iOpType |= TYPE_HU;
	//log_debug("ioptype %x " , iOpType);  


	return iOpType;
}


int  Table_jilin::ZhongFabaiChi_Judge(_uint8 pre_outcard, _uint8  ChairID )
{
	if (ChairID< 0 || ChairID >= m_GAME_PLAYER)
		return 0;
	if (pre_outcard > 0x40  && pre_outcard<= 0x43 )
	{
		int  all[4] = { 0 };
		int  face = get_face(pre_outcard);
		if (face >= 1 && face <= 3)
			all[face] = 1;
		const  HandCards *pc = &m_HandCard[ChairID]; 
		int fa = 1;
		for (int i = 0; i < pc->ChangeableCardsLen; i++)
		{
			if (pc->ChangeableCards[i]< 0x40)  continue; 
			fa = get_face(pc->ChangeableCards[i]); 
			if (fa >= 1 && fa <= 3) 
				all[fa] = 1;
		}
		if (all[1] && all[2] && all[3])
		{
			if (face == 2 )
				return TYPE_CENTER_CHI;
			else if (face == 1)
				return TYPE_LEFT_CHI;
			else
				return TYPE_RIGHT_CHI;
		}
	}
	return  0;
	
}


int Table_jilin::CheckOutCardOtherPlayerCanOpCode(_uint8 ChairID, _uint8 bCard, MJ_major& major, int& fan_num)
{
	
	int iOpType = 0;	
	iOpType = Table::CheckOutCardOtherPlayerCanOpCode(ChairID, bCard, major, fan_num);
	//log_debug("op_code  %x  %s" , iOpType  , CatHandCard( m_HandCard[ChairID] , ChairID) );
	if (m_bCurChairID != PreChair(ChairID))   // 不是出牌的下家不能吃。 
	{
		iOpType &= ~TYPE_CHI;
	}
	else
	{	
		iOpType |= ZhongFabaiChi_Judge(bCard, ChairID); 		
	}
	

	//int   winlost = GetPlayerWinlostMoney(m_bOutCardChairID );   // m_bCurChairID
	// 如果玩家A为0锅，那么A报听前，其他人不能胡他打出的点炮，A报听后点炮其他玩家可以胡 
	//if ( winlost < 0 && abs(winlost) >= g_jilin_conf.guo  
		// && m_bOutCardChairID< m_GAME_PLAYER &&  !m_jilin_baoting[m_bOutCardChairID].bao_ting ) 
	if(LingGuoHu(m_bOutCardChairID) == 0 )
	{
		log_debug("cancel %x   m_bOutCardChairID[%x]    sid [%d ] ", iOpType, m_bOutCardChairID  ,  ChairID  );
		iOpType &= (~TYPE_HU);
	}

	if (!m_jilin_baoting[ChairID].bao_ting) // || m_SiLou[ChairID] )
	{
		//log_debug("no baoting  sid %d %x  m_bOutCardChairID[%d] ", ChairID, iOpType, m_bOutCardChairID );
		iOpType &= (~TYPE_HU); 
	}
	if ( m_jilin_baoting[ChairID].bao_ting)
	{
		iOpType &= ~(TYPE_CHI | TYPE_PENG  ); 
		if( (iOpType & TYPE_ZHIGANG) &&    !IsCanGangAfterTing(ChairID, bCard) )
		{
			iOpType &= (~TYPE_ZHIGANG); 
			log_debug("can gang !!");
		} 		
		if ( KaiMen(&m_HandCard[ChairID] , ChairID ) == 0)
			iOpType &= (~TYPE_ZHIGANG);
	}
	
	iOpType &= ~(TYPE_BU_ZHANG);
	if ( m_bIsTongBaoTime)
	{
		iOpType &= ~(TYPE_CHI | TYPE_PENG | TYPE_GANG | TYPE_JILIN_GANG );
	}
	//log_debug("ioptype [%x]", iOpType);

	return iOpType;
}

int  Table_jilin::GrapPaiAutoHu(_uint8  ChairID)
{
	if (ChairID >= m_GAME_PLAYER)
		return  0; 
	map<_uint8, _uint8>  curmap;
	GetAllTingCard(curmap, ChairID);
	for (map<_uint8, _uint8>::iterator it = curmap.begin(); it != curmap.end(); ++it)
	{
		if (it->first == m_baoPai || (m_baoPai == 0x11 && g_jilin_conf.yaoji_wanneng))
		{
			m_bIsTongBaoTime = true;
			m_nPlayerTongBao[ChairID] = 1;
			m_bUserOperate[ChairID] |= TYPE_HU;
			m_bCurChairID = ChairID;
			m_bCurCard = m_baoPai;
			m_GameLogic.AddCard(&m_HandCard[ChairID], m_baoPai);
			log_debug(" auto  hu !! !!  %x  sid[%d]  uid[%d]", m_baoPai, ChairID, GetPlayerUid(ChairID));
			Table::UserOperateHu(ChairID, m_baoPai);     // 自摸的时候通宝。。！！  bug!!!
			GameEnd();
			return 1;
		}
	}
	return 0; 

}

void Table_jilin::OnUserGrapCard(_uint8 ChairID, bool bGetLast)
{
	if (ChairID< 0 || ChairID>= m_GAME_PLAYER)
	{
		log_error("error %d" , ChairID); 
		return; 
	}
	m_TingHouCanOutPaiMap.clear(); 	
	if (m_CardPool.Count()<= 13 )
	{
		GameEnd(); 
		log_debug("haidi over");
		return; 
	}
	if (m_baoPai != 0 &&  m_jilin_baoting[ChairID].bao_ting)
	{		
		if (GrapPaiAutoHu(ChairID))
			return; 
		else
		{
			 //HuanBao(ChairID); 
			int iCount = 0;
			int pre_bao = m_baoPai;
			while (m_CardPool.Count() > 13)
			{
				if (iCount++ > 100)
					break;
				pre_bao = m_baoPai;
				ProtoBaoSwap(ChairID);
				if (pre_bao != m_baoPai)
				{					
					if (GrapPaiAutoHu(ChairID))
						return; 										
				}
				else
					break;
			}

		}		
	}

	Table::OnUserGrapCard(ChairID, bGetLast); 
	if (m_bUserOperate[ChairID]& TYPE_HU) 
	{
		//log_debug("grap auto  hu !! %x   len[ %d ] ", m_bCurCard, m_HandCard[ChairID].ChangeableCardsLen); 		
		Table::UserOperateHu(ChairID, m_bCurCard);
		GameEnd();				
	}  
	if (m_jilin_baoting[ChairID].bao_ting && m_outCardCountAfterBaoTing[ChairID])
		m_outCardCountAfterBaoTing[ChairID] += 10; //  区分 包与不包。 ！！

}





void Table_jilin::UserAfterOperate(_uint8 ChairID, int iOpType, _uint8 bCard)
{
	m_bCurChairID = ChairID;
	log_info(" uid:%d chairid:%d operate: %x card: %x ", GetPlayerUid(ChairID), ChairID, iOpType, bCard);

	m_bUserOperate[ChairID] = TYPE_NULL;
	MJ_opinfo gang;
	m_bUserOperate[ChairID] = GetGangInfo(m_bCurChairID, gang);
	if ( !m_jilin_baoting[ChairID].bao_ting )
	{		
		if (TingJudge(ChairID) > 0 )
			m_bUserOperate[ChairID] |= TYPE_BAO_TING;
		if ( CheckJilinGangOp( ChairID))
		{
			m_bUserOperate[ChairID] |= TYPE_JILIN_GANG; 
		}
		//log_debug("op [%x]  " , m_bUserOperate[ChairID]  );
	}
	if (m_bUserOperate[ChairID] & TYPE_JILIN_GANG)
	{
		proto::game::AckOpNotify stNotify;
		for (int i = 0; i < m_GAME_PLAYER; i++)
		{
			Proto_Game_OpNotifyAfterPengChi(ChairID, i, gang, stNotify);
			unicast(SERVER_OPERATE_NOTIFY, i, stNotify);
		}
		ProtoJinLinAfterSpeGangCanOp(ChairID);
		set_operator_timer(ChairID);
		return; 
	}
	
	//    没哟 刚。也没有报听 
	if ( gang.Size() == 0    && (m_bUserOperate[ChairID] & TYPE_BAO_TING) == 0 )  
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
		// ProtoJinLinAfterSpeGangCanOp(ChairID);  
		set_operator_timer(ChairID);
	}
}


void Table_jilin::UserOperatePeng(_uint8 ChairID, _uint8 bCard)
{	
	if (ChairID>= m_GAME_PLAYER)
	{
		log_debug("error"); 
		return;
	}
	m_PlayGrapCard[ChairID] = 0;
	Table::UserOperatePeng(ChairID, bCard);

}


void Table_jilin::Proto_Game_OpNotifyAfterPengChi(_uint8 ChairID, _uint8 CurChairID, const MJ_opinfo& gang, proto::game::AckOpNotify& stNotify)
{
	stNotify.Clear();
	stNotify.set_chairid(ChairID);
	log_debug("chairid [%d]  CurChairID[%d]  op[%x] " , ChairID , CurChairID  , m_bUserOperate[ChairID] );

	if (ChairID == CurChairID)
	{	
		//非打牌玩家
		int iOpType = m_bUserOperate[ChairID];
		//log_debug("ioptype [%x]" , iOpType );
		if (m_bUserOperate[ChairID] & TYPE_GANG)
		{
			// m_bUserOperate[ChairID] |= TYPE_BU_ZHANG;
			//iOpType = m_bUserOperate[ChairID] & ~TYPE_GANG; 

			proto::game::hunanmj::tagBuZhang* bu = stNotify.mutable_bu();
			for (int i = 0; i < gang.Size(); i++)
			{
				proto::game::hunanmj::tagBuinfo* buinfo = bu->add_bu();
				buinfo->set_card(gang[i].cbCard);
				buinfo->set_state(TYPE_GANG);

				//if (1) //IsListening(ChairID, gang[i].cbCard))
				{
					proto::game::tagGangInfo* ganginfo = stNotify.add_gang();
					ganginfo->set_card(gang[i].cbCard);
					ganginfo->set_state(gang[i].nState);
					iOpType |= gang[i].nState;
				}
			}


		}
		if (iOpType  & TYPE_LISTEN ) // add  
		{
			iOpType |= TYPE_BAO_TING;
		}
		stNotify.set_card(0);
		stNotify.set_optype(iOpType);
	}
}


void Table_jilin::ProAddSpecialGangInfo(_uint8 ChairID, proto::game::AckUserGetCard& stUsergetcard,  
	                      bool is_rec, proto::game::jilin::JilinReconnectInfo  &stReconnect )
{ 
	if (ChairID < 0 || ChairID >= m_GAME_PLAYER)
		return; 
	// 特殊钢。。操作。 。

	int arr[5][10];
	memset(arr, 0, sizeof(arr));
	int  yaoji = 0;
	proto::game::jilin::SpeAllGangInfo  * pAdd = NULL;
	if(!is_rec)
		pAdd  =  stUsergetcard.mutable_ganglist();
	else
		pAdd =  stReconnect.mutable_gang_info();

	const HandCards* phc = &m_HandCard[ChairID];
	
	for (int i = 0; i < phc->ChangeableCardsLen; i++)
	{
		int suit = get_suits(phc->ChangeableCards[i]);
		int face = get_face(phc->ChangeableCards[i]);
		if (suit >= 0 && suit <= 4 && face > 0  && face < 10 )
		{
			arr[suit][face] = 1;
			arr[suit][0]++;
			yaoji += (phc->ChangeableCards[i] == 0x11);
		}
	}
	if (m_jilin_baoting[ChairID].bao_ting)  		
		goto next_label; 

	//log_debug("len [%d]  yaoji %d  all  [%s]  19dan[%d]  %d %d  %d ", 
		//phc->ChangeableCardsLen, yaoji , CatHandCard(*phc, ChairID ) , g_jilin_conf.yao_jiu_dan , arr[0][9] , arr[1][9], arr[2][9] ) ;

	if (arr[3][1] + arr[3][2] + arr[3][3] + arr[3][4] >= 3)   // 3个东风。杠  。不加  癞子。。
	{
		for (int po = 1; po <= 4; po++)
		{
			int all = 0;
			for (int st = 1; st <= 4; st++)
			{
				if (po == st) continue;
				all += arr[3][st];
				if (all >= 3)
				{
					proto::game::jilin::Speganginfo *pSpe = pAdd->add_gang_list();
					proto::game::jilin::gangcard *pt = pSpe->add_gang_card_list();
					pSpe->set_gang_type(1);
					for (int r = 1; r <= 4; r++)
					{
						if (r == po)  continue;
						pt->add_card(0x10 * 3 + r);
						 ///log_debug("a[%x]  ", 3 * 0x10 + r);
					}
				}
			}

		}
	}// end >= 3
	if (g_jilin_conf.xiaojifeidan && (arr[3][1] + arr[3][2] + arr[3][3] + arr[3][4] >= 2) && yaoji >= 1)  // 2个东风。 
	{
		for (int i = 1; i <= 4; i++)
		{
			for (int j = i + 1; j <= 4; j++)
			{
				if (arr[3][i] && arr[3][j])
				{
					proto::game::jilin::Speganginfo *pSpe = pAdd->add_gang_list();
					proto::game::jilin::gangcard *pt = pSpe->add_gang_card_list();
					pSpe->set_gang_type(1);
					pt->add_card(0x10 * 3 + i);
					pt->add_card(0x10 * 3 + j);
					pt->add_card(0x11);
				}
			}
		}
	}
	if (g_jilin_conf.xiaojifeidan
		&& (arr[3][1] + arr[3][2] + arr[3][3] + arr[3][4] >= 1)
		&& yaoji >= 2)    // 1个东风。。 不加癞子 
	{
		for (int r = 1; r <= 4; r++)
		{
			if (arr[3][r])
			{
				proto::game::jilin::Speganginfo *pSpe = pAdd->add_gang_list();
				proto::game::jilin::gangcard *pt = pSpe->add_gang_card_list();
				pSpe->set_gang_type(1);
				pt->add_card(0x30 + r);
				pt->add_card(0x11);
				pt->add_card(0x11);
			}
		}
	}
	if (arr[4][1] + arr[4][2] + arr[4][3] >= 3)   // 中发白。杠 。不加癞子。 
	{
		proto::game::jilin::Speganginfo *pSpe = pAdd->add_gang_list();
		proto::game::jilin::gangcard *pt = pSpe->add_gang_card_list();
		pSpe->set_gang_type(1);
		pt->add_card(0x41);
		pt->add_card(0x42);
		pt->add_card(0x43);
	}
	if ((arr[4][1] + arr[4][2] + arr[4][3] >= 2) && yaoji >= 1 && g_jilin_conf.xiaojifeidan )   // 中发白。杠 。不加癞子。 
	{
		for (int i = 1; i <= 3; i++)
		{
			for (int j = i + 1; j <= 3; j++)
			{
				if (arr[4][i] && arr[4][j])
				{
					proto::game::jilin::Speganginfo *pSpe = pAdd->add_gang_list();
					proto::game::jilin::gangcard *pt = pSpe->add_gang_card_list();
					pSpe->set_gang_type(1);
					pt->add_card(0x40 + i);
					pt->add_card(0x40 + j);
					pt->add_card(0x11);
				}
			}
		}
	}
	if (g_jilin_conf.xiaojifeidan && (arr[4][1] + arr[4][2] + arr[4][3] >= 1) && yaoji >= 2)
	{
		for (int i = 1; i <= 3; i++)
		{
			if (arr[4][i])
			{
				proto::game::jilin::Speganginfo *pSpe = pAdd->add_gang_list();
				proto::game::jilin::gangcard *pt = pSpe->add_gang_card_list();
				pSpe->set_gang_type(1);
				pt->add_card(0x40 + i);
				pt->add_card(0x11);
				pt->add_card(0x11);
			}
		}
	}


	if (g_jilin_conf.yao_jiu_dan && (arr[0][1] + arr[1][1] + arr[2][1] >= 3))
	{
		proto::game::jilin::Speganginfo *pSpe = pAdd->add_gang_list();
		proto::game::jilin::gangcard *pt = pSpe->add_gang_card_list();
		pSpe->set_gang_type(1);
		pt->add_card(0x1);
		pt->add_card(0x11);
		pt->add_card(0x21);
	}
	if (g_jilin_conf.yao_jiu_dan && g_jilin_conf.xiaojifeidan && ( (arr[0][1] + arr[2][1])>= 1 && yaoji >= 2))
	{
		if (arr[0][1])
		{
			proto::game::jilin::Speganginfo *pSpe = pAdd->add_gang_list();
			proto::game::jilin::gangcard *pt = pSpe->add_gang_card_list();
			pSpe->set_gang_type(1);
			pt->add_card( 0x1);
			pt->add_card( 0x11);
			pt->add_card( 0x11 );
		}
		if (arr[2][1])
		{
			proto::game::jilin::Speganginfo *pSpe = pAdd->add_gang_list();
			proto::game::jilin::gangcard *pt = pSpe->add_gang_card_list();
			pSpe->set_gang_type(1);
			pt->add_card(0x21);
			pt->add_card(0x11);
			pt->add_card(0x11 );
		}

	}
	if (g_jilin_conf.yao_jiu_dan && (arr[0][9] + arr[1][9] + arr[2][9] >= 3))
	{
		proto::game::jilin::Speganginfo *pSpe = pAdd->add_gang_list();
		proto::game::jilin::gangcard *pt = pSpe->add_gang_card_list();
		pSpe->set_gang_type(1);
		pt->add_card(0x9);
		pt->add_card(0x19);
		pt->add_card(0x29);
	}
	if (g_jilin_conf.yao_jiu_dan && g_jilin_conf.xiaojifeidan &&  ( ( (arr[0][9] + arr[1][9] + arr[2][9]) >= 2) && yaoji >= 1 ))
	{
		for (int i = 0; i < 3; i++)
		{
			for (int j = i + 1; j < 3; j++)
			{
				if (arr[i][9] && arr[j][9])
				{
					proto::game::jilin::Speganginfo *pSpe = pAdd->add_gang_list();
					proto::game::jilin::gangcard *pt = pSpe->add_gang_card_list();
					pSpe->set_gang_type(1);
					pt->add_card(0x10 * i + 9);
					pt->add_card(0x10 * j + 9);
					pt->add_card(0x11);
					//log_debug("add  19 gang !!!!");
				}
			}
		}
	}
	if (g_jilin_conf.yao_jiu_dan && g_jilin_conf.xiaojifeidan && (arr[0][9] + arr[1][9] + arr[2][9] >= 1 && yaoji >= 2))
	{
		for (int i = 0; i < 3; i++)
		{
			if (arr[i][9])
			{
				proto::game::jilin::Speganginfo *pSpe = pAdd->add_gang_list();
				proto::game::jilin::gangcard *pt = pSpe->add_gang_card_list();
				pSpe->set_gang_type(1);
				pt->add_card(0x10 * i + 9);
				pt->add_card(0x11);
				pt->add_card(0x11);
			}
		}
	}

	//JiGangInfo     m_JilinGangInfo[4]; 
	// start   bugang !!
	 // log_debug("gangsize %d" , m_JilinGangInfo[ChairID].len ); 

next_label:
	if ( m_JilinGangInfo[ChairID].len == 0)
		return;

	map<_uint8, int>  bu_gangid_map;
	JiGangInfo   &pGangInfo = m_JilinGangInfo[ChairID];
	for (int i = 0; i < pGangInfo.len; i++)
	{
		gang_card   &pCurInfo = pGangInfo.g_arr[i];
		for (int j = 0; j < XUAN_FENG_GANG_ARR_LEN; j++)
		{
			int ct = pCurInfo.arr[j]; 
			// log_debug("cur--gang--pai %x" , ct  ) ;
			if (ct == 0x11 || ct == 0) continue;
			if (ct>0x30 && ct < 0x40 && arr[3][0])
			{
				for (int r = 1; r <= 4; r++)
				{
					if (arr[3][r] == 0)continue;
					if (bu_gangid_map[0x30 + r] > 0) continue;
					bu_gangid_map[0x30 + r] = pCurInfo.gang_id;
				}
			}
			if (ct > 0x40 && arr[4][0])
			{
				for (int r = 1; r <= 3; r++)
				{
					if (arr[4][r] == 0)continue;					
					if (bu_gangid_map[0x40 + r] > 0)  continue;
					bu_gangid_map[0x40 + r] = pCurInfo.gang_id;
				}
			}
			if (ct < 0x30 && get_face(ct) == 1 && g_jilin_conf.yao_jiu_dan )
			{
				for (int r = 0; r < 3; r++)
				{
					if (r == 1)continue; // 先过掉，幺鸡，， 
					// if (!g_jilin_conf.yao_jiu_dan)  break;  		
					if (arr[r][1] == 0)  continue;
					if (bu_gangid_map[0x10 * r + 1]> 0)  continue;
					bu_gangid_map[0x10 * r + 1] = pCurInfo.gang_id;
				}
			}

			if (get_face(ct) == 9 && ct < 0x30  && g_jilin_conf.yao_jiu_dan )
			{
				for (int r = 0; r < 3; r++)
				{
					//  if (r == 1)continue;  bug  !!!! 
					if (arr[r][9] == 0)continue;
					if (bu_gangid_map[0x10 * r + 9] > 0)continue;
					bu_gangid_map[0x10 * r + 9 ] = pCurInfo.gang_id;

				}
			}
			if (yaoji && g_jilin_conf.xiaojifeidan && bu_gangid_map[0x11] == 0)
			{
				bu_gangid_map[0x11] = pCurInfo.gang_id;
			}
		}
	}
	map<_uint8, int>::iterator it = bu_gangid_map.begin();
	for (; it != bu_gangid_map.end();++it)
	{
		if (m_jilin_baoting[ChairID].bao_ting &&  it->first != m_PlayGrapCard[ChairID]) 
			continue; 
		
		proto::game::jilin::Speganginfo *pSpe = pAdd->add_gang_list();
		proto::game::jilin::gangcard *pt = pSpe->add_gang_card_list();
		pSpe->set_gang_type(2);
		pSpe->set_gang_id( it->second ); 
		pt->add_card( it->first ); 		
		//log_debug("first %x sec %x   grap %x " , it->first , it->second  ,  m_PlayGrapCard[ChairID] ) ; 

	}

}



int  Table_jilin::Add_Ting_Pai_Info(_uint8 ChairID ,    proto::game::AckUserGetCard & Ack)
{ 
	tagListenInfos stListenInfo[14];
	_uint8  bListenLen = 0;
	bool can_ting = false;
	//胡牌不用判断听牌，自动胡牌不用判断听牌
	if (GetListeningInfo(ChairID, stListenInfo, bListenLen))
	{
		// m_bUserOperate[ChairID] |= TYPE_LISTEN;
		can_ting = true;
	}  
	// 发送听牌协议
	// proto::game::AckListenNotify stListen;
	if (  can_ting )
	{
		// log_info(" uid:%d chairid:%d ListenLen:%d " , GetPlayerUid(ChairID), ChairID, bListenLen);
		// stListen.set_optype(TYPE_LISTEN);
		//打包听牌信息
		for (int j = 0; j < bListenLen; j++)
		{
			//log_debug("peng out %x   blen %d", stListenInfo[j].bOutCard, bListenLen);
			PackageTingInfo(ChairID, stListenInfo, j, Ack.add_listeninfo());
		}
	}
	else
	{
		//stListen.set_optype(TYPE_NULL);
		//log_info("  uid:%d chairid:%d no listen ", GetPlayerUid(ChairID), ChairID);
	}
	
	//log_debug("ting listen   %s  ", Ack.ShortDebugString().c_str());  
	return 0; 
}


int Table_jilin::ProtoJinLinAfterSpeGangCanOp(_uint8  ChairID)
{
	if (  ChairID >= m_GAME_PLAYER)
		return 0;
	//debug 
	// CatHandCard(m_HandCard[ChairID], ChairID);
	log_debug("allpai  %s" , CatHandCard( m_HandCard[ChairID] , ChairID) );
	
	// (_uint8 ChairID, _uint8 MoChairID, _uint8 bCard, proto::game::AckUserGetCard& stUsergetcard)
	bool  IsExit = false;
	proto::game::AckUserGetCard  stUsergetcard; 
	for (int i = 0; i < m_HandCard[ChairID].ChangeableCardsLen; i++)
	{
		if (m_bCurCard == m_HandCard[ChairID].ChangeableCards[i])
		{
			IsExit = true;
			break;
		}
	}
	if (IsExit) 
	{ 
		log_debug("into  this  exit %d  m_bCurCard [%x]", IsExit, m_bCurCard );
		m_bUserOperate[ChairID] = 0;
		MJ_major  major; 
		int fan = 0;
		bool remove = false;
		//if (m_HandCard[ChairID].ChangeableCardsLen % 3 != 1 && m_bCurCard < 0x43 && m_GameLogic.ChuPai(&m_HandCard[ChairID], m_bCurCard))	
		if( m_bCurCard < 0x43 && m_GameLogic.ChuPai(&m_HandCard[ChairID], m_bCurCard))  // bug !!!!!  
			remove = true;		
		m_bUserOperate[ChairID] = CheckPlaySlefGrpCardCanOperateCode(ChairID, m_bCurCard,   major , fan );	 
		if (remove)	
			m_GameLogic.AddCard(&m_HandCard[ChairID], m_bCurCard); 		
		Proto_Game_UserGetCard(ChairID, ChairID, m_bCurCard, stUsergetcard);
	}	
	else
	{
		m_bUserOperate[ChairID] = 0;
		MJ_opinfo stGang;
		GetGangInfo(ChairID, stGang);
		for (int i = 0; i < stGang.Size(); i++)
		{
			//log_info(" uid:%d chairID:%d angang: %x", GetPlayerUid(ChairID), ChairID, stGang[i].cbCard);
			proto::game::tagGangInfo *pstGang = stUsergetcard.add_ganginfo();
			pstGang->set_card(stGang[i].cbCard);
			pstGang->set_state(stGang[i].nState);
			m_bUserOperate[ChairID] |= TYPE_GANG;
		}
		if ( !m_jilin_baoting[ChairID].bao_ting  && CheckJilinGangOp(ChairID))
			m_bUserOperate[ChairID]  |= TYPE_JILIN_GANG;

		//	log_info("op %x" , m_bUserOperate[ChairID] ); 
		proto::game::jilin::JilinReconnectInfo  Ack; 
		ProAddSpecialGangInfo(ChairID, stUsergetcard , false ,  Ack );   
		
	} 
	m_bUserOperate[ChairID] |= TingJudge(ChairID);  
	stUsergetcard.set_optype(m_bUserOperate[ChairID]);
	stUsergetcard.set_chairid(ChairID);
	if (!IsExit && m_bUserOperate[ChairID]& TYPE_BAO_TING)
	{
		Add_Ting_Pai_Info(ChairID, stUsergetcard);  
	}
	

	unicast( SER_CMD_JILING_GANG_INFO_AFTER_GANG , ChairID , stUsergetcard);  //   

	//log_debug("cur_info   op[%x]  %s  ", m_bUserOperate[ChairID] , stUsergetcard.ShortDebugString().c_str());
	

	//set_out_timer(ChairID);  
	return 0;

}


int Table_jilin::DebugTest(_uint8   ChairID)
{ 
	// debug 
	//JiGangInfo &ganginfo = m_JilinGangInfo[ChairID];
	//for (int i = 0; i < ganginfo.len; i++)
	//{
		//gang_card   &pInfo = ganginfo.g_arr[i];
		//for (int j = 0; j < XUAN_FENG_GANG_ARR_LEN; j++)
	//	{
			//if(pInfo.arr[j] != 0 )
				//log_debug("i , arr bu  %d %x %d", i, pInfo.arr[j], pInfo.bu[j]);
	//	}

	//}
	// end debug
	return 0;
}

int Table_jilin::Proto_Jilin_bu(_uint8  ChairID ,  _uint8  bu , int gang_id )
{
	if (ChairID < 0 || ChairID >= m_GAME_PLAYER)
		return 0;
	// log_debug("bu %x" , bu);
	proto::game::jilin::JilinOperateRes  Ack;
	Ack.set_optype(2);   // 杠是1  补是2  
	Ack.set_gang_id(gang_id);
	Ack.set_chairid(ChairID);
	JiGangInfo &ganginfo = m_JilinGangInfo[ChairID];
	for (int i = 0; i < ganginfo.len; i++)
	{
		gang_card   &pInfo = ganginfo.g_arr[i];
		bool  MoreThan3 = false;
		if (gang_id == pInfo.gang_id)
		{
			for (int j = 0; j < XUAN_FENG_GANG_ARR_LEN; j++)
			{
				if (bu == pInfo.arr[j])
				{
					pInfo.bu[j]++;
					MoreThan3 = (j >= 3);
					break;
				}
				if (pInfo.arr[j] == 0)
				{
					pInfo.bu[j]++;
					pInfo.arr[j] = bu;
					MoreThan3 = (j >= 3);
					log_debug("bu [%x]", bu);
					break;
				}
			}
			//处理。。小花   冬   幺鸡 幺鸡  北风的问题。 的问题。   0x31 0x11 0x11 0x32  --> 0x21 0x11*2 0x32 
			if (MoreThan3)
			{
				int  one_card = 0;
				for (int i = 0; i < XUAN_FENG_GANG_ARR_LEN; i++)
				{
					if (pInfo.arr[i] == 0) break;
					one_card = 1;
					for (int j = i + 1; j < XUAN_FENG_GANG_ARR_LEN; j++)
					{
						if (pInfo.arr[i] == pInfo.arr[j])
							one_card++;
						if (one_card> 1)
						{
							int pos = j;
							pInfo.bu[i] += pInfo.bu[j];
							for (int r = pos; r < XUAN_FENG_GANG_ARR_LEN - 1; r++)
							{
								pInfo.arr[r] = pInfo.arr[r + 1];
								pInfo.bu[r] = pInfo.bu[r + 1];
							}
							break;
						}
					}
				}
			}
			// end 排序   !!
			proto::game::jilin::gangcard  *pt = Ack.add_card_list();
			for (int j = 0; j < XUAN_FENG_GANG_ARR_LEN; j++)
			{
				if (pInfo.arr[j] == 0)
					break;
				pt->add_card(pInfo.arr[j]);
				pt->add_bu_count(pInfo.bu[j]);
				// log_debug("arr [%d]  arr[%x] bu[%d]", j, pInfo.arr[j], pInfo.bu[j]);

			}
		}
	}
	if (bu > 0)
		m_GameLogic.ChuPai(&m_HandCard[ChairID], bu);
	DebugTest(ChairID);

	Ack.set_bucard(bu);
	broadcast(SER_CMD_JILIN_GAGN_SUCCESS, Ack);
	if (m_jilin_baoting[ChairID].bao_ting)
	{
		m_outCardCountAfterBaoTing[ChairID] += 2;
		m_nGangShangHua[ChairID] = 1; 
	}
		

	

	std::string str = Ack.SerializeAsString();
	save_video_data(SER_CMD_JILIN_GAGN_SUCCESS   , str);

	return 0;
}

char* Debug_All_Hand( int * pHc, int len  )
{
	static char  all_buf[0x1000];
	all_buf[0] = '\0';
	for (int i = 0; i <  len  ; i++)
	{
		char tmp[0x10] = { 0 };
		sprintf(tmp, "%02x,",   pHc[i] );
		strcat(all_buf, tmp);
	}
	return  all_buf;
}


int bu = 0;  
int bu_chairid  = 0;

int Table_jilin::ProtoJilinGangOperate(int  ChairID , int optype   , int *pArr , int card_len   , int gang_id)
{
	if (  ChairID >= m_GAME_PLAYER)
		return 0;

	 //log_debug("recv jilin gang %x  gangid[%d]" , optype , gang_id  );  
	 // log_debug("pai all %s ", Debug_All_Hand(pArr, card_len));  
	 // for (int i = 0; i < card_len; i++)
		//log_debug("card [%x] " , pArr[i] );   

	if(optype ==  1  )  // 旋风刚
	{		
		gang_card   ginfo;
		card_len = min( XUAN_FENG_GANG_ARR_LEN , card_len);
		int  yaojiu_dan = 0;
		for (int i = 0; i < card_len; i++)
		{
			ginfo.arr[i] = pArr[i];		
			ginfo.bu[i] = 1;
			if( pArr[i] != 0x11 && pArr[i] < 0x30)
				yaojiu_dan = 1;
		}
		ginfo.g_type = optype;
		ginfo.gang_id = m_gangID++;
		ginfo.yaojiudan = yaojiu_dan;  
		int  pos = ++m_JilinGangInfo[ChairID].len;
		if (pos>=1 && pos<  10 )
			m_JilinGangInfo[ChairID].g_arr[pos-1 ] = ginfo; 
		// pb send  
		for (int i = 0; i < card_len; i++)		
			m_GameLogic.ChuPai(&m_HandCard[ChairID], pArr[i]); 	

		proto::game::jilin::JilinOperateRes  Ack;						
		Ack.set_res(1);
		Ack.set_gang_id(ginfo.gang_id);
		Ack.set_optype(1);		
		Ack.set_chairid(ChairID); 
		proto::game::jilin::gangcard *pt = Ack.add_card_list();
		for (int i = 0; i < card_len; i++)
		{
			pt->add_card(pArr[i]);  
		}
		broadcast( SER_CMD_JILIN_GAGN_SUCCESS, Ack);	
		DebugTest(ChairID);
		m_nGangShangHua[ChairID] = 0;

		std::string str = Ack.SerializeAsString();
		save_video_data(SER_CMD_JILIN_GAGN_SUCCESS, str);
		ProtoJinLinAfterSpeGangCanOp(ChairID);

		// end pb send  
	}
	else 
	{
		
		if (card_len > 0 )
			 bu = pArr[0];
		int ret =  EstimateUserJilinGangRespond(ChairID,  bu , gang_id ) ;   // true 能抢。。 fale 不能抢。 
		if (!ret)
		{
			if (m_jilin_baoting[ChairID].bao_ting)
			{
				m_outCardCountAfterBaoTing[ChairID] += 2;
				log_debug("baoting [%d] all[%d]" , ChairID , m_outCardCountAfterBaoTing[ChairID]);
			}
			Proto_Jilin_bu( ChairID ,bu , gang_id );
			OnUserGrapCard(ChairID, false); 
		}
		else
		{
			proto::game::jilin::jilinbuganginfo   Ack;
			Ack.set_chairid(ChairID); 
			Ack.set_card(bu); 
			bu_chairid = ChairID; 
			broadcast(SER_CMD_JILIN_NOWING_BU , Ack);
		}
		
		
	}

	
	return 0;
}



void Table_jilin::UserOperateChi(_uint8 ChairID, int iOpType, _uint8 bCard)
{
	
	if (ChairID >= m_GAME_PLAYER)
	{
		log_error("error %d" , ChairID );
		return;
	}
	m_PlayGrapCard[ChairID] = 0;
	
    if (bCard> 0x40 && (iOpType == TYPE_LEFT_CHI || iOpType == TYPE_CENTER_CHI || TYPE_RIGHT_CHI ) )
    {
		log_debug("into this  %x %x %x" , ChairID , iOpType , bCard);
		UserOperateZhongfaBai( ChairID , iOpType , bCard );
		return;

    }

	Table::UserOperateChi(ChairID, iOpType, bCard); 

    	
}

void Table_jilin::DoSimpleOperate(_uint8 ChairID, int iOpType, _uint8 bCard)
{
	if(ChairID< 0 || ChairID>= m_GAME_PLAYER )
	{
		log_error("error %d" , ChairID); 
		return;
	}
	if ( (iOpType == TYPE_BAO_TING)   &&   (m_bUserOperate[ChairID]  &  TYPE_BAO_TING) )
	{
		m_jilin_baoting[ChairID].bao_ting = true;		
		log_debug(" bao Ting uid[%d] ", GetPlayerUid(ChairID ) ); 
		proto::game::hunanmj::serMsgBaojiao  pbAll;
		pbAll.set_uid(GetPlayerUid(ChairID));
		pbAll.set_chairid(ChairID); 

		m_nRecvBaoTingPlayerMap[ChairID] = TYPE_BAO_TING;
		m_bUserOperate[ChairID] &=  ~(  TYPE_JILIN_GANG | TYPE_GANG | TYPE_BAO_TING ); 
		broadcast(SERVER_BAO_TING_SUCC, pbAll);

		TingHouFirstGang(ChairID) ; 		

		std::string str = pbAll.SerializeAsString();
		save_video_data(SERVER_BAO_TING_SUCC , str);  // baoting !!shangbao 

		m_nRecvBaoTingPlayerMap[ChairID] = TYPE_BAO_TING;
		 // m_bUserOperate[ChairID] &=  ~(TYPE_GANG | TYPE_JILIN_GANG  );  

		m_nXianBayi[ChairID] = m_HandCard[ChairID].ChangeableCardsLen; 
		if (m_baoPai == 0)
		{
			proto::game::jilin::BaoPaiMsg   ack;			
			 //m_baoPai = m_CardPool.GetOneCard();   
			m_baoPai = m_CardPool.GetLast();
			ack.set_card(m_baoPai);
			ack.set_new_card(m_baoPai ); 
			ack.set_type(1);  
			log_debug("baopai [%x]" , m_baoPai );
			broadcast(SER_CMD_JILIN_BAOPAI_MSG, ack);			
			std::string str = ack.SerializeAsString();
			save_video_data(SER_CMD_JILIN_BAOPAI_MSG, str);

			if (m_CardPool.Count()<= 13 )
				GameEnd();
		}
		log_debug("buser[%x]" , m_bUserOperate[ChairID] );
		return;


	}
	if ( (iOpType  == TYPE_PASS)  &&  (m_bUserOperate[ChairID] & TYPE_BAO_TING))
	{
		m_nRecvBaoTingPlayerMap[ChairID] = TYPE_PASS;
		m_bUserOperate[ChairID] &= ~(TYPE_GANG | TYPE_JILIN_GANG | TYPE_BAO_TING); 
	}

	if (iOpType == TYPE_JILIN_GANG  )
	{
		log_error("error %x " , iOpType );
		return;
	}

	m_nGameRunState = GAME_QISHOU_BAOTING_OVER;  // 报听结束 !!
	switch (iOpType)
	{
	case TYPE_PENG:
		UserOperatePeng(ChairID, bCard);
		break;
	case TYPE_WANGANG:
		if (m_conf.m_bQiangGangHu && RobGangHu(ChairID, bCard, iOpType))   //   抢杠胡。 
			return;
	case TYPE_ANGANG:
		if (m_conf.m_bQiangAnGang && RobGangHu(ChairID, bCard, iOpType))
			return;
	case TYPE_ZHIGANG:
		if (m_conf.m_bQiangZhiGang && RobGangHu(ChairID, bCard, iOpType))
			return;
		do
		{
			int ret = UserOperateGang(ChairID, iOpType, bCard);
			if (ret > 0)
				return;
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


void Table_jilin::UserOperateZhongfaBai(_uint8 ChairID, int iOpType, _uint8 bCard)
{

	memset(m_bAddFanFlag, 0, sizeof(m_bAddFanFlag));
	memset(m_bUserOperate, 0, sizeof(m_bUserOperate)); 
	ZhongfabaiChi chi;
	chi.chi_card = bCard;
	chi.chi_id = ChairID;
	chi.pre_id = m_bOutCardChairID;
	m_Jilinchi.push_back(chi);
	for (int i = 0x41; i <= 0x43; i++)
	{
		if (bCard == i)  continue;
		m_GameLogic.ChuPai(&m_HandCard[ChairID], i);
	}



	m_bGetCardCount[ChairID]++;
	m_stUserOutCard[m_bOutCardChairID].RemoveLast(bCard);

	//广播操作结果的消息
	BroadcastOperateResult(ChairID, iOpType, bCard);
	log_info(" uid[%d] ChairID[%d]  bCard: %x  ", GetPlayerUid(ChairID), ChairID, bCard);

	SendTingInfo(ChairID);

	int score[GAME_PLAYER] = { 0 };
	save_video_operate(iOpType, ChairID, m_bOutCardChairID, bCard, score);

	UserAfterOperate(ChairID, iOpType, bCard);

}



bool Table_jilin::is_gang_shang_kai_hua(_uint8  chairID, bool zimo)
{
	if (chairID >= 0 && chairID < m_GAME_PLAYER)
	{		
		if (m_bAddFanFlag[chairID] == 1 && zimo  )
		{
			return true;
		}
	}
	return false;
}


bool Table_jilin::IsQuanqiuren(const HandCards& pHc)
{
	return m_GameLogic.IsDanDiao(pHc);
	
}



int Table_jilin::check_bian_hu(_uint8  ChairID, _uint8  hu_card, const  MJ_win_pattern &pattern , bool laizi  , bool  canhu_paixng )
{
	if( ChairID >= m_GAME_PLAYER)
	{
		log_error("error %d ", ChairID);
		return 0;
	}
	if (hu_card > 0x30 && laizi == 0 ) // bug!!  
		return  0; 

	const  HandCards* pHc = &m_HandCard[ChairID];
	vector<int> all_han_card;	
	int all = pHc->ChangeableCardsLen;
	if (all> 14){
		all = 14;
		log_error("error ");
		return 0;
	}

	for (int i = 0; i < all; i++)	
		all_han_card.push_back(pHc->ChangeableCards[i]);
	
	for (vector<int>::iterator it = all_han_card.begin(); it != all_han_card.end(); ++it){
		if (*it == hu_card){
			all_han_card.erase(it);
			break;
		}
	}		
	if (laizi){
		if (jilin_hu_can_be_bianhu(all_han_card, laizi))
			return 1; 		 
	} 	

	if (le_check_hu_jilin(  (_uint8*)pHc->ChangeableCards, pHc->ChangeableCardsLen))
	{
		if (hu_at_bianhu(hu_card))
			return 1;
	}

	//for (int i = 0; i < 4; i++) 
	//{
	//	for (int j = 0; j < 1; j++)
	//	{
	//		if (pattern.kan[i][j] == pattern.kan[i][j + 1]) 			
	//			continue;
	//		
	//		int  first_a = pattern.kan[i][j];			
	//		int  third_c = pattern.kan[i][j + 2]; 
	//		if( hu_card == third_c  &&    get_face(third_c) == 3 &&  In_Hand(pHc , first_a ,  first_a+1 ,  third_c) )
	//			return 1;
	//		else if (hu_card == first_a && get_face(first_a) == 7 && In_Hand(pHc, first_a , first_a+1 , third_c ) )
	//			return 1;
	//		
	//	}
	//}

	return  0;

}

int Table_jilin::In_Hand_New(const  HandCards  *phc, int  a)
{
	for (int i = 0; i < phc->ChangeableCardsLen; i++)
	{
		if (phc->ChangeableCards[i] == a)
			return  1;
	}
	return  0; 
}

int Table_jilin::In_Hand(const  HandCards  *phc, int  a, int b, int c)
{
	int all[3] = { 0 };
	for (int i = 0; i < phc->ChangeableCardsLen ; i++)
	{
		if (phc->ChangeableCards[i] == a)
			all[0] = 1;
		else if (phc->ChangeableCards[i] == b)
			all[1] = 1;
		else if (phc->ChangeableCards[i] == c)
			all[2] = 1;
	}
	return    ( all[0] + all[2] + all[1] == 3 );
}

int Table_jilin::check_jia_hu(_uint8  ChairID, _uint8  hu_card, const  MJ_win_pattern &pattern , bool  laizi  , bool  can_hu_paixing)
{
	if ( ChairID >= m_GAME_PLAYER)
	{
		log_error("error %d ", ChairID);
		return 0;
	}
	const  HandCards* pHc = &m_HandCard[ChairID];
	if (pHc->ChangeableCardsLen   == 2 ) 
	{
		if (pHc->ChangeableCards[0] == pHc->ChangeableCards[1])   //  摸宝。。 bug修改。
			return 1;
		else{
			if (laizi){
				return  1;
			}
			return 0;
		} 		
	} 		
	vector<int> all_han_card;	
	int all = pHc->ChangeableCardsLen;
	if (all> 14){
		all = 14;
		log_error("error ");
	}
	int card_array[20] = { 0 };
	for (int i = 0; i < all; i++){
		all_han_card.push_back(pHc->ChangeableCards[i]);
		card_array[i] = pHc->ChangeableCards[i];
	}
	if (Ting_Zhongfabai(pHc->ChangeableCards, pHc->ChangeableCardsLen, hu_card) )
	{
		log_debug("zhongfabai !!");
		return 1;
	}
	
	
	for (vector<int>::iterator it = all_han_card.begin(); it != all_han_card.end(); ++it){
		if (*it == hu_card){
			all_han_card.erase(it);
			break;
		}
	}			
	if (Can_hu(card_array, all) && (jiang_after_hu() == hu_card || AtShunMid(hu_card)))
	{
		log_debug("can ting ");
		return 1;
	}	
	if (Spe_hu(card_array, all, hu_card))
	{
		log_debug("canhu hu %x" , hu_card );
		return 1;
	}
	if ( jilin_hu_if_add_can_be_jia_hu(all_han_card , laizi ))
	{
		log_debug("jilin jia hu ");
		return 1;  
	}
	
	if (hu_card ==   pattern.jiang  )
	{		
		log_debug("jilin jia hu card  ");
		return 1;
	}
		 
	if (le_check_hu_jilin( (_uint8*)pHc->ChangeableCards, pHc->ChangeableCardsLen))
	{
		if (hu_at_Middle(hu_card) > 0)
			return 1;
	}

	//for (int i = 0; i < 4; i++)
	//{
	//	for (int j = 0; j < 1; j++)
	//	{
	//		if( pattern.kan[i][j] == pattern.kan[i][j + 1])	
	//			continue;  			 
	//		int  sec_b = pattern.kan[i][j + 1]; 			 
	//		if (hu_card == sec_b && In_Hand(pHc, pattern.kan[i][j], pattern.kan[i][j + 1], pattern.kan[i][j + 2]))
	//			return  1;			
	//		
	//	}
	//}
	return  0;

}

int Table_jilin::GetOneZFB(map<int,int> & all_map )
{
	for (int i = 1; i <= 3; i++)
	{
		if (all_map[0x40 + i] == 0)
			return   0x40 + i;
	}
	return  0x41;
}

int  Table_jilin::Is_PengPneghu_Jilin(_uint8  ChairId , const  HandCards  *phc  ,  bool laizi  )
{
	if (ChairId < 0 || ChairId >= m_GAME_PLAYER)
		return 0;

	for (int i = 0; i < phc->FixedCardsLen ; i++)   // 碰碰胡。。不可以吃。。  
	{
		if (phc->FixedCards[i].state & TYPE_CHI)
		{
			return false;
		}
	}
	bool  remove = false;
	int  is_can_hu = false;
	remove = m_GameLogic.ChuPai( &m_HandCard[ChairId], m_baoPai);  
	if (  remove )
	{
		map<int, int> allmap;
		int  my_card_array[20] = { 0 };
		int  len = 0;
		for (int i = 0; i < phc->ChangeableCardsLen && len<=13  ; i++)
		{ 
			int ct = phc->ChangeableCards[i]; 
			allmap[ct]++; 			
			my_card_array[len] = ct;
			len++; 
		}					
		for (map<int, int>::iterator it = allmap.begin(); it != allmap.end(); ++it)
		{			
			if (len<= 13 )
			{
				if (it->first > 0x40)
					my_card_array[len] = GetOneZFB(allmap);  
				else
					my_card_array[len] = it->first;

				log_debug("all pai %s ", Debug_All_Hand(my_card_array, len) );
				len++;
				int ret = Can_hu(my_card_array, len); 
				if (ret > 0   && is_all_kezi() )
				{
					is_can_hu = true;
					log_debug("can hu  ");
					break;
				}
				len--;
			}

		}
		m_GameLogic.AddCard(&m_HandCard[ChairId], m_baoPai);
	}
	return  is_can_hu;

}

int Table_jilin::Ting_Zhongfabai_PengPengHu_Judge( const  HandCards *phc  ,   _uint8 add_last )
{
	for (int i = 0; i < phc->FixedCardsLen; i++)
	{
		if (phc->FixedCards[i].state & TYPE_CHI)
			return 0;
	}
	if (Ting_Zhongfabai(phc->ChangeableCards , phc->ChangeableCardsLen  , add_last) && is_all_kezi() )
	{
		log_debug("ting  add last %x" , add_last);
		return 1;
	}
	return  0; 
}

//吉林吃。。算。。开门 。 

int  Table_jilin::CheckIfCanSwapBaoPai(int sid  )
{
	if (!g_jilin_conf.quick_bao && m_NoQuickBaoTongBaoGrapPaiCount[sid] == BAO_TING_TONG_BAO_BU_HU)
		return  1; 

	if (!g_jilin_conf.quick_bao && m_NoQuickBaoTongBaoGrapPaiCount[sid] == BAO_TING_FIRST   )
	{
		return 0;  
	}
	return 1; 
}

int Table_jilin::KaiMen(const  HandCards *phc , int sid  )
{
	for(int i = 0; i < phc->FixedCardsLen; i++)
	{
		if (phc->FixedCards[i].state&TYPE_CHI || (phc->FixedCards[i].state & TYPE_PENG))
			return 1;
		else if (phc->FixedCards[i].state  & TYPE_ZHIGANG || phc->FixedCards[i].state == TYPE_WANGANG)
			return 1;
	} 
	for (  size_t  i = 0; i < m_Jilinchi.size(); i++)
	{
		if (m_Jilinchi[i].chi_id == sid)
			return 1;
	}

	return 0;
}

int Table_jilin::phc_allkezi(const  HandCards  *phc)
{
	for (int i = 0; i < phc->FixedCardsLen; i++)
	{
		if (phc->FixedCards[i].state & TYPE_CHI)
			return  0;
	}
	return 1;
}


int Table_jilin::AddFanJilinHu(_uint8 ChairID, const HandCards& pHc,
	MJ_fan_buffer& oFan, const MJ_win_pattern& oPattern, bool  can_hu_paixing  , _uint8  add_last)
{
	if( ChairID >= m_GAME_PLAYER)
		return 0; 
	bool  laizi = (oFan.Find(MJ_FAN_TYPE_MO_BAO) || oFan.Find(MJ_FAN_TYPE_TONG_BAO));
	bool has_one = false;
	if (m_bHasTianhu)
		add_last = m_addlastCard;
	if (m_bGrapId == ChairID && add_last == 0xFF)
		add_last = m_bCurCard;
	if (laizi)
		add_last = m_baoPai;  

	log_debug(" laizi %d  add_last  %x  pai [%s]  FAN %d", laizi ,  add_last, CatHandCard(pHc, ChairID) ,  oFan.Size() );	
	if(has_one  ==false  && (!oFan.Find(MJ_FAN_TYPE_QIDUI)))
	{
		if (laizi)
		{
			if ( Is_PengPneghu_Jilin(ChairID, &pHc, true))
			{
				oFan.Add(MJ_FAN_TYPE_PENGPENGHU);
				has_one = true; 
			}			
		}
		else
		{
			
			if (m_GameLogic.IsPengPengHu(&pHc , oPattern.kan , oPattern.jiang) || Ting_Zhongfabai_PengPengHu_Judge(&pHc,add_last) ) 
			{
				oFan.Add(MJ_FAN_TYPE_PENGPENGHU);
				log_debug("jiahu hu !!");
				has_one = true;
			}
		}		
	}


	if (has_one == false && (!oFan.Find(MJ_FAN_TYPE_QIDUI)) && check_jia_hu(ChairID, add_last, oPattern, laizi, can_hu_paixing  ))
	{
		oFan.Add(MJ_FAN_YA_ZI);
		log_debug("jiahu hu !!");
		has_one = true; 		
	}

	if (( !oFan.Find(MJ_FAN_TYPE_QIDUI)) && check_bian_hu(ChairID, add_last, oPattern, laizi , can_hu_paixing ))
	{
		oFan.Add(MJ_FAN_TYPE_BIAN_HU);  
		oFan.Remove(MJ_FAN_YA_ZI); 
		log_debug("bianhu hu !!");
		has_one = true;
	}
	
	if (oFan.Size())
	{
		if (laizi)
		{
			if( OneColor(ChairID ,   &pHc , true ))			
				oFan.Add(MJ_FAN_TYPE_QINGYISE);
			
		}
		else
		{
			if (ThreeColorOrQingYiSe(ChairID , &pHc) ==1)			
				oFan.Add(MJ_FAN_TYPE_QINGYISE);
			
		}
	} 
	//、、加番牌型。  2杠上开花
	if (oFan.Size() &&  m_bGrapId== ChairID &&
		 (is_gang_shang_kai_hua(ChairID, m_bGrapId == ChairID) || m_nGangShangHua[ChairID]) 
		)
	{
		oFan.Add(MJ_FAN_TYPE_GANGSHANGHUA);		
		
	}
	if (oFan.Size() && !KaiMen(&pHc , ChairID) )
	{  //  闭门修改，， 
		oFan.Add( MJ_FAN_TYPE_BI_MEN );
		
	}
	if ( m_QiangGangHu[ChairID])
	{
		oFan.Add(MJ_FAN_TYPE_QIANGGANGHU); 		
	}
	// 全球人，4句全都吃碰杠，手上只有一张牌后胡牌 
	if (oFan.Size()  && (m_GameLogic.IsDanDiao(pHc) ||   dan_diao(laizi , &pHc) )) 
	{
		if (g_jilin_conf.xian_ba_yi && m_nXianBayi[ChairID])
			oFan.Add(MJ_FAN_TYPE_QUANQIUREN); 
		else if(!g_jilin_conf.xian_ba_yi && m_nXianBayi[ChairID]>2) 
			oFan.Add(MJ_FAN_TYPE_QUANQIUREN); 
	} 
	if (m_bHasTianhu)
	{
		oFan.Add(MJ_FAN_TYPE_TIANHU);
	}
	return 0; 
}

int Table_jilin::dan_diao(bool laizi, const  HandCards *pc)
{
	return    (laizi  && pc->ChangeableCardsLen <= 2 ) ;
}

int Table_jilin::Check_Hu_Pattern(_uint8 ChairID, const HandCards& pHc, MJ_fan_buffer& oFan, const MJ_win_pattern& oPattern)
{
		
	AddFanJilinHu(ChairID, pHc, oFan, oPattern , 0  ,0 ); 	
	return  0;

}



int Table_jilin::Calc_Hu(_uint8 ChairID, _uint8 bCard, int Score[GAME_PLAYER])
{
	
	return Table::Calc_Hu(ChairID, bCard, Score);	


}

int  Table_jilin::GetGangFen( int  gang_base   ,  int  is_normal  ,  int  curGaneCard , int gang_type  )
{
	int   base  = 1;
	if (g_jilin_conf.feng_ding == 16 ||  g_jilin_conf.feng_ding == 64 )
	{
		if (is_normal)
		{			
			if (gang_type == TYPE_ZHIGANG || gang_type == TYPE_WANGANG)
			{
				if (curGaneCard == 0x21)
					base = 2;
				else if (curGaneCard == 0x22)
					base = 4;
				else if (curGaneCard == 0x8 || curGaneCard == 0x11)
					base = 8;
			}
			else if (gang_type == TYPE_ANGANG)
			{
				base = 2;
				if (curGaneCard == 0x21)
					base = 4;
				else if (curGaneCard == 0x22)
					base = 8;
				else if (curGaneCard == 0x8 || curGaneCard == 0x11)
					base = 16 ;
			}
		}
		else
		{
			base = 1;
		}
		return base;
	}
	else if( g_jilin_conf.feng_ding==  32  )
	{ 
		if (is_normal)
		{
			// int base = 1;  // fen  bug !! 
			if (gang_type == TYPE_ZHIGANG || gang_type == TYPE_WANGANG)
			{
				if (curGaneCard == 0x21)
					base = 4;
				else if (curGaneCard == 0x22)
					base = 8; 
				else if (curGaneCard == 0x8 || curGaneCard == 0x11)
					base = 16;
			}
			else if (gang_type == TYPE_ANGANG)
			{
				base = 2;
				if (curGaneCard == 0x21)
					base = 8;
				else if (curGaneCard == 0x22)
					base = 16;
				else if (curGaneCard == 0x8 || curGaneCard == 0x11)
					base = 32;
			}
		}
		else
		{
			base = 1;
		}
		return base;
	}
	else if (g_jilin_conf.feng_ding == 40)
	{
		if (is_normal)
		{
			 base = 2;
			if (gang_type == TYPE_ZHIGANG || gang_type == TYPE_WANGANG)
			{
				if (curGaneCard == 0x21)
					base = 5;
				else if (curGaneCard == 0x22)
					base = 10;
				else if (curGaneCard == 0x8 || curGaneCard == 0x11)
					base = 20;
			}
			else if (gang_type == TYPE_ANGANG)
			{
				base = 5;  // modify !! 
				if (curGaneCard == 0x21)
					base = 10;
				else if (curGaneCard == 0x22)
					base = 20;
				else if (curGaneCard == 0x8 || curGaneCard == 0x11)
					base = 40;
			}
		}
		else
		{
			base = 2;
		}
		return base;
	}
	else
	{
		if (is_normal)
		{
			 base = 5;
			if (gang_type == TYPE_ZHIGANG || gang_type == TYPE_WANGANG)
			{
				if (curGaneCard == 0x21)
					base = 10;
				else if (curGaneCard == 0x22)
					base = 20;
				else if (curGaneCard == 0x8 || curGaneCard == 0x11)
					base = 40;
			}
			else if (gang_type == TYPE_ANGANG)
			{
				base = 10;
				if (curGaneCard == 0x21)
					base = 20;
				else if (curGaneCard == 0x22)
					base = 40;
				else if (curGaneCard == 0x8 || curGaneCard == 0x11)
					base = 80;
			}
		}
		else
		{
			base = 5;
		}
		return base;
	} 
	return  base; 
}


void Table_jilin::proto_gang_money(int *pPlayerIsZeroGuo ,   int  *playerTrueMoney   ,  int hu_sid  ) 
{
	//debug 
	int  TempMoney[4]; 
	int  TempGangMoney[4] = { 0 };
	for (int i = 0; i < m_GAME_PLAYER;i++)
	{
		log_debug(" truemonye sid %d  %d" ,  i ,  playerTrueMoney[i] ); 
		TempMoney[i] = playerTrueMoney[i]; 
	}
	int  IdeaPlayerMoney[4][4];
	memset(IdeaPlayerMoney, 0, sizeof(IdeaPlayerMoney));  	 

	for (int chairid = 0; chairid < m_GAME_PLAYER; chairid++)
	{
		const   HandCards  *pc = &m_HandCard[chairid];
		for (int i= 0; i < pc->FixedCardsLen ; i++)
		{
			if (pc->FixedCards[i].state  & TYPE_GANG)
			{ 
				int ct = pc->FixedCards[i].CardData;
				if(ct== 0 ) continue; 
				log_debug("ct %x  type %d" , ct  , pc->FixedCards[i].state ); 
				int fen = GetGangFen(0 ,  true  , ct ,   pc->FixedCards[i].state ); 								
				for (int k = 0; k < m_GAME_PLAYER; k++)
				{
					if (chairid == k)  continue;
					if( pPlayerIsZeroGuo[k]) continue;				

					TempGangMoney[chairid] += fen;
					TempGangMoney[k] -= fen;
					
					IdeaPlayerMoney[chairid][k] += fen;
					IdeaPlayerMoney[k][chairid] -= fen;
					log_debug("win lost  win_lsot %d  %d  %d" , chairid , k  , IdeaPlayerMoney[chairid][k]);

					//log_debug("ct [%x] fen[%d]  gangmoney base[%d]  ", ct, fen, IdeaPlayerMoney[chairid][k] );
				}				
			}			
		} 
		JiGangInfo &ganginfo = m_JilinGangInfo[chairid]; 
		for (int j = 0; j < ganginfo.len; j++)
		{
			gang_card  &g = ganginfo.g_arr[j];  
			int fen = 0;
			for (int st = 0; st < XUAN_FENG_GANG_ARR_LEN; st++)
			{
				int ct = GetGangFen(0 , false  ,g.arr[0] , 0  );
				if (g.arr[st])
				{
					fen += ct*g.bu[st];
				}
			}
			for (int k = 0; k < m_GAME_PLAYER; k++)
			{
				if (chairid == k)  continue;
				if( pPlayerIsZeroGuo[k]) continue;
				
				
				TempGangMoney[chairid] += fen;
				TempGangMoney[k] -= fen;

				IdeaPlayerMoney[chairid][k] += fen;
				IdeaPlayerMoney[k][chairid] -= fen;
				log_debug("win lost  win_lsot %d  %d  %d", chairid, k, IdeaPlayerMoney[chairid][k]);

				//log_debug("ct [%x] fen[%d] gang w[%d] l[%d]  wl_base[%d]", 0, fen, chairid, k, IdeaPlayerMoney[chairid][k]);
			}

		}	
	}
	for (int i = 0; i < m_GAME_PLAYER;i++)
	{
		log_debug("i ====  %d  %d %d %d", TempGangMoney[0], TempGangMoney[1], TempGangMoney[2], TempGangMoney[3]);
		for (int j = 0; j < m_GAME_PLAYER; j++)
		{
			// log_debug("idea winlost %d " , IdeaPlayerMoney[i][j] );
		}
	}
	for (int i = 0; i < m_GAME_PLAYER; i++)
	{	
		m_fenResult[i][0] = TempGangMoney[i];
	}


	map<int, int>  all_pai_chu_map ;

	//  胡牌人的下架的 第一个不够 赔的   第一个。   // 杠钱大于自己的本钱了。    输家 不够赔的时候。。 ！！！！  
	for (int i = 0; i < m_GAME_PLAYER; i++)
	{
		int  lost_id = (i + hu_sid) % m_GAME_PLAYER;		
		if (TempGangMoney[lost_id] < 0 && abs(TempGangMoney[lost_id]) > TempMoney[lost_id])
		{
			if (all_pai_chu_map[lost_id] == 1)  continue;
			for (int win_id = NextChair(lost_id); win_id != lost_id; win_id = NextChair(win_id))
			{
				if (all_pai_chu_map[win_id] == 1)
				{
					if (IdeaPlayerMoney[win_id][lost_id] >= 0)
					{
						TempGangMoney[win_id]   -= abs(IdeaPlayerMoney[win_id][lost_id]);
						TempGangMoney[lost_id] += abs(IdeaPlayerMoney[win_id][lost_id]); 
						log_debug("win[%d] lost[%d]  win_gang[%d] lose{%d}",
							win_id , lost_id , TempGangMoney[win_id] , TempGangMoney[lost_id]);

					}
					continue;
				}
				if(TempGangMoney[lost_id] >= 0 )
					break;

				if (IdeaPlayerMoney[win_id][lost_id] > 0)
				{
					if (abs(TempMoney[lost_id] >= IdeaPlayerMoney[win_id][lost_id]))
					{
						TempMoney[lost_id] -= IdeaPlayerMoney[win_id][lost_id];
						TempMoney[win_id] += IdeaPlayerMoney[win_id][lost_id]; 
						log_debug("idea winlost[%d]   win_money[%d]  lost_money[%d]   w_sid[%d] l_sid[%d]   win_gang[%d] lost_gang[%d] ",
							IdeaPlayerMoney[win_id][lost_id], TempMoney[win_id], TempMoney[lost_id], win_id, lost_id ,
							TempGangMoney[win_id] , TempGangMoney[lost_id] 
							);
					}
					else
					{
						
						TempMoney[win_id] += abs( TempMoney[lost_id]);
						TempGangMoney[win_id] -= abs(IdeaPlayerMoney[win_id][lost_id]);
						TempGangMoney[win_id] += abs(TempMoney[lost_id]); 
						TempGangMoney[lost_id] += abs(IdeaPlayerMoney[win_id][lost_id]);
						TempGangMoney[lost_id] -= abs(TempMoney[lost_id]);
						TempMoney[lost_id]     -= abs(TempMoney[lost_id]);
						 
						log_debug("idea winlost222[%d]   win_money[%d]  lost_money[%d]   w_sid[%d] l_sid[%d]  win_gang[%d] lose_gang[%d] ",
							IdeaPlayerMoney[win_id][lost_id], TempMoney[win_id], TempMoney[lost_id], win_id, lost_id ,TempGangMoney[win_id]  , TempGangMoney[lost_id] );

					}
				}
			}
			all_pai_chu_map[lost_id] = 1;
		}
	}


	// 第二种情况 
	for (int i = 0; i < m_GAME_PLAYER; i++)
	{		 
		if (playerTrueMoney[i] < 0)
		{
			log_error("i[%d] uid[%d] %d %d " , i, GetPlayerUid(i) , playerTrueMoney[i] , TempGangMoney[i]); 
			TempGangMoney[i] += abs(playerTrueMoney[i]);
			playerTrueMoney[i] = 0; 
		}
		m_GangMoney[i] = TempGangMoney[i]; 
		playerTrueMoney[i] += m_GangMoney[i]; 
		log_debug("sid[%d]  truemoney[%d] gangmoney[%d]" , i ,  playerTrueMoney[i] ,m_GangMoney[i] ) ;
		
	} 
	
	

	// debug
	for (int i = 0; i < m_GAME_PLAYER;i++)
	{
		log_debug("sid[%d] uid[%d] gangmon[%d]  turemonye[%d] base " , 
			i  , GetPlayerUid(i) , m_GangMoney[i] ,    playerTrueMoney[i] ); 
		 
	}
	// end debug 	

}




void Table_jilin::Score_GameEndDraw(HuScoreSet& score, proto::game::AckGameEnd& stGameend)
{
	if (m_conf.m_bHuangGang)
	{
		memset(m_stGang, 0, sizeof(m_stGang));
	}
	if (m_bBanker < 0 || m_bBanker >= m_GAME_PLAYER)
		return;

	int TrueMoney_[4];
	memset(TrueMoney_, 0, sizeof(TrueMoney_));

	int is_guo0[4] = { 0 };
	for (int i = 0; i < m_GAME_PLAYER; i++)
	{
		int ct = GetPlayerWinlostMoney(i);
		TrueMoney_[i] = ct + g_jilin_conf.guo;
		if (TrueMoney_[i] <= 0)
		{
			is_guo0[i] = 1;
		}
		log_debug("truemney [%d]", TrueMoney_[i]);

	}
	proto_gang_money(is_guo0 , TrueMoney_  ,m_bBanker );

	for (int i = 0; i < m_GAME_PLAYER; i++)
	{
		score[i].iTotleScore =  m_GangMoney[i];
		log_debug("winlost %d", m_GangMoney[i]); 
	}

}

void Table_jilin::Proto40Spe(int  &fan )
{
	if (g_jilin_conf.feng_ding == 40 && fan == 4)
		fan = 5;

}
void Table_jilin::Calc_Hu_Player_Base_Score(_uint8 ChairID, int iBaseScore[GAME_PLAYER])
{
	if( ChairID>= m_GAME_PLAYER)
	{
		log_error("error  !");
		return;
	}
	if (!m_bIsHu[ChairID])
		return;

	int TrueMoney_[4]; 
	memset(TrueMoney_, 0, sizeof(TrueMoney_));
	int is_guo0[4] = { 0 };   // 玩家的真实钱数，， 
	for (int i = 0; i < m_GAME_PLAYER; i++)
	{
		int ct = GetPlayerWinlostMoney(i);
		TrueMoney_[i] = ct + g_jilin_conf.guo;
		if ( TrueMoney_[i] <= 0 )
		{
			is_guo0[i] = 1;
		}
		log_debug("truemney  i[%d] [%d]" , i ,  TrueMoney_[i] );
		
	}
	proto_gang_money(is_guo0  , TrueMoney_ , ChairID ) ;

	int hu_base = 1;
	if (g_jilin_conf.feng_ding == 40)
		hu_base = 2;
	else if (g_jilin_conf.feng_ding == 80)
		hu_base = 5;

	MJ_fan_buffer  OutFan;
	m_ft2fn->FanFilter(m_oFan[ChairID], OutFan); 
	//夹胡 = 基础分
	
	for (int i = 0; i < OutFan.Size(); i++)
		log_debug("fan %d " , OutFan[i] );
	if (OutFan.Find(MJ_FAN_TYPE_BIAN_HU))  
	{ 
		hu_base *= 2;
		Proto40Spe(hu_base); 
		log_debug("bianhu *2   hu_base[%d] ", hu_base);
	}
	if (OutFan.Find(MJ_FAN_TYPE_MO_BAO))
	{
		hu_base *= 2;
		Proto40Spe(hu_base); 
		hu_base *= 2;
		log_debug("mobao * 4  hu_base[%d] ", hu_base);
	}
	if (OutFan.Find(MJ_FAN_TYPE_TONG_BAO))
	{
		hu_base *= 2;
		Proto40Spe(hu_base); //  modify !! 
		hu_base *= 4; 
		log_debug("tongbao *16   hu_base[%d] ", hu_base);
	}
	if (OutFan.Find(MJ_FAN_TYPE_PENGPENGHU))
	{
		hu_base *= 2;
		Proto40Spe(hu_base); 
		hu_base *= 2;
		log_debug("pphu  * 4 hu_base[%d] ", hu_base);
	}
	if (OutFan.Find(MJ_FAN_TYPE_QIDUI))
	{
		hu_base = g_jilin_conf.feng_ding; 
		log_debug("diqui fending ");
	} 
	if (OutFan.Find(MJ_FAN_TYPE_QINGYISE))
	{
		hu_base *= 2;
		Proto40Spe(hu_base);
		log_debug("yise  hu_base [%d] ", hu_base);
	}
	if (OutFan.Find(MJ_FAN_TYPE_BI_MEN))
	{
		hu_base *= 2;
		Proto40Spe(hu_base);
		log_debug("bimen   hu_base[%d] ", hu_base);
	}
	if (OutFan.Find(MJ_FAN_TYPE_QUANQIUREN))
	{
		hu_base *= 2;
		Proto40Spe(hu_base);
		log_debug("quanqiuren hu_base  %d ", hu_base);
	}
	if (OutFan.Find( MJ_FAN_TYPE_GANGSHANGHUA))
	{
		hu_base *= 2;
		Proto40Spe(hu_base);
		log_debug("gang shang hua hu_base[%d] ", hu_base);
	}
	if (ChairID == m_bBanker)
	{
		hu_base *= 2;
		Proto40Spe(hu_base);
		log_debug("hupai bank *2  hu_base %d" , hu_base );
	}
	if ( ChairID == m_bGrapId && m_stHuScore[ChairID].stHu.Size() && m_stHuScore[ChairID].stHu[0].bType == HU_ZIMO 
		&& !(OutFan.Find(MJ_FAN_TYPE_TONG_BAO) || OutFan.Find(MJ_FAN_TYPE_MO_BAO)))
	{
		hu_base *= 2;
		Proto40Spe(hu_base);
		log_debug(" zimo  *2  hu_base %d  chairdi %d  grap %d ", hu_base ,ChairID ,   m_bGrapId );
	}
	
	if (OutFan.Find(MJ_FAN_TYPE_QINGYISE)  && OutFan.Find(MJ_FAN_TYPE_PENGPENGHU)  &&  OutFan.Find(MJ_FAN_TYPE_QUANQIUREN))
	{
		hu_base *= 2;
		Proto40Spe(hu_base); 
		log_debug("  yise   pphu   quanqiuren    *2 %d", hu_base);
	}
	log_debug("hubase %d" , hu_base );

	

	for (int i = 0; i < m_GAME_PLAYER; i++)
	{
		if (i == ChairID) continue;
		//if (is_guo0[i])   continue;  // 0 锅 。。 不开钱。  
		int cur_base = hu_base;

		//炮胡时点炮者 + 1番  
		if (m_bCurChairID == i  && m_stHuScore[ChairID].stHu.Size() && m_stHuScore[ChairID].stHu[0].bType == HU_PAOHU)
		{
			cur_base *= 2;
			//Proto40Spe(hu_base); 
			Proto40Spe(cur_base);
			//log_debug("dian pao add yifan  *2  %d", cur_base);
		}
		//别人胡牌时庄家 + 1番
		if (m_bBanker == i)
		{
			cur_base *= 2;
			//Proto40Spe(hu_base); 
			Proto40Spe(cur_base);
			//log_debug("bank_hu *2  cur_base  %d", cur_base);
		}
		 //别人胡牌时门清玩家 + 1番 h // && m_HandCard[i].ChangeableCardsLen>= 13  )  //  闭门判断。 bug!!! 
		if( !m_oFan[ChairID].Find( MJ_FAN_TYPE_BI_MEN)  &&  !KaiMen(&m_HandCard[i] , i ) ) 
		{
			cur_base *= 2;
			//Proto40Spe(hu_base); 

			Proto40Spe(cur_base);
			log_debug("menqing  * 2 curbase %d", cur_base);
		} 

		//抢杠时被抢杠玩家 + 2番
		if (OutFan.Find(MJ_FAN_TYPE_QIANGGANGHU) && m_bGrapId == i)
		{
			cur_base *= 2;  
			//Proto40Spe(hu_base); 
			Proto40Spe(cur_base);
			log_debug("beiqianggan 4  base %d", cur_base);
		}
		cur_base = std::min(cur_base, g_jilin_conf.feng_ding);

		bool  bIsBaoAll =  (!m_oFan[ChairID].Find(MJ_FAN_TYPE_QIDUI) && m_stHuScore[ChairID].stHu.Size()
			&& m_bOutCardChairID < m_GAME_PLAYER && m_bOutCardChairID >= 0
			&& m_stHuScore[ChairID].stHu[0].bType == HU_PAOHU
			&&
			((m_jilin_baoting[m_bOutCardChairID].bao_ting&& m_outCardCountAfterBaoTing[m_bOutCardChairID] <= 1)
			|| !m_jilin_baoting[m_bOutCardChairID].bao_ting
			) ) ;
		if (is_guo0[i] && i != m_bOutCardChairID  && bIsBaoAll == false)  //  0 锅，不包的时候。 
		{
			log_debug("0guo curbase[%d] guo[%d] " , cur_base ,  i );
			continue;
		}
		// 七对 放炮。。不包。 
		if(bIsBaoAll )
		{
			m_WinlostArr[ChairID][m_bOutCardChairID] += cur_base; 
			m_WinlostArr[m_bOutCardChairID][ChairID] -= cur_base;	
			if(m_bOutCardChairID< m_GAME_PLAYER)
				log_debug("bao size[%d]", m_outCardCountAfterBaoTing[m_bOutCardChairID]);

			log_debug("all bao !! ");
			log_debug("curbase[%d] winlost[%d][%d] w_l[%d] ding[%d]   ",  
				cur_base, ChairID, m_bOutCardChairID, m_WinlostArr[ChairID][m_bOutCardChairID], g_jilin_conf.feng_ding );
		}
		else
		{
			if(is_guo0[i] ) continue; 
			m_WinlostArr[ChairID][i] += cur_base;
			m_WinlostArr[i][ChairID] -= cur_base;			
			
			
			log_debug("curbase[%d] winlost[%d][%d] w_l[%d] ding[%d]", 
				cur_base, ChairID, i, m_WinlostArr[ChairID][i], g_jilin_conf.feng_ding);
		}	  		
	}
	for (int i = 0; i < m_GAME_PLAYER; i++)
	{		
		if (i != ChairID)
		{
			m_fenResult[i][1]		-=  abs(m_WinlostArr[ChairID][i]) ;
			m_fenResult[ChairID][1] += abs(m_WinlostArr[ChairID][i]); 
			log_debug("win[%d]  lost [%d]  winlost[%d]  lost[%d] " , 
				ChairID , i  ,   m_fenResult[ChairID][1] ,    m_fenResult[i][0] );
		}
	}

	for (int   lose_id  = 0;  lose_id  < m_GAME_PLAYER; lose_id++)
	{
		if(ChairID ==lose_id )  continue; 
		if(  abs( TrueMoney_[lose_id]) < m_WinlostArr[ChairID][lose_id]) 
		{
			m_WinlostArr[ChairID][lose_id] = abs(TrueMoney_[lose_id]); 
			if (TrueMoney_[lose_id] <= 0)
			{
				m_WinlostArr[ChairID][lose_id] = 0;
				log_debug("spce win[%d]  lost[%d]" , ChairID , lose_id ); 
			}
		}
		m_TrueWinLost[ChairID] +=     abs(m_WinlostArr[ChairID][lose_id]) ;
		m_TrueWinLost[lose_id] -=     abs(m_WinlostArr[ChairID][lose_id]);  
		log_debug("win[%d]  lost[%d]   winlost[%d]" , ChairID , lose_id ,   abs(m_WinlostArr[ChairID][lose_id]) );
	}



}



void Table_jilin::TurnBanker()
{
	int pre_bank  = m_bBanker;
	MJ_user_buffer UserID;
	int iCount = MultiHu(UserID);
	if (iCount <= 0)
		return; 
	if (iCount == 1 && m_bIsHu[m_bBanker])
		return; 
	else if (iCount > 1)
	{		
		int si = 0;
		for (_uint8 c = NextChair(m_bOutCardChairID); c != m_bOutCardChairID; c = NextChair(c))
		{
			if (si++ > 10)
				return;  
			if (m_bIsHu[c])
			{
				if (c == m_bBanker)
					return; 
				else
					m_bBanker = NextChair(m_bBanker);  				
				return; 
			}							
		}
	}

	else
		m_bBanker = NextChair(m_bBanker); 

	if (pre_bank != m_bBanker)
		m_nCurRound++;

	log_debug("pre next %d %d   round[%d]   ", pre_bank, m_bBanker, m_nCurRound  );
}




int  Table_jilin::UserOperateGang(_uint8 ChairID, int iOpType, _uint8 bCard)
{
	 
	bool  can_qiang = true;
	if (!LingGuoHu(ChairID))
	{
		log_debug("cannot qianggang hu !!");
		can_qiang = false;
	}
	if (can_qiang && iOpType == TYPE_ZHIGANG )
	{
		if (m_bOutCardChairID >= 0 && m_bOutCardChairID < m_GAME_PLAYER
			&& m_jilin_baoting[m_bOutCardChairID].bao_ting
			&& m_allTingPosInfoMap[m_bOutCardChairID].out_size == 1
			&& m_allTingPosInfoMap[m_bOutCardChairID].outcard == bCard 
			&& m_allTingPosInfoMap[m_bOutCardChairID].pos == m_CardPool.Count()
			)
		{
			MJ_major major;
			int op = CheckOutCardOtherPlayerCanOpCode(m_bOutCardChairID, bCard , major, m_HuFanNums[m_bOutCardChairID]);
			if (op & TYPE_HU)
			{
				log_debug("spe hu[%d]", m_bOutCardChairID);
				m_QiangGangHu[m_bOutCardChairID] = 1;				 
				m_bCurChairID = ChairID; 				 
				m_bUserOperate[m_bOutCardChairID] = TYPE_HU;
				Table::UserOperateHu(m_bOutCardChairID, bCard);
				m_bOutCardChairID = ChairID; 
				GameEnd();
				return 1;				
			}
			

		}
	}
	Table::UserOperateGang(ChairID, iOpType, bCard);  
	return  0;
	 
}

void Table_jilin::Proto_Game_OperateGang(_uint8 ChairID, _uint8 bCard, int iOpType, int iGangScore[GAME_PLAYER], proto::game::AckBcOpResult& stOpresult)
{
	Table::Proto_Game_OperateGang(ChairID, bCard, iOpType, iGangScore, stOpresult);

}


void Table_jilin::Proto_GameEndInfo_GangInfo(_uint8 ChairID, proto::game::AckGameEnd& stGameend)
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
				if (!m_jilin_baoting[i].buzhang.Find(t.Card))
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



void Table_jilin::Proto_Game_OpNotify(_uint8 ChairID, _uint8 CurChairID, _uint8 bCard, proto::game::AckOpNotify& stNotify)
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
			if (1) // IsListening(ChairID, bCard))
			{
				proto::game::tagGangInfo* ganginfo = stNotify.add_gang();
				ganginfo->set_card(bCard);
				ganginfo->set_state(TYPE_ZHIGANG);
			}
			
		}
		stNotify.set_card(bCard);
		stNotify.set_optype(iOpType);
		log_debug("uid[%d] op[%x]" ,  GetPlayerUid(ChairID) , iOpType);

	}
	stNotify.set_outcardid(m_bOutCardChairID );

}


bool Table_jilin::CheckUserOutCard(_uint8 ChairID, _uint8 cbCard)
{
	
	
	if (!Table::CheckUserOutCard(ChairID, cbCard))
		return false;

	return true;
}

int  Table_jilin::GetCountSize(const  HandCards  *phc, _uint8 c)
{
	int  all = 0;
	for (int i = 0; i < phc->ChangeableCardsLen; i++)
	{
		if (c == phc->ChangeableCards[i])
		{
			all++;
		}
	}
	return  all; 
}


int  Table_jilin::get_card_size(const  HandCards  *phc , _uint8 c )
{
	int  all = 0;
	for (int i = 0; i < phc->ChangeableCardsLen; i++)
	{
		if (c == phc->ChangeableCards[i])
		{
			all++;
		}
	}
	if (all >= 4)
		return TYPE_ANGANG;
	else
		return TYPE_WANGANG;
}

void  Table_jilin::Reconnect(int ChairID)
{
	if (ChairID < 0 || ChairID >= m_GAME_PLAYER)
	{
		log_error("sid %d", ChairID );
		return;
	}
	if (m_jilin_baoting[ChairID].bao_ting  &&  m_outCardCountAfterBaoTing[ChairID] < 1)
	{
		TingHouFirstGang(ChairID);
		log_debug("connect %d", ChairID);
	}
}

bool Table_jilin::Check_Listening(HandCards *pHc, _uint8 ChairID, _uint8 bCard, tagListenInfos stListenInfo[14], _uint8 &bListenLen)
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
	_relativity.Add(0x42);
	_relativity.Add(0x43);
	map<int, int> zhongfa_map;


	int ret = 0;
	for (_uint8 i = 0; i < _relativity.Size(); i++)
	{

		ret = m_GameLogic.AddCard(&tempHandCard, _relativity[i]);
		MJ_fan_buffer fan_buffer;
		if (Get_Hand_All_Card_Count(tempHandCard, _relativity[i]) <= 4
			&& Check_Hu(ChairID, tempHandCard, fan_buffer, _relativity[i]) == TYPE_HU)
		{

			if (stListenInfo[bListenLen].bHuLen < 18)
			{
				if (zhongfa_map[_relativity[i]] == 1)
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
				m_TingHouCanOutPaiMap[bCard] = 1; 
			}
		}
		if (ret == 0)
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



void Table_jilin::Proto_Game_UserGetCard(_uint8 ChairID, _uint8 MoChairID, _uint8 bCard, proto::game::AckUserGetCard& stUsergetcard)
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
			if (m_jilin_baoting[ChairID].bao_ting)
			{
				map<int, int>  allmap;
				GangJudgeAfterTing(ChairID, stGang, allmap);  // debug  
				if (allmap.size())
				{
					stGang.Clear();
					for (map<int, int>::iterator it = allmap.begin(); it != allmap.end(); ++it)
					{
						stGang.Add(OpState(it->first,  get_card_size( &m_HandCard[ChairID] , it->first)) );
						log_debug("cant gang %x" , it->first );
					}
				}
			}

			proto::game::hunanmj::tagBuZhang* bu = stUsergetcard.mutable_bu();
			for (int i = 0; i < stGang.Size(); i++)
			{
				//if ( m_jilin_baoting[ChairID].bao_ting && stGang[i].cbCard != bCard)
					//continue;
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
		proto::game::jilin::JilinReconnectInfo  Ack; 
		ProAddSpecialGangInfo(ChairID, stUsergetcard , false ,  Ack );



	}

}

void Table_jilin::Proto_GameEndInfo(const MJ_user_buffer& UserID, proto::game::AckGameEnd& stGameend)
{

	
	MJ_user_buffer   huUserId;
	MultiHu(  huUserId ); 
	
	log_debug("hucount [%d]" , huUserId.Size() ); 
	for (int i = 0; i < UserID.Size(); i++)
	{
		 //log_debug("huid [%d]" , UserID[i] );
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

	//if (0) //  m_curGangPlayerSid != INVALID_CHAIRID)   // 这个地方。不会 跑到。。 
	//{
		
	//}
	//else
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
			if (m_oFan[win_seat].Find(MJ_FAN_TYPE_TONG_BAO))
				info[win_seat]->set_specialtype(HU_TONG_BAO );  // 通宝。。
			else if (m_oFan[win_seat].Find(MJ_FAN_TYPE_MO_BAO)) 
				info[win_seat]->set_specialtype(HU_MO_BAO);   //  6 
			else
				info[win_seat]->set_specialtype(HU_ZIMO); 		

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
				//int add_c = m_stHuScore[win_seat].stHu.Size() - 1;
				//for (int i = 0; i < add_c; i++)
				//{
				//	AddFanInfo* pfan = info[win_seat]->add_addfan();
				//	pfan->set_addtype(MJ_FAN_TYPE_GANGSHANGHUA);
				//	pfan->set_addnum(8);
				//	log_debug("addc[%d]", add_c);
				//}
			}

			for (_uint8 c = NextChair(win_seat); c != win_seat; c = NextChair(c))
			{
				info[c]->set_chairid(c);
				if (m_oFan[win_seat].Find(MJ_FAN_TYPE_TONG_BAO))
					info[c]->set_specialtype(0 );
				else if (m_oFan[win_seat].Find(MJ_FAN_TYPE_MO_BAO))
					info[c]->set_specialtype(0); 
				else
					info[c]->set_specialtype(HU_BEIZIMO);

				info[c]->set_fannum(0);
				
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
				//int add_c = m_stHuScore[win_seat].stHu.Size() - 1;
				//for (int i = 0; i < add_c; i++)
				//{
				//	AddFanInfo* pfan = info[win_seat]->add_addfan();
				//	pfan->set_addtype(MJ_FAN_TYPE_GANGHOUPAO);
				//	pfan->set_addnum(8);
				//	log_debug("add type [%d]", MJ_FAN_TYPE_GANGHOUPAO);
				//}
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
					//int add_c = m_stHuScore[i].stHu.Size() - 1;
					//log_debug("addc [%d]", add_c);
					//for (int st = 0; st < add_c; st++)
					//{
					//	AddFanInfo* pfan = info[i]->add_addfan();
					//	pfan->set_addtype(MJ_FAN_TYPE_GANGHOUPAO);
					//	pfan->set_addnum(8);
					//	log_debug("add fan 8");
					//}
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

	proto::game::jilin::JilinGameEnd   *pEnd = stGameend.mutable_jilingameend();
	pEnd->set_bao_card(m_baoPai );
	log_debug("bao_pai %x" , m_baoPai ); 

	for (int i = 0; i < m_GAME_PLAYER; i++)
	{
		JiGangInfo &ganginfo = m_JilinGangInfo[i];
		proto::game::jilin::RecAllSpeGangInfo  *pPlyaerOweGangInfo = pEnd->add_gang_list();
		pPlyaerOweGangInfo->set_chairid(i);
		for (int i = 0; i < ganginfo.len; i++)
		{
			gang_card   &pct = ganginfo.g_arr[i];
			proto::game::jilin::Speganginfo   *pInfo = pPlyaerOweGangInfo->add_gang_list();
			pInfo->set_gang_id(pct.gang_id);
			proto::game::jilin::gangcard *p = pInfo->add_gang_card_list();
			for (int j = 0; j < XUAN_FENG_GANG_ARR_LEN; j++)
			{
				if (pct.arr[j] == 0)  continue;
				p->add_card(pct.arr[j]);
				p->add_bu_count(pct.bu[j]);
			}
		}
	}

	for (int i = 0; i < m_GAME_PLAYER;i++)
	{
		proto::game::jilin::feninfo  *p = pEnd->add_fen_list();
		p->set_chairid(i); 
		p->set_hu_paifen( m_fenResult[i][1] );    
		p->set_gang_fen(   m_fenResult[i][0] );
	}
	if (m_Jilinchi.size())
	{		
		for (int s = 0; s < m_GAME_PLAYER; s++)
		{
			proto::game::jilin::zhongfabaiinfo  * p = NULL;
			log_debug("chi s %x" , s );
			for (int i = 0; i < (int)m_Jilinchi.size(); i++)
			{				
				if ( m_Jilinchi[i].chi_id == s)
				{
					if (p == NULL)
					{
						p = pEnd->add_chi_list();
						p->set_chairid(s);
						p->add_card(m_Jilinchi[i].chi_card);
						log_debug("chi %x" , m_Jilinchi[i].chi_card );
					}
					else
					{
						p->add_card(m_Jilinchi[i].chi_card);
						log_debug("chi %x", m_Jilinchi[i].chi_card);  
					}
				}
			}
		}
	}




}




void Table_jilin::Score_GameEndSucc(HuScoreSet& score, proto::game::AckGameEnd& stGameend)
{
	Table::Score_GameEndSucc(score, stGameend);   // 非流局

	for (int i = 0; i < m_GAME_PLAYER; i++)
	{
		//只有在每局的最后才将m_HandCard的杠牌状态转换成 TYPE_BU_ZHANG
		for (int j = 0; j < m_HandCard[i].FixedCardsLen; j++)
		{
			if (m_HandCard[i].FixedCards[j].state & TYPE_GANG &&
				m_jilin_baoting[i].buzhang.Find(m_HandCard[i].FixedCards[j].CardData))
			{
				m_HandCard[i].FixedCards[j].state = TYPE_BU_ZHANG;
			}
		}
	}

}

void Table_jilin::Calc_HuScore(_uint8 ChairID, int iBaseScore[GAME_PLAYER], int iScore[GAME_PLAYER])
{

	if (m_bIsHu[ChairID])
	{
		for (int j = 0; j < m_GAME_PLAYER; j++)
		{
			iScore[j] = m_TrueWinLost[j] + m_GangMoney[j];  
			log_debug("socre [%d]  win[%d] gang[%d]" , iScore[j] , m_TrueWinLost[j] , m_GangMoney[j]  );

		}
	}

	
}


int Table_jilin::Calc_Fan(_uint8 ChairID, MJ_fan_buffer& oFan)
{
	int iTotalFan = 0;
	MJ_fan_buffer oFilterFan;
	if (m_ft2fn->FanFilter(oFan, oFilterFan))
	{
		//计算番之前把如果过滤番，如果有大胡就过滤所有小胡
		for (int i = 0; i < oFilterFan.Size(); i++)
		{
			iTotalFan += m_ft2fn->GetFan(oFilterFan[i]);
			// log_debug("iTotal [%d]  GetFan[%d]", iTotalFan, m_ft2fn->GetFan(oFilterFan[i]));
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

bool Table_jilin::IsListening(_uint8 ChairID, _uint8 bRemoveCard)
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



void Table_jilin::Proto_GameScene(_uint8 ChairID, Player* player, proto::game::AckGameScene& stGameScene)
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
			info->add_jinzhihuanpai((m_jilin_baoting[i].bao_ting ) ? 1 : 0);
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
						(m_jilin_baoting[ChairID].buzhang.Find(pstFix->carddata())))
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
						(m_jilin_baoting[i].buzhang.Find(pstFix->carddata())))
					{
						pstFix->set_state(TYPE_BU_ZHANG);
					}
				}
			}
		}

	}
	proto::game::jilin::JilinReconnectInfo  *pRecInfo = stGameScene.mutable_reconect_info();		
	for (int i = 0; i < m_GAME_PLAYER; i++)
	{
		JiGangInfo &ganginfo = m_JilinGangInfo[i];
		proto::game::jilin::RecAllSpeGangInfo  *pPlyaerOweGangInfo = pRecInfo->add_gang_list();
		pPlyaerOweGangInfo->set_chairid(i);
		for (int i = 0; i < ganginfo.len; i++)
		{
			gang_card   &pct = ganginfo.g_arr[i];
			proto::game::jilin::Speganginfo   *pInfo = pPlyaerOweGangInfo->add_gang_list(); 
			pInfo->set_gang_id(pct.gang_id);
			proto::game::jilin::gangcard *p = pInfo->add_gang_card_list();
			for (int j = 0; j < XUAN_FENG_GANG_ARR_LEN; j++)
			{
				if (pct.arr[j] == 0)  continue;
				p->add_card(pct.arr[j]);
				p->add_bu_count(pct.bu[j]);
			}
		}
	}

	if (  m_bUserOperate[ChairID] & TYPE_JILIN_GANG)
	{
		proto::game::AckUserGetCard  Ack;
		ProAddSpecialGangInfo(ChairID, Ack, true, *pRecInfo);
	}
	pRecInfo->set_bao_card(m_baoPai); 
	log_debug("bao %x" , m_baoPai) ;
	if (bIsCanQiangGangOrPeng )
	{
		pRecInfo->set_bu_card(bu); 
		pRecInfo->set_bu_chairid(bu_chairid );
	} 

	//debug 
	for (int i = 0; i < (int)m_Jilinchi.size(); i++)
	{
		log_debug("chi id card pre %d %x %d", m_Jilinchi[i].chi_id, m_Jilinchi[i].chi_card, m_Jilinchi[i].pre_id);  
	}

	// end debug  
	if (m_Jilinchi.size())
	{
		for (int s = 0; s < m_GAME_PLAYER; s++)
		{
			proto::game::jilin::zhongfabaiinfo  * p = NULL; 
			for (int i = 0; i < (int)m_Jilinchi.size(); i++)
			{								
				if (m_Jilinchi[i].chi_id ==  s)
				{
					if (p ==  NULL )
					{
						p = pRecInfo->add_chi_list();
						p->set_chairid(s);
						p->add_card(m_Jilinchi[i].chi_card);
						log_debug("chi %x" ,   m_Jilinchi[i].chi_card );
					}
					else
					{
						p->add_card(m_Jilinchi[i].chi_card);
						log_debug("chi %x", m_Jilinchi[i].chi_card); 
					}										
				}				
			}
		} 		
	}
	

}


void Table_jilin::Proto_GameScene_Operate(_uint8 ChairID, Player* player, proto::game::AckGameScene& stGameScene)
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
		//no_bao_ting = 0;
		log_debug("baoting %x", TYPE_BAO_TING);
	}
	else
	{
		if (m_nRecvBaoTingPlayerMap[ChairID] & TYPE_BAO_TING || (m_nRecvBaoTingPlayerMap[ChairID] & TYPE_PASS))   // 报听修改。  
		{
			stGameScene.set_optype(m_bUserOperate[ChairID] & (~TYPE_BAO_TING));
			//no_bao_ting = 1;
			log_debug("code %x  ", m_bUserOperate[ChairID] & (~TYPE_BAO_TING)) ;
		}		
		else
		{
			stGameScene.set_optype(m_bUserOperate[ChairID]);
			log_debug("code %x  " , m_bUserOperate[ChairID]);
		}
		

	}

	
	if (m_bUserOperate[ChairID] & TYPE_GANG)
	{
		stGameScene.clear_ganginfo();
		MJ_opinfo stGang;
		if (m_bOutCardChairID != ChairID && !m_jilin_baoting[ChairID].bao_ting && m_bGrapId != ChairID  )
		{
			m_GameLogic.GetPengGang(&m_HandCard[ChairID], m_bCurOutCard, stGang);
			for (int i = 0; i < stGang.Size(); i++)
			{
				if (stGang[i].nState == TYPE_PENG)   continue;
				proto::game::tagGangInfo* ganginfo = stGameScene.add_ganginfo();
				ganginfo->set_card(stGang[i].cbCard);
				ganginfo->set_state(stGang[i].nState); 
					//log_debug("gang %x  %x   "  ,   stGang[i].cbCard  , stGang[i].nState  );
			}
		}
		else  if (m_bGrapId == ChairID)
		{
			MJ_major   ma_jor;
			int  fan = 0;
			CheckPlaySlefGrpCardCanOperateCode(ChairID, m_PlayGrapCard[ChairID], ma_jor, fan);
			for (int i = 0; i < ma_jor.m_GetGangInfo.Size(); i++)
			{
				proto::game::tagGangInfo* ganginfo = stGameScene.add_ganginfo();
				ganginfo->set_card(ma_jor.m_GetGangInfo[i].cbCard);
				ganginfo->set_state(ma_jor.m_GetGangInfo[i].nState);
				 //	log_debug("gang %x %x  " , ma_jor.m_GetGangInfo[i].cbCard  , ma_jor.m_GetGangInfo[i].nState );
			}
		}
	}
	int iOpType = m_bUserOperate[ChairID]; 
	if (stGameScene.ganginfo_size() == 0)
	{
		iOpType &= ~(TYPE_GANG);
	}
	stGameScene.set_optype(iOpType); 

	// log_debug("short_info  %s  ", stGameScene.ShortDebugString().c_str()); 

}

void Table_jilin::Proto_GameScene_lookon(_uint8 ChairID, Player* player, proto::game::AckGameScene& stGameScene)
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

int  Table_jilin::UserOperateHu(_uint8 ChairID, _uint8 bHuCard /* = 0xFF */)
{
	return  Table::UserOperateHu(ChairID, bHuCard);	

}

int Table_jilin::TingHouFirstGang(_uint8  ChairID )
{
	if (ChairID >= m_GAME_PLAYER)
	{
		log_error("error chairid %d" , ChairID);
		return  0;
	} 

	proto::game::AckUserGetCard  stUsergetcard; 
	map<_uint8, int>  bu_gangid_map;
	JiGangInfo   &pGangInfo = m_JilinGangInfo[ChairID];

	proto::game::jilin::SpeAllGangInfo  * pAdd  =  stUsergetcard.mutable_ganglist();
	for (map<int, int>::iterator it= m_TingHouCanOutPaiMap.begin(); it != m_TingHouCanOutPaiMap.end();++it)
	{
		if( !In_Hand_New(&m_HandCard[ChairID] , it->first ))   continue;  
		for (int i = 0; i < pGangInfo.len; i++)
		{
			gang_card   &pCurGangInfo = pGangInfo.g_arr[i];
			for (int j = 0; j < XUAN_FENG_GANG_ARR_LEN; j++)
			{
				int ct = pCurGangInfo.arr[j]; 
				if (ct == 0  || ct== 0x11  ) continue; 
				if (ct> 0x30 && ct< 0x40 && it->first > 0x30 && it->first < 0x40)
					bu_gangid_map[it->first] = pCurGangInfo.gang_id; 
				else if (ct>0x40 && it->first> 0x40 && it->first < 0x44)
					bu_gangid_map[it->first] = pCurGangInfo.gang_id;
				else if (ct < 0x30 && it->first< 0x30  && get_face(ct) == 1 && get_face(it->first) == 1 && g_jilin_conf.yao_jiu_dan)
					bu_gangid_map[it->first] = pCurGangInfo.gang_id;
				else if (ct < 0x30 && it->first < 0x30 && get_face(ct) == 9 && get_face(it->first) == 9 && g_jilin_conf.yao_jiu_dan)
					bu_gangid_map[it->first] = pCurGangInfo.gang_id;
				else if ( it->first == 0x11 && g_jilin_conf.xiaojifeidan  )
					bu_gangid_map[it->first] = pCurGangInfo.gang_id;
			}
		}
	}

	
	map<_uint8, int>::iterator it = bu_gangid_map.begin();
	for (; it != bu_gangid_map.end(); ++it)
	{
		
		proto::game::jilin::Speganginfo *pSpe = pAdd->add_gang_list();
		proto::game::jilin::gangcard *pt = pSpe->add_gang_card_list();
		pSpe->set_gang_type(2);
		pSpe->set_gang_id(it->second);
		pt->add_card(it->first);
		log_debug("first %x sec %x   grap %x ", it->first, it->second, m_PlayGrapCard[ChairID]);
	}

	if (bu_gangid_map.size())
	{
		stUsergetcard.set_optype(TYPE_JILIN_GANG);
		stUsergetcard.set_chairid(ChairID );
		tagListenInfos stListenInfo[14];
		_uint8 bListenLen = 0;		
		GetListeningInfo(ChairID, stListenInfo, bListenLen); 	 
		 
		for (int j = 0; j < bListenLen; j++)
			PackageTingInfo(ChairID, stListenInfo, j, stUsergetcard.add_listeninfo());


		m_bUserOperate[ChairID] = TYPE_JILIN_GANG; 
		unicast( SER_CMD_JILING_GANG_INFO_AFTER_GANG , ChairID, stUsergetcard);
		log_debug("ting   %s  ", stUsergetcard.ShortDebugString().c_str() );

	}

	//	m_TingHouCanOutPaiMap.clear(); 

	return  0; 
}




