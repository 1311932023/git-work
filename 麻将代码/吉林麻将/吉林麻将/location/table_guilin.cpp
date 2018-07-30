#include "table_guilin.h"
#include <vector>
#include <math.h>
void Table_guilin::GameEndReset()
{
	m_TianGangList.clear();
	m_DiGangList.clear();
	m_zui.clear();
	m_preoutCard = 0xFF;
	m_GangMap.clear();
	m_yiGangDaoDiVec.clear();
	memset(m_nPlayerOp, -1 , sizeof(m_nPlayerOp));
	m_IsTianhu = false;
	
	memset( m_guoZhangHuArr , 0 ,  sizeof(m_guoZhangHuArr)) ;

	memset(m_hubao,  0 , sizeof(m_hubao)) ; 
	memset(m_playerXingCount, 0, sizeof(m_playerXingCount));
	m_AllLunXingCard.clear(); 
	memset(m_WinLostArr, 0, sizeof(m_WinLostArr));
	memset(m_TmpGuoArr, 0, sizeof(m_TmpGuoArr));
	
	memset(m_zhuaXingOverArr, 0, sizeof(m_zhuaXingOverArr));
	for (int i = 0; i < m_GAME_PLAYER; i++)
	{
		m_CanotOutAfterChiVec[i].clear();  
	}
 
	Table_changsha::GameEndReset();

}
Table_guilin::Table_guilin(int player)
{

	m_GAME_PLAYER = player;
	m_preoutCard = 0xFF;
	m_IsTianhu = false;
	memset(m_WinLostArr, 0, sizeof(m_WinLostArr));
	memset(&g_guilin_conf, 0, sizeof(guilin_conf));
	m_ft2fn = &g_guilin_ft2fn;	
	m_conf.m_AnGangPoint = 2;
	m_conf.m_WanGangPoint = 3;
	m_conf.m_ZhiGangPoint = 3 ;
	m_GangMap.clear();
	m_yiGangDaoDiVec.clear(); 
	memset(m_nPlayerOp, -1, sizeof(m_nPlayerOp)); 
	memset(m_TmpGuoArr, 0, sizeof(m_TmpGuoArr));
	memset(m_zhuaXingOverArr, 0, sizeof(m_zhuaXingOverArr));
	memset( m_guoZhangHuArr, 0, sizeof(m_guoZhangHuArr));

	m_AllLunXingCard.clear();
	memset(m_playerXingCount, 0, sizeof(m_playerXingCount));
	memset(m_hubao, 0, sizeof(m_hubao));

	m_conf.m_bCalcAllNiao = false;
	

}


Table_guilin::~Table_guilin()
{

}

void  Table_guilin::GameStart()
{
	Table::GameStart();
	Table::OnUserGrapCard(m_bBanker);	
}



int Table_guilin::Check_Hu(_uint8 ChairID, HandCards& pHc, MJ_fan_buffer& oFan , _uint8 add_last_card )
{
	
	if (pHc.ChangeableCardsLen > 14)
	{
		log_error("cardlen %d", pHc.ChangeableCardsLen);
		return TYPE_NULL;
	}
	//取余3不等于2的手牌不做胡牌判断
	if (pHc.ChangeableCardsLen != 0 && pHc.ChangeableCardsLen % 3 != 2)
	{
		log_error("cardlen[%d] uid[%d]%s", pHc.ChangeableCardsLen, GetPlayerUid(ChairID), CatHandCard(m_HandCard[ChairID] ,ChairID) );
		return TYPE_NULL; 
	}
	//oFan.Clear(); 	 
	MJ_win_pattern pattern;
	if ( Is7Dui(ChairID, pHc, oFan))
	{		
		log_info("3.七大对");		
	}	  
	if (All_Is_Feng(&pHc))
	{
		oFan.Add(MJ_FAN_ZI_YI_SE);
		log_debug("zi yise");
	}
	if ( (!oFan.Find(MJ_FAN_ZI_YI_SE))  &&   xi_lan(&pHc))
	{
		oFan.Add(MJ_FAN_XI_LUAN);
		log_debug("xi luan 1!");
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


void Table_guilin::DoMultiOperate(_uint8 ChairID, int iOpType, _uint8 bCard)
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
		return DoMultiOperate_HuWeiDa(ChairID, iOpType, bCard);
	}
	Table::DoMultiOperate(ChairID, iOpType, bCard);

}

void Table_guilin::ConfInit_Remote(const Json::Value& val)
{
	Table::ConfInit_Remote(val);

	m_conf.m_bZimoHu        = false ;   // 这个变量是 只能自模胡的意思 。。我日。。 	
	m_conf.m_bSupportChi    = true  ;
	m_conf.m_bSupportZhuaniao = false;
	m_conf.m_bQiangGangHu = true   ;
	m_conf.m_bQiangAnGang = false;
	m_conf.m_bQiangZhiGang = false;  
	m_conf.m_bFourceHuMutilHu = true ;    

	m_conf.m_bYiPaoDuoXiang =  true ;
	

	m_CardPool.Set( CARD_POOL_NO_FLOWER, tid);
	m_CardPool.GameId = GUILIN_MAJIANG_GAME_ID;   // 3030 


	if (json_check_play_type_id(val))
	{
		MJ_SUB_ID subid;
		json_get_play_type_id(val, 3031 , subid);
		if (subid.Find(3031001))
		{			
			g_guilin_conf.yi_gang_dao_di = true;
			log_debug(" add YI GANG DAO DI  ");

		}
		if (subid.Find(3031002))
		{
			g_guilin_conf.quan_qiu_ren_buqiuzi = true;
			log_debug("  QUAN QIU REN  ");
		}
		if (subid.Find(3031003))
		{
			g_guilin_conf.haidi_laoyue2 = true;
			log_debug("  HAI DI LAOYUE  2");
		}
		if (subid.Find(3031004 ))
		{
			g_guilin_conf.menqing = true;
			log_debug(" MEN QING  ");
		}
		if (subid.Find(3031005 ))
		{
			g_guilin_conf.chisandou_hubao = true;
			log_debug(" CHI SAN DOU HU BAO  ");
		}
		if (subid.Find(3031006 ))
		{
			g_guilin_conf.quanquan_zhui  = true;
			log_debug(" QUANQUANZHI  ");
		}
		m_room_config += subid;   
		json_get_play_type_id(val, 3032, subid);
		if (subid.Find(3032001))
		{
			g_guilin_conf.shangzhongxia_xing = true;
			log_debug("pre b next");
		}
		if (subid.Find(3032002 ))
		{
			g_guilin_conf.shangxia_xing = true;
			log_debug("ss");
		}
		m_room_config += subid;

	}

}



