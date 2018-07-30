#ifndef __TABLE_JILIN_H__
#define __TABLE_JILIN_H__
#include "../table.h"
#include "le_majiang.h"


enum 
{
	BAO_TING_FIRST          = 1 , 
	BAO_TING_TONG_BAO_BU_HU = 2 , 
};

struct JiLinBuzhangInfo
{
	MJ_BUFFER<_uint8, 4> buzhang;
	bool				 bao_ting;	//是否报听		
	

};



struct  jilin_conf
{
	bool  yao_jiu_dan ;  
	bool  quick_bao;   
	bool  xiaojifeidan; 
	bool  xian_ba_yi; 
	int   guo;
	int   feng_ding; 
	bool  si_lou; 
	bool  yao_jiu_three_se; 
	bool  yaoji_wanneng; 

};


#define  XUAN_FENG_GANG_ARR_LEN   7 

struct  gang_card 
{
	int  arr[XUAN_FENG_GANG_ARR_LEN];   // 牌数组 
	int  bu[XUAN_FENG_GANG_ARR_LEN];    // 补的次数数组， 
	int   g_type;  
	int   gang_id; 
	int   yaojiudan;
	gang_card()
	{
		memset(arr, 0, sizeof(arr));
		memset(bu, 0, sizeof(bu)); 
		g_type = 0;
		gang_id = 0; 
		yaojiudan = 0; 
	}
};

struct  JiGangInfo
{	
	gang_card   g_arr[10]; 
	int         len;
	JiGangInfo()
	{
		len = 0;
		memset(g_arr, 0, sizeof(g_arr));
	}
	void clear()
	{
		len = 0;
		memset(g_arr, 0, sizeof(g_arr));
	}
};


struct ZhongfabaiChi
{
	
	int  chi_id;
	int  pre_id;  // 打的id 
	int  chi_card;
	ZhongfabaiChi()
	{		
		chi_card = 0;
		chi_id = 0;
		pre_id = 0;
	}

};




class   jilin_Fantype_to_FanNum : public Fantype_to_FanNum
{
public:
	jilin_Fantype_to_FanNum()
	{
	}
	bool FanFilter(const MJ_fan_buffer& oFan, MJ_fan_buffer& oOutFan)   // 过滤 小胡 如果有大胡的番。
	{
		oOutFan.Clear();				
		map<int, int>  all_map;
		for (int i = 0; i < oFan.Size(); i++)
			all_map[oFan[i]] = 1;  
		map<int, int>::iterator it = all_map.begin();
		for (; it != all_map.end();++it)
		{
			oOutFan.Add(it->first); 
		} 		
		return !(oOutFan.IsNull());//根据
	}

	int GetFan(int fan)const
	{		
		return 1;
	}
};



class Table_jilin : public Table
{
public:
	Table_jilin(int player);
	~Table_jilin();

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
	
	int Check_Hu(_uint8 ChairID, HandCards& pHc, MJ_fan_buffer& oFan, _uint8 add_last_card = 0  );
		
	void DoSimpleOperate(_uint8 ChairID, int iOpType, _uint8 bCard);
	

	bool EstimateUserRespond(_uint8 bOutChairID, _uint8 bOutCard);
	int  UserOperateGang(_uint8 ChairID, int iOpType, _uint8 bCard);
	void Proto_Game_OperateGang(_uint8 ChairID, _uint8 bCard, int iOpType, int iGangScore[GAME_PLAYER], proto::game::AckBcOpResult& stOpresult);
	void Proto_Game_OpNotify(_uint8 ChairID, _uint8 CurChairID, _uint8 bCard, proto::game::AckOpNotify& stNotify);
	bool CheckUserOutCard(_uint8 ChairID, _uint8 cbCard);
	void Proto_Game_UserGetCard(_uint8 ChairID, _uint8 MoChairID, _uint8 bCard, proto::game::AckUserGetCard& stUsergetcard);
	int Calc_Hu(_uint8 ChairID, _uint8 bCard, int Score[GAME_PLAYER]);
	void DoMultiOperate(_uint8 ChairID, int iOpType, _uint8 bCard);
	
