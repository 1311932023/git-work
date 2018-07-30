#ifndef __TABLE_CHONG_QING_MJ_H__
#define __TABLE_CHONG_QING_MJ_H__

//#include "../table.h"
#include "table_zz_laizi.h"
#include "../proto/chengdugameproto.pb.h"

struct MJ_cq_conf_js
{
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive & ar, const unsigned int /* file_version */) {
		printf("MJ_cq_conf_js::serialize\n");
		ar & iGameType;		    //游戏类型          0：血战到底 1：血流成河 2：常规玩法
		ar & bYiPaoDuoXiang;    //是否一炮多响      false：不能一炮多响 true：可以一炮多响
		ar & bQiangGangHu;      //是否抢杠胡        false：不能抢杠胡   true：可以抢杠胡
		ar & bHuanSanZhang;     //是否换三张        false：不换三张     true：换三张
		ar & bAllDianGangHua;   //点杠花是否家家有  false：一家有       true：家家有
		ar & bZiMoJiaFan;       //是否自摸加番      false：不自摸加番   true：自摸加番
		ar & bZiMoJiaDi;        //是否自摸加底      false：不自摸加底   true：自摸加底
		ar & iFengDingFan;       //封顶番数
		ar & bYaoJiuJiangDui;   //妖九将对加番
		ar & bMenQingZhongZhang;//门清中张加番
		ar & bTianDiHu;         //天地胡加番
		ar & bDianPaoPingHu;    //平胡点炮
		ar & bYiTiaoLong;       //一条龙
		ar & bGuaCa;            //刮擦
		ar & iPlayNum;           //成都玩法人数
		ar & iPaiZhangShu;       //牌张数
		ar & iTianHuFan;         //天胡4番
		ar & iDiHuFan;           //地胡3番
		ar & iDuiDuiHuFan;       //对对胡2番
		ar & bJiaXinWu;         //夹心5
		ar & bHaiDiHuaJiaFan;   //海底花加番
		ar & bHaiDiPaoJiaFan;   //海底花加番
	}

	int iGameType;		    //游戏类型          0：血战到底 1：血流成河 2：常规玩法
	bool bYiPaoDuoXiang;    //是否一炮多响      false：不能一炮多响 true：可以一炮多响
	bool bQiangGangHu;      //是否抢杠胡        false：不能抢杠胡   true：可以抢杠胡
	bool bHuanSanZhang;     //是否换三张        false：不换三张     true：换三张
	bool bAllDianGangHua;   //点杠花是否家家有  false：一家有       true：家家有
	bool bZiMoJiaFan;       //是否自摸加番      false：不自摸加番   true：自摸加番
	bool bZiMoJiaDi;        //是否自摸加底      false：不自摸加底   true：自摸加底
	int iFengDingFan;       //封顶番数
	bool bYaoJiuJiangDui;   //妖九将对加番
	bool bMenQingZhongZhang;//门清中张加番
	bool bTianDiHu;         //天地胡加番
	bool bDianPaoPingHu;    //平胡点炮
	bool bYiTiaoLong;       //一条龙
	bool bGuaCa;            //刮擦
    int iPlayNum;           //成都玩法人数
	int iPaiZhangShu;       //牌张数
	int iTianHuFan;         //天胡4番
	int iDiHuFan;           //地胡3番
	int iDuiDuiHuFan;       //对对胡2番
	bool bJiaXinWu;         //夹心5
	bool bHaiDiHuaJiaFan;   //海底花加番
	bool bHaiDiPaoJiaFan;   //海底花加番

	MJ_cq_conf_js()
	{
		iGameType             = 0;       //游戏类型          0：血战到底 1：血流成河 2：常规玩法
		bYiPaoDuoXiang        = false;   //是否一炮多响      false：不能一炮多响 true：可以一炮多响
		bQiangGangHu          = false;   //是否抢杠胡        false：不能抢杠胡   true：可以抢杠胡
		bHuanSanZhang         = false;   //是否换三张        false：不换三张     true：换三张
		bAllDianGangHua       = true;    //点杠花是否家家有  false：一家有       true：家家有
		bZiMoJiaFan           = true;    //是否自摸加番      false：不自摸加番   true：自摸加番
		bZiMoJiaDi            = false;   //是否自摸加底      false：不自摸加底   true：自摸加底
		iFengDingFan          = 4;       //封顶番数
		bYaoJiuJiangDui       = false;   //妖九将对加番
		bMenQingZhongZhang    = false;   //门清中张加番
		bTianDiHu             = false;   //天地胡加番
		bDianPaoPingHu        = false;   //平胡点炮
		bYiTiaoLong           = false;   //一条龙
		bGuaCa                = false;   //刮擦
		iPlayNum              = 4;       //成都玩法人数
		iPaiZhangShu          = 108;     //牌张数
		iTianHuFan            = 4;       //天胡4番
		iDiHuFan              = 3;       //地胡3番
		iDuiDuiHuFan          = 1;       //对对胡2番
		bJiaXinWu             = false;
		bHaiDiHuaJiaFan       = false;
		bHaiDiPaoJiaFan       = false;
	}
};

