#include "CXueZhanTrustee.h"

CXueZhanTrustee::CXueZhanTrustee(Table* t) : CTrustee(t)
{

}

CXueZhanTrustee::CXueZhanTrustee()
{

}

void CXueZhanTrustee::RobotOutCard(_uint8 ChairID)
{
	_uint8 outCard = INVALID_MJ_CARD;
	if (table->hasQuePai(ChairID, outCard))
	{
		table->recv_client_outcard(ChairID, outCard);
	}
	else {
		CTrustee::RobotOutCard(ChairID);
	}
}