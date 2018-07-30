#include "table_changsha.h"
#include "../proto/proto.h"
#include "../zjh.h"
#include "../common/log.h"
#include "../game.h"
#include "../libnormalmahjong/mj_common2.h"
#include "../libnormalmahjong/game_logic.h"
#include "../table_helper.h"

using namespace proto::game::hunanmj;

extern ZJH zjh;
extern Log log;

class Changsha_Fantype_to_FanNum : public Fantype_to_FanNum
{
public:
	Changsha_Fantype_to_FanNum()
	{			
		fan_num[MJ_FAN_TYPE_LONGQIDUI] = 6;
		fan_num[MJ_FAN_TYPE_QIDUI] = 6;
		fan_num[MJ_FAN_TYPE_QINGYISE] = 6;
		fan_num[MJ_FAN_TYPE_HAIDILAOYUE] = 6;
		fan_num[MJ_FAN_TYPE_HAIDIPAO] = 6;
		fan_num[MJ_FAN_TYPE_PENGPENGHU] = 6;
		fan_num[MJ_FAN_TYPE_TIANHU] = 6;
		fan_num[MJ_FAN_TYPE_DIHU] = 6;
		fan_num[MJ_FAN_TYPE_QUANQIUREN] = 6;
		fan_num[MJ_FAN_TYPE_QIANGGANGHU] = 6;
		fan_num[MJ_FAN_TYPE_GANGSHANGHUA] = 6;
		fan_num[MJ_FAN_TYPE_GANGHOUPAO] = 6;		
		fan_num[MJ_FAN_TYPE_JIANGDUI] = 1;
		fan_num[MJ_FAN_TYPE_MENQIANQING] = 2;
				
		SetFan(MJ_FAN_CHANGSHA_SIXIHU, 1);
		SetFan(MJ_FAN_CHANGSHA_BANBANHU, 1);
		SetFan(MJ_FAN_CHANGSHA_QUEYIMEN, 1);
		SetFan(MJ_FAN_CHANGSHA_LIULIUSHUN, 1);
		SetFan(MJ_FAN_CHANGSHA_JIANGJIANGHU, 6);
		SetFan(MJ_FAN_CHANGSHA_JIANGYIZHIHUA, 1);
		SetFan(MJ_FAN_CHANGSHA_DANSEYIZHIHUA, 1);
		SetFan(MJ_FAN_CHANGSHA_YIZHINIAO, 1);
		SetFan(MJ_FAN_CHANGSHA_SANTONG, 1);
		SetFan(MJ_FAN_CHANGSHA_ZHONGTU_SIXI, 1);
		SetFan(MJ_FAN_CHANGSHA_JIEJIEGAO, 1);
		SetFan(MJ_FAN_NINGXIANG_JINTONGYUNV, 1);
		SetFan(MJ_FAN_NINGXIANG_YIDIANHONG, 1);


	}
	bool FanFilter(const MJ_fan_buffer& oFan, MJ_fan_buffer& oOutFan)
	{
		oOutFan.Clear();
		for (int i = 0; i < oFan.Size(); i++)
		{
			if (GetFan(oFan[i]) >= 6)
			{
				oOutFan.Add(oFan[i]);
			}
		}
		if (oOutFan.Find(MJ_FAN_TYPE_GANGHOUPAO) || oOutFan.Find(MJ_FAN_TYPE_GANGSHANGHUA))
		{
			oOutFan.Remove(MJ_FAN_TYPE_HAIDILAOYUE);
			oOutFan.Remove(MJ_FAN_TYPE_HAIDIPAO);
		}
		return !(oOutFan.IsNull());//根据
	}
	int operator[](int fan)const
	{
		return GetFan(fan);	
	}
	int GetFan(int fan)const
	{
		if (fan < MJ_FAN_TYPE_MAX)
		{
			return fan_num[fan];
		}
		else if (fan < MJ_FAN_TYPE_SPECIAL_MAX && fan >= MJ_FAN_TYPE_SPECIAL)
		{
			return special_fan_num[fan - MJ_FAN_TYPE_SPECIAL];
		}
		throw("out of range!\n");
		return 0;
	}
};

static Changsha_Fantype_to_FanNum g_changsha_ft2fn;

Table_changsha::Table_changsha()
{	

		
	memset(m_buzhang, 0, sizeof(m_buzhang));
	m_DaNiaoID = INVALID_CHAIRID;

	m_HaidiUser.Clear();
	m_HaidiPlayer = INVALID_CHAIRID;

	m_qishouhu_Timer_stamp = 4;
	m_qishouhu_Timer.data = this;
	ev_timer_init(&m_qishouhu_Timer, Table_changsha::Qishou_Timer_cb, m_qishouhu_Timer_stamp, m_qishouhu_Timer_stamp);

	m_qishouhu_Timer_stamp_0 = 2;
	m_qishouhu_Timer_0.data = this;
	ev_timer_init(&m_qishouhu_Timer_0, Table_changsha::Qishou_Timer_cb_0, m_qishouhu_Timer_stamp_0, m_qishouhu_Timer_stamp_0);
	
	memset(m_qishou_score, 0, sizeof(m_qishou_score));
	//m_bQishouHu = false;

	m_bMultiOpUserNum = 0;
		
	//memset(m_gang_get_multicard, 0, sizeof(m_gang_get_multicard));
	m_cur_gang_player = INVALID_CHAIRID;
	for (int i = 0; i < m_GAME_PLAYER; i++)
	{
		m_MultiFan[i].Clear();
	}
	memset(m_MultiHufanNums, 0, sizeof(m_MultiHufanNums));

	m_gang_dashaizi_Timer_stamp = 3;
	m_gang_dashaizi_Timer.data = this;
	m_Gang_buzhang_res = 0;
	ev_timer_init(&m_gang_dashaizi_Timer, Table_changsha::Gang_Dashaizi_Timer_cb, m_gang_dashaizi_Timer_stamp, m_gang_dashaizi_Timer_stamp);

	m_HuWeiDa = false;
	m_HuWeiDaCard = INVALID_MJ_CARD;
	m_HuWeiDaOp = TYPE_NULL;

	m_qishou_state = false;

	m_conf.m_bCalcAllNiao = true;

	m_ft2fn = &g_changsha_ft2fn;

	m_conf_changsha.hu_wei_da = false;
	
}
Table_changsha::~Table_changsha()
{


	ev_timer_stop(zjh.loop, &m_qishouhu_Timer);
	ev_timer_stop(zjh.loop, &m_qishouhu_Timer_0);
	ev_timer_stop(zjh.loop, &m_gang_dashaizi_Timer);		

}