struct CqOtherHu
{
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive & ar, const unsigned int /* file_version */) {
		printf("CqOtherHu::serialize\n");
		ar & oFan;
		ar & bYinghu;
	}
	MJ_fan_buffer oFan;
	bool bYinghu;
	CqOtherHu()
	{
		oFan.Clear();
		bYinghu = false;
	}
};

struct StructGangZhuanYi
{
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive & ar, const unsigned int /* file_version */) {
		printf("StructGangZhuanYi::serialize\n");
		ar & bIsUsed;
		ar & nUserGang;
		ar & nUserHu;
		ar & nGangScore;
		ar & nHuUserGangScore;
	}
	bool bIsUsed;
	_uint8 nUserGang;
	_uint8 nUserHu;
	int nGangScore;
	int nHuUserGangScore;
	StructGangZhuanYi()
	{
		bIsUsed = false;
		nUserGang = INVALID_CHAIRID;
		nUserHu = INVALID_CHAIRID;
		nGangScore = 0;
		nHuUserGangScore = 0;
	}

	void InitGangZhuanYi()
	{
		bIsUsed = false;
		nUserGang = INVALID_CHAIRID;
		nUserHu = INVALID_CHAIRID;
		nGangScore = 0;
		nHuUserGangScore = 0;
	}
};

struct StructDianPaoInfo
{
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive & ar, const unsigned int /* file_version */) {
		printf("StructDianPaoInfo::serialize\n");
		ar & bIsUsed;
		ar & nDianPaoUser;
		ar & nDianPaoCard;
		ar & nDianPaoShunXu;
	}
	bool bIsUsed;
	_uint8 nDianPaoUser;
	_uint8 nDianPaoCard;
	int nDianPaoShunXu;
	StructDianPaoInfo()
	{
		bIsUsed = false;
		nDianPaoUser = INVALID_CHAIRID;
		nDianPaoShunXu = INVALID_MJ_CARD;
		nDianPaoCard = INVALID_MJ_CARD;
	}

	void InitDianPaoInfo()
	{
		bIsUsed = false;
		nDianPaoUser = INVALID_CHAIRID;
		nDianPaoShunXu = INVALID_MJ_CARD;
		nDianPaoCard = INVALID_MJ_CARD;
	}
};



struct StructQiHu
{
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive & ar, const unsigned int /* file_version */) {
		printf("StructQiHu::serialize\n");
		ar & bIsQiHu;
		ar & nQiHuFan;
		ar & nDangQianHuFan;
	}
	bool bIsQiHu;
	int nQiHuFan;
	int nDangQianHuFan;
	StructQiHu()
	{
		bIsQiHu = false;
		nQiHuFan = 0;
		nDangQianHuFan = 0;
	}

	void InitQiHuInfo()
	{
		bIsQiHu = false;
		nQiHuFan = 0;
		nDangQianHuFan = 0;
	}
};

struct ZiMoPaoHuInfo
{
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive & ar, const unsigned int /* file_version */) {
		printf("ZiMoPaoHuInfo::serialize\n");
		ar & IsZiMoOrPaoHu;
		ar & nHuChaird;
		ar & nZiMoChaird;
		ar & nPaoHuChaird;
		ar & nDianPaoChaird;
		ar & nHuFan;
		ar & nPaoFan;
	}
	bool IsZiMoOrPaoHu;
	_uint8 nHuChaird;
	_uint8 nZiMoChaird;
	_uint8 nPaoHuChaird;
	_uint8 nDianPaoChaird;
	int nHuFan;
	int nPaoFan;
	ZiMoPaoHuInfo()
	{
		IsZiMoOrPaoHu = false;
		nHuChaird = INVALID_CHAIRID;
		nZiMoChaird = INVALID_CHAIRID;
		nPaoHuChaird = INVALID_CHAIRID;
		nDianPaoChaird = INVALID_CHAIRID;
		nHuFan = 0;
		nPaoFan = 0;
	}

};

