#ifndef __MATCH_EXT_H__
#define __MATCH_EXT_H__
#include "match.h"
#include "common/client.h"
#include "player.h"

typedef std::list<Player*> lstPlayer;
typedef std::map<int, lstPlayer, std::greater<int> > mapPlayerRank;

class MatchExt : public Match
{
	typedef Match Parent;
public:
	MatchExt();
protected:
	std::map<int, std::multimap<float, Player*> >    m_PlayersRank;   //玩家排名情况<money,<time, player>>		
	std::map<int, Player*>      m_playersInSeat[GAME_PLAYER];//按座位分玩家   	
public:
	int UploadMPInfo();
public:
	//人机赛
	void CalculateCurRank(Player *player, int curRound);

	int StatisticGameResult(Table *table);

	void CalculateTotalRank(std::map<int, Player*>& map_players, bool update_rank, bool single_turn);

	void CalculateTotalRank3(int curround, bool single_turn);

	void CalculateTotalRank4(std::map<int, Player*>& map_players, bool update_rank, bool single_turn);

public:
	//人人赛
	void StatisticSeatRank(int,int);

	void EliminateRobot();

public:
	//总分赛
	void StatisticCommonRank();

	void CalculateCommonRank();

public:
	void StatisticScoreRank(int round, Player *player);
	//时段赛
	int UpdateMPInfo2();

	int CalculateTotalRank2(Player *player);

	int UpdateRank(int cur_round, bool bEnd);

	int UpdateRank_total();

public:
	void MatchOver();
};
#endif