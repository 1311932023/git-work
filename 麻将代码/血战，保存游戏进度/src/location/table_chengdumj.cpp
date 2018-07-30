#include "table_chengdumj.h"

#include <time.h>
#include<cstdlib>
#include<ctime>

#include "../zjh.h"
#include "../common/log.h"
#include "../trustee/CTrustee.h"
#include "../trustee/CXueZhanTrustee.h"
#include "../match.h"
#include "../libnormalmahjong/common.h"


extern ZJH zjh;
extern Log log;

#define MAX_RAND_NUM 10

struct SGSI_SortNodes
{
	_uint8 ChairID;
	int iTotalScore;
};

Table_chengdumj::Table_chengdumj()
{	
	m_GameEndTimes = 0;

	m_bFirstCard = false;
	memset(m_bIsPassHu, false, sizeof(m_bIsPassHu));
	memset(m_UserOtherHu, 0, sizeof(m_UserOtherHu));
	memset(m_bOtherHu, 0, sizeof(m_bOtherHu));
	m_nHaveDingQueNum = 0;
	m_nHaveHuanZhangNum = 0;
	m_nYiPaoDuoXiangCharid = INVALID_CHAIRID;
	m_nLastHuPaiNum = 0;
	m_nZhiGangChairID = INVALID_CHAIRID;
	m_nHuPaiChairid = INVALID_CHAIRID;
	nOprOpType2 = 0;
	m_nJiChuDiFen = 1;
    // 新增信息
	for (int i = 0; i < GAME_PLAYER; i++)
	{

		m_bCanDiHu[i] = 0;
		m_bChuJu[i] = false;
        // 玩家金币
		m_nGouPaiNum[i] = 0;
        m_nPlayerMoney[i] = 0;
        m_nHuPaiShunXu[i] = INVALID_CHAIRID;
		m_UserDingQueSe[i]= INVALID_MJ_CARD;
        m_nDaJiao[i] = 0;
		m_nDaJiaoFan[i] = 0;
		m_nTuiGang[i] = 0;
        m_nGangScore[i] = 0;
		m_nZhuanYiGang[i] = 0;
		m_nLastHuScore[i] = 0;
		m_nHuCard[i] = INVALID_MJ_CARD;
		m_nHuCardShunXu[i] = INVALID_MJ_CARD;
		m_nZiMoHuCard[i] = INVALID_MJ_CARD;
		m_nPaoHuCard[i] = INVALID_MJ_CARD;
        m_nHuType[i] = 3;
		for (int j = 0; j < 3; j++)
		{
			m_UserHuanZhangPai[i][j]= INVALID_MJ_CARD;
		}
		m_stQiHuInfo[i].InitQiHuInfo();
		m_stGangZhuanYiInfo[i].InitGangZhuanYi();
		m_stDianPaoInfo[i].InitDianPaoInfo();

		for (int iTemp2 = 0; iTemp2 < 5; iTemp2++)
		{
			for (int iTemp3 = 0; iTemp3 < 10; iTemp3++)
			{
				m_bQiGang[i][iTemp2][iTemp3] = false;
			}
		}
		m_ShuFenSeatID[i].clear();
	}

	m_DingQue_Timer_stamp = 10;
	m_DingQue_Timer_running = false;
	m_DingQue_Timer.data = this;
	ev_timer_init(&m_DingQue_Timer, Table_chengdumj::DingQue_Timer_cb, m_DingQue_Timer_stamp, 0);

	m_HuanZhang_Timer_stamp = 10;
	m_HuanZhang_Timer_running = false;
	m_HuanZhang_Timer.data = this;
	ev_timer_init(&m_HuanZhang_Timer, Table_chengdumj::HuanZhang_Timer_cb, m_HuanZhang_Timer_stamp, m_HuanZhang_Timer_stamp);

	srand((unsigned)time(NULL));
	for (int i = 0; i < GAME_PLAYER - 1; i++)
	{
		double t = (rand() % 20) / 10.0f + 2.0;
		robot_dingque_stamp[i] = t;
		robot_dingque_timer[i].data = this;
		robot_dingque_timer_running[i] = false;
		ev_timer_init(&robot_dingque_timer[i], Table_chengdumj::robot_dingque_Timer_cb, robot_dingque_stamp[i], 0);
	}
	m_trustee.reset(new CXueZhanTrustee(this));
} 

Table_chengdumj::~Table_chengdumj()
{
	TIMER_STOP(m_DingQue_Timer);
	TIMER_STOP(m_HuanZhang_Timer);
}

void Table_chengdumj::ConfInit_Remote(const Json::Value& val)
{
	Table::ConfInit_Remote(val);

	if (json_check_play_type_id(val))
	{
		MJ_SUB_ID SubID;
		/***************************************************************************/
		json_get_play_type_id(val, 1028, SubID);
		// 血战到底
		if (SubID.Find(1028001))
		{
			log_info("%s tid:%d 1028001 \n", __FUNCTION__, tid);
			m_conf_js.iGameType = 0;    
		}
		// 血流成河
		if (SubID.Find(1028002)) 
		{
			log_info("%s tid:%d 1028002 \n", __FUNCTION__, tid);
			m_conf_js.iGameType = 1;
		}
		// 常规玩法
		if (SubID.Find(1028003)) 
		{
			log_info("%s tid:%d 1028003 \n", __FUNCTION__, tid);
			m_conf_js.iGameType = 2;
		}
		m_room_config += SubID;
		/***************************************************************************/
		json_get_play_type_id(val, 1012, SubID);
		// 是否一炮多响
		if (SubID.Find(1012001))
		{
			log_info("%s tid:%d 1012001 \n", __FUNCTION__, tid);
			m_conf_js.bYiPaoDuoXiang = true;    
		}
		// 是否抢杠胡
		if (SubID.Find(1012002)) 
		{
			log_info("%s tid:%d 1012002 \n", __FUNCTION__, tid);
			m_conf_js.bQiangGangHu = true;
		}
		m_room_config += SubID;
		/***************************************************************************/
		json_get_play_type_id(val, 1027, SubID);
		// 是否换三张
		if (SubID.Find(1027001))
		{
			log_info("%s tid:%d 1027001 \n", __FUNCTION__, tid);
			m_conf_js.bHuanSanZhang = true;    
		}
		m_room_config += SubID;
		/***************************************************************************/
		json_get_play_type_id(val, 1077, SubID);
		// 点杠花一家有
		if (SubID.Find(1077001))
		{
			log_info("%s tid:%d 1077001 \n", __FUNCTION__, tid);
			m_conf_js.bAllDianGangHua = false;    
		}
		// 点杠花家家有
		if (SubID.Find(1077002))
		{
			log_info("%s tid:%d 1077002 \n", __FUNCTION__, tid);
			m_conf_js.bAllDianGangHua = true;
		}
		m_room_config += SubID;
		/***************************************************************************/
		json_get_play_type_id(val, 1087, SubID);
		// 自摸加番
		if (SubID.Find(1087001))
		{
			log_info("%s tid:%d 1087001 \n", __FUNCTION__, tid);
			m_conf_js.bZiMoJiaFan = false;  
			m_conf_js.bZiMoJiaDi = true;
		}
		// 自摸加底
		if (SubID.Find(1087002))
		{
			log_info("%s tid:%d 1087002 \n", __FUNCTION__, tid);
			m_conf_js.bZiMoJiaFan = true; 
			m_conf_js.bZiMoJiaDi = false;
		}
		m_room_config += SubID;
		/***************************************************************************/
		json_get_play_type_id(val, 1089, SubID);
		// 封顶2番
		if (SubID.Find(1089001))
		{
			log_info("%s tid:%d 1089001 \n", __FUNCTION__, tid);
			m_conf_js.iFengDingFan = 2;  
		}
		// 封顶3番
		if (SubID.Find(1089002))
		{
			log_info("%s tid:%d 1089002 \n", __FUNCTION__, tid);
			m_conf_js.iFengDingFan = 3;  
		}
		// 封顶4番
		if (SubID.Find(1089003))
		{
			log_info("%s tid:%d 1089003 \n", __FUNCTION__, tid);
			m_conf_js.iFengDingFan = 4;  
		}
		// 封顶5番
		if (SubID.Find(1089004))
		{
			log_info("%s tid:%d 1089004 \n", __FUNCTION__, tid);
			m_conf_js.iFengDingFan = 5;  
		}
		m_room_config += SubID;
		/***************************************************************************/
		json_get_play_type_id(val, 1163, SubID);
		// 妖九将对
		if (SubID.Find(1163001))
		{
			log_info("%s tid:%d 1163001 \n", __FUNCTION__, tid);
			m_conf_js.bYaoJiuJiangDui = true;  
		}
		// 门清中张
		if (SubID.Find(1163002))
		{
			log_info("%s tid:%d 1163002 \n", __FUNCTION__, tid);
			m_conf_js.bMenQingZhongZhang = true;  
		}
		// 天地胡
		if (SubID.Find(1163003))
		{
			log_info("%s tid:%d 1163003 \n", __FUNCTION__, tid);
			m_conf_js.bTianDiHu = true;  
		}
		// 点炮可平胡
		if (SubID.Find(1163004))
		{
			log_info("%s tid:%d 1163004 \n", __FUNCTION__, tid);
			m_conf_js.bDianPaoPingHu = true;  
		}
		// 一条龙
		if (SubID.Find(1163005))
		{
			log_info("%s tid:%d 1163005 \n", __FUNCTION__, tid);
			m_conf_js.bYiTiaoLong = true;  
		}
		// 刮擦
		if (SubID.Find(1163006))
		{
			log_info("%s tid:%d 1163006 \n", __FUNCTION__, tid);
			m_conf_js.bGuaCa = true;  
		}

		// 对对胡2番
		if (SubID.Find(1163007))
		{
			log_info("%s tid:%d 1163007 \n", __FUNCTION__, tid);
			m_conf_js.iDuiDuiHuFan = 2;
			m_chongqingmj_Fantype_to_FanNum.SetFan(MJ_FAN_TYPE_PENGPENGHU, m_conf_js.iDuiDuiHuFan);
			m_chongqingmj_Fantype_to_FanNum.SetFan(MJ_FAN_TYPE_QINGDUI, 4);
		}

		// 夹心五
		if (SubID.Find(1163008))
		{
			log_info("%s tid:%d 1163008 \n", __FUNCTION__, tid);
			m_conf_js.bJiaXinWu = true;
		}

		// 海底花
		if (SubID.Find(1163009))
		{
			log_info("%s tid:%d 1163009 \n", __FUNCTION__, tid);
			m_conf_js.bHaiDiHuaJiaFan = true;
		}

		// 海底花
		if (SubID.Find(1163010))
		{
			log_info("%s tid:%d 1163010 \n", __FUNCTION__, tid);
			m_conf_js.bHaiDiPaoJiaFan = true;
		}

		m_room_config += SubID;
		/***************************************************************************/
		json_get_play_type_id(val, 1166, SubID);
		// 成都三人
		if (SubID.Find(3))
		{
			log_info("%s tid:%d 3 \n", __FUNCTION__, tid);
			m_conf_js.iPlayNum = 3;  
		}
		m_room_config += SubID;
		/***************************************************************************/
		json_get_play_type_id(val, 1167, SubID);
		// 条桶万
		if (SubID.Find(1167001))
		{
			log_info("%s tid:%d 1167001 \n", __FUNCTION__, tid);
			m_conf_js.iPaiZhangShu = 108;  
		}
		// 条桶
		if (SubID.Find(1167002))
		{
			log_info("%s tid:%d 1167002 \n", __FUNCTION__, tid);
			m_conf_js.iPaiZhangShu = 72;  
		}
		m_room_config += SubID;
		/***************************************************************************/
		json_get_play_type_id(val, 1168, SubID);
		// 天胡4番
		if (SubID.Find(1168001))
		{
			log_info("%s tid:%d 1168001 \n", __FUNCTION__, tid);
			m_conf_js.iTianHuFan = 4;
			m_chongqingmj_Fantype_to_FanNum.SetFan(MJ_FAN_TYPE_TIANHU, m_conf_js.iTianHuFan);
		}
		// 地胡3番
		if (SubID.Find(1168002))
		{
			log_info("%s tid:%d 1168002 \n", __FUNCTION__, tid);
			m_conf_js.iDiHuFan = 3;
			m_chongqingmj_Fantype_to_FanNum.SetFan(MJ_FAN_TYPE_DIHU, m_conf_js.iDiHuFan);
		}
		m_room_config += SubID;
		/***************************************************************************/
		json_get_play_type_id(val, 1169, SubID);
		// 底分1分
		if (SubID.Find(1169001))
		{
			log_info("%s tid:%d 1169001 \n", __FUNCTION__, tid);
			m_nJiChuDiFen = 1;
		}
		// 底分2分
		if (SubID.Find(1169002))
		{
			log_info("%s tid:%d 1169002 \n", __FUNCTION__, tid);
			m_nJiChuDiFen = 2;
		}
		// 底分3分
		if (SubID.Find(1169003))
		{
			log_info("%s tid:%d 1169003 \n", __FUNCTION__, tid);
			m_nJiChuDiFen = 3;
		}
		// 底分4分
		if (SubID.Find(1169004))
		{
			log_info("%s tid:%d 1169004 \n", __FUNCTION__, tid);
			m_nJiChuDiFen = 5;
		}
		// 底分5分
		if (SubID.Find(1169005))
		{
			log_info("%s tid:%d 1169005 \n", __FUNCTION__, tid);
			m_nJiChuDiFen = 10;
		}
		m_room_config += SubID;
		/***************************************************************************/
	}

    // 条桶万
	if (108 == m_conf_js.iPaiZhangShu)
	{
		m_CardPool.Set(CARD_POOL_NO_WIND_DRAGON_FLOWER);
	}
	if (72 == m_conf_js.iPaiZhangShu)
	{
		m_CardPool.Set(CARD_POOL_TIAOTONG);
	}
	
	m_CardPool.InitPool();
    // 玩家数量
	if (4 == m_conf_js.iPlayNum)
	{
		m_GAME_PLAYER = 4;
	}
	if (3 == m_conf_js.iPlayNum)
	{
		m_GAME_PLAYER = 3;
	}
    // 是否支持吃
	m_conf.m_bSupportChi = false;
    // 是否只能自摸胡
    m_conf.m_bZimoHu = false;
    // 是否可以一炮多响
	if (m_conf_js.bYiPaoDuoXiang)
	{
		m_conf.m_bYiPaoDuoXiang = true;
	}
    else
	{
		m_conf.m_bYiPaoDuoXiang = false;
	}
    // 是否可以抢杠胡
    m_conf.m_bQiangGangHu = true;
    // 是否可以杠上炮
    m_conf.m_bGangShangPao = true;	
    // 是否可以抢直杠
    m_conf.m_bQiangZhiGang = false;
    // 是否荒庄荒杠	
    m_conf.m_bHuangGang = false;
    // 是否强制抢杠胡
    m_conf.m_bForceHuQiangGangHu = false;
    // 暗杠点数
    m_conf.m_AnGangPoint = 2;
    // 直杠点数
    m_conf.m_ZhiGangPoint = 2;
    // 弯杠点数
    m_conf.m_WanGangPoint = 1;
    // 是否可以胡七对
    m_conf.m_bHu7dui = true;
    // 是否支持飘
    m_conf.m_bSupportPiao = false;
    // 一炮多响是否自动胡牌
    m_conf.m_bFourceHuMutilHu = false;
	// 杠分是否立即结算
	m_bGangCalcImme = false;
    // 是否支持定缺
    m_conf.m_bSupportDingQue = true;

	m_ft2fn = &m_chongqingmj_Fantype_to_FanNum;
}

// 成都麻将配置项
void Table_chengdumj::ChengDuMJPeiZhi(proto::game::chengdumj::tagCDGameScence& stcqGameScence)
{
	stcqGameScence.set_gametype(m_conf_js.iGameType);
	stcqGameScence.set_byipaoduoxiang(m_conf_js.bYiPaoDuoXiang);
	stcqGameScence.set_bqiangganghu(m_conf_js.bQiangGangHu);
	stcqGameScence.set_bhuansanzhang(m_conf_js.bHuanSanZhang);
	stcqGameScence.set_balldianganghua(m_conf_js.bAllDianGangHua);
	stcqGameScence.set_bzimojiafanjiadi(m_conf_js.bZiMoJiaDi);
	stcqGameScence.set_bhufanfengding(m_conf_js.iFengDingFan);
	stcqGameScence.set_byaojiujiangdui(m_conf_js.bYaoJiuJiangDui);
	stcqGameScence.set_bmenqingzhongzhang(m_conf_js.bMenQingZhongZhang);
	stcqGameScence.set_btiandihu(m_conf_js.bTianDiHu);
	stcqGameScence.set_bdianpaokepinghu(m_conf_js.bDianPaoPingHu);
	stcqGameScence.set_byitiaolong(m_conf_js.bYiTiaoLong);
	stcqGameScence.set_bguaca(m_conf_js.bGuaCa);
	stcqGameScence.set_nplaynum(m_conf_js.iPlayNum);
	stcqGameScence.set_npaizhangshu(m_conf_js.iPaiZhangShu);
	stcqGameScence.set_ntianhufan(m_conf_js.iTianHuFan);
	stcqGameScence.set_ndihufan(m_conf_js.iDiHuFan);
	if (2 == m_conf_js.iDuiDuiHuFan)
	{
		stcqGameScence.set_nduiduihufan(1);
	}
	else
	{
		stcqGameScence.set_nduiduihufan(0);
	}
	stcqGameScence.set_bjiaxinwu(m_conf_js.bJiaXinWu);
	stcqGameScence.set_bhaidihuajiafan(m_conf_js.bHaiDiHuaJiaFan);
	stcqGameScence.set_bhaidipaojiafan(m_conf_js.bHaiDiPaoJiaFan);
	stcqGameScence.set_bdifen(m_nJiChuDiFen);
}

void Table_chengdumj::InitData()
{
	Table::InitData();
	m_bFirstCard = false;
	memset(m_UserOtherHu, 0, sizeof(m_UserOtherHu));
	memset(m_bIsPassHu, false, sizeof(m_bIsPassHu));
	memset(m_bOtherHu, 0, sizeof(m_bOtherHu));
}

void Table_chengdumj::GameStart()
{
	log_error("%s tid:%d ", __FUNCTION__, tid);

	Table::GameStart();

    if (m_conf_js.bHuanSanZhang)
    {
        StartChooseHuanZhang();
        return;
    }
	else
	{
		// 三色牌则定缺，两色不定缺
		if (108 == m_conf_js.iPaiZhangShu)
		{
			StartChooseDingQue();
			return;
		}
	}

	m_bGameState = GAME_PLAYING;

	m_bFirstCard = true;
	OnUserGetCard(m_bBanker);
	m_bFirstCard = false;
}


void Table_chengdumj::Send_Hand_Card()
{  
	log_error("%s tid:%d ", __FUNCTION__, tid);

	for (int i = 0; i < m_GAME_PLAYER; i++)
	{
		m_HandCard[i].ChangeableCardsLen = (i == m_bBanker) ? HAND_CARD_SIZE_MAX : HAND_CARD_SIZE_MAX - 1;
		m_CardPool.GetCard(m_HandCard[i].ChangeableCards, m_HandCard[i].ChangeableCardsLen);
	}

	//给客户端发牌
	proto::game::AckFirstHandCard stFirsthandcard;
	for (int i = 0; i < m_GAME_PLAYER; i++)
	{
		//memset(buf, 0, sizeof(buf));
		stFirsthandcard.Clear();
		proto::game::HandCards *pstHc = stFirsthandcard.mutable_usercard();
		pstHc->set_changeablecardslen(m_HandCard[i].ChangeableCardsLen);
		
		for (int j=0; j<m_HandCard[i].ChangeableCardsLen; j++)
		{
			pstHc->add_changeablecards(m_HandCard[i].ChangeableCards[j]);		
		}

		for (int k = 0; k < m_GAME_PLAYER; k++)
		{
			stFirsthandcard.add_handcardnums(m_HandCard[k].ChangeableCardsLen);
		}		
		unicast(SERVER_FIRST_HANDCARD, i, stFirsthandcard);

		log_info("%s first uid:%d chairid:%d [%s]\n", __FUNCTION__, GetPlayerUid(i), i, CatHandCard(m_HandCard[i], i));

	}

	for (int i = 0; i < m_GAME_PLAYER; i++)
	{
		m_GameLogic.SortCard(m_HandCard[i].ChangeableCards, m_HandCard[i].ChangeableCardsLen);
	}
	save_video_firsthand_card();
}

void Table_chengdumj::Send_Game_Scene(Player *player)
{
	log_error("%s tid:%d ", __FUNCTION__, tid);

	if (player == NULL)
	{
		log_error("Send_GameScene, player is NULL \n");
		return;
	}
	int ChairID = player->seatid;	    
	if (ChairID < 0 || ChairID >= m_GAME_PLAYER)
	{
		Send_lookon_GameScene(player);
		return;
	}

	proto::game::chengdumj::tagCDGameScence stcqGameScence;
	proto::game::AckGameScene *stGameScene = stcqGameScence.mutable_gamesence();	
	
	Proto_GameScene(ChairID, player, *stGameScene);

	stGameScene->set_scenestatus(m_bGameState);	//客户端认为状态不为free游戏就已经开始
	stcqGameScence.set_dingqueing(m_bGameState == GAME_CHOOSE_LACK ? true : false);
	stcqGameScence.set_huanzhanging(m_bGameState == GAME_CHANGECARD ? true : false);
	for (int i = 0; i < m_GAME_PLAYER; i++)
	{

		if (INVALID_MJ_CARD != m_UserHuanZhangPai[i][0])
		{
			stcqGameScence.add_huanzhangisok(1);
		}
		else
		{
			stcqGameScence.add_huanzhangisok(0);
		}

		if (INVALID_MJ_CARD != m_UserDingQueSe[i])
		{
			stcqGameScence.add_dingqueisok(1);
		}
		else
		{
			stcqGameScence.add_dingqueisok(0);
		}

		if (m_bGameState == GAME_CHOOSE_LACK)
		{
			if (i == ChairID)
			{
				stcqGameScence.add_dingquese(m_UserDingQueSe[i]);
			}
			else
			{
				stcqGameScence.add_dingquese(INVALID_MJ_CARD);
			}
		}
		else
		{

			stcqGameScence.add_dingquese(m_UserDingQueSe[i]);
		}

		if (i == ChairID)
		{
			proto::game::chengdumj::ReqChangeCard* strTempReqChangeCard = stcqGameScence.add_huanzhangpai();

			for (int j = 0; j < 3; j++)
			{
				strTempReqChangeCard->add_card(m_UserHuanZhangPai[i][j]);
			}
		}

        stcqGameScence.add_nhutype(m_nHuType[i]);

		for (int j = 0; j < m_GAME_PLAYER; j++)
		{
			if (true == m_stDianPaoInfo[i].bIsUsed)
			{
				proto::game::chengdumj::DianPaoInfo* pstDianPaoInfo = stcqGameScence.add_stdianpaoinfo();
				pstDianPaoInfo->set_seatid(m_stDianPaoInfo[i].nDianPaoUser);
				pstDianPaoInfo->set_ndianpaocardshunxu(m_stDianPaoInfo[i].nDianPaoShunXu);
				pstDianPaoInfo->set_ndianpaocard(m_stDianPaoInfo[i].nDianPaoCard);
				break;
			}
		}
		// stcqGameScence.add_ndianpaopaishunxu(m_nHuCardShunXu[i]);
		stcqGameScence.add_nzimohucard(m_nZiMoHuCard[i]);
		stcqGameScence.add_ndianpaohucard(m_nPaoHuCard[i]);
	}
	stcqGameScence.set_dingque_remain_time(ev_timer_remaining(zjh.loop, &m_DingQue_Timer));
	stcqGameScence.set_huanzhang_remain_time(ev_timer_remaining(zjh.loop, &m_HuanZhang_Timer));
	ChengDuMJPeiZhi(stcqGameScence);

	//log_debug("%s pjsGameScence:%s .\n", __FUNCTION__, stcqGameScence.DebugString().c_str());
	unicast(SERVER_GAME_SCENE, player, stcqGameScence);
	log_info("%s over to uid:%d chairid:%d \n", __FUNCTION__, player->uid, player->seatid);	
}


