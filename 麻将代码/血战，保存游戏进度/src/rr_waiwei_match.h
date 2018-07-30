#ifndef __RR_WAIWEI_MATCH_H__
#define __RR_WAIWEI_MATCH_H__
#include "rr_match.h"

class RRWW_Match : public RRMatch
{	
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive & ar, const unsigned int /* file_version */) {
		printf("RRWW_Match::serialize\n");
		ar & boost::serialization::base_object<RRMatch>(*this);
		/*boost::serialization::void_cast_register<RRWW_Match, RRMatch>(
			static_cast<RRWW_Match *>(NULL),
			static_cast<RRMatch *>(NULL)
			);*/
	}

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

	/*反序列化用，用于重启*/
	virtual void restart();
};

#endif