	void Proto_GameEndInfo(const MJ_user_buffer& UserID, proto::game::AckGameEnd& stGameend);
	

	
	int AddFanJilinHu(_uint8 ChairID, const HandCards& pHc, MJ_fan_buffer& oFan, const MJ_win_pattern& oPattern, bool, _uint8);
	void Proto_Game_OpNotifyAfterPengChi(_uint8 ChairID, _uint8 CurChairID, const MJ_opinfo& gang, proto::game::AckOpNotify& stNotify);
	void UserOperatePeng(_uint8 ChairID, _uint8 bCard);
	void UserAfterOperate(_uint8 ChairID, int iOpType, _uint8 bCard);	

	void Proto_GameEndInfo_GangInfo(_uint8 ChairID, proto::game::AckGameEnd& stGameend);
	
	bool is_gang_shang_kai_hua(_uint8 chairID, bool zimo);
	void GameEndReset();

	bool IsQuanqiuren(const HandCards& pHc);

	void Score_GameEndSucc(HuScoreSet& score, proto::game::AckGameEnd& stGameend);
	
	void Calc_HuScore(_uint8 ChairID, int iBaseScore[GAME_PLAYER], int iScore[GAME_PLAYER]); 
	
	int Calc_Fan(_uint8 ChairID, MJ_fan_buffer& oFan);
	
	bool IsListening(_uint8 ChairID, _uint8 bRemoveCard);
	void Proto_GameScene_Operate(_uint8 ChairID, Player* player, proto::game::AckGameScene& stGameScene);
	void Proto_GameScene_lookon(_uint8 ChairID, Player* player, proto::game::AckGameScene& stGameScene);

	void Proto_GameScene(_uint8 ChairID, Player* player, proto::game::AckGameScene& stGameScene);
	int UserOperateHu(_uint8 ChairID, _uint8 bHuCard /* = 0xFF */);
	void InitBanker();

	int ThreeColorOrQingYiSe(int sid, const HandCards *phc);



	int Card_19(int sid, const HandCards *phc);
	


	 int  ProtoJilinGangOperate(int, int, int *arr, int len, int gang_id);
	 int  GetPlayerWinlostMoney(_uint8 ChairID);
	 
	 int check_jia_hu(_uint8 ChairID, _uint8 hu_card, 
		 const MJ_win_pattern &pattern , bool laizi = false  ,  bool  can_hu_paixing = false );
	 int check_bian_hu(_uint8 ChairID, _uint8 hu_card, 
		 const MJ_win_pattern &pattern , bool laizi = false ,  bool canhupaixng = false  );
	 int check_hu_kezi(_uint8 ChairID);
	 void  GameEnd();
	 int   ProtoBaoSwap(_uint8);
	 int GetGangFen(int base, int is_normal, int curGaneCard, int gang_type);
	 void proto_gang_money(int *guo ,  int *p , int );
	 
	 int  ZhongFabaiChi_Judge(_uint8 pre_outcard, _uint8 ChairID);
	 void Score_GameEndDraw(HuScoreSet& score, proto::game::AckGameEnd& stGameend);
	 int  hu_test(_uint8 ChairID);
	 int CheckJilinGangOp(_uint8 ChairID   );
	 int ProtoJinLinAfterSpeGangCanOp(_uint8 ChairID);


	 void ProAddSpecialGangInfo(_uint8 ChairID, proto::game::AckUserGetCard& stUsergetcard, bool is_rec, proto::game::jilin::JilinReconnectInfo &stReconnect);
	 int DebugTest(_uint8);
	 void OnRecvUserOperate(_uint8 ChairID, int iOpType, int iCard);
	 bool EstimateUserJilinGangRespond(_uint8 GangId, _uint8 bOutCard , int );
	 int Proto_Jilin_bu(_uint8 ChairID, _uint8 bu , int );
	 int HuIfPreRemoveOneCardNextAddOne( _uint8 ,const HandCards *phc, MJ_fan_buffer & oFan ,  _uint8 , bool  has_kezi = false );
	 int TingJudge(_uint8 ChairID);
	 void ProtoQiangJIlinGang(_uint8 qiang, _uint8 bei_qiang, _uint8 card);
	 void UserOperateZhongfaBai(_uint8 ChairID, int iOpType, _uint8 bCard);


