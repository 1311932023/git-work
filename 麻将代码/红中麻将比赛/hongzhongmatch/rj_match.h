#ifndef _RJ_MATCH_H_
#define _RJ_MATCH_H_

#include "match_ext.h"
#include "common/client.h"
#include "player.h"

class RJMatch: public MatchExt
{
private:
    typedef MatchExt Parent;
 public:
    RJMatch();
    virtual ~RJMatch();           

    void InitPlayerTid();
    void TurnReset();
    void UploadRealTimeRank();
    void TurnStart();
    void TurnOver();
    void NextTurn();
	void NextRound();
    void RoundOver(Table *table);
};




#endif
