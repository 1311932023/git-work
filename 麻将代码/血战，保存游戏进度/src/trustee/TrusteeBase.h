#ifndef __TRUSTEEBASE_H__
#define __TRUSTEEBASE_H__
#include <memory>
#include <ev.h>

#include "../common/boostHeader.hpp"

class TrusteeBase
{
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive & ar, const unsigned int /* file_version */) {
		printf("TrusteeBase::serialize\n");
		ar & m_Robot;
		ar & m_CurChairID;
	}

public:
	TrusteeBase()
	{
		for (int i = 0; i < GAME_PLAYER; i++)
			m_Robot[i] = false;
		m_CurChairID = 0;
	}
	virtual ~TrusteeBase() {}
	virtual void Init() {}
	virtual void OnUserGetCard(_uint8, _uint8) {}
	virtual void OnUserOutCard(_uint8, _uint8) {}
	virtual void EstimateUserRespond(_uint8) {}
//  	virtual void UserOperateChi(_uint8) {}
//  	virtual void UserOperatePeng(_uint8) {}
//  	virtual void UserOperateGang(_uint8) {}
//  	virtual void UserOperateHu(_uint8) {}	
//  	virtual void UserOperatePass(_uint8) {}
	virtual void UserAfterOperate(_uint8) {}
	virtual void DoSimpleOperate(_uint8, int) {}
	virtual void DoMultiOperate(_uint8, int) {}
	virtual void RobGangOperate(_uint8) {}
	virtual void GameEnd() {}	
	virtual void TriggerTimer(_uint8) {}

	virtual void restart() { printf("TrusteeBase::restart\n"); }
public:
	bool m_Robot[GAME_PLAYER];
	ev_tstamp m_Delay[GAME_PLAYER];
	_uint8 m_CurChairID;
};

BOOST_SERIALIZATION_ASSUME_ABSTRACT(TrusteeBase)
#endif