#include <algorithm>
#include "sd_match.h"
#include "zjh.h"
#include "common/log.h"

extern ZJH zjh;
extern Log log;

SDMatch::SDMatch()
{
	m_fBaseScore = 1.0f;
	m_bScoreDependOnSeat = true;	
	m_bSameCard = false;
	m_cbInitCardPool = true;
	m_bCalcMpScore = false;
}

SDMatch::~SDMatch()
{

}

void SDMatch::InitPlayerTid()
{

}

void SDMatch::RoundOver(Table *table)
{

	Player* player = table->m_SeatPlayer[0].player;
	StatisticScoreRank(table->m_CurRound, player);	
	UpdateRank(player->m_CurRound, false);

	Base::RoundOver(table);
	if (table->m_CurRound >= m_AllRound)
	{	
		RecordRank(player);
		
		CalculateTotalRank2(player);

		UpdateRank_total();
		
		for (int i = 0; i < table->m_GAME_PLAYER; i++)
		{
			if (DelRobot(table->m_SeatPlayer[i].player))
			{	
				table->m_SeatPlayer[i].clear();				
			}
		}
		m_TurnOverTables.insert(table->tid);				
		
		if (table->m_CurRound >= m_AllRound && m_CurTurn >= m_AllTurn)
		{
			if (m_State == MATCH_OVER &&
				all_tables.size() == m_TurnOverTables.size())
			{
				MatchOver();
			}	
		}				
	}
	else
	{		
		TIMER_AGAIN(table->m_NextRound_timer, table->m_NextRound_stamp);
	}
}
void SDMatch::TurnOver()
{	

}
void SDMatch::MatchOver()
{
	UpdateRank_total();
	UpdateMPInfo2();

	Base::MatchOver();
	
	for (std::map<int, Player*>::iterator it = all_players.begin(); it != all_players.end(); ++it)
	{
		Player* player = it->second;
		if (player->client) {
			Client *client = player->client;
			client->player = NULL;
			Game::SendLogout(client, player->uid, 106);
		}		
	}
	log.info("%s match %d over \n", __FUNCTION__, matchid);	
	
}

void SDMatch::NextRound()
{
	for (std::map<int, Table*>::iterator it = all_tables.begin(); it != all_tables.end(); ++it) {
		Table* table = it->second;
		if (table->m_bGameState == GAME_FREE)
		{
			table->m_CurRound++;
			TableGameStart(table);
		}
	}
	log.info("%s roundinfo round:%d/%d turn:%d/%d\n", __FUNCTION__, m_CurRound, m_AllRound, m_CurTurn, m_AllTurn);
}

void SDMatch::RecordRank(Player* player)
{
	if (player != NULL)
	{
		if (player->m_TurnScore.empty())
		{
			player->m_TurnScore.push_back(player->m_RankInfo);
		}
		else
		{
			if (RankInfo::LessCB(player->m_RankInfo, player->m_TurnScore[0]))
			{
				player->m_TurnScore[0] = player->m_RankInfo;
			}
		}
	}
}
void SDMatch::NextTurn()
{	
	m_CurTurn++;
}