void Table_changsha::ConfInit_Remote(const Json::Value& val)
{
	Table::ConfInit_Remote(val);

	m_conf.m_bSupportChi = true;	
	m_conf.m_bZimoHu = false;	
	m_conf.m_bYiPaoDuoXiang = true;
	m_conf.m_bGangShangPao = true;
	m_conf.m_bZhuangXian = true;

	m_conf.m_bSupportBuyHorse = false;	
	
	m_conf.m_bHu13yao = false;

	//m_conf.m_ZiMoMultiple = 0;
	
	m_conf.m_bNoSupport_Quanqiuren = true;
				
	if (json_check_play_type_id(val))
	{
		MJ_SUB_ID subid;

		int iCounts = json_get_play_type_id(val, 2000, subid);
		if (iCounts != 0 )
		{			
			m_conf.m_bSupportZhuaniao = true;
			switch (subid[0])
			{
			case 2000001:
				m_conf.m_ZhuaniaoNum = 1;
				break;
			case 2000002:
				m_conf.m_ZhuaniaoNum = 2;
				break;
			case 2000003:
				m_conf.m_ZhuaniaoNum = 4;
				break;
			case 2000004:
				m_conf.m_ZhuaniaoNum = 6;
				break;
			case 2000005:
				m_conf.m_ZhuaniaoNum = 0;
				m_conf.m_bSupportZhuaniao = false;
				break;
			default:
				break;
			}			 
			m_room_config.Add(subid[0]);
		}

		iCounts = json_get_play_type_id(val, 2070, subid);
		if (iCounts != 0 && subid[0] == 2070002)
		{
			m_conf.m_bZimoHu = true;			
			m_conf.m_bYiPaoDuoXiang = true;
			m_conf.m_bGangShangPao = true;

			m_room_config.Add(subid[0]);
		}	
	
		iCounts = json_get_play_type_id(val, 2370, subid);
		if (iCounts != 0 && subid[0] == 2370001)
		{//长沙麻将

			m_conf_changsha.changsha_qishouhu = true;
			m_conf.m_bZhuangXian = true;

			m_CardPool.Set(CARD_POOL_NO_WIND_DRAGON_FLOWER, tid);			

			m_room_config.Add(subid[0]);

			//m_conf_changsha.gang_hou_bu_zhang = 6;
		}		

		iCounts = json_get_play_type_id(val, 2020, subid);
		if (iCounts != 0)
		{
			if (subid[0] == 2020002)
			{
				m_conf_changsha.feng_ding = -42;
			}
			else if (subid[0] == 2020003)
			{
				m_conf_changsha.feng_ding = -21;
			}

			m_room_config.Add(subid[0]);
		}
	/*	iCounts = json_get_play_type_id(val, 2447, subid);
		if (iCounts != 0)
		{
			if (subid[0] == 2447001)
			{
				m_conf_changsha.gang_hou_bu_zhang = 2;
			}
			else if (subid[0] == 2447002)
			{
				m_conf_changsha.gang_hou_bu_zhang = 4;
			}
			else if (subid[0] == 2447003)
			{
				m_conf_changsha.gang_hou_bu_zhang = 6;
			}

			m_room_config.Add(subid[0]);
		}*/
		iCounts = json_get_play_type_id(val, 2448, subid);
		if (iCounts != 0)
		{
			if (subid[0] == 2448001)
			{
				m_GAME_PLAYER = 3;
			}
			m_room_config.Add(subid[0]);
		}
		json_get_play_type_id(val, 2709, subid);
		if (subid.Find(2709001))
		{
			m_conf_changsha.hu_wei_da = true;
		}
		json_get_play_type_id(val, 2001, subid);
		if (subid.Find(2001001))
		{
			m_conf_changsha.jiang_yi_zhi_hua = true;
		}
		if (subid.Find(2001002))
		{
			m_conf_changsha.dan_se_yi_zhi_hua = true;
		}
		if (subid.Find(2001003))
		{
			m_conf_changsha.yi_zhi_niao = true;
		}
		if (subid.Find(2001004))
		{
			m_conf_changsha.san_tong = true;
		}
		if (subid.Find(2001005))
		{
			m_conf_changsha.zhong_tu_si_xi = true;
		}
		if (subid.Find(2001006))
		{
			m_conf_changsha.hai_di_niao = true;
		}
		if (subid.Find(2001007))
		{
			m_conf_changsha.jie_jie_gao = true;
		}
		m_room_config += subid;

		json_get_play_type_id(val, 2002, subid);
		if (subid.Find(2002001))
		{
			m_conf_changsha.hai_di_niao_quan_zhong = false;
		}
		else if (subid.Find(2002002))
		{
			m_conf_changsha.hai_di_niao_quan_zhong = true;
		}
		m_room_config += subid;
	}	
}

void Table_changsha::GameStart()
{
	Table::GameStart();

	if (!QishouHu())
	{//起手胡	
		OnUserGetCard(m_bBanker);
	}
}

void Table_changsha::GameEnd()
{

	if (m_conf.m_bSupportZhuaniao)			
	{			

		m_DaNiaoID = m_bBanker;
		MJ_user_buffer UserID;
		int iCount = MultiHu(UserID);
		if (iCount == 0)
		{
			return Table::GameEnd();
		}			
		else if (iCount > 1)
		{
			m_DaNiaoID = m_bCurChairID;
		}
		else if (iCount == 1)
		{
			m_DaNiaoID = UserID[0];
		}

				
		_uint8 CountRes = m_CardPool.Count();
		if (m_HaidiPlayer != INVALID_CHAIRID && m_conf_changsha.hai_di_niao)
		{
			_uint8 uiCard = m_bCurCard;
			if (m_conf_changsha.hai_di_niao_quan_zhong)
			{
				for (int i = 0; i < m_conf.m_ZhuaniaoNum; i++)
				{
					m_niao.Add(uiCard);
				}
			}
			else
			{
				m_niao.Add(uiCard);
			}
		}
		else
		{
			for (int i = 0; i < MIN(m_conf.m_ZhuaniaoNum, CountRes); i++)
			{
				_uint8 uiCard = m_CardPool.GetOneCard();
				m_niao.Add(uiCard);
			}
		}
	}
	Table::GameEnd();
}
void Table_changsha::GameEndReset()
{

	Table::GameEndReset();	
	
	memset(m_buzhang, 0, sizeof(m_buzhang));
	m_HaidiUser.Clear();
	m_HaidiPlayer = INVALID_CHAIRID;

	//m_qishou_zhongniao_user.Clear();
	for (int i = 0; i < m_GAME_PLAYER; i++)
	{
		m_qishou_fan[i].Clear();
		m_qishou_banpai[i].Clear();
		m_qishou_score[i] = 0;
	}
	memset(&m_qishou_shai, 0, sizeof(m_qishou_shai));

	m_bMultiOpUserNum = 0;

	GangHouClear();
		
	TIMER_STOP(m_qishouhu_Timer);
	TIMER_STOP(m_qishouhu_Timer_0);

	m_qishou_state = false;
	if (m_conf_changsha.zhong_tu_si_xi)
	{
		for (int i = 0; i < GAME_PLAYER; i++)
		{
			m_SiXi[i].Clear();
		}
	}	
}

