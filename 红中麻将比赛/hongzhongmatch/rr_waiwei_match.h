#ifndef __RR_WAIWEI_MATCH_H__
#define __RR_WAIWEI_MATCH_H__
#include "rr_match.h"

class RRWW_Match : public RRMatch
{	
	typedef RRMatch Parent;
	typedef Match Base;
public:
	RRWW_Match();
	void RoundOver(Table *table);
	void TurnOver();

	void RecordRank();
	int CalcTotalScore(float ratio);
	float ConvertTotalScore(int score);

	int CurRound()const { return m_CurRound; }
	int AllRound()const { return m_AllRound; }
	int CurTurn()const { return m_CurTurn; }
	int AllTurn()const { return m_AllTurn; }
};

#endif