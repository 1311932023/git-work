#ifndef __TABLE_YUNXIAOMAJIANG_H__
#define __TABLE_YUNXIAOMAJIANG_H__
#include "../table.h"
//0x01 1??ò
//0x11 1????
//0x21 1????
//0x31 ????
//0x41 ????

struct mj_conf_yunxiaomajiang
{

	bool yun_xiao_dian_pao_bao_pei;
	bool yunxiao_qiang_ming_gang;
	bool yunxiao_zidong_chupai_hupai_pass;
	bool yunxiao_twopeople_zimohu_flag; //二人是否可以点炮，true：二人不能点炮。
	mj_conf_yunxiaomajiang()
	{
		yun_xiao_dian_pao_bao_pei = false;
		yunxiao_qiang_ming_gang = true;
		yunxiao_zidong_chupai_hupai_pass = false;
		yunxiao_twopeople_zimohu_flag = false;
	}
};

class Table_yunxiaomajiang : public Table
{
public:
	Table_yunxiaomajiang();
	~Table_yunxiaomajiang();
public:

	void ConfInit_Remote(const Json::Value& val);//配置信息，牌数等。

	void GameStart();//实现开金
	/**
	*  @fuction: OnUserGetCard
	*  @brief: 玩家摸牌
	*  @return:
	**/
	void OnUserGetCard(_uint8 ChairID, bool bGetLast = false);
	/**
	*  @fuction: GetGangInfo
	*  @brief: 玩家弯杠、暗杠信息,金不能参与吃碰杠.
	*  @return:
	**/
	int GetGangInfo(_uint8 ChairID, MJ_opinfo& GangCardInfo);
	/**
	*  @fuction: Check_Hu
	*  @brief: 检查牌有没有胡.
	*  @return:
	**/
	int Check_Hu(_uint8 ChairID, HandCards& pHc, MJ_fan_buffer& oFan);
	/**
	*  @fuction: IsSanJinDao
	*  @brief: 
	*  @return:
	**/
	bool IsSanJinDao(_uint8 ChairID, MJ_fan_buffer& oFan);
	/**
	*  @fuction: Is7Dui
	*  @brief: 7对
	*  @r
	*/
	bool Is7Dui(_uint8 ChairID, HandCards& pHc, MJ_fan_buffer& oFan, int LaiZiNum);
	/**
	*  @fuction: Is19Hu
	*  @brief: 19胡
	*  @return:
	**/
	bool Is19Hu(_uint8 ChairID, HandCards& pHc, MJ_fan_buffer& oFan, MJ_win_pattern& pattern);
	/**
	*  @fuction: IsAnGangHu
	*  @brief: 暗杠胡加番
	*  @return:
	**/
	void IsAnGangHu(MJ_fan_buffer& oFan, int iOpType);
	/**
	*  @fuction: IsMingGangHu
	*  @brief: 明杠胡加番
	*  @return:
	**/
	void IsMingGangHu(MJ_fan_buffer& oFan, int iOpType);
	/**
	*  @fuction: Calc_Hu
	*  @brief: 胡时算分
	*  @return:
	**/
	int Calc_Hu(_uint8 ChairID, _uint8 bCard, int Score[GAME_PLAYER]);
	/**
	*  @fuction: IsBuDa
	*  @brief: 暗杠胡
	*  @return:
	**/
	bool IsBuDa(HandCards& pHc, MJ_fan_buffer& oFan, MJ_win_pattern& pattern);
	/**
	*  @fuction: IsQingYiSe
	*  @brief: 
	*  @return:
	**/
	bool IsQingYiSe(const HandCards* p, _uint8 ChairID);
	/**
	*  @fuction: IsZiYiSe
	*  @brief:
	*  @return:
	**/
	bool IsZiYiSe(const HandCards* p,_uint8 ChairID);
	/**
	*  @fuction: CheckUserOutCard
	*  @brief: 
	*  @return:
	**/
	bool CheckUserOutCard(_uint8 ChairID, _uint8 cbCard);
	/**
	*  @fuction: OnUserOutCard
	*  @brief: 出牌,用户出的牌如果是金或白板,需要进行互换,但是注意反馈的还是原来的牌.
	*  @return:
	**/
	void OnUserOutCard(_uint8 ChairID, _uint8 bCard);
	/**
	*  @fuction: UserOperatePass
	*  @brief: 用户满足单游金的情况下,选择pass,则下次摸牌前,不能胡其他人的牌
	*  @return:
	**/
	void UserOperatePass(_uint8 ChairID, int iOpType, _uint8 bCard);
	/**
	*  @fuction: TurnBanker
	*  @brief:
	*  @return:
	**/
	void TurnBanker();
	/**
	*  @fuction: Calc_Fan
	*  @brief: 计分规则
	*  @return:
	**/
	int Calc_Fan(_uint8 ChairID, MJ_fan_buffer& oFan);
	/**
	*  @fuction: GetCardEstimate
	*  @brief: 玩家自摸一张牌，此时判断是否可以听杠胡。
	*  @return:
	**/
	int GetCardEstimate(_uint8 ChairID, _uint8 bCard, MJ_major& major, int& fan_num);
	/**
	*  @fuction: Check_Listening
	*  @brief: 玩家自摸一张牌，此时判断是否可以听杠胡。
	*  @return:
	**/
	void Check_Listening(HandCards *pHc, _uint8 ChairID, _uint8 bCard, tagListenInfos stListenInfo[14], _uint8 &bListenLen);
	/**
	*  @fuction: GameEnd
	*  @brief: 
	*  @return:
	**/
	void GameEnd();
	/**
	*  @fuction: GameEndReset
	*  @brief: 在游戏结束时,需要将金与白板替换过来.
	*  @return:
	**/
	void GameEndReset();
	/**
	*  @fuction: GameEndReset
	*  @brief: 在游戏结束时,需要将金与白板替换过来.
	*  @return:
	**/
	void UserOperateChi(_uint8 ChairID, int iOpType, _uint8 bCard);
	/**
	*  @fuction: UserOperatePeng
	*  @brief: 玩家执行碰操作
	*  @return:
	**/
	void UserOperatePeng(_uint8 ChairID, _uint8 bCard);
	/**
	*  @fuction: UserOperateGang
	*  @brief: 玩家执行杠操作
	*  @return:
	**/
	void UserOperateGang(_uint8 ChairID, int iOpType, _uint8 bCard);
	/**
	*  @fuction: Proto_GameScene
	*  @brief: 玩家断线重新链接,重新发送金的信息.
	*  @return:
	**/
	void Proto_GameScene(_uint8 ChairID, Player* player, proto::game::AckGameScene& stGameScene);
	/**
	*  @fuction: GetAllHandCardRelativity
	*  @brief: .
	*  @return:
	**/
	void GetAllHandCardRelativity(HandCards *pHc, MJ_BUFFER<_uint8, POOL_CARD_TYPE_MAX>& _relativity);
	/**
	*  @fuction: GetHandCardRelativity
	*  @brief:
	*  @return:
	**/
	void GetHandCardRelativity(_uint8 bCard, MJ_BUFFER<_uint8, POOL_CARD_TYPE_MAX>& _relativity);
	/**
	*  @fuction: ChangeHandCardBaiBanAndJin
	*  @brief: 交换玩家手中的金与白板.
	*  @return:
	**/
	void ChangeHandCardBaiBanAndJin();
	/**
	*  @fuction: OutCardEstimate
	*  @brief: 
	*  @return:
	**/
	int OutCardEstimate(_uint8 ChairID, _uint8 bCard, MJ_major& major, int& fan_num);
	/**
	*  @fuction: IsQiangJinHu
	*  @brief: 玩家地胡,与抢金胡判断.
	*  @return:
	**/
	bool IsQiangJinHu(_uint8 ChairID);
	/**
	*  @fuction: QiangJinHuCheck_Hu
	*  @brief: .因为check_hu 的过张胡影响校验，重写该函数
	*  @return:
	**/
	int QiangJinHuCheck_Hu(_uint8 ChairID, HandCards& pHc, MJ_fan_buffer& oFan);
	/**
	*  @fuction: UserAfterOperate
	*  @brief: 玩家碰之后,检查杠,当操作的是金时,需要金与白板的互换.
	*  @return:
	**/
	void UserAfterOperate(_uint8 ChairID, int iOpType, _uint8 bCard);
	/**
	*  @fuction: Proto_GameEndInfo
	*  @brief: 算番信息
	*  @return:
	**/
	void Proto_GameEndInfo(const MJ_user_buffer& UserID, proto::game::AckGameEnd& stGameend);
	/**
	*  @fuction: Calc_BaseScore
	*  @brief: 算番信息
	*  @return:
	**/
	void Calc_BaseScore(_uint8 ChairID, int iBaseScore[GAME_PLAYER]);
	/**
	*  @fuction: Calc_HuScore
	*  @brief: 算番信息
	*  @return:
	**/
	void Calc_HuScore(_uint8 ChairID, int iBaseScore[GAME_PLAYER], int iScore[GAME_PLAYER]);
	/**
	*  @fuction: IsDanYouJin
	*  @brief: 
	*  @return:
	**/
	bool IsDanYouJin(HandCards& pHc, _uint8 ChairID);
	/**
	*  @fuction: IsSanJinDao
	*  @brief: 抢金胡，任何一名玩家手中有3个金，即可胡牌，无需任何牌型
	*  @return:
	**/
	void OnUserSendDanYoujinHu(_uint8 ChairID);
	/**
	*  @fuction: DoSimpleOperate
	*  @brief:
	*  @return:
	**/
	void DoSimpleOperate(_uint8 ChairID, int iOpType, _uint8 bCard);
	/**
	*  @fuction: UserOperateHuDanYouJin
	*  @brief:
	*  @return:
	**/
	void UserOperateHuDanYouJin(_uint8 ChairID);
	/**
	*  @fuction: IsDoubleYouJin
	*  @brief: 
	*  @return:
	**/
	bool IsDoubleYouJin(HandCards& pHc, _uint8 ChairID);
	/**
	*  @fuction: IsDoubleYouJin
	*  @brief: 
	*  @return:
	**/
	bool IsThreeYouJin(HandCards& pHc, _uint8 ChairID);

