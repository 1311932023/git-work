#include "table_changsha_zhuanzhuan.h"
#include "../proto/proto.h"
#include "../zjh.h"
#include "../common/log.h"
#include "../libnormalmahjong/mj_common2.h"
#include "../libnormalmahjong/game_logic.h"
#include "../table_helper.h"
#include "../trustee/CTrustee.h"

using namespace proto::game::hunanmj;

extern ZJH zjh;
extern Log log;

static CS_zhuanzhuan_Fantype_to_FanNum g_zhuanzhuan_ft2fn;

Table_cs_zhuanzhuan::Table_cs_zhuanzhuan()
{
	m_ft2fn = &g_zhuanzhuan_ft2fn;	
	m_bGangCalcImme = true;

	m_conf.m_bNoSupport_Quanqiuren = true;


	/*m_trustee.reset(new CTrustee(this));

	if (!zjh.conf["robot"].empty())
	{

		CTrustee* ctrustee = (CTrustee*)m_trustee.get();
		ctrustee->m_GuiCard.push_back(MJ_HONG_ZHONG);
	}*/
	
}
Table_cs_zhuanzhuan::~Table_cs_zhuanzhuan()
{

}

void Table_cs_zhuanzhuan::ConfInit_Remote(const Json::Value& val)
{
	Table::ConfInit_Remote(val);
	
	m_conf.m_bSupportChi = false;

	m_conf.m_bSupportBuyHorse = false;
	
	m_conf.m_bHu13yao = false;

	m_conf.m_bZimoHu = false;
	m_conf.m_bYiPaoDuoXiang = true;
	m_conf.m_bQiangGangHu = true;

	m_conf.m_bZhuangXian = false;
	m_conf.m_bHu7dui = false;
		
	m_CardPool.Set(CARD_POOL_NO_WIND_DRAGON_FLOWER);	

	if (json_check_play_type_id(val))
	{
		MJ_SUB_ID SubID;
		
		int iCount = json_get_play_type_id(val, 2060, SubID);
		if (iCount != 0)
		{
			if (SubID[0] == 2060001)
			{
				m_conf.m_bZimoHu = false;
				m_conf.m_bYiPaoDuoXiang = true;
				m_conf.m_bQiangGangHu = true;
			}
			else if (SubID[0] == 2060002)
			{
				m_conf.m_bZimoHu = true;
				m_conf.m_bYiPaoDuoXiang = true;
				m_conf.m_bQiangGangHu = false;
			}
			m_room_config += SubID;
		}
		iCount = json_get_play_type_id(val, 2070, SubID);
		if (iCount != 0)
		{
			if (SubID.Find(2070001))
			{
				m_conf.m_bZhuangXian = true;
			}
			if (SubID.Find(2070002))
			{
				m_conf.m_bHu7dui = true;
			}
			m_room_config += SubID;
		}
		iCount = json_get_play_type_id(val, 2080, SubID);
		if (iCount != 0)
		{
			m_conf.m_bSupportZhuaniao = true;
			if (SubID[0] == 2080001)
			{
				m_conf.m_ZhuaniaoNum = 2;
			}
			else if (SubID[0] == 2080002)
			{
				m_conf.m_ZhuaniaoNum = 4;
			}
			else if (SubID[0] == 2080003)
			{
				m_conf.m_ZhuaniaoNum = 6;
			}
			else if (SubID[0] == 2080004)
			{
				m_conf.m_bSupportZhuaniao = false;
				m_conf.m_ZhuaniaoNum = 0;
			}
			m_room_config += SubID;
		}
		json_get_play_type_id(val, 2441, SubID);
		if (SubID.Find(2441001))
		{	
			m_GAME_PLAYER = 2;
			m_CardPool.Set(CARD_POOL_2_PEOPLE);			
			m_room_config.Add(SubID[0]);
		}
		json_get_play_type_id(val, 2442, SubID);
		if (SubID.Find(2442001))
		{
			m_GAME_PLAYER = 3;
			m_CardPool.Set(CARD_POOL_3_PEOPLE);			
			m_room_config.Add(SubID[0]);
		}

		json_get_play_type_id(val, 2071, SubID);
		if (SubID.Find(2071002))
		{
			if (m_GAME_PLAYER < GAME_PLAYER)
				m_conf_zhuanzhuan.m_147niao = true;
		}
	}
}

