#include "table_xianningmajiang.h"
#include "../proto/proto.h"
#include "../zjh.h"
#include "../common/log.h"
#include "../game.h"
#include "../libnormalmahjong/mj_common2.h"
#include "../libnormalmahjong/game_logic.h"
#include "../table_helper.h"
#include "../libnormalmahjong/common.h"



using namespace proto::game::xianningmj;
using namespace proto::game::hunanmj;

extern ZJH zjh;
extern Log log;

class XianNing_Fantype_to_FanNum : public Fantype_to_FanNum
{

public:
	XianNing_Fantype_to_FanNum()
	{			
		fan_num[MJ_FAN_TYPE_PINGHU] = 0;//平胡
		fan_num[MJ_FAN_TYPE_PENGPENGHU]  = 1;//砰砰胡
		fan_num[MJ_FAN_TYPE_QINGYISE] = 1;//清一色
		fan_num[MJ_FAN_XIANNING_258JIANGYISE] = 1;//将一色
		fan_num[MJ_FAN_TYPE_GANGSHANGHUA] = 1;//杠上花
		fan_num[MJ_FAN_TYPE_QUANQIUREN] = 1;//全求人
		fan_num[MJ_FAN_TYPE_HAIDILAOYUE] = 1;//海底捞月
		fan_num[MJ_FAN_TYPE_QIANGGANGHU] = 1;//抢杠胡
		fan_num[MJ_FAN_XIANNING_HEIHU] = 1;//黑胡
		fan_num[MJ_FAN_XIANNING_SIGUIJIAYIFAN] = 1;//四鬼加一蕃
		fan_num[MJ_FAN_TYPE_QIDUI] = 1;//7对
		fan_num[MJ_FAN_XIANNING_SIHONGZHONG] = 1;//四红中

		SetFan(MJ_FAN_TYPE_PINGHU, 0);
		SetFan(MJ_FAN_TYPE_PENGPENGHU, 1);
		SetFan(MJ_FAN_TYPE_QINGYISE, 1);
		SetFan(MJ_FAN_XIANNING_258JIANGYISE, 1);
		SetFan(MJ_FAN_TYPE_GANGSHANGHUA, 1);
		SetFan(MJ_FAN_TYPE_QUANQIUREN, 1);
		SetFan(MJ_FAN_TYPE_HAIDILAOYUE, 1);
		SetFan(MJ_FAN_TYPE_QIANGGANGHU, 1);
		SetFan(MJ_FAN_XIANNING_HEIHU, 1);
		SetFan(MJ_FAN_XIANNING_SIGUIJIAYIFAN, 1);
		SetFan(MJ_FAN_TYPE_QIDUI, 1);
		SetFan(MJ_FAN_XIANNING_SIHONGZHONG, 1);

	}

	bool FanFilter(const MJ_fan_buffer& oFan, MJ_fan_buffer& oOutFan)
	{
		//进行加番处理
		oOutFan.Clear();
		if (oFan.Size()== 1)
		{
			for (int i = 0; i < oFan.Size(); i++)
			{
				oOutFan.Add(oFan[i]);
			}
		}
		else
		{
			for (int i = 0; i < oFan.Size(); i++)
			{
				oOutFan.Add(oFan[i]);

			}
			oOutFan.Remove(MJ_FAN_TYPE_PINGHU);
		}

		return !(oOutFan.IsNull());
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
		//throw("out of range!\n");
		return 0;
	}
};
static XianNing_Fantype_to_FanNum g_xianning_ft2fn;

int Table_xianningmajiang::Calc_Fan(_uint8 ChairID, MJ_fan_buffer& oFan)
{

	if (m_conf_xianningmj.mj_conf_xianningmajiang==true)
	{
		int iTotalFan = 0;
		for (int i = 0; i < oFan.Size(); i++)
		{
			iTotalFan += m_ft2fn->GetFan(oFan[i]);
		}

		for (int i = 0; i < m_stGang[ChairID].Gang.Size(); i++)
		{
			if (m_stGang[ChairID].Gang[i].iType == TYPE_ANGANG)
			{
				iTotalFan += 2;
			}
			else if (m_stGang[ChairID].Gang[i].iType == TYPE_ZHIGANG)
			{
				iTotalFan += 1;
			}
			else if (m_stGang[ChairID].Gang[i].iType == TYPE_WANGANG)
			{
				iTotalFan += 1;
			}
		}
		iTotalFan += m_bUserOutGuiCardNum[ChairID] * 2 + m_bUserOutHongZhongCardNum[ChairID];
		return iTotalFan;
	}
	else
	{
		int iTotalFan = 0;
		for (int i = 0; i < oFan.Size(); i++)
		{
			iTotalFan += m_ft2fn->GetFan(oFan[i]);
		}
		return iTotalFan;
	}



}

int Table_xianningmajiang::Calc_GangFan(_uint8 ChairID)
{
	int iTotalFan = 0;
	for (int i = 0; i < m_stGang[ChairID].Gang.Size(); i++)
	{
		if (m_stGang[ChairID].Gang[i].iType == TYPE_ANGANG)
		{
			iTotalFan +=2;

		}else if (m_stGang[ChairID].Gang[i].iType == TYPE_ZHIGANG)
		{
			iTotalFan += 1;
		}
		else if (m_stGang[ChairID].Gang[i].iType == TYPE_WANGANG)
		{
			iTotalFan += 1;
		}
	}

	iTotalFan += m_bUserOutGuiCardNum[ChairID] * 2 + m_bUserOutHongZhongCardNum[ChairID];

	return iTotalFan;

}


void Table_xianningmajiang::ConfInit_Remote(const Json::Value& val)
{
	Table::ConfInit_Remote(val);
	m_conf.m_bSupportChi = true;	 //是否能吃	
	m_conf.m_bZimoHu = false;		//是否只能自摸胡	
	m_conf.m_bYiPaoDuoXiang = false; //抢杠胡支持一炮多响
	m_conf.m_bHu13yao = false;		//
	m_conf.m_bHu7dui = false;
	m_conf.m_bZhuangXian = false;	//庄家闲家分开计算
	m_conf.m_bQiangGangHu = true;
	m_conf.m_bQiShou_14_zhang = false;
	m_conf.m_AnGangPoint = 1;
	m_conf.m_ZhiGangPoint = 3;
	m_conf.m_WanGangPoint = 1;
	m_conf.m_bQiangZhiGang = true;
	m_conf.m_bQiangAnGang = false;

	if (json_check_play_type_id(val))
	{
		MJ_SUB_ID subid;
		int iCounts = json_get_play_type_id(val, 5380, subid);
		if (iCounts != 0)
		{
			if (subid.Find(5380001))
			{
				m_GAME_PLAYER = 4;
			}
			else if (subid.Find(5380002))
			{
				m_GAME_PLAYER = 2;
			}
			m_conf_xianningmj.mj_conf_xianningmajiang = true;
			m_CardPool.Set(CARD_POOL_NO_WIND_DRAGON_FLOWER_ADD_ZHONG, tid); //设置牌的数量,筒条万+4张红中
			m_room_config.Add(subid[0]);
		}

		iCounts = json_get_play_type_id(val, 5381, subid);
		if (iCounts != 0)
		{
			if (subid.Find(5381001))
			{
				m_BaseScore = 1;
			}
			else if (subid.Find(5381002))
			{
				m_BaseScore = 2;
			}
			else if (subid.Find(5381003))
			{
				m_BaseScore = 5;
			}
			else if (subid.Find(5381004))
			{
				m_BaseScore = 10;
			}
			m_room_config.Add(subid[0]);
		}
		iCounts = json_get_play_type_id(val, 5385, subid);
		if (iCounts != 0)
		{//咸宁转转
			if (subid.Find(5385001))
			{
				m_GAME_PLAYER = 4;
			}
			else if (subid.Find(5385002))
			{
				m_GAME_PLAYER = 3;
			}
			else if (subid.Find(5385003))
			{
				m_GAME_PLAYER = 2;
			}
			m_room_config.Add(subid[0]);
			m_conf_xianningmj.mj_conf_xianningzhuanzhuan = true;
			m_conf.m_bSupportChi = false;//转转不能吃
			m_conf.m_bYiPaoDuoXiang = true; //一炮多响
			m_conf.m_bHuangGang = false;//转转流局时计算杠分。
			m_conf.m_AnGangPoint = 2;
			m_conf.m_ZhiGangPoint = m_GAME_PLAYER-1;
			m_conf.m_WanGangPoint = 1;
		}

		iCounts = json_get_play_type_id(val, 5386, subid);
		if (iCounts != 0)
		{//是否可以点炮
			if (subid.Find(5386001))
			{
				m_conf.m_bZimoHu = false;		//可点炮	
			}
			else if (subid.Find(5386002))
			{
				m_conf.m_bZimoHu = true;		//不可点炮	
			}
			m_room_config.Add(subid[0]);
		}

		iCounts = json_get_play_type_id(val, 5387, subid);
		if (iCounts != 0)
		{//是否可以点炮
			if (subid.Find(5387001))
			{
				m_CardPool.Set(CARD_POOL_NO_WIND_DRAGON_FLOWER_ADD_ZHONG, tid); //设置牌的数量,筒条万+4张红中
				m_bXianNingGuiPai = 0x41;
			}
			else
			{
				m_CardPool.Set(CARD_POOL_NO_WIND_DRAGON_FLOWER, tid); //设置牌的数量,筒条万
			}
			if (subid.Find(5387002))
			{
				m_conf.m_bHu7dui = true; //可以胡七对
			}
			if (subid.Find(5387003))
			{
				m_conf.m_bZhuangXian = true;	//庄家闲家分开计算
			}
			for (int i = 0; i < subid.Size();i++)
			{
				m_room_config.Add(subid[i]);
			}
		}
		iCounts = json_get_play_type_id(val, 5388, subid);
		if (iCounts != 0)
		{//s是否抓鸟
			if (subid.Find(5388001))
			{
				m_conf_xianningmj.mj_conf_zhuanzhuan_zhuaniao = 0;

			}else if (subid.Find(5388002))
			{
				m_conf_xianningmj.mj_conf_zhuanzhuan_zhuaniao = 2;
			}
			else if (subid.Find(5388003))
			{
				m_conf_xianningmj.mj_conf_zhuanzhuan_zhuaniao = 4;
			}
			else if (subid.Find(5388004))
			{
				m_conf_xianningmj.mj_conf_zhuanzhuan_zhuaniao = 6;
			}

			m_room_config.Add(subid[0]);
		}
		iCounts = json_get_play_type_id(val, 5382, subid);
		if (iCounts != 0)
		{//s是否抓鸟
			if (subid.Find(5382001))
			{
				m_conf_xianningmj.mj_conf_xianningmajiang_hongzhonglaizi_gang = true;
				//m_room_config.Add(subid[0]);
			}
			if (subid.Find(5382002))
			{	
				m_conf.m_bQiangAnGang = true;
			}
			for (int i = 0; i < subid.Size(); i++)
			{
				m_room_config.Add(subid[i]);
			}
		}

	}
}

Table_xianningmajiang::Table_xianningmajiang()
{	
	m_ft2fn = &g_xianning_ft2fn;
	m_bXianNingGuiPai = INVALID_MJ_CARD;
	memset(&m_bUserHasGuiPaiNum, 0, sizeof(m_bUserHasGuiPaiNum));
	m_bXianNingFanGui = INVALID_MJ_CARD;
	memset(m_bUserOperatePengCardInfo, 0, sizeof(m_bUserOperatePengCardInfo));
	memset(&m_bUserOperateGuoZhangHuFlag, 0, sizeof(m_bUserOperateGuoZhangHuFlag));//过张胡
	memset(&m_bUserOutGuiCardNum, 0, sizeof(m_bUserOutGuiCardNum));
	memset(&m_bUserOutHongZhongCardNum, 0, sizeof(m_bUserOutHongZhongCardNum));//
	memset(&m_bUserOperateLzHzGangFlag, 0, sizeof(m_bUserOperateLzHzGangFlag));//
	memset(&m_bGetMaPai, 0, sizeof(m_bGetMaPai));
	memset(&m_bZhongMaPai, 0, sizeof(m_bZhongMaPai));//
	memset(&m_bUserGangOperatePengFlag, 0, sizeof(m_bUserGangOperatePengFlag));//
	memset(&m_bUserQingYiSeAutoWarning, 0, sizeof(m_bUserQingYiSeAutoWarning));//
	m_bXianNingZhiGangCard = INVALID_MJ_CARD;
	memset(&m_bUserQuanQiuRenAutoWarning, 0, sizeof(m_bUserQuanQiuRenAutoWarning));//
	memset(&m_bUserOutHongZhongLaiZiCardFlag, 0, sizeof(m_bUserOutHongZhongLaiZiCardFlag));//
	memset(&M_bUserDoMultiOperate, 0, sizeof(M_bUserDoMultiOperate));//
	memset(&m_bUser258JiangYiSeAutoWarning, 0, sizeof(m_bUser258JiangYiSeAutoWarning));//
	memset(&m_bUserOperateGuoZhangHuTotalFen, 0, sizeof(m_bUserOperateGuoZhangHuTotalFen));//过张时的蕃
	m_RobAnGangHuNum = 0;
	memset(&M_bSendUserQiangAnGangHuFlag, 0, sizeof(M_bSendUserQiangAnGangHuFlag));//
	memset(&M_bUserDoMultiOperateQiangAnGang, TYPE_NULL, sizeof(M_bUserDoMultiOperateQiangAnGang));//
	memset(&M_bCalcUserQiangAnGangHuFlag, 0, sizeof(M_bCalcUserQiangAnGangHuFlag));//
	memset(&m_bQiangAnGangTingCard, 0, sizeof(m_bQiangAnGangTingCard));//

	m_GetCardOperateCard_Timer_stamp = 2;
	m_GetCardOperateCard_Timer.data = this;
	ev_timer_init(&m_GetCardOperateCard_Timer, Table_xianningmajiang::GetCardOperateCard_Timer_cb, m_GetCardOperateCard_Timer_stamp, m_GetCardOperateCard_Timer_stamp);
	memset(&m_OnUserGetFenZhangCard, 0, sizeof(m_OnUserGetFenZhangCard));//
	memset(&m_OnUserQiangAnGangCard, 0, sizeof(m_OnUserQiangAnGangCard));//
	memset(&m_OnUserQiangAnCalcScoreFlag, 0, sizeof(m_OnUserQiangAnCalcScoreFlag));//

}


void Table_xianningmajiang::GameEndReset()
{
	Table::GameEndReset();
	m_ft2fn = &g_xianning_ft2fn;
	memset(&m_bUserOperateGuoZhangHuTotalFen, 0, sizeof(m_bUserOperateGuoZhangHuTotalFen));//过张时的蕃

	memset(&m_bUserHasGuiPaiNum, 0, sizeof(m_bUserHasGuiPaiNum));

	memset(m_bUserOperatePengCardInfo, 0, sizeof(m_bUserOperatePengCardInfo));
	memset(&m_bUserOperateGuoZhangHuFlag, 0, sizeof(m_bUserOperateGuoZhangHuFlag));//过张胡
	memset(&m_bUserOutGuiCardNum, 0, sizeof(m_bUserOutGuiCardNum));
	memset(&m_bUserOutHongZhongCardNum, 0, sizeof(m_bUserOutHongZhongCardNum));//
	memset(&m_bUserOperateLzHzGangFlag, 0, sizeof(m_bUserOperateLzHzGangFlag));//
	memset(&m_bGetMaPai, 0, sizeof(m_bGetMaPai));
	memset(&m_bZhongMaPai, 0, sizeof(m_bZhongMaPai));//
	memset(&m_bUserGangOperatePengFlag, 0, sizeof(m_bUserGangOperatePengFlag));//
	memset(&m_bUserQingYiSeAutoWarning, 0, sizeof(m_bUserQingYiSeAutoWarning));//
	if (m_conf_xianningmj.mj_conf_xianningmajiang==true)
	{
		m_bXianNingFanGui = INVALID_MJ_CARD;
		m_bXianNingGuiPai = INVALID_MJ_CARD;
	}
	m_bXianNingZhiGangCard = INVALID_MJ_CARD;
	memset(&m_bUserQuanQiuRenAutoWarning, 0, sizeof(m_bUserQuanQiuRenAutoWarning));//
	memset(&m_bUserOutHongZhongLaiZiCardFlag, 0, sizeof(m_bUserOutHongZhongLaiZiCardFlag));//
	memset(&M_bUserDoMultiOperate, 0, sizeof(M_bUserDoMultiOperate));//
	memset(&m_bUser258JiangYiSeAutoWarning, 0, sizeof(m_bUser258JiangYiSeAutoWarning));//
	m_RobAnGangHuNum = 0;
	memset(&M_bSendUserQiangAnGangHuFlag, 0, sizeof(M_bSendUserQiangAnGangHuFlag));//
	memset(&M_bUserDoMultiOperateQiangAnGang, TYPE_NULL, sizeof(M_bUserDoMultiOperateQiangAnGang));//
	memset(&M_bCalcUserQiangAnGangHuFlag, 0, sizeof(M_bCalcUserQiangAnGangHuFlag));//
	memset(&m_bQiangAnGangTingCard, 0, sizeof(m_bQiangAnGangTingCard));//
	memset(&m_OnUserGetFenZhangCard, 0, sizeof(m_OnUserGetFenZhangCard));//
	memset(&m_OnUserQiangAnGangCard, 0, sizeof(m_OnUserQiangAnGangCard));//
	memset(&m_OnUserQiangAnCalcScoreFlag, 0, sizeof(m_OnUserQiangAnCalcScoreFlag));//

}



Table_xianningmajiang::~Table_xianningmajiang()
{	
	ev_timer_stop(zjh.loop, &m_GetCardOperateCard_Timer);

}

void Table_xianningmajiang::GameStart()
{

	Table::GameStart();	

	if (m_conf_xianningmj.mj_conf_xianningmajiang == true)
	{//咸宁麻将
		GameGetGuiCard();
		/*广播赖子*/
		//m_bXianNingGuiPai = 0x08;
		if (m_bXianNingFanGui != INVALID_MJ_CARD)
		{
			::proto::game::xianningmj::GameLaiziInfo laiZiInfo;
			laiZiInfo.add_laizishow(m_bXianNingFanGui);
			laiZiInfo.add_laizisign(m_bXianNingGuiPai);
			srand(time(NULL));
			laiZiInfo.add_dicelist(rand() % 6 + 1);
			laiZiInfo.add_dicelist(rand() % 6 + 1);
			laiZiInfo.set_flag(true);
			broadcast(SERVER_ROOM_MAJINAG_LAI_ZI, laiZiInfo);
			/*保存录像*/
			SaveVideoData(SERVER_ROOM_MAJINAG_LAI_ZI, laiZiInfo);
			log_info("GameStart tid:%d laiZiInfo = %s",tid, laiZiInfo.DebugString().c_str());

		}
	}
	else
	{//咸宁转转

		if (m_bXianNingGuiPai == 0x41)
		{
			::proto::game::xianningmj::GameLaiziInfo laiZiInfo;
			laiZiInfo.add_laizishow(m_bXianNingGuiPai);
			laiZiInfo.add_laizisign(m_bXianNingGuiPai);
			//srand(time(NULL));
			//laiZiInfo.add_dicelist(rand() % 6 + 1);
			//laiZiInfo.add_dicelist(rand() % 6 + 1);
			laiZiInfo.set_flag(true);
			broadcast(SERVER_ROOM_MAJINAG_LAI_ZI, laiZiInfo);
			/*保存录像*/
			SaveVideoData(SERVER_ROOM_MAJINAG_LAI_ZI, laiZiInfo);
		}
	}
	//log_info("GameStart m_bXianNingGuiPai = %x", m_bXianNingGuiPai);
	OnUserGetCard(m_bBanker);
}

void Table_xianningmajiang::Proto_GameScene(_uint8 ChairID, Player* player, proto::game::AckGameScene& stGameScene)
{

	log_info("%s tid:%d begin to uid:%d chairid:%d gamestate:%d \n", __FUNCTION__,tid, player->uid, ChairID, m_bGameState);
	stGameScene.set_outcardtime(m_Outcard_Timer_stamp);
	stGameScene.set_operatetime(m_Operate_Timer_stamp0);
	stGameScene.set_delaytime(m_bDelayTime);
	stGameScene.set_basescore(m_BaseScore);
	if (m_bDeleteTable)
	{
		stGameScene.set_scenestatus(10);
	}
	else
	{
		stGameScene.set_scenestatus(m_bGameState);
	}
	stGameScene.set_curoutcarduser(m_bOutCardChairID);
	stGameScene.set_curoutcard(m_bCurOutCard);
	//stGameScene.set_optype(m_bUserOperate[ChairID]);
	stGameScene.set_optype(0);
	stGameScene.set_bankerchairid(m_bBanker);
	_uint8 bIsMoPai = (ChairID == m_bCurChairID) ? 1 : 0;//当前摸牌玩家
	stGameScene.set_iscurgetcard(bIsMoPai);
	stGameScene.set_card(m_bCurCard);//摸到的牌
	_uint8 bAutoHu = (m_bAutoHu[ChairID] == true) ? 1 : 0;
	stGameScene.set_isautohu(bAutoHu);
	int nleftTime = 0;
	if (m_bUserOperate[ChairID] == TYPE_NULL || m_bUserOperate[ChairID] == TYPE_LISTEN)
	{
		nleftTime = get_operate_remain_time(TYPE_NULL, true);
		if (nleftTime > m_Outcard_Timer_stamp)
		{
			nleftTime = m_Outcard_Timer_stamp;
		}

	}
	else
	{
		nleftTime = get_operate_remain_time(TYPE_NULL, true);
		if (nleftTime > m_Operate_Timer_stamp0)
		{
			nleftTime = m_Operate_Timer_stamp0;
		}

	}
	stGameScene.set_lefttime(nleftTime);
	log_info("%s tid:%d uid:%d chairid:%d left timer:%d m_bUserOperate:0x%x\n", __FUNCTION__,tid, GetPlayerUid(player), ChairID, stGameScene.lefttime(), m_bUserOperate[ChairID]);

	stGameScene.set_stand_money(stand_money);
	stGameScene.set_poolcardleftnums(m_CardPool.Count());
	stGameScene.set_preready_timer(preready_timer_stamp);

	stGameScene.set_isdisbandroom(m_bIsDisbandTable);
	proto::game::AckDisbandApply *pDisInfo = stGameScene.mutable_disbandinfo();
	pDisInfo->set_applychairid(m_ApplyDisbandChairid);
	int leftchoosetime = Disband_timer_remaintime(ChairID);
	pDisInfo->set_waittime(leftchoosetime);
	for (int i = 0; i < m_GAME_PLAYER; i++)
	{
		pDisInfo->add_onlinechairid(i);
	}

	proto::game::HandCards *pstHc = stGameScene.mutable_usercard();
	Fill_HandCard(ChairID, pstHc);

	for (int i = 0; i < m_GAME_PLAYER; i++)
	{
		stGameScene.add_handcardnums(m_HandCard[i].ChangeableCardsLen);
		stGameScene.add_totalscore(m_stHuScore[i].iTotalScore + m_stGang[i].iTotalScore);
		stGameScene.add_offlinestate(m_IsOffline[i]);

		proto::game::AckDisbandChooseBc *pDisChoose = stGameScene.add_userdisbandstate();
		pDisChoose->set_chairid(i);
		pDisChoose->set_choosestate(m_DisbandChoose[i]);
		proto::game::AckUserOnlineState *pOnlinestate = stGameScene.add_useronlinestate();
		pOnlinestate->set_chairid(i);
		pOnlinestate->set_state(m_IsOffline[i]);
		proto::game::AckUserOvertimeState *pOvertime = stGameScene.add_userovertime();
		pOvertime->set_chairid(i);
		pOvertime->set_state(m_bOverTimeState[i]);
		//打出牌
		proto::game::tagShowOutCard *pstShow = stGameScene.add_showcard();
		for (int j = 0; j < m_stUserOutCard[i].Size(); j++)
		{
			pstShow->add_outcard(m_stUserOutCard[i][j]);
		}
		pstShow->set_outcardlen(m_stUserOutCard[i].Size());
		//碰杠牌
		proto::game::tagGroupCard *pstGr = stGameScene.add_groupcard();
		for (int j = 0; j < m_HandCard[i].FixedCardsLen && j < 4; j++)
		{
			proto::game::FixedCard *pstFix = pstGr->add_fixedcard();
			if (m_conf_xianningmj.mj_conf_xianningmajiang == true)
			{//咸宁麻将暗杠别人不能看到暗杠的pai
				if (i == j)
				{
					Fill_FixedCard(i, j, pstFix);
				}
				else
				{
					Fill_FixedCard_AnGang(ChairID, i, j, pstFix);
				}
			}
			else
			{//
				Fill_FixedCard(i, j, pstFix);
			}
		}
		//胡牌信息
		proto::game::tagUserHuInfo *pstUserHu = stGameScene.add_huinfo();
		for (int j = 0; j < m_stHuScore[i].stHu.Size(); j++)
		{
			proto::game::tagHu *pstHu = pstUserHu->add_hu();
			pstHu->set_hucard(m_stHuScore[i].stHu[j].bHuCard);
			pstHu->set_pashuchiarid(m_stHuScore[i].stHu[j].bPasHuChairID);
		}
	}

	Proto_GameScene_Operate(ChairID, player, stGameScene);

	AckCustomiedInfo* info = stGameScene.mutable_hunanscene();
	if (info != NULL)
	{
		info->set_gametype(proto::game::hunanmj::HUNAN_CHANGSHA);
		info->set_ishavepiao(0);
		info->set_jiangma(proto::game::hunanmj::JIANGMA);//抓鸟
		info->set_laizi(0);//没有癞子

		for (int i = 0; i < m_room_config.Size(); i++)
		{
			info->add_roomconfigid(m_room_config[i]);
		}
	}
	::proto::game::xianningmj::tagGameScene *xianningSceneInfo = stGameScene.mutable_xianninggamescene();
	if (xianningSceneInfo != NULL)
	{
		/*广播赖子*/
		if (m_bXianNingGuiPai != INVALID_MJ_CARD&&m_bGameState == GAME_PLAYING)
		{
			::proto::game::xianningmj::GameLaiziInfo *laiziInfo = xianningSceneInfo->mutable_laizi_info();
			if (m_conf_xianningmj.mj_conf_xianningmajiang == true)
			{
				laiziInfo->add_laizishow(m_bXianNingFanGui);
			}
			
			laiziInfo->add_laizisign(m_bXianNingGuiPai);
			laiziInfo->set_flag(false);
		}
	}


	if (m_conf_xianningmj.mj_conf_xianningmajiang == true)
	{
		::proto::game::xianningmj::RspGamePlayerAlertingInfo  * PlayerAlertingInfo[GAME_PLAYER];

		for (int i = 0; i < m_GAME_PLAYER; i++)
		{
			PlayerAlertingInfo[i] = xianningSceneInfo->add_playeralertinginfo();
			if (m_bUserQingYiSeAutoWarning[i] == true)
			{
				if (m_HandCard[i].FixedCardsLen >= 2)
				{
					PlayerAlertingInfo[i]->set_seatid(i);
					PlayerAlertingInfo[i]->set_flag(true);

				}
			}
		}
	}

	if (m_conf_xianningmj.mj_conf_xianningmajiang == true)
	{
		::proto::game::xianningmj::RspGamePlayerAlertingInfo  * PlayerAlertingInfo[GAME_PLAYER];

		for (int i = 0; i < m_GAME_PLAYER; i++)
		{
			PlayerAlertingInfo[i] = xianningSceneInfo->add_playeralertinginfo();
			if (m_bUser258JiangYiSeAutoWarning[i] == true)
			{
				if (m_HandCard[i].FixedCardsLen >= 2)
				{
					PlayerAlertingInfo[i]->set_seatid(i);
					PlayerAlertingInfo[i]->set_flag(true);
				}
			}
		}
	}
	if (m_conf_xianningmj.mj_conf_xianningmajiang == true)
	{
		::proto::game::xianningmj::RspGamePlayerAlertingInfo  * PlayerAlertingInfo[GAME_PLAYER];

		for (int i = 0; i < m_GAME_PLAYER; i++)
		{
			PlayerAlertingInfo[i] = xianningSceneInfo->add_playeralertinginfo();
			if (m_bUserQuanQiuRenAutoWarning[i] == true)
			{
				if (m_HandCard[i].FixedCardsLen == 4)
				{
					PlayerAlertingInfo[i]->set_seatid(i);
					PlayerAlertingInfo[i]->set_flag(true);
				}
			}
		}
	}
	if (M_bSendUserQiangAnGangHuFlag[ChairID] == true)
	{
		for (int i = 0; i < m_bQiangAnGangTingCard[ChairID].Size(); i++)
		{
			xianningSceneInfo->add_bangangting(m_bQiangAnGangTingCard[ChairID][i]);
		}

	}

}

