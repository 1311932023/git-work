#ifndef __TABLE_CHANGTAIMAJIANG_H__
#define __TABLE_CHANGTAIMAJIANG_H__
#include "../table.h"
#include "le_majiang.h"
#include <vector>
#include <map>

struct mj_conf_changtaimajiang
{
	bool mj_conf_changtai_lianzhuang;
	mj_conf_changtaimajiang()
	{
		mj_conf_changtai_lianzhuang = false;
	}
};

class Table_changtaimajiang : public Table
{
public:
	Table_changtaimajiang();
	~Table_changtaimajiang();
public:
	void ConfInit_Remote(const Json::Value& val);//配置信息，牌数等。

	int Calc_Fan(_uint8 ChairID, MJ_fan_buffer& oFan);

	void GameStart();

	void GameEnd();

	void GameEndReset();

	void Proto_GameScene(_uint8 ChairID, Player* player, proto::game::AckGameScene& stGameScene);

	void Proto_GameEndInfo(const MJ_user_buffer& UserID, proto::game::AckGameEnd& stGameend);

	int Check_Hu(_uint8 ChairID, HandCards& pHc, MJ_fan_buffer& oFan);

	void GameStartAndGetJin();

	//如果金牌是花牌，则白板会进行补花，手中不会有白板，则不用进行金牌与白板的替换。
	//如果金牌是数牌或字牌，则进行金牌与白板的替换，此时涉及到白板的吃碰杠。
	void ChangeHandCardJinAndBaiBan();

	void OnUserGetCard(_uint8 ChairID, bool bGetLast = false);

	void OnUserOutCard(_uint8 ChairID, _uint8 bCard);

	bool CheckUserOutCard(_uint8 ChairID, _uint8 cbCard);

	int OutCardEstimate(_uint8 ChairID, _uint8 bCard, MJ_major& major, int& fan_num);

	void DoMultiOperate(_uint8 ChairID, int iOpType, _uint8 bCard);

	int GetCardEstimate(_uint8 ChairID, _uint8 bGetCard, MJ_major& major, int& fan_num);

	int GetGangInfo(_uint8 ChairID, MJ_opinfo& GangCardInfo);

	int GetGangInfo(const HandCards*pHc, MJ_opinfo& ganginfo);

	void UserOperateChi(_uint8 ChairID, int iOpType, _uint8 bCard);

	void UserOperatePeng(_uint8 ChairID, _uint8 bCard);

	void UserOperateGang(_uint8 ChairID, int iOpType, _uint8 bCard);

	void TurnBanker();

	int Check_Hu_Pattern(_uint8 ChairID, const HandCards& pHc, MJ_fan_buffer& oFan, const MJ_win_pattern& oPattern);

	void UserGetFirstCard(_uint8 ChairID, int& iOpType, MJ_fan_buffer& oFan);

	void GetAllHandCardRelativity(HandCards *pHc, MJ_BUFFER<_uint8, POOL_CARD_TYPE_MAX>& _relativity);
	
	void GetHandCardRelativity(_uint8 bCard, MJ_BUFFER<_uint8, POOL_CARD_TYPE_MAX>& _relativity);

	void Check_Listening(HandCards *pHc, _uint8 ChairID, _uint8 bCard, tagListenInfos stListenInfo[17], _uint8 &bListenLen);
	
	int GetListeningInfo_CheckJin(_uint8 ChairID, tagListenInfos stListenInfo[14], _uint8& bListenLen, HandCards &pHc);

	void DoSimpleOperate(_uint8 ChairID, int iOpType, _uint8 bCard);

	void UserOperateHuYouJin(_uint8 ChairID, _uint8 bHuCard = 0xFF);

	void UserOperateHu(_uint8 ChairID, _uint8 bHuCard = 0xFF);

	void Calc_HuScore(_uint8 ChairID, int iBaseScore[GAME_PLAYER], int iScore[GAME_PLAYER]);

	void Calc_BaseScore(_uint8 ChairID, int iBaseScore[GAME_PLAYER]);

	void Calc_GangScore();

