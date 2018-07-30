#ifndef __TABLE_CHANGSHA_H__
#define __TABLE_CHANGSHA_H__
#include "../table.h"
#include "le_majiang.h"

#define CHANGSHA_SHAIZI_COUNT		(2)
#define CHANGSHA_GANGHOU_CARD_MAX	(6)

#define CHANGSHA_DAHU_FAN			(8)



class Changsha_Fantype_to_FanNum : public Fantype_to_FanNum
{
public:
	Changsha_Fantype_to_FanNum()
	{
		fan_num[MJ_FAN_TYPE_PENGPENGHU] = 8;
		fan_num[MJ_FAN_CHANGSHA_JIANGJIANGHU] = 8;
		fan_num[MJ_FAN_TYPE_QINGYISE] = 8;
		fan_num[MJ_FAN_TYPE_HAIDILAOYUE] = 8;   // 4  
		fan_num[MJ_FAN_TYPE_HAIDIPAO] = 8;
		fan_num[MJ_FAN_TYPE_QIDUI] = 8;   //66
		fan_num[MJ_FAN_HAO_HUA_THREE_QI_XIAO_DUI] = 32;
		fan_num[MJ_FAN_HAO_HUA_DOUBLE_QI_XIAO_DUI] = 24;
		fan_num[MJ_FAN_HAO_HUA_QI_XIAO_DUI] = 16;               ////999999999999

		fan_num[MJ_FAN_TYPE_GANGSHANGHUA] = 8;
		fan_num[MJ_FAN_TYPE_QIANGGANGHU] = 8;
		fan_num[MJ_FAN_TYPE_GANGHOUPAO] = 8;   // 12        1313 

		fan_num[MJ_FAN_TYPE_QUANQIUREN] = 8;
		fan_num[MJ_FAN_TYPE_MENQIANQING] = 8;
		fan_num[MJ_FAN_TYPE_TIANHU] = 8;
		fan_num[MJ_FAN_TYPE_DIHU] = 8;  //16   17 

		fan_num[MJ_FAN_TYPE_JIANGDUI] = 4;
		fan_num[MJ_FAN_BAOTING_JIE_PAO] = 8;
		fan_num[MJ_FAN_BAOTING_ZIMO] = 8;
		fan_num[MJ_FAN_BAOTING_FANG_PAO] = 8;    // 21 

		SetFan(MJ_FAN_CHANGSHA_SIXIHU, 4);
		SetFan(MJ_FAN_CHANGSHA_BANBANHU, 4);
		SetFan(MJ_FAN_CHANGSHA_QUEYIMEN, 4);
		SetFan(MJ_FAN_CHANGSHA_LIULIUSHUN, 4);
		SetFan(MJ_FAN_CHANGSHA_JIEJIEGAO, 4);
		SetFan(MJ_FAN_NINGXIANG_JINTONGYUNV, 4);
		SetFan(MJ_FAN_CHANGSHA_DANSEYIZHIHUA, 4);
		SetFan(MJ_FAN_CHANGSHA_SANTONG, 4);               //29 
		SetFan(MJ_FAN_NINGXIANG_YIDIANHONG, 4);   //30 

	}
	bool FanFilter(const MJ_fan_buffer& oFan, MJ_fan_buffer& oOutFan)   // 过滤 小胡 如果有大胡的番。
	{
		oOutFan.Clear();
		map<int, int>  nFanMap;
		bool HaoHuaQidui = false;
		for (int i = 0; i < oFan.Size(); i++)
		{
			log_debug("fan [%d]  getfan[%d] ", oFan[i], GetFan(oFan[i]));
			if (!king)
			{
				if (GetFan(oFan[i]) >= 8)
				{
					nFanMap[oFan[i]] = 1;
				}
				if (GetFan(oFan[i]) >= 16)
				{
					HaoHuaQidui = true;
				}
			}
			else
			{
				if (GetFan(oFan[i]) >=  7 )
				{
					if (oFan[i] == MJ_FAN_SI_CHUN_kING &&  nFanMap[oFan[i]] == 0  )
						nFanMap[oFan[i]] = 1;
					if (oFan[i] == MJ_FAN_SI_CHUN_kING &&  nFanMap[oFan[i]] ==  1  )
						nFanMap[oFan[i]]++;
					else
						nFanMap[oFan[i]] = 1; 
					
				}
				if (GetFan(oFan[i]) >= 14 )
				{
					HaoHuaQidui = true;
				}
			}

		}
		for (map<int, int>::iterator it = nFanMap.begin(); it != nFanMap.end(); ++it)
		{

			oOutFan.Add(it->first); 		
		}
		if (HaoHuaQidui == true)
		{
			oOutFan.Remove(MJ_FAN_TYPE_QIDUI);
			//log_debug("into !");

		}

		return !(oOutFan.IsNull());//根据
	}
	int operator[](int fan)const
	{
		return GetFan(fan);
	}
	int GetFan(int fan)const
	{
		if (!king)
		{
			switch (fan)
			{
			case MJ_FAN_TYPE_PENGPENGHU:
				return 8;
			case  MJ_FAN_CHANGSHA_JIANGJIANGHU:
				return 8;
			case  MJ_FAN_TYPE_QINGYISE:
				return 8;
			case  MJ_FAN_TYPE_HAIDILAOYUE:
				return 8;                                  //44 

			case MJ_FAN_TYPE_HAIDIPAO:
				return  8;
			case MJ_FAN_TYPE_QIDUI:
				return 8;
			case  MJ_FAN_HAO_HUA_QI_XIAO_DUI:
				return 16;
			case  MJ_FAN_HAO_HUA_DOUBLE_QI_XIAO_DUI:
				return 24;
			case  MJ_FAN_HAO_HUA_THREE_QI_XIAO_DUI:
				return 32;                            /////99999999 


			case MJ_FAN_TYPE_GANGSHANGHUA:
				return 8;
			case MJ_FAN_TYPE_QIANGGANGHU:
				return 8;
			case  MJ_FAN_TYPE_GANGHOUPAO:
				return 8;                           //  1313 


			case  MJ_FAN_TYPE_QUANQIUREN:
				return 8;
			case MJ_FAN_TYPE_MENQIANQING:
				return 8;
			case MJ_FAN_TYPE_TIANHU:
				return 8;
			case MJ_FAN_TYPE_DIHU:                       /// 1717  
				return 8;


			case MJ_FAN_BAOTING_JIE_PAO:
				return 8;
			case MJ_FAN_BAOTING_ZIMO:
				return 8;
			case  MJ_FAN_BAOTING_FANG_PAO:                  //2121 
				return 8;

			case  MJ_FAN_TYPE_JIANGDUI:     //将对。。 
				return  4;
			case MJ_FAN_CHANGSHA_SIXIHU:
				return  4;
			case   MJ_FAN_CHANGSHA_BANBANHU:
				return   4;
			case  MJ_FAN_CHANGSHA_QUEYIMEN:
				return  4;
			case  MJ_FAN_CHANGSHA_LIULIUSHUN:              //2525 
				return  4;
			case MJ_FAN_CHANGSHA_JIEJIEGAO:
				return  4;
			case  MJ_FAN_NINGXIANG_JINTONGYUNV:
				return  4;
			case  MJ_FAN_CHANGSHA_DANSEYIZHIHUA:
				return  4;
			case MJ_FAN_CHANGSHA_SANTONG:
				return  4;

			case  MJ_FAN_NINGXIANG_YIDIANHONG:              ////  3030 
				return   4;


			default:
				break;
			}
			return  8;
		}
		else
		{
			switch (fan)
			{
			case MJ_FAN_TYPE_PENGPENGHU:
				return 7;
			case  MJ_FAN_CHANGSHA_JIANGJIANGHU:
				return 7;
			case  MJ_FAN_TYPE_QINGYISE:
				return 7;

			case MJ_FAN_TYPE_QIDUI:
				return 7;
			case  MJ_FAN_HAO_HUA_QI_XIAO_DUI:
				return 14;
			case  MJ_FAN_HAO_HUA_DOUBLE_QI_XIAO_DUI:
				return 21;
			case  MJ_FAN_HAO_HUA_THREE_QI_XIAO_DUI:
				return 28;                            /////99999999 
			case MJ_FAN_TYPE_GANGSHANGHUA:
				return 7;
			case MJ_FAN_TYPE_QIANGGANGHU:
				return 7;
			case  MJ_FAN_TYPE_GANGHOUPAO:
				return 7;                           //  1313 
			case  MJ_FAN_TYPE_QUANQIUREN:
				return 7;
			case MJ_FAN_TYPE_MENQIANQING:
				return 7;
			case MJ_FAN_TYPE_TIANHU:
				return 7;
			case MJ_FAN_TYPE_DIHU:                       /// 1717  
				return 7;

			case MJ_FAN_BAOTING_JIE_PAO:
				return 7;
			case MJ_FAN_BAOTING_ZIMO:
				return 7;
			case  MJ_FAN_BAOTING_FANG_PAO:                  //2121 
				return 7;

			case  MJ_FAN_TYPE_JIANGDUI:     //将对。。 
				return  2;
			case   MJ_FAN_CHANGSHA_BANBANHU:		 // 板板胡。 		
				return  7;
			case  MJ_FAN_CHANGSHA_QUEYIMEN:    // 缺一门 
				return  7;
			case MJ_FAN_CHANGSHA_DANSEYIZHIHUA:    // 一枝花。 
				return 7;

			case MJ_FAN_SAN_ZHANG_KING:   //三正王
				return  7;
			case MJ_FAN_SI_CHUN_kING:  // 四纯王
				return 7;

			case MJ_FAN_NO_KING_FAN:
				return 7;

			default:
				break;
			}
			return  8;
		}

	}
	int  king;
};


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

	bool gang_hou_zhi_neng_hu0;    // 这个代码。不知道是谁写的。  注释也没写。我也没看懂。 好像是， 只能胡。 
	bool gang_hou_zhi_neng_hu1;

	bool men_qing;	
	bool jin_tong_yu_nv;
	bool yi_dian_hong;
	int gang_hou_bu_zhang;	//杠后补张数
	int feng_ding; 
	
	bool ping_hu_bu_jie_pao;
	bool  quan_kaifang;
	bool  ban_kaifang;
	bool   kai_king;
	mj_conf_hunan()
	{
		hu_wei_da = true;
		changsha_qishouhu = true;
		dan_se_yi_zhi_hua = false;
		jiang_yi_zhi_hua = false;
		yi_zhi_niao = false;
		san_tong = true;
		zhong_tu_si_xi = false;  // 默认。 没有中途四喜 。
		hai_di_niao = false;
		jie_jie_gao = true;
		hai_di_niao_quan_zhong = false;   // 海底一只鸟。 

		gang_hou_zhi_neng_hu0 =  true;
		gang_hou_zhi_neng_hu1 =  true ;

		men_qing = false;    //门清默认是 false 
		bao_liu_hai_di_niao = false;
		jin_tong_yu_nv = false;
		yi_dian_hong =  true;
		quan_kaifang = false;
		ban_kaifang = false;
		
		ping_hu_bu_jie_pao = false;
		gang_hou_bu_zhang = 2;
		kai_king = false;
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

	int CheckOutCardOtherPlayerCanOpCode(_uint8 ChairID, _uint8 bCard, MJ_major& major, int& fan_num);

	int CheckPlaySlefGrpCardCanOperateCode(_uint8 ChairID, _uint8 bCard, MJ_major& major, int& fan_num);

	int Check_Hu(_uint8 ChairID, HandCards& pHc, MJ_fan_buffer& oFan , _uint8 add_last_card = 0  );

	bool Is7Dui(_uint8 ChairID, HandCards& pHc, MJ_fan_buffer& oFan);

	// 单调胡牌。 

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
	int  UserOperateHu(_uint8 ChairID, _uint8 bHuCard /* = 0xFF */);
	/*
	 *	多个玩家有提示
	 */
	void DoMultiOperate(_uint8 ChairID, int iOpType, _uint8 bCard);
	/*
	 *	只有一个玩家有提示
	 */
	void DoSimpleOperate(_uint8 ChairID, int iOpType, _uint8 bCard);
	
	void MultiOperateOver(_uint8 chairID, int iOpType, _uint8 bCard);
	
	void OnUserGrapCard(_uint8 ChairID, bool bGetLast = false);

	void OnUserOutCard(_uint8 bOutChairID, _uint8 bOutCard);

	void  OnRecvQishouHuBaiPai_Operate(int sid , int op  );

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
	typedef MJ_BUFFER<_uint32,  8> MJ_qishou_fan;
	
	bool Hu_Special4Hunan(_uint8 ChairID, MJ_qishou_fan& oFan, MJ_hand_buffer& hc);
	
	bool QishouHu();	

	void QishouZhongNiaoSuanFen(_uint8 ChairID, _uint8 niaonum, Shai_Set shai);
	
	

	void Send_BanpaiInfo(_uint8 ChairID = INVALID_CHAIRID);
	void Send_BanpaiOver(_uint8 ChairID);

	static void Qishou_Timer_cb(struct ev_loop *loop, struct ev_timer *w, int revents);
	static void Qishou_Timer_cb_0(struct ev_loop *loop, struct ev_timer *w, int revents); 
	static void qishou_baipai_sel_timer_cb(struct ev_loop *loop, struct ev_timer *w, int revents);
	static void baojiao_sel_Timer_cb(struct ev_loop *loop, struct ev_timer *w, int revents);
	


public:
	bool IsListening(_uint8 ChairID, _uint8 bRemoveCard);

private:	
	ev_timer			m_StartBankOutCardTimer; 
	ev_tstamp			m_StartBankOutCard_Stamp;   

	ev_timer            m_qishou_timer_baiPai_sel;
	ev_tstamp           m_qishou_timer_baipai_sel_stamp; 

	ev_timer            m_baoJiao_Timer;
	ev_tstamp           m_baoJiao_Timer_stamp ;

	

	ev_timer			m_qishouhu_Timer_0;
	ev_tstamp			m_qishouhu_Timer_stamp_0;

	MJ_qishou_fan		m_qishou_fan[GAME_PLAYER];
	
	int					m_qishou_score[GAME_PLAYER];
	Shai_Set			m_qishou_shai;
	bool				m_qishou_state;
	_uint8              m_canBaiPaiPlayerCount;
	

	_uint8              m_nCanBaoTingCount;
	//int                  
	
	

public:
	void Send_HaidiCardMsg(_uint8 ChairID);
protected:
	MJ_user_buffer m_HaidiUser;
	_uint8         m_HaidiPlayer;
	

protected:
	mj_conf_hunan m_conf_changsha;



public:
	void Calc_Hu_Player_Base_Score(_uint8 ChairID, int iBaseScore[GAME_PLAYER]);

	void Calc_HuScore(_uint8 ChairID, int iBaseScore[GAME_PLAYER], int iScore[GAME_PLAYER]);
	
	void Calc_ZhongNiao(_uint8 ChairID, _uint8 ShaiNum, int iBaseScore[GAME_PLAYER], int iScore[GAME_PLAYER]);
	
	void Calc_FengDing(_uint8 ChairID, int iScore[GAME_PLAYER]);
	

protected:
	struct BuzhangInfo
	{
		MJ_BUFFER<_uint8, 4> buzhang;
		bool				 bao_ting;	//是否报听		
		bool                 is_first_bao_ting; 
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
	_uint8 m_curGangPlayerSid;

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
	
	bool is_gang_shang_kai_hua(_uint8 chairID, bool zimo);

	bool is_qiang_gang_hu();

	int is_258_jiang(_uint8 jiang);
	
	void send_msg_to_client_bai_pai_sel();
	
	bool dihu_judge();

	int  DealBaoTingOp();    // 报听操作 。。

	void OnRecvBaoTingOp(int sid, int op);
	
	bool IsCanGangAfterTing(_uint8 sid, MJ_major & major , _uint8 add_card );
	
	void AddOtherFan(_uint8 ChairID, const HandCards& pHc, MJ_fan_buffer& oFan, const MJ_win_pattern& oPattern);
	int hu_if_remove_one_can_card(_uint8 ChairID);
	void AfterQishouBaoTing();
	int GetNextJuBanker();
	void DealNingxiangHuBroad(_uint8 chairid, MJ_fan_buffer & fan);
	bool other_chu_pai();
	int GetChunKingZhengKing(int card, int &pre_card, int &next_card);
	bool hu_qishouKaiKing(_uint8 ChairID, MJ_qishou_fan &ofan , MJ_hand_buffer &hc);

	int KingHu(_uint8 ChairID);
	int   m_last_card_pos;
	_uint8   m_zhengwang;
	_uint8   m_chunwanga; 
	_uint8   m_chunwangb;
	int      m_cannothu[GAME_PLAYER];
	int      m_fengdong[GAME_PLAYER];

	int  ProtoNingXiangFengDong(int op);
	int no_king_fan(_uint8 ChairID);
	int IsJiangJiangHu_King(const HandCards *phand);
	bool Check_Listening(HandCards *pHc, _uint8 ChairID, _uint8 bCard, tagListenInfos stListenInfo[14], _uint8 &bListenLen);

	void OnRecvUserOperate(_uint8 chairid, int iOpType, int bCard);
	int pengpenghu_king(_uint8 ChairID, const HandCards *pHc, const MJ_win_pattern& oPattern, int nlaizi);
	int NoQiDui(MJ_fan_buffer &oFan);
	
	void PackageTingInfo(_uint8 ChairID, tagListenInfos stListenInfo[14], _uint8 iPos, proto::game::tagListenInfo* listeninfo);
	int GetGangInfo(_uint8 ChairID, MJ_opinfo& GangCardInfo);
	Changsha_Fantype_to_FanNum g_changsha_ft2fn;

	int    m_speGang[GAME_PLAYER]; 

	int    all_lazi; 
	int    m_ningxiag_guozhang[GAME_PLAYER];   // md 傻逼。。过张胡。写成那个鸟样。 

};

#endif