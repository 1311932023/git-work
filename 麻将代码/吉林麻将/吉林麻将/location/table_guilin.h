#ifndef __TABLE_GUILIN_H__
#define __TABLE_GUILIN_H__

#include "../table.h"
#include "table_changsha.h"
#include "le_majiang.h"

//   1天杠，2地杠。 3天追 。 4普通追   5 普通杠 
enum 
{
	BASE_ZERO = 0 ,
	TIAN_GANG = 1 , 
	DI_GANG = 2 ,  
	TIAN_ZHUI  =  3 ,
	PU_TONG_ZHUI =  4 , 
	PU_TONG_GANG = 5 , 
};


struct  guilin_conf  
{
	bool  yi_gang_dao_di;
	bool  quan_qiu_ren_buqiuzi;
	bool  haidi_laoyue2;
	bool  menqing;
	bool  chisandou_hubao;
	bool  quanquan_zhui;

	bool  shangzhongxia_xing;
	bool  shangxia_xing;


};

struct xing_info
{
	_uint8  sid;
	_uint8  xing;
	_uint8  all_xing;
};

struct  cur_lun_xing_info
{
	int                 lun;
	_uint8              getcard;
	vector<xing_info>   arr;
};


class Guilin_Fantype_to_FanNum : public Fantype_to_FanNum
{
public:
	Guilin_Fantype_to_FanNum()
	{		
	}
	bool FanFilter_Guilin(const MJ_fan_buffer& oFan, MJ_fan_buffer& oOutFan  , MJ_fan_buffer  &OtherFan  )   // 过滤 小胡 如果有大胡的番。
	{
		oOutFan.Clear();
		map<int, int> BigHumap;
		map<int, int>  SmallHuMap;
		for (int i = 0; i < oFan.Size()  ;i++)
		{
			int t = oFan[i];  			

			if (t == MJ_FAN_TYPE_PENGPENGHU || t == MJ_FAN_TYPE_QINGYISE || t == MJ_FAN_ZI_YI_SE || t == MJ_FAN_TYPE_QUANQIUREN
				|| t == MJ_FAN_QI_XI_GUI_LIN || t == MJ_FAN_TYPE_QIDUI || t == MJ_FAN_HAO_HUA_QI_XIAO_DUI
				|| t == MJ_FAN_HAO_HUA_DOUBLE_QI_XIAO_DUI || t == MJ_FAN_HAO_HUA_THREE_QI_XIAO_DUI
				)
			{
				BigHumap[t] = 1;
			}
			else
			{
				SmallHuMap[t] = 1;
			}
			
		}
		map<int, int>::iterator  it = BigHumap.begin();
		for (; it != BigHumap.end(); ++it)
		{
			oOutFan.Add(it->first);
		}
		for (map<int, int>::iterator iter = SmallHuMap.begin(); iter != SmallHuMap.end();++iter)
		{
			OtherFan.Add(iter->first);			
		}
		OtherFan.Remove(MJ_FAN_TYPE_JIANGDUI);
		return true; 
	}

	bool FanFilter(const MJ_fan_buffer& oFan, MJ_fan_buffer& oOutFan)   // 过滤 小胡 如果有大胡的番。
	{
		oOutFan.Clear();
		map<int, int>  nFanMap;
		for (int i = 0; i < oFan.Size(); i++)
		{
			log_debug("fan [%d]", oFan[i]);						
			

			nFanMap[oFan[i]] = 1;			
		}
		for (map<int, int>::iterator it = nFanMap.begin(); it != nFanMap.end(); ++it)
		{
			oOutFan.Add(it->first);
			log_debug("ofan[%d]", it->first);
		}
		return true;       //根据
	}

	int GetFan(int fan)const
	{		
		return 8;
	}

};


class Table_guilin : public Table_changsha
{
public:

	Table_guilin(int player);
	~Table_guilin();

public:
	void ConfInit_Remote(const Json::Value& val);
	
	int Check_Hu_Pattern(_uint8 ChairID, const HandCards& pHc, MJ_fan_buffer& oFan, const MJ_win_pattern& oPattern);
	void OnUserOutCard(_uint8 ChairID, _uint8 bOutCard);
	void OnUserGrapCard(_uint8 ChairID, bool bGetLast);

	
	void Calc_Hu_Player_Base_Score(_uint8 ChairID, int iBaseScore[GAME_PLAYER]);

	
	void TurnBanker();
	int CheckPlaySlefGrpCardCanOperateCode(_uint8 ChairID, _uint8 bCard, MJ_major& major, int& fan_num);
	void GameStart();
	int CheckOutCardOtherPlayerCanOpCode(_uint8 ChairID, _uint8 bCard, MJ_major& major, int& fan_num);
	
	int Check_Hu(_uint8 ChairID, HandCards& pHc, MJ_fan_buffer& oFan, _uint8 add_last_card =0 );
	
	bool Is7Dui(_uint8 ChairID, HandCards& pHc, MJ_fan_buffer& oFan);
	
	void DoSimpleOperate(_uint8 ChairID, int iOpType, _uint8 bCard);

	

	bool EstimateUserRespond(_uint8 bOutChairID, _uint8 bOutCard);
	