	/**
	*  @fuction: IsDoubleYouJin
	*  @brief:
	*  @return:
	**/
	bool Is7DuiDoubleYouJin(HandCards& pHc, _uint8 ChairID);
	/**
	*  @fuction: IsDoubleYouJin
	*  @brief:
	*  @return:
	**/
	bool Is7DuiThreeYouJin(HandCards& pHc, _uint8 ChairID);
	/**
	*  @fuction: IsDoubleYouJin
	*  @brief:
	*  @return:
	**/
	void Fill_FixedCard(_uint8 OpChairID, _uint8 ChairID, _uint8 Pos, proto::game::FixedCard* pstFix);
	/**
	*  @fuction: ExecuteGang
	*  @brief:
	*  @return:
	**/
	void ExecuteGang(_uint8 ChairID, HandCards *pHc, _uint8 bOpCardChairID, _uint8 cbOperateCard, int cbType);
	/**
	*  @fuction: RobGangHu
	*  @brief:
	*  @return:
	**/
	bool RobGangHu(_uint8 ChairID, _uint8 CardData, int iOpType);
	/**
	*  @fuction: Proto_Game_OperateGang
	*  @brief:
	*  @return:
	**/
	void Proto_Game_OperateGang(_uint8 ChairID, _uint8 bCard, int iOpType, int iGangScore[GAME_PLAYER], proto::game::AckBcOpResult& stOpresult);
	/**
	*  @fuction: Calc_Jin_Fan
	*  @brief:
	*  @return:
	**/
	int Calc_Jin_Fan(_uint8 ChairID, MJ_fan_buffer& oFan);
	/**
	*  @fuction: GetPengGang
	*  @brief:
	*  @return:
	**/
	int DanYouJinCheck_Hu(_uint8 ChairID, HandCards& pHc, MJ_fan_buffer& oFan);

