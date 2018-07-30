#ifndef __TABLE_XIAOHUA_H__
#define __TABLE_XIAOHUA_H__

#include "../table.h"

#include "le_majiang.h"

struct BuzhangInfo
{
	MJ_BUFFER<_uint8, 4> buzhang;
	bool				 bao_ting;	//是否报听		
	bool                 is_first_bao_ting;

};


class XiaoHua_Fantype_to_FanNum : public Fantype_to_FanNum
{
public:
	XiaoHua_Fantype_to_FanNum()
	{
	}
	bool FanFilter(const MJ_fan_buffer& oFan, MJ_fan_buffer& oOutFan)   // 过滤 小胡 如果有大胡的番。
	{
		oOutFan.Clear();
		map<int, int>    all_fan_map;
		for (int i = 0; i < oFan.Size(); i++)
		{
			log_debug("fan [%d]", oFan[i]);
			if (oFan[i] != MJ_FAN_TYPE_JIANGDUI)
			{
				all_fan_map[oFan[i]] = 1;
			}
		}
		for (map<int, int>::iterator it = all_fan_map.begin(); it != all_fan_map.end(); ++it)
		{
			oOutFan.Add(it->first);
			log_debug("ofan[%d]", it->first);
		}

		return !(oOutFan.IsNull());//根据
	}

	int GetFan(int fan)const
	{
		if ( base  != 3 )    // 5分的底注。。 
		{
			switch (fan)
			{
			case MJ_FAN_TYPE_JIANGDUI:   //1.平胡
				return  0;
			case  MJ_FAN_TYPE_GANGSHANGHUA:   //2.杠上开花
				return   5;
			case  MJ_FAN_TYPE_QIDUI:   // 3.七大对
				return  10;
			case  MJ_FAN_HAO_HUA_QI_XIAO_DUI:  // 4.豪华七大对
				return  30;
			case  MJ_FAN_TYPE_PENGPENGHU:   //  5对对胡 
				return  10;
			case  MJ_FAN_TYPE_QINGYISE:   //6 清一色 
				return  30;
			case MJ_FAN_DUI_DUI_QING:    // 7对对请 
				return  50;
			case MJ_FAN_ZAOZAOHU:   // 8糟糟胡
				return  50;
			case MJ_FAN_YI_TIAO_LONG:    //9 通天。一条龙。 
				return  5;
			case  MJ_FAN_YA_ZI:   // 10丫子
				return 5;
			case  MJ_FAN_DUI_DAO_HU:  // 12 对倒胡
				return  5;
			case MJ_FAN_TUIDAO_HU:   // 13 推倒胡
				return   0;
			case MJ_FAN_DA_JUE:     // 14.大绝
				return   20;
			case  MJ_FAN_WU_HUA_GUO:   //15 无花  无花果 
				return 20;
			case  MJ_FAN_SI_XI_XIAO_HUA_MAJIANG:   //17 四喜
				return 5;
			case  MJ_FAN_TYPE_TIANHU:   // 18天胡
				return  40;

			case MJ_FAN_TYPE_HAIDILAOYUE:   //19 海底捞月
				return 5;
			case  MJ_FAN_TYPE_QUANQIUREN:   // 11单吊  修改为5分。
				return  5;
			case MJ_FAN_HUO_CHA:   // 16火叉
				return   5;
			case  MJ_FAN_ALL_19:
				return 50; 
			case  MJ_FAN_TYPE_HUNYISE :  // MJ_FAN_TYPE_HUNYISE =15,					//混一色
				return  5;

			default:
				break;
			}
			return 0;
		}

		switch (fan)
		{
		case MJ_FAN_TYPE_JIANGDUI:   //1.平胡
			return  0;
		case  MJ_FAN_TYPE_GANGSHANGHUA:   //2.杠上开花
			return 3;
		case  MJ_FAN_TYPE_QIDUI:   // 3.七大对
			return  7;
		case  MJ_FAN_HAO_HUA_QI_XIAO_DUI:  // 4.豪华七大对
			return  22;
		case  MJ_FAN_TYPE_PENGPENGHU:   //  5对对胡 
			return  7;
		case  MJ_FAN_TYPE_QINGYISE:   //6 清一色 
			return  15;
		case MJ_FAN_DUI_DUI_QING:    // 7对对请 
			return  30;
		case MJ_FAN_ZAOZAOHU:   // 8糟糟胡
			return 30;
		case MJ_FAN_YI_TIAO_LONG:    //9 通天。一条龙。 
			return 3;
		case  MJ_FAN_YA_ZI:   // 10丫子
			return 3;
		case  MJ_FAN_DUI_DAO_HU:  // 12 对倒胡
			return  4;
		case MJ_FAN_TUIDAO_HU:   // 13 推倒胡
			return  0;
		case MJ_FAN_DA_JUE:     // 14.大绝
			return 15;
		case  MJ_FAN_WU_HUA_GUO:   //15 无花  无花果 
			return  15;
		case  MJ_FAN_SI_XI_XIAO_HUA_MAJIANG:   //17 四喜
			return   3;
		case  MJ_FAN_TYPE_TIANHU:   // 18天胡
			return  20;

		case MJ_FAN_TYPE_HAIDILAOYUE:   //19 海底捞月
			return 3;
		case  MJ_FAN_TYPE_QUANQIUREN:   // 11单吊
			return  3;
		case MJ_FAN_HUO_CHA:   // 16火叉 .
			return  3;
		case  MJ_FAN_ALL_19:
			return  30;
		case  MJ_FAN_TYPE_HUNYISE: //  MJ_FAN_TYPE_HUNYISE =15,					//混一色
			return  3; 

		default:
			break;
		}
		log_error("error !");
		return 0;
	}
	int  base;
	



};