class chongqingmj_Fantype_to_FanNum : public Fantype_to_FanNum
{
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive & ar, const unsigned int /* file_version */) {
		printf("chongqingmj_Fantype_to_FanNum::serialize\n");
		ar & boost::serialization::base_object<Fantype_to_FanNum>(*this);
		/*boost::serialization::void_cast_register<chongqingmj_Fantype_to_FanNum, Fantype_to_FanNum>(
			static_cast<chongqingmj_Fantype_to_FanNum *>(NULL),
			static_cast<Fantype_to_FanNum *>(NULL)
			);*/
	}

public:
	chongqingmj_Fantype_to_FanNum()
	{
		fan_num[MJ_FAN_TYPE_PINGHU]           = 0;
		fan_num[MJ_FAN_TYPE_PENGPENGHU]       = 1;
		fan_num[MJ_FAN_TYPE_QINGYISE]         = 2;
		fan_num[MJ_FAN_TYPE_QIDUI]            = 2;
		fan_num[MJ_FAN_TYPE_LONGQIDUI]        = 3;
		fan_num[MJ_FAN_TYPE_QUANYAOJIU]       = 3;	
		fan_num[MJ_FAN_TYPE_JIANGDUI]         = 3;

		fan_num[MJ_FAN_TYPE_QINGQIDUI]        = 4;
		fan_num[MJ_FAN_TYPE_QINGDUI]          = 3;
		fan_num[MJ_FAN_TYPE_JIANGQIDUI]       = 4;
		fan_num[MJ_FAN_TYPE_QINGLONGQIDUI]    = 5;	
		fan_num[MJ_FAN_TYPE_TIANHU]           = 3;
		fan_num[MJ_FAN_TYPE_DIHU]             = 2;

		fan_num[MJ_FAN_TYPE_GANGHOUPAO]       = 1;
		fan_num[MJ_FAN_TYPE_GANGSHANGHUA]     = 1;
		fan_num[MJ_FAN_TYPE_QIANGGANGHU]      = 1;
		fan_num[MJ_FAN_TYPE_HAIDIPAO]         = 1;
		fan_num[MJ_FAN_TYPE_HAIDILAOYUE]      = 1;
		fan_num[MJ_FAN_TYPE_JINGOUHU]         = 1;
		fan_num[MJ_FAN_TYPE_MENQING]          = 1;
		fan_num[MJ_FAN_TYPE_ZHONGZHANG]       = 1;
		fan_num[MJ_FAN_TYPE_YITIAOLONG]       = 1;
		fan_num[MJ_FAN_TYPE_GOUPAI]           = 1;
		fan_num[MJ_FAN_TYPE_JIAXINWU]         = 1;

		fan_num[MJ_FAN_TYPE_SHUANGLONGQIDUI]      = 4;
		fan_num[MJ_FAN_TYPE_QINGSHUANGLONGQIDUI]  = 4;
		fan_num[MJ_FAN_TYPE_QINGYISEJINGOUHU]  = 4;
		fan_num[MJ_FAN_TYPE_JIANGDUIDUIJINGOUHU]  = 4;

	}
	bool SetFan(int iFanType, _uint32 iFanNum)
	{
		if (iFanType < MJ_FAN_TYPE_MAX)
		{
			fan_num[iFanType] = iFanNum;
			return true;
		}
		return false;
	}
};

struct STRecodeInfo
{
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive & ar, const unsigned int /* file_version */) {
		printf("STRecodeInfo::serialize\n");
		ar & iZiMoTimes;
		ar & iDianPaoTimes;
		ar & iJiePaoTimes;
		ar & iAnGangTimes;
		ar & iMingGangTimes;
		ar & iChaJiaoTimes;
	}

	int iZiMoTimes;
    int iDianPaoTimes;
	int iJiePaoTimes;
	int iAnGangTimes;
	int iMingGangTimes;
	int iChaJiaoTimes;
	STRecodeInfo()
	{
		iZiMoTimes = 0;
		iDianPaoTimes = 0;
		iJiePaoTimes = 0;
		iAnGangTimes = 0;
		iMingGangTimes = 0;
		iChaJiaoTimes = 0;
	}
};

