#include "table_changsha.h"
#include "../proto/proto.h"
#include "../zjh.h"
#include "../common/log.h"
#include "../game.h"
#include "../libnormalmahjong/mj_common2.h"
#include "../libnormalmahjong/game_logic.h"
#include "../table_helper.h"
#include "le_majiang.h"


#define  KING_WANG_DA_HU     7  
#define  NO_KING_WANG_DA_HU  8  

using namespace proto::game::hunanmj;

extern ZJH zjh;




Table_changsha::Table_changsha()
{			
	memset(m_buzhang, 0, sizeof(m_buzhang));
	memset(m_RecvBaipaiMap, 0, sizeof(m_RecvBaipaiMap));  //默认为0   摆牌选择 
	memset(m_nRecvBaoTingPlayerMap, 0, sizeof(m_nRecvBaoTingPlayerMap)) ;
	memset(m_nCanBaoTingArr, 0, sizeof(m_nCanBaoTingArr));
	memset(m_stUserOutCardAfterBaoTing, 0, sizeof(m_stUserOutCardAfterBaoTing));
	memset(m_cannothu, 0, sizeof(m_cannothu));
	memset(m_fengdong, 0, sizeof(m_fengdong));
	memset(m_speGang, 0, sizeof(m_speGang)); 
	memset(m_ningxiag_guozhang, 0, sizeof(m_ningxiag_guozhang));

	m_DaNiaoID = INVALID_CHAIRID;

	m_HaidiUser.Clear();
	m_HaidiPlayer = INVALID_CHAIRID;

	m_StartBankOutCard_Stamp = 0.1;
	m_StartBankOutCardTimer.data = this;
	  // 这个定时器。并没有什么卵用。。  可以把定时器。改为1s  或者是直接取消 
	ev_timer_init(&m_StartBankOutCardTimer, Table_changsha::Qishou_Timer_cb, m_StartBankOutCard_Stamp, m_StartBankOutCard_Stamp);

	 // 起手摆牌的选择 
	m_qishou_timer_baipai_sel_stamp = 1200 ;
	m_qishou_timer_baiPai_sel.data = this;
	ev_timer_init(&m_qishou_timer_baiPai_sel, Table_changsha::qishou_baipai_sel_timer_cb,
		m_qishou_timer_baipai_sel_stamp, m_qishou_timer_baipai_sel_stamp);

	m_baoJiao_Timer_stamp = 1000;
	m_baoJiao_Timer.data = this;
	ev_timer_init(&m_baoJiao_Timer, Table_changsha::baojiao_sel_Timer_cb,
		m_baoJiao_Timer_stamp, m_baoJiao_Timer_stamp);



	m_qishouhu_Timer_stamp_0 = 0.1 ;
	m_qishouhu_Timer_0.data = this;
	ev_timer_init(&m_qishouhu_Timer_0, Table_changsha::Qishou_Timer_cb_0, m_qishouhu_Timer_stamp_0, m_qishouhu_Timer_stamp_0);
	
	memset(m_qishou_score, 0, sizeof(m_qishou_score));
	

	m_bMultiOpUserNum = 0;
		
	
	m_curGangPlayerSid = INVALID_CHAIRID;
	for (int i = 0; i < m_GAME_PLAYER; i++)
	{
		m_MultiFan[i].Clear();
	}
	memset(m_MultiHufanNums, 0, sizeof(m_MultiHufanNums));

	m_gang_dashaizi_Timer_stamp = 2;
	m_gang_dashaizi_Timer.data = this;
	m_Gang_buzhang_res = 0;
	ev_timer_init(&m_gang_dashaizi_Timer, Table_changsha::Gang_Dashaizi_Timer_cb,
		m_gang_dashaizi_Timer_stamp, m_gang_dashaizi_Timer_stamp);

	m_HuWeiDa = false;
	m_HuWeiDaCard = INVALID_MJ_CARD;
	m_HuWeiDaOp = TYPE_NULL;

	m_qishou_state = false;
	m_canBaiPaiPlayerCount = 0;
	m_nCanBaoTingCount = 0;
	m_chunwanga = 0;
	m_chunwangb = 0;
	m_zhengwang = 0;

	m_conf.m_bCalcAllNiao = true;
	m_last_card_pos = 0;
	m_ft2fn = &g_changsha_ft2fn;

	m_conf_changsha.hu_wei_da = false;
	for (int i = 0; i < GAME_PLAYER; i++)   // 这个四喜。。没一把都要初始化。。要不。就挂了。  超过5个就挂了。
		m_SiXi[i].Clear();

	
}
Table_changsha::~Table_changsha()
{
	log_debug("delete changsha   !!");
	ev_timer_stop(zjh.loop, &m_StartBankOutCardTimer);
	ev_timer_stop(zjh.loop, &m_qishouhu_Timer_0);
	ev_timer_stop(zjh.loop, &m_gang_dashaizi_Timer);		
	ev_timer_stop(zjh.loop, &m_qishou_timer_baiPai_sel);
	ev_timer_stop(zjh.loop, &m_baoJiao_Timer );
	
}

//配置信息，，传过来   创建桌子的时候。。会 立马加载远程配置项 。。

void Table_changsha::ConfInit_Remote(const Json::Value& val)
{
	log_debug("load Remote conf !!");
	Table::ConfInit_Remote(val);

	m_conf.m_bSupportChi = true;	
	
	m_conf.m_bYiPaoDuoXiang = true;
	m_conf.m_bGangShangPao = true;
	m_CardPool.Set(CARD_POOL_NO_WIND_DRAGON_FLOWER, tid);
	m_CardPool.GameId = 2252;
	g_changsha_ft2fn.king = false;
	m_conf.m_bFourceHuMutilHu =  true ;
	m_GAME_PLAYER = 4;	
	m_conf.m_bHu13yao = false;	
	m_conf.m_bNoSupport_Quanqiuren = true; 
				
	if ( json_check_play_type_id(val))
	{
		MJ_SUB_ID subid;
		int iCounts = json_get_play_type_id(val, 2822, subid);
		if (iCounts != 0 )
		{			
			m_conf.m_bSupportZhuaniao = true;
			log_debug("subid[%d]" , subid[0]);
			switch (subid[0])
			{
			case 2822001:
				m_conf.m_ZhuaniaoNum = 0;
				m_conf.m_bSupportZhuaniao = false;
				break;
			case 2822002:
				m_conf.m_ZhuaniaoNum = 1;
				break;
			case 2822003:
				m_conf.m_ZhuaniaoNum = 2;
				break;
			case 2822004:
				m_conf.m_ZhuaniaoNum = 4;
				break;
			case 2822005:
				m_conf.m_ZhuaniaoNum = 6;
				
				break;
			default:
				break;
			}			 
			m_room_config.Add(subid[0]);
		}

		

		iCounts = json_get_play_type_id(val, 2824, subid);
		if (iCounts != 0)
		{
			if (subid[0] == 2824001)
			{
				m_conf_changsha.gang_hou_bu_zhang = 2;
				log_debug("gang 2 ");
			}

			else if (subid[0] == 2824002 )
			{
				m_conf_changsha.gang_hou_bu_zhang = 4;
				log_debug("gang 4 ");
			}
			m_room_config.Add(subid[0]);
		}
		
		iCounts = json_get_play_type_id(val,  2820 , subid);
		if (iCounts != 0)
		{
			if (subid[0] == 2820002  )
			{
				m_GAME_PLAYER = 3;
				log_debug("player 33 !!");
			}
			
			m_room_config.Add(subid[0]);
		}
		json_get_play_type_id(val, 2823, subid);   
		if (subid.Find( 2823001))
		{
			m_conf_changsha.men_qing = true;
			log_debug("menqing  dahu gou xuan!! "); 
		}
		if (subid.Find(2823002))
		{
			m_conf_changsha.ping_hu_bu_jie_pao = true;
			log_debug("pinghu_bujie_pao");
		}
		m_room_config += subid;
		json_get_play_type_id(val, 2821, subid);   // 全开放，， 半开放 
		if (subid.Find(2821002 ))
		{
			m_conf_changsha.quan_kaifang = true;
			log_debug("quan_kai_fang");
		}
		if (subid.Find(2821003))
		{
			m_conf_changsha.ban_kaifang = true;
			log_debug("ban_kai_fang");
		}
		m_room_config += subid;
		json_get_play_type_id(val, 2825, subid);  
		if (subid.Find(2825001))
		{
			m_room_config += subid;
			m_conf_changsha.ping_hu_bu_jie_pao = true; 
			log_debug("pinghu bujie pao  ");
		}

		json_get_play_type_id(val, 2826, subid);
		if (subid.Find(2826001))
		{
			m_room_config += subid;  
			m_conf_changsha.kai_king = true;
			g_changsha_ft2fn.king = true;

			log_debug("kaiwang !!");
		}
			
	}	
}

// 处理。。报听。。 函数 。

int  Table_changsha::DealBaoTingOp()
{
	_uint8  nCanBaoTing = 0;
	for (int i = 0; i < m_GAME_PLAYER; i++)
	{
		if (i == m_bBanker) continue;
		if (Table::check_player_can_bao_ting(i , 27  )  )
		{
			nCanBaoTing++;
			m_nCanBaoTingArr[i] = 1;  
			proto::game::AckOpNotify stNotify;
			stNotify.set_chairid(i);	
			stNotify.set_optype(TYPE_BAO_TING);
			unicast(SERVER_OPERATE_NOTIFY, i, stNotify);

			log_debug("canbaojiao sid[%d] uid[%d]", i,   GetPlayerUid(i));
		}
	}
	m_nCanBaoTingCount = nCanBaoTing;
	log_debug("m_nCanBaoTingCount  _player_count [%d]  ", nCanBaoTing);
	if (nCanBaoTing > 0)
	{
		if (m_HandCard[m_bBanker].ChangeableCardsLen == 13)
		{
			log_debug("no  qishouhu ");
			GetFirstHandCard_NoOp(m_bBanker);
		}
		
		proto::game::hunanmj::MsgCmdNofityQiShouHu  pbQishouHu;    // 
		pbQishouHu.set_isqishouhustart(1);
		unicast(SER_CMD_QISHOU_HU_RESULT, m_bBanker, pbQishouHu);
		m_canBankCanOpCard = false; 
		m_nGameRunState = GAME_QISHOU_BAOTING_SEL;
		
		return  1;
	}
	return  0;
}

void Table_changsha::GameStart()
{
	log_debug("king %d" , m_conf_changsha.kai_king  );

	if(m_conf_changsha.kai_king ==false)
	{
		Table::GameStart();
		if (!QishouHu())
		{
			//  没有起手胡牌，， 专家摸牌。。继续 
			if (DealBaoTingOp() == 0)   // 没哟报叫。就继续庄家抓牌 
			{			
				OnUserGrapCard(m_bBanker); 
				m_nGameRunState = GAME_QISHOU_BAOTING_OVER;				
			}			
			else
				log_debug("you qishou hu !!")

		}
	}
	else
	{
		Table::GameStart(); 
		proto::game::ningxiang::gamestartmsg  syn;  
		int a = rand() % 6 + 1;
		int b = rand() % 6 +1  ; 
		syn.set_shai_one(a);
		syn.set_shai_two(b); 
		m_last_card_pos = 108 - (a + b) * 2; 
		a = m_CardPool.GetOneCard(); 
		log_debug("zhengwang [%x]" , a  );

		syn.set_zheng_wang(a); 
		m_zhengwang = a;
		GetChunKingZhengKing(a, a, b);
		syn.add_chun_wang(a ); 
		syn.add_chun_wang(b);
		m_chunwanga = a; 
		m_chunwangb = b;
		broadcast(SET_CMD_NINGXIAGN_GAME_START_SHAI_LAIZI  , syn); 
		std::string str = syn.SerializeAsString(); 
		save_video_data(SET_CMD_NINGXIAGN_GAME_START_SHAI_LAIZI,  str );  //  癞子，加录像的东西


		if (!QishouHu())
		{
			if (DealBaoTingOp() == 0)   // 没哟报叫。就继续庄家抓牌 
			{				
				OnUserGrapCard(m_bBanker); 
				m_nGameRunState = GAME_QISHOU_BAOTING_OVER;
			}
				
			else
				log_debug("you qishou hu !!") 

		}
	}
	
}

int Table_changsha::GetChunKingZhengKing(int card , int &pre_card  ,  int &next_card  )
{
	if (card< 0 || card> 0x30)
		return 0;

	int suit = get_suits(card);
	int face = get_face(card); 
	if (face == 1)
	{
		pre_card = make_card(suit, 9);
		next_card = make_card(suit ,  2);
	}
	else if (face == 9)
	{
		pre_card = make_card(suit , 8);
		next_card = make_card(suit ,  1);
	}
	else
	{
		pre_card = make_card(suit, face - 1);
		next_card = make_card(suit, face + 1);
	}
	log_debug("pre [%x] car[%x] next[%x] " , pre_card  , card  , next_card ); 
	return  0;
}

void Table_changsha::GameEnd()
{

	if (m_conf.m_bSupportZhuaniao)			  //是否支持抓鸟！！！
	{			
		m_DaNiaoID = m_bBanker;
		MJ_user_buffer UserID;
		int iCount = MultiHu(UserID);
		if (iCount == 0)
		{
			return Table::GameEnd();   //   没人胡牌，，流局 。。
		}			
		else if (iCount > 1)
		{
			m_DaNiaoID = m_bCurChairID;    //多人糊了
		}
		else if (iCount == 1) 
		{
			m_DaNiaoID = UserID[0];   //一人糊了
		}				
		_uint8 CountRes = m_CardPool.Count(); 

		log_debug("icount[%d] countres[%d] m_HaidiPlayer[%x ] m_bCurChairID[%d]",iCount, CountRes, m_HaidiPlayer, m_bCurChairID);

		if (  m_HaidiPlayer != INVALID_CHAIRID && m_conf_changsha.hai_di_niao)
		{
			_uint8 uiCard = m_bCurCard;
			if (m_conf_changsha.hai_di_niao_quan_zhong)
			{
				log_debug("into  this place  !!  ");
				for (int i = 0; i < m_conf.m_ZhuaniaoNum; i++)				
					m_niao.Add(uiCard);				
			}
			else
			{
				m_niao.Add(uiCard);
			}

			if (m_niao.Size() == 0 && CountRes == 0 && m_CardPool.IsEmpty())
			{
				m_niao.Add(m_bCurCard);
				log_debug("cao add niao [%x]" , m_bCurCard); 
				// 这个地方。。我实在不知道。 这几个配置项是啥意思。。日了狗了。。 
				//  但是有一条就是。。如果。是海底虎牌了。。 到这里。还没鸟。。 就吧最后一张牌。。加进来。做为鸟。 。艹 。。 

			}

		}
		else
		{

			int AllNiaoCount = MIN(m_conf.m_ZhuaniaoNum, CountRes);
			log_debug("allniaocount [%d]" , AllNiaoCount ) ;
			if (AllNiaoCount == 0 && m_CardPool.IsEmpty())
			{
				m_niao.Add(m_bCurCard);
				log_debug("add card [%x]" , m_bCurCard);
			}
			else
			{
				for (int i = 0; i < AllNiaoCount ; i++)
				{
					_uint8 uiCard = m_CardPool.GetOneCard();
					m_niao.Add(uiCard);
				}
			}			
		}
	}
	if (m_conf_changsha.kai_king)
	{
		_uint8  cent = m_bCurChairID; 
		if (PlayerAllOutCard[0].Size() + PlayerAllOutCard[1].Size() + PlayerAllOutCard[2].Size() + PlayerAllOutCard[3].Size() == 0)
			cent = m_bBanker; 
		if (cent< 0 || cent >= m_GAME_PLAYER)
			cent = 0; 

		MJ_user_buffer UserID;
		MultiHu(UserID);
		if (UserID.Size() > 1)
		{
			int ct = -1;
			for (_uint8 c = NextChair(cent); c != cent; c = NextChair(c))
			{
				if (c <0 || c >=m_GAME_PLAYER) continue; 
				if (m_bIsHu[c] && ct == -1)
					ct = 1;
				else if (m_bIsHu[c] && ct == 1)
				{
					m_bIsHu[c] = 0; 
					log_debug("cancel hu %d ", c);
				}

			}
		} 
	}

	Table::GameEnd();

}


void Table_changsha::GameEndReset()
{

	Table::GameEndReset();	
	
	memset( m_buzhang, 0, sizeof(m_buzhang));
	memset( m_cannothu, 0, sizeof(m_cannothu));
	memset(m_ningxiag_guozhang, 0, sizeof(m_ningxiag_guozhang));

	memset(m_RecvBaipaiMap, 0, sizeof(m_RecvBaipaiMap));
	memset(m_nRecvBaoTingPlayerMap, 0, sizeof(m_nRecvBaoTingPlayerMap));
	memset(m_fengdong, 0, sizeof(m_fengdong)); 
	memset( m_stUserOutCardAfterBaoTing, 0, sizeof(m_stUserOutCardAfterBaoTing));
	memset(m_speGang, 0, sizeof(m_speGang));

	m_nCanBaoTingCount = 0;
	m_nCanBaoTingCount = 0;
	m_chunwanga = 0;
	m_chunwangb = 0;
	m_zhengwang = 0;
	m_HaidiUser.Clear();
	m_HaidiPlayer = INVALID_CHAIRID;

	
	for (int i = 0; i < m_GAME_PLAYER; i++)
	{
		log_debug("uid[%d] m_nPlayingTimes[%d] tid[%d]", GetPlayerUid(i), m_nPlayingTimes ,tid  );
		m_qishou_fan[i].Clear();
		m_qishou_banpai[i].Clear();
		m_qishou_score[i] = 0;
	}
	memset(&m_qishou_shai, 0, sizeof(m_qishou_shai));

	m_bMultiOpUserNum = 0;

	GangHouClear();
		
	TIMER_STOP(m_StartBankOutCardTimer);
	TIMER_STOP(m_qishouhu_Timer_0);

	m_qishou_state = false;
	m_nGameRunState = 0;
	for (int i = 0; i < GAME_PLAYER; i++)		
		m_SiXi[i].Clear();

	


}

bool Table_changsha::CheckUserOutCard(_uint8 ChairID, _uint8 cbCard)
{	
	if (m_qishou_state)
	{
		log_error("error qishouhu_state[%d]" ,  m_qishou_state );
		return false;
	}
	if ( m_curGangPlayerSid != INVALID_CHAIRID)
	{
		log_debug("curGangSid[%d] ", m_curGangPlayerSid );
		return true;
	}
	if (!Table::CheckUserOutCard(ChairID, cbCard))
		return false;
	if ( m_buzhang[ChairID].bao_ting)
	{		
		if (m_bCurCard == cbCard)
		{
			return true;
		}
		log_error("baoting");
		return false;
	}
	return true;
}


bool Table_changsha::CheckUserOperator(_uint8 ChairID, int iOpType, _uint8 bCard)
{
	if (m_curGangPlayerSid != INVALID_CHAIRID)
	{
		if ((iOpType != TYPE_PASS) && (iOpType & (TYPE_CHI | TYPE_PENG)) && m_gang_hou_card.Find(bCard))
		{
			return true;			
		}
	}
	return Table::CheckUserOperator(ChairID, iOpType, bCard);
}

void Table_changsha::Score_GameEndSucc(HuScoreSet& score, proto::game::AckGameEnd& stGameend)
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

