#ifndef _RJ_MATCH_H_
#define _RJ_MATCH_H_

#include "match_ext.h"
#include "common/client.h"
#include "player.h"

class RJMatch: public MatchExt
{
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive & ar, const unsigned int /* file_version */) {
		printf("RJMatch::serialize\n");
		ar & boost::serialization::base_object<MatchExt>(*this);
		/*boost::serialization::void_cast_register<RJMatch, MatchExt>(
			static_cast<RJMatch *>(NULL),
			static_cast<MatchExt *>(NULL)
			);*/
	}

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

	/*反序列化用，用于重启*/
	virtual void restart();
};




#endif