	void DanYouJinCheck_Listening(HandCards *pHc, _uint8 ChairID, _uint8 bCard, tagListenInfos stListenInfo[14], _uint8 &bListenLen);

	int GetPengGang(_uint8 ChairID, _uint8 cbCard, MJ_opinfo& GangCardInfo);

	void DoMultiOperate(_uint8 ChairID, int iOpType, _uint8 bCard);

	void MultiOperateOver(_uint8 ChairID, int iOpType, _uint8 bCard);

	void OnRecvUserOperate(_uint8 ChairID, int iOpType, int iCard);

	void Proto_Game_UserOutCard(_uint8 ChairID, _uint8 bCard, proto::game::AckUserOutCard& stUseroutcard);

	int OutCardEstimate_Qiangganghu(_uint8 ChairID, _uint8 bCard, MJ_major& major, int& fan_num);

	//校验手牌
	void CheckUserHandCards(_uint8 ChairID, _uint8 bCard[20], int cards_len);

	void RobGangPriority(_uint8 ChairID, _uint8 CardData, int OperateType);


	static void GetCardOperateOutCard_Timer_cb(struct ev_loop *loop, struct ev_timer *w, int revents);

	static void GetCardOperateHu_Timer_cb(struct ev_loop *loop, struct ev_timer *w, int revents);

