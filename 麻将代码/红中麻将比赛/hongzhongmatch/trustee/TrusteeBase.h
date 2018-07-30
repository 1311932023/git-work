#ifndef __TRUSTEEBASE_H__
#define __TRUSTEEBASE_H__
#include <memory>
#include <ev.h>

class TrusteeBase
{
public:
	TrusteeBase()
	{
		for (int i = 0; i < GAME_PLAYER; i++)
		{
			m_Robot[i] = false;
			m_Delay[i] = 0.0f;
			m_timeout_count[i] = 0;
		}
		m_CurChairID = INVALID_CHAIRID;
		m_init_delay = 0.0f;
		m_init_timeout = 0.0f;
		m_auto_out_card = false;		
		m_UserAfterOperate = false;
	}
	virtual ~TrusteeBase() {}
	virtual void Init() {}
	virtual void OnUserGetCard(_uint8, _uint8) {}
	virtual void OnUserOutCard(_uint8, _uint8) {}
	virtual void EstimateUserRespond(_uint8) {}

	virtual void UserAfterOperate(_uint8) {}
	virtual void DoSimpleOperate(_uint8, int) {}
	virtual void DoMultiOperate(_uint8, int) {}
	virtual void RobGangOperate(_uint8) {}
	virtual void GameEnd() {}	
	virtual void TriggerTimer(_uint8) {}
	virtual void UserOperate(_uint8, int) {}

	bool isAutoOut(_uint8 ChairID) {
		return m_timeout_count[ChairID] > 1;
	}
public:
	bool m_auto_out_card;
	bool m_Robot[GAME_PLAYER];
	_uint8 m_CurChairID;
	bool m_UserAfterOperate;
	ev_tstamp m_Delay[GAME_PLAYER];	
	ev_tstamp m_init_delay;
	ev_tstamp m_init_timeout;
	int m_timeout_count[GAME_PLAYER];

};


#endif