void Table_changsha::Proto_GameEndInfo(const MJ_user_buffer& UserID, proto::game::AckGameEnd& stGameend)
{

	Table::Proto_GameEndInfo(UserID, stGameend);

	tagHuNanGameEnd* end = stGameend.mutable_hunangameendinfo();
	end->set_leftcardnum(m_CardPool.Count());

	FanInfo* info[GAME_PLAYER] = { 0 };
	for (_uint8 i = 0; i < m_GAME_PLAYER; i++)
	{
		info[i] = end->add_faninfo();
	}

	if (m_conf.m_bSupportZhuaniao)
	{
		proto::game::hunanmj::tagJiangma* stJiangma = end->mutable_niao();
		stJiangma->set_num( m_niao.Size());
		stJiangma->set_cardtype(proto::game::hunanmj::ZHUANIAO);
		if ( m_HaidiPlayer != INVALID_CHAIRID && m_conf_changsha.hai_di_niao_quan_zhong)
		{
			if (m_niao.NotNull())
				stJiangma->add_card(m_niao[0]);
		}
		else
		{
			for (int i = 0; i < m_niao.Size(); i++)
			{
				stJiangma->add_card(m_niao[i]);
			}
		}
		stJiangma->set_chairid(m_DaNiaoID);

		if ( m_HaidiPlayer != INVALID_CHAIRID && m_conf_changsha.hai_di_niao_quan_zhong)
		{
			if (m_CardPool.Count() == 0 && m_niao.NotNull())
			{
				_uint8 iNiaoNum = CardNum(m_niao[0]) - 1;
				_uint8 next_banker = GetNextJuBanker();
				stJiangma->add_zhongchairid( NextChair(next_banker, iNiaoNum));   // m_bBanker
				log_debug("next_banker_sid [%d] next_bank_uid[%d] zhogn[%d] niao[%d]" ,
					next_banker, GetPlayerUid(next_banker), NextChair(next_banker, iNiaoNum) , m_niao[0]-1 );
			}
		}
		else
		{
			_uint8 next_banker = GetNextJuBanker();
			for (int i = 0; i < m_niao.Size(); i++)
			{
				_uint8 iNiaoNum = CardNum(m_niao[i]) - 1; 
				stJiangma->add_zhongchairid( NextChair(next_banker, iNiaoNum));   // m_bBanker
				log_debug("next_banker_sid [%d] next_bank_uid[%d] zhogn[%d] niao[%d]",
					next_banker, GetPlayerUid(next_banker), NextChair(next_banker, iNiaoNum) ,  m_niao[i] -1 );
			}
		}
	} // end 抓鸟 

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
				for (int j = 0; j < m_stHuScore[i].stHu.Size();j++)
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
	
	if (UserID.Size() == 1)
	{	
		//一个人胡
		_uint8 win_seat = UserID[0];	
		log_debug("hucount[%lu] win_id[%d]", m_stHuScore[win_seat].stHu.Size()  ,win_seat);
		if (m_stHuScore[win_seat].stHu.Size() == 0)
		{
			log_error("Error!!");
			return;
		}

		if ( m_stHuScore[win_seat].stHu[0].bType == HU_ZIMO)
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
					pfan->set_addnum( m_ft2fn->GetFan(tmpFan[win_seat][i]) ); 

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
					pfan->set_addnum( m_conf_changsha.kai_king ?  KING_WANG_DA_HU: NO_KING_WANG_DA_HU );
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
					pOtherFan->set_addnum(  m_conf_changsha.kai_king ?  KING_WANG_DA_HU: NO_KING_WANG_DA_HU );
					log_debug("uid[%d] sid[%d]" , GetPlayerUid(c ) , c );
				}				
			}
			info[win_seat]->set_fannum(tmpHuFanNums[win_seat] ); 
		}
		else if (m_stHuScore[win_seat].stHu.Size() &&  m_stHuScore[win_seat].stHu[0].bType == HU_PAOHU)
		{	
			//点炮			
			info[win_seat]->set_chairid(win_seat);
			info[win_seat]->set_specialtype(HU_PAOHU);						
			_uint8 iPaoID = m_stHuScore[win_seat].stHu[0].bPasHuChairID;
			log_debug("ipaoid [%d]" , iPaoID );
			
			if (m_stHuScore[win_seat].stHu.Size() == 1)
			{
				for (int i = 0; i < tmpFan[win_seat].Size(); i++)
				{
					AddFanInfo* pfan = info[win_seat]->add_addfan();
					pfan->set_addtype(tmpFan[win_seat][i]);
					log_debug("out type[%d]" , tmpFan[win_seat][i] );
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
					log_debug("out fan [%d]" , outFan[i]);
				}
				int add_c = m_stHuScore[win_seat].stHu.Size() - 1;
				for (int i = 0; i < add_c; i++)
				{
					AddFanInfo* pfan = info[win_seat]->add_addfan();
					pfan->set_addtype(MJ_FAN_TYPE_GANGHOUPAO);
					pfan->set_addnum(m_conf_changsha.kai_king ? KING_WANG_DA_HU:NO_KING_WANG_DA_HU );
					log_debug("add type [%d]", MJ_FAN_TYPE_GANGHOUPAO );
				}
				
			}


			for (_uint8 c = NextChair(win_seat); c != win_seat; c = NextChair(c))
			{
				info[c]->set_chairid(c);
				info[c]->set_specialtype(HU_INVALID);
				info[c]->set_fannum(0);				
			}			
			info[win_seat]->set_fannum(tmpHuFanNums[win_seat] );
			info[ m_stHuScore[win_seat].stHu[0].bPasHuChairID]->set_specialtype(HU_DIANPAO);
		}
	}
	else if (UserID.Size() > 1)
	{
		//一炮多响
		for (_uint8 i = 0; i < m_GAME_PLAYER; i++)
		{
			if (UserID.Find(i))
			{
				info[i]->set_chairid(i);
				info[i]->set_specialtype(HU_PAOHU);			
				log_debug("m_stHuScore[i].stHu.Size() [%lu] uid[%d]", m_stHuScore[i].stHu.Size() , GetPlayerUid(i) );
				if( m_stHuScore[i].stHu.Size() == 0)
				{
					log_error("error!!");
					continue;
				}
				_uint8 iPaoID = m_stHuScore[i].stHu[0].bPasHuChairID;
				log_debug("ipaoid [%d]", iPaoID);
				if ( m_stHuScore[i].stHu.Size() == 1)
				{
					for (int j = 0; j < tmpFan[i].Size(); j++)
					{ 
						AddFanInfo* pfan = info[i]->add_addfan();
						pfan->set_addtype(tmpFan[i][j]);
						pfan->set_addnum(m_ft2fn->GetFan(tmpFan[i][j])); 
						log_debug("add fan type[%d]" , tmpFan[i][j]);
					}
				}
				else
				{
					MJ_fan_buffer  outfan;
					m_ft2fn->FanFilter(tmpFan[i], outfan);
					for (int j = 0; j < outfan.Size(); j++)
					{
						AddFanInfo* pfan = info[i]->add_addfan();
						pfan->set_addtype( outfan[j]);
						pfan->set_addnum(m_ft2fn->GetFan( outfan[j]));
						log_debug("add fan type[%d]" , outfan[j]);
					}
					int add_c = m_stHuScore[i].stHu.Size() -1 ;
					log_debug("addc [%d]" ,add_c );
					for (int st  = 0; st < add_c; st++)
					{
						AddFanInfo* pfan = info[i]->add_addfan();
						pfan->set_addtype(MJ_FAN_TYPE_GANGHOUPAO );
						pfan->set_addnum( m_conf_changsha.kai_king?  KING_WANG_DA_HU:NO_KING_WANG_DA_HU  );
						log_debug("add fan 8");
					}
				} 
				info[i]->set_fannum(tmpHuFanNums[i] );
			}
			else
			{
				info[i]->set_chairid(i);
				info[i]->set_specialtype(HU_INVALID);
				info[i]->set_fannum(0);
				
			}

		}
		if (m_stHuScore[UserID[0]].stHu.Size() == 0)
		{
			log_error("error !!!");
			return;
		}
		_uint8 dianpaoID = m_stHuScore[UserID[0]].stHu[0].bPasHuChairID;		
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


}
void Table_changsha::Proto_GameEndInfo_GangInfo(_uint8 ChairID, proto::game::AckGameEnd& stGameend)
{

	proto::game::tagGameEndInfo* pGameendinfo = stGameend.mutable_endinfo(ChairID);
	for (int i = 0; i < m_stGang[ChairID].Gang.Size(); i++)
	{
		const tagGangScore& t = m_stGang[ChairID].Gang[i];
		proto::game::tagGameEndGang *pGang = pGameendinfo->add_gang();
		
		pGang->set_gangcard(t.Card);
		if (m_buzhang[ChairID].buzhang.Find(t.Card))
		{
			pGang->set_gangtype(TYPE_BU_ZHANG);
		}
		else
		{
			pGang->set_gangtype(t.iType);
		}		
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
	stGameend.set_gangscore(m_stGang[ChairID].iTotalScore);
}


void Table_changsha::Proto_Game_OpNotify(_uint8 ChairID, _uint8 CurChairID, _uint8 bCard, proto::game::AckOpNotify& stNotify)
{
	stNotify.Clear();
	stNotify.set_chairid(ChairID);

	if (ChairID == CurChairID)
	{	
		//非打牌玩家
		int iOpType = m_bUserOperate[ChairID];
		if (m_bUserOperate[ChairID] & TYPE_GANG)
		{
			m_bUserOperate[ChairID] |= TYPE_BU_ZHANG;
			iOpType = m_bUserOperate[ChairID];			
			
			proto::game::hunanmj::tagBuZhang* bu = stNotify.mutable_bu();
			proto::game::hunanmj::tagBuinfo* buinfo = bu->add_bu();
			buinfo->set_card(bCard);
			buinfo->set_state(TYPE_BU_ZHANG);
			if ( IsListening(ChairID, bCard))
			{
				proto::game::tagGangInfo* ganginfo = stNotify.add_gang();
				ganginfo->set_card(bCard);
				ganginfo->set_state(TYPE_ZHIGANG);
				
			}			
			if (stNotify.gang_size() == 0)
			{
				iOpType &= ~TYPE_GANG;
			}
		}
		stNotify.set_card(bCard);
		stNotify.set_optype(iOpType);
	}		
}

void Table_changsha::Proto_Game_OpNotifyAfterPengChi(_uint8 ChairID, _uint8 CurChairID, const MJ_opinfo& gang, proto::game::AckOpNotify& stNotify)
{
	stNotify.Clear();
	stNotify.set_chairid(ChairID);

	if (ChairID == CurChairID)
	{   
		//非打牌玩家
		int iOpType = m_bUserOperate[ChairID];
		if (m_bUserOperate[ChairID] & TYPE_GANG)
		{
			m_bUserOperate[ChairID] |= TYPE_BU_ZHANG;
			iOpType = m_bUserOperate[ChairID] & ~TYPE_GANG;;
			proto::game::hunanmj::tagBuZhang* bu = stNotify.mutable_bu();
			for (int i = 0; i < gang.Size();i++)
			{
				proto::game::hunanmj::tagBuinfo* buinfo = bu->add_bu();
				buinfo->set_card(gang[i].cbCard);
				buinfo->set_state(TYPE_BU_ZHANG);

				if ( IsListening(ChairID, gang[i].cbCard))
				{
					proto::game::tagGangInfo* ganginfo = stNotify.add_gang();
					ganginfo->set_card(gang[i].cbCard);
					ganginfo->set_state(gang[i].nState);
					iOpType |= gang[i].nState;
				}
			}
		}
		stNotify.set_card(0);
		stNotify.set_optype(iOpType);
	}
}

void Table_changsha::Proto_Game_UserGetCard(_uint8 ChairID, _uint8 MoChairID, _uint8 bCard, proto::game::AckUserGetCard& stUsergetcard)
{
	
	// Table::Proto_Game_UserGetCard(ChairID, MoChairID, bCard, stUsergetcard);	 
	//log_debug(" MoChairID [%d]  opcode %x", MoChairID ,  m_bUserOperate[MoChairID]);

#if 1

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
		stUsergetcard.set_optype(m_bUserOperate[MoChairID]);
		if (m_bUserOperate[MoChairID] & TYPE_LISTEN)
		{
			//打包听牌信息  这个sb 代码。。 额。。  稀乱的。 
			for (int j = 0; j < bListenLen; j++)
			{
				// stListenInfo[iPos].bOutCard 
				if(m_conf_changsha.kai_king &&  (stListenInfo[j].bOutCard == m_chunwangb || m_chunwanga==  stListenInfo[j].bOutCard  ) ) 
					continue; 			
				PackageTingInfo(ChairID, stListenInfo, j,  stUsergetcard.add_listeninfo());  
												
			}
		}
	}

#endif
	if (ChairID == MoChairID)
	{	
		//摸牌玩家
		if ( m_bUserOperate[ChairID] & TYPE_GANG)
		{
			stUsergetcard.clear_ganginfo();
			m_bUserOperate[ChairID] |= TYPE_BU_ZHANG;
			int iOpType = m_bUserOperate[ChairID];			
			
			MJ_opinfo stGang;			
			m_GameLogic.GetGangInfo(&m_HandCard[MoChairID], stGang); 
			//log_debug("all %d" , stGang.Size() ); 
			proto::game::hunanmj::tagBuZhang* bu = stUsergetcard.mutable_bu();
			for (int i = 0; i < stGang.Size(); i++)
			{				
				// log_debug("buzhang %x", stGang[i].cbCard); 
				if (m_buzhang[ChairID].bao_ting && stGang[i].cbCard != bCard)
					  continue;
				proto::game::hunanmj::tagBuinfo* buinfo = bu->add_bu();
				buinfo->set_card(stGang[i].cbCard);
				buinfo->set_state(TYPE_BU_ZHANG);
				// log_debug("buzhang %x" , stGang[i].cbCard );

				if ( IsListening(ChairID, stGang[i].cbCard))
				{
					proto::game::tagGangInfo* ganginfo = stUsergetcard.add_ganginfo();
					ganginfo->set_card(stGang[i].cbCard);
					ganginfo->set_state(stGang[i].nState);
				}				
			}
			if (stUsergetcard.ganginfo_size() == 0)
			{
				iOpType &= ~(TYPE_GANG);
			}
			else
			{
				iOpType &= ~(TYPE_GANG);
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


void Table_changsha::Proto_Game_OperateGang(_uint8 ChairID, _uint8 bCard, int iOpType, int iGangScore[GAME_PLAYER], proto::game::AckBcOpResult& stOpresult)
{
	Table::Proto_Game_OperateGang(ChairID, bCard, iOpType, iGangScore, stOpresult);
	if ( m_buzhang[ChairID].buzhang.Find(bCard))
	{
		stOpresult.set_optype(TYPE_BU_ZHANG);
		proto::game::hunanmj::tagBuZhangOpResult* pResult = stOpresult.mutable_hnopresult();
		//pResult->set_buzhangoptype(m_bUserOperate[ChairID] & TYPE_GANG);
		pResult->set_buzhangoptype(iOpType & TYPE_GANG);
	}
	else
	{
		stOpresult.set_optype(iOpType | TYPE_SUODING);
	}
	
}
void Table_changsha::InitBanker()
{	
	log_debug("into this_place  banker[%d]" ,m_bBanker);
	if ((m_nPlayingTimes == 0) || (m_bBanker >= m_GAME_PLAYER))
	{
		m_bBanker = 0;
		for (int i = 0; i < m_GAME_PLAYER; i++)
		{			
			if (m_SeatPlayer[i].player != NULL )				
			{
				log_info("room %d player uid:%d, m_room_owner_uid [%d] ",  tid, GetPlayerUid(i), m_room_owner_uid);
				if (m_SeatPlayer[i].player->uid == m_room_owner_uid)
				{
					m_bBanker = m_SeatPlayer[i].seatid;
					break;
				}				
			}
		}
	}
	
}

int Table_changsha::GetNextJuBanker()
{
	MJ_user_buffer UserID;
	int iCount = MultiHu(UserID);
	if (iCount == 0)
	{
		if ( m_HaidiPlayer == INVALID_CHAIRID)
		{
			if (m_HaidiUser.NotNull())
			{
				//m_bBanker = m_HaidiUser[0];
				return m_HaidiUser[0];
			}
			else
			{
				//m_bBanker = m_bBanker;
				return m_bBanker;
			}
		}
		else
		{
			//m_bBanker = m_HaidiPlayer;
			return m_HaidiPlayer;
		}

	}
	else if (iCount > 1)  // 一炮多想。 放炮这。。为下局 庄家！！
	{
		//m_bBanker = m_bCurChairID;
		return m_bCurChairID;;
	}
	else
	{
		return UserID[0]  ;
		//m_bBanker = UserID[0];  // 一炮一响 ， 虎牌的人为 下局庄家
	}
}

//		创建房间的玩家为庄，谁胡牌下局谁坐庄，一炮多响放炮的玩家下局为庄，流局摸	
//		海底牌的玩家为庄（若都不要海底牌，则第一个可以选择海底牌的玩家为庄），海	底牌被补走则补海底牌的玩家为庄
//算一下局。。庄家是谁

void Table_changsha::TurnBanker()
{
	MJ_user_buffer UserID;
	int iCount = MultiHu(UserID);
	if (iCount == 0)
	{		
		if (m_HaidiPlayer == INVALID_CHAIRID)
		{
			if ( m_HaidiUser.NotNull())
			{
				m_bBanker = m_HaidiUser[0];
			}
			else
			{
				m_bBanker = m_bBanker;
			}			
		}
		else
		{
			m_bBanker = m_HaidiPlayer;
		}

	}
	else if (iCount > 1)  // 一炮多想。 放炮这。。为下局 庄家！！
	{
		m_bBanker = m_bCurChairID;
	}
	else
	{
		m_bBanker = UserID[0];  // 一炮一响 ， 虎牌的人为 下局庄家
	}	
}
void Table_changsha::UserOperateChi(_uint8 ChairID, int iOpType, _uint8 bCard)
{
	if (m_qishou_banpai[ChairID].NotNull())
	{
		Send_BanpaiOver(ChairID);		
	}	
	if (m_curGangPlayerSid != INVALID_CHAIRID)
	{
		GangHouClear();
	}
	Table::UserOperateChi(ChairID, iOpType, bCard);
	
}

void Table_changsha::UserOperatePeng(_uint8 ChairID, _uint8 bCard)
{
	if (m_qishou_banpai[ChairID].NotNull())
	{
		Send_BanpaiOver(ChairID);		
	}
	if (m_curGangPlayerSid != INVALID_CHAIRID)
	{
		GangHouClear();
	}
	Table::UserOperatePeng(ChairID, bCard);
	
}

int  Table_changsha::GetGangInfo(_uint8 ChairID, MJ_opinfo& GangCardInfo)
{
	if (ChairID< 0 || ChairID>=  m_GAME_PLAYER)
		return  0;

	if (m_conf_changsha.kai_king == false)
		return m_GameLogic.GetGangInfo(&m_HandCard[ChairID], GangCardInfo);
	else
		return   m_GameLogic.GetGangInfo_King(&m_HandCard[ChairID], GangCardInfo , m_chunwanga , m_chunwangb ); 

	return 0;  
}

void Table_changsha::UserOperateGang(_uint8 ChairID, int iOpType, _uint8 bCard)
{
	if (m_qishou_banpai[ChairID].NotNull())
	{
		Send_BanpaiOver(ChairID);		
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
	
	if(  !m_buzhang[ChairID].buzhang.Find(bCard) )
	{	
		//是杠听牌不是补张
		m_buzhang[ChairID].bao_ting          = true;
		m_buzhang[ChairID].is_first_bao_ting = false;
		log_info("uid[%d] sid[%d] bao Ting success!", GetPlayerUid(ChairID), ChairID );

		//m_gang_get_multicard[ChairID] = true;
		m_curGangPlayerSid = ChairID;

		SendTingInfo_Simple(ChairID);
	}

}

void Table_changsha::PackageTingInfo(_uint8 ChairID, tagListenInfos stListenInfo[14], _uint8 iPos,  proto::game::tagListenInfo* listeninfo)
{
	if(m_conf_changsha.kai_king  &&  ((m_chunwanga == stListenInfo[iPos].bOutCard) ||  (m_chunwangb == stListenInfo[iPos].bOutCard)))
		return; 

	// log_debug("out %x " , stListenInfo[iPos].bOutCard );

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


int  Table_changsha::UserOperateHu(_uint8 ChairID, _uint8 bHuCard /* = 0xFF */)
{
	return   Table::UserOperateHu(ChairID, bHuCard);
	//return ret; 

}

//起手胡牌的列子。 

bool Table_changsha::hu_qishouKaiKing(_uint8  ChairID, MJ_qishou_fan &oFan, MJ_hand_buffer   &hc)
{
	const  HandCards* pHc = &m_HandCard[ChairID];
	int ct = 0;
	bool flag = true;
	bool dao_all_pai = false; 
	log_debug("zhenking [%x]  chuna[%x] chun[%x]" , m_zhengwang , m_chunwanga , m_chunwangb );
	int king_suit = get_suits(m_zhengwang);

	for (int i = 0; i < pHc->ChangeableCardsLen; i++)
	{
		ct = pHc->ChangeableCards[i];		
		if (ct == m_chunwangb || ct == m_chunwanga || ct == m_zhengwang)
		{
			flag = false;
			break;
		}
		int face = get_face(ct);
		if (face == 2 || face == 5 || face == 8)
		{
			flag = false;
			break;
		}
		
	}
	if (flag)
	{
		oFan.Add(MJ_FAN_CHANGSHA_BANBANHU); ////////////// 5555555555555555
		log_debug("add_ ban ban hu  ");
		dao_all_pai = true;
	} 

	// 却一色 
	bool  flag_queyise = true;
	for (int i = 0; i < pHc->ChangeableCardsLen; i++)
	{
		int suit = get_suits(pHc->ChangeableCards[i]);
		if (suit == king_suit)
		{
			flag_queyise = false;
			break;
		}
	}
	if (flag_queyise)
	{
		oFan.Add(MJ_FAN_CHANGSHA_QUEYIMEN); 
		log_debug("queyimen  ");
		dao_all_pai = true;
	}
	//  一枝花。 
	//  一枝花：起手牌中王的花色中，只有1张，并且这张牌是3张王中的一个
	int  all_c = 0;
	int  Find_one = -1;
	for (int i = 0; i < pHc->ChangeableCardsLen; i++)
	{
		int suit = get_suits(pHc->ChangeableCards[i]);
		if (suit == king_suit)
		{
			all_c++;  
			Find_one = pHc->ChangeableCards[i]; 
			if (all_c> 1) 
				break;
		}
		
	}
	if (all_c == 1 &&  (Find_one == m_chunwanga || Find_one== m_chunwangb || Find_one== m_zhengwang)  )
	{
		oFan.Add(MJ_FAN_CHANGSHA_DANSEYIZHIHUA);   //一枝花。胡牌。 //////777777777777777777	
		log_debug(" yizhihua  ");
		dao_all_pai = true;
	}
	int  zhen_king = 0;
	int  chun_king = 0, chun_king_b = 0;
	for (int i = 0; i < pHc->ChangeableCardsLen  ; i++)
	{
		if (pHc->ChangeableCards[i] == m_zhengwang)
			zhen_king++;
		else if (pHc->ChangeableCards[i] == m_chunwanga)
			chun_king++;
		else if (pHc->ChangeableCards[i] == m_chunwangb)
			chun_king_b++;

	}
	if (zhen_king>=3)
	{
		oFan.Add(MJ_FAN_SAN_ZHANG_KING);
		log_debug(" 3king  ");
	}
	if (chun_king>=4 ) 
	{
		oFan.Add( MJ_FAN_SI_CHUN_kING);
		log_debug(" 4king!!  ");
	} 
	if (chun_king_b>= 4)
	{
		oFan.Add(MJ_FAN_SI_CHUN_kING); 
		log_debug(" 4king!!  ");  
	}
	return !oFan.IsNull();

}

bool Table_changsha::Hu_Special4Hunan(_uint8 ChairID, MJ_qishou_fan& oFan, MJ_hand_buffer& hc)
{
	const  HandCards* pHc = &m_HandCard[ChairID];
	//log_debug("start__  sid[%d] ban_kaifang[%d]", ChairID , m_conf_changsha.ban_kaifang );   // 半开放。默认为。。false 
	bool  dao_all_pai   = false;
	map<int, int> daoMap;
	if (m_conf_changsha.changsha_qishouhu)  //
	{		
		oFan.Clear();
		hc.Clear();
		MJ_hand_buffer j258;
		m_GameLogic.Find_Jiang258(pHc, j258);		
		
		if ( m_conf_changsha.ban_kaifang == false)
		{
			if (j258.Size() == 1)
			{ 
				dao_all_pai = true;							
				oFan.Add(MJ_FAN_NINGXIANG_YIDIANHONG);   // 一点红。
				log_debug("add_ 一点红  ");
			}
		}
		if (  m_conf_changsha.ban_kaifang == false ) // if 金童玉女  // 22  
		{
			if (j258.Size() >= 4)
			{
				MJ_hand_buffer tmp = j258;
				if (tmp.Remove(0x12) && tmp.Remove(0x12) && tmp.Remove(0x22) && tmp.Remove(0x22))
				{
					if (!hc.Find(0x12) )
					{						
						daoMap[0x12] = 2;
					}
					if (!hc.Find(0x22))
					{						
						daoMap[0x22] = 2; 
					}
					oFan.Add(MJ_FAN_NINGXIANG_JINTONGYUNV); // 222222222222222
					log_debug("add_  金童玉女  ");
				}
			}
		}

		MJ_opinfo opinfo;			// 大四喜  四个一样的牌 // 333333333333333333  
		if (m_GameLogic.GetGangInfo(pHc, opinfo) & TYPE_ANGANG  && m_conf_changsha.ban_kaifang == false )		
		{
			for (int i = 0; i < opinfo.Size(); i++)
			{
				if (opinfo[i].nState == TYPE_ANGANG  )
				{
					oFan.Add(MJ_FAN_CHANGSHA_SIXIHU);  // 大四喜																
					daoMap[opinfo[i].cbCard] = 4;					
					log_debug("add  %x  " , opinfo[i].cbCard );
					m_SiXi[ChairID].Add(opinfo[i].cbCard);
				}
			}
		}		
	
		if (m_conf_changsha.ban_kaifang == false )
		{
			map<int, int> liuliushun_map; 
			for (int i = 0; i < pHc->ChangeableCardsLen; i++)
				liuliushun_map[pHc->ChangeableCards[i]]++;
			
			{
				map<int, int>  SangeMap;
				for (map<int, int>::iterator it = liuliushun_map.begin(); it != liuliushun_map.end(); ++it)
				{
					if (it->second >= 3)
						SangeMap[it->first] = it->second;
				}

				if (SangeMap.size() == 4)
				{
					oFan.Add(MJ_FAN_CHANGSHA_LIULIUSHUN);  //  66 顺 //44444444444444
					oFan.Add(MJ_FAN_CHANGSHA_LIULIUSHUN);  //  66 顺 //44444444444444
					for (map<int, int>::iterator s_it = SangeMap.begin(); s_it != SangeMap.end(); ++s_it)
					{
						int card  = s_it->first; 		
						log_debug("c == %d", card ); 
						if (daoMap[card] < 3)
							daoMap[card] = 3;
					}
				}
				else if (SangeMap.size() <= 3 && SangeMap.size()  >= 2   )
				{
					oFan.Add(MJ_FAN_CHANGSHA_LIULIUSHUN);  //  66 顺 //
					int add = 0;
					for (map<int, int>::iterator s_it = SangeMap.begin(); s_it != SangeMap.end(); ++s_it)
					{
						int c = s_it->first;			
						log_debug("c == %d" , c );
						add++;
						if (daoMap[c] < 3)
							daoMap[c] = 3;
						if (add>=2)
							break;
					}
				}
			}
		}


		if (j258.IsNull())  // 没有一个将。。 板板胡  。。
		{
			oFan.Add(MJ_FAN_CHANGSHA_BANBANHU); ////////////// 5555555555555555
			log_debug("add_ ban ban hu  ");			
			dao_all_pai = true;
			
		}
		
				
		_uint8 iTypeCount[MJ_TYPE_COMMON] = { 0 };
		if (m_GameLogic.GetCardTypeCount(pHc, iTypeCount) && m_conf_changsha.ban_kaifang == false )
		{
			if (iTypeCount[0] == 0 ||
				iTypeCount[1] == 0 ||
				iTypeCount[2] == 0)
			{
				oFan.Add(MJ_FAN_CHANGSHA_QUEYIMEN);  // 缺一门胡牌 /// 66666				
				if (iTypeCount[0] + iTypeCount[1] == 0 || iTypeCount[0] + iTypeCount[2] == 0 || iTypeCount[1] + iTypeCount[2] == 0)
				{
					
					oFan.Add(MJ_FAN_CHANGSHA_QUEYIMEN);   // 却两门 。胡两次 
					log_debug("add_ double  que yimen fan !!");
				}
				log_debug("add_  缺一门   ");			
				dao_all_pai = true;			
			}
			if (1)    
			{
				if (iTypeCount[0] == 1 && j258.Find(0x05))
				{
					
					oFan.Add(MJ_FAN_CHANGSHA_DANSEYIZHIHUA);   //一枝花。胡牌。 //////777777777777777777
					log_debug("add yizhi hua  ");
					dao_all_pai = true;
				}
				if (iTypeCount[1] == 1 && j258.Find(0x15))
				{
					
					oFan.Add(MJ_FAN_CHANGSHA_DANSEYIZHIHUA);
					log_debug("add_ 一枝花  ");
					dao_all_pai = true;
				}
				if (iTypeCount[2] == 1 && j258.Find(0x25))
				{
					
					oFan.Add(MJ_FAN_CHANGSHA_DANSEYIZHIHUA);
					dao_all_pai = true;
					log_debug("add_  yi zhi hua   ");
				}
			}	
			
		}

			
		if (  m_conf_changsha.ban_kaifang == false)
		{
			map<int, int > all_map;
			for (int i = 0; i < pHc->ChangeableCardsLen; i++)
			{
				int num = pHc->ChangeableCards[i];
				all_map[num]++;
			}
			for (int i = 1 ; i < 10;i++)
			{
				int a = all_map[i];  
				int b = all_map[0x10 + i];
				int c = all_map[0x20 + i];
				if ( a >= 2 && b>=2  && c>=2  )
				{
					oFan.Add(MJ_FAN_CHANGSHA_SANTONG); 
					log_debug("a b c %d %d %d" , a , b , c );
					if ( daoMap[i] < 2)
						daoMap[i] = 2;
					if ( daoMap[0x10+i] < 2)
						daoMap[0x10+i] = 2;
					if ( daoMap[0x20+i]<  2 )
						daoMap[0x20+i ] = 2;
				}
			}

		}		
		if ( m_conf_changsha.ban_kaifang == false)
		{
			map<int, int>  jiejiegao_map;
			for (int i = 0; i < pHc->ChangeableCardsLen; i++)
				jiejiegao_map[pHc->ChangeableCards[i]]++;

			for (int i = 0 ; i < 3  ;i++)
			{
				for (int j = 1; j <=7   ; j++)
				{
					int a = 0x10 * i + j;
					int b = a + 1;
					int c = b + 1;
					if (jiejiegao_map[a] >= 2 && jiejiegao_map[b] >= 2 && jiejiegao_map[c] >=  2 )
					{
						oFan.Add(MJ_FAN_CHANGSHA_JIEJIEGAO);
						log_debug("a b c %d %d %d" , a , b ,c );
						if (daoMap[a] < 2)
							daoMap[a] = 2; 
						if (daoMap[b] < 2)
							daoMap[b] = 2;
						if (daoMap[c] < 2)
							daoMap[c] = 2;
						j += 2;
						
					}
				}
			} 
		}		
	}
	if (dao_all_pai)
	{
		hc.Clear();
		hc.Add(pHc->ChangeableCards, pHc->ChangeableCardsLen);
	}
	else
	{
		hc.Clear();
		for (map<int, int>::iterator it = daoMap.begin(); it != daoMap.end();++it)
		{
			int  c = it->first;
			int  count = it->second;
			log_debug("c %x count %d" , c , count );
			for (int i = 0; i < count;i++)
				hc.Add(c);
		}
	}

	return !oFan.IsNull();
}


void Table_changsha::Send_HaidiCardMsg(_uint8 ChairID)
{
	m_bUserOperate[ChairID] = TYPE_HAIDI_YAO | TYPE_SUODING;
	for (int i = 0; i < m_GAME_PLAYER; i++)
	{
		proto::game::AckOpNotify stNotify;
		stNotify.set_chairid(ChairID);
		if (i == ChairID)
		{
			stNotify.set_optype(m_bUserOperate[ChairID]);
			stNotify.set_card(0);
		}
		unicast(SERVER_OPERATE_NOTIFY, i, stNotify);
	}
}



int Table_changsha::KingHu(_uint8  ChairID)
{
	if (m_conf_changsha.kai_king)
	{
		if (ChairID < 0 || ChairID  >= m_GAME_PLAYER )
			return 0;
		const  HandCards &pHc = m_HandCard[ChairID];
		int z_king = 0, chun_king = 0;
		for (int i = 0; i < pHc.ChangeableCardsLen; i++)
		{
			if (pHc.ChangeableCards[i] == m_zhengwang)
				z_king++;
			else if (pHc.ChangeableCards[i] == m_chunwangb || m_chunwanga == pHc.ChangeableCards[i])
				chun_king++;
		}
		if (z_king >= 3 || chun_king >= 4)
			return 1;
	}
	return 0;
	

}

int Table_changsha::IsJiangJiangHu_King(const HandCards  *phand)
{
	if (!m_conf_changsha.kai_king || phand== NULL)
		return 0;
	for (int i = 0; i < phand->FixedCardsLen; i++)
	{
		_uint8  iTemp = phand->FixedCards[i].CardData;
		if (phand->FixedCards[i].state  & TYPE_CHI)
			return false;  // 将将胡，，修改。 

		if (CardNum(iTemp) != 2 && CardNum(iTemp) != 5 && CardNum(iTemp) != 8)
			return false;
	}
	for (int i = 0; i < phand->ChangeableCardsLen; i++)
	{
		_uint8 iTemp = phand->ChangeableCards[i];
		if (iTemp == m_chunwangb || iTemp == m_chunwanga )  continue; 
		if (CardNum(iTemp) != 2 && CardNum(iTemp) != 5 && CardNum(iTemp) != 8)
			return false;
	}
	return true;
}


int Table_changsha::Check_Hu(_uint8 ChairID, HandCards& pHc, MJ_fan_buffer& oFan , _uint8 add_last_card )
{	
	
	
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
	MJ_win_pattern pattern;  

	if ( Is7Dui(ChairID, pHc, oFan))
	{
		oFan.Add(MJ_FAN_TYPE_QIDUI);	
		log_info("qidui");
		
	}   
	// m_bCurChairID 
	//if (m_conf_changsha.kai_king && ChairID != m_bCurChairID  &&  m_nGameRunState == GAME_QISHOU_BAOTING_OVER )
	if (m_conf_changsha.kai_king && ChairID == m_bCurChairID  &&  m_nGameRunState == GAME_QISHOU_BAOTING_OVER)
	{
		int z_king = 0, chun_king = 0, chun_king_b = 0; 
		for (int i = 0; i < pHc.ChangeableCardsLen; i++)
		{
			if (pHc.ChangeableCards[i] == m_zhengwang)
				z_king++;
			else if (pHc.ChangeableCards[i] == m_chunwanga)   // || m_chunwanga == pHc.ChangeableCards[i])
				chun_king++;
			else if (pHc.ChangeableCards[i] == m_chunwangb)
				chun_king_b++;
		}
		if (z_king >= 3)
			oFan.Add(MJ_FAN_SAN_ZHANG_KING);
		if (chun_king >= 4)
			oFan.Add( MJ_FAN_SI_CHUN_kING); 
		if (chun_king_b >= 4  )
			oFan.Add(MJ_FAN_SI_CHUN_kING);  
		
	}
	

	if ( !m_conf_changsha.kai_king  &&  m_GameLogic.is_jiang_jiang_hu(&pHc))
	{
		oFan.Add(MJ_FAN_CHANGSHA_JIANGJIANGHU);		
		log_info("jiang jiang hu !!");
	}
	if (IsJiangJiangHu_King(&pHc))
	{
		oFan.Add(MJ_FAN_CHANGSHA_JIANGJIANGHU);		
		log_info("jiang jiang hu ");
	}
	
	if (!m_conf_changsha.kai_king)
	{
		if( FormatCards(&pHc, pattern))  // 判断是否能胡牌
		{
			Check_Hu_Pattern(ChairID, pHc, oFan, pattern);
		}
		else
		{
			if (oFan.Size() > 0)
				AddOtherFan(ChairID, pHc, oFan, pattern);
		}
	}
	else
	{
		int all_a = 0;
		int all_b = 0;
		for (int i = 0; i < pHc.ChangeableCardsLen; i++)
		{
			if (pHc.ChangeableCards[i] == m_chunwanga) // || m_chunwangb == pHc.ChangeableCards[i])
				all_a++;
			else if (m_chunwangb == pHc.ChangeableCards[i])
				all_b++;
		}
		//  del  -------------------------------------
		for (int i = 0; i < all_a; i++)
			m_GameLogic.ChuPai(&pHc, m_chunwanga);
		for (int i = 0; i < all_b; i++)
			m_GameLogic.ChuPai(&pHc, m_chunwangb);

		//int ret = m_GameLogic.FormatCards_Laizi(pHc.ChangeableCards, pHc.ChangeableCardsLen, all_a + all_b);  
		int ret = m_GameLogic.FormatCards_LaiZi(&pHc,  pattern , all_a + all_b);
		all_lazi = all_a + all_b; 

		if (ret > 0)
			Check_Hu_Pattern(ChairID, pHc, oFan, pattern);
		else
		{
			if (oFan.Size() )
				AddOtherFan(ChairID, pHc, oFan, pattern);
		}
		//  add-----------------------------------------
		for (int i = 0; i < all_a; i++)
			m_GameLogic.AddCard(&pHc, m_chunwanga);
		for (int i = 0; i < all_b; i++)
			m_GameLogic.AddCard(&pHc, m_chunwangb);


#if 1
		// debug   
		int arr[20] = { 0 };
		int all_c = 0;
		int pos = 0;
		for (int i = 0; i < pHc.ChangeableCardsLen && pos<=13  ; i++)
		{
			if (pHc.ChangeableCards[i] == m_chunwangb || m_chunwanga == pHc.ChangeableCards[i])
				all_c++;
			else
				arr[pos++] = pHc.ChangeableCards[i];
		}
		int ok = laizi_le_can_hu(arr, pos, all_c  );
		if (ok != ret&& pos != 14  )
		{
			log_error("ret %d ok[%d] pos[%d] allc[%d] %s  lazie %x %x ",
				ret, ok, pos, all_c, CatHandCard(pHc, ChairID), m_chunwanga, m_chunwangb);
		}

		// debug   end  
#endif 
	}

	
	if (m_conf_changsha.kai_king &&  GAME_QISHOU_BAOTING_OVER != m_nGameRunState)
	{
		//log_debug("size fan[%d]  GAME_QISHOU_BAOTING_SEL [%d]", oFan.Size(), m_nGameRunState); 
		if ((oFan.Size() == 1 && oFan.Find(MJ_FAN_SAN_ZHANG_KING))
			|| (oFan.Size() == 1 && oFan.Find(MJ_FAN_SI_CHUN_kING))
			|| (oFan.Size() == 2 && oFan.Find(MJ_FAN_SI_CHUN_kING) && oFan.Find(MJ_FAN_SAN_ZHANG_KING))
			)
			//log_debug("return null !");
			return TYPE_NULL; 
	}
	
	int  iOpType = oFan.IsNull() ? TYPE_NULL : TYPE_HU; 
	return iOpType;
}

bool  Table_changsha::is_qiang_gang_hu()
{
	//log_debug("m_bRobWanGang [%d]  m_curGangPlayerSid[%d]", m_bRobWanGang, m_curGangPlayerSid );
	if (m_bRobWanGang == true && m_curGangPlayerSid != INVALID_CHAIRID)
		return true;
	return false; 

	//return  m_bRobWanGang == true;   //m_bRobWanGang 
}

bool Table_changsha::is_gang_shang_kai_hua(_uint8  chairID  ,  bool zimo)
{
	if (chairID >= 0 && chairID < m_GAME_PLAYER)
	{
		// log_debug("ChairID[%d]   m_bAddFanFlag[%d]  m_curGangPlayerSid[%d]", chairID, m_bAddFanFlag[chairID], m_curGangPlayerSid );
		if (m_bAddFanFlag[chairID] == 1 && zimo  && m_curGangPlayerSid != INVALID_CHAIRID )
		{ 
			return true;   
		}
	}
	return false;	
}


bool Table_changsha::Is7Dui(_uint8 ChairID, HandCards& pHc, MJ_fan_buffer& oFan)
{
	if (!m_conf_changsha.kai_king  && m_GameLogic.is_qi_dui(&pHc))
	{
		oFan.Add(MJ_FAN_TYPE_QIDUI);
		_uint8 hao_hua_qidui = 0;
		hao_hua_qidui = m_GameLogic.hao_hua_qi_dui(&pHc);
		log_debug("haohua [%d]", hao_hua_qidui);

		if (hao_hua_qidui == 1)
		{
			oFan.Add(MJ_FAN_HAO_HUA_QI_XIAO_DUI);
			oFan.Remove(MJ_FAN_TYPE_QIDUI);
		}
		else if (hao_hua_qidui == 2)
		{
			oFan.Add(MJ_FAN_HAO_HUA_DOUBLE_QI_XIAO_DUI);
			oFan.Remove(MJ_FAN_TYPE_QIDUI);
		}
		else if (hao_hua_qidui == 3)
		{
			oFan.Add(MJ_FAN_HAO_HUA_THREE_QI_XIAO_DUI);
			oFan.Remove(MJ_FAN_TYPE_QIDUI);
		} 				
		return true;
	}
	else   if(m_conf_changsha.kai_king )
	{
		int  haohua_size = 0;
		int ret = m_GameLogic.qidui_laizi(&pHc, m_chunwanga, m_chunwangb, haohua_size); 
		if (ret > 0)
		{
			oFan.Add(MJ_FAN_TYPE_QIDUI);
			log_debug("haohua [%d]", haohua_size );
			if (haohua_size == 1)
			{
				oFan.Add(MJ_FAN_HAO_HUA_QI_XIAO_DUI);
				oFan.Remove(MJ_FAN_TYPE_QIDUI);
			}
			else if (haohua_size == 2)
			{
				oFan.Add(MJ_FAN_HAO_HUA_DOUBLE_QI_XIAO_DUI);
				oFan.Remove(MJ_FAN_TYPE_QIDUI);
			}
			else if (haohua_size == 3)
			{
				oFan.Add(MJ_FAN_HAO_HUA_THREE_QI_XIAO_DUI);
				oFan.Remove(MJ_FAN_TYPE_QIDUI);
			}
			return true;
		}
	}

	return false;
}


bool Table_changsha::IsQuanqiuren(const HandCards& pHc)
{
	return m_GameLogic.IsDanDiao(pHc);
	//return (pHc.ChangeableCardsLen == 2) && (pHc.ChangeableCards[0] == pHc.ChangeableCards[1]);	
}

int Table_changsha::is_258_jiang(_uint8  jiang  )
{
	if (CardNum(jiang) == 0x2 || CardNum(jiang) == 0x5 || CardNum(jiang) == 0x8)
		return  1;
	return 0;
}

bool Table_changsha::other_chu_pai()
{
	for (int i = 0; i < m_GAME_PLAYER; i++)
	{
		if (PlayerAllOutCard[i].Size()  > 0 )
			return  false;
	}
	return  true;
}

void Table_changsha::AddOtherFan(_uint8 ChairID, const HandCards& pHc, MJ_fan_buffer& oFan, const MJ_win_pattern& oPattern)
{
	if (m_conf_changsha.kai_king )
	{
		//if(  m_GameLogic.IsPengPengHu(&pHc, oPattern.kan, oPattern.jiang))
		if( NoQiDui(oFan) && pengpenghu_king(ChairID ,  &pHc , oPattern  , all_lazi) )
		{
			oFan.Add(MJ_FAN_TYPE_PENGPENGHU);
			log_debug(" peng peng hu  nfan[%d]", 1);
		}
		//if (m_GameLogic.IsQingYiSe(&pHc))   //  
		if (m_GameLogic.qingyise_King(&pHc , m_chunwangb ,m_chunwanga ))
		{
			oFan.Add(MJ_FAN_TYPE_QINGYISE);
			log_info(" qing yi se  nfan[%d]", 1);
		}
		if (oFan.Size() && is_gang_shang_kai_hua(ChairID, m_bCurChairID == ChairID))
		{
			oFan.Add(MJ_FAN_TYPE_GANGSHANGHUA);			
			log_info(" gang shagn kai hua nfan[%d] f[%d]", 1, 1);
		}
		//、、加番牌型。 抢杠胡 
		if (oFan.Size() && is_qiang_gang_hu())
		{
			oFan.Add(MJ_FAN_TYPE_QIANGGANGHU);			
			log_info("qiang gang hu  nfan[%d] f[%d]", 1, 1);
		}
		//杠上炮  //、、加番牌型。 

		if (oFan.Size() && ChairID != m_bCurChairID  && !m_bRobWanGang && m_bAddFanFlag[m_bCurChairID] && m_buzhang[m_bCurChairID].bao_ting == 1)
		{
			oFan.Add(MJ_FAN_TYPE_GANGHOUPAO);			
			log_info("gang shang pao  nfan[%d] m_buzhang[ChairID].bao_ting[%d]", 1, m_buzhang[ChairID].bao_ting);
		}
		if (oFan.Size() && ChairID == m_bBanker&&   other_chu_pai())  
		{
			oFan.Add(MJ_FAN_TYPE_TIANHU); 			
			//log_info("add 13天胡[%d] ", 1);
		}

		// 地胡  庄家起手14张打出一张后，其他人胡牌，则为地胡

		if (oFan.Size() && m_bBanker != ChairID &&  m_bBanker == m_bCurChairID  &&   dihu_judge())
		{
			oFan.Add(MJ_FAN_TYPE_DIHU);			
			log_info("di hu nfan[%d] f[%d]", 1, 1);
		}
		if (oFan.Size() && all_lazi == 0  && no_king_fan(ChairID))
		{
			oFan.Add(MJ_FAN_NO_KING_FAN);
		}
		// 报听。。放炮 
		if (oFan.Size() &&  m_nGameRunState != GAME_QISHOU_BAOTING_SEL 
			  && ChairID != m_bCurChairID && m_buzhang[m_bCurChairID].bao_ting && m_buzhang[m_bCurChairID].is_first_bao_ting)
		{
			
			oFan.Add(MJ_FAN_BAOTING_JIE_PAO);		
			log_debug("add fan baotingpao [%d]  isbgihu[%d]", 1, 1);
		}
		//  报听。接炮 
		if( oFan.Size() && m_nGameRunState != GAME_QISHOU_BAOTING_SEL
			  && ChairID != m_bCurChairID && m_buzhang[ChairID].bao_ting && m_buzhang[ChairID].is_first_bao_ting)
		{		
			oFan.Add(MJ_FAN_BAOTING_FANG_PAO);		
			log_debug("add fan baotingpao [%d]  isbgihu[%d]  ChairID[%d]", 1, 1, ChairID);
		}
		
		return; 

	}
	bool  Is_Gang_Shang_Kai_Hua = false;
	if (oFan.Size() &&  m_GameLogic.IsPengPengHu(&pHc, oPattern.kan, oPattern.jiang))	
		oFan.Add(MJ_FAN_TYPE_PENGPENGHU);
	if(oFan.Size() && m_GameLogic.IsQingYiSe(&pHc))   //  	
		oFan.Add(MJ_FAN_TYPE_QINGYISE);
	if (IsQuanqiuren(pHc))	
		oFan.Add(MJ_FAN_TYPE_QUANQIUREN);
	if (oFan.Size() && m_GameLogic.hai_di_lao_yue(m_CardPool.IsEmpty(), 0, ChairID == m_bCurChairID, 0))
		oFan.Add(MJ_FAN_TYPE_HAIDILAOYUE);
	if (oFan.Size() && m_GameLogic.hai_di_pao(m_CardPool.IsEmpty(), ChairID != m_bCurChairID))	
		oFan.Add(MJ_FAN_TYPE_HAIDIPAO);
	if (oFan.Size() && is_gang_shang_kai_hua(ChairID, m_bCurChairID == ChairID))
	{
		oFan.Add( MJ_FAN_TYPE_GANGSHANGHUA);
		Is_Gang_Shang_Kai_Hua = true;
	}	
	if (oFan.Size() && is_qiang_gang_hu())
		oFan.Add( MJ_FAN_TYPE_QIANGGANGHU);
	if (oFan.Size() && ChairID != m_bCurChairID  && !m_bRobWanGang && m_bAddFanFlag[m_bCurChairID] && m_buzhang[m_bCurChairID].bao_ting == 1)
		oFan.Add(MJ_FAN_TYPE_GANGHOUPAO);
	if (oFan.Size() && ChairID == m_bCurChairID &&  m_GameLogic.IsMenQing(&pHc))
	{
		if (m_conf_changsha.men_qing == true)
		{
			if (m_GameLogic.HasOneAnGang(&pHc) > 1 || (Is_Gang_Shang_Kai_Hua == false && m_GameLogic.HasOneAnGang(&pHc) == 1))
			{
				log_debug("  有一个暗杠 ，但是不是杠上开花， 或者是2个暗杠的 都不是门清了 ");
			}
			else
			{
				oFan.Add(MJ_FAN_TYPE_MENQIANQING);				
			}
		}				
	}
	if (oFan.Size() && ChairID == m_bBanker &&   other_chu_pai() ) // PlayerAllOutCard[m_bBanker].Size() == 0)
		oFan.Add( MJ_FAN_TYPE_TIANHU);	
	// 地胡  庄家起手14张打出一张后，其他人胡牌，则为地胡
	
	if (oFan.Size() && m_bBanker != ChairID &&  m_bBanker == m_bCurChairID  &&   dihu_judge())	
		oFan.Add(MJ_FAN_TYPE_DIHU);			

	// 报听。。放炮 
	if (oFan.Size() && ChairID != m_bCurChairID && m_buzhang[m_bCurChairID].bao_ting && m_buzhang[m_bCurChairID].is_first_bao_ting)
	{		
		oFan.Add(MJ_FAN_BAOTING_JIE_PAO);							
	}
	//  报听。接炮 
	if (oFan.Size() && ChairID != m_bCurChairID && m_buzhang[ChairID].bao_ting && m_buzhang[ChairID].is_first_bao_ting)
	{
		oFan.Add(MJ_FAN_BAOTING_FANG_PAO);				
	}
	if (oFan.Size() && ChairID == m_bCurChairID  && m_buzhang[ChairID].bao_ting  && m_buzhang[ChairID].is_first_bao_ting)
	{
		oFan.Add(MJ_FAN_BAOTING_ZIMO);
	}
	if (no_king_fan(ChairID))
	{
		oFan.Add(MJ_FAN_NO_KING_FAN);
	}
}


int Table_changsha::no_king_fan(_uint8  ChairID)
{
	if (!m_conf_changsha.kai_king)
		return 0;

	if (ChairID < 0 || ChairID >= m_GAME_PLAYER)
		return 0;

	const HandCards  *phc = &m_HandCard[ChairID];
	for (int i = 0; i < phc->ChangeableCardsLen; i++)
	{
		if (phc->ChangeableCards[i] == m_chunwanga || m_chunwangb == phc->ChangeableCards[i])
			return 0;
		if (phc->ChangeableCards[i] == m_zhengwang)
			return 0;
	}
	for (int i = 0; i < phc->FixedCardsLen;i++)
	{
		if (phc->FixedCards[i].state  & TYPE_PENG || phc->FixedCards[i].state & TYPE_GANG)
		{
			if (phc->FixedCards[i].CardData == m_zhengwang)
				return 0;
		}
		else if (phc->FixedCards[i].state & TYPE_CHI)
		{
			int a = phc->FixedCards[i].c[0];
			int b = phc->FixedCards[i].c[2];
			int c = phc->FixedCards[i].c[2];
			if (a == m_zhengwang || b == m_zhengwang || c == m_zhengwang)
				return 0;
		}
	}
	return 1;
}

int Table_changsha::pengpenghu_king(_uint8  ChairID, const  HandCards  *pHc, const MJ_win_pattern& oPattern, int  nlaizi)
{
	if (ChairID < 0 || ChairID >= m_GAME_PLAYER)
		return 0; 	
	for (int i = 0; i < pHc->FixedCardsLen;i++)
	{
		if (pHc->FixedCards[i].state  & TYPE_CHI)
			return  0;
	}
	if (nlaizi== 0  ) 
		return   m_GameLogic.IsPengPengHu(  pHc , oPattern.kan, oPattern.jiang)  ; 

	map<int, int> all_map; 
	int arr[5][10];
	memset(arr, 0, sizeof(arr));
	for (int i = 0; i < pHc->ChangeableCardsLen; i++)
	{
		int ct = pHc->ChangeableCards[i];
		if (ct == m_chunwangb || ct == m_chunwanga)  continue;
		all_map[ct]++;
		int suit = get_suits(ct);
		int face = get_face(ct);
		if (suit >= 0 && suit < 3 && face > 0 && face < 10)
		{
			arr[suit][face]++;
			//arr[suit][0]++;
		}
	}
	int  f_laizi = nlaizi;
	for (map<int, int>::iterator it = all_map.begin(); it != all_map.end(); ++it)
	{
		int jiang = it->first; 
		nlaizi = f_laizi; 
		log_debug("jiang %x  nlaizi[%d] " , jiang  ,nlaizi  );
		for (int r = 0; r < 3; r++) 
		{
			for (int li = 1; li <= 9; li++)
			{
				if (arr[r][li ] == 0 )   // 
					continue;  
				if (jiang !=  (0x10 * r + li) )
				{
					if ( arr[r][li] == 1 )
						nlaizi -= 2;
					else if ( arr[r][li] == 2)
						nlaizi--;
					else if ( arr[r][li] == 3)
						continue;
					else if ( arr[r][li] ==  4 )
						nlaizi -= 2;
				}
				else
				{
					if ( arr[r][li] == 1  )
						nlaizi--;
					else if ( arr[r][li] == 3)
						nlaizi -= 2; 
					else if ( arr[r][li] ==  4 )
						nlaizi--;
				}
			}
		}
		if (nlaizi >= 0)
		{
			log_debug("jaing %d nlaizi%d" , jiang , nlaizi );
			return 1;
		}
		

	}
	
	return  0;
}

int Table_changsha::NoQiDui(MJ_fan_buffer &oFan)
{
	if (oFan.Find(MJ_FAN_TYPE_DIHU)
		|| oFan.Find(MJ_FAN_HAO_HUA_QI_XIAO_DUI)
		|| oFan.Find(MJ_FAN_HAO_HUA_DOUBLE_QI_XIAO_DUI)
		|| oFan.Find(MJ_FAN_HAO_HUA_THREE_QI_XIAO_DUI)
		)
	{
		return 0;
	}
	return 1; 
}

int Table_changsha::Check_Hu_Pattern(_uint8 ChairID, const HandCards& pHc, MJ_fan_buffer& oFan, const MJ_win_pattern& oPattern)
{
	bool PingHuBuJiePao = m_conf_changsha.ping_hu_bu_jie_pao;
	int base = m_conf_changsha.kai_king ? KING_WANG_DA_HU : NO_KING_WANG_DA_HU;
	int nFan = oFan.Size() ? oFan.Size() * base : 0;
	bool IsBigHu = (nFan >= base ? 1 : 0);
	
	bool IsAddJiangDui = false;
	bool  Is_Gang_Shang_Kai_Hua = false;

	if (m_conf_changsha.kai_king)
	{
		//if (m_GameLogic.IsPengPengHu(&pHc, oPattern.kan, oPattern.jiang))
		if ( NoQiDui(oFan) &&  pengpenghu_king(ChairID ,  &pHc , oPattern  , all_lazi ) )
		{
			oFan.Add(MJ_FAN_TYPE_PENGPENGHU);		
			IsBigHu = true;
			nFan += KING_WANG_DA_HU;
			log_debug(" peng peng hu  nfan[%d]", nFan);
		}		
		//if (m_GameLogic.IsQingYiSe(&pHc))   //  
		if (m_GameLogic.qingyise_King(&pHc , m_chunwangb , m_chunwanga ))
		{
			oFan.Add(MJ_FAN_TYPE_QINGYISE);			
			IsBigHu = true;
			nFan += KING_WANG_DA_HU;
			log_info(" qing yi se  nfan[%d]", nFan);
		}
		log_debug("jiamg %x  len %d",oPattern.jiang , pHc.ChangeableCardsLen  );
		//if (oFan.Size() == 0 && 
		//	( is_258_jiang(oPattern.jiang)  || laizi_le_can_hu_jiang258( (int*)pHc.ChangeableCards  , pHc.ChangeableCardsLen , all_lazi)))		 
		int arr[20] = { 0 };
		int  len = 0;
		for (int i = 0; i < pHc.ChangeableCardsLen  && len<= 13; i++)
		{
			arr[len++] = pHc.ChangeableCards[i];
		}
		if (oFan.Size() == 0 && (is_258_jiang(oPattern.jiang) || (laizi_le_can_hu_jiang258(arr , len  , all_lazi))))
		{
			
			oFan.Add(MJ_FAN_TYPE_JIANGDUI);
			nFan += 2;
			IsAddJiangDui = true;
			log_debug("jiangdui[%d]  %d", nFan , oPattern.jiang );
		}

		if (oFan.Size() && is_gang_shang_kai_hua(ChairID, m_bCurChairID == ChairID))
		{
			oFan.Add(MJ_FAN_TYPE_GANGSHANGHUA);
			IsBigHu = true;
			nFan += KING_WANG_DA_HU;
			Is_Gang_Shang_Kai_Hua = true;
			log_info(" gang shagn kai hua nfan[%d] f[%d]", nFan, 1);
		}
		//、、加番牌型。 抢杠胡 
		if (oFan.Size() && is_qiang_gang_hu())
		{
			oFan.Add(MJ_FAN_TYPE_QIANGGANGHU);
			IsBigHu = true;
			nFan += KING_WANG_DA_HU;
			log_info("qiang gang hu  nfan[%d] f[%d]", nFan, 1);
		}
		//杠上炮  //、、加番牌型。 

		if (oFan.Size() && ChairID != m_bCurChairID  && !m_bRobWanGang && m_bAddFanFlag[m_bCurChairID] && m_buzhang[m_bCurChairID].bao_ting == 1)
		{
			oFan.Add(MJ_FAN_TYPE_GANGHOUPAO);
			nFan += KING_WANG_DA_HU;
			IsBigHu = true;
			log_info("gang shang pao  nfan[%d] m_buzhang[ChairID].bao_ting[%d]", nFan, m_buzhang[ChairID].bao_ting);
		}
		if (oFan.Size() && ChairID == m_bBanker&&   other_chu_pai())  
		{
			oFan.Add(MJ_FAN_TYPE_TIANHU);
			IsBigHu = true;
			nFan += KING_WANG_DA_HU;
			log_info("add 13天胡[%d] ", nFan);
		}

		// 地胡  庄家起手14张打出一张后，其他人胡牌，则为地胡

		if (oFan.Size() && m_bBanker != ChairID &&  m_bBanker == m_bCurChairID  &&   dihu_judge())
		{
			oFan.Add(MJ_FAN_TYPE_DIHU);
			IsBigHu = true;
			nFan += KING_WANG_DA_HU;
			log_info("di hu nfan[%d] f[%d]", nFan, 1);
		}
		if (oFan.Size() && all_lazi == 0   && no_king_fan(ChairID))
		{
			oFan.Add(MJ_FAN_NO_KING_FAN);
		}
		// 报听。。放炮 
		if (oFan.Size() && ChairID != m_bCurChairID && m_buzhang[m_bCurChairID].bao_ting && m_buzhang[m_bCurChairID].is_first_bao_ting)
		{
			if (IsBigHu == false)
			{
				oFan.Add(MJ_FAN_BAOTING_JIE_PAO);
				oFan.Remove(MJ_FAN_TYPE_JIANGDUI);
				nFan += 5;
			}
			else
			{
				oFan.Add(MJ_FAN_BAOTING_JIE_PAO);
				nFan += KING_WANG_DA_HU;
				IsBigHu = true;
			}
			log_debug("add fan baotingpao [%d]  isbgihu[%d]", nFan, IsBigHu);
		}
		//  报听。接炮 
		if (oFan.Size() && ChairID != m_bCurChairID && m_buzhang[ChairID].bao_ting && m_buzhang[ChairID].is_first_bao_ting)
		{
			if (IsBigHu)
			{
				oFan.Add(MJ_FAN_BAOTING_FANG_PAO);
				nFan += KING_WANG_DA_HU;
			}
			else
			{
				oFan.Add(MJ_FAN_BAOTING_FANG_PAO);
				oFan.Remove(MJ_FAN_TYPE_JIANGDUI);
				nFan += 5;
			}
			log_debug("add fan baotingpao [%d]  isbgihu[%d]  ChairID[%d]", nFan, IsBigHu, ChairID);
		}
		if (IsBigHu && IsAddJiangDui)
		{
			oFan.Remove(MJ_FAN_TYPE_JIANGDUI);
			nFan -= 2;
			log_debug("bighu nfan[%d]", nFan);
		} 
		if (PingHuBuJiePao   &&  nFan<= 2    && m_bCurChairID  != ChairID)
		{
			oFan.Clear();
		}
		return 0;
	}
	
	

	//  门清要是自己莫得。 
	//log_debug("charid[%d] sid[%d] uid[%d] m_bCurCard[%x]", m_bCurChairID, ChairID, GetPlayerUid(ChairID), m_bCurCard);
	
	// 2条 碰碰胡   胡牌的牌型。  可以碰/杠/补，此时任意对子都可以做将，不再限定258
	if (m_GameLogic.IsPengPengHu(&pHc, oPattern.kan, oPattern.jiang))
	{
		oFan.Add(MJ_FAN_TYPE_PENGPENGHU);
		nFan += 8;
		IsBigHu = true;
		log_debug(" peng peng hu  nfan[%d]" , nFan );
	}
	// 3 清一色。任意牌将  胡牌的牌型。  可以碰/杠/补，此时任意对子都可以做将，不再限定258 
	// 可以组成 3*n  + 2  的 组合 。

	if ( m_GameLogic.IsQingYiSe(&pHc))   //  
	{
		oFan.Add( MJ_FAN_TYPE_QINGYISE );
		nFan += 8;
		IsBigHu = true;
		log_info(" qing yi se  nfan[%d]" , nFan );
	}
	// 将对  普通的小胡。。如果有了大胡，，就不要小胡了。 
	if (  oFan.Size() == 0  && is_258_jiang(oPattern.jiang))		
	{		
		oFan.Add(MJ_FAN_TYPE_JIANGDUI);		
		nFan += 4;
		IsAddJiangDui = true;
		log_debug("xiao  hu   nfan[%d] " , nFan );
	}
	// 自摸报听   加番牌型。
	if ( oFan.Size() &&  ChairID == m_bCurChairID  && m_buzhang[ChairID].bao_ting  && m_buzhang[ChairID].is_first_bao_ting )
	{
		oFan.Add(MJ_FAN_BAOTING_ZIMO);
		nFan += 8;
		IsBigHu = true;
		log_debug("nfan[%d]  zimobaoting " , nFan);
	}

	//  4  海底捞月。 加番牌型。
	if (oFan.Size() && m_GameLogic.hai_di_lao_yue( m_CardPool.IsEmpty() ,0, ChairID == m_bCurChairID,0) )
	{
		oFan.Add(MJ_FAN_TYPE_HAIDILAOYUE);
		nFan += 8;
		IsBigHu = true;
		log_info("hai di lao yue  nfan[%d]" , nFan );
	}

	//、、加番牌型。 杠上开花 
	if (oFan.Size()  &&  is_gang_shang_kai_hua(ChairID, m_bCurChairID == ChairID))
	{
		oFan.Add( MJ_FAN_TYPE_GANGSHANGHUA);
		IsBigHu = true;
		nFan += 8;
		Is_Gang_Shang_Kai_Hua = true;
		log_info(" gang shagn kai hua nfan[%d] f[%d]", nFan, 1);
	}
	// 1条   门清。。
	//门清：勾上时门清算大胡，不勾时门清也能胡牌，但是不算门清的分数
	//平胡不接炮：小胡平胡只能自摸，大胡平胡可以接炮不自摸  加番牌型。

	if ( oFan.Size()&&  ChairID == m_bCurChairID &&  m_GameLogic.IsMenQing(&pHc)) 		
	{
		if (m_conf_changsha.men_qing == true)
		{
			if ( m_GameLogic.HasOneAnGang(&pHc)>1 ||  (Is_Gang_Shang_Kai_Hua == false && m_GameLogic.HasOneAnGang(&pHc)==1))
			{
				log_debug("  有一个暗杠 ，但是不是杠上开花， 或者是2个暗杠的 都不是门清了 ");
			}
			else
			{
				oFan.Add(MJ_FAN_TYPE_MENQIANQING);
				nFan += 8;
				IsBigHu = true;
				log_debug("add menqing dahu ..remover jiangdui xiaohu  nfan[%d] f[%d]", nFan, 1);
			}
		}						
		else
		{
			log_debug("no gou xuan men qing !!");
		}
		
		log_debug("men qing !");
	}
	//、、加番牌型。  海底炮 (加番5)
	if ( oFan.Size() && m_GameLogic.hai_di_pao(m_CardPool.IsEmpty(), ChairID != m_bCurChairID) )
	{
		oFan.Add(MJ_FAN_TYPE_HAIDIPAO);		
		IsBigHu = true;
		nFan += 8;
		
		log_debug(" haidi  pao nfan[%d] f[%d]"  , nFan , 1);
	}
	//、、加番牌型。 
	if (    IsQuanqiuren(pHc))
	{
		oFan.Add(MJ_FAN_TYPE_QUANQIUREN);			
		nFan += 8;
		IsBigHu = true;
		
		log_info("quan qiu ren dan tiao nfan[%d] f[%d]" , nFan , 1);
	}

	//、、加番牌型。 抢杠胡 
	if (oFan.Size() &&  is_qiang_gang_hu())
	{
		oFan.Add( MJ_FAN_TYPE_QIANGGANGHU );		
		IsBigHu = true;
		nFan += 8;		
		log_info("qiang gang hu  nfan[%d] f[%d]" , nFan , 1 );
	}
	//杠上炮  //、、加番牌型。 
	
	if (oFan.Size() &&  ChairID != m_bCurChairID  && !m_bRobWanGang && m_bAddFanFlag[m_bCurChairID] && m_buzhang[m_bCurChairID].bao_ting ==  1 )
	{
		oFan.Add( MJ_FAN_TYPE_GANGHOUPAO);		
		nFan += 8;
		IsBigHu = true;		
		log_info("gang shang pao  nfan[%d] m_buzhang[ChairID].bao_ting[%d]", nFan, m_buzhang[ChairID].bao_ting  );
	}
	//、、加番牌型。 天胡
	if (oFan.Size()&& ChairID == m_bBanker&&   other_chu_pai())   //PlayerAllOutCard[m_bBanker].Size() == 0)
	{
		oFan.Add( MJ_FAN_TYPE_TIANHU);		
		IsBigHu = true;
		nFan += 8;
		
		log_info("add 13天胡[%d] " , nFan  );
	}	
	
	// 地胡  庄家起手14张打出一张后，其他人胡牌，则为地胡
	
	if (oFan.Size() && m_bBanker != ChairID &&  m_bBanker == m_bCurChairID  &&   dihu_judge()) 		
	{
		oFan.Add(MJ_FAN_TYPE_DIHU);
		IsBigHu = true;
		
		nFan += 8;		
		log_info("di hu nfan[%d] f[%d]" , nFan , 1);
	}

	// 报听。。放炮 
	if (oFan.Size() && ChairID != m_bCurChairID && m_buzhang[m_bCurChairID].bao_ting && m_buzhang[m_bCurChairID].is_first_bao_ting)
	{
		if (IsBigHu == false)
		{
			oFan.Add(MJ_FAN_BAOTING_JIE_PAO);
			oFan.Remove(MJ_FAN_TYPE_JIANGDUI);
			nFan += 4;
		}
		else
		{
			oFan.Add(MJ_FAN_BAOTING_JIE_PAO);
			nFan += 8;
			IsBigHu = true;
		}
		log_debug("add fan baotingpao [%d]  isbgihu[%d]", nFan, IsBigHu);
	}
	//  报听。接炮 
	if (oFan.Size() && ChairID != m_bCurChairID && m_buzhang[ChairID].bao_ting && m_buzhang[ChairID].is_first_bao_ting)
	{
		if (IsBigHu)
		{
			oFan.Add(MJ_FAN_BAOTING_FANG_PAO);
			nFan += 8;
		}
		else
		{
			oFan.Add(MJ_FAN_BAOTING_FANG_PAO);
			oFan.Remove(MJ_FAN_TYPE_JIANGDUI);
			nFan += 4;
		}
		log_debug("add fan baotingpao [%d]  isbgihu[%d]  ChairID[%d]", nFan, IsBigHu, ChairID);
	}
	if (IsBigHu && IsAddJiangDui)
	{
		oFan.Remove(MJ_FAN_TYPE_JIANGDUI);
		nFan -= 4;
		log_debug("bighu nfan[%d]", nFan);
	}


	if (PingHuBuJiePao && nFan == 4 && m_bCurChairID != ChairID)
	{
		//log_debug("pinghu bujie pao  bcurid[%d]  chaidid[%d]", m_bCurChairID, ChairID);
		oFan.Clear();

	}

	return  0;
	

}



//地胡的判断

bool Table_changsha::dihu_judge()
{
	
	for (int i = 0; i < m_GAME_PLAYER;i++)   // m_bGetCardCount
	{		
		if (i != m_bBanker  &&  (m_bGetCardCount[i]  ))
			return false;
		if (i != m_bBanker &&  PlayerAllOutCard[i].Size()  )
			return  false; 
	}
	if( PlayerAllOutCard[m_bBanker].Size() <= 1 )
		return true;
	return false;
}


void Table_changsha::Calc_Hu_Player_Base_Score(_uint8 ChairID, int iBaseScore[GAME_PLAYER])
{
	if (ChairID < 0 || ChairID >= m_GAME_PLAYER)
		return; 
	if (!m_bIsHu[ChairID])
		return;


	// 开往。。起手胡。。 ！！
	if (m_conf_changsha.kai_king  && PlayerAllOutCard[0].Size() + PlayerAllOutCard[1].Size() + PlayerAllOutCard[2].Size() + PlayerAllOutCard[3].Size() == 0)
	{
		int sore = m_oFan[ChairID].Size() * KING_WANG_DA_HU;
		sore = std::min(sore, 21); 		
		for (_uint8 c = NextChair(ChairID); c != ChairID; c = NextChair(c))
		{			
			if (c<0 || c>= m_GAME_PLAYER) continue; 			
			iBaseScore[c] = sore * 1;			
			log_debug("c[%d] iBaseScore[%d]", c, iBaseScore[c]);

		}
		return;
	}

	MJ_fan_buffer OutFan;
	int iScore = 0;
	int iFanCount = 0;
	log_debug("gang_id[%x]  BaseScore[%d] sid[%d]uid[%d]", m_curGangPlayerSid, m_BaseScore , ChairID , GetPlayerUid(ChairID) );
	
	if (m_curGangPlayerSid == INVALID_CHAIRID)
	{
		if (m_ft2fn->FanFilter( m_oFan[ChairID], OutFan))
		{
			iFanCount = OutFan.Size();
			log_debug("ifancount%d", iFanCount);
		}
		else
		{
			iFanCount = m_oFan[ChairID].Size();
		}
		log_debug("sid[%d] ifancount[%d]", ChairID, iFanCount);
	}
	else
	{
		if (m_ft2fn->FanFilter( m_MultiFan[ChairID], OutFan))
		{
			iFanCount = OutFan.Size();			
			log_debug("ifancount %d" , iFanCount );
		}		
		else
		{			
			iFanCount = m_MultiFan[ChairID].Size();
		}			
		log_debug("sid[%d] ifancount[%d]" ,  ChairID , iFanCount );

	}
	int hu = m_stHuScore[ChairID].stHu.Size();

	if(!m_conf_changsha.kai_king)
	{
		for (int i = 0; i < hu; i++)
		{
			if (i == 0)
				iScore += m_stHuScore[ChairID].stHu[i].iFanNum;
			else
			{
				if (m_stHuScore[ChairID].stHu[i].iFanNum >= 8)
					iScore += 8;
				else
					iScore += 4;
			}
			log_debug("iscore [%d]  m_stHuScore[ChairID].stHu[i].iFanNum [%d]", iScore, m_stHuScore[ChairID].stHu[i].iFanNum);
		}

		int BaoTingScore[4] = { 0 };
		if (m_stHuScore[ChairID].stHu.Size() && m_stHuScore[ChairID].stHu[0].bType == HU_ZIMO)
		{
			for (int j = NextChair(ChairID); j != ChairID; j = NextChair(j))
			{
				if (m_buzhang[j].bao_ting  && m_buzhang[j].is_first_bao_ting)
				{
					BaoTingScore[j] = 8;
					log_info("into add j[%d] uid[%d]", j, GetPlayerUid(j));
				}
			}
		}
		log_debug("socre [%d]", iScore);
		iBaseScore[ChairID] = iScore * m_BaseScore;

		for (_uint8 c = NextChair(ChairID); c != ChairID; c = NextChair(c))
		{
			int tmpScore = iScore * m_BaseScore;
			iBaseScore[c] = tmpScore;
			if (BaoTingScore[c] != 0)
			{
				if (iScore <= 4)
					iBaseScore[c] = 8;
				else
					iBaseScore[c] += 8;
				log_info("iBaseScore[c] %d", iBaseScore[c]);

			}
			log_debug("c[%d] iBaseScore[%d]", c, iBaseScore[c]);

		}
	}
	else
	{
		for (int i = 0; i < hu; i++)
		{
			if (i == 0)
				iScore += m_stHuScore[ChairID].stHu[i].iFanNum;
			else
			{
				if (m_stHuScore[ChairID].stHu[i].iFanNum >=  KING_WANG_DA_HU )
					iScore += KING_WANG_DA_HU;
				else
					iScore += 2;
			}
			iScore = std::min( 3* KING_WANG_DA_HU   , iScore);  // 3个大胡封顶 
			log_debug("i[%d] iscore[%d]  m_stHuScore[ChairID].stHu[i].iFanNum [%d] hu[%d]",i, iScore, m_stHuScore[ChairID].stHu[i].iFanNum , hu);
		}

		int BaoTingScore[4] = { 0 };
		if (m_stHuScore[ChairID].stHu.Size() && m_stHuScore[ChairID].stHu[0].bType == HU_ZIMO)
		{
			for (int j = NextChair(ChairID); j != ChairID; j = NextChair(j))
			{
				if (m_buzhang[j].bao_ting  && m_buzhang[j].is_first_bao_ting)
				{
					BaoTingScore[j] = KING_WANG_DA_HU;
					log_info("into add j[%d] uid[%d]", j, GetPlayerUid(j));
				}
			}
		}
		log_debug("socre [%d]", iScore);
		iBaseScore[ChairID] = iScore * 1; 
		for (_uint8 c = NextChair(ChairID); c != ChairID; c = NextChair(c))
		{
			int tmpScore = iScore *  1 ;
			iBaseScore[c] = tmpScore;
			if ( m_stHuScore[ChairID].stHu.Size() && m_stHuScore[ChairID].stHu[0].bType == HU_PAOHU  
				 && m_bCurChairID>= 0 && m_bCurChairID< m_GAME_PLAYER&&  m_cannothu[m_bCurChairID] 
				 )
			{
				log_debug("paohu ,,king !! %d" ,  m_bCurChairID );
				break;
			}
			if (BaoTingScore[c] != 0)
			{
				if (iScore <= 2)
					iBaseScore[c] = KING_WANG_DA_HU;
				else
					iBaseScore[c] += KING_WANG_DA_HU;
				log_info("iBaseScore[c] %d", iBaseScore[c]);

			}
			log_debug("c[%d] iBaseScore[%d]", c, iBaseScore[c]);

		}
	}
	

}

void Table_changsha::Calc_HuScore(_uint8 ChairID, int iBaseScore[GAME_PLAYER], int iScore[GAME_PLAYER])
{
	if (m_bIsHu[ChairID])
	{		
		
		if( m_bCurChairID == ChairID)  //自摸
		{
			for (_uint8 c = NextChair(ChairID); c != ChairID; c = NextChair(c))
			{
				iScore[ChairID] += iBaseScore[c];
				iScore[c]       -= iBaseScore[c];
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


void Table_changsha::Calc_ZhongNiao(_uint8 ChairID, _uint8 ShaiNum, int iBaseScore[GAME_PLAYER], int iScore[GAME_PLAYER])
{
	if (ChairID < 0 || ChairID >= m_GAME_PLAYER)
		return;
	if (m_bIsHu[ChairID])
	{		
		_uint8 shai = CardNum(ShaiNum) - 1;
		_uint8  next_banker = GetNextJuBanker();
		_uint8 ShaiChairID = NextChair(next_banker, shai);		  // m_bBanker
		if (m_bCurChairID == ChairID)   // 自摸。 
		{
			if (!m_conf_changsha.kai_king)
			{
				if (ChairID == ShaiChairID)
				{
					for (_uint8 c = NextChair(ChairID); c != ChairID; c = NextChair(c))
					{
						iScore[ChairID] += iBaseScore[c];
						iScore[c] -= iBaseScore[c];
					}
				}
				else
				{
					iScore[ChairID] += iBaseScore[ShaiChairID];
					iScore[ShaiChairID] -= iBaseScore[ShaiChairID];
				}
			}
			else
			{
				MJ_fan_buffer  fan; 
				m_ft2fn->FanFilter(m_oFan[ChairID], fan);
				//
				int add_ =  2; 
				if (fan.Size())
					add_ = KING_WANG_DA_HU;
				log_debug("add %d" , add_ );
				if (ChairID == ShaiChairID)
				{
					for (_uint8 c = NextChair(ChairID); c != ChairID; c = NextChair(c))
					{
						iScore[ChairID] += add_;
						iScore[c] -= add_;
					}
				}
				else
				{
					iScore[ChairID] += add_; 
						iScore[ShaiChairID] -= add_;
				}
			}

		}
		else
		{
			if (m_stHuScore[ChairID].stHu.Size() == 0)  // modify  
			{
				log_error("error !");
				return;
			}				
			if( m_conf_changsha.kai_king )
			{
				if (m_stHuScore[ChairID].stHu.Size() && m_stHuScore[ChairID].stHu[0].bType == HU_PAOHU
					&& m_bCurChairID >= 0 && m_bCurChairID < m_GAME_PLAYER&&  m_cannothu[m_bCurChairID]
					)
				{
					log_debug("paohu ,,king !! %d", m_bCurChairID);
					return;
				}

				MJ_fan_buffer  fan;
				m_ft2fn->FanFilter(m_oFan[ChairID], fan);
				int add_ = 2;
				if (fan.Size())
					add_ = KING_WANG_DA_HU;			
				_uint8 iPaoID = m_stHuScore[ChairID].stHu[0].bPasHuChairID; 
				if (iPaoID >= 0 && iPaoID < m_GAME_PLAYER  && ShaiChairID >= 0 && ShaiChairID < m_GAME_PLAYER)
				{
					if ((ChairID == ShaiChairID) || (iPaoID == ShaiChairID))
					{
						iScore[ChairID] += add_; //   iBaseScore[iPaoID];
						iScore[iPaoID] -= add_;  //    iBaseScore[iPaoID];
					}
				}
			}
			else
			{
				_uint8 iPaoID = m_stHuScore[ChairID].stHu[0].bPasHuChairID;
				if (iPaoID >= 0 && iPaoID < m_GAME_PLAYER  && ShaiChairID >= 0 && ShaiChairID < m_GAME_PLAYER)
				{
					if ((ChairID == ShaiChairID) || (iPaoID == ShaiChairID))
					{
						iScore[ChairID] += iBaseScore[iPaoID];
						iScore[iPaoID] -= iBaseScore[iPaoID];
					}
				}
			}
			
		}
		log_info("uid:%d ChairID:%d, niao:0x%x[%d,%d,%d,%d] ",
			GetPlayerUid(ChairID), ChairID, CardNum(ShaiNum) - 1, iScore[0], iScore[1], iScore[2], iScore[3]);
	}
	
}

int Table_changsha::Calc_Fan(_uint8 ChairID, MJ_fan_buffer& oFan)
{
	int iTotalFan = 0;		

	MJ_fan_buffer oFilterFan;
	if (m_ft2fn->FanFilter(oFan, oFilterFan))
	{
		//计算番之前把如果过滤番，如果有大胡就过滤所有小胡
		for (int i = 0; i < oFilterFan.Size(); i++)
		{
			iTotalFan += m_ft2fn->GetFan(oFilterFan[i]);
			log_debug("iTotal [%d]  GetFan[%d]", iTotalFan, m_ft2fn->GetFan(oFilterFan[i]) );
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

void  Table_changsha::DealNingxiangHuBroad(_uint8 chairid, MJ_fan_buffer  & fan)
{
	if (m_CardPool.GameId != NINGXIAG_MAJIAGN_GAME_ID )
		 return;	 
	proto::game::NingXiangHuFan  hu_ack;
	MJ_fan_buffer  outFan;
	m_ft2fn->FanFilter( fan, outFan);
	for (int i = 0; i < outFan.Size();i++)
	{
		hu_ack.add_hu_fan(outFan[i]);
	}
	hu_ack.set_chairid(chairid);
	broadcast(SER_CMD_NINGXIANG_HU_  , hu_ack);
	
	
}

int Table_changsha::Calc_Hu(_uint8 ChairID, _uint8 bCard, int Score[GAME_PLAYER])
{
	
	if (m_curGangPlayerSid != INVALID_CHAIRID)
	{
		log_debug("m_curGangPlayerSid %d", m_curGangPlayerSid );
		m_MultiFan[ChairID].Clear();
		m_MultiHufanNums[ChairID] = 0;		
		for (int i = 0; i < m_gang_hou_card.Size(); i++)
		{
			int tmpScore[GAME_PLAYER] = { 0 };
			if (m_bCurChairID == ChairID)
			{
				m_GameLogic.AddCard(&m_HandCard[ChairID], m_gang_hou_card[i]);
				Table::Calc_Hu(ChairID, m_gang_hou_card[i], tmpScore);
				m_GameLogic.ChuPai(&m_HandCard[ChairID], m_gang_hou_card[i]);
			}			
			else
			{
				Table::Calc_Hu(ChairID, m_gang_hou_card[i], tmpScore);
			}		

			m_MultiFan[ChairID] += m_oFan[ChairID];
			m_MultiHufanNums[ChairID] += m_HuFanNums[ChairID];
			Calc_Score(Score, tmpScore);
		}
		return 0;
	}
	return Table::Calc_Hu(ChairID, bCard, Score);	
}


void Table_changsha::UserLastOutCard(_uint8 ChairID, int& iOpType, MJ_fan_buffer& oFan)
{
	Table::UserLastOutCard(ChairID, iOpType, oFan);
	if (iOpType & TYPE_HU)
	{
		oFan.Add(MJ_FAN_TYPE_HAIDIPAO);
	}	
}

void Table_changsha::UserGangShangHua(_uint8 ChairID, _uint8 bCard, MJ_fan_buffer& oFan)
{
	if (!m_stGang[ChairID].Gang.IsNull())
	{
		_uint8 bGangCard = m_stGang[ChairID].Gang.Last().Card;
		if (!m_buzhang[ChairID].buzhang.Find(bGangCard))
		{
			Table::UserGangShangHua(ChairID, bCard, oFan);
		}
	}	
}

void Table_changsha::UserGangHouPao(_uint8 ChairID, _uint8 bCard, MJ_fan_buffer& oFan)
{
	if (!m_stGang[m_bCurChairID].Gang.IsNull())
	{
		_uint8 bGangCard = m_stGang[m_bCurChairID].Gang.Last().Card;
		if (!m_buzhang[m_bCurChairID].buzhang.Find(bGangCard))
		{
			Table::UserGangHouPao(ChairID, bCard, oFan);
		}		
	}
}

void Table_changsha::UserQiangGangHu(_uint8 ChairID, _uint8 bCard, MJ_fan_buffer& oFan)
{	
	if (!m_buzhang[m_bCurChairID].buzhang.Find(bCard))
	{
		Table::UserQiangGangHu(ChairID, bCard, oFan);
	}
}

int Table_changsha::CheckOutCardOtherPlayerCanOpCode(_uint8 ChairID, _uint8 bCard, MJ_major& major, int& fan_num)
{
	if (ChairID < 0 || ChairID >= m_GAME_PLAYER)
		return  0;  

	int iOpType = 0;
	bool guozhanghu = false;  
	if (ChairID < 0 || ChairID >= m_GAME_PLAYER)
		goto Next_Label;
	if(  m_conf_changsha.kai_king 
		&& (m_fengdong[m_bCurChairID]|| m_fengdong[ChairID] == 1 || m_cannothu[ChairID]|| ( m_chunwanga == bCard || m_chunwangb== bCard))
	  )
	{
		log_debug("spe.. ");
		goto Next_Label;  
	}
	
	iOpType = Table::CheckOutCardOtherPlayerCanOpCode(ChairID, bCard, major, fan_num);		
	log_debug("sid[%d] uid[%d]op[%x] %s" , ChairID , GetPlayerUid(ChairID) , iOpType , CatHandCard(m_HandCard[ChairID] , ChairID) ); 
		

	if ((!m_conf_changsha.kai_king)  && (iOpType & TYPE_HU) && m_ningxiag_guozhang[ChairID])
	{
		guozhanghu = true;
		log_debug("cancel hu ");
		iOpType &= (~TYPE_HU);
	}
		
	if (m_conf_changsha.kai_king && (iOpType & TYPE_HU) && m_ningxiag_guozhang[ChairID])
	{
		log_debug("fannum [%d] guozhanghu [%d]" , m_ningxiag_guozhang[ChairID] ,fan_num  );  // modify ..!!!!
		iOpType &= (~TYPE_HU);
		if (fan_num>= KING_WANG_DA_HU  && m_ningxiag_guozhang[ChairID]<= 2 )
		{
			iOpType |= TYPE_HU;
		}
		if ((iOpType & TYPE_HU) == 0)
			guozhanghu = true;
	}


	if (m_conf_changsha.kai_king && bCard< 0x30  && bCard> 0 )   // 日了够了。。  重写的。  吃  
	{
		iOpType &= ~(TYPE_CHI); 
		std::vector<int> chivec;
		major.m_Chi.Clear();
		chivec.push_back(m_chunwanga);
		chivec.push_back(m_chunwangb);
		if(ChairID>= 0&& ChairID< m_GAME_PLAYER)
			iOpType |=  m_GameLogic.Getchi_laizi((const HandCards*) &m_HandCard[ChairID],  bCard ,   major.m_Chi,  chivec );
	}



	if ( guozhanghu) 
	{
		//if ( m_louhu[ChairID].IsLock(bCard, fan_num))
		{			
			iOpType &= ~TYPE_HU;			
			log_info("uid[%d] sid[%d] bcard[%x] ", GetPlayerUid(ChairID), ChairID, bCard);
			proto::game::AckMessage msg;
			msg.set_message(msg_word(E_MW_GUO_ZHUANG));
			unicast(SERVER_SEND_MESSAGE, ChairID, msg);
		}
	}
	if (m_bCurChairID != PreChair(ChairID))   // 不是出牌的下家不能吃。 
	{
		iOpType &= ~TYPE_CHI;		
	}
	
	if (  m_buzhang[m_bCurChairID].bao_ting  
		&&   m_buzhang[m_bCurChairID].is_first_bao_ting == 0
		&&   m_stUserOutCardAfterBaoTing[m_bCurChairID].Size() != 0)
	{
		iOpType &= ~(TYPE_CHI | TYPE_PENG | TYPE_GANG | TYPE_BU_ZHANG ); 
		log_debug("quxiao chi peng gang !!！！ ");
	}
	
	if (m_buzhang[ChairID].bao_ting)
	{		
		iOpType &= ~(TYPE_CHI | TYPE_PENG);		
		if (iOpType & TYPE_GANG)
		{
			
			if ( IsListening(ChairID, bCard))
			{
				iOpType |= TYPE_BU_ZHANG;
			}
			else
			{
				iOpType &= ~(TYPE_GANG);
			}		
		}
		iOpType &= ~(TYPE_CHI | TYPE_PENG | TYPE_GANG | TYPE_BU_ZHANG  ); 
		log_debug("取消吃碰杠 [%d]" , iOpType );

	}
	else if(iOpType & TYPE_GANG)
	{
		iOpType |= TYPE_BU_ZHANG;				
		if ( !IsCanGangAfterTing(ChairID, major, bCard) )
		{
			iOpType &= ~(TYPE_GANG);
			log_debug("取消杠。 杠了不能听 ");
		}
	}	

	if (m_conf_changsha.kai_king)
	{		
		if (bCard == m_zhengwang)
			iOpType &= ~(TYPE_CHI | TYPE_HU); 		
	}
	

Next_Label: 
	log_debug("sid[%d] ioptype %x" , ChairID , iOpType );
	return iOpType;
}

//摸牌。。后判断能进行的操作  检查玩家 。自身。。抓牌后。能够进行的操作。 判断。 

int Table_changsha::CheckPlaySlefGrpCardCanOperateCode(_uint8 ChairID, _uint8 bCard, MJ_major& major, int& fan_num)
{
	int iOpType = 0;
	if(m_conf_changsha.kai_king  &&    (m_fengdong[ChairID] || m_cannothu[ChairID]) )
	{
		log_debug("feng chairid [%d]" , ChairID );
		goto  Next_labe;
	}
	// m_louhu[ChairID].Unlock();   // 过张胡牌 *	玩家摸牌后判断是否能够杠听胡 虎牌后。。 取消过张胡 。
	iOpType = Table::CheckPlaySlefGrpCardCanOperateCode(ChairID, bCard, major, fan_num);	

	if (m_conf_changsha.kai_king && (bCard == m_chunwanga || bCard == m_chunwangb))
	{
		iOpType &= ( ~TYPE_GANG ); 
	}
	if (m_conf_changsha.kai_king && (iOpType &TYPE_GANG) )
	{
		major.m_GetGangInfo.Clear();
		iOpType &= ~(TYPE_GANG); 
		HandCards  *phc = &m_HandCard[ChairID];
		if (phc->ChangeableCardsLen < 14 && bCard< 0x30 && bCard> 0 && ChairID>= 0 && ChairID< m_GAME_PLAYER )
		{
			m_GameLogic.AddCard(phc, bCard);
			int ret =   m_GameLogic.GetGangInfo_King( &m_HandCard[ChairID], major.m_GetGangInfo , m_chunwanga , m_chunwangb  ); 
			iOpType |= ret; 			
			m_GameLogic.ChuPai(phc, bCard); 
		} 		
	}


	if (m_buzhang[ChairID].bao_ting )  // 报听 
	{ 
		iOpType &= ~( TYPE_BU_ZHANG | TYPE_GANG| TYPE_BAO_TING | TYPE_CHI | TYPE_PENG ) ;   // 报听修改
		log_debug("cancel  chipenggang   [%d]" , iOpType );

	}
	else if( iOpType & TYPE_GANG ) 
	{
		iOpType |= TYPE_BU_ZHANG; 
		log_debug("ioptype [%x]" , iOpType );		
		if ( !IsCanGangAfterTing(ChairID, major, bCard) )
		{
			iOpType &= ~(TYPE_GANG  ); 			
			log_debug("cancel   ioptype [%x]", iOpType); 
		}
	}
	
	if (m_conf_changsha.kai_king)
	{
		HandCards  *phc = &m_HandCard[ChairID];
		if (phc->ChangeableCardsLen < 14 && bCard< 0x30 && bCard> 0)
		{
			m_GameLogic.AddCard(phc, bCard);
			int  ret = KingHu(ChairID);
			if (ret)
				iOpType |= TYPE_HU;
			m_GameLogic.ChuPai(phc, bCard);
		}
		if (bCard == m_chunwanga || bCard == m_chunwangb)
		{
			iOpType &= ~(TYPE_CHI | TYPE_PENG | TYPE_GANG | TYPE_BU_ZHANG);
		}
		if (m_cannothu[ChairID])
			iOpType &= ~(TYPE_CHI | TYPE_PENG | TYPE_GANG | TYPE_HU| TYPE_BU_ZHANG );
		
	}
Next_labe:
	log_debug("chairid[%d] ip[%d]" , ChairID , iOpType );

	return iOpType;
}

static bool fn_chi_peng_gang(int iOpType)
{
	if (iOpType == TYPE_PASS)
		return false;
	return iOpType & (TYPE_CHI | TYPE_GANG | TYPE_PENG | TYPE_BU_ZHANG);
}
void Table_changsha::DoMultiOperate(_uint8 ChairID, int iOpType, _uint8 bCard)
{

	if (fn_chi_peng_gang(iOpType))
	{
		m_louhu[ChairID].Unlock();
	}
	if ((m_bUserOperate[ChairID] & TYPE_BU_ZHANG) && (iOpType & TYPE_BU_ZHANG))
	{
		m_buzhang[ChairID].buzhang.Add(bCard);
		//iOpType = m_bUserOperate[ChairID] & TYPE_GANG;
		iOpType = TYPE_ZHIGANG;
	}
	if (iOpType & TYPE_HU && m_bUserOperate[ChairID] & TYPE_HU && iOpType != TYPE_PASS)
	{
		for (_uint8 c = NextChair(ChairID); c != ChairID; c = NextChair(c))
		{
			if (m_buzhang[c].buzhang.Find(bCard))
				m_buzhang[c].buzhang.RemoveLast();
		}
	}
	if ((m_bUserOperate[ChairID] & TYPE_HU) && (iOpType == TYPE_PASS || (iOpType & (TYPE_PENG | TYPE_CHI))))
	{
		if (m_HuFanNums[ChairID] >= CHANGSHA_DAHU_FAN)
		{
			m_louhu[ChairID].Lock(FW_LOCK_ALL_CARD, FW_LOCK_ANY_POINT);
		}
		else
		{
			m_louhu[ChairID].Lock(FW_LOCK_ALL_CARD, CHANGSHA_DAHU_FAN);
		}
		
	}	
	if (m_HuWeiDa)
	{
		return DoMultiOperate_HuWeiDa(ChairID, iOpType, bCard);
	}
	Table::DoMultiOperate(ChairID, iOpType, bCard);
	
}

void Table_changsha::DoMultiOperate_HuWeiDa(_uint8 ChairID, int iOpType, _uint8 bCard)
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
			log_info(" user uid:%d pass seat:%d, optype: %x " , GetPlayerUid(ChairID), ChairID, m_bUserOperate[ChairID]);
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


void Table_changsha::OnRecvUserOperate(_uint8 ChairID, int iOpType, int bCard)
{
	if (ChairID < 0 || ChairID >= m_GAME_PLAYER)
		return;
	// if (iOpType == TYPE_PASS)
		//m_bUserOperate[ChairID] = 0;  

	if ((m_bUserOperate[ChairID] & TYPE_HU) && (iOpType == TYPE_PASS) && (ChairID != m_bCurChairID) && (ChairID >= 0) && (ChairID < m_GAME_PLAYER) )
	{
		
		m_ningxiag_guozhang[ChairID] = m_HuFanNums[ChairID];
		log_debug("guozhanghu sid[%d] fan[%d] iOpType[%x]  bcard[%x]   m_bUserOperate[%x]", 
			ChairID, m_HuFanNums[ChairID]  , iOpType , bCard , m_bUserOperate[ChairID]);

	}
	Table::OnRecvUserOperate(ChairID, iOpType, bCard);
}

void Table_changsha::DoSimpleOperate(_uint8 ChairID, int iOpType, _uint8 bCard)
{	
	if (m_conf_changsha.kai_king  &&  m_speGang[ChairID] && iOpType == TYPE_PASS)
	{
		log_debug("sid[%d]  grap card !!", ChairID);
		OnUserGrapCard(ChairID); 
		return;
	} 
	if ( fn_chi_peng_gang(iOpType))
	{
		m_ningxiag_guozhang[ChairID] = 0;
		// log_debug("guozhanghu sid[%d] fan[%d] op[%x]  iOpType[%x ]", ChairID,  m_ningxiag_guozhang[ChairID] , m_bUserOperate[ChairID] , iOpType  );
		m_louhu[ChairID].Unlock();
	}

	if ( m_HaidiUser.Size() != 0)
	{
		if (ChairID >= m_GAME_PLAYER)
		{
			log_error(" client package is error uid:%d chairid:%d ", GetPlayerUid(ChairID), ChairID);
		}
		if (iOpType == TYPE_HU)
		{
			Table::DoSimpleOperate(ChairID, iOpType, bCard);
		}
		else if (iOpType == TYPE_HAIDI_YAO)
		{
			m_HaidiPlayer = ChairID;
			Table::OnUserGrapCard(ChairID);
		}			
		else //iOpType == TYPE_PASS
		{
			if (m_HaidiPlayer == INVALID_CHAIRID)
			{
				m_bUserOperate[ChairID] = TYPE_NULL;
				m_bCurChairID = NextChair(ChairID);
				OnUserGrapCard(m_bCurChairID);
			}
			else
			{
				Table::DoSimpleOperate(ChairID, iOpType, bCard);		
			}			
		}	
		return;
	}
	//补张的操作代码

	if ((m_bUserOperate[ChairID] & TYPE_BU_ZHANG) && ( iOpType & TYPE_BU_ZHANG))   
	{
		m_buzhang[ChairID].buzhang.Add(bCard);
		if ((m_curGangPlayerSid != INVALID_CHAIRID) && (m_bCurChairID == ChairID))
		{
			MJ_opinfo ganginfo;
			m_GameLogic.AddCard(&m_HandCard[ChairID], bCard);
			m_GameLogic.GetGangInfo(&m_HandCard[ChairID], ganginfo);
			for (int i = 0; i < ganginfo.Size(); i++)
			{
				if (bCard == ganginfo[i].cbCard)
					iOpType = ganginfo[i].nState;
			}
			//m_GameLogic.ChuPai(&m_HandCard[ChairID], bCard);		
		}
		else if (m_bCurChairID == ChairID)
		{
			MJ_opinfo ganginfo;
			//log_debug("all hand %s" ,   CatHandCard(m_HandCard[ChairID] , ChairID ) );

			m_GameLogic.GetGangInfo(&m_HandCard[ChairID], ganginfo);
			for (int i = 0; i < ganginfo.Size(); i++)
			{
				if (bCard == ganginfo[i].cbCard)
					iOpType = ganginfo[i].nState;
			}
		}		
		else 
		{
			iOpType = TYPE_ZHIGANG;
		}		

	}
	if ((m_bCurChairID != ChairID) && (m_bUserOperate[ChairID] & TYPE_HU) && (iOpType == TYPE_PASS || (iOpType & (TYPE_PENG | TYPE_CHI))))
	{
		if ( m_HuFanNums[ChairID] >= CHANGSHA_DAHU_FAN && m_conf_changsha.kai_king  == false )
		{
			m_louhu[ChairID].Lock(FW_LOCK_ALL_CARD, FW_LOCK_ANY_POINT);
		}
		else if (m_HuFanNums[ChairID] >= 7  && m_conf_changsha.kai_king )
		{
			m_louhu[ChairID].Lock(FW_LOCK_ALL_CARD, FW_LOCK_ANY_POINT );  
		}
		else
		{
			m_louhu[ChairID].Lock(FW_LOCK_ALL_CARD, CHANGSHA_DAHU_FAN);
		}		
	}

	//if (m_gang_get_multicard[ChairID])
	if (m_curGangPlayerSid != INVALID_CHAIRID)
	{
		if (m_curGangPlayerSid != ChairID)
		{
			if (m_HuWeiDa)
			{
				if (iOpType == TYPE_PASS)
				{					
					Table::DoSimpleOperate(m_curGangPlayerSid, m_HuWeiDaOp, m_HuWeiDaCard);
					m_HuWeiDa = false;
					m_HuWeiDaCard = INVALID_MJ_CARD;
					m_HuWeiDaOp = TYPE_NULL;
					if (m_HuWeiDaOp & TYPE_BU_ZHANG)
						GangHouClear();
					return;
				}
			}
			else if (iOpType == TYPE_PASS)
			{
				GangHouClear();
			}
			else if (!(iOpType & TYPE_HU))
			{
				GangHouClear();
			}
		}
		if ((m_curGangPlayerSid == ChairID) && (iOpType == TYPE_PASS))
		{
			if (m_HuFanNums[ChairID] >= CHANGSHA_DAHU_FAN)
			{
				m_louhu[ChairID].Lock(FW_LOCK_ALL_CARD, FW_LOCK_ANY_POINT);
			}
			else
			{
				m_louhu[ChairID].Lock(FW_LOCK_ALL_CARD, CHANGSHA_DAHU_FAN);
			}
			OnUserOutMultiCard(ChairID, m_gang_hou_card.Size());
			return;
		}
		if ((m_curGangPlayerSid == ChairID) && (iOpType & (TYPE_BU_ZHANG | TYPE_GANG)))
		{
			if (Check_HuWeiDa(ChairID, m_gang_hou_card.Size()))
			{
				m_HuWeiDaCard = bCard;
				m_HuWeiDaOp = iOpType;
				return;
			}				
		}
		if ((m_curGangPlayerSid == ChairID) && (iOpType & TYPE_BU_ZHANG))
		{
			GangHouClear();
		}
	}	
	
	Table::DoSimpleOperate(ChairID, iOpType, bCard);	

}


void Table_changsha::MultiOperateOver(_uint8 chairID, int iOpType, _uint8 bCard)
{
	if (m_curGangPlayerSid != INVALID_CHAIRID)
	{
		GangHouClear();
	}
	Table::MultiOperateOver(chairID, iOpType, bCard);
	
}

//////用户抓牌

void Table_changsha::OnUserGrapCard(_uint8 ChairID, bool bGetLast)
{		
	if (ChairID < 0 || ChairID >= m_GAME_PLAYER)
		return;

	m_ningxiag_guozhang[ChairID] = 0;
	if (m_conf_changsha.kai_king && ChairID >= 0 && ChairID < m_GAME_PLAYER )
	{				
		if(m_CardPool.get_cur_pos() >= m_last_card_pos)
		{
			log_debug("last pos [%d]", m_last_card_pos);
			GameEnd();			
			return;
		}		
		
		for (int i = 0; i < m_HandCard[ChairID].FixedCardsLen;i++)
		{
			if (m_speGang[ChairID])
			{
				m_speGang[ChairID] = 0;
				log_debug("cancel  00 ");
				break;
			}
			if (m_buzhang[ChairID].bao_ting == true)
				break; 

			if( m_HandCard[ChairID].FixedCards[i].state & TYPE_PENG &&  (m_HandCard[ChairID].FixedCards[i].CardData == m_zhengwang) )
			{
				if (Table::check_player_can_bao_ting(ChairID , 27 ))
				{  
					proto::game::AckOpNotify    Ack;
					Ack.set_chairid(ChairID);
					Ack.set_optype(TYPE_GANG);
					proto::game::tagGangInfo  *pGang = Ack.add_gang();
					pGang->set_card(m_zhengwang);
					pGang->set_state( TYPE_GANG  );
					unicast(SERVER_OPERATE_NOTIFY,  ChairID , Ack);
					log_debug("5005  %x   sid[%d] ", m_zhengwang  , ChairID  ); 
					m_bUserOperate[ChairID] |= TYPE_GANG;
					m_speGang[ChairID] = 1; 
					return; 
				}				
			}
		} 		
	}
	
	memset( m_stUserOutCardAfterBaoTing, 0, sizeof(m_stUserOutCardAfterBaoTing));

	if (m_CardPool.IsLastCard())  // 如果是最后一张牌。！！！
	{
		log_debug("is last card  !!");
		if (!m_bAddFanFlag[ChairID])
		{
			if (m_HaidiUser.Size() < m_GAME_PLAYER)
			{
				m_HaidiUser.Add(ChairID);
				Send_HaidiCardMsg(ChairID);      //给客户端发送海底牌
				return;
			}
			else
			{   //海底牌4个玩家都不要
				//Table::GetOneCard();
				m_CardPool.GetOneCard();//抛弃最后一张牌
				GameEnd();
				return;
			}
		}
		else if (m_conf_changsha.bao_liu_hai_di_niao)   // 保留海底鸟 配置项。 
		{
			m_bAddFanFlag[ChairID] = false;
			m_curGangPlayerSid = INVALID_CHAIRID;
			m_bCurChairID = NextChair(m_bCurChairID);
			OnUserGrapCard(m_bCurChairID);
			return;
		}
	}	
	if (m_qishou_banpai[ChairID].NotNull())	
		Send_BanpaiOver(ChairID);		 // 摆牌结束了。  摆牌收回。！！！
	
	
	if ( m_curGangPlayerSid != INVALID_CHAIRID)  // 如果是杠了牌，，就开始大筛子。。  摸两张牌 。
	{	
		//杠牌获取多张
		m_gang_shai = m_GameLogic.GetShaizi( CHANGSHA_SHAIZI_COUNT);
		//取两个晒子的和
		_uint8 shai =  MIN(12 ,m_gang_shai.Shai[0] + m_gang_shai.Shai[1] + 2 ) ;   // MIN(m_gang_shai.Shai[0], m_gang_shai.Shai[1]);  
		_uint8 dun = 0;
		if (m_conf_changsha.bao_liu_hai_di_niao && m_CardPool.Count() != 0)   //这个if  else  就是计算剩下多少砘牌
		{			
			_uint8 tmp_count = m_CardPool.Count() - 1;
			dun = tmp_count / 2 + tmp_count % 2;
		}
		else
		{
			dun = m_CardPool.Count() / 2 + m_CardPool.Count() % 2;
		}
		if (shai >= dun)   //打筛子的数超过了 牌的顿数量 。
		{	
			//打筛子不中，下家摸牌
			log_info("uid:%d dashai buzhong shai %d  dun %d, ganghou buzhang %d",
				GetPlayerUid(m_curGangPlayerSid), shai, dun, m_conf_changsha.gang_hou_bu_zhang);

			Send_GangDashaizi(ChairID, m_gang_shai, false);
			GangHouClear();

			//清除碰的标志位
			m_bAddFanFlag[ChairID] = false;

			m_bCurChairID = NextChair(ChairID);
			return OnUserGrapCard(m_bCurChairID);
		}
		//assert(m_CardPool.Count() >= shai * 2);		
		if (m_CardPool.Count() < shai * 2)
		{
			log_error("all[%d] shai[%d]" , m_CardPool.Count() , shai);
		}
		memset(&m_bUserOperate, 0, sizeof(m_bUserOperate));

		m_bCurChairID = ChairID;
		m_Gang_buzhang_res = MIN((m_CardPool.Count() - (shai * 2) - (m_conf_changsha.bao_liu_hai_di_niao ? 1 : 0)),
			m_conf_changsha.gang_hou_bu_zhang);

		//assert(m_Gang_buzhang_res <= m_conf_changsha.gang_hou_bu_zhang);

		Send_GangDashaizi(ChairID, m_gang_shai, true);	

		return;
	}
	
	Table::OnUserGrapCard(ChairID, bGetLast);

}




bool Table_changsha::Check_Listening(HandCards *pHc, _uint8 ChairID, _uint8 bCard, tagListenInfos stListenInfo[14], _uint8 &bListenLen)
{
	bool bTrue = false;
	if (m_conf_changsha.kai_king )
	{
		if (bListenLen >= 14)
		{
			log_error("%s chaochu zuida ting pai shu! ", __FUNCTION__);
			return false;
		}

		HandCards &tempHandCard = *pHc;
		m_GameLogic.ChuPai(&tempHandCard, bCard);
		
		MJ_BUFFER<_uint8, POOL_CARD_TYPE_MAX> _relativity;
		GetAllHandCardRelativity(&tempHandCard, _relativity);
		for (_uint8 i = 0; i < _relativity.Size(); i++)
		{
			if (_relativity[i] == m_zhengwang) continue;
			if (_relativity[i] == m_chunwanga || m_chunwangb == _relativity[i])  continue;

			m_GameLogic.AddCard(&tempHandCard, _relativity[i]);
			MJ_fan_buffer fan_buffer;


			if (Check_Hu(ChairID, tempHandCard, fan_buffer, _relativity[i]) == TYPE_HU)
			{
				if (stListenInfo[bListenLen].bHuLen < 18)
				{
					stListenInfo[bListenLen].bOutCard = bCard;//要打出的牌       
					stListenInfo[bListenLen].HuInfo[stListenInfo[bListenLen].bHuLen].bCard = _relativity[i];//听的牌
					stListenInfo[bListenLen].HuInfo[stListenInfo[bListenLen].bHuLen].bFanShu = Calc_Fan(ChairID, fan_buffer);
					bTrue = true;
					stListenInfo[bListenLen].bHuLen++;
				}
			}
			m_GameLogic.ChuPai(&tempHandCard, _relativity[i]);
		}
		m_GameLogic.AddCard(&tempHandCard, bCard);

	}
	else
	{
		if (bListenLen >= 14)
		{
			log_error("%s chaochu zuida ting pai shu! ", __FUNCTION__);
			return false;
		}
		HandCards &tempHandCard = *pHc;
		m_GameLogic.ChuPai(&tempHandCard, bCard);
		// bool bTrue = false;
		MJ_BUFFER<_uint8, POOL_CARD_TYPE_MAX> _relativity;
		GetAllHandCardRelativity(&tempHandCard, _relativity);


		for (_uint8 i = 0; i < _relativity.Size(); i++)
		{
			//if (_relativity[i] == m_zhengwang) continue;
			// if (_relativity[i] == m_chunwanga || m_chunwangb == _relativity[i])  continue; 
			m_GameLogic.AddCard(&tempHandCard, _relativity[i]);
			MJ_fan_buffer fan_buffer;
			if (Check_Hu(ChairID, tempHandCard, fan_buffer, _relativity[i]) == TYPE_HU)
			{
				if (stListenInfo[bListenLen].bHuLen < 18)
				{
					stListenInfo[bListenLen].bOutCard = bCard;//要打出的牌       
					stListenInfo[bListenLen].HuInfo[stListenInfo[bListenLen].bHuLen].bCard = _relativity[i];//听的牌
					stListenInfo[bListenLen].HuInfo[stListenInfo[bListenLen].bHuLen].bFanShu = Calc_Fan(ChairID, fan_buffer);
					bTrue = true;
					stListenInfo[bListenLen].bHuLen++;
				}
			}
			m_GameLogic.ChuPai(&tempHandCard, _relativity[i]);
		}
		m_GameLogic.AddCard(&tempHandCard, bCard);

	}
	
	if (bTrue)
	{
		if (bListenLen < 14)
		{
			bListenLen++;
		}
	}
	return bTrue;

}



int  Table_changsha::ProtoNingXiangFengDong(int  sid  )
{
	if (m_conf_changsha.kai_king == false)
		return  0; 
	if (sid >= 0 && sid < m_GAME_PLAYER)
	{
		m_fengdong[sid] = 1; 
		proto::game::ningxiang::fengdongack   Ack;
		Ack.set_chairid(sid);
		Ack.set_result(1);
		broadcast(SER_CMD_NINGXIAGN_FENG_DONG, Ack);
		m_bUserOperate[sid] = 0;  
		log_debug("fengdong sid[%d] uid[%d]",sid ,GetPlayerUid(sid)  );
 		int all = 0;
		for (int i = 0; i < m_GAME_PLAYER; i++)
		{
			if (m_fengdong[i])				
			   all++;
		}
		if (all== m_GAME_PLAYER )
		{
			GameEnd();
			return 0;
		}
		
	}
	
	return 0;
}
// 报听后。。 设置标志位。。 

void  Table_changsha::OnRecvBaoTingOp(int sid, int op)
{
	if (sid >= 0 && sid < m_GAME_PLAYER)
	{
		m_nRecvBaoTingPlayerMap[sid] = op;		
		if (op & TYPE_BAO_TING)
		{
			m_buzhang[sid].bao_ting			 = true;
			m_buzhang[sid].is_first_bao_ting = true;
		}
			
		log_info("uid[%d] sid[%d] bao Ting success op[%x]!" , GetPlayerUid(sid) ,sid  , op  );
	}
	int nCurBaoTing = 0;
	for (int i = 0; i < m_GAME_PLAYER; i++)
	{
		log_debug("baoting [%d]" , m_nRecvBaoTingPlayerMap[i]); 
		
		if (( m_nRecvBaoTingPlayerMap[i] & TYPE_BAO_TING) || ( m_nRecvBaoTingPlayerMap[i] & TYPE_PASS))
			nCurBaoTing++;
	}
	log_debug("nCurBaoTing[%d] m_nCanBaoTingCount[%d]", nCurBaoTing, m_nCanBaoTingCount);

	if (nCurBaoTing == m_nCanBaoTingCount)
	{
		m_nGameRunState = GAME_QISHOU_BAOTING_OVER;  	
		proto::game::hunanmj::MsgCmdNofityQiShouHu  pbQishouHu;    // 
		pbQishouHu.set_isqishouhustart(0);
		m_canBankCanOpCard = true;

		unicast(SER_CMD_QISHOU_HU_RESULT, m_bBanker, pbQishouHu);
		log_debug("报听结束1！！！ !!");

		TIMER_AGAIN(m_StartBankOutCardTimer, m_StartBankOutCard_Stamp);
	}

	

}

void  Table_changsha::OnRecvQishouHuBaiPai_Operate( int sid , int  op  )
{	

	if (sid < 0 || sid >= m_GAME_PLAYER)
		return;
	if ( m_CanBaiPaiArr[sid] == 0)   // 不能摆，就不要发来了。 
	{
		log_error("no bai sid[%d] uid[%d]" , sid , GetPlayerUid(sid)); 
		return;
	}

	bool  hasOperateBaiPai = m_RecvBaipaiMap[sid]; 
	log_debug("sid [%d] op[%x] hasOperateBaiPai[%d]", sid, op, hasOperateBaiPai);
	if (hasOperateBaiPai > 0)
		return;

	m_RecvBaipaiMap[sid] = op;
	int   bai_all_count = 0;

	if (op == TYPE_HU  && m_conf_changsha.kai_king ==false ) 
	{
		QishouZhongNiaoSuanFen(  sid , 0, m_qishou_shai);  // 起手设置鸟 为0不加倍 
		Send_BanpaiInfo(sid);

	}
	for (int i = 0; i < GAME_PLAYER; i++)
	{
		if ( (m_RecvBaipaiMap[i]& TYPE_HU) || ( m_RecvBaipaiMap[i] & TYPE_PASS))
			bai_all_count++;
	}
	log_debug("bai_all_count[%d] m_canBaiPaiPlayerCount[%d] banker[%d]   king[%d]", 
		bai_all_count, m_canBaiPaiPlayerCount  , m_bBanker , m_conf_changsha.kai_king  ); 

	if (m_conf_changsha.kai_king )
	{
		int nRecvCount = 0;
		for (int i  = 0; i < m_GAME_PLAYER; i++)
		{
			int  c = (m_bBanker + i) % m_GAME_PLAYER;  						
			log_debug("c [%d] bai[%x]  recv[%x]", c, m_CanBaiPaiArr[c], m_RecvBaipaiMap[c]);
			if ( m_CanBaiPaiArr[c] && m_RecvBaipaiMap[c] == 0)
			{
				log_debug("no op !");
				break;
			}
			if (m_CanBaiPaiArr[c] && m_RecvBaipaiMap[c] != 0)
			{
				nRecvCount++;
				if (nRecvCount >= m_canBaiPaiPlayerCount)
				{
					proto::game::hunanmj::MsgCmdNofityQiShouHu  pbQishouHu;    // 
					pbQishouHu.set_isqishouhustart(0);
					m_canBankCanOpCard = true;
					unicast(SER_CMD_QISHOU_HU_RESULT, m_bBanker, pbQishouHu);
				} 
			}
			if ( m_CanBaiPaiArr[c] && m_RecvBaipaiMap[c] == TYPE_HU)
			{
				m_bIsHu[c] = true;
				for (_uint8 sid = NextChair(c); sid != c; sid = NextChair(sid))
				{
					m_bIsHu[sid] = false;
				}				
				for (int i = 0; i < m_qishou_fan[c].Size(); i++)
					m_oFan[c].Add(m_qishou_fan[c][i]);  
				m_stHuScore[c].stHu.Add(tagHuRecord(c, 0, HU_ZIMO,   7 )); 
				m_bCurChairID = c;  
				GameEnd();
				return;
			}
		}
	}

	if ( bai_all_count== m_canBaiPaiPlayerCount)
	{
		log_debug("into stop baipia sel Timer!! ");
		TIMER_STOP(m_qishou_timer_baiPai_sel);
		proto::game::hunanmj::MsgCmdNofityQiShouHu  pbQishouHu;    // 
		pbQishouHu.set_isqishouhustart(0);
		m_canBankCanOpCard = true;
		unicast(SER_CMD_QISHOU_HU_RESULT, m_bBanker, pbQishouHu);

		// 起手胡。。结束。 
		if (DealBaoTingOp() == 0)   //摆牌完了如果没有报叫。。 启动起手定时器。 
		{
			m_nGameRunState = GAME_QISHOU_BAOTING_OVER;
			TIMER_AGAIN(m_StartBankOutCardTimer, m_StartBankOutCard_Stamp);

			log_debug("start bank op ..");
		}
			

	}
	
}

//用户出牌，，

void Table_changsha::OnUserOutCard(_uint8 ChairID, _uint8 bOutCard)
{
	if (ChairID < 0 || ChairID >= m_GAME_PLAYER)
		return;

	if ( m_qishou_banpai[ChairID].NotNull())	
		Send_BanpaiOver(ChairID);		
		
	m_ningxiag_guozhang[ChairID] = 0;
	memset( m_stUserOutCardAfterBaoTing, 0, sizeof(m_stUserOutCardAfterBaoTing));
	m_speGang[ChairID] = 0;
	

	MJ_fan_buffer oFan;
	int iOpType = Check_Hu(ChairID, m_HandCard[ChairID], oFan , bOutCard );

	if (iOpType & TYPE_HU)
	{
		if (m_HuFanNums[ChairID] >= CHANGSHA_DAHU_FAN)
		{
			m_louhu[ChairID].Lock(FW_LOCK_ALL_CARD, FW_LOCK_ANY_POINT);
		}
		else
		{
			m_louhu[ChairID].Lock(FW_LOCK_ALL_CARD, CHANGSHA_DAHU_FAN);
		}		
	}
	if(m_conf_changsha.kai_king  &&  m_fengdong[ChairID] == 1)
	{
		m_GameLogic.ChuPai(&m_HandCard[ChairID], bOutCard);
		bOutCard = 0; 
		log_debug("fendong chairid sid[%d] uid[%d]" , ChairID  , GetPlayerUid(ChairID));
	}


	Table::OnUserOutCard(ChairID, bOutCard);

	if ( m_conf_changsha.kai_king  &&  (bOutCard == m_chunwangb || bOutCard == m_chunwanga))
	{
		m_cannothu[ChairID] = 1; 
		proto::game::ningxiang::outkingmsg  syn; 
		syn.set_chairid(ChairID); 
		syn.set_card(bOutCard); 
		broadcast( SER_CMD_NINGXIAGN_OUT_KING, syn);
		log_debug("out king sid[%d]  card[%x]" ,  ChairID ,  bOutCard );
	}
	


}



bool Table_changsha::QishouHu()
{
	_uint8 bCard = m_CardPool.CurCard();
	m_GameLogic.AddCard(&m_HandCard[m_bBanker], bCard);    //  给庄家 发第14 张牌 
	for (int i = 0; i < m_GAME_PLAYER; i++)
	{	
		if ( m_conf_changsha.kai_king == false  &&    Hu_Special4Hunan(i, m_qishou_fan[i], m_qishou_banpai[i]))   // 是否可以起手胡牌 
		{
			m_qishou_state = true;
			m_CanBaiPaiArr[i] = 1; 
		}
		else if (m_conf_changsha.kai_king  &&     hu_qishouKaiKing(i  , m_qishou_fan[i]  , m_qishou_banpai[i]))
		{
			m_qishou_state = true;
			m_CanBaiPaiArr[i] = 1; 
		}
	}
	m_GameLogic.ChuPai(&m_HandCard[m_bBanker], bCard);    // 删掉专家的牌 14 张。

	if ( m_qishou_state)
	{
		
		GetFirstHandCard_NoOp(m_bBanker);    // 庄家摸牌。。不操作。	
		m_nGameRunState = 0;  
		log_debug("has qishouhu  state[%d]", m_nGameRunState  ); 

		send_msg_to_client_bai_pai_sel();
		proto::game::hunanmj::MsgCmdNofityQiShouHu  pbQishouHu;    // 
		pbQishouHu.set_isqishouhustart(1);
		m_canBankCanOpCard = false;
		unicast(SER_CMD_QISHOU_HU_RESULT, m_bBanker, pbQishouHu);
		
		return true;

	}
	return false;
}

inline int BankerDouble(_uint8 BankerID, _uint8 ChairID, bool bZhuangXian)
{
	if (bZhuangXian)
	{
		return (ChairID == BankerID) ? 2 : 1;
	}
	return 1;
}

void Table_changsha::QishouZhongNiaoSuanFen(_uint8 ChairID, _uint8 niaonum, Shai_Set shai)
{

	int iBaseScore = 0;	
	int iScore[GAME_PLAYER];
	memset(iScore, 0, sizeof(iScore));

	for (int i = 0; i < m_qishou_fan[ChairID].Size(); i++)
	{
		log_debug("m_qishou_fan[%x]", m_qishou_fan[ChairID][i] );
		iBaseScore += m_ft2fn->GetFan(  m_qishou_fan[ChairID][i]);
	}

	
	log_debug("sid[%d] uid[%d] zhuanixna %d iBaseScore[%d] niaonum[%d] ",
		ChairID ,  GetPlayerUid(ChairID), m_conf.m_bZhuangXian, iBaseScore, niaonum );


	for (_uint8 c = NextChair(ChairID); c != ChairID; c = NextChair(c))
	{
		_uint8 s = iBaseScore * 1;   // BankerDouble(m_bBanker, c, m_conf.m_bZhuangXian);
		iScore[ChairID] += s;
		iScore[c]		-= s;
	}
	
	if (niaonum == 1)
	{	
		//如果是1个鸟，就把筛子点数加起来算
		shai.Shai[0] += shai.Shai[1] + 1;   // 如果只有一个鸟。。 那么。。 shai.shai[1]== 0  
	}	
	
	Calc_Score(m_qishou_score, iScore);   // 起手胡的结算结果。 
	
	_uint8 dis = DisChair(m_bBanker, ChairID);    // 玩家之间的距离 
	for (int i = 0; i < niaonum;i++)
	{		
		if ((shai.Shai[i] % m_GAME_PLAYER) == dis)
		{			
			for (_uint8 c = NextChair(ChairID); c != ChairID; c = NextChair(c))
			{
				m_qishou_score[ChairID]  -= iScore[c];				//减负的
				m_qishou_score[c]        += iScore[c];				//加负的
				log_debug("qishou_scor_charid[%d] qishou_c[%d]  iscore[%d]" , m_qishou_score[ChairID] , m_qishou_score[c] , iScore[c]);
			}						
		}
		else
		{			
			_uint8 c = NextChair(m_bBanker, shai.Shai[i]);
			m_qishou_score[ChairID]   -= iScore[c];						//减负的
			m_qishou_score[c]         += iScore[c];						//加负的		
			log_debug("qishou_scor_charid[%d] qishou_c[%d]  iscore[%d]", m_qishou_score[ChairID], m_qishou_score[c], iScore[c]);
		}		
	}	

	log_info("tid[%d] [%d,%d,%d,%d] " ,  tid, m_qishou_score[0], m_qishou_score[1], m_qishou_score[2], m_qishou_score[3]);

}


//void Table_changsha::Send_QishouDaShaizi(Shai_Set shai)
//{
//	
//	proto::game::hunanmj::tagDaShai stDaShai;
//	if (m_conf.m_bSupportZhuaniao)
//	{		
//		for (int i = 0; i < shai.Num; i++)
//		{
//			stDaShai.add_shai(shai.Shai[i] + 1);
//		}		
//	}	
//	stDaShai.set_chairid(m_bBanker);
//	if (m_conf.m_ZhuaniaoNum == 1)
//	{
//		_uint8 pos = shai.Shai[0] + shai.Shai[1] + 1;
//		stDaShai.add_zhongchairid(NextChair(m_bBanker, pos));
//	}
//	else
//	{
//		for (int i = 0; i < shai.Num; i++)
//		{
//			stDaShai.add_zhongchairid(NextChair(m_bBanker, shai.Shai[i]));
//		}
//	}
//	
//	stDaShai.set_type(1);//抓鸟
//	for (int i = 0; i < m_GAME_PLAYER; i++)
//	{
//		stDaShai.add_score(m_qishou_score[i]);		
//	}
//	stDaShai.set_zhongpai(1);
//
//	broadcast(SERVER_DA_SHAIZI, stDaShai);	
//
//	SaveVideoData(SERVER_DA_SHAIZI, stDaShai);
//}

//通知客户端，，开始摆牌 选择 

void Table_changsha::send_msg_to_client_bai_pai_sel() 
{
	m_canBaiPaiPlayerCount = 0;
	
	for (int i = 0; i < m_GAME_PLAYER; i++)
	{
		if (m_qishou_fan[i].NotNull())
		{
			m_canBaiPaiPlayerCount++;
			proto::game::hunanmj::MsgSerNotifyClientSelBaiPai  pb_bai;    // 
			pb_bai.set_chairid(i);		

			unicast( SER_CMD_BAI_PAI_NOTIFY,  i , pb_bai);
			log_debug("Nofity baipai sid[%d] uid[%d]" , i , GetPlayerUid(i));

		}			
	}
	log_debug("m_canBaiPaiPlayerCount [%d]", m_canBaiPaiPlayerCount ); 	
}

//如果是起手。可以胡牌，那么要吧牌。。亮出来 

void Table_changsha::Send_BanpaiInfo(_uint8 ChairID)   // 妈蛋。。默认。。 是 0xff  . 走if 
{
	proto::game::hunanmj::tagBanPai banpai;
	if (ChairID == INVALID_CHAIRID)
	{		
		for (int i = 0; i < m_GAME_PLAYER; i++)
		{
			if (m_qishou_fan[i].NotNull() && m_RecvBaipaiMap[i] != 0  )
			{
				proto::game::hunanmj::tagBanInfo* info = banpai.add_banpai();
				info->set_chairid(i);
				//assert(m_qishou_banpai[i].NotNull());
				for (int j = 0; j < m_qishou_banpai[i].Size(); j++)
				{
					info->add_showcard(m_qishou_banpai[i][j]);
				}
				//assert(m_qishou_fan[i].NotNull());
				for (int j = 0; j < m_qishou_fan[i].Size(); j++)
				{
					proto::game::hunanmj::AddFanInfo* fan = info->add_addfan();
					fan->set_addtype(m_qishou_fan[i][j]);
					fan->set_addnum(m_ft2fn->GetFan(m_qishou_fan[i][j]));
				}
			}
		}
	}
	else
	{		
		proto::game::hunanmj::tagBanInfo* info = banpai.add_banpai();
		info->set_chairid(ChairID);
		// assert(m_qishou_banpai[ChairID].NotNull());
		if (m_qishou_banpai[ChairID].IsNull() || m_qishou_banpai[ChairID].Size() >= 18)
		{
			log_debug("len baipai_len [%ld]" , m_qishou_banpai[ChairID].Size() );
			return;
		}
		
		for (int j = 0; j < m_qishou_banpai[ChairID].Size(); j++)
		{
			info->add_showcard(m_qishou_banpai[ChairID][j]);
		}
		//assert(m_qishou_fan[ChairID].NotNull());
		if (m_qishou_fan[ChairID].IsNull())
			return;
		for (int j = 0; j < m_qishou_fan[ChairID].Size(); j++)
		{
			proto::game::hunanmj::AddFanInfo* fan = info->add_addfan();
			fan->set_addtype(m_qishou_fan[ChairID][j]);
			fan->set_addnum(m_ft2fn->GetFan(m_qishou_fan[ChairID][j]));
		}
	}
	broadcast(SERVER_QISHOUHU, banpai);
	SaveVideoData(SERVER_QISHOUHU, banpai);


}


void Table_changsha::Send_BanpaiOver(_uint8 ChairID)
{
	proto::game::hunanmj::tagBanPaiEnd banpai;
	banpai.set_chairid(ChairID);

	broadcast(SERVER_BANPAI_END, banpai);

	SaveVideoData(SERVER_BANPAI_END, banpai);	
	m_qishou_banpai[ChairID].Clear();
	for (int i = 0; i < GAME_PLAYER; i++)	
		m_qishou_fan[i].Clear();
	
	memset(m_qishou_score, 0, sizeof(m_qishou_score));
	m_qishou_state = false;

	
}

void Table_changsha::baojiao_sel_Timer_cb(struct ev_loop *loop, struct ev_timer *w, int revents)
{
	if (w->active == 0 )
	{
		log_debug("delete Timer chu fa Againt !!");
		return;
	}
	Table_changsha  *pTable = (Table_changsha*)w->data;
	ev_timer_stop(zjh.loop, &pTable->m_baoJiao_Timer);
	
	ev_timer_again(zjh.loop, &pTable->m_StartBankOutCardTimer);   //  


	// m_StartBankOutCardTimer  起手胡牌   定时器开启

}


void Table_changsha::qishou_baipai_sel_timer_cb(struct ev_loop *loop, struct ev_timer *w, int revents)
{
	if (w->active == 0 )
	{
		log_debug("delete Timer chu fa Againt !!");
		return;
	}
	Table_changsha  *pTable = (Table_changsha*)w->data;	
	ev_timer_stop(zjh.loop, &pTable->m_qishou_timer_baiPai_sel);
	if (pTable->DealBaoTingOp() == 0)   // 没哟报叫。。就继续下一步。 
	{
		//TIMER_AGAIN(m_StartBankOutCardTimer, m_StartBankOutCard_Stamp);
		ev_timer_again(zjh.loop, &pTable->m_StartBankOutCardTimer);
	}
	
	

	//ev_timer_again(zjh.loop, &pTable->m_baoJiao_Timer);
	
}

void Table_changsha::Qishou_Timer_cb(struct ev_loop *loop, struct ev_timer *w, int revents)
{
	if (w->active == 0  )
	{
		log_debug("delete Timer chu fa Againt !!");
		return;
	}
	Table_changsha* table =  (Table_changsha*)w->data;
	ev_timer_stop(zjh.loop, &table->m_StartBankOutCardTimer);


	if (table->m_conf.m_bSupportZhuaniao)
	{
		ev_timer_stop(zjh.loop, &table->m_qishouhu_Timer_0);
		ev_timer_set(&table->m_qishouhu_Timer_0, table->m_qishouhu_Timer_stamp_0, table->m_qishouhu_Timer_stamp_0);
		ev_timer_again(zjh.loop, &table->m_qishouhu_Timer_0);
	}
	else
	{
		Qishou_Timer_cb_0(loop, w, revents);
	}
	
	
}

void Table_changsha::AfterQishouBaoTing()
{		
	if ( m_GAME_PLAYER> 4 || m_GAME_PLAYER< 0)
	{
		log_error("error delete m_GAME_PLAYER  ");
		return;
	}
	Update_Score( m_qishou_score);
	memset( m_qishou_score, 0, sizeof(m_qishou_score));

	//开始判断庄家牌操作
	//_uint8 ChairID = table->m_bBanker;
	_uint8 ChairID =  m_bCurChairID;   // 当前出牌。或者是摸牌的id 
	MJ_major major;
	//*玩家摸牌后判断是否能够杠听胡
	log_debug("开始检查报听 庄家出牌 ！");
	m_bUserOperate[ChairID] = CheckPlaySlefGrpCardCanOperateCode(ChairID, INVALID_MJ_CARD, major, m_HuFanNums[ChairID]);

	for (int i = 0; i < m_GAME_PLAYER; i++)
	{
		proto::game::AckOpNotify stNotify;
		_uint8 CurChairID = i;		
		stNotify.Clear();
		stNotify.set_chairid(ChairID);
		if (ChairID == CurChairID)
		{
			//非打牌玩家
			int iOpType =  m_bUserOperate[ChairID];
			if ( m_bUserOperate[ChairID] & (TYPE_GANG | TYPE_BU_ZHANG))
			{
				m_bUserOperate[ChairID] |= TYPE_BU_ZHANG;
				iOpType =  m_bUserOperate[ChairID];
				MJ_opinfo ganginfo;
				m_GameLogic.GetGangInfo(&m_HandCard[ChairID], ganginfo);
				proto::game::hunanmj::tagBuZhang* bu = stNotify.mutable_bu();
				for (int j = 0; j < ganginfo.Size(); j++)
				{
					proto::game::hunanmj::tagBuinfo* buinfo = bu->add_bu();
					buinfo->set_card(ganginfo[j].cbCard);
					buinfo->set_state(TYPE_BU_ZHANG);
					if ( IsListening(ChairID, ganginfo[j].cbCard))
					{
						proto::game::tagGangInfo* gang = stNotify.add_gang();
						gang->set_card(ganginfo[j].cbCard);
						gang->set_state(ganginfo[j].nState);

					}
				}
				if (stNotify.gang_size() == 0)
				{
					iOpType &= ~(TYPE_GANG);
				}
				else
				{
					iOpType &= ~(TYPE_GANG);
					for (int i = 0; i < stNotify.gang_size(); i++)
					{
						proto::game::tagGangInfo* gang = stNotify.mutable_gang(i);
						iOpType |= gang->state();
					}
				}
			}
			stNotify.set_card( m_bCurCard);
			stNotify.set_optype(iOpType);
			log_debug("send to bank op[%d]  grap_card[%x]", iOpType,  m_bCurCard);
			m_bUserOperate[ChairID] = iOpType;
		}
		unicast(SERVER_OPERATE_NOTIFY, i, stNotify);
	}
	m_qishou_state = false;
}
void Table_changsha::Qishou_Timer_cb_0(struct ev_loop *loop, struct ev_timer *w, int revents)
{
	if (w->active == 0  )
	{
		Table_changsha* table = (Table_changsha*)w->data;
		table->m_qishou_state = false;
		log_error("delete Timer chu fa Againt !!");
		//return;
	}
	Table_changsha* table = (Table_changsha*)w->data;

	ev_timer_stop(zjh.loop, &table->m_qishouhu_Timer_0);
	if ( table->m_GAME_PLAYER> 4 || table->m_GAME_PLAYER< 0)
	{
		log_error("error delete m_GAME_PLAYER  ");
		return;
	}
	table->Update_Score(table->m_qishou_score);
	memset(table->m_qishou_score, 0, sizeof(table->m_qishou_score));

	//开始判断庄家牌操作
	//_uint8 ChairID = table->m_bBanker;
	_uint8 ChairID = table->m_bCurChairID;   // 当前出牌。或者是摸牌的id 
	MJ_major major;
	//*玩家摸牌后判断是否能够杠听胡
	log_debug("开始检查报听 庄家出牌 ！");
	table->m_bUserOperate[ChairID] = table->CheckPlaySlefGrpCardCanOperateCode(ChairID, INVALID_MJ_CARD, major, table->m_HuFanNums[ChairID]);

	for (int i = 0; i < table->m_GAME_PLAYER; i++)
	{
		proto::game::AckOpNotify stNotify;
		_uint8 CurChairID = i;
		//table->Proto_Game_OpNotify(ChairID, i, table->m_bCurCard, stNotify);

		stNotify.Clear();
		stNotify.set_chairid(ChairID);

		if (ChairID == CurChairID)
		{	
			//非打牌玩家
			int iOpType = table->m_bUserOperate[ChairID];
			if (table->m_bUserOperate[ChairID] & (TYPE_GANG | TYPE_BU_ZHANG))
			{
				table->m_bUserOperate[ChairID] |= TYPE_BU_ZHANG;
				iOpType = table->m_bUserOperate[ChairID];

				MJ_opinfo ganginfo;
				table->m_GameLogic.GetGangInfo(&table->m_HandCard[ChairID], ganginfo);

				proto::game::hunanmj::tagBuZhang* bu = stNotify.mutable_bu();
				for (int j = 0; j < ganginfo.Size(); j++)
				{
					proto::game::hunanmj::tagBuinfo* buinfo = bu->add_bu();					
					buinfo->set_card(ganginfo[j].cbCard);
					buinfo->set_state(TYPE_BU_ZHANG);
					if (table->IsListening(ChairID, ganginfo[j].cbCard))
					{
						proto::game::tagGangInfo* gang = stNotify.add_gang();
						gang->set_card(ganginfo[j].cbCard);
						gang->set_state(ganginfo[j].nState);

					}
				}				
				if (stNotify.gang_size() == 0)
				{
					iOpType &= ~(TYPE_GANG);
				}
				else
				{
					iOpType &= ~(TYPE_GANG);
					for (int i = 0; i < stNotify.gang_size(); i++)
					{
						proto::game::tagGangInfo* gang = stNotify.mutable_gang(i);
						iOpType |= gang->state();
					}					
				}
			}
			stNotify.set_card(table->m_bCurCard);
			stNotify.set_optype(iOpType);
			log_debug("send to bank op[%d]  grap_card[%x]"  , iOpType , table->m_bCurCard );

			table->m_bUserOperate[ChairID] = iOpType;
		}

		table->unicast( SERVER_OPERATE_NOTIFY, i, stNotify);
	}
	table->m_qishou_state = false;

}

void Table_changsha::Proto_GameScene(_uint8 ChairID, Player* player, proto::game::AckGameScene& stGameScene) 
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
			info->add_jinzhihuanpai((m_buzhang[i].bao_ting || m_HaidiPlayer == ChairID) ? 1 : 0);
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
		
		tagBanPai* banpai = info->mutable_banpai();    //摆牌的信息 。
		for (int i = 0; i < m_GAME_PLAYER; i++)
		{
			log_info("m_RecvBaipaiMap i[%d] [%d]", i, m_RecvBaipaiMap[i]); 

			if ( m_qishou_banpai[i].NotNull() && m_RecvBaipaiMap[i] == TYPE_HU  ) //  没有摆过的。就摆出来
			{
				tagBanInfo* baninfo = banpai->add_banpai();
				baninfo->set_chairid(i);
				for (int j = 0; j < m_qishou_banpai[i].Size(); j++)
				{
					baninfo->add_showcard(m_qishou_banpai[i][j]);
				}
				for (int j = 0; j < m_qishou_fan[i].Size(); j++)
				{
					AddFanInfo* fan = baninfo->add_addfan();
					fan->set_addtype(m_qishou_fan[i][j]);
					fan->set_addnum(1);
				}

			}
		}
	}
	if (m_conf_changsha.kai_king)
	{
		proto::game::ningxiang::reconnect_info   *p = stGameScene.mutable_re_info(); 
		p->set_zhengwang(m_zhengwang);
		p->add_chun_wang(m_chunwanga);
		p->add_chun_wang(m_chunwangb); 
		for (int i = 0; i < m_GAME_PLAYER; i++)
		{
			if (m_fengdong[i] == 0  ) continue; 
			proto::game::ningxiang::feng_dong  *pfen = p->add_feng_list(); 
			pfen->set_chairid(i);
			pfen->set_feng(1);
		}
		p->set_out_king( m_cannothu[ChairID]);   //   
		if (m_speGang[ChairID])
		{
			proto::game::tagGangInfo *pInfo = stGameScene.add_ganginfo();  
			pInfo->set_card(m_zhengwang);
			pInfo->set_state(TYPE_GANG);
		}		
	}
	
}
void Table_changsha::Proto_GameScene_Operate(_uint8 ChairID, Player* player, proto::game::AckGameScene& stGameScene)
{
	Table::Proto_GameScene_Operate(ChairID, player, stGameScene);
	if (m_bMultiOpUserNum != 0 && m_bMultiUserDoneOp[ChairID] != TYPE_NULL)
	{
		log_error("%d %d %d", m_bMultiOpUserNum, m_bMultiUserDoneOp[ChairID]  , ChairID );
		return;
	}
	if (m_nCanBaoTingArr[ChairID] != 0 && m_nRecvBaoTingPlayerMap[ChairID] == 0)
	{
		stGameScene.set_optype(m_bUserOperate[ChairID] | TYPE_BAO_TING );
		log_debug("baoting %x" ,TYPE_BAO_TING);
	}
	else
	{
		if ( m_nRecvBaoTingPlayerMap[ChairID] & TYPE_BAO_TING || (m_nRecvBaoTingPlayerMap[ChairID] & TYPE_PASS)  )   // 报听修改。  
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
						log_info(" player uid[%d] chairID[%d]  gangplayer[%d] card[%x] optype[%x]",
							GetPlayerUid(ChairID), ChairID, m_curGangPlayerSid,
							m_gang_hou_card[i], m_gang_hou_op[ChairID][i]);
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
		if (m_bUserOperate[ChairID] & TYPE_BU_ZHANG)
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
					buinfo->set_state(TYPE_BU_ZHANG);

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
				buinfo->set_state(TYPE_BU_ZHANG);

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
void Table_changsha::Proto_GameScene_lookon(_uint8 ChairID, Player* player, proto::game::AckGameScene& stGameScene)
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

void Table_changsha::OnUserGetMultiCard(_uint8 ChairID, _uint8 iGetCount, int iOpTypeFilter)
{
	if (m_bDeleteTable)
	{
		log_error("delete mul card fun!!");
		return;
	}	
	//assert(m_bUserOperate[ChairID] == TYPE_NULL);
	m_bUserOperate[ChairID] = TYPE_NULL;

	proto::game::hunanmj::tagGang_Zhuapai zhuapai;
	zhuapai.set_chairid(ChairID);	

	for (int i = 0; i < iGetCount; i++)
	{
		_uint8 bGetCard = m_CardPool.GetLast();
		m_gang_hou_card.Add(bGetCard);
		zhuapai.add_card(bGetCard);
	}	
	m_bGetCardCount[ChairID] += iGetCount;

	m_bOutCardChairID = ChairID;	
	m_bCurCard = m_gang_hou_card.Last();
	m_bCurOutCard = m_gang_hou_card.Last();

	zhuapai.set_leftcardnum(m_CardPool.Count());

	for (int i = 0; i < m_gang_hou_card.Size(); i++)
	{
		m_stUserOutCard[ChairID].Add(m_gang_hou_card[i]);				
		m_stUserOutCardAfterBaoTing[ChairID].Add(m_gang_hou_card[i]); 
		log_debug("ganghou chupaiuid[%d]  card[%x] ", GetPlayerUid(ChairID) , m_gang_hou_card[i]);

	}

	log_info(" uid:%d chairid:%d getcardcount:%d ",   GetPlayerUid(ChairID), ChairID, iGetCount);
	for (_uint8 c = NextChair(ChairID); c != ChairID; c = NextChair(c))	
		unicast(SERVER_GANG_MO, c, zhuapai);
	

	for (int i = 0; i < iGetCount; i++)
	{		
		MJ_major major;
		int iFanNums = 0;
		
		int iOpType = CheckPlaySlefGrpCardCanOperateCode(ChairID, m_gang_hou_card[i], major, iFanNums);  // 摸牌操作判断 ！！
		m_HuFanNums[ChairID] += iFanNums;
		
		if (iOpTypeFilter == TYPE_HU)
		{
			iOpType &= iOpTypeFilter;
			m_bUserOperate[ChairID] &= iOpTypeFilter;
		}
		proto::game::hunanmj::GangAckOpNotify* pNotify = zhuapai.add_opnotify();		
		if (iOpType & TYPE_GANG)
		{
			iOpType |= TYPE_BU_ZHANG;			

			MJ_opinfo stGang;			
			m_GameLogic.AddCard(&m_HandCard[ChairID], m_gang_hou_card[i]);
			m_GameLogic.GetGangInfo(&m_HandCard[ChairID], stGang);			

			proto::game::hunanmj::tagBuZhang* bu = pNotify->mutable_bu();			
			for (int j = 0; j < stGang.Size(); j ++)
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
			if (pNotify->gang_size() == 0)
			{
				iOpType &= ~(TYPE_GANG | TYPE_BU_ZHANG);				
			}
			
		}		

		m_gang_hou_op[ChairID].Add(iOpType);
		m_bUserOperate[ChairID] |= iOpType;

		pNotify->set_chairid(ChairID);
		pNotify->set_card(m_gang_hou_card[i]);
		pNotify->set_optype(iOpType);		
		log_info("uid[%d] chairid[%d] card[%x] , iOpType:[%x] ",  GetPlayerUid(ChairID), ChairID, m_gang_hou_card[i], iOpType);

	}

	unicast(SERVER_GANG_MO, ChairID, zhuapai);	
	SaveVideoData(SERVER_GANG_MO, zhuapai);
}

//////////杠牌的时候要打筛子

void Table_changsha::Send_GangDashaizi(_uint8 ChairID, Shai_Set shai, bool zhong)
{
	proto::game::hunanmj::tagDaShai stDaShai;
	for (int i = 0; i < shai.Num; i++)
	{
		stDaShai.add_shai(shai.Shai[i] + 1);
	}
	stDaShai.set_chairid(ChairID);
	stDaShai.set_type(0);
	stDaShai.set_zhongpai(zhong ? 1 : 0);

	broadcast(SERVER_DA_SHAIZI, stDaShai);

	SaveVideoData(SERVER_DA_SHAIZI, stDaShai);

	if (zhong)
	{
		TIMER_AGAIN(m_gang_dashaizi_Timer, m_gang_dashaizi_Timer_stamp);
	}
	
}

////杠后打筛子 消息 

void Table_changsha::Gang_Dashaizi_Timer_cb(struct ev_loop *loop, struct ev_timer *w, int revents)
{
	if (w->active == 0)
	{
		log_debug("delete Timer chu fa Againt !!");
		return;
	}
	Table_changsha* table = (Table_changsha*)w->data;
	TIMER_STOP(table->m_gang_dashaizi_Timer);

	if (table->m_conf_changsha.gang_hou_zhi_neng_hu0 )
	{
		table->OnUserGetMultiCard(table->m_bCurChairID, table->m_Gang_buzhang_res, TYPE_HU);
	}
	else
	{
		table->OnUserGetMultiCard(table->m_bCurChairID, table->m_Gang_buzhang_res);
	}	

	if (table->m_bUserOperate[table->m_bCurChairID] == TYPE_NULL)
	{
		if (table->m_conf_changsha.gang_hou_zhi_neng_hu1)
		{
			table->OnUserOutMultiCard(table->m_bCurChairID, table->m_Gang_buzhang_res, TYPE_HU);
		}
		else
		{
			table->OnUserOutMultiCard(table->m_bCurChairID, table->m_Gang_buzhang_res);
		}
	}
}



void Table_changsha::OnUserOutMultiCard(_uint8 ChairID, _uint8 iGetCount, int iOpTypeFilter)
{	
	if (m_bDeleteTable)
	{
		return;
	}
	log_info(" user out multi card uid:%d chairID:%d ",  GetPlayerUid(ChairID), ChairID);
	TIMER_STOP(m_Outcard_Timer);

	m_bCurChairID = ChairID;
	m_bOutCardChairID = ChairID;
	m_bCurCard = m_gang_hou_card.Last();
	m_bCurOutCard = m_gang_hou_card.Last();
	m_bOverTimeState[ChairID] = 0;
		
	m_bMultiOpUserNum = 0;

	proto::game::hunanmj::tagGang_Zhuapai zhuapai;
	zhuapai.set_chairid(ChairID);
	zhuapai.set_leftcardnum(m_CardPool.Count());

	for (int i = 0; i < iGetCount; i++)
	{
		zhuapai.add_card(m_gang_hou_card[i]);
	}	
	
	for (_uint8 c = NextChair(ChairID); c != ChairID; c = NextChair(c))
	{		
		Proto_UserOutMultiCard(c, zhuapai, iOpTypeFilter);
		if (m_bUserOperate[c] != TYPE_NULL)
		{
			if (m_bUserOperate[c] & TYPE_HU)
			{
				m_bMultiOpHuNum++;
			}
			m_bMultiOpUserNum++;
		
			unicast(SERVER_GANG_CHU, c, zhuapai);

			set_operator_timer(c);
		}
	}
	if ((m_bMultiOpUserNum == 0 && iOpTypeFilter != TYPE_HU) || 
		(m_bMultiOpUserNum == 0 && m_conf_changsha.gang_hou_zhi_neng_hu1))
	{
		GangHouClear();

		m_bCurChairID = NextChair(ChairID);
		memset(m_bAddFanFlag, 0, sizeof(m_bAddFanFlag));
		OnUserGrapCard(m_bCurChairID);
	}	

}


void Table_changsha::Proto_UserOutMultiCard(_uint8 ChairID, proto::game::hunanmj::tagGang_Zhuapai& zhuapai, int iOpTypeFilter)
{
	zhuapai.clear_opnotify();

	int iFanNum = 0;
	int iOpType = TYPE_NULL;
	for (int i = 0; i < m_gang_hou_card.Size(); i++)
	{
		
		MJ_major major;
		iOpType = CheckOutCardOtherPlayerCanOpCode(ChairID, m_gang_hou_card[i], major, iFanNum);		
		iOpType &= iOpTypeFilter;
		if (iOpType != TYPE_NULL)
		{
			proto::game::hunanmj::GangAckOpNotify* pNotify = zhuapai.add_opnotify();
			pNotify->set_chairid(ChairID);			
			pNotify->set_card(m_gang_hou_card[i]);

			if (iOpType & TYPE_GANG)
			{
				iOpType |= TYPE_BU_ZHANG;				

				proto::game::hunanmj::tagBuZhang* bu = pNotify->mutable_bu();
				proto::game::hunanmj::tagBuinfo* buinfo = bu->add_bu();
				buinfo->set_card(m_gang_hou_card[i]);
				buinfo->set_state(TYPE_BU_ZHANG);
				if ( IsListening(ChairID, m_gang_hou_card[i]))
				{
					proto::game::hunanmj::tagGangInfo* ganginfo = pNotify->add_gang();
					ganginfo->set_card(m_gang_hou_card[i]);
					ganginfo->set_state(TYPE_ZHIGANG);

				}				
				if (pNotify->gang_size() == 0)
				{
					iOpType &= ~TYPE_GANG;
				}
			}
			pNotify->set_optype(iOpType);
		}	

		m_bUserOperate[ChairID] |= iOpType;
		m_gang_hou_op[ChairID].Add(iOpType);	

		log_info("uid[%d]  chairid[%d] card[%x]  iOpType[%x] ", 
			GetPlayerUid(ChairID), ChairID, m_gang_hou_card[i], iOpType);
	}
		
}

void Table_changsha::GangHouClear()
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

void Table_changsha::Calc_FengDing(_uint8 ChairID, int iScore[GAME_PLAYER])
{
	if (m_bIsHu[ChairID])
	{
		iScore[ChairID] = 0;
		for (_uint8 c = NextChair(ChairID); c != ChairID; c = NextChair(c))
		{
			iScore[c] = MAX(iScore[c], m_conf_changsha.feng_ding);
			iScore[ChairID] -= iScore[c];
		}		
		log_info(" tid:%d, [%d,%d,%d,%d] ",  tid, iScore[0], iScore[1], iScore[2], iScore[3]);
	}
}

void Table_changsha::save_video_operate(int iOpType, _uint8 chairID, _uint8 paschairID, _uint8 cbCard, int score[GAME_PLAYER])
{	
	if (iOpType & TYPE_GANG)
	{
		if (m_buzhang[chairID].buzhang.Find(cbCard))
		{
			iOpType = TYPE_BU_ZHANG;
		}
	}
	Table::save_video_operate(iOpType, chairID, paschairID, cbCard, score);
}

void Table_changsha::SetRobGangHu_Flag(_uint8 ChairID, int iOpType, proto::game::AckBcOpResult& stOpresult)
{
	if (Is_HuOperator(iOpType) && m_bRobWanGang && m_bAddFanFlag[m_WangGangPlayer])		
	{
		if (!m_buzhang[m_WangGangPlayer].buzhang.Find(m_RobWanGangCard))
		{		
			log_info(" changsha set qiangganghu true ");
			stOpresult.set_isqiangganghu(1);
		}		
	}	
}
bool Table_changsha::Check_HuWeiDa(_uint8 ChairID, _uint8 iGetCount)
{	
	if (!m_conf_changsha.hu_wei_da)
		return false;
	OnUserOutMultiCard(ChairID, iGetCount, TYPE_HU);
	if (m_bMultiOpUserNum != 0)
	{
		m_HuWeiDa = true;
		return true;
	}
	return false;
}

bool Table_changsha::IsListening(_uint8 ChairID, _uint8 bRemoveCard)
{
	HandCards tempHandCard = m_HandCard[ChairID];
	if (bRemoveCard != INVALID_MJ_CARD)
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

	for (_uint8 i = 0; i < _relativity.Size(); ++i)
	{
		m_GameLogic.AddCard(&tempHandCard, _relativity[i]);
		MJ_fan_buffer fan;
		if ( Check_Hu(ChairID, tempHandCard, fan , _relativity[i] ) == TYPE_HU)
		{
			log_info(" uid:%d chairdid:%d Remove:%x ", GetPlayerUid(ChairID), ChairID, bRemoveCard);
			return true;
		}
		m_GameLogic.ChuPai(&tempHandCard, _relativity[i]);
	}
	return false;
}



int Table_changsha::hu_if_remove_one_can_card(_uint8  ChairID)
{
	HandCards* pHc = &m_HandCard[ChairID];
	vector<int> all_card_vec;
	int remove_pos = 0;
	while (remove_pos < pHc->ChangeableCardsLen)
	{
		all_card_vec.clear();
		for (int i = 0; i < pHc->ChangeableCardsLen; i++)
		{
			if (i != remove_pos)
				all_card_vec.push_back(pHc->ChangeableCards[i]);
		}
		log_debug("all_card  [%lu]", all_card_vec.size());
		int ret =  hu_if_add(all_card_vec , 27 );
		remove_pos++;
		if (ret > 0)
		{
			return 1;
		}
	}

	return 0;

}


 // 听牌以后，能不能杠的问题，，  


bool Table_changsha::IsCanGangAfterTing(_uint8  sid, MJ_major & major, _uint8  add_card)
{
	bool can = true;
	if (sid < 0 || sid >= m_GAME_PLAYER || add_card< 0 || add_card> 0x30  )
		return false;  
	log_debug(" pengang[%d]  ganginfo[%d]  ", major.m_GetPengGang.Size(), major.m_GetGangInfo.Size());

	if (Table::check_player_can_bao_ting(sid, 27) == false)
		return  true;

	if (m_bCurChairID == sid)
	{
		if( !m_conf_changsha.kai_king  &&  major.m_GetGangInfo.Size() == 1)
		{
			_uint8  cur_card = major.m_GetGangInfo[0].cbCard;
			if (cur_card < 0 || cur_card >= 0x30)
				return false; 
			HandCards* pHc = &m_HandCard[sid];
			log_debug("len [%d]", pHc->ChangeableCardsLen);
			int remove_count = 0;
			for (int i = 0; i < pHc->ChangeableCardsLen;)
			{
				if (pHc->ChangeableCards[i] == cur_card)
				{
					if (m_GameLogic.ChuPai(pHc, cur_card))
					{
						remove_count++;
						if (remove_count>=  3 )
							break;
					}					
				}
				else
					i++;
			}
			vector<int> AllHandCard;
			for (size_t i = 0; i < pHc->ChangeableCardsLen; i++)
				AllHandCard.push_back(pHc->ChangeableCards[i]);
			if (pHc->ChangeableCardsLen %3 == 1 ) // &&  hu_if_add(AllHandCard, 27 ) ==false ) 
			{
				int ret = hu_if_add(AllHandCard, 27);
				if (ret> 0)
				{
					ret = jiang_after_hu();
					if (ret > 0 && (get_face(ret) == 5 || get_face(ret) == 2 || get_face(ret) == 8))
						ret = 1;
				}
				if (ret <= 0)
					can = false; 
			}			
			for (int i = 0; i < remove_count; i++)
			{
				m_GameLogic.AddCard(pHc, cur_card);
			}
			if (can == false &&   m_GameLogic.is_jiang_jiang_hu(pHc))
			{
				can = true;
				log_debug("can hu jiangjiahu ");
			}
			log_debug("cur_card[%x] remove_count[%d] can[%d]  len [%d]", cur_card, remove_count, can , pHc->ChangeableCardsLen );
		}
		else if( m_conf_changsha.kai_king  &&  major.m_GetGangInfo.Size() == 1)
		{
			_uint8  cur_card = major.m_GetGangInfo[0].cbCard;
			if (cur_card < 0 || cur_card >= 0x30)
				return false;
			HandCards* pHc = &m_HandCard[sid];
			log_debug("len [%d]", pHc->ChangeableCardsLen);
			int remove_count = 0;
			for (int i = 0; i < pHc->ChangeableCardsLen;)
			{
				if (pHc->ChangeableCards[i] == cur_card)
				{
					if (m_GameLogic.ChuPai(pHc, cur_card))
					{
						remove_count++;
						if (remove_count>= 3 )
							break;
					}					
				}
				else
					i++;
			}
			if( Table::check_player_can_bao_ting(sid, 27) == false )
			{
				can = false;
			}
			
			for (int i = 0; i < remove_count; i++)
			{
				m_GameLogic.AddCard(pHc, cur_card);
			}
			
			log_debug("cur_card[%x] remove_count[%d] can[%d]  len [%d]", cur_card, remove_count, can, pHc->ChangeableCardsLen);
		}

	}
	else
	{
		if ( major.m_GetPengGang.Size() >= 1 && !m_conf_changsha.kai_king )
		{
			int gang_count = 0;
			for (int i = 0; i < major.m_GetPengGang.Size(); i++)
			{
				if (major.m_GetPengGang[i].nState == TYPE_ZHIGANG)
				{
					gang_count++;
				}
			}
			if (gang_count == 1)
			{
				for (int i = 0; i < major.m_GetPengGang.Size(); i++)
				{
					if (major.m_GetPengGang[i].nState  & TYPE_GANG)
					{
						_uint8  cur_card = major.m_GetPengGang[i].cbCard;
						if (cur_card < 0 || cur_card >= 0x30)
							return false;

						HandCards* pHc = &m_HandCard[sid];
						log_debug("len [%d]", pHc->ChangeableCardsLen);
						int remove_count = 0;
						for (int ct = 0; ct < pHc->ChangeableCardsLen;)
						{
							if (pHc->ChangeableCards[ct] == cur_card)
							{
								if (m_GameLogic.ChuPai(pHc, cur_card))
								{
									remove_count++;
									if (remove_count>=3)
										break;
								} 								
							}
							else{
								ct++;
							}
						}
						log_debug("ChangeableCardsLen [%d]", pHc->ChangeableCardsLen);
						vector<int> AllHandCard;
						for (size_t i = 0; i < pHc->ChangeableCardsLen; i++)
							AllHandCard.push_back(pHc->ChangeableCards[i]); 
						
						
						if (pHc->ChangeableCardsLen %3 == 1 ) // &&  ! hu_if_add(AllHandCard ,27)  )
						{
							int ret = hu_if_add(AllHandCard, 27);
							if (ret> 0)
							{
								ret = jiang_after_hu();
								if (ret > 0 && (get_face(ret) == 5 || get_face(ret) == 2 || get_face(ret) == 8))
									ret = 1;
							}
							if (ret <= 0 )
								can = false;
						} 					
						for (int i = 0; i < remove_count; i++)
						{
							m_GameLogic.AddCard(pHc, cur_card);
						}
						if (can == false && m_GameLogic.is_jiang_jiang_hu(pHc))
						{
							can = true;
							log_debug("can hu jiangjiahu ");
						}
						log_debug("cur_card[%x] remove_count[%d] can[%d]  len[%d]", cur_card, remove_count, can , pHc->ChangeableCardsLen );
						break;
					}
				}
			}
		}
		else if (major.m_GetPengGang.Size() >= 1 && m_conf_changsha.kai_king )
		{
			int gang_count = 0;
			for (int i = 0; i < major.m_GetPengGang.Size(); i++)
			{
				if (major.m_GetPengGang[i].nState == TYPE_ZHIGANG)
				{
					gang_count++;
				}
			}
			if (gang_count == 1)
			{
				for (int i = 0; i < major.m_GetPengGang.Size(); i++)
				{
					if (major.m_GetPengGang[i].nState  & TYPE_GANG)
					{
						_uint8  cur_card = major.m_GetPengGang[i].cbCard;
						if (cur_card < 0 || cur_card >= 0x30)
							return false;

						HandCards* pHc = &m_HandCard[sid];
						log_debug("len [%d]", pHc->ChangeableCardsLen);
						int remove_count = 0;
						for (int ct = 0; ct < pHc->ChangeableCardsLen;)
						{
							if (pHc->ChangeableCards[ct] == cur_card)
							{
								if (m_GameLogic.ChuPai(pHc, cur_card))
								{
									remove_count++;
									if (remove_count>=3 )
										break;
								}								
							}
							else{
								ct++;
							}
						}
						
						log_debug("ChangeableCardsLen [%d]", pHc->ChangeableCardsLen);
						if (Table::check_player_can_bao_ting(sid, 27) == false)
						{
							can = false;
						}
						
						for (int i = 0; i < remove_count; i++)
						{
							m_GameLogic.AddCard(pHc, cur_card);
						}						
						log_debug("cur_card[%x] remove_count[%d] can[%d]  len[%d]", cur_card, remove_count, can, pHc->ChangeableCardsLen);
						break;
					}
				}
			}
		}
	}


	return can;



}
