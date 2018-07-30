#ifndef __SD_MATCH_H__
#define __SD_MATCH_H__
#include "rj_match.h"

class SDMatch : public RJMatch
{
	typedef RJMatch Parent;
	typedef Match Base;
public:
	SDMatch();
	~SDMatch();

	void InitPlayerTid();
	void RoundOver(Table *table);

	void TurnOver();
	void MatchOver();

	void NextRound();

	void TurnReset() {}

	void RecordRank(Player* player);

	void NextTurn();
};

#endif