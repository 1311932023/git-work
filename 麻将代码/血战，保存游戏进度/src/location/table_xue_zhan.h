#ifndef __Table_xue_zhan_H__
#define __Table_xue_zhan_H__

#include "table_zz_laizi.h"

struct MJ_conf_hong_zhong
{
	bool m_bQiangGangQuanBao;
	bool m_bNoJokerDouble;
	_uint8 m_AddtionNiao;
	bool m_bJoker_No_PaoHu;
	int m_ScoreDouble[GAME_PLAYER];
	_uint8 m_NiaoFen;
	MJ_conf_hong_zhong()
	{
		m_bQiangGangQuanBao = false;
		m_bNoJokerDouble = false;
		m_AddtionNiao = 0;		
		m_bJoker_No_PaoHu = false;

		m_ScoreDouble[0] = 1;
		m_ScoreDouble[1] = 1;
		m_ScoreDouble[2] = 1;
		m_ScoreDouble[3] = 1;

		m_NiaoFen = 1;
	}
};
class  Table_xue_zhan : public Table_zz_laizi
{
public:
	Table_xue_zhan();
	~Table_xue_zhan();
public:
	
	void ConfInit_Remote(const Json::Value& val);

	void GameEndReset();
	
	void Score_GameEndSucc(HuScoreSet& score, proto::game::AckGameEnd& stGameend);

	void Proto_GameEndInfo(const MJ_user_buffer& UserID, proto::game::AckGameEnd& stGameend);	

	int OutCardEstimate(_uint8 ChairID, _uint8 bCard, MJ_major& major, int& fan_num);
	
	int Check_Hu(_uint8 ChairID, HandCards& pHc, MJ_fan_buffer& oFan);

	void TurnBanker() {	}
protected:
	MJ_conf_hong_zhong m_conf_hongzhong;	
	_uint8 m_QuanBaoPlayer[2];
};


#endif