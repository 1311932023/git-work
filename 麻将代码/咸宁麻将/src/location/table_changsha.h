#ifndef __TABLE_CHANGSHA_H__
#define __TABLE_CHANGSHA_H__
#include "../table.h"

#define CHANGSHA_SHAIZI_COUNT		(2)
#define CHANGSHA_GANGHOU_CARD_MAX	(6)
#define CHANGSHA_DAHU_FAN			(6)

struct mj_conf_hunan
{
	bool hu_wei_da;
	bool changsha_qishouhu;
	bool dan_se_yi_zhi_hua;
	bool jiang_yi_zhi_hua;
	bool yi_zhi_niao;
	bool san_tong;
	bool jie_jie_gao;
	bool zhong_tu_si_xi;
	bool hai_di_niao;
	bool hai_di_niao_quan_zhong;
	bool bao_liu_hai_di_niao;
	bool gang_hou_zhi_neng_hu0;
	bool gang_hou_zhi_neng_hu1;
	bool men_qing;	
	bool jin_tong_yu_nv;
	bool yi_dian_hong;
	int gang_hou_bu_zhang;	//杠后补张数
	int feng_ding;
	mj_conf_hunan()
	{
		hu_wei_da = false;
		changsha_qishouhu = false;
		dan_se_yi_zhi_hua = false;
		jiang_yi_zhi_hua = false;
		yi_zhi_niao = false;
		san_tong = false;
		zhong_tu_si_xi = false;
		hai_di_niao = false;
		jie_jie_gao = false;
		hai_di_niao_quan_zhong = false;
		gang_hou_zhi_neng_hu0 = false;
		gang_hou_zhi_neng_hu1 = false;
		men_qing = false;
		bao_liu_hai_di_niao = false;
		jin_tong_yu_nv = false;
		yi_dian_hong = false;

		gang_hou_bu_zhang = 2;
		feng_ding = -100000000;
	}
};


class Table_changsha : public Table
{
public:
	Table_changsha();
	~Table_changsha();
public:

	void ConfInit_Remote(const Json::Value& val);

	void GameStart();
	
	void GameEnd();

	void GameEndReset();	

	bool CheckUserOutCard(_uint8 ChairID, _uint8 cbCard);

	bool CheckUserOperator(_uint8 ChairID, int iOpType, _uint8 bCard);

	void UserLastOutCard(_uint8 ChairID, int& iOpType, MJ_fan_buffer& oFan);	
	
	void UserGangShangHua(_uint8 ChairID, _uint8 bCard, MJ_fan_buffer& oFan);
	
	void UserGangHouPao(_uint8 ChairID, _uint8 bCard, MJ_fan_buffer& oFan);

	void UserQiangGangHu(_uint8 ChairID, _uint8 bCard, MJ_fan_buffer& oFan);

	int OutCardEstimate(_uint8 ChairID, _uint8 bCard, MJ_major& major, int& fan_num);

	int GetCardEstimate(_uint8 ChairID, _uint8 bCard, MJ_major& major, int& fan_num);

	int Check_Hu(_uint8 ChairID, HandCards& pHc, MJ_fan_buffer& oFan);

	bool Is7Dui(_uint8 ChairID, HandCards& pHc, MJ_fan_buffer& oFan);

	bool IsQuanqiuren(const HandCards& pHc);
	/*
	 *	检查牌有没有胡
	 */
	int Check_Hu_Pattern(_uint8 ChairID, const HandCards& pHc, MJ_fan_buffer& oFan, const MJ_win_pattern& oPattern);
	/*
	 *	胡牌类型是几番，返回胡牌类型的总番数
	 */
	int Calc_Fan(_uint8 ChairID, MJ_fan_buffer& oFan);
	/*
	 *	胡牌类型是几番，返回胡牌类型的番数
	 */	
	int Calc_Hu(_uint8 ChairID, _uint8 bCard, int Score[GAME_PLAYER]);	
	/*
	 *	初始化庄家
	 */
	void InitBanker();
	/*
	 *	桌面庄家轮庄
	 */
	void TurnBanker();

	void UserOperateChi(_uint8 ChairID, int iOpType, _uint8 bCard);

	void UserOperatePeng(_uint8 ChairID, _uint8 bCard);
	/*
	 *	玩家执行杠操作
	 */
	void UserOperateGang(_uint8 ChairID, int iOpType, _uint8 bCard);
	/*
	 *	玩家胡
	 */
	void UserOperateHu(_uint8 ChairID, _uint8 bHuCard /* = 0xFF */);
	/*
	 *	多个玩家有提示
	 */
	void DoMultiOperate(_uint8 ChairID, int iOpType, _uint8 bCard);
	/*
	 *	只有一个玩家有提示
	 */
	void DoSimpleOperate(_uint8 ChairID, int iOpType, _uint8 bCard);
	void MultiOperateOver(_uint8 chairID, int iOpType, _uint8 bCard);
	void OnUserGetCard(_uint8 ChairID, bool bGetLast = false);
	void OnUserOutCard(_uint8 bOutChairID, _uint8 bOutCard);

	void Score_GameEndSucc(HuScoreSet& score, proto::game::AckGameEnd& stGameend);	

	void SetRobGangHu_Flag(_uint8 ChairID, int iOpType, proto::game::AckBcOpResult& stOpresult);

