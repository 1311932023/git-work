#include "table_changtaimajiang.h"
#include "../proto/proto.h"
#include "../zjh.h"
#include "../common/log.h"
#include "../game.h"
#include "../libnormalmahjong/mj_common2.h"
#include "../libnormalmahjong/game_logic.h"
#include "../table_helper.h"
#include "le_majiang.h"
#include "le_majiang.h"
#include <vector>
#include <map>
extern  vector<Group> groups;
using namespace proto::game::changtaimajiang;
using namespace proto::game::hunanmj;

extern ZJH zjh;
extern Log log;

class ZhanJiang_Fantype_to_FanNum : public Fantype_to_FanNum
{

public:
	ZhanJiang_Fantype_to_FanNum()
	{			
		fan_num[MJ_FAN_TYPE_PINGHU] = 1;//平胡 
		fan_num[MJ_FAN_CHANGTAI_DANYOUJIN]  = 4;
		fan_num[MJ_FAN_CHANGTAI_DOUBLEYOUJIN] = 8;
		fan_num[MJ_FAN_CHANGTAI_THERRYOUJIN] = 12;
		fan_num[MJ_FAN_CHANGTAI_SANJINDAO] = 2;
		fan_num[MJ_FAN_TYPE_QINGJINHU] = 2;
		fan_num[MJ_FAN_TYPE_QIANGGANGHU] = 1;
		fan_num[MJ_FAN_TYPE_GANGSHANGHUA] = 2;

		SetFan(MJ_FAN_TYPE_PINGHU, 1);
		SetFan(MJ_FAN_CHANGTAI_DANYOUJIN, 4);
		SetFan(MJ_FAN_CHANGTAI_DOUBLEYOUJIN, 8);
		SetFan(MJ_FAN_CHANGTAI_THERRYOUJIN, 12);
		SetFan(MJ_FAN_CHANGTAI_SANJINDAO,2);
		SetFan(MJ_FAN_TYPE_QINGJINHU, 2);
		SetFan(MJ_FAN_TYPE_QIANGGANGHU, 1);
		SetFan(MJ_FAN_TYPE_GANGSHANGHUA, 2);
	}

