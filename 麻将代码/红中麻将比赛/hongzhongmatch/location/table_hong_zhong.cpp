#include "table_hong_zhong.h"
#include "../zjh.h"
#include "../common/log.h"

extern ZJH zjh;
extern Log log;
Table_hong_zhong::Table_hong_zhong()
{
	m_QuanBaoPlayer[0] = INVALID_CHAIRID;
	m_QuanBaoPlayer[1] = INVALID_CHAIRID;

	m_ft2fn->SetFan(MJ_FAN_TYPE_QIANGGANGHU, 0);
	m_ft2fn->SetFan(MJ_FAN_TYPE_NOJOKER, 0);	
	
}
Table_hong_zhong::~Table_hong_zhong()
{

}

void Table_hong_zhong::ConfInit_Remote(const Json::Value& val)
{
	Table_cs_zhuanzhuan::ConfInit_Remote(val);
	m_LaiziCard = MJ_HONG_ZHONG;	

	if (json_check_play_type_id(val))
	{
		//bool HasJoker = false;
		MJ_SUB_ID SubID;
		json_get_play_type_id(val, 2450, SubID);
		if (SubID.Find(2450001))
		{
			m_conf.m_bSupportZhuaniao = true;
			m_conf.m_ZhuaniaoNum = 2;
		}
		else if (SubID.Find(2450002))
		{
			m_conf.m_bSupportZhuaniao = true;
			m_conf.m_ZhuaniaoNum = 4;
		}
		else if (SubID.Find(2450003))
		{
			m_conf.m_bSupportZhuaniao = true;
			m_conf.m_ZhuaniaoNum = 6;
		}
		else if (SubID.Find(2450004))
		{
			m_conf.m_bSupportZhuaniao = true;
			m_conf.m_ZhuaniaoNum = 1;
			m_conf.m_bMojiJiangji = true;			
		}
		else if (SubID.Find(2450005))
		{
			m_conf.m_bSupportZhuaniao = false;
		}
		else if (SubID.Find(2450006))
		{
			m_conf.m_bSupportZhuaniao = true;
			m_conf_zhuanzhuan.m_HuJiJiangJi = true;
			m_conf.m_bMojiJiangji = true;

		}
		m_room_config += SubID;

		json_get_play_type_id(val, 2451, SubID);
		if (SubID.Find(2451001))
		{
			m_room_config += SubID;
		}
		json_get_play_type_id(val, 2452, SubID);
		if (SubID.Find(2452001))
		{
			m_GAME_PLAYER = 3;
			m_room_config += SubID;
		}		
		json_get_play_type_id(val, 2449, SubID);
		if (SubID.Find(2449001))
		{
			m_CardPool.SetJoker(m_LaiziCard, 4);
			m_laizi_conf.m_bQishouHu = true;
			//HasJoker = true;
		}
		else if (SubID.Find(2449002))
		{
			m_CardPool.SetJoker(m_LaiziCard, 8);
			m_laizi_conf.m_bQishouHu = false;
			//HasJoker = true;
		}
		m_room_config += SubID;
		json_get_play_type_id(val, 2453, SubID);
		if (SubID.Find(2453001))
		{
			m_conf.m_bZhuangXian = true;
		}
		if (SubID.Find(2453002))
		{
			m_conf.m_bHu7dui = true;
		}
		if (SubID.Find(2453003))
		{
			//if (m_conf.m_bQiangGangHu)
				m_conf_hongzhong.m_bQiangGangQuanBao = true;
		}
		if (SubID.Find(2453004))
		{
			m_conf_hongzhong.m_bJoker_No_PaoHu = true;
		}		
		if (SubID.Find(2453005))
		{
			m_conf_hongzhong.m_bNoJokerDouble = true;
		}
		if (SubID.Find(2453006))
		{
			m_conf.m_bQiangGangHu = true;
		}
// 		if (SubID.Find(2453007))
// 		{
// 			if (HasJoker)
// 			{
// 				m_conf_zhuanzhuan.m_NoJokerDoubleNiao = true;
// 			}
// 		}
		m_room_config += SubID;
	}

}

