#ifndef __CTRUSTEE_COMMON_H__
#define __CTRUSTEE_COMMON_H__
#include "../table.h"
#include "CardsCounts.h"
#include "TrusteeBase.h"


struct TrusteeScore
{
	_uint8 OutCard;
	int iOpType;
	TrusteeScore() {}
	TrusteeScore(_uint8 card, int type) : OutCard(card), iOpType(type) {}
};
class CTrustee : public TrusteeBase
{		
	typedef TrusteeBase base;
public:
	CTrustee(Table* t);
	~CTrustee();
	void Init();
	void OnUserGetCard(_uint8, _uint8);
	void OnUserOutCard(_uint8, _uint8);
	void EstimateUserRespond(_uint8);	
	void DoSimpleOperate(_uint8, int);
	void DoMultiOperate(_uint8, int);
	void UserAfterOperate(_uint8);
	void RobGangOperate(_uint8);
	void GameEnd();

	void TimerOut(_uint8 ChairID);
	void TriggerTimer(_uint8 ChairID);
	void UserOperate(_uint8, int);
public:
	virtual void RobotOutCard(_uint8 ChairID);
	virtual void RobotGetOperate(_uint8 ChairID);
	virtual void RobotOutOperate(_uint8 ChairID);
	bool CheckWhetherPickUpGun(_uint8 ChairID, _uint8 huCard);
	virtual bool RobotOutCardScore(CardNode_set& h, CardNode_package& package, TrusteeScore& score);
	std::vector<_uint8> m_GuiCard;
private:
	CardsCounts m_CardCounts;
private:

	Table* table;	
	ev_timer trustee_timer[GAME_PLAYER];
private:
	static void cbTimerOut0(struct ev_loop *loop, struct ev_timer *w, int revents);
	static void cbTimerOut1(struct ev_loop *loop, struct ev_timer *w, int revents);
	static void cbTimerOut2(struct ev_loop *loop, struct ev_timer *w, int revents);
	static void cbTimerOut3(struct ev_loop *loop, struct ev_timer *w, int revents);
};

#endif