struct XiaoHua_conf  
{
	
	bool  is_3fen_di;  //    3分的底注 
	bool  man_tian_hua;

};

class Table_XiaoHua : public Table
{
public:
	Table_XiaoHua(int player  );
	~Table_XiaoHua();

public:
	void ConfInit_Remote(const Json::Value& val);
	
	int Check_Hu_Pattern(_uint8 ChairID, const HandCards& pHc, MJ_fan_buffer& oFan, const MJ_win_pattern& oPattern);
	void OnUserOutCard(_uint8 ChairID, _uint8 bOutCard);
	void OnUserGrapCard(_uint8 ChairID, bool bGetLast);

	int check_ya_zi(_uint8 ChairID, _uint8 card, const MJ_win_pattern &pattern);
	int NoHuaGuo(_uint8 sid);
	int SixiCount(_uint8 sid);
	int dajue(_uint8 sid, _uint8 huPai);

	void Calc_Hu_Player_Base_Score(_uint8 ChairID, int iBaseScore[GAME_PLAYER]);

	int playerOutHuaCount(int sid);
	void TurnBanker();

	int CheckPlaySlefGrpCardCanOperateCode(_uint8 ChairID, _uint8 bCard, MJ_major& major, int& fan_num);
	void GameStart();
	int CheckOutCardOtherPlayerCanOpCode(_uint8 ChairID, _uint8 bCard, MJ_major& major, int& fan_num);
	
	int Check_Hu(_uint8 ChairID, HandCards& pHc, MJ_fan_buffer& oFan, _uint8 add_last_card = 0  );
	
	bool Is7Dui(_uint8 ChairID, HandCards& pHc, MJ_fan_buffer& oFan);
	
	void DoSimpleOperate(_uint8 ChairID, int iOpType, _uint8 bCard);

	int  HuoCha(const MJ_win_pattern &pattern,  HandCards *pHc);
	

	int TuiDaoHu( _uint8  , const MJ_win_pattern &pattern, _uint8 hupai);

	bool EstimateUserRespond(_uint8 bOutChairID, _uint8 bOutCard);
	void UserOperateGang(_uint8 ChairID, int iOpType, _uint8 bCard);
	void Proto_Game_OperateGang(_uint8 ChairID, _uint8 bCard, int iOpType, int iGangScore[GAME_PLAYER], proto::game::AckBcOpResult& stOpresult);
	void Proto_Game_OpNotify(_uint8 ChairID, _uint8 CurChairID, _uint8 bCard, proto::game::AckOpNotify& stNotify);
	bool CheckUserOutCard(_uint8 ChairID, _uint8 cbCard);
	void Proto_Game_UserGetCard(_uint8 ChairID, _uint8 MoChairID, _uint8 bCard, proto::game::AckUserGetCard& stUsergetcard);
	int Calc_Hu(_uint8 ChairID, _uint8 bCard, int Score[GAME_PLAYER]);
	void DoMultiOperate(_uint8 ChairID, int iOpType, _uint8 bCard);
	