class Table_chengdumj : public Table
{
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive & ar, const unsigned int /* file_version */)
	{
		printf("Table_chengdumj::serialize\n");
		ar & boost::serialization::base_object<Table>(*this);
		/*boost::serialization::void_cast_register<Table_chengdumj, Table>(
			static_cast<Table_chengdumj *>(NULL),
			static_cast<Table *>(NULL)
			);*/

		ar & m_GameEndTimes;
		ar & m_nHuPaiShunXu;
		ar & m_DingQue_Timer_running;
		ar & m_HuanZhang_Timer_running;
		ar & robot_dingque_timer_running;
		ar & m_conf_js;
		//ar & m_chongqingmj_Fantype_to_FanNum;
		ar & m_bFirstCard;
		ar & m_bIsPassHu;
		ar & m_UserOtherHu;
		ar & m_bOtherHu;
		ar & m_nHaveDingQueNum;
		ar & m_UserDingQueSe;
		ar & m_nHaveHuanZhangNum;
		ar & m_UserHuanZhangPai;
		ar & m_nPlayerMoney;
		ar & m_nYiPaoDuoXiangCharid;
		ar & m_nLastHuPaiNum;
		ar & m_nGouPaiNum;
		ar & m_stZiMoPaoHuInfo;
		ar & m_nDaJiaoFan;
		ar & m_nDaJiao;
		ar & m_nTuiGang;
		ar & m_nGangScore;
		ar & m_nZhuanYiGang;
		ar & m_nLastHuScore;
		ar & m_nHuCard;
		ar & m_nHuCardShunXu;
		ar & m_nZiMoHuCard;
		ar & m_nPaoHuCard;
		ar & m_nHuType;
		ar & m_stQiHuInfo;
		ar & m_nZhiGangChairID;
		ar & m_nHuPaiChairid;
		ar & nOprOpType2;
		ar & m_stGangZhuanYiInfo;
		ar & m_stDianPaoInfo;
		ar & m_bQiGang;
		ar & m_bCanDiHu;
		ar & m_nJiChuDiFen;
		ar & m_stRecodeInfo;
		ar & m_ShuFenSeatID;
	}

public:	
	Table_chengdumj();
	~Table_chengdumj();

