#include "table.h"
#include "match.h"

using namespace proto::game::guizhou;

void Table::PackageTingInfoV(_uint8 ChairID, tagListenInfos stListenInfo[14], _uint8 iPos, void* info)
{
	proto::game::guizhou::tagListenInfo* listeninfo = (proto::game::guizhou::tagListenInfo*) info;
	listeninfo->set_outcard(stListenInfo[iPos].bOutCard);
	for (int k = 0; k < stListenInfo[iPos].bHuLen; k++)
	{
		proto::game::guizhou::tagHuInfo *pstHuinfo = listeninfo->add_huinfo();
		_uint8 bListenCard = stListenInfo[iPos].HuInfo[k].bCard;
		pstHuinfo->set_card(bListenCard);
		pstHuinfo->set_hufannum(stListenInfo[iPos].HuInfo[k].bFanShu);
		int nCardLeftNum = Get_Card_Left_Num(ChairID, bListenCard);
		pstHuinfo->set_leftnum(nCardLeftNum);
	}
}

void Table::Send_PiaoFen(_uint8 ChairID, int iScore[GAME_PLAYER])
{

	proto::game::tagUserState state;
	state.set_chairid(ChairID);
	state.set_state(0);
	for (int i = 0; i < GAME_PLAYER; i++)
	{
		state.add_score(iScore[i]);
	}
	broadcast(SERVER_PIAO_FEN, state);

}
int Table::PlayerLogin(Player *player, bool login)
{
	if (login)
	{
		handler_login_succ_uc(player);
		handler_login_succ_bc(player);
	}
	if (!player->m_isRobot)
	{
		handler_table_info(player);
		Send_Game_Scene(player);
	}

	return 0;
}

void Table::OnNotify(int msg, int ChairID, int action)
{
	if (msg == SERVER_TRUSTEE)
	{
		proto::game::AckIsAuto stAck;
		stAck.set_chairid(ChairID);
		stAck.set_isauto(action);

		broadcast(SERVER_TRUSTEE, stAck);
	}
}