	 int GangTingJudget(_uint8 , HandCards *phc, MJ_fan_buffer ofan, _uint8 add_last);
	 int GangJudgeAfterTing(_uint8 ChairID, MJ_opinfo &major, map<int, int> &allmap);
	 void GetAllTingCard(map<_uint8, _uint8> & ting_map, _uint8 ChairID);
	 bool RobGangHu(_uint8 ChairID, _uint8 CardData, int iOpType);
	 int GetGangPeng_Id();
	 int GetNextBanker();
	 int Jilin_qidui(const HandCards *p, bool IsTongBaoMoBao);
	 int OneColor(int sid, const HandCards *phc, bool is_tongmo_bao);
	 int Is_PengPneghu_Jilin(_uint8 ChairId, const HandCards *phc, bool laizi);
	 int GetGangPeng_Card();
	 int Add_Ting_Pai_Info(_uint8 ChairID, proto::game::AckUserGetCard & Ack);
	 int get_card_size(const HandCards *phc, _uint8 c);
	 int dan_diao(bool laizi, const HandCards *pc);

	 int BuGangAfterBaoTing(_uint8 ChairID, _uint8 grap_card); 
	 int Ting_Zhongfabai(const  _uint8 *parr, _uint8 len, _uint8 add_last);
	 int Ting_Zhongfabai_PengPengHu_Judge(const HandCards *phc, _uint8 add_last);
	 void UserOperateChi(_uint8 ChairID, int iOpType, _uint8 bCard);
	 int  KaiMen(const HandCards *phc , int  );
	 int IsCanGangAfterTing(int sid, int bcard);
	 int phc_allkezi(const HandCards *phc);
	 int GetOneZFB(map<int, int> & all_map);
	 void Proto40Spe(int &fan);
	 int In_Hand(const HandCards *phc, int a, int b, int c);
	 int GetCountSize(const HandCards *phc, _uint8 c);
	 int LingGuoHu(_uint8 ChairID);
	 bool Check_Listening(HandCards *pHc, _uint8 ChairID, _uint8 bCard, tagListenInfos stListenInfo[14], _uint8 &bListenLen);
	 int TingHouFirstGang(_uint8 ChairID);
	 void  Reconnect(int sid); 
	 int In_Hand_New(const HandCards *phc, int a);
	 int CheckIfCanSwapBaoPai(int sid);
	 int  GrapPaiAutoHu(_uint8 ChairID);
	 int OutCardSwapBaoPaiFunction(_uint8 ChairID);
	 
	 void HuanBao(_uint8 ChairID);
	 int OutCardSwapBaoPai_NoQuickFun(_uint8 ChairID);

private:

	JiLinBuzhangInfo m_jilin_baoting[GAME_PLAYER];    //  1  1 
	jilin_conf    g_jilin_conf;

	JiGangInfo    m_JilinGangInfo[GAME_PLAYER];    //吉林杠的操作。 // 1  1 

	jilin_Fantype_to_FanNum g_xiaohua_ft2fn;

	
	vector<ZhongfabaiChi> m_Jilinchi;

	int           m_nqiangArr[GAME_PLAYER][3];  //  arr[1][0] === 可以抢杠的人， 收到抢杠值， // 11  22 
	bool          bIsCanQiangGangOrPeng; 


	int   m_gangID;

	int   m_baoPai;
	

	int   m_outCardCountAfterBaoTing[4];   // 报听后。打牌的数量。  //  1  2 3  //  区分 包与不包。 ！！

	int   m_WinlostArr[4][4];     // 1 

	int   m_TrueWinLost[4];  //  1  2 

	bool  m_bHasTianhu;  
	map<int, map<_uint8, _uint8> >   m_nPlayerTingmap; 
	bool  m_bTongBao;
	int   m_bIsTongBaoTime;
	int   m_nPlayerTongBao[GAME_PLAYER];  // 1   2 

	int   m_nGiveUphu[4];  //  1   2 
	int   m_addlastCard;

	int    m_fenResult[GAME_PLAYER][2];   // 1   2  

	int   m_nXianBayi[GAME_PLAYER];  //  1     22 

	int   m_QiangGangHu[GAME_PLAYER];  //  1   22 
	map<int, int>  m_nRecvPlaerOp;  

	int  m_GangMoney[GAME_PLAYER] ; 
	map<int, int>  m_TingHouCanOutPaiMap;
	_uint8  m_nGangShangHua[GAME_PLAYER]; 


	int    m_NoQuickBaoTongBaoGrapPaiCount[4];  // 不是快宝 的时候。有人通宝。不能马上 换宝  。
	
	struct TingPosinfo_t
	{
		int pos;
		int outcard;
		int out_size;
	};
	map<int, TingPosinfo_t>  m_allTingPosInfoMap;

	
};


#endif