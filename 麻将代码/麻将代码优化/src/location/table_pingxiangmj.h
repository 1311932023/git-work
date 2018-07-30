#ifndef __TABLE_PINGXIANGMJ_H__
#define __TABLE_PINGXIANGMJ_H__

#include "../libcommon/table.h"
#include "../protocol/pingxianggameproto.pb.h"


struct MJ_conf_pxmj
{
	bool bAutoHu;           //见牌胡	
	bool bNoneNiaoAsAll;    //不中算全中
	bool bAllNiaoDb;        //全中翻倍
	int iNiaoScore;         //鸟分
	bool bLouhuCanHu;       //漏胡也能胡
	bool bBankerScore;      //庄闲算分
	bool bHasFeng;          //有无风牌
	int iHongZhongNum;      //红中数
	bool bQingYiSeChi;      //清一色吃

	//代码内使用
	int iGameMode;      //玩法

	MJ_conf_pxmj()
	{
		bAutoHu = false;
		bNoneNiaoAsAll = false;
		bAllNiaoDb = false;
		bLouhuCanHu = false;
		bBankerScore = false;
		bHasFeng = false;
		iHongZhongNum = 0;
		iNiaoScore = 0;
		bQingYiSeChi = false;
		iGameMode = PLAY_MODE_PINGXIANGZZ;		
	}
};

