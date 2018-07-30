#include "common_mahjong_logic.h"
#include "mj_common3.h"
#include <assert.h>


int HandCardsHasCard(HandCards *pHc, _uint8 uiCard)
{	
	int iCount = 0;
	for (int i = 0; i < pHc->ChangeableCardsLen; i++)
	{
		if (pHc->ChangeableCards[i] == uiCard)
		{
			iCount++;
		}
	}
	for (int i = 0; i < pHc->FixedCardsLen;i++)
	{
		if (pHc->FixedCards[i].CardData == uiCard)
		{
			iCount += pHc->FixedCards[i].state == TYPE_PENG ? 3 : 4;
			break;
		}
	}
	return iCount;
}

int HandCardsHasMultiCard(HandCards *pHc, _uint8 cuiCounts, MJ_cards_type_buffer& pCardsType)
{
	if (pHc == NULL)
	{
		return 0;
	}
	HandCardsAnalysis hca(pHc);	
	for (int i = 0; i < hca.NodeSize(); i++)
	{
		if (hca[i].counts == cuiCounts)
		{
			pCardsType.Add(hca[i].cards);
		}
	}
	return pCardsType.Size();
}

bool HandCardsHasAnyCard(HandCards* pHc, const MJ_cards_type_buffer& pCardsType, MJ_cards_type_buffer& pOut)
{
	if (pHc == NULL)
	{
		return false;
	}
	if (0 == pCardsType.Size())
	{
		return false;
	}
	HandCardsAnalysis hca(pHc, true);
	for (int i = 0; i < pCardsType.Size(); i++)
	{
		if (hca.Counts(pCardsType[i]) != 0)
		{
			pOut.Add(pCardsType[i]);
		}
	}	
	return pOut.Size() != 0;
}
bool HandCardsHasSpecialNumber(HandCards* pHc, _uint8 uiNumber)
{
	for (int i = 0; i < pHc->ChangeableCardsLen; i++)
	{
		if (CardType(pHc->ChangeableCards[i]) < 0x3 && CardNum(pHc->ChangeableCards[i]) == uiNumber)
		{
			return true;
		}
	}
	return false;
}

void HandCardsSuitCounts(HandCards* pHc, _uint8 uiSuitCount[5])
{
	uiSuitCount[0] = 0;
	uiSuitCount[1] = 0;
	uiSuitCount[2] = 0;
	uiSuitCount[3] = 0;
	uiSuitCount[4] = 0;

	for (int i = 0; i < pHc->ChangeableCardsLen; i++)
	{	
		uiSuitCount[CardType(pHc->ChangeableCards[i])]++;
	}
	for (int i = 0; i < pHc->FixedCardsLen; i++)
	{
		uiSuitCount[CardType(pHc->FixedCards[i].CardData)] += pHc->FixedCards[i].state == TYPE_PENG ? 3 : 4;
	}
}