	void Proto_GameEndInfo(const MJ_user_buffer& UserID, proto::game::AckGameEnd& stGameend);
	
	int yi_tiao_long(const  MJ_win_pattern &pattern  );
	int zao_zao_hu(bool pengpenghu, const HandCards *pHc);
	
	char* CatPattern(const  MJ_win_pattern &patter);
	
	int AddFanFunXiaoHu(_uint8 ChairID, const HandCards& pHc, MJ_fan_buffer& oFan, const MJ_win_pattern& oPattern , bool , _uint8  );
	void Proto_Game_OpNotifyAfterPengChi(_uint8 ChairID, _uint8 CurChairID, const MJ_opinfo& gang, proto::game::AckOpNotify& stNotify);
	void UserOperatePeng(_uint8 ChairID, _uint8 bCard);
	void UserAfterOperate(_uint8 ChairID, int iOpType, _uint8 bCard);
	int IsHandHasHuaCard(const HandCards *pHc);

	void Proto_GameEndInfo_GangInfo(_uint8 ChairID, proto::game::AckGameEnd& stGameend);
	

	int Is119(_uint8  sid);
	int FixPengCard(const HandCards *phc);
	
	int DuiDaoHu(const HandCards *phc, const MJ_win_pattern &pattern, _uint8 hu_pai);
	bool is_gang_shang_kai_hua(_uint8 chairID, bool zimo);
	void GameEndReset();
	void GangHouClear();
	void DoMultiOperate_HuWeiDa(_uint8 ChairID, int iOpType, _uint8 bCard);  
	bool IsQuanqiuren(const HandCards& pHc);
	void Score_GameEndSucc(HuScoreSet& score, proto::game::AckGameEnd& stGameend);
	void Calc_HuScore(_uint8 ChairID, int iBaseScore[GAME_PLAYER], int iScore[GAME_PLAYER]); 
	int Calc_Fan(_uint8 ChairID, MJ_fan_buffer& oFan);
	bool IsListening(_uint8 ChairID, _uint8 bRemoveCard);
	void Proto_GameScene_Operate(_uint8 ChairID, Player* player, proto::game::AckGameScene& stGameScene);
	void Proto_GameScene_lookon(_uint8 ChairID, Player* player, proto::game::AckGameScene& stGameScene);
	void Proto_GameScene(_uint8 ChairID, Player* player, proto::game::AckGameScene& stGameScene);
	int UserOperateHu(_uint8 ChairID, _uint8 bHuCard /* = 0xFF */);


	int IsUserAterPengCanBaoTing(_uint8 ChairID);
	int Special_Hu_If_Remove_One_Next_Add(_uint8 chaidId, _uint8 add_last_card = 0);
	int IsBaoTingGrapCardCanGang(_uint8 chairID, _uint8 add_card); 
	int IsNoPreOutCardJudgeGang(_uint8 charidID, _uint8 add_card);
	int HunyiSe(_uint8 ChairId);
	int HasFeng(_uint8 ChairID);
//	int dandiao(const MJ_win_pattern &pattern, _uint8 hupai);
	int dandiao(const MJ_win_pattern &pattern, _uint8 hupai, _uint8 ChairID);
private:
	
	
	int   m_kezi;
	int   m_sixi;
	bool  m_HuWeiDa; 

	int  m_curGangPlayerSid;

	BuzhangInfo m_buzhang[GAME_PLAYER];

	MJ_fan_buffer m_MultiFan[GAME_PLAYER]; 
	int m_MultiHufanNums[GAME_PLAYER];
	Shai_Set m_gang_shai;   
	MJ_BUFFER<_uint8, 6> m_gang_hou_card;
	MJ_BUFFER<int, 6> m_gang_hou_op[GAME_PLAYER];


	XiaoHua_conf  _xiaohua_conf;

	XiaoHua_Fantype_to_FanNum g_xiaohua_ft2fn;


	_uint8 m_HuWeiDaCard; 
	int  m_HuWeiDaOp; 

	int   m_Gang_buzhang_res; 

	int  m_hucount;
	vector<int>  m_BaoTingGrapCardCannotGangCard;   // 报听后自己抓牌不能杠的牌。 
	int  m_BaoTingPos[GAME_PLAYER]; 

};


#endif