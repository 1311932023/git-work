#ifndef __TABLE_XIANNINGMAJIANG_H__
#define __TABLE_XIANNINGMAJIANG_H__
#include "../table.h"

struct mj_conf_xianningmj
{
	bool mj_conf_xianningmajiang;//咸宁麻将与咸宁转转。true：代表咸宁麻将
	bool mj_conf_xianningzhuanzhuan;//咸宁转转，：true代表咸宁转转
	int mj_conf_zhuanzhuan_zhuaniao;//鸟牌数量
	bool mj_conf_xianningmajiang_hongzhonglaizi_gang;
	mj_conf_xianningmj()
	{
		mj_conf_xianningmajiang = false;
		mj_conf_xianningzhuanzhuan = false;
		mj_conf_zhuanzhuan_zhuaniao = 0;
		mj_conf_xianningmajiang_hongzhonglaizi_gang = false;
	}
};

class Table_xianningmajiang : public Table
{
public:
	Table_xianningmajiang();
	~Table_xianningmajiang();
public:
	void ConfInit_Remote(const Json::Value& val);//配置信息，牌数等。

	void GameStart();

	void GameEnd();

	void GameEndReset();

	void GameGetGuiCard();

	void Proto_GameScene(_uint8 ChairID, Player* player, proto::game::AckGameScene& stGameScene);
	
	int  Calc_Fan(_uint8 ChairID, MJ_fan_buffer& oFan);

	void Proto_GameEndInfo(const MJ_user_buffer& UserID, proto::game::AckGameEnd& stGameend);

	int GetGangInfo(_uint8 ChairID, MJ_opinfo& GangCardInfo);

	int OutCardEstimate(_uint8 ChairID, _uint8 bCard, MJ_major& major, int& fan_num);

	int GetPengGang(_uint8 ChairID, _uint8 cbCard, MJ_opinfo& GangCardInfo);

	void OnUserOutCard(_uint8 bOutChairID, _uint8 bOutCard);

	void DoMultiOperate(_uint8 ChairID, int iOpType, _uint8 bCard);

	void OnUserGetCard(_uint8 ChairID, bool bGetLast = false);

	void OnUserGetLastFourCard(_uint8 ChairID);

	void UserOperateChi(_uint8 ChairID, int iOpType, _uint8 bCard);

	void UserOperatePeng(_uint8 ChairID, _uint8 bCard);

	void UserOperateGang(_uint8 ChairID, int iOpType, _uint8 bCard);

	void Fill_FixedCard_AnGang(_uint8 OpChairID, _uint8 ChairID, _uint8 Pos, proto::game::FixedCard* pstFix);

	void TurnBanker();

	int Check_Hu(_uint8 ChairID, HandCards& pHc, MJ_fan_buffer& oFan);

	int Check_Hu_Pattern(_uint8 ChairID, const HandCards& pHc, MJ_fan_buffer& oFan);

	bool FormatCards_Laizi(_uint8 card[14], int nLen, int nLaiZiNum,bool Is258Jiang);

	bool LaiZi_CanHu(MJ_hand_buffer stData, int LaiZiNum, bool Is258Jiang,bool bAllShunZi = false);

	bool IsPengPengHu(const HandCards& p, const _uint8 CardsFormatBuf[4][4], _uint8 Jiang);

	bool IsQingYiSe(const HandCards* p);

	bool Jiang258YiSe(const HandCards* pHc);

	bool IsQuanqiuren(const HandCards& pHc);

	bool IsHeiHu(const HandCards& pHc, MJ_fan_buffer& oFan);

	void Calc_BaseScore(_uint8 ChairID, int iBaseScore[GAME_PLAYER]);

	void Calc_HuScore(_uint8 ChairID, int iBaseScore[GAME_PLAYER], int iScore[GAME_PLAYER]);

	int GetCardEstimate(_uint8 ChairID, _uint8 bCard, MJ_major& major, int& fan_num);

	int Calc_GangFan(_uint8 ChairID);

	bool EstimateUserRespond(_uint8 bOutChairID, _uint8 bOutCard);

	void GetAllHandCardRelativity(HandCards *pHc, MJ_BUFFER<_uint8, POOL_CARD_TYPE_MAX>& _relativity);

	void GetHandCardRelativity(_uint8 bCard, MJ_BUFFER<_uint8, POOL_CARD_TYPE_MAX>& _relativity);

	int OutCardEstimate_QiangGangHu(_uint8 ChairID, _uint8 bCard, MJ_major& major, int& fan_num);

	bool RobGangHu(_uint8 chairID, _uint8 CardData, int iOpType = TYPE_WANGANG);

	void DoSimpleOperate(_uint8 ChairID, int iOpType, _uint8 bCard);

	int Calc_Hu(_uint8 ChairID, _uint8 bCard, int Score[GAME_PLAYER]);

