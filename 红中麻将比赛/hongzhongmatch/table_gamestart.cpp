#include "table.h"
#include "game.h"
#include "common/log.h"
#include "match.h"

extern ZJH zjh;
extern Log log;
void Table::GameStart()
{
	if (m_bGameState != GAME_FREE)
	{
		log.error("game start error, now tid:%d gamestate is playing... \n", tid);
		return;
	}

	//初始化数据，庄家椅子id不能再被初始化
	InitData();

	m_Start_Shai = m_GameLogic.GetShaizi(2);  //复用  摇骰子坐庄

	InitBanker();

	log.fatal("%s:%d:%s tid:%d round:%d Banker uid:%d chairid:%d player[%d, %d, %d, %d]\n",__FILE__,__LINE__, __FUNCTION__, tid, m_nPlayingTimes, GetPlayerUid(m_bBanker), m_bBanker,
		GetPlayerUid(0), GetPlayerUid(1), GetPlayerUid(2), GetPlayerUid(3));

	save_video_palyer_info();
	save_game_scene_info();


	//发送游戏开始协议
	proto::game::AckGameStart stGamestart;
	if (m_Start_Shai.Num > 0)
	{
		stGamestart.add_touziinfo(m_Start_Shai.Shai[0] + 1);
		stGamestart.add_touziinfo(m_Start_Shai.Shai[1] + 1);
	}
	if (false)
	{
		//static int index = 0;
		//m_CardPool.FixCard(index++ %2);
		//m_CardPool.FixCard_ReadJson();
		m_CardPool.FixCard(0);
		m_bBanker = 0;
	}
	else if (m_cbInitCardPool)
	{		
		const CardPoolArgs* cp = match->InitCardPool(m_CurRound, match->CurTurn());
		m_CardPool.InitPool(cp->cards_pool, 
			cp->cards_size,
			cp->same_cards ? 0 : (matchid << 16) + tid);
		m_bBanker = cp->banker;
	}
	else
	{
		m_CardPool.InitPool();
	}

	stGamestart.set_chairid(m_bBanker);
	stGamestart.set_ischangbanker(m_last_Banker == INVALID_CHAIRID ? 0 :
		m_last_Banker == m_bBanker ? 0 : 1);
	broadcast(SERVER_GAME_START, stGamestart);
	m_cbInitTableState = true;
	m_bGameState = GAME_PLAYING;
	m_game_start = true;
	save_video_gamestar();
	for (int i = 0; i < m_GAME_PLAYER; i++)
	{
		if (m_SeatPlayer[i].player != NULL)
		{	
			if (m_SeatPlayer[i].player->table == NULL)
			{
				m_SeatPlayer[i].player->table = this;
			}
			m_SeatPlayer[i].player->status = GAME_PLAYING;
		}
	}

	Send_Hand_Card();

}