void Table_xianningmajiang::Proto_GameScene_Operate(_uint8 ChairID, Player* player, proto::game::AckGameScene& stGameScene)
{
	
	Table::Proto_GameScene_Operate(ChairID, player, stGameScene);
	if (M_bSendUserQiangAnGangHuFlag[ChairID]==true)
	{
		stGameScene.set_optype(TYPE_YX_YOUJINHU);
	}
}


void Table_xianningmajiang::Fill_FixedCard_AnGang(_uint8 OpChairID, _uint8 ChairID, _uint8 Pos, proto::game::FixedCard* pstFix)
{
	_uint8 cbCard = 0xFF;
	if (m_HandCard[ChairID].FixedCards[Pos].state == TYPE_LEFT_CHI)
	{
		cbCard = m_HandCard[ChairID].FixedCards[Pos].c[0];
	}
	else if (m_HandCard[ChairID].FixedCards[Pos].state == TYPE_CENTER_CHI)
	{
		cbCard = m_HandCard[ChairID].FixedCards[Pos].c[1];
	}
	else if (m_HandCard[ChairID].FixedCards[Pos].state == TYPE_RIGHT_CHI)
	{
		cbCard = m_HandCard[ChairID].FixedCards[Pos].c[2];
	}
	else
	{
		if (m_HandCard[ChairID].FixedCards[Pos].state == TYPE_ANGANG &&OpChairID != ChairID)
		{
			cbCard = 0x00;
		}
		else
		{
			cbCard = m_HandCard[ChairID].FixedCards[Pos].CardData;
		}
	}
	pstFix->set_carddata(cbCard);
	pstFix->set_state(m_HandCard[ChairID].FixedCards[Pos].state);
	pstFix->set_chairid(m_HandCard[ChairID].FixedCards[Pos].chairID);
	//log_info("%s uid:%d ChairID:%d \n", __FUNCTION__, GetPlayerUid(ChairID), ChairID);
}


void Table_xianningmajiang::GameEnd()
{

	if (m_conf_xianningmj.mj_conf_xianningmajiang == false)
	{//转转抓鸟
		GameEndAndGetMaNum();
	}

	//摸完最后一张牌才算是结束
	MJ_user_buffer UserID;
	_uint8 bDraw = (MultiHu(UserID) == 0) ? 1 : 0;//结束状态，1表示流局，0表示胡牌结束	
												  //单发结算协议	
	proto::game::AckGameEnd stGameend;
	HuScoreSet hu_score;

	if (bDraw)
	{
		Score_GameEndDraw(hu_score, stGameend);
	}
	else
	{

		Score_GameEndSucc(hu_score, stGameend);
	}

	log_fatal("%s tid:%d round:%d score[%d,%d,%d,%d]", __FUNCTION__, tid, m_nPlayingTimes, hu_score[0].iTotleScore, hu_score[1].iTotleScore, hu_score[2].iTotleScore, hu_score[3].iTotleScore);

	int UserScore[GAME_PLAYER] = { 0 };//四个玩家输赢总分
	memset(UserScore, 0, sizeof(UserScore));
	for (int i = 0; i < m_GAME_PLAYER; i++)
	{
		log_debug("m_SeatPlayer[i].player->money = %d", m_SeatPlayer[i].player->money);
		stGameend.add_score(hu_score[i].iTotleScore);
		proto::game::HandCards *pHandcard = stGameend.add_usercard();
		Fill_HandCard(i, pHandcard);
		UserScore[i] = stGameend.score(i);
		if (m_SeatPlayer[i].player != NULL)
		{
			if (stGameend.score(i) < 0)
			{
				if (m_SeatPlayer[i].player->money + stGameend.score(i) < 0)
				{
					/*如果总分为负数，则归0*/
					UserScore[i] = -m_SeatPlayer[i].player->money;
				}
			}
		}
	}

	tagUserScore userscore[GAME_PLAYER];
	for (int i = 0; i < m_GAME_PLAYER; i++)
	{
		if (m_SeatPlayer[i].player != NULL)
		{
			userscore[i].uid = m_SeatPlayer[i].player->uid;
			userscore[i].score = UserScore[i];
			if (UserScore[i] > 0)
			{
				m_SeatPlayer[i].player->update_info(UserScore[i], 1, 1, win_exp);
			}
			else
			{
				m_SeatPlayer[i].player->update_info(UserScore[i], 1, 0, lose_exp);
			}

			/*此处去掉情况不明*/
			UpdateUserInfoToUser(m_SeatPlayer[i].player);

		}
	}

	Proto_GameEndInfo(UserID, stGameend);

	stGameend.set_endstate(bDraw);


	for (int i = 0; i < m_GAME_PLAYER; i++)
	{
		Proto_GameEndInfo_GangInfo(i, stGameend);
	}

	if (m_conf_xianningmj.mj_conf_xianningmajiang == false)
	{
		for (int i = 0; i < m_GAME_PLAYER; i++)
		{
			UserScore[i] += m_stGang[i].iTotalScore;
		}
		
	}
	for (int i = 0; i < m_GAME_PLAYER; i++)
	{

		stGameend.set_huscore(m_stHuScore[i].iTotalScore);
		stGameend.clear_score();

		for (int j = 0; j < m_GAME_PLAYER; j++)
		{ 

			stGameend.add_score(UserScore[j]);
		}

		unicast(SERVER_GAME_END, i, stGameend);
		log_fatal("%s tid:%d uid:%d chairid:%d score %d!\n", __FUNCTION__,tid, GetPlayerUid(i), i, UserScore[i]);
	}
	//log_debug("short_info  %s  ", stGameend.ShortDebugString().c_str());
	//发送录像
	save_video_game_end(stGameend);

	/*保存最终分数*/
	update_user_info_to_datasvr_dz(userscore, 1102);

	ev_timer_again(zjh.loop, &send_videodata_timer);

	CalcRecordInfo(UserID, UserScore);

	RedisUpdateScoreToServer(UserScore);

	m_last_Banker = m_bBanker;
	//轮庄

	TurnBanker();
	//初始化数据

	GameEndReset();
}

void Table_xianningmajiang::Fill_HandCard(_uint8 ChairID, proto::game::HandCards* pHandcard)
{
	pHandcard->set_changeablecardslen(m_HandCard[ChairID].ChangeableCardsLen);
	for (int j = 0; j < m_HandCard[ChairID].ChangeableCardsLen; j++)
	{
		pHandcard->add_changeablecards(m_HandCard[ChairID].ChangeableCards[j]);
	}
	for (int j = 0; j < m_HandCard[ChairID].FixedCardsLen; j++)
	{
		proto::game::FixedCard *pFixedcard = pHandcard->add_stfixedcards();
		Fill_FixedCard(ChairID, j, pFixedcard);
	}

	pHandcard->set_fixedcardslen(m_HandCard[ChairID].FixedCardsLen);
	if (m_OnUserGetFenZhangCard[ChairID] > 0)
	{
		pHandcard->set_stfenzhangcard(m_OnUserGetFenZhangCard[ChairID]);
	}

}

void Table_xianningmajiang::GameGetGuiCard()
{
	_uint8 bCard =  m_CardPool.GetOneCard();

	m_bXianNingFanGui = bCard;
	if (bCard == 0x09)
	{
		m_bXianNingGuiPai = 0x01;

	}else if (bCard == 0x19)
	{
		m_bXianNingGuiPai = 0x11;
	}
	else if (bCard == 0x29)
	{
		m_bXianNingGuiPai = 0x21;

	}else if (bCard == 0x41)
	{
		m_bXianNingGuiPai = 0x21;
	}
	else
	{
		m_bXianNingGuiPai = bCard + 1;
	}
}