public:
	void ConfInit_Remote(const Json::Value& val);

	void InitData();

	void GameStart();

	void Send_Hand_Card();

	void OnUserGetCard(_uint8 ChairID, bool bGetLast = false);

	virtual void Send_Game_Scene(Player *player);

	bool EstimateUserRespond(_uint8 bOutChairID, _uint8 bOutCard);

	void Proto_Game_UserGetCard(_uint8 ChairID, _uint8 MoChairID, _uint8 bCard, proto::game::AckUserGetCard& stUsergetcard);

	int OutCardEstimate(_uint8 ChairID, _uint8 bCard, MJ_major& major, int& fan_num);

	int GetCardEstimate(_uint8 ChairID, _uint8 bGetCard, MJ_major& major, int& fan_num);

	bool FormatCards(HandCards *hcs, MJ_win_pattern& pattern);

	void OnRecvUserOperate(_uint8 ChairID, int iOpType, int iCard);
    
	void Score_GameEndSucc(HuScoreSet& score, proto::game::chengdumj::AckCDGameEnd &stJSGameEnd);

	void save_video_game_end(const proto::game::chengdumj::AckCDGameEnd stJSGameEnd);

	void Proto_GameEndInfo_AddFan(const MJ_user_buffer& UserID, proto::game::AckGameEnd_Chengdu& stGameend);

	void UserGetFirstCard(_uint8 ChairID, int& iOpType, MJ_fan_buffer& oFan);

	void UserFirstOutCard(_uint8 ChairID, int& iOpType, MJ_fan_buffer& oFan);

	void UserGetLastCard(_uint8 ChairID, int& iOpType, MJ_fan_buffer& oFan);

	void UserLastOutCard(_uint8 ChairID, int& iOpType, MJ_fan_buffer& oFan);

	void UserGangShangHua(_uint8 ChairID, _uint8 bCard, MJ_fan_buffer& oFan);
 
 	void UserGangHouPao(_uint8 ChairID, _uint8 bCard, MJ_fan_buffer& oFan);
 
 	void UserQiangGangHu(_uint8 ChairID, _uint8 bCard, MJ_fan_buffer& oFan);

	int Calc_Fan(_uint8 ChairID, MJ_fan_buffer& oFan);

	void GameEnd();

	void Calc_BaseScore(_uint8 ChairID, int iBaseScore[GAME_PLAYER]);

	void Calc_HuScore(_uint8 ChairID, int iBaseScore[GAME_PLAYER], int iScore[GAME_PLAYER]);

	void Calc_ZhongNiao(_uint8 ChairID, _uint8 ShaiNum, int iBaseScore[GAME_PLAYER], int iScore[GAME_PLAYER]);

	void Calc_Piao(_uint8 ChairID, int iScore[GAME_PLAYER]);

	void TurnBanker();

	void GameEndReset();

	int GetListeningInfo(_uint8 ChairID, tagListenInfos stListenInfo[14], _uint8& bListenLen);

	bool Is7Dui(_uint8 ChairID, HandCards& pHc, MJ_fan_buffer& oFan);

	int Check_Hu_Pattern(_uint8 ChairID, const HandCards& pHc, MJ_fan_buffer& oFan, const MJ_win_pattern& oPattern);
	
	int Check_Hu(_uint8 ChairID, HandCards& pHc, MJ_fan_buffer& oFan);

	void save_game_scene_info();

	void CalcRecordInfo(const MJ_user_buffer& UserID, int iTotalScore[GAME_PLAYER]);

	void Proto_GameEnd_ScoreInfo(Player* player, proto::game::GameEndRecord &ger);

	void UserAfterOperate(_uint8 ChairID, int iOpType, _uint8 bCard);

	int dispatch_client(int cmd, Client *client, Player *player);
	
	void UserOperateGang(_uint8 ChairID, int iOpType, _uint8 bCard);

	void UserOperateHu(_uint8 ChairID, _uint8 bHuCard);

	void UserOperatePeng(_uint8 ChairID, _uint8 bCard);