int Table_guilin::Calc_Fan(_uint8 ChairID, MJ_fan_buffer& oFan)
{
	if (ChairID < 0 || ChairID >= m_GAME_PLAYER)
		return 0;

	TmpFan[ChairID].Clear();
	TmpFan[ChairID] = oFan; 

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


bool Table_guilin::EstimateUserRespond(_uint8 bOutChairID, _uint8 bOutCard)
{
		
	Table::EstimateUserRespond(bOutChairID, bOutCard); 
	
	
	if(g_guilin_conf.yi_gang_dao_di)   // 上家打出去一张，， 其他人，没有吃碰，， 一杠到底，就取消 。
	{
		memset(m_nPlayerOp, -1 , sizeof(m_nPlayerOp));
		int all = 0, hu_all = 0;
		for (int i = 0; i < m_GAME_PLAYER; i++)
		{
			if(bOutChairID == i ) 
				continue; 
			if(m_bUserOperate[i] &  TYPE_ZHIGANG )
				all++;
			if (m_bUserOperate[i] & TYPE_HU)
				hu_all++;
		}
		if (all == 0 && hu_all == 0)
		{
			log_debug("gang ting!");
			m_yiGangDaoDiVec.clear();
		}
		//log_debug("all %d %d" , all ,  hu_all );
		
	}
	
	return true;

}

//   1天杠，2地杠。 3天追 。 4普通追   5 普通杠 

void  Table_guilin::DealZhui(map<_uint8, int> & zui_map, _uint8 bOutCard ,   bool  ququan_zhui)
{
	if (zui_map[bOutCard] > 1000)
	{
		if (g_guilin_conf.quanquan_zhui)
		{
			int pay_id = zui_map[bOutCard] / 1000 - 1;
			if (pay_id < 0 || pay_id >= m_GAME_PLAYER)
			{
				log_error("error %d" , pay_id );
				return;
			}
			if (m_bBanker < 0 || m_bBanker >= m_GAME_PLAYER )
				return;

			int base = 2;
			_uint8  op_type = 4;
			if (PlayerAllOutCard[m_bBanker].Size() == 1)
			{
				base *= 2;
				op_type = 3;
				
			}
			log_debug("base [%x] type[%x]  payid [%d]", base, op_type , pay_id );
			int fen_arr[GAME_PLAYER] = { 0 };
			for (int i = 0; i < m_GAME_PLAYER; i++)
			{
				if (i == pay_id)
				{
					fen_arr[i] = -base * 3;
				}
				else
				{
					fen_arr[i] = base;
				}
			} 
			Guilin_OpAck(op_type, fen_arr);
			zui_map.clear();
		}
		else
		{
			if (m_bBanker < 0 || m_bBanker >= m_GAME_PLAYER)
				return;

			if (PlayerAllOutCard[m_bBanker].Size() != 1)
				return;
			int pay_id = zui_map[bOutCard] / 1000 - 1;
			int base = 2;						
			int fen_arr[GAME_PLAYER] = { 0 };
			for (int i = 0; i < m_GAME_PLAYER; i++)
			{
				if (i == pay_id)				
					fen_arr[i] = -base * 3;				
				else				
					fen_arr[i] = base;
				
			}
			Guilin_OpAck(  PU_TONG_ZHUI , fen_arr);
			zui_map.clear();
		}		
	}
}

void Table_guilin::OnUserOutCard(_uint8 ChairID, _uint8 bOutCard)
{
	if (ChairID < 0 || ChairID >= m_GAME_PLAYER)
		return;

	m_GangMap.clear();	
	if (g_guilin_conf.yi_gang_dao_di)
		memset(m_TmpGuoArr, 0, sizeof(m_TmpGuoArr));

	m_guoZhangHuArr[ChairID] = 0; 
	
	m_CanotOutAfterChiVec[ChairID].clear();

	Table::OnUserOutCard(ChairID, bOutCard);

	if( m_preoutCard == 0xFF || m_preoutCard != bOutCard )  //  1234 map< card, sid--> 1234 //
	{
		m_zui.clear();
		m_zui[bOutCard] = (ChairID + 1);
		m_preoutCard = bOutCard; 
	}  
	else
	{
		m_zui[bOutCard] = m_zui[bOutCard] * 10 + (ChairID + 1);  // 第一个人。出钱。2分。 
		m_preoutCard = bOutCard;
		if ( g_guilin_conf.quanquan_zhui)
		{
			DealZhui( m_zui, bOutCard, true);
		}
		else
		{
			DealZhui(m_zui, bOutCard, false); 
		}

	}
	
	
}




int Table_guilin::CheckPlaySlefGrpCardCanOperateCode(_uint8 ChairID, _uint8 bCard, MJ_major& major, int& fan_num)
{
	int iOpType = 0;	
	iOpType = Table::CheckPlaySlefGrpCardCanOperateCode(ChairID, bCard, major, fan_num);	
	return iOpType;

}



int Table_guilin::CheckOutCardOtherPlayerCanOpCode(_uint8 ChairID, _uint8 bCard, MJ_major& major, int& fan_num)
{
	int iOpType = 0;	
	if (ChairID < 0 || ChairID >= m_GAME_PLAYER)
		return 0;

	iOpType = Table::CheckOutCardOtherPlayerCanOpCode(ChairID, bCard, major, fan_num);		
	if (m_bCurChairID != PreChair(ChairID))   // 不是出牌的下家不能吃。 
	{
		iOpType &= ~TYPE_CHI;
	}
	if( m_guoZhangHuArr[ChairID]  &&  iOpType & TYPE_HU )
	{
		iOpType &= ~TYPE_HU; 
		bool guozhanghu = true;
		MJ_fan_buffer  nBigOutFan;
		MJ_fan_buffer  nSmallOutFan;
		m_ft2fn->FanFilter_Guilin(  major.m_oFan , nBigOutFan, nSmallOutFan);
		if (m_guoZhangHuArr[ChairID] == MJ_FAN_XI_LUAN  &&  major.m_oFan.Find(MJ_FAN_QI_XI_GUI_LIN))
		{
			guozhanghu = false;
			iOpType |= TYPE_HU;
		}			
		else if (nBigOutFan.Size() && m_guoZhangHuArr[ChairID] == 1)
		{
			guozhanghu = false;
			iOpType |= TYPE_HU;
		}
		if (guozhanghu)
		{
			proto::game::AckMessage msg;
			msg.set_message(msg_word(E_MW_GUO_ZHUANG));
			unicast(SERVER_SEND_MESSAGE, ChairID, msg);
		}			
		log_debug("guozhanghu  sid[%d]  uid[%d]  iOpType[%x]  guo[%d]" , ChairID  , GetPlayerUid(ChairID) , iOpType , m_guoZhangHuArr[ChairID] );
	}	
	iOpType &= ~( TYPE_BU_ZHANG ) ;	
	log_debug("ioptype [%x]", iOpType);

	return iOpType;
}


void Table_guilin::OnUserGrapCard(_uint8 ChairID, bool bGetLast)
{
	m_guoZhangHuArr[ChairID] = 0; 
	//log_debug("cur--pos [%d]" , m_CardPool.get_cur_pos() );

	if (m_CardPool.get_cur_pos() >= 122  )  // 留下 14 张。
	{
		GameEnd();
		log_debug("game end  !!!! ");
		return;		
	}
	Table::OnUserGrapCard(ChairID, bGetLast);
	
}



void Table_guilin::UserAfterOperate(_uint8 ChairID, int iOpType, _uint8 bCard)
{
	m_bCurChairID = ChairID;
	m_guoZhangHuArr[ChairID] = 0;
	m_CanotOutAfterChiVec[ChairID].clear();  // 不能打的牌 

	log_info(" uid[%d] chairid[%d] operate[%x] card[%x] ", GetPlayerUid(ChairID), ChairID, iOpType, bCard);

	m_bUserOperate[ChairID] = TYPE_NULL;
	MJ_opinfo gang;
	m_bUserOperate[ChairID] = GetGangInfo(m_bCurChairID, gang);
	
	
	if (gang.Size() == 0 ) 
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



void  Table_guilin::DealHuBao(_uint8  ChairID)
{
	if (g_guilin_conf.chisandou_hubao == false)
		return; 
	if (ChairID< 0 || ChairID >= m_GAME_PLAYER)
		return;

	const  HandCards *phc = &m_HandCard[ChairID];
	if (phc->FixedCardsLen >= 3)
	{
		map<_uint8, _uint8>  bao_map;
		for (int j = 0; j < phc->FixedCardsLen; j++)
		{
			_uint8  ct = phc->FixedCards[j].chairID;   // 对方打的。 
			if (ct != ChairID  && ct >=0 && ct <   m_GAME_PLAYER   )
			{
				bao_map[ct]++;
				if (bao_map[ct] >= 3)
				{
					m_hubao[ChairID][ct] = 1;  
					int type = 1;
					if (m_hubao[ct][ChairID] == 1)
						type = 2;
					proto::game::GuilinHuBao  BaoAck;
					BaoAck.set_chairid(ChairID);
					BaoAck.set_other_chairid(ct);
					BaoAck.set_bao_type(type);
					broadcast(SER_CMD_GUI_LIN_BAO, BaoAck);
				}				
				
				log_debug("hubao success [%d] [%d]  baomap[%lu] ", GetPlayerUid(ChairID), GetPlayerUid(ct) , bao_map.size() );
			}

		}
	}

}
void Table_guilin::UserOperatePeng(_uint8 ChairID, _uint8 bCard)
{
	if (g_guilin_conf.yi_gang_dao_di)
	{
		log_debug("gang ting!");
		m_yiGangDaoDiVec.clear();
	}
		

	if ( m_curGangPlayerSid != INVALID_CHAIRID)
	{
		GangHouClear();
	}
	Table::UserOperatePeng(ChairID, bCard);
	DealHuBao(ChairID);

}


void Table_guilin::Proto_Game_OpNotifyAfterPengChi(_uint8 ChairID, _uint8 CurChairID, const MJ_opinfo& gang, proto::game::AckOpNotify& stNotify)
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
			//m_bUserOperate[ChairID] |= TYPE_BU_ZHANG;
			//iOpType = m_bUserOperate[ChairID] & ~TYPE_GANG;;

			proto::game::hunanmj::tagBuZhang* bu = stNotify.mutable_bu();
			for (int i = 0; i < gang.Size(); i++)
			{
				proto::game::hunanmj::tagBuinfo* buinfo = bu->add_bu();
				buinfo->set_card(gang[i].cbCard);
				buinfo->set_state( TYPE_GANG  );

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
			//iOpType |= TYPE_BAO_TING;
		}
		stNotify.set_card(0);
		stNotify.set_optype(iOpType);
	}
}



void Table_guilin::UserOperateChi(_uint8 ChairID, int iOpType, _uint8 bCard)
{
	
	if (g_guilin_conf.yi_gang_dao_di)
	{
		m_yiGangDaoDiVec.clear();
		log_debug("gang ting!");
	}
		

	Table_changsha::UserOperateChi(ChairID, iOpType, bCard);
	DealHuBao(ChairID);

	deal_chi_op(ChairID, iOpType, bCard); 

}


void Table_guilin::OnRecvUserOperate(_uint8 ChairID, int iOpType, int iCard)
{
	if (ChairID < 0 || ChairID >= m_GAME_PLAYER)
	{
		log_error("error %d" ,ChairID);
		return; 
	}		
	m_nPlayerOp[ChairID] = iOpType;

	if ((iOpType == TYPE_PASS) && (m_bUserOperate[ChairID] & TYPE_HU) && (ChairID != m_bCurChairID))
	{		
		if ( TmpFan[ChairID].Find(MJ_FAN_XI_LUAN))
			m_guoZhangHuArr[ChairID] = MJ_FAN_XI_LUAN;  
		else
		{
			 MJ_fan_buffer  nBigOutFan;							  
			 MJ_fan_buffer  nSmallOutFan; 
			 m_ft2fn->FanFilter_Guilin( TmpFan[ChairID], nBigOutFan, nSmallOutFan);
			 if (nBigOutFan.Size() == 0)
				 m_guoZhangHuArr[ChairID] = 1;  //  1 xiaohu 2  dahu  
			 else
				 m_guoZhangHuArr[ChairID] = 2; 
		}					
		log_debug("guozhanghu   sid[%d]  uid[%d]  m_bCurChairID[%d] iOpType[%d]  guo[%d]",
			ChairID, GetPlayerUid(ChairID), m_bCurChairID, iOpType , m_guoZhangHuArr[ChairID] );

	}
	if (g_guilin_conf.yi_gang_dao_di &&  ChairID != m_bCurChairID  && m_yiGangDaoDiVec.size() ) // && m_bMultiOpUserNum > 1 )
	{
		int guo = 0;
		int can_op = 0;
		for (int i = 0; i < m_GAME_PLAYER; i++)
		{
			if (i == m_bCurChairID) continue; 
			if ((m_bUserOperate[i] & TYPE_HU || m_bUserOperate[i] & TYPE_GANG)) // && m_nPlayerOp[i]== TYPE_PASS  )
			{
				can_op++;
				if( m_nPlayerOp[ChairID]>= 0 &&  !(m_nPlayerOp[i] == TYPE_HU || m_nPlayerOp[i] & TYPE_GANG) )
					guo++;
			}
		}
		if (guo == can_op)
		{
			log_debug("gang ting!");
			m_yiGangDaoDiVec.clear();
		}
			
	}
	
	

	Table::OnRecvUserOperate(ChairID, iOpType, iCard);  

}

