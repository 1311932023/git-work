#ifndef __TABLE_CHANGSHA_ZHUANZHUAN_H__
#define __TABLE_CHANGSHA_ZHUANZHUAN_H__
#include "../table.h"

struct MJ_conf_zhuanzhuan
{
	bool m_147niao;
	bool m_HuJiJiangJi;
	bool m_QuanZhongJiaBei;
	MJ_conf_zhuanzhuan()
	{
		m_147niao = false;
		m_HuJiJiangJi = false;
		m_QuanZhongJiaBei = false;
	}
};
class Table_cs_zhuanzhuan : public Table
{
public:
	Table_cs_zhuanzhuan();
	~Table_cs_zhuanzhuan();

public:
	void ConfInit_Remote(const Json::Value& val);

	void GameStart();

	void GameEnd();
	
	void GameEndReset();

	void TurnBanker();

	void UserGetFirstCard(_uint8 ChairID, int& iOpType, MJ_fan_buffer& oFan);

	void UserFirstOutCard(_uint8 ChairID, int& iOpType, MJ_fan_buffer& oFan);

	void UserGetLastCard(_uint8 ChairID, int& iOpType, MJ_fan_buffer& oFan);

	void UserLastOutCard(_uint8 ChairID, int& iOpType, MJ_fan_buffer& oFan);

	void UserGangShangHua(_uint8 ChairID, _uint8 bCard, MJ_fan_buffer& oFan);

	void UserGangHouPao(_uint8 ChairID, _uint8 bCard, MJ_fan_buffer& oFan);

	void Score_GameEndSucc(HuScoreSet& score, proto::game::AckGameEnd& stGameend);

	void Score_GameEndDraw(HuScoreSet& score, proto::game::AckGameEnd& stGameend);
	
	void OnUserOutCard(_uint8 bOutChairID, _uint8 bOutCard);

	int OutCardEstimate(_uint8 ChairID, _uint8 bCard, MJ_major& major, int& fan_num);

	int GetCardEstimate(_uint8 ChairID, _uint8 bCard, MJ_major& major, int& fan_num);

	int GetGangInfo(_uint8 ChairID, MJ_opinfo& GangCardInfo);

	void UserAfterOperate(_uint8 ChairID, int iOpType, _uint8 bCard);
	
	//int GetCardEstimate(_uint8 ChairID, _uint8 bGetCard, MJ_fan_buffer& oFan, int& fan_num);

	int Calc_Fan(_uint8 ChairID, MJ_fan_buffer& oFan);

	int Check_Hu_Pattern(_uint8 ChairID, const HandCards& pHc, MJ_fan_buffer& oFan, const MJ_win_pattern& oPattern);

	void Proto_GameScene(_uint8 ChairID, Player* player, proto::game::AckGameScene& stGameScene);
	
	void Proto_GameScene_lookon(_uint8 ChairID, Player* player, proto::game::AckGameScene& stGameScene);

	void Proto_GameEndInfo(const MJ_user_buffer& UserID, proto::game::AckGameEnd& stGameend);	

	//void Proto_Game_UserGetCard(_uint8 ChairID, _uint8 CurChairID, _uint8 bCard, proto::game::AckUserGetCard& stUsergetcard);	

	void DoSimpleOperate(_uint8 ChairID, int iOpType, _uint8 bCard);

	void DoMultiOperate(_uint8 ChairID, int iOpType, _uint8 bCard);

protected:
	MJ_conf_zhuanzhuan m_conf_zhuanzhuan;
	//_uint8 m_NiaoDouble;
};

#endif