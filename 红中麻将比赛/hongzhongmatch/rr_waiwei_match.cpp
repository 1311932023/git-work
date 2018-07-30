#include "rr_waiwei_match.h"
#include "zjh.h"
#include "common/log.h"

extern ZJH zjh;
extern Log log;

RRWW_Match::RRWW_Match()
{
	m_bScoreDependOnSeat = false;
	m_fBaseScore = 1000.0f;
	m_cbInitCardPool = false;
}

void RRWW_Match::RoundOver(Table *table)
{
	
	StatisticCommonRank();

	CalculateCommonRank();

	if (table->m_CurRound == m_AllRound) {
		m_TurnOverTableCnt++;
		if (m_CurTurn < m_AllTurn) {
			m_TurnOverTables.insert(table->tid);
			for (std::set<int>::iterator it = m_TurnOverTables.begin(); it != m_TurnOverTables.end(); ++it) {
				int tmpTid = *it;
				for (int i = 0; i < GAME_PLAYER; ++i) {
					HandlerTurnOverInfo(all_tables[tmpTid]->m_SeatPlayer[i].player);
				}
			}
		}
	}
		
	Base::RoundOver(table);
	//int player_size = all_players.size() + all_robots.size();
	int player_size = all_players.size();
	if (m_TurnOverTableCnt == ((player_size / GAME_PLAYER) + (player_size % GAME_PLAYER != 0))) {
		TurnOver();
	}
	else
	{
		ev_timer_again(zjh.loop, &m_NextRoundTimer);
	}
}
void RRWW_Match::TurnOver()
{
	RecordRank();

	Base::TurnOver();
	//将当前轮mp和rank存入redis并通知平台
	UploadMPInfo();	
	UploadTurnRank();

	//所有轮次完毕 赛事结束
	if (m_CurTurn == m_AllTurn) {
		MatchOver();
	}
	else {
		m_bInterval = true;
		ev_timer_again(zjh.loop, &m_SendPromotionTimer);
	}

}
void RRWW_Match::RecordRank()
{
	for (std::map<int, Player*>::iterator it = 	all_players.begin(); it!= all_players.end(); ++it)
	{
		Player* player = it->second;
		if (player != NULL)
		{
			player->m_TurnScore.push_back(player->m_RankInfo);
		}
	}
}

int RRWW_Match::CalcTotalScore(float ratio)
{
	return int(ratio + m_fBaseScore);
}
float RRWW_Match::ConvertTotalScore(int score)
{
	return float(score) - m_fBaseScore;
}