int Table_chengdumj::GetCardEstimate(_uint8 ChairID, _uint8 bGetCard, MJ_major& major, int& fan_num)
{

	log_info("%s tid:%d ChairID:%d bGetCard:0x%02x\n", __FUNCTION__, tid, ChairID, bGetCard);

	assert(major.m_oFan.Size() == 0);

	int iOpType = TYPE_NULL;

	HandCards* pHc = &m_HandCard[ChairID];	
	m_GameLogic.AddCard(pHc, bGetCard);

	//assert(pHc->ChangeableCardsLen % 3 == 2);
	if (pHc->ChangeableCardsLen % 3 != 2)
	{
		log_error("%s tid:%d fatal paishu error uid:%d chairid:%d!\n", __FUNCTION__, tid, GetPlayerUid(ChairID), ChairID);
		m_GameLogic.ChuPai(pHc, bGetCard);
		return TYPE_NULL;
	}
	assert(pHc->ChangeableCardsLen <= HAND_CARD_SIZE_MAX);

	iOpType |= GetGangInfo(ChairID, major.m_GetGangInfo);

	iOpType |= Check_Hu(ChairID, *pHc, major.m_oFan);

	// 三色牌则定缺，两色不定缺
	if (108 == m_conf_js.iPaiZhangShu)
	{
		for (int i = 0; i < major.m_GetGangInfo.Size(); i++)
		{
			if (CardType(major.m_GetGangInfo[i].cbCard) == m_UserDingQueSe[ChairID])
			{
				major.m_GetGangInfo.Remove(OpState(major.m_GetGangInfo[i].cbCard, major.m_GetGangInfo[i].nState));
			}
		}

		if (iOpType & TYPE_WANGANG)
		{
			iOpType &=~ TYPE_WANGANG;
		}

		if (iOpType & TYPE_ZHIGANG)
		{
			iOpType &=~ TYPE_ZHIGANG;
		}

		if (iOpType & TYPE_ANGANG)
		{
			iOpType &=~ TYPE_ANGANG;
		}

		for (int i = 0; i < major.m_GetGangInfo.Size(); i++)
		{
			if (major.m_GetGangInfo[i].nState == TYPE_ANGANG)
			{
				iOpType |= TYPE_ANGANG;
			}

			if (major.m_GetGangInfo[i].nState == TYPE_WANGANG)
			{
				iOpType |= TYPE_WANGANG;
			}

			if (major.m_GetGangInfo[i].nState == TYPE_ZHIGANG)
			{
				iOpType |= TYPE_ZHIGANG;
			}
		}

		if (iOpType & TYPE_HU)
		{
			for (int i = 0; i < pHc->ChangeableCardsLen; i++)
			{
				if (CardType(pHc->ChangeableCards[i]) == m_UserDingQueSe[ChairID])
				{
					iOpType &=~ TYPE_HU;
					break;
				}
			}
		}
	}

	if (m_bGetCardCount[ChairID] == 1)
	{
		UserGetFirstCard(ChairID, iOpType, major.m_oFan);
	}		
	if (CardPoolIsEmpty())
	{
		UserGetLastCard(ChairID, iOpType, major.m_oFan);
	}		
	if (iOpType & TYPE_HU)
	{
		if (m_bAddFanFlag[ChairID])   //杠上花
		{
			UserGangShangHua(ChairID, bGetCard, major.m_oFan); 			
		}
		fan_num = Calc_Fan(ChairID, major.m_oFan);
	}

	m_GameLogic.ChuPai(pHc, bGetCard);

	log_info("%s tid:%d ChairID:%d bGetCard:%d iOpType:0x%x \n", __FUNCTION__, tid, ChairID, bGetCard, iOpType);

	return iOpType;
}

// 玩家摸牌
void Table_chengdumj::OnUserGetCard(_uint8 ChairID, bool bGetLast)
{
	log_info("%s tid:%d ChairID:%d bGetLast:%d \n", __FUNCTION__, tid, ChairID, bGetLast);

	if (m_bDeleteTable)
	{
		return;
	}

	if (CardPoolIsEmpty())
	{
		GameEnd();
		return;
	}

    // 胡过之后，那么就选择下一个没胡的小玩家摸牌
    if (true == m_bIsHu[ChairID])
    {
        m_bCurChairID = NextChair(ChairID);
		OnUserGetCard(m_bCurChairID);
        return;
    }
    
	memset(&m_bUserOperate, 0, sizeof(m_bUserOperate));
	m_stQiHuInfo[ChairID].InitQiHuInfo();
	m_bCurChairID = ChairID;
	nOprOpType2 = 0;
	m_bCanDiHu[ChairID]++;

	_uint8 bGetCard = INVALID_MJ_CARD;
	if (m_bFirstCard)
	{
		bGetCard = m_HandCard[ChairID].ChangeableCards[m_HandCard[ChairID].ChangeableCardsLen - 1];
		m_GameLogic.ChuPai(&m_HandCard[ChairID], bGetCard);
	}
	else
	{
		bGetCard = bGetLast ? m_CardPool.GetLast() : m_CardPool.GetOneCard();
	}

	m_bGetCardCount[ChairID]++;

	m_bCurCard = bGetCard;
	MJ_major major;
	//摸牌判断
	m_bUserOperate[ChairID] = GetCardEstimate(ChairID, bGetCard, major, m_HuFanNums[ChairID]);

	m_GameLogic.AddCard(&m_HandCard[ChairID], bGetCard);

	log_info("%s tid:%d uid:%d chairid:%d getcard:0x%02x optype:0x%x cardpoollen:%d \n", __FUNCTION__, tid, GetPlayerUid(ChairID), ChairID, bGetCard, m_bUserOperate[ChairID], m_CardPool.Count());
	if (!m_bFirstCard)
	{
		save_user_get_card(ChairID, bGetCard, m_CardPool.Count(), bGetLast);
	}
	else
	{
		save_user_get_card(ChairID, INVALID_MJ_CARD, m_CardPool.Count(), bGetLast);
	}
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
	}
	
	if (m_bUserOperate[ChairID] == TYPE_NULL || m_bUserOperate[ChairID] == TYPE_LISTEN)
	{
		set_out_timer(ChairID);
	}
	else
	{
		set_operator_timer(ChairID);
	}

	m_trustee.get()->OnUserGetCard(ChairID, m_bCurCard);
}