void Table_cs_zhuanzhuan::GameStart()
{
	Table::GameStart();

	OnUserGetCard(m_bBanker);
}

void Table_cs_zhuanzhuan::GameEnd()
{	
	if (m_conf.m_bSupportZhuaniao)
	{
		MJ_user_buffer UserID;
		int iCount = MultiHu(UserID);
		
		m_DaNiaoID = m_bBanker;
		if (iCount == 0)
		{
			m_DaNiaoID = INVALID_CHAIRID;					
		}
		else if (iCount > 1)
		{
			m_DaNiaoID = m_bCurChairID;
		}
		else if (iCount == 1)
		{
			m_DaNiaoID = UserID[0];
		}

		if (m_conf_zhuanzhuan.m_HuJiJiangJi)
		{
			if (iCount != 0)
			{
				m_niao.Add(m_bCurCard);		
				for (int i = 0; i < UserID.Size(); i++)
				{
					m_stHuScore[UserID[i]].stHu[0].iFanNum = 0;
					m_oFan[UserID[i]].Clear();
				}
			}
		}
		else
		{
			_uint8 niao_res = MIN(_uint8(m_conf.m_ZhuaniaoNum), m_CardPool.Count());
			for (_uint8 i = 0; i < niao_res; i++)
			{
				_uint8 uiCard = m_CardPool.GetOneCard();
				m_niao.Add(uiCard);
			}
		}

		if (m_conf_zhuanzhuan.m_147niao)
		{
			for (_uint8 i = 0; i < m_niao.Size(); i++)
			{
				_uint8 num = (CardNum(m_niao[i]));
				if (num == 1 || num == 4 || num == 7)
				{
					m_zhong_niao.Add(num);
				}
			}
		}
		else
		{
			for (_uint8 i = 0; i < m_niao.Size(); i++)
			{
				_uint8 num = (CardNum(m_niao[i]));
				if (num == 1 || num == 5 || num == 9)
				{
					m_zhong_niao.Add(num);
				}
			}
		}
	}
	Table::GameEnd();
	
}
void Table_cs_zhuanzhuan::GameEndReset()
{
	for (int i = 0; i < m_GAME_PLAYER; i++)
	{
		m_Peng[i].Clear();
	}	
	//m_NiaoDouble = 1;
	Table::GameEndReset();
}

void Table_cs_zhuanzhuan::TurnBanker()
{
	MJ_user_buffer UserID;
	int iCounts = MultiHu(UserID);
	if (iCounts > 1)
	{
		m_bBanker = m_bCurChairID;
	}
	else if (iCounts == 1)
	{
		m_bBanker = UserID[0];
	}
}

void Table_cs_zhuanzhuan::UserAfterOperate(_uint8 ChairID, int iOpType, _uint8 bCard)
{
	if (iOpType != TYPE_PASS && iOpType & TYPE_PENG)
	{
		for (int i = 0; i < m_HandCard[ChairID].ChangeableCardsLen; i++)
		{
			if (m_HandCard[ChairID].ChangeableCards[i] == bCard)
			{
				m_Peng[ChairID].Add(bCard);
				break;
			}
		}
	}	

	Table::UserAfterOperate(ChairID, iOpType, bCard);
}

