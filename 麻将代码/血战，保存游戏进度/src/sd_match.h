#ifndef __SD_MATCH_H__
#define __SD_MATCH_H__
#include "rj_match.h"

class SDMatch : public RJMatch
{
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive & ar, const unsigned int /* file_version */) {
		printf("SDMatch::serialize\n");
		ar & boost::serialization::base_object<RJMatch>(*this);
		/*boost::serialization::void_cast_register<SDMatch, MatchExt>(
			static_cast<SDMatch *>(NULL),
			static_cast<RJMatch *>(NULL)
			);*/
	}

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

	/*反序列化用，用于重启*/
	virtual void restart();
};

#endif