void Table_xianningmajiang::Proto_GameEndInfo(const MJ_user_buffer& UserID, proto::game::AckGameEnd& stGameend)
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
		xianning->set_laizisign(m_bXianNingGuiPai);
		for (int i = 0; i < m_CardPool.Count();i++)
		{
			xianning->add_remaindercards(m_CardPool[i]);
		}
	}
	else
	{
			::proto::game::xianningmj::GameEndXianNing *xianning = stGameend.mutable_xianninggameend();

			::proto::game::xianningmj::GameEndPlayerScoresInfo *playerScoreInfo[GAME_PLAYER] = { 0 };
			::proto::game::xianningmj::GameMapaiInfo *mapaizhongma;
			::proto::game::xianningmj::PlayerMapaiInfo *zhongmainfo[GAME_PLAYER];

			mapaizhongma = xianning->mutable_gamemapaiinfo();
			for (int j = 0; j < m_GAME_PLAYER; j++)
			{
				playerScoreInfo[j] = xianning->add_playerscoreinfolist();
			}
			for (int j = 0; j < m_GAME_PLAYER; j++)
			{
				playerScoreInfo[j]->set_chairid(j);
				playerScoreInfo[j]->set_gangscore(m_stGang[j].iTotalScore);
				playerScoreInfo[j]->set_huscroe(m_stHuScore[j].iTotalScore);

			}
			for (int i = 0; i < m_bGetMaPai.Size(); i++)
			{
				mapaizhongma->add_publicmapailist(m_bGetMaPai[i]);
			}
			for (int j = 0; j < m_GAME_PLAYER; j++)
			{
				zhongmainfo[j] = mapaizhongma->add_playermapaiinfolist();
			}
			for (int j = 0; j < m_GAME_PLAYER; j++)
			{
				zhongmainfo[j]->set_chairid(j);
				for (int z = 0; z < m_bZhongMaPai[j].Size(); z++)
				{
					zhongmainfo[j]->add_mapailist(m_bZhongMaPai[j][z]);
				}
			}
			for (int i = 0; i < m_CardPool.Count(); i++)
			{
				xianning->add_remaindercards(m_CardPool[i]);
			}

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
	//	assert(m_stHuScore[win_seat].stHu.Size() != 0);
		if (m_stHuScore[win_seat].stHu[0].bType == HU_ZIMO)
		{//自摸胡
			if (m_oFan[win_seat].Find(MJ_FAN_TYPE_GANGSHANGHUA))
			{
				info[win_seat]->set_specialtype(HU_GANGSHANGHU);
			}
			else
			{
				info[win_seat]->set_specialtype(HU_ZIMO);

			}
			info[win_seat]->set_chairid(win_seat);
			
			int iAddFanCount = 0;//垃圾代码
			for (int i = 0; i < tmpFan[win_seat].Size(); i++)
			{
				AddFanInfo* pfan = info[win_seat]->add_addfan();   //  hu fan 
				pfan->set_addtype(tmpFan[win_seat][i]);
				iAddFanCount += BankerScoreAdd(win_seat);//垃圾代码
				pfan->set_addnum(m_ft2fn->GetFan(tmpFan[win_seat][i]) + BankerScoreAdd(win_seat));
			}
			for (_uint8 c = NextChair(win_seat); c != win_seat; c = NextChair(c))
			{
				if (m_oFan[win_seat].Find(MJ_FAN_TYPE_GANGSHANGHUA))
				{
					info[c]->set_specialtype(HU_INVALID);
				}
				else
				{
					info[c]->set_specialtype(HU_BEIZIMO);

				}
				info[c]->set_chairid(c);
				//info[c]->set_specialtype(HU_BEIZIMO);
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

int Table_xianningmajiang::GetGangInfo(_uint8 ChairID, MJ_opinfo& ganginfo)
{
	if (m_CardPool.Count() <= 15)
	{	
		return TYPE_NULL;
	}
	HandCards * pHc = &m_HandCard[ChairID];
	_uint8 bCardCount[MJ_TYPE_COMMON][10];
	memset(bCardCount, 0, sizeof(bCardCount));
	ganginfo.Clear();
	int iOpType = TYPE_NULL;
	/*检查可换牌中是否有暗杠*/
	for (int i = 0; i < pHc->ChangeableCardsLen; i++)
	{
		_uint8 bType = CardType(pHc->ChangeableCards[i]);
		_uint8 bNum = CardNum(pHc->ChangeableCards[i]);

		bCardCount[bType][bNum]++;
		if (bCardCount[bType][bNum] == 4)
		{
			if (pHc->ChangeableCards[i] == 0x41|| pHc->ChangeableCards[i] == m_bXianNingGuiPai)
			{
				continue;
			}
			ganginfo.Add(OpState(pHc->ChangeableCards[i], TYPE_ANGANG));
			iOpType |= TYPE_ANGANG;
		}

	}
	/*检查固定牌中是否有弯杠*/
	for (int i = 0; i < pHc->FixedCardsLen; i++)
	{
		if (pHc->FixedCards[i].state == TYPE_PENG)
		{
			for (int j = 0; j < pHc->ChangeableCardsLen; j++)
			{
				if (pHc->FixedCards[i].CardData == pHc->ChangeableCards[j])
				{
					if (m_bUserGangOperatePengFlag[ChairID].Find(pHc->ChangeableCards[j]) && m_conf_xianningmj.mj_conf_xianningmajiang == false)
					{//转转麻将碰后不能再补杠
						continue;
					}
					ganginfo.Add(OpState(pHc->ChangeableCards[j], TYPE_WANGANG));
					iOpType |= TYPE_WANGANG;
					break;
				}
			}
		}
	}
	return iOpType;
}


int Table_xianningmajiang::OutCardEstimate_QiangGangHu(_uint8 ChairID, _uint8 bCard, MJ_major& major, int& fan_num)
{

	//assert(major.m_oFan.Size() == 0);
	int iOpType = TYPE_NULL;
	HandCards* pHc = &m_HandCard[ChairID];
	iOpType |= GetChi(ChairID, bCard, major.m_Chi);
	iOpType |= GetPengGang(ChairID, bCard, major.m_GetPengGang);
	//log_debug("iOpType = %#x", iOpType);
	//assert(pHc->ChangeableCardsLen < HAND_CARD_SIZE_MAX);
	if (!m_conf.m_bZimoHu)
	{
		m_GameLogic.AddCard(pHc, bCard);
		iOpType |= Check_Hu(ChairID, *pHc, major.m_oFan);
		if (CardPoolIsEmpty())
		{
			UserLastOutCard(ChairID, iOpType, major.m_oFan);
		}
		if (m_bGetCardCount[m_bBanker] == 1)
		{//庄家打出第一张牌
			for (_uint8 c = NextChair(m_bBanker); c != m_bBanker; c = NextChair(c))
			{
				if (m_bGetCardCount[c] != 0)
					goto BANKER_FIRST_OUTCARD;
			}
			//UserFirstOutCard(ChairID, iOpType, major.m_oFan);
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
	int bUserHasGuiPaiNum = 0;
	for (int i = 0; i < m_HandCard[ChairID].ChangeableCardsLen; i++)
	{
		if (m_HandCard[ChairID].ChangeableCards[i] == m_bXianNingGuiPai)
		{
			bUserHasGuiPaiNum++;
		}
	}

	if (major.m_oFan.Find(MJ_FAN_TYPE_QIANGGANGHU)&& major.m_oFan.Find(MJ_FAN_TYPE_PINGHU))
	{
		//major.m_oFan.Remove(MJ_FAN_TYPE_PINGHU);
	}
	return iOpType;
}

int Table_xianningmajiang::OutCardEstimate(_uint8 ChairID, _uint8 bCard, MJ_major& major, int& fan_num)
{
	if (m_conf_xianningmj.mj_conf_xianningmajiang == true)
	{
		return OutCardEstimate_0(ChairID, bCard, major, fan_num);
	}
	else
	{
		return OutCardEstimate_zhuanzhuan(ChairID, bCard, major, fan_num);

	}
}
int Table_xianningmajiang::OutCardEstimate_0(_uint8 ChairID, _uint8 bCard, MJ_major& major, int& fan_num)
{

//	assert(major.m_oFan.Size() == 0);
	int iOpType = TYPE_NULL;
	HandCards* pHc = &m_HandCard[ChairID];

	iOpType |= GetChi(ChairID, bCard, major.m_Chi);

	iOpType |= GetPengGang(ChairID, bCard, major.m_GetPengGang);
	//log_debug("iOpType = %#x", iOpType);
//	assert(pHc->ChangeableCardsLen < HAND_CARD_SIZE_MAX);
	if (!m_conf.m_bZimoHu)
	{
		m_GameLogic.AddCard(pHc, bCard);
		iOpType |= Check_Hu(ChairID, *pHc, major.m_oFan);
		//log_debug("ChairID = %d  iOpType = %#x", ChairID, iOpType);
		if (CardPoolIsEmpty())
		{
			UserLastOutCard(ChairID, iOpType, major.m_oFan);
		}
		if (m_bGetCardCount[m_bBanker] == 1)
		{//庄家打出第一张牌
			for (_uint8 c = NextChair(m_bBanker); c != m_bBanker; c = NextChair(c))
			{
				if (m_bGetCardCount[c] != 0)
					goto BANKER_FIRST_OUTCARD;
			}
			//UserFirstOutCard(ChairID, iOpType, major.m_oFan);
		}
	BANKER_FIRST_OUTCARD:
		if (iOpType & TYPE_HU)
		{
			if (m_conf.m_bGangShangPao)
			{
				if (!m_bRobWanGang && m_bAddFanFlag[m_bCurChairID]) //杠上炮
				{
					//UserGangHouPao(ChairID, bCard, major.m_oFan);
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
	if (iOpType & TYPE_HU)
	{
		if (major.m_oFan.Find(MJ_FAN_TYPE_PINGHU) && IsQuanqiuren(m_HandCard[ChairID]))
		{
			major.m_oFan.Add(MJ_FAN_TYPE_QUANQIUREN);
			major.m_oFan.Remove(MJ_FAN_TYPE_PINGHU);

		}
		else if (IsQuanqiuren(m_HandCard[ChairID]))
		{
			for (int i = 0; i < m_HandCard[ChairID].ChangeableCardsLen; i++)
			{
				if (CardNum(bCard) == 2 || CardNum(bCard) == 5 || CardNum(bCard) == 8)
				{
					//log_info("IsQuanqiuren is OK");
					major.m_oFan.Add(MJ_FAN_TYPE_QUANQIUREN);
				}
			}
		}
		int guozhanghufan = 0;
		if (major.m_oFan.Find(MJ_FAN_TYPE_PINGHU))
		{ //三番起胡
			int hu_fan_num = Calc_Fan(ChairID, major.m_oFan);
			int gang_fan_num = Calc_GangFan(m_bOutCardChairID);
			//gang_fan_num += 1;//点炮玩家1番（大胡加1番，平胡不加）
			for (int i = 0; i < m_HandCard[m_bOutCardChairID].FixedCardsLen; i++)
			{
				if (m_HandCard[m_bOutCardChairID].FixedCards[i].state != TYPE_ANGANG)
				{
					gang_fan_num += 1;
				//	log_info("OutCardEstimate ChairID = %d  m_bOutCardChairID = %d = %d", ChairID, m_bOutCardChairID, gang_fan_num);

					break;
				}
			}
			//log_info("OutCardEstimate ChairID = %d  m_bOutCardChairID = %d = %d", ChairID, m_bOutCardChairID, hu_fan_num);
			//log_info("OutCardEstimate ChairID = %d  m_bOutCardChairID = %d = %d", ChairID, m_bOutCardChairID, gang_fan_num);
			if (hu_fan_num + gang_fan_num < 3)
			{
				iOpType &= ~(TYPE_HU);
			}
			guozhanghufan = hu_fan_num + gang_fan_num;
		}
		else
		{
			int hu_fan_num = Calc_Fan(ChairID, major.m_oFan);
			int gang_fan_num = 0;
			for (int c = NextChair(ChairID); c != ChairID; c = NextChair(c))
			{
				gang_fan_num = Calc_GangFan(c);
				for (int i = 0; i < m_HandCard[c].FixedCardsLen; i++)
				{//开口加一蕃
					if (m_HandCard[c].FixedCards[i].state != TYPE_ANGANG)
					{
						gang_fan_num += 1;
						break;
					}
				}
				if (c == m_bOutCardChairID)
				{
					gang_fan_num += 1;
				}
				if (hu_fan_num + gang_fan_num >= 3)
				{
					//log_info("OutCardEstimate c = %d = %d",c, hu_fan_num);
					//log_info("OutCardEstimate c = %d = %d",c, gang_fan_num);
					iOpType |= TYPE_HU;
					if (guozhanghufan <= hu_fan_num + gang_fan_num)
					{
						guozhanghufan = hu_fan_num + gang_fan_num;
					}
					//break;
				}
				else
				{
					iOpType &= ~(TYPE_HU);
				}
				if (guozhanghufan >= 3 )
				{
					//log_info("OutCardEstimate guozhanghufan = %d", guozhanghufan);

					iOpType |= TYPE_HU;
				}
		
			}
		}
		int bUserHasGuiPaiNum = 0;
		for (int i = 0; i < m_HandCard[ChairID].ChangeableCardsLen; i++)
		{
			if (m_HandCard[ChairID].ChangeableCards[i] == m_bXianNingGuiPai)
			{
				bUserHasGuiPaiNum++;
			}
		}
		if (major.m_oFan.Find(MJ_FAN_TYPE_PINGHU) && bUserHasGuiPaiNum> 1)
		{
			iOpType &= ~(TYPE_HU);
		}
		
		int userscoreTotal = 0;
		int userscore[GAME_PLAYER] = { 0 };
		Calc_BaseScore_GuoZhangHu(ChairID, userscore, major.m_oFan);
		for (_uint8 c = NextChair(ChairID); c != ChairID; c = NextChair(c))
		{
			userscoreTotal -= userscore[c];
		}
		log_info("%s tid:%d uid:%d chairid:%d iOpType:0x%x m_bUserOperateGuoZhangHuTotalFen[ChairID]:%d userscoreTotal:%d\n", __FUNCTION__, tid, GetPlayerUid(ChairID), ChairID, iOpType, m_bUserOperateGuoZhangHuTotalFen[ChairID], userscoreTotal);

		bool HuFlag = false;
		if (userscoreTotal > m_bUserOperateGuoZhangHuTotalFen[ChairID])
		{
			HuFlag = true;
			m_bUserOperateGuoZhangHuTotalFen[ChairID] = userscoreTotal;
		}

		log_info("%s tid:%d uid:%d chairid:%d iOpType:0x%x m_bUserOperateGuoZhangHuTotalFen[ChairID]:%d userscoreTotal:%d\n", __FUNCTION__, tid, GetPlayerUid(ChairID), ChairID, iOpType, m_bUserOperateGuoZhangHuTotalFen[ChairID], userscoreTotal);

		if (m_bUserOperateGuoZhangHuFlag[ChairID] == true)
		{
			if (HuFlag == false)
			{
				iOpType &= ~TYPE_HU;
			}	
		}
	}

	if (m_bCurChairID != PreChair(ChairID))
	{
		iOpType &= ~TYPE_CHI;
	}

	if (iOpType &TYPE_LEFT_CHI)
	{
		if ((bCard+1)== m_bXianNingGuiPai|| (bCard + 2)== m_bXianNingGuiPai)
		{
			iOpType &= ~TYPE_LEFT_CHI;
		}
	}
	if (iOpType &TYPE_CENTER_CHI)
	{
		if ((bCard + 1) == m_bXianNingGuiPai || (bCard - 1) == m_bXianNingGuiPai)
		{
			iOpType &= ~TYPE_CENTER_CHI;
		}
	} 
	
	if (iOpType &TYPE_RIGHT_CHI)
	{
		if ((bCard - 2) == m_bXianNingGuiPai || (bCard - 1) == m_bXianNingGuiPai)
		{
			iOpType &= ~TYPE_RIGHT_CHI;
		}
	}

	return iOpType;
}

int Table_xianningmajiang::OutCardEstimate_zhuanzhuan(_uint8 ChairID, _uint8 bCard, MJ_major& major, int& fan_num)
{
	//assert(major.m_oFan.Size() == 0);
	int iOpType = TYPE_NULL;
	HandCards* pHc = &m_HandCard[ChairID];
	iOpType |= GetChi(ChairID, bCard, major.m_Chi);
	iOpType |= GetPengGang(ChairID, bCard, major.m_GetPengGang);
	//log_debug("iOpType = %#x", iOpType);
//	assert(pHc->ChangeableCardsLen < HAND_CARD_SIZE_MAX);
	if (!m_conf.m_bZimoHu)
	{
		m_GameLogic.AddCard(pHc, bCard);
		iOpType |= Check_Hu(ChairID, *pHc, major.m_oFan);
		if (m_bUserOperateGuoZhangHuFlag[ChairID] == true)
		{//过张胡
			iOpType &= ~TYPE_HU;
		}

		if (CardPoolIsEmpty())
		{
			//UserLastOutCard(ChairID, iOpType, major.m_oFan);
		}
		if (m_bGetCardCount[m_bBanker] == 1)
		{//庄家打出第一张牌
			for (_uint8 c = NextChair(m_bBanker); c != m_bBanker; c = NextChair(c))
			{
				if (m_bGetCardCount[c] != 0)
					goto BANKER_FIRST_OUTCARD;
			}
			//	UserFirstOutCard(ChairID, iOpType, major.m_oFan);
		}
	BANKER_FIRST_OUTCARD:
		if (iOpType & TYPE_HU)
		{
			if (m_conf.m_bGangShangPao)
			{
				if (!m_bRobWanGang && m_bAddFanFlag[m_bCurChairID]) //杠上炮
				{
					//	UserGangHouPao(ChairID, bCard, major.m_oFan);
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

	//保存别人出牌之后的操作提示录像
	proto::game::AckOpNotify stVideo;
	stVideo.set_chairid(ChairID);
	stVideo.set_optype(iOpType);
	std::string str = stVideo.SerializeAsString();
	save_video_data(SERVER_OPERATE_NOTIFY, str);

	return iOpType;
}

int Table_xianningmajiang::GetPengGang(_uint8 ChairID, _uint8 cbCard, MJ_opinfo& GangCardInfo)
{
	if (m_conf_xianningmj.mj_conf_xianningmajiang==true)
	{
		return GetPengGang_0(ChairID, cbCard, GangCardInfo);
	}
	else
	{
		return GetPengGang_zhuanzhuan(ChairID, cbCard, GangCardInfo);
	}
}
int Table_xianningmajiang::GetPengGang_zhuanzhuan(_uint8 ChairID, _uint8 cbCard, MJ_opinfo& GangCardInfo)
{
	if (cbCard == 0x41)
	{
		return TYPE_NULL;
	}
	int iOpType = TYPE_NULL;
	iOpType = Table::GetPengGang(ChairID, cbCard, GangCardInfo);
	return iOpType;
}
int Table_xianningmajiang::GetPengGang_0(_uint8 ChairID, _uint8 cbCard, MJ_opinfo& GangCardInfo)
{
	if (cbCard == 0x41)
	{
		return TYPE_NULL;
	}
	int iOpType = TYPE_NULL;
	iOpType = Table::GetPengGang(ChairID,  cbCard, GangCardInfo);
	if (m_bUserOperatePengCardInfo[ChairID].Find(OpState(cbCard, TYPE_PENG)))
	{
		m_bUserOperatePengCardInfo[ChairID].Clear();
		GangCardInfo.Remove(OpState(cbCard, TYPE_PENG));
		iOpType &= ~(TYPE_PENG);

	}
	else if (iOpType & TYPE_PENG)
	{
		m_bUserOperatePengCardInfo[ChairID].Add(OpState(cbCard, TYPE_PENG));
	}
	if (m_CardPool.Count() <= 15)
	{
		iOpType &= ~TYPE_ZHIGANG;
	}

	return iOpType;
}
void Table_xianningmajiang::OnUserOutCard(_uint8 ChairID, _uint8 bCard)
{
	if (m_conf_xianningmj.mj_conf_xianningmajiang == true)
	{
		if (m_CardPool.Count() <= 10)
		{
			GameEnd();
			return;
		}

	}
	memset(&M_bUserDoMultiOperate, 0, sizeof(M_bUserDoMultiOperate));//
	if (m_conf_xianningmj.mj_conf_xianningmajiang_hongzhonglaizi_gang ==true)
	{
		OnUserOutCard_1(ChairID, bCard);
	}
	else
	{
		OnUserOutCard_0(ChairID, bCard);
	}
}

void Table_xianningmajiang::OnUserOutCard_0(_uint8 ChairID, _uint8 bCard)
{
	m_bUserOperateLzHzGangFlag[ChairID] = false;
	m_bUserOperatePengCardInfo[ChairID].Clear();
	m_bUserOperateGuoZhangHuFlag[ChairID] = false;
	if (m_conf_xianningmj.mj_conf_xianningmajiang == true&&m_CardPool.Count() >= 15)
	{
		if (bCard == m_bXianNingGuiPai)
		{
			m_bUserOperateLzHzGangFlag[ChairID] = true;
			m_bUserOutGuiCardNum[ChairID]++;

		}
		else if (bCard == 0x41)
		{
			m_bUserOperateLzHzGangFlag[ChairID] = true;
			m_bUserOutHongZhongCardNum[ChairID]++;
		}
	}


	Table::OnUserOutCard(ChairID, bCard);
	if (m_conf_xianningmj.mj_conf_xianningmajiang == true)
	{
		if ((bCard == 0x41) || (bCard == m_bXianNingGuiPai))
		{//打出红中算杠，直接摸牌，之后打牌。
			if (m_CardPool.Count() >= 15)
			{
				OnUserGetCard(ChairID);
			}
			
		}
	}
	if (m_conf_xianningmj.mj_conf_xianningmajiang == true)
	{
		if (m_HandCard[ChairID].FixedCardsLen == 4)
		{
			for (int i = 0; i < m_HandCard[ChairID].FixedCardsLen; i++)
			{
				if (m_HandCard[ChairID].FixedCards[i].state == TYPE_ANGANG)
				{
					return;
				}
			}

			m_bUserQuanQiuRenAutoWarning[ChairID] = true;
			::proto::game::xianningmj::RspGamePlayerAlertingInfo WaringInfo;
			WaringInfo.set_seatid(ChairID);
			WaringInfo.set_flag(true);
			broadcast(SERVER_ROOM_MAJINAG_QINGYISE_WANING, WaringInfo);
			/*保存录像*/
			SaveVideoData(SERVER_ROOM_MAJINAG_QINGYISE_WANING, WaringInfo);
		}
	}
}

void Table_xianningmajiang::OnUserOutCard_1(_uint8 ChairID, _uint8 bCard)
{
	m_bUserOperateLzHzGangFlag[ChairID] = false;
	m_bUserOperatePengCardInfo[ChairID].Clear();
	m_bUserOperateGuoZhangHuFlag[ChairID] = false;
	if (m_conf_xianningmj.mj_conf_xianningmajiang == true)
	{

		if (bCard == m_bXianNingGuiPai)
		{
			m_bUserOutGuiCardNum[ChairID]++;
			if (m_bUserOutHongZhongLaiZiCardFlag[ChairID] == true)
			{
				m_bUserOperateLzHzGangFlag[ChairID] = true;

			}

		}
		else if (bCard == 0x41)
		{
			
			m_bUserOutHongZhongCardNum[ChairID]++;
			if (m_bUserOutHongZhongLaiZiCardFlag[ChairID] == true)
			{
				m_bUserOperateLzHzGangFlag[ChairID] = true;
			}
		}
	}


	// 删出牌时钟	
	ev_timer_stop(zjh.loop, &m_Outcard_Timer);
	log_info("%s tid:%d uid:%d chairid:%d outcard:0x%02x \n", __FUNCTION__, tid, GetPlayerUid(ChairID), ChairID, bCard);
	m_bCurChairID = ChairID;
	m_bOutCardChairID = ChairID;
	m_GameLogic.ChuPai(&m_HandCard[ChairID], bCard);
	m_bCurCard = bCard;
	m_bCurOutCard = bCard;
	m_bOverTimeState[ChairID] = 0;

	m_stUserOutCard[ChairID].Add(bCard);

	//广播玩家出牌	
	proto::game::AckUserOutCard stUseroutcard;

	Proto_Game_UserOutCard(ChairID, bCard, stUseroutcard);

	broadcast(SERVER_USER_OUTCARD, stUseroutcard);

	save_user_out_card(ChairID, bCard);

	if (m_conf_xianningmj.mj_conf_xianningmajiang == true)
	{
		if (m_bUserOutHongZhongLaiZiCardFlag[ChairID] == true)
		{
			if ((bCard == 0x41) || (bCard == m_bXianNingGuiPai))
			{//打出红中算杠，直接摸牌，之后打牌。
				OnUserGetCard(ChairID);
			}
		}
	}
	if (m_conf_xianningmj.mj_conf_xianningmajiang == true)
	{
		if (m_HandCard[ChairID].FixedCardsLen == 4)
		{
			for (int i = 0; i < m_HandCard[ChairID].FixedCardsLen; i++)
			{
				if (m_HandCard[ChairID].FixedCards[i].state == TYPE_ANGANG)
				{
					return;
				}
			}

			m_bUserQuanQiuRenAutoWarning[ChairID] = true;
			::proto::game::xianningmj::RspGamePlayerAlertingInfo WaringInfo;
			WaringInfo.set_seatid(ChairID);
			WaringInfo.set_flag(true);
			broadcast(SERVER_ROOM_MAJINAG_QINGYISE_WANING, WaringInfo);
			/*保存录像*/
			SaveVideoData(SERVER_ROOM_MAJINAG_QINGYISE_WANING, WaringInfo);
		}
	}
}

void Table_xianningmajiang::DoMultiOperate(_uint8 ChairID, int iOpType, _uint8 bCard)
{
	if (m_conf_xianningmj.mj_conf_xianningmajiang == true)
	{
		if (M_bUserDoMultiOperate[ChairID] == true)
		{
			return;
		}
		M_bUserDoMultiOperate[ChairID] = true;
	}

	if (m_conf_xianningmj.mj_conf_xianningmajiang == true)
	{
		if (m_bUserOperate[ChairID] & TYPE_ZHIGANG&&iOpType !=TYPE_ZHIGANG)
		{
			_uint8 HuChairID = 0;
			bool bXianNingZhiGangAndHuFlag = false;
			//log_info("m_bUserOperate[ChairID = %d] = %x", ChairID, m_bUserOperate[ChairID]);
			if (m_bUserOperate[ChairID] & TYPE_ZHIGANG)
			{
				for (int z = NextChair(ChairID); z != ChairID; z = NextChair(z))
				{
					//log_info("m_bUserOperate[z = %d] = %x", z, m_bUserOperate[z]);
					if (m_bUserOperate[z] & TYPE_HU)
					{
						bXianNingZhiGangAndHuFlag = true;
						HuChairID = z;
						//log_info("HuChairID= %d", HuChairID);			
					}
					if (m_bUserOperate[ChairID] & TYPE_ZHIGANG && bXianNingZhiGangAndHuFlag == true)
					{
						bXianNingZhiGangAndHuFlag = false;
					//	log_info("m_bXianNingZhiGangCard= %x", m_bXianNingZhiGangCard);
					//	log_info("HuChairID= %d", HuChairID);

						for (int j = 0; j < m_GAME_PLAYER; j++)
						{
							proto::game::AckOpNotify stNotify;
							Proto_Game_OpNotify(HuChairID, j, m_bXianNingZhiGangCard, stNotify);
							unicast(SERVER_OPERATE_NOTIFY, j, stNotify);
							std::string str = stNotify.SerializeAsString();
							save_video_data(SERVER_OPERATE_NOTIFY, str);
						}
					}
				}
			}
		}

		if (m_bUserOperate[ChairID] & TYPE_ZHIGANG&&iOpType == TYPE_ZHIGANG)
		{
			for (int c = NextChair(ChairID); c != ChairID; c = NextChair(c))
			{
				//log_info("m_bUserOperate[c = %d] = %x", c, m_bUserOperate[c]);
				if (m_bUserOperate[c] & TYPE_HU)
				{
					log_info("%s tid:%d uid:%d chaird:%d, curoptype:0x%x, optype:0x%x, \n", __FUNCTION__, tid,GetPlayerUid(c), c, m_bUserOperate[c], TYPE_PASS);
					DoMultiOperate(c, TYPE_PASS, bCard);
				}
			}
		}
	}
	m_bMultiDoneOpNum++;
	m_bMultiUserDoneOp[ChairID] = iOpType;
	m_bMultiUserDoneOpCard[ChairID] = bCard;
	log_info("%s tid:%d uid:%d chaird:%d, curoptype:0x%x, optype:0x%x, now done OpNum:%d, totalOpnum:%d \n", __FUNCTION__,tid,
		GetPlayerUid(ChairID), ChairID, m_bUserOperate[ChairID], iOpType, m_bMultiDoneOpNum, m_bMultiOpUserNum);
	if (iOpType == TYPE_HU)
	{
		m_bMulitFirstOperate = true;
		m_bMulitHuOpeCount++;

		/*只有选择胡的优先级最大，其它的操作自动略过，直接给客户端发送pass*/
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
				log_info("%s tid:%d  force user uid:%d pass seat:%d!\n", __FUNCTION__,tid, GetPlayerUid(c), c);
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
		if (m_conf_xianningmj.mj_conf_xianningmajiang == true)
		{
			if (m_bMultiOpHuNum > 1)
			{//如果优先级高的玩家选择胡牌，不必等待优先级低的玩家的操作。
				if (m_bMultiUserDoneOp[ChairID] == TYPE_HU && m_bUserOperate[ChairID] & TYPE_HU)
				{
					for (_uint8 c = NextChair(ChairID); c != m_bCurChairID; c = NextChair(c))
					{//把比自己优先级低的玩家直接pass
						if (m_bUserOperate[c]&TYPE_HU&&M_bUserDoMultiOperate[c] == false)
						{
							M_bUserDoMultiOperate[c] = true;
							m_bMultiDoneOpNum++;
							m_bMultiUserDoneOp[c] = TYPE_PASS;
							log_info("%s tid:%d uid:%d c:%d, curoptype:0x%x,now done OpNum:%d, totalOpnum:%d \n", __FUNCTION__, tid,
								GetPlayerUid(c), c, m_bUserOperate[c], m_bMultiDoneOpNum, m_bMultiOpUserNum);

						}
					}
				}
			}
		}

	}
	else
	{
		if (iOpType == TYPE_PASS)
		{
			if (m_bUserOperate[ChairID] & TYPE_HU)   //胡牌玩家选择了过牌
			{
				m_bMulitHuOpeCount++;
				log_info("%s tid:%d user uid:%d pass seat:%d, optype:0x%x!\n", __FUNCTION__,tid, GetPlayerUid(ChairID), ChairID, m_bUserOperate[ChairID]);
				m_PassHuPlayer.Add(ChairID);
			}
			UserOperatePass(ChairID, iOpType, bCard);
		}
		else
		{
			if (iOpType != TYPE_NULL)
			{
			//	assert(iOpType != TYPE_PASS);
			//	assert(m_bRecordMultiOp.iOpType != TYPE_PASS);
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
			//	assert(m_bRecordMultiOp.iOpType != TYPE_NULL);
			}
		}
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
		//	assert(m_bRecordMultiOp.iOpType != TYPE_PASS);
			if (m_bRecordMultiOp.iOpType != TYPE_NULL)
			{
				MultiOperateOver(m_bRecordMultiOp.ChairID, m_bRecordMultiOp.iOpType, m_bRecordMultiOp.bCard);
			}
			else
			{
				MultiOperateOver(ChairID, iOpType, bCard);

				OnUserGetCard(NextChair(m_bCurChairID));
			}
		}
		init_multi_data();
	}
	else//优先级操作了，优先级低的就忽略了
	{
		if (m_bMulitFirstOperate)
		{
			if (m_bMulitHuOpeCount != m_bMultiOpHuNum)
			{
				return;
			}
			/*未到达*/
			if (m_conf.m_bYiPaoDuoXiang)
			{//一炮多响,其他玩家自动胡

			}
			else
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
			init_multi_data();
			GameEnd();
		}
		else
		{
			bool bOver = true;
			for (_uint8 c = NextChair(m_bCurChairID); c != m_bCurChairID; c = NextChair(c))
			{
				if (m_bMultiUserDoneOp[c] != TYPE_NULL)
					continue;
				int tmpOpType = m_bUserOperate[c];
				if (tmpOpType == TYPE_NULL)
					continue;
			//	assert(tmpOpType != TYPE_PASS);
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
			}
		}
	}
}
void Table_xianningmajiang::OnUserGetCard(_uint8 ChairID, bool bGetLast)
{
	m_bUserOperateGuoZhangHuTotalFen[ChairID] = 0;

	if (m_conf_xianningmj.mj_conf_xianningmajiang == true)
	{
		OnUserGetCard_0(ChairID, bGetLast);
	}
	else
	{
		OnUserGetCard_zhuanzhuan(ChairID);
	}


}


void Table_xianningmajiang::OnUserGetCard_zhuanzhuan(_uint8 ChairID, bool bGetLast)
{
	m_bUserOperateGuoZhangHuFlag[ChairID] = false;
	if (m_bDeleteTable)
	{
		return;
	}

	if (m_CardPool.Count()==6)
	{
		GameEnd();
		return;
	}
	memset(&m_bUserOperate, 0, sizeof(m_bUserOperate));
	m_bCurChairID = ChairID;

	_uint8 bGetCard = bGetLast ? m_CardPool.GetLast() : m_CardPool.GetOneCard();

	m_bGetCardCount[ChairID]++;

	m_bCurCard = bGetCard;
	MJ_major major;
	//摸牌判断
	m_bUserOperate[ChairID] = GetCardEstimate(ChairID, bGetCard, major, m_HuFanNums[ChairID]);

	m_GameLogic.AddCard(&m_HandCard[ChairID], bGetCard);

	log_info("%s tid:%d uid:%d chairid:%d getcard:0x%02x optype:0x%x cardpoollen:%d \n", __FUNCTION__, tid, GetPlayerUid(ChairID), ChairID, bGetCard, m_bUserOperate[ChairID], m_CardPool.Count());
	save_user_get_card(ChairID, bGetCard, m_CardPool.Count());
	//广播玩家摸牌

	proto::game::AckUserGetCard stUsergetcard;
	for (int i = 0; i < m_GAME_PLAYER; i++)
	{
		if (m_SeatPlayer[i].player == NULL)
		{
			log_error("OnUserGetCard chairid:%d  error, one player is NULL\n", ChairID);
			continue;
		}

		Proto_Game_UserGetCard(i, ChairID, bGetCard, stUsergetcard);
		stUsergetcard.set_optype2(bGetLast ? 1 : 0);
		unicast(SERVER_USER_GETCARD, i, stUsergetcard);

		//保存获取牌之后的操作提示录像
		proto::game::AckOpNotify stVideo;
		stVideo.set_chairid(i);
		stVideo.set_optype(m_bUserOperate[i]);
		std::string str = stVideo.SerializeAsString();
		save_video_data(SERVER_OPERATE_NOTIFY, str);
	}

	if (m_bUserOperate[ChairID] == TYPE_NULL || m_bUserOperate[ChairID] == TYPE_LISTEN)
	{
		set_out_timer(ChairID);
	}
	else
	{
		set_operator_timer(ChairID);
	}
}
void Table_xianningmajiang::GetCardOperateCard_Timer_cb(struct ev_loop *loop, struct ev_timer *w, int revents)
{
	Table_xianningmajiang* table = (Table_xianningmajiang*)w->data;

	ev_timer_stop(zjh.loop, &table->m_GetCardOperateCard_Timer);

	table->OnUserGetLastFourCard(table->NextChair(table->m_bCurChairID));
}

void Table_xianningmajiang::OnUserGetCard_0(_uint8 ChairID, bool bGetLast)
{
	m_bUserGangOperatePengFlag[ChairID].Clear();
	m_bUserOperatePengCardInfo[ChairID].Clear();
	m_bUserOperateGuoZhangHuFlag[ChairID] = false;

	if (m_bDeleteTable)
	{
		return;
	}
	if (m_CardPool.Count() <= 10)
	{
		GameEnd();
		return;
	}

	if (m_CardPool.Count()==14)
	{	
		OnUserGetLastFourCard(ChairID);
	}

	if (m_CardPool.Count() <= 14)
	{
		return;
	}

	memset(&m_bUserOperate, 0, sizeof(m_bUserOperate));
	m_bCurChairID = ChairID;

	_uint8 bGetCard = bGetLast ? m_CardPool.GetLast() : m_CardPool.GetOneCard();

	m_bGetCardCount[ChairID]++;

	m_bCurCard = bGetCard;
	MJ_major major;
	if (bGetCard == m_bXianNingGuiPai)
	{
		m_bUserHasGuiPaiNum[ChairID]++;
	}
	//摸牌判断
	m_bUserOperate[ChairID] = GetCardEstimate(ChairID, bGetCard, major, m_HuFanNums[ChairID]);

	m_GameLogic.AddCard(&m_HandCard[ChairID], bGetCard);

	log_info("%s tid:%d uid:%d chairid:%d getcard:0x%02x optype:0x%x cardpoollen:%d \n", __FUNCTION__, tid, GetPlayerUid(ChairID), ChairID, bGetCard, m_bUserOperate[ChairID], m_CardPool.Count());
	save_user_get_card(ChairID, bGetCard, m_CardPool.Count());
	//广播玩家摸牌

	proto::game::AckUserGetCard stUsergetcard;
	for (int i = 0; i < m_GAME_PLAYER; i++)
	{
		if (m_SeatPlayer[i].player == NULL)
		{
			log_error("OnUserGetCard chairid:%d  error, one player is NULL\n", ChairID);
			continue;
		}

		Proto_Game_UserGetCard(i, ChairID, bGetCard, stUsergetcard);
		stUsergetcard.set_optype2(bGetLast ? 1 : 0);
		unicast(SERVER_USER_GETCARD, i, stUsergetcard);

		if (m_bUserOperate[i]>0)
		{
			//保存获取牌之后的操作提示录像
			proto::game::AckOpNotify stVideo;
			stVideo.set_chairid(i);
			stVideo.set_optype(m_bUserOperate[i]);
			std::string str = stVideo.SerializeAsString();
			save_video_data(SERVER_OPERATE_NOTIFY, str);
		}

	}

	if (m_bUserOperate[ChairID] == TYPE_NULL || m_bUserOperate[ChairID] == TYPE_LISTEN)
	{
		set_out_timer(ChairID);
	}
	else
	{
		set_operator_timer(ChairID);
	}
}


void Table_xianningmajiang::OnUserGetLastFourCard(_uint8 ChairID)
{
	memset(&m_bUserOperate, 0, sizeof(m_bUserOperate));
	if (m_bDeleteTable)
	{
		return;
	}
	if (m_CardPool.Count() <= 10)
	{
		GameEnd();
		return;
	}
	m_OnUserGetFenZhangCard[ChairID] = 0;
	memset(&m_bUserOperate, 0, sizeof(m_bUserOperate));
	m_bCurChairID = ChairID;

	_uint8 bGetCard = m_CardPool.GetOneCard();

	m_bGetCardCount[ChairID]++;

	m_bCurCard = bGetCard;
	MJ_major major;
	if (bGetCard == m_bXianNingGuiPai)
	{
		m_bUserHasGuiPaiNum[ChairID]++;
	}
	//摸牌判断
	m_bUserOperate[ChairID] = GetCardEstimate(ChairID, bGetCard, major, m_HuFanNums[ChairID]);

	m_GameLogic.AddCard(&m_HandCard[ChairID], bGetCard);
	m_OnUserGetFenZhangCard[ChairID] = bGetCard;
	log_info("%s tid:%d uid:%d chairid:%d getcard:0x%02x optype:0x%x cardpoollen:%d \n", __FUNCTION__, tid, GetPlayerUid(ChairID), ChairID, bGetCard, m_bUserOperate[ChairID], m_CardPool.Count());
	save_user_get_card(ChairID, bGetCard, m_CardPool.Count());
	//广播玩家摸牌
	if ((m_bUserOperate[ChairID] & TYPE_HU))
	{
		m_bUserOperate[ChairID] = TYPE_HU;
	}
	else
	{
		m_bUserOperate[ChairID] = TYPE_NULL;
		const int tmp_Timer_stamp_0 = 2;
		TIMER_AGAIN(m_GetCardOperateCard_Timer, tmp_Timer_stamp_0);
	}
	proto::game::AckUserGetCard stUsergetcard;
	for (int i = 0; i < m_GAME_PLAYER; i++)
	{
		if (m_SeatPlayer[i].player == NULL)
		{
			log_error("OnUserGetCard chairid:%d  error, one player is NULL\n", ChairID);
			continue;
		}
		stUsergetcard.Clear();
		stUsergetcard.set_chairid(ChairID);
		stUsergetcard.set_leftcardnum(m_CardPool.Count());
		stUsergetcard.set_card(bGetCard);
		stUsergetcard.set_optype(m_bUserOperate[ChairID]);
		//Proto_Game_UserGetCard(i, ChairID, bGetCard, stUsergetcard);
		stUsergetcard.set_optype2(0);
		unicast(SERVER_USER_GETCARD, i, stUsergetcard);

		//保存获取牌之后的操作提示录像
		if (m_bUserOperate[i] > 0)
		{
			proto::game::AckOpNotify stVideo;
			stVideo.set_chairid(i);
			stVideo.set_optype(m_bUserOperate[i]);
			std::string str = stVideo.SerializeAsString();
			save_video_data(SERVER_OPERATE_NOTIFY, str);
		}

	}

	if (m_bUserOperate[ChairID] == TYPE_NULL || m_bUserOperate[ChairID] == TYPE_LISTEN)
	{
		set_out_timer(ChairID);
	}
	else
	{
		set_operator_timer(ChairID);
	}
}


void Table_xianningmajiang::UserOperateChi(_uint8 ChairID, int iOpType, _uint8 bCard)
{
	if (m_conf_xianningmj.mj_conf_xianningmajiang == true)
	{
		m_bUserOperatePengCardInfo[ChairID].Clear();
	}

	Table::UserOperateChi(ChairID, iOpType, bCard);
	if (m_conf_xianningmj.mj_conf_xianningmajiang==true)
	{
		if (m_HandCard[ChairID].FixedCardsLen >= 2)
		{
			bool QingYiSeFlag = false;
			int  bAnGangNum = 0;
			int bChairId = 0;
			for (int i = 0; i < m_HandCard[ChairID].FixedCardsLen; i++)
			{
				if (m_HandCard[ChairID].FixedCards[i].state != TYPE_ANGANG)
				{
					bChairId = i;
				}
			}
			for (int i = 0; i < m_HandCard[ChairID].FixedCardsLen; i++)
			{
				if (m_HandCard[ChairID].FixedCards[i].state==TYPE_ANGANG)
				{
					bAnGangNum++;
					continue;
				}
				if (CardType(m_HandCard[ChairID].FixedCards[bChairId].CardData) != CardType(m_HandCard[ChairID].FixedCards[i].CardData))
				{
					QingYiSeFlag = true;
					break;
				}
			}
			if (QingYiSeFlag == false &&(m_HandCard[ChairID].FixedCardsLen - bAnGangNum)>=2&& m_bUserQingYiSeAutoWarning[ChairID] == false)
			{
				m_bUserQingYiSeAutoWarning[ChairID] = true;
				::proto::game::xianningmj::RspGamePlayerAlertingInfo WaringInfo;
				WaringInfo.set_seatid(ChairID);
				WaringInfo.set_flag(true);
				broadcast(SERVER_ROOM_MAJINAG_QINGYISE_WANING, WaringInfo);
				/*保存录像*/
				SaveVideoData(SERVER_ROOM_MAJINAG_QINGYISE_WANING, WaringInfo);

			}
			if (QingYiSeFlag==true && m_bUserQingYiSeAutoWarning[ChairID] == true)
			{
				m_bUserQingYiSeAutoWarning[ChairID] = false;
				::proto::game::xianningmj::RspGamePlayerAlertingInfo WaringInfo;
				WaringInfo.set_seatid(ChairID);
				WaringInfo.set_flag(false);
				broadcast(SERVER_ROOM_MAJINAG_QINGYISE_WANING, WaringInfo);
				/*保存录像*/
				SaveVideoData(SERVER_ROOM_MAJINAG_QINGYISE_WANING, WaringInfo);
			}
		}

		if (m_bUser258JiangYiSeAutoWarning[ChairID] == true)
		{
			m_bUser258JiangYiSeAutoWarning[ChairID] = false;
			::proto::game::xianningmj::RspGamePlayerAlertingInfo WaringInfo;
			WaringInfo.set_seatid(ChairID);
			WaringInfo.set_flag(false);
			broadcast(SERVER_ROOM_MAJINAG_QINGYISE_WANING, WaringInfo);
			/*保存录像*/
			SaveVideoData(SERVER_ROOM_MAJINAG_QINGYISE_WANING, WaringInfo);
		}
	}
}



void Table_xianningmajiang::UserOperatePeng(_uint8 ChairID, _uint8 bCard)
{

	if (m_bUserOperate[ChairID]&TYPE_ZHIGANG)
	{
		m_bUserGangOperatePengFlag[ChairID].Add(bCard);
	}
	
	m_bUserOperateLzHzGangFlag[ChairID] = false;
	m_bUserOperatePengCardInfo[ChairID].Clear();
	Table::UserOperatePeng(ChairID, bCard);
	if (m_conf_xianningmj.mj_conf_xianningmajiang == true)
	{
		if (m_HandCard[ChairID].FixedCardsLen >= 2)
		{
			bool QingYiSeFlag = false;
			int  bAnGangNum = 0;
			int  bChairId = 0;
			for (int i = 0; i < m_HandCard[ChairID].FixedCardsLen; i++)
			{
				if (m_HandCard[ChairID].FixedCards[i].state != TYPE_ANGANG)
				{
					bChairId = i;
				}
			}
			for(int i = 0; i < m_HandCard[ChairID].FixedCardsLen; i++)
			{
				if(m_HandCard[ChairID].FixedCards[i].state == TYPE_ANGANG)
				{
					bAnGangNum++;
					continue;
				}
				if (CardType(m_HandCard[ChairID].FixedCards[bChairId].CardData) != CardType(m_HandCard[ChairID].FixedCards[i].CardData))
				{
					QingYiSeFlag = true;
					break;
				}
			}
			if (QingYiSeFlag == false && (m_HandCard[ChairID].FixedCardsLen - bAnGangNum)>=2 && m_bUserQingYiSeAutoWarning[ChairID] == false)
			{
				m_bUserQingYiSeAutoWarning[ChairID] = true;
				::proto::game::xianningmj::RspGamePlayerAlertingInfo WaringInfo;
				WaringInfo.set_seatid(ChairID);
				WaringInfo.set_flag(true);
				broadcast(SERVER_ROOM_MAJINAG_QINGYISE_WANING, WaringInfo);
				/*保存录像*/
				SaveVideoData(SERVER_ROOM_MAJINAG_QINGYISE_WANING, WaringInfo);

			}
			if (QingYiSeFlag == true && m_bUserQingYiSeAutoWarning[ChairID] == true)
			{
				m_bUserQingYiSeAutoWarning[ChairID] = false;
				::proto::game::xianningmj::RspGamePlayerAlertingInfo WaringInfo;
				WaringInfo.set_seatid(ChairID);
				WaringInfo.set_flag(false);
				broadcast(SERVER_ROOM_MAJINAG_QINGYISE_WANING, WaringInfo);
				/*保存录像*/
				SaveVideoData(SERVER_ROOM_MAJINAG_QINGYISE_WANING, WaringInfo);
			}
		}
	}
	if (m_conf_xianningmj.mj_conf_xianningmajiang == true)
	{
		if (m_HandCard[ChairID].FixedCardsLen >= 2)
		{
			bool QingYiSeFlag = false;
			int  bAnGangNum = 0;
			for (int i = 0; i < m_HandCard[ChairID].FixedCardsLen; i++)
			{
				if (m_HandCard[ChairID].FixedCards[i].state == TYPE_ANGANG)
				{
					bAnGangNum++;
					continue;
				}
				if (m_HandCard[ChairID].FixedCards[i].state &TYPE_CHI)
				{
					QingYiSeFlag = true;
				}
				if (CardNum(m_HandCard[ChairID].FixedCards[i].CardData)!=2 && CardNum(m_HandCard[ChairID].FixedCards[i].CardData)!=5&& CardNum(m_HandCard[ChairID].FixedCards[i].CardData)!=8)
				{
					QingYiSeFlag = true;
				}
			}

			if (QingYiSeFlag == false && (m_HandCard[ChairID].FixedCardsLen - bAnGangNum) >= 2 && m_bUser258JiangYiSeAutoWarning[ChairID] == false)
			{
				m_bUser258JiangYiSeAutoWarning[ChairID] = true;
				::proto::game::xianningmj::RspGamePlayerAlertingInfo WaringInfo;
				WaringInfo.set_seatid(ChairID);
				WaringInfo.set_flag(true);
				broadcast(SERVER_ROOM_MAJINAG_QINGYISE_WANING, WaringInfo);
				/*保存录像*/
				SaveVideoData(SERVER_ROOM_MAJINAG_QINGYISE_WANING, WaringInfo);

			}
			if (QingYiSeFlag == true && m_bUser258JiangYiSeAutoWarning[ChairID] == true)
			{
				m_bUser258JiangYiSeAutoWarning[ChairID] = false;
				::proto::game::xianningmj::RspGamePlayerAlertingInfo WaringInfo;
				WaringInfo.set_seatid(ChairID);
				WaringInfo.set_flag(false);
				broadcast(SERVER_ROOM_MAJINAG_QINGYISE_WANING, WaringInfo);
				/*保存录像*/
				SaveVideoData(SERVER_ROOM_MAJINAG_QINGYISE_WANING, WaringInfo);
			}
		}
	}
}

 
void Table_xianningmajiang::UserOperateGang(_uint8 ChairID, int iOpType, _uint8 bCard)
{
	m_bUserOperatePengCardInfo[ChairID].Clear();

	memset(m_bAddFanFlag, 0, sizeof(m_bAddFanFlag));
	memset(m_bUserOperate, 0, sizeof(m_bUserOperate));

	log_info("%s tid:%d uid:%d ChairID:%d, iOpType:0x%x, bCard:0x%02x \n", __FUNCTION__,tid, GetPlayerUid(ChairID), ChairID, iOpType, bCard);
	int iGangScore[GAME_PLAYER] = { 0 };
	_uint8 bTempChairID = INVALID_CHAIRID;
	m_bAddFanFlag[ChairID] = true;
	switch (iOpType)
	{
	case TYPE_ANGANG:
	{
		for (_uint8 c = NextChair(ChairID); c != ChairID; c = NextChair(c))
		{
			iGangScore[ChairID] += m_BaseScore * m_conf.m_AnGangPoint;
			iGangScore[c] -= m_BaseScore * m_conf.m_AnGangPoint;
		}
		m_stGang[ChairID].Gang.Add(tagGangScore(bCard, ChairID, TYPE_ANGANG));
		bTempChairID = ChairID;
	}
	break;
	case TYPE_WANGANG:
	{
		for (int i = 0; i < HAND_CARD_KAN_MAX; i++)
		{
			if (bCard == m_HandCard[ChairID].FixedCards[i].CardData)
			{
				//_uint8 tmpChairID = m_HandCard[ChairID].FixedCards[i].chairID;
				break;
			}
		}
		for (_uint8 c = NextChair(ChairID); c != ChairID; c = NextChair(c))
		{
			iGangScore[ChairID] += m_BaseScore * m_conf.m_WanGangPoint;
			iGangScore[c] -= m_BaseScore * m_conf.m_WanGangPoint;
		}
		m_stGang[ChairID].Gang.Add(tagGangScore(bCard, ChairID, TYPE_WANGANG));
		bTempChairID = ChairID;
	}
	break;
	case TYPE_ZHIGANG:
	{
		iGangScore[ChairID] += m_BaseScore * m_conf.m_ZhiGangPoint;
		iGangScore[m_bOutCardChairID] -= m_BaseScore *  m_conf.m_ZhiGangPoint;

		m_stGang[ChairID].Gang.Add(tagGangScore(bCard, m_bOutCardChairID, TYPE_ZHIGANG));

		bTempChairID = m_bOutCardChairID;

		m_bGetCardCount[ChairID]++;
		//从打出牌删掉这张牌
		m_stUserOutCard[m_bOutCardChairID].RemoveLast(bCard);
	}
	break;
	default:
		log_error("UserOperateGang, wrong optype:%d \n", iOpType);
		break;
	}


	m_GameLogic.ExecuteGang(&m_HandCard[ChairID], bTempChairID, bCard, iOpType);

	Calc_Score(m_stGang, iGangScore);

	proto::game::AckBcOpResult stOpresult;
	if (m_conf_xianningmj.mj_conf_xianningmajiang == true)
	{
		if(iOpType == TYPE_ANGANG)
		{//咸宁麻将暗杠别人不能看到杠的牌
			int SaveCard = bCard;
			for (int i = 0; i < m_GAME_PLAYER; i++)
			{
				if (i != ChairID)
				{
					SaveCard = 0x00;
				}
				else
				{
					SaveCard = bCard;
				}
				Proto_Game_OperateGang(ChairID, SaveCard, iOpType, iGangScore, stOpresult);
				unicast(SERVER_OPERATE_RESULT, i, stOpresult);
			}
		}
		else
		{
			Proto_Game_OperateGang(ChairID, bCard, iOpType, iGangScore, stOpresult);
			broadcast(SERVER_OPERATE_RESULT, stOpresult);
		}
	}
	else
	{//转转麻将，暗杠可以看到别人的牌
		Proto_Game_OperateGang(ChairID, bCard, iOpType, iGangScore, stOpresult);
		broadcast(SERVER_OPERATE_RESULT, stOpresult);
	}

	m_bUserOperate[ChairID] = TYPE_NULL;
	if (m_conf_xianningmj.mj_conf_xianningmajiang == true)
	{
		if (iOpType == TYPE_ANGANG)
		{
			stOpresult.set_card(bCard);
		}
	}	
	SaveVideoData(SERVER_OPERATE_RESULT, stOpresult);

	if (m_conf_xianningmj.mj_conf_xianningmajiang == true)
	{
		if (m_HandCard[ChairID].FixedCardsLen >= 2)
		{
			bool QingYiSeFlag = false;
			int  bAnGangNum = 0;
			int  bChairId = 0;
			for (int i = 0; i < m_HandCard[ChairID].FixedCardsLen; i++)
			{
				if(m_HandCard[ChairID].FixedCards[i].state!=TYPE_ANGANG)
				{
					bChairId = i;
				}
			}
			for (int i = 0; i < m_HandCard[ChairID].FixedCardsLen; i++)
			{
				if (m_HandCard[ChairID].FixedCards[i].state == TYPE_ANGANG)
				{
					bAnGangNum++;
					continue;
				}

				if (CardType(m_HandCard[ChairID].FixedCards[bChairId].CardData) != CardType(m_HandCard[ChairID].FixedCards[i].CardData))
				{
					QingYiSeFlag = true;
					break;
				}
			}

			if (QingYiSeFlag == false && (m_HandCard[ChairID].FixedCardsLen - bAnGangNum)>=2 && m_bUserQingYiSeAutoWarning[ChairID] == false)
			{
				m_bUserQingYiSeAutoWarning[ChairID] = true;
				::proto::game::xianningmj::RspGamePlayerAlertingInfo WaringInfo;
				WaringInfo.set_seatid(ChairID);
				WaringInfo.set_flag(true);
				broadcast(SERVER_ROOM_MAJINAG_QINGYISE_WANING, WaringInfo);
				/*保存录像*/
				SaveVideoData(SERVER_ROOM_MAJINAG_QINGYISE_WANING, WaringInfo);

			}
			if (QingYiSeFlag == true && m_bUserQingYiSeAutoWarning[ChairID] == true)
			{
				m_bUserQingYiSeAutoWarning[ChairID] = false;
				::proto::game::xianningmj::RspGamePlayerAlertingInfo WaringInfo;
				WaringInfo.set_seatid(ChairID);
				WaringInfo.set_flag(false);
				broadcast(SERVER_ROOM_MAJINAG_QINGYISE_WANING, WaringInfo);
				/*保存录像*/
				SaveVideoData(SERVER_ROOM_MAJINAG_QINGYISE_WANING, WaringInfo);
			}
		}
	}
	if (m_conf_xianningmj.mj_conf_xianningmajiang == true)
	{
		if (m_HandCard[ChairID].FixedCardsLen >= 2)
		{
			bool QingYiSeFlag = false;
			int  bAnGangNum = 0;
			for (int i = 0; i < m_HandCard[ChairID].FixedCardsLen; i++)
			{
				if (m_HandCard[ChairID].FixedCards[i].state == TYPE_ANGANG)
				{
					bAnGangNum++;
					continue;
				}
				if (m_HandCard[ChairID].FixedCards[i].state &TYPE_CHI)
				{
					QingYiSeFlag = true;
				}
				if (CardNum(m_HandCard[ChairID].FixedCards[i].CardData) != 2 && CardNum(m_HandCard[ChairID].FixedCards[i].CardData) != 5 && CardNum(m_HandCard[ChairID].FixedCards[i].CardData) != 8)
				{
					QingYiSeFlag = true;
				}
			}

			if (QingYiSeFlag == false && (m_HandCard[ChairID].FixedCardsLen - bAnGangNum) >= 2 && m_bUser258JiangYiSeAutoWarning[ChairID] == false)
			{
				m_bUser258JiangYiSeAutoWarning[ChairID] = true;
				::proto::game::xianningmj::RspGamePlayerAlertingInfo WaringInfo;
				WaringInfo.set_seatid(ChairID);
				WaringInfo.set_flag(true);
				broadcast(SERVER_ROOM_MAJINAG_QINGYISE_WANING, WaringInfo);
				/*保存录像*/
				SaveVideoData(SERVER_ROOM_MAJINAG_QINGYISE_WANING, WaringInfo);

			}
			if (QingYiSeFlag == true && m_bUser258JiangYiSeAutoWarning[ChairID] == true)
			{
				m_bUser258JiangYiSeAutoWarning[ChairID] = false;
				::proto::game::xianningmj::RspGamePlayerAlertingInfo WaringInfo;
				WaringInfo.set_seatid(ChairID);
				WaringInfo.set_flag(false);
				broadcast(SERVER_ROOM_MAJINAG_QINGYISE_WANING, WaringInfo);
				/*保存录像*/
				SaveVideoData(SERVER_ROOM_MAJINAG_QINGYISE_WANING, WaringInfo);
			}
		}
	}
}


void Table_xianningmajiang::TurnBanker()
{
	MJ_user_buffer UserID;
	int iCount = MultiHu(UserID);
	if (iCount == 0)
	{//流局连庄
		m_bBanker = m_bBanker;
	}
	else if (iCount == 1)
	{
		m_bBanker = UserID[0];
	}
	else if (iCount > 1 && m_conf_xianningmj.mj_conf_xianningmajiang == false)
	{
		m_bBanker = m_bOutCardChairID;
	}
}
int Table_xianningmajiang::Check_Hu(_uint8 ChairID, HandCards& pHc, MJ_fan_buffer& oFan)
{
	if (m_conf_xianningmj.mj_conf_xianningmajiang == true)
	{
		return Check_Hu_0(ChairID, pHc, oFan);

	}
	else
	{
		return Check_Hu_zhuanzhuan(ChairID, pHc, oFan);
	}
}

int Table_xianningmajiang::Check_Hu_zhuanzhuan(_uint8 ChairID, HandCards& pHc, MJ_fan_buffer& oFan)
{
	if (pHc.ChangeableCardsLen > 14)
	{
		return TYPE_NULL;
	}
	//取余3不等于2的手牌不做胡牌判断
	if (pHc.ChangeableCardsLen != 0 && pHc.ChangeableCardsLen % 3 != 2)
	{
		return TYPE_NULL;
	}

	if (Is7Dui(ChairID, pHc, oFan))
	{
		return TYPE_HU;
	}
	/*格式化牌*/
	HandCards tmp;
	//去除手牌中的金,之后进行判断
	int bUserHasGuiNum = 0; //听牌的时候，可以听鬼牌。则需要重新计算拥有的鬼牌的数量
	for (int i = 0; i < pHc.ChangeableCardsLen; i++)
	{
		if (pHc.ChangeableCards[i] == m_bXianNingGuiPai)
		{
			bUserHasGuiNum++;
			continue;
		}
		m_GameLogic.AddCard(&tmp, pHc.ChangeableCards[i]);
	}
	if (m_GameLogic.FormatCards_Laizi(tmp.ChangeableCards, tmp.ChangeableCardsLen, bUserHasGuiNum))
	{
		/*格式化牌*/
		oFan.Add(MJ_FAN_TYPE_PINGHU);
		return TYPE_HU;
	}
	if (bUserHasGuiNum == 4)
	{
		oFan.Add(MJ_FAN_XIANNING_SIHONGZHONG);
		return TYPE_HU;
	}
	return TYPE_NULL;
}
int Table_xianningmajiang::Check_Hu_0(_uint8 ChairID, HandCards& pHc, MJ_fan_buffer& oFan)
{
	for (int i = 0; i < pHc.ChangeableCardsLen;i++)
	{
		if (pHc.ChangeableCards[i] == 0x41)
		{
			return TYPE_NULL;
		}
	}
	if (pHc.FixedCardsLen == 0)
	{//开口才能胡牌
		return TYPE_NULL;
	}
	else
	{
		int angangnum = 0;
		for (int i = 0; i < pHc.FixedCardsLen; i++)
		{
			if (pHc.FixedCards[i].state == TYPE_ANGANG)
			{
				angangnum++;
			}
		}
		if (angangnum == pHc.FixedCardsLen)
		{
			return TYPE_NULL;
		}
	}

	if (pHc.ChangeableCardsLen > 14)
	{
		return TYPE_NULL;
	}
	//取余3不等于2的手牌不做胡牌判断
	if (pHc.ChangeableCardsLen != 0 && pHc.ChangeableCardsLen % 3 != 2)
	{
		return TYPE_NULL;
	}

	HandCards tmp;
	//去除手牌中的金,之后进行判断
	int bUserHasGuiNum = 0; //听牌的时候，可以听鬼牌。则需要重新计算拥有的鬼牌的数量
	for (int i = 0; i < pHc.ChangeableCardsLen; i++)
	{
		if (pHc.ChangeableCards[i] == m_bXianNingGuiPai)
		{
			bUserHasGuiNum++;
			continue;
		}
		m_GameLogic.AddCard(&tmp, pHc.ChangeableCards[i]);
	}
	bool Is258Jiang = false;
	if (FormatCards_Laizi(tmp.ChangeableCards, tmp.ChangeableCardsLen, bUserHasGuiNum, Is258Jiang))
	{
		/*格式化牌*/
		return Check_Hu_Pattern(ChairID, pHc, oFan);/*计算加番*/
	}
	else
	{ //将一色无需成牌型
		if (Jiang258YiSe(&pHc))
		{
			int bXianNingGuiPaiNum = 0;
			for (int i = 0; i < pHc.ChangeableCardsLen;i++)
			{
				if (pHc.ChangeableCards[i]== m_bXianNingGuiPai)
				{
					bXianNingGuiPaiNum++;;
				}
			}
			if (bXianNingGuiPaiNum==0)
			{
				oFan.Add(MJ_FAN_XIANNING_HEIHU);

			}else if (CardNum(m_bXianNingGuiPai)==2|| CardNum(m_bXianNingGuiPai) == 5|| CardNum(m_bXianNingGuiPai) == 8)
			{
				oFan.Add(MJ_FAN_XIANNING_HEIHU);
			}
			oFan.Add(MJ_FAN_XIANNING_258JIANGYISE);
			return TYPE_HU;
		}
		
	}
	return TYPE_NULL;
}

int Table_xianningmajiang::Check_Hu_Pattern(_uint8 ChairID, const HandCards& pHc, MJ_fan_buffer& oFan)
{
	//log_info("FormatCards_Laizi Check_Hu_Pattern is ok");

	MJ_win_pattern oPattern;
	HandCards tmp;
	int bUserHasGuiNum = 0; 
	for (int i = 0; i < pHc.ChangeableCardsLen; i++)
	{
		if (pHc.ChangeableCards[i] == m_bXianNingGuiPai)
		{
			bUserHasGuiNum++;
			continue;
		}
		m_GameLogic.AddCard(&tmp, pHc.ChangeableCards[i]);
	}
	bool Is258Jiang = true;
	if (FormatCards_Laizi(tmp.ChangeableCards, tmp.ChangeableCardsLen, bUserHasGuiNum, Is258Jiang))
	{//平胡必须258做将
		//log_info("FormatCards_Laizi MJ_FAN_TYPE_PINGHU is ok");
		oFan.Add(MJ_FAN_TYPE_PINGHU);
		/*格式化牌*/
	}
	if(IsPengPengHu(pHc, oPattern.kan, oPattern.jiang))
	{
		//log_info("Check_Hu_Pattern IsPengPengHu is ok");

		oFan.Remove(MJ_FAN_TYPE_PINGHU);
		oFan.Add(MJ_FAN_TYPE_PENGPENGHU);
	}

	if (IsQingYiSe(&pHc))
	{
		//log_info("Check_Hu_Pattern IsQingYiSe is ok");
		oFan.Remove(MJ_FAN_TYPE_PINGHU);
		oFan.Add(MJ_FAN_TYPE_QINGYISE);
	}
	if (Jiang258YiSe(&pHc))
	{
		//log_info("Check_Hu_Pattern Jiang258YiSe is ok");

		oFan.Remove(MJ_FAN_TYPE_PINGHU);
		oFan.Add(MJ_FAN_XIANNING_258JIANGYISE);
	}
	if (IsHeiHu(pHc, oFan))
	{
		if (oFan.Size() != 0)
		{
		//	log_info("Check_Hu_Pattern IsHeiHu is ok");
			oFan.Add(MJ_FAN_XIANNING_HEIHU);
		}
		if (oFan.Find(MJ_FAN_TYPE_PENGPENGHU))
		{
			_uint8 CardDataNum[MJ_TYPE_COMMON][10] = { { 0 } };
			int bUserHasGuiNum = 0;
			HandCards tmp = pHc;
			m_GameLogic.GetCardCount(&tmp, CardDataNum);
			if (!m_GameLogic.L_IsPengPengHu(&tmp, INVALID_MJ_CARD, CardDataNum, bUserHasGuiNum))
			{
				oFan.Remove(MJ_FAN_XIANNING_HEIHU);
			}
		}
	}
	if (oFan.Size()>0)
	{
		return TYPE_HU;
	}
	return TYPE_NULL;
}


bool Table_xianningmajiang::FormatCards_Laizi(_uint8 card[14], int nLen, int nLaiZiNum, bool Is258Jiang)
{
	//MjCardData PaiCard[4];
	MJ_hand_buffer PaiCard[MJ_TYPE_JIAN];        //各种花式的牌数据
	memset(PaiCard, 0, sizeof(PaiCard));
	m_GameLogic.PyCard(card, nLen, PaiCard);

	int needHunNum = 0;
	int needMinHunNum = 8;
	m_GameLogic.get_need_hun_num(PaiCard[0], 0, needMinHunNum);
	int wan_need = needMinHunNum;
	needMinHunNum = 8;
	m_GameLogic.get_need_hun_num(PaiCard[1], 0, needMinHunNum);
	int tiao_need = needMinHunNum;
	needMinHunNum = 8;
	m_GameLogic.get_need_hun_num(PaiCard[2], 0, needMinHunNum);
	int tong_need = needMinHunNum;
	needMinHunNum = 8;
	m_GameLogic.get_need_hun_num(PaiCard[3], 0, needMinHunNum);
	int feng_need = needMinHunNum;

	needHunNum = tiao_need + tong_need + feng_need;
	if (needHunNum <= nLaiZiNum)
	{
		int hasNum = nLaiZiNum - needHunNum;
		bool ishu = LaiZi_CanHu(PaiCard[0], hasNum, Is258Jiang);
		if (ishu)
		{
			return true;
		}
	}
	needHunNum = wan_need + tong_need + feng_need;
	if (needHunNum <= nLaiZiNum)
	{
		int hasNum = nLaiZiNum - needHunNum;
		bool ishu = LaiZi_CanHu(PaiCard[1], hasNum, Is258Jiang);
		if (ishu)
		{
			return true;
		}
	}
	needHunNum = wan_need + tiao_need + feng_need;
	if (needHunNum <= nLaiZiNum)
	{
		int hasNum = nLaiZiNum - needHunNum;
		bool ishu = LaiZi_CanHu(PaiCard[2], hasNum,  Is258Jiang);
		if (ishu)
		{
			return true;
		}
	}
	needHunNum = wan_need + tiao_need + tong_need;
	if (needHunNum <= nLaiZiNum)
	{
		int hasNum = nLaiZiNum - needHunNum;
		bool ishu = LaiZi_CanHu(PaiCard[3], hasNum, Is258Jiang);
		if (ishu)
		{
			return true;
		}
	}
	return false;
}


bool Table_xianningmajiang::LaiZi_CanHu(MJ_hand_buffer stData, int LaiZiNum,  bool Is258Jiang, bool bAllShunZi)
{
	if (stData.Size() == 0)
	{
		if (LaiZiNum >= 2)
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	MJ_hand_buffer tempData;
	//memcpy(&tempData, &stData, sizeof(MjCardData));
	tempData = stData;
	for (int i = 0; i < tempData.Size(); i++)
	{
		if (i == tempData.Size() - 1)
		{
			if (LaiZiNum > 0)
			{
				LaiZiNum = LaiZiNum - 1;
				stData.Remove(tempData[i]);
				int nNeedMinLaiZiNum = 8;
				if (bAllShunZi)
				{
					m_GameLogic.get_need_hun_num_by_shun(stData, 0, nNeedMinLaiZiNum);
				}
				else
				{
					m_GameLogic.get_need_hun_num(stData, 0, nNeedMinLaiZiNum);
				}

				if (nNeedMinLaiZiNum <= LaiZiNum)
				{
					if (Is258Jiang== true)
					{
						if ((CardNum(tempData[i]) == 2)
							|| CardNum(tempData[i]) == 5 || CardNum(tempData[i]) == 8)
						{
							return true;
						}
					}
					else
					{
						return true;
					}		
				}
			}
		}
		else
		{
			if (((i + 2) == tempData.Size())
				|| tempData[i] != tempData[i + 2])
			{
				if (m_GameLogic.checke_combine_dui_zi(tempData[i], tempData[i + 1]))
				{
					stData.Remove(tempData[i]);
					stData.Remove(tempData[i + 1]);
					int nNeedMinLaiZiNum = 8;
					if (bAllShunZi)
					{
						m_GameLogic.get_need_hun_num_by_shun(stData, 0, nNeedMinLaiZiNum);
					}
					else
					{
						m_GameLogic.get_need_hun_num(stData, 0, nNeedMinLaiZiNum);
					}
					if (nNeedMinLaiZiNum <= LaiZiNum)
					{
						if (Is258Jiang == true)
						{
							if ((CardNum(tempData[i]) == 2)
								|| CardNum(tempData[i]) == 5 || CardNum(tempData[i]) == 8)
							{
								return true;
							}
						}
						else
						{
							return true;
						}
					}
					stData.Add(tempData[i]);
					stData.Add(tempData[i + 1]);
					stData.SortAscend();
				}
			}
			if (LaiZiNum > 0 && tempData[i] != tempData[i + 1])
			{
				LaiZiNum = LaiZiNum - 1;
				stData.Remove(tempData[i]);
				int nNeedMinLaiZiNum = 8;
				if (bAllShunZi)
				{
					m_GameLogic.get_need_hun_num_by_shun(stData, 0, nNeedMinLaiZiNum);
				}
				else
				{
					m_GameLogic.get_need_hun_num(stData, 0, nNeedMinLaiZiNum);
				}
				if (nNeedMinLaiZiNum <= LaiZiNum)
				{
					if (Is258Jiang == true)
					{
						if ((CardNum(tempData[i]) == 2)
							|| CardNum(tempData[i]) == 5 || CardNum(tempData[i]) == 8)
						{
							return true;
						}
					}
					else
					{
						return true;
					}
				}
				LaiZiNum = LaiZiNum + 1;
				stData.Add(tempData[i]);
				stData.SortAscend();
			}
		}
	}
	return false;
}

bool Table_xianningmajiang::IsPengPengHu(const HandCards& p, const _uint8 CardsFormatBuf[4][4], _uint8 Jiang)
{
	_uint8 CardDataNum[MJ_TYPE_COMMON][10] = { { 0 } };
	int bUserHasGuiNum = 0;
	HandCards tmp = p;
	for (int i = 0; i < p.ChangeableCardsLen; i++)
	{
		if (p.ChangeableCards[i] == m_bXianNingGuiPai)
		{
			m_GameLogic.RemoveCard(&tmp, m_bXianNingGuiPai, 1);
			bUserHasGuiNum++;
		}
	}
	m_GameLogic.GetCardCount(&tmp, CardDataNum);
	return  m_GameLogic.L_IsPengPengHu(&tmp, INVALID_MJ_CARD, CardDataNum, bUserHasGuiNum);
}

bool Table_xianningmajiang::IsQingYiSe(const HandCards* p)
{

	if (p == NULL) return false;

	HandCards tmp;

	for (int i = 0; i < p->ChangeableCardsLen; i++)
	{
		if (p->ChangeableCards[i] == m_bXianNingGuiPai)
			continue;
		m_GameLogic.AddCard(&tmp, p->ChangeableCards[i]);
	}

	_uint8 ct;
	if (tmp.ChangeableCardsLen == 0)
	{
		ct = CardType(p->FixedCards[0].CardData);
	}
	else
	{
		ct = CardType(tmp.ChangeableCards[0]);
	}
	if (ct == MJ_TYPE_FENG || ct == MJ_TYPE_JIAN)
		return false;

	for (int i = 0; i < tmp.ChangeableCardsLen; i++)
		if (ct != CardType(tmp.ChangeableCards[i]))
			return false;

	if (p->FixedCards->state != TYPE_NULL)
	{
		for (int i = 0; i < p->FixedCardsLen; i++)
		{
			if (ct != CardType(p->FixedCards[i].CardData))
				return false;
		}
	}

	return true;
}


bool Table_xianningmajiang::Jiang258YiSe(const HandCards* pHc)
{
	ShowCards Is258Jiang;
	_uint8 Is2Card = 0x02;
	_uint8 Is5Card = 0x05;
	_uint8 Is8Card = 0x08;
	for (int i = 0; i < 3; i++)
	{

		Is258Jiang.Add(Is2Card);
		Is258Jiang.Add(Is5Card);
		Is258Jiang.Add(Is8Card);
		Is2Card += 0x10;
		Is5Card += 0x10;
		Is8Card += 0x10;
	}

	for (int i = 0; i < pHc->ChangeableCardsLen;i++)
	{
		if (pHc->ChangeableCards[i] == m_bXianNingGuiPai)
		{
			continue;

		}else if (!Is258Jiang.Find(pHc->ChangeableCards[i]))
		{
			return false;
		}
	}
	for (int i = 0; i < pHc->FixedCardsLen;i++)
	{
		if (pHc->FixedCards[i].state & TYPE_CHI)
		{
			return false;
		}
		if (!Is258Jiang.Find(pHc->FixedCards[i].CardData))
		{
			return false;
		}
	}
	return true;
}

bool Table_xianningmajiang::Jiang258YiSe_HeiHu(const HandCards* pHc)
{
	ShowCards Is258Jiang;
	_uint8 Is2Card = 0x02;
	_uint8 Is5Card = 0x05;
	_uint8 Is8Card = 0x08;
	for (int i = 0; i < 3; i++)
	{

		Is258Jiang.Add(Is2Card);
		Is258Jiang.Add(Is5Card);
		Is258Jiang.Add(Is8Card);
		Is2Card += 0x10;
		Is5Card += 0x10;
		Is8Card += 0x10;
	}

	for (int i = 0; i < pHc->ChangeableCardsLen; i++)
	{
		if (!Is258Jiang.Find(pHc->ChangeableCards[i]))
		{
			return false;
		}
	}
	for (int i = 0; i < pHc->FixedCardsLen; i++)
	{
		if (pHc->FixedCards[i].state == TYPE_CHI)
		{
			return false;
		}
		if (!Is258Jiang.Find(pHc->FixedCards[i].CardData))
		{
			return false;
		}
	}
	return true;
}
bool Table_xianningmajiang::IsQuanqiuren(const HandCards& pHc)
{
	if (pHc.ChangeableCardsLen - 1 == 0)
	{

		for (int i = 0; i < pHc.FixedCardsLen; i++)
		{
			if (pHc.FixedCards[i].state == TYPE_ANGANG)
			{
				//log_info("IsQuanqiuren 02");
				return false;
			}
		}
		//log_info("IsQuanqiuren OK");
		return true;
	}

	return false;
}

bool Table_xianningmajiang::IsHeiHu(const HandCards& pHc, MJ_fan_buffer& oFan)
{
	HandCards tmp = pHc;
	//bool Is258Jiang = false;

	if (oFan.Find(MJ_FAN_TYPE_PINGHU))
	{
		if (FormatCards_Laizi(tmp.ChangeableCards, tmp.ChangeableCardsLen, 0, true))
		{
			/*格式化牌*/
			return true;
		}
	}
	else
	{
		if (FormatCards_Laizi(tmp.ChangeableCards, tmp.ChangeableCardsLen, 0, false))
		{
			if (oFan.Find(MJ_FAN_TYPE_QINGYISE))
			{
				if (m_GameLogic.IsQingYiSe(&tmp))
				{
					return true;
				}
			}else if (oFan.Find(MJ_FAN_XIANNING_258JIANGYISE))
			{
				if (Jiang258YiSe_HeiHu(&tmp))
				{
					return true;
				}
			}
			else
			{			
				return true;
			}
		}
	}


	return false;
}
void Table_xianningmajiang::Calc_BaseScore(_uint8 ChairID, int iBaseScore[GAME_PLAYER])
{
	if (m_conf_xianningmj.mj_conf_xianningmajiang == true)
	{
		Calc_BaseScore_0(ChairID, iBaseScore);

	}
	else
	{
		Calc_BaseScore_zhuanzhuan(ChairID, iBaseScore);

	}
}

void Table_xianningmajiang::Calc_BaseScore_zhuanzhuan(_uint8 ChairID, int iBaseScore[GAME_PLAYER])
{
	/*没胡则直接退出*/
	if (!m_bIsHu[ChairID])
		return;
	//摸完最后一张牌才算是结束
	MJ_user_buffer UserID;
	_uint8 bDraw = MultiHu(UserID);
	_uint8 zhongmaChairid = 0;
	
	if (m_bRobHu)
	{
		if (bDraw == 1)
		{
			zhongmaChairid = ChairID;
		}
		else
		{
			zhongmaChairid = m_WangGangPlayer;
		}
		int BaseScore = 1;
		int zhongmanum = m_bZhongMaPai[zhongmaChairid].Size();
		//log_info("Calc_BaseScore_zhuanzhuan BaseScore = %d", BaseScore);
		//log_info("Calc_BaseScore_zhuanzhuan zhongmanum = %d", zhongmanum);
		//log_info("Calc_BaseScore_zhuanzhuan m_conf.m_bZhuangXian = %d", m_conf.m_bZhuangXian);

		iBaseScore[ChairID] += BaseScore + zhongmanum;
		iBaseScore[m_WangGangPlayer] -= BaseScore + zhongmanum;
		if (m_conf.m_bZhuangXian == true)
		{
			if (ChairID ==m_bBanker)
			{
				iBaseScore[ChairID] += 1;
				iBaseScore[m_WangGangPlayer] -= 1;

			}else if (m_WangGangPlayer == m_bBanker)
			{
				iBaseScore[ChairID] += 1;
				iBaseScore[m_WangGangPlayer] -= 1;
			}
		}

	}
	else if (ChairID == m_bCurChairID)
	{
		int BaseScore = 2;
		int ZhongMaNum = m_bZhongMaPai[ChairID].Size();
		//log_info("Calc_BaseScore_zhuanzhuan BaseScore = %d", BaseScore);
		//log_info("Calc_BaseScore_zhuanzhuan zhongmanum = %d", ZhongMaNum);
		//log_info("Calc_BaseScore_zhuanzhuan m_conf.m_bZhuangXian = %d", m_conf.m_bZhuangXian);

		if (m_conf.m_bZhuangXian == true)
		{
			if (ChairID == m_bBanker)
			{
				BaseScore+=1;
			}
		}
		for (_uint8 c = NextChair(ChairID); c != ChairID; c = NextChair(c))
		{
			iBaseScore[ChairID] += BaseScore + ZhongMaNum;
			iBaseScore[c] -= BaseScore + ZhongMaNum;
		}

		if (m_conf.m_bZhuangXian == true && ChairID != m_bBanker)
		{
			if (ChairID == m_bBanker)
			{
				iBaseScore[ChairID] += 1;
			}
			for (_uint8 c = NextChair(ChairID); c != ChairID; c = NextChair(c))
			{
				if (c == m_bBanker)
				{
					iBaseScore[c] -= 1;
				}
			}
		}
	}
	else
	{
		if (bDraw == 1)
		{
			zhongmaChairid = ChairID;
		}
		else
		{
			zhongmaChairid = m_bOutCardChairID;
		}

		int BaseScore = 1;
		int zhongmanum = m_bZhongMaPai[zhongmaChairid].Size();
	//	log_info("Calc_BaseScore_zhuanzhuan BaseScore = %d", BaseScore);
		//log_info("Calc_BaseScore_zhuanzhuan zhongmanum = %d", zhongmanum);
		iBaseScore[ChairID] += BaseScore + zhongmanum;
		iBaseScore[m_bOutCardChairID] -= BaseScore + zhongmanum;
		if (m_conf.m_bZhuangXian == true)
		{
			if (ChairID == m_bBanker)
			{
				iBaseScore[ChairID] += 1;
				iBaseScore[m_bOutCardChairID] -= 1;

			}
			else if (m_bOutCardChairID == m_bBanker)
			{
				iBaseScore[ChairID] += 1;
				iBaseScore[m_bOutCardChairID] -= 1;
			}
		}
	}
}

void Table_xianningmajiang::Calc_BaseScore_0(_uint8 ChairID, int iBaseScore[GAME_PLAYER])
{
	/*没胡则直接退出*/
	if (!m_bIsHu[ChairID])
		return;

	int GuiNum = 0;
	for (int i = 0; i < m_HandCard[ChairID].ChangeableCardsLen; i++)
	{
		if (m_HandCard[ChairID].ChangeableCards[i] == m_bXianNingGuiPai)
		{
			GuiNum++;
		}
	}
	for (int i = 0; i < m_HandCard[ChairID].FixedCardsLen; i++)
	{
		if (m_HandCard[ChairID].FixedCards[i].state == TYPE_CHI)
		{
			for (int c = 0; c < 3; c++)
			{
				if (m_HandCard[ChairID].FixedCards[i].c[c] == m_bXianNingGuiPai)
				{
					GuiNum++;
				}
			}

		}
		else if (m_HandCard[ChairID].FixedCards[i].CardData == m_bXianNingGuiPai)
		{
			GuiNum++;
		}
	}
	for (int i = 0; i < m_stUserOutCard[ChairID].Size(); i++)
	{
		if (m_bXianNingGuiPai == m_stUserOutCard[ChairID][i])
		{
			GuiNum++;
		}
	}

	if (GuiNum == 4)
	{
		//log_info("222222222 Calc_BaseScore BaseHuFanNums = %d", BaseHuFanNums);
		m_oFan[ChairID].Add(MJ_FAN_XIANNING_SIGUIJIAYIFAN);
	}
	int BaseHuFanNums = Calc_Fan(ChairID, m_oFan[ChairID]);
	//log_info("1111111111 Calc_BaseScore BaseHuFanNums = %d", BaseHuFanNums);
	int IsKaiKou[GAME_PLAYER] = { 0 };
	for (int i = 0; i < m_GAME_PLAYER; i++)
	{
		if (m_HandCard[i].FixedCardsLen > 0)
		{
			for (int j = 0; j < m_HandCard[i].FixedCardsLen; j++)
			{
				if (m_HandCard[i].FixedCards[j].state != TYPE_ANGANG)
				{
					IsKaiKou[i] = 1;
					//log_info("33333333333333 Calc_BaseScore IsKaiKou[i = %d] = %d",i, IsKaiKou[i]);
					break;
				}
			}
		}
	}

	if (m_bRobHu)
	{//抢杠胡
		if (m_oFan[ChairID].Find(MJ_FAN_XIANNING_HEIHU))
		{
			iBaseScore[m_WangGangPlayer] -= 50 * m_BaseScore;
			iBaseScore[ChairID] += -iBaseScore[m_WangGangPlayer];
			//log_info("4444444444444 Calc_BaseScore iBaseScore[m_WangGangPlayer = %d] = %d", m_WangGangPlayer, iBaseScore[m_WangGangPlayer]);
			//log_info("5555555555555 Calc_BaseScore iBaseScore[ChairID = %d] = %d", ChairID, iBaseScore[ChairID]);

		}
		else
		{
			iBaseScore[m_WangGangPlayer] -= 40 * m_BaseScore;
			iBaseScore[ChairID] += -iBaseScore[m_WangGangPlayer];
			//log_info("666666666666 Calc_BaseScore iBaseScore[m_WangGangPlayer = %d] = %d", m_WangGangPlayer, iBaseScore[m_WangGangPlayer]);
			//log_info("777777777777 Calc_BaseScore iBaseScore[ChairID = %d] = %d", ChairID, iBaseScore[ChairID]);

		}

	}
	else if (ChairID == m_bCurChairID)
	{//自摸
		int HuFanNums = BaseHuFanNums;
		if (m_oFan[ChairID].Find(MJ_FAN_TYPE_GANGSHANGHUA) && (!m_oFan[ChairID].Find(MJ_FAN_TYPE_PINGHU)))
		{
			HuFanNums -= 1;//平胡杠上花加一番
			//log_info("8888888888888888 Calc_BaseScore HuFanNums = %d", HuFanNums);
		}

		if (!m_oFan[ChairID].Find(MJ_FAN_TYPE_PINGHU))
		{
			HuFanNums += 1;//自摸胡牌加一蕃
		}

		for (_uint8 c = NextChair(ChairID); c != ChairID; c = NextChair(c))
		{
			int HuGangFanNums_C = Calc_GangFan(c);
			//log_info("999999999999999 Calc_BaseScore HuFanNums = %d", HuFanNums);
			//log_info("999999999999999 Calc_BaseScore HuGangFanNums_C = %d", HuGangFanNums_C);

			int TotalHuFanNums = 1 << (HuFanNums + HuGangFanNums_C + IsKaiKou[c]);
			//log_info("1010101010101010 Calc_BaseScore TotalHuFanNums = %d", TotalHuFanNums);
			iBaseScore[c] -= (TotalHuFanNums * m_BaseScore);
			iBaseScore[ChairID] += -iBaseScore[c];
			//log_info("12121212121212 Calc_BaseScore iBaseScore[ChairID = %d] = %d", ChairID, iBaseScore[ChairID]);
			//log_info("13131313131313 Calc_BaseScore iBaseScore[c = %d] = %d", c, iBaseScore[c]);

		}
	}
	else
	{//放炮
		int HuFanNums = BaseHuFanNums;
		if (m_oFan[ChairID].Find(MJ_FAN_TYPE_PINGHU))
		{
			int HuGangFanNums_C = Calc_GangFan(m_bOutCardChairID);
			//log_info("141414141414 Calc_BaseScore HuGangFanNums_C = %d", HuGangFanNums_C);
			//log_info("141414141414 Calc_BaseScore HuFanNums = %d", HuFanNums);

			//HuGangFanNums_C += 1; //点炮玩家1蕃
			//log_info("151515151515 Calc_BaseScore HuGangFanNums_C = %d", HuGangFanNums_C);

			int TotalHuFanNums = 1 << (HuFanNums + HuGangFanNums_C + IsKaiKou[m_bOutCardChairID]);
			//log_info("161616161616 Calc_BaseScore TotalHuFanNums = %d", TotalHuFanNums);

			iBaseScore[m_bOutCardChairID] -= (TotalHuFanNums * m_BaseScore);
			iBaseScore[ChairID] += -iBaseScore[m_bOutCardChairID];
			//log_info("171717171717 Calc_BaseScore iBaseScore[ChairID = %d] = %d", ChairID, iBaseScore[ChairID]);
			//log_info("171717171717 Calc_BaseScore iBaseScore[m_bOutCardChairID = %d] = %d", m_bOutCardChairID, iBaseScore[m_bOutCardChairID]);

		}
		else
		{
			int HuFanNums = BaseHuFanNums;
			for (_uint8 c = NextChair(ChairID); c != ChairID; c = NextChair(c))
			{
				int HuGangFanNums_C = Calc_GangFan(c);
				if (m_bOutCardChairID == c)
				{
					HuGangFanNums_C += 1;//点炮加一番
					//log_info("181818181818 Calc_BaseScore HuGangFanNums_C = %d", HuGangFanNums_C);

				}
				//log_info("181818181818 Calc_BaseScore HuFanNums = %d", HuFanNums);
				//log_info("1818181818 Calc_BaseScore (HuFanNums  + HuGangFanNums_C + IsKaiKou[c]) = %d", (HuFanNums + HuGangFanNums_C + IsKaiKou[c]));

				int TotalHuFanNums = 1 << (HuFanNums + HuGangFanNums_C + IsKaiKou[c]);
				//log_info("191919191919 Calc_BaseScore HuFanNums = %d", HuFanNums);

				iBaseScore[c] -= (TotalHuFanNums * m_BaseScore);
				iBaseScore[ChairID] += -iBaseScore[c];
				//log_info("2020202020202020 Calc_BaseScore iBaseScore[ChairID = %d] = %d", ChairID, iBaseScore[ChairID]);
				//log_info("2121212121212121 Calc_BaseScore iBaseScore[c = %d] = %d", c, iBaseScore[c]);

			}
		}
	}

	//封顶
	int fengdingnum = 0;
	int kaikounum = 0;
	for (_uint8 c = NextChair(ChairID); c != ChairID; c = NextChair(c))
	{
		if (-iBaseScore[c] >= m_BaseScore * 30)
		{
			fengdingnum++;
		}
	}
	for (_uint8 c = NextChair(ChairID); c != ChairID; c = NextChair(c))
	{
		if (m_HandCard[c].FixedCardsLen > 0)
		{
			for (int j = 0; j < m_HandCard[c].FixedCardsLen; j++)
			{
				if (m_HandCard[c].FixedCards[j].state != TYPE_ANGANG)
				{
					kaikounum++;
					break;
				}
			}
		}
	}
	//log_info("99999999999 Calc_BaseScore fengdingnum = %d", fengdingnum);
	//log_info("99999999999 Calc_BaseScore kaikounum = %d", kaikounum);

	if (fengdingnum == (m_GAME_PLAYER - 1) && m_bRobHu == false)
	{
		if (kaikounum == 0)
		{
			for (_uint8 c = NextChair(ChairID); c != ChairID; c = NextChair(c))
			{
				iBaseScore[c] = -m_BaseScore * 60;
				//log_info("99999999999 Calc_BaseScore iBaseScore[c = %d] = %d", c, iBaseScore[c]);

			}
		}
		else
		{
			for (_uint8 c = NextChair(ChairID); c != ChairID; c = NextChair(c))
			{
				if (IsKaiKou[c])
				{
					iBaseScore[c] = -m_BaseScore * 40;
					//log_info("99999999999 Calc_BaseScore iBaseScore[c = %d] = %d", c, iBaseScore[c]);
				}
				else
				{
					iBaseScore[c] = -m_BaseScore * 50;
					//log_info("99999999999 Calc_BaseScore iBaseScore[c = %d] = %d", c, iBaseScore[c]);
				}
			}
		}
	}
	else if (m_bRobHu == false)
	{
		for (_uint8 c = NextChair(ChairID); c != ChairID; c = NextChair(c))
		{
			if (-iBaseScore[c] >= m_BaseScore * 30)
			{
				iBaseScore[c] = -m_BaseScore * 30;
				//	log_info("99999999999 Calc_BaseScore iBaseScore[c = %d] = %d", c, iBaseScore[c]);
			}
		}

	}

	int iAnGangNum = 0;
	for (int i = 0; i < m_HandCard[ChairID].FixedCardsLen; i++)
	{
		if (m_HandCard[ChairID].FixedCards[i].state == TYPE_ANGANG)
		{
			iAnGangNum++;
		}
		
	}
	if (m_oFan[ChairID].Find(MJ_FAN_TYPE_QINGYISE)&&m_HandCard[ChairID].FixedCardsLen- iAnGangNum >=3)
	{//5.包赔如果清一色胡牌时有3 / 4句吃碰杠，放出第3句的玩家包赔3家分数
		int ChairIdOutFixCard = 0;
		for (int i = 0, indexnum = 0; i < m_HandCard[ChairID].FixedCardsLen; i++)
		{
			if (m_HandCard[ChairID].FixedCards[i].state == TYPE_ANGANG)
			{
				continue;
			}
			if (indexnum==2)
			{
				ChairIdOutFixCard = m_HandCard[ChairID].FixedCards[i].chairID;
			}
			indexnum++;
		}
		int TotalScore = 0;

		for (_uint8 c = NextChair(ChairID); c != ChairID; c = NextChair(c))
		{
			TotalScore += iBaseScore[c];
			//log_info("99999999999 Calc_BaseScore TotalScore = %d",  iBaseScore[c]);

		}
		for (_uint8 c = NextChair(ChairID); c != ChairID; c = NextChair(c))
		{
			if (ChairIdOutFixCard == c)
			{
				iBaseScore[c] = TotalScore;
			} 
			else
			{
				iBaseScore[c] = 0;

			}
			//log_info("99999999999 Calc_BaseScore iBaseScore[c = %d] = %d", c, iBaseScore[c]);

		}
	}
	else if (m_oFan[ChairID].Find(MJ_FAN_XIANNING_258JIANGYISE) && m_HandCard[ChairID].FixedCardsLen - iAnGangNum >= 3 && !m_oFan[ChairID].Find(MJ_FAN_TYPE_QINGYISE))
	{//5.包赔如果清一色胡牌时有3 / 4句吃碰杠，放出第3句的玩家包赔3家分数

		int ChairIdOutFixCard = 0;
		for (int i = 0, indexnum = 0; i < m_HandCard[ChairID].FixedCardsLen; i++)
		{
			if (m_HandCard[ChairID].FixedCards[i].state == TYPE_ANGANG)
			{
				continue;
			}
			if (indexnum == 2)
			{
				ChairIdOutFixCard = m_HandCard[ChairID].FixedCards[i].chairID;
			}
			indexnum++;
		}

		int TotalScore = 0;

		for (_uint8 c = NextChair(ChairID); c != ChairID; c = NextChair(c))
		{
			TotalScore += iBaseScore[c];
			//log_info("99999999999 Calc_BaseScore TotalScore = %d",  iBaseScore[c]);

		}
		for (_uint8 c = NextChair(ChairID); c != ChairID; c = NextChair(c))
		{
			if (ChairIdOutFixCard == c)
			{
				iBaseScore[c] = TotalScore;
			}
			else
			{
				iBaseScore[c] = 0;

			}
			//log_info("99999999999 Calc_BaseScore iBaseScore[c = %d] = %d", c, iBaseScore[c]);

		}
	}
	else if (m_oFan[ChairID].Find(MJ_FAN_TYPE_QUANQIUREN) && ChairID != m_bCurChairID)
	{//全球人封顶
		//log_info("+++++++++++++++++++++");

		tagListenInfos stListenInfo[14];
		_uint8 bListenLen = 0;
		//胡牌不用判断听牌，自动胡牌不用判断听牌
		int TotalScore = 0;
		int ChairIdOut = 0;
		if (m_bRobHu)
		{//抢杠胡
			ChairIdOut = m_WangGangPlayer;
		}
		else
		{
			ChairIdOut = m_bOutCardChairID;
		}

		HandCards tempHandCard = m_HandCard[ChairIdOut];
		_uint8 bGetCard = 0xFF;
		if (tempHandCard.ChangeableCardsLen != 0 && tempHandCard.ChangeableCardsLen % 3 != 2)
		{
			
			if (m_bXianNingGuiPai==0x01)
			{
				bGetCard = 0x02;
				m_GameLogic.AddCard(&tempHandCard, bGetCard);
			}
			else
			{
				bGetCard = 0x01;
				m_GameLogic.AddCard(&tempHandCard, bGetCard);
			}

		}
		if (GetListeningInfo_1(tempHandCard, ChairIdOut, stListenInfo, bListenLen, bGetCard) != TYPE_LISTEN)
		{
		//	log_info("+++++++++++++++++++++");
			for (_uint8 c = NextChair(ChairID); c != ChairID; c = NextChair(c))
			{
				TotalScore += iBaseScore[c];
			//	log_info("99999999999 Calc_BaseScore TotalScore = %d",  iBaseScore[c]);

			}
			for (_uint8 c = NextChair(ChairID); c != ChairID; c = NextChair(c))
			{
				if (ChairIdOut == c)
				{
					iBaseScore[c] = TotalScore;
				}
				else
				{
					iBaseScore[c] = 0;

				}
			}
		}
	}

//	log_info("===============================================================================");


}

void Table_xianningmajiang::Calc_HuScore(_uint8 ChairID, int iBaseScore[GAME_PLAYER], int iScore[GAME_PLAYER])
{
	if (m_bIsHu[ChairID])
	{

		for (_uint8 c = NextChair(ChairID); c != ChairID; c = NextChair(c))
		{
			iScore[ChairID] -= iBaseScore[c];
			iScore[c] += iBaseScore[c];
			m_stHuScore[ChairID].iTotalScore -= iBaseScore[c];
			m_stHuScore[c].iTotalScore += iBaseScore[c];
		//	log_info("m_stHuScore[ChairID= %d].iTotalScore = %d", ChairID, m_stHuScore[ChairID].iTotalScore);
		//	log_info("m_stHuScore[c = %d].iTotalScore = %d", c, m_stHuScore[c].iTotalScore);

		}


		log_info("tid:%d %s uid:%d chairid:%d, base score [%d,%d,%d,%d]\n",tid, __FUNCTION__, GetPlayerUid(ChairID), ChairID, iScore[0], iScore[1], iScore[2], iScore[3]);
	}

}

int Table_xianningmajiang::GetCardEstimate(_uint8 ChairID, _uint8 bCard, MJ_major& major, int& fan_num)
{
	if (m_conf_xianningmj.mj_conf_xianningmajiang == true)
	{
		return GetCardEstimate_0(ChairID, bCard, major, fan_num);
	}
	else
	{
		return GetCardEstimate_zhuanzhuan(ChairID, bCard, major, fan_num);

	}
}
/*
玩家自摸一张牌，此时判断是否可以听杠胡。
*/
int Table_xianningmajiang::GetCardEstimate_zhuanzhuan(_uint8 ChairID, _uint8 bCard, MJ_major& major, int& fan_num)
{
//	assert(major.m_oFan.Size() == 0);

	int iOpType = TYPE_NULL;

	HandCards* pHc = &m_HandCard[ChairID];
	m_GameLogic.AddCard(pHc, bCard);

	if (pHc->ChangeableCardsLen % 3 != 2)
	{
		log_error("%s fatal paishu error uid:%d chairid:%d!\n", __FUNCTION__, GetPlayerUid(ChairID), ChairID);
		m_GameLogic.ChuPai(pHc, bCard);
		return TYPE_NULL;
	}
//	assert(pHc->ChangeableCardsLen <= HAND_CARD_SIZE_MAX);

	/*获取杠信息*/
	iOpType |= GetGangInfo(ChairID, major.m_GetGangInfo);
	/*检查能不能胡*/
	iOpType |= Check_Hu(ChairID, *pHc, major.m_oFan);
	if (m_bGetCardCount[ChairID] == 1)
	{
		/*如果为第一次摸牌,并且胡牌则为天胡*/
		//UserGetFirstCard(ChairID, iOpType, major.m_oFan);
	}
	if (m_CardPool.Count() == 6)
	{
		/*最后一张胡牌则为海底捞*/
		//UserGetLastCard(ChairID, iOpType, major.m_oFan);
	}
	if (iOpType & TYPE_HU)
	{
		if (m_bAddFanFlag[ChairID])   //杠上花
		{
			//UserGangShangHua(ChairID, bCard, major.m_oFan);
		}
		/*计算番*/
		fan_num = Calc_Fan(ChairID, major.m_oFan);
	}

	m_GameLogic.ChuPai(pHc, bCard);

	return iOpType;
}

int Table_xianningmajiang::GetCardEstimate_0(_uint8 ChairID, _uint8 bCard, MJ_major& major, int& fan_num)
{
	//assert(major.m_oFan.Size() == 0);

	int iOpType = TYPE_NULL;

	HandCards* pHc = &m_HandCard[ChairID];
	m_GameLogic.AddCard(pHc, bCard);

	if (pHc->ChangeableCardsLen % 3 != 2)
	{
		log_error("tid:%d %s fatal paishu error uid:%d chairid:%d!\n",tid, __FUNCTION__, GetPlayerUid(ChairID), ChairID);
		m_GameLogic.ChuPai(pHc, bCard);
		return TYPE_NULL;
	}
	//assert(pHc->ChangeableCardsLen <= HAND_CARD_SIZE_MAX);

	/*获取杠信息*/
	iOpType |= GetGangInfo(ChairID, major.m_GetGangInfo);
	/*检查能不能胡*/
	iOpType |= Check_Hu(ChairID, *pHc, major.m_oFan);

	if (m_bGetCardCount[ChairID] == 1)
	{
		/*如果为第一次摸牌,并且胡牌则为天胡*/
		//UserGetFirstCard(ChairID, iOpType, major.m_oFan);
	}

	if (iOpType & TYPE_HU)
	{
		if (m_CardPool.Count() < 15)
		{
			/*最后一张胡牌则为海底捞*/
			UserGetLastCard(ChairID, iOpType, major.m_oFan);
		}

		if (m_bAddFanFlag[ChairID]|| m_bUserOperateLzHzGangFlag[ChairID] == true)   //杠上花
		{
			if (major.m_oFan.Find(MJ_FAN_TYPE_HAIDILAOYUE))
			{
				major.m_oFan.Remove(MJ_FAN_TYPE_HAIDILAOYUE);
			}
			UserGangShangHua(ChairID, bCard, major.m_oFan);

		}
		/*计算番*/
		fan_num = Calc_Fan(ChairID, major.m_oFan);
		if (m_oFan[ChairID].Find(MJ_FAN_TYPE_GANGSHANGHUA) && (!m_oFan[ChairID].Find(MJ_FAN_TYPE_PINGHU)))
		{
			fan_num -= 1;//杠上花1番（平胡才加，大胡不加）

		}

		if(!major.m_oFan.Find(MJ_FAN_TYPE_PINGHU))
		{
			fan_num += 1;//自摸胡牌加一蕃
		}
		int gang_fan_num = 0;
	//	log_info("GetCardEstimate fan_num = %d", fan_num);

		for (int c = NextChair(ChairID); c != ChairID; c = NextChair(c))
		{
			gang_fan_num = Calc_GangFan(c);
		//	log_info("GetCardEstimate gang_fan_num = %d", gang_fan_num);
			for (int i = 0; i < m_HandCard[c].FixedCardsLen;i++)
			{
				if (m_HandCard[c].FixedCards[i].state!=TYPE_ANGANG)
				{
					gang_fan_num += 1;
				//	log_info("GetCardEstimate gang_fan_num = %d", gang_fan_num);

					break;
				}
			}
			if (fan_num + gang_fan_num >=3)
			{
				iOpType |= TYPE_HU;
			//	log_info("GetCardEstimate fan_num + gang_fan_num  = %d", fan_num + gang_fan_num);

				break;
			}
			else
			{
				iOpType &= ~(TYPE_HU);
			}
		}
	}
	int bUserHasGuiPaiNum = 0;
	for (int i = 0; i < m_HandCard[ChairID].ChangeableCardsLen;i++)
	{
		if (m_HandCard[ChairID].ChangeableCards[i]== m_bXianNingGuiPai)
		{
			bUserHasGuiPaiNum++;
		}
	}

	if (major.m_oFan.Find(MJ_FAN_TYPE_PINGHU)&& bUserHasGuiPaiNum>1&& !major.m_oFan.Find(MJ_FAN_TYPE_GANGSHANGHUA)&& !major.m_oFan.Find(MJ_FAN_TYPE_HAIDILAOYUE))
	{
		//log_info("111111111111111GetCardEstimate  %s   uid:%d chairid:%d! iOpType= %x\n", __FUNCTION__, GetPlayerUid(ChairID), ChairID, iOpType);
		iOpType &= ~(TYPE_HU);
	}
	//log_info("111111111111111GetCardEstimate  %s   uid:%d chairid:%d! iOpType= %x\n", __FUNCTION__, GetPlayerUid(ChairID), ChairID, iOpType);
	m_GameLogic.ChuPai(pHc, bCard);
	return iOpType;
}

void Table_xianningmajiang::GetAllHandCardRelativity(HandCards *pHc, MJ_BUFFER<_uint8, POOL_CARD_TYPE_MAX>& _relativity)
{
	_uint8 last_check = INVALID_MJ_CARD;

	GetHandCardRelativity(last_check, _relativity);
}

void Table_xianningmajiang::GetHandCardRelativity(_uint8 bCard, MJ_BUFFER<_uint8, POOL_CARD_TYPE_MAX>& _relativity)
{

	for (int i = 1; i <= 9; i++)
	{
		_relativity.Add(i);
	}
	for (int i = 0x11; i <= 0x19; i++)
	{
		_relativity.Add(i);
	}
	for (int i = 0x21; i <= 0x29; i++)
	{
		_relativity.Add(i);
	}

}
bool Table_xianningmajiang::RobGangHu(_uint8 ChairID, _uint8 CardData, int iOpType)
{
	if (m_conf_xianningmj.mj_conf_xianningmajiang== true)
	{
		return RobGangHu_0(ChairID, CardData, iOpType);
	}
	else
	{
		return Table::RobGangHu(ChairID, CardData, iOpType);
	}
}

bool Table_xianningmajiang::RobGangHu_0(_uint8 ChairID, _uint8 CardData, int iOpType)
{
	log_info("%s tid:%d uid:%d ChairID:%d, iOpType:0x%x, CardData:0x%02x \n", __FUNCTION__, tid, GetPlayerUid(ChairID), ChairID, iOpType, CardData);

	//log_info("+++++++++++++++++++++++++++++++++++++++ RobGangHu START");
	if (iOpType & TYPE_WANGANG && !m_conf.m_bQiangGangHu)
		return false;
	if (iOpType & TYPE_ZHIGANG && !m_conf.m_bQiangZhiGang)
		return false;
	if (iOpType & TYPE_ANGANG && !m_conf.m_bQiangAnGang)
		return false;
	memset(&M_bSendUserQiangAnGangHuFlag, 0, sizeof(M_bSendUserQiangAnGangHuFlag));//
	memset(&M_bUserDoMultiOperateQiangAnGang, TYPE_NULL, sizeof(M_bUserDoMultiOperateQiangAnGang));//
	memset(&M_bCalcUserQiangAnGangHuFlag, 0, sizeof(M_bCalcUserQiangAnGangHuFlag));//
	memset(&m_bQiangAnGangTingCard, 0, sizeof(m_bQiangAnGangTingCard));//
	memset(&m_OnUserQiangAnGangCard, 0, sizeof(m_OnUserQiangAnGangCard));//
	memset(&m_OnUserQiangAnCalcScoreFlag, 0, sizeof(m_OnUserQiangAnCalcScoreFlag));//

	
	_uint8 cbNum = 0;
	for (int i = 0; i < m_GAME_PLAYER; i++)
	{
		if (i == ChairID)
		{
			continue;
		}
		MJ_major major;
		m_bRobGangHuJudge = true;
		m_WangGangPlayerOpType = iOpType;
		m_bUserOperate[i] = OutCardEstimate_QiangGangHu(i, CardData, major, m_HuFanNums[i]);
		//log_info("++++++++++++++++++++m_bUserOperate[i = %d] = %x",i, m_bUserOperate[i]);

		m_bRobGangHuJudge = false;
		m_WangGangPlayerOpType = TYPE_NULL;
		m_bUserOperate[i] &= TYPE_HU;
		if (m_bUserOperate[i] & TYPE_HU)
		{
			const HandCards& hc = m_HandCard[ChairID];
			MJ_opinfo opinfo;

			if (iOpType & TYPE_WANGANG && GetWanGang(&hc, opinfo, CardData))
			{
				M_bSendUserQiangAnGangHuFlag[i] = true;
				for (int c = 0; c < m_GAME_PLAYER; c++)
				{
					m_bUserQiangZhiGangHuPass = true;
					proto::game::AckOpNotify stNotify;
					Proto_Game_OpNotify_QiangGangHu(i, c, CardData, stNotify);
					unicast(SERVER_OPERATE_NOTIFY, c, stNotify);
					std::string str = stNotify.SerializeAsString();
					save_video_data(SERVER_OPERATE_NOTIFY, str);
				}
				cbNum++;
			}
			else if (iOpType & TYPE_ZHIGANG && GetZhiGang(&hc, opinfo, CardData))
			{
				M_bSendUserQiangAnGangHuFlag[i] = true;
				m_bUserQiangZhiGangHuPass = true;
				for (int c = 0; c < m_GAME_PLAYER; c++)
				{
					proto::game::AckOpNotify stNotify;
					Proto_Game_OpNotify_QiangGangHu(i, c, CardData, stNotify);
					unicast(SERVER_OPERATE_NOTIFY, c, stNotify);
					std::string str = stNotify.SerializeAsString();
					save_video_data(SERVER_OPERATE_NOTIFY, str);
				}
				cbNum++;
			}
			else if (iOpType & TYPE_ANGANG && GetAnGang(&hc, opinfo, CardData))
			{
				//for (int c = 0; c < m_GAME_PLAYER; c++)
				//{
				//	proto::game::AckOpNotify stNotify;
				//	Proto_Game_OpNotify(i, c, CardData, stNotify);
				//	unicast(SERVER_OPERATE_NOTIFY, c, stNotify);
				//	std::string str = stNotify.SerializeAsString();
				//	save_video_data(SERVER_OPERATE_NOTIFY, str);
				//}
				//cbNum++;
			}
		}
	}
	if (iOpType & TYPE_ANGANG)
	{
		for (_uint8 i = 0; i < m_GAME_PLAYER; i++)
		{
			if (i == ChairID)
			{
				continue;
			}
			HandCards tmp = m_HandCard[i];
			ShowCards bTingCard;
			if (Check_Listening_AnGang(&tmp, i, bTingCard))
			{			
				m_bQiangAnGangTingCard[i].Clear();
				for (int z = 0; z < bTingCard.Size();z++)
				{
					bool bOutFlag = false;
					for (_uint8 k = 0; k < m_GAME_PLAYER; k++)
					{
						for (int kk = 0; kk < m_stUserOutCard[k].Size(); kk++)
						{
							if (bTingCard[z] == m_stUserOutCard[k][kk])
							{
								bOutFlag = true;
								break;
							}
						}
						for (int kk = 0; kk < m_HandCard[k].FixedCardsLen; kk++)
						{
							if (m_HandCard[k].FixedCards[kk].state&TYPE_CHI)
							{
								for (int kkk = 0; kkk < 3; kkk++)
								{
									if (m_HandCard[k].FixedCards[kk].c[kkk] == bTingCard[z])
									{
										bOutFlag = true;
										break;
									}
								}
							}
							else if (m_HandCard[k].FixedCards[kk].state != TYPE_ANGANG)
							{
								if (m_HandCard[k].FixedCards[kk].CardData == bTingCard[z])
								{
									bOutFlag = true;
									break;
								}
							}
						}
						for (int kk = 0; kk < m_HandCard[i].ChangeableCardsLen;kk++)
						{
							if (bTingCard[z]== m_HandCard[i].ChangeableCards[kk])
							{
								bOutFlag = true;
								break;
							}
						}
						if (bOutFlag == true)
						{
							break;
						}
					}
					if (bOutFlag == false)
					{
						m_bQiangAnGangTingCard[i].Add(bTingCard[z]);
					}
				}
				if (m_bQiangAnGangTingCard[i].Size() == 0)
				{
					continue;
				}

				m_bUserQiangZhiGangHuPass = true;
				for (int c = 0; c < m_GAME_PLAYER; c++)
				{
					proto::game::AckOpNotify stNotify;
					Proto_Game_OpNotify_QiangAnGangHu(i, c, CardData, stNotify, m_bQiangAnGangTingCard[i]);
					unicast(SERVER_OPERATE_NOTIFY, c, stNotify);
					std::string str = stNotify.SerializeAsString();
					save_video_data(SERVER_OPERATE_NOTIFY, str);
					log_info("tid:%d stNotify = %s",tid, stNotify.DebugString().c_str());
				}
				m_RobAnGangHuNum++;
				cbNum++;
				M_bSendUserQiangAnGangHuFlag[i] = true;
				set_operator_timer(i);
			}
		}
	}

	//开启操作时钟 TODO
	for (int i = 0; i < m_GAME_PLAYER; i++)
	{
		if (i == ChairID)
		{
			continue;
		}
		log_info("RobGangHu tid:%d uid:%d chaird:%d out card, so that chairid:%d have operate:0x%x\n", tid, GetPlayerUid(ChairID), ChairID, i, m_bUserOperate[i]);
		if (m_bUserOperate[i] & TYPE_HU)
		{
			set_operator_timer(i);
		}

	}
	if (cbNum > 0)
	{
		memset(&M_bUserDoMultiOperate, 0, sizeof(M_bUserDoMultiOperate));//
		m_RobHuNum = cbNum;
		m_bRobWanGang = true;
		m_WangGangPlayer = ChairID;
		m_WangGangPlayerOpType = iOpType;
		m_RobWanGangCard = CardData;
		log_info("RobGangHu tid:%d robot ganghu, and hu user m_RobHuNum:%d m_RobAnGangHuNum = %d\n",tid, m_RobHuNum, m_RobAnGangHuNum);
	}

	return (cbNum > 0);
}


void Table_xianningmajiang::Proto_Game_OpNotify_QiangGangHu(_uint8 ChairID, _uint8 CurChairID, _uint8 bCard, proto::game::AckOpNotify& stNotify)
{

	stNotify.Clear();
	stNotify.set_chairid(ChairID);
	//非提示玩家,下面的协议字段为空
	if (CurChairID == ChairID)
	{
		stNotify.set_optype(TYPE_YX_YOUJINHU);
		stNotify.set_card(bCard);
	}

}


void Table_xianningmajiang::Proto_Game_OpNotify_QiangAnGangHu(_uint8 ChairID, _uint8 CurChairID, _uint8 bCard, proto::game::AckOpNotify& stNotify,ShowCards &buf)
{

	stNotify.Clear();
	stNotify.set_chairid(ChairID);
	//非提示玩家,下面的协议字段为空
	if (CurChairID == ChairID)
	{
		stNotify.set_optype(TYPE_YX_YOUJINHU);
		stNotify.set_card(bCard);
		if (buf.Size() < 20)
		{
			for (int i = 0; i < buf.Size(); i++)
			{
				stNotify.add_banhugang(buf[i]);
			}
		}
		else
		{
			stNotify.add_banhugang(0x255);
		}
	}
}

int Table_xianningmajiang::Calc_Hu(_uint8 ChairID, _uint8 bCard, int Score[GAME_PLAYER])
{
	log_info("Calc_Hu tid:%d uid:%d chaird:%d have operate:0x%x bCard:0x%x\n",tid, GetPlayerUid(ChairID),  ChairID, m_bUserOperate[ChairID], bCard);
	if (m_CardPool.Count() <14)
	{
		m_bCurChairID = ChairID;
	}
	m_oFan[ChairID].Clear();
	m_HuFanNums[ChairID] = 0;

	MJ_major major;
	if (m_bRobHu)
	{
		int iOpType = TYPE_NULL;
		if (m_conf_xianningmj.mj_conf_xianningmajiang == true)
		{
			iOpType = OutCardEstimate_QiangGangHu(ChairID, bCard, major, m_HuFanNums[ChairID]);
			log_info("Calc_Hu tid:%d uid:%d chaird:%d have iOpType:0x%x bCard:0x%x\n", tid, GetPlayerUid(ChairID), ChairID, iOpType, bCard);

		}
		else
		{
			iOpType = OutCardEstimate(ChairID, bCard, major, m_HuFanNums[ChairID]);
			log_info("Calc_Hu tid:%d uid:%d chaird:%d have iOpType:0x%x bCard:0x%x\n", tid, GetPlayerUid(ChairID), ChairID, iOpType, bCard);

		}
		if (iOpType & TYPE_HU)
		{
			m_oFan[ChairID] = major.m_oFan;
			Score[ChairID] += m_HuFanNums[ChairID] * m_BaseScore;
			Score[m_WangGangPlayer] -= m_HuFanNums[ChairID] * m_BaseScore;

			m_stHuScore[ChairID].stHu.Add(tagHuRecord(m_WangGangPlayer, bCard, HU_PAOHU, m_HuFanNums[ChairID]));
		}
	}
	else if (ChairID == m_bCurChairID)
	{//自摸
		int iOpType = GetCardEstimate(ChairID, INVALID_MJ_CARD, major, m_HuFanNums[ChairID]);
		log_info("Calc_Hu tid:%d uid:%d chaird:%d have iOpType:0x%x bCard:0x%x\n", tid, GetPlayerUid(ChairID), ChairID, iOpType, bCard);

		if (iOpType & TYPE_HU)
		{
			m_oFan[ChairID] = major.m_oFan;

			for (_uint8 c = NextChair(ChairID); c != ChairID; c = NextChair(c))
			{
				Score[ChairID] += m_HuFanNums[ChairID] * m_BaseScore;
				Score[c] -= m_HuFanNums[ChairID] * m_BaseScore;
				//log_debug("Score[ChairID] = %d", Score[ChairID]);
			}
			m_stHuScore[ChairID].stHu.Add(tagHuRecord(ChairID, bCard, HU_ZIMO, m_HuFanNums[ChairID]));
		}
	}
	else
	{//放炮
		if (bCard != m_bCurOutCard)
		{
			bCard = m_bCurOutCard;
		}
		m_bUserOperateGuoZhangHuFlag[ChairID] = false;
		int iOpType = OutCardEstimate(ChairID, bCard, major, m_HuFanNums[ChairID]);
		log_info("Calc_Hu tid:%d uid:%d chaird:%d have iOpType:0x%x bCard:0x%x\n", tid, GetPlayerUid(ChairID), ChairID, iOpType, bCard);

		if (iOpType & TYPE_HU)
		{
			m_oFan[ChairID] = major.m_oFan;
			Score[ChairID] += m_HuFanNums[ChairID] * m_BaseScore;
			Score[m_bCurChairID] -= m_HuFanNums[ChairID] * m_BaseScore;

			m_stHuScore[ChairID].stHu.Add(tagHuRecord(m_bCurChairID, bCard, HU_PAOHU, m_HuFanNums[ChairID]));
		}
	}

	return 0;

}


void Table_xianningmajiang::GameEndAndGetMaNum()
{
	memset(&m_bGetMaPai, 0, sizeof(m_bGetMaPai));
	memset(&m_bZhongMaPai, 0, sizeof(m_bZhongMaPai));

	if (m_conf_xianningmj.mj_conf_zhuanzhuan_zhuaniao == 0)
	{
		return;
	}
	//摸完最后一张牌才算是结束
	MJ_user_buffer UserID;
	_uint8 bDraw = MultiHu(UserID);
	if (bDraw == 0)
	{
		return;
	}
	_uint8 zhongmaChairid = 0;
	if (m_bRobHu)
	{//抢杠胡
		if (bDraw == 1)
		{
			zhongmaChairid = UserID[0];
		}
		else
		{
			zhongmaChairid = m_WangGangPlayer;
		}

	}
	else
	{
		if (bDraw == 1)
		{
			zhongmaChairid = UserID[0];
		}
		else
		{
			zhongmaChairid = m_bOutCardChairID;
		}
	}

	_uint8   GetMaPaiCards[20] = { 0 };
	int MaPaiCardNum = (m_CardPool.Count() > m_conf_xianningmj.mj_conf_zhuanzhuan_zhuaniao) ? m_conf_xianningmj.mj_conf_zhuanzhuan_zhuaniao : m_CardPool.Count();
	m_CardPool.GetCard(GetMaPaiCards, MaPaiCardNum);

	for (int i = 0; i < MaPaiCardNum; i++)
	{
		m_bGetMaPai.Add(GetMaPaiCards[i]);
		_uint8 num = CardNum(GetMaPaiCards[i]);
		if (1 == num || 5 == num || 9 == num)
		{
			if (GetMaPaiCards[i] <0x41)
			{
				m_bZhongMaPai[zhongmaChairid].Add(GetMaPaiCards[i]);
			}
		}
	}
}

bool Table_xianningmajiang::Is7Dui(_uint8 ChairID, HandCards& pHc, MJ_fan_buffer& oFan)
{
	if (m_conf.m_bHu7dui == false)
	{
		return false;
	}

	HandCards tmp;
	if (pHc.ChangeableCardsLen < 14)
	{
		return false;
	}
	for (int i = 0; i < pHc.ChangeableCardsLen; i++)
	{
		if (pHc.ChangeableCards[i] == m_bXianNingGuiPai)
			continue;
		m_GameLogic.AddCard(&tmp, pHc.ChangeableCards[i]);
	}

	int DuiNum = 0;
	for (int i = 0; i < tmp.ChangeableCardsLen - 1; i++)
	{
		if (tmp.ChangeableCards[i] == tmp.ChangeableCards[i + 1])
		{
			DuiNum++;
			i++;
		}
	}
	int bUserHasGuiPaiNum = 0;
	for (int i = 0; i < pHc.ChangeableCardsLen; i++)
	{
		if (pHc.ChangeableCards[i] == m_bXianNingGuiPai)
		{
			bUserHasGuiPaiNum++;
		}
	}
	if (DuiNum + bUserHasGuiPaiNum >= 7)
	{
		if (m_conf.m_bHu7dui)
		{
			oFan.Add(MJ_FAN_TYPE_QIDUI);
			return true;
		}
	}
	return false;
}


void Table_xianningmajiang::Score_GameEndSucc(HuScoreSet& score, proto::game::AckGameEnd& stGameend)
{
	int iTotalScore[GAME_PLAYER] = { 0 };
	int iBaseScore[GAME_PLAYER][GAME_PLAYER];
	int iScore[GAME_PLAYER][GAME_PLAYER];
	memset(iBaseScore, 0, sizeof(iBaseScore));
	memset(iScore, 0, sizeof(iScore));
	for (int i = 0; i < m_GAME_PLAYER; i++)
	{
		Calc_BaseScore(i, iBaseScore[i]);
	}
	for (int i = 0; i < m_GAME_PLAYER; i++)
	{
		m_stHuScore[i].iTotalScore = 0;
	}
	for (int i = 0; i < m_GAME_PLAYER; i++)
	{
		Calc_HuScore(i, iBaseScore[i], iScore[i]);
	}
	if (m_conf.m_bCalcAllNiao)
	{
		for (int i = 0; i < m_GAME_PLAYER; i++)
		{
			for (_uint8 j = 0; j < m_niao.Size(); j++)
			{
				Calc_ZhongNiao(i, m_niao[j], iBaseScore[i], iScore[i]);
			}
		}
	}
	else
	{
		for (int i = 0; i < m_GAME_PLAYER; i++)
		{
			Calc_ZhongNiao(i, INVALID_MJ_CARD, iBaseScore[i], iScore[i]);
		}
	}
	for (int i = 0; i < m_GAME_PLAYER; i++)
	{
		Calc_FengDing(i, iScore[i]);
	}
	for (int i = 0; i < m_GAME_PLAYER; i++)
	{
		Calc_Score(iTotalScore, iScore[i]);
	}
	for (int i = 0; i < m_GAME_PLAYER; i++)
	{
		score[i].iTotleScore = iTotalScore[i];
	}

}

void Table_xianningmajiang::Score_GameEndDraw(HuScoreSet& score, proto::game::AckGameEnd& stGameend)
{
	
	if (m_conf.m_bHuangGang)
	{
		memset(m_stGang, 0, sizeof(m_stGang));
	}
}


//出牌后其他玩家操作判断,并提示其它玩家
bool Table_xianningmajiang::EstimateUserRespond(_uint8 bOutChairID, _uint8 bOutCard)
{
	if (m_conf_xianningmj.mj_conf_xianningmajiang == true)
	{
		
		if (m_CardPool.Count() <14)
		{
			return false;
		}
		if (m_conf_xianningmj.mj_conf_xianningmajiang_hongzhonglaizi_gang==true)
		{
			if (m_bUserOutHongZhongLaiZiCardFlag[bOutChairID] == true)
			{
				m_bUserOutHongZhongLaiZiCardFlag[bOutChairID] = false;
				if (bOutCard == 0x41 || bOutCard == m_bXianNingGuiPai)
				{
					return false;
				}
				return EstimateUserRespond_0(bOutChairID, bOutCard);
			}
			else
			{
				return EstimateUserRespond_0(bOutChairID, bOutCard);
			}

		}
		else
		{
			if (bOutCard == 0x41 || bOutCard == m_bXianNingGuiPai)
			{
				return false;
			}
			return EstimateUserRespond_0(bOutChairID, bOutCard);
		}

	}
	return Table::EstimateUserRespond(bOutChairID, bOutCard);
}


bool Table_xianningmajiang::EstimateUserRespond_0(_uint8 bOutChairID, _uint8 bOutCard)
{
	m_bMultiOpUserNum = 0;		//在有多操作提示的情况下，选择操作的玩家个数
	m_bMultiOpHuNum = 0;		//选择胡操作的玩家数
	bool bIsChangeUser = true;  //是否轮到下一玩家摸牌

	memset(m_bUserOperate, TYPE_NULL, sizeof(m_bUserOperate));   //玩家的操作清零
	for (int i = 0; i < m_GAME_PLAYER; i++)
	{
		if (i == bOutChairID)
		{
			m_bUserOperate[i] = TYPE_NULL;
			continue;
		}
		//牌型判断			
		MJ_major major;
		//判断能不能进行吃碰杠一类的操作
		if (bOutCard != 0x41&& bOutCard != m_bXianNingGuiPai)
		{
			m_bUserOperate[i] = OutCardEstimate(i, bOutCard, major, m_HuFanNums[i]);

		}

	}
	for (int i = 0; i < m_GAME_PLAYER; i++)
	{
		int  bUserOperate = TYPE_NULL;
		if (m_bUserOperate[i] != TYPE_NULL)
		{
			if (m_bUserOperate[i] & TYPE_HU)
			{
				//m_bAddFanFlag[i] = true;
				m_bMultiOpHuNum++;
				//log_debug("has hu");
			}
			m_bMultiOpUserNum++;
			bIsChangeUser = false;
			
			//准备向四个玩家发送操作提示，每个玩家的操作其它玩家都需要知道
			for (int j = 0; j < m_GAME_PLAYER; j++)
			{
				proto::game::AckOpNotify stNotify;
				if (m_bUserOperate[i] & TYPE_HU && (!(m_bUserOperate[i] & TYPE_ZHIGANG)))
				{
					bool ZhiGangFlag = false;
				
					for (int c = NextChair(i); c != i; c = NextChair(c))
					{
						//log_info("m_bUserOperate[c = %d] = %x", c, m_bUserOperate[c]);

						if (m_bUserOperate[c] & TYPE_ZHIGANG)
						{
							m_bXianNingZhiGangCard = bOutCard;
							ZhiGangFlag = true;
							break;;
						}
					}
					if (ZhiGangFlag == false)
					{
						bUserOperate = m_bUserOperate[i];
					}
					//log_info("bUserOperate= %x", bUserOperate);

					stNotify.Clear();
					stNotify.set_chairid(i);
					//非提示玩家,下面的协议字段为空
					if (i == j)
					{
						stNotify.set_optype(bUserOperate);
						stNotify.set_card(bOutCard);
					}
				}
				else
				{
					Proto_Game_OpNotify(i, j, bOutCard, stNotify);
				}
				unicast(SERVER_OPERATE_NOTIFY, j, stNotify);
				//保存获取牌之后的操作提示录像
				if (m_bUserOperate[i] >0)
				{
					std::string str = stNotify.SerializeAsString();
					save_video_data(SERVER_OPERATE_NOTIFY, str);
				}
			}
		}
		if (m_bUserOperate[i] != 0)
		{
			log_info("%s tid:%d uid:%d chairid:%d Operate:0x%x, totalop:%d \n", __FUNCTION__,tid, GetPlayerUid(i), i, m_bUserOperate[i], m_bMultiOpUserNum);
		}
		if (m_bUserOperate[i] != 0)
		{
			log_info("%s tid:%d uid:%d chairid:%d bUserOperate:0x%x, totalop:%d \n", __FUNCTION__, tid, GetPlayerUid(i), i, bUserOperate, m_bMultiOpUserNum);
		}
	}

	if (bIsChangeUser)
	{
		m_bCurChairID = NextChair(bOutChairID);
		memset(m_bAddFanFlag, 0, sizeof(m_bAddFanFlag));
		OnUserGetCard(m_bCurChairID);
	}
	else
	{
		//开启操作时钟 TODO		
		for (_uint8 c = NextChair(bOutChairID); c != bOutChairID; c = NextChair(c))
		{
			if (m_bUserOperate[c] != TYPE_NULL)
			{
				set_operator_timer(c);
			}
		}
	}

	return bIsChangeUser;
}

void Table_xianningmajiang::OnRecvUserOperate(_uint8 ChairID, int iOpType, int iCard)
{
	_uint8 bCard = (_uint8)iCard;

	/*检查用户选择的操作*/
	if (!CheckUserOperator(ChairID, iOpType, bCard))
	{
		return;
	}

	stop_operator_timer(ChairID);
	m_bOverTimeState[ChairID] = 0;
	//如果是多操作

	if (m_bUserOperate[ChairID] & TYPE_HU&&iOpType != TYPE_HU)
	{
		m_bUserOperateGuoZhangHuFlag[ChairID] = true;
	}

	if (m_bMultiOpUserNum > 1)
	{
		DoMultiOperate(ChairID, iOpType, bCard);
	}
	else
	{
		if (m_RobAnGangHuNum > 0)
		{
			RobGangPriority_QiangAnGang(ChairID, bCard, iOpType);
			return;
		}
		else if (m_bRobWanGang)  //抢杠胡操作优先级
		{
			RobGangPriority(ChairID, bCard, iOpType);
			return;
		}
		DoSimpleOperate(ChairID, iOpType, bCard);
		//判断是下一玩家摸牌，或者当前玩家出牌		
	}
}

//抢杠胡操作优先级
void Table_xianningmajiang::RobGangPriority(_uint8 ChairID, _uint8 CardData, int OperateType)
{
	if (m_conf_xianningmj.mj_conf_xianningmajiang == true)
	{
		if (M_bUserDoMultiOperate[ChairID] == true)
		{
			return;
		}
		M_bUserDoMultiOperate[ChairID] = true;
	}

	stop_operator_timer(ChairID);

	m_RobOperateNum++;
	m_RobHuDoneOp[ChairID] = OperateType;
	if (TYPE_HU == OperateType)
	{
		m_bRobHu = true;
		m_bAddFanFlag[m_WangGangPlayer] = true;
		log_info("RobGangPriority m_conf.m_bYiPaoDuoXiang = %d", m_conf.m_bYiPaoDuoXiang);

		if (m_conf.m_bYiPaoDuoXiang)
		{
			m_bIsHu[ChairID] = true;
			UserOperateHu(ChairID, m_RobWanGangCard);

			if (m_conf.m_bForceHuQiangGangHu)
			{
				RobGangForceHu(ChairID, m_RobWanGangCard);
			}

		}
	}
	else if (OperateType == TYPE_PASS)
	{
		UserOperatePass(ChairID, OperateType, CardData);
		m_PassHuPlayer.Add(ChairID);
	}
	if (m_conf_xianningmj.mj_conf_xianningmajiang == true)
	{
	//	log_info("m_RobHuNum = %d", m_RobHuNum);
		if (m_RobHuNum > 1)
		{//如果优先级高的玩家选择胡牌，不必等待优先级低的玩家的操作。
	//		log_info("tid:%d RobGangPriority m_RobHuDoneOp[ChairID = %d] = %x",tid, ChairID, m_RobHuDoneOp[ChairID]);
	//		log_info("tid:%d RobGangPriority m_bUserOperate[ChairID = %d] = %x",tid, ChairID, m_bUserOperate[ChairID]);
	//		log_info("tid:%d RobGangPriority m_WangGangPlayerOpType = %x", m_WangGangPlayerOpType);

			 if (m_WangGangPlayerOpType & TYPE_ANGANG)
			 {
	//			 log_info("tid:%d RobGangPriority m_RobHuDoneOp[ChairID = %d] = %x", tid, ChairID, m_RobHuDoneOp[ChairID]);
	//			 log_info("tid:%d RobGangPriority M_bSendUserQiangAnGangHuFlag[ChairID = %d] = %d", tid, ChairID, M_bSendUserQiangAnGangHuFlag[ChairID]);

				 if (m_RobHuDoneOp[ChairID] == TYPE_HU && M_bSendUserQiangAnGangHuFlag[ChairID] ==true)
				 {

					 for (_uint8 c = NextChair(ChairID); c != m_WangGangPlayer; c = NextChair(c))
					 {//把比自己优先级低的玩家直接pass
		//				 log_info("tid:%d RobGangPriority M_bSendUserQiangAnGangHuFlag[c = %d] = %x", tid, c, M_bSendUserQiangAnGangHuFlag[c]);
		//				 log_info("tid:%d RobGangPriority M_bUserDoMultiOperate[c = %d] = %x", tid, c, M_bUserDoMultiOperate[c]);

						 if (M_bSendUserQiangAnGangHuFlag[c] ==true &&M_bUserDoMultiOperate[c] == false)
						 {
		//					 log_info("%s tid:%d uid:%d c:%d OperateNum:%d RobHuNum:%d OperateType:%0x\n", __FUNCTION__, tid, GetPlayerUid(c), c, m_RobOperateNum, m_RobHuNum, OperateType);
							 M_bUserDoMultiOperate[c] = true;
							 m_RobHuDoneOp[c] = TYPE_PASS;
							 m_RobOperateNum++;
							// UserOperatePass(c, TYPE_PASS, CardData);
						 }
					 }
				 }
			 }
			 else
			 {
				 if (m_RobHuDoneOp[ChairID] == TYPE_HU && m_bUserOperate[ChairID] & TYPE_HU)
				 {
					 for (_uint8 c = NextChair(ChairID); c != m_WangGangPlayer; c = NextChair(c))
					 {//把比自己优先级低的玩家直接pass
						 if (m_bUserOperate[c] & TYPE_HU&&M_bUserDoMultiOperate[c] == false)
						 {
			//				 log_info("%s tid:%d uid:%d c:%d OperateNum:%d RobHuNum:%d OperateType:%0x\n", __FUNCTION__, tid, GetPlayerUid(c), c, m_RobOperateNum, m_RobHuNum, OperateType);
							 M_bUserDoMultiOperate[c] = true;
							 m_RobHuDoneOp[c] = TYPE_PASS;
							 m_RobOperateNum++;
							 //UserOperatePass(c, TYPE_PASS, CardData);

						 }
					 }
				 }
			 }
		}
	}


	log_info("%s tid:%d uid:%d chairid:%d OperateNum:%d RobHuNum:%d OperateType:%0x\n", __FUNCTION__,tid, GetPlayerUid(ChairID), ChairID, m_RobOperateNum, m_RobHuNum, OperateType);
	if (m_RobOperateNum == m_RobHuNum)  //所有胡玩家都已经操作完毕
	{
		if (m_bRobHu)   //如果有人胡牌
		{
			if (!m_conf.m_bYiPaoDuoXiang)
			{
				for (_uint8 c = NextChair(m_WangGangPlayer); c != m_WangGangPlayer; c = NextChair(c))
				{
		//			log_info("RobGangPriority tid:%d m_bUserOperate[c = %d] = %x",tid, c, m_bUserOperate[c]);

					if (m_bUserOperate[c] & TYPE_HU)
					{
						m_bIsHu[c] = true;
						UserOperateHu(c, m_RobWanGangCard);
						break;
					}
				}
			}
			if (m_WangGangPlayerOpType & (TYPE_ANGANG | TYPE_WANGANG))
				m_GameLogic.RemoveCard(&m_HandCard[m_WangGangPlayer], m_RobWanGangCard, 1);


			GameEnd();

		}
		else
		{
			if (m_bUserQiangZhiGangHuPass == false)
			{
				UserOperateGang(m_WangGangPlayer, m_WangGangPlayerOpType, m_RobWanGangCard);

			}
			m_bUserQiangZhiGangHuPass = false;
			OnUserGetCard(m_WangGangPlayer);
		}

	}
	if (m_RobOperateNum == m_RobHuNum)
	{
		m_RobAnGangHuNum = 0;//可以抢暗杠的人数。
		m_bRobWanGang = false;
		m_bRobHu = false;
		m_RobOperateNum = 0;
		m_RobHuNum = 0;

		m_WangGangPlayer = INVALID_CHAIRID;
		m_WangGangPlayerOpType = TYPE_NULL;
		m_RobWanGangCard = INVALID_MJ_CARD;
		memset(&m_RobHuDoneOp, 0, sizeof(m_RobHuDoneOp));
		m_PassHuPlayer.Clear();

		memset(&M_bSendUserQiangAnGangHuFlag, 0, sizeof(M_bSendUserQiangAnGangHuFlag));//
		memset(&M_bUserDoMultiOperateQiangAnGang, TYPE_NULL, sizeof(M_bUserDoMultiOperateQiangAnGang));//
		memset(&M_bCalcUserQiangAnGangHuFlag, 0, sizeof(M_bCalcUserQiangAnGangHuFlag));//
		memset(&m_OnUserQiangAnGangCard, 0, sizeof(m_OnUserQiangAnGangCard));//

	}
}

#define MultiOpOr(X,Y)	(((X) & (Y) ? (Y) : 0) & (m_bMultiUserDoneOp[c] & (Y) ? (Y) :0))

void Table_xianningmajiang::MultiOperateOver(_uint8 ChairID, int iOpType, _uint8 bCard)
{
	if (iOpType == TYPE_PASS)
	{
		return;
	}

	for (_uint8 c = NextChair(m_bCurChairID); c != ChairID; c = NextChair(c))
	{
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
		UserOperateGang(ChairID, iOpType, bCard);
		if (m_conf.m_bQiangZhiGang && RobGangHu(ChairID, bCard, iOpType))
			return;
		
		OnUserGetCard(ChairID, true);
		break;
	default:
		break;
	}
}

void Table_xianningmajiang::DoSimpleOperate(_uint8 ChairID, int iOpType, _uint8 bCard)
{
	log_info("DoSimpleOperate tid:%d uid:%d chairid:%d, iOptype:0x%x bCard:%0x\n",tid, GetPlayerUid(ChairID), ChairID, iOpType, bCard);

	switch (iOpType)
	{
	case TYPE_PENG:
		UserOperatePeng(ChairID, bCard);
		break;
	case TYPE_WANGANG:
		UserOperateGang(ChairID, iOpType, bCard);
		if (m_conf.m_bQiangGangHu && RobGangHu(ChairID, bCard, iOpType))
			return;
		do
		{
			OnUserGetCard(ChairID, true);
		} while (0);
		break;
	case TYPE_ANGANG:
		UserOperateGang(ChairID, iOpType, bCard);
		if (m_conf.m_bQiangAnGang && RobGangHu(ChairID, bCard, iOpType))
			return;
		do
		{
			OnUserGetCard(ChairID, true);
		} while (0);
		break;

	case TYPE_ZHIGANG:
		UserOperateGang(ChairID, iOpType, bCard);
		if (m_conf.m_bQiangZhiGang && RobGangHu(ChairID, bCard, iOpType))
			return;
		do
		{		
			OnUserGetCard(ChairID, true);
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
		//如果自摸，则轮到他出牌
		if (m_bCurChairID == ChairID)
		{
			if (m_CardPool.Count() < 14)
			{
				OnUserGetLastFourCard(NextChair(m_bCurChairID));
			}
			//开启出牌时钟
			set_out_timer(ChairID);
		}
		else
		{
			if (m_bCurChairID == INVALID_CHAIRID)
			{
				OnUserGetCard(m_bBanker);
			}
			else
			{
				m_bCurChairID = NextChair(m_bCurChairID);
				OnUserGetCard(m_bCurChairID);
			}
		}
		break;
	case TYPE_HU:
		UserOperateHu(ChairID);

		GameEnd();
		break;
	default:
		break;
	}
}

bool Table_xianningmajiang::GetAnGang(const HandCards* pHc, MJ_opinfo& ganginfo, _uint8 bCard)
{
	ganginfo.Clear();
	if (bCard == INVALID_MJ_CARD)
	{
		return false;
	}
	else
	{
		for (int i = 0; i < pHc->FixedCardsLen; i++)
		{
			if (pHc->FixedCards[i].state == TYPE_ANGANG&&pHc->FixedCards[i].CardData == bCard)
			{
				return true;
			}
		}
		return false;
	}
}

bool Table_xianningmajiang::GetZhiGang(const HandCards* pHc, MJ_opinfo& ganginfo, _uint8 bCard)
{
	ganginfo.Clear();
	if (bCard == INVALID_MJ_CARD)
	{
		return false;
	}
	else
	{
		for (int i = 0; i < pHc->FixedCardsLen; i++)
		{
			if (pHc->FixedCards[i].state == TYPE_ZHIGANG&&pHc->FixedCards[i].CardData == bCard)
			{
				return true;
			}
		}
		return false;
	}
}

//手牌为14张牌的时候的判断听牌
int Table_xianningmajiang::GetListeningInfo_1(HandCards &HandCard, _uint8 ChairID, tagListenInfos stListenInfo[14], _uint8& bListenLen,_uint8 bCard)
{
	HandCards tempHandCard = HandCard;

	memset(stListenInfo, 0, sizeof(tagListenInfos) * 14);
	bListenLen = 0;

	Check_Listening_QuanQiuRen(&tempHandCard, ChairID, bCard, stListenInfo, bListenLen);
	

	return (bListenLen != 0 ? TYPE_LISTEN : TYPE_NULL);
}


bool Table_xianningmajiang::GetWanGang(const HandCards* pHc, MJ_opinfo& ganginfo, _uint8 bCard)
{

	for (int i = 0; i < pHc->FixedCardsLen; i++)
	{
		for (int i = 0; i < pHc->FixedCardsLen; i++)
		{
			if (pHc->FixedCards[i].state == TYPE_WANGANG&&pHc->FixedCards[i].CardData == bCard)
			{
				return true;
			}
		}
	}
	
	return false;
}

void Table_xianningmajiang::OnUserOutCard_XianNing(_uint8 bOutChairID, _uint8 bOutCard)
{
	m_bUserOutHongZhongLaiZiCardFlag[bOutChairID] = true;
	OnUserOutCard_1(bOutChairID, bOutCard);
}


void Table_xianningmajiang::Proto_Game_UserOutCard(_uint8 ChairID, _uint8 bCard, proto::game::AckUserOutCard& stUseroutcard)
{
	stUseroutcard.Clear();
	stUseroutcard.set_chairid(ChairID);
	stUseroutcard.set_card(bCard);
	for (int i = 0; i < m_GAME_PLAYER; i++)
	{
		proto::game::tagShowOutCard *pstShow = stUseroutcard.add_showcard();
		for (int j = 0; j < m_stUserOutCard[i].Size(); j++)
		{
			pstShow->add_outcard(m_stUserOutCard[i][j]);
		}
		pstShow->set_outcardlen(m_stUserOutCard[i].Size());
	}
	if (m_bUserOutHongZhongLaiZiCardFlag[ChairID]==true)
	{
		stUseroutcard.set_xianninglaizi(true);
	}

}

void Table_xianningmajiang::UserOperateHu(_uint8 ChairID, _uint8 bHuCard)
{
	log_info("tid:%d %s uid:%d ChairID:%d hu FanNum:%d \n",tid, __FUNCTION__, GetPlayerUid(ChairID), ChairID, m_HuFanNums[ChairID]);
	_uint8 bCard = 0xFF;
	if (bHuCard == 0xFF)
	{
		bCard = m_bCurCard;
	}
	else
	{
		bCard = bHuCard;
	}

//	BroadcastOperateResult(ChairID, TYPE_HU, bCard);

	if (!(m_bUserOperate[ChairID] & TYPE_HU))
	{
		log_error("tid:%d hu error, hu chairid:%d, m_bUserOperate[%d]:%d \n",tid, ChairID, ChairID, m_bUserOperate[ChairID]);
		//return ;
	}

	m_bIsHu[ChairID] = true;
	int userscore[GAME_PLAYER] = { 0 };
	Calc_Hu(ChairID, bCard, userscore);

	if (m_oFan[ChairID].Find(MJ_FAN_TYPE_GANGSHANGHUA))
	{
		proto::game::AckBcOpResult stOpresult;
		stOpresult.set_chairid(ChairID);
		stOpresult.set_outcardchairid(m_bCurChairID);
		stOpresult.set_optype(TYPE_HU);
		stOpresult.set_card(bCard);
		stOpresult.set_optype2(1);
		SetRobGangHu_Flag(ChairID, TYPE_HU, stOpresult);

		broadcast(SERVER_OPERATE_RESULT, stOpresult);
	}
	else
	{
		BroadcastOperateResult(ChairID, TYPE_HU, bCard);
	}

	Calc_Score(m_stHuScore, userscore);

	for (int i = 0; i < GAME_PLAYER; i++)
	{
		log_debug("tid:%d userscore[i] = %d m_conf_xianningmj.mj_conf_xianningmajiang = %d ",tid, userscore[i], m_conf_xianningmj.mj_conf_xianningmajiang);
	}
	/*userscore[0] = 1;
	userscore[1] = 1;
	userscore[2] = 1;
	userscore[3] = 1;*/
	//如果没有保存胡牌信息，重新获取

	proto::game::AckUserHu stHu;
	stHu.set_huchairid(ChairID);
	if (m_bCurChairID == ChairID)
	{
		stHu.set_pashuchairid(ChairID);
	}
	else
	{
		stHu.set_pashuchairid(m_bCurChairID);
	}
	for (int i = 0; i < m_GAME_PLAYER; i++)
	{
		stHu.add_score(userscore[i]);
	}

	broadcast(SERVER_USER_HU, stHu);

	m_bUserOperate[ChairID] = TYPE_NULL;

	int nscore[GAME_PLAYER] = { 0 };
	save_video_operate(TYPE_HU, ChairID, stHu.pashuchairid(), bCard, nscore);
	save_video_hu(ChairID, stHu.pashuchairid(), userscore);


}

void Table_xianningmajiang::SendTingInfo(_uint8 ChairID)
{
	tagListenInfos stListenInfo[14];
	_uint8 bListenLen = 0;
	//胡牌不用判断听牌，自动胡牌不用判断听牌
	if (GetListeningInfo(ChairID, stListenInfo, bListenLen))
	{
		m_bUserOperate[ChairID] |= TYPE_LISTEN;
	}

	// 发送听牌协议
	proto::game::AckListenNotify stListen;
	if (m_bUserOperate[ChairID] & TYPE_LISTEN)
	{
		log_info("%s tid:%d uid:%d chairid:%d ListenLen:%d \n", __FUNCTION__,tid, GetPlayerUid(ChairID), ChairID, bListenLen);
		stListen.set_optype(TYPE_LISTEN);
		//打包听牌信息
		for (int j = 0; j < bListenLen; j++)
		{
			PackageTingInfo(ChairID, stListenInfo, j, stListen.add_listeninfo());
		}
	}
	else
	{
		stListen.set_optype(TYPE_NULL);
		log_info("%s uid:%d chairid:%d no listen\n", __FUNCTION__, GetPlayerUid(ChairID), ChairID);
	}
	unicast(SERVER_LISTEN_NOTIFY, ChairID, stListen);

}
int Table_xianningmajiang::GetListeningInfo(_uint8 ChairID, tagListenInfos stListenInfo[14], _uint8& bListenLen)
{
	HandCards tempHandCard = m_HandCard[ChairID];

	_uint8 bCard = 0xFF;
	memset(stListenInfo, 0, sizeof(tagListenInfos) * 14);
	bListenLen = 0;

	for (int i = 0; i < tempHandCard.ChangeableCardsLen; i++)
	{
		//防止手牌有有对、刻字时，连续对相同的牌做多次的判断胡牌
		if (bCard == tempHandCard.ChangeableCards[i])
		{
			continue;
		}
		bCard = tempHandCard.ChangeableCards[i];
		Check_Listening(&tempHandCard, ChairID, bCard, stListenInfo, bListenLen);
	}

	return (bListenLen != 0 ? TYPE_LISTEN : TYPE_NULL);
}


void Table_xianningmajiang::Check_Listening(HandCards *pHc, _uint8 ChairID, _uint8 bCard, tagListenInfos stListenInfo[14], _uint8 &bListenLen)
{
	if (bListenLen >= 14)
	{
		log_error("%s chaochu zuida ting pai shu!\n", __FUNCTION__);
		return;
	}

	HandCards tempHandCard = *pHc;

	m_GameLogic.ChuPai(&tempHandCard, bCard);
	bool bTrue = false;

	MJ_BUFFER<_uint8, POOL_CARD_TYPE_MAX> _relativity;
	GetAllHandCardRelativity(&tempHandCard, _relativity);

	for (_uint8 i = 0; i < _relativity.Size(); i++)
	{
		MJ_major major;
		int fan_num = 0;
		if (OutCardEstimate_Ting(ChairID, _relativity[i], major, fan_num, tempHandCard) == TYPE_HU)
		{
			if (_relativity[i] == 0x41)
			{
				continue;
			}
			if (bCard == m_bXianNingGuiPai)
			{
				continue;
			}
			if (stListenInfo[bListenLen].bHuLen < 18)
			{
				stListenInfo[bListenLen].bOutCard = bCard;//要打出的牌       
				stListenInfo[bListenLen].HuInfo[stListenInfo[bListenLen].bHuLen].bCard = _relativity[i];//听的牌
				stListenInfo[bListenLen].HuInfo[stListenInfo[bListenLen].bHuLen].bFanShu = Calc_Fan(ChairID, major.m_oFan);
				bTrue = true;
				stListenInfo[bListenLen].bHuLen++;


			}
		}
		else if (TYPE_HU == GetCardEstimate_Ting(ChairID, _relativity[i], major, fan_num, tempHandCard))
		{
			if (_relativity[i] == 0x41)
			{
				continue;
			}
			if (bCard == m_bXianNingGuiPai)
			{
				continue;
			}
			if (stListenInfo[bListenLen].bHuLen < 18)
			{
				stListenInfo[bListenLen].bOutCard = bCard;//要打出的牌       
				stListenInfo[bListenLen].HuInfo[stListenInfo[bListenLen].bHuLen].bCard = _relativity[i];//听的牌
				stListenInfo[bListenLen].HuInfo[stListenInfo[bListenLen].bHuLen].bFanShu = Calc_Fan(ChairID, major.m_oFan);
				bTrue = true;
				stListenInfo[bListenLen].bHuLen++;

			}
		}
	}
	m_GameLogic.AddCard(&tempHandCard, bCard);
	if (bTrue)
	{
		if (bListenLen < 14)
		{
			bListenLen++;
		}
	}
}


int Table_xianningmajiang::OutCardEstimate_Ting(_uint8 ChairID, _uint8 bCard, MJ_major& major, int& fan_num, HandCards pHcTing)
{
	if (m_conf_xianningmj.mj_conf_xianningmajiang == true)
	{
		return OutCardEstimate_0_Ting(ChairID, bCard, major, fan_num,pHcTing);
	}
	else
	{
		return OutCardEstimate_zhuanzhuan_Ting(ChairID, bCard, major, fan_num, pHcTing);

	}
}
int Table_xianningmajiang::OutCardEstimate_0_Ting(_uint8 ChairID, _uint8 bCard, MJ_major& major, int& fan_num, HandCards pHcTing)
{
	HandCards* pHc = &pHcTing;

	int iOpType = TYPE_NULL;
	if (!m_conf.m_bZimoHu)
	{
		m_GameLogic.AddCard(pHc, bCard);
		iOpType |= Check_Hu(ChairID, *pHc, major.m_oFan);
	//	log_debug("ChairID = %d  iOpType = %#x", ChairID, iOpType);
		if (iOpType & TYPE_HU)
		{
			if (major.m_oFan.Find(MJ_FAN_TYPE_PINGHU) && IsQuanqiuren(m_HandCard[ChairID]))
			{
				major.m_oFan.Add(MJ_FAN_TYPE_QUANQIUREN);
				major.m_oFan.Remove(MJ_FAN_TYPE_PINGHU);

			}
			else if (IsQuanqiuren(m_HandCard[ChairID]))
			{
				for (int i = 0; i < m_HandCard[ChairID].ChangeableCardsLen; i++)
				{
					if (CardNum(bCard) == 2 || CardNum(bCard) == 5 || CardNum(bCard) == 8)
					{
						major.m_oFan.Add(MJ_FAN_TYPE_QUANQIUREN);
					}
				}
			}
			int guozhanghufan = 0;
			if (major.m_oFan.Find(MJ_FAN_TYPE_PINGHU))
			{ //三番起胡
				int hu_fan_num = Calc_Fan(ChairID, major.m_oFan);
				int gang_fan_num = Calc_GangFan(m_bOutCardChairID);
				//gang_fan_num += 1;//点炮玩家1番（大胡加1番，平胡不加）
				for (int i = 0; i < m_HandCard[m_bOutCardChairID].FixedCardsLen; i++)
				{
					if (m_HandCard[m_bOutCardChairID].FixedCards[i].state != TYPE_ANGANG)
					{
						gang_fan_num += 1;

						break;
					}
				}
				if (hu_fan_num + gang_fan_num < 3)
				{
					iOpType &= ~(TYPE_HU);
				}
				guozhanghufan = hu_fan_num + gang_fan_num;
			}
			else
			{
				int hu_fan_num = Calc_Fan(ChairID, major.m_oFan);
				int gang_fan_num = 0;
				for (int c = NextChair(ChairID); c != ChairID; c = NextChair(c))
				{
					gang_fan_num = Calc_GangFan(c);
					for (int i = 0; i < m_HandCard[c].FixedCardsLen; i++)
					{//开口加一蕃
						if (m_HandCard[c].FixedCards[i].state != TYPE_ANGANG)
						{
							gang_fan_num += 1;
							break;
						}
					}
					if (c == m_bOutCardChairID)
					{
						gang_fan_num += 1;
					}
					if (hu_fan_num + gang_fan_num >= 3)
					{
						iOpType |= TYPE_HU;
						if (guozhanghufan <= hu_fan_num + gang_fan_num)
						{
							guozhanghufan = hu_fan_num + gang_fan_num;
						}
					}
					else
					{
						iOpType &= ~(TYPE_HU);
					}
					if (guozhanghufan >= 3)
					{

						iOpType |= TYPE_HU;
					}
				}
			}
			int bUserHasGuiPaiNum = 0;
			for (int i = 0; i < pHc->ChangeableCardsLen; i++)
			{
				if (pHc->ChangeableCards[i] == m_bXianNingGuiPai)
				{
					bUserHasGuiPaiNum++;
				}
			}
			if (major.m_oFan.Find(MJ_FAN_TYPE_PINGHU) && bUserHasGuiPaiNum > 1)
			{
				iOpType &= ~(TYPE_HU);
			}

		}
	}

	return iOpType;
}

int Table_xianningmajiang::OutCardEstimate_zhuanzhuan_Ting(_uint8 ChairID, _uint8 bCard, MJ_major& major, int& fan_num,HandCards pHcTing)
{
	HandCards* pHc = &pHcTing;
	int iOpType = TYPE_NULL;
	if(!m_conf.m_bZimoHu)
	{
		m_GameLogic.AddCard(pHc, bCard);
		iOpType |= Check_Hu(ChairID, *pHc, major.m_oFan);
		if (m_bUserOperateGuoZhangHuFlag[ChairID] == true)
		{//过张胡
			iOpType &= ~TYPE_HU;
		}
		m_GameLogic.ChuPai(pHc, bCard);
	}

	return iOpType;
}



int Table_xianningmajiang::GetCardEstimate_Ting(_uint8 ChairID, _uint8 bCard, MJ_major& major, int& fan_num, HandCards pHcTing)
{
	major.m_oFan.Clear();
	if (m_conf_xianningmj.mj_conf_xianningmajiang == true)
	{
		return GetCardEstimate_0_Ting(ChairID, bCard, major, fan_num, pHcTing);
	}
	else
	{
		return GetCardEstimate_zhuanzhuan_Ting(ChairID, bCard, major, fan_num, pHcTing);
	}
}
/*
玩家自摸一张牌，此时判断是否可以听杠胡。
*/
int Table_xianningmajiang::GetCardEstimate_zhuanzhuan_Ting(_uint8 ChairID, _uint8 bCard, MJ_major& major, int& fan_num, HandCards pHcTing)
{
	//	assert(major.m_oFan.Size() == 0);

	int iOpType = TYPE_NULL;

	HandCards* pHc = &pHcTing;
	m_GameLogic.AddCard(pHc, bCard);

	if (pHc->ChangeableCardsLen % 3 != 2)
	{
		log_error("%s fatal paishu error uid:%d chairid:%d!\n", __FUNCTION__, GetPlayerUid(ChairID), ChairID);
		m_GameLogic.ChuPai(pHc, bCard);
		return TYPE_NULL;
	}
	//	assert(pHc->ChangeableCardsLen <= HAND_CARD_SIZE_MAX);

	/*获取杠信息*/
	iOpType |= GetGangInfo(ChairID, major.m_GetGangInfo);
	/*检查能不能胡*/
	iOpType |= Check_Hu(ChairID, *pHc, major.m_oFan);
	if (iOpType & TYPE_HU)
	{
		if (m_bAddFanFlag[ChairID])   //杠上花
		{
			//UserGangShangHua(ChairID, bCard, major.m_oFan);
		}
		/*计算番*/
		fan_num = Calc_Fan(ChairID, major.m_oFan);
	}

	m_GameLogic.ChuPai(pHc, bCard);

	return iOpType;
}

int Table_xianningmajiang::GetCardEstimate_0_Ting(_uint8 ChairID, _uint8 bCard, MJ_major& major, int& fan_num, HandCards pHcTing)
{
	int iOpType = TYPE_NULL;

	HandCards* pHc = &pHcTing;
	m_GameLogic.AddCard(pHc, bCard);

	if (pHc->ChangeableCardsLen % 3 != 2)
	{
		log_error("tid:%d %s fatal paishu error uid:%d chairid:%d!\n", tid, __FUNCTION__, GetPlayerUid(ChairID), ChairID);
		m_GameLogic.ChuPai(pHc, bCard);
		return TYPE_NULL;
	}
	//assert(pHc->ChangeableCardsLen <= HAND_CARD_SIZE_MAX);
	/*检查能不能胡*/
	iOpType |= Check_Hu(ChairID, *pHc, major.m_oFan);

	if (iOpType & TYPE_HU)
	{
		if (m_CardPool.Count() < 15)
		{
			/*最后一张胡牌则为海底捞*/
			UserGetLastCard(ChairID, iOpType, major.m_oFan);
		}

		if (m_bAddFanFlag[ChairID] || m_bUserOperateLzHzGangFlag[ChairID] == true)   //杠上花
		{
			if (major.m_oFan.Find(MJ_FAN_TYPE_HAIDILAOYUE))
			{
				major.m_oFan.Remove(MJ_FAN_TYPE_HAIDILAOYUE);
			}
			UserGangShangHua(ChairID, bCard, major.m_oFan);

		}
		/*计算番*/
		fan_num = Calc_Fan(ChairID, major.m_oFan);
		if (major.m_oFan.Find(MJ_FAN_TYPE_GANGSHANGHUA) && (!major.m_oFan.Find(MJ_FAN_TYPE_PINGHU)))
		{
			fan_num -= 1;//杠上花1番（平胡才加，大胡不加）
		}
		if (!major.m_oFan.Find(MJ_FAN_TYPE_PINGHU))
		{
			fan_num += 1;//自摸胡牌加一蕃
		}
		int gang_fan_num = 0;
		for (int c = NextChair(ChairID); c != ChairID; c = NextChair(c))
		{
			gang_fan_num = Calc_GangFan(c);
			for (int i = 0; i < m_HandCard[c].FixedCardsLen; i++)
			{
				if (m_HandCard[c].FixedCards[i].state != TYPE_ANGANG)
				{
					gang_fan_num += 1;
					break;
				}
			}
			if (fan_num + gang_fan_num >= 3)
			{
				iOpType |= TYPE_HU;
				break;
			}
			else
			{
				iOpType &= ~(TYPE_HU);
			}
		}
	}
	if (iOpType & TYPE_HU)
	{
		int bUserHasGuiPaiNum = 0;
		for (int i = 0; i < pHc->ChangeableCardsLen; i++)
		{
			if (pHc->ChangeableCards[i] == m_bXianNingGuiPai)
			{
				bUserHasGuiPaiNum++;
			}
		}

		if (major.m_oFan.Find(MJ_FAN_TYPE_PINGHU) && bUserHasGuiPaiNum>1)
		{
			iOpType &= ~(TYPE_HU);
		}
	}
	m_GameLogic.ChuPai(pHc, bCard);
	return iOpType;
}


void Table_xianningmajiang::Calc_BaseScore_GuoZhangHu(_uint8 ChairID, int iBaseScore[GAME_PLAYER], MJ_fan_buffer mHuoFan)
{

	int GuiNum = 0;
	for (int i = 0; i < m_HandCard[ChairID].ChangeableCardsLen; i++)
	{
		if (m_HandCard[ChairID].ChangeableCards[i] == m_bXianNingGuiPai)
		{
			GuiNum++;
		}
	}
	for (int i = 0; i < m_HandCard[ChairID].FixedCardsLen; i++)
	{
		if (m_HandCard[ChairID].FixedCards[i].state == TYPE_CHI)
		{
			for (int c = 0; c < 3; c++)
			{
				if (m_HandCard[ChairID].FixedCards[i].c[c] == m_bXianNingGuiPai)
				{
					GuiNum++;
				}
			}

		}
		else if (m_HandCard[ChairID].FixedCards[i].CardData == m_bXianNingGuiPai)
		{
			GuiNum++;
		}
	}
	for (int i = 0; i < m_stUserOutCard[ChairID].Size(); i++)
	{
		if (m_bXianNingGuiPai == m_stUserOutCard[ChairID][i])
		{
			GuiNum++;
		}
	}

	if (GuiNum == 4)
	{
		mHuoFan.Add(MJ_FAN_XIANNING_SIGUIJIAYIFAN);
	}
	int BaseHuFanNums = Calc_Fan(ChairID, mHuoFan);
	int IsKaiKou[GAME_PLAYER] = { 0 };
	for (int i = 0; i < m_GAME_PLAYER; i++)
	{
		if (m_HandCard[i].FixedCardsLen > 0)
		{
			for (int j = 0; j < m_HandCard[i].FixedCardsLen; j++)
			{
				if (m_HandCard[i].FixedCards[j].state != TYPE_ANGANG)
				{
					IsKaiKou[i] = 1;
					break;
				}
			}
		}
	}

	if (m_bRobHu)
	{//抢杠胡
		if (mHuoFan.Find(MJ_FAN_XIANNING_HEIHU))
		{
			iBaseScore[m_WangGangPlayer] -= 50 * m_BaseScore;
			iBaseScore[ChairID] += -iBaseScore[m_WangGangPlayer];
		}
		else
		{
			iBaseScore[m_WangGangPlayer] -= 40 * m_BaseScore;
			iBaseScore[ChairID] += -iBaseScore[m_WangGangPlayer];
		}

	}
	else if (ChairID == m_bCurChairID)
	{//自摸
		int HuFanNums = BaseHuFanNums;
		if (mHuoFan.Find(MJ_FAN_TYPE_GANGSHANGHUA) && (!mHuoFan.Find(MJ_FAN_TYPE_PINGHU)))
		{
			HuFanNums -= 1;//平胡杠上花加一番
		}

		if (!mHuoFan.Find(MJ_FAN_TYPE_PINGHU))
		{
			HuFanNums += 1;//自摸胡牌加一蕃
		}

		for (_uint8 c = NextChair(ChairID); c != ChairID; c = NextChair(c))
		{
			int HuGangFanNums_C = Calc_GangFan(c);
			int TotalHuFanNums = 1 << (HuFanNums + HuGangFanNums_C + IsKaiKou[c]);
			iBaseScore[c] -= (TotalHuFanNums * m_BaseScore);
			iBaseScore[ChairID] += -iBaseScore[c];

		}
	}
	else
	{//放炮
		int HuFanNums = BaseHuFanNums;
		if (mHuoFan.Find(MJ_FAN_TYPE_PINGHU))
		{
			int HuGangFanNums_C = Calc_GangFan(m_bOutCardChairID);
			int TotalHuFanNums = 1 << (HuFanNums + HuGangFanNums_C + IsKaiKou[m_bOutCardChairID]);
			iBaseScore[m_bOutCardChairID] -= (TotalHuFanNums * m_BaseScore);
			iBaseScore[ChairID] += -iBaseScore[m_bOutCardChairID];

		}
		else
		{
			int HuFanNums = BaseHuFanNums;
			for (_uint8 c = NextChair(ChairID); c != ChairID; c = NextChair(c))
			{
				int HuGangFanNums_C = Calc_GangFan(c);
				if (m_bOutCardChairID == c)
				{
					HuGangFanNums_C += 1;//点炮加一番
				}
				int TotalHuFanNums = 1 << (HuFanNums + HuGangFanNums_C + IsKaiKou[c]);
				iBaseScore[c] -= (TotalHuFanNums * m_BaseScore);
				iBaseScore[ChairID] += -iBaseScore[c];
			}
		}
	}

	//封顶
	int fengdingnum = 0;
	int kaikounum = 0;
	for (_uint8 c = NextChair(ChairID); c != ChairID; c = NextChair(c))
	{
		if (-iBaseScore[c] >= m_BaseScore * 30)
		{
			fengdingnum++;
		}
	}
	for (_uint8 c = NextChair(ChairID); c != ChairID; c = NextChair(c))
	{
		if (m_HandCard[c].FixedCardsLen > 0)
		{
			for (int j = 0; j < m_HandCard[c].FixedCardsLen; j++)
			{
				if (m_HandCard[c].FixedCards[j].state != TYPE_ANGANG)
				{
					kaikounum++;
					break;
				}
			}
		}
	}
	if (fengdingnum == (m_GAME_PLAYER - 1) && m_bRobHu == false)
	{
		if (kaikounum == 0)
		{
			for (_uint8 c = NextChair(ChairID); c != ChairID; c = NextChair(c))
			{
				iBaseScore[c] = -m_BaseScore * 60;
			}
		}
		else
		{
			for (_uint8 c = NextChair(ChairID); c != ChairID; c = NextChair(c))
			{
				if (IsKaiKou[c])
				{
					iBaseScore[c] = -m_BaseScore * 40;
				}
				else
				{
					iBaseScore[c] = -m_BaseScore * 50;
				}
			}
		}
	}
	else if (m_bRobHu == false)
	{
		for (_uint8 c = NextChair(ChairID); c != ChairID; c = NextChair(c))
		{
			if (-iBaseScore[c] >= m_BaseScore * 30)
			{
				iBaseScore[c] = -m_BaseScore * 30;
			}
		}

	}

	int iAnGangNum = 0;
	for (int i = 0; i < m_HandCard[ChairID].FixedCardsLen; i++)
	{
		if (m_HandCard[ChairID].FixedCards[i].state == TYPE_ANGANG)
		{
			iAnGangNum++;
		}

	}
	if (mHuoFan.Find(MJ_FAN_TYPE_QINGYISE) && m_HandCard[ChairID].FixedCardsLen - iAnGangNum >= 3)
	{//5.包赔如果清一色胡牌时有3 / 4句吃碰杠，放出第3句的玩家包赔3家分数
		int ChairIdOutFixCard = 0;
		for (int i = 0, indexnum = 0; i < m_HandCard[ChairID].FixedCardsLen; i++)
		{
			if (m_HandCard[ChairID].FixedCards[i].state == TYPE_ANGANG)
			{
				continue;
			}
			if (indexnum == 2)
			{
				ChairIdOutFixCard = m_HandCard[ChairID].FixedCards[i].chairID;
			}
			indexnum++;
		}
		int TotalScore = 0;

		for (_uint8 c = NextChair(ChairID); c != ChairID; c = NextChair(c))
		{
			TotalScore += iBaseScore[c];
		}
		for (_uint8 c = NextChair(ChairID); c != ChairID; c = NextChair(c))
		{
			if (ChairIdOutFixCard == c)
			{
				iBaseScore[c] = TotalScore;
			}
			else
			{
				iBaseScore[c] = 0;

			}
		}
	}
	else if (mHuoFan.Find(MJ_FAN_XIANNING_258JIANGYISE) && m_HandCard[ChairID].FixedCardsLen - iAnGangNum >= 3 && !mHuoFan.Find(MJ_FAN_TYPE_QINGYISE))
	{//5.包赔如果清一色胡牌时有3 / 4句吃碰杠，放出第3句的玩家包赔3家分数

		int ChairIdOutFixCard = 0;
		for (int i = 0, indexnum = 0; i < m_HandCard[ChairID].FixedCardsLen; i++)
		{
			if (m_HandCard[ChairID].FixedCards[i].state == TYPE_ANGANG)
			{
				continue;
			}
			if (indexnum == 2)
			{
				ChairIdOutFixCard = m_HandCard[ChairID].FixedCards[i].chairID;
			}
			indexnum++;
		}

		int TotalScore = 0;

		for (_uint8 c = NextChair(ChairID); c != ChairID; c = NextChair(c))
		{
			TotalScore += iBaseScore[c];
		}
		for (_uint8 c = NextChair(ChairID); c != ChairID; c = NextChair(c))
		{
			if (ChairIdOutFixCard == c)
			{
				iBaseScore[c] = TotalScore;
			}
			else
			{
				iBaseScore[c] = 0;

			}

		}
	}
	else if (mHuoFan.Find(MJ_FAN_TYPE_QUANQIUREN) && ChairID != m_bCurChairID)
	{//全球人封顶
		tagListenInfos stListenInfo[14];
		_uint8 bListenLen = 0;
		//胡牌不用判断听牌，自动胡牌不用判断听牌
		int TotalScore = 0;
		int ChairIdOut = 0;
		if (m_bRobHu)
		{//抢杠胡
			ChairIdOut = m_WangGangPlayer;
		}
		else
		{
			ChairIdOut = m_bOutCardChairID;
		}

		HandCards tempHandCard = m_HandCard[ChairIdOut];
		_uint8 bGetCard = 0xFF;
		if (tempHandCard.ChangeableCardsLen != 0 && tempHandCard.ChangeableCardsLen % 3 != 2)
		{

			if (m_bXianNingGuiPai == 0x01)
			{
				bGetCard = 0x02;
				m_GameLogic.AddCard(&tempHandCard, bGetCard);
			}
			else
			{
				bGetCard = 0x01;
				m_GameLogic.AddCard(&tempHandCard, bGetCard);
			}

		}
		if (GetListeningInfo_1(tempHandCard, ChairID, stListenInfo, bListenLen, bGetCard) != TYPE_LISTEN)
		{
			for (_uint8 c = NextChair(ChairID); c != ChairID; c = NextChair(c))
			{
				TotalScore += iBaseScore[c];
			}
			for (_uint8 c = NextChair(ChairID); c != ChairID; c = NextChair(c))
			{
				if (ChairIdOut == c)
				{
					iBaseScore[c] = TotalScore;
				}
				else
				{
					iBaseScore[c] = 0;
				}
			}
		}
	}
}

void Table_xianningmajiang::Check_Listening_QuanQiuRen(HandCards *pHc, _uint8 ChairID, _uint8 bCard, tagListenInfos stListenInfo[14], _uint8 &bListenLen)
{
	if (bListenLen >= 14)
	{
		log_error("%s chaochu zuida ting pai shu!\n", __FUNCTION__);
		return;
	}

	HandCards tempHandCard = *pHc;

	m_GameLogic.ChuPai(&tempHandCard, bCard);
	bool bTrue = false;

	MJ_BUFFER<_uint8, POOL_CARD_TYPE_MAX> _relativity;
	GetAllHandCardRelativity(&tempHandCard, _relativity);

	for (_uint8 i = 0; i < _relativity.Size(); i++)
	{
		MJ_major major;
		int fan_num = 0;
		if (OutCardEstimate_QuanQiuRen_Ting(ChairID, _relativity[i], major, fan_num, tempHandCard) == TYPE_HU)
		{
			if (_relativity[i] == 0x41)
			{
				continue;
			}
			if (bCard == m_bXianNingGuiPai)
			{
				continue;
			}
			if (stListenInfo[bListenLen].bHuLen < 18)
			{
				stListenInfo[bListenLen].bOutCard = bCard;//要打出的牌       
				stListenInfo[bListenLen].HuInfo[stListenInfo[bListenLen].bHuLen].bCard = _relativity[i];//听的牌
				stListenInfo[bListenLen].HuInfo[stListenInfo[bListenLen].bHuLen].bFanShu = Calc_Fan(ChairID, major.m_oFan);
				bTrue = true;
				stListenInfo[bListenLen].bHuLen++;


			}
		}
		else if (TYPE_HU == GetCardEstimate_QuanQiuRen_Ting(ChairID, _relativity[i], major, fan_num, tempHandCard))
		{
			if (_relativity[i] == 0x41)
			{
				continue;
			}
			if (bCard == m_bXianNingGuiPai)
			{
				continue;
			}
			if (stListenInfo[bListenLen].bHuLen < 18)
			{
				stListenInfo[bListenLen].bOutCard = bCard;//要打出的牌       
				stListenInfo[bListenLen].HuInfo[stListenInfo[bListenLen].bHuLen].bCard = _relativity[i];//听的牌
				stListenInfo[bListenLen].HuInfo[stListenInfo[bListenLen].bHuLen].bFanShu = Calc_Fan(ChairID, major.m_oFan);
				bTrue = true;
				stListenInfo[bListenLen].bHuLen++;

			}
		}
	}
	m_GameLogic.AddCard(&tempHandCard, bCard);
	if (bTrue)
	{
		if (bListenLen < 14)
		{
			bListenLen++;
		}
	}
}

int Table_xianningmajiang::OutCardEstimate_QuanQiuRen_Ting(_uint8 ChairID, _uint8 bCard, MJ_major& major, int& fan_num, HandCards pHcTing)
{
	HandCards* pHc = &pHcTing;

	int iOpType = TYPE_NULL;
	if (!m_conf.m_bZimoHu)
	{
		m_GameLogic.AddCard(pHc, bCard);
		iOpType |= Check_Hu(ChairID, *pHc, major.m_oFan);
		//	log_debug("ChairID = %d  iOpType = %#x", ChairID, iOpType);
		if (iOpType & TYPE_HU)
		{
			if (major.m_oFan.Find(MJ_FAN_TYPE_PINGHU) && IsQuanqiuren(m_HandCard[ChairID]))
			{
				major.m_oFan.Add(MJ_FAN_TYPE_QUANQIUREN);
				major.m_oFan.Remove(MJ_FAN_TYPE_PINGHU);

			}
			else if (IsQuanqiuren(m_HandCard[ChairID]))
			{
				for (int i = 0; i < m_HandCard[ChairID].ChangeableCardsLen; i++)
				{
					if (CardNum(bCard) == 2 || CardNum(bCard) == 5 || CardNum(bCard) == 8)
					{
						major.m_oFan.Add(MJ_FAN_TYPE_QUANQIUREN);
					}
				}
			}
			int bUserHasGuiPaiNum = 0;
			for (int i = 0; i < pHc->ChangeableCardsLen; i++)
			{
				if (pHc->ChangeableCards[i] == m_bXianNingGuiPai)
				{
					bUserHasGuiPaiNum++;
				}
			}
			if (major.m_oFan.Find(MJ_FAN_TYPE_PINGHU) && bUserHasGuiPaiNum > 1)
			{
				iOpType &= ~(TYPE_HU);
			}

		}
	}

	return iOpType;
}

int Table_xianningmajiang::GetCardEstimate_QuanQiuRen_Ting(_uint8 ChairID, _uint8 bCard, MJ_major& major, int& fan_num, HandCards pHcTing)
{
	int iOpType = TYPE_NULL;

	HandCards* pHc = &pHcTing;
	m_GameLogic.AddCard(pHc, bCard);

	if (pHc->ChangeableCardsLen % 3 != 2)
	{
		log_error("tid:%d %s fatal paishu error uid:%d chairid:%d!\n", tid, __FUNCTION__, GetPlayerUid(ChairID), ChairID);
		m_GameLogic.ChuPai(pHc, bCard);
		return TYPE_NULL;
	}
	//assert(pHc->ChangeableCardsLen <= HAND_CARD_SIZE_MAX);
	/*检查能不能胡*/
	iOpType |= Check_Hu(ChairID, *pHc, major.m_oFan);

	if (iOpType & TYPE_HU)
	{
		int bUserHasGuiPaiNum = 0;
		for (int i = 0; i < pHc->ChangeableCardsLen; i++)
		{
			if (pHc->ChangeableCards[i] == m_bXianNingGuiPai)
			{
				bUserHasGuiPaiNum++;
			}
		}

		if (major.m_oFan.Find(MJ_FAN_TYPE_PINGHU) && bUserHasGuiPaiNum>1)
		{
			iOpType &= ~(TYPE_HU);
		}
	}
	m_GameLogic.ChuPai(pHc, bCard);
	return iOpType;
}


void Table_xianningmajiang::Calc_AnGangScore(_uint8 ChairID)
{
	if (m_OnUserQiangAnCalcScoreFlag[ChairID]==true)
	{
		return;
	}
	m_OnUserQiangAnCalcScoreFlag[ChairID] = true;
	int iBaseScore[GAME_PLAYER] = { 0 };
	HandCards tmp = m_HandCard[ChairID];
	m_GameLogic.AddCard(&tmp, m_OnUserQiangAnGangCard[ChairID]);
	MJ_fan_buffer oFan;
	int baseScore = 0;
	if (Check_Hu_0(ChairID, tmp, oFan))
	{

		if (oFan.Find(MJ_FAN_XIANNING_HEIHU))
		{
			baseScore = 50;
		}
		else
		{
			baseScore = 40;
		}
	}
	
	iBaseScore[ChairID] -= baseScore * m_BaseScore;
	iBaseScore[m_WangGangPlayer] += baseScore * m_BaseScore;
	//log_info("11111111111111111 iBaseScore[ChairID = %d]  = %d", ChairID, iBaseScore[ChairID]);
	log_info("11111111111111111111111 iBaseScore[m_WangGangPlayer = %d]  = %d", m_WangGangPlayer, iBaseScore[m_WangGangPlayer]);
	proto::game::xianningmj::AckBcOpResultAnGangScore stOpresult;

	for (int i = 0; i < m_GAME_PLAYER; i++)
	{

		stOpresult.add_score(iBaseScore[i]);
		
	}
	log_info("Calc_AnGangScore tid:%d ChairID:%d m_WangGangPlayer:%d stOpresult = %s", tid, ChairID, m_WangGangPlayer, stOpresult.DebugString().c_str())
	broadcast(SERVER_OPERATE_RESULT_ANGANG, stOpresult);

	Update_Score(iBaseScore);
}

bool Table_xianningmajiang::Check_Listening_AnGang(HandCards *pHc, _uint8 ChairID,ShowCards& bufCard)
{
	bufCard.Clear();
	HandCards tempHandCard = *pHc;

	MJ_BUFFER<_uint8, POOL_CARD_TYPE_MAX> _relativity;
	GetAllHandCardRelativity(&tempHandCard, _relativity);

	for (_uint8 i = 0; i < _relativity.Size(); i++)
	{
		if (_relativity[i] == m_bXianNingGuiPai)
		{
			continue;
		}
		int HuFanNums[GAME_PLAYER] = {0};                     //胡牌番数	
		MJ_major major;
		//log_info("ChairID = %d  _relativity[i] = %x", ChairID, _relativity[i]);
		if (OutCardEstimate_QiangGangHu_AnGang(ChairID, _relativity[i], major, HuFanNums[i]) & TYPE_HU)
		{
			log_info("tid:%d ChairID = %d  _relativity[i = %d] = %x",tid, ChairID,i, _relativity[i]);
			bufCard.Add(_relativity[i]);
			
		}
	}
	if (bufCard.Size())
	{
		return true;
	}
	return false;
}

bool Table_xianningmajiang::CheckUserOperator(_uint8 ChairID, int iOpType, _uint8 bCard)
{
	if (m_RobAnGangHuNum > 0)
	{
		return true;
	}
	if (ChairID >= m_GAME_PLAYER)
	{
		log_error("%s error chairid:%d iOpType:0x%x\n", __FUNCTION__, ChairID, iOpType);
		return false;
	}
	if ((iOpType != TYPE_NULL && !(m_bUserOperate[ChairID] & iOpType) && iOpType != TYPE_PASS)
		|| (iOpType == TYPE_PASS && m_bUserOperate[ChairID] == TYPE_NULL))
	{
		log_error("%s error uid:%d chairid:%d iOpType:0x%x m_bUserOperate:0x%x\n", __FUNCTION__, GetPlayerUid(ChairID), ChairID, iOpType, m_bUserOperate[ChairID]);
		return false;
	}
	if (Is_ChiPengGang(iOpType) && (bCard == 0x0 || bCard == 0xff))
	{
		log_error("%s error uid:%d chairid:%d iOpType:0x%x card 0x%02x!\n", __FUNCTION__, GetPlayerUid(ChairID), ChairID, iOpType, bCard);
		return false;
	}
	if ((iOpType != TYPE_PASS) && (iOpType & (TYPE_CHI | TYPE_PENG)) && (bCard != m_bCurOutCard))
	{
		log_error("%s error uid:%d chairid:%d iOpType:0x%x card 0x%02x outcard:0x%02x!\n", __FUNCTION__, GetPlayerUid(ChairID), ChairID, iOpType, bCard, m_bCurOutCard);
		return false;
	}
	if (m_bUserOperate[ChairID] == TYPE_NULL)
	{
		log_error("%s error uid:%d chairid:%d iOpType:0x%x m_bUserOperate:0x%x\n", __FUNCTION__, GetPlayerUid(ChairID), ChairID, iOpType, m_bUserOperate[ChairID]);
		return false;
	}
	if (m_bRobWanGang && ChairID == m_WangGangPlayer)
	{
		log_error("%s rob gang error uid:%d chairid:%d iOpType:0x%x m_bUserOperate:0x%x\n", __FUNCTION__, GetPlayerUid(ChairID), ChairID, iOpType, m_bUserOperate[ChairID]);
		return false;
	}
	return true;
}

int Table_xianningmajiang::OutCardEstimate_QiangGangHu_AnGang(_uint8 ChairID, _uint8 bCard, MJ_major& major, int& fan_num)
{
	int iOpType = TYPE_NULL;
	HandCards pHc = m_HandCard[ChairID];
	m_GameLogic.AddCard(&pHc, bCard);

//	log_debug("iOpType = %#x", iOpType);
	if (!m_conf.m_bZimoHu)
	{
		iOpType |= Check_Hu(ChairID, pHc, major.m_oFan);
	}
	int bUserHasGuiPaiNum = 0;
	for (int i = 0; i <pHc.ChangeableCardsLen; i++)
	{
		if (pHc.ChangeableCards[i] == m_bXianNingGuiPai)
		{
			bUserHasGuiPaiNum++;
		}
	}
	m_GameLogic.ChuPai(&pHc, bCard);

	if (major.m_oFan.Find(MJ_FAN_TYPE_QIANGGANGHU) && major.m_oFan.Find(MJ_FAN_TYPE_PINGHU))
	{
		//major.m_oFan.Remove(MJ_FAN_TYPE_PINGHU);
	}
	//log_info("iOpType = %x", iOpType);
	return iOpType;
}

void Table_xianningmajiang::RobGangPriority_QiangAnGang(_uint8 ChairID, _uint8 CardData, int OperateType)
{

	if (m_conf_xianningmj.mj_conf_xianningmajiang == true)
	{
		if (M_bUserDoMultiOperate[ChairID] == true)
		{
			return;
		}
	}
	m_OnUserQiangAnGangCard[ChairID] = CardData;
	M_bUserDoMultiOperateQiangAnGang[ChairID] = OperateType;
	log_info("tid:%d RobGangPriority_QiangAnGang ChairID = %d CardData = %x OperateType = %x m_RobWanGangCard = %x m_bUserOperate[ChairID]= %x",tid, ChairID, CardData, OperateType, m_RobWanGangCard, m_bUserOperate[ChairID]);
	if (OperateType == TYPE_HU&&CardData == m_RobWanGangCard&&(m_bUserOperate[ChairID]&TYPE_HU))
	{
		RobGangPriority(ChairID, CardData, OperateType);
	}
	else if (CardData != m_RobWanGangCard&&OperateType != TYPE_PASS)
	{

		bool bflag = false;
	//	log_info("M_bUserDoMultiOperateQiangAnGang[ChairID = %d] = %x", ChairID, M_bUserDoMultiOperateQiangAnGang[ChairID]);

		for (_uint8 c = PreChair(ChairID); c != m_WangGangPlayer; c = PreChair(c))
		{
		//	log_info("M_bUserDoMultiOperateQiangAnGang[c = %d] = %x", ChairID, M_bUserDoMultiOperateQiangAnGang[c]);

			if (M_bUserDoMultiOperateQiangAnGang[c]==TYPE_NULL&&M_bSendUserQiangAnGangHuFlag[c]==true)
			{
				bflag = true;
			}
		}	
	//	log_info("M_bUserDoMultiOperateQiangAnGang[ChairID = %d] = %x bflag = %d", ChairID, M_bUserDoMultiOperateQiangAnGang[ChairID], bflag);

		if (bflag == true)
		{
		//	log_info("M_bCalcUserQiangAnGangHuFlag[ChairID = %d] = %x", ChairID, M_bCalcUserQiangAnGangHuFlag[ChairID]);

			M_bCalcUserQiangAnGangHuFlag[ChairID] = true;
		}
		else
		{
			//log_info("M_bCalcUserQiangAnGangHuFlag[ChairID = %d] = %x", ChairID, M_bCalcUserQiangAnGangHuFlag[ChairID]);

			Calc_AnGangScore(ChairID);
			for (_uint8 c = NextChair(ChairID); c != m_WangGangPlayer;c = NextChair(c))
			{
				//log_info("M_bCalcUserQiangAnGangHuFlag[c = %d] = %x", c, M_bCalcUserQiangAnGangHuFlag[c]);
				if (M_bUserDoMultiOperateQiangAnGang[c] == TYPE_NULL&&M_bSendUserQiangAnGangHuFlag[c] == true)
				{
					break;
				}
				if (M_bCalcUserQiangAnGangHuFlag[c]==true)
				{
					Calc_AnGangScore(c);
				}
			}
		}	
		RobGangPriority(ChairID, CardData, TYPE_PASS);
	}
	else
	{
		bool bflag = false;
		for (_uint8 c = PreChair(ChairID); c != m_WangGangPlayer; c = PreChair(c))
		{
			//	log_info("M_bUserDoMultiOperateQiangAnGang[c = %d] = %x", ChairID, M_bUserDoMultiOperateQiangAnGang[c]);

			if (M_bUserDoMultiOperateQiangAnGang[c] == TYPE_NULL&&M_bSendUserQiangAnGangHuFlag[c] == true)
			{
				bflag = true;
			}
		}
		if (bflag==false)
		{
			for (_uint8 c = NextChair(ChairID); c != m_WangGangPlayer; c = NextChair(c))
			{
				//log_info("M_bCalcUserQiangAnGangHuFlag[c = %d] = %x", c, M_bCalcUserQiangAnGangHuFlag[c]);
				if (M_bUserDoMultiOperateQiangAnGang[c] == TYPE_NULL&&M_bSendUserQiangAnGangHuFlag[c] == true)
				{
					break;
				}
				if (M_bCalcUserQiangAnGangHuFlag[c] == true)
				{
					Calc_AnGangScore(c);
				}
			}
		}
		RobGangPriority(ChairID, CardData, TYPE_PASS);
	}
}
