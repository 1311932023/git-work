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
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive & ar, const unsigned int /* file_version */) {
		printf("CTrustee::serialize\n");
		ar & boost::serialization::base_object<TrusteeBase>(*this);
		/*boost::serialization::void_cast_register<CTrustee, TrusteeBase>(
			static_cast<CTrustee *>(NULL),
			static_cast<TrusteeBase *>(NULL)
			);*/
		ar & m_GuiCard;
		ar & m_CardCounts;
		ar & table;
		ar & trustee_timer_running;
	}
public:
	CTrustee(Table* t);
	CTrustee();
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
public:
	virtual void RobotOutCard(_uint8 ChairID);
	virtual void RobotGetOperate(_uint8 ChairID);
	virtual void RobotOutOperate(_uint8 ChairID);
	bool CheckWhetherPickUpGun(_uint8 ChairID, _uint8 huCard);
	virtual bool RobotOutCardScore(CardNode_set& h, CardNode_package& package, TrusteeScore& score);

	virtual void restart();
	std::vector<_uint8> m_GuiCard;
private:
	CardsCounts m_CardCounts;
protected:

	Table* table;	
	bool	 trustee_timer_running[GAME_PLAYER];
	ev_timer trustee_timer[GAME_PLAYER];
private:
	static void cbTimerOut0(struct ev_loop *loop, struct ev_timer *w, int revents);
	static void cbTimerOut1(struct ev_loop *loop, struct ev_timer *w, int revents);
	static void cbTimerOut2(struct ev_loop *loop, struct ev_timer *w, int revents);
	static void cbTimerOut3(struct ev_loop *loop, struct ev_timer *w, int revents);
};

#endif

