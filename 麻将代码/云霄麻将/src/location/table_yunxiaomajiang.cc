#include "table_yunxiaomajiang.h"
#include "../proto/proto.h"
#include "../zjh.h"
#include "../common/log.h"
#include "../game.h"
#include "../libnormalmahjong/mj_common2.h"
#include "../libnormalmahjong/game_logic.h"
#include "../table_helper.h"
/*修改记录
2018/3/26
新增规则
清一色过张胡特例，如果其他胡点炮不胡，清一色点炮仍可以胡，如果清一色点炮不胡，不过张不能再胡清一色
2018/4/19
新增二人时可以不能点炮的的选型。
*/




using namespace proto::game::yunxiaomj;

extern ZJH zjh;
extern Log log;

class Yunxiaomj_Fantype_to_FanNum : public Fantype_to_FanNum
{

public:
	Yunxiaomj_Fantype_to_FanNum()
	{			
		fan_num[MJ_FAN_TYPE_PINGHU]			 = 2;//17	//平胡
		fan_num[MJ_FAN_TYPE_ZIMO]			 = 2;//19	//自摸胡
		fan_num[MJ_FAN_YUNXIAO_SANJINDAO]	 = 2;//564	//三金倒 
		fan_num[MJ_FAN_TYPE_TIANHU]			 = 10;//19	//天胡 
		fan_num[MJ_FAN_TYPE_DIHU]			 = 10; //20	//庄家10分，其余2家9分
		fan_num[MJ_FAN_YUNXIAO_QIANGJINHU]   = 2;//565	//抢金胡 
		fan_num[MJ_FAN_YUNXIAO_19HU]         = 4;//35	//幺九胡
		fan_num[MJ_FAN_TYPE_QIANGGANGHU]     = 2;//22	//抢杠胡，
		fan_num[MJ_FAN_YUNXIAO_MINGGANGHU]   = 4; //567  //明杠胡：
		fan_num[MJ_FAN_YUNXIAO_ANGANGHU]     = 2;//566	//暗杠胡：
		fan_num[MJ_FAN_YUNXIAO_BUDAHU]       = 2;//568	//不搭：
		fan_num[MJ_FAN_TYPE_QIDUI]           = 2;//36	//对对胡：
		fan_num[MJ_FAN_TYPE_QINGYISE]        = 10;//14	//清一色，
		fan_num[MJ_FAN_YUNXIAO_DANYOUJIN]	 = 4;//569  //单游金
		fan_num[MJ_FAN_YUNXIAO_DOUBLEYOUJIN] = 8;//570   //双游金
		fan_num[MJ_FAN_YUNXIAO_THERRYOUJIN] = 16;//570   //三游金
		SetFan(MJ_FAN_TYPE_PINGHU, 2);
		SetFan(MJ_FAN_TYPE_ZIMO, 2);
		SetFan(MJ_FAN_YUNXIAO_SANJINDAO, 2);
		SetFan(MJ_FAN_TYPE_TIANHU, 10);
		SetFan(MJ_FAN_TYPE_DIHU, 10);
		SetFan(MJ_FAN_YUNXIAO_QIANGJINHU, 2);
		SetFan(MJ_FAN_YUNXIAO_19HU, 4);
		SetFan(MJ_FAN_TYPE_QIANGGANGHU, 2);
		SetFan(MJ_FAN_YUNXIAO_MINGGANGHU, 4);
		SetFan(MJ_FAN_YUNXIAO_ANGANGHU, 2);
		SetFan(MJ_FAN_YUNXIAO_BUDAHU, 2);
		SetFan(MJ_FAN_TYPE_QIDUI, 2);
		SetFan(MJ_FAN_TYPE_QINGYISE, 10);
		SetFan(MJ_FAN_YUNXIAO_DANYOUJIN, 4);
		SetFan(MJ_FAN_YUNXIAO_DOUBLEYOUJIN, 8);
		SetFan(MJ_FAN_YUNXIAO_THERRYOUJIN, 16);
	
	}
	bool FanFilter(const MJ_fan_buffer& oFan, MJ_fan_buffer& oOutFan)
	{
		//进行加番处理
		oOutFan.Clear();

		for (int i = 0; i < oFan.Size(); i++)
		{	
			oOutFan.Add(oFan[i]);
		}
		if (oOutFan.Size() >= 2)
		{	//当有平胡和其他胡时，平胡不算分。
			oOutFan.Find(MJ_FAN_TYPE_PINGHU);
			oOutFan.Remove(MJ_FAN_TYPE_PINGHU);
		}
		if (oOutFan.Find(MJ_FAN_YUNXIAO_19HU)&& oOutFan.Find(MJ_FAN_TYPE_QINGYISE))
		{
			oOutFan.Remove(MJ_FAN_YUNXIAO_19HU);
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
		throw("out of range!\n");
		return 0;
	}
};

static Yunxiaomj_Fantype_to_FanNum g_yunxiaomajiang_ft2fn;

Table_yunxiaomajiang::Table_yunxiaomajiang()
{	
	m_bJinCard			= INVALID_MJ_CARD;	 //金牌标志
	memset(&m_bUserHasJinNum, 0, sizeof(m_bUserHasJinNum));//玩家拥有金牌的数量
	m_bReplaceJinCard	= 0x43; //使用白板代替金原来的牌
	memset(&m_bHasReplaceJinCardNum, 0, sizeof(m_bHasReplaceJinCardNum));//游戏开始白板的数量
	memset(&m_bSaveZiMoCard, INVALID_MJ_CARD, sizeof(m_bSaveZiMoCard));
	memset(&m_bSaveUserOutCard, INVALID_MJ_CARD, sizeof(m_bSaveUserOutCard));
	memset(&m_bIsAnGangHuFlag, 0, sizeof(m_bIsAnGangHuFlag));
	memset(&m_bIsMingGangHuFlag, 0, sizeof(m_bIsMingGangHuFlag));
	memset(&m_bIsGuoZhangHuFlag, 0, sizeof(m_bIsGuoZhangHuFlag));//满足过张胡条件。	
	memset(&m_bUserOperateGuoZhangHuFlag, 0, sizeof(m_bUserOperateGuoZhangHuFlag));//过张胡
//	memset(&m_bUserGetJinFlag, 0, sizeof(m_bUserGetJinFlag));//抢金胡,得到金标志
	memset(&m_bUserOperateQiangJinHuFlag, 0, sizeof(m_bUserOperateQiangJinHuFlag));//抢金胡,得到金标志
	m_ft2fn = &g_yunxiaomajiang_ft2fn;
	//单游金
	memset(&m_bUserDanYouJinHuFlag, 0, sizeof(m_bUserDanYouJinHuFlag)); //满足单游金胡牌
	memset(&m_bUserOperateDanJinPass, 0, sizeof(m_bUserOperateDanJinPass));//单游金胡牌时,pass
	memset(&m_bUserOperateDanYouJinHu, 0, sizeof(m_bUserOperateDanYouJinHu));//满足游，可以胡游
	memset(&m_bUserOperateDanYouJinstep, 0, sizeof(m_bUserOperateDanYouJinstep));//抢金胡,得到金标志
	memset(&m_bSaveUserOutOnlyCard, 0, sizeof(m_bSaveUserOutOnlyCard));//单游胡时用户可以打出的牌使进入单游金状态。
	//双游金
	memset(&m_bUserDoubleYouJinHuFlag, 0, sizeof(m_bUserDoubleYouJinHuFlag));//抢金胡,得到金标志
	memset(&m_bUserOperateDoubleYouJinFlag, 0, sizeof(m_bUserOperateDoubleYouJinFlag));//单游金运行标志
	memset(&m_bUserOperateDoubleYouJinStep, 0, sizeof(m_bUserOperateDoubleYouJinStep));//单游胡时用户可以打出的牌使进入单游金状态。
	//三游金
	memset(&m_bUserThreeYouJinHuFlag, 0, sizeof(m_bUserThreeYouJinHuFlag));//抢金胡,得到金标志
	memset(&m_bUserOperateThreeYouJinFlag, 0, sizeof(m_bUserOperateThreeYouJinFlag));//单游金运行标志
	memset(&m_bUserOperateThreeYouJinStep, 0, sizeof(m_bUserOperateThreeYouJinStep));//单游胡时用户可以打出的牌使进入单游金状态。

	m_QingYiSeDianPaoBaoPei = false;
	memset(&m_bUserOperateQiangJinHuPass, 0, sizeof(m_bUserOperateQiangJinHuPass));//单游胡时用户可以打出的牌使进入单游金状态。

	m_bUserOperateGangNum = 0;
	memset(&m_bUserOperateGangCardInfo, 0, sizeof(m_bUserOperateGangCardInfo));
	memset(&m_bUserOperateChiPengGangDanYouJinstep, 0, sizeof(m_bUserOperateChiPengGangDanYouJinstep));
	memset(&m_bUserOperateChiPengGangDoubleYouJinstep, 0, sizeof(m_bUserOperateChiPengGangDoubleYouJinstep));
	memset(&m_bUserOperateChiPengGangThreeYouJinstep, 0, sizeof(m_bUserOperateChiPengGangThreeYouJinstep));
 
	memset(&m_bUserDoMultiOperateiOpType, 0, sizeof(m_bUserDoMultiOperateiOpType));
	memset(&m_bUserDoMultiOperateiOpTypeFlag, 0, sizeof(m_bUserDoMultiOperateiOpTypeFlag));
	memset(&m_bUerOutErrFlag, 0, sizeof(m_bUerOutErrFlag));

	memset(&m_bOutTimeStopOperateOutCardFlag, 0, sizeof(m_bOutTimeStopOperateOutCardFlag));
	memset(&m_bUserOutCardFlag, INVALID_MJ_CARD, sizeof(m_bUserOutCardFlag));
	memset(m_bUserCheckDoMultiOperateiOpTypeFlag, 0, sizeof(m_bUserCheckDoMultiOperateiOpTypeFlag));
	//memset(m_bUserChiCardOutCardFlag, 0, sizeof(m_bUserChiCardOutCardFlag));
	memset(m_bUserHuTypeoFan, 0, sizeof(m_bUserHuTypeoFan));
	memset(m_bUserHuQingYiSeFlag, 0, sizeof(m_bUserHuQingYiSeFlag));

	memset(m_bUserOperateShuangYouJinZhaungThreeFlag, 0, sizeof(m_bUserOperateShuangYouJinZhaungThreeFlag));

	m_bUserAutoOutCardFlag = false;
	//摸牌判断出牌的定时器
	m_GetCardOperateOutCard_Timer_stamp = 16;
	m_GetCardOperateOutCard_Timer.data = this;
	ev_timer_init(&m_GetCardOperateOutCard_Timer, Table_yunxiaomajiang::GetCardOperateOutCard_Timer_cb, m_GetCardOperateOutCard_Timer_stamp, m_GetCardOperateOutCard_Timer_stamp);
	//摸牌胡牌判断的定时器
	m_GetCardOperateHu_Timer_stamp = 15;
	m_GetCardOperateHu_Timer.data = this;
	ev_timer_init(&m_GetCardOperateHu_Timer, Table_yunxiaomajiang::GetCardOperateHu_Timer_cb, m_GetCardOperateHu_Timer_stamp, m_GetCardOperateHu_Timer_stamp);
	//游金超时定时器
	m_YouJinOperateHuYouJin_Timer_stamp_0 = 15;
	m_YouJinOperateHuYouJin_Timer_0.data = this;
	ev_timer_init(&m_YouJinOperateHuYouJin_Timer_0, Table_yunxiaomajiang::YunXiaoYouJinOperateHu_Timer_cb_0, m_YouJinOperateHuYouJin_Timer_stamp_0, m_YouJinOperateHuYouJin_Timer_stamp_0);

	m_YouJinOperateHuYouJin_Timer_stamp_1 = 15;
	m_YouJinOperateHuYouJin_Timer_1.data = this;
	ev_timer_init(&m_YouJinOperateHuYouJin_Timer_1, Table_yunxiaomajiang::YunXiaoYouJinOperateHu_Timer_cb_1, m_YouJinOperateHuYouJin_Timer_stamp_1, m_YouJinOperateHuYouJin_Timer_stamp_1);

	//出牌，如果玩家超时，并且不胡牌，则直接选择pass
	m_OutCardOutTimeOperatePass_Timer_stamp_0 = 15;
	m_OutCardOutTimeOperatePass_Timer_0.data = this;
	ev_timer_init(&m_OutCardOutTimeOperatePass_Timer_0, Table_yunxiaomajiang::OutCardOperatePass_Timer_cb_0, m_OutCardOutTimeOperatePass_Timer_stamp_0, m_OutCardOutTimeOperatePass_Timer_stamp_0);
	m_OutCardOutTimeOperatePass_Timer_stamp_1 = 15;
	m_OutCardOutTimeOperatePass_Timer_1.data = this;
	ev_timer_init(&m_OutCardOutTimeOperatePass_Timer_1, Table_yunxiaomajiang::OutCardOperatePass_Timer_cb_1, m_OutCardOutTimeOperatePass_Timer_stamp_1, m_OutCardOutTimeOperatePass_Timer_stamp_1);
	m_OutCardOutTimeOperatePass_Timer_stamp_2 = 15;
	m_OutCardOutTimeOperatePass_Timer_2.data = this;
	ev_timer_init(&m_OutCardOutTimeOperatePass_Timer_2, Table_yunxiaomajiang::OutCardOperatePass_Timer_cb_2, m_OutCardOutTimeOperatePass_Timer_stamp_2, m_OutCardOutTimeOperatePass_Timer_stamp_2);
	m_OutCardOutTimeOperatePass_Timer_stamp_3 = 15;
	m_OutCardOutTimeOperatePass_Timer_3.data = this;
	ev_timer_init(&m_OutCardOutTimeOperatePass_Timer_3, Table_yunxiaomajiang::OutCardOperatePass_Timer_cb_3, m_OutCardOutTimeOperatePass_Timer_stamp_3, m_OutCardOutTimeOperatePass_Timer_stamp_3);
	//出牌，如果玩家超时，并且胡牌，则直接选择胡牌
	m_OutCardOutTimeOperateHu_Timer_stamp_0 = 15;
	m_OutCardOutTimeOperateHu_Timer_0.data = this;
	ev_timer_init(&m_OutCardOutTimeOperateHu_Timer_0, Table_yunxiaomajiang::OutCardOperateHu_Timer_cb_0, m_OutCardOutTimeOperateHu_Timer_stamp_0, m_OutCardOutTimeOperateHu_Timer_stamp_0);
	m_OutCardOutTimeOperateHu_Timer_stamp_1 = 15;
	m_OutCardOutTimeOperateHu_Timer_1.data = this;
	ev_timer_init(&m_OutCardOutTimeOperateHu_Timer_1, Table_yunxiaomajiang::OutCardOperateHu_Timer_cb_1, m_OutCardOutTimeOperateHu_Timer_stamp_1, m_OutCardOutTimeOperateHu_Timer_stamp_1);
	m_OutCardOutTimeOperateHu_Timer_stamp_2 = 15;
	m_OutCardOutTimeOperateHu_Timer_2.data = this;
	ev_timer_init(&m_OutCardOutTimeOperateHu_Timer_2, Table_yunxiaomajiang::OutCardOperateHu_Timer_cb_2, m_OutCardOutTimeOperateHu_Timer_stamp_2, m_OutCardOutTimeOperateHu_Timer_stamp_2);
	m_OutCardOutTimeOperateHu_Timer_stamp_3 = 15;
	m_OutCardOutTimeOperateHu_Timer_3.data = this;
	ev_timer_init(&m_OutCardOutTimeOperateHu_Timer_3, Table_yunxiaomajiang::OutCardOperateHu_Timer_cb_3, m_OutCardOutTimeOperateHu_Timer_stamp_3, m_OutCardOutTimeOperateHu_Timer_stamp_3);

	m_ChiPengCardOperateOutCard_Timer_stamp = 16;
	m_ChiPengCardOperateOutCard_Timer.data = this;
	ev_timer_init(&m_ChiPengCardOperateOutCard_Timer, Table_yunxiaomajiang::ChiPengCardOperateOutCard_Timer_cb, m_ChiPengCardOperateOutCard_Timer_stamp, m_ChiPengCardOperateOutCard_Timer_stamp);

	m_OutTimeStopOperateOutCard_Timer_stamp = 13;
	m_OutTimeStopOperateOutCard_Timer.data = this;
	ev_timer_init(&m_OutTimeStopOperateOutCard_Timer, Table_yunxiaomajiang::OutTimeStopOperateOutCard_Timer_cb, m_OutTimeStopOperateOutCard_Timer_stamp, m_OutTimeStopOperateOutCard_Timer_stamp);

}
Table_yunxiaomajiang::~Table_yunxiaomajiang()
{	
	ev_timer_stop(zjh.loop, &m_GetCardOperateOutCard_Timer);
	ev_timer_stop(zjh.loop, &m_GetCardOperateHu_Timer);
	ev_timer_stop(zjh.loop, &m_YouJinOperateHuYouJin_Timer_0);
	ev_timer_stop(zjh.loop, &m_YouJinOperateHuYouJin_Timer_1);
	ev_timer_stop(zjh.loop, &m_ChiPengCardOperateOutCard_Timer);
	ev_timer_stop(zjh.loop, &m_OutTimeStopOperateOutCard_Timer);

	ev_timer_stop(zjh.loop, &m_OutCardOutTimeOperatePass_Timer_0);
	ev_timer_stop(zjh.loop, &m_OutCardOutTimeOperatePass_Timer_1);
	ev_timer_stop(zjh.loop, &m_OutCardOutTimeOperatePass_Timer_2);
	ev_timer_stop(zjh.loop, &m_OutCardOutTimeOperatePass_Timer_3);

	ev_timer_stop(zjh.loop, &m_OutCardOutTimeOperateHu_Timer_0);
	ev_timer_stop(zjh.loop, &m_OutCardOutTimeOperateHu_Timer_1);
	ev_timer_stop(zjh.loop, &m_OutCardOutTimeOperateHu_Timer_2);
	ev_timer_stop(zjh.loop, &m_OutCardOutTimeOperateHu_Timer_3);

}

void Table_yunxiaomajiang::ConfInit_Remote(const Json::Value& val)
{
	Table::ConfInit_Remote(val);
	m_conf.m_bSupportChi		= true;			//是否能吃	
	m_conf.m_bZimoHu			= false;		//是否只能自摸胡	
	m_conf.m_bHu13yao			= true;
	m_conf.m_bHu7dui			= true;
	m_conf.m_bZhuangXian		= false;			//庄家闲家分开计算
	m_conf.m_bQiangGangHu		= true;
	m_conf.m_bQiShou_14_zhang   = false;
	m_CardPool.Set(CARD_POOL_NO_FLOWER, tid); //设置牌的数量,云霄麻将 ，136张，筒条万+风（东南西北中发白）

	if (json_check_play_type_id(val))
	{
		MJ_SUB_ID subid;
		json_get_play_type_id(val, 5060, subid);

		if(subid.Find(5060001))
		{	
			m_GAME_PLAYER = 4;
			m_room_config.Add(subid[0]);
		}		
		else if (subid.Find(5060002))
		{
			m_GAME_PLAYER = 3;
			m_room_config.Add(subid[0]);
		}
		else
		{
			m_GAME_PLAYER = 2;
			m_room_config.Add(subid[0]);
		}

		
		/*点炮包赔：默认不勾，*/
		json_get_play_type_id(val, 5061, subid);
		if (subid.Find(5061001))
		{
			m_room_config.Add(5061001);
			m_conf_yunxiaomj.yun_xiao_dian_pao_bao_pei = true;
		}

		/*自动出牌，*/
		json_get_play_type_id(val, 5062, subid);
		if (subid.Find(5062001))
		{
			m_conf_yunxiaomj.yunxiao_zidong_chupai_hupai_pass = true;
			m_room_config.Add(5062001);
		}
		if (subid.Find(5062002))
		{
			m_room_config.Add(5062002);
		}

		int iCounts = json_get_play_type_id(val, 5063, subid);
		if (iCounts != 0)
		{
			if (subid.Find(5063001))
			{//二人可胡点炮
				m_room_config.Add(5063001);
				m_conf_yunxiaomj.yunxiao_twopeople_zimohu_flag = false;
			}else if (subid.Find(5063002)&& m_GAME_PLAYER==2)
			{
				m_room_config.Add(5063002);
				m_conf_yunxiaomj.yunxiao_twopeople_zimohu_flag = true;
			}
		}
			
	}
}

void Table_yunxiaomajiang::GameStart()
{
	Table::GameStart();
	//玩家开金
	m_bJinCard = m_CardPool.GetOneCard();
	//log_info("m_bJinCard =%x ", m_bJinCard);
	//统计玩家手中金与白板个数的信息
	for (int i = 0; i < m_GAME_PLAYER; i++)
	{
		for (int j = 0; j < m_HandCard[i].ChangeableCardsLen; j++)
		{
			if (m_HandCard[i].ChangeableCards[j] == m_bJinCard)
			{
				m_bUserHasJinNum[i]++;
			}
			if (m_HandCard[i].ChangeableCards[j] == m_bReplaceJinCard)
			{
				m_bHasReplaceJinCardNum[i]++;
			}
		}
	}
	//把开金信息广播
	if (m_bJinCard != INVALID_MJ_CARD)
	{
		proto::game::yunxiaomj::JinInfo stKaiJinInfo;
		stKaiJinInfo.set_kaijincard(m_bJinCard);
		stKaiJinInfo.set_gamestartjinflag(true);
		broadcast(SERVER_ROOM_KAIJIN_INFO, stKaiJinInfo);
		std::string str = stKaiJinInfo.SerializeAsString();
		save_video_data(SERVER_ROOM_KAIJIN_INFO, str);
		//对玩家的牌进行金与白板的替换，在后台处理中,始终使用白板作为金来操作.
	}

	for (int i = 0; i < m_GAME_PLAYER; i++)
	{
		int JinIndex = 0;
		for (int j = 0; j < m_HandCard[i].ChangeableCardsLen; j++)
		{
			if (m_HandCard[i].ChangeableCards[j] == m_bJinCard)
			{
				if (JinIndex >= m_bUserHasJinNum[i])
				{
					break;
				}
				m_HandCard[i].ChangeableCards[j] = m_bReplaceJinCard;
				JinIndex++;
			}
		}
	}
	for (int i = 0; i < m_GAME_PLAYER; i++)
	{
		int JinIndex = 0;
		for (int j = 0; j < m_HandCard[i].ChangeableCardsLen; j++)
		{
			if (m_HandCard[i].ChangeableCards[j] == m_bReplaceJinCard)
			{
				if (JinIndex >= m_bHasReplaceJinCardNum[i])
				{
					break;
				}
				m_HandCard[i].ChangeableCards[j] = m_bJinCard;
				JinIndex++;
			}
		}
	}
	//替换完成后,对手牌进行重新排序.
	for (int i = 0; i < m_GAME_PLAYER; i++)
	{
		m_GameLogic.SortCard(m_HandCard[i].ChangeableCards, m_HandCard[i].ChangeableCardsLen);
	}
	log_info("%s tid:%d  \n", __FUNCTION__, tid);
	/*庄家抓第一张牌*/
	OnUserGetCard(m_bBanker);
}

 void Table_yunxiaomajiang::Proto_GameScene(_uint8 ChairID, Player* player, proto::game::AckGameScene& stGameScene)
 {
	 //断线后,需要替换掉手牌中的金与白板,之后发给客户端后,重新进行替换
	
	// Table::Proto_GameScene(ChairID, player, stGameScene);

	 log_info("%s begin to uid:%d chairid:%d gamestate:%d \n", __FUNCTION__, player->uid, ChairID, m_bGameState);
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
	// log_info("m_bCurCard =%x", m_bCurCard);
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
	 ChangeHandCardBaiBanAndJin();
	 Fill_HandCard(ChairID, pstHc);
	 ChangeHandCardBaiBanAndJin();
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
			 ChangeHandCardBaiBanAndJin();
			 Fill_FixedCard(ChairID,i, j, pstFix);
			 ChangeHandCardBaiBanAndJin();
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
	 //客户端显示的房间配置信息.
	 AckCustomiedInfo* info = stGameScene.mutable_yunxiaomjscene();
	 if (info != NULL)
	 {
		 info->set_gametype(proto::game::yunxiaomj::HUNAN_CHANGSHA);
		 info->set_ishavepiao(0);
		 info->set_jiangma(proto::game::yunxiaomj::JIANGMA);//抓鸟
		 info->set_laizi(0);//没有癞子

		 for (int i = 0; i < m_room_config.Size(); i++)
		 {
			 info->add_roomconfigid(m_room_config[i]);
		 }
		 //if (m_bUserChiCardOutCardFlag[ChairID]!= INVALID_MJ_CARD)
		 //{
			// info->add_bancards(m_bUserChiCardOutCardFlag[ChairID]);
		 //}
	 }
	 log_info("%s tid:%d uid:%d chairid:%d left timer:%d m_bUserOperate:0x%x stGameScene:%s\n", __FUNCTION__, tid, GetPlayerUid(player), ChairID, stGameScene.lefttime(), m_bUserOperate[ChairID], stGameScene.DebugString().c_str());

	 //把开金信息广播
	 if (m_bJinCard != INVALID_MJ_CARD)
	 {
		 proto::game::yunxiaomj::JinInfo stKaiJinInfo;
		 stKaiJinInfo.set_kaijincard(m_bJinCard);
		 stKaiJinInfo.set_gamestartjinflag(false);
		 unicast(SERVER_ROOM_KAIJIN_INFO, ChairID, stKaiJinInfo);
		 std::string str = stKaiJinInfo.SerializeAsString();
		 save_video_data(SERVER_ROOM_KAIJIN_INFO, str);
		 log_info("%s tid:%d uid:%d chairid:%d stKaiJinInfo:%s", __FUNCTION__,tid, GetPlayerUid(player), ChairID, stKaiJinInfo.DebugString().c_str());

	 }
	 if (m_bOutTimeStopOperateOutCardFlag[ChairID] == true&& m_conf_yunxiaomj.yunxiao_zidong_chupai_hupai_pass == true)
	 {
		 proto::game::yunxiaomj::AckUserStopOutCard stUserStopOutCard;
		 stUserStopOutCard.set_chairid(m_bCurChairID);
		 stUserStopOutCard.set_stopoutcardflag(true);
		 broadcast(SER_CMD_STOP_OUT_CARD, stUserStopOutCard);
		 log_info("999 Proto_GameScene m_bOutTimeStopOperateOutCardFlag stUserStopOutCard = %s", stUserStopOutCard.DebugString().c_str());
	 }

 }