int Table_cs_zhuanzhuan::GetGangInfo(_uint8 ChairID, MJ_opinfo& GangCardInfo)
{
	Table::GetGangInfo(ChairID, GangCardInfo);

	for (int i = 0; i < m_Peng[ChairID].Size(); i++)
	{
		GangCardInfo.Remove(OpState(m_Peng[ChairID][i], TYPE_WANGANG));
	}
	int iOpType = TYPE_NULL;
	for (int i = 0; i < GangCardInfo.Size(); i++)
	{
		iOpType |= GangCardInfo[i].nState;
	}
	return iOpType;
}
void Table_cs_zhuanzhuan::Score_GameEndSucc(HuScoreSet& score, proto::game::AckGameEnd& stGameend)
{
	int iTotalScore[GAME_PLAYER] = { 0 };	
	int iScore[GAME_PLAYER][GAME_PLAYER];
	memset(iScore, 0, sizeof(iScore));
		
	int iZhongNiao = m_zhong_niao.Size();
	if (m_conf.m_bJinNiao)
	{//金鸟
		if (m_zhong_niao.Size() == 0)
		{
			iZhongNiao = m_conf.m_ZhuaniaoNum;
		}
		else if(m_bIsHu[m_bCurChairID] && m_GameLogic.Has4SameCards(m_HandCard[m_bCurChairID], MJ_HONG_ZHONG))
		{
			iZhongNiao = m_conf.m_ZhuaniaoNum;
		}		
	}
	else if (m_conf.m_bMojiJiangji && m_niao.Size() != 0)
	{//一鸟全中 一码全中
		//assert(m_niao.Size() == 1);
		iZhongNiao = CardNum(m_niao[0]);
		if (m_niao[0] == MJ_HONG_ZHONG)
		{
			iZhongNiao = 10;
		}		
	}
	for (int i = 0; i < m_GAME_PLAYER; i++)
	{		
		if (m_bIsHu[i])
		{
			int iBaseScore = (m_stHuScore[i].stHu[0].iFanNum + BankerScoreAdd(i)) * m_BaseScore + iZhongNiao * m_BaseScore;
			if (m_stHuScore[i].stHu[0].bType == HU_ZIMO)
			{
				iBaseScore += m_BaseScore;
				for (_uint8 c = NextChair(i); c != i; c = NextChair(c))
				{
					int iTmpScore = iBaseScore + BankerScoreAdd(c) * m_BaseScore;
					iScore[i][c] -= iTmpScore;
					iScore[i][i] += iTmpScore;
				}				
			}
			else
			{
				_uint8 c = m_stHuScore[i].stHu[0].bPasHuChairID;
				int iTmpScore = iBaseScore + BankerScoreAdd(c) * m_BaseScore;				
				iScore[i][c] -= iTmpScore;
				iScore[i][i] += iTmpScore;
			}			
		}
	}

	for (int i = 0; i < m_GAME_PLAYER; i++)
	{
		Calc_Score(iTotalScore, iScore[i]);
	}
	for (int i = 0; i < m_GAME_PLAYER; i++)
	{
		//score[i].iTotleScore = iTotalScore[i] + m_stGang[i].iTotalScore;
		score[i].iTotleScore = iTotalScore[i];
	}

}

void Table_cs_zhuanzhuan::Score_GameEndDraw(HuScoreSet& score, proto::game::AckGameEnd& stGameend)
{	
}
void Table_cs_zhuanzhuan::OnUserOutCard(_uint8 bOutChairID, _uint8 bOutCard)
{
	if (!m_conf.m_bZimoHu)
	{
		MJ_fan_buffer oFan;
		int iOpType = Check_Hu(bOutChairID, m_HandCard[bOutChairID], oFan);
		if (iOpType & TYPE_HU)
		{
			m_louhu[bOutChairID].Lock(FW_LOCK_ALL_CARD, FW_LOCK_ANY_POINT);
		}
	}
	Table::OnUserOutCard(bOutChairID, bOutCard);
}
int Table_cs_zhuanzhuan::OutCardEstimate(_uint8 ChairID, _uint8 bCard, MJ_major& major, int& fan_num)
{
	int iOpType = Table::OutCardEstimate(ChairID, bCard, major, fan_num);
	if (iOpType & TYPE_HU)
	{
		if (m_louhu[ChairID].IsLock(bCard))
		{
			iOpType &= ~TYPE_HU;
			log_info("%s uid:%d chaird:%d louhu card:0x%02x\n", __FUNCTION__, GetPlayerUid(ChairID), ChairID, bCard);

			proto::game::AckMessage msg;
		
			msg.set_message(msg_word(E_MW_GUO_ZHUANG));

			unicast(SERVER_SEND_MESSAGE, ChairID, msg);
		}
	}
	return iOpType;
}

