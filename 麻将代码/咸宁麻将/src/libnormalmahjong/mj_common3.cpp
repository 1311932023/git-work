#include "mj_common3.h"
#include <assert.h>

void HandCardsAnalysis::AddCard(_uint8 bCard, _uint8 size)
{
	if (cards_hash[CardType(bCard)][CardNum(bCard)] == (_uint8)-1)
	{
		cards_hash[CardType(bCard)][CardNum(bCard)] = cards_len;
		cards_buffer[cards_len].cards = bCard;
		cards_buffer[cards_len++].counts = size;
	}
	else
	{
		assert(cards_hash[CardType(bCard)][CardNum(bCard)] < sizeof(cards_buffer));
		assert(cards_hash[CardType(bCard)][CardNum(bCard)] >= 0);
		cards_buffer[cards_hash[CardType(bCard)][CardNum(bCard)]].counts += size;
	}
}
void HandCardsAnalysis::RemoveCard(_uint8 bCard, _uint8 size)
{
	if (cards_hash[CardType(bCard)][CardNum(bCard)] == (_uint8)-1)
		return;
	assert(cards_buffer[cards_hash[CardType(bCard)][CardNum(bCard)]].counts >= size);
	cards_buffer[cards_hash[CardType(bCard)][CardNum(bCard)]].counts -= size;
}
HandCardsAnalysis::HandCardsAnalysis(const HandCards* pHc, bool fix )
{
	if (pHc == NULL)
	{
		throw("param is null!");
	}
	memset(cards_hash, -1, sizeof(cards_hash));
	memset(cards_buffer, 0, sizeof(cards_buffer));
	cards_len = 0;
	for (int i = 0; i < pHc->ChangeableCardsLen; i++)
	{
		AddCard(pHc->ChangeableCards[i]);
	}
	m_fix = fix;
	if (!fix)	return;
	for (int i = 0; i < pHc->FixedCardsLen; i++)
	{
		switch (pHc->FixedCards[i].state)
		{
		case TYPE_PENG:
			AddCard(pHc->FixedCards[i].CardData, 3);			
			break;
		case TYPE_ANGANG:
		case TYPE_WANGANG:
		case TYPE_ZHIGANG:
			AddCard(pHc->FixedCards[i].CardData, 4);			
			break;
		case TYPE_LEFT_CHI:
		case TYPE_RIGHT_CHI:
		case TYPE_CENTER_CHI:
			AddCard(pHc->FixedCards[i].c[0]);
			AddCard(pHc->FixedCards[i].c[1]);
			AddCard(pHc->FixedCards[i].c[2]);
			break;
		default:
			break;
		}
	}
}