// 玩家摸牌广播
void Table_chengdumj::Proto_Game_UserGetCard(_uint8 ChairID, _uint8 MoChairID, _uint8 bCard, proto::game::AckUserGetCard& stUsergetcard)
{

	log_info("%s tid:%d ChairID:%d MoChairID:%d bCard:0x%02x\n", __FUNCTION__, tid, ChairID, MoChairID, bCard);
	//_uint8 tempChairid = m_SeatPlayer[ChairID].player->seatid;
	stUsergetcard.Clear();

	stUsergetcard.set_chairid(MoChairID);
	stUsergetcard.set_leftcardnum(m_CardPool.Count());
	if (m_bFirstCard)
	{
		stUsergetcard.set_card(INVALID_MJ_CARD);
	}

	//如果是摸牌用户，则打包摸牌、听牌等数据，非摸牌用户则不发送这些数据
	if (ChairID == MoChairID)
	{
		if (m_bFirstCard)
		{
			stUsergetcard.set_card(INVALID_MJ_CARD);
		}
		else
		{
			stUsergetcard.set_card(bCard);
		}

		if (m_bUserOperate[MoChairID] & TYPE_ANGANG || m_bUserOperate[MoChairID] & TYPE_WANGANG)
		{
			MJ_opinfo stGang;			
			GetGangInfo(MoChairID, stGang);

			for (int i = 0; i < stGang.Size(); i++)
			{
				//log_info("玩家还没胡牌,此时暗杠牌有:0x%02x \n", stGang[i].cbCard);
				log_info("%s uid:%d chairID:%d angang:0x%02x\n", __FUNCTION__, GetPlayerUid(ChairID), ChairID, stGang[i].cbCard);
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
			//打包听牌信息
			for (int j = 0; j < bListenLen; j++)
			{
				PackageTingInfo(ChairID, stListenInfo, j, stUsergetcard.add_listeninfo());
			}
		}
	}
}

//用户打牌后的判断
bool Table_chengdumj::EstimateUserRespond(_uint8 bOutChairID, _uint8 bOutCard)
{

	log_error("%s tid:%d ChairID:%d", __FUNCTION__, tid, bOutChairID);

	m_bMultiOpUserNum = 0;
	m_bMultiOpHuNum = 0;
	bool bIsChangeUser = true;//是否轮到下一玩家摸牌

	memset(m_bUserOperate, TYPE_NULL, sizeof(m_bUserOperate));
	for (int i = 0; i < m_GAME_PLAYER; i++)
	{
		if (i == bOutChairID) 
		{
			m_bUserOperate[i] = TYPE_NULL;
			continue;
		}
		//牌型判断			
		MJ_major major;		
		m_bUserOperate[i] = OutCardEstimate(i, bOutCard, major, m_HuFanNums[i]);

		// 0番取消胡牌
		if (m_bUserOperate[i] & TYPE_HU)
		{
			if ((0 == m_HuFanNums[i])&&(false == m_conf_js.bDianPaoPingHu))
			{
				m_bUserOperate[i] &=~ TYPE_HU;
			}

			// 弃胡
			if (true == m_stQiHuInfo[i].bIsQiHu)
			{
				if (m_stQiHuInfo[i].nQiHuFan >= m_HuFanNums[i])
				{
					m_bUserOperate[i] &=~ TYPE_HU;
				}
			}
		}

		if (m_bUserOperate[i] != TYPE_NULL)
		{
			if (m_bUserOperate[i] & TYPE_HU)
			{
				//m_bAddFanFlag[i] = true;
				m_stQiHuInfo[i].nDangQianHuFan = m_HuFanNums[i];
				m_bMultiOpHuNum++;				
			}
			m_bMultiOpUserNum++;
			bIsChangeUser = false;
			//准备向四个玩家发送操作提示

			for (int j = 0; j < m_GAME_PLAYER; j++)
			{
				proto::game::AckOpNotify stNotify;		

				Proto_Game_OpNotify(i, j, bOutCard, stNotify);
				unicast(SERVER_OPERATE_NOTIFY, j, stNotify);	

				if (i == j) save_operate_notify(stNotify);		
			}
		}
		if (m_bUserOperate[i] != 0)
		{
			log_info("%s uid:%d chairid:%d Operate:0x%x, totalop:%d \n", __FUNCTION__, GetPlayerUid(i), i, m_bUserOperate[i], m_bMultiOpUserNum);
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

// 出牌判断
int Table_chengdumj::OutCardEstimate(_uint8 ChairID, _uint8 bCard, MJ_major& major, int& fan_num)
{	

	log_info("%s tid:%d ChairID:%d bCard:0x%02x \n", __FUNCTION__, tid, ChairID, bCard);

	assert(major.m_oFan.Size() == 0);

	// 胡牌玩家不做判断
	if (true == m_bChuJu[ChairID])
	{
		return TYPE_NULL;
	}

	// 三色牌则定缺，两色不定缺
	if (108 == m_conf_js.iPaiZhangShu)
	{
		// 如果出的牌是定缺的色，则不处理
		if (m_UserDingQueSe[ChairID] == CardType(bCard))
		{
			return TYPE_NULL;
		}
	}

	int iOpType = TYPE_NULL;

	HandCards* pHc = &m_HandCard[ChairID];

	// iOpType |= GetChi(ChairID, bCard, major.m_Chi);

	iOpType |= GetPengGang(ChairID, bCard, major.m_GetPengGang);


	assert(pHc->ChangeableCardsLen < HAND_CARD_SIZE_MAX);

	if (!m_conf.m_bZimoHu)
	{
		m_GameLogic.AddCard(pHc, bCard);

		iOpType |= Check_Hu(ChairID, *pHc, major.m_oFan);	

		// 三色牌则定缺，两色不定缺
		if (108 == m_conf_js.iPaiZhangShu)
		{
			if (iOpType & TYPE_HU)
			{
				for (int i = 0; i < pHc->ChangeableCardsLen; i++)
				{
					if (CardType(pHc->ChangeableCards[i]) == m_UserDingQueSe[ChairID])
					{
						iOpType &=~ TYPE_HU;
						break;
					}
				}
			}
		}

		if (CardPoolIsEmpty())
		{
			UserLastOutCard(ChairID, iOpType, major.m_oFan);
		}		

		if (IsQuanqiuren(*pHc, major.m_oFan, iOpType))
		{
		}		

		if (m_bGetCardCount[m_bBanker] == 1)
		{//庄家打出第一张牌
			for (_uint8 c = NextChair(m_bBanker); c != m_bBanker; c = NextChair(c))
			{
				if (m_bGetCardCount[c] != 0)
					goto BANKER_FIRST_OUTCARD;
			}
			UserFirstOutCard(ChairID, iOpType, major.m_oFan);			
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
			for (int i = 0; i < pHc->ChangeableCardsLen; i++)
			{
				if (CardType(pHc->ChangeableCards[i]) == m_UserDingQueSe[ChairID])
				{
					iOpType &=~ TYPE_HU;
					break;
				}
			}
		}

		if (iOpType & TYPE_HU)
		{
			UserQiangGangHu(ChairID, bCard, major.m_oFan);
			fan_num = Calc_Fan(ChairID, major.m_oFan);
		}
		m_GameLogic.ChuPai(pHc, bCard);
	}

	log_info("%s tid:%d ChairID:%d bCard:%d fan_num:%d iOpType:0x%x\n", __FUNCTION__, tid, ChairID, bCard, fan_num, iOpType);

	return iOpType;
}

// 收到玩家操作

// 天胡
void Table_chengdumj::UserGetFirstCard(_uint8 ChairID, int& iOpType, MJ_fan_buffer& oFan)
{
	if ((iOpType & TYPE_HU) && (true == m_conf_js.bTianDiHu))
	{
		oFan.Add(MJ_FAN_TYPE_TIANHU);
	}
}

// 地胡
void Table_chengdumj::UserFirstOutCard(_uint8 ChairID, int& iOpType, MJ_fan_buffer& oFan)
{
	if ((iOpType & TYPE_HU) && (true == m_conf_js.bTianDiHu))
	{
		if (false == oFan.Find(MJ_FAN_TYPE_DIHU))
		{
			oFan.Add(MJ_FAN_TYPE_DIHU);
		}
	}
}

// 海底捞
void Table_chengdumj::UserGetLastCard(_uint8 ChairID, int& iOpType, MJ_fan_buffer& oFan)
{
	iOpType &= ~(TYPE_PENG | TYPE_CHI | TYPE_GANG);
	
	if (iOpType & TYPE_HU)
	{
		if (m_conf_js.bHaiDiHuaJiaFan)
		{
			oFan.Add(MJ_FAN_TYPE_HAIDILAOYUE);
		}
	}
}

// 海底炮
void Table_chengdumj::UserLastOutCard(_uint8 ChairID, int& iOpType, MJ_fan_buffer& oFan)
{
	iOpType &= ~(TYPE_CHI | TYPE_GANG);

	if (iOpType & TYPE_HU)
	{
		if (m_conf_js.bHaiDiPaoJiaFan)
		{
			oFan.Add(MJ_FAN_TYPE_HAIDIPAO);
		}
	}
}

// 杠上花
void Table_chengdumj::UserGangShangHua(_uint8 ChairID, _uint8 bCard, MJ_fan_buffer& oFan)
{
	oFan.Add(MJ_FAN_TYPE_GANGSHANGHUA);
}

// 杠上炮
void Table_chengdumj::UserGangHouPao(_uint8 ChairID, _uint8 bCard, MJ_fan_buffer& oFan)
{
	oFan.Add(MJ_FAN_TYPE_GANGHOUPAO);
}

// 抢杠胡
void Table_chengdumj::UserQiangGangHu(_uint8 ChairID, _uint8 bCard, MJ_fan_buffer& oFan)
{
	oFan.Add(MJ_FAN_TYPE_QIANGGANGHU);
}

// 计算番数
int Table_chengdumj::Calc_Fan(_uint8 ChairID, MJ_fan_buffer& oFan)
{
	//log_error("%s tid:%d ChairID:%d", __FUNCTION__, tid, ChairID);

	int iTotalFan = 0;

	if (oFan.Find(MJ_FAN_TYPE_QUANQIUREN))
	{
		oFan.Remove(MJ_FAN_TYPE_QUANQIUREN);
	}

// 	if (
// 		oFan.Find(MJ_FAN_TYPE_QINGSHUANGLONGQIDUI)|| 
// 		oFan.Find(MJ_FAN_TYPE_SHUANGLONGQIDUI)|| 
// 		oFan.Find(MJ_FAN_TYPE_JIANGQIDUI)|| 
// 		oFan.Find(MJ_FAN_TYPE_LONGQIDUI)|| 
// 		oFan.Find(MJ_FAN_TYPE_QINGLONGQIDUI)|| 
// 		oFan.Find(MJ_FAN_TYPE_QINGQIDUI)||
// 		oFan.Find(MJ_FAN_TYPE_QIDUI)
// 		)
// 	{
// 		if (oFan.Find(MJ_FAN_TYPE_MENQING))
// 		{
// 			oFan.Remove(MJ_FAN_TYPE_MENQING);
// 		}
// 	}

	for (int i = 0; i < oFan.Size(); i++)
	{
		// 勾牌单独计算
		if (oFan[i] == MJ_FAN_TYPE_GOUPAI)
		{
			iTotalFan += m_nGouPaiNum[ChairID];
			log_debug("%s oFan:%d fan_num:%d .\n", __FUNCTION__, oFan[i], m_nGouPaiNum[ChairID]);
			continue;
		}
		iTotalFan += m_ft2fn->GetFan(oFan[i]);
		log_debug("%s oFan:%d fan_num:%d .\n", __FUNCTION__, oFan[i], m_ft2fn->GetFan(oFan[i]));
	}
	return iTotalFan;
}

void Table_chengdumj::InitGameEndInfo()
{
	// 杠上炮之后下一个玩家摸牌
	memset(m_bAddFanFlag, 0, sizeof(m_bAddFanFlag));
}

// 游戏结束
void Table_chengdumj::GameEnd()
{
	log_error("%s tid:%d ", __FUNCTION__, tid);

	InitGameEndInfo();
	 m_GameEndTimes++;
	//摸完最后一张牌才算是结束
	MJ_user_buffer UserID;
	_uint8 bDraw = 0;
    // 牌打完了，但是胡牌玩家不是只小于所有玩家一人
    if (CardPoolIsEmpty())
	{
		if (MultiHu(UserID) != (m_GAME_PLAYER -1))
		{
			bDraw = 1;
		}
	}
	// 一炮多响，适合小于等于四人的玩法
	int nTempHuNum = MultiHu(UserID);
	// 一炮多响达成
	if ((nTempHuNum - m_nLastHuPaiNum) > 1)
	{
		// 一炮多响玩家
		m_nYiPaoDuoXiangCharid = m_bCurChairID;
	}
	m_nLastHuPaiNum = nTempHuNum;
    
	TurnGangHouPaoFen();

	//单发结算协议	
	proto::game::chengdumj::AckCDGameEnd stCQGameEnd;
	proto::game::AckGameEnd_Chengdu *stGameend = stCQGameEnd.mutable_gameend();
	HuScoreSet hu_score;

	Score_GameEndSucc(hu_score, stCQGameEnd);

	stGameend->set_endstate(bDraw);

	Proto_GameEndInfo_AddFan(UserID, *stGameend);

	int UserScore[GAME_PLAYER] = {0};
	for (int i = 0; i < m_GAME_PLAYER; i++)
	{
		proto::game::HandCards *pHandcard = stGameend->add_usercard();
		Fill_HandCard(i, pHandcard);

		Proto_GameEndInfo_GangInfo(i, *stGameend);

		UserScore[i] = hu_score[i].iTotleScore + m_nLastHuScore[i];

		// stGameend->add_score(UserScore[i]);	

		m_nLastHuScore[i] = UserScore[i];

		log.fatal("%s tid:%d bDraw:%d score[%d]\n", __FUNCTION__, tid, bDraw, hu_score[i].iTotleScore);

	}

	Calc_HuPaiShunXu(m_GameEndTimes, m_bCurChairID);

	log.fatal("%s tid:%d bDraw:%d score[%d,%d,%d,%d]\n", __FUNCTION__, tid, bDraw, UserScore[0], UserScore[1], UserScore[2], UserScore[3]);

	CalcRecordDianPaoInfo(UserID, UserScore);

	// 游戏结束未结束
	if ((bDraw == 0) && (MultiHu(UserID) < (m_GAME_PLAYER -1)))
	{
		for (int i = 0; i < m_GAME_PLAYER; i++)
		{
			if (m_bIsHu[i])
			{
				// 玩家已经经历过结算
				m_bChuJu[i] = true;
			}
		}
		OnUserGetCard(m_nHuPaiChairid);
		return ;
	}

	if (bDraw)
	{
		CheckDaJiaoTuiGang(stCQGameEnd);
	}

	CalGangScore();

	for (int i = 0; i < m_GAME_PLAYER; i++)
	{
		proto::game::chengdumj::HuPaiInfo* pstHuPaiInfo = stCQGameEnd.add_hupaiinfo();

		if (m_bIsHu[i])
		{
			for (int k = 0; k < m_GAME_PLAYER; k++)
			{
				if (i == m_nHuPaiShunXu[k])
				{
					int nShunXu = k + 1;
					Proto_GameEndInfo_ChengDu_Hu_Info (nShunXu, i, pstHuPaiInfo);
					break;
				}
			}
		}
		else
		{
			Proto_GameEndInfo_ChengDu_Hu_Info (INVALID_CHAIRID, i, pstHuPaiInfo);
		}

		stCQGameEnd.add_husocore(UserScore[i] + m_nDaJiao[i] + m_nGangScore[i] + m_nTuiGang[i] + m_nZhuanYiGang[i]);
		stGameend->add_score(UserScore[i] + m_nDaJiao[i] + m_nGangScore[i] + m_nTuiGang[i] + m_nZhuanYiGang[i]);

		log_info("%s, tid:%d, uid:%d, ChairID:%d, bDraw:%d, UserScore[%d], m_nDaJiao[%d], m_nGangScore[%d], m_nTuiGang[%d], m_nZhuanYiGang[%d] \n", 
			__FUNCTION__, tid, bDraw, GetPlayerUid(i), i, UserScore[i], m_nDaJiao[i], m_nGangScore[i], m_nTuiGang[i], m_nZhuanYiGang[i]);

		//proto::game::chengdumj::
		//stCQGameEnd.add_handcards();
	}


	tagUserScore userscore[GAME_PLAYER];

	for (int i = 0; i < m_GAME_PLAYER; i++)
	{
		if (m_SeatPlayer[i].player != NULL)
		{  
			userscore[i].uid = m_SeatPlayer[i].player->uid;
			userscore[i].score = UserScore[i] + m_nDaJiao[i] + m_nGangScore[i] + m_nTuiGang[i] + m_nZhuanYiGang[i];
			if (userscore[i].score  > 0)
			{
				m_SeatPlayer[i].player->update_info(userscore[i].score, 1, 1, win_exp);
			}
			else
			{
				m_SeatPlayer[i].player->update_info(userscore[i].score, 1, 0, lose_exp);
			}

			//UpdateUserInfoToUser(m_SeatPlayer[i].player); //客户端动画会有问题
			stGameend->add_money(m_SeatPlayer[i].player->money); 
		}
		// m_nHuScoreFirst[i] = hu_score[i].iTotleScore - m_nHuScoreFirst[i];
	}

	broadcast(SERVER_GAME_END, stCQGameEnd);
	log_debug("%s tid:%d stGameEnd:%s .\n", __FUNCTION__, tid, stCQGameEnd.DebugString().c_str());

 	//发送录像
	/*save_video_game_end(stCQGameEnd);
	update_user_info_to_datasvr_dz(userscore, 1102);
	ev_timer_again(zjh.loop, &send_videodata_timer);
	CalcRecordInfo(UserID, UserScore);
	RedisUpdateScoreToServer(UserScore);*/

	m_trustee.get()->GameEnd();

	m_last_Banker = m_bBanker;
	//轮庄
	TurnBanker();
	//初始化数据
	//GameEndReset();
	for (int i = 0; i < m_GAME_PLAYER; i++)
	{
		Player* player = m_SeatPlayer[i].player;
		if (player != NULL)
		{
			SettleInfo& settle = player->m_SettleInfo[m_CurRound];
			settle.sSeatId = player->seatid;
			settle.sCurMoney = player->m_CurMoney;
			settle.sUseTime = player->m_UseTime;
		}
	}

	m_RoundOver_timer_running = true;
	TIMER_AGAIN(m_RoundOver_timer, m_RoundOver_stamp);
}

// 平反杠分
void Table_chengdumj::PingGangFen(_uint8 ChairID, _uint8 nNum)
{
	// 平掉杠分
	m_stGang[m_bCurChairID].iTotalScore = m_stGang[m_bCurChairID].iTotalScore - m_GangFen[m_bCurChairID][nNum].totalscore;

	if (INVALID_MJ_CARD != m_GangFen[m_bCurChairID][nNum].uid1)
	{
		m_stGang[m_GangFen[m_bCurChairID][nNum].uid1].iTotalScore = m_stGang[m_GangFen[m_bCurChairID][nNum].uid1].iTotalScore + m_GangFen[m_bCurChairID][nNum].score1;
	}
	
	if (INVALID_MJ_CARD != m_GangFen[m_bCurChairID][nNum].uid2)
	{
		m_stGang[m_GangFen[m_bCurChairID][nNum].uid2].iTotalScore = m_stGang[m_GangFen[m_bCurChairID][nNum].uid2].iTotalScore + m_GangFen[m_bCurChairID][nNum].score2;
	}

	if (INVALID_MJ_CARD != m_GangFen[m_bCurChairID][nNum].uid3)
	{
		m_stGang[m_GangFen[m_bCurChairID][nNum].uid3].iTotalScore = m_stGang[m_GangFen[m_bCurChairID][nNum].uid3].iTotalScore + m_GangFen[m_bCurChairID][nNum].score3;
	}

	m_GangFen[m_bCurChairID][nNum].init();
}

void Table_chengdumj::TurnGangHouPaoFen()
{
	// 看看是不是弃杠之后的转移杠

	if (true == m_bQiGang[m_bCurChairID][CardType(m_bCurCard)][CardNum(m_bCurCard)])
	{
		return ;
	}
	for (int i = 0; i < m_GAME_PLAYER; i++)
	{
		if ((true == m_bIsHu[i]) && (false == m_bChuJu[i]))
		{
			if (m_oFan[i].Find(MJ_FAN_TYPE_GANGHOUPAO))
			{
				for (int j = 0; j < 5; j++)
				{
					if (m_GangFen[m_bCurChairID][j].bIsNull)
					{
						if (j > 0)
						{
							// 一炮双响
							if (m_bCurChairID == m_nYiPaoDuoXiangCharid)
							{
								PingGangFen(m_bCurChairID, j-1);
								break;
							}
							else
							{
								for (int k = 0; k < m_GAME_PLAYER; k++)
								{
									if (false == m_stGangZhuanYiInfo[k].bIsUsed)
									{
										m_stGangZhuanYiInfo[k].bIsUsed = true;
										m_stGangZhuanYiInfo[k].nUserGang = m_bCurChairID;
										m_stGangZhuanYiInfo[k].nUserHu = i;
										m_stGangZhuanYiInfo[k].nGangScore = m_GangFen[m_bCurChairID][j-1].totalscore;

										if (i == m_GangFen[m_bCurChairID][j-1].uid1)
										{
											m_stGangZhuanYiInfo[k].nHuUserGangScore = m_GangFen[m_bCurChairID][j-1].score1;
										}
										else if (i == m_GangFen[m_bCurChairID][j-1].uid2)
										{
											m_stGangZhuanYiInfo[k].nHuUserGangScore = m_GangFen[m_bCurChairID][j-1].score2;
										}
										else if (i == m_GangFen[m_bCurChairID][j-1].uid3)
										{
											m_stGangZhuanYiInfo[k].nHuUserGangScore = m_GangFen[m_bCurChairID][j-1].score3;
										}

										log_info("%s tid:%d nUserGang:%d nUserHu:%d nGangScore[%d]\n", __FUNCTION__, tid, m_bCurChairID,
											i, m_GangFen[m_bCurChairID][j-1].totalscore);

										m_GangFen[m_bCurChairID][j-1].init();

										break;
									}
								}
								break;
							}
						}
					}
				}
			}
		}
	}

}


void Table_chengdumj::Proto_GameEndInfo_ChengDu_Hu_Info(_uint8 nHuPaiShunXu, _uint8 HuChairID, proto::game::chengdumj::HuPaiInfo* pstHuPaiInfo)
{
	log_error("%s tid:%d nHuPaiShunXu:%d ", __FUNCTION__, tid, nHuPaiShunXu);

	pstHuPaiInfo->set_seatid(HuChairID);

	pstHuPaiInfo->set_totalgangfen(m_nGangScore[HuChairID]);

	pstHuPaiInfo->set_totalzhuanyufen(m_nZhuanYiGang[HuChairID]);

	Proto_GameEnd_GangInfo(HuChairID, pstHuPaiInfo);

	if (false == m_bIsHu[HuChairID])
	{
		pstHuPaiInfo->set_hutype(3);
		return ;
	}

	pstHuPaiInfo->set_hushunxu(nHuPaiShunXu);

	pstHuPaiInfo->set_hucard(m_nHuCard[HuChairID]);

	for (int i = 0; i < 2; i++)
	{
		if (HuChairID == m_stZiMoPaoHuInfo[HuChairID].nHuChaird)
		{
			if (true == m_stZiMoPaoHuInfo[HuChairID].IsZiMoOrPaoHu)
			{
				if (HuChairID == m_stZiMoPaoHuInfo[HuChairID].nPaoHuChaird)
				{
					pstHuPaiInfo->set_hutype(2);
					pstHuPaiInfo->set_dianpaoseatid(m_stZiMoPaoHuInfo[HuChairID].nDianPaoChaird);
					//pHuPaiInfo->set_stdianpaofan(m_stZiMoPaoHuInfo[HuChairID].nPaoFan);
				}
			}
			else
			{
				if (HuChairID == m_stZiMoPaoHuInfo[HuChairID].nZiMoChaird)
				{
					pstHuPaiInfo->set_hutype(1);
					pstHuPaiInfo->set_dianpaoseatid(INVALID_CHAIRID);
					//pHuPaiInfo->set_stdianpaofan(0);
				}
			}
			pstHuPaiInfo->set_totalhufan(m_stZiMoPaoHuInfo[HuChairID].nHuFan);
		}
	}

	for (int i = 0; i < MJ_FAN_TYPE_MAX; i ++)
	{
		// 不加鸡胡
		if (m_oFan[HuChairID].Size() > m_oFan[HuChairID].Counts(MJ_FAN_TYPE_JIHU))
		{
			if (MJ_FAN_TYPE_JIHU == i)
			{
				continue;
			}
		}

		if (m_oFan[HuChairID].Find(i))
		{
			int nTempFan = m_ft2fn->GetFan(i);

			if (MJ_FAN_TYPE_GOUPAI == i)
			{
				nTempFan = m_nGouPaiNum[HuChairID];
			}
			pstHuPaiInfo->add_hufanleixing(i);
			pstHuPaiInfo->add_hufan(nTempFan);
		}
	}

	for (_uint8 i = 0; i < m_ShuFenSeatID[HuChairID].size(); i++)
	{
		pstHuPaiInfo->add_beihuseatid(m_ShuFenSeatID[HuChairID][i]);
	}
}



// 保存游戏结束录像
void Table_chengdumj::save_video_game_end(const proto::game::chengdumj::AckCDGameEnd stCQGameEnd)
{
	std::string str = stCQGameEnd.SerializeAsString();
	save_video_data(SERVER_GAME_END, str);
}

// 正常结束游戏算分
void Table_chengdumj::Score_GameEndSucc(HuScoreSet& score, proto::game::chengdumj::AckCDGameEnd &stCQGameEnd)
{
	//int iTotalScore[GAME_PLAYER] = { 0 };
	int iTempScore[GAME_PLAYER] = { 0 };
	int iBaseScore[GAME_PLAYER][GAME_PLAYER];
	int iHuScore[GAME_PLAYER][GAME_PLAYER];
	int iPiaoScore[GAME_PLAYER][GAME_PLAYER];

	memset(iBaseScore, 0, sizeof(iBaseScore));
	memset(iHuScore, 0, sizeof(iHuScore));
	memset(iPiaoScore, 0, sizeof(iPiaoScore));

	for (int i = 0; i < m_GAME_PLAYER; i++)
	{
		Calc_BaseScore(i, iBaseScore[i]);
	}

	//胡牌分数
	for (int i = 0; i < m_GAME_PLAYER; i++)
	{		
		Calc_HuScore(i, iBaseScore[i], iHuScore[i]);		
	}
	for (int i = 0; i < m_GAME_PLAYER; i++)
	{		
		Calc_Score(iTempScore, iHuScore[i]);
	}
// 	for (int i = 0; i < m_GAME_PLAYER; i++)
// 	{
// 		stCQGameEnd.add_hu_score(iTempScore[i]);
// 	}

	for (int i = 0; i < m_GAME_PLAYER; i++)
	{
		score[i].iTotleScore = iTempScore[i];
	}
}

// 发送游戏番数
void Table_chengdumj::Proto_GameEndInfo_AddFan(const MJ_user_buffer& UserID, proto::game::AckGameEnd_Chengdu& stGameend)
{
	log_error("%s tid:%d ", __FUNCTION__, tid);

	proto::game::FanInfo* info[GAME_PLAYER];
	for (_uint8 i = 0; i < m_GAME_PLAYER; i++)
	{
		info[i] = stGameend.add_faninfo();
	}	

	proto::game::AddFanInfo* pfan = NULL;	
	if (UserID.Size() == 1)
	{//一个人胡
		_uint8 win_seat = UserID[0];
		assert(m_stHuScore[win_seat].stHu.Size() != 0);
		if (m_stHuScore[win_seat].stHu[0].bType == HU_ZIMO)
		{//自摸胡
			info[win_seat]->set_chairid(win_seat);
			info[win_seat]->set_specialtype(HU_ZIMO);
			info[win_seat]->set_fannum(m_HuFanNums[win_seat]);
			info[win_seat]->set_hucard(m_stHuScore[win_seat].stHu[0].bHuCard);

			for (int i = 0; i < m_oFan[win_seat].Size(); i++)
			{
				pfan = info[win_seat]->add_addfan();
				pfan->set_addtype(m_oFan[win_seat][i]);

				if (m_oFan[win_seat][i] == MJ_FAN_TYPE_GOUPAI)
				{
					pfan->set_addnum(m_nGouPaiNum[win_seat]);
				}
				else
				{
					pfan->set_addnum(m_ft2fn->GetFan(m_oFan[win_seat][i]));
				}

				pfan->set_addtype2(1);
			}

			for (int i = 0; i < m_UserOtherHu[win_seat].oFan.Size(); i++)
			{
				pfan = info[win_seat]->add_addfan();
				pfan->set_addtype(m_UserOtherHu[win_seat].oFan[i]);
				pfan->set_addnum(m_ft2fn->GetFan(m_UserOtherHu[win_seat].oFan[i]));
				pfan->set_addtype2(m_UserOtherHu[win_seat].bYinghu);
			}

			for (_uint8 c = NextChair(win_seat); c != win_seat; c = NextChair(c))
			{
				info[c]->set_chairid(c);
				info[c]->set_specialtype(HU_BEIZIMO);
				info[c]->set_fannum(0);
				info[c]->set_hucard(0);
				//info[c]->set_fantype(0);				
			}
		}
		else if (m_stHuScore[win_seat].stHu[0].bType == HU_PAOHU)
		{//点炮 或者抢杠胡
			info[win_seat]->set_chairid(win_seat);
			info[win_seat]->set_specialtype(m_bRobHu ? HU_QIANG_GANG : HU_PAOHU);
			info[win_seat]->set_fannum(m_HuFanNums[win_seat]);
			info[win_seat]->set_hucard(m_stHuScore[win_seat].stHu[0].bHuCard);
			//info[win_seat]->set_fantype(m_HuFanType[win_seat]);

			for (int i = 0; i < m_oFan[win_seat].Size(); i++)
			{
				pfan = info[win_seat]->add_addfan();
				pfan->set_addtype(m_oFan[win_seat][i]);
				pfan->set_addnum(m_ft2fn->GetFan(m_oFan[win_seat][i]));
				pfan->set_addtype2(1);
			}

			for (int i = 0; i < m_UserOtherHu[win_seat].oFan.Size(); i++)
			{
				pfan = info[win_seat]->add_addfan();
				pfan->set_addtype(m_UserOtherHu[win_seat].oFan[i]);
				pfan->set_addnum(m_ft2fn->GetFan(m_UserOtherHu[win_seat].oFan[i]));
				pfan->set_addtype2(m_UserOtherHu[win_seat].bYinghu);
			}

			for (_uint8 c = NextChair(win_seat); c != win_seat; c = NextChair(c))
			{
				info[c]->set_chairid(c);
				info[c]->set_specialtype(HU_INVALID);
				info[c]->set_fannum(0);
				info[c]->set_hucard(0);
				//info[c]->set_fantype(0);

			}
			info[m_stHuScore[win_seat].stHu[0].bPasHuChairID]->set_specialtype(m_bRobHu ? HU_BEI_QIANG_GANG : HU_DIANPAO);
		}
	}
	else if (UserID.Size() > 1)
	{//一炮多响
		for (_uint8 i = 0; i < m_GAME_PLAYER; i++)
		{
			if (UserID.Find(i))
			{

				info[i]->set_chairid(i);
				info[i]->set_specialtype(m_bRobHu ? HU_QIANG_GANG : HU_PAOHU);
				info[i]->set_fannum(m_HuFanNums[i]);
				info[i]->set_hucard(m_stHuScore[i].stHu[0].bHuCard);

				for (int j = 0; j < m_oFan[i].Size(); j++)
				{
					pfan = info[i]->add_addfan();
					pfan->set_addtype(m_oFan[i][j]);
					pfan->set_addnum(m_ft2fn->GetFan(m_oFan[i][j]));
					pfan->set_addtype2(1);
				}

				for (int j = 0; j < m_UserOtherHu[i].oFan.Size(); j++)
				{
					pfan = info[i]->add_addfan();
					pfan->set_addtype(m_UserOtherHu[i].oFan[j]);
					pfan->set_addnum(m_ft2fn->GetFan(m_UserOtherHu[i].oFan[j]));
					pfan->set_addtype2(m_UserOtherHu[i].bYinghu);
				}
			}
			else
			{
				info[i]->set_chairid(i);
				info[i]->set_specialtype(HU_INVALID);
				info[i]->set_fannum(0);
				info[i]->set_hucard(0);
				//info[i]->set_fantype(0);
			}

		}
		_uint8 dianpaoID = m_stHuScore[UserID[0]].stHu[0].bPasHuChairID;
		info[dianpaoID]->set_specialtype(m_bRobHu ? HU_BEI_QIANG_GANG : HU_YI_PAO_DUO_XIANG);
	}
	else
	{
		for (int i = 0; i < m_GAME_PLAYER; i++)
		{
			info[i]->set_chairid(i);
			info[i]->set_specialtype(HU_INVALID);
			info[i]->set_fannum(0);
			info[i]->set_hucard(0);
		}
	}
}


// 计算基础胡分
void Table_chengdumj::Calc_BaseScore(_uint8 ChairID, int iBaseScore[GAME_PLAYER])
{
	log_debug("%s tid:%d ChairID%d \n", __FUNCTION__, tid, ChairID);
	if (!m_bIsHu[ChairID])
	{
		return;
	}

	if (true == m_bChuJu[ChairID])
	{
		return;
	}

    //自摸加底低分
    int m_DiFenScore = 2;
	int nArrScore[GAME_PLAYER] = {0};
	int iScore = Calc_Fan(ChairID, m_oFan[ChairID]);

	for (int i = 0; i < m_GAME_PLAYER; i++)
	{
		if (
			(true == m_oFan[ChairID].Find(MJ_FAN_TYPE_GANGSHANGHUA))
			&& (false == m_conf_js.bAllDianGangHua) 
			&& (i != m_nZhiGangChairID)
			&& (iScore > 0)
			&& (INVALID_CHAIRID != m_nZhiGangChairID)
			)
		{
			nArrScore[i] = iScore - 1;
		}
		else
		{
			nArrScore[i] = iScore;
		}

		log_info("%s tid:%d uid:%d chairid:%d nArrScore[%d]\n", __FUNCTION__, tid, GetPlayerUid(ChairID), ChairID, nArrScore[i]);
	}

	log_info("%s tid:%d uid:%d chairid:%d bType[%d] bZiMoJiaFan[%d] iFengDingFan[%d]\n", __FUNCTION__, tid, GetPlayerUid(ChairID), ChairID, m_stHuScore[ChairID].stHu[0].bType, m_conf_js.bZiMoJiaFan, m_conf_js.iFengDingFan);
	// 自摸加一番
	if (m_stHuScore[ChairID].stHu[0].bType == HU_ZIMO)
	{	
		if (true == m_conf_js.bZiMoJiaFan)
		{
			for (int i = 0; i < m_GAME_PLAYER; i++)
			{
				nArrScore[i] += 1;
			}
		}
	}

	for (int i = 0; i < m_GAME_PLAYER; i++)
	{
		if (nArrScore[i] > m_conf_js.iFengDingFan)
		{
			nArrScore[i] = m_conf_js.iFengDingFan;
		}
	}

	// 记录胡牌信息
	if (m_GameEndTimes > 0)
	{
		m_stZiMoPaoHuInfo[ChairID].nHuChaird = ChairID;
		if (m_stHuScore[ChairID].stHu[0].bType == HU_ZIMO)
		{
			// 自摸false
			m_stZiMoPaoHuInfo[ChairID].IsZiMoOrPaoHu = false;
			m_stZiMoPaoHuInfo[ChairID].nZiMoChaird = ChairID;
			m_stZiMoPaoHuInfo[ChairID].nPaoHuChaird = INVALID_CHAIRID;
			m_stZiMoPaoHuInfo[ChairID].nDianPaoChaird = INVALID_CHAIRID;
			m_stZiMoPaoHuInfo[ChairID].nPaoFan = 0;
		}
		else
		{
			// 点炮true
			_uint8 iPaoId = m_stHuScore[ChairID].stHu[0].bPasHuChairID;
			m_stZiMoPaoHuInfo[ChairID].IsZiMoOrPaoHu = true;
			m_stZiMoPaoHuInfo[ChairID].nZiMoChaird = INVALID_CHAIRID;
			m_stZiMoPaoHuInfo[ChairID].nPaoHuChaird = ChairID;
			m_stZiMoPaoHuInfo[ChairID].nDianPaoChaird = iPaoId;
			m_stZiMoPaoHuInfo[ChairID].nPaoFan = nArrScore[iPaoId];
		}
		m_stZiMoPaoHuInfo[ChairID].nHuFan = nArrScore[ChairID];
	}

	for (int i = 0; i < m_GAME_PLAYER; i++)
	{
		int iTempS = 0;
		iTempS = nArrScore[i];
		if (iTempS > 0)
		{
			nArrScore[i] = 1;
		}
		for (int j = 0; j < iTempS; j++)
		{
			nArrScore[i] = nArrScore[i]*2;
		}

		log_info("%s tid:%d uid:%d chairid:%d nArrScore[%d]\n", __FUNCTION__, tid, GetPlayerUid(i), i, nArrScore[i]);
	}

    // 自摸加底
	if (m_stHuScore[ChairID].stHu[0].bType == HU_ZIMO && true == m_conf_js.bZiMoJiaDi)
	{	
		if (nArrScore[ChairID] == 0)
		{
			m_DiFenScore = 2;
		}
		else
		{
			m_DiFenScore = 1;
		}
		iBaseScore[ChairID] = (nArrScore[ChairID] * m_BaseScore + m_DiFenScore) * m_nJiChuDiFen;

		for (_uint8 c = NextChair(ChairID); c != ChairID; c = NextChair(c))
		{
			if (nArrScore[c] == 0)
			{
				m_DiFenScore = 2;
			}
			else
			{
				m_DiFenScore = 1;
			}
			iBaseScore[c] = (nArrScore[c] * m_BaseScore + m_DiFenScore) * m_nJiChuDiFen;
		}
	}
	else
	{
        // 自摸加番
		if (m_stHuScore[ChairID].stHu[0].bType == HU_ZIMO)
		{		
			iBaseScore[ChairID] = (nArrScore[ChairID] * m_BaseScore) * m_nJiChuDiFen;
			for (_uint8 c = NextChair(ChairID); c != ChairID; c = NextChair(c))
			{
				iBaseScore[c] = (nArrScore[c] * m_BaseScore) * m_nJiChuDiFen;
			}
		}
        // 炮胡
		else
		{
			if (nArrScore[ChairID] == 0)
			{
				iBaseScore[ChairID] = m_BaseScore * m_nJiChuDiFen;
			}
			else
			{
				iBaseScore[ChairID] = (nArrScore[ChairID] * m_BaseScore) * m_nJiChuDiFen;
			}
			
			for (_uint8 c = NextChair(ChairID); c != ChairID; c = NextChair(c))
			{
				if (nArrScore[c] == 0)
				{
					iBaseScore[c] = m_BaseScore * m_nJiChuDiFen;
				}
				else
				{
					iBaseScore[c] = (nArrScore[c] * m_BaseScore) * m_nJiChuDiFen;
				}
			}
		}
	}

	log_info("%s tid:%d uid:%d chairid:%d score [%d,%d,%d,%d]\n", __FUNCTION__, tid, GetPlayerUid(ChairID), ChairID, iBaseScore[0], iBaseScore[1], iBaseScore[2], iBaseScore[3]);
}


// 计算胡分
void Table_chengdumj::Calc_HuScore(_uint8 ChairID, int iBaseScore[GAME_PLAYER], int iScore[GAME_PLAYER])
{

	log_error("%s tid:%d ChairID:%d \n", __FUNCTION__, tid, ChairID);

	if (m_bIsHu[ChairID] && false == m_bChuJu[ChairID])
	{
		if ( m_stHuScore[ChairID].stHu[0].bType == HU_ZIMO)
		{
			for (_uint8 c = NextChair(ChairID); c != ChairID; c = NextChair(c))
			{
				if (m_bChuJu[c])
				{
					continue;
				}
				iScore[ChairID] += iBaseScore[c] ;
				iScore[c] -= iBaseScore[c] ;
			}
		}
		else
		{
			_uint8 iPaoId = m_stHuScore[ChairID].stHu[0].bPasHuChairID;
			iScore[ChairID] += iBaseScore[ChairID];
			iScore[iPaoId] -= iBaseScore[ChairID];
		}
		log_info("%s uid:%d chairid:%d score [%d,%d,%d,%d]\n", __FUNCTION__, GetPlayerUid(ChairID), ChairID, iScore[0], iScore[1], iScore[2], iScore[3]);
	}
}


// 计算胡牌顺序
void Table_chengdumj::Calc_HuPaiShunXu(int nGameEndTimes, _uint8 unCurChairId)
{

	log_error("%s tid:%d \n", __FUNCTION__, tid);

	bool bFist = true;

    // 计算胡牌顺序
	for (_uint8 iTemp = unCurChairId; iTemp != unCurChairId || true == bFist; iTemp = NextChair(iTemp))
	{
		bFist = false;
		if ((true == m_bIsHu[iTemp]) && (false == m_bChuJu[iTemp]))
		{
			for (int i = 0; i < m_GAME_PLAYER; i++)
			{
				if (INVALID_CHAIRID == m_nHuPaiShunXu[i])
				{
					m_nHuPaiShunXu[i] = iTemp;
					break;
				}
			}
		}
	}
	
}

// 计算
void Table_chengdumj::Calc_ZhongNiao(_uint8 ChairID, _uint8 ShaiNum, int iBaseScore[GAME_PLAYER], int iScore[GAME_PLAYER])
{
	
}

// 
void Table_chengdumj::Calc_Piao(_uint8 ChairID, int iScore[GAME_PLAYER])
{

}


// 判断平胡
bool Table_chengdumj::FormatCards(HandCards *pHc, MJ_win_pattern& pattern)
{
    return Table::FormatCards(pHc, pattern);
}


// 成都庆轮庄
// 轮庄规则，有人胡牌最先胡牌的玩家坐庄，一炮多响则点炮玩家坐庄，没人胡的时候则上局庄家的下家做庄。
void Table_chengdumj::TurnBanker()
{
 //   //摸完最后一张牌才算是结束
	//MJ_user_buffer UserID;
	//_uint8 bDraw = 0;

 //   if (CardPoolIsEmpty())
	//{
	//	if (MultiHu(UserID) == 0)
	//	{
	//		bDraw = 1;
	//	}
	//}
 //   // 没人胡牌..
 //   if (bDraw)
	//{
 //       // 庄家的下一家坐庄
	//	m_bBanker = NextChair(m_bBanker);
	//}
	//else
	//{
	//	// 是否一炮双响0
	//	if (INVALID_CHAIRID != m_nYiPaoDuoXiangCharid)
	//	{
	//		m_bBanker = m_nYiPaoDuoXiangCharid;
	//	}
	//	else
	//	{
	//		// 正常庄家
	//		m_bBanker = m_nHuPaiShunXu[0];
	//	}
	//}
}


// 游戏结束初始化数据
void Table_chengdumj::GameEndReset()
{
	Table::GameEndReset();

	memset(m_stGang, 0, sizeof(m_stGang));
	memset(m_stHuScore, 0, sizeof(m_stHuScore));
	//memset(m_bIsHu, 0, sizeof(m_bIsHu));
	for (int i = 0; i < m_GAME_PLAYER; i++)
	{
		m_louhu[i].Unlock();
	}

	m_niao.Clear();
	m_zhong_niao.Clear();
	m_DaNiaoID = INVALID_CHAIRID;
	m_bGameState = GAME_FREE;

	m_bRobWanGang = false;
	m_bRobGangHuJudge = false;
	m_WangGangPlayerOpType = TYPE_NULL;
	m_PassHuPlayer.Clear();
	for (int i = 0; i < m_GAME_PLAYER; i++)
	{
		if (m_SeatPlayer[i].player == NULL)
		{
			continue;
		}
		m_SeatPlayer[i].player->status =  SIT_DOWN;		
		{
			m_SeatPlayer[i].action = ACTION_SIDOWN;
		}
	}

	log_info("%s player uid:[%d,%d,%d,%d] game reset\n", __FUNCTION__,
		GetPlayerUid(0), GetPlayerUid(1), GetPlayerUid(2), GetPlayerUid(3));
	
	memset(&m_HandCard, 0, sizeof(m_HandCard));
	m_Outcard_Timer_running = false;
	ev_timer_stop(zjh.loop, &m_Outcard_Timer);
	m_Operate_Timer_running[0] = false;
	ev_timer_stop(zjh.loop, &m_Operate_Timer[0]);
	m_Operate_Timer_running[1] = false;
	ev_timer_stop(zjh.loop, &m_Operate_Timer[1]);
	m_Operate_Timer_running[2] = false;
	ev_timer_stop(zjh.loop, &m_Operate_Timer[2]);
	m_Operate_Timer_running[3] = false;
	ev_timer_stop(zjh.loop, &m_Operate_Timer[3]);

	memset(m_bUserOperate, 0, sizeof(m_bUserOperate));

    // 新增信息

	m_GameEndTimes = 0;
	m_nHaveDingQueNum = 0;
	m_nHaveHuanZhangNum = 0;
	m_nYiPaoDuoXiangCharid = INVALID_CHAIRID;
	m_nLastHuPaiNum = 0;
	m_nZhiGangChairID = INVALID_CHAIRID;
	m_nHuPaiChairid = INVALID_CHAIRID;\
	nOprOpType2 = 1;
	// 新增信息
	for (int i = 0; i < GAME_PLAYER; i++)
	{

		m_bCanDiHu[i] = 0;
		m_bChuJu[i] = false;
		// 玩家金币
		m_nGouPaiNum[i] = 0;
		m_nPlayerMoney[i] = 0;
		m_nHuPaiShunXu[i] = INVALID_CHAIRID;
		m_UserDingQueSe[i]= INVALID_MJ_CARD;
		m_nDaJiao[i] = 0;
		m_nDaJiaoFan[i] = 0;
		m_nTuiGang[i] = 0;
		m_nGangScore[i] = 0;
		m_nZhuanYiGang[i] = 0;
		m_nLastHuScore[i] = 0;
		m_nHuCard[i] = INVALID_MJ_CARD;
		m_nHuCardShunXu[i] = INVALID_MJ_CARD;
		m_nZiMoHuCard[i] = INVALID_MJ_CARD;
		m_nPaoHuCard[i] = INVALID_MJ_CARD;
		m_nHuType[i] = 3;
		for (int j = 0; j < 3; j++)
		{
			m_UserHuanZhangPai[i][j]= INVALID_MJ_CARD;
		}
		m_stQiHuInfo[i].InitQiHuInfo();
		m_stGangZhuanYiInfo[i].InitGangZhuanYi();
		m_stDianPaoInfo[i].InitDianPaoInfo();
	}

	for (int i = 0; i < m_GAME_PLAYER; i++)
	{
		for (int j = 0; j < 5; j++)
		{
			m_GangFen[i][j].init();
		}

		for (int iTemp2 = 0; iTemp2 < 5; iTemp2++)
		{
			for (int iTemp3 = 0; iTemp3 < 10; iTemp3++)
			{
				m_bQiGang[i][iTemp2][iTemp3] = false;
			}
		}
	}


	for (int i = 0; i < GAME_PLAYER - 1; i++)
	{
		srand((unsigned)time(NULL));
		double t = (rand() % 20) / 10.0f + 2.0;
		robot_dingque_stamp[i] = t;
		robot_dingque_timer_running[i] = false;
		robot_dingque_timer[i].data = this;
		ev_timer_init(&robot_dingque_timer[i], Table_chengdumj::robot_dingque_Timer_cb, robot_dingque_stamp[i], 0);
	}
    
}

// 获取听牌信息
int Table_chengdumj::GetDaJiaoInfo(_uint8 ChairID, tagListenInfos stListenInfo[14], _uint8& bListenLen)
{
	HandCards tempHandCard = m_HandCard[ChairID];
	bool bTrue = false;
	MJ_BUFFER<_uint8, POOL_CARD_TYPE_MAX> _relativity;
	GetAllHandCardRelativity(&tempHandCard, _relativity);

	for (_uint8 i = 0; i< _relativity.Size(); i++)
	{
		m_GameLogic.AddCard(&tempHandCard, _relativity[i]);
		MJ_fan_buffer fan_buffer;
		if (Check_Hu(ChairID, tempHandCard, fan_buffer) == TYPE_HU)
		{			
			if (stListenInfo[bListenLen].bHuLen <18)
			{				
				stListenInfo[bListenLen].bOutCard = _relativity[i];//要打出的牌       
				stListenInfo[bListenLen].HuInfo[stListenInfo[bListenLen].bHuLen].bCard = _relativity[i];//听的牌
				stListenInfo[bListenLen].HuInfo[stListenInfo[bListenLen].bHuLen].bFanShu = Calc_Fan(ChairID, fan_buffer);
				bTrue = true;
				stListenInfo[bListenLen].bHuLen++;
			}	
			//log_info("Check_Listening chairid:%d OutCard:0x%02x, listenLen:%d hulen:%d \n", ChairID, bCard, bListenLen+ 1, stListenInfo[bListenLen].bHuLen);
		}
		m_GameLogic.ChuPai(&tempHandCard, _relativity[i]);
	}

	if (bTrue)
	{ 	
		//log_info("%s uid:%d chairid:%d outcard:0x%02x hulen:%d\n", __FUNCTION__, GetPlayerUid(ChairID), ChairID, bCard, stListenInfo[bListenLen].bHuLen);
		if (bListenLen < 14)
		{
			bListenLen++;
		}		
	}
	return bListenLen;
}


// 获取听牌信息
int Table_chengdumj::GetListeningInfo(_uint8 ChairID, tagListenInfos stListenInfo[14], _uint8& bListenLen)
{	
	log_error("%s tid:%d ChairID:%d", __FUNCTION__, tid, ChairID);

	HandCards tempHandCard = m_HandCard[ChairID];

	_uint8 bCard = 0xFF;

	memset(stListenInfo, 0, sizeof(stListenInfo));
	for (int i = 0; i<tempHandCard.ChangeableCardsLen; i++)
	{
		// 如果是无效的牌则继续
		if (bCard == tempHandCard.ChangeableCards[i])
		{
			continue;
		}
		bCard = tempHandCard.ChangeableCards[i];
		Check_Listening(&tempHandCard, ChairID, bCard, stListenInfo, bListenLen);
	}	
	return (bListenLen != 0 ? TYPE_LISTEN : TYPE_NULL);
}

// 包含有青龙七对，请七对，龙七对，七对
bool Table_chengdumj::Is7Dui(_uint8 ChairID, HandCards& pHc, MJ_fan_buffer& oFan)
{
	//if (m_conf.m_bHu7dui)
	//{
	//	// 是否是七对
	//	bool bIsQiDui = false;
	//	// 每张牌的数量
	//	_uint8 CardDataNum[MJ_TYPE_COMMON][10] = { { 0 }, { 0 }, { 0 }, { 0 }, { 0 } };
	//	m_GameLogic.GetCardCount(&pHc, CardDataNum);
	//	// 
	//	int iLongQiDui = 0;
	//	if (m_GameLogic.L_IsQiDui_03(&pHc, INVALID_MJ_CARD, CardDataNum, 0, iLongQiDui))
	//	{
	//		if (iLongQiDui > 0)
	//		{
	//			oFan.Add(m_GameLogic.IsQingYiSe(&pHc) ? MJ_FAN_TYPE_QINGLONGQIDUI : MJ_FAN_TYPE_LONGQIDUI);
	//		}
	//		else 
	//		{
	//			oFan.Add(m_GameLogic.IsQingYiSe(&pHc) ? MJ_FAN_TYPE_QINGQIDUI : MJ_FAN_TYPE_QIDUI);
	//		}
	//		bIsQiDui = true;
	//	}

	//	addGuiCard(&pHc, guiCardCountMap);

	//	//硬胡和自摸
	//	if (bIsQiDui)
	//	{
	//		iLongQiDui = 0;
	//		m_bYingHu[ChairID] = false;
	//		if (m_GameLogic.L_IsQiDui_03(&pHc, INVALID_MJ_CARD, CardDataNum, 0, iLongQiDui))
	//		{
	//			if ((oFan.Find(MJ_FAN_TYPE_QINGLONGQIDUI) && iLongQiDui > 0 && m_GameLogic.IsQingYiSe(&pHc))
	//				|| (oFan.Find(MJ_FAN_TYPE_LONGQIDUI) && iLongQiDui > 0)
	//				|| (oFan.Find(MJ_FAN_TYPE_QINGQIDUI) && m_GameLogic.IsQingYiSe(&pHc))
	//				|| (oFan.Find(MJ_FAN_TYPE_QIDUI)))
	//			{
	//				m_bYingHu[ChairID] = true;
	//			}
	//		}
	//	}
	//	return bIsQiDui;

	//}
	return false;
}

// 胡牌牌型计算并添加响应番
int Table_chengdumj::Check_Hu_Pattern(_uint8 ChairID, const HandCards& pHc, MJ_fan_buffer& oFan, const MJ_win_pattern& oPattern)
{
	//HandCards tmpHc = pHc;

	//bool isFourGui = false;
	//std::map<_uint8, _uint8> guiCardCountMap;
	//int iLaiziNum = removeAllGuiCard(&tmpHc, guiCardCountMap, isFourGui);

	//_uint8 CardDataNum[MJ_TYPE_COMMON][10] = { { 0 }, { 0 }, { 0 }, { 0 }, { 0 } };
	//m_GameLogic.GetCardCount(&pHc, CardDataNum);

	//if (m_GameLogic.IsQingYiSe(&tmpHc) && m_GameLogic.L_IsPengPengHu(&tmpHc, INVALID_MJ_CARD, CardDataNum, iLaiziNum))
	//{
	//	oFan.Add(MJ_FAN_TYPE_QINGPENG);
	//}
	//else if (m_GameLogic.IsQingYiSe(&tmpHc))
	//{
	//	oFan.Add(MJ_FAN_TYPE_QINGYISE);
	//}
	//else if (m_GameLogic.L_IsPengPengHu(&tmpHc, INVALID_MJ_CARD, CardDataNum, iLaiziNum))
	//{
	//	oFan.Add(MJ_FAN_TYPE_PENGPENGHU);
	//}

	//addGuiCard(&tmpHc, guiCardCountMap);

	//if (oFan.IsNull())
	//{
	//	oFan.Add(MJ_FAN_TYPE_PINGHU);
	//}

	////硬胡
	//MJ_win_pattern oTempPattern;
	//m_bYingHu[ChairID] = false;
	//if (m_GameLogic.FormatCards(&tmpHc, oTempPattern.kan, oTempPattern.jiang))
	//{
	//	if ((oFan.Find(MJ_FAN_TYPE_QINGPENG) && m_GameLogic.IsQingYiSe(&tmpHc) && m_GameLogic.L_IsPengPengHu(&tmpHc, INVALID_MJ_CARD, CardDataNum, 0))
	//		|| (oFan.Find(MJ_FAN_TYPE_QINGYISE) && m_GameLogic.IsQingYiSe(&tmpHc))
	//		|| (oFan.Find(MJ_FAN_TYPE_PENGPENGHU) && m_GameLogic.L_IsPengPengHu(&tmpHc, INVALID_MJ_CARD, CardDataNum, 0))
	//		|| (oFan.Find(MJ_FAN_TYPE_PINGHU)))
	//	{
	//		m_bYingHu[ChairID] = true;
	//	}
	//}

	//m_UserOtherHu[ChairID].oFan.Clear();
	//m_UserOtherHu[ChairID].bYinghu = false;
	//if (!m_bYingHu[ChairID])
	//{
	//	EstimateOtherHu(ChairID, tmpHc, oFan);
	//}

	return TYPE_NULL;
}

// 胡牌番数判断逻辑
int Table_chengdumj::Check_Hu(_uint8 ChairID, HandCards& pHc, MJ_fan_buffer& oFan)
{
	//log_debug("%s tid:%d ChairID:%d \n", __FUNCTION__, tid, ChairID);

	if (pHc.ChangeableCardsLen > 14)
	{
		return TYPE_NULL;
	}
	// 手牌数量为0，或者手牌数量取余3不等于2，则不能胡牌
	if (pHc.ChangeableCardsLen != 0 && pHc.ChangeableCardsLen % 3 != 2)
	{
		return TYPE_NULL;
	}

	// 是否符合缺门
	for (int i = 0; i < pHc.ChangeableCardsLen; i++)
	{
		if (m_UserDingQueSe[ChairID] != INVALID_MJ_CARD && CardType(pHc.ChangeableCards[i]) == m_UserDingQueSe[ChairID])
		{
			return TYPE_NULL;
		}
	}

	if (IsQiDui(ChairID, pHc, oFan))
	{
		IsJiaFan(ChairID, pHc, oFan);
		return TYPE_HU;
	}
	else
	{
		MJ_win_pattern pattern;

		if (FormatCards(&pHc, pattern))
		{
			// 计算胡番
			if (IsPingHuLeiXing(ChairID, pHc, oFan))
			{
				IsJiaFan(ChairID, pHc, oFan);
				return TYPE_HU;
			}
		}
		return TYPE_NULL;
	}
	return TYPE_NULL;
}


// 保存游戏场景信息
void Table_chengdumj::save_game_scene_info()
{
	proto::game::chengdumj::tagCDGameScence stjsGameScence;
	proto::game::AckGameScene *stGameScene = stjsGameScence.mutable_gamesence();	
	
	stGameScene->set_outcardtime(m_Outcard_Timer_stamp);
	stGameScene->set_operatetime(m_Operate_Timer_stamp[0]);
	stGameScene->set_delaytime(m_bDelayTime);
	stGameScene->set_basescore(m_BaseScore);
	stGameScene->set_scenestatus(m_bGameState);
	stGameScene->set_curoutcarduser(m_bOutCardChairID);
	stGameScene->set_curoutcard(m_bCurOutCard);
	stGameScene->set_bankerchairid(m_bBanker);
	stGameScene->set_stand_money(stand_money);

	for (int i = 0; i < m_room_config.Size(); i++)
	{
		stGameScene->add_roomconfigid(m_room_config[i]);
	}

	stGameScene->set_scenestatus(m_bGameState);	//客户端认为状态不为free游戏就已经开始
	stjsGameScence.set_dingqueing(m_bGameState == GAME_CHOOSE_LACK ? true : false);
	stjsGameScence.set_huanzhanging(m_bGameState == GAME_CHANGECARD ? true : false);
	for (int i = 0; i < m_GAME_PLAYER; i++)
	{
		stjsGameScence.add_dingquese(m_UserDingQueSe[i]);

		proto::game::chengdumj::ReqChangeCard* strTempReqChangeCard = stjsGameScence.add_huanzhangpai();

		for (int j = 0; j < 3; j++)
		{
			strTempReqChangeCard->add_card(m_UserHuanZhangPai[i][j]);
		}

		if (INVALID_MJ_CARD != m_UserHuanZhangPai[i][0])
		{
			stjsGameScence.add_huanzhangisok(1);
		}
		else
		{
			stjsGameScence.add_huanzhangisok(0);
		}

		if (INVALID_MJ_CARD != m_UserDingQueSe[i])
		{
			stjsGameScence.add_dingqueisok(1);
		}
		else
		{
			stjsGameScence.add_dingqueisok(0);
		}

		stjsGameScence.add_nhutype(m_nHuType[i]);

		for (int j = 0; j < m_GAME_PLAYER; j++)
		{
			if (true == m_stDianPaoInfo[i].bIsUsed)
			{
				proto::game::chengdumj::DianPaoInfo* pstDianPaoInfo = stjsGameScence.add_stdianpaoinfo();
				pstDianPaoInfo->set_seatid(m_stDianPaoInfo[i].nDianPaoUser);
				pstDianPaoInfo->set_ndianpaocardshunxu(m_stDianPaoInfo[i].nDianPaoShunXu);
				pstDianPaoInfo->set_ndianpaocard(m_stDianPaoInfo[i].nDianPaoCard);
				break;
			}
		}
		// stjsGameScence.add_ndianpaopaishunxu(m_nHuCardShunXu[i]);

		stjsGameScence.add_nzimohucard(m_nZiMoHuCard[i]);
		stjsGameScence.add_ndianpaohucard(m_nPaoHuCard[i]);
	}

	stjsGameScence.set_dingque_remain_time(ev_timer_remaining(zjh.loop, &m_DingQue_Timer));
	stjsGameScence.set_huanzhang_remain_time(ev_timer_remaining(zjh.loop, &m_HuanZhang_Timer));

	ChengDuMJPeiZhi(stjsGameScence);

	std::string str = stjsGameScence.SerializeAsString();
	save_video_data(SERVER_GAME_SCENE, str);
}


// 没听
int Table_chengdumj::filterNoCardTingGroup(_uint8 ChairID,tagListenInfos stListenInfo[14], tagListenInfos tempListenInfo[14], _uint8 bListenLen)
{
	MJ_BUFFER<_uint8, POOL_CARD_TYPE_MAX> noCardList;   //??óDμ???
	MJ_BUFFER<_uint8, POOL_CARD_TYPE_MAX> haveCardList; //óDμ???
	int nRealLen = 0;
	for (int i = 0; i < bListenLen&&i<14; i++)
	{
		tagHuInfos(&tempHuInfo)[18] = tempListenInfo[i].HuInfo;
		for (int j = 0; j < tempListenInfo[i].bHuLen;j++)
		{
			_uint8 tempCard = tempHuInfo[j].bCard;
			if (haveCardList.Find(tempCard))
			{
				stListenInfo[nRealLen].bOutCard = tempListenInfo[i].bOutCard;//òa′ò3?μ???       
				stListenInfo[nRealLen].HuInfo[stListenInfo[nRealLen].bHuLen].bCard = tempCard;//ìyμ???
				stListenInfo[nRealLen].HuInfo[stListenInfo[nRealLen].bHuLen].bFanShu = tempHuInfo[j].bFanShu;
				stListenInfo[nRealLen].bHuLen++;
			}
			else if (!noCardList.Find(tempCard))
			{
				int nCardLeftNum = Get_Card_Left_Num(ChairID, tempCard);
				if (nCardLeftNum == 0)
				{
					noCardList.Add(tempCard);
				}
				else
				{
					haveCardList.Add(tempCard);
					stListenInfo[nRealLen].bOutCard = tempListenInfo[i].bOutCard;//òa′ò3?μ???       
					stListenInfo[nRealLen].HuInfo[stListenInfo[nRealLen].bHuLen].bCard = tempCard;//ìyμ???
					stListenInfo[nRealLen].HuInfo[stListenInfo[nRealLen].bHuLen].bFanShu = tempHuInfo[j].bFanShu;
					stListenInfo[nRealLen].bHuLen++;
				}
			}
		}
		if (stListenInfo[nRealLen].bHuLen > 0)
		{
			nRealLen++;
		}
	}
	return nRealLen;
}

// 
void Table_chengdumj::CalcRecordDianPaoInfo(MJ_user_buffer& UserID, int iTotalScore[GAME_PLAYER])
{
	UserID.Clear();

	for (int i = 0; i < m_GAME_PLAYER; i++)
	{
		if (m_bIsHu[i] == true && m_bChuJu[i] == false)
		{
			UserID.Add(i);
		}
	}

	if (UserID.Size() == 1)
	{
		_uint8 ChairID = UserID[0];
		if (m_stHuScore[ChairID].stHu[0].bType == HU_ZIMO)
		{
			m_totalrecordinfo.hu[ChairID]++;
			m_totalrecordinfo.zimo[ChairID]++;
			log_info("%s uid:%d chairid:%d HU_ZIMO\n", __FUNCTION__, GetPlayerUid(ChairID), ChairID);
		}
		else
		{
			m_totalrecordinfo.hu[ChairID]++;
			m_totalrecordinfo.paohu[ChairID]++;
			_uint8 PaoID = m_stHuScore[ChairID].stHu[0].bPasHuChairID;
			m_totalrecordinfo.dianpao[PaoID]++;
			m_totalrecordinfo.piao_count[PaoID]++;
			log_info("%s uid:%d chairid:%d HU_PAOHU dianpaochairid:0x%02x \n", __FUNCTION__, GetPlayerUid(ChairID), ChairID, PaoID);
		}

	}
	else if (UserID.Size() > 1)
	{
		for (int i = 0; i < UserID.Size(); i++)
		{
			_uint8 ChairID = UserID[i];
			if (m_stHuScore[ChairID].stHu[0].bType == HU_PAOHU)
			{
				m_totalrecordinfo.hu[ChairID]++;
				m_totalrecordinfo.paohu[ChairID]++;

				_uint8 PaoID = m_stHuScore[ChairID].stHu[0].bPasHuChairID;
				m_totalrecordinfo.dianpao[PaoID]++;
				m_totalrecordinfo.piao_count[PaoID]++;
			}
		}
	}	

	if (m_bGangCalcImme)
	{
		for (int i = 0; i < m_GAME_PLAYER; i++)
		{
			if (iTotalScore[i] + m_stGang[i].iTotalScore > 0)
			{
				m_totalrecordinfo.win_counts[i]++;
			}
		}
	}
	else
	{
		for (int i = 0; i < m_GAME_PLAYER; i++)
		{
			if (iTotalScore[i] > 0)
			{
				m_totalrecordinfo.win_counts[i]++;
			}
		}		
	}

}

// 
void Table_chengdumj::CalcRecordInfo(const MJ_user_buffer& UserID, int iTotalScore[GAME_PLAYER])
{
	m_totalrecordinfo.banker[m_bBanker]++;
	for (int i = 0; i < m_GAME_PLAYER; i++)
	{
		if (m_SeatPlayer[i].player != NULL)
		{
			m_totalrecordinfo.total_score[i] = m_SeatPlayer[i].player->money - roomGold;
		}
	}	

	for (int i = 0; i < m_GAME_PLAYER; i++)
	{
		m_totalrecordinfo.gang_times[i] += m_stGang[i].Gang.Size();
	}
	for (int i = 0; i < m_GAME_PLAYER; i++)
	{
		for (int j = 0; j < m_stGang[i].Gang.Size(); j++)
		{
			if (m_stGang[i].Gang[j].iType == TYPE_ZHIGANG)
			{
				m_totalrecordinfo.dian_gang_times[m_stGang[i].Gang[j].ChairID]++;
			}
		}
	}
}

_uint8 SortNode(SGSI_SortNodes n[GAME_PLAYER], _uint8 game_player)
{
	SGSI_SortNodes tmp;
	for (int i = 0; i < (game_player - 1); i++)
	{
		for (int j = 0; j < (game_player - 1); j++)
		{
			if (n[j].iTotalScore < n[j + 1].iTotalScore)
			{
				tmp = n[j];
				n[j] = n[j + 1];
				n[j + 1] = tmp;
			}
		}
	}
	if (n[0].iTotalScore == 0)
		return 0;
	int iMax = n[0].iTotalScore;
	_uint8 iCounts = 1;
	for (int i = 1; i < game_player; i++)
	{
		if (iMax == n[i].iTotalScore)
			iCounts++;
	}
	return iCounts;
}

// 游戏计算算分信息
void Table_chengdumj::Proto_GameEnd_ScoreInfo(Player* player, proto::game::GameEndRecord &ger)
{
	// Table::Proto_GameEnd_ScoreInfo(player, ger);

	for (int i = 0; i < m_GAME_PLAYER; i++)
	{
		proto::game::ReordInfo* info = ger.add_userrecoreinfo();
		// 自摸次数
		info->set_banker_num(m_stRecodeInfo[i].iZiMoTimes);
		// 接炮次数
		info->set_hu_num(m_stRecodeInfo[i].iJiePaoTimes);
		// 点炮次数
		info->set_outtime_num(m_stRecodeInfo[i].iDianPaoTimes);
		info->set_totalscore(m_totalrecordinfo.total_score[i]);
		// info->set_totalscore(m_stRecodeInfo[i].iAnGangTimes);
		// 明杠次数
		info->set_wincounts(m_stRecodeInfo[i].iMingGangTimes);
		// 查大叫次数
		info->set_gangtimes(m_stRecodeInfo[i].iChaJiaoTimes);
		// 
		info->set_diangangtimes(m_stRecodeInfo[i].iAnGangTimes);
	}

	_uint8 iCounts;
	SGSI_SortNodes dianpao[GAME_PLAYER];
	SGSI_SortNodes winner[GAME_PLAYER];
	for (int i = 0; i < m_GAME_PLAYER; i++)
	{
		dianpao[i].ChairID = i;
		dianpao[i].iTotalScore = m_totalrecordinfo.dianpao[i];
		winner[i].ChairID = i; 
		winner[i].iTotalScore = m_totalrecordinfo.total_score[i];
	}
	iCounts = SortNode(dianpao, m_GAME_PLAYER);
	for (int i = 0; i < iCounts; i++)
	{
		ger.add_mostdianpaouser(dianpao[i].ChairID);
	}
	iCounts = SortNode(winner, m_GAME_PLAYER);
	for (int i = 0; i < iCounts; i++)
	{
		ger.add_mostwinscoreuser(winner[i].ChairID);
	}

	ger.set_datatime(time(NULL));

	for (_uint8 i = 0; i < m_GAME_PLAYER; ++i)
	{
		proto::game::ReordInfo* pReordInfo = ger.mutable_userrecoreinfo(i);
		pReordInfo->set_ex1(m_totalrecordinfo.piao_count[i]);
		log_info("%s uid:%d chairid:%d piao_count:%d\n", __FUNCTION__, GetPlayerUid(i), i, m_totalrecordinfo.piao_count[i]);
	}
}

// 玩家操作之后
void Table_chengdumj::UserAfterOperate(_uint8 ChairID, int iOpType, _uint8 bCard)
{
	m_bCurChairID = ChairID;
	log_info("%s uid:%d chairid:%d operate:0x%x card:0x%02x\n", __FUNCTION__, GetPlayerUid(ChairID), ChairID, iOpType, bCard);

	m_bUserOperate[ChairID] = TYPE_NULL;

	m_trustee.get()->UserAfterOperate(ChairID);
}

// 处理客户端消息
int Table_chengdumj::dispatch_client(int cmd, Client *client, Player *player)
{
	if (client == NULL || player == NULL)
	{
		log_error("%s:%d\t%s roomid:%d client has disbanded \n",__FILE__,__LINE__,__FUNCTION__ , tid);
		return -1;
	}

	switch (cmd)
	{
		case CLIENT_GAME_END_RECORD_REG:
		{
			proto::game::GameEndRecord ger;		
			Proto_GameEnd_ScoreInfo(player, ger);
			unicast(SERVER_GAME_RECORD, player, ger);
			return 0;
		}
		case CLIENT_REQ_ROUND_SCORE:
			Send_GameRound_ScoreInfo(player);
			return 0;
		case CLIENT_MJ_CHOOSE_PIAO:
			// RecvClientChoosePiao(client, player);
			return 0;
			// 换牌
		case CLIENT_MJ_CHANGE_CARD:
			RecvClientChooseHuanZhang(client, player);
			return 0;	
		case CLIENT_MJ_CHOOSE_LACKTYPE:
			RecvClientChooseDingQue(client, player);
			return 0;
		default:
			break;
	}

	return Table::dispatch_client(cmd, client, player);

	//log_error("%s tid:%d invalid command[%d] uid:%d\n", __FUNCTION__, tid, cmd, player->uid);
	//return -1;
}

/*
 *  开始定缺
 */
void Table_chengdumj::StartChooseDingQue()
{
	log_error("%s tid:%d ", __FUNCTION__, tid);

	m_bGameState = GAME_CHOOSE_LACK;

	proto::game::chengdumj::AckUserLackStart stAck;
    
	stAck.set_istimeout(0);

	stAck.set_remain_time(m_DingQue_Timer_stamp);

	broadcast(SERVER_CHENGDU_START_DINGQUE, stAck);

	std::string str = stAck.SerializeAsString();
	
	save_video_data(SERVER_CHENGDU_START_DINGQUE, str);

	//TIMER_AGAIN_s(m_DingQue_Timer);
	m_DingQue_Timer_running = true;
	TIMER_START(m_DingQue_Timer, m_DingQue_Timer_stamp);

	if (!match->m_FightMode)
	{
		for (int i = 0; i < m_GAME_PLAYER - 1; i++)
		{
			robot_dingque_timer_running[i] = true;
			TIMER_START(robot_dingque_timer[i], robot_dingque_stamp[i]);
		}
	}
}

/*
 *  开始换张
 */
void Table_chengdumj::StartChooseHuanZhang()
{
	log_error("%s tid:%d ", __FUNCTION__, tid);

	m_bGameState = GAME_CHANGECARD;

	proto::game::chengdumj::AckUserChangeCardStart stAck;
	stAck.set_istimeout(0);
	stAck.set_remain_time(m_HuanZhang_Timer_stamp);
	broadcast(SERVER_CHENGDU_START_HUANZHANG, stAck);
	
	std::string str = stAck.SerializeAsString();
	save_video_data(SERVER_CHENGDU_START_HUANZHANG, str);
	
	m_HuanZhang_Timer_running = true;
	TIMER_AGAIN_s(m_HuanZhang_Timer);
}

/*
 *  开始收到选择定缺信息
 */
void Table_chengdumj::RecvClientChooseDingQue(Client *client, Player * player)
{
	if (player == NULL)
	{
		log_error("%s tid:%d error, player is NULL \n", __FUNCTION__, tid);
		return;
	}

	_uint8 chairid = player->seatid;

	if (m_UserDingQueSe[chairid] != INVALID_MJ_CARD)
	{
		return ;
	}

	proto::game::chengdumj::ReqChooseLackType stReq;
	stReq.ParseFromArray(client->m_Buffer->proto_data, client->m_Buffer->proto_head.length);
	int type = stReq.type();

	log_info("%s tid:%d info, uid:%d ChairId:%d, type[%d]\n", __FUNCTION__, tid, GetPlayerUid(chairid), chairid, type);

	if (type == INVALID_MJ_CARD || type < 0 || type >= 4)
	{
		log_error("%s tid:%d error, type:%d is error \n", __FUNCTION__, tid, type);
		return;
	}

	if (m_UserDingQueSe[chairid] != INVALID_MJ_CARD)
	{
		log_debug("matchid<%d>, tid<%d>, chairid<%d> already dingque", matchid, tid, chairid);
		return;
	}

	m_UserDingQueSe[chairid] = type;

	//广播玩家选择的飘
	proto::game::chengdumj::AckUserLackTypeOk stAck;

	for (int i = 0; i < m_GAME_PLAYER; i++)
	{
		stAck.add_chairid(i);
		if (INVALID_MJ_CARD != m_UserDingQueSe[i])
		{
			stAck.add_isok(1);
		}
		else
		{
			stAck.add_isok(0);
		}
	}
	// 广播玩家选定信息
	broadcast(SERVER_CHENGDU_USER_DINGQUE_DONE, stAck);

	std::string str = stAck.SerializeAsString();

	save_video_data(SERVER_CHENGDU_USER_DINGQUE_DONE, str);

	int count = 0;
	for (int i=0; i<m_GAME_PLAYER; i++)
	{
		if (INVALID_MJ_CARD != m_UserDingQueSe[i])
			count++;
	}
	if (count == m_GAME_PLAYER)
	{	
		GameStartAfterDingQue();
	}
}

void Table_chengdumj::RecvClientChooseDingQue2(_uint8 chairid, int type)
{
	if (m_UserDingQueSe[chairid] != INVALID_MJ_CARD)
	{
		log_debug("matchid<%d>, tid<%d>, chairid<%d> already dingque type<%d>", matchid, tid, chairid, m_UserDingQueSe[chairid]);
		return;
	}
	log_debug("matchid<%d>, tid<%d>, chairid<%d> dingque type<%d>", matchid, tid, chairid, type);

	m_UserDingQueSe[chairid] = type;

	//广播玩家选择的飘
	proto::game::chengdumj::AckUserLackTypeOk stAck;

	for (int i = 0; i < m_GAME_PLAYER; i++)
	{
		stAck.add_chairid(i);
		if (INVALID_MJ_CARD != m_UserDingQueSe[i])
		{
			stAck.add_isok(1);
		}
		else
		{
			stAck.add_isok(0);
		}
	}
	// 广播玩家选定信息
	broadcast(SERVER_CHENGDU_USER_DINGQUE_DONE, stAck);

	std::string str = stAck.SerializeAsString();

	save_video_data(SERVER_CHENGDU_USER_DINGQUE_DONE, str);

	int count = 0;
	for (int i = 0; i < m_GAME_PLAYER; i++)
	{
		if (INVALID_MJ_CARD != m_UserDingQueSe[i])
			count++;
	}
	if (count == m_GAME_PLAYER)
	{
		GameStartAfterDingQue();
	}
}

int Table_chengdumj::DingQueDecision(_uint8 chairId)
{
	std::map<int, int> cardTypeCnt;
	for (int i = 0; i < m_HandCard[chairId].ChangeableCardsLen; i++)
	{
		cardTypeCnt[CardType(m_HandCard[chairId].ChangeableCards[i])]++;
	}
	int num = cardTypeCnt.begin()->second;
	int type = cardTypeCnt.begin()->first;
	for (std::map<int, int>::iterator it = cardTypeCnt.begin(); it != cardTypeCnt.end(); it++)
	{
		if (num > it->second)
		{
			num = it->second;
			type = it->first;
		}
	}
	return type;
}


bool Table_chengdumj::IsCardInHand(_uint8 ChairID, _uint8 bCard)
{
	bool bInHand = false;
	
	if (ChairID >= m_GAME_PLAYER)
	{
		return false;
	}

	for (int i = 0; i < m_HandCard[ChairID].ChangeableCardsLen; i++)
	{
		if (bCard == m_HandCard[ChairID].ChangeableCards[i] && bCard != INVALID_MJ_CARD)
		{
			bInHand = true;
			break;
		}
	}
	return bInHand;
}

/*
 *  开始收到选择换张信息
 */
void Table_chengdumj::RecvClientChooseHuanZhang(Client *client, Player * player)
{
	if (player == NULL)
	{
		log_error("%s tid:%d error, player is NULL \n", __FUNCTION__, tid);
		return;
	}

	_uint8 chairid = player->seatid;
	if (m_UserHuanZhangPai[chairid][0] != INVALID_MJ_CARD)
	{
		log_error("%s tid:%d error, m_UserHuanZhangPai is not NULL \n", __FUNCTION__, tid);
		return ;
	}
	proto::game::chengdumj::ReqChangeCard stReq;
	stReq.ParseFromArray(client->m_Buffer->proto_data, client->m_Buffer->proto_head.length);
	int isChange = stReq.ischange();
	if (0 == isChange)
	{
		log_error("%s tid:%d error, isChange is ZERO \n", __FUNCTION__, tid);
		//广播玩家选择换张完成
		proto::game::chengdumj::AckChangeCardOk stAck;
		stAck.add_chairid(chairid);
		stAck.add_isok(0);
		unicast(SERVER_CHENGDU_USER_HUANZHANG_DONE, chairid, stAck);

		std::string str = stAck.SerializeAsString();
		save_video_data(SERVER_CHENGDU_USER_HUANZHANG_DONE, str);
		return;
	}

	int iCardSize = stReq.card_size();
	
	if (iCardSize != 3)
	{
		log_error("%s tid:%d error, iCardSize is %d \n", __FUNCTION__, tid, iCardSize);
		//广播玩家选择换张完成
		proto::game::chengdumj::AckChangeCardOk stAck;
		stAck.add_chairid(chairid);
		stAck.add_isok(0);
		unicast(SERVER_CHENGDU_USER_HUANZHANG_DONE, chairid, stAck);

		std::string str = stAck.SerializeAsString();
		save_video_data(SERVER_CHENGDU_USER_HUANZHANG_DONE, str);
		return;
	}
	
	for (int i = 0; i < iCardSize; i++)
	{
		m_UserHuanZhangPai[chairid][i] = stReq.card(i);
		if (false == IsCardInHand(chairid, m_UserHuanZhangPai[chairid][i]) || CardType(m_UserHuanZhangPai[chairid][0]) != CardType(m_UserHuanZhangPai[chairid][i]))
		{
			for (int k = 0; k < GAME_PLAYER; k++)
			{
				for (int j = 0; j < 3; j++)
				{
					m_UserHuanZhangPai[k][j]= INVALID_MJ_CARD;
				}
			}

			//广播玩家选择换张完成
			proto::game::chengdumj::AckChangeCardOk stAck;
			stAck.add_chairid(chairid);
			stAck.add_isok(0);
			unicast(SERVER_CHENGDU_USER_HUANZHANG_DONE, chairid, stAck);

			std::string str = stAck.SerializeAsString();
			save_video_data(SERVER_CHENGDU_USER_HUANZHANG_DONE, str);

			log_error("%s tid:%d error, chairid:%d m_UserHuanZhangPai[chairid][i] is 0x%02x \n", __FUNCTION__, tid, iCardSize, chairid, m_UserHuanZhangPai[chairid][i]);
			return;
		}
		log_info("%s tid:%d info, uid:%d ChairId:%d, m_UserHuanZhangPai[0x%02x]\n", __FUNCTION__, tid, GetPlayerUid(chairid), chairid, m_UserHuanZhangPai[chairid][i]);
	}

	//广播玩家选择换张完成
	proto::game::chengdumj::AckChangeCardOk stAck;
	stAck.add_chairid(chairid);
	stAck.add_isok(1);
	broadcast(SERVER_CHENGDU_USER_HUANZHANG_DONE, stAck);

	std::string str = stAck.SerializeAsString();
	save_video_data(SERVER_CHENGDU_USER_HUANZHANG_DONE, str);

	int count = 0;
	for (int i=0; i<m_GAME_PLAYER; i++)
	{
		if (m_UserHuanZhangPai[i][0] != INVALID_MJ_CARD)
			count++;
	}
	
	if (count == m_GAME_PLAYER)
	{	
		GameStartAfterHuanZhang();
	}
}

/*
 *  换张之后的操作
 */
void Table_chengdumj::GameStartAfterHuanZhang()
{
	m_HuanZhang_Timer_running = false;
	TIMER_STOP(m_HuanZhang_Timer);

	m_bGameState = GAME_PLAYING;

	// 定缺换牌
	bool bPass = true;
	_uint8 nStartHuanZhangUser = 0;
	// 随机换
	srand(int(time(0)));
	_uint8 GetPaiChairID[GAME_PLAYING] = {INVALID_CHAIRID};

	while(1)
	{
		_uint8 TempGetPaiChairID[GAME_PLAYING] = {INVALID_CHAIRID};
		bool bHaveChongFu = false;
		for (int i = 0; i < m_GAME_PLAYER; i++)
		{
			int index = 0;
			while(1)
			{
				index = rand() % m_GAME_PLAYER;
				if (index != i)
				{
					break;
				}
			}
			TempGetPaiChairID[i] = index;
		}
		for (int i = 0; i < m_GAME_PLAYER; i++)
		{
			if (i == TempGetPaiChairID[i])
			{
				bHaveChongFu = true;
				break;
			}
			for (int j = i; j < m_GAME_PLAYER; j++)
			{
				if (i != j)
				{
					if (TempGetPaiChairID[i] == TempGetPaiChairID[j])
					{
						bHaveChongFu = true;
						break;
					}
				}
			}
			if (bHaveChongFu == true)
			{
				break;
			}
		}
		if (bHaveChongFu == false)
		{
			for (int i = 0; i < m_GAME_PLAYER; i++)
			{
				GetPaiChairID[i] = TempGetPaiChairID[i];
			}
			break;
		}
	}
	//for (int i = 0; i < m_GAME_PLAYER; i++)
	//{
	//	
	//	int nIndex = 0;
	//	nChangeNum++;
	//	if (m_GAME_PLAYER - nChangeNum < 2)
	//	{
	//		int nFirstChairID = INVALID_CHAIRID;
	//		int nSecondChairID = INVALID_CHAIRID;
	//		for (int j = 0; j < m_GAME_PLAYER; j++)
	//		{
	//			if (bGetPaiChairID[j] == false)
	//			{
	//				if (nFirstChairID == INVALID_CHAIRID)
	//				{
	//					nFirstChairID = j;
	//				}
	//				else
	//				{
	//					nSecondChairID = j;
	//				}
	//			}
	//		}

	//		for (int j = 0; j < m_GAME_PLAYER; j++)
	//		{
	//			if (GetPaiChairID[j] == INVALID_CHAIRID)
	//			{
	//				if (j == nFirstChairID)
	//				{
	//					GetPaiChairID[j] = nSecondChairID;
	//					GetPaiChairID[j+1] = nFirstChairID;
	//				}
	//				else
	//				{
	//					GetPaiChairID[j] = nFirstChairID;
	//					GetPaiChairID[j+1] = nSecondChairID;
	//				}
	//				break;
	//			}
	//		}
	//		break;
	//	}
	//	for (int j = 0; j < m_GAME_PLAYER; j++)
	//	{
	//		if (i == j)
	//		{
	//			continue;
	//		}
	//		else
	//		{
	//			if (bGetPaiChairID[j] == false && GetPaiChairID[i] == INVALID_CHAIRID)
	//			{
	//				if (nIndex == index)
	//				{
	//					bGetPaiChairID[index] = true;
	//					GetPaiChairID[i] = index;
	//					break;
	//				}
	//				else
	//				{
	//					nIndex++;
	//				}
	//			}
	//		}
	//	}
	//}

	log_info("%s tid:%d info, ChairdId[%d, %d, %d, %d] \n", __FUNCTION__, tid, GetPaiChairID[0], GetPaiChairID[1], GetPaiChairID[2], GetPaiChairID[3]);

	for (int i = nStartHuanZhangUser; i != nStartHuanZhangUser || true == bPass; i = NextChair(i))
	{
		proto::game::chengdumj::AckChangeCard stAck;
		stAck.set_chairid(i);
		bPass = false;
		for (int j = 0; j < 3; j++)
		{
			m_GameLogic.RemoveCard(&m_HandCard[i], m_UserHuanZhangPai[i][j], 1);
			m_GameLogic.AddCard(&m_HandCard[i], m_UserHuanZhangPai[GetPaiChairID[i]][j]);
			stAck.add_card(m_UserHuanZhangPai[GetPaiChairID[i]][j]);
			stAck.add_oldcard(m_UserHuanZhangPai[i][j]);
			log_info("%s tid:%d info, uid:%d, ChairdId:%d, PaiShunXu:%d, add_oldcard[%d]\n", __FUNCTION__, tid, GetPlayerUid(i), i, j, m_UserHuanZhangPai[i][j]);
			log_info("%s tid:%d info, uid:%d, ChairdId:%d, PaiShunXu:%d,  add_card[%d]\n", __FUNCTION__, tid, GetPlayerUid(i), i, j, m_UserHuanZhangPai[GetPaiChairID[i]][j]);
		}

		m_GameLogic.SortCard(m_HandCard[i].ChangeableCards, m_HandCard[i].ChangeableCardsLen);
		unicast(SERVER_CHENGDU_ALL_HUANZHANG_DONE, i, stAck);

		std::string str = stAck.SerializeAsString();

		// log_info("%s tid:%d info, %s \n", __FUNCTION__, tid, stAck.DebugString().c_str());

		save_video_data(SERVER_CHENGDU_ALL_HUANZHANG_DONE, str);
	}

	// 三色牌则定缺，两色不定缺
	if (108 == m_conf_js.iPaiZhangShu)
	{
		// 发送协议
		StartChooseDingQue();
	}
}

/*
 *  定缺之后的操作
 */
void Table_chengdumj::GameStartAfterDingQue()
{
	if (SeatPlayerCount() == m_GAME_PLAYER);
	{
		m_trustee.get()->TriggerTimer(INVALID_CHAIRID);
		log_info("matchid<%d>, tid<%d>, roundinfo trigger timer\n", matchid, tid);
	}

	m_DingQue_Timer_running = false;
	TIMER_STOP(m_DingQue_Timer);
	m_bGameState = GAME_PLAYING;

	proto::game::chengdumj::AckUserLackType stAck;

	for (int i = 0; i < m_GAME_PLAYER; i++)
	{
		stAck.add_lacktype(m_UserDingQueSe[i]);
		log_info("%s tid:%d info, uid:%d ChairId:%d, m_UserDingQueSe\n", __FUNCTION__, tid, GetPlayerUid(i), i, m_UserDingQueSe[i]);
	}

	// 发送定缺信息
	broadcast(SERVER_CHENGDU_ALL_DINGQUE_DONE, stAck);

	std::string str = stAck.SerializeAsString();
	save_video_data(SERVER_CHENGDU_ALL_DINGQUE_DONE, str);

	m_bFirstCard = true;
	OnUserGetCard(m_bBanker);
	m_bFirstCard = false;

}

// 定缺超时
void Table_chengdumj::DingQue_Timer_cb(struct ev_loop *loop, struct ev_timer *w, int revents)
{
	Table_chengdumj* table = (Table_chengdumj*)w->data;
	table->m_DingQue_Timer_running = false;
	TIMER_STOP(table->m_DingQue_Timer);
	// 定缺超时
	proto::game::chengdumj::AckUserLackStart stAck;
	// 定缺超时
	stAck.set_istimeout(1);

	stAck.set_remain_time(table->m_DingQue_Timer_stamp);

	table->broadcast(SERVER_CHENGDU_START_DINGQUE, stAck);

	/*定缺超时，主动给玩家定缺*/
	for (int i = 0; i < table->m_GAME_PLAYER; i++)
	{
		table->RecvClientChooseDingQue2(i, table->DingQueDecision(i));
	}
}

// 换张超时
void Table_chengdumj::HuanZhang_Timer_cb(struct ev_loop *loop, struct ev_timer *w, int revents)
{
	Table_chengdumj* table = (Table_chengdumj*)w->data;
	table->m_HuanZhang_Timer_running = false;
	TIMER_STOP(table->m_HuanZhang_Timer);
	// 定缺超时
	proto::game::chengdumj::AckUserChangeCardStart stAck;
	// 定缺超时
	stAck.set_istimeout(1);

	stAck.set_remain_time(table->m_HuanZhang_Timer_stamp);

	table->broadcast(SERVER_CHENGDU_START_HUANZHANG, stAck);
}

void Table_chengdumj::robot_dingque_Timer_cb(struct ev_loop *loop, struct ev_timer *w, int revents)
{
	Table_chengdumj* table = (Table_chengdumj*)w->data;
	TIMER_STOP(*w);
	for (int i = 0; i < table->m_GAME_PLAYER; i++)
	{
		if (table->m_SeatPlayer[i].player->m_isRobot &&
			table->m_UserDingQueSe[i] == INVALID_MJ_CARD)
		{
			table->RecvClientChooseDingQue2(i, table->DingQueDecision(i));
			break;
		}
	}
}

// 自动飘
void Table_chengdumj::AutoPiao()
{
	//for (_uint8 i = 0; i < m_GAME_PLAYER; ++i)
	//{
	//	if (m_UserPiaoScore[i] < 0)
	//	{
	//		m_UserPiaoScore[i] = m_UserLastPiaoScore[i] >= 0 ? m_UserLastPiaoScore[i] : 0;
	//		m_UserLastPiaoScore[i] = m_UserPiaoScore[i];

	//		//广播玩家选择的飘
	//		proto::game::chengdumj::AckBroadPiao stAck;
	//		stAck.set_chairid(i);
	//		stAck.set_userpiao(m_UserPiaoScore[i]);
	//		broadcast(SERVER_BROAD_PIAO, stAck);
	//		log_info("%s tid:%d chairid:%d piaonum:%d .\n", __FUNCTION__, tid, i, stAck.userpiao());
	//	}
	//}

	//GameStartAfterHuanZhang();
}

//检查是否能炮胡
bool Table_chengdumj::Check_CanPaoHu(_uint8 ChairID, _uint8 bCard)
{
	//if (m_bIsPassHu[ChairID])
	//{
	//	log_info("%s tid:%d ChairID:%d is pass hu .\n", __FUNCTION__, tid, ChairID);
	//	return false;
	//}

	//if (bCard == m_bLaiziCard)
	//{
	//	log_info("%s tid:%d ChairID:%d bCard:%d is laizi .\n", __FUNCTION__, tid, ChairID, bCard );
	//	return false;
	//}

	//bool isFourGui = false;
	//std::map<_uint8, _uint8> guiCardCountMap;
	//HandCards tempHandCard = m_HandCard[ChairID];
	//int iLaiziNum = removeAllGuiCard(&tempHandCard, guiCardCountMap, isFourGui);

	//if (m_GameLogic.IsJianZiHu(tempHandCard.ChangeableCards, tempHandCard.ChangeableCardsLen, iLaiziNum))
	//{
	//	log_info("%s tid:%d ChairID:%d iGameType:%d iLaiziNum:%d is jianzihu .\n", __FUNCTION__, tid, ChairID, m_conf_js.iGameType, iLaiziNum);
	//	return false;
	//}

	//if (m_conf_js.iGameType == 0 && iLaiziNum > 0 && !m_bYingHu[ChairID])
	//{
	//	if (!m_UserOtherHu[ChairID].bYinghu)
	//	{
	//		log_info("%s tid:%d ChairID:%d iGameType:%d iLaiziNum:%d have laizi not yinghu .\n", __FUNCTION__, tid, ChairID, m_conf_js.iGameType, iLaiziNum);
	//		return false;	
	//	}	
	//	m_bOtherHu[ChairID] = true;
	//}

	//if (m_conf_js.iGameType == 1 && iLaiziNum >= 2)
	//{
	//	log_info("%s tid:%d ChairID:%d iGameType:%d iLaiziNum:%d laizi more .\n", __FUNCTION__, tid, ChairID, m_conf_js.iGameType, iLaiziNum);
	//	return false;
	//}

	//if (m_conf_js.iGameType == 2)
	//{
	//	log_info("%s tid:%d ChairID:%d iGameType:%d iLaiziNum:%d only zimo .\n", __FUNCTION__, tid, ChairID, m_conf_js.iGameType, iLaiziNum);
	//	return false;
	//}

	//addGuiCard(&tempHandCard, guiCardCountMap);

	return true;
}

// 保存癞子信息
void Table_chengdumj::save_video_laizi_info(_uint8 bCard)
{
	//proto::game::chongqingmj::AckLaiziCard stAck;
	//stAck.set_card(bCard);
	//stAck.set_laizicard(m_bLaiziCard);
	//stAck.set_laizipicard(m_bLaizipiCard);
	//
	//std::string str = stAck.SerializeAsString();
	//save_video_data(SERVER_HUBEI_LAIZIINFO, str);
}


// 判断是否是其他胡信息
int Table_chengdumj::EstimateOtherHu(_uint8 ChairID, HandCards& pHc, MJ_fan_buffer& oFan)
{
	//int oriFan = Calc_Fan(ChairID, oFan) * GetYingHuFan(ChairID);

	//_uint8 CardDataNum[MJ_TYPE_COMMON][10] = { { 0 }, { 0 }, { 0 }, { 0 }, { 0 } };
	//m_GameLogic.GetCardCount(&pHc, CardDataNum);

	//if (m_conf.m_bHu7dui)
	//{
	//	int iLongQiDui = 0;
	//	MJ_fan_buffer oTempFan;
	//	if (m_GameLogic.L_IsQiDui_03(&pHc, INVALID_MJ_CARD, CardDataNum, 0, iLongQiDui))
	//	{
	//		if (iLongQiDui > 0)
	//		{
	//			oTempFan.Add(m_GameLogic.IsQingYiSe(&pHc) ? MJ_FAN_TYPE_QINGLONGQIDUI : MJ_FAN_TYPE_LONGQIDUI);
	//		}
	//		else 
	//		{
	//			oTempFan.Add(m_GameLogic.IsQingYiSe(&pHc) ? MJ_FAN_TYPE_QINGQIDUI : MJ_FAN_TYPE_QIDUI);
	//		}

	//		int tempFan = Calc_Fan(ChairID, oTempFan) * 2;
	//		if (tempFan == oriFan)
	//		{
	//			m_UserOtherHu[ChairID].oFan = oTempFan;
	//			m_UserOtherHu[ChairID].bYinghu = true;
	//			return true;
	//		}
	//	}
	//}

	//MJ_win_pattern oTempPattern;
	//MJ_fan_buffer oTempFan;
	//if (m_GameLogic.FormatCards(&pHc, oTempPattern.kan, oTempPattern.jiang))
	//{
	//	if (m_GameLogic.IsQingYiSe(&pHc) && m_GameLogic.L_IsPengPengHu(&pHc, INVALID_MJ_CARD, CardDataNum, 0))
	//	{
	//		oTempFan.Add(MJ_FAN_TYPE_QINGPENG);
	//	}
	//	else if (m_GameLogic.IsQingYiSe(&pHc))
	//	{
	//		oTempFan.Add(MJ_FAN_TYPE_QINGYISE);
	//	}
	//	else if (m_GameLogic.L_IsPengPengHu(&pHc, INVALID_MJ_CARD, CardDataNum, 0))
	//	{
	//		oTempFan.Add(MJ_FAN_TYPE_PENGPENGHU);
	//	}
	//	else
	//	{
	//		oTempFan.Add(MJ_FAN_TYPE_PINGHU);
	//	}
	//	int tempFan = Calc_Fan(ChairID, oTempFan) * 2;
	//	if (tempFan == oriFan)
	//	{
	//		m_UserOtherHu[ChairID].oFan = oTempFan;
	//		m_UserOtherHu[ChairID].bYinghu = true;
	//		return true;
	//	}
	//}

	return false;
}

// 成都麻将龙七对
bool Table_chengdumj::IsShuangLongQiDui(_uint8 ChairID, HandCards& pHc, MJ_fan_buffer& oFan)
{
	//log_debug("%s tid:%d ChairID:%d \n", __FUNCTION__, tid, ChairID);

	int nGouNum = 0;

	if (m_GameLogic.Is7Dui(&pHc))
	{
		HandCards m_TempHandCards = pHc;
		_uint8 last = INVALID_MJ_CARD;
		_uint8 CardDataNum[MJ_TYPE_COMMON][10] = { { 0 }, { 0 }, { 0 }, { 0 }, { 0 } };
		m_GameLogic.GetCardCount(&m_TempHandCards, CardDataNum);
		for (int i = 0; i < m_TempHandCards.ChangeableCardsLen; )
		{
			if (m_TempHandCards.ChangeableCards[i] == last) continue;
			_uint8 m = CardType(m_TempHandCards.ChangeableCards[i]);
			_uint8 n = CardNum(m_TempHandCards.ChangeableCards[i]);
			if (CardDataNum[m][n] == 4)
			{
				nGouNum ++ ;
			}
			i += CardDataNum[m][n];
		}
	}

	if (nGouNum > 1)
	{
		return true;
	}

	return false;
}


// 成都麻将龙七对
bool Table_chengdumj::IsLongQiDui(_uint8 ChairID, HandCards& pHc, MJ_fan_buffer& oFan)
{
	//log_debug("%s tid:%d ChairID:%d \n", __FUNCTION__, tid, ChairID);

	if (m_GameLogic.Is7Dui(&pHc))
	{
		HandCards m_TempHandCards = pHc;
		_uint8 last = INVALID_MJ_CARD;
		_uint8 CardDataNum[MJ_TYPE_COMMON][10] = { { 0 }, { 0 }, { 0 }, { 0 }, { 0 } };
		m_GameLogic.GetCardCount(&m_TempHandCards, CardDataNum);
		for (int i = 0; i < m_TempHandCards.ChangeableCardsLen; i++)
		{
			if (m_TempHandCards.ChangeableCards[i] == last) continue;
			_uint8 m = CardType(m_TempHandCards.ChangeableCards[i]);
			_uint8 n = CardNum(m_TempHandCards.ChangeableCards[i]);
			if (CardDataNum[m][n] == 4)
			{
				return true;
			}
		}
	}
	return false;
}

// 成都麻将七对
bool Table_chengdumj::IsAnQiDui(_uint8 ChairID, HandCards& pHc, MJ_fan_buffer& oFan)
{
	//log_debug("%s tid:%d ChairID:%d \n", __FUNCTION__, tid, ChairID);

	if (IsLongQiDui(ChairID, pHc, oFan))
	{
		return false;
	}

	if (m_GameLogic.Is7Dui(&pHc))
	{
		return true;
	}
	return false;
}

// 七对
bool Table_chengdumj::IsQiDui(_uint8 ChairID, HandCards& pHc, MJ_fan_buffer& oFan)
{
	//log_debug("%s tid:%d ChairID:%d \n", __FUNCTION__, tid, ChairID);

	if (IsLongQiDui(ChairID, pHc, oFan))
	{
		// 将七对一定是龙七对、
		if (IsShuangLongQiDui(ChairID, pHc, oFan))
		{
			oFan.Add(m_GameLogic.IsQingYiSe(&pHc) ? MJ_FAN_TYPE_QINGSHUANGLONGQIDUI : MJ_FAN_TYPE_SHUANGLONGQIDUI);
		}
		else if (IsJiangDui(ChairID, pHc, oFan))
		{
			// 将对
			if (true == m_conf_js.bYaoJiuJiangDui)
			{
				oFan.Add(MJ_FAN_TYPE_JIANGQIDUI);
			}
			else
			{
				oFan.Add(MJ_FAN_TYPE_LONGQIDUI);
			}
		}
		else
		{
			// 青龙七对，龙七对
			oFan.Add(m_GameLogic.IsQingYiSe(&pHc) ? MJ_FAN_TYPE_QINGLONGQIDUI : MJ_FAN_TYPE_LONGQIDUI);
		}
		return true;
	}
	else
	{
		if (IsAnQiDui(ChairID, pHc, oFan))
		{
			// 清七对，七对
			oFan.Add(m_GameLogic.IsQingYiSe(&pHc) ? MJ_FAN_TYPE_QINGQIDUI : MJ_FAN_TYPE_QIDUI);
			return true;
		}
	}
	return false;
}

// 成都麻将七对
bool Table_chengdumj::IsPingHuLeiXing(_uint8 ChairID, HandCards& pHc, MJ_fan_buffer& oFan)
{
	//log_info("%s tid:%d ChairID:%d \n", __FUNCTION__, tid, ChairID);

	MJ_win_pattern oTempPattern;
	if (m_GameLogic.FormatCards(&pHc, oTempPattern.kan, oTempPattern.jiang))
	{
		//if (IsJinGouHu(ChairID, pHc, oFan))
		//{
		//	if (IsJiangDui(ChairID, pHc, oFan))
		//	{
		//		// 将对
		//		if (true == m_conf_js.bYaoJiuJiangDui)
		//		{
		//			oFan.Add(MJ_FAN_TYPE_JIANGDUIDUIJINGOUHU);
		//		}
		//		else
		//		{
		//			oFan.Add(MJ_FAN_TYPE_JINGOUHU);
		//		}
		//	}
		//	else
		//	{
		//		oFan.Add(m_GameLogic.IsQingYiSe(&pHc) ? MJ_FAN_TYPE_QINGYISEJINGOUHU : MJ_FAN_TYPE_JINGOUHU);
		//	}
		//}
		//else if (IsDuiDuiHu(ChairID, pHc, oFan))
		if (IsDuiDuiHu(ChairID, pHc, oFan))
		{
			if (IsJiangDui(ChairID, pHc, oFan))
			{
				// 将对
				if (true == m_conf_js.bYaoJiuJiangDui)
				{
					oFan.Add(MJ_FAN_TYPE_JIANGDUI);
				}
				else
				{
					oFan.Add(MJ_FAN_TYPE_PENGPENGHU);
				}
			}
			else
			{
				oFan.Add(m_GameLogic.IsQingYiSe(&pHc) ? MJ_FAN_TYPE_QINGDUI : MJ_FAN_TYPE_PENGPENGHU);
			}
		}
		else
		{
			// log_info("%s HandCards[%s]\n", __FUNCTION__, CatHandCard(pHc, ChairID));

			if (IsQuanYaoJiu(ChairID, pHc, oFan))
			{
				if (true == m_conf_js.bYaoJiuJiangDui)
				{
					oFan.Add(MJ_FAN_TYPE_QUANYAOJIU);
				}
				else
				{
					oFan.Add(MJ_FAN_TYPE_PINGHU);
				}
			}
			else
			{
				oFan.Add(m_GameLogic.IsQingYiSe(&pHc) ? MJ_FAN_TYPE_QINGYISE : MJ_FAN_TYPE_PINGHU);
			}
			// log_info("%s HandCards[%s]\n", __FUNCTION__, CatHandCard(pHc, ChairID));
		}
		return true;
	}
	return false;
}

// 对对胡
bool Table_chengdumj::IsDuiDuiHu(_uint8 ChairID, HandCards& pHc, MJ_fan_buffer& oFan)
{
	//log_debug("%s tid:%d ChairID:%d \n", __FUNCTION__, tid, ChairID);
	HandCards m_TempHandCards = pHc;
	bool bIsDuiDuiHu = false;

	_uint8 CardDataNum[MJ_TYPE_COMMON][10] = { { 0 }, { 0 }, { 0 }, { 0 }, { 0 } };
	m_GameLogic.GetCardCount(&pHc, CardDataNum);

	if (m_GameLogic.L_IsPengPengHu(&m_TempHandCards, INVALID_MJ_CARD, CardDataNum, 0))
	{
		bIsDuiDuiHu = true;
	}

	return bIsDuiDuiHu;
}


// 将对（成都将七对）
bool Table_chengdumj::IsJiangDui(_uint8 ChairID, HandCards& pHc, MJ_fan_buffer& oFan)
{
	//log_debug("%s tid:%d ChairID:%d \n", __FUNCTION__, tid, ChairID);

	_uint8 last = INVALID_MJ_CARD;
	for (int i = 0; i < pHc.ChangeableCardsLen; i++)
	{
		if (pHc.ChangeableCards[i] == last) continue;
		_uint8 nCardNum = CardNum(pHc.ChangeableCards[i]);
		if (nCardNum == 2 || nCardNum == 5 || nCardNum == 8)
		{
			// 必须是2 5 8
		}
		else
		{
			return false;
		}
	}

	for (int i = 0; i < pHc.FixedCardsLen; i++)
	{
		if (pHc.FixedCards[i].CardData == last) continue;
		if ((pHc.FixedCards[i].state == TYPE_PENG) || (pHc.FixedCards[i].state & TYPE_GANG))
		{
			_uint8 nCardNum = CardNum(pHc.FixedCards[i].CardData);
			if (nCardNum == 2 || nCardNum == 5 || nCardNum == 8)
			{
				// 必须是2 5 8
			}
			else
			{
				return false;
			}
		}
	}

	return true;
}

// 全妖九
bool Table_chengdumj::IsQuanYaoJiu(_uint8 ChairID, HandCards& pHc, MJ_fan_buffer& oFan)
{
	//log_debug("%s tid:%d ChairID:%d \n", __FUNCTION__, tid, ChairID);

	HandCards m_TempHandCards = pHc;
	
	MJ_win_pattern pattern;

	_uint8 CardDataNum[MJ_TYPE_COMMON][10] = { { 0 }, { 0 }, { 0 }, { 0 }, { 0 } };
	m_GameLogic.GetCardCount(&m_TempHandCards, CardDataNum);

	if (FormatCards(&m_TempHandCards, pattern))
	{
		if (!(0x01 == pattern.jiang || 0x09 == pattern.jiang ||
			  0x11 == pattern.jiang || 0x19 == pattern.jiang ||
			  0x21 == pattern.jiang || 0x29 == pattern.jiang))
		{
			return false;
		}

		for (int i = 0; i < 4; i++)
		{
			bool bIsHaveYaoJiu = false;

			if (INVALID_MJ_CARD == pattern.kan[i][0])
			{
				bIsHaveYaoJiu = true;
				continue;
			}

			if (1 == CardNum(pattern.kan[i][0]) || 9 == CardNum(pattern.kan[i][0]))
			{
				// 刻子
				if (CardNum(pattern.kan[i][0]) == CardNum(pattern.kan[i][1]))
				{
					if (1 == CardNum(pattern.kan[i][0]))
					{
						// 看看能不能找到 2 3刻子
						if (3 == CardDataNum[CardType(pattern.kan[i][0])][1] && 
							3 == CardDataNum[CardType(pattern.kan[i][0])][2] && 
							3 == CardDataNum[CardType(pattern.kan[i][0])][3] )
						{
							HandCards m_TempHandCards_1 = m_TempHandCards;
							MJ_fan_buffer oFan1;
							m_GameLogic.RemoveCard(&m_TempHandCards_1, CardValue(CardType(pattern.kan[i][0]), 1), 3);
							m_GameLogic.RemoveCard(&m_TempHandCards_1, CardValue(CardType(pattern.kan[i][0]), 2), 3);
							m_GameLogic.RemoveCard(&m_TempHandCards_1, CardValue(CardType(pattern.kan[i][0]), 3), 3);
							if (IsQuanYaoJiu(ChairID, m_TempHandCards_1, oFan1))
							{
								return true;
							}
						}
					}

					if (9 == CardNum(pattern.kan[i][0]))
					{
						// 看看能不能找到 2 3刻子
						if (3 == CardDataNum[CardType(pattern.kan[i][0])][7] && 
							3 == CardDataNum[CardType(pattern.kan[i][0])][8] && 
							3 == CardDataNum[CardType(pattern.kan[i][0])][9] )
						{
							HandCards m_TempHandCards_1 = m_TempHandCards;
							MJ_fan_buffer oFan1;
							m_GameLogic.RemoveCard(&m_TempHandCards_1, CardValue(CardType(pattern.kan[i][0]), 7), 3);
							m_GameLogic.RemoveCard(&m_TempHandCards_1, CardValue(CardType(pattern.kan[i][0]), 8), 3);
							m_GameLogic.RemoveCard(&m_TempHandCards_1, CardValue(CardType(pattern.kan[i][0]), 9), 3);
							if (IsQuanYaoJiu(ChairID, m_TempHandCards_1, oFan1))
							{
								return true;
							}
						}
					}
				}
			}
		}

		for (int i = 0; i < 4; i++)
		{
			bool bIsHaveYaoJiu = false;

			if (INVALID_MJ_CARD == pattern.kan[i][0])
			{
				bIsHaveYaoJiu = true;
				continue;
			}

			for (int j = 0; j < 4; j++)
			{
				if (INVALID_MJ_CARD == pattern.kan[i][0])
				{
					bIsHaveYaoJiu = true;
					continue;
				}
				if (1 == CardNum(pattern.kan[i][j]) || 9 == CardNum(pattern.kan[i][j]))
				{
					bIsHaveYaoJiu = true;
				}
			}

			if (false == bIsHaveYaoJiu)
			{
				return false;
			}
		}
		return true;
	}
	return false;
}

// 加番
bool Table_chengdumj::IsJiaFan(_uint8 ChairID, HandCards& pHc, MJ_fan_buffer& oFan)
{
	//log_debug("%s tid:%d ChairID:%d \n", __FUNCTION__, tid, ChairID);

	bool bIsHaveJiaFan = false;

	if ((ChairID != m_bBanker) && (ChairID != m_bCurChairID) && (2 > m_bCanDiHu[ChairID]))
	{
		// oFan.Add(MJ_FAN_TYPE_DIHU);
		if (false == oFan.Find(MJ_FAN_TYPE_DIHU))
		{
			oFan.Add(MJ_FAN_TYPE_DIHU);
		}
		bIsHaveJiaFan = true;
	}

	if (IsJinGouHu(ChairID, pHc, oFan))
	{
		oFan.Add(MJ_FAN_TYPE_JINGOUHU);
		bIsHaveJiaFan = true;
	}

	if (IsMenQing(ChairID, pHc, oFan))
	{
		if (true == m_conf_js.bMenQingZhongZhang)
		{
			oFan.Add(MJ_FAN_TYPE_MENQING);
			bIsHaveJiaFan = true;
		}
	}

	if (IsZhongZhang(ChairID, pHc, oFan))
	{
		if (true == m_conf_js.bMenQingZhongZhang)
		{
			oFan.Add(MJ_FAN_TYPE_ZHONGZHANG);
			bIsHaveJiaFan = true;
		}
	}

	if (IsYiTiaoLong(ChairID, pHc, oFan))
	{
		if (true == m_conf_js.bYiTiaoLong)
		{
			oFan.Add(MJ_FAN_TYPE_YITIAOLONG);
			bIsHaveJiaFan = true;
		}
	}

	if (IsJiaXinWu(ChairID, pHc, oFan))
	{
		if (true == m_conf_js.bJiaXinWu)
		{
			oFan.Add(MJ_FAN_TYPE_JIAXINWU);
			bIsHaveJiaFan = true;
		}
	}

	m_nGouPaiNum[ChairID] = 0;
	m_nGouPaiNum[ChairID] = IsGouGeng(ChairID, pHc, oFan);
	if (m_nGouPaiNum[ChairID] > 0)
	{
		oFan.Add(MJ_FAN_TYPE_GOUPAI);
		bIsHaveJiaFan = true;
	}

	return bIsHaveJiaFan;
}

// 金钩胡
bool Table_chengdumj::IsJinGouHu(_uint8 ChairID, HandCards& pHc, MJ_fan_buffer& oFan)
{
	//log_debug("%s tid:%d ChairID:%d \n", __FUNCTION__, tid, ChairID);

	if (pHc.ChangeableCardsLen == 2)
	{
		return true;
	}
	return false;
}

// 一条龙
bool Table_chengdumj::IsYiTiaoLong(_uint8 ChairID, HandCards& pHc, MJ_fan_buffer& oFan)
{
	//log_debug("%s tid:%d ChairID:%d \n", __FUNCTION__, tid, ChairID);

	_uint8 last = INVALID_MJ_CARD;
	HandCards m_TempHandCards = pHc;
	_uint8 CardDataNum[MJ_TYPE_COMMON][10] = { { 0 }, { 0 }, { 0 }, { 0 }, { 0 } };
	m_GameLogic.GetCardCount(&m_TempHandCards, CardDataNum);

	for (int i = 0; i < m_TempHandCards.ChangeableCardsLen; )
	{
		if (m_TempHandCards.ChangeableCards[i] == last) continue;
		_uint8 m = CardType(m_TempHandCards.ChangeableCards[i]);
		_uint8 n = CardNum(m_TempHandCards.ChangeableCards[i]);
		if ((n == 1)&&(CardDataNum[m][n] >= 1))
		{
			bool bAllHave = true;
			for (int j = 1; j < 10; j++)
			{
				if (CardDataNum[m][j] < 1)
				{
					bAllHave = false;
				}
			}
            // 判断去掉一条龙之后是不是都是对子和刻子
			if (true == bAllHave)
			{
				for (int nTemp = 1; nTemp < 10; nTemp++)
				{
					if (CardDataNum[m][nTemp] > 0)
					{
						if (1 != CardDataNum[m][nTemp])
						{
							if (CardDataNum[m][nTemp] < 3)
							{
								return false;
							}
						}
					}
				}
				return true;
			}
		}
		i += CardDataNum[m][n];
	}
	return false;
}

// 金钩胡
bool Table_chengdumj::IsMenQing(_uint8 ChairID, HandCards& pHc, MJ_fan_buffer& oFan)
{
	//log_debug("%s tid:%d ChairID:%d \n", __FUNCTION__, tid, ChairID);

	for (int i = 0; i < pHc.FixedCardsLen; i++)
	{
		// 不能有直杠，弯杠，碰
	    // 只有暗杠算门清
		if (pHc.FixedCards[i].state & TYPE_ZHIGANG || pHc.FixedCards[i].state & TYPE_WANGANG || pHc.FixedCards[i].state & TYPE_PENG)
		{
			return false;
		}
	}
	return true;
}

// 中张
bool Table_chengdumj::IsZhongZhang(_uint8 ChairID, HandCards& pHc, MJ_fan_buffer& oFan)
{
	//log_debug("%s tid:%d ChairID:%d \n", __FUNCTION__, tid, ChairID);

	// 胡牌时手中没有1和9
	HandCards m_TempHandCards = pHc;
	_uint8 last = INVALID_MJ_CARD;
	for (int i = 0; i < m_TempHandCards.ChangeableCardsLen; i++)
	{
		if (m_TempHandCards.ChangeableCards[i] == last) continue;
		if (1 == CardNum(m_TempHandCards.ChangeableCards[i]))
		{
			return false;
		}
		if (9 == CardNum(m_TempHandCards.ChangeableCards[i]))
		{
			return false;
		}
	}

	for (int i = 0; i < m_TempHandCards.FixedCardsLen; i++)
	{
		if (1 == CardNum(m_TempHandCards.FixedCards[i].CardData))
		{
			return false;
		}
		if (9 == CardNum(m_TempHandCards.FixedCards[i].CardData))
		{
			return false;
		}
	}

	return true;
}


// 夹心五
bool Table_chengdumj::IsJiaXinWu(_uint8 ChairID, HandCards& pHc, MJ_fan_buffer& oFan)
{
	//log_debug("%s tid:%d ChairID:%d \n", __FUNCTION__, tid, ChairID);

	// 胡牌时手中没有1和9
	HandCards m_TempHandCards = pHc;
	int nCardType = INVALID_MJ_CARD;
	int nCardNum = INVALID_MJ_CARD;
	_uint8 CardDataNum[MJ_TYPE_COMMON][10] = { { 0 }, { 0 }, { 0 }, { 0 }, { 0 } };
	nCardNum = CardNum(m_bCurCard);
	nCardType = CardType(m_bCurCard);

	if (nCardNum != 5)
	{
		return false;
	}

	m_GameLogic.GetCardCount(&m_TempHandCards, CardDataNum);
	if ((CardDataNum[nCardType][4] > 0) && (CardDataNum[nCardType][6] > 0))
	{
		bool bPingHu = false;
		MJ_win_pattern pattern;
		m_GameLogic.RemoveCard(&m_TempHandCards, CardValue(nCardType, 4), 1);
		m_GameLogic.RemoveCard(&m_TempHandCards, CardValue(nCardType, 5), 1);
		m_GameLogic.RemoveCard(&m_TempHandCards, CardValue(nCardType, 6), 1);
		if (FormatCards(&m_TempHandCards, pattern))
		{
			bPingHu = true;
		}
		m_GameLogic.AddCard(&m_TempHandCards, CardValue(nCardType, 4));
		m_GameLogic.AddCard(&m_TempHandCards, CardValue(nCardType, 5));
		m_GameLogic.AddCard(&m_TempHandCards, CardValue(nCardType, 6));

		if (true == bPingHu)
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	return false;
}

// 金钩胡
int Table_chengdumj::IsGouGeng(_uint8 ChairID, HandCards& pHc, MJ_fan_buffer& oFan)
{
	//log_debug("%s tid:%d ChairID:%d \n", __FUNCTION__, tid, ChairID);

	int nGenNum = 0;
	_uint8 last = INVALID_MJ_CARD;
	HandCards m_TempHandCards = pHc;
	_uint8 CardDataNum[MJ_TYPE_COMMON][10] = { { 0 }, { 0 }, { 0 }, { 0 }, { 0 } };
	m_GameLogic.GetCardCount(&m_TempHandCards, CardDataNum);

	for (int i = 0; i < m_TempHandCards.ChangeableCardsLen; )
	{
		if (m_TempHandCards.ChangeableCards[i] == last) continue;
		_uint8 m = CardType(m_TempHandCards.ChangeableCards[i]);
		_uint8 n = CardNum(m_TempHandCards.ChangeableCards[i]);
		if (CardDataNum[m][n] == 4)
		{
			nGenNum ++;
		}
		i += CardDataNum[m][n];
	}

	for (int i = 0; i < pHc.FixedCardsLen; i++)
	{
		if (TYPE_GANG & pHc.FixedCards[i].state)
		{
			nGenNum ++;
		}
	}

	for (int i = 0; i < pHc.FixedCardsLen; i++)
	{
		if (TYPE_PENG & pHc.FixedCards[i].state)
		{
			for (int j = 0; j < m_TempHandCards.ChangeableCardsLen;/* j++*/)
			{
				if (m_TempHandCards.ChangeableCards[j] == last) continue;
				_uint8 m = CardType(m_TempHandCards.ChangeableCards[j]);
				_uint8 n = CardNum(m_TempHandCards.ChangeableCards[j]);
				if (pHc.FixedCards[i].CardData == m_TempHandCards.ChangeableCards[j])
				{
					nGenNum ++;
				}
				j += CardDataNum[m][n];
			}
		}
	}
	// 双龙七对去掉两个勾
	if (IsShuangLongQiDui(ChairID, pHc, oFan))
	{
		if (nGenNum > 1)
		{
			nGenNum = nGenNum - 2;
		}
	}
	else if (IsLongQiDui(ChairID, pHc, oFan))
	{
		if (nGenNum > 0)
		{
			nGenNum = nGenNum - 1;
		}	
	}

	return (nGenNum);
}


void Table_chengdumj::CheckDaJiao(proto::game::chengdumj::AckCDGameEnd& stHuPaiInfo)
{
	//log_info("%s tid:%d ", __FUNCTION__, tid);

	bool nTingPaiUser[GAME_PLAYER] = {0};
	int nTingPaiUserNum = 0;
	bool nNoTingPaiUser[GAME_PLAYER] = {0};
	int nNoTingPaiUserNum = 0;
	bool nArrMaxFan[GAME_PLAYER] = {0};
	int nMaxTuiFan = 0;
	int nMaxFan = 0;
	_uint8 nDaJiaoCard[GAME_PLAYER];

	for (int i = 0; i < GAME_PLAYER; i++)
	{
		nDaJiaoCard[i] = INVALID_MJ_CARD;
	}

	for (int i = 0; i < m_GAME_PLAYER; i++)
	{
		// 没有胡的判断有没有没有听的
		if (!m_bIsHu[i])
		{
			HandCards stTempHandCards = m_HandCard[i];
			tagListenInfos stListenInfo[14];
			_uint8 iListenCardNum = 0;
			GetDaJiaoInfo(i, stListenInfo, iListenCardNum);
			log_info("%s Check_Listening_Thirteen%d\n", __FUNCTION__, iListenCardNum);
			nMaxFan = 0;
			if (iListenCardNum)
			{
				nTingPaiUserNum++;
				if (nTingPaiUserNum)
				{
					nTingPaiUser[i] = true;
				}
				for (int j = 0; j < iListenCardNum; j ++)
				{
					for (int k = 0; k < stListenInfo[j].bHuLen; k++)
					{
						if (stListenInfo[j].HuInfo[k].bFanShu >= nMaxFan)
						{
							nDaJiaoCard[i] = stListenInfo[j].HuInfo[k].bCard;
							nMaxFan = stListenInfo[j].HuInfo[k].bFanShu;

							if (5 == CardNum(nDaJiaoCard[i]))
							{
								int nCardType = INVALID_MJ_CARD;
								int nCardNum = INVALID_MJ_CARD;
								_uint8 CardDataNum[MJ_TYPE_COMMON][10] = { { 0 }, { 0 }, { 0 }, { 0 }, { 0 } };
								nCardNum = CardNum(nDaJiaoCard[i]);
								nCardType = CardType(nDaJiaoCard[i]);

								m_GameLogic.GetCardCount(&stTempHandCards, CardDataNum);
								if ((CardDataNum[nCardType][4] > 0) && (CardDataNum[nCardType][6] > 0))
								{
									bool bPingHu = false;
									MJ_win_pattern pattern;
									m_GameLogic.RemoveCard(&stTempHandCards, CardValue(nCardType, 4), 1);
									m_GameLogic.RemoveCard(&stTempHandCards, CardValue(nCardType, 5), 1);
									m_GameLogic.RemoveCard(&stTempHandCards, CardValue(nCardType, 6), 1);
									if (FormatCards(&stTempHandCards, pattern))
									{
										bPingHu = true;
									}
									m_GameLogic.AddCard(&stTempHandCards, CardValue(nCardType, 4));
									m_GameLogic.AddCard(&stTempHandCards, CardValue(nCardType, 5));
									m_GameLogic.AddCard(&stTempHandCards, CardValue(nCardType, 6));

									if (true == bPingHu)
									{
										if (m_conf_js.bJiaXinWu)
										{
											nMaxFan += 1;
										}
									}
								}
							}
							log_info("%s tid:%d uid:%d ChairID:%d nDaJiaoCard%d nMaxFan%d\n", __FUNCTION__, tid, GetPlayerUid(i), i, nDaJiaoCard[i], nMaxFan);
						}
					}
				}

				int nTempMaxFan = 2;

				if (0 == nMaxFan)
				{
					nTempMaxFan = 1;
				}
				else
				{
					if (nMaxFan > m_conf_js.iFengDingFan)
					{
						nMaxFan = m_conf_js.iFengDingFan;
					}

					for (int k = 1; k < nMaxFan; k++)
					{
						nTempMaxFan = 2*nTempMaxFan;
					}
				}
				m_nDaJiaoFan[i] = nMaxFan;
				nArrMaxFan[i] = nTempMaxFan;
				m_nDaJiao[i] = nTempMaxFan * m_nJiChuDiFen;
			}
			else
			{
				nNoTingPaiUserNum++;
				nNoTingPaiUser[i] = true;
			}
		}
	}	


	if (nNoTingPaiUserNum < 1)
	{
		for (int i = 0; i < m_GAME_PLAYER; i++)
		{
			m_nDaJiao[i] = 0;
		}
		return;
	}

	//log_info("%s __m_nDaJiao1_%d__m_nDaJiao2_%d__m_nDaJiao3_%d__m_nDaJiao4_%d__nMaxTuiFan:%d\n", __FUNCTION__,  m_nDaJiao[0], 
		//m_nDaJiao[1],m_nDaJiao[2],m_nDaJiao[3],nMaxTuiFan);

	nMaxTuiFan = 0;
	// 计算大叫玩家退倍,总退分
	for (int i = 0; i < m_GAME_PLAYER; i++)
	{
		if (true == nTingPaiUser[i])
		{
			nMaxTuiFan += m_nDaJiao[i];
		}
	}

	// 大叫玩家减分
	for (int i = 0; i < m_GAME_PLAYER; i++)
	{
		if (true == nNoTingPaiUser[i])
		{
			m_nDaJiao[i] -= nMaxTuiFan;
			if (nMaxTuiFan != 0)
			{
				m_stRecodeInfo[i].iChaJiaoTimes++;
			}
		}
	}


	for (int i = 0; i < m_GAME_PLAYER; i++)
	{
		if (true == nTingPaiUser[i])
		{
			int nTempDaJiao = m_nDaJiao[i];
			m_nDaJiao[i] = 0;
			for (int j = 0; j < m_GAME_PLAYER; j++)
			{
				if (true == nNoTingPaiUser[j])
				{
					m_nDaJiao[i] += nTempDaJiao;
				}
			}
		}
	}


	//log_info("%s tid:%d uid:%d ChairID:%d __m_nDaJiao1_%d__m_nDaJiao2_%d__m_nDaJiao3_%d__m_nDaJiao4_%d__nMaxTuiFan:%d\n", __FUNCTION__,  tid, GetPlayerUid(0), 0, m_nDaJiao[0], 
		//m_nDaJiao[1],m_nDaJiao[2],m_nDaJiao[3],nMaxTuiFan);


	Proto_GameEnd_DaJiaoInfo(nDaJiaoCard, m_nDaJiao, stHuPaiInfo);
}

void Table_chengdumj::CheckTuiGang(proto::game::chengdumj::AckCDGameEnd& stHuPaiInfo)
{
	//log_info("%s tid:%d ", __FUNCTION__, tid);

	int nTingUserNum = 0;

	int nNoTingUserNum = 0;

	for (int i = 0; i < m_GAME_PLAYER; i++)
	{
		// 没有胡的判断有没有没有听的
		if (!m_bIsHu[i])
		{
			HandCards stTempHandCards = m_HandCard[i];
			tagListenInfos stListenInfo[14];
			_uint8 iListenCardNum = 0;
			GetDaJiaoInfo(i, stListenInfo, iListenCardNum);
			if (0 == iListenCardNum)
			{
				nNoTingUserNum ++;
				for (int j = 0; j < 5; j++)
				{
					if (false == m_GangFen[i][j].bIsNull)
					{
						if (INVALID_CHAIRID != m_GangFen[i][j].uid1)
						{
							m_nTuiGang[m_GangFen[i][j].uid1] += m_GangFen[i][j].score1;
						}
						if (INVALID_CHAIRID != m_GangFen[i][j].uid2)
						{
							m_nTuiGang[m_GangFen[i][j].uid2] += m_GangFen[i][j].score2;
						}
						if (INVALID_CHAIRID != m_GangFen[i][j].uid3)
						{
							m_nTuiGang[m_GangFen[i][j].uid3] += m_GangFen[i][j].score3;
						}
						m_nTuiGang[i] -= (m_GangFen[i][j].score1 + m_GangFen[i][j].score2 + m_GangFen[i][j].score3);

						m_GangFen[i][j].bIsNull = true;
					}
				}
			}
			else
			{
				nTingUserNum ++;
			}
		}
	}

	if (nNoTingUserNum < 1)
	{
		for (int i = 0; i < m_GAME_PLAYER; i++)
		{
			m_nTuiGang[i] = 0;
		}
		return;
	}

	log_info("%s __m_nTuiGang1_%d__m_nTuiGang2_%d__m_nTuiGang3_%d__m_nTuiGang4_%d\n", __FUNCTION__,  m_nTuiGang[0], 
		m_nTuiGang[1],m_nTuiGang[2],m_nTuiGang[3]);

	for (int i = 0; i < m_GAME_PLAYER; i++)
	{
		if (0 != m_nTuiGang[i])
		{
			// Proto_GameEndInfo_TuiGang(i, m_nTuiGang[i], stGameend, false);
		}
	}

}

void Table_chengdumj::CheckDaJiaoTuiGang(proto::game::chengdumj::AckCDGameEnd& stHuPaiInfo)
{
	CheckDaJiao(stHuPaiInfo);
	CheckTuiGang(stHuPaiInfo);
}


void Table_chengdumj::UserOperateGang(_uint8 ChairID, int iOpType, _uint8 bCard)
{
	log_info("%s tid:%d ChairID:%d", __FUNCTION__, tid, ChairID);

	memset(m_bAddFanFlag, 0, sizeof(m_bAddFanFlag));
	memset(m_bUserOperate, 0, sizeof(m_bUserOperate));
	m_stQiHuInfo[ChairID].InitQiHuInfo();

	log_info("%s uid:%d ChairID:%d, iOpType:0x%x, bCard:0x%02x \n", __FUNCTION__, GetPlayerUid(ChairID), ChairID, iOpType, bCard);
	//直杠 2*底分，弯杠1*底分，暗杠2*底分*3
	int iGangScore[GAME_PLAYER] = {0};
	_uint8 bTempChairID = INVALID_CHAIRID;
	m_bAddFanFlag[ChairID] = true;

	if (iOpType == TYPE_ZHIGANG)
	{
		m_nZhiGangChairID = m_bCurChairID;
	}
	else
	{
		m_nZhiGangChairID = INVALID_CHAIRID;
	}

	switch (iOpType)
	{
	case TYPE_ANGANG:
		{
			m_stRecodeInfo[ChairID].iAnGangTimes++;
			for (_uint8 c = NextChair(ChairID); c != ChairID; c = NextChair(c))
			{
				if (true == m_bChuJu[c])
				{
					continue;
				}

				iGangScore[ChairID] += (m_BaseScore * m_conf.m_AnGangPoint) * m_nJiChuDiFen;
				iGangScore[c] -= (m_BaseScore * m_conf.m_AnGangPoint) * m_nJiChuDiFen;
			}
			m_stGang[ChairID].Gang.Add(tagGangScore(bCard, ChairID, TYPE_ANGANG));				
			bTempChairID = ChairID;
		}
		break;
	case TYPE_WANGANG:
		{	
			m_stRecodeInfo[ChairID].iMingGangTimes++;
			for (_uint8 c = NextChair(ChairID); c != ChairID; c = NextChair(c))
			{
				if (true == m_bChuJu[c])
				{
					continue;
				}

				iGangScore[ChairID] += (m_BaseScore * m_conf.m_WanGangPoint) * m_nJiChuDiFen;
				iGangScore[c] -= (m_BaseScore * m_conf.m_WanGangPoint) * m_nJiChuDiFen;					
			}
			m_stGang[ChairID].Gang.Add(tagGangScore(bCard, ChairID, TYPE_WANGANG));
			bTempChairID = ChairID;				
		}
		break;
	case TYPE_ZHIGANG:
		{
			m_stRecodeInfo[ChairID].iMingGangTimes++;
			for (_uint8 c = NextChair(ChairID); c != ChairID; c = NextChair(c))
			{

				if (true == m_bChuJu[c])
				{
					continue;
				}

				if (c == m_bOutCardChairID)
				{
					iGangScore[ChairID] +=  (m_BaseScore * m_conf.m_ZhiGangPoint) * m_nJiChuDiFen;
					iGangScore[m_bOutCardChairID] -= (m_BaseScore *  m_conf.m_ZhiGangPoint) * m_nJiChuDiFen;
				}
				else
				{
					if (true == m_conf_js.bGuaCa)
					{
						iGangScore[ChairID] += (m_BaseScore * m_conf.m_WanGangPoint) * m_nJiChuDiFen;
						iGangScore[c] -=  (m_BaseScore * m_conf.m_WanGangPoint) * m_nJiChuDiFen;
					}
				}	
			}

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


	if (m_bQiGang[ChairID][CardType(bCard)][CardNum(bCard)] == false)
	{
		Calc_Score(m_stGang, iGangScore);
	}
	else
	{
		for (int i = 0; i < GAME_PLAYER; i++)
		{
			iGangScore[i] = 0;
		}
	}

	// 记录杠牌分细节
	for (int j = 0; j < 5; j++)
	{
		if (m_GangFen[ChairID][j].bIsNull)
		{
			m_GangFen[ChairID][j].bIsNull = false;
			m_GangFen[ChairID][j].nGangType = iOpType;
			m_GangFen[ChairID][j].totalscore = iGangScore[ChairID];
			if (iOpType == TYPE_ZHIGANG)
			{
				m_GangFen[ChairID][j].nDianGangChairId = m_bCurChairID;
			}
			m_GangFen[ChairID][j].nGangPai = bCard;
			int n = 0;
			for (int i = 0; i < m_GAME_PLAYER; i++)
			{
				if ((i != ChairID) && (iGangScore[i] < 0))
				{
					n++;
					if (n == 1)
					{
						m_GangFen[ChairID][j].uid1 = i;
						m_GangFen[ChairID][j].score1 = -iGangScore[i];
					}
					if (n == 2)
					{
						m_GangFen[ChairID][j].uid2 = i;
						m_GangFen[ChairID][j].score2 = -iGangScore[i];
					}
					if (n == 3)
					{
						m_GangFen[ChairID][j].uid3 = i;
						m_GangFen[ChairID][j].score3 = -iGangScore[i];
					}
				}
			}
			break;
		}
	}	

	proto::game::AckBcOpResult stOpresult;

	Proto_Game_OperateGang(ChairID, bCard, iOpType, iGangScore, stOpresult);

	broadcast(SERVER_OPERATE_RESULT, stOpresult);

	m_bUserOperate[ChairID] = TYPE_NULL;

	SaveVideoData(SERVER_OPERATE_RESULT, stOpresult);
}

void Table_chengdumj::OnRecvUserOperate(_uint8 ChairID, int iOpType, int iCard)
{
	log_info("%s tid:%d uid:%d ChairID:%d iOpType:0x%x \n", __FUNCTION__, tid, GetPlayerUid(ChairID), ChairID, iOpType);

	_uint8 bCard = (_uint8)iCard;

	if (!CheckUserOperator(ChairID, iOpType, bCard))
	{
		return;
	}

	if ((m_bUserOperate[ChairID] & TYPE_HU) && (iOpType != TYPE_HU))
	{
		if (ChairID != m_bCurChairID)
		{
			m_stQiHuInfo[ChairID].bIsQiHu = true;
			if (m_stQiHuInfo[ChairID].nDangQianHuFan > m_stQiHuInfo[ChairID].nQiHuFan)
			{
				m_stQiHuInfo[ChairID].nQiHuFan = m_stQiHuInfo[ChairID].nDangQianHuFan;
			}
		}
	}
	// 及时雨
	if ((m_bUserOperate[ChairID] & TYPE_GANG)&&(iOpType == TYPE_PASS || (!(TYPE_GANG & iOpType))))
	{
		MJ_opinfo stGang;

		GetGangInfo(ChairID, stGang);

		if ((ChairID != m_bCurChairID) && (m_bUserOperate[ChairID] & TYPE_ZHIGANG))
		{
			_uint8 bTempCardCount[MJ_TYPE_COMMON][10];
			memset(bTempCardCount, 0, sizeof(bTempCardCount));

			for (int i = 0; i < m_HandCard[ChairID].ChangeableCardsLen; i++)
			{
				if (m_bCurCard == m_HandCard[ChairID].ChangeableCards[i])
				{
					_uint8 bType = CardType(m_HandCard[ChairID].ChangeableCards[i]);
					_uint8 bNum = CardNum(m_HandCard[ChairID].ChangeableCards[i]);

					bTempCardCount[bType][bNum]++;
					if (bTempCardCount[bType][bNum] == 3)
					{
						stGang.Add(OpState(m_HandCard[ChairID].ChangeableCards[i], TYPE_ZHIGANG));
					}
				}
			}
		}

		for (int i = 0; i < stGang.Size(); i++)
		{
			if ((CardType(stGang[i].cbCard) != m_UserDingQueSe[ChairID]) && (stGang[i].nState != TYPE_ANGANG))
			{
				m_bQiGang[ChairID][CardType(stGang[i].cbCard)][CardNum(stGang[i].cbCard)] = true;
				log_info("%s tid:%d uid:%d chairid:%d [%d]\n", __FUNCTION__, tid, GetPlayerUid(ChairID), ChairID, stGang[i].cbCard);
			}
		}
	}


	stop_operator_timer(ChairID);
	m_bOverTimeState[ChairID] = 0;
	//如果是多操作
	if (m_bMultiOpUserNum > 1)
	{
		m_trustee.get()->DoMultiOperate(ChairID, iOpType);
		Player* player = m_SeatPlayer[ChairID].player;
		if (player != NULL)
			player->m_UseTime += elapse_operator_timer(ChairID);
		DoMultiOperate(ChairID, iOpType, bCard);
	}
	else 
	{   
		if (m_bRobWanGang)  //抢杠胡操作优先级
		{
			// 取消直杠玩家信息
			m_nZhiGangChairID = INVALID_CHAIRID;
			RobGangPriority(ChairID, bCard, iOpType);
			return ;
		}
		Player* player = m_SeatPlayer[ChairID].player;
		if (player != NULL)
			player->m_UseTime += elapse_operator_timer(ChairID);
		m_trustee.get()->DoSimpleOperate(ChairID, iOpType);
		DoSimpleOperate(ChairID, iOpType, bCard);
		//判断是下一玩家摸牌，或者当前玩家出牌		
	}
}



void Table_chengdumj::UserOperateHu(_uint8 ChairID, _uint8 bHuCard)
{

	log_info("%s tid:%d uid:%d ChairID:%d bHuCard:0x%02x Hu_FanNum:%d \n", __FUNCTION__, tid, GetPlayerUid(ChairID), ChairID, bHuCard, m_HuFanNums[ChairID]);
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
		log_error("hu error, hu chairid:%d, m_bUserOperate[%d]:%x \n", ChairID, ChairID, m_bUserOperate[ChairID]);
		return ;
	}	
	// 
	m_stQiHuInfo[ChairID].InitQiHuInfo();

	if (nOprOpType2 == 2)
	{
		m_stUserOutCard[m_bCurChairID].Add(bCard);
		nOprOpType2 = 0;
	}
	
	if (ChairID != m_bCurChairID)
	{
		m_nHuCardShunXu[m_bCurChairID] = m_stUserOutCard[m_bCurChairID].Size();

		m_stUserOutCard[m_bCurChairID].Remove(bCard);

		for (int i = 0; i < m_GAME_PLAYER; i++)
		{
			if (false == m_stDianPaoInfo[i].bIsUsed)
			{
				m_stDianPaoInfo[i].bIsUsed = true;
				m_stDianPaoInfo[i].nDianPaoShunXu = m_stUserOutCard[m_bCurChairID].Size();
				m_stDianPaoInfo[i].nDianPaoUser = m_bCurChairID;
				m_stDianPaoInfo[i].nDianPaoCard = bCard;
				break;
			}
		}
		m_nPaoHuCard[ChairID] = bCard;
		m_nHuType[ChairID] = 2;

		m_ShuFenSeatID[ChairID].push_back(m_bCurChairID);
	}
	else
	{
		m_nHuType[ChairID] = 1;
		m_nZiMoHuCard[ChairID] = bCard;

		for (int i = 0; i < m_GAME_PLAYER; i++)
		{
			if (m_bIsHu[i] || ChairID == i)
			{
				continue;
			}
			m_ShuFenSeatID[ChairID].push_back(i);
		}
	}

	m_nHuCard[ChairID] = bCard;

	m_nHuPaiChairid = ChairID;
	m_bIsHu[ChairID] = true;	
	int userscore[GAME_PLAYER] = { 0 };
	Calc_Hu(ChairID, bCard, userscore);	

	Calc_Score(m_stHuScore, userscore);
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

	int nscore[GAME_PLAYER] = {0};
	save_video_operate(TYPE_HU, ChairID, stHu.pashuchairid(), bCard, nscore);
	save_video_hu(ChairID, stHu.pashuchairid(), userscore);
}


int Table_chengdumj::Calc_Hu(_uint8 ChairID, _uint8 bCard, int Score[GAME_PLAYER])
{

	assert(Score[0] == 0);
	assert(Score[1] == 0);
	assert(Score[2] == 0);
	assert(Score[3] == 0);

	m_oFan[ChairID].Clear();	
	m_HuFanNums[ChairID] = 0;

	MJ_major major;
	if (m_bRobHu)
	{
		int iOpType = OutCardEstimate(ChairID, bCard, major, m_HuFanNums[ChairID]);
		if (iOpType & TYPE_HU)
		{
			m_oFan[ChairID] = major.m_oFan;
			Score[ChairID] += m_HuFanNums[ChairID] * m_BaseScore;
			Score[m_WangGangPlayer] -= m_HuFanNums[ChairID] * m_BaseScore;

			m_stHuScore[ChairID].stHu.Add(tagHuRecord(m_WangGangPlayer, bCard, HU_PAOHU, m_HuFanNums[ChairID]));
		}
		m_stRecodeInfo[m_WangGangPlayer].iDianPaoTimes++;
		m_stRecodeInfo[ChairID].iJiePaoTimes++;
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
		m_stRecodeInfo[ChairID].iZiMoTimes++;
	}
	else
	{//放炮
		int iOpType = OutCardEstimate(ChairID, bCard, major, m_HuFanNums[ChairID]);
		if (iOpType & TYPE_HU)
		{
			m_oFan[ChairID] = major.m_oFan;
			Score[ChairID] += m_HuFanNums[ChairID] * m_BaseScore;			
			Score[m_bCurChairID] -= m_HuFanNums[ChairID] * m_BaseScore;

			m_stHuScore[ChairID].stHu.Add(tagHuRecord(m_bCurChairID, bCard, HU_PAOHU, m_HuFanNums[ChairID] ));
		}	
		m_stRecodeInfo[m_bCurChairID].iDianPaoTimes++;
		m_stRecodeInfo[ChairID].iJiePaoTimes++;
	}

	return 0;
}


bool Table_chengdumj::hasQuePai(_uint8 ChairID, _uint8 & queCard)
{
	_uint8 type = m_UserDingQueSe[ChairID];
	for (int i = 0; i < m_HandCard[ChairID].ChangeableCardsLen; i++)
	{
		if (CardType(m_HandCard[ChairID].ChangeableCards[i]) == type)
		{
			queCard = m_HandCard[ChairID].ChangeableCards[i];
			return true;
		}
	}
	return false;
}

void Table_chengdumj::UserOperatePeng(_uint8 ChairID, _uint8 bCard)
{	
	log_info("%s uid:%d ChairID:%d, bCard:0x%02x \n", __FUNCTION__, GetPlayerUid(ChairID), ChairID, bCard);
	memset(m_bAddFanFlag, 0, sizeof(m_bAddFanFlag));	
	memset(m_bUserOperate, 0, sizeof(m_bUserOperate));
	m_stQiHuInfo[ChairID].InitQiHuInfo();

	BroadcastOperateResult(ChairID, TYPE_PENG, bCard);

	int fixedlen = m_HandCard[ChairID].FixedCardsLen;
	//if (fixedlen >= 4)  //17张牌可以碰5下
	if (fixedlen >= HAND_CARD_KAN_MAX)
	{
		log_error("玩家组合牌长度:%d大于正常的长度 \n", fixedlen);
		return;
	}
	m_GameLogic.ExecutePeng(&m_HandCard[ChairID], m_bOutCardChairID, bCard);
	m_bGetCardCount[ChairID]++;

	//从打出牌删掉这张牌	
	m_stUserOutCard[m_bOutCardChairID].RemoveLast(bCard);

	//m_bOutCardCount[CardType(bCard)][CardNum(bCard)] += 2;


	//TODO 检查是否听牌
	SendTingInfo(ChairID);

	UserAfterOperate(ChairID, TYPE_PENG, bCard);

	int score[GAME_PLAYER] = { 0 };
	save_video_operate(TYPE_PENG, ChairID, m_bOutCardChairID, bCard, score);
}

void Table_chengdumj::Proto_GameEnd_GangInfo(_uint8 ChairID, proto::game::chengdumj::HuPaiInfo* pstHuPaiInfo)
{
	for (int j = 0; j < 5; j++)
	{
		if (false == m_GangFen[ChairID][j].bIsNull)
		{
			proto::game::chengdumj::GangInfo* pGangInfo = pstHuPaiInfo->add_ganginfo();
			pGangInfo->set_gangpai(m_GangFen[ChairID][j].nGangPai);
			pGangInfo->set_gangtype(m_GangFen[ChairID][j].nGangType);
			pGangInfo->set_diangangid(m_GangFen[ChairID][j].nDianGangChairId);
			pGangInfo->set_gangyingfen(m_GangFen[ChairID][j].totalscore);

			if (INVALID_CHAIRID != m_GangFen[ChairID][j].uid1)
			{
				proto::game::chengdumj::GangFenInfo* pGangFenInfo = pGangInfo->add_gangfeninfo();
				pGangFenInfo->set_seatid(m_GangFen[ChairID][j].uid1); 
				pGangFenInfo->set_gangfen(m_GangFen[ChairID][j].score1); 
			}
			if (INVALID_CHAIRID != m_GangFen[ChairID][j].uid2)
			{
				proto::game::chengdumj::GangFenInfo* pGangFenInfo = pGangInfo->add_gangfeninfo();
				pGangFenInfo->set_seatid(m_GangFen[ChairID][j].uid2); 
				pGangFenInfo->set_gangfen(m_GangFen[ChairID][j].score2); 
			}
			if (INVALID_CHAIRID != m_GangFen[ChairID][j].uid3)
			{
				proto::game::chengdumj::GangFenInfo* pGangFenInfo = pGangInfo->add_gangfeninfo();
				pGangFenInfo->set_seatid(m_GangFen[ChairID][j].uid3); 
				pGangFenInfo->set_gangfen(m_GangFen[ChairID][j].score3); 
			}
		}
	}
}

void Table_chengdumj::Proto_GameEnd_DaJiaoInfo(_uint8 bCard[GAME_PLAYER], int nDaJiaoScore[GAME_PLAYER], proto::game::chengdumj::AckCDGameEnd& stHuPaiInfo)
{

	for (int i = 0; i < m_GAME_PLAYER; i++)
	{	
		log_info("%s tid:%d chairid:%d bCard:0x%02x, nDaJiaoScore:%d \n",__FUNCTION__, tid, i, bCard[i], nDaJiaoScore[i]);

		proto::game::chengdumj::DaJiaoInfo* stDaJiaoInfo = stHuPaiInfo.add_dajiaoinfo();

		stDaJiaoInfo->set_seatid(i);
		if (0 == nDaJiaoScore[i])
		{
			stDaJiaoInfo->set_dajiaotype(1);
		}
		else if (0 > nDaJiaoScore[i])
		{
			stDaJiaoInfo->set_dajiaotype(2);
		}
		else
		{
			stDaJiaoInfo->set_dajiaotype(3);
		}
		stDaJiaoInfo->set_dajiaofan(m_nDaJiaoFan[i]);
		stDaJiaoInfo->set_dajiaofenshu(nDaJiaoScore[i]);
		stDaJiaoInfo->set_dajiaopai(bCard[i]);

		if (nDaJiaoScore[i] > 0 && bCard[i] != INVALID_MJ_CARD)
		{
			HandCards tempHandCard = m_HandCard[i];
			m_GameLogic.AddCard(&tempHandCard, bCard[i]);
			MJ_fan_buffer fan_buffer;
			if (Check_Hu(i, tempHandCard, fan_buffer) == TYPE_HU)
			{
				if (5 == CardNum(bCard[i]))
				{
					int nCardType = INVALID_MJ_CARD;
					int nCardNum = INVALID_MJ_CARD;
					_uint8 CardDataNum[MJ_TYPE_COMMON][10] = { { 0 }, { 0 }, { 0 }, { 0 }, { 0 } };
					nCardNum = CardNum(bCard[i]);
					nCardType = CardType(bCard[i]);

					m_GameLogic.GetCardCount(&tempHandCard, CardDataNum);
					if ((CardDataNum[nCardType][4] > 0) && (CardDataNum[nCardType][6] > 0))
					{
						bool bPingHu = false;
						MJ_win_pattern pattern;
						m_GameLogic.RemoveCard(&tempHandCard, CardValue(nCardType, 4), 1);
						m_GameLogic.RemoveCard(&tempHandCard, CardValue(nCardType, 5), 1);
						m_GameLogic.RemoveCard(&tempHandCard, CardValue(nCardType, 6), 1);
						if (FormatCards(&tempHandCard, pattern))
						{
							bPingHu = true;
						}
						m_GameLogic.AddCard(&tempHandCard, CardValue(nCardType, 4));
						m_GameLogic.AddCard(&tempHandCard, CardValue(nCardType, 5));
						m_GameLogic.AddCard(&tempHandCard, CardValue(nCardType, 6));

						if (true == bPingHu)
						{
							if (m_conf_js.bJiaXinWu)
							{
								fan_buffer.Add(MJ_FAN_TYPE_JIAXINWU);
							}
						}
					}
				}

				for (int i = 0; i < fan_buffer.Size(); i++)
				{
					if (fan_buffer[i] == MJ_FAN_TYPE_GOUPAI)
					{
						log_debug("%s oFan:%d fan_num:%d .\n", __FUNCTION__, fan_buffer[i], m_nGouPaiNum[i]);
						stDaJiaoInfo->add_dajiaoleixing(fan_buffer[i]);
						stDaJiaoInfo->add_dajiaoleixingfan(m_nGouPaiNum[i]);
					}
					else
					{
						log_debug("%s oFan:%d fan_num:%d .\n", __FUNCTION__, fan_buffer[i], m_ft2fn->GetFan(fan_buffer[i]));
						stDaJiaoInfo->add_dajiaoleixing(fan_buffer[i]);
						stDaJiaoInfo->add_dajiaoleixingfan(m_ft2fn->GetFan(fan_buffer[i]));
					}
				}
			}
			m_GameLogic.ChuPai(&tempHandCard, bCard[i]);
		}
		else
		{

		}
	}
	


}

// 使用本函数之前需要先计算退杠分
void Table_chengdumj::CalGangScore()
{
	TurnGangScore();

	for (int i = 0; i < m_GAME_PLAYER; i++)
	{
		m_nGangScore[i] = m_stGang[i].iTotalScore + m_nTuiGang[i];
		log_info("%s tid:%d uid:%d ChairID:%d iTotalScore:%d m_nTuiGang:%d m_nGangScore:%d.\n", __FUNCTION__, tid, GetPlayerUid(i), i,  m_stGang[i].iTotalScore, m_nTuiGang[i], m_nGangScore[i]);
		m_nTuiGang[i] = 0;
	}
}

// 抢杠胡
bool Table_chengdumj::RobGangHu(_uint8 ChairID, _uint8 CardData, int iOpType)
{  
	if (iOpType & TYPE_WANGANG && !m_conf.m_bQiangGangHu)
		return false;
	if (iOpType & TYPE_ZHIGANG && !m_conf.m_bQiangZhiGang)
		return false;
	if (iOpType & TYPE_ANGANG && !m_conf.m_bQiangAnGang)
		return false;	

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
		m_bUserOperate[i] = OutCardEstimate(i, CardData, major, m_HuFanNums[i]);		
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

					if (i == c) save_operate_notify(stNotify);		
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

					if (i == c) save_operate_notify(stNotify);	
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

					if (i == c) save_operate_notify(stNotify);
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
		log_info("%s uid:%d chaird:%d out card, so that chairid:%d have operate:0x%x\n", __FUNCTION__, GetPlayerUid(ChairID), ChairID, i, m_bUserOperate[i]);			
		if (m_bUserOperate[i] & TYPE_HU)
		{
			set_operator_timer(i);

			m_trustee.get()->RobGangOperate(i);
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

		int iGangScore[GAME_PLAYER] = {0};

		proto::game::AckBcOpResult stOpresult;

		Proto_Game_OperateGang(ChairID, CardData, iOpType, iGangScore, stOpresult);

		broadcast(SERVER_OPERATE_RESULT, stOpresult);

		nOprOpType2 = 2;

	}

	return (cbNum > 0);
}


void Table_chengdumj::Proto_Game_OperateGang(_uint8 ChairID, _uint8 bCard, int iOpType, int iGangScore[GAME_PLAYER], proto::game::AckBcOpResult& stOpresult)
{
	stOpresult.set_chairid(ChairID);
	stOpresult.set_outcardchairid(m_bOutCardChairID);
	stOpresult.set_optype(iOpType);
	stOpresult.set_card(bCard);
	if (2 == nOprOpType2)
	{
		stOpresult.set_optype2(nOprOpType2);
	}

	for (int i = 0; i < m_GAME_PLAYER; i++)
	{
		stOpresult.add_score(iGangScore[i]);
		//记录杠的总分
		//m_stGang[i].nTotalScore += nGangScore[i];
		//log_info("chairid:%d , this gangscore:%d, totalscore:%d \n", i, iGangScore[i], m_stGang[i].iTotalScore);
	}

	if (m_bGangCalcImme)
	{
		Update_Score(iGangScore);
	}
}


void Table_chengdumj::TurnGangScore()
{
	for (int i = 0; i < m_GAME_PLAYER; i++)
	{
		// 杠分转移
		for (int k = 0; k < m_GAME_PLAYER; k++)
		{
			if (true == m_stGangZhuanYiInfo[k].bIsUsed)
			{
				if (i == m_stGangZhuanYiInfo[k].nUserGang)
				{
					m_stGangZhuanYiInfo[k].bIsUsed = false;
					m_stGang[m_stGangZhuanYiInfo[k].nUserGang].iTotalScore = m_stGang[m_stGangZhuanYiInfo[k].nUserGang].iTotalScore - m_stGangZhuanYiInfo[k].nGangScore;
					m_stGang[m_stGangZhuanYiInfo[k].nUserHu].iTotalScore = m_stGang[m_stGangZhuanYiInfo[k].nUserHu].iTotalScore + m_stGangZhuanYiInfo[k].nHuUserGangScore;
					m_nZhuanYiGang[m_stGangZhuanYiInfo[k].nUserHu] = m_nZhuanYiGang[m_stGangZhuanYiInfo[k].nUserHu] + (m_stGangZhuanYiInfo[k].nGangScore - m_stGangZhuanYiInfo[k].nHuUserGangScore);
					log_info("%s tid:%d uid:%d ChairID:%d nUserGang%d nUserHu:%d nGangScore:%d nHuUserGangScore:%d ", __FUNCTION__, tid, GetPlayerUid(i), i, m_stGangZhuanYiInfo[k].nUserGang, m_stGangZhuanYiInfo[k].nUserHu, m_stGangZhuanYiInfo[k].nGangScore, m_stGangZhuanYiInfo[k].nHuUserGangScore);
				}
			}
		}
	}
}

void Table_chengdumj::OnUserOutCard(_uint8 ChairID, _uint8 bCard)
{
	Table::OnUserOutCard(ChairID, bCard);
	m_bCanDiHu[ChairID] = 2;
}

/*反序列化用，用于重启*/
void Table_chengdumj::restart()
{
	Table::restart();
	if (m_DingQue_Timer_running)
	{
		TIMER_START(m_DingQue_Timer, m_DingQue_Timer_stamp);
	}
	if (m_HuanZhang_Timer_running)
	{
		TIMER_AGAIN_s(m_HuanZhang_Timer);
	}
	for (int i = 0; i < GAME_PLAYER - 1; i++)
	{
		if (robot_dingque_timer_running[i])
		{
			TIMER_START(robot_dingque_timer[i], robot_dingque_stamp[i]);
		}
	}

}