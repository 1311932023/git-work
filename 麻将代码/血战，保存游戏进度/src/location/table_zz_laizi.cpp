#include "table_zz_laizi.h"
#include "../zjh.h"
#include "../common/log.h"

extern ZJH zjh;
extern Log log;

using namespace proto::game::hunanmj;

Table_zz_laizi::Table_zz_laizi()
{

}

Table_zz_laizi::~Table_zz_laizi()
{

}

void Table_zz_laizi::ConfInit_Remote(const Json::Value& val)
{
	Table_cs_zhuanzhuan::ConfInit_Remote(val);	
	
	m_LaiziCard = MJ_HONG_ZHONG;
	
	if (json_check_play_type_id(val))
	{
		MJ_SUB_ID SubID;
		json_get_play_type_id(val, 2430, SubID);
		if (SubID.Find(2430003))
		{					
			m_CardPool.SetJoker(m_LaiziCard, 8);
			m_laizi_conf.m_bQishouHu = false;
		}
		else if (SubID.Find(2430002))
		{
			m_CardPool.SetJoker(m_LaiziCard, 4);
			m_laizi_conf.m_bQishouHu = true;
		}
		m_room_config += SubID;
	}
}
bool Table_zz_laizi::FormatCards(HandCards *pHc, MJ_win_pattern& pattern)
{	
	int iLaiziNum = 0;
	while (m_GameLogic.ChuPai(pHc, m_LaiziCard))
	{
		iLaiziNum++;
	}
	bool r = m_GameLogic.FormatCards_Laizi(pHc->ChangeableCards, pHc->ChangeableCardsLen, iLaiziNum);
	for (int i = 0; i < iLaiziNum; i++)
	{
		m_GameLogic.AddCard(pHc, m_LaiziCard);
	}
	return r;
}
bool Table_zz_laizi::Is7Dui(_uint8 ChairID, HandCards& pHc, MJ_fan_buffer& oFan)
{
	if (m_conf.m_bHu7dui)
	{
		bool r = false;
		int iLaiziNum = 0;
		while (m_GameLogic.ChuPai(&pHc, m_LaiziCard))
		{
			iLaiziNum++;
		}
		_uint8 CardDataNum[MJ_TYPE_COMMON][10];
		m_GameLogic.GetCardCount(&pHc, CardDataNum);
		if (m_GameLogic.L_IsQiDui(&pHc, INVALID_MJ_CARD, CardDataNum, iLaiziNum))
		{
			oFan.Add(MJ_FAN_TYPE_PINGHU);
			r = true;
		}
		for (int i = 0; i < iLaiziNum; i++)
		{
			m_GameLogic.AddCard(&pHc, m_LaiziCard);
		}
		return r;
		
	}
	return false;
}
void Table_zz_laizi::Proto_GameScene(_uint8 ChairID, Player* player, proto::game::AckGameScene& stGameScene)
{
	Table_cs_zhuanzhuan::Proto_GameScene(ChairID, player, stGameScene);
	if (stGameScene.has_hunanscene())
	{
		AckCustomiedInfo* info = stGameScene.mutable_hunanscene();
		if (info != NULL)
		{
			info->set_laizi(m_LaiziCard);
		}
	}
}
void Table_zz_laizi::Proto_GameScene_lookon(_uint8 ChairID, Player* player, proto::game::AckGameScene& stGameScene)
{
	Table_cs_zhuanzhuan::Proto_GameScene_lookon(ChairID, player, stGameScene);
	if (stGameScene.has_hunanscene())
	{
		AckCustomiedInfo* info = stGameScene.mutable_hunanscene();
		if (info != NULL)
		{
			info->set_laizi(m_LaiziCard);
		}
	}
}
void Table_zz_laizi::UserGetFirstCard(_uint8 ChairID, int& iOpType, MJ_fan_buffer& oFan)
{	
	if (m_laizi_conf.m_bQishouHu && Hu_Special_zz_laizi(ChairID, oFan))
	{
		iOpType |= TYPE_HU;
	}
}
bool Table_zz_laizi::Hu_Special_zz_laizi(_uint8 ChairID, MJ_fan_buffer& oFan)
{
	CardNode_set h;
	m_GameLogic.GetCardCount(&m_HandCard[ChairID], h);
	if (h.Find(CardNode_count(m_LaiziCard, 4)))
	{
		if (oFan.IsNull())
		{
			oFan.Add(MJ_FAN_TYPE_PINGHU);
		}
		return true;
	}
	return false;
}
int Table_zz_laizi::GetGangInfo(_uint8 ChairID, MJ_opinfo& GangCardInfo)
{
	Table_cs_zhuanzhuan::GetGangInfo(ChairID, GangCardInfo);
	for (int i = 0; i < GangCardInfo.Size(); i++)
	{
		if (GangCardInfo[i].cbCard == m_LaiziCard)
		{
			GangCardInfo.Remove(GangCardInfo[i]);
			break;
		}
	}
	int iOpType = TYPE_NULL;
	for (int i = 0; i < GangCardInfo.Size(); i++)
	{
		iOpType |= GangCardInfo[i].nState;
	}
	return iOpType;
}

int Table_zz_laizi::OutCardEstimate(_uint8 ChairID, _uint8 bCard, MJ_major& major, int& fan_num)
{
	if (bCard == m_LaiziCard)
	{
		return TYPE_NULL;
	}
	return Table_cs_zhuanzhuan::OutCardEstimate(ChairID, bCard, major, fan_num);		
}