void Table_guilin::DoSimpleOperate(_uint8 ChairID, int iOpType, _uint8 bCard)
{
	// 过张胡。。操作。 

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
					if (ret >= 0) 
						GameEnd();
					
	}break;
		
	default:
		break;
	}
}

int Table_guilin::GetAnGagnCount(const  HandCards *phc)
{
	if (phc->FixedCardsLen < 4)
		return 0;
	int angang = 0;
	for (int i = 0; i < phc->FixedCardsLen; i++)
	{ 
		if (phc->FixedCards[i].state  & TYPE_ANGANG)
			angang++;
	}
	//log_debug("gang %d" , angang ); 
	return angang;
}


bool Table_guilin::is_gang_shang_kai_hua(_uint8  chairID, bool zimo)
{
	if (chairID >= 0 && chairID < m_GAME_PLAYER)
	{
		log_debug("ChairID[%d]   m_bAddFanFlag[%d]", chairID, m_bAddFanFlag[chairID]);
		if (m_bAddFanFlag[chairID] == 1 && zimo)
		{
			return true;
		}
	}
	return false;
} 


bool  Table_guilin::IsQuanqiuren(const HandCards& pHc, MJ_fan_buffer& oFan, int& iOpType) 
{
	return m_GameLogic.IsDanDiao(pHc);
	//return  true;   
}

int Table_guilin::AddFanFunXiaoHu(_uint8 ChairID, const HandCards& pHc,
									MJ_fan_buffer& oFan, const MJ_win_pattern& oPattern, bool  IsAddJiangDui , _uint8  add_last  )
{	
	//、、加番牌型。  2杠上开花 1
	// 2条 碰碰胡   胡牌的牌型。  可以碰/杠/补，此时任意对子都可以做将，不再限定258
	bool duiduihu = false;
	bool qinyise = false;
	bool IsBigHu = oFan.Size() > 0;
	
	int nFan = 3;
	bool isPengpenghu = false;
	//4句话全部吃 / 碰 / 杠，手上拿着一张胡牌，如果4句话全是暗杠  不算求人。 
	//全求人。。 不可以自摸。。桂林麻将 

	if (oFan.Size() && (m_bCurChairID != ChairID)  && (GetAnGagnCount(&pHc) != 4) && IsQuanqiuren(pHc, oFan, nFan))
	{
		if (g_guilin_conf.quan_qiu_ren_buqiuzi  &&  m_bCurCard > 0x30)
		{
			log_debug(" budiao zi  ");
		}
		else
		{
			oFan.Add(MJ_FAN_TYPE_QUANQIUREN); 
			log_info(" 单调 6-9  ");
		}
		
	}
	if (oFan.Find(MJ_FAN_XI_LUAN) &&  feng_type_count(&pHc) == 7 )
	{
		oFan.Add(MJ_FAN_QI_XI_GUI_LIN);
		oFan.Remove(MJ_FAN_XI_LUAN); 
		log_debug("qi xi !!");
	}
	//、、加番牌型。  2杠上开花
	if (oFan.Size() && is_gang_shang_kai_hua(ChairID, m_bCurChairID == ChairID))
	{
		oFan.Add(MJ_FAN_TYPE_GANGSHANGHUA);
		IsBigHu = true;
		//nFan += 8;
		log_info(" 2.杠上开花 3-5 ");
	}
	//13.杠上炮
	if (oFan.Size() && ChairID != m_bCurChairID  && !m_bRobWanGang && m_bAddFanFlag[m_bCurChairID] )
	{
		oFan.Add(MJ_FAN_TYPE_GANGHOUPAO);
		//nFan += 8;
		IsBigHu = true;
		log_info("gang shang pao  nfan[%d] m_buzhang[ChairID].bao_ting[%d]", nFan, m_buzhang[ChairID].bao_ting);
	}
	//门清
	if ((!oFan.Find(MJ_FAN_TYPE_QIDUI)) &&  g_guilin_conf.menqing  &&  (oFan.Find(MJ_FAN_XI_LUAN)==0)  
		  &&  oFan.Size() 
		  && ChairID == m_bCurChairID 
		  && m_GameLogic.IsMenQing(&pHc))
	{
		oFan.Add(MJ_FAN_TYPE_MENQIANQING);
		log_debug("menqing"); 
	}

	if (m_GameLogic.IsPengPengHu(&pHc, oPattern.kan, oPattern.jiang))
	{
		duiduihu = true;
		oFan.Add(MJ_FAN_TYPE_PENGPENGHU);
		//nFan += 12;
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
		log_info(" 清一色 15-35 " );
	}
	
	//  4  海底捞月。 加番牌型。 15.海底捞月 海底捞月 19 
	if ( g_guilin_conf.haidi_laoyue2  &&   oFan.Size() &&  
		  m_GameLogic.hai_di_lao_yue(  (m_CardPool.IsEmpty()  ||  m_CardPool.get_cur_pos() >=  122 ) , 0, ChairID == m_bCurChairID, 0) 
		  )
	{
		oFan.Add(MJ_FAN_TYPE_HAIDILAOYUE);
		//nFan += 8;
		IsBigHu = true;
		log_info(" 15.海底捞月 3-5");
	}
	if (oFan.Size() && m_GameLogic.hai_di_pao( (  (m_CardPool.get_cur_pos() >=122)  || m_CardPool.IsEmpty()  )  , ChairID != m_bCurChairID))
		oFan.Add(MJ_FAN_TYPE_HAIDIPAO);

	if (oFan.Size() && m_bBanker == ChairID && m_bCurChairID == ChairID&&   other_chu_pai() )   
	{
		oFan.Add(MJ_FAN_TYPE_TIANHU);		
		log_debug("add tian hu  ");  
	}
	
	// 地胡  庄家起手14张打出一张后，其他人胡牌，则为地胡

	if (oFan.Size() &&  (m_bBanker != ChairID)  &&  (m_bBanker == m_bCurChairID)   &&   di_hu_guilin(ChairID))
		oFan.Add(MJ_FAN_TYPE_DIHU);

	if (oFan.Find(MJ_FAN_QI_XI_GUI_LIN) 
		|| oFan.Find(MJ_FAN_HAO_HUA_QI_XIAO_DUI)
		|| oFan.Find(MJ_FAN_HAO_HUA_DOUBLE_QI_XIAO_DUI) 
		|| oFan.Find(MJ_FAN_HAO_HUA_THREE_QI_XIAO_DUI )) 
	{
		oFan.Remove(MJ_FAN_TYPE_MENQIANQING);
	}

	return oFan.Size() > 1;
}

int Table_guilin::di_hu_guilin(_uint8  chairid )
{
	for (int i = 0; i < m_GAME_PLAYER; i++)   // m_bGetCardCount
	{
		if ( (chairid != i || i != m_bBanker )  && m_bGetCardCount[i])
			return false;
		if (i != m_bBanker &&  PlayerAllOutCard[i].Size())
			return  false;
	}
	
	return false;
}