	static void YunXiaoYouJinOperateHu_Timer_cb_0(struct ev_loop *loop, struct ev_timer *w, int revents);

	static void YunXiaoYouJinOperateHu_Timer_cb_1(struct ev_loop *loop, struct ev_timer *w, int revents);

	//出牌之后，其他玩家超时，如果没有胡，则直接pass。
	static void OutCardOperatePass_Timer_cb_0(struct ev_loop *loop, struct ev_timer *w, int revents);
	static void OutCardOperatePass_Timer_cb_1(struct ev_loop *loop, struct ev_timer *w, int revents);
	static void OutCardOperatePass_Timer_cb_2(struct ev_loop *loop, struct ev_timer *w, int revents);
	static void OutCardOperatePass_Timer_cb_3(struct ev_loop *loop, struct ev_timer *w, int revents);
	//出牌之后，其他玩家超时，如果没有胡，则直接pass。
	static void OutCardOperateHu_Timer_cb_0(struct ev_loop *loop, struct ev_timer *w, int revents);
	static void OutCardOperateHu_Timer_cb_1(struct ev_loop *loop, struct ev_timer *w, int revents);
	static void OutCardOperateHu_Timer_cb_2(struct ev_loop *loop, struct ev_timer *w, int revents);
	static void OutCardOperateHu_Timer_cb_3(struct ev_loop *loop, struct ev_timer *w, int revents);
	void stop_yunxiao_operator_timer_stop(_uint8 ChairID);

	static void ChiPengCardOperateOutCard_Timer_cb(struct ev_loop *loop, struct ev_timer *w, int revents);
	//自动出牌前，发送给客户端，禁止出牌
	static void OutTimeStopOperateOutCard_Timer_cb(struct ev_loop *loop, struct ev_timer *w, int revents);

	void BroadcastOperateResult(_uint8 bOpChairID, int iOpType, _uint8 bOpCard);
	void Proto_GameScene_Operate(_uint8 ChairID, Player* player, proto::game::AckGameScene& stGameScene);
	void UserOperateHu(_uint8 ChairID, _uint8 bHuCard = 0xFF);
	void Proto_Game_OpNotify(_uint8 ChairID, _uint8 CurChairID, _uint8 bCard, proto::game::AckOpNotify& stNotify);
	void Proto_Game_UserGetCard(_uint8 ChairID, _uint8 MoChairID, _uint8 bCard, proto::game::AckUserGetCard& stUsergetcard);
	bool EstimateUserRespond(_uint8 bOutChairID, _uint8 bOutCard);

protected:
	mj_conf_yunxiaomajiang m_conf_yunxiaomj;

private:
	_uint8		m_bJinCard;						//金牌
	_uint8		m_bUserHasJinNum[GAME_PLAYER];	  //玩家拥有金牌的数量
	_uint8      m_bReplaceJinCard;				 //代替游金的白板	
	_uint8      m_bHasReplaceJinCardNum[GAME_PLAYER];//玩家拥有白板的数量
	_uint8      m_bSaveZiMoCard[GAME_PLAYER];
	_uint8      m_bSaveUserOutCard[GAME_PLAYER];
	bool		m_bIsAnGangHuFlag[GAME_PLAYER];
	bool		m_bIsMingGangHuFlag[GAME_PLAYER];
	bool        m_bIsGuoZhangHuFlag[GAME_PLAYER]; 
	bool		m_bUserOperateGuoZhangHuFlag[GAME_PLAYER];	 
//	bool		m_bUserGetJinFlag[GAME_PLAYER];		

	bool		m_bUserDanYouJinHuFlag[GAME_PLAYER];		
	bool		m_bUserOperateDanJinPass[GAME_PLAYER];		  
	bool        m_bUserOperateDanYouJinHu[GAME_PLAYER];	 
	_uint8      m_bUserOperateDanYouJinstep[GAME_PLAYER];
 
	HandCards   m_bSaveUserOutOnlyCard[GAME_PLAYER];	
	//
	bool		m_bUserDoubleYouJinHuFlag[GAME_PLAYER];		  
	bool		m_bUserOperateDoubleYouJinFlag[GAME_PLAYER];  
	bool        m_bUserOperateDoubleYouJinStep[GAME_PLAYER];
	//三游金
	bool		m_bUserThreeYouJinHuFlag[GAME_PLAYER];		 
	bool		m_bUserOperateThreeYouJinFlag[GAME_PLAYER];  	
	bool        m_bUserOperateThreeYouJinStep[GAME_PLAYER];
	