class pingxiangmj_Fantype_to_FanNum : public Fantype_to_FanNum
{
public:
	pingxiangmj_Fantype_to_FanNum()
	{
        for (int i = 0; i < MJ_FAN_TYPE_MAX; ++i)
        {
            fan_num[i] = 1;
        }
        for (int i = 0; i < MJ_FAN_TYPE_SPECIAL_MAX - MJ_FAN_TYPE_SPECIAL; ++i)
        {
            special_fan_num[i] = 1;
        }
	
        fan_num[MJ_FAN_TYPE_PENGPENGHU] = 2;
        fan_num[MJ_FAN_TYPE_QINGYISE] = 2;
        fan_num[MJ_FAN_TYPE_YITIAOLONG] = 2;
        fan_num[MJ_FAN_TYPE_HAIDILAOYUE] = 2;
        fan_num[MJ_FAN_TYPE_QIDUI] = 2;     
        fan_num[MJ_FAN_TYPE_GANGSHANGHUA] = 2;
        
        special_fan_num[MJ_FAN_TYPE_LUANJIANGHU - MJ_FAN_TYPE_SPECIAL] = 2;
        special_fan_num[MJ_FAN_TYPE_13LUAN - MJ_FAN_TYPE_SPECIAL] = 2;
        special_fan_num[MJ_FAN_TYPE_JIANGJIANGHU - MJ_FAN_TYPE_SPECIAL] = 2;
        special_fan_num[MJ_FAN_TYPE_13FENG - MJ_FAN_TYPE_SPECIAL] = 2;

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


class Table_pingxiangmj : public Table
{
public:	
	Table_pingxiangmj();
	~Table_pingxiangmj();

public:
	void InitConf(const Json::Value &val, const std::string &roomid);

    void SaveProtoTable();

	void ParseProtoTable();

	void RecoverConfig();

	void Reset();

	void save_video_notify(proto::game::AckOpNotify &stNotify);

    void save_user_get_card(proto::game::AckUserGetCard &stUsergetcard);

    void save_video_game_end(proto::game::pingxiangmj::AckGameEndPingXiang& stGameEndPingXiang);

    int game_start_res(const Jpacket &packet);

	void GameStart();

	void SendLaiziCard();

	bool CheckFixedCardsType(_uint8 ChairID, _uint8 bCard);

    bool CheckUserOperator(_uint8 ChairID, int iOpType, _uint8 bCard);

    void UserOperatePass(_uint8 ChairID, int iOpType, _uint8 bCard);

    void UserOperateChi(_uint8 ChairID, int iOpType, _uint8 bCard);

    void UserOperateGang(_uint8 ChairID, int iOpType, _uint8 bCard);

    bool CheckCardGang(_uint8 ChairID, _uint8 bCard);   //检查指定牌是否能杠    

    void UserOperatePeng(_uint8 ChairID, _uint8 bCard);

	void UserAfterOperate(_uint8 ChairID, int iOpType, _uint8 bCard);

	int GetPengGang(_uint8 ChairID, _uint8 cbCard, MJ_opinfo& GangCardInfo, _uint8 laizi_card);
	int GetChi(_uint8 ChairID, _uint8 bCard, MJ_opinfo& ChiInfo);
	bool RobGangHu(_uint8 chairID, _uint8 CardData, int iOpType = TYPE_WANGANG);

	int GetCardEstimate(_uint8 ChairID, _uint8 bCard, MJ_major& major, int& fan_num);

	void OnUserGetCard(_uint8 ChairID, bool bGetLast = false);

	bool EstimateUserRespond(_uint8 bOutChairID, _uint8 bOutCard);

	void Proto_Game_UserGetCard(_uint8 ChairID, _uint8 MoChairID, _uint8 bCard, proto::game::AckUserGetCard& stUsergetcard);
    
	int OutCardEstimate(_uint8 ChairID, _uint8 bCard, MJ_major& major, int& fan_num);

	bool FormatCards(HandCards *hcs, MJ_win_pattern& pattern);

	int Calc_Fan(_uint8 ChairID, MJ_fan_buffer& oFan);

	int Calc_Hu(_uint8 ChairID, _uint8 bCard, int Score[GAME_PLAYER]);

	void Score_GameEndSucc(HuScoreSet& score, proto::game::pingxiangmj::AckGameEndPingXiang& stGameEndPingXiang);

	void Calc_BaseScore(_uint8 ChairID, int iBaseScore[GAME_PLAYER]);

	void Calc_HuScore(_uint8 ChairID, int iBaseScore[GAME_PLAYER], int iScore[GAME_PLAYER]);

	void Calc_GangScore(int iGangScore[GAME_PLAYER]);

	void Calc_ZhongNiao(_uint8 ChairID, _uint8 ShaiNum, int iBaseScore[GAME_PLAYER], int iScore[GAME_PLAYER], 
	                    proto::game::pingxiangmj::AckGameEndPingXiang& stGameEndPingXiang,
	                    bool& bMoNiao, int& iNiaoScoreStore);

	void Calc_Piao(_uint8 ChairID, int iScore[GAME_PLAYER]);

	void TurnBanker();

	int GetListeningInfo(_uint8 ChairID, tagListenInfos stListenInfo[14], _uint8& bListenLen);

	int handler_table_info(Player *player);

	bool Is7Dui(_uint8 ChairID, HandCards& pHc, MJ_fan_buffer& oFan);

	bool Is13Yao(HandCards& pHc, _uint8 c, _uint8 CardDataNum[MJ_TYPE_COMMON][10], int nHunNum, MJ_fan_buffer& oFan);

	int Check_Hu_Pattern(_uint8 ChairID, HandCards& pHc, MJ_fan_buffer& oFan, MJ_win_pattern& oPattern);
	
	int Check_Hu(_uint8 ChairID, HandCards& pHc, MJ_fan_buffer& oFan);

protected:
	int filterNoCardTingGroup(_uint8 ChairID, tagListenInfos stListenInfo[14], tagListenInfos tempListenInfo[14], _uint8 bListenLen);

	void GameEnd();

protected:
	MJ_conf_pxmj m_conf_pxmj;					//萍乡转转麻将配置
	pingxiangmj_Fantype_to_FanNum m_pingxiangmj_Fantype_to_FanNum; 

	MJ_gang             m_lou_gang[GAME_PLAYER];        //漏杠
	bool                m_bLouhu[GAME_PLAYER];          //漏胡玩家    
	_uint8              m_bChiCard[GAME_PLAYER];        //记录清一色吃牌
};

#endif