	void Proto_GameScene(_uint8 ChairID, Player* player, proto::game::AckGameScene& stGameScene);
	void Proto_GameScene_Operate(_uint8 ChairID, Player* player, proto::game::AckGameScene& stGameScene);
	void Proto_GameScene_lookon(_uint8 ChairID, Player* player, proto::game::AckGameScene& stGameScene);
	void Proto_GameEndInfo(const MJ_user_buffer& UserID, proto::game::AckGameEnd& stGameend);
	void Proto_GameEndInfo_GangInfo(_uint8 ChairID, proto::game::AckGameEnd& stGameend);
	void Proto_Game_OpNotify(_uint8 ChairID, _uint8 OutChairID, _uint8 bCard, proto::game::AckOpNotify& stNotify);
	void Proto_Game_OpNotifyAfterPengChi(_uint8 ChairID, _uint8 CurChairID, const MJ_opinfo& gang, proto::game::AckOpNotify& stNotify);
	void Proto_Game_UserGetCard(_uint8 ChairID, _uint8 CurChairID, _uint8 bCard, proto::game::AckUserGetCard& stUsergetcard);
	void Proto_Game_OperateGang(_uint8 ChairID, _uint8 bCard, int iOpType, int iGangScore[GAME_PLAYER], proto::game::AckBcOpResult& stOpresult);


public:	
	typedef MJ_BUFFER<_uint32, 4> MJ_qishou_fan;
	
	bool Hu_Special4Hunan(_uint8 ChairID, MJ_qishou_fan& oFan, MJ_hand_buffer& hc);
	
	bool QishouHu();	
	void QishouZhongNiaoSuanFen(_uint8 ChairID, _uint8 niaonum, Shai_Set shai);
	void Send_QishouDaShaizi(Shai_Set shai);	
	void Send_BanpaiInfo(_uint8 ChairID = INVALID_CHAIRID);
	void Send_BanpaiOver(_uint8 ChairID);

	static void Qishou_Timer_cb(struct ev_loop *loop, struct ev_timer *w, int revents);
	static void Qishou_Timer_cb_0(struct ev_loop *loop, struct ev_timer *w, int revents);

public:
	bool IsListening(_uint8 ChairID, _uint8 bRemoveCard);

private:	
	ev_timer			m_qishouhu_Timer;
	ev_tstamp			m_qishouhu_Timer_stamp;
	ev_timer			m_qishouhu_Timer_0;
	ev_tstamp			m_qishouhu_Timer_stamp_0;

	MJ_qishou_fan		m_qishou_fan[GAME_PLAYER];
	MJ_hand_buffer		m_qishou_banpai[GAME_PLAYER];
	//MJ_user_buffer		m_qishou_zhongniao_user;
	int					m_qishou_score[GAME_PLAYER];
	Shai_Set			m_qishou_shai;
	bool				m_qishou_state;
	

public:
	void Send_HaidiCardMsg(_uint8 ChairID);
protected:
	MJ_user_buffer m_HaidiUser;
	_uint8 m_HaidiPlayer;
	

protected:
	mj_conf_hunan m_conf_changsha;



public:
	void Calc_BaseScore(_uint8 ChairID, int iBaseScore[GAME_PLAYER]);
	void Calc_HuScore(_uint8 ChairID, int iBaseScore[GAME_PLAYER], int iScore[GAME_PLAYER]);
	void Calc_ZhongNiao(_uint8 ChairID, _uint8 ShaiNum, int iBaseScore[GAME_PLAYER], int iScore[GAME_PLAYER]);
	void Calc_FengDing(_uint8 ChairID, int iScore[GAME_PLAYER]);
	

protected:
	struct BuzhangInfo
	{
		MJ_BUFFER<_uint8, 4> buzhang;
		bool ting_ed;	//是否报听		
	};
	BuzhangInfo m_buzhang[GAME_PLAYER];

public:
	void OnUserGetMultiCard(_uint8 ChairID, _uint8 iGetCount, int iOpTypeFilter = (TYPE_CHI_PENG_GANG | TYPE_HU | TYPE_BU_ZHANG));
	void OnUserOutMultiCard(_uint8 ChairID, _uint8 iGetCount, int iOpTypeFilter = (TYPE_CHI_PENG_GANG | TYPE_HU | TYPE_BU_ZHANG));
	void Proto_UserOutMultiCard(_uint8 ChairID, proto::game::hunanmj::tagGang_Zhuapai& zhuapai, int iOpTypeFilter);

	void Send_GangDashaizi(_uint8 ChairID, Shai_Set shai, bool zhong);		
	static void Gang_Dashaizi_Timer_cb(struct ev_loop *loop, struct ev_timer *w, int revents);
	void GangHouClear();
	
protected:
	_uint8 m_cur_gang_player;
	int m_Gang_buzhang_res;//杠后有几张
	Shai_Set m_gang_shai;
	MJ_BUFFER<_uint8, CHANGSHA_GANGHOU_CARD_MAX> m_gang_hou_card;
	MJ_BUFFER<int, CHANGSHA_GANGHOU_CARD_MAX> m_gang_hou_op[GAME_PLAYER];
	MJ_fan_buffer m_MultiFan[GAME_PLAYER];
	int m_MultiHufanNums[GAME_PLAYER];	

	ev_timer m_gang_dashaizi_Timer;
	ev_tstamp m_gang_dashaizi_Timer_stamp;
	
protected:
	/*
	 *	杠后杠
	 *	胡为大
	 */	
	bool m_HuWeiDa;
	_uint8 m_HuWeiDaCard;
	int m_HuWeiDaOp;
	MJ_BUFFER<_uint8, 5> m_SiXi[GAME_PLAYER];
	bool Check_HuWeiDa(_uint8 ChairID, _uint8 iGetCount);	
	void DoMultiOperate_HuWeiDa(_uint8 ChairID, int iOpType, _uint8 bCard);
public:
	void save_video_operate(int iOpType, _uint8 chairID, _uint8 paschairID, _uint8 cbCard, int score[GAME_PLAYER]);			

};

#endif