void Table_hong_zhong::GameEndReset()
{
	Table::GameEndReset();
	m_QuanBaoPlayer[0] = INVALID_CHAIRID;
	m_QuanBaoPlayer[1] = INVALID_CHAIRID;

	m_conf_hongzhong.m_ScoreDouble[0] = 1;
	m_conf_hongzhong.m_ScoreDouble[1] = 1;
	m_conf_hongzhong.m_ScoreDouble[2] = 1;
	m_conf_hongzhong.m_ScoreDouble[3] = 1;
}

// void Table_hong_zhong::GameEnd()
// {
// 	Table_zz_laizi::GameEnd();
// }

void Table_hong_zhong::Score_GameEndSucc(HuScoreSet& score, proto::game::AckGameEnd& stGameend)
{
	int iTotalScore[GAME_PLAYER] = { 0 };
	int iScore[GAME_PLAYER][GAME_PLAYER];
	memset(iScore, 0, sizeof(iScore));

	int iZhongNiao = m_zhong_niao.Size();
	if (m_conf.m_bMojiJiangji && m_niao.Size() != 0)
	{//一鸟全中 一码全中
		iZhongNiao = 0;
		for (int i = 0; i < m_niao.Size();i++)
		{
			if (m_niao[i] == m_LaiziCard)
				iZhongNiao += 10;
			else
				iZhongNiao += CardNum(m_niao[i]);
		}		
	}
	iZhongNiao *= m_conf_hongzhong.m_NiaoFen;
	for (int i = 0; i < m_GAME_PLAYER; i++)
	{
		if (m_bIsHu[i])
		{
			if (m_conf_hongzhong.m_bQiangGangQuanBao && m_bRobHu && m_bAddFanFlag[m_WangGangPlayer])
			{
				int iBaseScore = (m_stHuScore[i].stHu[0].iFanNum + BankerScoreAdd(i)) * m_BaseScore + iZhongNiao * m_BaseScore;
				if (m_stHuScore[i].stHu[0].bType == HU_PAOHU)
				{					
					for (_uint8 c = NextChair(i); c != i; c = NextChair(c))
					{
						int iTmpScore = iBaseScore + BankerScoreAdd(c) * m_BaseScore;
						iScore[i][m_WangGangPlayer] -= iTmpScore * m_conf_hongzhong.m_ScoreDouble[i];
						iScore[i][i] += iTmpScore * m_conf_hongzhong.m_ScoreDouble[i];
					}
				}
				m_QuanBaoPlayer[0] = m_WangGangPlayer;
			}
			else
			{
				int iBaseScore = (m_stHuScore[i].stHu[0].iFanNum + BankerScoreAdd(i)) * m_BaseScore + iZhongNiao * m_BaseScore;
				if (m_stHuScore[i].stHu[0].bType == HU_ZIMO)
				{
					iBaseScore += m_BaseScore;
					for (_uint8 c = NextChair(i); c != i; c = NextChair(c))
					{
						int iTmpScore = iBaseScore + BankerScoreAdd(c) * m_BaseScore;
						iScore[i][c] -= iTmpScore * m_conf_hongzhong.m_ScoreDouble[i];
						iScore[i][i] += iTmpScore * m_conf_hongzhong.m_ScoreDouble[i];
					}
				}
				else
				{
					_uint8 c = m_stHuScore[i].stHu[0].bPasHuChairID;
					int iTmpScore = iBaseScore + BankerScoreAdd(c) * m_BaseScore;
					iScore[i][c] -= iTmpScore * m_conf_hongzhong.m_ScoreDouble[i];
					iScore[i][i] += iTmpScore * m_conf_hongzhong.m_ScoreDouble[i];
				}
			}			
		}
	}	
	for (int i = 0; i < m_GAME_PLAYER; i++)
	{
		Calc_Score(iTotalScore, iScore[i]);
	}	
	for (int i = 0; i < m_GAME_PLAYER; i++)
	{		
		score[i].iTotleScore = iTotalScore[i];

		m_SeatPlayer[i].player->m_BaseScore = iTotalScore[i];
	}

}

