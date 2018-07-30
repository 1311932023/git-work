#ifndef __TABLE_YUNXIAOMAJIANG_H__
#define __TABLE_YUNXIAOMAJIANG_H__
#include "../table.h"
//0x01 1??��
//0x11 1????
//0x21 1????
//0x31 ????
//0x41 ????

struct mj_conf_yunxiaomajiang
{

	bool yun_xiao_dian_pao_bao_pei;
	bool yunxiao_qiang_ming_gang;
	bool yunxiao_zidong_chupai_hupai_pass;
	bool yunxiao_twopeople_zimohu_flag; //�����Ƿ���Ե��ڣ�true�����˲��ܵ��ڡ�
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

	void ConfInit_Remote(const Json::Value& val);//������Ϣ�������ȡ�

	void GameStart();//ʵ�ֿ���
	/**
	*  @fuction: OnUserGetCard
	*  @brief: �������
	*  @return:
	**/
	void OnUserGetCard(_uint8 ChairID, bool bGetLast = false);
	/**
	*  @fuction: GetGangInfo
	*  @brief: �����ܡ�������Ϣ,���ܲ��������.
	*  @return:
	**/
	int GetGangInfo(_uint8 ChairID, MJ_opinfo& GangCardInfo);
	/**
	*  @fuction: Check_Hu
	*  @brief: �������û�к�.
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
	*  @brief: 7��
	*  @r
	*/
	bool Is7Dui(_uint8 ChairID, HandCards& pHc, MJ_fan_buffer& oFan, int LaiZiNum);
	/**
	*  @fuction: Is19Hu
	*  @brief: 19��
	*  @return:
	**/
	bool Is19Hu(_uint8 ChairID, HandCards& pHc, MJ_fan_buffer& oFan, MJ_win_pattern& pattern);
	/**
	*  @fuction: IsAnGangHu
	*  @brief: ���ܺ��ӷ�
	*  @return:
	**/
	void IsAnGangHu(MJ_fan_buffer& oFan, int iOpType);
	/**
	*  @fuction: IsMingGangHu
	*  @brief: ���ܺ��ӷ�
	*  @return:
	**/
	void IsMingGangHu(MJ_fan_buffer& oFan, int iOpType);
	/**
	*  @fuction: Calc_Hu
	*  @brief: ��ʱ���
	*  @return:
	**/
	int Calc_Hu(_uint8 ChairID, _uint8 bCard, int Score[GAME_PLAYER]);
	/**
	*  @fuction: IsBuDa
	*  @brief: ���ܺ�
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
	*  @brief: ����,�û�����������ǽ��װ�,��Ҫ���л���,����ע�ⷴ���Ļ���ԭ������.
	*  @return:
	**/
	void OnUserOutCard(_uint8 ChairID, _uint8 bCard);
	/**
	*  @fuction: UserOperatePass
	*  @brief: �û����㵥�ν�������,ѡ��pass,���´�����ǰ,���ܺ������˵���
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
	*  @brief: �Ʒֹ���
	*  @return:
	**/
	int Calc_Fan(_uint8 ChairID, MJ_fan_buffer& oFan);
	/**
	*  @fuction: GetCardEstimate
	*  @brief: �������һ���ƣ���ʱ�ж��Ƿ�������ܺ���
	*  @return:
	**/
	int GetCardEstimate(_uint8 ChairID, _uint8 bCard, MJ_major& major, int& fan_num);
	/**
	*  @fuction: Check_Listening
	*  @brief: �������һ���ƣ���ʱ�ж��Ƿ�������ܺ���
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
	*  @brief: ����Ϸ����ʱ,��Ҫ������װ��滻����.
	*  @return:
	**/
	void GameEndReset();
	/**
	*  @fuction: GameEndReset
	*  @brief: ����Ϸ����ʱ,��Ҫ������װ��滻����.
	*  @return:
	**/
	void UserOperateChi(_uint8 ChairID, int iOpType, _uint8 bCard);
	/**
	*  @fuction: UserOperatePeng
	*  @brief: ���ִ��������
	*  @return:
	**/
	void UserOperatePeng(_uint8 ChairID, _uint8 bCard);
	/**
	*  @fuction: UserOperateGang
	*  @brief: ���ִ�иܲ���
	*  @return:
	**/
	void UserOperateGang(_uint8 ChairID, int iOpType, _uint8 bCard);
	/**
	*  @fuction: Proto_GameScene
	*  @brief: ��Ҷ�����������,���·��ͽ����Ϣ.
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
	*  @brief: ����������еĽ���װ�.
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
	*  @brief: ��ҵغ�,��������ж�.
	*  @return:
	**/
	bool IsQiangJinHu(_uint8 ChairID);
	/**
	*  @fuction: QiangJinHuCheck_Hu
	*  @brief: .��Ϊcheck_hu �Ĺ��ź�Ӱ��У�飬��д�ú���
	*  @return:
	**/
	int QiangJinHuCheck_Hu(_uint8 ChairID, HandCards& pHc, MJ_fan_buffer& oFan);
	/**
	*  @fuction: UserAfterOperate
	*  @brief: �����֮��,����,���������ǽ�ʱ,��Ҫ����װ�Ļ���.
	*  @return:
	**/
	void UserAfterOperate(_uint8 ChairID, int iOpType, _uint8 bCard);
	/**
	*  @fuction: Proto_GameEndInfo
	*  @brief: �㷬��Ϣ
	*  @return:
	**/
	void Proto_GameEndInfo(const MJ_user_buffer& UserID, proto::game::AckGameEnd& stGameend);
	/**
	*  @fuction: Calc_BaseScore
	*  @brief: �㷬��Ϣ
	*  @return:
	**/
	void Calc_BaseScore(_uint8 ChairID, int iBaseScore[GAME_PLAYER]);
	/**
	*  @fuction: Calc_HuScore
	*  @brief: �㷬��Ϣ
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
	*  @brief: ��������κ�һ�����������3���𣬼��ɺ��ƣ������κ�����
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

	//У������
	void CheckUserHandCards(_uint8 ChairID, _uint8 bCard[20], int cards_len);

	void RobGangPriority(_uint8 ChairID, _uint8 CardData, int OperateType);


	static void GetCardOperateOutCard_Timer_cb(struct ev_loop *loop, struct ev_timer *w, int revents);

	static void GetCardOperateHu_Timer_cb(struct ev_loop *loop, struct ev_timer *w, int revents);

	static void YunXiaoYouJinOperateHu_Timer_cb_0(struct ev_loop *loop, struct ev_timer *w, int revents);

	static void YunXiaoYouJinOperateHu_Timer_cb_1(struct ev_loop *loop, struct ev_timer *w, int revents);

	//����֮��������ҳ�ʱ�����û�к�����ֱ��pass��
	static void OutCardOperatePass_Timer_cb_0(struct ev_loop *loop, struct ev_timer *w, int revents);
	static void OutCardOperatePass_Timer_cb_1(struct ev_loop *loop, struct ev_timer *w, int revents);
	static void OutCardOperatePass_Timer_cb_2(struct ev_loop *loop, struct ev_timer *w, int revents);
	static void OutCardOperatePass_Timer_cb_3(struct ev_loop *loop, struct ev_timer *w, int revents);
	//����֮��������ҳ�ʱ�����û�к�����ֱ��pass��
	static void OutCardOperateHu_Timer_cb_0(struct ev_loop *loop, struct ev_timer *w, int revents);
	static void OutCardOperateHu_Timer_cb_1(struct ev_loop *loop, struct ev_timer *w, int revents);
	static void OutCardOperateHu_Timer_cb_2(struct ev_loop *loop, struct ev_timer *w, int revents);
	static void OutCardOperateHu_Timer_cb_3(struct ev_loop *loop, struct ev_timer *w, int revents);
	void stop_yunxiao_operator_timer_stop(_uint8 ChairID);

	static void ChiPengCardOperateOutCard_Timer_cb(struct ev_loop *loop, struct ev_timer *w, int revents);
	//�Զ�����ǰ�����͸��ͻ��ˣ���ֹ����
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
	_uint8		m_bJinCard;						//����
	_uint8		m_bUserHasJinNum[GAME_PLAYER];	  //���ӵ�н��Ƶ�����
	_uint8      m_bReplaceJinCard;				 //�����ν�İװ�	
	_uint8      m_bHasReplaceJinCardNum[GAME_PLAYER];//���ӵ�аװ������
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
	//���ν�
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
	bool        m_bOutTimeStopOperateOutCardFlag[GAME_PLAYER];//��������������Ѿ���ʱ�����ֹ���ơ�
	_uint8		m_bUserOutCardFlag[GAME_PLAYER];						//�����ƣ��ڹ���֮ǰ��������
	bool        m_bUserCheckDoMultiOperateiOpTypeFlag[GAME_PLAYER];
	//_uint8		m_bUserChiCardOutCardFlag[GAME_PLAYER];//��ĳ���ƣ���ֹ�����
	MJ_fan_buffer m_bUserHuTypeoFan[GAME_PLAYER];
	bool		m_bUserHuQingYiSeFlag[GAME_PLAYER];
	bool		m_bUserOperateShuangYouJinZhaungThreeFlag[GAME_PLAYER];
	//������Ƴ�ʱ���������
	ev_timer			m_GetCardOperateOutCard_Timer;
	ev_tstamp			m_GetCardOperateOutCard_Timer_stamp;
	ev_timer			m_GetCardOperateHu_Timer;
	ev_tstamp			m_GetCardOperateHu_Timer_stamp;
	//���ν�
	//����ν�ʱ�����ν�
	ev_timer			m_YouJinOperateHuYouJin_Timer_0;
	ev_tstamp			m_YouJinOperateHuYouJin_Timer_stamp_0;
	ev_timer			m_YouJinOperateHuYouJin_Timer_1;
	ev_tstamp			m_YouJinOperateHuYouJin_Timer_stamp_1;

	//��ҳ��ƺ�������ҳ�ʱѡ��pass��
	ev_timer			m_OutCardOutTimeOperatePass_Timer_0;
	ev_tstamp			m_OutCardOutTimeOperatePass_Timer_stamp_0;
	ev_timer			m_OutCardOutTimeOperatePass_Timer_1;
	ev_tstamp			m_OutCardOutTimeOperatePass_Timer_stamp_1;
	ev_timer			m_OutCardOutTimeOperatePass_Timer_2;
	ev_tstamp			m_OutCardOutTimeOperatePass_Timer_stamp_2;
	ev_timer			m_OutCardOutTimeOperatePass_Timer_3;
	ev_tstamp			m_OutCardOutTimeOperatePass_Timer_stamp_3;

	//��ҳ��ƺ�������ҳ�ʱ������ƣ����Զ�����
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