int Table_cs_zhuanzhuan::GetCardEstimate(_uint8 ChairID, _uint8 bCard, MJ_major& major, int& fan_num)
{
	m_louhu[ChairID].Unlock();
	return Table::GetCardEstimate(ChairID, bCard, major, fan_num);
}
int Table_cs_zhuanzhuan::Calc_Fan(_uint8 ChairID, MJ_fan_buffer& oFan)
{
	int iTotalFan = 0; 
	for (int i = 0; i < oFan.Size(); i++)
	{
		iTotalFan += m_ft2fn->GetFan(oFan[i]);
	}
	return iTotalFan;
}
int Table_cs_zhuanzhuan::Check_Hu_Pattern(_uint8 ChairID, const HandCards& pHc, MJ_fan_buffer& oFan, const MJ_win_pattern& oPattern)
{
	oFan.Add(MJ_FAN_TYPE_PINGHU);
	return TYPE_HU;
}

void Table_cs_zhuanzhuan::Proto_GameScene(_uint8 ChairID, Player* player, proto::game::AckGameScene& stGameScene)
{
	Table::Proto_GameScene(ChairID, player, stGameScene);
	AckCustomiedInfo* info = stGameScene.mutable_hunanscene();
	if (info != NULL)
	{
		info->set_gametype(proto::game::hunanmj::HUNAN_ZHUANZHUAN);
		info->set_ishavepiao(0);
		info->set_jiangma(proto::game::hunanmj::ZHUA_159);
		info->set_laizi(0);	
		info->set_gangcalcimm(m_bGangCalcImme);
	}	
}
void Table_cs_zhuanzhuan::Proto_GameScene_lookon(_uint8 ChairID, Player* player, proto::game::AckGameScene& stGameScene)
{
	Table::Proto_GameScene_lookon(ChairID, player, stGameScene);
	AckCustomiedInfo* info = stGameScene.mutable_hunanscene();
	if (info != NULL)
	{
		info->set_gametype(proto::game::hunanmj::HUNAN_ZHUANZHUAN);
		info->set_ishavepiao(0);
		info->set_jiangma(proto::game::hunanmj::ZHUA_159);//抓鸟
		info->set_laizi(0);//没有癞子
		info->set_gangcalcimm(m_bGangCalcImme);
	}
}
void Table_cs_zhuanzhuan::Proto_GameEndInfo(const MJ_user_buffer& UserID, proto::game::AckGameEnd& stGameend)
{
	Table::Proto_GameEndInfo(UserID, stGameend);

	tagHuNanGameEnd* end = stGameend.mutable_hunangameendinfo();

	if (m_conf.m_bSupportZhuaniao)
	{
		proto::game::hunanmj::tagJiangma* stJiangma = end->mutable_niao();
		stJiangma->set_num(m_niao.Size());
		stJiangma->set_cardtype(proto::game::hunanmj::ZHUA_159);
		for (int i = 0; i < m_niao.Size(); i++)
		{
			stJiangma->add_card(m_niao[i]);

		}
		stJiangma->set_chairid(m_DaNiaoID);
		for (_uint8 i = 0; i < m_niao.Size(); i++)
		{
			_uint8 num = (CardNum(m_niao[i]));
			if (m_conf_zhuanzhuan.m_147niao)
			{
				if (num == 1 || num == 4 || num == 7)
				{
					stJiangma->add_zhongchairid(m_DaNiaoID);
				}
				else
				{
					stJiangma->add_zhongchairid(-1);
				}					
			}
			else
			{
				if (num == 1 || num == 5 || num == 9)
				{
					stJiangma->add_zhongchairid(m_DaNiaoID);
				}
				else
				{
					stJiangma->add_zhongchairid(-1);
				}
			}
		}
		if (m_conf.m_bJinNiao)
		{			
			if (m_zhong_niao.Size() == 0)
			{
				stJiangma->clear_zhongchairid();
				stJiangma->set_cardtype(proto::game::hunanmj::JINNIAO);
				for (_uint8 i = 0; i < m_niao.Size(); i++)
				{
					stJiangma->add_zhongchairid(m_DaNiaoID);
				}
			}
			else if (m_bIsHu[m_bCurChairID] && m_GameLogic.Has4SameCards(m_HandCard[m_bCurChairID], MJ_HONG_ZHONG))
			{
				stJiangma->clear_zhongchairid();
				stJiangma->set_cardtype(proto::game::hunanmj::JINNIAO);
				for (_uint8 i = 0; i < m_niao.Size(); i++)
				{
					stJiangma->add_zhongchairid(m_DaNiaoID);
				}
			}			
		}
		else if (m_conf.m_bMojiJiangji && m_niao.Size() != 0)
		{
			stJiangma->clear_zhongchairid();
			stJiangma->set_cardtype(proto::game::hunanmj::MOJIJIANGJI);
			for (int i = 0; i < m_niao.Size();i++)
			{
				stJiangma->add_zhongchairid(m_DaNiaoID);
			}			
		}
	}

	end->set_leftcardnum(m_CardPool.Count());

	Proto_GameEndInfo_AddFan(UserID, stGameend, end, *m_ft2fn);
	
}
void Table_cs_zhuanzhuan::UserGetFirstCard(_uint8 ChairID, int& iOpType, MJ_fan_buffer& oFan)
{

}
void Table_cs_zhuanzhuan::UserFirstOutCard(_uint8 ChairID, int& iOpType, MJ_fan_buffer& oFan)
{

}
void Table_cs_zhuanzhuan::UserGetLastCard(_uint8 ChairID, int& iOpType, MJ_fan_buffer& oFan)
{
	
}
void Table_cs_zhuanzhuan::UserLastOutCard(_uint8 ChairID, int& iOpType, MJ_fan_buffer& oFan)
{
	
}
void Table_cs_zhuanzhuan::UserGangShangHua(_uint8 ChairID, _uint8 bCard, MJ_fan_buffer& oFan)
{

}

void Table_cs_zhuanzhuan::UserGangHouPao(_uint8 ChairID, _uint8 bCard, MJ_fan_buffer& oFan)
{

}

void Table_cs_zhuanzhuan::DoMultiOperate(_uint8 ChairID, int iOpType, _uint8 bCard)
{
	if ((m_bUserOperate[ChairID] & TYPE_HU) && (iOpType == TYPE_PASS || Is_ChiPengGang(iOpType)))
	{
		m_louhu[ChairID].Lock(FW_LOCK_ALL_CARD, FW_LOCK_ANY_POINT);
	}
	Table::DoMultiOperate(ChairID, iOpType, bCard);
}

void Table_cs_zhuanzhuan::DoSimpleOperate(_uint8 ChairID, int iOpType, _uint8 bCard)
{
	if ((m_bCurChairID != ChairID) && (m_bUserOperate[ChairID] & TYPE_HU) && (iOpType == TYPE_PASS || Is_ChiPengGang(iOpType)))
	{
		m_louhu[ChairID].Lock(FW_LOCK_ALL_CARD, FW_LOCK_ANY_POINT);
	}
	Table::DoSimpleOperate(ChairID, iOpType, bCard);
}