	bool FanFilter(const MJ_fan_buffer& oFan, MJ_fan_buffer& oOutFan)
	{
		//进行加番处理
		oOutFan.Clear();

		for (int i = 0; i < oFan.Size(); i++)
		{	
			oOutFan.Add(oFan[i]);
	
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

static ZhanJiang_Fantype_to_FanNum g_changtaimajiang_ft2fn;

int Table_changtaimajiang::Calc_Fan(_uint8 ChairID, MJ_fan_buffer& oFan)
{
	int iTotalFan = 0;
	if (oFan.Size()>=2&& oFan.Find(MJ_FAN_TYPE_PINGHU))
	{
		oFan.Remove(MJ_FAN_TYPE_PINGHU);
	}
	for (int i = 0; i < oFan.Size(); i++)
	{
		iTotalFan += m_ft2fn->GetFan(oFan[i]);
	}
	return iTotalFan;
}

void Table_changtaimajiang::ConfInit_Remote(const Json::Value& val)
{
	Table::ConfInit_Remote(val);
	m_conf.m_bSupportChi = true;	 //是否能吃	
	m_conf.m_bZimoHu = false;		//是否只能自摸胡	
	m_conf.m_bYiPaoDuoXiang = false; //
	m_conf.m_bHu13yao = false;		//
	m_conf.m_bHu7dui = false;
	m_conf.m_bZhuangXian = false;	//庄家闲家分开计算
	m_conf.m_bQiangGangHu = true;
	m_conf.m_bQiShou_14_zhang = false;
	m_conf.m_AnGangPoint = 1;
	m_conf.m_ZhiGangPoint = 1;
	m_conf.m_WanGangPoint = 1;
	m_conf.m_bHuangGang = true;
	if (json_check_play_type_id(val))
	{
		MJ_SUB_ID subid;
		int iCounts = json_get_play_type_id(val, 5620, subid);
		if (iCounts != 0)
		{
			if (subid.Find(5620001))
			{
				m_GAME_PLAYER = 4;
			}
			else if (subid.Find(5620002))
			{
				m_GAME_PLAYER = 3;
			}
			else if (subid.Find(5620003))
			{
				m_GAME_PLAYER = 2;
			}
			m_CardPool.Set(CARD_POOL_ALL, tid); //设置牌的数量

			m_room_config.Add(subid[0]);
		}

		iCounts = json_get_play_type_id(val, 5621, subid);
		if (iCounts != 0)
		{//连庄
			if (subid.Find(5621001))
			{
				m_conf_changtaimajiang.mj_conf_changtai_lianzhuang = true;
			}
			for (int i = 0; i < subid.Size(); i++)
			{
				m_room_config.Add(subid[i]);
			}
		}

		iCounts = json_get_play_type_id(val, 5622, subid);
		if (iCounts != 0)
		{//底分
			if (subid.Find(5622001))
			{
				m_bZhuangBaseScore = 2;
				m_bXianBaseScore = 1;
			}
			if (subid.Find(5622002))
			{
				m_bZhuangBaseScore = 3;
				m_bXianBaseScore = 2;
			}
			if (subid.Find(5622003))
			{
				m_bZhuangBaseScore = 4;
				m_bXianBaseScore = 3;
			}
			if (subid.Find(5622004))
			{
				m_bZhuangBaseScore = 5;
				m_bXianBaseScore = 4;
			}
			if (subid.Find(5622005))
			{
				m_bZhuangBaseScore = 6;
				m_bXianBaseScore = 5;
			}
			if (subid.Find(5622006))
			{
				m_bZhuangBaseScore = 7;
				m_bXianBaseScore = 6;
			}

			for (int i = 0; i < subid.Size(); i++)
			{
				m_room_config.Add(subid[i]);
			}
		}
	}
}

Table_changtaimajiang::Table_changtaimajiang()
{	
	m_ft2fn = &g_changtaimajiang_ft2fn;
	m_bJinCard.Clear();//金牌
	m_bReplaceJinBaiBanCard = INVALID_MJ_CARD;//代替游金的白板	
	memset(m_bUserHasJinNum, 0, sizeof(m_bUserHasJinNum));//玩家拥有金牌的数量
	m_bHuaCardList.Clear();
	memset(M_bUserDoMultiOperate, 0, sizeof(M_bUserDoMultiOperate));
	memset(m_bUserOperateDanYouJinFlag, 0, sizeof(m_bUserOperateDanYouJinFlag));//
	memset(m_bUserOperateDoubleYouJinFlag, 0, sizeof(m_bUserOperateDoubleYouJinFlag));//
	memset(m_bUserOperateThreeYouJinFlag, 0, sizeof(m_bUserOperateThreeYouJinFlag));//
	//memset(m_bUserOutCardJinNum, 0, sizeof(m_bUserOutCardJinNum));//
	memset(m_bUserOutHuaNum, 0, sizeof(m_bUserOutHuaNum));//
	memset(M_bUserDoMultiOperateQiangGangHu, 0, sizeof(M_bUserDoMultiOperateQiangGangHu));
	m_bBankerOutCard = INVALID_MJ_CARD;
	m_bUserFenBingFlag = false;
	m_bUserFenBingNum = 0;
	memset(m_bUserOperatePengCardInfo, 0, sizeof(m_bUserOperatePengCardInfo));
	memset(&m_bUserOperateGuoZhangHuFlag, 0, sizeof(m_bUserOperateGuoZhangHuFlag));//过张胡
	m_bZhuangBaseScore = 0;
	m_bXianBaseScore = 0;
	m_bLianZhuangNum = 0;
	memset(&m_bUserRogJinHuFlag, 0, sizeof(m_bUserRogJinHuFlag));//过张胡
	memset(&m_bUserRogJinHuPassFlag, 0, sizeof(m_bUserRogJinHuPassFlag));//过张胡
	m_bUserFenOutCardNum = 0;
	memset(&m_bCanYouJinOutCardList, 0, sizeof(m_bCanYouJinOutCardList));//过张胡


}

void Table_changtaimajiang::GameEndReset()
{
	Table::GameEndReset();
	m_bJinCard.Clear();//金牌
	m_bReplaceJinBaiBanCard = INVALID_MJ_CARD;//代替游金的白板	
	memset(m_bUserHasJinNum, 0, sizeof(m_bUserHasJinNum));//玩家拥有金牌的数量
	m_bHuaCardList.Clear();
	memset(M_bUserDoMultiOperate, 0, sizeof(M_bUserDoMultiOperate));
	memset(m_bUserOperateDanYouJinFlag, 0, sizeof(m_bUserOperateDanYouJinFlag));//
	memset(m_bUserOperateDoubleYouJinFlag, 0, sizeof(m_bUserOperateDoubleYouJinFlag));//
	memset(m_bUserOperateThreeYouJinFlag, 0, sizeof(m_bUserOperateThreeYouJinFlag));//
	//memset(m_bUserOutCardJinNum, 0, sizeof(m_bUserOutCardJinNum));//
	memset(m_bUserOutHuaNum, 0, sizeof(m_bUserOutHuaNum));//
	memset(M_bUserDoMultiOperateQiangGangHu, 0, sizeof(M_bUserDoMultiOperateQiangGangHu));
	m_bBankerOutCard = INVALID_MJ_CARD;
	m_bUserFenBingFlag = false;
	m_bUserFenBingNum = 0;
	memset(m_bUserOperatePengCardInfo, 0, sizeof(m_bUserOperatePengCardInfo));
	memset(&m_bUserOperateGuoZhangHuFlag, 0, sizeof(m_bUserOperateGuoZhangHuFlag));//过张胡
	//m_bLianZhuangNum = 0;
	memset(&m_bUserRogJinHuFlag, 0, sizeof(m_bUserRogJinHuFlag));//过张胡
	memset(&m_bUserRogJinHuPassFlag, 0, sizeof(m_bUserRogJinHuPassFlag));//过张胡
	m_bUserFenOutCardNum = 0;
	memset(&m_bCanYouJinOutCardList, 0, sizeof(m_bCanYouJinOutCardList));//过张胡

}

Table_changtaimajiang::~Table_changtaimajiang()
{	

}

void Table_changtaimajiang::GameStartAndGetJin()
{
	_uint8 bCard_Hua[] =
	{
		0x51,0x52,0x53,0x54,0x55,0x56,0x57,0x58,
	};
	_uint8 bGetJinCard = m_CardPool.GetOneCard();
	if (bGetJinCard >= 0x51 && bGetJinCard <= 0x54)
	{
		
		for (int i = 0; i < 4; i++)
		{
			if (bCard_Hua[i] == bGetJinCard)
			{
				continue;
			}
			m_bJinCard.Add(bCard_Hua[i]);
		}
	}
	else if (bGetJinCard >= 0x55 && bGetJinCard <= 0x58)
	{
		
		for (int i = 4; i < 8; i++)
		{
			if (bCard_Hua[i] == bGetJinCard)
			{
				continue;
			}
			m_bJinCard.Add(bCard_Hua[i]);
		}
	}
	else
	{
		m_bJinCard.Add(bGetJinCard);
	}

	//向客户端发送花牌信息。
	m_bReplaceJinBaiBanCard = 0x43;
	if (bGetJinCard >= 0x51 && bGetJinCard <= 0x54)
	{
		m_bHuaCardList.Add(bGetJinCard);
		for (int i = 4; i < 8; i++)
		{
			m_bHuaCardList.Add(bCard_Hua[i]);
		}
		m_bHuaCardList.Add(m_bReplaceJinBaiBanCard);
	}
	else if (bGetJinCard >= 0x55 && bGetJinCard <= 0x58)
	{
		for (int i = 0; i < 4; i++)
		{
			m_bHuaCardList.Add(bCard_Hua[i]);
		}
		m_bHuaCardList.Add(m_bReplaceJinBaiBanCard);
	}
	else
	{
		for (int i = 0; i < 8; i++)
		{
			m_bHuaCardList.Add(bCard_Hua[i]);
		}
	}
}

void Table_changtaimajiang::InitBanker()
{
	if (m_nPlayingTimes == 0)
	{
		m_bBanker = 0;
		for (int i = 0; i < m_GAME_PLAYER; i++)
		{
			if (m_SeatPlayer[i].player != NULL)
			{
				log_info("%s room %d player uid:%d, room owner:%d\n", __FUNCTION__, tid, GetPlayerUid(i), m_room_owner_uid);
				if (m_SeatPlayer[i].player->uid == m_room_owner_uid)
				{
					srand((int)time(0));
					m_bBanker = rand() % m_GAME_PLAYER;
					break;
				}
			}
		}
	}
	if (m_bBanker >= m_GAME_PLAYER)
	{
		m_bBanker = 0;
	}
	assert(m_bBanker < m_GAME_PLAYER);
}


void Table_changtaimajiang::GameStart()
{

	Table::GameStart();	
	GameStartAndGetJin();//开金
	Send_Hand_Card();	
	if (m_bJinCard.Size() > 0)
	{
		_uint8 bCard_Hua[] =
		{
			0x51,0x52,0x53,0x54,0x55,0x56,0x57,0x58,
		};
		::proto::game::changtaimajiang::RspGameLaiziInfo laiZiInfo;
		for (int i = 0; i < m_bJinCard.Size(); i++)
		{
			laiZiInfo.add_laizisignlist(m_bJinCard[i]);
		}
		if (m_bJinCard.Size() == 1)
		{
			laiZiInfo.add_laizishowlist(m_bJinCard[0]);
			laiZiInfo.add_laizireplacelist(0x43);
		}
		else
		{
			if (m_bJinCard[0]< 0x55)
			{
				for (int i = 0; i < 4;i++)
				{
					if (m_bJinCard.Find(bCard_Hua[i]))
					{
						continue;
					}
					laiZiInfo.add_laizishowlist(bCard_Hua[i]);
				}
			}
			else
			{
				for (int i = 4; i < 8; i++)
				{
					if (m_bJinCard.Find(bCard_Hua[i]))
					{
						continue;
					}
					laiZiInfo.add_laizishowlist(bCard_Hua[i]);
				}

			}
		}
		laiZiInfo.set_flag(true);
		broadcast(SERVER_ROOM_KAIJIN_INFO, laiZiInfo);
		/*保存录像*/
		SaveVideoData(SERVER_ROOM_KAIJIN_INFO, laiZiInfo);
	}
	m_bReplaceJinBaiBanCard = 0x43;//使用白板代替金牌原来值。

								   //向客户端发送花牌信息。
	::proto::game::changtaimajiang::RspGameHuapaiInfo HuaPaiInfo;

	for (int j = 0; j < m_bHuaCardList.Size();j++)
	{
		HuaPaiInfo.add_huapailist(m_bHuaCardList[j]);
	}
	broadcast(SERVER_GAME_BUHUA_HUAPAILIST, HuaPaiInfo);
	SaveVideoData(SERVER_GAME_BUHUA_HUAPAILIST, HuaPaiInfo);
	for (int i = 0; i < m_GAME_PLAYER; i++)
	{
		::proto::game::changtaimajiang::RspGameHuapaiInitCardInfo UserHuaPaiInfo;
		UserHuaPaiInfo.set_seatid(i);
		for (int j = 0; j < m_bUserOutHuaNum[i].Size();j++)
		{		
			UserHuaPaiInfo.add_cards(m_bUserOutHuaNum[i][j]);
		}
		broadcast(SERVER_GAME_BUHUA_USERHUAPAILIST, UserHuaPaiInfo);
		SaveVideoData(SERVER_GAME_BUHUA_USERHUAPAILIST, UserHuaPaiInfo);
	}

	ChangeHandCardJinAndBaiBan();
	OnUserGetCard(m_bBanker);
}

void Table_changtaimajiang::Send_Hand_Card()
{	
	for (int i = 0; i < m_GAME_PLAYER; i++)
	{
		for (int j = 0; j < m_HandCard[i].ChangeableCardsLen;j++)
		{
			if (m_bHuaCardList.Find(m_HandCard[i].ChangeableCards[j]))
			{

				_uint8 bGetCard = m_HandCard[i].ChangeableCards[j];
				m_bUserOutHuaNum[i].Add(bGetCard);
				m_GameLogic.ChuPai(&m_HandCard[i], bGetCard);
				bGetCard = m_CardPool.GetLast();	
				m_GameLogic.AddCard(&m_HandCard[i], bGetCard);
				j = -1;
			}
		}
	}
	//给客户端发牌
	proto::game::AckFirstHandCard stFirsthandcard;
	for (int i = 0; i < m_GAME_PLAYER; i++)
	{
		//memset(buf, 0, sizeof(buf));
		stFirsthandcard.Clear();
		proto::game::HandCards *pstHc = stFirsthandcard.mutable_usercard();
		pstHc->set_changeablecardslen(m_HandCard[i].ChangeableCardsLen);

		for (int j = 0; j < m_HandCard[i].ChangeableCardsLen; j++)
		{
			pstHc->add_changeablecards(m_HandCard[i].ChangeableCards[j]);
		}

		/*每个人都要知道自己和别人的牌张数*/
		for (int k = 0; k < m_GAME_PLAYER; k++)
		{
			stFirsthandcard.add_handcardnums(m_HandCard[k].ChangeableCardsLen);
		}
		/*把首个手牌信息发送给客户端*/
		unicast(SERVER_FIRST_HANDCARD, i, stFirsthandcard);

		log_info(" %s tid:%d first uid:%d chairid:%d [%s]\n", __FUNCTION__, tid, GetPlayerUid(i), i, CatHandCard(m_HandCard[i], i));

	}

	for (int i = 0; i < m_GAME_PLAYER; i++)
	{
		m_GameLogic.SortCard(m_HandCard[i].ChangeableCards, m_HandCard[i].ChangeableCardsLen);
	}
	/*保存首次手牌视频信息*/
	save_video_firsthand_card();

}

void Table_changtaimajiang::Proto_GameScene_Operate(_uint8 ChairID, Player* player, proto::game::AckGameScene& stGameScene)
{
	if (m_bMultiOpUserNum != 0 && m_bMultiUserDoneOp[ChairID] != TYPE_NULL)
	{
		log_info("%s user has selected uid:%d chairid:%d iOpType:0x%x!\n", __FUNCTION__, GetPlayerUid(ChairID), ChairID, m_bMultiUserDoneOp[ChairID]);
		return;
	}
	//log_info("m_bUserOperate[ChairID = %d] = %x", ChairID, m_bUserOperate[ChairID]);
	stGameScene.set_optype(m_bUserOperate[ChairID]);
	if (m_bUserOperate[ChairID] & TYPE_ANGANG || m_bUserOperate[ChairID] & TYPE_WANGANG)
	{
		MJ_opinfo stGang;
		Table::GetGangInfo(ChairID, stGang);
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

	if (m_bCanYouJinOutCardList[ChairID].Size()>0)
	{
	
		for (int i = 0; i < m_bCanYouJinOutCardList[ChairID].Size(); i++)
		{
			stGameScene.add_youjinoutcardlist(m_bCanYouJinOutCardList[ChairID][i]);
		}
	}
}

void Table_changtaimajiang::Proto_GameScene(_uint8 ChairID, Player* player, proto::game::AckGameScene& stGameScene)
{
	//断线重连前，替换手牌中的金与白板。发送数据后，重新替换收牌中的金与白板。
	ChangeHandCardJinAndBaiBan();
	Table::Proto_GameScene(ChairID, player, stGameScene);
	ChangeHandCardJinAndBaiBan();
	//log_info("ChairID:%d Proto_GameScene stGameScene = %s", ChairID, stGameScene.DebugString().c_str());
	if (m_bJinCard.Size()> 0 && m_bGameState == GAME_PLAYING)
	{
		if (m_bJinCard.Size() > 0)
		{
			_uint8 bCard_Hua[] =
			{
				0x51,0x52,0x53,0x54,0x55,0x56,0x57,0x58,
			};
			::proto::game::changtaimajiang::RspGameLaiziInfo laiZiInfo;
			for (int i = 0; i < m_bJinCard.Size(); i++)
			{
				laiZiInfo.add_laizisignlist(m_bJinCard[i]);
			}
			if (m_bJinCard.Size() == 1)
			{
				laiZiInfo.add_laizishowlist(m_bJinCard[0]);
				laiZiInfo.add_laizireplacelist(0x43);
			}
			else
			{
				if (m_bJinCard[0] < 0x55)
				{
					for (int i = 0; i < 4; i++)
					{
						if (m_bJinCard.Find(bCard_Hua[i]))
						{
							log_info("bCard_Hua[i] = %x", bCard_Hua[i]);

							continue;
						}
						laiZiInfo.add_laizishowlist(bCard_Hua[i]);
					}
				}
				else
				{
					for (int i = 4; i < 8; i++)
					{
						if (m_bJinCard.Find(bCard_Hua[i]))
						{
							continue;
						}
						laiZiInfo.add_laizishowlist(bCard_Hua[i]);
					}

				}
			}
			laiZiInfo.set_flag(false);
			unicast(SERVER_ROOM_KAIJIN_INFO, ChairID, laiZiInfo);
		}
	}
	//向客户端发送花牌信息。
	if (m_bHuaCardList.Size() > 0)
	{
		::proto::game::changtaimajiang::RspGameHuapaiInfo HuaPaiInfo;
		for (int i = 0; i < m_bHuaCardList.Size(); i++)
		{
			HuaPaiInfo.add_huapailist(m_bHuaCardList[i]);
		}
		unicast(SERVER_GAME_BUHUA_HUAPAILIST, ChairID, HuaPaiInfo);
	}

	for (int i = 0; i < m_GAME_PLAYER; i++)
	{
		::proto::game::changtaimajiang::RspGameHuapaiInitCardInfo UserHuaPaiInfo;
		UserHuaPaiInfo.set_seatid(i);
		for (int j = 0; j < m_bUserOutHuaNum[i].Size(); j++)
		{
			UserHuaPaiInfo.add_cards(m_bUserOutHuaNum[i][j]);
		}
		unicast(SERVER_GAME_BUHUA_USERHUAPAILIST, ChairID, UserHuaPaiInfo);
		log_info("UserHuaPaiInfo = %s", UserHuaPaiInfo.DebugString().c_str());
	}
}

void Table_changtaimajiang::GameEnd()
{
	ChangeHandCardJinAndBaiBan();
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
		Calc_GangScore();
		Score_GameEndSucc(hu_score, stGameend);
	}
	
	log_fatal("%s tid:%d round:%d score[%d,%d,%d,%d]", __FUNCTION__, tid, m_nPlayingTimes, hu_score[0].iTotleScore, hu_score[1].iTotleScore, hu_score[2].iTotleScore, hu_score[3].iTotleScore);

	int UserScore[GAME_PLAYER] = { 0 };//四个玩家输赢总分
	memset(UserScore, 0, sizeof(UserScore));
	for (int i = 0; i < m_GAME_PLAYER; i++)
	{
		m_stHuScore[i].iTotalScore = hu_score[i].iTotleScore;
		if (m_bUserFenBingNum > 0)
		{
			if (i == m_bBanker)
			{
				stGameend.add_score(hu_score[i].iTotleScore + m_stGang[i].iTotalScore- m_bUserFenBingNum*(m_GAME_PLAYER-1));
			}
			else
			{
				stGameend.add_score(hu_score[i].iTotleScore + m_stGang[i].iTotalScore+ m_bUserFenBingNum);

			}
		}
		else
		{
			stGameend.add_score(hu_score[i].iTotleScore + m_stGang[i].iTotalScore);

		}

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

	for (int i = 0; i < m_GAME_PLAYER; i++)
	{

		stGameend.set_huscore(m_stHuScore[i].iTotalScore);
		stGameend.clear_score();
		for (int j = 0; j < m_GAME_PLAYER; j++)
		{
			stGameend.add_score(UserScore[j]);
		}
		//	log_debug("short_info  %s  ", stGameend.zhanjiangmjgameendinfo().ShortDebugString().c_str());

		unicast(SERVER_GAME_END, i, stGameend);
		log_fatal("%s uid:%d chairid:%d score %d!\n", __FUNCTION__, GetPlayerUid(i), i, UserScore[i]);
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

void Table_changtaimajiang::Proto_GameEndInfo(const MJ_user_buffer& UserID, proto::game::AckGameEnd& stGameend)
{

	Table::Proto_GameEndInfo(UserID, stGameend);

	proto::game::changtaimajiang::GameEndPlayerScoresInfo  *pEnd[GAME_PLAYER] = {0};
	for (_uint8 i = 0; i < m_GAME_PLAYER; i++)
	{
		pEnd[i] = stGameend.add_playerscoreinfolist();
	}
	for (int i = 0; i < m_GAME_PLAYER; i++)
	{
		pEnd[i]->set_seatid(i);
		pEnd[i]->set_huscore(m_stHuScore[i].iTotalScore);
		pEnd[i]->set_gangscore(m_stGang[i].iTotalScore);
		if (m_bUserFenBingNum >0&& UserID.Size()>0)
		{
			if (i == m_bBanker)
			{
				int score = -m_bUserFenBingNum*(m_GAME_PLAYER-1);
				pEnd[i]->set_zengscore(score);
			}
			else
			{
				pEnd[i]->set_zengscore(m_bUserFenBingNum);

			}
		}
		if (i == m_bBanker)
		{
			if (m_conf_changtaimajiang.mj_conf_changtai_lianzhuang == true)
			{
				pEnd[i]->set_extra_dianscore(m_bLianZhuangNum);
			}
		}
	}

	//log_info("stGameend = %s", stGameend.DebugString().c_str());
	tagHuNanGameEnd* end = stGameend.mutable_hunangameendinfo();
	
	end->set_leftcardnum(m_CardPool.Count());
	//向客户端发送马牌
	MJ_user_buffer HuUserID;

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
		assert(m_stHuScore[win_seat].stHu.Size() != 0);
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

void Table_changtaimajiang::ChangeHandCardJinAndBaiBan()
{
	if (m_bJinCard.Size()==1)
	{
		for (int i = 0; i < m_GAME_PLAYER; i++)
		{
			for (int j = 0; j < m_HandCard[i].ChangeableCardsLen; j++)
			{
				if (m_HandCard[i].ChangeableCards[j] == m_bJinCard[0])
				{
					m_HandCard[i].ChangeableCards[j] = m_bReplaceJinBaiBanCard;
				}
				else if (m_HandCard[i].ChangeableCards[j] == m_bReplaceJinBaiBanCard)
				{
					m_HandCard[i].ChangeableCards[j] = m_bJinCard[0];
				}
			}

			for (int j = 0; j < m_HandCard[i].FixedCardsLen; j++)
			{
				if (m_HandCard[i].FixedCards[j].state & TYPE_CHI)
				{
					for (int z = 0; z < 3; z++)
					{
						if (m_HandCard[i].FixedCards[j].c[z] == m_bJinCard[0])
						{
							m_HandCard[i].FixedCards[j].c[z] = m_bReplaceJinBaiBanCard;
						}
						else if (m_HandCard[i].FixedCards[j].c[z] == m_bReplaceJinBaiBanCard)
						{
							m_HandCard[i].FixedCards[j].c[z] = m_bJinCard[0];
						}

					}
				}
				else
				{
					if (m_HandCard[i].FixedCards[j].CardData == m_bJinCard[0])
					{
						m_HandCard[i].FixedCards[j].CardData = m_bReplaceJinBaiBanCard;
					}
					else if (m_HandCard[i].FixedCards[j].CardData == m_bReplaceJinBaiBanCard)
					{
						m_HandCard[i].FixedCards[j].CardData = m_bJinCard[0];
					}
				}
			}
		}
	}

	////替换完成后,对手牌进行重新排序.
	for (int i = 0; i < m_GAME_PLAYER; i++)
	{
		m_GameLogic.SortCard(m_HandCard[i].ChangeableCards, m_HandCard[i].ChangeableCardsLen);
	}
}

void Table_changtaimajiang::OnUserGetCard(_uint8 ChairID, bool bGetLast)
{
	if (RobJinHu( ChairID))
	{
		m_bCurChairID = ChairID;
		m_bUserRogJinHuFlag[ChairID] = true;
		return;
	}
	if (m_bUserFenBingFlag == false&& ChairID==m_bBanker&& m_bUserFenOutCardNum == m_GAME_PLAYER)
	{

		if (ChairID == m_bBanker&&m_bGetCardCount[m_bBanker]>=1)
		{
			proto::game::changtaimajiang::RspGenZhuang sGenZhuang;
			sGenZhuang.set_gengzhaungflag(true);
			broadcast(SERVER_ROOM_GENG_ZHUANG, sGenZhuang);
			SaveVideoData(SERVER_ROOM_GENG_ZHUANG, sGenZhuang);
		}
		int bnum = 0;
		if (m_bGetCardCount[m_bBanker] >= 1)
		{
			bnum = m_bGetCardCount[m_bBanker] - 1;
			m_bUserFenBingNum += 1 << bnum;
		}		
	}

	if (m_bDeleteTable)
	{
		return;
	}

	if (m_CardPool.Count()==16)
	{
		GameEnd();
		return;
	}
	memset(&m_bUserOperate, 0, sizeof(m_bUserOperate));
	m_bCurChairID = ChairID;

	_uint8 bGetCard = bGetLast ? m_CardPool.GetLast() : m_CardPool.GetOneCard();
	
	m_bGetCardCount[ChairID]++;

	m_bCurCard = bGetCard;

	//进行金牌与白板的互换
	_uint8 bSaveGetCard = bGetCard;

	if (m_bJinCard.Size()==1)
	{
		if (m_bJinCard[0]== bGetCard)
		{
			bSaveGetCard = m_bReplaceJinBaiBanCard;
		} 
		else if(bGetCard== m_bReplaceJinBaiBanCard)
		{
			bSaveGetCard = m_bJinCard[0];
		}
	}
	MJ_major major;
	//摸牌判断
	m_bUserOperate[ChairID] = GetCardEstimate(ChairID, bSaveGetCard, major, m_HuFanNums[ChairID]);

	m_GameLogic.AddCard(&m_HandCard[ChairID], bSaveGetCard);
	m_bCanYouJinOutCardList[ChairID].Clear();
	int bYouJinFlag = 0;
	if ((m_bUserOperate[ChairID] && TYPE_HU))
	{
		IsYouJinCanOutCard(ChairID, bYouJinFlag);
	}
	//log_info("m_bCanYouJinOutCardList[ChairID = %d].Size() = %d bYouJinFlag = %d", ChairID, m_bCanYouJinOutCardList[ChairID].Size(), bYouJinFlag);
	if (m_bCanYouJinOutCardList[ChairID].Size() > 0)
	{
		if (major.m_oFan.Find(MJ_FAN_CHANGTAI_DOUBLEYOUJIN))
		{
			if (bYouJinFlag == 3)
			{
				m_bUserOperate[ChairID] |= TYPE_YUNXIAO_YOUJIN_THREE;
				m_bUserOperate[ChairID] |= TYPE_FORBID_PASS;
			}
		}else if (major.m_oFan.Find(MJ_FAN_CHANGTAI_DANYOUJIN))
		{
			if (bYouJinFlag == 2)
			{
				m_bUserOperate[ChairID] |= TYPE_YUNXIAO_YOUJIN_TWO;
				m_bUserOperate[ChairID] |= TYPE_FORBID_PASS;
			}

		}else if (m_bUserOperate[ChairID] & TYPE_HU&&!major.m_oFan.Find(MJ_FAN_CHANGTAI_DOUBLEYOUJIN)&& !major.m_oFan.Find(MJ_FAN_CHANGTAI_THERRYOUJIN))
		{
			if (bYouJinFlag == 1)
			{
				m_bUserOperate[ChairID] |= TYPE_YUNXIAO_YOUJIN;
				m_bUserOperate[ChairID] |= TYPE_FORBID_PASS;
			}
			else if (bYouJinFlag == 2)
			{
				m_bUserOperate[ChairID] |= TYPE_YUNXIAO_YOUJIN_TWO;
				m_bUserOperate[ChairID] |= TYPE_FORBID_PASS;
			}
		}

	}

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
		if (m_bUserOperate[i]>0)
		{
			proto::game::AckOpNotify stVideo;
			stVideo.set_chairid(i);
			stVideo.set_optype(m_bUserOperate[i]);
			std::string str = stVideo.SerializeAsString();
			save_video_data(SERVER_OPERATE_NOTIFY, str);
			//log_info("OnUserGetCard stUsergetcard = %s", stUsergetcard.DebugString().c_str());

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
	//补花
	if (m_bHuaCardList.Find(bGetCard))
	{
		OnUserOutCard_BuHua( ChairID, bGetCard);
		OnUserGetCard(ChairID,true);
	}

}

void Table_changtaimajiang::OnUserGetCard_YouJinBuPai(_uint8 ChairID, bool bGetLast)
{

	if (m_bDeleteTable)
	{
		return;
	}

	if (m_CardPool.Count() == 16)
	{
		GameEnd();
		return;
	}
	memset(&m_bUserOperate, 0, sizeof(m_bUserOperate));
	m_bCurChairID = ChairID;

	_uint8 bGetCard = bGetLast ? m_CardPool.GetLast() : m_CardPool.GetOneCard();

	m_bGetCardCount[ChairID]++;

	m_bCurCard = bGetCard;

	//进行金牌与白板的互换
	_uint8 bSaveGetCard = bGetCard;

	if (m_bJinCard.Size() == 1)
	{
		if (m_bJinCard[0] == bGetCard)
		{
			bSaveGetCard = m_bReplaceJinBaiBanCard;
		}
		else if (bGetCard == m_bReplaceJinBaiBanCard)
		{
			bSaveGetCard = m_bJinCard[0];
		}
	}
	MJ_major major;
	//摸牌判断
	m_bUserOperate[ChairID] = GetCardEstimate(ChairID, bSaveGetCard, major, m_HuFanNums[ChairID]);

	m_GameLogic.AddCard(&m_HandCard[ChairID], bSaveGetCard);
	if (!(m_bUserOperate[ChairID]&TYPE_HU))
	{
		m_bUserOperate[ChairID] = TYPE_NULL;
	}
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
		if (m_bUserOperate[i] > 0)
		{
			proto::game::AckOpNotify stVideo;
			stVideo.set_chairid(i);
			stVideo.set_optype(m_bUserOperate[i]);
			std::string str = stVideo.SerializeAsString();
			save_video_data(SERVER_OPERATE_NOTIFY, str);
			//log_info("OnUserGetCard_YouJinBuPai stUsergetcard = %s", stUsergetcard.DebugString().c_str());
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

int Table_changtaimajiang::GetCardEstimate(_uint8 ChairID, _uint8 bCard, MJ_major& major, int& fan_num)
{

	if (m_bUserRogJinHuFlag[ChairID]==true)
	{
		major.m_oFan.Add(MJ_FAN_TYPE_QINGJINHU);
		fan_num = Calc_Fan(ChairID, major.m_oFan);
		return TYPE_HU;
	}
	assert(major.m_oFan.Size() == 0);

	int iOpType = TYPE_NULL;

	HandCards* pHc = &m_HandCard[ChairID];
	m_GameLogic.AddCard(pHc, bCard);

	if (pHc->ChangeableCardsLen % 3 != 2)
	{
		log_error("%s tid:%d fatal paishu error uid:%d chairid:%d! bCard:%x\n", __FUNCTION__,tid, GetPlayerUid(ChairID), ChairID, bCard);
		for (int i = 0; i < pHc->ChangeableCardsLen;i++)
		{
			log_info("tid:%d ChairID:%d pHc->ChangeableCards[i = %d] = %x",tid, ChairID,i, pHc->ChangeableCards[i]);
		}
		m_GameLogic.ChuPai(pHc, bCard);
		return TYPE_NULL;
	}
	//assert(pHc->ChangeableCardsLen <= HAND_CARD_SIZE_MAX);

	/*获取杠信息*/
	iOpType |= GetGangInfo(ChairID, major.m_GetGangInfo);
	//log_info("ChairID = %d iOpType = %x", ChairID, iOpType);

	/*检查能不能胡*/
	iOpType |= Check_Hu(ChairID, *pHc, major.m_oFan);
	log_info("ChairID = %d iOpType = %x", ChairID, iOpType);
	if (m_bGetCardCount[ChairID] == 1)
	{
		/*如果为第一次摸牌,并且胡牌则为天胡*/
		//UserGetFirstCard(ChairID, iOpType, major.m_oFan);
	}
	if (CardPoolIsEmpty())
	{
		/*最后一张胡牌则为海底捞*/
		//UserGetLastCard(ChairID, iOpType, major.m_oFan);
	}
	if (iOpType & TYPE_HU)
	{
		if (m_bAddFanFlag[ChairID])   //杠上花
		{
			major.m_oFan.Remove(MJ_FAN_TYPE_PINGHU);
			UserGangShangHua(ChairID, bCard, major.m_oFan);
		}
		/*计算番*/
		fan_num = Calc_Fan(ChairID, major.m_oFan);
	}
	int bUserHasJinNum = 0;
	for (int i = 0; i < pHc->ChangeableCardsLen; i++)
	{//每次都检测手牌中金的个数，用于听牌的时候，能够听全部的牌

		if (m_bJinCard.Size() == 1)
		{
			if (pHc->ChangeableCards[i] == m_bReplaceJinBaiBanCard)
			{
				bUserHasJinNum++;
			}
		}
		else
		{
			if (m_bJinCard.Find(pHc->ChangeableCards[i]))
			{
				bUserHasJinNum++;
			}
		}
	}

	if (bUserHasJinNum == 3)
	{
		if (major.m_oFan.Size()==0)
		{
			major.m_oFan.Add(MJ_FAN_CHANGTAI_SANJINDAO);
			iOpType |= TYPE_HU;
		}
	}




	if (m_bUserOperateThreeYouJinFlag[ChairID] == true)
	{
		iOpType = TYPE_HU;
		major.m_oFan.Add(MJ_FAN_CHANGTAI_THERRYOUJIN);
		//iOpType |= TYPE_YUNXIAO_YOUJIN_THREE;
		iOpType |= TYPE_FORBID_PASS;
	}
	else if (m_bUserOperateDoubleYouJinFlag[ChairID] == true)
	{
		iOpType = TYPE_HU;
		major.m_oFan.Add(MJ_FAN_CHANGTAI_DOUBLEYOUJIN);
		//iOpType |= TYPE_YUNXIAO_YOUJIN_TWO;
		iOpType |= TYPE_FORBID_PASS;
	}
	else if (m_bUserOperateDanYouJinFlag[ChairID] == true)
	{
		iOpType = TYPE_HU;
		major.m_oFan.Add(MJ_FAN_CHANGTAI_DANYOUJIN);
		//iOpType |= TYPE_YUNXIAO_YOUJIN;
		iOpType |= TYPE_FORBID_PASS;
	}

	//log_info("m_bUserOperateDanYouJinFlag[ChairID = %d]  = %d", ChairID,m_bUserOperateDanYouJinFlag[ChairID]);
	//log_info("m_bUserOperateDoubleYouJinFlag[ChairID = %d]  = %d", ChairID, m_bUserOperateDoubleYouJinFlag[ChairID]);
	//log_info("m_bUserOperateThreeYouJinFlag[ChairID = %d]  = %d", ChairID, m_bUserOperateThreeYouJinFlag[ChairID]);

	m_GameLogic.ChuPai(pHc, bCard);

	return iOpType;
}

void Table_changtaimajiang::UserGetFirstCard(_uint8 ChairID, int& iOpType, MJ_fan_buffer& oFan)
{

	if (iOpType & TYPE_HU)
	{
		if (ChairID == m_bBanker&&!oFan.Find(MJ_FAN_CHANGTAI_SANJINDAO))
		{
			oFan.Add(MJ_FAN_TYPE_TIANHU);
		}
		else
		{
			if (m_HandCard[ChairID].FixedCardsLen==0 && !oFan.Find(MJ_FAN_CHANGTAI_SANJINDAO))
			{//天听
				oFan.Add(MJ_FAN_TYPE_TIANTINGHU);
			}
		}
	}
}

void Table_changtaimajiang::OnUserOutCard(_uint8 ChairID, _uint8 bCard)
{
	m_bCanYouJinOutCardList[ChairID].Clear();
	m_bUserOperateGuoZhangHuFlag[ChairID] = false;
	m_bUserOperatePengCardInfo[ChairID].Clear();
	//分饼
	if (m_bUserFenBingFlag == false)
	{
		if (ChairID == m_bBanker)
		{
			m_bUserFenOutCardNum = 1;
			m_bBankerOutCard = bCard;
		}
		else
		{
			int bFixedCardsLen = 0;
			for (int i = 0; i < m_GAME_PLAYER;i++)
			{
				for (int j = 0; j < m_HandCard[i].FixedCardsLen; j++)
				{
					if (m_HandCard[i].FixedCards[j].state != TYPE_ANGANG)
					{
						bFixedCardsLen++;
					}
				}
			}

			m_bUserFenOutCardNum++;
			if (m_bBankerOutCard != bCard || bFixedCardsLen > 0)
			{
				m_bUserFenBingFlag = true;
			}
		}
	}


	memset(M_bUserDoMultiOperate, 0, sizeof(M_bUserDoMultiOperate));
	_uint8 bSavebCard = bCard;
	if (m_bJinCard[0] < 0x51)
	{
		if (bSavebCard == m_bJinCard[0])
		{
			bSavebCard = m_bReplaceJinBaiBanCard;

		}
		else if (bSavebCard == m_bReplaceJinBaiBanCard)
		{
			bSavebCard = m_bJinCard[0];
		}
	}

	// 删出牌时钟	
	ev_timer_stop(zjh.loop, &m_Outcard_Timer);
	log_info("%s tid:%d uid:%d chairid:%d outcard:0x%02x \n", __FUNCTION__, tid, GetPlayerUid(ChairID), ChairID, bCard);
	m_bCurChairID = ChairID;
	m_bOutCardChairID = ChairID;
	m_GameLogic.ChuPai(&m_HandCard[ChairID], bSavebCard);
	m_bCurCard = bCard;
	m_bCurOutCard = bCard;
	m_bOverTimeState[ChairID] = 0;

	m_stUserOutCard[ChairID].Add(bCard);

	//广播玩家出牌	
	proto::game::AckUserOutCard stUseroutcard;

	Proto_Game_UserOutCard(ChairID, bCard, stUseroutcard);

	broadcast(SERVER_USER_OUTCARD, stUseroutcard);

	save_user_out_card(ChairID, bCard);
	//判断出牌后是否进入单游金，双游金，三游金状态。
	HandCards tmp = m_HandCard[ChairID];
	_uint8 bAddCard = 0x01;
	if (!m_bJinCard.Find(bAddCard))
	{
		m_GameLogic.AddCard(&tmp, bAddCard);
	}
	else
	{
		bAddCard = 0x02;
		m_GameLogic.AddCard(&tmp, bAddCard);
	}
	tagListenInfos stListenInfo[17];
	_uint8 bListenLen = 0;
	GetListeningInfo_CheckJin(ChairID, stListenInfo, bListenLen, tmp);
	bool bIsTingCardNum = false;
	for (int i = 0; i < bListenLen;i++)
	{//如果打出一张牌之后听任意一张牌，则是游金状态。
		if (stListenInfo[i].bHuLen == 33&& stListenInfo[i].bOutCard== bAddCard)
		{
			//log_info("stListenInfo[i].bHuLen = %d", stListenInfo[i].bHuLen);
			//log_info("stListenInfo[i].bOutCard = %x", stListenInfo[i].bOutCard);

			bIsTingCardNum = true;
			break;
		}
	}

	if (bIsTingCardNum==true)
	{
		if (!m_bJinCard.Find(bCard))
		{
			m_bUserOperateDanYouJinFlag[ChairID] = true;

		}
		else if(m_bUserOperateDoubleYouJinFlag[ChairID] == false)
		{
			m_bUserOperateDoubleYouJinFlag[ChairID] = true;
			m_bUserOperateDanYouJinFlag[ChairID] = false;
		}else if (m_bUserOperateDoubleYouJinFlag[ChairID] == true)
		{
			m_bUserOperateDoubleYouJinFlag[ChairID] = false;
			m_bUserOperateDanYouJinFlag[ChairID] = false;
			m_bUserOperateThreeYouJinFlag[ChairID] = true;
		}
	} 
	else
	{
		m_bUserOperateDanYouJinFlag[ChairID] = false;
		m_bUserOperateDoubleYouJinFlag[ChairID] = false;
		m_bUserOperateThreeYouJinFlag[ChairID] = false;
	}
	//log_info("m_bUserOperateDanYouJinFlag[ChairID = %d]  = %d", ChairID, m_bUserOperateDanYouJinFlag[ChairID]);
	//log_info("m_bUserOperateDoubleYouJinFlag[ChairID = %d]  = %d", ChairID, m_bUserOperateDoubleYouJinFlag[ChairID]);
	//log_info("m_bUserOperateThreeYouJinFlag[ChairID = %d]  = %d", ChairID,m_bUserOperateThreeYouJinFlag[ChairID]);

	if (m_bHuaCardList.Find(bCard))
	{//统计打出的花牌
		m_bUserOutHuaNum[ChairID].Add(bCard);
	}

}

void Table_changtaimajiang::OnUserOutCard_BuHua(_uint8 ChairID, _uint8 bCard)
{

	// 删出牌时钟	
	ev_timer_stop(zjh.loop, &m_Outcard_Timer);
	log_info("%s tid:%d uid:%d chairid:%d outcard:0x%02x \n", __FUNCTION__, tid, GetPlayerUid(ChairID), ChairID, bCard);
	m_bCurChairID = ChairID;
	m_bOutCardChairID = ChairID;
	m_GameLogic.ChuPai(&m_HandCard[ChairID], bCard);
	m_bCurCard = bCard;
	m_bCurOutCard = bCard;
	m_bOverTimeState[ChairID] = 0;

	//广播玩家出牌	
	proto::game::AckUserOutCard stUseroutcard;

	Proto_Game_UserOutCard(ChairID, bCard, stUseroutcard);

	broadcast(SERVER_USER_OUTCARD, stUseroutcard);

	save_user_out_card(ChairID, bCard);
	if (m_bHuaCardList.Find(bCard))
	{//统计打出的花牌
		m_bUserOutHuaNum[ChairID].Add(bCard);
	}
}


int Table_changtaimajiang::GetListeningInfo_CheckJin(_uint8 ChairID, tagListenInfos stListenInfo[14], _uint8& bListenLen, HandCards &pHc)
{
	HandCards &tempHandCard = pHc;

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
		Check_Listening_CheckYouJin(&tempHandCard, ChairID, bCard, stListenInfo, bListenLen);
	}

	return (bListenLen != 0 ? TYPE_LISTEN : TYPE_NULL);
}

bool Table_changtaimajiang::CheckUserOutCard(_uint8 ChairID, _uint8 cbCard)
{
	//客户端中的金在服务器上保持的是白板
	//客户端的白板在服务器上保持的是金牌。如果打出的牌不替换，造成出牌错误。

	if (m_bJinCard.Size()==1)
	{
		if (cbCard == m_bJinCard[0])
		{
			cbCard = m_bReplaceJinBaiBanCard;
		}
		else if (cbCard == m_bReplaceJinBaiBanCard)
		{
			cbCard = m_bJinCard[0];
		}
	}

	return Table::CheckUserOutCard(ChairID, cbCard);
}

int Table_changtaimajiang::OutCardEstimate(_uint8 ChairID, _uint8 bCard, MJ_major& major, int& fan_num)
{

	if (m_bJinCard.Find(bCard))
	{
		//别人打出的金牌，自己不能进行吃碰杠胡牌
		return TYPE_NULL;
	}
	if (m_bJinCard[0] < 0x51)
	{
		if (bCard == m_bReplaceJinBaiBanCard) //进行替换
		{
			bCard = m_bJinCard[0];
		}
	}
	assert(major.m_oFan.Size() == 0);
	int iOpType = TYPE_NULL;
	HandCards* pHc = &m_HandCard[ChairID];
	iOpType |= GetChi(ChairID, bCard, major.m_Chi);
	if (m_bCurChairID != PreChair(ChairID))
	{//只能下家吃上家的牌
		iOpType &= ~TYPE_CHI;
	}
	iOpType |= GetPengGang(ChairID, bCard, major.m_GetPengGang);
	//log_debug("iOpType = %#x", iOpType);

	//assert(pHc->ChangeableCardsLen < HAND_CARD_SIZE_MAX);
	if (m_bUserOperateDoubleYouJinFlag[ChairID] == true)
	{
		return iOpType;
	}
	for (int i = 0; i < m_GAME_PLAYER; i++)
	{

		if (m_bUserOperateDoubleYouJinFlag[i] == true)
		{
			return iOpType;
		}
		if (m_bUserOperateThreeYouJinFlag[i] == true)
		{
			return iOpType;
		}
	}
	if (!m_conf.m_bZimoHu)
	{
		m_GameLogic.AddCard(pHc, bCard);
		iOpType |= Check_Hu(ChairID, *pHc, major.m_oFan);
		//log_debug("iOpType = %#x", iOpType);

		if (m_bUserOperateGuoZhangHuFlag[ChairID] == true)
		{//过张胡
			iOpType &= ~TYPE_HU;
		}

		if ((m_bUserOperateDanYouJinFlag[ChairID]==true) && (iOpType&TYPE_HU))
		{
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

	if (iOpType&TYPE_HU)
	{
		int bUserHasJinNum = 0;
		for (int i = 0; i < pHc->ChangeableCardsLen; i++)
		{//每次都检测手牌中金的个数，用于听牌的时候，能够听全部的牌

			if (m_bJinCard.Size() == 1)
			{
				if (pHc->ChangeableCards[i] == m_bReplaceJinBaiBanCard)
				{
					bUserHasJinNum++;
				}
			}
			else
			{
				if (m_bJinCard.Find(pHc->ChangeableCards[i]))
				{
					bUserHasJinNum++;
				}
			}
		}

	}
	return iOpType;
}

int Table_changtaimajiang::GetPengGang(_uint8 ChairID, _uint8 cbCard, MJ_opinfo& GangCardInfo)
{
	int iOpType = TYPE_NULL;
	iOpType = m_GameLogic.GetPengGang(&m_HandCard[ChairID], cbCard, GangCardInfo);

	if (m_bUserOperatePengCardInfo[ChairID].Find(OpState(cbCard, TYPE_PENG)))
	{

		//m_bUserOperatePengCardInfo[ChairID].Clear();
		GangCardInfo.Remove(OpState(cbCard, TYPE_PENG));
		iOpType &= ~(TYPE_PENG);
		//log_info("ChairID = %d iOpType = %x", ChairID, iOpType);

	}
	if (iOpType & TYPE_PENG)
	{
		m_bUserOperatePengCardInfo[ChairID].Add(OpState(cbCard, TYPE_PENG));
	}

	return iOpType;
}


void Table_changtaimajiang::UserQiangGangHu(_uint8 ChairID, _uint8 bCard, MJ_fan_buffer& oFan)
{
	
	oFan.Remove(MJ_FAN_TYPE_PINGHU);
	oFan.Add(MJ_FAN_TYPE_QIANGGANGHU);
	
}

void Table_changtaimajiang::DoMultiOperate(_uint8 ChairID, int iOpType, _uint8 bCard)
{
	if (M_bUserDoMultiOperate[ChairID]==true)
	{
		return;
	}
	M_bUserDoMultiOperate[ChairID] = true;
	if (m_bMultiOpHuNum > 1)
	{//如果优先级高的玩家选择胡牌，不必等待优先级低的玩家的操作。

		if (iOpType == TYPE_HU && m_bUserOperate[ChairID] & TYPE_HU)
		{
			for (_uint8 c = NextChair(ChairID); c != m_bOutCardChairID; c = NextChair(c))
			{//把比自己优先级低的玩家直接pass
				if (m_bUserOperate[c] & TYPE_HU && M_bUserDoMultiOperate[c] == false)
				{
					M_bUserDoMultiOperate[c] = true;
					Table::DoMultiOperate(ChairID, TYPE_PASS, bCard);
					log_info("%s tid:%d uid:%d c:%d, curoptype:0x%x,now done OpNum:%d, totalOpnum:%d \n", __FUNCTION__, tid,
						GetPlayerUid(c), c, m_bUserOperate[c], m_bMultiDoneOpNum, m_bMultiOpUserNum);

				}
			}
		}
	}
	Table::DoMultiOperate(ChairID, iOpType, bCard);
}

int Table_changtaimajiang::GetGangInfo(_uint8 ChairID, MJ_opinfo& GangCardInfo)
{
	return GetGangInfo(&m_HandCard[ChairID], GangCardInfo);
}

int Table_changtaimajiang::GetGangInfo(const HandCards*pHc, MJ_opinfo& ganginfo)
{
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

			if (pHc->ChangeableCards[i] == m_bReplaceJinBaiBanCard)
			{	 //在服务器手牌中白板代表金牌。
				continue;
			}

			if (pHc->ChangeableCards[i] == m_bJinCard[0])
			{//如果是金牌则代表白板可以暗杠
				ganginfo.Add(OpState(m_bReplaceJinBaiBanCard, TYPE_ANGANG));
				iOpType |= TYPE_ANGANG;
			}
			else
			{
				ganginfo.Add(OpState(pHc->ChangeableCards[i], TYPE_ANGANG));
				iOpType |= TYPE_ANGANG;
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
					if (pHc->ChangeableCards[j] == m_bJinCard[0])
					{
						ganginfo.Add(OpState(m_bReplaceJinBaiBanCard, TYPE_WANGANG));
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
	return iOpType;
}

void Table_changtaimajiang::UserOperateChi(_uint8 ChairID, int iOpType, _uint8 bCard)
{
	_uint8 bSavebCard = bCard;
	if (bCard == m_bReplaceJinBaiBanCard)
	{
		bSavebCard = m_bJinCard[0];
	}
	memset(m_bAddFanFlag, 0, sizeof(m_bAddFanFlag));
	memset(m_bUserOperate, 0, sizeof(m_bUserOperate));

	MJ_opinfo stChi;
	switch (iOpType)
	{
	case TYPE_LEFT_CHI:
	{
		//吃牌检查
		if (!(GetChi(ChairID, bSavebCard, stChi) & TYPE_LEFT_CHI))
		{
			log_info("user_operate_chi ChairID:%d operate:0x%x，card: 0x%02x,but handcard has no cur operate \n", ChairID, iOpType, bCard);
			return;
		}
		m_GameLogic.ExecuteLeftChi(&m_HandCard[ChairID], m_bOutCardChairID, bSavebCard);
	}
	break;
	case TYPE_CENTER_CHI:
	{
		//吃牌检查
		if (!(GetChi(ChairID, bSavebCard, stChi) & TYPE_CENTER_CHI))
		{
			log_info("user_operate_chi ChairID:%d operate:0x%x，card: 0x%02x,but handcard has no cur operate \n", ChairID, iOpType, bCard);
			return;
		}
		m_GameLogic.ExecuteCenterChi(&m_HandCard[ChairID], m_bOutCardChairID, bSavebCard);
	}
	break;
	case TYPE_RIGHT_CHI:
	{
		//吃牌检查
		if (!(GetChi(ChairID, bSavebCard, stChi) & TYPE_RIGHT_CHI))
		{
			log_info("user_operate_chi ChairID:%d operate:0x%x，card: 0x%02x,but handcard has no cur operate \n", ChairID, iOpType, bCard);
			return;
		}
		m_GameLogic.ExecuteRightChi(&m_HandCard[ChairID], m_bOutCardChairID, bSavebCard);
	}
	break;
	default:
		break;
	}
	m_bGetCardCount[ChairID]++;

	m_stUserOutCard[m_bOutCardChairID].RemoveLast(bCard);

	//广播操作结果的消息
	BroadcastOperateResult(ChairID, iOpType, bCard);
	log_info("%s tid:%d uid:%d ChairID:%d, bCard:0x%02x \n", __FUNCTION__,tid, GetPlayerUid(ChairID), ChairID, bCard);

	SendTingInfo(ChairID);

	int score[GAME_PLAYER] = { 0 };
	save_video_operate(iOpType, ChairID, m_bOutCardChairID, bCard, score);

	UserAfterOperate(ChairID, iOpType, bCard);

}

void Table_changtaimajiang::UserOperatePeng(_uint8 ChairID, _uint8 bCard)
{
	_uint8 bSavebCard = bCard;
	if (bCard == m_bReplaceJinBaiBanCard)
	{
		bSavebCard = m_bJinCard[0];
	}

	log_info("%s tid:%d uid:%d ChairID:%d, bCard:0x%02x \n", __FUNCTION__,tid, GetPlayerUid(ChairID), ChairID, bCard);
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
	m_GameLogic.ExecutePeng(&m_HandCard[ChairID], m_bOutCardChairID, bSavebCard);
	m_bGetCardCount[ChairID]++;

	//从打出牌删掉这张牌	
	m_stUserOutCard[m_bOutCardChairID].RemoveLast(bCard);
	//TODO 检查是否听牌
	SendTingInfo(ChairID);

	UserAfterOperate(ChairID, TYPE_PENG, bCard);

	int score[GAME_PLAYER] = { 0 };
	save_video_operate(TYPE_PENG, ChairID, m_bOutCardChairID, bCard, score);
}

void Table_changtaimajiang::UserOperateGang(_uint8 ChairID, int iOpType, _uint8 bCard)
{
	m_bUserOperateDanYouJinFlag[ChairID] = false;
	m_bUserOperateDoubleYouJinFlag[ChairID] = false;
	m_bUserOperateThreeYouJinFlag[ChairID] = false;

	_uint8 bSavebCard = bCard;
	if (bCard == m_bReplaceJinBaiBanCard)
	{
		bSavebCard = m_bJinCard[0];
	}


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


	m_GameLogic.ExecuteGang(&m_HandCard[ChairID], bTempChairID, bSavebCard, iOpType);

	Calc_Score(m_stGang, iGangScore);

	proto::game::AckBcOpResult stOpresult;
	int SaveCard = INVALID_MJ_CARD;
	if (iOpType == TYPE_ANGANG)
	{
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
	m_bUserOperate[ChairID] = TYPE_NULL;
	stOpresult.set_card(bCard);
	SaveVideoData(SERVER_OPERATE_RESULT, stOpresult);
}

void Table_changtaimajiang::TurnBanker()
{

	MJ_user_buffer UserID;
	MultiHu(UserID);
	if (UserID.Size() == 0)
	{//后续流局或庄家胡牌时连庄，庄家继续当庄
		m_bBanker = m_bBanker;
		m_bLianZhuangNum++;
	}
	else
	{
		if (m_bBanker == UserID[0])
		{
			m_bBanker = m_bBanker;
			m_bLianZhuangNum++;
		}
		else
		{
			m_bLianZhuangNum = 0;
			m_bBanker = NextChair(m_bBanker);
		}
	}
	//log_info("m_bBanker = %d  m_bLianZhuangNum = %d", m_bBanker, m_bLianZhuangNum)
}

int Table_changtaimajiang::Check_Hu(_uint8 ChairID, HandCards& pHc, MJ_fan_buffer& oFan)
{
	if (pHc.ChangeableCardsLen > 17)
	{
		return TYPE_NULL;
	}
	//取余3不等于2的手牌不做胡牌判断
	if (pHc.ChangeableCardsLen != 0 && pHc.ChangeableCardsLen % 3 != 2)
	{
		return TYPE_NULL;
	}
	
	int bUserHasJinNum = 0;
	HandCards tmp;
	for (int i = 0; i < pHc.ChangeableCardsLen; i++)
	{//每次都检测手牌中金的个数，用于听牌的时候，能够听全部的牌
		if (m_bJinCard.Size()==1)
		{
			if (pHc.ChangeableCards[i] == m_bReplaceJinBaiBanCard)
			{
				bUserHasJinNum++;
			}
			else
			{
				m_GameLogic.AddCard(&tmp, pHc.ChangeableCards[i]);
			}
		}
		else
		{
			if (m_bJinCard.Find(pHc.ChangeableCards[i]))
			{
				bUserHasJinNum++;
			}
			else
			{
				m_GameLogic.AddCard(&tmp, pHc.ChangeableCards[i]);
			}
		}
	}
	MJ_win_pattern pattern;
	/*格式化牌*/
	if (m_GameLogic.FormatCards_LaiZi(&tmp, pattern, bUserHasJinNum))/*递归，检查胡牌的形式*/
	{	
		return Check_Hu_Pattern(ChairID, pHc, oFan, pattern);
	}
	return TYPE_NULL;
}

int Table_changtaimajiang::Check_Hu_Pattern(_uint8 ChairID, const HandCards& pHc, MJ_fan_buffer& oFan, const MJ_win_pattern& oPattern)
{
	oFan.Add(MJ_FAN_TYPE_PINGHU);
	return TYPE_HU;
}

void Table_changtaimajiang::GetAllHandCardRelativity(HandCards *pHc, MJ_BUFFER<_uint8, POOL_CARD_TYPE_MAX>& _relativity)
{
	_uint8 last_check = INVALID_MJ_CARD;
	GetHandCardRelativity(last_check, _relativity);
}

void Table_changtaimajiang::GetHandCardRelativity(_uint8 bCard, MJ_BUFFER<_uint8, POOL_CARD_TYPE_MAX>& _relativity)
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
	for (int i = 0x41; i <= 0x42; i++)
	{
		_relativity.Add(i);
	}

}

void Table_changtaimajiang::Check_Listening(HandCards *pHc, _uint8 ChairID, _uint8 bCard, tagListenInfos stListenInfo[17], _uint8 &bListenLen)
{
	if (bListenLen >= 14)
	{
		log_error("%s chaochu zuida ting pai shu!\n", __FUNCTION__);
		return;
	}

	int bUserHasJinNum = 0;
	for (int i = 0; i < pHc->ChangeableCardsLen; i++)
	{//每次都检测手牌中金的个数，用于听牌的时候，能够听全部的牌

		if (m_bJinCard.Size() == 1)
		{
			if (pHc->ChangeableCards[i] == m_bReplaceJinBaiBanCard)
			{
				bUserHasJinNum++;
			}
		}
		else
		{
			if (m_bJinCard.Find(pHc->ChangeableCards[i]))
			{
				bUserHasJinNum++;
			}
		}
	}

	HandCards &tempHandCard = *pHc;

	m_GameLogic.ChuPai(&tempHandCard, bCard);
	bool bTrue = false;

	MJ_BUFFER<_uint8, POOL_CARD_TYPE_MAX> _relativity;
	GetAllHandCardRelativity(&tempHandCard, _relativity);

	for (_uint8 i = 0; i < _relativity.Size(); i++)
	{		//保存可能要打出的牌与需要的牌。
		_uint8 SavebCard = bCard;
		_uint8 SaveRelativity = _relativity[i];
		m_GameLogic.AddCard(&tempHandCard, _relativity[i]);
		MJ_fan_buffer fan_buffer;
		if (Check_Hu(ChairID, tempHandCard, fan_buffer) == TYPE_HU)
		{
			if (stListenInfo[bListenLen].bHuLen < 35)
			{
				if (m_bJinCard.Find(bCard))
				{//如果需要打出金牌，则客户端操作的应该是白板，反馈给客户端。
					SavebCard = m_bReplaceJinBaiBanCard;

				}else if (SavebCard == m_bReplaceJinBaiBanCard)
				{//如果需要打出白板，则客户端操作的应该打出的是金牌。
					SavebCard = m_bJinCard[0];
				}

				if (m_bJinCard.Find(_relativity[i]))
				{//如果听得牌是金，则反馈给客户端的是白板
					SaveRelativity = m_bReplaceJinBaiBanCard;
				}

				stListenInfo[bListenLen].bOutCard = SavebCard;//要打出的牌       
				stListenInfo[bListenLen].HuInfo[stListenInfo[bListenLen].bHuLen].bCard = SaveRelativity;//听的牌
				stListenInfo[bListenLen].HuInfo[stListenInfo[bListenLen].bHuLen].bFanShu = Calc_Fan(ChairID, fan_buffer);
				bTrue = true;
				stListenInfo[bListenLen].bHuLen++;

				//log_info("ChairID = %d SavebCard = %x SaveRelativity = %x stListenInfo[bListenLen].bHuLen =%d", ChairID, SavebCard, SaveRelativity, stListenInfo[bListenLen].bHuLen);
			}
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
void Table_changtaimajiang::Check_Listening_CheckYouJin(HandCards *pHc, _uint8 ChairID, _uint8 bCard, tagListenInfos stListenInfo[17], _uint8 &bListenLen)
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
	{		//保存可能要打出的牌与需要的牌。
		_uint8 SavebCard = bCard;
		_uint8 SaveRelativity = _relativity[i];
		m_GameLogic.AddCard(&tempHandCard, _relativity[i]);
		MJ_fan_buffer fan_buffer;
		if (Check_Hu(ChairID, tempHandCard, fan_buffer) == TYPE_HU)
		{
			if (stListenInfo[bListenLen].bHuLen < 35)
			{
				if (m_bJinCard.Find(bCard)&& m_bJinCard.Size()==1)
				{//如果需要打出金牌，则客户端操作的应该是白板，反馈给客户端。
					SavebCard = m_bReplaceJinBaiBanCard;

				}
				else if (SavebCard == m_bReplaceJinBaiBanCard)
				{//如果需要打出白板，则客户端操作的应该打出的是金牌。
					SavebCard = m_bJinCard[0];
				}

				if (m_bJinCard.Find(_relativity[i]))
				{//如果听得牌是金，则反馈给客户端的是白板
					SaveRelativity = m_bReplaceJinBaiBanCard;
				}

				stListenInfo[bListenLen].bOutCard = SavebCard;//要打出的牌       
				stListenInfo[bListenLen].HuInfo[stListenInfo[bListenLen].bHuLen].bCard = SaveRelativity;//听的牌
				stListenInfo[bListenLen].HuInfo[stListenInfo[bListenLen].bHuLen].bFanShu = Calc_Fan(ChairID, fan_buffer);
				bTrue = true;
				stListenInfo[bListenLen].bHuLen++;

				//log_info("ChairID = %d SavebCard = %x SaveRelativity = %x stListenInfo[bListenLen].bHuLen =%d", ChairID, SavebCard, SaveRelativity, stListenInfo[bListenLen].bHuLen);
			}
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

void Table_changtaimajiang::DoSimpleOperate(_uint8 ChairID, int iOpType, _uint8 bCard)
{
	log_info("DoSimpleOperate tid:%d uid:%d chairid:%d, iOptype:0x%x bCard:%x\n",tid, GetPlayerUid(ChairID), ChairID, iOpType, bCard);

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
		
		if (m_bCurChairID == ChairID)
		{
			//开启出牌时钟
			set_out_timer(ChairID);
			if (m_bUserRogJinHuFlag[ChairID]==true)
			{
				//log_info("m_bUserRogJinHuFlag[ChairID] = %d", m_bUserRogJinHuFlag[ChairID]);
				m_bUserRogJinHuPassFlag[ChairID] = true;
				m_bUserRogJinHuFlag[ChairID] = false;
				OnUserGetCard(ChairID);
			}
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
		if (m_bUserOperateThreeYouJinFlag[ChairID] == true&& m_bCurChairID == ChairID)
		{
			UserOperateHuYouJin(ChairID);
			GameEnd();
			break;
		}else if (m_bUserOperateDoubleYouJinFlag[ChairID] == true && m_bCurChairID == ChairID)
		{
			UserOperateHuYouJin(ChairID);
			GameEnd();
			break;
		}
		else if (m_bUserOperateDanYouJinFlag[ChairID] == true && m_bCurChairID == ChairID)
		{
			UserOperateHuYouJin(ChairID);
			GameEnd();
			break;
		}

		UserOperateHu(ChairID);

		GameEnd();
		break;

	case  TYPE_YUNXIAO_YOUJIN:
		{
			m_bUserOperate[ChairID] = TYPE_NULL;
			//广播发送操作结果	
			proto::game::AckBcOpResult stOpresult;
			stOpresult.set_chairid(ChairID);
			stOpresult.set_outcardchairid(m_bCurChairID);
			stOpresult.set_optype(TYPE_YUNXIAO_YOUJIN);
			stOpresult.set_card(bCard);
			unicast(SERVER_OPERATE_RESULT, ChairID, stOpresult);
			break;
		}

			
	case TYPE_YUNXIAO_YOUJIN_TWO:
		{			
			m_bUserOperate[ChairID] = TYPE_NULL;
			//广播发送操作结果	
			proto::game::AckBcOpResult stOpresult;
			stOpresult.set_chairid(ChairID);
			stOpresult.set_outcardchairid(m_bCurChairID);
			stOpresult.set_optype(TYPE_YUNXIAO_YOUJIN_TWO);
			stOpresult.set_card(bCard);
			unicast(SERVER_OPERATE_RESULT, ChairID, stOpresult);
			break; 
		}

	case TYPE_YUNXIAO_YOUJIN_THREE:
		{
			m_bUserOperate[ChairID] = TYPE_NULL;
			//广播发送操作结果	
			proto::game::AckBcOpResult stOpresult;
			stOpresult.set_chairid(ChairID);
			stOpresult.set_outcardchairid(m_bCurChairID);
			stOpresult.set_optype(TYPE_YUNXIAO_YOUJIN_THREE);
			stOpresult.set_card(bCard);
			unicast(SERVER_OPERATE_RESULT, ChairID, stOpresult);
			break;
		}

	default:
		break;
	}
}

void Table_changtaimajiang::UserOperateHuYouJin(_uint8 ChairID, _uint8 bHuCard)
{
	log_info("%s tid:%d uid:%d ChairID:%d hu FanNum:%d bHuCard:%x\n", __FUNCTION__,tid, GetPlayerUid(ChairID), ChairID, m_HuFanNums[ChairID], bHuCard);
	_uint8 bCard = 0xFF;
	if (bHuCard == 0xFF)
	{
		bCard = m_bCurCard;
	}
	else
	{
		bCard = bHuCard;
	}
	//广播发送操作结果	
	proto::game::AckBcOpResult stOpresult;
	stOpresult.set_chairid(ChairID);
	stOpresult.set_outcardchairid(m_bCurChairID);
	if (m_bUserOperateDanYouJinFlag[ChairID]==true)
	{
		stOpresult.set_optype(TYPE_YUNXIAO_YOUJIN);

	}else if (m_bUserOperateDoubleYouJinFlag[ChairID] == true)
	{
		stOpresult.set_optype(TYPE_YUNXIAO_YOUJIN_TWO);

	}else if (m_bUserOperateThreeYouJinFlag[ChairID] == true)
	{
		stOpresult.set_optype(TYPE_YUNXIAO_YOUJIN_THREE);
	}

	stOpresult.set_card(bCard);

	broadcast(SERVER_OPERATE_RESULT, stOpresult);

	m_bIsHu[ChairID] = true;
	int userscore[GAME_PLAYER] = { 0 };
	Calc_Hu(ChairID, bCard, userscore);

	Calc_Score(m_stHuScore, userscore);

	for (int i = 0; i < GAME_PLAYER; i++)
	{
		log_debug("tid:%d userscore[i] = %d",tid, userscore[i]);
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

void Table_changtaimajiang::UserOperateHu(_uint8 ChairID, _uint8 bHuCard)
{
	m_bUserOperateDanYouJinFlag[ChairID] = false;
	m_bUserOperateDoubleYouJinFlag[ChairID] = false;
	m_bUserOperateThreeYouJinFlag[ChairID] = false;

	Table::UserOperateHu(ChairID, bHuCard);
}

void Table_changtaimajiang::Calc_HuScore(_uint8 ChairID, int iBaseScore[GAME_PLAYER], int iScore[GAME_PLAYER])
{
	if (m_bIsHu[ChairID])
	{
		for (_uint8 c = NextChair(ChairID); c != ChairID; c = NextChair(c))
		{
			iScore[ChairID] -= iBaseScore[c];
			iScore[c] = iBaseScore[c];
		}
		log_info("%s tid:%d uid:%d chairid:%d, base score [%d,%d,%d,%d]\n", __FUNCTION__,tid, GetPlayerUid(ChairID), ChairID, iScore[0], iScore[1], iScore[2], iScore[3]);
	}
}

void Table_changtaimajiang::Calc_BaseScore(_uint8 ChairID, int iBaseScore[GAME_PLAYER])
{
	if (!m_bIsHu[ChairID])
		return;
	int HuFanNums = Calc_Fan(ChairID, m_oFan[ChairID]);
	log_info("tid:%d Calc_BaseScore ChairID = %d HuFanNums = %d",tid, ChairID, HuFanNums);
	if (ChairID ==m_bCurChairID && m_oFan[ChairID].Find(MJ_FAN_TYPE_PINGHU))
	{
		HuFanNums *= 2;
	}

	int HuFanShui = 0;

	log_info("tid:%d Calc_BaseScore ChairID = %d m_bZhuangBaseScore = %d", tid, ChairID, m_bZhuangBaseScore);
	log_info("tid:%d Calc_BaseScore ChairID = %d m_bXianBaseScore = %d", tid, ChairID, m_bXianBaseScore);
	log_info("tid:%d Calc_BaseScore ChairID = %d m_bLianZhuangNum = %d", tid, ChairID, m_bLianZhuangNum);

	if (m_conf_changtaimajiang.mj_conf_changtai_lianzhuang==false)
	{
		m_bLianZhuangNum = 0;
	}

	if (m_bUserOutHuaNum[ChairID].Size() >=4)
	{
		HuFanShui += (m_bUserOutHuaNum[ChairID].Size()-3);
	}

	log_info("tid:%d Calc_BaseScore HuFanShui = %d",tid, HuFanShui);

	if (ChairID == m_bBanker)
	{
		for (_uint8 c = NextChair(ChairID); c != ChairID; c = NextChair(c))
		{
			iBaseScore[ChairID] += HuFanNums * (m_bZhuangBaseScore + m_bLianZhuangNum) + HuFanShui;
			iBaseScore[c] -= HuFanNums * (m_bZhuangBaseScore + m_bLianZhuangNum) + HuFanShui;
			log_info("tid:%d Calc_BaseScore iBaseScore[ChairID = %d]  = %d ", tid, ChairID, iBaseScore[ChairID]);
			log_info("tid:%d Calc_BaseScore iBaseScore[c = %d] = %d ", tid, c, iBaseScore[c]);
		}
	}
	else
	{
		for (_uint8 c = NextChair(ChairID); c != ChairID; c = NextChair(c))
		{
			if (c == m_bBanker)
			{
				iBaseScore[ChairID] += HuFanNums * (m_bZhuangBaseScore + m_bLianZhuangNum) + HuFanShui;
				iBaseScore[c] -= HuFanNums * (m_bZhuangBaseScore + m_bLianZhuangNum) + HuFanShui;
				log_info("tid:%d Calc_BaseScore iBaseScore[ChairID = %d]  = %d ", tid, ChairID, iBaseScore[ChairID]);
				log_info("tid:%d Calc_BaseScore iBaseScore[c = %d] = %d ", tid, c, iBaseScore[c]);
			}
			else
			{
				iBaseScore[ChairID] += HuFanNums * (m_bXianBaseScore + m_bLianZhuangNum) + HuFanShui;
				iBaseScore[c] -= HuFanNums * (m_bXianBaseScore + m_bLianZhuangNum) + HuFanShui;
				log_info("tid:%d Calc_BaseScore iBaseScore[ChairID = %d]  = %d ", tid, ChairID, iBaseScore[ChairID]);
				log_info("tid:%d Calc_BaseScore iBaseScore[c = %d] = %d ", tid, c, iBaseScore[c]);
			}
		}
	}


	if (ChairID != m_bCurChairID&&m_CardPool.Count() < 20)
	{
		_uint8 bChairID = 0;
		if (m_bRobHu)
		{
			bChairID = m_WangGangPlayer;
		}
		else
		{
			bChairID = m_bOutCardChairID;

		}
		for (_uint8 c = NextChair(ChairID); c != ChairID; c = NextChair(c))
		{
			iBaseScore[c] = 0;
			log_info("tid:%d Calc_BaseScore iBaseScore[ChairID = %d]  = %d ", tid, ChairID, iBaseScore[ChairID]);
			log_info("tid:%d Calc_BaseScore iBaseScore[c = %d] = %d ", tid, c, iBaseScore[c]);
		}
		iBaseScore[bChairID] = -iBaseScore[ChairID];
		log_info("tid:%d Calc_BaseScore iBaseScore[ChairID = %d]  = %d ", tid, ChairID, iBaseScore[ChairID]);
		log_info("tid:%d Calc_BaseScore iBaseScore[bChairID = %d] = %d ", tid, bChairID, iBaseScore[bChairID]);
	}
}




void Table_changtaimajiang::Calc_GangScore()
{
	for (int i = 0; i < m_GAME_PLAYER; i++)
	{
		m_stGang[i].iTotalScore = 0;
	}
	MJ_user_buffer UserID;
	MultiHu(UserID);
	_uint8 HuChairID = UserID[0];
	for (int i = 0; i < m_GAME_PLAYER; i++)
	{
		if (HuChairID == i)
		{
			for (int j = 0; j < m_stGang[i].Gang.Size(); j++)
			{
				int ibasescore = 0;
				if (m_stGang[i].Gang[j].iType== TYPE_ANGANG)
				{
					ibasescore = 2;
				} 
				else
				{
					ibasescore = 1;
				}
				for (int c = NextChair(i); c != i; c = NextChair(c))
				{

					m_stGang[i].iTotalScore += ibasescore;
					m_stGang[c].iTotalScore -= ibasescore;
				}
			}
		}
		else
		{
			for (int j = 0; j < m_stGang[i].Gang.Size(); j++)
			{
				int ibasescore = 0;
				if (m_stGang[i].Gang[j].iType == TYPE_ANGANG)
				{
					ibasescore = 2;
				}
				else
				{
					ibasescore = 1;
				}
				for (int c = NextChair(i); c != i; c = NextChair(c))
				{
					if (c == HuChairID)
					{
						continue;
					}
					m_stGang[i].iTotalScore += ibasescore;
					m_stGang[c].iTotalScore -= ibasescore;
				}
			}
		}
	}
}

bool Table_changtaimajiang::IsDoubleYouJin(HandCards& pHc, _uint8 ChairID)
{
	int bUserHasJinNum = 0;
	HandCards tmp;
	MJ_hand_buffer PaiCard[MJ_TYPE_JIAN];  //各种花式的牌数据
	memset(PaiCard, 0, sizeof(PaiCard));
	for (int i = 0; i < pHc.ChangeableCardsLen; i++)
	{//每次都检测手牌中金的个数，用于听牌的时候，能够听全部的牌
		if (m_bJinCard.Size() == 1)
		{
			if (pHc.ChangeableCards[i] == m_bReplaceJinBaiBanCard)
			{
				bUserHasJinNum++;
			}
			else
			{
				m_GameLogic.AddCard(&tmp, pHc.ChangeableCards[i]);
			}
		}
		else
		{
			if (m_bJinCard.Find(pHc.ChangeableCards[i]))
			{
				bUserHasJinNum++;
			}
			else
			{
				m_GameLogic.AddCard(&tmp, pHc.ChangeableCards[i]);
			}
		}
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
	if (bUserHasJinNum - needHunNum >= 2)
	{
		return true;
	}
	return false;

}

bool Table_changtaimajiang::IsThreeYouJin(HandCards& pHc, _uint8 ChairID)
{
	if (m_bUserOperateDoubleYouJinFlag[ChairID] == false)
	{
		return false;
	}	

	int bUserHasJinNum = 0;
	HandCards tmp;
	MJ_hand_buffer PaiCard[MJ_TYPE_JIAN];  //各种花式的牌数据
	memset(PaiCard, 0, sizeof(PaiCard));
	for (int i = 0; i < pHc.ChangeableCardsLen; i++)
	{//每次都检测手牌中金的个数，用于听牌的时候，能够听全部的牌
		if (m_bJinCard.Size() == 1)
		{
			if (pHc.ChangeableCards[i] == m_bReplaceJinBaiBanCard)
			{
				bUserHasJinNum++;
			}
			else
			{
				m_GameLogic.AddCard(&tmp, pHc.ChangeableCards[i]);
			}
		}
		else
		{
			if (m_bJinCard.Find(pHc.ChangeableCards[i]))
			{
				bUserHasJinNum++;
			}
			else
			{
				m_GameLogic.AddCard(&tmp, pHc.ChangeableCards[i]);
			}
		}
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
//	log_info("bUserHasJinNum = %d", bUserHasJinNum);
//	log_info("needHunNum = %d", needHunNum);

	if (bUserHasJinNum - needHunNum >= 2)
	{
		return true;
	}

	return false;

}

bool Table_changtaimajiang::EstimateUserRespond(_uint8 bOutChairID, _uint8 bOutCard)
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
		m_bUserOperate[i] = OutCardEstimate(i, bOutCard, major, m_HuFanNums[i]);

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
			}
			//保存获取牌之后的操作提示录像
			if (m_bUserOperate[i] >0)
			{
				proto::game::AckOpNotify stVideo;
				stVideo.set_chairid(i);
				stVideo.set_optype(m_bUserOperate[i]);
				std::string str = stVideo.SerializeAsString();
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

void Table_changtaimajiang::RobGangPriority(_uint8 ChairID, _uint8 CardData, int OperateType)
{
	//log_info("ChairID = %d CardData= %x OperateType = %x", ChairID, CardData, OperateType);
	if (M_bUserDoMultiOperateQiangGangHu[ChairID] == true)
	{
		return;
	}
	M_bUserDoMultiOperateQiangGangHu[ChairID] = true;

	//log_info("M_bUserDoMultiOperateQiangGangHu[ChairID = %d] = %x", ChairID, M_bUserDoMultiOperateQiangGangHu[ChairID]);
	if (m_RobHuNum > 1)
	{//如果优先级高的玩家选择胡牌，不必等待优先级低的玩家的操作。

		if (OperateType == TYPE_HU && m_bUserOperate[ChairID] & TYPE_HU)
		{
		//	log_info("M_bUserDoMultiOperateQiangGangHu[ChairID = %d] = %x", ChairID, M_bUserDoMultiOperateQiangGangHu[ChairID]);

			for (_uint8 c = NextChair(ChairID); c != m_WangGangPlayer; c = NextChair(c))
			{//把比自己优先级低的玩家直接pass
			//	log_info("M_bUserDoMultiOperateQiangGangHu[ChairID = %d] = %x", ChairID, M_bUserDoMultiOperateQiangGangHu[ChairID]);

				if (m_bUserOperate[c] & TYPE_HU && M_bUserDoMultiOperateQiangGangHu[c] == false)
				{
					//log_info("M_bUserDoMultiOperateQiangGangHu[ChairID = %d] = %x", ChairID, M_bUserDoMultiOperateQiangGangHu[ChairID]);

					M_bUserDoMultiOperateQiangGangHu[c] = true;
					Table::RobGangPriority( c, CardData, TYPE_PASS);
				}
			}
		}
	}
	Table::RobGangPriority(ChairID, CardData, OperateType);
}


bool Table_changtaimajiang::RobGangHu(_uint8 ChairID, _uint8 CardData, int iOpType)
{
	memset(M_bUserDoMultiOperateQiangGangHu, 0, sizeof(M_bUserDoMultiOperateQiangGangHu));
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
			_uint8 bSaveCardData = CardData;
			if (m_bJinCard[0] < 0x51)
			{
				if (CardData == m_bReplaceJinBaiBanCard) //进行替换
				{
					bSaveCardData = m_bJinCard[0];
				}
			}
			const HandCards& hc = m_HandCard[ChairID];
			MJ_opinfo opinfo;

			if (iOpType & TYPE_WANGANG && m_GameLogic.GetWanGang(&hc, opinfo, bSaveCardData))
			{
				for (int c = 0; c < m_GAME_PLAYER; c++)
				{
					proto::game::AckOpNotify stNotify;
					Proto_Game_OpNotify(i, c, CardData, stNotify);
					unicast(SERVER_OPERATE_NOTIFY, c, stNotify);

				}
				cbNum++;
			}
			else if (iOpType & TYPE_ZHIGANG && m_GameLogic.GetZhiGang(&hc, opinfo, bSaveCardData))
			{
				for (int c = 0; c < m_GAME_PLAYER; c++)
				{
					proto::game::AckOpNotify stNotify;
					Proto_Game_OpNotify(i, c, CardData, stNotify);
					unicast(SERVER_OPERATE_NOTIFY, c, stNotify);

				}
				cbNum++;
			}
			else if (iOpType & TYPE_ANGANG && m_GameLogic.GetAnGang(&hc, opinfo, bSaveCardData))
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
	//开启操作时钟 TODO
	for (int i = 0; i < m_GAME_PLAYER; i++)
	{
		if (i == ChairID)
		{
			continue;
		}
		log_info("RobGangHu uid:%d chaird:%d out card, so that chairid:%d have operate:0x%x\n", GetPlayerUid(ChairID), ChairID, i, m_bUserOperate[i]);
		if (m_bUserOperate[i] & TYPE_HU)
		{
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

/*接收用户选择的操作*/
void Table_changtaimajiang::OnRecvUserOperate(_uint8 ChairID, int iOpType, int iCard)
{
	if (m_bUserOperate[ChairID] & TYPE_HU && iOpType != TYPE_HU)
	{
		m_bUserOperateGuoZhangHuFlag[ChairID] = true;
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

bool Table_changtaimajiang::RobJinHu(_uint8 ChairID)
{
	//log_info("m_bGetCardCount[m_bBanker = %d] = %d", m_bBanker, m_bGetCardCount[m_bBanker]);
	if (m_bGetCardCount[m_bBanker]!=1)
	{
		return false;
	}
	if (m_bUserRogJinHuPassFlag[ChairID]==true)
	{
		return false;
	}
	//log_info("m_bGetCardCount[ChairID = %d] = %d", ChairID, m_bGetCardCount[ChairID]);

	if (m_bGetCardCount[ChairID] > 0|| ChairID==m_bBanker)
	{
		return false;
	}
	//log_info("m_bGetCardCount[ChairID = %d] = %d", ChairID, m_bGetCardCount[ChairID]);

	memset(m_bUserOperate, 0, sizeof(m_bUserOperate));
	HandCards tempHandCard = m_HandCard[ChairID];

	MJ_BUFFER<_uint8, POOL_CARD_TYPE_MAX> _relativity;
	GetAllHandCardRelativity(&tempHandCard, _relativity);
	bool bRobJinHuFlag = false;
	for (_uint8 i = 0; i < _relativity.Size(); i++)
	{		//保存可能要打出的牌与需要的牌。
		m_GameLogic.AddCard(&tempHandCard, _relativity[i]);
		MJ_fan_buffer fan_buffer;
		if (Check_Hu(ChairID, tempHandCard, fan_buffer) == TYPE_HU)
		{
			bRobJinHuFlag = true;
		}
		m_GameLogic.ChuPai(&tempHandCard, _relativity[i]);
	}
	if (bRobJinHuFlag == true)
	{
		_uint8 bGetCard = 0xFF;
		m_bUserOperate[ChairID] = TYPE_HU;
		proto::game::AckUserGetCard stUsergetcard;
		for (int i = 0; i < m_GAME_PLAYER; i++)
		{
			if (m_SeatPlayer[i].player == NULL)
			{
				log_error("OnUserGetCard chairid:%d  error, one player is NULL\n", ChairID);
				continue;
			}

			Proto_Game_UserGetCard(i, ChairID, bGetCard, stUsergetcard);
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
				//log_info("OnUserGetCard stUsergetcard = %s", stUsergetcard.DebugString().c_str());

			}
		}
		return true;
	}
	return false;
}


void Table_changtaimajiang::IsYouJinCanOutCard(_uint8 ChairID, int &bYouJinFlag)
{
	m_bCanYouJinOutCardList[ChairID].Clear();

	HandCards tmp = m_HandCard[ChairID];
	tagListenInfos stListenInfo[17];
	_uint8 bListenLen = 0;
	GetListeningInfo_CheckJin(ChairID, stListenInfo, bListenLen, tmp);
	for (int i = 0; i < bListenLen; i++)
	{//如果打出一张牌之后听任意一张牌，则是游金状态。
		if (stListenInfo[i].bHuLen == 33)
		{
			//log_info("stListenInfo[i = %d].bOutCard = %x", i, stListenInfo[i].bOutCard);
			bYouJinFlag = 1;
			m_bCanYouJinOutCardList[ChairID].Add(stListenInfo[i].bOutCard);
		}
	}

	for (int i = 0; i < bListenLen; i++)
	{//如果打出一张牌之后听任意一张牌，则是游金状态。
		if (stListenInfo[i].bHuLen == 33)
		{
			//log_info("stListenInfo[i = %d].bOutCard = %x",i, stListenInfo[i].bOutCard);
			if (m_bJinCard.Find(stListenInfo[i].bOutCard))
			{
				//log_info("stListenInfo[i = %d].bOutCard = %x", i, stListenInfo[i].bOutCard);

				m_bCanYouJinOutCardList[ChairID].Clear();
				m_bCanYouJinOutCardList[ChairID].Add(stListenInfo[i].bOutCard);
				if (m_bUserOperateDoubleYouJinFlag[ChairID]==true)
				{
					bYouJinFlag = 3;
				}
				else
				{
					bYouJinFlag = 2;
				}
						
			}
		}
	}
}

void Table_changtaimajiang::Proto_Game_UserGetCard(_uint8 ChairID, _uint8 MoChairID, _uint8 bCard, proto::game::AckUserGetCard& stUsergetcard)
{
	//_uint8 tempChairid = m_SeatPlayer[ChairID].player->seatid;
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
				//log_info("玩家还没胡牌,此时暗杠牌有:0x%02x \n", stGang[i].cbCard);
				log_info("%s uid:%d chairID:%d angang:0x%02x\n", __FUNCTION__, GetPlayerUid(ChairID), ChairID, stGang[i].cbCard);
				proto::game::tagGangInfo *pstGang = stUsergetcard.add_ganginfo();
				pstGang->set_card(stGang[i].cbCard);
				pstGang->set_state(stGang[i].nState);
			}
		}
		//听牌判断 TODO
		tagListenInfos stListenInfo[17];
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

		if (m_bCanYouJinOutCardList[ChairID].Size()>0)
		{
			for (int i = 0; i < m_bCanYouJinOutCardList[ChairID].Size(); i++)
			{
				stUsergetcard.add_youjinoutcardlist(m_bCanYouJinOutCardList[ChairID][i]);
			}
		}
		//log_info("tid:%d ChairID = %d  stUsergetcard = %s",tid, ChairID, stUsergetcard.DebugString().c_str());
	}
}

void Table_changtaimajiang::UserAfterOperate(_uint8 ChairID, int iOpType, _uint8 bCard)
{
	m_bCurChairID = ChairID;
	log_info("%s uid:%d chairid:%d operate:0x%x card:0x%02x\n", __FUNCTION__, GetPlayerUid(ChairID), ChairID, iOpType, bCard);

	m_bUserOperate[ChairID] = TYPE_NULL;
	MJ_opinfo gang;
	m_bUserOperate[ChairID] = GetGangInfo(m_bCurChairID, gang);

	int bYouJinFlag = 0;
	IsYouJinCanOutCard(ChairID, bYouJinFlag);
	if (m_bCanYouJinOutCardList[ChairID].Size() > 0)
	{

		if (bYouJinFlag == 1)
		{
			m_bUserOperate[ChairID] |= TYPE_YUNXIAO_YOUJIN;
			m_bUserOperate[ChairID] |= TYPE_FORBID_PASS;
		}
		else if (bYouJinFlag == 2)
		{
			m_bUserOperate[ChairID] |= TYPE_YUNXIAO_YOUJIN_TWO;
			m_bUserOperate[ChairID] |= TYPE_FORBID_PASS;
		}
	
	}
	if (gang.Size() == 0&& m_bCanYouJinOutCardList[ChairID].Size()==0)
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

void Table_changtaimajiang::Proto_Game_OpNotifyAfterPengChi(_uint8 ChairID, _uint8 CurChairID, const MJ_opinfo& gang, proto::game::AckOpNotify& stNotify)
{
	stNotify.Clear();
	stNotify.set_chairid(ChairID);
	//非提示玩家,下面的协议字段为空
	if (CurChairID == ChairID)
	{
		stNotify.set_optype(m_bUserOperate[ChairID]);
		stNotify.set_card(0);
		for (int i = 0; i < gang.Size(); i++)
		{
			proto::game::tagGangInfo* ganginfo = stNotify.add_gang();
			ganginfo->set_card(gang[i].cbCard);
			ganginfo->set_state(gang[i].nState);
		}

		if (m_bCanYouJinOutCardList[ChairID].Size()>0)
		{
			for (int i = 0; i < m_bCanYouJinOutCardList[ChairID].Size(); i++)
			{
				stNotify.add_youjinoutcardlist(m_bCanYouJinOutCardList[ChairID][i]);
			}
		}
		log_info("tid:%d ChairID = %d  stNotify = %s",tid, ChairID, stNotify.DebugString().c_str());

	}
}