	bool        m_QingYiSeDianPaoBaoPei;

	bool		m_bUserOperateQiangJinHuFlag[GAME_PLAYER];
	bool		m_bUserOperateQiangJinHuPass[GAME_PLAYER];

	MJ_opinfo	m_bUserOperateGangCardInfo[GAME_PLAYER];
	_uint8      m_bUserOperateGangNum;

	bool        m_bUserOperateChiPengGangDanYouJinstep[GAME_PLAYER];
	bool        m_bUserOperateChiPengGangDoubleYouJinstep[GAME_PLAYER];
	bool        m_bUserOperateChiPengGangThreeYouJinstep[GAME_PLAYER];

	int         m_bUserDoMultiOperateiOpType[GAME_PLAYER];
	int         m_bUserDoMultiOperateiOpTypeFlag [GAME_PLAYER];
	bool        m_bUserAutoOutCardFlag;
	bool        m_bUerOutErrFlag[GAME_PLAYER];
	bool        m_bOutTimeStopOperateOutCardFlag[GAME_PLAYER];//断线重连后，如果已经超时，则禁止出牌。
	_uint8		m_bUserOutCardFlag[GAME_PLAYER];						//出的牌，在过张之前不能再碰
	bool        m_bUserCheckDoMultiOperateiOpTypeFlag[GAME_PLAYER];
	//_uint8		m_bUserChiCardOutCardFlag[GAME_PLAYER];//吃某张牌，禁止打出。
	MJ_fan_buffer m_bUserHuTypeoFan[GAME_PLAYER];
	bool		m_bUserHuQingYiSeFlag[GAME_PLAYER];
	bool		m_bUserOperateShuangYouJinZhaungThreeFlag[GAME_PLAYER];
	//玩家摸牌超时出牌与胡牌
	ev_timer			m_GetCardOperateOutCard_Timer;
	ev_tstamp			m_GetCardOperateOutCard_Timer_stamp;
	ev_timer			m_GetCardOperateHu_Timer;
	ev_tstamp			m_GetCardOperateHu_Timer_stamp;
	//胡游金
	//玩家游金超时，胡游金。
	ev_timer			m_YouJinOperateHuYouJin_Timer_0;
	ev_tstamp			m_YouJinOperateHuYouJin_Timer_stamp_0;
	ev_timer			m_YouJinOperateHuYouJin_Timer_1;
	ev_tstamp			m_YouJinOperateHuYouJin_Timer_stamp_1;

	//玩家出牌后，其他玩家超时选择pass。
	ev_timer			m_OutCardOutTimeOperatePass_Timer_0;
	ev_tstamp			m_OutCardOutTimeOperatePass_Timer_stamp_0;
	ev_timer			m_OutCardOutTimeOperatePass_Timer_1;
	ev_tstamp			m_OutCardOutTimeOperatePass_Timer_stamp_1;
	ev_timer			m_OutCardOutTimeOperatePass_Timer_2;
	ev_tstamp			m_OutCardOutTimeOperatePass_Timer_stamp_2;
	ev_timer			m_OutCardOutTimeOperatePass_Timer_3;
	ev_tstamp			m_OutCardOutTimeOperatePass_Timer_stamp_3;

	//玩家出牌后，其他玩家超时如果胡牌，则自动胡牌
	ev_timer			m_OutCardOutTimeOperateHu_Timer_0;
	ev_tstamp			m_OutCardOutTimeOperateHu_Timer_stamp_0;
	ev_timer			m_OutCardOutTimeOperateHu_Timer_1;
	ev_tstamp			m_OutCardOutTimeOperateHu_Timer_stamp_1;
	ev_timer			m_OutCardOutTimeOperateHu_Timer_2;
	ev_tstamp			m_OutCardOutTimeOperateHu_Timer_stamp_2;
	ev_timer			m_OutCardOutTimeOperateHu_Timer_3;
	ev_tstamp			m_OutCardOutTimeOperateHu_Timer_stamp_3;

	ev_timer			m_ChiPengCardOperateOutCard_Timer;
	ev_tstamp			m_ChiPengCardOperateOutCard_Timer_stamp;

	ev_timer			m_OutTimeStopOperateOutCard_Timer;
	ev_tstamp			m_OutTimeStopOperateOutCard_Timer_stamp;

};

#endif