protected:
    		
	int filterNoCardTingGroup(_uint8 ChairID, tagListenInfos stListenInfo[14], tagListenInfos tempListenInfo[14], _uint8 bListenLen);

	void StartChooseDingQue();

	void StartChooseHuanZhang();

	void RecvClientChooseDingQue(Client *client, Player * player);

	void RecvClientChooseDingQue2(_uint8 chairid, int type);

	int DingQueDecision(_uint8 chairId);

	void RecvClientChooseHuanZhang(Client *client, Player * player);

	void GameStartAfterHuanZhang();

	void GameStartAfterDingQue();

	bool Check_CanPaoHu(_uint8 ChairID, _uint8 bCard);

	void save_video_laizi_info(_uint8 bCard);

	int EstimateOtherHu(_uint8 ChairID, HandCards& pHc, MJ_fan_buffer& oFan); 

	// 双龙对
	bool IsShuangLongQiDui(_uint8 ChairID, HandCards& pHc, MJ_fan_buffer& oFan);
	// 龙七对
	bool IsLongQiDui(_uint8 ChairID, HandCards& pHc, MJ_fan_buffer& oFan);
	// 暗七对
	bool IsAnQiDui(_uint8 ChairID, HandCards& pHc, MJ_fan_buffer& oFan);
	// 七对
	bool IsQiDui(_uint8 ChairID, HandCards& pHc, MJ_fan_buffer& oFan);
	// 平胡
	bool IsPingHu(_uint8 ChairID, HandCards& pHc, MJ_fan_buffer& oFan);
	// 平胡
	bool IsPingHuLeiXing(_uint8 ChairID, HandCards& pHc, MJ_fan_buffer& oFan);
	// 对对胡
	bool IsDuiDuiHu(_uint8 ChairID, HandCards& pHc, MJ_fan_buffer& oFan);
	// 对对胡
	bool IsJiangDui(_uint8 ChairID, HandCards& pHc, MJ_fan_buffer& oFan);
	// 全妖九
	bool IsQuanYaoJiu(_uint8 ChairID, HandCards& pHc, MJ_fan_buffer& oFan);
	// 加番
	bool IsJiaFan(_uint8 ChairID, HandCards& pHc, MJ_fan_buffer& oFan);
	// 金钩胡
	bool IsJinGouHu(_uint8 ChairID, HandCards& pHc, MJ_fan_buffer& oFan);
	// 金钩胡
	bool IsMenQing(_uint8 ChairID, HandCards& pHc, MJ_fan_buffer& oFan);
	// 中张
	bool IsZhongZhang(_uint8 ChairID, HandCards& pHc, MJ_fan_buffer& oFan);
	// 金钩胡
	int IsGouGeng(_uint8 ChairID, HandCards& pHc, MJ_fan_buffer& oFan);
	// 一条龙
	bool IsYiTiaoLong(_uint8 ChairID, HandCards& pHc, MJ_fan_buffer& oFan);
	// 查大叫
	void CheckDaJiao(proto::game::chengdumj::AckCDGameEnd& stHuPaiInfo);
	// 退杠
	void CheckTuiGang(proto::game::chengdumj::AckCDGameEnd& stHuPaiInfo);
	// 大叫退杠
	void CheckDaJiaoTuiGang(proto::game::chengdumj::AckCDGameEnd& stHuPaiInfo);
	// 游戏结束重置一些记录信息
	void InitGameEndInfo();
	// 成都麻将配置项
	void ChengDuMJPeiZhi(proto::game::chengdumj::tagCDGameScence& stcqGameScence);
	// 游戏结束计算杠牌分
	void Proto_GameEnd_GangInfo(_uint8 ChairID, proto::game::chengdumj::HuPaiInfo* pstHuPaiInfo);
	// 大叫信息
	void Proto_GameEnd_DaJiaoInfo(_uint8 bCard[GAME_PLAYER], int nDaJiaoScore[GAME_PLAYER], proto::game::chengdumj::AckCDGameEnd& stHuPaiInfo);
	// 成都麻将胡牌信息
	void Proto_GameEndInfo_ChengDu_Hu_Info(_uint8 ChairID, _uint8 HuChairID, proto::game::chengdumj::HuPaiInfo* pstHuPaiInfo);
	// 计算杠分，需要考虑退杠
	void CalGangScore();
	// 抢杠胡虚函数
	bool RobGangHu(_uint8 chairID, _uint8 CardData, int iOpType = TYPE_WANGANG);
	// 广播玩家操作
	void Proto_Game_OperateGang(_uint8 ChairID, _uint8 bCard, int iOpType, int nGangScore[GAME_PLAYER], proto::game::AckBcOpResult& stOpresult);
	// 检查大叫信息
	int GetDaJiaoInfo(_uint8 ChairID, tagListenInfos stListenInfo[14], _uint8& bListenLen);
	// 夹心五
	bool IsJiaXinWu(_uint8 ChairID, HandCards& pHc, MJ_fan_buffer& oFan);
	// 杠分转移
	void TurnGangHouPaoFen();
	// 平杠分
	void PingGangFen(_uint8 ChairID, _uint8 nNum);
	// 转移杠分
	void TurnGangScore();
	// 计算胡牌顺序
	void Calc_HuPaiShunXu(int nGameEndTimes, _uint8 unCurChairId);
	// 出牌
	void OnUserOutCard(_uint8 ChairID, _uint8 bCard);
	// 记录点炮和胡的信息
	void CalcRecordDianPaoInfo(MJ_user_buffer& UserID, int iTotalScore[GAME_PLAYER]);
	// 手牌是否在手中
	bool IsCardInHand(_uint8 ChairID, _uint8 bCard);
	// 记录胡牌信息
	int Calc_Hu(_uint8 ChairID, _uint8 bCard, int Score[GAME_PLAYER]);
	//检测有没有缺牌，并选择一张缺牌
	virtual bool hasQuePai(_uint8 ChairID, _uint8 & queCard);
public:
    // 成员变量
    // 第几次走进GameEnd
    int m_GameEndTimes;	
    // 玩家胡牌顺序
	_uint8 m_nHuPaiShunXu[GAME_PLAYER];
    