 void Table_yunxiaomajiang::Fill_FixedCard(_uint8 OpChairID, _uint8 ChairID, _uint8 Pos, proto::game::FixedCard* pstFix)
 {
	// log_info("OpChairID = %d ChairID = %d", OpChairID, ChairID);
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
			// log_info("OpChairID = %d ChairID = %d", OpChairID, ChairID);
			 cbCard = 0x00;
		 }
		 else
		 {
			 cbCard = m_HandCard[ChairID].FixedCards[Pos].CardData;
		 }
	 }
	// log_info("cbCard  = %x ", cbCard);

	 pstFix->set_carddata(cbCard);
	 pstFix->set_state(m_HandCard[ChairID].FixedCards[Pos].state);
	 pstFix->set_chairid(m_HandCard[ChairID].FixedCards[Pos].chairID);
	 log_info("%s uid:%d ChairID:%d \n", __FUNCTION__, GetPlayerUid(ChairID), ChairID);
 }

 void Table_yunxiaomajiang::OnUserGetCard(_uint8 ChairID, bool bGetLast)
 {
	// m_bUserChiCardOutCardFlag[ChairID] = INVALID_MJ_CARD;
	 memset(m_bUserCheckDoMultiOperateiOpTypeFlag, 0, sizeof(m_bUserCheckDoMultiOperateiOpTypeFlag));
	 m_bUserOutCardFlag[ChairID] = INVALID_MJ_CARD;
	 m_bUerOutErrFlag[ChairID] = false;
	 m_bUserHuQingYiSeFlag[ChairID] = false;
	 if (m_conf_yunxiaomj.yunxiao_zidong_chupai_hupai_pass == true)
	 {
		 //摸牌之后关闭所有的定时器，

		 TIMER_STOP(m_GetCardOperateOutCard_Timer);
		 TIMER_STOP(m_GetCardOperateHu_Timer);
		 TIMER_STOP(m_YouJinOperateHuYouJin_Timer_0);
		 TIMER_STOP(m_YouJinOperateHuYouJin_Timer_1);
		 TIMER_STOP(m_ChiPengCardOperateOutCard_Timer);
		 TIMER_STOP(m_OutTimeStopOperateOutCard_Timer);

		 TIMER_STOP(m_OutCardOutTimeOperatePass_Timer_0);
		 TIMER_STOP(m_OutCardOutTimeOperatePass_Timer_1);
		 TIMER_STOP(m_OutCardOutTimeOperatePass_Timer_2);
		 TIMER_STOP(m_OutCardOutTimeOperatePass_Timer_3);

		 TIMER_STOP(m_OutCardOutTimeOperateHu_Timer_0);
		 TIMER_STOP(m_OutCardOutTimeOperateHu_Timer_1);
		 TIMER_STOP(m_OutCardOutTimeOperateHu_Timer_2);
		 TIMER_STOP(m_OutCardOutTimeOperateHu_Timer_3);

	 }

	 if (m_bDeleteTable)
	 {
		 return;
	 }


	 if (IsQiangJinHu( ChairID))
	 {
		 if (m_conf_yunxiaomj.yunxiao_zidong_chupai_hupai_pass == true)
		 {
			 set_operator_timer(ChairID);
			 const int tmp_Timer_stamp = 15;
			 TIMER_AGAIN(m_GetCardOperateHu_Timer, tmp_Timer_stamp);		 
		 }
		 return;
	 }

	 if (m_bUserOperateThreeYouJinFlag[ChairID] == true)
	 {
		 m_bUserOperateThreeYouJinStep[ChairID] = true;
		 OnUserSendDanYoujinHu(ChairID);
		 return;
	 }
	 if (m_bUserOperateDoubleYouJinFlag[ChairID]== true)
	 {
		 m_bUserOperateDoubleYouJinStep[ChairID] = true;
		 OnUserSendDanYoujinHu(ChairID);
		 return;
	 }
	 //满足单游金状态,，并且可以游，则向客户端发送单游金
	 if (m_bUserOperateDanYouJinHu[ChairID] == true)
	 {
		 //set_operator_timer(ChairID);
		 m_bUserOperateDanYouJinstep[ChairID] = true;
		 OnUserSendDanYoujinHu(ChairID);
		 return;
	 }

	 if (m_CardPool.Count() <= 34 && m_bAddFanFlag[ChairID] == false)
	 {
		 GameEnd();
		 return;
	 }
	 memset(&m_bUserOperateGangCardInfo[ChairID], 0, sizeof(m_bUserOperateGangCardInfo[ChairID]));
	 m_bUserOperateGuoZhangHuFlag[ChairID] = false;	 //操作过张胡标志

	 memset(&m_bUserOperate, 0, sizeof(m_bUserOperate));
	 m_bCurChairID = ChairID;//当前出牌或摸牌的玩家ID
	 _uint8 bGetCard = bGetLast ? m_CardPool.GetLast() : m_CardPool.GetOneCard();
	 //if (bGetCard == m_bJinCard)
	 //{
		// m_bUserGetJinFlag[ChairID] = true;
	 //}
	 m_bGetCardCount[ChairID]++;
	 m_bSaveZiMoCard[ChairID] = bGetCard;
	 //进行金与白板的互换.
	 if (bGetCard == m_bJinCard)
	 {
		 m_bUserHasJinNum[ChairID]++; //保存金的数量
		 bGetCard = m_bReplaceJinCard;
	 }
	 else if (bGetCard == m_bReplaceJinCard)
	 {
		 bGetCard = m_bJinCard;
	 }
	 m_bCurCard = m_bSaveZiMoCard[ChairID]; //保持当前操作的牌，需要保持实际的牌值。
	 MJ_major major;
	 //摸牌判断
	 m_bUserHuTypeoFan[ChairID].Clear();
	 m_bUserOperate[ChairID] = GetCardEstimate(ChairID, bGetCard, major, m_HuFanNums[ChairID]);
	 //log_info("m_bUserOperate[ChairID = %d] = %x", ChairID, m_bUserOperate[ChairID]);
	 m_GameLogic.AddCard(&m_HandCard[ChairID], bGetCard);
	 bGetCard = m_bSaveZiMoCard[ChairID];
	 log_info("%s tid:%d uid:%d chairid:%d getcard:0x%02x optype:0x%x cardpoollen:%d \n", __FUNCTION__, tid, GetPlayerUid(ChairID), ChairID, bGetCard, m_bUserOperate[ChairID], m_CardPool.Count());
	 save_user_get_card(ChairID, bGetCard, m_CardPool.Count());
	 //广播玩家摸牌
	 if (m_bUserOperateGangNum >= 3)
	 {
		 if (!major.m_oFan.Find(MJ_FAN_YUNXIAO_ANGANGHU)&& !major.m_oFan.Find(MJ_FAN_YUNXIAO_MINGGANGHU))
		 {
			 m_bUserOperate[ChairID] = TYPE_NULL;
		 }
		
	 }

	 m_bUserHuTypeoFan[ChairID] = major.m_oFan;
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
		 if (m_bUserOperate[i]>0)
		 {
			 proto::game::AckOpNotify stVideo;
			 stVideo.set_chairid(i);
			 stVideo.set_optype(m_bUserOperate[i]);
			 std::string str = stVideo.SerializeAsString();
			 save_video_data(SERVER_OPERATE_NOTIFY, str);
		 }

		// log_info("%s tid:%d uid:%d chairid:%d i:%d Send UserGetCard stUsergetcard : %s\n", __FUNCTION__, tid, GetPlayerUid(ChairID), ChairID, i, stUsergetcard.DebugString().c_str());

	 }

	 if (m_bUserOperateGangNum >= 3 && !major.m_oFan.Find(MJ_FAN_YUNXIAO_ANGANGHU) && !major.m_oFan.Find(MJ_FAN_YUNXIAO_MINGGANGHU))
	 {
		 GameEnd();
	 }

	 if (m_bUserOperate[ChairID] == TYPE_NULL || m_bUserOperate[ChairID] == TYPE_LISTEN)
	 {
		// log_info("UserOperatePass m_bUserOperate[ChairID = %d]", m_bUserOperate[ChairID]);
		 set_out_timer(ChairID);
		 if (m_conf_yunxiaomj.yunxiao_zidong_chupai_hupai_pass == true)
		 {
			 const int tmp_Timer_stamp_0 = 13;
			 TIMER_AGAIN(m_OutTimeStopOperateOutCard_Timer, tmp_Timer_stamp_0);

			 const int tmp_Timer_stamp = 16;
			 TIMER_AGAIN(m_GetCardOperateOutCard_Timer, tmp_Timer_stamp);
		 }

	 }
	 else
	 {
		 set_operator_timer(ChairID);
		 if (m_conf_yunxiaomj.yunxiao_zidong_chupai_hupai_pass == true)
		 {
		//	 log_info("UserOperatePass m_bUserOperate[ChairID = %d]", m_bUserOperate[ChairID]);
			 if (m_bUserOperate[ChairID] & TYPE_HU)
			 {
				 const int tmp_Timer_stamp = 15;
				 TIMER_AGAIN(m_GetCardOperateHu_Timer, tmp_Timer_stamp);
			 }
			 else
			 {
				 const int tmp_Timer_stamp_0 = 13;
				 TIMER_AGAIN(m_OutTimeStopOperateOutCard_Timer, tmp_Timer_stamp_0);
				 const int tmp_Timer_stamp = 16;
				 TIMER_AGAIN(m_GetCardOperateOutCard_Timer, tmp_Timer_stamp);
			 }
		 }

	 }
 }

 int Table_yunxiaomajiang::GetGangInfo(_uint8 ChairID, MJ_opinfo& GangCardInfo)
 {

	 MJ_opinfo& ganginfo = GangCardInfo;
	 HandCards*pHc = &m_HandCard[ChairID];
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
			 if (pHc->ChangeableCards[i] == m_bReplaceJinCard)
			 {	 //如果自摸的牌是金,则不能暗杠.
				 continue;
			 }
			 if (pHc->ChangeableCards[i] == m_bJinCard)
			 {
				 ganginfo.Add(OpState(m_bReplaceJinCard, TYPE_ANGANG));
				 iOpType |= TYPE_ANGANG;
				 ganginfo.Add(OpState(m_bReplaceJinCard, TYPE_ZHIGANG));
				 iOpType |= TYPE_ZHIGANG;
			 } 
			 else
			 {
				 ganginfo.Add(OpState(pHc->ChangeableCards[i], TYPE_ANGANG));
				 iOpType |= TYPE_ANGANG;
				 ganginfo.Add(OpState(pHc->ChangeableCards[i], TYPE_ZHIGANG));
				 iOpType |= TYPE_ZHIGANG;
			 }
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
					 if (pHc->ChangeableCards[j] == m_bJinCard)
					 {
						 ganginfo.Add(OpState(m_bReplaceJinCard, TYPE_WANGANG));
						 iOpType |= TYPE_WANGANG;
						 break;
					 }
					 else
					 {
						 ganginfo.Add(OpState(pHc->ChangeableCards[j], TYPE_WANGANG));
						 iOpType |= TYPE_WANGANG;
						 break;
					 }
				 }
			 }
		 }
	 }
	// log_info("ChairID = Get GangInfo stGang. size() = %d", ChairID, ganginfo.Size());
	 //for (int i = 0; i < ganginfo.Size();i++)
	 //{
		// log_info("ganginfo: i = %d  ganginfo[i].cbCard= %x  anginfo[i].nState=%x", i, ganginfo[i].cbCard, ganginfo[i].nState);
	 //}

	 return iOpType;
	// return m_GameLogic.GetGangInfo(&m_HandCard[ChairID], GangCardInfo);
	
 }

 int Table_yunxiaomajiang::Check_Hu(_uint8 ChairID, HandCards& pHc, MJ_fan_buffer& oFan)
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

	 bool IsHuFlag = false;
	 MJ_win_pattern pattern;
	 if (Is7Dui(ChairID,pHc, oFan, m_bUserHasJinNum[ChairID]))
	 {
		 //log_info("Is7Dui ok");
		 ///*格式化牌*/
		 HandCards tmp;
		 ////去除手牌中的金,之后进行判断

		 for (int i = 0; i < pHc.ChangeableCardsLen; i++)
		 {
			 if (pHc.ChangeableCards[i] == m_bReplaceJinCard)
				 continue;
			 m_GameLogic.AddCard(&tmp, pHc.ChangeableCards[i]);
		 }

		 if (IsDanYouJin(pHc, ChairID) && ChairID == m_bCurChairID)
		 {
			// log_info("Is7DuiDanYouJin ok");
			 m_bUserDanYouJinHuFlag[ChairID] = true;
		 }
		 else
		 {//开启单游金状态后，如果不满足单金胡，则关闭单游金。
			 m_bUserDanYouJinHuFlag[ChairID] = false;
		 }
		 //满足双金胡
		 if (Is7DuiDoubleYouJin(tmp, ChairID) && (ChairID == m_bCurChairID))
		 {
			// log_info("Is7DuiDoubleYouJin ok");
			 m_bUserDoubleYouJinHuFlag[ChairID] = true;
		 }
		 else
		 {
			 m_bUserDoubleYouJinHuFlag[ChairID] = false;
		 }
		 ////三游金
		 if (Is7DuiThreeYouJin(tmp, ChairID))
		 {
			 m_bUserOperateShuangYouJinZhaungThreeFlag[ChairID] = false;
			// log_info("Is7DuiThreeYouJin ok");
			 m_bUserDoubleYouJinHuFlag[ChairID] = false;
			 m_bUserThreeYouJinHuFlag[ChairID] = true;
		 }
		 else
		 {
			 m_bUserThreeYouJinHuFlag[ChairID] = false;
			

		 }
		 //7对的时候是否满足19胡
		 Is19Hu(ChairID, pHc, oFan, pattern);
		 //7对的时候h是否满足3金倒

		 if (m_GameLogic.FormatCards_Laizi(tmp.ChangeableCards, tmp.ChangeableCardsLen, m_bUserHasJinNum[ChairID]))/*递归，检查胡牌的形式*/
		 {
			 //满足双金胡
			 if (IsDoubleYouJin(pHc, ChairID) && (ChairID == m_bCurChairID)&& m_bUserDoubleYouJinHuFlag[ChairID] == false)
			 {
				 m_bUserDoubleYouJinHuFlag[ChairID] = true;
			 }
			 ////三游金
			 if (IsThreeYouJin(pHc, ChairID)&& m_bUserThreeYouJinHuFlag[ChairID] == false)
			 {
				 m_bUserOperateShuangYouJinZhaungThreeFlag[ChairID] = false;
				 m_bUserThreeYouJinHuFlag[ChairID] = true;
				 m_bUserDoubleYouJinHuFlag[ChairID] = false;
			 }
		 }
		 m_bUserOperateShuangYouJinZhaungThreeFlag[ChairID] = false;
		 return TYPE_HU;
	 }
	 if( Is19Hu( ChairID, pHc,  oFan,  pattern))
	 {
		 IsHuFlag = true;	
	 }
	 if (IsBuDa(pHc, oFan, pattern))
	 {
		 IsHuFlag = true;
	 }

	 /*格式化牌*/
	 HandCards tmp;
	 //去除手牌中的金,之后进行判断

	 for (int i = 0; i < pHc.ChangeableCardsLen; i++)
	 {
		 if (pHc.ChangeableCards[i] == m_bReplaceJinCard)
			 continue;

		 m_GameLogic.AddCard(&tmp, pHc.ChangeableCards[i]);
	 }

	 if (m_GameLogic.FormatCards_Laizi(tmp.ChangeableCards, tmp.ChangeableCardsLen, m_bUserHasJinNum[ChairID]))/*递归，检查胡牌的形式*/
	 {
		 //单金胡判断
		if (IsDanYouJin( pHc, ChairID)&& (ChairID == m_bCurChairID))
		{
			//log_info("IsDanYouJin ok");
			m_bUserDanYouJinHuFlag[ChairID] = true;
		}
		else
		{//开启单游金状态后，如果不满足单金胡，则关闭单游金。
			m_bUserDanYouJinHuFlag[ChairID] = false;
		}
		//满足双金胡
		if(IsDoubleYouJin(pHc, ChairID)&&(ChairID == m_bCurChairID))
		{
			//log_info("IsDoubleYouJin ok");
			m_bUserDoubleYouJinHuFlag[ChairID] = true;
		}
		else
		{
			m_bUserDoubleYouJinHuFlag[ChairID] = false;
		}
		////三游金
		if(IsThreeYouJin( pHc, ChairID))
		{
			m_bUserOperateShuangYouJinZhaungThreeFlag[ChairID] = false;
			m_bUserThreeYouJinHuFlag[ChairID] = true;
			m_bUserDoubleYouJinHuFlag[ChairID] = false;
		}
		else
		{
			m_bUserThreeYouJinHuFlag[ChairID] = false;
			m_bUserOperateShuangYouJinZhaungThreeFlag[ChairID] = false;

		}

		 return Check_Hu_Pattern(ChairID, tmp, oFan, pattern);/*计算加番*/
	 }else
	 {
		 m_bUserDanYouJinHuFlag[ChairID] = false;
		 m_bUserDoubleYouJinHuFlag[ChairID] = false;
		 m_bUserThreeYouJinHuFlag[ChairID] = false;
		 memset(&m_bSaveUserOutOnlyCard, 0, sizeof(m_bSaveUserOutOnlyCard));
	 }
	 if (IsHuFlag == true)
	 {
		 return TYPE_HU;
	 }
	 if (IsSanJinDao(ChairID, oFan)&& ChairID == m_bCurChairID)
	 {
		 if (m_bBanker== ChairID)
		 {
			 if (m_bGetCardCount[m_bBanker] >= 2)
			 {
				 return TYPE_HU;
			 }
		 }
		 else
		 {
			 return TYPE_HU;
		 }
		
	 }
	 return TYPE_NULL;
 }

 void Table_yunxiaomajiang::GameEnd()
 {
	 //对玩家的牌进行金与白板的替换，在后台处理中,始终使用白板作为金来操作.
	 //统计玩家手中金与白板个数的信息,在游戏结束时,对玩家的手牌进行,金与白板的替换.解决胡牌时与胡牌后,牌的信息不对应的问题.
	 ChangeHandCardBaiBanAndJin();
	 Table::GameEnd();
 }

 bool Table_yunxiaomajiang::CheckUserOutCard(_uint8 ChairID, _uint8 cbCard)
 {
	 if (m_bUerOutErrFlag[ChairID] == true)
	 {
		 proto::game::yunxiaomj::AckUserOutCardErr  Ack;
		 Ack.set_card(cbCard);
		 Ack.set_chairid(ChairID);
		 Ack.set_err_code(2);
		 unicast(SER_CMD_OUT_CARD_ERR, ChairID, Ack);
		 log_info("%s tid:%d uid:%d ChairID:%d Error CheckUserOutCard Out Two Card Err Ack = %s\n", __FUNCTION__,tid, GetPlayerUid(ChairID), ChairID, Ack.DebugString().c_str());
		 for (int i = 0; i < m_HandCard[ChairID].ChangeableCardsLen;i++)
		 {
			 log_info("%s tid:%d Error  m_HandCard[ChairID = %d].ChangeableCards[i = %d] = %x", __FUNCTION__, tid, ChairID,i, m_HandCard[ChairID].ChangeableCards[i]);
		 }
		 for (int i = 0; i < m_stUserOutCard[ChairID].Size();i++)
		 {
			 log_info("%s tid:%d  Error  m_stUserOutCard[ChairID = %d][i = %d] = %x", __FUNCTION__, tid, ChairID, i, m_stUserOutCard[ChairID][i]);

		 }
		 return false;
	 }

	 //对后台的操作是用白板作为金来使用,需要进行替换.客户端金与白板的不对等,会造成服务端的牌错误.
	 if (cbCard == m_bJinCard)
	 {
		 cbCard = m_bReplaceJinCard;
	 }
	 else if (cbCard == m_bReplaceJinCard)
	 {
		 cbCard = m_bJinCard;
	 }
	 if (Table::CheckUserOutCard(ChairID, cbCard))
	 {
		 return true;
	 }
	 else
	 {
		 proto::game::yunxiaomj::AckUserOutCardErr  Ack;
		 Ack.set_card(cbCard);
		 Ack.set_chairid(ChairID);
		 Ack.set_err_code(2);
		 unicast(SER_CMD_OUT_CARD_ERR, ChairID, Ack);
		 log_info("%s tid:%d uid:%d ChairID:%d Error CheckUserOutCard Out Two Card Err Ack = %s\n", __FUNCTION__, tid, GetPlayerUid(ChairID), ChairID, Ack.DebugString().c_str());
		 for (int i = 0; i < m_HandCard[ChairID].ChangeableCardsLen; i++)
		 {
			 log_info("%s tid:%d Error  m_HandCard[ChairID = %d].ChangeableCards[i = %d] = %x", __FUNCTION__, tid, ChairID, i, m_HandCard[ChairID].ChangeableCards[i]);
		 }
		 for (int i = 0; i < m_stUserOutCard[ChairID].Size(); i++)
		 {
			 log_info("%s tid:%d  Error  m_stUserOutCard[ChairID = %d][i = %d] = %x", __FUNCTION__, tid, ChairID, i, m_stUserOutCard[ChairID][i]);

		 }
		 return false;

	 }
 }

 void Table_yunxiaomajiang::OnUserOutCard(_uint8 ChairID, _uint8 bCard)
 {
	// m_bUserChiCardOutCardFlag[ChairID] = INVALID_MJ_CARD;
	 m_bUserOperateGuoZhangHuFlag[ChairID] = false;
	 m_bUserHuQingYiSeFlag[ChairID] = false;

	 memset(m_bUserCheckDoMultiOperateiOpTypeFlag, 0, sizeof(m_bUserCheckDoMultiOperateiOpTypeFlag));
	 m_bUerOutErrFlag[ChairID] = true;
	 if (m_conf_yunxiaomj.yunxiao_zidong_chupai_hupai_pass == true)
	 {
		 //出牌之后关闭所有的定时器
		 TIMER_STOP(m_GetCardOperateOutCard_Timer);
		 TIMER_STOP(m_GetCardOperateHu_Timer);
		 TIMER_STOP(m_YouJinOperateHuYouJin_Timer_0);
		 TIMER_STOP(m_YouJinOperateHuYouJin_Timer_1);
		 TIMER_STOP(m_ChiPengCardOperateOutCard_Timer);
		 TIMER_STOP(m_OutTimeStopOperateOutCard_Timer);

		 TIMER_STOP(m_OutCardOutTimeOperatePass_Timer_0);
		 TIMER_STOP(m_OutCardOutTimeOperatePass_Timer_1);
		 TIMER_STOP(m_OutCardOutTimeOperatePass_Timer_2);
		 TIMER_STOP(m_OutCardOutTimeOperatePass_Timer_3);

		 TIMER_STOP(m_OutCardOutTimeOperateHu_Timer_0);
		 TIMER_STOP(m_OutCardOutTimeOperateHu_Timer_1);
		 TIMER_STOP(m_OutCardOutTimeOperateHu_Timer_2);
		 TIMER_STOP(m_OutCardOutTimeOperateHu_Timer_3);
		 m_bOutTimeStopOperateOutCardFlag[ChairID] = false;

	 }

	 m_bSaveUserOutCard[ChairID] = bCard;
	 m_bIsAnGangHuFlag[ChairID] = false;//玩家杠后,如果出牌,则暗杠胡与明杠胡标记无效
	 m_bIsMingGangHuFlag[ChairID] = false;
	 //玩家如果打出的是金,则后台处理的是白板,如果玩家打出的是白板,后台的是金原先的值.
	 if (bCard == m_bJinCard)
	 {
		 bCard = m_bReplaceJinCard;
		 m_bUserHasJinNum[ChairID]--; //用户出金牌,金牌计数清理.
		// log_info("PLAYER: %d  has JinNum = %d", ChairID, m_bUserHasJinNum[ChairID]);
	 }
	 else if (bCard == m_bReplaceJinCard)
	 {
		 bCard = m_bJinCard;
	 }
	 m_bUserOutCardFlag[ChairID] = bCard;
	 ////玩家满足三游金状态胡
	// log_info("m_bUserThreeYouJinHuFlag[%d] = %d ,", ChairID,m_bUserThreeYouJinHuFlag[ChairID]);
	 if (m_bUserThreeYouJinHuFlag[ChairID] == true && bCard == m_bReplaceJinCard)
	 {
	//	 log_info("131 OnUserOutCard 3 jin ok");
		 m_bUserOperateThreeYouJinFlag[ChairID] = true;
		 m_bUserOperateDoubleYouJinFlag[ChairID] = false;
		 m_bUserDanYouJinHuFlag[ChairID] =false;
		 m_bUserDoubleYouJinHuFlag[ChairID] = false;
	 }
	 else
	 {
		 m_bUserOperateThreeYouJinFlag[ChairID] = false;
		 m_bUserThreeYouJinHuFlag[ChairID] = false;
	 }
	 ////满足双游金胡牌，打出一张金，则进入到双游金状态。
	 ////玩家进入到双游金状态后.

	 if (m_bUserDoubleYouJinHuFlag[ChairID] == true && bCard == m_bReplaceJinCard)
	 {
	//	 log_info("131 OnUserOutCard 2 jin ok");
		 m_bUserOperateDoubleYouJinFlag[ChairID] = true;   //双游金胡时出金标志
		 m_bUserOperateDanYouJinHu[ChairID] = false;//双游金开启，则单游金关闭。
		 m_bUserDanYouJinHuFlag[ChairID] = false;
	 }
	 else
	 {
		 m_bUserDoubleYouJinHuFlag[ChairID] = false;
		 m_bUserOperateDoubleYouJinFlag[ChairID] = false;   //双游金胡时出金标志
	 }
	 //单金胡时，选择PASS，并且打出那个单张牌则进入单游金状态。
	 bool ValueFlag = false;
	 for (int i = 0; i < m_bSaveUserOutOnlyCard[ChairID].ChangeableCardsLen;i++)
	 {
		 if (bCard == m_bSaveUserOutOnlyCard[ChairID].ChangeableCards[i])
		 {
			 ValueFlag = true;
			 break;
		 }
	 }
	 if ((m_bUserOperateDanJinPass[ChairID] == true) && ValueFlag== true &&(m_bUserDanYouJinHuFlag[ChairID]==true))
	 {//进入单游金状态后，如果玩家手牌不满足四句话+1金，则单游金状态关闭。	
		 m_bUserOperateDanYouJinHu[ChairID] = true;
	 }
	 else
	 {
		 m_bUserDanYouJinHuFlag[ChairID] = false;
		 m_bUserOperateDanJinPass[ChairID] = false;
		 m_bUserOperateDanYouJinHu[ChairID] = false;
	 }
 
	 // 删出牌时钟	
	 ev_timer_stop(zjh.loop, &m_Outcard_Timer);
	 log_info("%s tid:%d uid:%d chairid:%d outcard:0x%02x \n", __FUNCTION__, tid, GetPlayerUid(ChairID), ChairID, bCard);
	 m_bCurChairID = ChairID; 
	 m_bOutCardChairID = ChairID;
	 m_GameLogic.ChuPai(&m_HandCard[ChairID], bCard);

	 bCard = m_bSaveUserOutCard[ChairID];//删除后台的牌后,需要还原以前的牌,与客户端保持一致.
	 m_bCurCard = m_bSaveUserOutCard[ChairID]; //当前摸到或打出的牌
	 m_bCurOutCard = m_bSaveUserOutCard[ChairID];//当前打出的牌
	 m_bOverTimeState[ChairID] = 0;

	 m_stUserOutCard[ChairID].Add(bCard);

	 //广播玩家出牌	
	 proto::game::AckUserOutCard stUseroutcard;

	 Proto_Game_UserOutCard(ChairID, bCard, stUseroutcard);
	 m_bUserAutoOutCardFlag = false;//自动出牌判断

	 broadcast(SERVER_USER_OUTCARD, stUseroutcard);
	// log_info("%s tid:%d uid:%d chairid:%d Send OnUserOutCard stUseroutcard :%s \n", __FUNCTION__, tid, GetPlayerUid(ChairID), ChairID, stUseroutcard.DebugString().c_str());

	 save_user_out_card(ChairID, bCard);

 }

 //断线重连,需要恢复玩家手牌时,需要对金与白板进行交换.
 void Table_yunxiaomajiang::ChangeHandCardBaiBanAndJin()
 {
	// log_info("ChangeHandCardBaiBanAndJin start");
	 _uint8 bUserHasJinNum[GAME_PLAYER] = { 0 };
	 _uint8 bHasReplaceJinCardNum[GAME_PLAYER] = { 0 };
	 for (int i = 0; i < m_GAME_PLAYER; i++)
	 {	
		 for (int j = 0; j < m_HandCard[i].ChangeableCardsLen; j++)
		 {//统计金与白板的个数。
			 if (m_HandCard[i].ChangeableCards[j] == m_bJinCard)
			 {
				 bUserHasJinNum[i]++;
			 }
			 if (m_HandCard[i].ChangeableCards[j] == m_bReplaceJinCard)
			 {
				 bHasReplaceJinCardNum[i]++;
			 }
		 }
		 //log_info("ChaidID = %d  bUserHasJinNum = %d", i, bUserHasJinNum[i]);
		// log_info("ChaidID = %d  bHasReplaceJinCardNum = %d", i, bHasReplaceJinCardNum[i]);
	 }

	 for (int i = 0; i < m_GAME_PLAYER; i++)
	 {
		 int JinIndex = 0;
		 for (int j = 0; j < m_HandCard[i].ChangeableCardsLen; j++)
		 {
			 if (m_HandCard[i].ChangeableCards[j] == m_bJinCard)
			 {
				 if (JinIndex >= bUserHasJinNum[i])
				 {
					 break;
				 }
				// log_info("ChaidID = %d  bUserHasJinNum = %d", i, bUserHasJinNum[i]);
				 m_HandCard[i].ChangeableCards[j] = m_bReplaceJinCard;
				 JinIndex++;
			 }
		 }
	 }

	 for (int i = 0; i < m_GAME_PLAYER; i++)
	 {
		 int JinIndex = 0;
		 for (int j = 0; j < m_HandCard[i].ChangeableCardsLen; j++)
		 {
			 if (m_HandCard[i].ChangeableCards[j] == m_bReplaceJinCard)
			 {
				 if (JinIndex >= bHasReplaceJinCardNum[i])
				 {
					 break;
				 }
				 //log_info("ChaidID = %d  bHasReplaceJinCardNum = %d", i, bHasReplaceJinCardNum[i]);
				 m_HandCard[i].ChangeableCards[j] = m_bJinCard;
				 JinIndex++;
			 }
		 }
	 }

	 for (int i = 0; i < m_GAME_PLAYER; i++)
	 {
		 for (int j = 0; j < m_HandCard[i].FixedCardsLen; j++)
		 {
			 if ((m_HandCard[i].FixedCards[j].state == TYPE_LEFT_CHI) || (m_HandCard[i].FixedCards[j].state == TYPE_CENTER_CHI) || (m_HandCard[i].FixedCards[j].state == TYPE_RIGHT_CHI))
			 {
				 for (int m = 0; m < 3; m++)
				 {		
					 if (m_HandCard[i].FixedCards[j].c[m] == m_bReplaceJinCard)
					 {
						 m_HandCard[i].FixedCards[j].c[m] = m_bJinCard;
					 }
					 else if (m_HandCard[i].FixedCards[j].c[m] == m_bJinCard)
					 {
						 m_HandCard[i].FixedCards[j].c[m] = m_bReplaceJinCard;
					 }
				 }
			 }
	

		 }
	 }
	
	 for (int i = 0; i < m_GAME_PLAYER; i++)
	 {
		 for (int j = 0; j < m_HandCard[i].FixedCardsLen; j++)
		 {
			 if ((m_HandCard[i].FixedCards[j].state == TYPE_PENG) || (m_HandCard[i].FixedCards[j].state == TYPE_ZHIGANG) || (m_HandCard[i].FixedCards[j].state == TYPE_WANGANG) || (m_HandCard[i].FixedCards[j].state == TYPE_ANGANG))
			 {
				 if (m_HandCard[i].FixedCards[j].CardData == m_bJinCard)
				 {
					 m_HandCard[i].FixedCards[j].CardData = m_bReplaceJinCard;
				 }
				 else if (m_HandCard[i].FixedCards[j].CardData == m_bReplaceJinCard)
				 {
					 m_HandCard[i].FixedCards[j].CardData = m_bJinCard;
				 }
			 }
		 }
	 }
	// log_info("ChangeHandCardBaiBanAndJin end");
 }
 
 void Table_yunxiaomajiang::UserOperatePass(_uint8 ChairID, int iOpType, _uint8 bCard)
 {
	 if (m_conf_yunxiaomj.yunxiao_zidong_chupai_hupai_pass == true)
	 {
		 if (ChairID == m_bCurChairID && m_bUserOperate[ChairID] != TYPE_NULL &&m_bUserOperate[ChairID] != TYPE_YX_YOUJINHU&&m_bUserOperateQiangJinHuFlag[ChairID]==false)
		 {
			 const int tmp_Timer_stamp_0 = 13;
			 TIMER_AGAIN(m_OutTimeStopOperateOutCard_Timer, tmp_Timer_stamp_0);

			 const int tmp_Timer_stamp = 16;
			 TIMER_AGAIN(m_GetCardOperateOutCard_Timer, tmp_Timer_stamp);
			 set_out_timer(ChairID);	
		 }
	 }

	 m_bUserDoMultiOperateiOpTypeFlag[ChairID] = iOpType;
	 m_bUserDoMultiOperateiOpType[ChairID] = iOpType;
	 for (int i = 0; i < m_GAME_PLAYER;i++)
	 {
		 m_bUserDoMultiOperateiOpType[i] = m_bUserDoMultiOperateiOpTypeFlag[i];
	 }

	 m_bIsAnGangHuFlag[ChairID] = false;
	 m_bIsMingGangHuFlag[ChairID] = false;
	 int iOperateiOpTypeFlag = false;

	 if (m_bUserOperateChiPengGangDanYouJinstep[ChairID] == true && m_bUserOperateDanYouJinstep[ChairID] == false)
	 {

		 if (m_bUserOperate[ChairID] & TYPE_YX_YOUJINHU )
		 {
			 if (m_bUserDoMultiOperateiOpType[ChairID] == TYPE_PASS)
			 {
				 for (_uint8 c = NextChair(ChairID); c != ChairID; c = NextChair(c))
				 {
					 if (m_bUserDoMultiOperateiOpType[c] != TYPE_NULL)
					 {
						 iOperateiOpTypeFlag = true;
					 }
				 }
				 if (iOperateiOpTypeFlag == false)
				 {
					 m_bUserOperateDanYouJinHu[ChairID] = false;
					 m_bUserOperateChiPengGangDanYouJinstep[ChairID] = false;
					 memset(m_bUserDoMultiOperateiOpType, TYPE_NULL, sizeof(m_bUserDoMultiOperateiOpType));

				 }
			 }
		 }

	 }
	 if (m_bUserOperateChiPengGangDoubleYouJinstep[ChairID] == true&& m_bUserOperateDoubleYouJinStep[ChairID] == false)
	 {
		 if (m_bUserOperate[ChairID] & TYPE_YX_YOUJINHU)
		 {

			 if (m_bUserDoMultiOperateiOpType[ChairID] == TYPE_PASS)
			 {
				 for (_uint8 c = NextChair(ChairID); c != ChairID; c = NextChair(c))
				 {
					 if (m_bUserDoMultiOperateiOpType[c] != TYPE_NULL)
					 {
						 iOperateiOpTypeFlag = true;
					 }
				 }
				 if (iOperateiOpTypeFlag == false)
				 {
					 m_bUserOperateShuangYouJinZhaungThreeFlag[ChairID] = true;
					 m_bUserOperateDoubleYouJinFlag[ChairID] = false;
					 m_bUserOperateChiPengGangDoubleYouJinstep[ChairID] = false;
					 memset(m_bUserDoMultiOperateiOpType, TYPE_NULL, sizeof(m_bUserDoMultiOperateiOpType));

				 }
			 }
		 }
	 }
	 if (m_bUserOperateChiPengGangThreeYouJinstep[ChairID] == true&& m_bUserOperateThreeYouJinStep[ChairID] == false)
	 {

		 if (m_bUserOperate[ChairID] & TYPE_YX_YOUJINHU)
		 {
			 if (m_bUserDoMultiOperateiOpType[ChairID] == TYPE_PASS)
			 {
				 for (_uint8 c = NextChair(ChairID); c != ChairID; c = NextChair(c))
				 {

					 if (m_bUserDoMultiOperateiOpType[c] != TYPE_NULL)
					 {
						 iOperateiOpTypeFlag = true;
					 }
				 }
				 if (iOperateiOpTypeFlag == false)
				 {
					 m_bUserOperateChiPengGangThreeYouJinstep[ChairID] = false;
					 m_bUserOperateThreeYouJinFlag[ChairID] = false;
					 memset(m_bUserDoMultiOperateiOpType, TYPE_NULL, sizeof(m_bUserDoMultiOperateiOpType));

				 }
			 }
		 }
	 }

	 memset(m_bUserDoMultiOperateiOpTypeFlag, 0, sizeof(m_bUserDoMultiOperateiOpTypeFlag));

	 Table::UserOperatePass(ChairID, iOpType, bCard);
	 //玩家满足单金胡牌时，选择pass
	 if (m_bUserDanYouJinHuFlag[ChairID]==true)
	 {
		 m_bUserOperateDanJinPass[ChairID] = true;//单游金胡牌时,pass
	 }
	 else
	 {
		 m_bUserOperateDanJinPass[ChairID] = false;
	 }
	 //如果玩家单游金时，选择pass，则继续摸牌
	 if (m_bUserOperateDanYouJinHu[ChairID] == true && m_bUserOperateDanYouJinstep[ChairID] ==true)
	 {
		 m_bUserOperateDanYouJinHu[ChairID] = false;
		 m_bUserOperateDanYouJinstep[ChairID] = false;
		 m_bUserOperateDanJinPass[ChairID] = false;
		 OnUserGetCard(ChairID);
	 }
	 if (m_bUserOperateDoubleYouJinFlag[ChairID] == true && m_bUserOperateDoubleYouJinStep[ChairID] == true)
	 {
		m_bUserOperateShuangYouJinZhaungThreeFlag[ChairID]=true;
		 m_bUserOperateDoubleYouJinFlag[ChairID] = false;
		 m_bUserOperateDoubleYouJinStep[ChairID] = false;
		 OnUserGetCard(ChairID);
	 }
	 if (m_bUserOperateThreeYouJinFlag[ChairID] == true && m_bUserOperateThreeYouJinStep[ChairID] == true)
	 {
		 m_bUserOperateDoubleYouJinFlag[ChairID] = false;
		 m_bUserOperateThreeYouJinFlag[ChairID] = false;
		 m_bUserOperateThreeYouJinStep[ChairID] = false;
		 OnUserGetCard(ChairID);
	 }
	 if ((m_bUserOperateQiangJinHuFlag[ChairID] == true) && (m_bUserOperateQiangJinHuPass[ChairID] == false))
	 {
		 m_bUserOperateQiangJinHuFlag[ChairID] = false;
		 m_bUserOperateQiangJinHuPass[ChairID] = true;
		 OnUserGetCard(ChairID);
	 }

 }

 void Table_yunxiaomajiang::UserOperateChi(_uint8 ChairID, int iOpType, _uint8 bCard)
 {
	 m_bUserHuQingYiSeFlag[ChairID] = false;
	 m_bUserOperateGuoZhangHuFlag[ChairID] = false;
	 m_bUserOutCardFlag[ChairID] = INVALID_MJ_CARD;
	 m_bUerOutErrFlag[ChairID] = false;
	 m_bUserOperateChiPengGangDanYouJinstep[ChairID] = TYPE_NULL;
	 m_bUserOperateChiPengGangDoubleYouJinstep[ChairID] = TYPE_NULL;
	 m_bUserOperateChiPengGangThreeYouJinstep[ChairID] = TYPE_NULL;

	 m_bIsAnGangHuFlag[ChairID] = false;
	 m_bIsMingGangHuFlag[ChairID] = false;
	 m_bUserOperateDanYouJinHu[ChairID] = false;

	 memset(&m_bUserOperateGangCardInfo[ChairID], 0, sizeof(m_bUserOperateGangCardInfo[ChairID]));

	 //进行金与白板的互换.
	 m_bSaveUserOutCard[ChairID] = bCard;
	 if (bCard == m_bReplaceJinCard)
	 {
		 bCard = m_bJinCard;
	 }

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
			 log_info("user_operate_chi ChairID:%d operate:0x%x，card: 0x%02x,but handcard has no cur operate \n", ChairID, iOpType, bCard);
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
			 log_info("user_operate_chi ChairID:%d operate:0x%x，card: 0x%02x,but handcard has no cur operate \n", ChairID, iOpType, bCard);
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
			 log_info("user_operate_chi ChairID:%d operate:0x%x，card: 0x%02x,but handcard has no cur operate \n", ChairID, iOpType, bCard);
			 return;
		 }
		 m_GameLogic.ExecuteRightChi(&m_HandCard[ChairID], m_bOutCardChairID, bCard);
	 }
	 break;
	 default:
		 break;
	 }
	 //进行金与白板的互换.
	 if (m_bSaveUserOutCard[ChairID] == m_bReplaceJinCard)
	 {
		 bCard = m_bReplaceJinCard;
	 }

	 m_bGetCardCount[ChairID]++;
	 m_stUserOutCard[m_bOutCardChairID].RemoveLast(bCard);
	 //广播操作结果的消息
	 BroadcastOperateResult(ChairID, iOpType, bCard);

	 SendTingInfo(ChairID);

	 int score[GAME_PLAYER] = { 0 };
	 save_video_operate(iOpType, ChairID, m_bOutCardChairID, bCard, score);

	 UserAfterOperate(ChairID, iOpType, bCard);

	 
	 //检测手牌是否满足胡，进入游金状态。
	 HandCards tmp;
	 tmp.FixedCardsLen = 0;
	 for (int i = 0; i < m_HandCard[ChairID].ChangeableCardsLen; i++)
	 {
		 m_GameLogic.AddCard(&tmp, m_HandCard[ChairID].ChangeableCards[i]);
	 }
	 MJ_fan_buffer oFan;
	 if (Check_Hu(ChairID, tmp, oFan))
	 {
		 if (m_bUserDanYouJinHuFlag[ChairID]== true)
		 {
			 m_bUserOperateDanJinPass[ChairID] = true;
		 }
	 }
	 if (m_conf_yunxiaomj.yunxiao_zidong_chupai_hupai_pass == true)
	 {
		 const int tmp_Timer_stamp_0 = 13;
		 TIMER_AGAIN(m_OutTimeStopOperateOutCard_Timer, tmp_Timer_stamp_0);
		 const int tmp_Timer_stamp = 16;
		 TIMER_AGAIN(m_ChiPengCardOperateOutCard_Timer, tmp_Timer_stamp);
		 set_operator_timer(ChairID);
	 }

 }


 void Table_yunxiaomajiang::UserOperatePeng(_uint8 ChairID, _uint8 bCard)
 {
	 m_bUserHuQingYiSeFlag[ChairID] = false;
	 m_bUserOperateGuoZhangHuFlag[ChairID] = false;
	 m_bUserOutCardFlag[ChairID] = INVALID_MJ_CARD;

	 m_bUerOutErrFlag[ChairID] = false;
	 m_bUserOperateChiPengGangDanYouJinstep[ChairID] = TYPE_NULL;
	 m_bUserOperateChiPengGangDoubleYouJinstep[ChairID] = TYPE_NULL;
	 m_bUserOperateChiPengGangThreeYouJinstep[ChairID] = TYPE_NULL;

	// m_bUserGetJinFlag[ChairID] = false;
	 m_bIsAnGangHuFlag[ChairID]		= false;
	 m_bIsMingGangHuFlag[ChairID]	= false;
	 m_bUserOperateDanYouJinHu[ChairID] = false;

	 memset(&m_bUserOperateGangCardInfo[ChairID], 0, sizeof(m_bUserOperateGangCardInfo[ChairID]));

	 log_info("%s uid:%d ChairID:%d, bCard:0x%02x \n", __FUNCTION__, GetPlayerUid(ChairID), ChairID, bCard);
	 memset(m_bAddFanFlag, 0, sizeof(m_bAddFanFlag));
	 memset(m_bUserOperate, 0, sizeof(m_bUserOperate));

	 BroadcastOperateResult(ChairID, TYPE_PENG, bCard);

	 int fixedlen = m_HandCard[ChairID].FixedCardsLen;
	 //if (fixedlen >= 4)  //17张牌可以碰5下
	 if (fixedlen >= HAND_CARD_KAN_MAX)
	 {
		 log_error("玩家组合牌长度:%d大于正常的长度 \n", fixedlen);
		 return;
	 }
	 //进行金与白板的互换.
	 m_bSaveUserOutCard[ChairID] = bCard;
	 if (bCard == m_bReplaceJinCard)
	 {
		 bCard = m_bJinCard;
	 }
	 m_GameLogic.ExecutePeng(&m_HandCard[ChairID], m_bOutCardChairID, bCard);
	 m_bGetCardCount[ChairID]++;
	// bCard = m_bSaveUserOutCard[ChairID];
	 if (m_bSaveUserOutCard[ChairID] == m_bReplaceJinCard)
	 {
		 bCard = m_bReplaceJinCard;
	 }
	 //从打出牌删掉这张牌	
	 m_stUserOutCard[m_bOutCardChairID].RemoveLast(bCard);

	 //m_bOutCardCount[CardType(bCard)][CardNum(bCard)] += 2;
	 //TODO 检查是否听牌
	 SendTingInfo(ChairID);

	
	 UserAfterOperate(ChairID, TYPE_PENG, bCard);

	 int score[GAME_PLAYER] = { 0 };
	 save_video_operate(TYPE_PENG, ChairID, m_bOutCardChairID, bCard, score);
	 HandCards tmp;
	 tmp.FixedCardsLen = 0;
	 for (int i = 0; i < m_HandCard[ChairID].ChangeableCardsLen; i++)
	 {
		 m_GameLogic.AddCard(&tmp, m_HandCard[ChairID].ChangeableCards[i]);
	 }
	 MJ_fan_buffer oFan;
	 if (Check_Hu(ChairID, tmp, oFan))
	 {
		 if (m_bUserDanYouJinHuFlag[ChairID] == true)
		 {
			 m_bUserOperateDanJinPass[ChairID] = true;
		 }
	 }
	// log_info("m_conf_yunxiaomj.yunxiao_zidong_chupai_hupai_pass  = %d", m_conf_yunxiaomj.yunxiao_zidong_chupai_hupai_pass);
	 if (m_conf_yunxiaomj.yunxiao_zidong_chupai_hupai_pass == true)
	 {
		 const int tmp_Timer_stamp_0 = 13;
		 TIMER_AGAIN(m_OutTimeStopOperateOutCard_Timer, tmp_Timer_stamp_0);
	 
		 set_operator_timer(ChairID);
		 const int tmp_Timer_stamp = 16;
		 TIMER_AGAIN(m_ChiPengCardOperateOutCard_Timer, tmp_Timer_stamp);
		
	 }

 }

 void Table_yunxiaomajiang::UserOperateGang(_uint8 ChairID, int iOpType, _uint8 bCard)
 {
	 //m_bUserChiCardOutCardFlag[ChairID] = INVALID_MJ_CARD;
	 m_bUserOutCardFlag[ChairID] = INVALID_MJ_CARD;

	 m_bUserOperateChiPengGangDanYouJinstep[ChairID] = TYPE_NULL;
	 m_bUserOperateChiPengGangDoubleYouJinstep[ChairID] = TYPE_NULL;
	 m_bUserOperateChiPengGangThreeYouJinstep[ChairID] = TYPE_NULL;

	 m_bUserOperateDanYouJinHu[ChairID] = false;
	 m_bUserOperateDoubleYouJinFlag[ChairID] = false;
	 m_bUserOperateThreeYouJinFlag[ChairID] = false;
	 m_bUserOperateGuoZhangHuFlag[ChairID] = false;

	 m_bSaveUserOutCard[ChairID] = bCard;
	 m_bUserOperateGangNum += 1;
	 memset(&m_bUserOperateGangCardInfo[ChairID], 0, sizeof(m_bUserOperateGangCardInfo[ChairID]));

	 if (iOpType == TYPE_ANGANG)
	 {
		 m_bIsAnGangHuFlag[ChairID] = true;
		 m_bIsMingGangHuFlag[ChairID] = false;
	 }
	 if (iOpType == TYPE_ZHIGANG || iOpType == TYPE_WANGANG)
	 {
		 m_bIsMingGangHuFlag[ChairID] = true;
		 m_bIsAnGangHuFlag[ChairID] = false;
	 }
	 memset(m_bAddFanFlag, 0, sizeof(m_bAddFanFlag));
	 memset(m_bUserOperate, 0, sizeof(m_bUserOperate));
	 //进行金与白板的互换.

	 if (bCard == m_bReplaceJinCard)
	 {
		 bCard = m_bJinCard;
	 }
	 log_info("%s uid:%d ChairID:%d, iOpType:0x%x, bCard:0x%02x \n", __FUNCTION__, GetPlayerUid(ChairID), ChairID, iOpType, bCard);
	 int iGangScore[GAME_PLAYER] = { 0 };
	 _uint8 bTempChairID = INVALID_CHAIRID;
	 m_bAddFanFlag[ChairID] = true;
	 switch (iOpType)
	 {
	 case TYPE_ANGANG:
	 {
		// log_info("9911TYPE_ANGANG ChairID = %d m_bCurChairID = %d m_bOutCardChairID = %d", ChairID, m_bCurChairID, m_bOutCardChairID);
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
		// log_info("9911TYPE_WANGANG ChairID = %d m_bCurChairID = %d m_bOutCardChairID = %d", ChairID, m_bCurChairID, m_bOutCardChairID);

		 for (int i = 0; i < HAND_CARD_KAN_MAX; i++)
		 {
			 if (bCard == m_HandCard[ChairID].FixedCards[i].CardData)
			 {
				 _uint8 tmpChairID = m_HandCard[ChairID].FixedCards[i].chairID;
				 iGangScore[ChairID] += m_BaseScore * m_conf.m_WanGangPoint;
				 iGangScore[tmpChairID] -= m_BaseScore * m_conf.m_WanGangPoint;
				 break;
			 }
		 }
		 m_stGang[ChairID].Gang.Add(tagGangScore(bCard, ChairID, TYPE_WANGANG));
		 bTempChairID = ChairID;
	 }
	 break;
	 case TYPE_ZHIGANG:
	 {
		 if (ChairID == m_bCurChairID)
		 {
			 for (_uint8 c = NextChair(ChairID); c != ChairID; c = NextChair(c))
			 {
				 iGangScore[ChairID] += m_BaseScore * m_conf.m_AnGangPoint;
				 iGangScore[c] -= m_BaseScore * m_conf.m_AnGangPoint;
			 }
			 m_stGang[ChairID].Gang.Add(tagGangScore(bCard, ChairID, TYPE_ZHIGANG));
			 bTempChairID = ChairID;
		 }
		 else
		 {
			// log_info("9911TYPE_ZHIGANG ChairID = %d m_bCurChairID = %d m_bOutCardChairID = %d", ChairID, m_bCurChairID, m_bOutCardChairID);

			 iGangScore[ChairID] += m_BaseScore * m_conf.m_ZhiGangPoint;
			 iGangScore[m_bOutCardChairID] -= m_BaseScore *  m_conf.m_ZhiGangPoint;

			 m_stGang[ChairID].Gang.Add(tagGangScore(bCard, m_bOutCardChairID, TYPE_ZHIGANG));

			 bTempChairID = m_bOutCardChairID;

			 m_bGetCardCount[ChairID]++;
			 //从打出牌删掉这张牌
			 if (m_bSaveUserOutCard[ChairID] == m_bReplaceJinCard)
			 {
				 bCard = m_bReplaceJinCard;
			 }

			 m_stUserOutCard[m_bOutCardChairID].RemoveLast(bCard);

			 if (m_bSaveUserOutCard[ChairID] == m_bReplaceJinCard)
			 {
				 bCard = m_bJinCard;
			 }
		 }
	 }
	 break;
	 default:
		 log_error("UserOperateGang, wrong optype:%d \n", iOpType);
		 break;
	 }

	 ExecuteGang(ChairID,&m_HandCard[ChairID], bTempChairID, bCard, iOpType);
	 //if (m_bSaveUserOutCard[ChairID] == m_bReplaceJinCard)//广播用户杠牌
	 //{
		// bCard = m_bReplaceJinCard;
	 //}

	 Calc_Score(m_stGang, iGangScore);
	 bCard = m_bSaveUserOutCard[ChairID];
	 proto::game::AckBcOpResult stOpresult;
	 int SaveCard = bCard;
	 if (iOpType == TYPE_ANGANG)
	 {
		
		 for (int i = 0; i < m_GAME_PLAYER; i++)
		 {
			 if (i != ChairID)
			 {
				 bCard = 0x00;
			 }
			 else
			 {
				 bCard = SaveCard;
			 }
			// log_info("unicast ChairID =%d bCard = %x iOpType = %x ", i, bCard, iOpType);
			 Proto_Game_OperateGang(ChairID, bCard, iOpType, iGangScore, stOpresult);
			 unicast(SERVER_OPERATE_RESULT, i, stOpresult);
		 }
	 }
	 else
	 {
		// log_info("broadcast mChairID =%d bCard = %x iOpType = %x ", ChairID, bCard, iOpType);
		 Proto_Game_OperateGang(ChairID, bCard, iOpType, iGangScore, stOpresult);
		 if (ChairID == m_bCurChairID)
		 {
			 stOpresult.set_minggangflag(1);
		 } 
		 else
		 {
			 stOpresult.set_minggangflag(0);
		 }
	
		 broadcast(SERVER_OPERATE_RESULT, stOpresult);
	 }

	 m_bUserOperate[ChairID] = TYPE_NULL;
	 stOpresult.set_card(SaveCard);
	 SaveVideoData(SERVER_OPERATE_RESULT, stOpresult);

 }

 void Table_yunxiaomajiang::GameEndReset()
 {
	 Table::GameEndReset();
	 //清理数据
	 m_bJinCard = INVALID_MJ_CARD;	 //金牌标志
	 memset(&m_bUserHasJinNum, 0, sizeof(m_bUserHasJinNum));//玩家拥有金牌的数量
	 m_bReplaceJinCard = 0x43; //使用白板代替金原来的牌
	 memset(&m_bHasReplaceJinCardNum, 0, sizeof(m_bHasReplaceJinCardNum));//游戏开始白板的数量
	 memset(&m_bSaveZiMoCard, INVALID_MJ_CARD, sizeof(m_bSaveZiMoCard));
	 memset(&m_bSaveUserOutCard, INVALID_MJ_CARD, sizeof(m_bSaveUserOutCard));
	 memset(&m_bIsAnGangHuFlag, 0, sizeof(m_bIsAnGangHuFlag));
	 memset(&m_bIsMingGangHuFlag, 0, sizeof(m_bIsMingGangHuFlag));
	 memset(&m_bIsGuoZhangHuFlag, 0, sizeof(m_bIsGuoZhangHuFlag));//满足过张胡条件。	
	 memset(&m_bUserOperateGuoZhangHuFlag, 0, sizeof(m_bUserOperateGuoZhangHuFlag));//过张胡
	// memset(&m_bUserGetJinFlag, 0, sizeof(m_bUserGetJinFlag));//抢金胡,得到金标志
	 memset(&m_bUserOperateQiangJinHuFlag, 0, sizeof(m_bUserOperateQiangJinHuFlag));//抢金胡,得到金标志
	 m_ft2fn = &g_yunxiaomajiang_ft2fn;
	 //单游金
	 memset(&m_bUserDanYouJinHuFlag, 0, sizeof(m_bUserDanYouJinHuFlag)); //满足单游金胡牌
	 memset(&m_bUserOperateDanJinPass, 0, sizeof(m_bUserOperateDanJinPass));//单游金胡牌时,pass
	 memset(&m_bUserOperateDanYouJinHu, 0, sizeof(m_bUserOperateDanYouJinHu));//满足游，可以胡游
	 memset(&m_bUserOperateDanYouJinstep, 0, sizeof(m_bUserOperateDanYouJinstep));//抢金胡,得到金标志
	 memset(&m_bSaveUserOutOnlyCard, 0, sizeof(m_bSaveUserOutOnlyCard));//单游胡时用户可以打出的牌使进入单游金状态。
																		//双游金
	 memset(&m_bUserDoubleYouJinHuFlag, 0, sizeof(m_bUserDoubleYouJinHuFlag));//抢金胡,得到金标志
	 memset(&m_bUserOperateDoubleYouJinFlag, 0, sizeof(m_bUserOperateDoubleYouJinFlag));//单游金运行标志
	 memset(&m_bUserOperateDoubleYouJinStep, 0, sizeof(m_bUserOperateDoubleYouJinStep));//单游胡时用户可以打出的牌使进入单游金状态。
																						//三游金
	 memset(&m_bUserThreeYouJinHuFlag, 0, sizeof(m_bUserThreeYouJinHuFlag));//抢金胡,得到金标志
	 memset(&m_bUserOperateThreeYouJinFlag, 0, sizeof(m_bUserOperateThreeYouJinFlag));//单游金运行标志
	 memset(&m_bUserOperateThreeYouJinStep, 0, sizeof(m_bUserOperateThreeYouJinStep));//单游胡时用户可以打出的牌使进入单游金状态。
	 memset(&m_bUserOperateQiangJinHuPass, 0, sizeof(m_bUserOperateQiangJinHuPass));//单游胡时用户可以打出的牌使进入单游金状态。
	 memset(&m_bUerOutErrFlag, 0, sizeof(m_bUerOutErrFlag));
	 memset(&m_bOutTimeStopOperateOutCardFlag, 0, sizeof(m_bOutTimeStopOperateOutCardFlag));
	 memset(&m_bUserOutCardFlag, INVALID_MJ_CARD, sizeof(m_bUserOutCardFlag));
	 memset(m_bUserCheckDoMultiOperateiOpTypeFlag, 0, sizeof(m_bUserCheckDoMultiOperateiOpTypeFlag));
	 memset(m_bUserHuTypeoFan, 0, sizeof(m_bUserHuTypeoFan));
	 memset(m_bUserHuQingYiSeFlag, 0, sizeof(m_bUserHuQingYiSeFlag));
	 memset(m_bUserOperateShuangYouJinZhaungThreeFlag, 0, sizeof(m_bUserOperateShuangYouJinZhaungThreeFlag));

	 m_bUserOperateGangNum = 0;
	 m_bUserAutoOutCardFlag = false;
	 if (m_QingYiSeDianPaoBaoPei == true)
	 {
		 m_QingYiSeDianPaoBaoPei = false;
		 m_conf_yunxiaomj.yun_xiao_dian_pao_bao_pei = false;
	 }
	 memset(&m_bUserOperateGangCardInfo, 0, sizeof(m_bUserOperateGangCardInfo));
	 memset(&m_bUserOperateChiPengGangDanYouJinstep, 0, sizeof(m_bUserOperateChiPengGangDanYouJinstep));
	 memset(&m_bUserOperateChiPengGangDoubleYouJinstep, 0, sizeof(m_bUserOperateChiPengGangDoubleYouJinstep));
	 memset(&m_bUserOperateChiPengGangThreeYouJinstep, 0, sizeof(m_bUserOperateChiPengGangThreeYouJinstep));

	 memset(&m_bUserDoMultiOperateiOpType, 0, sizeof(m_bUserDoMultiOperateiOpType));

	 if (m_conf_yunxiaomj.yunxiao_zidong_chupai_hupai_pass == true)
	 {

		 TIMER_STOP(m_GetCardOperateOutCard_Timer);
		 TIMER_STOP(m_GetCardOperateHu_Timer);
		 TIMER_STOP(m_YouJinOperateHuYouJin_Timer_0);
		 TIMER_STOP(m_YouJinOperateHuYouJin_Timer_1);
		 TIMER_STOP(m_ChiPengCardOperateOutCard_Timer);
		 TIMER_STOP(m_OutTimeStopOperateOutCard_Timer);

		 TIMER_STOP(m_OutCardOutTimeOperatePass_Timer_0);
		 TIMER_STOP(m_OutCardOutTimeOperatePass_Timer_1);
		 TIMER_STOP(m_OutCardOutTimeOperatePass_Timer_2);
		 TIMER_STOP(m_OutCardOutTimeOperatePass_Timer_3);

		 TIMER_STOP(m_OutCardOutTimeOperateHu_Timer_0);
		 TIMER_STOP(m_OutCardOutTimeOperateHu_Timer_1);
		 TIMER_STOP(m_OutCardOutTimeOperateHu_Timer_2);
		 TIMER_STOP(m_OutCardOutTimeOperateHu_Timer_3);

	 }

 }

 int Table_yunxiaomajiang::OutCardEstimate(_uint8 ChairID, _uint8 bCard, MJ_major& major, int& fan_num)
 {
	 if (bCard == m_bJinCard)
	 {
		 return TYPE_NULL;
	 }

	 if (bCard == m_bReplaceJinCard) //进行替换
	 {
		 bCard = m_bJinCard;
	 }
	// assert(major.m_oFan.Size() == 0);

	 int iOpType = TYPE_NULL;

	 HandCards* pHc = &m_HandCard[ChairID];

	 iOpType |= GetChi(ChairID, bCard, major.m_Chi);

	 if (m_bCurChairID != PreChair(ChairID))
	 {//只能下家吃上家的牌
		 iOpType &= ~TYPE_CHI;
	 }

	 iOpType |= GetPengGang(ChairID, bCard, major.m_GetPengGang);
	 //log_info("GetPengGang Info major.m_GetPengGang.Size() = %d", major.m_GetPengGang.Size());
	 //for (int i = 0; i < major.m_GetPengGang.Size();i++)
	 //{
		// log_info("major.m_GetPengGang[%d].cbCard = %x  major.m_GetPengGang[%d].nState = %x", i, major.m_GetPengGang[i].cbCard, i, major.m_GetPengGang[i].nState);
		// major.m_GetPengGang[i].cbCard;
	 //}
	// assert(pHc->ChangeableCardsLen < HAND_CARD_SIZE_MAX);
	 //玩家在单游金的状态下，无法点炮胡。
	 //如果玩家中有处于双游金状态的玩家，则所有玩家都不能点炮
	
	 for (int i = 0; i < m_GAME_PLAYER;i++)
	 {
		 if ((m_bUserOperateDoubleYouJinFlag[i] ==true)|| m_bUserOperateThreeYouJinFlag[i] == true)
		 {
			 m_conf.m_bZimoHu = true;
			 break;
		 }
		 else
		 {
			 m_conf.m_bZimoHu = false;
		 }
	 }
	// log_info(" m_bUserDanYouJinState[%d] = %d", ChairID, m_bUserDanYouJinState[ChairID]);
	// log_info(" m_conf.m_bZimoHu = %d", m_conf.m_bZimoHu);
	 if (m_conf_yunxiaomj.yunxiao_twopeople_zimohu_flag == true)
	 {
		 m_conf.m_bZimoHu = true;
	 }

	 if ((!m_conf.m_bZimoHu )&& m_bUserOperateDanYouJinHu[ChairID]!=true)
	 {		
		 m_GameLogic.AddCard(pHc, bCard);
		 iOpType |= Check_Hu(ChairID, *pHc, major.m_oFan);

		 if (iOpType & TYPE_HU)
		 {//如果玩家A放炮时，玩家B选择不胡，那在B玩家吃 / 碰 / 杠 / 摸牌之前都不能再胡点炮
			 m_bIsGuoZhangHuFlag[ChairID] = true;
		 }
		 

		 if (m_bUserOperateGuoZhangHuFlag[ChairID] == true)	 //过张胡
		 {
			 if (IsQingYiSe(&m_HandCard[ChairID], ChairID)) 
			 {
				 if (m_bUserHuQingYiSeFlag[ChairID] == true)
				 {
					 iOpType &= (~TYPE_HU);
				 }
			 }
			 else
			 {
				 iOpType &= (~TYPE_HU);
			 }
			
		 }

		 if (iOpType & TYPE_HU)
		 {
			 if (IsQingYiSe(&m_HandCard[ChairID], ChairID)) //清一色
			 {//清一色胡牌时，炮胡或自摸或游金都算双游金
				 m_bUserHuQingYiSeFlag[ChairID] = true;
			 }
		 }

		 if (iOpType & TYPE_HU)
		 {
			 if (m_bUserOutCardFlag[ChairID] == bCard)
			 {
				 iOpType &= (~TYPE_HU);
			 }
		 }
		
		 if (iOpType & TYPE_HU)
		 {
			 //如果玩家A放炮时，玩家B选择不胡，那在B玩家吃 / 碰 / 杠 / 摸牌之前都不能再胡点炮
			// m_bIsGuoZhangHuFlag[ChairID] = true;
			 //云霄麻将三金倒胡牌必须摸牌之后才能胡，吃碰杠都不行
			 if (major.m_oFan.Find(MJ_FAN_YUNXIAO_SANJINDAO))
			 {
				 major.m_oFan.Clear();
				 iOpType &= (~TYPE_HU);
			 }
		 }

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
			 /*如果为第一次摸牌,并且胡牌则为地胡*/
			 if (!major.m_oFan.Find(MJ_FAN_YUNXIAO_SANJINDAO))
			 {
				 UserFirstOutCard(ChairID, iOpType, major.m_oFan);
			 }
			 if (major.m_oFan.Find(MJ_FAN_TYPE_DIHU) && major.m_oFan.Find(MJ_FAN_TYPE_QIANGGANGHU) && m_bGetCardCount[m_bBanker] == 1 && m_bCurChairID != m_bBanker)
			 {//庄家第一次出牌出牌时，如果不是庄家明杠，则不能添加地胡类型。
				 major.m_oFan.Remove(MJ_FAN_TYPE_DIHU);
			 }
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
			 if (IsQingYiSe(&m_HandCard[ChairID], ChairID)) //清一色
			 {//清一色胡牌时，炮胡或自摸或游金都算双游金
				// major.m_oFan.Add(MJ_FAN_YUNXIAO_DOUBLEYOUJIN);
				 major.m_oFan.Add(MJ_FAN_TYPE_QINGYISE);
				 //如果玩家胡牌清一色，则开启点炮包赔
			 }
			 else
			 {
				 if (m_QingYiSeDianPaoBaoPei == true)
				 {
					 m_conf_yunxiaomj.yun_xiao_dian_pao_bao_pei = false;
					 m_QingYiSeDianPaoBaoPei = false;
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
			 if (IsQingYiSe(&m_HandCard[ChairID], ChairID)) //清一色
			 {//清一色胡牌时，炮胡或自摸或游金都算双游金
			  // major.m_oFan.Add(MJ_FAN_YUNXIAO_DOUBLEYOUJIN);
				 major.m_oFan.Add(MJ_FAN_TYPE_QINGYISE);
				 //如果玩家胡牌清一色，则开启点炮包赔
			 }
		 }
		 if (iOpType & TYPE_HU)
		 {
			 UserQiangGangHu(ChairID, bCard, major.m_oFan);
			 fan_num = Calc_Fan(ChairID, major.m_oFan);
		 }
		 m_GameLogic.ChuPai(pHc, bCard);
	 }
	 
	 if ((iOpType & TYPE_HU) && m_bUserHasJinNum[ChairID] == 3&& !major.m_oFan.Find(MJ_FAN_TYPE_QIANGGANGHU)&& !major.m_oFan.Find(MJ_FAN_TYPE_DIHU))
	 {//如果玩家手牌中有三张金，则不能胡点炮。
		 iOpType &= (~TYPE_HU);
	 }
	 
	 if (m_bUserOperateThreeYouJinFlag[ChairID] == true && iOpType != TYPE_NULL && m_bCurChairID == PreChair(ChairID))
	 {
		 m_bUserOperateChiPengGangThreeYouJinstep[ChairID] = true;
		 iOpType |= TYPE_YX_YOUJINHU;
		// const int tmp_Timer_stamp = 15;
		// TIMER_AGAIN(m_YouJinOperateHuYouJin_Timer_1, tmp_Timer_stamp);
	 }
	 if (m_bUserOperateDoubleYouJinFlag[ChairID] == true && iOpType != TYPE_NULL&& m_bCurChairID == PreChair(ChairID))
	 {
		 m_bUserOperateChiPengGangDoubleYouJinstep[ChairID] = true;
		 iOpType |= TYPE_YX_YOUJINHU;
		// const int tmp_Timer_stamp = 15;
		// TIMER_AGAIN(m_YouJinOperateHuYouJin_Timer_1, tmp_Timer_stamp);
	 }
	 //满足单游金状态,，并且可以游，则向客户端发送单游金
	 if (m_bUserOperateDanYouJinHu[ChairID] == true && iOpType != TYPE_NULL&& m_bCurChairID == PreChair(ChairID))
	 {
		 m_bUserOperateChiPengGangDanYouJinstep[ChairID] = true;
		 iOpType |= TYPE_YX_YOUJINHU;
		// const int tmp_Timer_stamp = 15;
		// TIMER_AGAIN(m_YouJinOperateHuYouJin_Timer_1, tmp_Timer_stamp);
	 }

	 //设置玩家超时的定时器
	if (m_conf_yunxiaomj.yunxiao_zidong_chupai_hupai_pass==true)
	{
		switch (ChairID)
		{
		case 0:
			if (iOpType & TYPE_YX_YOUJINHU)
			{
				const int tmp_Timer_stamp = 15;
				TIMER_AGAIN(m_YouJinOperateHuYouJin_Timer_1, tmp_Timer_stamp);

			}
			else if (iOpType & TYPE_HU)
			{
				const int tmp_Timer_stamp = 15;
				TIMER_AGAIN(m_OutCardOutTimeOperateHu_Timer_0, tmp_Timer_stamp);
			}
			else if (iOpType != TYPE_NULL)
			{
				const int tmp_Timer_stamp = 15;
				TIMER_AGAIN(m_OutCardOutTimeOperatePass_Timer_0, tmp_Timer_stamp);
			}
			break;
		case 1:
			if (iOpType & TYPE_YX_YOUJINHU)
			{
				const int tmp_Timer_stamp = 15;
				TIMER_AGAIN(m_YouJinOperateHuYouJin_Timer_1, tmp_Timer_stamp);

			}
			else if (iOpType & TYPE_HU)
			{
				const int tmp_Timer_stamp = 15;
				TIMER_AGAIN(m_OutCardOutTimeOperateHu_Timer_1, tmp_Timer_stamp);
			}
			else if (iOpType != TYPE_NULL)
			{
				const int tmp_Timer_stamp = 15;
				TIMER_AGAIN(m_OutCardOutTimeOperatePass_Timer_1, tmp_Timer_stamp);
			}
			break;
		case 2:
			if (iOpType & TYPE_YX_YOUJINHU)
			{
				const int tmp_Timer_stamp = 15;
				TIMER_AGAIN(m_YouJinOperateHuYouJin_Timer_1, tmp_Timer_stamp);

			}
			else if (iOpType & TYPE_HU)
			{
				const int tmp_Timer_stamp = 15;
				TIMER_AGAIN(m_OutCardOutTimeOperateHu_Timer_2, tmp_Timer_stamp);
			}
			else if (iOpType != TYPE_NULL)
			{
				const int tmp_Timer_stamp = 15;
				TIMER_AGAIN(m_OutCardOutTimeOperatePass_Timer_2, tmp_Timer_stamp);
			}
			break;
		case 3:
			if (iOpType & TYPE_YX_YOUJINHU)
			{
				const int tmp_Timer_stamp = 15;
				TIMER_AGAIN(m_YouJinOperateHuYouJin_Timer_1, tmp_Timer_stamp);
			}
			else if (iOpType & TYPE_HU)
			{
				const int tmp_Timer_stamp = 15;
				TIMER_AGAIN(m_OutCardOutTimeOperateHu_Timer_3, tmp_Timer_stamp);
			}
			else if (iOpType != TYPE_NULL)
			{
				const int tmp_Timer_stamp = 15;
				TIMER_AGAIN(m_OutCardOutTimeOperatePass_Timer_3, tmp_Timer_stamp);
			}
			break;
		default:
			break;
		}
	}


	 return iOpType;
 }

 void Table_yunxiaomajiang::GetAllHandCardRelativity(HandCards *pHc, MJ_BUFFER<_uint8, POOL_CARD_TYPE_MAX>& _relativity)
 {

	 HandCards tmp;
	 for (int i = 0; i < pHc->ChangeableCardsLen; i++)
	 {
		 if (pHc->ChangeableCards[i] == m_bReplaceJinCard)
		 {
			 continue;
		 }

		 m_GameLogic.AddCard(&tmp, pHc->ChangeableCards[i]);
	 }
	 _uint8 last_check = INVALID_MJ_CARD;
	 //for (int i = 0; i < tmp.ChangeableCardsLen; ++i)
	 //{
		// if (last_check == tmp.ChangeableCards[i])
		// {
		//	 continue;
		// }
		// last_check = tmp.ChangeableCards[i];
		 GetHandCardRelativity(last_check, _relativity);

	// }
 }

 void Table_yunxiaomajiang::GetHandCardRelativity(_uint8 bCard, MJ_BUFFER<_uint8, POOL_CARD_TYPE_MAX>& _relativity)
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
	 for (int i = 0x31; i <= 0x34; i++)
	 {
		 _relativity.Add(i);
	 }
	 for (int i = 0x41; i <= 0x43; i++)
	 {
		 _relativity.Add(i);
	 }
 }


 void Table_yunxiaomajiang::Check_Listening(HandCards *pHc, _uint8 ChairID, _uint8 bCard, tagListenInfos stListenInfo[14], _uint8 &bListenLen)
 {
	  if (bListenLen >= 14)
	  {
 		 log_error("%s chaochu zuida ting pai shu!\n", __FUNCTION__);
 		 return;
	  }

	  HandCards &tempHandCard = *pHc;
	  m_GameLogic.ChuPai(&tempHandCard, bCard);
	  bool bTrue = false;
	  MJ_BUFFER<_uint8, POOL_CARD_TYPE_MAX> _relativity;
	  GetAllHandCardRelativity(&tempHandCard, _relativity);
	  for (_uint8 i = 0; i < _relativity.Size(); i++)
	  {

		  _uint8 SavebCard = bCard;
		  _uint8 SaveRelativity = _relativity[i];
 		 m_GameLogic.AddCard(&tempHandCard, _relativity[i]);
 		 MJ_fan_buffer fan_buffer;
		// log_info("SavebCard = %x", SavebCard);
		// log_info("SaveRelativity = %x", SaveRelativity);
 		 if (QiangJinHuCheck_Hu(ChairID, tempHandCard, fan_buffer) == TYPE_HU)
 		 {
 			 if (stListenInfo[bListenLen].bHuLen < 18)
 			 {
 				 if (SavebCard == m_bJinCard)
 				 {
					 SavebCard = m_bReplaceJinCard;
 				 }
 				 if (SaveRelativity == m_bJinCard)
 				 {
					 SaveRelativity = m_bReplaceJinCard;

 				 }else if (SaveRelativity == m_bReplaceJinCard)
				 {
					 m_GameLogic.ChuPai(&tempHandCard, _relativity[i]);
					 continue;
				 }
 				 stListenInfo[bListenLen].bOutCard = SavebCard;//要打出的牌       
 				 stListenInfo[bListenLen].HuInfo[stListenInfo[bListenLen].bHuLen].bCard = SaveRelativity;//听的牌
 				 stListenInfo[bListenLen].HuInfo[stListenInfo[bListenLen].bHuLen].bFanShu = Calc_Fan(ChairID, fan_buffer);
 				 bTrue = true;
 				 stListenInfo[bListenLen].bHuLen++;

				 //如果是需要金原先的值胡牌的话,需要替换成白板,进行通知,之后需要把金这张牌打出.
 			 }
 		 }
		// log_info("ChuPai  _relativity[%d]= %x",i, _relativity[i]);

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
 }

 int Table_yunxiaomajiang::GetCardEstimate(_uint8 ChairID, _uint8 bCard, MJ_major& major, int& fan_num)
 {
	// assert(major.m_oFan.Size() == 0);

	 int iOpType = TYPE_NULL;
	 HandCards* pHc = &m_HandCard[ChairID];
	 m_GameLogic.AddCard(pHc, bCard);

	 if (pHc->ChangeableCardsLen % 3 != 2)
	 {
		 log_error("%s tid:%d fatal paishu error uid:%d chairid:%d!\n", __FUNCTION__,tid, GetPlayerUid(ChairID), ChairID);
		 m_GameLogic.ChuPai(pHc, bCard);
		 return TYPE_NULL;
	 }
	 //assert(pHc->ChangeableCardsLen <= HAND_CARD_SIZE_MAX);
	 if (pHc->ChangeableCardsLen > HAND_CARD_SIZE_MAX)
	 {
		 log_error("%s tid:%d fatal Card Len error uid:%d chairid:%d!\n", __FUNCTION__,tid, GetPlayerUid(ChairID), ChairID);
		 return -1;
	 }
	 /*获取杠信息*/
	 iOpType |= GetGangInfo(ChairID, major.m_GetGangInfo);
	 /*检查能不能胡*/
	 iOpType |= Check_Hu(ChairID, *pHc, major.m_oFan);

	 if (m_bGetCardCount[ChairID] == 1)
	 {	
		 /*如果为第一次摸牌,并且胡牌则为天胡*/
		 if (!major.m_oFan.Find(MJ_FAN_YUNXIAO_SANJINDAO))
		 {
			 UserGetFirstCard(ChairID, iOpType, major.m_oFan);
		 }
		// IsQiangJinHu(ChairID, iOpType, major.m_oFan);   //判断玩家的抢金胡,计算加番.

	 }
	 if (CardPoolIsEmpty())
	 {
		 /*最后一张胡牌则为海底捞*/
		// UserGetLastCard(ChairID, iOpType, major.m_oFan);
	 }
	 if (iOpType & TYPE_HU)
	 {
		 if (m_bAddFanFlag[ChairID]&& (!major.m_oFan.Find(MJ_FAN_YUNXIAO_SANJINDAO)))   //杠上花
		 {
			// UserGangShangHua(ChairID, bCard, major.m_oFan);
			 if ((m_bIsAnGangHuFlag[ChairID] == true)) //暗杠胡
			 {
				 IsAnGangHu(major.m_oFan, iOpType);
			 }
			 if ((m_bIsMingGangHuFlag[ChairID] == true) ) //明杠胡
			 {
				 IsMingGangHu(major.m_oFan, iOpType);
			 }
		 }
		 if (IsQingYiSe(&m_HandCard[ChairID], ChairID)&&!major.m_oFan.Find(MJ_FAN_YUNXIAO_SANJINDAO)) //清一色
		 {//清一色胡牌时，炮胡或自摸或游金都算双游金
			 //major.m_oFan.Add(MJ_FAN_YUNXIAO_DOUBLEYOUJIN);
			 major.m_oFan.Add(MJ_FAN_TYPE_QINGYISE);
		 }
		 else
		 {
			 if (m_QingYiSeDianPaoBaoPei == true)
			 {
				 m_conf_yunxiaomj.yun_xiao_dian_pao_bao_pei = false;
				 m_QingYiSeDianPaoBaoPei = false;
			 }
		 }
		 /*计算番*/
		 fan_num = Calc_Fan(ChairID, major.m_oFan);
	 }
	 m_GameLogic.ChuPai(pHc, bCard);
	 ////如果玩家中有人满足 则其他玩家只能胡杠上花与单游金。
	 bool ThreeJinFlag = false;
	 _uint8 JinStateChairId = 0xFF;
	 for (int i = 0; i < m_GAME_PLAYER;i++)
	 {
		 if (m_bUserOperateThreeYouJinFlag[i] == true)
		 {
			 JinStateChairId = i;
			 ThreeJinFlag = true;
			 break;
		 }
	 }
	 if (ThreeJinFlag == true && JinStateChairId != ChairID)
	 {
		 if (major.m_oFan.Find(MJ_FAN_YUNXIAO_ANGANGHU))
		 {
			 major.m_oFan.Clear();
			 major.m_oFan.Add(MJ_FAN_YUNXIAO_ANGANGHU);
			 fan_num = Calc_Fan(ChairID, major.m_oFan);

		 }
		 else if (major.m_oFan.Find(MJ_FAN_YUNXIAO_MINGGANGHU))
		 {
			 major.m_oFan.Clear();
			 major.m_oFan.Add(MJ_FAN_YUNXIAO_MINGGANGHU);
			 fan_num = Calc_Fan(ChairID, major.m_oFan);
		 }
		 else
		 {
			 major.m_oFan.Clear();
			 iOpType &= (~TYPE_HU);
		 }
	 }

	 return iOpType;
 }

 void Table_yunxiaomajiang::IsAnGangHu(MJ_fan_buffer& oFan, int iOpType)
 {
	 if ((iOpType & TYPE_HU))
	 {
		 oFan.Add(MJ_FAN_YUNXIAO_ANGANGHU);
	 }
 }

 void Table_yunxiaomajiang::IsMingGangHu(MJ_fan_buffer& oFan, int iOpType)
 {
	 if (iOpType & TYPE_HU)
	 {
		 oFan.Add(MJ_FAN_YUNXIAO_MINGGANGHU);
	 }
 }

 void Table_yunxiaomajiang::TurnBanker()
 {

	 MJ_user_buffer UserID;
	 int iCount = MultiHu(UserID);
	 if (iCount == 0)
	 {
		 //流局,3.	流局时，如果有人杠牌，庄家的下家当庄，如果没人杠牌，庄家下一把继续当庄
		 for (int i = 0; i < m_GAME_PLAYER; i++)
		 {
			 for (int j = 0; j < m_HandCard[i].FixedCardsLen; j++)
			 {
				 if ((m_HandCard[i].FixedCards[j].state == TYPE_ZHIGANG) || (m_HandCard[i].FixedCards[j].state == TYPE_WANGANG) || (m_HandCard[i].FixedCards[j].state == TYPE_ANGANG))
				 {
					 m_bBanker = NextChair(m_bBanker);
					 break;
				 }
				 else
				 {
					 m_bBanker = m_bBanker;
				 }
			 }
		 }
	 }
	 else if (iCount == 1)
	 {
		 for (int i = 0; i < m_GAME_PLAYER; i++)
		 {
			 if (m_bIsHu[i])
			 {
				 m_bBanker = i;
				 break;
			 }
		 }
	 }
 }

 bool Table_yunxiaomajiang::IsSanJinDao(_uint8 ChairID, MJ_fan_buffer& oFan)
 {
	 if (m_bUserHasJinNum[ChairID] == 3)
	 {
		 oFan.Add(MJ_FAN_YUNXIAO_SANJINDAO);
		 return true;
	 }
	 return false;
 }

 bool Table_yunxiaomajiang::IsQiangJinHu(_uint8 ChairID)
 {
	 _uint8 bPreChairid =  PreChair(ChairID);      //庄家
	 //log_info("IsQiangJinHu bPreChairid = %d", bPreChairid);
	// log_info("IsQiangJinHu m_HandCard[bPreChairid = %d].FixedCardsLen = %d", bPreChairid, m_HandCard[bPreChairid].FixedCardsLen);

	 if (m_HandCard[bPreChairid].FixedCardsLen !=0)
	 {//. 抢金胡的条件加一个，上家有吃碰杠时，下家不能抢金胡

		 return false;
	 }

	 for (int i = 0; i < m_GAME_PLAYER; i++)
	 {
		 if (m_bUserOperateThreeYouJinFlag[i] == true|| m_bUserOperateDoubleYouJinFlag[i]==true)
		 {
			 return false;
		 }
	 }

	 if (ChairID == m_bBanker)
	 {
		 return false ;
	 }
	 if (m_bGetCardCount[m_bBanker] != 1)
	 {
		 return false;
	 }
	 for (int i = 0; i < m_GAME_PLAYER;i++)
	 {
		 if (m_HandCard[i].FixedCardsLen!=0)
		 {
			 return false;
		 }
	 }
	// log_info("m_bGetCardCount[%d] = %d ", m_bBanker, m_bGetCardCount[m_bBanker]);
	// log_info("m_bGetCardCount[%d] = %d ", ChairID, m_bGetCardCount[ChairID]);
	// log_info("m_bUserOperateQiangJinHuPass[%d] = %d ", ChairID, m_bUserOperateQiangJinHuPass[ChairID]);
	 if ((m_bGetCardCount[m_bBanker] == 1) && (m_bGetCardCount[ChairID] == 0) && (ChairID != m_bBanker) && m_bUserOperateQiangJinHuPass[ChairID] == false)
	 {
		 _uint8 bCard = 0x00;
		 //检验手牌是否可以听牌
		 MJ_BUFFER<_uint8, POOL_CARD_TYPE_MAX> _relativity;
		 GetHandCardRelativity(bCard, _relativity);
		 HandCards tmp = m_HandCard[ChairID];
		 MJ_fan_buffer fan_buffer;
		 for (int i = 0; i < _relativity.Size(); i++)
		 {
			 m_GameLogic.AddCard(&tmp, _relativity[i]);
			// log_info("ChairID = %d _relativity[%d] = %x", ChairID, i, _relativity[i]);
			 //for (int j = 0; j < tmp.ChangeableCardsLen; j++)
			 //{
				// log_info("tmp: %x ", tmp.ChangeableCards[j]);
			 //}

			 if (QiangJinHuCheck_Hu(ChairID, tmp, fan_buffer) == TYPE_HU)
			 {
			//	 log_info("ChairID = %d IsQiangJinHu ok ", ChairID);
			// log_info("IsQiangJinHu ok ");
				 //满足抢金胡条件
				 for (int i = 0; i < m_GAME_PLAYER;i++)
				 {
					 if (i == ChairID)
					 {
						 memset(&m_bUserOperate, 0, sizeof(m_bUserOperate));
						 m_bCurChairID = ChairID;
						 m_bUserOperate[ChairID] |= TYPE_HU;
						 proto::game::yunxiaomj::AckUserHuYouJin stUserHuYouJin;
						 stUserHuYouJin.set_chairid(ChairID);
						 stUserHuYouJin.set_optype(m_bUserOperate[ChairID]);
						 stUserHuYouJin.set_optypehutype(0x20);
						 unicast(SERVER_ROOM_YUNXIAO_JIN_HU, ChairID, stUserHuYouJin);
						 std::string str = stUserHuYouJin.SerializeAsString();
						 save_video_data(SERVER_ROOM_YUNXIAO_JIN_HU, str);
						 set_operator_timer(ChairID);
						 m_bUserOperateQiangJinHuFlag[ChairID] = true;
					 }
					 else
					 {
						 m_bUserOperate[i] = TYPE_NULL;
						 proto::game::yunxiaomj::AckUserHuYouJin stUserHuYouJin;
						 stUserHuYouJin.set_chairid(i);
						 stUserHuYouJin.set_optype(m_bUserOperate[i]);
						 unicast(SERVER_ROOM_YUNXIAO_JIN_HU, i, stUserHuYouJin);
						 std::string str = stUserHuYouJin.SerializeAsString();
						 save_video_data(SERVER_ROOM_YUNXIAO_JIN_HU, str);
						 set_operator_timer(ChairID);

					 }
				 }
				 return true;
			 }
			 m_GameLogic.ChuPai(&tmp, _relativity[i]);
		 }
	 }
	 return false;

 }

 bool Table_yunxiaomajiang::Is19Hu(_uint8 ChairID, HandCards& pHc, MJ_fan_buffer& oFan, MJ_win_pattern& pattern)
 {//在13幺中，白板可以继续做白板
	 //log_info("Is19Hu start");
	 _uint8 YaoPai[13] = { 0x01, 0x09, 0x11, 0x19, 0x21, 0x29, 0x31, 0x32, 0x33, 0x34, 0x41, 0x42, 0x43 };

	 for (int i = 0; i < pHc.FixedCardsLen; i++)
	 {
		 if ((pHc.FixedCards[i].state == TYPE_LEFT_CHI) || (pHc.FixedCards[i].state == TYPE_CENTER_CHI) || (pHc.FixedCards[i].state == TYPE_RIGHT_CHI))
		 {
			 //log_info("pHc.FixedCards[i].state error");
			 return false;
		 }
		 //如果手牌不在19内，则false
		 if ((pHc.FixedCards[i].state == TYPE_PENG) || (pHc.FixedCards[i].state == TYPE_ZHIGANG) || (pHc.FixedCards[i].state == TYPE_WANGANG) || (pHc.FixedCards[i].state == TYPE_ANGANG))
		 {
			 //log_info("pHc.FixedCards[i].state error");
			 return false;
		 }
	 }

		 HandCards tmp;
		 if (m_bUserHasJinNum[ChairID] == 0)
		 {//在没有金的情况下，白板只能做白板使用，手牌中金的数量是原先手牌中白板的数量
			 for (int i = 0; i < pHc.ChangeableCardsLen; i++)
			 {
				 if (pHc.ChangeableCards[i] == m_bJinCard)
				 {
					 m_GameLogic.AddCard(&tmp, m_bReplaceJinCard);
					 continue;
				 }
				 m_GameLogic.AddCard(&tmp, pHc.ChangeableCards[i]);
			 }
			 bool Is19Flag = false;
			 for (int i = 0; i < tmp.ChangeableCardsLen; i++)
			 {//如果手牌不在19内，则false
				 for (int j = 0; j < 13; j++)
				 {
					 Is19Flag = false;
					 if (tmp.ChangeableCards[i] == YaoPai[j])
					 {
						 Is19Flag = true;
						 break;
					 }
				 }
				 if (Is19Flag == false)
				 {
					 return false;
				 }
			 }

		 }
		 else
		 {
			 bool Is19Flag = false;
			 _uint8 YaoPai[13] = { 0x01, 0x09, 0x11, 0x19, 0x21, 0x29, 0x31, 0x32, 0x33, 0x34, 0x41, 0x42, 0x43 };
			 for (int i = 0; i < 13; i++)
			 {
				 Is19Flag = false;
				 if (m_bJinCard == YaoPai[i])
				 {
					 Is19Flag = true;
					 break;
				 }
			 }
			 if (Is19Flag == false)
			 {//如果金的值不在13幺牌里面，则fals
				 return false;
			 }
			 // log_info("m_bJinCard is 19 ok");
			 for (int i = 0; i < pHc.ChangeableCardsLen; i++)
			 {//把白板原来的值替换回来，白板不在做金原来的值使用。
				 if (pHc.ChangeableCards[i] == m_bJinCard)
				 {
					 m_GameLogic.AddCard(&tmp, m_bReplaceJinCard);
					 continue;
				 }
				 else if (pHc.ChangeableCards[i] == m_bReplaceJinCard)
				 {
					 m_GameLogic.AddCard(&tmp, m_bJinCard);
					 continue;
				 }
				 m_GameLogic.AddCard(&tmp, pHc.ChangeableCards[i]);
			 }
			 // log_info("m_bJinCard is 19 01");
			 Is19Flag = false;
			 for (int i = 0; i < tmp.ChangeableCardsLen; i++)
			 {//如果手牌不在19内，则false
				 Is19Flag = false;
				 for (int j = 0; j < 13; j++)
				 {
					 if (tmp.ChangeableCards[i] == YaoPai[j])
					 {
						 Is19Flag = true;
						 break;
					 }
				 }
				 if (Is19Flag == false)
				 {
					 return false;
				 }
			 }
		 }
		 oFan.Add(MJ_FAN_YUNXIAO_19HU);
		 return true;
	 
 }
 bool Table_yunxiaomajiang::IsBuDa(HandCards& pHc, MJ_fan_buffer& oFan, MJ_win_pattern& pattern)
 {

	 if (pHc.ChangeableCardsLen < 14)
	 {
		 return false;
 	 }

	 HandCards tmp;
	 MJ_hand_buffer PaiCard[MJ_TYPE_JIAN];  //各种花式的牌数据
	 memset(PaiCard, 0, sizeof(PaiCard));
	 for (int i = 0; i < pHc.ChangeableCardsLen; i++)
	 {//手牌中不搭判断,白板不当金原来的值，还当作白板使用，金可以替换成任意的值
		 if (pHc.ChangeableCards[i] == m_bReplaceJinCard)
		 {
			 //m_GameLogic.AddCard(&tmp, m_bJinCard);
			 continue;
		 }
		 else if (pHc.ChangeableCards[i] == m_bJinCard)
		 {
			 m_GameLogic.AddCard(&tmp, m_bReplaceJinCard);
		 }
		 else
		 {
			 m_GameLogic.AddCard(&tmp, pHc.ChangeableCards[i]);
		 }

	 }
	 m_GameLogic.PyCard(tmp.ChangeableCards, tmp.ChangeableCardsLen, PaiCard);	

	 for (int i = 0; i < MJ_TYPE_JIAN-1;i++)
	 {//如果手牌中的，筒条万各一组，每组的个数>3 则错误。
		 if (PaiCard[i].Size()>3)
		 {
			 return false;
		 }
		 for (int j = 0; j < PaiCard[i].Size()-1;j++)
		 {
			 if ((PaiCard[i][j+1] - PaiCard[i][j])<3)
			 {
				 return false;
			 }
		 }
	 }

	 for (int i = 0; i <( PaiCard[MJ_TYPE_JIAN - 1].Size()-1);i++)
	 {

		 if (PaiCard[MJ_TYPE_JIAN - 1][i]== PaiCard[MJ_TYPE_JIAN - 1][i+1])
		 {
			 return false;
		 }
	 }
	 oFan.Add(MJ_FAN_YUNXIAO_BUDAHU);
	 return true;
 }
 
 bool Table_yunxiaomajiang::Is7Dui(_uint8 ChairID, HandCards& pHc, MJ_fan_buffer& oFan, int LaiZiNum)
 {
	 HandCards tmp;
	 if (pHc.ChangeableCardsLen <14)
	 {
		 return false;
	 }
	 int bUserHasJinNum = 0;
	 for (int i = 0; i < pHc.ChangeableCardsLen; i++)
	 {
		 if (pHc.ChangeableCards[i] == m_bReplaceJinCard)
		 {
			 bUserHasJinNum++;
			 continue;
		 }		 
		 m_GameLogic.AddCard(&tmp, pHc.ChangeableCards[i]);
	 }

	 int DuiNum = 0;
	 for (int i = 0; i < tmp.ChangeableCardsLen-1; i++)
	 {
		 if (tmp.ChangeableCards[i] == tmp.ChangeableCards[i+1])
		 {
			// log_info("tmp.ChangeableCards[%d]  = %x",i, tmp.ChangeableCards[i]);
			 DuiNum++;
			 i++;
		 }
	 }
	 //待添加游金的信息。
	// log_info("DuiNum = %d", DuiNum);
	 if (DuiNum + bUserHasJinNum >= 7)
	 {
		 if (m_conf.m_bHu7dui)
		 {
			 oFan.Add(MJ_FAN_TYPE_QIDUI);
			 return true;
		 }
	 }
	 return false;
 }

 bool Table_yunxiaomajiang::IsQingYiSe(const HandCards* p, _uint8 ChairID)
 {
	 int iFixedCardsLen = 0;
	 for (int i = 0; i < p->FixedCardsLen;i++)
	 {
		 if (p->FixedCards[i].state!=TYPE_ANGANG)
		 {
			 iFixedCardsLen++;
		 }
	 }

	  if (IsZiYiSe( p,  ChairID))
	  {
		  if (iFixedCardsLen >= 3)
		  {
			  if (m_conf_yunxiaomj.yun_xiao_dian_pao_bao_pei == false)
			  {
				  m_conf_yunxiaomj.yun_xiao_dian_pao_bao_pei = true;
				  m_QingYiSeDianPaoBaoPei = true;
			  }

		  }
		  else
		  {
			  if (m_QingYiSeDianPaoBaoPei == true)
			  {
				  m_conf_yunxiaomj.yun_xiao_dian_pao_bao_pei = false;
				  m_QingYiSeDianPaoBaoPei = false;
			  }

		  }
		  return true;
	  }
	 
	 HandCards tmp;
	 for (int i = 0; i < p->ChangeableCardsLen; i++)
	 {
		 if (p->ChangeableCards[i] == m_bReplaceJinCard)
		 {//如果是白板，过滤掉白板。
			// m_GameLogic.AddCard(&tmp, m_bJinCard);
			 continue;
		 }
		 if (p->ChangeableCards[i] == m_bJinCard)
		 {
			 return false;
		 }

		 m_GameLogic.AddCard(&tmp, p->ChangeableCards[i]);
	 }

	 for (int i = 0; i < p->FixedCardsLen; i++)
	 {
		 if (p->FixedCards[i].state== TYPE_LEFT_CHI || p->FixedCards[i].state == TYPE_CENTER_CHI|| p->FixedCards[i].state == TYPE_RIGHT_CHI)
		 {
			 for (int j = 0;j< 3;j++)
			 {
				 if (p->FixedCards[i].c[j] == m_bJinCard)
				 {
					 return false;
				 }
			 }
		 }
		 if (p->FixedCards[i].state == TYPE_PENG || p->FixedCards[i].state == TYPE_ZHIGANG || p->FixedCards[i].state == TYPE_WANGANG || p->FixedCards[i].state == TYPE_ANGANG)
		 {
			 if (p->FixedCards[i].CardData == m_bJinCard)
			 {
				 return false;
			 }
		 }
	 }
	 if (m_oFan[ChairID].Find(MJ_FAN_YUNXIAO_QIANGJINHU))
	 {
		 _uint8 Jintype = CardType(m_bJinCard);
		 _uint8 Cardtype = CardType(p->ChangeableCards[0]);
		 if (Jintype != Cardtype)
		 {
			 return false;
		 }
	 }
	 //胡牌时，金也必须是同一色
	 if (m_bUserHasJinNum[ChairID]>0)
	 {

		 if(p->ChangeableCards[0]== m_bReplaceJinCard)
		 {
			 _uint8 Cardtype;
			 if (p->FixedCards[0].state == TYPE_LEFT_CHI || p->FixedCards[0].state == TYPE_CENTER_CHI || p->FixedCards[0].state == TYPE_RIGHT_CHI)
			 {
				  Cardtype = CardType(p->FixedCards[0].c[0]);
			 }
			 else
			 {
				  Cardtype = CardType(p->FixedCards[0].CardData);

			 }
			 _uint8 Jintype = CardType(m_bJinCard);
			 if (Jintype != Cardtype)
			 {
				 return false;
			 }
		 }
		 else
		 {
			 _uint8 Jintype = CardType(m_bJinCard);
			 _uint8 Cardtype = CardType(p->ChangeableCards[0]);
			 if (Jintype != Cardtype)
			 {
				 return false;
			 }
		 }
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
		 //清一色点炮胡时，如果胡牌玩家已经有3/4句吃碰杠在下面，那么点炮玩家出22分，其他2家只出1分
	if (iFixedCardsLen >= 3)
	{
		 if (m_conf_yunxiaomj.yun_xiao_dian_pao_bao_pei == false)
		{
			 m_conf_yunxiaomj.yun_xiao_dian_pao_bao_pei = true;
			m_QingYiSeDianPaoBaoPei = true;
		}

	}
	else
	{
		 if (m_QingYiSeDianPaoBaoPei == true)
		{
			m_conf_yunxiaomj.yun_xiao_dian_pao_bao_pei = false;
			m_QingYiSeDianPaoBaoPei = false;
		 }
		
	}
	return true;
 }


 void Table_yunxiaomajiang::UserAfterOperate(_uint8 ChairID, int iOpType, _uint8 bCard)
 {
	 m_bCurChairID = ChairID;
	 log_info("%s uid:%d chairid:%d operate:0x%x card:0x%02x\n", __FUNCTION__, GetPlayerUid(ChairID), ChairID, iOpType, bCard);

	 m_bUserOperate[ChairID] = TYPE_NULL;
	 MJ_opinfo gang;

	 //吃碰之后，不能马上杠，要等下回合，杠后可以继续杠
	 if ((iOpType!= TYPE_PENG) &&(iOpType != TYPE_LEFT_CHI)&&(iOpType != TYPE_CENTER_CHI)&&(iOpType != TYPE_RIGHT_CHI))
	 {
		 m_bUserOperate[ChairID] = GetGangInfo(m_bCurChairID, gang);
	 }
	 if (gang.Find(OpState(m_bJinCard, TYPE_WANGANG)))//如果白板碰后,去检查杠,则需要发送给客户端的时候,需要对金与白板进行互换.
	 {
		 gang.Remove(OpState(m_bJinCard, TYPE_WANGANG));
		 gang.Add(OpState(m_bReplaceJinCard, TYPE_WANGANG));
	 }
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

 int Table_yunxiaomajiang::Calc_Hu(_uint8 ChairID, _uint8 bCard, int Score[GAME_PLAYER])
 {

	 m_oFan[ChairID].Clear();
	 m_HuFanNums[ChairID] = 0;
	 int iTotalFan = 0;
	 MJ_major major;
	 if (m_bRobHu)
	 {
		 int iOpType = OutCardEstimate_Qiangganghu(ChairID, bCard, major, m_HuFanNums[ChairID]);
		 if (iOpType & TYPE_HU)
		 {
			 m_HuFanNums[ChairID] = iTotalFan;
			 m_oFan[ChairID] = major.m_oFan;
			 for (_uint8 c = NextChair(ChairID); c != ChairID; c = NextChair(c))
			 {
				 m_HuFanNums[c] = m_HuFanNums[ChairID];
				 if (m_oFan[ChairID].Find(MJ_FAN_TYPE_DIHU) && (c != m_bBanker))//地胡时,庄家10分其他玩家9分.
				 {
					
					 m_HuFanNums[c] -= 1;
				 }
			 }
			 for (_uint8 c = NextChair(ChairID); c != ChairID; c = NextChair(c))
			 {
				 Score[ChairID] += m_HuFanNums[ChairID] * m_BaseScore;
				 Score[c] -= m_HuFanNums[c] * m_BaseScore;
			 }

			 m_stHuScore[ChairID].stHu.Add(tagHuRecord(ChairID, bCard, HU_ZIMO, m_HuFanNums[ChairID]));
		 }
	 }
	 else if (ChairID == m_bCurChairID)
	 {//自摸
		 int iOpType = GetCardEstimate(ChairID, INVALID_MJ_CARD, major, m_HuFanNums[ChairID]);
		 if (iOpType & TYPE_HU)
		 {
			 m_oFan[ChairID] = major.m_oFan;
			 for (_uint8 c = NextChair(ChairID); c != ChairID; c = NextChair(c))
			 {
				 Score[ChairID] += m_HuFanNums[ChairID] * m_BaseScore;
				 Score[c] -= m_HuFanNums[ChairID] * m_BaseScore;
			 }
			 m_stHuScore[ChairID].stHu.Add(tagHuRecord(ChairID, bCard, HU_ZIMO, m_HuFanNums[ChairID]));
		 }
	 }
	 else
	 {//放炮
		 m_bUserOperateGuoZhangHuFlag[ChairID] = false;
		 int iOpType = OutCardEstimate(ChairID, bCard, major, m_HuFanNums[ChairID]);
		 if (iOpType & TYPE_HU)
		 {
			 m_oFan[ChairID] = major.m_oFan;
			 MJ_fan_buffer		CalcoFan;	//胡番信息
			 int Num = 0;
			 for (int i = 0; i < m_oFan[ChairID].Size();i++)
			 {
		
				if (m_ft2fn->GetFan(m_oFan[ChairID][Num]) < m_ft2fn->GetFan(m_oFan[ChairID][i]))
				{
					Num = i;
				}

			 }
			 CalcoFan.Add(m_oFan[ChairID][Num]);
			 for (_uint8 c = NextChair(ChairID); c != ChairID; c = NextChair(c))
			 {
				 m_HuFanNums[c] = m_HuFanNums[ChairID] ;
			 }
			 for (_uint8 c = NextChair(ChairID); c != ChairID; c = NextChair(c))
			 {
				 if (CalcoFan.Find(MJ_FAN_TYPE_DIHU) && (c != m_bBanker))//地胡时,庄家10分其他玩家9分.
				 {
					 m_HuFanNums[c] -= 1;
				 }
				 if (CalcoFan.Find(MJ_FAN_TYPE_QINGYISE) && c != m_bOutCardChairID)
				 {
					 m_HuFanNums[c] -= 1;
				 }

				if (CalcoFan.Find(MJ_FAN_TYPE_PINGHU) && c != m_bOutCardChairID)
				{//1.	平胡点炮：放炮者2分，其他玩家1分 
					m_HuFanNums[c] -= 1;
				}
				if (CalcoFan.Find(MJ_FAN_YUNXIAO_19HU))//19时,点炮各家2分,
				{
					m_HuFanNums[c] -= 2;
				}
				if (CalcoFan.Find(MJ_FAN_YUNXIAO_BUDAHU) && c != m_bOutCardChairID)
				{
					m_HuFanNums[c] -= 1;
				}
				if (CalcoFan.Find(MJ_FAN_TYPE_QIDUI) && c != m_bOutCardChairID)
				{
					m_HuFanNums[c] -= 1;
				}

			 }
			 for (_uint8 c = NextChair(ChairID); c != ChairID; c = NextChair(c))
			 {
	
				 Score[ChairID] += m_HuFanNums[c] * m_BaseScore;
				 Score[c] -= m_HuFanNums[c] * m_BaseScore;

			 }
			 if (m_conf_yunxiaomj.yun_xiao_dian_pao_bao_pei == true) //点炮包陪
			 {
				 for (_uint8 c = NextChair(ChairID); c != ChairID; c = NextChair(c))
				 {
					 if (c == m_bOutCardChairID)
					 {
						 Score[c] = -Score[ChairID];
						 if (major.m_oFan.Find(MJ_FAN_TYPE_QINGYISE)&& m_QingYiSeDianPaoBaoPei==true)
						 {
							 Score[c] += (m_GAME_PLAYER-2);
						 }
					 }
					 else if (major.m_oFan.Find(MJ_FAN_TYPE_QINGYISE)&& c != m_bOutCardChairID&&m_QingYiSeDianPaoBaoPei == true)
					 {
						 Score[c] = -1;
					 }
					 else
					 {
						 Score[c] = 0;
					 }
				 }
			 }
			 m_stHuScore[ChairID].stHu.Add(tagHuRecord(m_bCurChairID, bCard, HU_PAOHU, m_HuFanNums[ChairID]));
		 }
	 }

	if(m_bUserOperateThreeYouJinFlag[ChairID]==true&& m_bUserOperateThreeYouJinStep[ChairID]==true && m_bRobHu == false)
	{
		if (IsQingYiSe(&m_HandCard[ChairID], ChairID))
		{
			m_oFan[ChairID].Add(MJ_FAN_TYPE_QINGYISE);
		}
		 m_oFan[ChairID].Add(MJ_FAN_YUNXIAO_THERRYOUJIN);

		 m_HuFanNums[ChairID] = Calc_Jin_Fan(ChairID, m_oFan[ChairID]);
		 if (m_oFan[ChairID].Find(MJ_FAN_TYPE_QINGYISE))
		 {
			 m_HuFanNums[ChairID] -= 2;
		 }
		 for (_uint8 c = NextChair(ChairID); c != ChairID; c = NextChair(c))
		 {
			 Score[ChairID] += m_HuFanNums[ChairID] * m_BaseScore;
			 Score[c] -= m_HuFanNums[ChairID] * m_BaseScore;
		 }
		 m_stHuScore[ChairID].stHu.Add(tagHuRecord(ChairID, bCard, HU_ZIMO, m_HuFanNums[ChairID]));
	 }else  if (m_bUserOperateDoubleYouJinFlag[ChairID]== true&& m_bUserOperateDoubleYouJinStep[ChairID]==true && m_bRobHu == false)
	 {
		 if (IsQingYiSe(&m_HandCard[ChairID], ChairID))
		 {
			 m_oFan[ChairID].Add(MJ_FAN_TYPE_QINGYISE);
		 }
		 m_oFan[ChairID].Add(MJ_FAN_YUNXIAO_DOUBLEYOUJIN);
		 m_HuFanNums[ChairID] = Calc_Jin_Fan(ChairID, m_oFan[ChairID]);
		 if (m_oFan[ChairID].Find(MJ_FAN_TYPE_QINGYISE))
		 {
			 m_HuFanNums[ChairID] -= 2;
		 }
		 for (_uint8 c = NextChair(ChairID); c != ChairID; c = NextChair(c))
		 {
			 Score[ChairID] += m_HuFanNums[ChairID] * m_BaseScore;
			 Score[c] -= m_HuFanNums[ChairID] * m_BaseScore;
		 }
		 m_stHuScore[ChairID].stHu.Add(tagHuRecord(ChairID, bCard, HU_ZIMO, m_HuFanNums[ChairID]));
	 }else if (m_bUserOperateDanYouJinHu[ChairID] == true&& m_bUserOperateDanYouJinstep[ChairID]==true && m_bRobHu == false)
	 {//如果满足单游金，则计算加番。

		 if (IsQingYiSe(&m_HandCard[ChairID], ChairID))
		 {
			 m_oFan[ChairID].Add(MJ_FAN_TYPE_QINGYISE);
		 }
		// log_info(" m_oFan[ChairID].Add(MJ_FAN_YUNXIAO_DOUBLEYOUJIN);");
		 m_oFan[ChairID].Add(MJ_FAN_YUNXIAO_DANYOUJIN);
		 m_HuFanNums[ChairID] = Calc_Jin_Fan(ChairID, m_oFan[ChairID]);
		 if (m_oFan[ChairID].Find(MJ_FAN_TYPE_QINGYISE))
		 {
			 m_HuFanNums[ChairID] -= 2;
		 }
		 if (m_oFan[ChairID].Find(MJ_FAN_YUNXIAO_THERRYOUJIN) && m_oFan[ChairID].Find(MJ_FAN_TYPE_QINGYISE))
		 {
			 m_HuFanNums[ChairID] = 24;
		 }
		 for (_uint8 c = NextChair(ChairID); c != ChairID; c = NextChair(c))
		 {
			 Score[ChairID] += m_HuFanNums[ChairID] * m_BaseScore;
			 Score[c] -= m_HuFanNums[ChairID] * m_BaseScore;
		 }
		 m_stHuScore[ChairID].stHu.Add(tagHuRecord(ChairID, bCard, HU_ZIMO, m_HuFanNums[ChairID]));
	 }
	 //出牌时的游金判断加蕃

	 if (m_bUserOperateThreeYouJinFlag[ChairID] == true && m_bUserOperateChiPengGangThreeYouJinstep[ChairID] == true&& m_bUserOperateThreeYouJinStep[ChairID] == false && m_bRobHu == false)
	 {
		 if (IsQingYiSe(&m_HandCard[ChairID], ChairID))
		 {
			 m_oFan[ChairID].Add(MJ_FAN_TYPE_QINGYISE);
		 }
		 m_oFan[ChairID].Add(MJ_FAN_YUNXIAO_THERRYOUJIN);

		 m_HuFanNums[ChairID] = Calc_Jin_Fan(ChairID, m_oFan[ChairID]);
		 if (m_oFan[ChairID].Find(MJ_FAN_TYPE_QINGYISE))
		 {
			 m_HuFanNums[ChairID] -= 2;
		 }
		 for (_uint8 c = NextChair(ChairID); c != ChairID; c = NextChair(c))
		 {
			 Score[ChairID] += m_HuFanNums[ChairID] * m_BaseScore;
			 Score[c] -= m_HuFanNums[ChairID] * m_BaseScore;
		 }
		 m_stHuScore[ChairID].stHu.Add(tagHuRecord(ChairID, bCard, HU_ZIMO, m_HuFanNums[ChairID]));
	 }
	 else  if (m_bUserOperateDoubleYouJinFlag[ChairID] == true && m_bUserOperateChiPengGangDoubleYouJinstep[ChairID] == true && m_bUserOperateDoubleYouJinStep[ChairID] == false && m_bRobHu == false)
	 {
		 if (IsQingYiSe(&m_HandCard[ChairID], ChairID))
		 {
			 m_oFan[ChairID].Add(MJ_FAN_TYPE_QINGYISE);
		 }
		// log_info(" m_oFan[ChairID].Add(MJ_FAN_YUNXIAO_DOUBLEYOUJIN);");
		 m_oFan[ChairID].Add(MJ_FAN_YUNXIAO_DOUBLEYOUJIN);
		 m_HuFanNums[ChairID] = Calc_Jin_Fan(ChairID, m_oFan[ChairID]);
		 if (m_oFan[ChairID].Find(MJ_FAN_TYPE_QINGYISE))
		 {
			 m_HuFanNums[ChairID] -= 2;
		 }
		 for (_uint8 c = NextChair(ChairID); c != ChairID; c = NextChair(c))
		 {
			 Score[ChairID] += m_HuFanNums[ChairID] * m_BaseScore;
			 Score[c] -= m_HuFanNums[ChairID] * m_BaseScore;
		 }
		 m_stHuScore[ChairID].stHu.Add(tagHuRecord(ChairID, bCard, HU_ZIMO, m_HuFanNums[ChairID]));
	 }
	 else if (m_bUserOperateDanYouJinHu[ChairID] == true && m_bUserOperateChiPengGangDanYouJinstep[ChairID] == true && m_bUserOperateDanYouJinstep[ChairID] == false && m_bRobHu == false)
	 {//如果满足单游金，则计算加番。

		 if (IsQingYiSe(&m_HandCard[ChairID], ChairID))
		 {
			 m_oFan[ChairID].Add(MJ_FAN_TYPE_QINGYISE);
		 }
		// log_info(" m_oFan[ChairID].Add(MJ_FAN_YUNXIAO_DOUBLEYOUJIN);");
		 m_oFan[ChairID].Add(MJ_FAN_YUNXIAO_DANYOUJIN);
		 m_HuFanNums[ChairID] = Calc_Jin_Fan(ChairID, m_oFan[ChairID]);
		 if (m_oFan[ChairID].Find(MJ_FAN_TYPE_QINGYISE))
		 {
			 m_HuFanNums[ChairID] -= 2;
		 }
		 if (m_oFan[ChairID].Find(MJ_FAN_YUNXIAO_THERRYOUJIN) && m_oFan[ChairID].Find(MJ_FAN_TYPE_QINGYISE))
		 {
			 m_HuFanNums[ChairID] = 24;
		 }
		 for (_uint8 c = NextChair(ChairID); c != ChairID; c = NextChair(c))
		 {
			 Score[ChairID] += m_HuFanNums[ChairID] * m_BaseScore;
			 Score[c] -= m_HuFanNums[ChairID] * m_BaseScore;
		 }
		 m_stHuScore[ChairID].stHu.Add(tagHuRecord(ChairID, bCard, HU_ZIMO, m_HuFanNums[ChairID]));
	 }

	 if (m_bUserOperateQiangJinHuFlag[ChairID] == true)
	 {
		 m_oFan[ChairID].Add(MJ_FAN_YUNXIAO_QIANGJINHU);
		 if (IsQingYiSe(&m_HandCard[ChairID], ChairID))
		 {
			 m_oFan[ChairID].Add(MJ_FAN_TYPE_QINGYISE);
		 }
		 m_HuFanNums[ChairID] = Calc_Jin_Fan(ChairID, m_oFan[ChairID]);

		 for (_uint8 c = NextChair(ChairID); c != ChairID; c = NextChair(c))
		 {
			 Score[ChairID] += m_HuFanNums[ChairID] * m_BaseScore;
			 Score[c] -= m_HuFanNums[ChairID] * m_BaseScore;
		 }
		 m_stHuScore[ChairID].stHu.Add(tagHuRecord(ChairID, m_bJinCard, HU_ZIMO, m_HuFanNums[ChairID]));
	 }

	 return 0;
 }

 int Table_yunxiaomajiang::Calc_Fan(_uint8 ChairID, MJ_fan_buffer& oFan)
 {
	 int iTotalFan = 0;

	 MJ_fan_buffer oFilterFan;
	 /*过滤完之后有大胡*/
	 if (m_ft2fn->FanFilter(oFan, oFilterFan))
	 {
		 oFan = oFilterFan;
	 }

	 for (int i = 0; i < oFan.Size(); i++)
	 {
		 if (iTotalFan < m_ft2fn->GetFan(oFan[i]))
		 {
			 iTotalFan = m_ft2fn->GetFan(oFan[i]);
		 }
	}
	 return iTotalFan;
	// log_info("Calc_Fan");
 }
 int Table_yunxiaomajiang::Calc_Jin_Fan(_uint8 ChairID, MJ_fan_buffer& oFan)
 {
	 int iTotalFan = 0;

	 MJ_fan_buffer oFilterFan;
	 /*过滤完之后有大胡*/
	 if (m_ft2fn->FanFilter(oFan, oFilterFan))
	 {
		 oFan = oFilterFan;
	 }

	 for (int i = 0; i < oFan.Size(); i++)
	 {
		iTotalFan += m_ft2fn->GetFan(oFan[i]);
	 }
	 return iTotalFan;
	 // log_info("Calc_Fan");
 }

 void Table_yunxiaomajiang::Calc_BaseScore(_uint8 ChairID, int iBaseScore[GAME_PLAYER])
 {
	 /*没胡则直接退出*/
	 if (!m_bIsHu[ChairID])
		 return;
	 MJ_fan_buffer OutFan;
	 //int iFanCount = 0;
	 int HuFanNums[m_GAME_PLAYER];//胡牌番数

	 int iTotalFan = Calc_Fan(ChairID, m_oFan[ChairID]);
	// log_info("iTotalFan = %d m_oFan[ChairID].size()=%d", iTotalFan, m_oFan[ChairID].Size());
	 for (int i = 0; i < m_GAME_PLAYER; i++)
	 {
		 HuFanNums[i] = iTotalFan;
	 }
	 if (m_oFan[ChairID].Find(MJ_FAN_YUNXIAO_DANYOUJIN)|| m_oFan[ChairID].Find(MJ_FAN_YUNXIAO_DOUBLEYOUJIN)|| m_oFan[ChairID].Find(MJ_FAN_YUNXIAO_THERRYOUJIN))
	 {
		 iTotalFan = Calc_Jin_Fan(ChairID, m_oFan[ChairID]);
		 for (int i = 0; i < m_GAME_PLAYER; i++)
		 {
			 HuFanNums[i] = iTotalFan;
			 if (m_oFan[ChairID].Find(MJ_FAN_YUNXIAO_DANYOUJIN) && m_oFan[ChairID].Find(MJ_FAN_TYPE_QINGYISE))
			 {
				 HuFanNums[i] -= 2;
			 }
			 if (m_oFan[ChairID].Find(MJ_FAN_YUNXIAO_DOUBLEYOUJIN) && m_oFan[ChairID].Find(MJ_FAN_TYPE_QINGYISE))
			 {
				 HuFanNums[i] -= 2;
			 }
			 if (m_oFan[ChairID].Find(MJ_FAN_YUNXIAO_THERRYOUJIN) && m_oFan[ChairID].Find(MJ_FAN_TYPE_QINGYISE))
			 {
				 HuFanNums[i] = 24;
			 }
		 }
	 }
	 if (m_bRobHu)
	 {
		 iTotalFan = 0;
		 for (int i = 0; i < m_oFan[ChairID].Size(); i++)
		 {
			 if (iTotalFan < m_ft2fn->GetFan(m_oFan[ChairID][i]))
			 {
				// log_info("The Max oFan = %d", m_ft2fn->GetFan(m_oFan[ChairID][i]));
				 iTotalFan = m_ft2fn->GetFan(m_oFan[ChairID][i]);
			 }
		 }
		 HuFanNums[ChairID] = iTotalFan;
		// log_info("m_oFan ChairID = %d SIZE = %d  HuFanNums[ChairID] = %d ", ChairID, m_oFan[ChairID].Size(), HuFanNums[ChairID]);
		 for (_uint8 c = NextChair(ChairID); c != ChairID; c = NextChair(c))
		 {
			 HuFanNums[c] = HuFanNums[ChairID];
			 if (m_oFan[ChairID].Find(MJ_FAN_TYPE_DIHU) && (c != m_bBanker))//地胡时,庄家10分其他玩家9分.
			 {
	
				 HuFanNums[c] -= 1;
			 }
		 }
		 for (_uint8 c = NextChair(ChairID); c != ChairID; c = NextChair(c))
		 {
			// log_info("m_HuFanNums[%d] = %d", ChairID, m_HuFanNums[ChairID]);
			// log_info("m_BaseScore= %d", m_BaseScore);
			 iBaseScore[ChairID] += HuFanNums[ChairID] * m_BaseScore;
			 iBaseScore[c] -= HuFanNums[c] * m_BaseScore;
		 }
		 if (m_QingYiSeDianPaoBaoPei == true&& m_oFan[ChairID].Find(MJ_FAN_TYPE_QINGYISE)) //点炮包陪
		 {
			 for (_uint8 c = NextChair(ChairID); c != ChairID; c = NextChair(c))
			 {
				 if (c == m_WangGangPlayer)
				 {
					 iBaseScore[c] = -iBaseScore[ChairID];
					 if (m_oFan[ChairID].Find(MJ_FAN_TYPE_QINGYISE) && m_QingYiSeDianPaoBaoPei == true)
					 {
						 iBaseScore[c] += (m_GAME_PLAYER - 2);
					 }
				 }
				 else if (m_oFan[ChairID].Find(MJ_FAN_TYPE_QINGYISE) && c != m_WangGangPlayer&&m_QingYiSeDianPaoBaoPei == true)
				 {
					 iBaseScore[c] = -1;
				 }
				 else
				 {
					 iBaseScore[c] = 0;
				 }
			 }
		 }

	 }
	 else if (ChairID == m_bCurChairID)
	 {//自摸
		 //游金时，是自摸状态，则在此处进行计算。
		 if (m_oFan[ChairID].Find(MJ_FAN_TYPE_QINGYISE) && m_oFan[ChairID].Find(MJ_FAN_YUNXIAO_MINGGANGHU))
		 {//清一色，杠上花，时每家出分16fen		 
			 for (int i = 0; i < m_GAME_PLAYER; i++)
			 {
				 HuFanNums[i] = 16;
			 }
		 }
		 if (m_oFan[ChairID].Find(MJ_FAN_TYPE_QINGYISE) && m_oFan[ChairID].Find(MJ_FAN_YUNXIAO_ANGANGHU))
		 {
			 for (int i = 0; i < m_GAME_PLAYER; i++)
			 {
				 HuFanNums[i] = 16;
			 }
		 }
		 for (_uint8 c = NextChair(ChairID); c != ChairID; c = NextChair(c))
		 {
			 iBaseScore[ChairID] += HuFanNums[c] * m_BaseScore;
			 iBaseScore[c] -= HuFanNums[c] * m_BaseScore;
			// log_info("iTotalFan %d = %d",c, iTotalFan);
		 }
	 }
	 else
	 {//放炮
		 MJ_fan_buffer		CalcoFan;	//胡番信息
		 int Num = 0;
		 for (int i = 0; i < m_oFan[ChairID].Size(); i++)
		 {

			 if (m_ft2fn->GetFan(m_oFan[ChairID][Num]) < m_ft2fn->GetFan(m_oFan[ChairID][i]))
			 {
				 Num = i;
			 }

		 }
		 CalcoFan.Add(m_oFan[ChairID][Num]);

		 for (_uint8 c = NextChair(ChairID); c != ChairID; c = NextChair(c))
		 {
			 if (CalcoFan.Find(MJ_FAN_TYPE_PINGHU) && c != m_bOutCardChairID)
			 {//1.	平胡点炮：放炮者2分，其他玩家1分 
				 HuFanNums[c] -= 1;
			 }
			 if (CalcoFan.Find(MJ_FAN_TYPE_DIHU) && (c != m_bBanker))
			 {//地胡时,庄家10分其他玩家9分.
				 HuFanNums[c] -= 1;
			 }
			 if (CalcoFan.Find(MJ_FAN_YUNXIAO_19HU))
			 {//19时,点炮各家2分,
				 HuFanNums[c] -= 2;
			 }
			 if (CalcoFan.Find(MJ_FAN_YUNXIAO_BUDAHU) && c != m_bOutCardChairID)
			 {//15.	不搭点炮：点炮者2分，其他1分
				 HuFanNums[c] -= 1;
			 }
			 if (CalcoFan.Find(MJ_FAN_TYPE_QINGYISE) && c != m_bOutCardChairID)
			 {//17.	对对胡点炮：点炮者2分，其他1分
				 HuFanNums[c] -= 1;
			 }
			 if (CalcoFan.Find(MJ_FAN_TYPE_QIDUI) && c != m_bOutCardChairID)
			 {//17.	对对胡点炮：点炮者2分，其他1分
				 HuFanNums[c] -= 1;
			 }
		
		 }
		 for (_uint8 c = NextChair(ChairID); c != ChairID; c = NextChair(c))
		 {
			 iBaseScore[ChairID] += HuFanNums[c] * m_BaseScore;
			 iBaseScore[c] -= HuFanNums[c] * m_BaseScore;
			 //log_info("m_HuFanNums[%d] = %x", c, HuFanNums[c]);
		 }
		 if (m_conf_yunxiaomj.yun_xiao_dian_pao_bao_pei == true) //点炮包陪
		 {
			 for (_uint8 c = NextChair(ChairID); c != ChairID; c = NextChair(c))
			 {
				 if (c == m_bOutCardChairID)
				 {
					 iBaseScore[c] = -iBaseScore[ChairID];
					 if (m_oFan[ChairID].Find(MJ_FAN_TYPE_QINGYISE)&&m_QingYiSeDianPaoBaoPei == true)
					 {
						 iBaseScore[c] += (m_GAME_PLAYER - 2);
					 }
				 }
				 else if (m_oFan[ChairID].Find(MJ_FAN_TYPE_QINGYISE) && c != m_bOutCardChairID&&m_QingYiSeDianPaoBaoPei == true)
				 {
					 iBaseScore[c] = -1;
				 }
				 else
				 {
					 iBaseScore[c] = 0;
				 }
			 }
		 }
	 }
 }
 void Table_yunxiaomajiang::Calc_HuScore(_uint8 ChairID, int iBaseScore[GAME_PLAYER], int iScore[GAME_PLAYER])
 {
	 if (m_bIsHu[ChairID])
	 {

		 if (m_bCurChairID == ChairID)
		 {
			 for (_uint8 c = NextChair(ChairID); c != ChairID; c = NextChair(c))
			 {
				 iScore[ChairID] -= iBaseScore[c];
				 iScore[c] = iBaseScore[c];
			 }
		 }
		 else
		 {

			 for (_uint8 c = NextChair(ChairID); c != ChairID; c = NextChair(c))
			 {
				 iScore[ChairID] -= iBaseScore[c];
				 iScore[c] = iBaseScore[c];
			 }

		 }
		 log_info("%s uid:%d chairid:%d, base score [%d,%d,%d,%d]\n", __FUNCTION__, GetPlayerUid(ChairID), ChairID, iScore[0], iScore[1], iScore[2], iScore[3]);
	 }
 }

 void Table_yunxiaomajiang::Proto_GameEndInfo(const MJ_user_buffer& UserID, proto::game::AckGameEnd& stGameend)
 {

	 Table::Proto_GameEndInfo(UserID, stGameend);

	 tagYunXiaomjGameEnd* end = stGameend.mutable_yunxiaomjgameendinfo();
	 end->set_leftcardnum(m_CardPool.Count());

	 FanInfo* info[GAME_PLAYER] = { 0 };
	 for (_uint8 i = 0; i < m_GAME_PLAYER; i++)
	 {
		 info[i] = end->add_faninfo();
	 }
	
	 MJ_fan_buffer tmpFan[GAME_PLAYER] = m_oFan;
	 int tmpHuFanNums[GAME_PLAYER] = { 0 };

	 Calc_Score(tmpHuFanNums, m_HuFanNums);

	 for (int i = 0; i < m_GAME_PLAYER; i++)
	 {
		 log_info("m_HuFanNums[%d]= %d ", i, m_HuFanNums[i]);
	 }

	 if (UserID.Size() == 1)
	 {//一个人胡 

		 _uint8 win_seat = UserID[0];
		// assert(m_stHuScore[win_seat].stHu.Size() != 0);
		 if (m_stHuScore[win_seat].stHu[0].bType == HU_ZIMO)
		 {//自摸胡
			 info[win_seat]->set_chairid(win_seat);
			 info[win_seat]->set_specialtype(HU_ZIMO);
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

 void Table_yunxiaomajiang::OnUserSendDanYoujinHu(_uint8 ChairID)
 {

	 for (int i = 0; i < m_GAME_PLAYER; i++)
	 {
		 if (i == ChairID)
		 {
			 memset(&m_bUserOperate, 0, sizeof(m_bUserOperate));
			 m_bCurChairID = ChairID;
			 m_bUserOperate[ChairID] |= TYPE_YX_YOUJINHU;
			 //玩家满足单游金，则向玩家发送单游金的状态。
			 proto::game::yunxiaomj::AckUserHuYouJin stUserHuYouJin;
			 stUserHuYouJin.set_chairid(ChairID);
			 stUserHuYouJin.set_optype(m_bUserOperate[ChairID]);
			 if (m_bUserOperateThreeYouJinFlag[ChairID] == true)
			 {
				 stUserHuYouJin.set_optypehutype(0x10);
			 }
			 else  if (m_bUserOperateDoubleYouJinFlag[ChairID] == true)
			 {
				 stUserHuYouJin.set_optypehutype(0x08);
			 }
			 else if (m_bUserOperateDanYouJinHu[ChairID] == true)
			 {
				 stUserHuYouJin.set_optypehutype(0x04);
			 }
			 unicast(SERVER_ROOM_YUNXIAO_JIN_HU, ChairID, stUserHuYouJin);
			 std::string str = stUserHuYouJin.SerializeAsString();
			 save_video_data(SERVER_ROOM_YUNXIAO_JIN_HU, str);
			 set_operator_timer(ChairID);
			 if (m_conf_yunxiaomj.yunxiao_zidong_chupai_hupai_pass == true)
			 {
				 const int tmp_Timer_stamp = 15;
				 TIMER_AGAIN(m_YouJinOperateHuYouJin_Timer_0, tmp_Timer_stamp);
			 }
			// log_info("SERVER_ROOM_YUNXIAO_JIN_HU i  = %d  stUserHuYouJin.SerializeAsString = %s", i, stUserHuYouJin.ShortDebugString().c_str());

		 }
		 else
		 {
			 m_bUserOperate[i] = TYPE_NULL;
			 //玩家满足单游金，则向玩家发送单游金的状态。
			 proto::game::yunxiaomj::AckUserHuYouJin stUserHuYouJin;
			 stUserHuYouJin.set_chairid(i);
			 stUserHuYouJin.set_optype(m_bUserOperate[i]);
			 unicast(SERVER_ROOM_YUNXIAO_JIN_HU, i, stUserHuYouJin);
			 std::string str = stUserHuYouJin.SerializeAsString();
			 save_video_data(SERVER_ROOM_YUNXIAO_JIN_HU, str);
			 set_operator_timer(ChairID);
			// log_info("SERVER_ROOM_YUNXIAO_JIN_HU i  =%d  stUserHuYouJin.SerializeAsString = %s",i, stUserHuYouJin.)
		 }
	 }
 }

 void Table_yunxiaomajiang::DoSimpleOperate(_uint8 ChairID, int iOpType, _uint8 bCard)
 {

	 log_info("DoSimpleOperate tid:%d uid:%d chairid:%d, iOptype:0x%x \n",tid, GetPlayerUid(ChairID), ChairID, iOpType);
	 if (m_bIsGuoZhangHuFlag[ChairID] == true && iOpType != TYPE_HU)
	 {//过张
		 m_bUserOperateGuoZhangHuFlag[ChairID] = true;
		 m_bIsGuoZhangHuFlag[ChairID] = false;

	 }
	 if (iOpType & TYPE_YX_YOUJINHU) //操作游金胡牌。
	 {
		//m_UserOperateHuJinFlag = true;
		 UserOperateHuDanYouJin(ChairID);	
		 GameEnd();
	 }
	 else 
	 {
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
		//	log_info("m_conf_yunxiaomj.yunxiao_qiang_ming_gang = %d", m_conf_yunxiaomj.yunxiao_qiang_ming_gang);
		//	log_info("ChairID = %d m_bCurChairID =%d", ChairID, m_bCurChairID);
			if ((m_conf_yunxiaomj.yunxiao_qiang_ming_gang && RobGangHu(ChairID, bCard, iOpType))&&( ChairID == m_bCurChairID))
				return;
			do
			{
				UserOperateGang(ChairID, iOpType, bCard);
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
			if (m_bCurChairID == ChairID)
			{
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
 }

 void Table_yunxiaomajiang::UserOperateHuDanYouJin(_uint8 ChairID)
 {

 	//关闭所有的有金状态,防止在游戏结束后还进行摸牌，上报多余的操作。
	// log_info("UserOperateHuDanYouJin start ChairID = %d", ChairID);
	 //广播发送操作结果
	 proto::game::AckBcOpResult stOpresult;
	 stOpresult.set_chairid(ChairID);
	 stOpresult.set_outcardchairid(m_bCurChairID);
	 stOpresult.set_optype(TYPE_YX_YOUJINHU);

	 if (m_bUserOperateThreeYouJinFlag[ChairID]==true)
	 {
		 stOpresult.set_yunxiaoyoujintype(0x10); //待添加向客户端发送的胡的金的类型。

	 }else if (m_bUserOperateDoubleYouJinFlag[ChairID] == true)
	 {
		 stOpresult.set_yunxiaoyoujintype(0x08); //待添加向客户端发送的胡的金的类型。

	 }else if (m_bUserOperateDanYouJinHu[ChairID]==true)
	 {
		 stOpresult.set_yunxiaoyoujintype(0x04); //待添加向客户端发送的胡的金的类型。
	 }
	 else
	 {
		 stOpresult.set_yunxiaoyoujintype(0); //待添加向客户端发送的胡的金的类型。
	 }

	// SetRobGangHu_Flag(ChairID, TYPE_YX_YOUJINHU, stOpresult);
	 broadcast(SERVER_OPERATE_RESULT, stOpresult);
	 if (!(m_bUserOperate[ChairID] & TYPE_YX_YOUJINHU))
	 {
		 log_error("hu error, hu chairid:%d, m_bUserOperate[%d]:%d \n", ChairID, ChairID, m_bUserOperate[ChairID]);
		// return;
	 }
	 m_bCurChairID = ChairID;
	 m_bIsHu[ChairID] = true;
	 int userscore[GAME_PLAYER] = { 0 };
	 _uint8 bCard = 0xFF;
	 Calc_Hu(ChairID, bCard, userscore);
	 Calc_Score(m_stHuScore, userscore);
	 //for (int i = 0; i < GAME_PLAYER; i++)
	 //{
		// log_debug("userscore[%d] = %d", userscore[i]);
	 //}
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
	 m_bUserOperateDanYouJinHu[ChairID] = false;
	 m_bUserOperateDoubleYouJinFlag[ChairID] = false;
	 m_bUserOperateThreeYouJinFlag[ChairID] = false;
	 broadcast(SERVER_USER_HU, stHu);
	 m_bUserOperate[ChairID] = TYPE_NULL;
	 int nscore[GAME_PLAYER] = { 0 };
	 save_video_operate(TYPE_HU, ChairID, stHu.pashuchairid(), bCard, nscore);
	 save_video_hu(ChairID, stHu.pashuchairid(), userscore); 
 }


 bool Table_yunxiaomajiang::IsDoubleYouJin(HandCards& pHc, _uint8 ChairID)
 {
	 /*
	 可以满足双游金的情况：
	 满足两个金做将，则满足双金胡的条件，打出一张金，则进入到双游金的状态 
	 */

	 if (m_bUserHasJinNum[ChairID] < 2)
	 {
		 return false;
	 }

	 HandCards tmp;
	 MJ_hand_buffer PaiCard[MJ_TYPE_JIAN];  //各种花式的牌数据
	 memset(PaiCard, 0, sizeof(PaiCard));
	 //去掉手牌中的金
	 for (int i = 0; i < pHc.ChangeableCardsLen; i++)
	 {
		 if (pHc.ChangeableCards[i] == m_bReplaceJinCard)
			 continue;
		 m_GameLogic.AddCard(&tmp, pHc.ChangeableCards[i]);
	 }

	 m_GameLogic.PyCard(tmp.ChangeableCards, tmp.ChangeableCardsLen, PaiCard);
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
	 needHunNum = wan_need + tiao_need + tong_need + feng_need;
	 if ((needHunNum == 0) && (m_bUserHasJinNum[ChairID] == 2))
	 {
		 return true;
	 }
	 else if ((needHunNum == 1) && (m_bUserHasJinNum[ChairID] == 3))
	 {
		 return true;
	 }

	 return false;
 }


bool Table_yunxiaomajiang::IsThreeYouJin(HandCards& pHc, _uint8 ChairID)
{
	if (m_bUserOperateDoubleYouJinFlag[ChairID] == false && m_bUserOperateShuangYouJinZhaungThreeFlag[ChairID]==false)
	{
		return false;
	}

	if(m_bUserHasJinNum[ChairID]!=2)
	{

		return false;
	}
	//过滤掉手牌中的金牌
	 HandCards tmp;
	 MJ_hand_buffer PaiCard[MJ_TYPE_JIAN];  //各种花式的牌数据
											//去掉手牌中的金
	 memset(&PaiCard,0,sizeof(PaiCard));
	 for (int i = 0; i < pHc.ChangeableCardsLen; i++)
	 {
		 if (pHc.ChangeableCards[i] == m_bReplaceJinCard)
			 continue;
		 m_GameLogic.AddCard(&tmp, pHc.ChangeableCards[i]);
	 }

	 m_GameLogic.PyCard(tmp.ChangeableCards, tmp.ChangeableCardsLen, PaiCard);
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
	 needHunNum = wan_need + tiao_need + tong_need + feng_need;
	 if ((needHunNum == 0) && (m_bUserHasJinNum[ChairID] == 2))
	 {//手牌两张金，去掉后依然能够胡牌，则
		 return true;
	 }
	 return false;

}

bool Table_yunxiaomajiang::Is7DuiDoubleYouJin(HandCards& pHc, _uint8 ChairID)
{

	if (m_bUserHasJinNum[ChairID] < 2)
	{
		return false;
	}

	int DuiNum = 0;
	for (int i = 0; i < pHc.ChangeableCardsLen - 1; i++)
	{
		if (pHc.ChangeableCards[i] == pHc.ChangeableCards[i + 1])
		{
			DuiNum++;
			i++;
		}
	}
	//log_info("Is7DuiDoubleYouJin DuiNum = %d", DuiNum);
	//七对双游金，必须一对金做将。
	if ( DuiNum==6 && m_bUserHasJinNum[ChairID] == 2)
	{
		return true;

	}else if (DuiNum == 5 && m_bUserHasJinNum[ChairID] == 3)
	{
		return true;
	}
	/*格式化牌*/
	HandCards tmp;
	//去除手牌中的金,之后进行判断

	for (int i = 0; i < pHc.ChangeableCardsLen; i++)
	{
		if (pHc.ChangeableCards[i] == m_bReplaceJinCard)
			continue;

		m_GameLogic.AddCard(&tmp, pHc.ChangeableCards[i]);
	}
	if (m_GameLogic.FormatCards_Laizi(tmp.ChangeableCards, tmp.ChangeableCardsLen, m_bUserHasJinNum[ChairID])&& m_bUserHasJinNum[ChairID]==3&& DuiNum == 4)/*递归，检查胡牌的形式*/
	{
		return true;
	}

	return false;
}

bool Table_yunxiaomajiang::Is7DuiThreeYouJin(HandCards& pHc, _uint8 ChairID)
{

	if (m_bUserHasJinNum[ChairID] != 2)
	{

		return false;
	}

	if (m_bUserOperateDoubleYouJinFlag[ChairID] != true&& m_bUserOperateShuangYouJinZhaungThreeFlag[ChairID]==false)
	{
		return false;
	}

	int DuiNum = 0;
	for (int i = 0; i < pHc.ChangeableCardsLen - 1; i++)
	{
		if (pHc.ChangeableCards[i] == pHc.ChangeableCards[i + 1])
		{
			DuiNum++;
			i++;
		}
	}
	//log_info("Is7DuiThreeYouJin DuiNum = %d", DuiNum);
	if (DuiNum == 6)
	{
		return true;
	}

	return false;
}


//操作函数
void Table_yunxiaomajiang::ExecuteGang(_uint8 ChairID, HandCards *pHc, _uint8 bOpCardChairID, _uint8 cbOperateCard, int cbType)
{

	if (cbType == TYPE_WANGANG)
	{
		m_GameLogic.RemoveCard(pHc, cbOperateCard, 1);
		for (int i = 0; i < pHc->FixedCardsLen; i++)
		{
			if (pHc->FixedCards[i].CardData == cbOperateCard)
			{
				pHc->FixedCards[i].state = cbType;
				//pHc->FixedCards[i].chairID = bOpCardChairID;
			}
		}
	}
	else if (ChairID == m_bCurChairID&&cbType== TYPE_ZHIGANG)
	{
		//log_info("TYPE_MINGGANG ChairID = %d m_bCurChairID = %d m_bOutCardChairID = %d", ChairID, m_bCurChairID, m_bOutCardChairID);
		_uint8 bLen = 4;
		m_GameLogic.RemoveCard(pHc, cbOperateCard, bLen);
		pHc->FixedCards[pHc->FixedCardsLen].CardData = cbOperateCard;
		pHc->FixedCards[pHc->FixedCardsLen].state = cbType;
		pHc->FixedCards[pHc->FixedCardsLen].chairID = bOpCardChairID;
		pHc->FixedCardsLen++;

	}
	else 
	{
		//log_info("TYPE_ZHI AND AN ChairID = %d m_bCurChairID = %d m_bOutCardChairID = %d", ChairID, m_bCurChairID, m_bOutCardChairID);
		_uint8 bLen = (cbType == TYPE_ANGANG ? 4 : 3);
		m_GameLogic.RemoveCard(pHc, cbOperateCard, bLen);
		pHc->FixedCards[pHc->FixedCardsLen].CardData = cbOperateCard;
		pHc->FixedCards[pHc->FixedCardsLen].state = cbType;
		pHc->FixedCards[pHc->FixedCardsLen].chairID = bOpCardChairID;
		pHc->FixedCardsLen++;

	}

}


bool Table_yunxiaomajiang::RobGangHu(_uint8 ChairID, _uint8 CardData, int iOpType)
{
	for (int i = 0; i < m_GAME_PLAYER;i++)
	{
		if ( m_bUserOperateThreeYouJinFlag[i]==true|| m_bUserOperateDoubleYouJinFlag[i] == true)
		{
			return false;
		}
	}

	if (iOpType & TYPE_WANGANG && !m_conf.m_bQiangGangHu)
		return false;
	if (iOpType & TYPE_ZHIGANG && !m_conf.m_bQiangZhiGang)
		if (ChairID != m_bCurChairID)
		{
			return false;
		}
	if (iOpType & TYPE_ANGANG && !m_conf.m_bQiangAnGang)
		return false;
	if (iOpType & TYPE_ZHIGANG && !m_conf_yunxiaomj.yunxiao_qiang_ming_gang)
		return false;
	_uint8 cbNum = 0;
	for (int i = 0; i < m_GAME_PLAYER; i++)
	{
		if (m_bUserOperateDanYouJinHu[i] == true || m_bUserOperateDoubleYouJinFlag[i] == true || m_bUserOperateThreeYouJinFlag[i] == true)
		{//进入游金，不能胡抢杠胡
			continue;
		}
		if (i == ChairID)
		{
			continue;
		}
		MJ_major major;
		m_bRobGangHuJudge = true;
		m_WangGangPlayerOpType = iOpType;
		m_bUserHuTypeoFan[i].Clear();
		m_bUserOperate[i] = OutCardEstimate_Qiangganghu(i, CardData, major, m_HuFanNums[i]);

		m_bRobGangHuJudge = false;
		m_WangGangPlayerOpType = TYPE_NULL;
		m_bUserOperate[i] &= TYPE_HU;
		if (m_bUserOperate[i] & TYPE_HU)
		{
			m_bUserHuTypeoFan[i] = major.m_oFan;
			if (CardData== m_bReplaceJinCard)
			{
				CardData = m_bJinCard;
			}
			const HandCards& hc = m_HandCard[ChairID];
			MJ_opinfo opinfo;

			if (iOpType & TYPE_ZHIGANG && m_GameLogic.GetAnGang(&hc, opinfo, CardData)&& ChairID == m_bCurChairID)
			{
				if (CardData == m_bJinCard)
				{				
					CardData = m_bReplaceJinCard;
				}
				//log_info("RobGangHu TYPE_MING GANG GetAnGang OK");
				for (int c = 0; c < m_GAME_PLAYER; c++)
				{
					proto::game::AckOpNotify stNotify;
					Proto_Game_OpNotify(i, c, CardData, stNotify);
					unicast(SERVER_OPERATE_NOTIFY, c, stNotify);

				}
				cbNum++;
			}else if (iOpType & TYPE_WANGANG && m_GameLogic.GetWanGang(&hc, opinfo, CardData))
			{
				if (CardData == m_bJinCard)
				{
					CardData = m_bReplaceJinCard;
				}
				//log_info(" RobGangHu TYPE_WANGANG  GetWanGang OK");
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
				if (CardData == m_bJinCard)
				{
					CardData = m_bReplaceJinCard;
				}
				//log_info(" RobGangHu TYPE_ZHIGANG  GetZhiGang OK");
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
	//开启操作时钟 TODO
	for (int i = 0; i < m_GAME_PLAYER; i++)
	{
		if (i == ChairID)
		{
			continue;
		}
		log_info("RobGangHu tid:%d uid:%d chaird:%d out card, so that chairid:%d have operate:0x%x\n",tid, GetPlayerUid(ChairID), ChairID, i, m_bUserOperate[i]);
		if (m_bUserOperate[i] & TYPE_HU)
		{
			if (m_conf_yunxiaomj.yunxiao_zidong_chupai_hupai_pass == true)
			{
				switch (i)
				{
				case 0:
					 if (m_bUserOperate[i] & TYPE_HU)
					{
						const int tmp_Timer_stamp = 15;
						TIMER_AGAIN(m_OutCardOutTimeOperateHu_Timer_0, tmp_Timer_stamp);
					}

					break;
				case 1:
					 if (m_bUserOperate[i] & TYPE_HU)
					{
						const int tmp_Timer_stamp = 15;
						TIMER_AGAIN(m_OutCardOutTimeOperateHu_Timer_1, tmp_Timer_stamp);
					}

					break;
				case 2:
					if (m_bUserOperate[i] & TYPE_HU)
					{
						const int tmp_Timer_stamp = 15;
						TIMER_AGAIN(m_OutCardOutTimeOperateHu_Timer_2, tmp_Timer_stamp);
					}

					break;
				case 3:
					if (m_bUserOperate[i] & TYPE_HU)
					{
						const int tmp_Timer_stamp = 15;
						TIMER_AGAIN(m_OutCardOutTimeOperateHu_Timer_3, tmp_Timer_stamp);
					}
					break;
				default:
					break;
				}
			}
			set_operator_timer(i);
		}

	}
	if (cbNum > 0)
	{
		m_RobHuNum = cbNum;
		m_bRobWanGang = true;
		m_WangGangPlayer = ChairID;
		m_WangGangPlayerOpType = iOpType;
		m_RobWanGangCard = CardData;
		//log_info("robot ganghu, and hu user nums:%d \n", m_RobHuNum);
	}

	return (cbNum > 0);
}


int Table_yunxiaomajiang::QiangJinHuCheck_Hu(_uint8 ChairID, HandCards& pHc, MJ_fan_buffer& oFan)
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

	MJ_win_pattern pattern;
	if (Is7Dui(ChairID, pHc, oFan, m_bUserHasJinNum[ChairID]))
	{
		return TYPE_HU;
	}
	if (Is19Hu(ChairID, pHc, oFan, pattern))
	{
		return TYPE_HU;
	}
	if (IsBuDa(pHc, oFan, pattern))
	{
		return TYPE_HU;
	}

	/*格式化牌*/
	HandCards tmp;
	//去除手牌中的金,之后进行判断

	for (int i = 0; i < pHc.ChangeableCardsLen; i++)
	{
		if (pHc.ChangeableCards[i] == m_bReplaceJinCard)
			continue;

		m_GameLogic.AddCard(&tmp, pHc.ChangeableCards[i]);
	}

	if (m_GameLogic.FormatCards_Laizi(tmp.ChangeableCards, tmp.ChangeableCardsLen, m_bUserHasJinNum[ChairID]))/*递归，检查胡牌的形式*/
	{
		return Check_Hu_Pattern(ChairID, tmp, oFan, pattern);/*计算加番*/
	}
	//if (IsSanJinDao(ChairID, oFan) && ChairID == m_bCurChairID)
	//{
	//	return TYPE_HU;
	//}
	return TYPE_NULL;
}
void Table_yunxiaomajiang::Proto_Game_OperateGang(_uint8 ChairID, _uint8 bCard, int iOpType, int iGangScore[GAME_PLAYER], proto::game::AckBcOpResult& stOpresult)
{
	stOpresult.set_chairid(ChairID);
	stOpresult.set_outcardchairid(m_bOutCardChairID);
	stOpresult.set_optype(iOpType);
	stOpresult.set_card(bCard);
	//杠牌不算分
	//for (int i = 0; i < m_GAME_PLAYER; i++)
	//{
	//	stOpresult.add_score(iGangScore[i]);
	//	//记录杠的总分
	//	//m_stGang[i].nTotalScore += nGangScore[i];
	//	//log_info("chairid:%d , this gangscore:%d, totalscore:%d \n", i, iGangScore[i], m_stGang[i].iTotalScore);
	//}

	//if (m_bGangCalcImme)
	//{
	//	Update_Score(iGangScore);
	//}
}

int Table_yunxiaomajiang::GetPengGang(_uint8 ChairID, _uint8 cbCard, MJ_opinfo& GangCardInfo)
{
	HandCards *pHc = &m_HandCard[ChairID];
	GangCardInfo.Clear();
	int count = 0;
	for (int i = 0; i < pHc->ChangeableCardsLen; i++)
	{
		if (cbCard == pHc->ChangeableCards[i])
		{
			count++;
		}
	}
	//log_debug("count = %d, cbCard = %#x", count, cbCard);
	int iOpType = TYPE_NULL;
	if (count >= 2)
	{
		//log_info("GetPengGang  cbCard = %x", cbCard);
		if (m_bUserOutCardFlag[ChairID] != cbCard)
		{
			if (!m_bUserOperateGangCardInfo[ChairID].Find(OpState(cbCard, TYPE_PENG)))
			{
				GangCardInfo.Add(OpState(cbCard, TYPE_PENG));
				m_bUserOperateGangCardInfo[ChairID].Add(OpState(cbCard, TYPE_PENG));
				iOpType |= TYPE_PENG;
			}
		}

	}
	if (count >= 3)
	{
		GangCardInfo.Add(OpState(cbCard, TYPE_ZHIGANG));
		iOpType |= TYPE_ZHIGANG;
	}
	return iOpType;

}


void Table_yunxiaomajiang::DoMultiOperate(_uint8 ChairID, int iOpType, _uint8 bCard)
{

	if (m_bUserCheckDoMultiOperateiOpTypeFlag[ChairID] == true)
	{
		log_info("Err %s tid:%d uid:%d chaird:%d, curoptype:0x%x, optype:0x%x, now done OpNum:%d, totalOpnum:%d \n", __FUNCTION__, tid,
			GetPlayerUid(ChairID), ChairID, m_bUserOperate[ChairID], iOpType, m_bMultiDoneOpNum, m_bMultiOpUserNum);
		return;
	}
	if (m_bIsGuoZhangHuFlag[ChairID] == true && iOpType != TYPE_HU)
	{//过张
		m_bUserOperateGuoZhangHuFlag[ChairID] = true;
		m_bIsGuoZhangHuFlag[ChairID] = false;

	}
	m_bUserCheckDoMultiOperateiOpTypeFlag[ChairID] = true;
	m_bUserDoMultiOperateiOpTypeFlag[ChairID] = iOpType;
	//m_bUserDoMultiOperateiOpType[ChairID] = iOpType;
//	log_info("555555555 m_bUserDoMultiOperateiOpType[%d] = %d", ChairID, m_bUserDoMultiOperateiOpType[ChairID]);
	m_bMultiDoneOpNum++;
	m_bMultiUserDoneOp[ChairID] = iOpType;
	m_bMultiUserDoneOpCard[ChairID] = bCard;
	log_info("%s tid:%d uid:%d chaird:%d, curoptype:0x%x, optype:0x%x, now done OpNum:%d, totalOpnum:%d \n", __FUNCTION__, tid,
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
				log_info("%s force user uid:%d pass seat:%d!\n", __FUNCTION__, GetPlayerUid(c), c);
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
		if (iOpType == TYPE_PASS)
		{
			if (m_bUserOperate[ChairID] & TYPE_HU)   //胡牌玩家选择了过牌
			{
				m_bMulitHuOpeCount++;
				log_info("%s user uid:%d pass seat:%d, optype:0x%x!\n", __FUNCTION__, GetPlayerUid(ChairID), ChairID, m_bUserOperate[ChairID]);
				m_PassHuPlayer.Add(ChairID);
			}
			UserOperatePass(ChairID, iOpType, bCard);
		}
		else
		{
			if (iOpType != TYPE_NULL)
			{

				//assert(iOpType != TYPE_PASS);
				//assert(m_bRecordMultiOp.iOpType != TYPE_PASS);

				if (
					(m_bRecordMultiOp.iOpType == TYPE_NULL && (iOpType & (TYPE_CHI | TYPE_PENG | TYPE_GANG | TYPE_HU| TYPE_YX_YOUJINHU))) ||
					((iOpType & TYPE_GANG) && (m_bRecordMultiOp.iOpType & (TYPE_CHI | TYPE_PENG| TYPE_YX_YOUJINHU))) ||
					((iOpType & TYPE_PENG) && (m_bRecordMultiOp.iOpType & (TYPE_CHI| TYPE_YX_YOUJINHU)))||
					((iOpType & TYPE_CHI) && (m_bRecordMultiOp.iOpType & (TYPE_YX_YOUJINHU)))
					)
				{
				
					m_bRecordMultiOp.iOpType = iOpType;
					m_bRecordMultiOp.ChairID = ChairID;
					m_bRecordMultiOp.bCard = bCard;
				//	log_info("m_bRecordMultiOp.iOpType = %x", iOpType);
				//	log_info("m_bRecordMultiOp.ChairID = %x", ChairID);
				//	log_info("m_bRecordMultiOp.bCard = %x", bCard);
				}
				//assert(m_bRecordMultiOp.iOpType != TYPE_NULL);
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
					(m_bRecordMultiOp.iOpType == TYPE_NULL && (tmpOpType & (TYPE_CHI | TYPE_PENG | TYPE_GANG | TYPE_HU| TYPE_YX_YOUJINHU))) ||
					(tmpOpType & TYPE_HU) ||
					(tmpOpType & TYPE_GANG && (m_bRecordMultiOp.iOpType & (TYPE_CHI | TYPE_PENG | TYPE_GANG| TYPE_YX_YOUJINHU))) ||
					(tmpOpType & TYPE_PENG && (m_bRecordMultiOp.iOpType & (TYPE_CHI | TYPE_PENG| TYPE_YX_YOUJINHU)))||
					(tmpOpType & TYPE_CHI && (m_bRecordMultiOp.iOpType & (TYPE_CHI | TYPE_YX_YOUJINHU)))
					)
				{
					bOver = false;
					break;
				}
			}
			if (bOver)   //优先级高的玩家操作后，立即停止多操作
			{
				for (int i = 0; i < m_GAME_PLAYER; i++)  //其他低操作玩家服务器主动发   过操作
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

#define MultiOpOr(X,Y)	(((X) & (Y) ? (Y) : 0) & (m_bMultiUserDoneOp[c] & (Y) ? (Y) :0))

void Table_yunxiaomajiang::MultiOperateOver(_uint8 ChairID, int iOpType, _uint8 bCard)
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
		else if (MultiOpOr(iOpType, TYPE_YX_YOUJINHU))
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

		OnUserGetCard(ChairID, true);
		break;
	case TYPE_YX_YOUJINHU:
		//log_info("111 MultiOperateOver TYPE_YX_YOUJINHU");
		UserOperateHuDanYouJin(ChairID);
		GameEnd();

	default:
		break;
	}
}


void Table_yunxiaomajiang::GetCardOperateOutCard_Timer_cb(struct ev_loop *loop, struct ev_timer *w, int revents)
{
	Table_yunxiaomajiang* table = (Table_yunxiaomajiang*)w->data;
	table->m_bUserAutoOutCardFlag = true;

	ev_timer_stop(zjh.loop, &table->m_GetCardOperateOutCard_Timer);

	if (table->CheckUserOutCard(table->m_bCurChairID, table->m_bCurCard))
	{
		table->OnUserOutCard(table->m_bCurChairID, table->m_bCurCard);
		//出牌后其他玩家操作判断,并提示其它玩家
		table->EstimateUserRespond(table->m_bCurChairID, table->m_bCurCard);
	}
}

void Table_yunxiaomajiang::GetCardOperateHu_Timer_cb(struct ev_loop *loop, struct ev_timer *w, int revents)
{
	Table_yunxiaomajiang* table = (Table_yunxiaomajiang*)w->data;
	ev_timer_stop(zjh.loop, &table->m_GetCardOperateHu_Timer);
	if (table->m_bUserOperate[table->m_bCurChairID] & TYPE_HU)
	{
		table->DoSimpleOperate(table->m_bCurChairID, TYPE_HU, table->m_bCurCard);
	}
}


/*接收用户选择的操作*/
void Table_yunxiaomajiang::OnRecvUserOperate(_uint8 ChairID, int iOpType, int iCard)
{
	if (m_conf_yunxiaomj.yunxiao_zidong_chupai_hupai_pass == true)
	{
		//玩家选择操作，则关闭计时器
		TIMER_STOP(m_OutTimeStopOperateOutCard_Timer);
		TIMER_STOP(m_GetCardOperateOutCard_Timer);
		TIMER_STOP(m_GetCardOperateHu_Timer);
		TIMER_STOP(m_YouJinOperateHuYouJin_Timer_0);
		stop_yunxiao_operator_timer_stop(ChairID);
		if ((m_bUserOperate[NextChair(m_bCurOutCard)] & TYPE_YX_YOUJINHU) && (ChairID == NextChair(m_bCurOutCard)))
		{
			TIMER_STOP(m_YouJinOperateHuYouJin_Timer_1);
		}
	}

	_uint8 bCard = (_uint8)iCard;

	/*检查用户选择的操作*/
	if (!CheckUserOperator(ChairID, iOpType, bCard))
	{
		return;
	}

	stop_operator_timer(ChairID);
	m_bOverTimeState[ChairID] = 0;
	//如果是多操作
	if (m_bMultiOpUserNum > 1)
	{
		DoMultiOperate(ChairID, iOpType, bCard);
	}
	else
	{
		if (m_bRobWanGang)  //抢杠胡操作优先级
		{
			RobGangPriority(ChairID, bCard, iOpType);
			return;
		}
		DoSimpleOperate(ChairID, iOpType, bCard);
		//判断是下一玩家摸牌，或者当前玩家出牌		
	}
}

void Table_yunxiaomajiang::YunXiaoYouJinOperateHu_Timer_cb_0(struct ev_loop *loop, struct ev_timer *w, int revents)
{
	Table_yunxiaomajiang* table = (Table_yunxiaomajiang*)w->data;

	ev_timer_stop(zjh.loop, &table->m_YouJinOperateHuYouJin_Timer_0);
	if (table->m_bUserOperate[table->m_bCurChairID] & TYPE_YX_YOUJINHU)
	{
		table->DoSimpleOperate(table->m_bCurChairID, TYPE_YX_YOUJINHU, table->m_bCurCard);
	}

}

void Table_yunxiaomajiang::YunXiaoYouJinOperateHu_Timer_cb_1(struct ev_loop *loop, struct ev_timer *w, int revents)
{
	Table_yunxiaomajiang* table = (Table_yunxiaomajiang*)w->data;
	ev_timer_stop(zjh.loop, &table->m_YouJinOperateHuYouJin_Timer_1);
	if (table->m_bUserOperate[table->NextChair(table->m_bOutCardChairID)] & TYPE_YX_YOUJINHU)
	{
		table->OnRecvUserOperate(table->NextChair(table->m_bOutCardChairID), TYPE_YX_YOUJINHU, 0x01);
	}
}


void Table_yunxiaomajiang::OutCardOperatePass_Timer_cb_0(struct ev_loop *loop, struct ev_timer *w, int revents)
{
	Table_yunxiaomajiang* table = (Table_yunxiaomajiang*)w->data;

	ev_timer_stop(zjh.loop, &table->m_OutCardOutTimeOperatePass_Timer_0);
	if (table->m_bUserOperate[0] != TYPE_NULL&&!(table->m_bUserOperate[0] & TYPE_HU))
	{
		table->OnRecvUserOperate(0, TYPE_PASS, table->m_bCurOutCard);
	}

}

void Table_yunxiaomajiang::OutCardOperatePass_Timer_cb_1(struct ev_loop *loop, struct ev_timer *w, int revents)
{
	Table_yunxiaomajiang* table = (Table_yunxiaomajiang*)w->data;

	ev_timer_stop(zjh.loop, &table->m_OutCardOutTimeOperatePass_Timer_1);
	if (table->m_bUserOperate[1] != TYPE_NULL&&!(table->m_bUserOperate[1] & TYPE_HU))
	{
		table->OnRecvUserOperate(1, TYPE_PASS, table->m_bCurOutCard);
	}

}

void Table_yunxiaomajiang::OutCardOperatePass_Timer_cb_2(struct ev_loop *loop, struct ev_timer *w, int revents)
{
	Table_yunxiaomajiang* table = (Table_yunxiaomajiang*)w->data;

	ev_timer_stop(zjh.loop, &table->m_OutCardOutTimeOperatePass_Timer_2);
	if (table->m_bUserOperate[2] != TYPE_NULL&&!(table->m_bUserOperate[2] & TYPE_HU))
	{
		table->OnRecvUserOperate(2, TYPE_PASS, table->m_bCurOutCard);

	}

}

void Table_yunxiaomajiang::OutCardOperatePass_Timer_cb_3(struct ev_loop *loop, struct ev_timer *w, int revents)
{
	Table_yunxiaomajiang* table = (Table_yunxiaomajiang*)w->data;

	ev_timer_stop(zjh.loop, &table->m_OutCardOutTimeOperatePass_Timer_3);
	if (table->m_bUserOperate[3] != TYPE_NULL &&!(table->m_bUserOperate[3] & TYPE_HU))
	{
		table->OnRecvUserOperate(3, TYPE_PASS, table->m_bCurOutCard);

	}
}


void Table_yunxiaomajiang::OutCardOperateHu_Timer_cb_0(struct ev_loop *loop, struct ev_timer *w, int revents)
{
	Table_yunxiaomajiang* table = (Table_yunxiaomajiang*)w->data;

	ev_timer_stop(zjh.loop, &table->m_OutCardOutTimeOperateHu_Timer_0);
	if (table->m_bUserOperate[0] & TYPE_HU)
	{
		table->OnRecvUserOperate(0, TYPE_HU, table->m_bCurOutCard);
	}

}

void Table_yunxiaomajiang::OutCardOperateHu_Timer_cb_1(struct ev_loop *loop, struct ev_timer *w, int revents)
{
	Table_yunxiaomajiang* table = (Table_yunxiaomajiang*)w->data;

	ev_timer_stop(zjh.loop, &table->m_OutCardOutTimeOperateHu_Timer_1);
	if (table->m_bUserOperate[1] & TYPE_HU)
	{
		table->OnRecvUserOperate(1, TYPE_HU, table->m_bCurOutCard);
	}

}

void Table_yunxiaomajiang::OutCardOperateHu_Timer_cb_2(struct ev_loop *loop, struct ev_timer *w, int revents)
{
	Table_yunxiaomajiang* table = (Table_yunxiaomajiang*)w->data;

	ev_timer_stop(zjh.loop, &table->m_OutCardOutTimeOperateHu_Timer_2);
	if (table->m_bUserOperate[2] & TYPE_HU)
	{
		table->OnRecvUserOperate(2, TYPE_HU, table->m_bCurOutCard);
	}

}

void Table_yunxiaomajiang::OutCardOperateHu_Timer_cb_3(struct ev_loop *loop, struct ev_timer *w, int revents)
{
	Table_yunxiaomajiang* table = (Table_yunxiaomajiang*)w->data;

	ev_timer_stop(zjh.loop, &table->m_OutCardOutTimeOperateHu_Timer_3);
	if (table->m_bUserOperate[3] & TYPE_HU)
	{
		table->OnRecvUserOperate(3, TYPE_HU, table->m_bCurOutCard);
	}

}

void Table_yunxiaomajiang::stop_yunxiao_operator_timer_stop(_uint8 ChairID)
{
	switch (ChairID)
	{
	case 0:
		ev_timer_stop(zjh.loop, &m_OutCardOutTimeOperatePass_Timer_0);
		ev_timer_stop(zjh.loop, &m_OutCardOutTimeOperateHu_Timer_0);
		break;
	case 1:
		ev_timer_stop(zjh.loop, &m_OutCardOutTimeOperatePass_Timer_1);
		ev_timer_stop(zjh.loop, &m_OutCardOutTimeOperateHu_Timer_1);

		break;
	case 2:
		ev_timer_stop(zjh.loop, &m_OutCardOutTimeOperatePass_Timer_2);
		ev_timer_stop(zjh.loop, &m_OutCardOutTimeOperateHu_Timer_2);

		break;
	case 3:
		ev_timer_stop(zjh.loop, &m_OutCardOutTimeOperatePass_Timer_3);
		ev_timer_stop(zjh.loop, &m_OutCardOutTimeOperateHu_Timer_3);
		break;
	default:
		break;
	}
}

void Table_yunxiaomajiang::ChiPengCardOperateOutCard_Timer_cb(struct ev_loop *loop, struct ev_timer *w, int revents)
{

	Table_yunxiaomajiang* table = (Table_yunxiaomajiang*)w->data;

	_uint8 bOutCard = 0x00;

	HandCards tmp;

	for (int i = 0; i< table->m_HandCard[table->m_bCurChairID].ChangeableCardsLen;i++)
	{
		if (table->m_HandCard[table->m_bCurChairID].ChangeableCards[i] == table->m_bReplaceJinCard)
		{
			continue;
		}
		table->m_GameLogic.AddCard(&tmp, table->m_HandCard[table->m_bCurChairID].ChangeableCards[i]);

	}
	if (tmp.ChangeableCardsLen == 0)
	{
		bOutCard = table->m_bJinCard;
	}
	else
	{
		bOutCard = tmp.ChangeableCards[tmp.ChangeableCardsLen-1];
		if (bOutCard == table->m_bJinCard)
		{
			bOutCard = table->m_bReplaceJinCard;
		}
	}

	ev_timer_stop(zjh.loop, &table->m_ChiPengCardOperateOutCard_Timer);
	if (table->CheckUserOutCard(table->m_bCurChairID, bOutCard))
	{
		table->OnUserOutCard(table->m_bCurChairID, bOutCard);
		//出牌后其他玩家操作判断,并提示其它玩家
		table->EstimateUserRespond(table->m_bCurChairID, bOutCard);
	}
}

void Table_yunxiaomajiang::Proto_Game_UserOutCard(_uint8 ChairID, _uint8 bCard, proto::game::AckUserOutCard& stUseroutcard)
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
	if (m_bUserAutoOutCardFlag == true)
	{
		stUseroutcard.set_autooutflag(true);
	}
	else
	{
		stUseroutcard.set_autooutflag(false);
	}

}

bool Table_yunxiaomajiang::IsZiYiSe(const HandCards* p,_uint8 ChairID)
{
	HandCards tmp = *p;
	if (m_bUserHasJinNum[ChairID] == 0)
	{
		if (m_GameLogic.IsZiYiSe(&tmp))
		{
			return true;
		}
	}
	else
	{
		if (m_bJinCard < 0x31)
		{
			return false;
		}

		for (int i = 0; i < p->ChangeableCardsLen; i++)
		{
			if (p->ChangeableCards[i] == m_bReplaceJinCard)
			{
				m_GameLogic.RemoveCard(&tmp, m_bReplaceJinCard, 1);
			}
		}
		if (m_GameLogic.IsZiYiSe(&tmp))
		{
			return true;
		}
		return false;
	}
	return false;
}

void Table_yunxiaomajiang::OutTimeStopOperateOutCard_Timer_cb(struct ev_loop *loop, struct ev_timer *w, int revents)
{
	Table_yunxiaomajiang* table = (Table_yunxiaomajiang*)w->data;

	ev_timer_stop(zjh.loop, &table->m_OutTimeStopOperateOutCard_Timer);
	proto::game::yunxiaomj::AckUserStopOutCard stUserStopOutCard;
	stUserStopOutCard.set_chairid(table->m_bCurChairID);
	stUserStopOutCard.set_stopoutcardflag(true);
	table->broadcast(SER_CMD_STOP_OUT_CARD, stUserStopOutCard);
	table->m_bOutTimeStopOperateOutCardFlag[table->m_bCurChairID] = true;
}


int Table_yunxiaomajiang::DanYouJinCheck_Hu(_uint8 ChairID, HandCards& pHc, MJ_fan_buffer& oFan)
{
	int iOpType = TYPE_NULL;
	if (pHc.ChangeableCardsLen > 14)
	{
		return TYPE_NULL;
	}
	//取余3不等于2的手牌不做胡牌判断
	if (pHc.ChangeableCardsLen != 0 && pHc.ChangeableCardsLen % 3 != 2)
	{
		return TYPE_NULL;
	}
	/*格式化牌*/
	HandCards tmp;
	//去除手牌中的金,之后进行判断

	int bUserHasJinNum = 0;
	for (int i = 0; i < pHc.ChangeableCardsLen; i++)
	{
		if (pHc.ChangeableCards[i] == m_bReplaceJinCard)
		{
			bUserHasJinNum++;
			continue;
		}	
		m_GameLogic.AddCard(&tmp, pHc.ChangeableCards[i]);
	}
	if (Is7Dui(ChairID, pHc, oFan, bUserHasJinNum))
	{
		iOpType =  TYPE_HU;
	}
	if (m_GameLogic.FormatCards_Laizi(tmp.ChangeableCards, tmp.ChangeableCardsLen, bUserHasJinNum))/*递归，检查胡牌的形式*/
	{
		iOpType =  TYPE_HU;/*计算加番*/
	}
	return iOpType;
}


void Table_yunxiaomajiang::DanYouJinCheck_Listening(HandCards *pHc, _uint8 ChairID, _uint8 bCard, tagListenInfos stListenInfo[14], _uint8 &bListenLen)
{
	if (bListenLen >= 14)
	{
		log_error("%s chaochu zuida ting pai shu!\n", __FUNCTION__);
		return;
	}

	HandCards &tempHandCard = *pHc;

	m_GameLogic.ChuPai(&tempHandCard, bCard);
	bool bTrue = false;

	MJ_BUFFER<_uint8, POOL_CARD_TYPE_MAX> _relativity;
	GetAllHandCardRelativity(&tempHandCard, _relativity);
	//log_info("111111111111111111111111111111111 Check_Listening _relativity.Size() = %d", _relativity.Size());
	for (_uint8 i = 0; i < _relativity.Size(); i++)
	{
		m_GameLogic.AddCard(&tempHandCard, _relativity[i]);
		MJ_fan_buffer fan_buffer;
		if (DanYouJinCheck_Hu(ChairID, tempHandCard, fan_buffer) == TYPE_HU)
		{
			if (stListenInfo[bListenLen].bHuLen <= 34)
			{
				stListenInfo[bListenLen].bOutCard = bCard;//要打出的牌       
				stListenInfo[bListenLen].HuInfo[stListenInfo[bListenLen].bHuLen].bCard = _relativity[i];//听的牌
				stListenInfo[bListenLen].HuInfo[stListenInfo[bListenLen].bHuLen].bFanShu = Calc_Fan(ChairID, fan_buffer);
				bTrue = true;
				stListenInfo[bListenLen].bHuLen++;
			}
			//log_info("stListenInfo[bListenLen = %d].bHuLen = %d", bListenLen, stListenInfo[bListenLen].bHuLen);
			//log_info("i = %d bCard =%x  _relativity[i] = %x  ", i, bCard, _relativity[i]);
		}

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

}


bool Table_yunxiaomajiang::IsDanYouJin(HandCards& pHc, _uint8 ChairID)
{
	//如果玩家手牌满足四句话，一张金，则打出一张牌后，处于听牌，并且所听的牌的个数为所有的可以用来胡牌的牌。
	//则可以打出该张牌，之后进入到游金状态。
	memset(m_bSaveUserOutOnlyCard, 0, sizeof(m_bSaveUserOutOnlyCard));

	tagListenInfos stListenInfo[14];
	_uint8 bCard = 0xFF;
	memset(stListenInfo, 0, sizeof(tagListenInfos) * 14);
	_uint8 bListenLen = 0;

	for (int i = 0; i < pHc.ChangeableCardsLen; i++)
	{
		//防止手牌有有对、刻字时，连续对相同的牌做多次的判断胡牌
		if (bCard == pHc.ChangeableCards[i])
		{
			continue;
		}
		bCard = pHc.ChangeableCards[i];
		DanYouJinCheck_Listening(&pHc, ChairID, bCard, stListenInfo, bListenLen);

	}

	for (int i = 0; i < bListenLen; i++)
	{
		if (stListenInfo[i].bHuLen == 34)
		{
			m_GameLogic.AddCard(&m_bSaveUserOutOnlyCard[ChairID], stListenInfo[i].bOutCard);

		}
	}
	for (int i = 0; i < m_bSaveUserOutOnlyCard[ChairID].ChangeableCardsLen;i++)
	{
	//	log_info("m_bSaveUserOutOnlyCard[ChairID = %d] = %x", ChairID, m_bSaveUserOutOnlyCard[ChairID].ChangeableCards[i]);
	}
	if (m_bSaveUserOutOnlyCard[ChairID].ChangeableCardsLen > 0)
	{
		return true;
	}
	return false;
}


int Table_yunxiaomajiang::OutCardEstimate_Qiangganghu(_uint8 ChairID, _uint8 bCard, MJ_major& major, int& fan_num)
{
	if (bCard == m_bJinCard)
	{
		return TYPE_NULL;
	}

	if (bCard == m_bReplaceJinCard) //进行替换
	{
		bCard = m_bJinCard;
	}
//	assert(major.m_oFan.Size() == 0);

	int iOpType = TYPE_NULL;

	HandCards* pHc = &m_HandCard[ChairID];

	iOpType |= GetChi(ChairID, bCard, major.m_Chi);

	if (m_bCurChairID != PreChair(ChairID))
	{//只能下家吃上家的牌
		iOpType &= ~TYPE_CHI;
	}

	iOpType |= GetPengGang(ChairID, bCard, major.m_GetPengGang);
	//log_info("GetPengGang Info major.m_GetPengGang.Size() = %d", major.m_GetPengGang.Size());
	//for (int i = 0; i < major.m_GetPengGang.Size();i++)
	//{
	// log_info("major.m_GetPengGang[%d].cbCard = %x  major.m_GetPengGang[%d].nState = %x", i, major.m_GetPengGang[i].cbCard, i, major.m_GetPengGang[i].nState);
	// major.m_GetPengGang[i].cbCard;
	//}
//	assert(pHc->ChangeableCardsLen < HAND_CARD_SIZE_MAX);
	//玩家在单游金的状态下，无法点炮胡。
	//如果玩家中有处于双游金状态的玩家，则所有玩家都不能点炮

	for (int i = 0; i < m_GAME_PLAYER; i++)
	{
		if ((m_bUserOperateDoubleYouJinFlag[i] == true) || m_bUserOperateThreeYouJinFlag[i] == true)
		{
			m_conf.m_bZimoHu = true;
			break;
		}
		else
		{
			m_conf.m_bZimoHu = false;
		}
	}
	// log_info(" m_bUserDanYouJinState[%d] = %d", ChairID, m_bUserDanYouJinState[ChairID]);
	// log_info(" m_conf.m_bZimoHu = %d", m_conf.m_bZimoHu);
	if ((!m_conf.m_bZimoHu) && m_bUserOperateDanYouJinHu[ChairID] != true)
	{
		m_GameLogic.AddCard(pHc, bCard);
		iOpType |= Check_Hu(ChairID, *pHc, major.m_oFan);
		//if ((iOpType & TYPE_HU) && m_bUserHasJinNum[ChairID] == 3)
		//{//如果玩家手牌中有三张金，则不能胡点炮。
		//	iOpType &= (~TYPE_HU);
		//}

		if (iOpType & TYPE_HU)
		{//如果玩家A放炮时，玩家B选择不胡，那在B玩家吃 / 碰 / 杠 / 摸牌之前都不能再胡点炮
			m_bIsGuoZhangHuFlag[ChairID] = true;
		}
		if (m_bUserOperateGuoZhangHuFlag[ChairID] == true)	 //过张胡
		{
			iOpType &= (~TYPE_HU);
		}

		if (iOpType & TYPE_HU)
		{
			//如果玩家A放炮时，玩家B选择不胡，那在B玩家吃 / 碰 / 杠 / 摸牌之前都不能再胡点炮
			// m_bIsGuoZhangHuFlag[ChairID] = true;
			//云霄麻将三金倒胡牌必须摸牌之后才能胡，吃碰杠都不行
			if (major.m_oFan.Find(MJ_FAN_YUNXIAO_SANJINDAO))
			{
				major.m_oFan.Clear();
				iOpType &= (~TYPE_HU);
			}
		}

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
			/*如果为第一次摸牌,并且胡牌则为地胡*/
			if (!major.m_oFan.Find(MJ_FAN_YUNXIAO_SANJINDAO))
			{
				UserFirstOutCard(ChairID, iOpType, major.m_oFan);
			}
			if (major.m_oFan.Find(MJ_FAN_TYPE_DIHU) && major.m_oFan.Find(MJ_FAN_TYPE_QIANGGANGHU) && m_bGetCardCount[m_bBanker] == 1 && m_bCurChairID != m_bBanker)
			{//庄家第一次出牌出牌时，如果不是庄家明杠，则不能添加地胡类型。
				major.m_oFan.Remove(MJ_FAN_TYPE_DIHU);
			}
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
			if (IsQingYiSe(&m_HandCard[ChairID], ChairID)) //清一色
			{//清一色胡牌时，炮胡或自摸或游金都算双游金
			 // major.m_oFan.Add(MJ_FAN_YUNXIAO_DOUBLEYOUJIN);
				major.m_oFan.Add(MJ_FAN_TYPE_QINGYISE);
				//如果玩家胡牌清一色，则开启点炮包赔
			}
			else
			{
				if (m_QingYiSeDianPaoBaoPei == true)
				{
					m_conf_yunxiaomj.yun_xiao_dian_pao_bao_pei = false;
					m_QingYiSeDianPaoBaoPei = false;
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
			if (IsQingYiSe(&m_HandCard[ChairID], ChairID)) //清一色
			{//清一色胡牌时，炮胡或自摸或游金都算双游金
			 // major.m_oFan.Add(MJ_FAN_YUNXIAO_DOUBLEYOUJIN);
				major.m_oFan.Add(MJ_FAN_TYPE_QINGYISE);
				//如果玩家胡牌清一色，则开启点炮包赔
			}
		}
		if (iOpType & TYPE_HU)
		{
			UserQiangGangHu(ChairID, bCard, major.m_oFan);
			fan_num = Calc_Fan(ChairID, major.m_oFan);
		}
		m_GameLogic.ChuPai(pHc, bCard);
	}

	//设置玩家超时的定时器
	if (m_conf_yunxiaomj.yunxiao_zidong_chupai_hupai_pass == true)
	{
		switch (ChairID)
		{
		case 0:
			if (iOpType & TYPE_YX_YOUJINHU)
			{
				const int tmp_Timer_stamp = 15;
				TIMER_AGAIN(m_YouJinOperateHuYouJin_Timer_1, tmp_Timer_stamp);

			}
			else if (iOpType & TYPE_HU)
			{
				const int tmp_Timer_stamp = 15;
				TIMER_AGAIN(m_OutCardOutTimeOperateHu_Timer_0, tmp_Timer_stamp);
			}
			else if (iOpType != TYPE_NULL)
			{
				const int tmp_Timer_stamp = 15;
				TIMER_AGAIN(m_OutCardOutTimeOperatePass_Timer_0, tmp_Timer_stamp);
			}
			break;
		case 1:
			if (iOpType & TYPE_YX_YOUJINHU)
			{
				const int tmp_Timer_stamp = 15;
				TIMER_AGAIN(m_YouJinOperateHuYouJin_Timer_1, tmp_Timer_stamp);

			}
			else if (iOpType & TYPE_HU)
			{
				const int tmp_Timer_stamp = 15;
				TIMER_AGAIN(m_OutCardOutTimeOperateHu_Timer_1, tmp_Timer_stamp);
			}
			else if (iOpType != TYPE_NULL)
			{
				const int tmp_Timer_stamp = 15;
				TIMER_AGAIN(m_OutCardOutTimeOperatePass_Timer_1, tmp_Timer_stamp);
			}
			break;
		case 2:
			if (iOpType & TYPE_YX_YOUJINHU)
			{
				const int tmp_Timer_stamp = 15;
				TIMER_AGAIN(m_YouJinOperateHuYouJin_Timer_1, tmp_Timer_stamp);

			}
			else if (iOpType & TYPE_HU)
			{
				const int tmp_Timer_stamp = 15;
				TIMER_AGAIN(m_OutCardOutTimeOperateHu_Timer_2, tmp_Timer_stamp);
			}
			else if (iOpType != TYPE_NULL)
			{
				const int tmp_Timer_stamp = 15;
				TIMER_AGAIN(m_OutCardOutTimeOperatePass_Timer_2, tmp_Timer_stamp);
			}
			break;
		case 3:
			if (iOpType & TYPE_YX_YOUJINHU)
			{
				const int tmp_Timer_stamp = 15;
				TIMER_AGAIN(m_YouJinOperateHuYouJin_Timer_1, tmp_Timer_stamp);
			}
			else if (iOpType & TYPE_HU)
			{
				const int tmp_Timer_stamp = 15;
				TIMER_AGAIN(m_OutCardOutTimeOperateHu_Timer_3, tmp_Timer_stamp);
			}
			else if (iOpType != TYPE_NULL)
			{
				const int tmp_Timer_stamp = 15;
				TIMER_AGAIN(m_OutCardOutTimeOperatePass_Timer_3, tmp_Timer_stamp);
			}
			break;
		default:
			break;
		}
	}

	return iOpType;
}


void Table_yunxiaomajiang::CheckUserHandCards(_uint8 ChairID, _uint8 bCard[20], int cards_len)
{ 
	if (cards_len > 14)
	{
		proto::game::yunxiaomj::AckUserOutCardErr  Ack;
		Ack.set_card(0);
		Ack.set_chairid(ChairID);
		Ack.set_err_code(3);
		unicast(SER_CMD_OUT_CARD_ERR, ChairID, Ack);
		log_info("%s tid:%d uid:%d ChairID:%d Error CheckUserHandCards Out Two Card Err Ack = %s\n", __FUNCTION__, tid, GetPlayerUid(ChairID), ChairID, Ack.DebugString().c_str());
		for (int i = 0; i < m_HandCard[ChairID].ChangeableCardsLen; i++)
		{
			log_info("%s tid:%d Error  m_HandCard[ChairID = %d].ChangeableCards[i = %d] = %x", __FUNCTION__, tid, ChairID, i, m_HandCard[ChairID].ChangeableCards[i]);
		}
		for (int i = 0; i < m_stUserOutCard[ChairID].Size(); i++)
		{
			log_info("%s tid:%d  Error  m_stUserOutCard[ChairID = %d][i = %d] = %x", __FUNCTION__, tid, ChairID, i, m_stUserOutCard[ChairID][i]);
		}
		log_info("%s tid:%d++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++", __FUNCTION__, tid);
		for (int i = 0; i < cards_len; i++)
		{
			log_info("%s tid:%d ChairID:%d cards_len = %d bCard[i = %d] = %x", __FUNCTION__, tid, ChairID, cards_len, i, bCard[i]);
		}
		return;
	}


	bool bCheckUserHandCardErr = false;
	HandCards ServerHandCards = m_HandCard[ChairID];
	HandCards ClientHandCards;

	for (int i = 0; i < cards_len;i++)
	{
		if (bCard[i]== m_bJinCard)
		{
			m_GameLogic.AddCard(&ClientHandCards, m_bReplaceJinCard);

		}else if (bCard[i] == m_bReplaceJinCard)
		{
			m_GameLogic.AddCard(&ClientHandCards, m_bJinCard);
		}
		else
		{
			m_GameLogic.AddCard(&ClientHandCards, bCard[i]);

		}
	}

	if (ServerHandCards.ChangeableCardsLen != ClientHandCards.ChangeableCardsLen)
	{
		bCheckUserHandCardErr = true;
	}
	int cardindex = 0;
	for (int i = 0; i < ServerHandCards.ChangeableCardsLen;i++)
	{
		if (ServerHandCards.ChangeableCards[i] != ClientHandCards.ChangeableCards[i])
		{
			bCheckUserHandCardErr = true;
			cardindex = i;
			break;
		}
	}

	if (bCheckUserHandCardErr == true)
	{
		proto::game::yunxiaomj::AckUserOutCardErr  Ack;
		Ack.set_card(0);
		Ack.set_chairid(ChairID);
		Ack.set_err_code(3);
		unicast(SER_CMD_OUT_CARD_ERR, ChairID, Ack);
		log_info("%s tid:%d uid:%d ChairID:%d Error CheckUserHandCards Out Two Card Err Ack = %s\n", __FUNCTION__, tid, GetPlayerUid(ChairID), ChairID, Ack.DebugString().c_str());
		for (int i = 0; i < m_HandCard[ChairID].ChangeableCardsLen; i++)
		{
			log_info("%s tid:%d Error  m_HandCard[ChairID = %d].ChangeableCards[i = %d] = %x", __FUNCTION__, tid, ChairID, i, m_HandCard[ChairID].ChangeableCards[i]);
		}
		for (int i = 0; i < m_stUserOutCard[ChairID].Size(); i++)
		{
			log_info("%s tid:%d  Error  m_stUserOutCard[ChairID = %d][i = %d] = %x", __FUNCTION__, tid, ChairID, i, m_stUserOutCard[ChairID][i]);
		}
		log_info("%s tid:%d++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++", __FUNCTION__, tid)
		for (int i = 0; i < ClientHandCards.ChangeableCardsLen; i++)
		{
			log_info("%s tid:%d  Error  ClientHandCards[ChairID = %d][i = %d] = %x", __FUNCTION__, tid, ChairID, i, ClientHandCards.ChangeableCards[i]);
		}

	}

};

//抢杠胡操作优先级
void Table_yunxiaomajiang::RobGangPriority(_uint8 ChairID, _uint8 CardData, int OperateType)
{
	stop_operator_timer(ChairID);

	m_RobOperateNum++;
	m_RobHuDoneOp[ChairID] = OperateType;
	if (TYPE_HU == OperateType)
	{
		m_bRobHu = true;
		m_bAddFanFlag[m_WangGangPlayer] = true;

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

	log_info("%s tid:%d uid:%d chairid:%d OperateNum:%d RobHuNum:%d \n", __FUNCTION__,tid, GetPlayerUid(ChairID), ChairID, m_RobOperateNum, m_RobHuNum);
	if (m_RobOperateNum == m_RobHuNum)  //所有胡玩家都已经操作完毕
	{
		if (m_bRobHu)   //如果有人胡牌
		{
			if (!m_conf.m_bYiPaoDuoXiang)
			{
				for (_uint8 c = NextChair(m_bCurChairID); c != m_bCurChairID; c = NextChair(c))
				{
					if (m_bUserOperate[c] & TYPE_HU)
					{
						m_bIsHu[c] = true;
						UserOperateHu(c, m_RobWanGangCard);
						break;
					}
				}
			}
			if (m_RobWanGangCard == m_bReplaceJinCard)
			{
				m_RobWanGangCard = m_bJinCard;
			}
			if (m_WangGangPlayerOpType & (TYPE_ANGANG | TYPE_WANGANG))
				m_GameLogic.RemoveCard(&m_HandCard[m_WangGangPlayer], m_RobWanGangCard, 1);


			GameEnd();

		}
		else
		{
			UserOperateGang(m_WangGangPlayer, m_WangGangPlayerOpType, m_RobWanGangCard);
			OnUserGetCard(m_WangGangPlayer);
		}

	}
	if (m_RobOperateNum == m_RobHuNum)
	{
		m_bRobWanGang = false;
		m_bRobHu = false;
		m_RobOperateNum = 0;
		m_RobHuNum = 0;

		m_WangGangPlayer = INVALID_CHAIRID;
		m_WangGangPlayerOpType = TYPE_NULL;
		m_RobWanGangCard = INVALID_MJ_CARD;
		memset(&m_RobHuDoneOp, 0, sizeof(m_RobHuDoneOp));
		m_PassHuPlayer.Clear();

	}
}

void Table_yunxiaomajiang::UserOperateHu(_uint8 ChairID, _uint8 bHuCard)
{

	log_info("%s tid:%d uid:%d ChairID:%d hu FanNum:%d \n", __FUNCTION__,tid, GetPlayerUid(ChairID), ChairID, m_HuFanNums[ChairID]);
	_uint8 bCard = 0xFF;
	if (bHuCard == 0xFF)
	{
		bCard = m_bCurCard;
	}
	else
	{
		bCard = bHuCard;
	}

	BroadcastOperateResult(ChairID, TYPE_HU, bCard);
	if (!(m_bUserOperate[ChairID] & TYPE_HU))
	{
		log_error("tid:%d hu error, hu chairid:%d, m_bUserOperate[%d]:%d \n",tid, ChairID, ChairID, m_bUserOperate[ChairID]);
		//return;
	}

	m_bIsHu[ChairID] = true;
	int userscore[GAME_PLAYER] = { 0 };
	Calc_Hu(ChairID, bCard, userscore);

	Calc_Score(m_stHuScore, userscore);

	for (int i = 0; i < GAME_PLAYER; i++)
	{
		log_debug("userscore[i] = %d", userscore[i]);
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

void Table_yunxiaomajiang::BroadcastOperateResult(_uint8 bOpChairID, int iOpType, _uint8 bOpCard)
{
	//广播发送操作结果	
	proto::game::AckBcOpResult stOpresult;
	stOpresult.set_chairid(bOpChairID);
	stOpresult.set_outcardchairid(m_bCurChairID);
	stOpresult.set_optype(iOpType);
	stOpresult.set_card(bOpCard);
	if (iOpType & TYPE_HU)
	{
		if (m_bUserOperateQiangJinHuFlag[bOpChairID]==true)
		{
			stOpresult.set_yunxiaoyoujintype(32);
		}
		else
		{
			if (m_bRobHu)
			{
				if (m_bUserHuTypeoFan[bOpChairID].Find(MJ_FAN_TYPE_QIANGGANGHU))
				{
					stOpresult.set_yunxiaoyoujintype(0x40);
				}
			}
			else if (bOpChairID == m_bCurChairID)
			{//自摸
				
				if (m_bUserHuTypeoFan[bOpChairID].Find(MJ_FAN_TYPE_TIANHU))
				{
					stOpresult.set_yunxiaoyoujintype(0x100);
				}
				else if (m_bUserHuTypeoFan[bOpChairID].Find(MJ_FAN_TYPE_QINGYISE))
				{
					stOpresult.set_yunxiaoyoujintype(0x8000);
				}
				else if (m_bUserHuTypeoFan[bOpChairID].Find(MJ_FAN_YUNXIAO_SANJINDAO))
				{//
					stOpresult.set_yunxiaoyoujintype(0x80);
				}
				else if (m_bUserHuTypeoFan[bOpChairID].Find(MJ_FAN_YUNXIAO_19HU))
				{
					stOpresult.set_yunxiaoyoujintype(0x400);
				}
				else if (m_bUserHuTypeoFan[bOpChairID].Find(MJ_FAN_YUNXIAO_MINGGANGHU))
				{
					stOpresult.set_yunxiaoyoujintype(0x800);
				}
				else if (m_bUserHuTypeoFan[bOpChairID].Find(MJ_FAN_YUNXIAO_ANGANGHU))
				{
					stOpresult.set_yunxiaoyoujintype(0x1000);
				}
				else if (m_bUserHuTypeoFan[bOpChairID].Find(MJ_FAN_YUNXIAO_BUDAHU))
				{
					stOpresult.set_yunxiaoyoujintype(0x2000);
				}
				else if (m_bUserHuTypeoFan[bOpChairID].Find(MJ_FAN_TYPE_QIDUI))
				{
					stOpresult.set_yunxiaoyoujintype(0x4000);
				}
				else if (m_bUserHuTypeoFan[bOpChairID].Find(MJ_FAN_TYPE_PINGHU))
				{
					stOpresult.set_yunxiaoyoujintype(0x2);
				}			
			}
			else
			{
				if (m_bUserHuTypeoFan[bOpChairID].Find(MJ_FAN_TYPE_DIHU))
				{
					stOpresult.set_yunxiaoyoujintype(0x200);
				}
				else if (m_bUserHuTypeoFan[bOpChairID].Find(MJ_FAN_TYPE_QINGYISE))
				{
					stOpresult.set_yunxiaoyoujintype(0x8000);
				}
				else if (m_bUserHuTypeoFan[bOpChairID].Find(MJ_FAN_YUNXIAO_19HU))
				{
					stOpresult.set_yunxiaoyoujintype(0x400);
				}
				else if (m_bUserHuTypeoFan[bOpChairID].Find(MJ_FAN_YUNXIAO_BUDAHU))
				{
					stOpresult.set_yunxiaoyoujintype(0x2000);
				}
				else if (m_bUserHuTypeoFan[bOpChairID].Find(MJ_FAN_TYPE_QIDUI))
				{
					stOpresult.set_yunxiaoyoujintype(0x4000);
				}
				else if (m_bUserHuTypeoFan[bOpChairID].Find(MJ_FAN_TYPE_PINGHU))
				{
					stOpresult.set_yunxiaoyoujintype(0x1);
				}

			}
		}

	}
	SetRobGangHu_Flag(bOpChairID, iOpType, stOpresult);

	broadcast(SERVER_OPERATE_RESULT, stOpresult);
	log_info("%s tid:%d uid:%d chairid:%d outchairid:%d optype:0x%x card:0x%02x\n", __FUNCTION__,tid, GetPlayerUid(bOpChairID), bOpChairID, m_bCurChairID, iOpType, bOpCard);

}

void Table_yunxiaomajiang::Proto_GameScene_Operate(_uint8 ChairID, Player* player, proto::game::AckGameScene& stGameScene)
{
	if (m_bMultiOpUserNum != 0 && m_bMultiUserDoneOp[ChairID] != TYPE_NULL)
	{
		log_info("%s tid:%d user has selected uid:%d chairid:%d iOpType:0x%x!\n", __FUNCTION__,tid, GetPlayerUid(ChairID), ChairID, m_bMultiUserDoneOp[ChairID]);
		return;
	}
	if (m_bUserOperate[ChairID] & TYPE_HU || m_bUserOperate[ChairID] & TYPE_YX_YOUJINHU)
	{
		if (m_bUserOperateThreeYouJinFlag[ChairID] == true)
		{
			stGameScene.set_optypehutype(0x10);
		}
		if (m_bUserOperateDoubleYouJinFlag[ChairID] == true)
		{
			stGameScene.set_optypehutype(0x08);
		}
		//满足单游金状态,，并且可以游，则向客户端发送单游金
		if (m_bUserOperateDanYouJinHu[ChairID] == true)
		{
			stGameScene.set_optypehutype(0x04);
		}
		if (m_bUserOperateQiangJinHuFlag[ChairID] == true)
		{
			stGameScene.set_optypehutype(0x20);
		}
		if (m_bRobHu)
		{
			if (m_bUserHuTypeoFan[ChairID].Find(MJ_FAN_TYPE_QIANGGANGHU))
			{
				stGameScene.set_optypehutype(0x40);
			}
		}
		else if (ChairID == m_bCurChairID)
		{//自摸
			if (m_bUserOperate[ChairID] & TYPE_HU)
			{
				if (m_bUserHuTypeoFan[ChairID].Find(MJ_FAN_TYPE_TIANHU))
				{
					stGameScene.set_optypehutype(0x100);
				}
				else if (m_bUserHuTypeoFan[ChairID].Find(MJ_FAN_TYPE_QINGYISE))
				{
					stGameScene.set_optypehutype(0x8000);
				}
				else if (m_bUserHuTypeoFan[ChairID].Find(MJ_FAN_YUNXIAO_SANJINDAO))
				{//
					stGameScene.set_optypehutype(0x80);
				}
				else if (m_bUserHuTypeoFan[ChairID].Find(MJ_FAN_YUNXIAO_19HU))
				{
					stGameScene.set_optypehutype(0x400);
				}
				else if (m_bUserHuTypeoFan[ChairID].Find(MJ_FAN_YUNXIAO_MINGGANGHU))
				{
					stGameScene.set_optypehutype(0x800);
				}
				else if (m_bUserHuTypeoFan[ChairID].Find(MJ_FAN_YUNXIAO_ANGANGHU))
				{
					stGameScene.set_optypehutype(0x1000);
				}
				else if (m_bUserHuTypeoFan[ChairID].Find(MJ_FAN_YUNXIAO_BUDAHU))
				{
					stGameScene.set_optypehutype(0x2000);
				}
				else if (m_bUserHuTypeoFan[ChairID].Find(MJ_FAN_TYPE_QIDUI))
				{
					stGameScene.set_optypehutype(0x4000);
				}
				else if (m_bUserHuTypeoFan[ChairID].Find(MJ_FAN_TYPE_PINGHU))
				{
					stGameScene.set_optypehutype(0x2);
				}
			}
		}
		else
		{
			if (m_bUserOperate[ChairID] & TYPE_HU)
			{
				if (m_bUserHuTypeoFan[ChairID].Find(MJ_FAN_TYPE_QIANGGANGHU))
				{
					stGameScene.set_optypehutype(0x40);
				}else
				if (m_bUserHuTypeoFan[ChairID].Find(MJ_FAN_TYPE_DIHU))
				{
					stGameScene.set_optypehutype(0x200);
				}
				else if (m_bUserHuTypeoFan[ChairID].Find(MJ_FAN_TYPE_QINGYISE))
				{
					stGameScene.set_optypehutype(0x8000);
				}
				else if (m_bUserHuTypeoFan[ChairID].Find(MJ_FAN_YUNXIAO_SANJINDAO))
				{//
					stGameScene.set_optypehutype(0x80);
				}
				else if (m_bUserHuTypeoFan[ChairID].Find(MJ_FAN_YUNXIAO_19HU))
				{
					stGameScene.set_optypehutype(0x400);
				}
				else if (m_bUserHuTypeoFan[ChairID].Find(MJ_FAN_YUNXIAO_BUDAHU))
				{
					stGameScene.set_optypehutype(0x2000);
				}
				else if (m_bUserHuTypeoFan[ChairID].Find(MJ_FAN_TYPE_QIDUI))
				{
					stGameScene.set_optypehutype(0x4000);
				}
				else if (m_bUserHuTypeoFan[ChairID].Find(MJ_FAN_TYPE_PINGHU))
				{
					stGameScene.set_optypehutype(0x1);
				}
			}
		}
	}
	stGameScene.set_optype(m_bUserOperate[ChairID]);
	if (m_bUserOperate[ChairID] & TYPE_ANGANG || m_bUserOperate[ChairID] & TYPE_WANGANG)
	{
		MJ_opinfo stGang;
		GetGangInfo(ChairID, stGang);
		for (int len = 0; len < stGang.Size(); len++)
		{
			proto::game::tagGangInfo *pstGang = stGameScene.add_ganginfo();
			pstGang->set_card(stGang[len].cbCard);
			pstGang->set_state(stGang[len].nState);
		}
	}
	if ((m_bUserOperate[ChairID] & TYPE_LISTEN) && !(m_bUserOperate[ChairID] & TYPE_HU))
	{
		//打包听牌信息
		tagListenInfos stListenInfo[14];
		_uint8 bListenLen = 0;
		//胡牌不用判断听牌，自动胡牌不用判断听牌
		if (!(m_bUserOperate[ChairID] & TYPE_HU))
		{
			m_bUserOperate[ChairID] |= GetListeningInfo(ChairID, stListenInfo, bListenLen);
		}

		for (int j = 0; j < bListenLen; j++)
		{
			PackageTingInfo(ChairID, stListenInfo, j, stGameScene.add_listeninfo());
		}
	}
}


void Table_yunxiaomajiang::Proto_Game_OpNotify(_uint8 ChairID, _uint8 CurChairID, _uint8 bCard, proto::game::AckOpNotify& stNotify)
{

	stNotify.Clear();
	stNotify.set_chairid(ChairID);
	//非提示玩家,下面的协议字段为空
	if (CurChairID == ChairID)
	{
		stNotify.set_optype(m_bUserOperate[ChairID]);
		stNotify.set_card(bCard);

		if (m_bUserOperate[ChairID] & TYPE_YX_YOUJINHU)
		{
			if (m_bUserOperateThreeYouJinFlag[ChairID] == true && m_bUserOperate[ChairID] != TYPE_NULL && m_bCurChairID == PreChair(ChairID))
			{
				stNotify.set_optypehutype(16);
			}
			else if (m_bUserOperateDoubleYouJinFlag[ChairID] == true && m_bUserOperate[ChairID] != TYPE_NULL&& m_bCurChairID == PreChair(ChairID))
			{
				stNotify.set_optypehutype(8);
			}
			else if (m_bUserOperateDanYouJinHu[ChairID] == true && m_bUserOperate[ChairID] != TYPE_NULL&& m_bCurChairID == PreChair(ChairID))
			{
				stNotify.set_optypehutype(4);
			}
		}
		else if (m_bUserOperate[ChairID] & TYPE_HU)
		{
			if (m_bUserHuTypeoFan[ChairID].Find(MJ_FAN_TYPE_QIANGGANGHU))
			{
				stNotify.set_optypehutype(0x40);

			}else if (m_bUserHuTypeoFan[ChairID].Find(MJ_FAN_TYPE_DIHU))
			{
				stNotify.set_optypehutype(0x200);
			}
			else if (m_bUserHuTypeoFan[ChairID].Find(MJ_FAN_TYPE_QINGYISE))
			{
				stNotify.set_optypehutype(0x8000);
			}
			else if (m_bUserHuTypeoFan[ChairID].Find(MJ_FAN_YUNXIAO_19HU))
			{
				stNotify.set_optypehutype(0x400);
			}
			else if (m_bUserHuTypeoFan[ChairID].Find(MJ_FAN_YUNXIAO_BUDAHU))
			{
				stNotify.set_optypehutype(0x2000);
			}
			else if (m_bUserHuTypeoFan[ChairID].Find(MJ_FAN_TYPE_QIDUI))
			{
				stNotify.set_optypehutype(0x4000);
			}
			else if (m_bUserHuTypeoFan[ChairID].Find(MJ_FAN_TYPE_PINGHU))
			{
				stNotify.set_optypehutype(0x1);
			}
		
		}
	
	}
}

void Table_yunxiaomajiang::Proto_Game_UserGetCard(_uint8 ChairID, _uint8 MoChairID, _uint8 bCard, proto::game::AckUserGetCard& stUsergetcard)
{
	//_uint8 tempChairid = m_SeatPlayer[ChairID].player->seatid;
	stUsergetcard.Clear();

	stUsergetcard.set_chairid(MoChairID);
	stUsergetcard.set_leftcardnum(m_CardPool.Count());
	if (m_bUserOperate[MoChairID]&TYPE_HU)
	{

		if (m_bUserHuTypeoFan[MoChairID].Find(MJ_FAN_TYPE_TIANHU))
		{
			stUsergetcard.set_optypehutype(0x100);
		}
		else if (m_bUserHuTypeoFan[MoChairID].Find(MJ_FAN_TYPE_QINGYISE))
		{
			stUsergetcard.set_optypehutype(0x8000);
		}
		else if (m_bUserHuTypeoFan[MoChairID].Find(MJ_FAN_YUNXIAO_SANJINDAO))
		{//
			stUsergetcard.set_optypehutype(0x80);
		}
		else if (m_bUserHuTypeoFan[MoChairID].Find(MJ_FAN_YUNXIAO_19HU))
		{
			stUsergetcard.set_optypehutype(0x400);
		}
		else if (m_bUserHuTypeoFan[MoChairID].Find(MJ_FAN_YUNXIAO_MINGGANGHU))
		{
			stUsergetcard.set_optypehutype(0x800);
		}
		else if (m_bUserHuTypeoFan[MoChairID].Find(MJ_FAN_YUNXIAO_ANGANGHU))
		{
			stUsergetcard.set_optypehutype(0x1000);
		}
		else if (m_bUserHuTypeoFan[MoChairID].Find(MJ_FAN_YUNXIAO_BUDAHU))
		{
			stUsergetcard.set_optypehutype(0x2000);
		}
		else if (m_bUserHuTypeoFan[MoChairID].Find(MJ_FAN_TYPE_QIDUI))
		{
			stUsergetcard.set_optypehutype(0x4000);
		}
		else if (m_bUserHuTypeoFan[MoChairID].Find(MJ_FAN_TYPE_PINGHU))
		{
			stUsergetcard.set_optypehutype(0x02);
		}
		
	}
	//如果是摸牌用户，则打包摸牌、听牌等数据，非摸牌用户则不发送这些数据
	if (ChairID == MoChairID)
	{
		stUsergetcard.set_card(bCard);

		if (m_bUserOperate[MoChairID] & TYPE_ANGANG || m_bUserOperate[MoChairID] & TYPE_WANGANG)
		{
			MJ_opinfo stGang;
			GetGangInfo(MoChairID, stGang);
			//log_info("stGang. size() = %d", stGang.Size());
			for (int i = 0; i < stGang.Size(); i++)
			{
				//log_info("玩家还没胡牌,此时暗杠牌有:0x%02x \n", stGang[i].cbCard);
				log_info("tid:%d %s uid:%d chairID:%d angang:0x%02x\n",tid, __FUNCTION__, GetPlayerUid(ChairID), ChairID, stGang[i].cbCard);
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
		//log_info("1111 Proto_Game_UserGetCard m_bUserOperate[%d] = %d", MoChairID, m_bUserOperate[MoChairID]);
		stUsergetcard.set_optype(m_bUserOperate[MoChairID]);
		if (m_bUserOperate[MoChairID] & TYPE_LISTEN)
		{
			//打包听牌信息
			for (int j = 0; j < bListenLen; j++)
			{
				PackageTingInfo(ChairID, stListenInfo, j, stUsergetcard.add_listeninfo());

			}
		}
	}
}

bool Table_yunxiaomajiang::EstimateUserRespond(_uint8 bOutChairID, _uint8 bOutCard)
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
		m_bUserHuTypeoFan[i].Clear();
		m_bUserOperate[i] = OutCardEstimate(i, bOutCard, major, m_HuFanNums[i]);
		m_bUserHuTypeoFan[i] = major.m_oFan;
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

				Proto_Game_OpNotify(i, j, bOutCard, stNotify);
				unicast(SERVER_OPERATE_NOTIFY, j, stNotify);
				//保存录像
				std::string str = stNotify.SerializeAsString();
				save_video_data(SERVER_OPERATE_NOTIFY, str);
			}
		}
		if (m_bUserOperate[i] != 0)
		{
			log_info("%s tid:%d uid:%d chairid:%d Operate:0x%x, totalop:%d \n", __FUNCTION__, tid, GetPlayerUid(i), i, m_bUserOperate[i], m_bMultiOpUserNum);
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