	void UserOperateGang(_uint8 ChairID, int iOpType, _uint8 bCard);
	
	void Proto_Game_OperateGang(_uint8 ChairID, _uint8 bCard, int iOpType, int iGangScore[GAME_PLAYER], proto::game::AckBcOpResult& stOpresult);
	
	void Proto_Game_OpNotify(_uint8 ChairID, _uint8 CurChairID, _uint8 bCard, proto::game::AckOpNotify& stNotify);
	
	bool CheckUserOutCard(_uint8 ChairID, _uint8 cbCard);
	void Proto_Game_UserGetCard(_uint8 ChairID, _uint8 MoChairID, _uint8 bCard, proto::game::AckUserGetCard& stUsergetcard);
	

	void DoMultiOperate(_uint8 ChairID, int iOpType, _uint8 bCard);
	void Proto_GameEndInfo(const MJ_user_buffer& UserID, proto::game::AckGameEnd& stGameend);
	
	
	
	
	char* CatPattern(const  MJ_win_pattern &patter);
	
	int AddFanFunXiaoHu(_uint8 ChairID, const HandCards& pHc, MJ_fan_buffer& oFan, const MJ_win_pattern& oPattern , bool , _uint8  );
	void Proto_Game_OpNotifyAfterPengChi(_uint8 ChairID, _uint8 CurChairID, const MJ_opinfo& gang, proto::game::AckOpNotify& stNotify);
	void UserOperatePeng(_uint8 ChairID, _uint8 bCard);
	void UserAfterOperate(_uint8 ChairID, int iOpType, _uint8 bCard);
	
	void Proto_GameEndInfo_GangInfo(_uint8 ChairID, proto::game::AckGameEnd& stGameend);
	
	int All_Is_Feng(const HandCards *phc);
	int Tian_Gang(_uint8 chairid);
	void GameEndReset();
	int di_gang(_uint8 chairid , int );

	int xi_lan_big_2(int (*card_arr)[10], int row_all);
	int xi_lan(const HandCards *phc , int  ChairID=0  );
	
	int feng_type_count(const HandCards *phc);
	int GetAnGagnCount(const HandCards *phc);
	int di_hu_guilin(_uint8 chairid);
	int GetXing(_uint8 c, _uint8 & pre, _uint8 &next);
	void Guilin_OpAck(int op_type, int  hufen[4]);
	void DealZhui(map<_uint8, int> & zui_map, _uint8 bOutCard, bool ququan_zhui);
	

	int XingTongSeCount(_uint8 bGetcard, _uint8 bPreCard, _uint8 bNextCard, cur_lun_xing_info &pInfo);
	void GetXingLun(int &lun, _uint8 bGetCard, vector<cur_lun_xing_info> & xing_list);
	void GameEnd();
	void DealHuBao(_uint8 ChairID);
	void UserOperateChi(_uint8 ChairID, int iOpType, _uint8 bCard);
	
	void DealGuiLinJieSuan(proto::game::AckGameEnd& stGameend);
	void Calc_HuScore(_uint8 ChairID, int iBaseScore[GAME_PLAYER], int iScore[GAME_PLAYER]);
	
	
	int deal_chi_op(_uint8 chairid, int op, _uint8 op_card);
	void Proto_GameScene(_uint8 ChairID, Player* player, proto::game::AckGameScene& stGameScene);
	bool is_gang_shang_kai_hua(_uint8 chairID, bool zimo);

	bool IsQuanqiuren(const HandCards& pHc, MJ_fan_buffer& oFan, int& iOpType);
	int  check_guilin_listen_xilun(_uint8 ChairID);
	void Score_GameEndDraw(HuScoreSet& score, proto::game::AckGameEnd& stGameend);
	void OnRecvUserOperate(_uint8 ChairID, int iOpType, int iCard);
	int Calc_Fan(_uint8 ChairID, MJ_fan_buffer& oFan);
private:
	
	Guilin_Fantype_to_FanNum   g_guilin_ft2fn;
	MJ_fan_buffer           TmpFan[GAME_PLAYER];


	map<_uint8, _uint8>  m_TianGangList;
	map<_uint8, _uint8>  m_DiGangList;
	map<_uint8, int >   m_zui;  
	_uint8              m_preoutCard;
	

	guilin_conf          g_guilin_conf;

	map<_uint8, _uint8>  m_GangMap;  
	
	vector<int>			m_yiGangDaoDiVec;

	
	int                m_playerXingCount[GAME_PLAYER];

	int                  m_hubao[GAME_PLAYER][GAME_PLAYER];

	vector<_uint8>         m_AllLunXingCard;   // 牌局结束后。 每一轮摸到的醒牌。 ！！！

	int                    m_WinLostArr[GAME_PLAYER][GAME_PLAYER];
	bool                   m_IsTianhu;

	int                  m_guoZhangHuArr[GAME_PLAYER];

	map<_uint8, vector<_uint8> >  m_CanotOutAfterChiVec;

	_uint8  m_zhuaXingOverArr[GAME_PLAYER]; 
	int       m_TmpGuoArr[GAME_PLAYER];  

	int       m_nPlayerOp[GAME_PLAYER]; 
	


};


#endif