void Table_hong_zhong::Proto_GameEndInfo(const MJ_user_buffer& UserID, proto::game::AckGameEnd& stGameend)
{
	Table_zz_laizi::Proto_GameEndInfo(UserID, stGameend);

	if (m_conf_hongzhong.m_bQiangGangQuanBao && m_QuanBaoPlayer[0] != INVALID_CHAIRID)
	{
		proto::game::hunanmj::tagHuNanGameEnd* end = stGameend.mutable_hunangameendinfo();
		proto::game::hunanmj::FanInfo* info = end->mutable_faninfo(m_QuanBaoPlayer[0]);		
		if (info != NULL)
		{
			proto::game::hunanmj::AddFanInfo* pfan = info->add_addfan();
			pfan->set_addtype(MJ_FAN_GUANGDONG_QIANGGANGQUANBAO);
			pfan->set_addnum(m_GAME_PLAYER - 1);
		}
	}
// 	if (m_conf_zhuanzhuan.m_NoJokerDoubleNiao)
// 	{
// 		if (m_NiaoDouble == 2)
// 		{
// 			proto::game::hunanmj::tagHuNanGameEnd* end = stGameend.mutable_hunangameendinfo();
// 			proto::game::hunanmj::FanInfo* info = end->mutable_faninfo(m_DaNiaoID);
// 			proto::game::hunanmj::AddFanInfo* pfan = info->add_addfan();
// 			pfan->set_addtype(MJ_FAN_TYPE_NOJOKER);
// 			pfan->set_addnum(2);
// 		}
// 	}
}

int Table_hong_zhong::OutCardEstimate(_uint8 ChairID, _uint8 bCard, MJ_major& major, int& fan_num)
{
	if (m_conf_hongzhong.m_bJoker_No_PaoHu)
	{
		if (bCard == m_LaiziCard)
		{
			return TYPE_NULL;
		}
		int iOpType = Table::OutCardEstimate(ChairID, bCard, major, fan_num);
		if (m_bRobGangHuJudge || m_bRobHu)
		{
			goto __ROB_GANG_HU_END__;
		}
		if (iOpType & TYPE_HU) 
		{
			int iLaiziNum = m_GameLogic.GetLaiZiSum(m_HandCard[ChairID], m_LaiziCard);
			if (iLaiziNum > 0)
			{
				iOpType &= ~(TYPE_HU);
			}			
		}
	__ROB_GANG_HU_END__:
		if (iOpType & TYPE_HU)
		{
			if (m_louhu[ChairID].IsLock(bCard))
			{
				iOpType &= ~TYPE_HU;				

				proto::game::AckMessage msg;

				msg.set_message(msg_word(E_MW_GUO_ZHUANG));

				unicast(SERVER_SEND_MESSAGE, ChairID, msg);
			}
		}
		return iOpType;
	}
	else
	{
		return Table_zz_laizi::OutCardEstimate(ChairID, bCard, major, fan_num);
	}	
}

int Table_hong_zhong::Check_Hu(_uint8 ChairID, HandCards& pHc, MJ_fan_buffer& oFan)
{
	int iOpType = Table_zz_laizi::Check_Hu(ChairID, pHc, oFan);
	if (m_conf_hongzhong.m_bNoJokerDouble)
	{
		if (iOpType & TYPE_HU)
		{
			if (m_GameLogic.GetCardCountX(&pHc, m_LaiziCard) == 0)
			{
				if (!oFan.Find(MJ_FAN_TYPE_NOJOKER))
				{
					oFan.Add(MJ_FAN_TYPE_NOJOKER);
				}
				m_conf_hongzhong.m_ScoreDouble[ChairID] = 2;
			}
			else
			{
				m_conf_hongzhong.m_ScoreDouble[ChairID] = 1;
			}
		}
	}
	return iOpType;
}