#ifndef _RR_MATCH_H_
#define _RR_MATCH_H_
#include <set>
#include "match_ext.h"
#include "player.h"

class RRMatch: public MatchExt
{
    typedef MatchExt Parent;

public:
    RRMatch();
    virtual ~RRMatch();
    void InitPlayerTid();
    void TurnReset();
    void UploadRealTimeRank();
    void TurnStart();
    void TurnOver();
    void NextTurn();

    int UploadMoveInfo();        
    void RoundOver(Table *table);
    void SwitzerlandMove();
	void HandlerRankListReq2(Player *player);

	int CurRound()const
	{ 
		if (m_single_turn_split)
		{
			int r = 0;
			for (int i = 0; i < (m_CurTurn - 1); i++)
			{
				r += m_TurnRoundNum[i];
			}
			r += m_CurRound;
			return r;
		}
		return m_CurRound;
	}
	int AllRound()const 
	{ 
		if (m_single_turn_split)
		{
			int r = 0;
			for (int i = 0; i < m_AllTurn; i++)
			{
				r += m_TurnRoundNum[i];
			}
			return r;
		}
		return m_AllRound;
	}
	int CurTurn()const 
	{
		if (m_single_turn_split)
		{
			return 1;
		}
		return m_CurTurn; 
	}
	int AllTurn()const
	{
		if (m_single_turn_split)
		{
			return 1;
		}
		return m_AllTurn; 
	}		
	void procSpawnCards();
	int InitRoundNumber()const {
		if (m_single_turn_split)
		{
			int r = 0;
			for (int i = 0; i < (m_CurTurn - 1); i++)
			{
				r += m_TurnRoundNum[i];
			}
			return r + 1;
		}
		return 1;
	}
};
#endif
