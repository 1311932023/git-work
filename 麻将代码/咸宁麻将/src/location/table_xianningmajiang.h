#ifndef __TABLE_XIANNINGMAJIANG_H__
#define __TABLE_XIANNINGMAJIANG_H__
#include "../table.h"

struct mj_conf_xianningmj
{
	bool mj_conf_xianningmajiang;//�����齫������תת��true�����������齫
	bool mj_conf_xianningzhuanzhuan;//����תת����true��������תת
	int mj_conf_zhuanzhuan_zhuaniao;//��������
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
	void ConfInit_Remote(const Json::Value& val);//������Ϣ�������ȡ�

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

	//����תת
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
	void OnUserOutCard_1(_uint8 ChairID, _uint8 bCard);//��������������������
	void OnUserOutCard_0(_uint8 ChairID, _uint8 bCard);//�����������Ӹܡ�
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
	//���������
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
	_uint8  m_bXianNingGuiPai;//����
	_uint8  m_bXianNingFanGui;//����
	_uint8  m_bUserHasGuiPaiNum[GAME_PLAYER];//��������
	MJ_opinfo m_bUserOperatePengCardInfo[GAME_PLAYER];//����
	bool	m_bUserOperateGuoZhangHuFlag[GAME_PLAYER];//���ź�
	int     m_bUserOutGuiCardNum[GAME_PLAYER];
	int     m_bUserOutHongZhongCardNum[GAME_PLAYER];
	bool    m_bUserOperateLzHzGangFlag[GAME_PLAYER];
	//תת
	ShowCards m_bGetMaPai; //תת����
	ShowCards m_bZhongMaPai[GAME_PLAYER];//תת����
	ShowCards m_bUserGangOperatePengFlag[GAME_PLAYER];//������ʱ��ѡ��������䲻���ٲ���
	bool    m_bUserQingYiSeAutoWarning[GAME_PLAYER];//��һɫ�Զ�����
	bool    m_bUser258JiangYiSeAutoWarning[GAME_PLAYER];//��258�Զ�����
	_uint8  m_bXianNingZhiGangCard;
	bool     m_bUserQiangZhiGangHuPass;
	bool    m_bUserQuanQiuRenAutoWarning[GAME_PLAYER];
	bool    m_bUserOutHongZhongLaiZiCardFlag[GAME_PLAYER];
	bool	M_bUserDoMultiOperate[GAME_PLAYER];
	int     m_bUserOperateGuoZhangHuTotalFen[GAME_PLAYER];//����ʱ��ެ
	int     m_RobAnGangHuNum;                             //�����ܺ��������		
	bool	M_bSendUserQiangAnGangHuFlag[GAME_PLAYER];
	int	    M_bUserDoMultiOperateQiangAnGang[GAME_PLAYER];//�Ƿ������������
	bool	M_bCalcUserQiangAnGangHuFlag[GAME_PLAYER]; //�Ƿ�Ӧ�ü�������ʧ�ܵķ�����
	ShowCards m_bQiangAnGangTingCard[GAME_PLAYER];

//���ŵ�ʱ��Ķ�ʱ��
	ev_timer			m_GetCardOperateCard_Timer;
	ev_tstamp			m_GetCardOperateCard_Timer_stamp;
	int	    m_OnUserGetFenZhangCard[GAME_PLAYER];//�Ƿ������������
	int	    m_OnUserQiangAnGangCard[GAME_PLAYER];//�����ܵ���
	bool	m_OnUserQiangAnCalcScoreFlag[GAME_PLAYER];//�Ƿ�����˸ܷ�
};

#endif

