#ifndef __MJ_COMMON3_H__
#define __MJ_COMMON3_H__
#include "mj_common2.h"

class HandCardsAnalysis
{
public:
	struct cards_node
	{
		_uint8 cards;
		_uint8 counts;
	};
	void AddCard(_uint8 bCard, _uint8 size = 1);	
	void RemoveCard(_uint8 bCard, _uint8 size = 1);
	HandCardsAnalysis(const HandCards* pHc, bool fix = false);	
	_uint8 NodeSize()	{ return cards_len; }	
	_uint8 Counts(_uint8 cards) const{ return Exist(cards) ? cards_buffer[cards_hash[CardType(cards)][CardNum(cards)]].counts : 0; }
	cards_node& operator[](int pos)
	{
		if (pos >= cards_len && pos < 0)
		{
			throw("beyond range");
		}
		return cards_buffer[pos];
	}
private:
	bool Exist(_uint8 cards) const{ return (cards_hash[CardType(cards)][CardNum(cards)] == (_uint8)-1) ? false : true; }
private:
	const HandCards* pHc;
	_uint8 cards_hash[MJ_TYPE_COMMON][10];
	cards_node cards_buffer[POOL_CARD_TYPE_MAX];
	_uint8 cards_len;
	bool m_fix;
};

#endif