	bool Jiang258YiSe_HeiHu(const HandCards* pHc);

	void Proto_GameScene_Operate(_uint8 ChairID, Player* player, proto::game::AckGameScene& stGameScene);

	//咸宁转转
	void GameEndAndGetMaNum();
	int  OutCardEstimate_0(_uint8 ChairID, _uint8 bCard, MJ_major& major, int& fan_num);
	int  OutCardEstimate_zhuanzhuan(_uint8 ChairID, _uint8 bCard, MJ_major& major, int& fan_num);
	void OnUserGetCard_0(_uint8 ChairID, bool bGetLast=false);
	void OnUserGetCard_zhuanzhuan(_uint8 ChairID, bool bGetLast=false);
	int  Check_Hu_0(_uint8 ChairID, HandCards& pHc, MJ_fan_buffer& oFan);
	int  Check_Hu_zhuanzhuan(_uint8 ChairID, HandCards& pHc, MJ_fan_buffer& oFan);
	bool Is7Dui(_uint8 ChairID, HandCards& pHc, MJ_fan_buffer& oFan);
	void Calc_BaseScore_0(_uint8 ChairID, int iBaseScore[GAME_PLAYER]);
	void Calc_BaseScore_zhuanzhuan(_uint8 ChairID, int iBaseScore[GAME_PLAYER]);
	int GetCardEstimate_0(_uint8 ChairID, _uint8 bCard, MJ_major& major, int& fan_num);
	int GetCardEstimate_zhuanzhuan(_uint8 ChairID, _uint8 bCard, MJ_major& major, int& fan_num);
	bool RobGangHu_0(_uint8 ChairID, _uint8 CardData, int iOpType);
	void Score_GameEndSucc(HuScoreSet& score, proto::game::AckGameEnd& stGameend);
	int GetPengGang_0(_uint8 ChairID, _uint8 cbCard, MJ_opinfo& GangCardInfo);
	int GetPengGang_zhuanzhuan(_uint8 ChairID, _uint8 cbCard, MJ_opinfo& GangCardInfo);
	void Score_GameEndDraw(HuScoreSet& score, proto::game::AckGameEnd& stGameend);
	void Check_Listening(HandCards *pHc, _uint8 ChairID, _uint8 bCard, tagListenInfos stListenInfo[14], _uint8 &bListenLen);
	//
	bool EstimateUserRespond_0(_uint8 bOutChairID, _uint8 bOutCard);
	void OnRecvUserOperate(_uint8 ChairID, int iOpType, int iCard);
	void RobGangPriority(_uint8 ChairID, _uint8 CardData, int OperateType);
	void MultiOperateOver(_uint8 ChairID, int iOpType, _uint8 bCard);
	bool GetZhiGang(const HandCards* pHc, MJ_opinfo& ganginfo, _uint8 bCard);
	int GetListeningInfo_1(HandCards &HandCard, _uint8 ChairID, tagListenInfos stListenInfo[14], _uint8& bListenLen,_uint8 bCard);
	bool GetWanGang(const HandCards* pHc, MJ_opinfo& ganginfo, _uint8 bCard);
	void OnUserOutCard_1(_uint8 ChairID, _uint8 bCard);//咸宁红中赖子正常出牌
	void OnUserOutCard_0(_uint8 ChairID, _uint8 bCard);//咸宁红中赖子杠。
	void OnUserOutCard_XianNing(_uint8 bOutChairID, _uint8 bOutCard);
	void Proto_Game_UserOutCard(_uint8 ChairID, _uint8 bCard, proto::game::AckUserOutCard& stUseroutcard);
	void UserOperateHu(_uint8 ChairID, _uint8 bHuCard = 0xFF);
	void SendTingInfo(_uint8 ChairID);
	int GetListeningInfo(_uint8 ChairID, tagListenInfos stListenInfo[14], _uint8& bListenLen);
	int OutCardEstimate_zhuanzhuan_Ting(_uint8 ChairID, _uint8 bCard, MJ_major& major, int& fan_num, HandCards pHcTing);
	int OutCardEstimate_0_Ting(_uint8 ChairID, _uint8 bCard, MJ_major& major, int& fan_num, HandCards pHcTing);
	int OutCardEstimate_Ting(_uint8 ChairID, _uint8 bCard, MJ_major& major, int& fan_num, HandCards pHcTing);
	int GetCardEstimate_0_Ting(_uint8 ChairID, _uint8 bCard, MJ_major& major, int& fan_num, HandCards pHcTing);
	int GetCardEstimate_zhuanzhuan_Ting(_uint8 ChairID, _uint8 bCard, MJ_major& major, int& fan_num, HandCards pHcTing);
	int GetCardEstimate_Ting(_uint8 ChairID, _uint8 bCard, MJ_major& major, int& fan_num, HandCards pHcTing);
	void Calc_BaseScore_GuoZhangHu(_uint8 ChairID, int iBaseScore[GAME_PLAYER], MJ_fan_buffer mHuoFan);
	void Check_Listening_QuanQiuRen(HandCards *pHc, _uint8 ChairID, _uint8 bCard, tagListenInfos stListenInfo[14], _uint8 &bListenLen);
	int OutCardEstimate_QuanQiuRen_Ting(_uint8 ChairID, _uint8 bCard, MJ_major& major, int& fan_num, HandCards pHcTing);
	int GetCardEstimate_QuanQiuRen_Ting(_uint8 ChairID, _uint8 bCard, MJ_major& major, int& fan_num, HandCards pHcTing);
	//添加抢暗杠
	bool GetAnGang(const HandCards* pHc, MJ_opinfo& ganginfo, _uint8 bCard);
	void Proto_Game_OpNotify_QiangGangHu(_uint8 ChairID, _uint8 CurChairID, _uint8 bCard, proto::game::AckOpNotify& stNotify);
	void Calc_AnGangScore(_uint8 ChairID);
	void RobGangPriority_QiangAnGang(_uint8 ChairID, _uint8 CardData, int OperateType);
	bool Check_Listening_AnGang(HandCards *pHc, _uint8 ChairID, ShowCards &bufCard);
	bool CheckUserOperator(_uint8 ChairID, int iOpType, _uint8 bCard);
	int OutCardEstimate_QiangGangHu_AnGang(_uint8 ChairID, _uint8 bCard, MJ_major& major, int& fan_num);
	void Proto_Game_OpNotify_QiangAnGangHu(_uint8 ChairID, _uint8 CurChairID, _uint8 bCard, proto::game::AckOpNotify& stNotify, ShowCards &buf);
	static void GetCardOperateCard_Timer_cb(struct ev_loop *loop, struct ev_timer *w, int revents);
	void Fill_HandCard(_uint8 ChairID, proto::game::HandCards* pHandcard);

protected:
	mj_conf_xianningmj m_conf_xianningmj;

private:
	_uint8  m_bXianNingGuiPai;//鬼牌
	_uint8  m_bXianNingFanGui;//翻鬼
	_uint8  m_bUserHasGuiPaiNum[GAME_PLAYER];//鬼牌数量
	MJ_opinfo m_bUserOperatePengCardInfo[GAME_PLAYER];//过碰
	bool	m_bUserOperateGuoZhangHuFlag[GAME_PLAYER];//过张胡
	int     m_bUserOutGuiCardNum[GAME_PLAYER];
	int     m_bUserOutHongZhongCardNum[GAME_PLAYER];
	bool    m_bUserOperateLzHzGangFlag[GAME_PLAYER];
	//转转
	ShowCards m_bGetMaPai; //转转马牌
	ShowCards m_bZhongMaPai[GAME_PLAYER];//转转中马
	ShowCards m_bUserGangOperatePengFlag[GAME_PLAYER];//有碰杠时，选择碰，这句不能再补杠
	bool    m_bUserQingYiSeAutoWarning[GAME_PLAYER];//清一色自动报警
	bool    m_bUser258JiangYiSeAutoWarning[GAME_PLAYER];//将258自动报警
	_uint8  m_bXianNingZhiGangCard;
	bool     m_bUserQiangZhiGangHuPass;
	bool    m_bUserQuanQiuRenAutoWarning[GAME_PLAYER];
	bool    m_bUserOutHongZhongLaiZiCardFlag[GAME_PLAYER];
	bool	M_bUserDoMultiOperate[GAME_PLAYER];
	int     m_bUserOperateGuoZhangHuTotalFen[GAME_PLAYER];//过张时的蕃
	int     m_RobAnGangHuNum;                             //抢暗杠胡的玩家数		
	bool	M_bSendUserQiangAnGangHuFlag[GAME_PLAYER];
	int	    M_bUserDoMultiOperateQiangAnGang[GAME_PLAYER];//是否操作过抢暗杠
	bool	M_bCalcUserQiangAnGangHuFlag[GAME_PLAYER]; //是否应该计算抢杠失败的分数。
	ShowCards m_bQiangAnGangTingCard[GAME_PLAYER];

//分张的时候的定时器
	ev_timer			m_GetCardOperateCard_Timer;
	ev_tstamp			m_GetCardOperateCard_Timer_stamp;
	int	    m_OnUserGetFenZhangCard[GAME_PLAYER];//是否操作过抢暗杠
	int	    m_OnUserQiangAnGangCard[GAME_PLAYER];//抢暗杠的牌
	bool	m_OnUserQiangAnCalcScoreFlag[GAME_PLAYER];//是否计算了杠分
};

#endif

