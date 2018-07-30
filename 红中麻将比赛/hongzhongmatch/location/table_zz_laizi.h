#ifndef __TABLE_LAIZI_H__
#define __TABLE_LAIZI_H__
#include "table_changsha_zhuanzhuan.h"

struct MJ_zz_laizi_Conf
{
	bool m_bQishouHu;
	MJ_zz_laizi_Conf()
	{
		m_bQishouHu = false;
	}
};
class Table_zz_laizi :public Table_cs_zhuanzhuan
{
public:
	Table_zz_laizi();
	~Table_zz_laizi();
public:
	void ConfInit_Remote(const Json::Value& val);

	bool FormatCards(HandCards *hcs, MJ_win_pattern& pattern);	

	int GetGangInfo(_uint8 ChairID, MJ_opinfo& GangCardInfo);	
	
	void UserGetFirstCard(_uint8 ChairID, int& iOpType, MJ_fan_buffer& oFan);

	int OutCardEstimate(_uint8 ChairID, _uint8 bCard, MJ_major& major, int& fan_num);

	void Proto_GameScene(_uint8 ChairID, Player* player, proto::game::AckGameScene& stGameScene);

	void Proto_GameScene_lookon(_uint8 ChairID, Player* player, proto::game::AckGameScene& stGameScene);

	bool Is7Dui(_uint8 ChairID, HandCards& pHc, MJ_fan_buffer& oFan);

	virtual bool Hu_Special_zz_laizi(_uint8 ChairID, MJ_fan_buffer& oFan);
protected:
	_uint8 m_LaiziCard;
	MJ_zz_laizi_Conf m_laizi_conf;
};

#endif