int Table_guilin::Check_Hu_Pattern(_uint8 ChairID, const HandCards& pHc, MJ_fan_buffer& oFan, const MJ_win_pattern& oPattern)
{
		
	//  门清要是自己莫得。 
	log_debug("charid[%d] sid[%d] uid[%d] m_bCurCard[%x]", m_bCurChairID, ChairID, GetPlayerUid(ChairID), m_bCurCard);
	bool IsAddJiangDui = false;
	//log_debug("all %s   _xiaohua_conf.is_3fen_di [%d]", CatPattern(oPattern), _xiaohua_conf.is_3fen_di );

	
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



void Table_guilin::Score_GameEndDraw(HuScoreSet& score, proto::game::AckGameEnd& stGameend)
{
	if (m_conf.m_bHuangGang)
	{
		memset(m_stGang, 0, sizeof(m_stGang));
	}
	if (m_bBanker < 0 || m_bBanker >= m_GAME_PLAYER)
		return;

	
	MJ_user_buffer UserID;
	MultiHu(UserID); 
	if (UserID.Size() == 0)
	{
		if (m_nPlayingTimes == 1)
			return;
		else
		{
			for (int i = 0; i < m_GAME_PLAYER; i++)
			{
				if (i == m_bBanker) continue;
				//iTotalScore[i] += 2;
				//iTotalScore[m_bBanker] -= 2; 
				score[i].iTotleScore += 2;
				score[m_bBanker].iTotleScore -= 2;
			}
			log_debug("huang zhuang   jisuan ..!!");
			return;
		}
	}

	

}


void Table_guilin::Calc_HuScore(_uint8 ChairID, int iBaseScore[GAME_PLAYER], int iScore[GAME_PLAYER])
{	
	if (m_bIsHu[ChairID])
	{
		if (m_bCurChairID == ChairID)  //自摸
		{
			for (_uint8 c = NextChair(ChairID); c != ChairID; c = NextChair(c))
			{
				//iScore[ChairID] += iBaseScore[c];
				//iScore[c] -= iBaseScore[c];
				iScore[ChairID] += abs(m_WinLostArr[ChairID][c]);
				iScore[c] -=       abs(m_WinLostArr[ChairID][c]);
			}
		}
		else     // 放炮 
		{
			if ( m_stHuScore[ChairID].stHu.Size() == 0)  // modify  
			{
				log_error("error");
				return;
			}
			//_uint8 iPaoID = m_stHuScore[ChairID].stHu[0].bPasHuChairID;
			////iScore[ChairID] += iBaseScore[iPaoID];
			////iScore[iPaoID] -= iBaseScore[iPaoID];
			//iScore[ChairID] += abs(m_WinLostArr[ChairID][iPaoID]);
			//iScore[iPaoID] -= abs(m_WinLostArr[ChairID][iPaoID]); 

			for (_uint8 c = NextChair(ChairID); c != ChairID; c = NextChair(c))
			{
				
				iScore[ChairID] += abs(m_WinLostArr[ChairID][c]);
				iScore[c] -= abs(m_WinLostArr[ChairID][c]);
			}

		}
		log_info("uid:%d chairid:%d, base score [%d,%d,%d,%d] ",
			GetPlayerUid(ChairID), ChairID, iScore[0], iScore[1], iScore[2], iScore[3]);
	}
}





void Table_guilin::Calc_Hu_Player_Base_Score(_uint8 ChairID, int iBaseScore[GAME_PLAYER])
{
	if (!m_bIsHu[ChairID])
		return;
	if (ChairID< 0 || ChairID >= m_GAME_PLAYER || m_bCurChairID < 0 || m_bCurChairID>= m_GAME_PLAYER )
		return;

	int hu_type = 0;
	if (m_stHuScore[ChairID].stHu.Size() && m_stHuScore[ChairID].stHu[0].bType == HU_ZIMO )
		hu_type = 1;
	if (m_stHuScore[ChairID].stHu.Size() == 0)
	{
		log_error("error !!!!!!!!!!!!!!!!!!!!!");
	}
	int zhigang_gangshanghu = false;
	int zhigang_gangshanghu_outid = 0;
	if (m_stHuScore[ChairID].stHu.Size() && m_stHuScore[ChairID].stHu[0].bType == HU_ZIMO && m_oFan[ChairID].Find(MJ_FAN_TYPE_GANGSHANGHUA))
	{
		//m_stGang[ChairID]; 
		int size = m_stGang[ChairID].Gang.Size();
		for (int i = 0; i <   size ; i++)
		{
			if (i == size - 1)
			{
				int type = m_stGang[ChairID].Gang[size - 1].iType; 
				if (type == TYPE_ZHIGANG)
				{
					zhigang_gangshanghu = type; 
					zhigang_gangshanghu_outid = m_stGang[ChairID].Gang[size - 1].ChairID; 
					log_debug("zhi gang hua  %d  " , zhigang_gangshanghu_outid );
				}
				
			}
		}
	}

	MJ_fan_buffer  OutFan;
	MJ_fan_buffer  BigOutFan; 
	MJ_fan_buffer  otherOutFan;
	m_ft2fn->FanFilter(m_oFan[ChairID], OutFan);   // 全部的番数 ，

	for (int i = 0; i < OutFan.Size(); i++) 
		log_debug("fan [%d] " , OutFan[i] );

	m_ft2fn->FanFilter_Guilin(m_oFan[ChairID], BigOutFan, otherOutFan);   // FanFilter_Guilin

	int xing_count = m_playerXingCount[ChairID];
	log_debug("xingcount[%d]   m_nPlayingTimes[%d]  hutype[%d]  m_bBanker[%d]", xing_count, m_nPlayingTimes , hu_type , m_bBanker);  

	if ( hu_type == HU_ZIMO)
	{		
		bool bank_zimo = (m_bBanker == ChairID && m_nPlayingTimes > 1);
		if (OutFan.Find(MJ_FAN_XI_LUAN ))  // 乱烂
		{
			for (int i = 0; i < m_GAME_PLAYER; i++)
			{
				if (i == ChairID) continue;
				if (bank_zimo)				
					m_WinLostArr[ChairID][i] += (5 + xing_count );  //庄自摸每家5分，1醒算2醒，1醒=每家1分										
				else
				{
					if (i == m_bBanker  && m_nPlayingTimes> 1 )					
						m_WinLostArr[ChairID][i] += (5 + xing_count );//闲自摸庄家5分，其他闲家3分，1醒算2醒，1醒=每家1分																
					else					
						m_WinLostArr[ChairID][i] += (3 + xing_count );																				
				} 
				//将对--------------------- -------------------------------- 1 个 
				//碰碰胡、清一色、字一色、全求人、七喜、七对 ，1豪华七对 ，2豪华七对，3豪华七对，  9 个 
				//大胡相互叠加时，每叠加一个，胡牌分数翻倍，醒的分数不翻倍

				//乱烂----------------。。 ----------------------------- 1个 

				//门清、海底捞月、海底炮  杠上花、杠上炮 抢杠胡  地胡    ------ 7 个  
				//和其他胡叠加时，胡牌分数翻倍，醒的分数翻倍，

				//天胡  ------------------------------ ----------------- 1 个 
				// 和其他胡叠加时，胡牌分数翻3倍，醒的分数翻3倍 

				for (int  type = 0;  type < OutFan.Size(); type++) 
				{
					if (OutFan[ type] == MJ_FAN_XI_LUAN || OutFan[type] == MJ_FAN_TYPE_JIANGDUI) 
						continue;				

					if (OutFan[type] == MJ_FAN_TYPE_TIANHU)
						m_WinLostArr[ChairID][i] *= 3;     
					else
						m_WinLostArr[ChairID][i] *= 2; 					
				}
				
			}
		}  //  乱牌，， 
		else  if (BigOutFan.Size()  > 0 )
		{			
			int base = 1; 					
			for (int c = 0; c  < m_GAME_PLAYER; c++)
			{
				if (c == ChairID)continue; 
				base = 1;
				int cur_xing = xing_count * 1 ;
				bool  tianhu = false;

				for (int type = 0; type < OutFan.Size();type++)
				{
					int  fan = OutFan[type]; 
					log_debug("fan[%d] uid[%d]" , fan ,  GetPlayerUid(ChairID));					
					if (fan == MJ_FAN_TYPE_JIANGDUI )
						continue;
					if (fan == MJ_FAN_TYPE_TIANHU)
					{
						tianhu = true;
						continue; 
					}						
					if (base < 15)					
						base = 15;					
					else					
						base *= 2;
					
					if(fan != MJ_FAN_TYPE_TIANHU && BigOutFan.Find( fan) == 0 )
					{
						cur_xing *= 2;
					}  
					if ( fan == MJ_FAN_HAO_HUA_QI_XIAO_DUI)
						base *= 2;
					else if (fan == MJ_FAN_HAO_HUA_DOUBLE_QI_XIAO_DUI)
						base *= 4;
					else  if (fan == MJ_FAN_HAO_HUA_THREE_QI_XIAO_DUI)
						base *= 8;							
				}
				if ( tianhu ) 
				{
					base *= 3;
					cur_xing *= 3;

				}
				log_debug("base [%d]  xing[%d]  tianhu[%d]" , base , cur_xing ,  tianhu   );
				m_WinLostArr[ChairID][c] = base + cur_xing ;				
				

			}
			
		}  // end  大胡， ，
		else        // 小胡 
		{			
			for (int i = 0; i < m_GAME_PLAYER; i++)
			{
				if (i == ChairID)  continue;
				if (bank_zimo == 1)				
					m_WinLostArr[ChairID][i] += (4 + xing_count);				
				else
				{
					if (i == m_bBanker && m_nPlayingTimes > 1 )
						m_WinLostArr[ChairID][i] += (4 + xing_count);
					else
						m_WinLostArr[ChairID][i] += (2 + xing_count);
				}
				for (int r  = 0;  r  < otherOutFan.Size(); r++)
				{
					if ( i  == ChairID)  continue; 
					if (otherOutFan[r] == MJ_FAN_TYPE_JIANGDUI )  continue;
					if (otherOutFan[r] == MJ_FAN_TYPE_TIANHU)
						m_WinLostArr[ChairID][i] *= 3;
					else
						m_WinLostArr[ChairID][i] *= 2;
				}

			}			
		} 
		int  win_Total = 0;
		bool has_hubao = false;
		for (int i = 0; i < m_GAME_PLAYER;i++)
		{
			if (ChairID == i) continue; 
			win_Total += abs( m_WinLostArr[ChairID][i]); 			
		}
		for (int i = 0; i < m_GAME_PLAYER; i++)
		{
			if (i== ChairID)  continue; 
			if (m_hubao[ChairID][i] && m_hubao[i][ChairID])
			{
				m_WinLostArr[ChairID][i] = win_Total * 4;
				has_hubao = true;
				log_debug("winlsot  %d %d %d %d ", ChairID, i, m_WinLostArr[ChairID][i], win_Total );
			}
			else if (m_hubao[ChairID][i] || m_hubao[i][ChairID])
			{
				m_WinLostArr[ChairID][i] = win_Total * 2;
				has_hubao = true;
				log_debug("winlsot  %d %d %d win_Total %d", ChairID, i, m_WinLostArr[ChairID][i], win_Total  );
			}
		}  
		for (int i = 0; i < m_GAME_PLAYER; i++)
		{
			if (i == ChairID) continue; 
			if (has_hubao &&  m_hubao[ChairID][i] == 0 && m_hubao[i][ChairID] == 0 && m_WinLostArr[ChairID][i])
			{
				m_WinLostArr[ChairID][i] = 0;
				log_debug("no hubao ..quxiao  !!");
			}
		} 

	}  // end zimo

	else  // 放炮 
	{
		
		bool hu_is_banker = (m_bBanker == ChairID && m_nPlayingTimes > 1);
		if (OutFan.Find(MJ_FAN_XI_LUAN))  // 乱烂
		{			
			if (hu_is_banker)
				m_WinLostArr[ChairID][m_bCurChairID] += (10 + xing_count * 3);
			else
				m_WinLostArr[ChairID][m_bCurChairID] += (8 + xing_count * 3  );

			for (int fan = 0; fan < OutFan.Size(); fan++)
			{
				if (OutFan[fan] == MJ_FAN_XI_LUAN || OutFan[fan] ==MJ_FAN_TYPE_JIANGDUI) 
					continue;

				if (OutFan[fan ] == MJ_FAN_TYPE_TIANHU)
					m_WinLostArr[ChairID][m_bCurChairID] *= 3;
				else
					m_WinLostArr[ChairID][m_bCurChairID] *= 2;

				log_debug("winlost[%d]", m_WinLostArr[ChairID][m_bCurChairID]);
			}			
		}
		else  if (BigOutFan.Size() > 0)
		{
			int base = 1;
			xing_count *= 3;

			for (int fan = 0; fan < OutFan.Size(); fan++)
			{
				int cur_fan = OutFan[fan];
				if (  cur_fan  == MJ_FAN_TYPE_JIANGDUI )  continue;
				if (base < 35)
				{
					base = 35;
				}
				else
					base *= 2; 
				if( BigOutFan.Find(OutFan[fan] ) == 0  )
				{
					xing_count *= 2;
					log_debug("add fan !!");
				}
				if ( cur_fan  == MJ_FAN_HAO_HUA_QI_XIAO_DUI)
					base *= 2;
				else if ( cur_fan  == MJ_FAN_HAO_HUA_DOUBLE_QI_XIAO_DUI)
					base *= 4;
				else  if ( cur_fan  == MJ_FAN_HAO_HUA_THREE_QI_XIAO_DUI)
					base *= 8;				
				 

			}
			m_WinLostArr[ChairID][m_bCurChairID] = base + xing_count;
			log_debug("winlost[%d]" , m_WinLostArr[ChairID][m_bCurChairID] ); 

		}
		else
		{
			if (hu_is_banker == 1)
			{
				m_WinLostArr[ChairID][m_bCurChairID] += (7 + xing_count * 3);
			}
			else
			{
				m_WinLostArr[ChairID][m_bCurChairID] += (5 + xing_count * 3);
			}
			for (int i = 0; i < otherOutFan.Size(); i++)
			{							
				if (otherOutFan[i] == MJ_FAN_TYPE_JIANGDUI) continue; 
				m_WinLostArr[ChairID][m_bCurChairID] *= 2;   
				log_debug("winlost[%d]", m_WinLostArr[ChairID][m_bCurChairID]);
			} 
		} 


		for (int i = 0; i < m_GAME_PLAYER; i++)
		{
			if (i == ChairID) continue;  // 胡牌自己。
			log_debug("enter i %d  hu %d %d " , i , m_hubao[ChairID][i]  , m_hubao[i][ChairID] );
			//双互包。。 自己放炮
			if (i == m_bCurChairID && (m_hubao[ChairID][m_bCurChairID] + m_hubao[m_bCurChairID][ChairID] >= 2))  // 自己放的泡
			{
				m_WinLostArr[ChairID][i] *= 4;
				log_debug("i[%d]  winlost[%d]" , i ,  m_WinLostArr[ChairID][i]);
			}
			//互包，自己放炮 
			else if (i == m_bCurChairID && (m_hubao[ChairID][i] || m_hubao[i][ChairID]))
			{
				m_WinLostArr[ChairID][i] *= 2;
				log_debug("i[%d]  winlost[%d]", i, m_WinLostArr[ChairID][i]);
			}
			////双互包 别人放炮
			else if (m_hubao[ChairID][i] + m_hubao[i][ChairID] >= 2)
			{
				m_WinLostArr[ChairID][i] = m_WinLostArr[ChairID][m_bCurChairID] * 2; 
				log_debug("i[%d]  winlost[%d]  m_bCurChairID[%d]  winlostt[%d]",
					i, m_WinLostArr[ChairID][i], m_bCurChairID , m_WinLostArr[ChairID][m_bCurChairID]  );

			} 
			//互包， 别人放炮
			else if (m_hubao[ChairID][i] || m_hubao[i][ChairID])
			{
				m_WinLostArr[ChairID][i] = m_WinLostArr[ChairID][m_bCurChairID];
				log_debug("i[%d]  winlost[%d] m_bCurChairID[%d]   winlost[%d]", 
					i, m_WinLostArr[ChairID][i], m_bCurChairID , m_WinLostArr[ChairID][m_bCurChairID]);
			}
		}
	}
	if(g_guilin_conf.yi_gang_dao_di && (m_oFan[ChairID].Find(MJ_FAN_TYPE_GANGSHANGHUA) || m_oFan[ChairID].Find(MJ_FAN_TYPE_GANGHOUPAO)))
	{
		int size = m_yiGangDaoDiVec.size(); 
		size = std::min(16, size); 
		log_debug("gangsize %d " , size );
		if (size > 1)
		{
			for (int i = 0; i < m_GAME_PLAYER; i++)
			{
				if (i == ChairID)continue;
				m_WinLostArr[ChairID][i] *=  (pow(2, size - 1)) ;
				log_debug("winlost [%d]", m_WinLostArr[ChairID][i]);
			}
		}
	}
	if (zhigang_gangshanghu && zhigang_gangshanghu_outid>=0 && zhigang_gangshanghu_outid< m_GAME_PLAYER )
	{
		log_debug("outid[%d]" , zhigang_gangshanghu_outid );
		for (int i = 0; i < m_GAME_PLAYER; i++)
		{
			if (ChairID ==i)  continue; 
			log_debug("winlost [%d]" , m_WinLostArr[ChairID][i] );			
			if (i==  zhigang_gangshanghu_outid)  continue; 
			m_WinLostArr[ChairID][zhigang_gangshanghu_outid] += m_WinLostArr[ChairID][i];
			m_WinLostArr[ChairID][i] = 0;
			log_debug("winlost [%d]", m_WinLostArr[ChairID][i]);

		}
	}

	MJ_user_buffer UserID;
	MultiHu(UserID);
	if (UserID.Size() == 3)
	{
		m_WinLostArr[ChairID][m_bCurChairID] *= 2;		
	}

}




void Table_guilin::TurnBanker()
{
	MJ_user_buffer UserID;
	int iCount = MultiHu(UserID);
	if (iCount == 0)
	{
		if (m_nPlayingTimes ==1)
			m_bBanker = m_bCurChairID;
		else
			m_bBanker = m_bBanker;
	}
		
	else if (iCount == 3)
		m_bBanker = m_bCurChairID;
	else if (iCount == 1)
		m_bBanker = UserID[0];
	else if (iCount == 2 )
	{
		for (int i = 0; i < m_GAME_PLAYER; i++)
		{
			int next_id = (i + m_bCurChairID) % m_GAME_PLAYER;
			if (m_bIsHu[next_id])
			{
				m_bBanker = next_id;
				return;
			}			
		}
	}
	//log_error("error !");
	log_debug("banker [%x]  bcurid [%x]" , m_bBanker , m_bCurChairID);
	//m_bBanker = 0;



}

bool Table_guilin::Is7Dui(_uint8 ChairID, HandCards& pHc, MJ_fan_buffer& oFan)
{
	if (m_GameLogic.is_qi_dui(&pHc))
	{   
		oFan.Add(MJ_FAN_TYPE_QIDUI);		
		int ret = m_GameLogic.hao_hua_qi_dui(&pHc);
		if (ret == 1)
			oFan.Add( MJ_FAN_HAO_HUA_QI_XIAO_DUI);
		else if (ret == 2)
			oFan.Add(MJ_FAN_HAO_HUA_DOUBLE_QI_XIAO_DUI);
		else if (ret == 3)
			oFan.Add(MJ_FAN_HAO_HUA_THREE_QI_XIAO_DUI);

		if (ret)
			oFan.Remove(MJ_FAN_TYPE_QIDUI);
		log_debug("into table  qidui ret[%d]" ,ret);
		
		return true;
		
	}
	return  false;

}

int  get_map_vec(map<_uint8, _uint8> &AllMap)
{
	int all = 0;
	map<_uint8, _uint8>::iterator  it = AllMap.begin();
	for (; it != AllMap.end();++it)
	{
		all += it->second;
	}
	return all;
}

int Table_guilin::XingTongSeCount(  _uint8 bGetcard, _uint8 bPreCard, _uint8 bNextCard   , cur_lun_xing_info   &pInfo )
{
	map<_uint8, _uint8> tong_map;
	bool  IsMoreThan3 = 0 ;
	log_debug("get [%x] pre[%x]  nex[%x]" ,  bPreCard  , bGetcard  , bNextCard );
	xing_info   xinfo;
	for (int hu_id = 0; hu_id < m_GAME_PLAYER; hu_id++)
	{
		if (!m_bIsHu[hu_id])  continue;
		if (m_zhuaXingOverArr[hu_id]) continue; 		
		memset(&xinfo, 0, sizeof(xinfo)); 		
		HandCards *phc = &m_HandCard[hu_id];
		bool is_Add = false;
		
		if (m_stHuScore[hu_id].stHu.Size() && m_stHuScore[hu_id].stHu[0].bType != HU_ZIMO && phc->ChangeableCardsLen <= 13)
		{
			is_Add = true;
			m_GameLogic.AddCard(phc, m_bCurCard);   
		}
			
		log_debug("huid[%d] [%d]" , hu_id ,  phc->ChangeableCardsLen );

		if (m_oFan[hu_id].Find(MJ_FAN_QI_XI_GUI_LIN) ) 
		{
			if (bGetcard > 0x30  )
			{
				m_playerXingCount[hu_id] += 6; 
				xinfo.sid = hu_id;
				xinfo.xing = 6;
				xinfo.all_xing = m_playerXingCount[hu_id];
				pInfo.arr.push_back(xinfo);
				if (is_Add)
					m_GameLogic.ChuPai(phc, m_bCurCard);				
				IsMoreThan3 = 1;
				continue; 
			} 

			else
			{
				int tongse = 0;				
				for (int i = 0; i < phc->ChangeableCardsLen; i++)
				{
					_uint8  ct = phc->ChangeableCards[i]; 
					if ( g_guilin_conf.shangzhongxia_xing && (ct == bGetcard || ct == bPreCard || ct == bNextCard))
					{
						tongse++;
					}
					else  if (g_guilin_conf.shangxia_xing && (ct == bPreCard || ct == bNextCard))
						tongse++; 

				}
				m_playerXingCount[hu_id] += (2 * tongse);  // tongse;
				xinfo.sid = hu_id;
				xinfo.xing = tongse * 2;
				xinfo.all_xing = m_playerXingCount[hu_id];
				pInfo.arr.push_back(xinfo);
				if (is_Add)
					m_GameLogic.ChuPai(phc, m_bCurCard);
				m_zhuaXingOverArr[hu_id] = 1;
				continue;   
			}
			
		}
		if (m_oFan[hu_id].Find(MJ_FAN_XI_LUAN))
		{
			if (bGetcard < 0x30)
			{
				int tongse = 0;				
				for (int i = 0; i < phc->ChangeableCardsLen; i++)
				{
					_uint8  ct = phc->ChangeableCards[i];
					if (g_guilin_conf.shangzhongxia_xing && (ct == bGetcard || ct == bPreCard || ct == bNextCard))
						tongse++;
					else if (g_guilin_conf.shangxia_xing && (ct == bPreCard || ct == bNextCard))
						tongse++;

				}
				m_playerXingCount[hu_id] += (2 * tongse);  // tongse;
				xinfo.sid = hu_id;
				xinfo.xing = tongse * 2;
				xinfo.all_xing = m_playerXingCount[hu_id];
				pInfo.arr.push_back(xinfo);
				if (is_Add)
					m_GameLogic.ChuPai(phc, m_bCurCard);
				m_zhuaXingOverArr[hu_id] = 1;
				continue;  
			}
			else
			{
				int tongse = 0;				
				for (int i = 0; i < phc->ChangeableCardsLen; i++)
				{
					if (bGetcard > 0x40)
					{
						if (phc->ChangeableCards[i] > 0x40)
						{
							tongse++;
							if (tongse >= 3)
								IsMoreThan3 = 1;
						}
					}
					else
					{ 
						if (phc->ChangeableCards[i] > 0x30  && phc->ChangeableCards[i] < 0x40  )
						{
							int ct = phc->ChangeableCards[i];
							if (g_guilin_conf.shangzhongxia_xing && (ct == bGetcard || ct == bPreCard || ct == bNextCard))
								tongse++;
							if (g_guilin_conf.shangxia_xing && (ct == bPreCard || ct == bNextCard))
								tongse++;

							if (tongse >= 3)
								IsMoreThan3 = 1;
						}
					}
					
				}
				m_playerXingCount[hu_id] += (2 * tongse);  // tongse;
				xinfo.sid = hu_id; 
				xinfo.xing = tongse * 2;
				xinfo.all_xing = m_playerXingCount[hu_id];
				pInfo.arr.push_back(xinfo);
				if (is_Add)
					m_GameLogic.ChuPai(phc, m_bCurCard);
				
				if (IsMoreThan3 == 0  )
					m_zhuaXingOverArr[hu_id] = 1;
				continue; 
			}
		}
		else
		{
			tong_map.clear();
			for (int i = 0; i < phc->ChangeableCardsLen; i++)
			{
				_uint8  ct = phc->ChangeableCards[i];
				if (((ct == bGetcard || ct == bPreCard || ct == bNextCard) && g_guilin_conf.shangzhongxia_xing)
					|| ((ct == bPreCard || ct == bNextCard) && g_guilin_conf.shangxia_xing)

					)
				{
					tong_map[ct]++;
					log_debug("add ct[%x]", ct);
					if (tong_map[ct] >= 3)
					{
						IsMoreThan3 = 1;
						log_debug("card %x", ct);
					}

				}
			} // end changge  

			for (int i = 0; i < phc->FixedCardsLen; i++)
			{
				_uint8 ct = phc->FixedCards[i].CardData;
				if (phc->FixedCards[i].state  & TYPE_PENG || phc->FixedCards[i].state  & TYPE_GANG)
				{
					int gang = phc->FixedCards[i].state  & TYPE_GANG;

					if (g_guilin_conf.shangzhongxia_xing && (ct == bGetcard || ct == bPreCard || ct == bNextCard))
					{
						tong_map[ct] += ((gang > 0) ? 4 : 3);						
					}
					else  if (g_guilin_conf.shangxia_xing && (ct == bPreCard || ct == bNextCard))
					{
						tong_map[ct] += ((gang > 0) ? 4 : 3);						
					}
				}
				else
				{
					_uint8  ct = phc->FixedCards[i].c[0];
					_uint8  ct1 = phc->FixedCards[i].c[1];
					_uint8  ct2 = phc->FixedCards[i].c[2];
					if ((g_guilin_conf.shangzhongxia_xing && (ct == bPreCard || ct == bGetcard || ct == bNextCard))
						|| (g_guilin_conf.shangxia_xing && (ct == bPreCard || ct == bNextCard))
						)
					{
						tong_map[ct]++;
						log_debug("add ct[%x]", ct);
						if (tong_map[ct] >= 3)
						{
							IsMoreThan3 = 1;
							log_debug("card %x", ct);
						}

					}
					if ((g_guilin_conf.shangzhongxia_xing && (ct1 == bPreCard || ct1 == bGetcard || ct1 == bNextCard))
						|| (g_guilin_conf.shangxia_xing && (ct1 == bPreCard || ct1 == bNextCard))
						)
					{
						tong_map[ct1]++;
						log_debug("add ct[%x]", ct);
						if (tong_map[ct1] >= 3)
						{
							IsMoreThan3 = 1;
							log_debug("card %x", ct);
						}

					}
					if (( g_guilin_conf.shangzhongxia_xing && (ct2 == bPreCard || ct2 == bGetcard || ct2 == bNextCard))
						|| (g_guilin_conf.shangxia_xing && (ct2 == bPreCard || ct2 == bNextCard)))
					{
						tong_map[ct2]++;
						log_debug("add ct[%x]", ct);
						if (tong_map[ct2] >= 3)
						{
							IsMoreThan3 = 1;
							log_debug("card %x", ct);
						}

					}

				}

			}  // end fix  

			int all = get_map_vec(tong_map);
			m_playerXingCount[hu_id] += all;
			xinfo.sid = hu_id;
			xinfo.xing = all;
			xinfo.all_xing = m_playerXingCount[hu_id];
			pInfo.arr.push_back(xinfo);
			if (is_Add)
				m_GameLogic.ChuPai(phc, m_bCurCard);


			for (map<_uint8, _uint8>::iterator it = tong_map.begin(); it != tong_map.end(); ++it)
			{
				if (it->second >= 3)
					IsMoreThan3 = 1;
			}

			if (IsMoreThan3 == 0)
				m_zhuaXingOverArr[hu_id] = 1;

			log_debug("xing [%lu]  morethan[%d] huid[%d] huuid[%d] all_xing[%d]",
				all, IsMoreThan3, hu_id, GetPlayerUid(hu_id), m_playerXingCount[hu_id]);
		}		
	}
	

	return  IsMoreThan3;

}

void Table_guilin::GetXingLun(int  &lun , _uint8  bGetCard,  vector<cur_lun_xing_info> & xing_list   )
{
	
	_uint8  pre_card = 0, next_card = 0;
	GetXing(bGetCard, pre_card, next_card);	
	lun++;
	cur_lun_xing_info  info;  
	info.lun = lun;
	info.getcard = bGetCard;
	log_debug("getcard pre next [%x]  [%x][%x]  lun[%d]", pre_card ,  bGetCard, next_card, lun);
	_uint8  grp_card = bGetCard;

	while ( XingTongSeCount(grp_card, pre_card, next_card, info) > 0)
	{
		xing_list.push_back(info);
		if (!m_CardPool.IsEmpty()) 
		{
			memset(&info, 0, sizeof(info));
			lun++;
			info.lun = lun;			
			grp_card = m_CardPool.GetOneCard();
			m_AllLunXingCard.push_back(grp_card);
			info.getcard = grp_card ;			
			log_debug("getcard pre next [%x]  [%x][%x]  lun[%d]", pre_card,  grp_card , next_card, lun);

			GetXing(grp_card, pre_card, next_card);
			
		}
		else
		{
			memset( &info, 0, sizeof(info));
			break;
		}
			
		
	}
	if (info.lun  > 0  )
		xing_list.push_back(info);
	
}

void Table_guilin::GameEnd()
{
	//if (m_CardPool.get_cur_pos() == 122 ) 
	//  1  不是海底胡牌 ， 继续摸
	MJ_user_buffer UserID;
	MultiHu(UserID);
	if (UserID.Size()  )
	{
		int lun = 0;
		vector<cur_lun_xing_info>   xlist;
		log_debug("pos [%d]  isempty [%d]", m_CardPool.get_cur_pos(), m_CardPool.IsEmpty());

		if (m_CardPool.get_cur_pos() == 122 && m_CardPool.IsEmpty())
		{
			log_debug("into ");
			m_AllLunXingCard.push_back(m_bCurCard);
			GetXingLun(lun, m_bCurCard, xlist);
		}
		else
		{
			_uint8  bgetcard = m_CardPool.GetOneCard();
			m_AllLunXingCard.push_back(bgetcard);
			GetXingLun(lun, bgetcard, xlist);
		}
		proto::game::GuilinZhuaXingAllInfo  Guilin_XingAck;
		for (size_t i = 0; i < xlist.size(); i++)
		{
			proto::game::GuilinZhuaXing  *pXing = Guilin_XingAck.add_info_list();   // 抓醒没一轮的 
			cur_lun_xing_info   &pCur = xlist[i];
			pXing->set_lun(pCur.lun);
			log_debug("guilin_lun [%d] ", pCur.lun);

			pXing->set_get_card(pCur.getcard);                          // 醒牌。

			for (size_t j = 0; j < pCur.arr.size(); j++)
			{
				proto::game::XingInfo *pX = pXing->add_xing_list();     // 胡牌的人的醒  
				pX->set_chairid(pCur.arr[j].sid);
				pX->set_xing(pCur.arr[j].xing);
				pX->set_all_xing(pCur.arr[j].all_xing);

				log_debug("sid [%d] xing[%d] all_xing[%d] ", pCur.arr[j].sid, pCur.arr[j].xing, pCur.arr[j].all_xing);
			}
		}

		broadcast(SER_CMD_GUILIN_XING_, Guilin_XingAck);
	}

	Table::GameEnd();
}



void  Table_guilin::Guilin_OpAck(int  op_type, int  hufen[] )
{
	proto::game::GuilinOpAck  op_ack;
	op_ack.set_op_type(op_type);
	log_debug("op type [%x]" , op_type );   
	for (int i = 0; i < m_GAME_PLAYER; i++)
	{
		proto::game::PosAndFen *pInfo = op_ack.add_op_fen_list();
		pInfo->set_chairid(i);
		pInfo->set_fen(hufen[i]);
		log_debug("sid [%d]  uid[%d] fen[%d]" , i , GetPlayerUid(i) , hufen[i] );
	}
	broadcast(SER_CMD_GUILIN_OP_GANG_ZHUI, op_ack);
	Update_Score(hufen);

}



void Table_guilin::UserOperateGang(_uint8 ChairID, int iOpType, _uint8 bCard)
{
	int b_peng_out_id = 0;
	if (iOpType  &  TYPE_WANGANG)
	{
		const  HandCards *phc = &m_HandCard[ChairID];  
		for (int i = 0; i < phc->FixedCardsLen;i++)
		{
			if (bCard == phc->FixedCards[i].CardData)
				b_peng_out_id = phc->FixedCards[i].chairID;
		}
		if (b_peng_out_id == -1)
		{
			log_error("error !!");
			b_peng_out_id = 0;
		}
		
	}
	

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

	m_GangMap[ChairID]++;
	int base = 1;
	// if(g_guilin_conf.yi_gang_dao_di)
	m_yiGangDaoDiVec.push_back(ChairID);
	log_debug("size  %lu " , m_yiGangDaoDiVec.size() ); 

	//if (g_guilin_conf.yi_gang_dao_di &&  m_GangMap[ChairID] > 1)
	if (g_guilin_conf.yi_gang_dao_di  && m_yiGangDaoDiVec.size() > 1)
	{		
		//for (int i = 1; i < m_GangMap[ChairID]; i++)
			//base *= 2; 
		int size = m_yiGangDaoDiVec.size();
		for (int i = 1; i < size; i++)
			base *= 2;  
	}
	

	if (Tian_Gang(ChairID) &&   m_GangMap[ChairID] == 1 &&  m_yiGangDaoDiVec.size() ==1  )
	{		
		int  fen_arr[GAME_PLAYER] = { 0 };
		for (int i = 0; i < m_GAME_PLAYER; i++) 
		{
			if (i == ChairID)
				fen_arr[i] = 18;
			else
				fen_arr[i] = -6 ;			
		}
		log_debug("tiangang !!!");
		Guilin_OpAck(  TIAN_GANG , fen_arr);
	}
	else if (di_gang(ChairID , iOpType )  &&  m_GangMap[ChairID] ==1) // && m_yiGangDaoDiVec.size() ==1   )
	{
		log_debug("di gang !!!");
		int  fen_arr[GAME_PLAYER] = { 0 };
		if (base == 1)
		{
			fen_arr[ChairID] = 9;
			fen_arr[m_bOutCardChairID] = -9;
		}
		else
		{
			fen_arr[ChairID] = 9 * base;
			fen_arr[m_bOutCardChairID] = -9 * base;

		}	
		Guilin_OpAck(  DI_GANG , fen_arr);
	}  
	else   // 普通杠， 
	{
		log_debug("op [%x] base [%d]" , iOpType , base  );
		int  fen_arr[GAME_PLAYER] = { 0 }; 
		if (iOpType &  TYPE_ZHIGANG )
		{
			fen_arr[ChairID] = 3 * base; 
			fen_arr[m_bCurChairID] = -3 * base; 
		} 
		else if (iOpType  &  TYPE_ANGANG   ) 
		{
			for (int i = 0; i < m_GAME_PLAYER; i++)
			{
				if (i != ChairID)
					fen_arr[i] = -2 * base;
				else
					fen_arr[i] = 6 * base;
			}
		}		
		else if (TYPE_WANGANG  & iOpType)
		{
			fen_arr[ChairID] = 3 * base;
			fen_arr[b_peng_out_id] = -3 * base;
		}
		Guilin_OpAck(  PU_TONG_GANG  , fen_arr);
	}
	DealHuBao(ChairID);
	
	

}

void Table_guilin::Proto_Game_OperateGang(_uint8 ChairID, _uint8 bCard, int iOpType, int iGangScore[GAME_PLAYER], proto::game::AckBcOpResult& stOpresult)
{
	Table::Proto_Game_OperateGang(ChairID, bCard, iOpType, iGangScore, stOpresult);
	if (Tian_Gang(ChairID))
	{
		m_TianGangList[bCard] = ChairID;
	}
	if ( di_gang(ChairID , iOpType ))
	{
		m_DiGangList[bCard] = ChairID;
	}

} 

void Table_guilin::Proto_GameScene(_uint8 ChairID, Player* player, proto::game::AckGameScene& stGameScene)
{
	Table_changsha::Proto_GameScene(ChairID, player, stGameScene);
	proto::game::GuilinReconnectInfo  *p = stGameScene.mutable_re_guilin_info();
	if ( m_CanotOutAfterChiVec[ChairID].size() )
	{
		vector<_uint8> &arr = m_CanotOutAfterChiVec[ChairID];
		for(int  i = 0; i <  (int) arr.size() ; i++)
		{
			p->add_card( arr[i] ); 
			log_debug("arr [%d]" , arr[i]);
		}
	} 
	for (int i = 0; i < m_GAME_PLAYER; i++)
	{	
		for (int j = 0; j < m_GAME_PLAYER; j++)  
		{  
			if (i== j ) continue;  
			if (m_hubao[i][j] || m_hubao[j][i] )
			{
				proto::game::ReconnectHubao *pInfo = p->add_bao_list(); 
				pInfo->set_chairid(i);
				pInfo->set_bao(1);  
				break; 
			}			
		}
	}

}


void Table_guilin::Proto_GameEndInfo_GangInfo(_uint8 ChairID, proto::game::AckGameEnd& stGameend)
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



void Table_guilin::Proto_Game_OpNotify(_uint8 ChairID, _uint8 CurChairID, _uint8 bCard, proto::game::AckOpNotify& stNotify)
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


bool Table_guilin::CheckUserOutCard(_uint8 ChairID, _uint8 cbCard)
{
	
	if (m_curGangPlayerSid != INVALID_CHAIRID)
	{
		log_debug("curGangSid[%d] ", m_curGangPlayerSid);
		return true;
	}
	if (!Table::CheckUserOutCard(ChairID, cbCard))
		return false;

	
	
	return true;
}



void Table_guilin::Proto_Game_UserGetCard(_uint8 ChairID, _uint8 MoChairID, _uint8 bCard, proto::game::AckUserGetCard& stUsergetcard)
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

void Table_guilin::Proto_GameEndInfo(const MJ_user_buffer& UserID, proto::game::AckGameEnd& stGameend)
{

	
	Table::Proto_GameEndInfo(UserID, stGameend);
	
}


//天杠：庄家（第一把时无庄，为开房玩家）起手直接暗杠 

int Table_guilin::Tian_Gang(_uint8  chairid )
{
	if (chairid == m_bBanker && PlayerAllOutCard[m_bBanker].Size() == 0) // && m_nPlayingTimes > 1)
	{
		return 1;
	}
	for (int i = 0; i < m_GAME_PLAYER;i++)
	{
		if (PlayerAllOutCard[i].Size())
			return 0;
	}
	return 0; 
}

//////地杠：庄家（第一把时无庄，为开房玩家）打出的第一张点杠给闲家

int Table_guilin::di_gang(_uint8  chairid , int op  )
{
	for (int i = 0; i < m_GAME_PLAYER; i++)
	{
		if (i== chairid || i== m_bBanker )
			continue; 
		if (PlayerAllOutCard[i].Size() > 0)
			return false; 
	}
	if (m_bBanker< 0 || m_bBanker >=  m_GAME_PLAYER)
		return 0;

	if (PlayerAllOutCard[m_bBanker].Size() == 1 && PlayerAllOutCard[chairid].Size() == 0 && m_bBanker == m_bCurChairID 
		 && op == TYPE_ZHIGANG 
		)
	{
		log_debug("di gang !!"); 
		return 1; 
	}
	return  0;
}

int Table_guilin::xi_lan_big_2(int (*card_arr)[10] , int row_all   )
{
	
	for (int row = 0; row < row_all ; row++)
	{
		int    *pcurRow  = card_arr[row];
		int pos = -1; 
		for (int i = 1; i < 10; i++)  
		{  
			//log_debug("row %d   i[%d] " , pcurRow[i] , i  ); 
			if (pcurRow[i] != 0 && pos == -1)
				pos = i;			
			else if (pcurRow[i] != 0 && pos> 0)
			{
				if (i - pos <= 2)
					return 0 ;  //  乱牌bug 				
				pos = i;
			}
		}
	}
	return 1;
	
}

////风牌的种类

int Table_guilin::feng_type_count(const HandCards *phc)
{
	map<int, int> card_map;
	for (int i = 0; i< phc->ChangeableCardsLen;i++)
	{ 
		if (phc->ChangeableCards[i] < 0x30)  continue; 
		card_map[phc->ChangeableCards[i]] = 1;
	}
	return  card_map.size(); 

}

int  Table_guilin::check_guilin_listen_xilun(_uint8  ChairID)
{
	if (ChairID < 0 || ChairID >= m_GAME_PLAYER)
		return 0;

	//log_debug("charid %d" ,ChairID );

	const  HandCards *phc = &m_HandCard[ChairID]; 
	return   xi_lan(phc , ChairID ); 
	

}

int Table_guilin::xi_lan(const  HandCards   *phc,  int  ChairID  )
{
	
	//CatHandCard(*phc, ChairID);
	//log_debug("xiluan into this   len %d   sid[%d]" , phc->ChangeableCardsLen  , ChairID );

	if (phc->FixedCardsLen > 0)
		return 0 ;

	map<int, int> feng_map;
	int   color[5][10];	  //  zhee
	memset(color, 0, sizeof(color)); 

	int suit = 0, face = 0;
	for (int i = 0;  i<phc->ChangeableCardsLen; i++)
	{
		suit = get_suits(phc->ChangeableCards[i]);
		face = get_face(phc->ChangeableCards[i]);
		if (suit< 0 || suit> 4 || face< 0 || face>9)
			return 0;

		if (phc->ChangeableCards[i] > 0x30)
			feng_map[phc->ChangeableCards[i]] = 1;

		color[suit][face]++;
		color[suit][0]++; 
		//log_debug("suit[%d] face [%d]  %x" , suit , face ,  phc->ChangeableCards[i]  );

		if (color[suit][face] >= 2)
		{
			//log_debug("all %d %d   %x" ,  suit , face , color[suit][face] );
			return 0;
		}
			
		if (color[suit][0] > 3 && suit< 3)
		{
			//log_error(" suit[%d] all[%d]  " , suit   , color[suit][0]);
			return 0;
		}		
	}

	if ((color[3][0] + color[4][0])  > 7)
	{
		//log_error("no this %d %d   " ,  color[3][0] ,  color[4][0]);
		return 0;
	}		


	//log_debug("map %lu" , feng_map.size() );

	if (feng_map.size() < 5)
		return 0 ;
	if (feng_map.size() == 5 )
	{
		if (color[0][0] != 3 || color[1][0] != 3 || color[2][0] != 3)
			return  0;
		if (xi_lan_big_2(color, 3 ) == 0)
			return  0;
	}
	if (feng_map.size() >= 6)
	{
		if (color[0][0] > 3 || color[1][0] > 3 || color[2][0] > 3)
			return 0;
		if (xi_lan_big_2(color, 3) == 0)
			return 0; 
	}
	return 1;

}
int  Table_guilin::All_Is_Feng(const  HandCards *phc)
{
	for (int i = 0; i < phc->ChangeableCardsLen; i++)
	{
		if (phc->ChangeableCards[i]   < 0x30)  
			return 0;
	}

	for (int i = 0; i < phc->FixedCardsLen; i++)
	{
		if (phc->FixedCards[i].state  & TYPE_GANG || phc->FixedCards[i].state  & TYPE_PENG)
		{
			if (phc->FixedCards[i].CardData < 0x30)
				return 0;
		}
		if (phc->FixedCards[i].state  & TYPE_CHI)
		{		
		   return 0;
		} 		
	}
	return  1;

}

int Table_guilin::GetXing(_uint8  c ,  _uint8  & pre , _uint8   &next  )
{
	int  suit = get_suits(c);
	int  face = get_face(c);
	if (suit <= 2)
	{
		if (face == 9)
			next = make_card(suit, 1);
		else
			next = make_card(suit ,  face + 1);

		if (face == 1)
			pre = make_card(suit, 9);
		else
			pre = make_card(suit  ,face - 1);


	}
	else if (suit ==3)
	{ 
		if (face == 4)
			next = make_card(suit, 1);
		else
			next = make_card(suit,  face + 1);

		if (face == 1)
			pre = make_card(suit, 4 );
		else
			pre = make_card(suit,  face - 1);
	}
	else
	{
		if (face == 3)
			next = make_card(suit, 1);
		else
			next = make_card(suit , face + 1);

		if (face == 1)
			pre = make_card(suit, 3 );
		else
			pre = make_card(suit,  face - 1);
	}
	return 0;
}


void Table_guilin::DealGuiLinJieSuan(  proto::game::AckGameEnd& stGameend)
{
	log_debug("into this ");	
	proto::game::GuiLinJieSuan   *pinfo = stGameend.mutable_guilininfo();  // mutable_usercard();	
	MJ_user_buffer UserID;
	MultiHu(UserID);  
	bool zimo = false;
	log_debug("hucount %d" , UserID.Size() ); 
	
	if (UserID.Size())
	{
		for (int i = 0; i < m_GAME_PLAYER;i++)
		{
			if(!m_bIsHu[i]) continue;
			proto::game::GuilinfanInfo *pfan = pinfo->add_fan_list();
			pfan->set_chairid(i);
			MJ_fan_buffer  OutFan;
			MJ_fan_buffer  BigHuFan;
			MJ_fan_buffer  SmallHuFan;
			
			m_ft2fn->FanFilter_Guilin(m_oFan[i], BigHuFan, SmallHuFan);
			for (int type = 0; type < m_oFan[i].Size(); type++)
			{
				if (BigHuFan.Size() && type == MJ_FAN_TYPE_JIANGDUI )
					continue; 
				log_debug("fan[%d]   type[%d] "  , m_oFan[i][type]  , type ); 

				proto::game::GuilinFan  *p = pfan->add_fan_list(); 
				p->set_fan( m_oFan[i][type] );				
			}		
			if (m_stHuScore[i].stHu.Size() && m_stHuScore[i].stHu[0].bType == HU_ZIMO)
				zimo = true;  
			
		}
		for (int i = 0; i < m_GAME_PLAYER; i++)
		{
			proto::game::GuiLinHuPos  *pPosInfo = pinfo->add_hu_info_list();
			pPosInfo->set_chairid(i);  
			if (zimo)
			{				
				if (m_bIsHu[i])								
					pPosInfo->set_hu_type(HU_ZIMO);				
				else				
					pPosInfo->set_hu_type(HU_BEIZIMO);								
			}
			else
			{ 
				if (m_bIsHu[i])
					pPosInfo->set_hu_type(HU_PAOHU );
				else  if (i== m_bCurChairID )
					pPosInfo->set_hu_type(HU_DIANPAO );
				else
					pPosInfo->set_hu_type(HU_INVALID );
			}						
		}
		for (size_t i = 0; i < m_AllLunXingCard.size(); i++)
		{
			pinfo->add_xing_card_list(m_AllLunXingCard[i]); 
			log_debug(" xing [%x]", m_AllLunXingCard[i]);
		}

		for (int i = 0; i < m_GAME_PLAYER; i++)
		{		
			log_debug("xing [%d]" , m_playerXingCount[i] ) ;
			if (m_playerXingCount[i])
			{
				proto::game::xing_count *px = pinfo->add_xing_list();    
				px->set_chairid(i);
				px->set_xing(m_playerXingCount[i]);

			} 		
			int  type = 0;
			for (int pos = 0; pos < m_GAME_PLAYER; pos++)
			{
				if (i == pos )  continue; 
									
				if (m_hubao[i][pos] && m_hubao[pos][i])
					type |= 0x10;
				else if (m_hubao[i][pos] || m_hubao[pos][i])
					type |= 0x1;
											
			}
			if (type == 0) 
				continue; 
			proto::game::hu_bao *pbao  = pinfo->add_hu_bao_list();
			log_debug("hu_type [%x]" , type );
			if (type ==  0x11 )
			{ 
				pbao->set_chairid(i); 
				pbao->add_bao_list(2);
				pbao->add_bao_list(1);
			}
			if (type ==  0x10 )
			{
				pbao->set_chairid(i);
				pbao->add_bao_list(2);
			}
			if (type ==  0x1  ) 
			{
				pbao->set_chairid(i);
				pbao->add_bao_list(1);
			}
			
		}
		
	}	
}



int Table_guilin::deal_chi_op( _uint8  chairid  , int op , _uint8  op_card )
{
	if (chairid< 0 || chairid  >= m_GAME_PLAYER)
		return  0;

	vector<_uint8>  vec;
	int suit = get_suits(op_card);  
	int face = get_face(op_card);  

	m_CanotOutAfterChiVec[chairid].clear();
	if (op  & TYPE_LEFT_CHI)
	{
		vec.push_back(op_card);
		if (  face  + 3 < 10)  
			vec.push_back(  make_card(suit , face+ 3) );
	}
	else if (op  &  TYPE_CENTER_CHI)
		vec.push_back(op_card);
	else
	{
		vec.push_back(op_card);
		if ( face - 3 > 0)
			vec.push_back(  make_card(suit , face-3 ) );
	}
	m_CanotOutAfterChiVec[chairid] = vec;

	return  0;
}