	bool IsDoubleYouJin(HandCards& pHc, _uint8 ChairID);

	bool IsThreeYouJin(HandCards& pHc, _uint8 ChairID);

	bool EstimateUserRespond(_uint8 bOutChairID, _uint8 bOutCard);

	void OnUserGetCard_YouJinBuPai(_uint8 ChairID, bool bGetLast=false);

	void Send_Hand_Card();

	void Check_Listening_CheckYouJin(HandCards *pHc, _uint8 ChairID, _uint8 bCard, tagListenInfos stListenInfo[17], _uint8 &bListenLen);

	void OnUserOutCard_BuHua(_uint8 bOutChairID, _uint8 bOutCard);

	friend int laizi_chibi_check_hu(unsigned  char  *arr, unsigned  char  len, int n_lazi);

	void UserQiangGangHu(_uint8 ChairID, _uint8 bCard, MJ_fan_buffer& oFan);

	void RobGangPriority(_uint8 ChairID, _uint8 CardData, int OperateType);

	bool RobGangHu(_uint8 ChairID, _uint8 CardData, int iOpType);

	void OnRecvUserOperate(_uint8 ChairID, int iOpType, int iCard);

	int GetPengGang(_uint8 ChairID, _uint8 cbCard, MJ_opinfo& GangCardInfo);

	void InitBanker();

	bool RobJinHu(_uint8 ChairID);

	void Proto_GameScene_Operate(_uint8 ChairID, Player* player, proto::game::AckGameScene& stGameScene);

	void IsYouJinCanOutCard(_uint8 ChairID, int &bYouJinFlag);

	void Proto_Game_UserGetCard(_uint8 ChairID, _uint8 MoChairID, _uint8 bCard, proto::game::AckUserGetCard& stUsergetcard);

	void UserAfterOperate(_uint8 ChairID, int iOpType, _uint8 bCard);

	void Proto_Game_OpNotifyAfterPengChi(_uint8 ChairID, _uint8 CurChairID, const MJ_opinfo& gang, proto::game::AckOpNotify& stNotify);

protected:
	mj_conf_changtaimajiang m_conf_changtaimajiang;

private:
	ShowCards	 m_bJinCard;//金牌
	_uint8		 m_bReplaceJinBaiBanCard;//代替游金的白板	
	int		     m_bUserHasJinNum[GAME_PLAYER];//玩家拥有金牌的数量
	ShowCards	 m_bHuaCardList;//花牌
	bool		 M_bUserDoMultiOperate[GAME_PLAYER];//多玩家操作时，不必等待优先级低的玩家胡牌，可以直接胡牌
	bool         m_bUserOperateDanYouJinFlag[GAME_PLAYER];//单游金
	bool         m_bUserOperateDoubleYouJinFlag[GAME_PLAYER];//双游金
	bool		 m_bUserOperateThreeYouJinFlag[GAME_PLAYER]; //三游金
	//int          m_bUserOutCardJinNum[GAME_PLAYER];//打出金牌的个数
	ShowCards    m_bUserOutHuaNum[GAME_PLAYER]; //打出花牌的个数
	bool		 M_bUserDoMultiOperateQiangGangHu[GAME_PLAYER];//多玩家操作时，不必等待优先级低的玩家胡牌，可以直接胡牌
	_uint8       m_bBankerOutCard;
	bool         m_bUserFenBingFlag; //true ：分饼结束
	int          m_bUserFenBingNum; //分饼分数
	int          m_bUserFenOutCardNum; //分饼分数
	MJ_opinfo m_bUserOperatePengCardInfo[GAME_PLAYER];//过碰
	bool	m_bUserOperateGuoZhangHuFlag[GAME_PLAYER];//过张胡
	int     m_bZhuangBaseScore;
	int     m_bXianBaseScore;
	int     m_bLianZhuangNum;
	bool	m_bUserRogJinHuFlag[GAME_PLAYER]; //QiangJinHu
	bool	m_bUserRogJinHuPassFlag[GAME_PLAYER]; //QiangJinHu
	ShowCards m_bCanYouJinOutCardList[GAME_PLAYER];
};

#endif