bool Table_changsha::CheckUserOutCard(_uint8 ChairID, _uint8 cbCard)
{	
	if (m_qishou_state)
	{
		return false;
	}
	if (m_cur_gang_player != INVALID_CHAIRID)
	{
		return true;
	}
	if (!Table::CheckUserOutCard(ChairID, cbCard))
		return false;
	if (m_buzhang[ChairID].ting_ed)
	{		
		if (m_bCurCard == cbCard)
		{
			return true;
		}
		return false;
	}
	return true;
}
bool Table_changsha::CheckUserOperator(_uint8 ChairID, int iOpType, _uint8 bCard)
{
	if (m_cur_gang_player != INVALID_CHAIRID)
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
	Table::Score_GameEndSucc(score, stGameend);

	for (int i = 0; i < m_GAME_PLAYER; i++)
	{//只有在每局的最后才将m_HandCard的杠牌状态转换成 TYPE_BU_ZHANG
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

	if (m_conf_xianningmj.mj_conf_xianningmajiang == true)
	{
		::proto::game::xianningmj::GameEndXianNing *xianning = stGameend.mutable_xianninggameend();
		::proto::game::xianningmj::tagHongZhongLaiZiGang *hongzhonglaiziInfo[GAME_PLAYER];

		for (int i = 0; i < m_GAME_PLAYER; i++)
		{
			hongzhonglaiziInfo[i] = xianning->add_hongzhonglaizigang();
		}

		for (int i = 0; i < m_GAME_PLAYER; i++)
		{

			hongzhonglaiziInfo[i]->set_hongzhonggang(m_bUserOutHongZhongCardNum[i]);
			hongzhonglaiziInfo[i]->set_laizegang(m_bUserOutGuiCardNum[i]);
		}
	}
	else
	{

	}

	tagHuNanGameEnd* end = stGameend.mutable_hunangameendinfo();
	end->set_leftcardnum(m_CardPool.Count());

	FanInfo* info[GAME_PLAYER] = { 0 };
	for (_uint8 i = 0; i < m_GAME_PLAYER; i++)
	{
		info[i] = end->add_faninfo();
	}

	MJ_fan_buffer tmpFan[GAME_PLAYER] = m_oFan;
	int tmpHuFanNums[GAME_PLAYER] = { 0 };	

	Calc_Score(tmpHuFanNums, m_HuFanNums);
	
	if (UserID.Size() == 1)
	{//一个人胡
		_uint8 win_seat = UserID[0];
		//assert(m_stHuScore[win_seat].stHu.Size() != 0);
		if (m_stHuScore[win_seat].stHu[0].bType == HU_ZIMO)
		{//自摸胡
			info[win_seat]->set_chairid(win_seat);
			info[win_seat]->set_specialtype(HU_ZIMO);			
			int iAddFanCount = 0;//垃圾代码
			for (int i = 0; i < tmpFan[win_seat].Size(); i ++)
			{
				AddFanInfo* pfan = info[win_seat]->add_addfan();   //  hu fan 
				pfan->set_addtype(tmpFan[win_seat][i]);
				iAddFanCount += BankerScoreAdd(win_seat);//垃圾代码
				pfan->set_addnum(m_ft2fn->GetFan(tmpFan[win_seat][i]) + BankerScoreAdd(win_seat));
			}	
			for (_uint8 c = NextChair(win_seat); c != win_seat; c = NextChair(c))
			{
				info[c]->set_chairid(c);				
				info[c]->set_specialtype(HU_BEIZIMO);
				info[c]->set_fannum(0);
				//info[c]->set_fantype(0);				
			}
			info[win_seat]->set_fannum(tmpHuFanNums[win_seat] + iAddFanCount);
		}
		else if (m_stHuScore[win_seat].stHu[0].bType == HU_PAOHU)
		{//点炮			
			info[win_seat]->set_chairid(win_seat);
			info[win_seat]->set_specialtype(HU_PAOHU);			
			int iAddFanCount = 0;//垃圾代码
			_uint8 iPaoID = m_stHuScore[win_seat].stHu[0].bPasHuChairID;
			//info[win_seat]->set_fantype(m_HuFanType[win_seat]);
			for (int i = 0; i < tmpFan[win_seat].Size(); i++)
			{
				AddFanInfo* pfan = info[win_seat]->add_addfan();
				pfan->set_addtype(tmpFan[win_seat][i]);
				iAddFanCount += BankerScoreAdd(win_seat) + BankerScoreAdd(iPaoID);//垃圾代码
				pfan->set_addnum(m_ft2fn->GetFan(tmpFan[win_seat][i]) + BankerScoreAdd(win_seat));
			}

			for (_uint8 c = NextChair(win_seat); c != win_seat; c = NextChair(c))
			{
				info[c]->set_chairid(c);
				info[c]->set_specialtype(HU_INVALID);
				info[c]->set_fannum(0);
				//info[c]->set_fantype(0);

			}			
			info[win_seat]->set_fannum(tmpHuFanNums[win_seat] + iAddFanCount);
			info[m_stHuScore[win_seat].stHu[0].bPasHuChairID]->set_specialtype(HU_DIANPAO);
		}
	}
	else if (UserID.Size() > 1)
	{//一炮多响
		for (_uint8 i = 0; i < m_GAME_PLAYER; i++)
		{
			if (UserID.Find(i))
			{
				
				info[i]->set_chairid(i);
				info[i]->set_specialtype(HU_PAOHU);				
				int iAddFanCount = 0;

				_uint8 iPaoID = m_stHuScore[i].stHu[0].bPasHuChairID;
				for (int j = 0; j < tmpFan[i].Size(); j++)
				{
					AddFanInfo* pfan = info[i]->add_addfan();
					pfan->set_addtype(tmpFan[i][j]);
					iAddFanCount += BankerScoreAdd(i) + BankerScoreAdd(iPaoID);//垃圾代码
					pfan->set_addnum(m_ft2fn->GetFan(tmpFan[i][j]) + BankerScoreAdd(i));
				}
				info[i]->set_fannum(tmpHuFanNums[i] + iAddFanCount);
			}
			else
			{				
				info[i]->set_chairid(i);
				info[i]->set_specialtype(HU_INVALID);
				info[i]->set_fannum(0);
				//info[i]->set_fantype(0);
			}
			
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
	{//非打牌玩家
		int iOpType = m_bUserOperate[ChairID];
		if (m_bUserOperate[ChairID] & TYPE_GANG)
		{
			m_bUserOperate[ChairID] |= TYPE_BU_ZHANG;
			iOpType = m_bUserOperate[ChairID];			
			
			proto::game::hunanmj::tagBuZhang* bu = stNotify.mutable_bu();
			proto::game::hunanmj::tagBuinfo* buinfo = bu->add_bu();
			buinfo->set_card(bCard);
			buinfo->set_state(TYPE_BU_ZHANG);
			if (IsListening(ChairID, bCard))
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
	{//非打牌玩家
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

				if (IsListening(ChairID, gang[i].cbCard))
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
	
	Table::Proto_Game_UserGetCard(ChairID, MoChairID, bCard, stUsergetcard);	
	if (ChairID == MoChairID)
	{//摸牌玩家
		if (m_bUserOperate[ChairID] & TYPE_GANG)
		{
			stUsergetcard.clear_ganginfo();

			m_bUserOperate[ChairID] |= TYPE_BU_ZHANG;

			int iOpType = m_bUserOperate[ChairID];			
			
			MJ_opinfo stGang;			
			m_GameLogic.GetGangInfo(&m_HandCard[MoChairID], stGang);
			proto::game::hunanmj::tagBuZhang* bu = stUsergetcard.mutable_bu();
			for (int i = 0; i < stGang.Size(); i++)
			{				
				if (m_buzhang[ChairID].ting_ed && stGang[i].cbCard != bCard)
					continue;
				proto::game::hunanmj::tagBuinfo* buinfo = bu->add_bu();
				buinfo->set_card(stGang[i].cbCard);
				buinfo->set_state(TYPE_BU_ZHANG);

				if (IsListening(ChairID, stGang[i].cbCard))
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
	if (m_buzhang[ChairID].buzhang.Find(bCard))
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
	if ((m_nPlayingTimes == 0) || (m_bBanker >= m_GAME_PLAYER))
	{
		m_bBanker = 0;
		for (int i = 0; i < m_GAME_PLAYER; i++)
		{			
			if (m_SeatPlayer[i].player != NULL )				
			{
				log_info("%s room %d player uid:%d, room owner:%d\n", __FUNCTION__, tid, GetPlayerUid(i), m_room_owner_uid);
				if (m_SeatPlayer[i].player->uid == m_room_owner_uid)
				{
					m_bBanker = m_SeatPlayer[i].seatid;
					break;
				}				
			}
		}
	}
	
}
void Table_changsha::TurnBanker()
{
	MJ_user_buffer UserID;
	int iCount = MultiHu(UserID);
	if (iCount == 0)
	{		
		if (m_HaidiPlayer == INVALID_CHAIRID)
		{
			if (m_HaidiUser.NotNull())
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
	else if (iCount > 1)
	{
		m_bBanker = m_bCurChairID;
	}
	else
	{
		m_bBanker = UserID[0];
	}	
}
void Table_changsha::UserOperateChi(_uint8 ChairID, int iOpType, _uint8 bCard)
{
	if (m_qishou_banpai[ChairID].NotNull())
	{
		Send_BanpaiOver(ChairID);		
	}	
	if (m_cur_gang_player != INVALID_CHAIRID)
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
	if (m_cur_gang_player != INVALID_CHAIRID)
	{
		GangHouClear();
	}
	Table::UserOperatePeng(ChairID, bCard);
	
}
void Table_changsha::UserOperateGang(_uint8 ChairID, int iOpType, _uint8 bCard)
{
	if (m_qishou_banpai[ChairID].NotNull())
	{
		Send_BanpaiOver(ChairID);		
	}
	if (m_cur_gang_player != INVALID_CHAIRID)
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
	
	if (!m_buzhang[ChairID].buzhang.Find(bCard))
	{//是杠听牌不是补张
		m_buzhang[ChairID].ting_ed = true;

		//m_gang_get_multicard[ChairID] = true;
		m_cur_gang_player = ChairID;

		SendTingInfo_Simple(ChairID);
	}

}
void Table_changsha::UserOperateHu(_uint8 ChairID, _uint8 bHuCard /* = 0xFF */)
{
	Table::UserOperateHu(ChairID, bHuCard);
}
bool Table_changsha::Hu_Special4Hunan(_uint8 ChairID, MJ_qishou_fan& oFan, MJ_hand_buffer& hc)
{
	HandCards* pHc = &m_HandCard[ChairID];
	if (m_conf_changsha.changsha_qishouhu)
	{		
		oFan.Clear();
		hc.Clear();
		MJ_hand_buffer j258;
		/*找258的将，看不太懂*/
		m_GameLogic.Find_Jiang258(pHc, j258);		

		if (m_conf_changsha.yi_dian_hong)
		{
			if (j258.Size() == 1)
			{
				if (!hc.Find(j258[0]))
				{
					hc.Add(j258[0]);
				}
				oFan.Add(MJ_FAN_NINGXIANG_YIDIANHONG);
			}
		}
		if (m_conf_changsha.jin_tong_yu_nv)
		{
			if (j258.Size() >= 4)
			{
				MJ_hand_buffer tmp = j258;
				if (tmp.Remove(0x12) && tmp.Remove(0x12) && tmp.Remove(0x22) && tmp.Remove(0x22))
				{
					if (!hc.Find(0x12))
					{
						hc.Add(0x12);
						hc.Add(0x12);
					}
					if (!hc.Find(0x22))
					{
						hc.Add(0x22);
						hc.Add(0x22);
					}
				}
			}
		}

		MJ_opinfo opinfo;
		if (m_GameLogic.GetGangInfo(pHc, opinfo) & TYPE_ANGANG)		
		{
			for (int i = 0; i < opinfo.Size(); i++)
			{
				if (opinfo[i].nState == TYPE_ANGANG)
				{
					oFan.Add(MJ_FAN_CHANGSHA_SIXIHU);
					hc.Add(opinfo[i].cbCard);
					hc.Add(opinfo[i].cbCard);
					hc.Add(opinfo[i].cbCard);
					hc.Add(opinfo[i].cbCard);		
					m_SiXi[ChairID].Add(opinfo[i].cbCard);
				}
			}
		}		
	
		MJ_cards_type_buffer oCards1;
		HandCardsHasMultiCard(pHc, 3, oCards1);
		if (opinfo.NotNull())
		{
			for (int i = 0; i < opinfo.Size(); i++)
			{
				if (opinfo[i].nState == TYPE_ANGANG)
				{
					oCards1.Remove(opinfo[i].cbCard);
				}
			}			
		}
		if (oCards1.Size() > 1)
		{
			oFan.Add(MJ_FAN_CHANGSHA_LIULIUSHUN);
			hc.Add(oCards1[0]);
			hc.Add(oCards1[0]);
			hc.Add(oCards1[0]);
			hc.Add(oCards1[1]);
			hc.Add(oCards1[1]);
			hc.Add(oCards1[1]);
		}

		if (j258.IsNull())
		{
			oFan.Add(MJ_FAN_CHANGSHA_BANBANHU);
			hc.Clear();
			hc.Add(pHc->ChangeableCards, pHc->ChangeableCardsLen);
		}
		
				
		_uint8 iTypeCount[MJ_TYPE_COMMON] = { 0 };
		if (m_GameLogic.GetCardTypeCount(pHc, iTypeCount))
		{
			if (iTypeCount[0] == 0 ||
				iTypeCount[1] == 0 ||
				iTypeCount[2] == 0)
			{
				oFan.Add(MJ_FAN_CHANGSHA_QUEYIMEN);
				hc.Clear();
				hc.Add(pHc->ChangeableCards, pHc->ChangeableCardsLen);
			}
			if (m_conf_changsha.dan_se_yi_zhi_hua)
			{
				if (iTypeCount[0] == 1 && j258.Find(0x05))
				{
					if (!hc.Find(0x05))	hc.Add(0x05);
					oFan.Add(MJ_FAN_CHANGSHA_DANSEYIZHIHUA);
				}
				if (iTypeCount[1] == 1 && j258.Find(0x15))
				{
					if (!hc.Find(0x15)) hc.Add(0x15);
					oFan.Add(MJ_FAN_CHANGSHA_DANSEYIZHIHUA);
				}
				if (iTypeCount[2] == 1 && j258.Find(0x25))
				{
					if (!hc.Find(0x25)) hc.Add(0x25);
					oFan.Add(MJ_FAN_CHANGSHA_DANSEYIZHIHUA);
				}
			}	
			if (m_conf_changsha.yi_zhi_niao)
			{
				if (iTypeCount[1] == 1)
				{
					for (int i = 0; i < pHc->ChangeableCardsLen; i++)
					{
						if (pHc->ChangeableCards[i] == 0x11)
						{
							if (!hc.Find(0x11)) hc.Add(0x11);
							oFan.Add(MJ_FAN_CHANGSHA_YIZHINIAO);
						}
					}
				}
			}
		}
		if (m_conf_changsha.jiang_yi_zhi_hua)
		{
			if (j258.NotNull())
			{
				_uint8 num = CardNum(j258[0]);
				if (j258.Size() == 1 && num == 5)
				{
					if (!hc.Find(j258[0]))
					{
						hc.Add(j258[0]);
					}
					oFan.Add(MJ_FAN_CHANGSHA_JIANGYIZHIHUA);
				}
			}
		}

		MJ_hand_buffer tmp_hc;
		if (m_conf_changsha.san_tong)
		{
			if (m_GameLogic.Find_SanTong(pHc, tmp_hc))
			{
				oFan.Add(MJ_FAN_CHANGSHA_SANTONG);
				for (int i = 0; i < tmp_hc.Size(); i++)
				{
					int res = hc.Counts(tmp_hc[i]);
					while (res++ < 2)
					{
						hc.Add(tmp_hc[i]);						
					}
				}				
			}
		}
		if (m_conf_changsha.jie_jie_gao)
		{
			if (m_GameLogic.Find_JieJieGao(pHc, tmp_hc))
			{
				oFan.Add(MJ_FAN_CHANGSHA_JIEJIEGAO);
				for (int i = 0; i < tmp_hc.Size(); i++)
				{
					int res = hc.Counts(tmp_hc[i]);
					while (res++ < 2)
					{
						hc.Add(tmp_hc[i]);
					}
				}
			}
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



int Table_changsha::Check_Hu(_uint8 ChairID, HandCards& pHc, MJ_fan_buffer& oFan)
{	
	int iOpType = Table::Check_Hu(ChairID, pHc, oFan);
	MJ_hand_buffer hc;
	if (m_GameLogic.Find_Jiang258(&pHc, hc))	
	{
		oFan.Add(MJ_FAN_CHANGSHA_JIANGJIANGHU);
		iOpType |= TYPE_HU;
	}	
	return iOpType;
}
bool Table_changsha::Is7Dui(_uint8 ChairID, HandCards& pHc, MJ_fan_buffer& oFan)
{
	if (m_conf.m_bHu7dui && m_GameLogic.Is7Dui(&pHc))
	{
		_uint8 uiCounts = 0;
		if (m_GameLogic.IsLong7Dui(&pHc, uiCounts))
		{
			for (int i = 0; i < uiCounts; i++)
				oFan.Add(MJ_FAN_TYPE_LONGQIDUI);
		}
		if (m_GameLogic.IsQingYiSe(&pHc))
		{
			oFan.Add(MJ_FAN_TYPE_QINGYISE);
		}
		oFan.Add(MJ_FAN_TYPE_QIDUI);
		return true;
	}
	return false;
}
bool Table_changsha::IsQuanqiuren(const HandCards& pHc)
{
	return m_GameLogic.IsDanDiao(pHc);
	//return (pHc.ChangeableCardsLen == 2) && (pHc.ChangeableCards[0] == pHc.ChangeableCards[1]);	
}
int Table_changsha::Check_Hu_Pattern(_uint8 ChairID, const HandCards& pHc, MJ_fan_buffer& oFan, const MJ_win_pattern& oPattern)
{
	//Table::Check_Hu_Pattern(pHc, oFan, oPattern);
		
	if (CardNum(oPattern.jiang) == 0x2 || CardNum(oPattern.jiang) == 0x5 || CardNum(oPattern.jiang) == 0x8)
	{		
		if (m_conf_changsha.men_qing && m_GameLogic.IsMenQing(&pHc))
		{
			oFan.Add(MJ_FAN_TYPE_MENQIANQING);
		}
		else
		{
			oFan.Add(MJ_FAN_TYPE_JIANGDUI);
		}
	}
	if (m_GameLogic.IsPengPengHu(&pHc, oPattern.kan, oPattern.jiang))
	{
		oFan.Add(MJ_FAN_TYPE_PENGPENGHU);		
	}
	if (m_GameLogic.IsQingYiSe(&pHc))
	{
		oFan.Add(MJ_FAN_TYPE_QINGYISE);
	}	
	if (IsQuanqiuren(pHc))
	{
		oFan.Add(MJ_FAN_TYPE_QUANQIUREN);		
	}
	return oFan.IsNull() ? TYPE_NULL : TYPE_HU;
}



void Table_changsha::Calc_BaseScore(_uint8 ChairID, int iBaseScore[GAME_PLAYER])
{
	if (!m_bIsHu[ChairID])
		return;

	MJ_fan_buffer OutFan;
	int iScore = 0;
	int iFanCount = 0;
	if (m_cur_gang_player == INVALID_CHAIRID)
	{

		if (m_ft2fn->FanFilter(m_oFan[ChairID], OutFan))
		{
			iFanCount = OutFan.Size();
		}
		else
		{
			iFanCount = m_oFan[ChairID].Size();
		}
	}
	else
	{
		if (m_ft2fn->FanFilter(m_MultiFan[ChairID], OutFan))
		{
			iFanCount = OutFan.Size();
		}
		else
		{
			iFanCount = m_MultiFan[ChairID].Size();
		}
	}
	for (int i = 0; i < m_stHuScore[ChairID].stHu.Size(); i++)
	{
		iScore += m_stHuScore[ChairID].stHu[i].iFanNum;
	}
	iScore += BankerScoreAdd(ChairID) * iFanCount;
	iBaseScore[ChairID] = iScore * m_BaseScore;
	for (_uint8 c = NextChair(ChairID); c != ChairID; c = NextChair(c))
	{
		int tmpScore = (iScore + BankerScoreAdd(c) * iFanCount)* m_BaseScore;		
		iBaseScore[c] = tmpScore;
	}
}
void Table_changsha::Calc_HuScore(_uint8 ChairID, int iBaseScore[GAME_PLAYER], int iScore[GAME_PLAYER])
{
	if (m_bIsHu[ChairID])
	{		

		if (m_bCurChairID == ChairID)
		{
			for (_uint8 c = NextChair(ChairID); c != ChairID; c = NextChair(c))
			{
				iScore[ChairID] += iBaseScore[c];
				iScore[c] -= iBaseScore[c];
			}
		}
		else
		{
			_uint8 iPaoID = m_stHuScore[ChairID].stHu[0].bPasHuChairID;
			iScore[ChairID] += iBaseScore[iPaoID];
			iScore[iPaoID] -= iBaseScore[iPaoID];

		}
		log_info("%s uid:%d chairid:%d, base score [%d,%d,%d,%d]\n", __FUNCTION__, GetPlayerUid(ChairID), ChairID, iScore[0], iScore[1], iScore[2], iScore[3]);
	}
}
void Table_changsha::Calc_ZhongNiao(_uint8 ChairID, _uint8 ShaiNum, int iBaseScore[GAME_PLAYER], int iScore[GAME_PLAYER])
{
	if (m_bIsHu[ChairID])
	{		
		_uint8 shai = CardNum(ShaiNum) - 1;
		_uint8 ShaiChairID = NextChair(m_bBanker, shai);		
		if (m_bCurChairID == ChairID)
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
			_uint8 iPaoID = m_stHuScore[ChairID].stHu[0].bPasHuChairID;
			if ((ChairID == ShaiChairID) || (iPaoID == ShaiChairID))
			{
				iScore[ChairID] += iBaseScore[iPaoID];
				iScore[iPaoID] -= iBaseScore[iPaoID];
			}
		}
		log_info("%s uid:%d ChairID:%d, niao:0x%x[%d,%d,%d,%d]\n", __FUNCTION__, GetPlayerUid(ChairID), ChairID, CardNum(ShaiNum) - 1, iScore[0], iScore[1], iScore[2], iScore[3]);
	}
	
}

int Table_changsha::Calc_Fan(_uint8 ChairID, MJ_fan_buffer& oFan)
{
	int iTotalFan = 0;		

	MJ_fan_buffer oFilterFan;
	if (m_ft2fn->FanFilter(oFan, oFilterFan))
	{//计算番之前把如果过滤番，如果有大胡就过滤所有小胡
		for (int i = 0; i < oFilterFan.Size(); i++)
		{
			iTotalFan += m_ft2fn->GetFan(oFilterFan[i]);
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
int Table_changsha::Calc_Hu(_uint8 ChairID, _uint8 bCard, int Score[GAME_PLAYER])
{
	
	if (m_cur_gang_player != INVALID_CHAIRID)
	{
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

int Table_changsha::OutCardEstimate(_uint8 ChairID, _uint8 bCard, MJ_major& major, int& fan_num)
{
	int iOpType = Table::OutCardEstimate(ChairID, bCard, major, fan_num);	
	if (iOpType & TYPE_HU) 
	{
		if (m_louhu[ChairID].IsLock(bCard, fan_num))
		{			
			iOpType &= ~TYPE_HU;			
			log_info("%s uid:%d chaird:%d louhu card:0x%02x\n", __FUNCTION__, GetPlayerUid(ChairID), ChairID, bCard);

			proto::game::AckMessage msg;

			msg.set_message(msg_word(E_MW_GUO_ZHUANG));

			unicast(SERVER_SEND_MESSAGE, ChairID, msg);
		}
	}
	if (m_bCurChairID != PreChair(ChairID))
	{
		iOpType &= ~TYPE_CHI;		
	}
	
	if (m_buzhang[ChairID].ting_ed)
	{		
		iOpType &= ~(TYPE_CHI | TYPE_PENG);		
		if (iOpType & TYPE_GANG)
		{
			
			if (IsListening(ChairID, bCard))
			{
				iOpType |= TYPE_BU_ZHANG;
			}
			else
			{
				iOpType &= ~(TYPE_GANG);
			}		
		}
	}
	else if(iOpType & TYPE_GANG)
	{
		iOpType |= TYPE_BU_ZHANG;
	}	
	return iOpType;
}

int Table_changsha::GetCardEstimate(_uint8 ChairID, _uint8 bCard, MJ_major& major, int& fan_num)
{
	m_louhu[ChairID].Unlock();
	int iOpType = Table::GetCardEstimate(ChairID, bCard, major, fan_num);

	if (m_buzhang[ChairID].ting_ed)
	{
		iOpType &= ~(TYPE_CHI | TYPE_PENG);
		if (iOpType & TYPE_GANG)
		{
			iOpType &= ~(TYPE_GANG);
			MJ_opinfo ganginfo;
			m_GameLogic.AddCard(&m_HandCard[ChairID], bCard);
			m_GameLogic.GetGangInfo(&m_HandCard[ChairID], ganginfo);			
			for (int i = 0; i < ganginfo.Size(); i++)
			{
				if (m_buzhang[ChairID].ting_ed && ganginfo[i].cbCard != bCard)
					continue;
				if (IsListening(ChairID, ganginfo[i].cbCard))
				{
					iOpType |= TYPE_BU_ZHANG | ganginfo[i].nState;
				}
			}
			m_GameLogic.ChuPai(&m_HandCard[ChairID], bCard);
			
		}
	}
	else if (iOpType & TYPE_GANG)
	{
		iOpType |= TYPE_BU_ZHANG;
	}
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
	log_info("%s uid:%d chaird:%d, curoptype:0x%x, optype:0x%x, now done OpNum:%d, totalOpnum:%d \n", __FUNCTION__,
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
			log_info("%s user uid:%d pass seat:%d, optype:0x%x!\n", __FUNCTION__, GetPlayerUid(ChairID), ChairID, m_bUserOperate[ChairID]);
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
			Table::DoSimpleOperate(m_cur_gang_player, m_HuWeiDaOp, m_HuWeiDaCard);
			if (m_HuWeiDaOp & TYPE_BU_ZHANG)
				GangHouClear();
		}
		init_multi_data();
	}
}
void Table_changsha::DoSimpleOperate(_uint8 ChairID, int iOpType, _uint8 bCard)
{	
	if (fn_chi_peng_gang(iOpType))
	{
		m_louhu[ChairID].Unlock();
	}
	if (m_HaidiUser.Size() != 0)
	{
		if (ChairID >= m_GAME_PLAYER)
		{
			log_error("%s client package is error uid:%d chairid:%d\n", __FUNCTION__, GetPlayerUid(ChairID), ChairID);
		}
		if (iOpType == TYPE_HU)
		{
			Table::DoSimpleOperate(ChairID, iOpType, bCard);
		}
		else if (iOpType == TYPE_HAIDI_YAO)
		{
			m_HaidiPlayer = ChairID;
			Table::OnUserGetCard(ChairID);
		}			
		else //iOpType == TYPE_PASS
		{
			if (m_HaidiPlayer == INVALID_CHAIRID)
			{
				m_bUserOperate[ChairID] = TYPE_NULL;
				m_bCurChairID = NextChair(ChairID);
				OnUserGetCard(m_bCurChairID);
			}
			else
			{
				Table::DoSimpleOperate(ChairID, iOpType, bCard);		
			}			
		}	
		return;
	}
	if ((m_bUserOperate[ChairID] & TYPE_BU_ZHANG) && (iOpType & TYPE_BU_ZHANG))
	{
		m_buzhang[ChairID].buzhang.Add(bCard);
		if ((m_cur_gang_player != INVALID_CHAIRID) && (m_bCurChairID == ChairID))
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
		if (m_HuFanNums[ChairID] >= CHANGSHA_DAHU_FAN)
		{
			m_louhu[ChairID].Lock(FW_LOCK_ALL_CARD, FW_LOCK_ANY_POINT);
		}
		else
		{
			m_louhu[ChairID].Lock(FW_LOCK_ALL_CARD, CHANGSHA_DAHU_FAN);
		}		
	}
	//if (m_gang_get_multicard[ChairID])
	if (m_cur_gang_player != INVALID_CHAIRID)
	{
		if (m_cur_gang_player != ChairID)
		{
			if (m_HuWeiDa)
			{
				if (iOpType == TYPE_PASS)
				{					
					Table::DoSimpleOperate(m_cur_gang_player, m_HuWeiDaOp, m_HuWeiDaCard);
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
		if ((m_cur_gang_player == ChairID) && (iOpType == TYPE_PASS))
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
		if ((m_cur_gang_player == ChairID) && (iOpType & (TYPE_BU_ZHANG | TYPE_GANG)))
		{
			if (Check_HuWeiDa(ChairID, m_gang_hou_card.Size()))
			{
				m_HuWeiDaCard = bCard;
				m_HuWeiDaOp = iOpType;
				return;
			}				
		}
		if ((m_cur_gang_player == ChairID) && (iOpType & TYPE_BU_ZHANG))
		{
			GangHouClear();
		}
	}
	Table::DoSimpleOperate(ChairID, iOpType, bCard);	
}
void Table_changsha::MultiOperateOver(_uint8 chairID, int iOpType, _uint8 bCard)
{
	if (m_cur_gang_player != INVALID_CHAIRID)
	{
		GangHouClear();
	}
	Table::MultiOperateOver(chairID, iOpType, bCard);
	
}
void Table_changsha::OnUserGetCard(_uint8 ChairID, bool bGetLast)
{		
	if (m_CardPool.IsLastCard())
	{
		if (!m_bAddFanFlag[ChairID])
		{
			if (m_HaidiUser.Size() < m_GAME_PLAYER)
			{
				m_HaidiUser.Add(ChairID);
				Send_HaidiCardMsg(ChairID);
				return;
			}
			else
			{//海底牌4个玩家都不要
				//Table::GetOneCard();
				m_CardPool.GetOneCard();//抛弃最后一张牌
				GameEnd();
				return;
			}
		}
		else if (m_conf_changsha.bao_liu_hai_di_niao)
		{
			m_bAddFanFlag[ChairID] = false;
			m_cur_gang_player = INVALID_CHAIRID;
			m_bCurChairID = NextChair(m_bCurChairID);
			OnUserGetCard(m_bCurChairID);
			return;
		}
	}	
	if (m_qishou_banpai[ChairID].NotNull())
	{
		Send_BanpaiOver(ChairID);		
	}
	
	//if (m_gang_get_multicard[ChairID])
	if (m_cur_gang_player != INVALID_CHAIRID)
	{//杠牌获取多张
		m_gang_shai = m_GameLogic.GetShaizi(CHANGSHA_SHAIZI_COUNT);

		_uint8 shai = MIN(m_gang_shai.Shai[0], m_gang_shai.Shai[1]);
		_uint8 dun;
		if (m_conf_changsha.bao_liu_hai_di_niao && m_CardPool.Count() != 0)
		{			
			_uint8 tmp_count = m_CardPool.Count() - 1;
			dun = tmp_count / 2 + tmp_count % 2;
		}
		else
		{
			dun = m_CardPool.Count() / 2 + m_CardPool.Count() % 2;
		}
		if (shai >= dun)
		{//打筛子不中，下家摸牌
			log_info("uid:%d dashai buzhong shai %d  dun %d, ganghou buzhang %d\n", GetPlayerUid(m_cur_gang_player), shai, dun, m_conf_changsha.gang_hou_bu_zhang);
			Send_GangDashaizi(ChairID, m_gang_shai, false);
			GangHouClear();

			//清除碰的标志位
			m_bAddFanFlag[ChairID] = false;

			m_bCurChairID = NextChair(ChairID);
			return OnUserGetCard(m_bCurChairID);
		}
		//assert(m_CardPool.Count() >= shai * 2);		
		memset(&m_bUserOperate, 0, sizeof(m_bUserOperate));

		m_bCurChairID = ChairID;
		m_Gang_buzhang_res = MIN((m_CardPool.Count() - (shai * 2) - (m_conf_changsha.bao_liu_hai_di_niao ? 1 : 0)),
			m_conf_changsha.gang_hou_bu_zhang);

	//	assert(m_Gang_buzhang_res <= m_conf_changsha.gang_hou_bu_zhang);
		Send_GangDashaizi(ChairID, m_gang_shai, true);		
		return;
	}
	if (m_conf_changsha.zhong_tu_si_xi && m_CardPool.Count() > 0)
	{		
		_uint8 bCard = bGetLast ? m_CardPool.LastCard() : m_CardPool.CurCard();
		m_GameLogic.AddCard(&m_HandCard[ChairID], bCard);
		MJ_opinfo opinfo;
		int iOpType = m_GameLogic.GetGangInfo(&m_HandCard[ChairID], opinfo);
		m_GameLogic.ChuPai(&m_HandCard[ChairID], bCard);
		if (iOpType & TYPE_ANGANG)
		{			
			for (int i = 0; i < opinfo.Size(); i++)
			{
				if (opinfo[i].nState == TYPE_ANGANG && !m_SiXi[ChairID].Find(opinfo[i].cbCard))
				{
					m_qishou_banpai[ChairID].Add(opinfo[i].cbCard);
					m_qishou_banpai[ChairID].Add(opinfo[i].cbCard);
					m_qishou_banpai[ChairID].Add(opinfo[i].cbCard);
					m_qishou_banpai[ChairID].Add(opinfo[i].cbCard);
					m_SiXi[ChairID].Add(opinfo[i].cbCard);
					m_qishou_fan[ChairID].Add(MJ_FAN_CHANGSHA_ZHONGTU_SIXI);
				}				

			}
			if (m_qishou_banpai[ChairID].NotNull())
			{
				m_qishou_state = true;

				GetFirstHandCard_NoOp(ChairID);

				if (m_conf.m_bSupportZhuaniao)
				{
					m_qishou_shai = m_GameLogic.GetShaizi(m_conf.m_ZhuaniaoNum);
				}											
				_uint8 i = ChairID;
				if (m_qishou_fan[i].NotNull())
				{
					QishouZhongNiaoSuanFen(i, m_conf.m_ZhuaniaoNum, m_qishou_shai);
				}				
				
				Send_BanpaiInfo(ChairID);

				const int tmp_Timer_stamp = 1;
				TIMER_AGAIN(m_qishouhu_Timer, tmp_Timer_stamp);
				return;
			}
		}		
	}
	Table::OnUserGetCard(ChairID, bGetLast);
}

void Table_changsha::OnUserOutCard(_uint8 ChairID, _uint8 bOutCard)
{
	if (m_qishou_banpai[ChairID].NotNull())//摆牌  摆的是起手胡的牌。 
	{
		Send_BanpaiOver(ChairID);		
	}
	MJ_fan_buffer oFan;
	int iOpType = Check_Hu(ChairID, m_HandCard[ChairID], oFan);  //过张胡
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
	Table::OnUserOutCard(ChairID, bOutCard);
}
bool Table_changsha::QishouHu()
{
	_uint8 bCard = m_CardPool.CurCard();
	m_GameLogic.AddCard(&m_HandCard[m_bBanker], bCard);
	for (int i = 0; i < m_GAME_PLAYER; i++)
	{
		if (Hu_Special4Hunan(i, m_qishou_fan[i], m_qishou_banpai[i]))
		{
			m_qishou_state = true;
		}
	}
	m_GameLogic.ChuPai(&m_HandCard[m_bBanker], bCard);

	if (m_qishou_state)
	{
		GetFirstHandCard_NoOp(m_bBanker);

		if (m_conf.m_bSupportZhuaniao)
		{
			m_qishou_shai = m_GameLogic.GetShaizi(m_conf.m_ZhuaniaoNum);
		}
		for (int i = 0; i < m_GAME_PLAYER; i++)
		{
			if (m_qishou_fan[i].NotNull())
			{
				QishouZhongNiaoSuanFen(i, m_conf.m_ZhuaniaoNum, m_qishou_shai);
			}
		}

		//发送板牌信息	
		Send_BanpaiInfo();

		TIMER_AGAIN(m_qishouhu_Timer, m_qishouhu_Timer_stamp);		
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
		iBaseScore += m_ft2fn->GetFan(m_qishou_fan[ChairID][i]);
	}
	iBaseScore *= BankerDouble(m_bBanker, ChairID, m_conf.m_bZhuangXian);
	for (_uint8 c = NextChair(ChairID); c != ChairID; c = NextChair(c))
	{
		_uint8 s = iBaseScore * BankerDouble(m_bBanker, c, m_conf.m_bZhuangXian);
		iScore[ChairID] += s;
		iScore[c] -= s;
	}
	
	if (niaonum == 1)
	{//如果是1个鸟，就把筛子点数加起来算
		shai.Shai[0] += shai.Shai[1] + 1;
	}	
	
	Calc_Score(m_qishou_score, iScore);
	
	_uint8 dis = DisChair(m_bBanker, ChairID);
	for (int i = 0; i < niaonum;i++)
	{		
		if ((shai.Shai[i] % m_GAME_PLAYER) == dis)
		{			
			for (_uint8 c = NextChair(ChairID); c != ChairID; c = NextChair(c))
			{
				m_qishou_score[ChairID] -= iScore[c];//减负的
				m_qishou_score[c] += iScore[c];//加负的
			}						
		}
		else
		{			
			_uint8 c = NextChair(m_bBanker, shai.Shai[i]);
			m_qishou_score[ChairID] -= iScore[c];//减负的
			m_qishou_score[c] += iScore[c];//加负的									
		}		
	}	
	log_info("%s tid:%d [%d,%d,%d,%d]\n", __FUNCTION__, tid, m_qishou_score[0], m_qishou_score[1], m_qishou_score[2], m_qishou_score[3]);
}
void Table_changsha::Send_QishouDaShaizi(Shai_Set shai)
{
	
	proto::game::hunanmj::tagDaShai stDaShai;
	if (m_conf.m_bSupportZhuaniao)
	{		
		for (int i = 0; i < shai.Num; i++)
		{
			stDaShai.add_shai(shai.Shai[i] + 1);
		}		
	}	
	stDaShai.set_chairid(m_bBanker);
	if (m_conf.m_ZhuaniaoNum == 1)
	{
		_uint8 pos = shai.Shai[0] + shai.Shai[1] + 1;
		stDaShai.add_zhongchairid(NextChair(m_bBanker, pos));
	}
	else
	{
		for (int i = 0; i < shai.Num; i++)
		{
			stDaShai.add_zhongchairid(NextChair(m_bBanker, shai.Shai[i]));
		}
	}
	
	stDaShai.set_type(1);//抓鸟
	for (int i = 0; i < m_GAME_PLAYER; i++)
	{
		stDaShai.add_score(m_qishou_score[i]);		
	}
	stDaShai.set_zhongpai(1);

	broadcast(SERVER_DA_SHAIZI, stDaShai);	

	SaveVideoData(SERVER_DA_SHAIZI, stDaShai);
}
void Table_changsha::Send_BanpaiInfo(_uint8 ChairID)
{
	proto::game::hunanmj::tagBanPai banpai;
	if (ChairID == INVALID_CHAIRID)
	{		
		for (int i = 0; i < m_GAME_PLAYER; i++)
		{
			if (m_qishou_fan[i].NotNull())
			{
				proto::game::hunanmj::tagBanInfo* info = banpai.add_banpai();
				info->set_chairid(i);
				//assert(m_qishou_banpai[i].NotNull());
				for (int j = 0; j < m_qishou_banpai[i].Size(); j++)
				{
					info->add_showcard(m_qishou_banpai[i][j]);
				}
			//	assert(m_qishou_fan[i].NotNull());
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
	//	assert(m_qishou_banpai[ChairID].NotNull());
		for (int j = 0; j < m_qishou_banpai[ChairID].Size(); j++)
		{
			info->add_showcard(m_qishou_banpai[ChairID][j]);
		}
	//	assert(m_qishou_fan[ChairID].NotNull());
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
	{
		m_qishou_fan[i].Clear();
	}
	memset(m_qishou_score, 0, sizeof(m_qishou_score));
	m_qishou_state = false;

	
}
void Table_changsha::Qishou_Timer_cb(struct ev_loop *loop, struct ev_timer *w, int revents)
{
	Table_changsha* table = (Table_changsha*)w->data;

	ev_timer_stop(zjh.loop, &table->m_qishouhu_Timer);

	table->Send_QishouDaShaizi(table->m_qishou_shai);	

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
void Table_changsha::Qishou_Timer_cb_0(struct ev_loop *loop, struct ev_timer *w, int revents)
{
	Table_changsha* table = (Table_changsha*)w->data;

	ev_timer_stop(zjh.loop, &table->m_qishouhu_Timer_0);

	table->Update_Score(table->m_qishou_score);
	memset(table->m_qishou_score, 0, sizeof(table->m_qishou_score));

	//开始判断庄家牌操作
	//_uint8 ChairID = table->m_bBanker;
	_uint8 ChairID = table->m_bCurChairID;
	MJ_major major;
	table->m_bUserOperate[ChairID] = table->GetCardEstimate(ChairID, INVALID_MJ_CARD, major, table->m_HuFanNums[ChairID]);

	for (int i = 0; i < table->m_GAME_PLAYER; i++)
	{
		proto::game::AckOpNotify stNotify;
		_uint8 CurChairID = i;
		//table->Proto_Game_OpNotify(ChairID, i, table->m_bCurCard, stNotify);

		stNotify.Clear();
		stNotify.set_chairid(ChairID);

		if (ChairID == CurChairID)
		{//非打牌玩家
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
			table->m_bUserOperate[ChairID] = iOpType;
		}

		table->unicast(SERVER_OPERATE_NOTIFY, i, stNotify);
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
			info->add_jinzhihuanpai((m_buzhang[i].ting_ed || m_HaidiPlayer == ChairID) ? 1 : 0);
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
		{//修改杠牌信息 为补张信息
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
		
		tagBanPai* banpai = info->mutable_banpai();
		for (int i = 0; i < m_GAME_PLAYER; i++)
		{
			if (m_qishou_banpai[i].NotNull())
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
	
}
void Table_changsha::Proto_GameScene_Operate(_uint8 ChairID, Player* player, proto::game::AckGameScene& stGameScene)
{
	Table::Proto_GameScene_Operate(ChairID, player, stGameScene);
	if (m_bMultiOpUserNum != 0 && m_bMultiUserDoneOp[ChairID] != TYPE_NULL)
	{
		return;
	}
	stGameScene.set_optype(m_bUserOperate[ChairID]);
	AckCustomiedInfo* info = stGameScene.mutable_hunanscene();
	if (info != NULL)	
	{
		if (m_HuWeiDaCard != INVALID_MJ_CARD && m_HuWeiDaOp != TYPE_NULL && m_cur_gang_player == ChairID)
		{
			stGameScene.clear_ganginfo();
			stGameScene.clear_optype();
		}
		else if (m_cur_gang_player != INVALID_CHAIRID)
		{
			stGameScene.clear_ganginfo();
			info->clear_bu();
			stGameScene.clear_optype();

			tagGang_Zhuapai* zhuapai = info->mutable_zhuapai();
			zhuapai->set_chairid(ChairID);
			zhuapai->set_leftcardnum(m_CardPool.Count());
			for (int i = 0; i < m_gang_hou_card.Size();i++)
			{
				zhuapai->add_card(m_gang_hou_card[i]);

				if (m_bUserOperate[ChairID] != TYPE_NULL)
				{
					GangAckOpNotify* pNotify = zhuapai->add_opnotify();
					pNotify->set_chairid(ChairID);					
					pNotify->set_card(m_gang_hou_card[i]);
					pNotify->set_optype(m_gang_hou_op[ChairID][i]);
					if (m_cur_gang_player == ChairID)
					{
						if (m_gang_hou_op[ChairID][i] & TYPE_GANG)
						{
							MJ_opinfo stGang;
							m_GameLogic.AddCard(&m_HandCard[ChairID], m_gang_hou_card[i]);
							m_GameLogic.GetGangInfo(&m_HandCard[ChairID], stGang);

							proto::game::hunanmj::tagBuZhang* bu = pNotify->mutable_bu();
							for (int j = 0; j < stGang.Size(); j++)
							{
								if (m_buzhang[ChairID].ting_ed && !m_gang_hou_card.Find(stGang[j].cbCard))
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
							log_info("%s player uid:%d chairID:%d  gangplayer:%d, card:0x%x, optype:0x%x\n", __FUNCTION__, GetPlayerUid(ChairID), ChairID, m_cur_gang_player, m_gang_hou_card[i], m_gang_hou_op[ChairID][i]);
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
						log_info("%s player uid:%d chairID:%d gang_player:%d, card:0x%x, optype:0x%x\n", __FUNCTION__, GetPlayerUid(ChairID), ChairID, m_cur_gang_player, m_gang_hou_card[i], m_gang_hou_op[ChairID][i]);
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
						if (m_buzhang[ChairID].ting_ed && stGang[i].cbCard != m_bCurCard)
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
		return;
	}	
//	assert(m_bUserOperate[ChairID] == TYPE_NULL);
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
	}

	log_info("%s uid:%d chairid:%d getcardcount:%d\n", __FUNCTION__, GetPlayerUid(ChairID), ChairID, iGetCount);
	for (_uint8 c = NextChair(ChairID); c != ChairID; c = NextChair(c))
	{
		unicast(SERVER_GANG_MO, c, zhuapai);
	}

	for (int i = 0; i < iGetCount; i++)
	{		
		MJ_major major;
		int iFanNums = 0;
		
		int iOpType = GetCardEstimate(ChairID, m_gang_hou_card[i], major, iFanNums);
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
				if (m_buzhang[ChairID].ting_ed && !m_gang_hou_card.Find(stGang[j].cbCard))
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
		log_info("%s uid:%d chairid:%d card:0x%x, iOpType:0x%x\n", __FUNCTION__, GetPlayerUid(ChairID), ChairID, m_gang_hou_card[i], iOpType);
	}
	unicast(SERVER_GANG_MO, ChairID, zhuapai);	

	SaveVideoData(SERVER_GANG_MO, zhuapai);
}
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
void Table_changsha::Gang_Dashaizi_Timer_cb(struct ev_loop *loop, struct ev_timer *w, int revents)
{

	Table_changsha* table = (Table_changsha*)w->data;

	TIMER_STOP(table->m_gang_dashaizi_Timer);

	if (table->m_conf_changsha.gang_hou_zhi_neng_hu0)
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
	log_info("%s user out multi card uid:%d chairID:%d\n", __FUNCTION__, GetPlayerUid(ChairID), ChairID);
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
		OnUserGetCard(m_bCurChairID);
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
		iOpType = OutCardEstimate(ChairID, m_gang_hou_card[i], major, iFanNum);		
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
				if (IsListening(ChairID, m_gang_hou_card[i]))
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

		log_info("%s uid:%d chairid:%d card:0x%x, iOpType:0x%x\n", __FUNCTION__, GetPlayerUid(ChairID), ChairID, m_gang_hou_card[i], iOpType);
	}
		
}

void Table_changsha::GangHouClear()
{
	m_cur_gang_player = INVALID_CHAIRID;
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
		log_info("%s tid:%d, [%d,%d,%d,%d]\n", __FUNCTION__, tid, iScore[0], iScore[1], iScore[2], iScore[3]);
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
			log_info("%s changsha set qiangganghu true\n", __FUNCTION__);
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
		if (Check_Hu(ChairID, tempHandCard, fan) == TYPE_HU)
		{
			log_info("%s uid:%d chairdid:%d Remove:0x%02x\n", __FUNCTION__, GetPlayerUid(ChairID), ChairID, bRemoveCard);
			return true;
		}
		m_GameLogic.ChuPai(&tempHandCard, _relativity[i]);
	}
	return false;
}