protected:
	bool						m_DingQue_Timer_running;
	ev_timer                    m_DingQue_Timer;
	ev_tstamp                   m_DingQue_Timer_stamp;

	bool						m_HuanZhang_Timer_running;
	ev_timer                    m_HuanZhang_Timer;
	ev_tstamp                   m_HuanZhang_Timer_stamp;

	/*定缺机器人定时器*/
	bool						robot_dingque_timer_running[GAME_PLAYER - 1];
	ev_timer                    robot_dingque_timer[GAME_PLAYER - 1];
	ev_tstamp                   robot_dingque_stamp[GAME_PLAYER - 1];

	static void DingQue_Timer_cb(struct ev_loop *loop, struct ev_timer *w, int revents);
	static void HuanZhang_Timer_cb(struct ev_loop *loop, struct ev_timer *w, int revents);
	static void robot_dingque_Timer_cb(struct ev_loop *loop, struct ev_timer *w, int revents);

	void AutoPiao();
protected:
	//重庆麻将配置
	MJ_cq_conf_js m_conf_js;		             
	// 重庆麻将番信息
	chongqingmj_Fantype_to_FanNum m_chongqingmj_Fantype_to_FanNum;    
	//记录是否是庄家首张
	bool m_bFirstCard;						     
	//是否过胡
	bool m_bIsPassHu[GAME_PLAYER];			     
	//其他胡番类型
	CqOtherHu m_UserOtherHu[GAME_PLAYER];		 
	//选择其他胡
	bool m_bOtherHu[GAME_PLAYER];			     
	//用户已经定缺人数
	int m_nHaveDingQueNum;
	//用户定缺花色
	_uint8 m_UserDingQueSe[GAME_PLAYER];	
	//用户已经定缺人数
	int m_nHaveHuanZhangNum;
	//用户换张牌
	_uint8 m_UserHuanZhangPai[GAME_PLAYER][3];	 
	//玩家钱数
    int m_nPlayerMoney[GAME_PLAYER];
	//一炮多响点炮玩家
	_uint8 m_nYiPaoDuoXiangCharid;
	//上一局胡牌玩家数量
	int m_nLastHuPaiNum;
	// 勾牌数量
	int m_nGouPaiNum[GAME_PLAYER];
	// 胡牌信息
	ZiMoPaoHuInfo m_stZiMoPaoHuInfo[GAME_PLAYER];
	// 大叫分数
	int m_nDaJiaoFan[GAME_PLAYER];
    // 大叫分数
	int m_nDaJiao[GAME_PLAYER];
	// 退杠分数
	int m_nTuiGang[GAME_PLAYER];
	// 玩家的退杠之前的杠牌分
	int m_nGangScore[GAME_PLAYER];
	// 转移分数
	int m_nZhuanYiGang[GAME_PLAYER];
	// 上一次胡牌分
	int m_nLastHuScore[GAME_PLAYER];
	// 最后胡牌玩家
	_uint8 m_nHuCard[GAME_PLAYER];
	// 胡牌牌张排序
	_uint8 m_nHuCardShunXu[GAME_PLAYER];
	// 胡牌牌张排序
	_uint8 m_nZiMoHuCard[GAME_PLAYER];
	// 炮胡牌牌张排序
	_uint8 m_nPaoHuCard[GAME_PLAYER];
	// 胡牌类型
	_uint8 m_nHuType[GAME_PLAYER];
	// 玩家弃胡信息
	StructQiHu m_stQiHuInfo[GAME_PLAYER];
	// 点直杠的玩家
	_uint8 m_nZhiGangChairID;
	// 最后胡牌玩家
	_uint8 m_nHuPaiChairid;
	// 最后胡牌玩家
	int nOprOpType2;
	// 玩家弃胡信息
	StructGangZhuanYi m_stGangZhuanYiInfo[GAME_PLAYER];
	// 点炮信息
	StructDianPaoInfo m_stDianPaoInfo[GAME_PLAYER];
	// 弃杠信息
	bool m_bQiGang[GAME_PLAYER][5][10];
	// 是否可以胡地胡
	int m_bCanDiHu[GAME_PLAYER];
	// 是否可以胡地胡
	int m_nJiChuDiFen;
	// 统计信息
	STRecodeInfo m_stRecodeInfo[GAME_PLAYER];
	// 胡牌信息
	vector<int> m_ShuFenSeatID[GAME_PLAYER];

public:
	/*反序列化用，用于重启*/
	virtual void restart();
};

//BOOST_CLASS_EXPORT(Table_chengdumj)

#endif