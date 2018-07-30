#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <vector>
#include <assert.h>
#include <stdio.h>

#include "game_logic.h"
#include <stdio.h>
#include "../common/log.h"



#define _CARD_TEST

extern Log log;

int power(int base, int times)
{
	int r = 1;
	for (int i = 0; i < times; i++)
	{
		r *= base;
	}
	return r;
}
bool isjiang(_uint8 card)
{
	return true;
}
bool is258jiang(_uint8 card)
{
	static bool num[] = { false, false, true, false, false, true, false, false, true, false, false };
	if (CardType(card) > MJ_TYPE_TONG) return false;
	return num[CardNum(card)];
}
bool ishongzhongjiang(_uint8 card)
{
	return card == MJ_HONG_ZHONG;
}
CGameLogic::CGameLogic(void) : m_cbJiang(isjiang)
{

}

CGameLogic::~CGameLogic(void)
{

}

void CGameLogic::AddCard(HandCards *pHc, _uint8 bCard)
{
	if (bCard == 0)
		return;
	if (bCard == INVALID_MJ_CARD)
	{
		SortCard(pHc->ChangeableCards, pHc->ChangeableCardsLen);
		return;
	}
	else
	{
        pHc->ChangeableCards[pHc->ChangeableCardsLen] = bCard;
		pHc->ChangeableCardsLen++;
	}	
	SortCard(pHc->ChangeableCards, pHc->ChangeableCardsLen);		
	if (pHc->ChangeableCardsLen > HAND_CARD_SIZE_MAX)	
	{
		char words[500];
		sprintf(words, "%s(%d)%s", __FILE__, __LINE__, __FUNCTION__);
		throw(words);		
	}
	return;
}

void CGameLogic::SortCard(_uint8 p[], _uint8 len)
{
	int i,j;
	for (i=0; i<len; i++)
	{
		for (j=i+1; j<len; j++)
		{
			if (p[i]>p[j])
			{
				_uint8 tempCard = p[i];
				p[i] = p[j];
				p[j] = tempCard;
			}
		}		
	}
}

bool CGameLogic::ChuPai(HandCards *pHc, _uint8 bCard)
{
	bool bfind = false;
	for (int i=0; i<pHc->ChangeableCardsLen; i++)
	{
		if (pHc->ChangeableCards[i] == bCard)
		{
			bfind = true;
			for (int j = i+1;j<pHc->ChangeableCardsLen;j++)
			{
				pHc->ChangeableCards[j-1] = pHc->ChangeableCards[j];        
			}
			break;
		}
	}
	if (bfind)
	{	
		pHc->ChangeableCards[pHc->ChangeableCardsLen-1] = INVALID_MJ_CARD;
		pHc->ChangeableCardsLen--;
	}	
	return bfind;
}

int CGameLogic::GetPengGang(HandCards *pHc, _uint8 bCard, MJ_opinfo& GangCardInfo)
{
	GangCardInfo.Clear();
	int count = 0;
	for (int i = 0; i < pHc->ChangeableCardsLen; i++)
	{
		if (bCard == pHc->ChangeableCards[i])
		{
			count++;
		}
	}
	int iOpType = TYPE_NULL;
	if (count >= 2)
	{		
		GangCardInfo.Add(OpState(bCard, TYPE_PENG));
		iOpType |= TYPE_PENG;
	}
	if (count >= 3)
	{		
		GangCardInfo.Add(OpState(bCard, TYPE_ZHIGANG));
		iOpType |= TYPE_ZHIGANG;
	}	
	return iOpType;
}


int CGameLogic::GetGangInfo(const HandCards*pHc, MJ_opinfo& ganginfo)
{
	_uint8 bCardCount[MJ_TYPE_COMMON][10];
	memset(bCardCount, 0, sizeof(bCardCount));
	ganginfo.Clear();
	int iOpType = TYPE_NULL;
	for (int i = 0; i < pHc->ChangeableCardsLen; i++)
	{
		_uint8 bType = CardType(pHc->ChangeableCards[i]);
		_uint8 bNum = CardNum(pHc->ChangeableCards[i]);
		
		bCardCount[bType][bNum]++;
		if (bCardCount[bType][bNum] == 4)
		{
			ganginfo.Add(OpState(pHc->ChangeableCards[i], TYPE_ANGANG));
			iOpType |= TYPE_ANGANG;
		}
		
	}
	for (int i = 0; i < pHc->FixedCardsLen; i++)
	{
		if (pHc->FixedCards[i].state == TYPE_PENG)
		{
			for (int j = 0; j < pHc->ChangeableCardsLen; j++)
			{
				if (pHc->FixedCards[i].CardData == pHc->ChangeableCards[j])
				{					
					ganginfo.Add(OpState(pHc->ChangeableCards[j], TYPE_WANGANG));
					iOpType |= TYPE_WANGANG;
					break;
				}
			}
		}
	}
	return iOpType;
}

bool CGameLogic::GetWanGang(const HandCards* pHc, MJ_opinfo& ganginfo, _uint8 bCard)
{
	ganginfo.Clear();
	if (bCard == INVALID_MJ_CARD)
	{
		for (int i = 0; i < pHc->FixedCardsLen; i++)
		{
			if (pHc->FixedCards[i].state == TYPE_PENG)
			{
				for (int j = 0; j < pHc->ChangeableCardsLen; j++)
				{
					if (pHc->FixedCards[i].CardData == pHc->ChangeableCards[j])
					{
						ganginfo.Add(OpState(pHc->ChangeableCards[j], TYPE_WANGANG));
					}
				}
			}
		}
		return ganginfo.NotNull();
	}
	else
	{
		for (int i = 0; i < pHc->FixedCardsLen; i++)
		{
			if (pHc->FixedCards[i].state == TYPE_PENG && pHc->FixedCards[i].CardData == bCard)
			{
				ganginfo.Add(OpState(pHc->FixedCards[i].CardData, TYPE_WANGANG));
				return true;
			}
		}
	}
	return false;
}
bool CGameLogic::GetZhiGang(const HandCards* pHc, MJ_opinfo& ganginfo, _uint8 bCard)
{	
	ganginfo.Clear();
	if (bCard == INVALID_MJ_CARD)
	{
		return false;
	}
	else
	{
		int iCounts = 0;		
		for (int i = 0; i < pHc->ChangeableCardsLen; i++)
		{
			if (pHc->ChangeableCards[i] == bCard)
			{				
				if (++iCounts == 3)
				{
					ganginfo.Add(OpState(pHc->ChangeableCards[i], TYPE_ZHIGANG));
					return true;
				}
			}
		}
		return false;
	}
}
bool CGameLogic::GetAnGang(const HandCards* pHc, MJ_opinfo& ganginfo, _uint8 bCard)
{
	ganginfo.Clear();
	if (bCard == INVALID_MJ_CARD)
	{
		_uint8 bCardCount[MJ_TYPE_COMMON][10];
		memset(bCardCount, 0, sizeof(bCardCount));				
		for (int i = 0; i < pHc->ChangeableCardsLen; i++)
		{
			_uint8 bType = CardType(pHc->ChangeableCards[i]);
			_uint8 bNum = CardNum(pHc->ChangeableCards[i]);
			if (++bCardCount[bType][bNum] == 4)
			{
				ganginfo.Add(OpState(pHc->ChangeableCards[i], TYPE_ANGANG));				
			}
		}
		return ganginfo.NotNull();
	}
	else
	{
		int iCounts = 0;
		for (int i = 0; i < pHc->ChangeableCardsLen; i++)
		{
			if (pHc->ChangeableCards[i] == bCard)
			{
				if (++iCounts == 4)
				{
					ganginfo.Add(OpState(pHc->ChangeableCards[i], TYPE_ANGANG));
					return true;
				}
			}
		}
	}
	return true;
}
Shai_Set CGameLogic::GetShaizi(_uint8 iCount)
{	
	assert(iCount < 8);
	Shai_Set set;
	srand((int)time(0));

	for (int i = 0; i < iCount; i++)
	{
		set.Shai[i] = rand() % 6;
	}
	set.Num = iCount;
	return set;
}

//操作函数
void CGameLogic::ExecuteGang(HandCards *pHc, _uint8 bOpCardChairID, _uint8 cbOperateCard, int cbType)
{

	if (cbType == TYPE_WANGANG)
	{
		RemoveCard(pHc, cbOperateCard, 1);
		for (int i=0; i<pHc->FixedCardsLen; i++)
		{
			if (pHc->FixedCards[i].CardData == cbOperateCard)
			{
				pHc->FixedCards[i].state = cbType;
				//pHc->FixedCards[i].chairID = bOpCardChairID;
			}
		}
	}
	else
	{
		_uint8 bLen = (cbType == TYPE_ANGANG ? 4 : 3);
		RemoveCard(pHc, cbOperateCard, bLen);
		pHc->FixedCards[pHc->FixedCardsLen].CardData = cbOperateCard;
		pHc->FixedCards[pHc->FixedCardsLen].state = cbType;
		pHc->FixedCards[pHc->FixedCardsLen].chairID = bOpCardChairID;
		pHc->FixedCardsLen++;
	}
}
void CGameLogic::ExecuteQiangGang(HandCards *pHc, _uint8 bOpCardChairID, _uint8 bOperateCard, int iOpType)
{
	for (int i = 0; i < pHc->FixedCardsLen; i++)
	{
		if (pHc->FixedCards[i].state == iOpType && pHc->FixedCards[i].CardData == bOperateCard)
		{
			pHc->FixedCards[i].state = TYPE_PENG;
			break;
		}
	}
}
void CGameLogic::ExecuteQiangGang_Special(HandCards *pHc, _uint8 ChairID, _uint8 bCard)
{	
	RemoveCard(pHc, bCard, 3);
	pHc->FixedCards[pHc->FixedCardsLen].CardData = bCard;
	pHc->FixedCards[pHc->FixedCardsLen].state = TYPE_PENG;
	pHc->FixedCards[pHc->FixedCardsLen].chairID = ChairID;
	pHc->FixedCardsLen++;
}
void CGameLogic::ExecutePeng(HandCards *pHc, _uint8 bOpCardChairID, _uint8 cbOperateCard)
{
	RemoveCard(pHc, cbOperateCard, 2);
	pHc->FixedCards[pHc->FixedCardsLen].CardData = cbOperateCard;
	pHc->FixedCards[pHc->FixedCardsLen].state = TYPE_PENG;
	pHc->FixedCards[pHc->FixedCardsLen].chairID = bOpCardChairID;
	pHc->FixedCardsLen++;
}


/*
void CGameLogic::ChangeCard(tagChangeCard stUserCard[4])
{
    //换牌，优先换到不是自己的牌
	_uint8 bCard[12] = {0};
	_uint8 bLen = 0;
    for (int i=0; i<4; i++)
    {
        if (stUserCard[i].bLen == 0)
        {
            continue;
        }
		memcpy(bCard + bLen, stUserCard[i].bCard, sizeof(_uint8)*stUserCard[i].bLen);
        bLen += stUserCard[i].bLen;
    }

	_uint8 btemplen = 0;
    for (int i=3; i>=0; i--)
    {
        memcpy(stUserCard[i].bCard, bCard+btemplen, sizeof(_uint8)*stUserCard[i].bLen);
        btemplen += stUserCard[i].bLen;
    }
}
*/
//输入：手牌，当前牌，返回值：不能碰则返回false
bool CGameLogic::EstimateFixCard(HandCards *pHC, _uint8 cbCardData)
{
	if (pHC->FixedCardsLen < 2)
	{
		return true;
	}
	_uint8 cbNum[MJ_TYPE_NUM] = {0};
	_uint8 cbType = CardType(cbCardData);
	for (int i=0; i<pHC->FixedCardsLen; i++)
	{
		cbNum[CardType(pHC->FixedCards[i].CardData)]++;
	}
	if (cbNum[cbType] > 0)
	{
		return true;
	}

	_uint8 cbTypeNum = 0;
	for (int i = 0; i < MJ_TYPE_NUM; ++i)
	{
		if (cbNum[i] > 0)++cbTypeNum;
	}
	return (cbTypeNum < 2);//碰了两种花色，第三种不能碰,
}

void CGameLogic::RemoveCard(HandCards *hcs, _uint8 c, int length)
{
	for (int i=0;i<length;i++)
	{
		ChuPai(hcs, c);
	}

}


void CGameLogic::RemoveCard(HandCards *pHc, _uint8 c)
{	
	for (int i = 0; i < pHc->ChangeableCardsLen;)
	{
		if (pHc->ChangeableCards[i] == c && (0 != --pHc->ChangeableCardsLen - i))
		{
			memmove(&pHc->ChangeableCards[i], &pHc->ChangeableCards[i + 1], pHc->ChangeableCardsLen - i);
			continue;
		}
		i++;
	}	
}


void CGameLogic::RemoveCard(_uint8 sCard[], _uint8 cblen, _uint8 c)
{
	bool bfind=false;
	for (int i=0;i < cblen; i++)
	{
		if (sCard[i] != c)continue;
		bfind = true;
		for (int j = i+1;j < cblen; j++)
		{
			sCard[j-1] = sCard[j];
		}
		break;
	}
	if (bfind)
	{	
		sCard[cblen-1]=INVALID_MJ_CARD;
	}
}

//功能函数 添加 wTableID 参数 防止同一时间内两桌发牌相同
void CGameLogic::RandomCard(_uint8 cbCard[], _uint8 cbLength, int iTableID)
{
	srand(_uint32(time(0)) + iTableID);

	int index;
	for (int i=0; i<cbLength; i++)
	{
		index = i + rand()%(cbLength-i);/*rand()%cbLength;*/
		if (i != index)
		{   
			_uint8 tempCard = cbCard[i];
			cbCard[i] = cbCard[index];
			cbCard[index] = tempCard;
		}
	}
}

void CGameLogic::RemoveCard(HandCards *hcs, _uint8 *c, int length)
{
	for (int i = 0; i < length; i++)
	{
		ChuPai(hcs, *(c + i));
	}
}

bool CGameLogic::FormatCards(HandCards *hcs, _uint8 CardsFormatBuf[4][4], _uint8 &Jiang)
{
	if (!hcs) return false;

	memset((char *)CardsFormatBuf, INVALID_MJ_CARD, 4 * 4);

	//-------------------
	_uint8 RestCardBuf[14] = { 0 };
	int count[4] = { 0 };
	int i, j, k;
	bool bJiang;
	bool b = false;
	int  SerialNum = 0;
	int JiangIndex = 0;
	//-------------------

	bJiang = GetJiang(hcs->ChangeableCards, hcs->ChangeableCardsLen, JiangIndex, Jiang, RestCardBuf);
	SerialNum = (hcs->ChangeableCardsLen - 2) / 3;

	if (SerialNum > 4) return false;
	if (hcs->ChangeableCardsLen - 2 > 13) return false;

	while (bJiang)
	{
		for (i = 0; i < 4; i++)
			count[i] = 0;
		for (i = 0; i < hcs->ChangeableCardsLen - 2; i++)
		{
			b = false;
			for (j = 0; j<SerialNum; j++)
			{
				if (count[j] == 0)
				{
					CardsFormatBuf[j][0] = RestCardBuf[i];
					count[j] ++;
					b = true;
					break;
				}
				else if (count[j] == 3)
					continue;
				else if (CardType(RestCardBuf[i]) > MJ_TYPE_BING)//字牌处理
				{
					if (CardsFormatBuf[j][count[j] - 1] == RestCardBuf[i])
					{
						CardsFormatBuf[j][count[j]] = RestCardBuf[i];
						count[j] ++;
						b = true;
						break;
					}
				}
				else//数牌处理
				{					
					//数牌刻子
					if (RestCardBuf[i] == CardsFormatBuf[j][count[j] - 1])
					{
						if (count[j] == 1)
						{
							if (RestCardBuf[i] == RestCardBuf[i + 1])
							{
								CardsFormatBuf[j][count[j]] = RestCardBuf[i];
								count[j] ++;
								b = true;
								break;
							}
						}
						else if (count[j] == 2)
						{
							if (CardsFormatBuf[j][0] == CardsFormatBuf[j][1])
							{
								CardsFormatBuf[j][count[j]] = RestCardBuf[i];
								count[j] ++;
								b = true;
								break;
							}
						}
					}
					//数牌顺子
					else if ((RestCardBuf[i] - CardsFormatBuf[j][count[j] - 1]) == 1)
					{
						CardsFormatBuf[j][count[j]] = RestCardBuf[i];
						count[j] ++;
						b = true;
						break;
					}
					
				}
			}
			if (!b)
				break;
		}
		if (b || SerialNum == 0)
		{
			if (hcs->FixedCardsLen > 4) return false;

			j = (hcs->ChangeableCardsLen - 2) / 3;

			for (i = 0; i < hcs->FixedCardsLen; i++)
			{
				if (j >= 4) return false;

				switch (hcs->FixedCards[i].state)
				{
				case TYPE_ZHIGANG:
				case TYPE_WANGANG:
				case TYPE_ANGANG:
				{
					for (k = 0; k < 4; k++)
					{
						CardsFormatBuf[j][k] = hcs->FixedCards[i].c[0];
					}
				}
				break;
				case TYPE_PENG:
				{
					for (k = 0; k < 3; k++)
					{
						CardsFormatBuf[j][k] = hcs->FixedCards[i].c[0];
					}
				}
				break;
				default:
					for (k = 0; k < 3; k++)
					{
						CardsFormatBuf[j][k] = hcs->FixedCards[i].c[k];
					}
					break;
				}
				j++;
			}
			return true;
		}
		JiangIndex += 2;
		bJiang = GetJiang(hcs->ChangeableCards, hcs->ChangeableCardsLen, JiangIndex, Jiang, RestCardBuf);
	}
	return false;
}

bool CGameLogic::GetJiang(_uint8* p, int len, int& JiangIndex, _uint8& jiang, _uint8* pRestCardBuf)
{
	SortCard(p,len);
	int i,j;
	for (i=0; i<JiangIndex; i++)
	{
        pRestCardBuf[i] = p[i]; 
	}
		
	for (i=JiangIndex; i<len-1; i++)
	{
		if (p[i] == p[i+1])
		{
			_uint8 card = p[i];
			if (!m_cbJiang(card))
				continue;
			JiangIndex =i;
			jiang = p[i];
			for (j=i+2; j<len; j++)
			{
                pRestCardBuf[j-2]=p[j];
			}
			return true;
		}
		pRestCardBuf[i] = p[i];
	}

	JiangIndex=len-1;
	return false;
}
bool CGameLogic::HasCard(const HandCards *hc, _uint8 c)
{
	for (int i=0; i<hc->ChangeableCardsLen; i++)
	{
		if (hc->ChangeableCards[i]==c)
		{
			return true;
		}
	}
	return false;
}

_uint8 CGameLogic::NextCard(_uint8 bCard)
{
	_uint8 bCardNext = INVALID_MJ_CARD;
	_uint8 type = CardType(bCard);
	_uint8 num = CardNum(bCard);
	if (type == MJ_TYPE_WAN || type == MJ_TYPE_TIAO || type == MJ_TYPE_TONG)
	{
		if (num == 9)
		{
			num = 0;
		}
		bCardNext = (type << 4) | (num + 1);
	}
	else
	{
		switch (bCard)
		{
		case 0x31:
		case 0x32:
		case 0x33:
		case 0x41:
		case 0x42:
			bCardNext = bCard + 1;
			break;
		case 0x34:
			bCardNext = 0x41;
			break;
		case 0x43:
			bCardNext = 0x31;
			break;
		default:
			assert(false);
			break;
		}
	}
	return bCardNext;
}

_uint8 CGameLogic::NextCard2(_uint8 bCard)
{
	_uint8 _cardtype = CardType(bCard);
	if (_cardtype < MJ_TYPE_FENG)
	{
		bCard = CheckNextCard(bCard, 9);
	}
	else if (_cardtype == MJ_TYPE_FENG)
	{
		bCard = CheckNextCard(bCard, 4);
	}
	else if (_cardtype == MJ_TYPE_JIAN)
	{
		bCard = CheckNextCard(bCard, 3);
	}
	else
	{
		assert(false);
	}

	return bCard;
}

_uint8 CGameLogic::CheckNextCard(_uint8 bCard, _uint8 max_card)
{
	if (CardNum(bCard) == max_card)
	{
		bCard = (CardType(bCard) << 4) + 1;
	}
	else
	{
		++bCard;
	}
	return bCard;
}

_uint8 CGameLogic::PrevCard(_uint8 bCard)
{
	_uint8 bCardPrev = INVALID_MJ_CARD;
	_uint8 type = CardType(bCard);
	_uint8 num = CardNum(bCard);
	if (type == MJ_TYPE_WAN || type == MJ_TYPE_TIAO || type == MJ_TYPE_TONG)
	{
		if (num == 1)
		{
			num = 10;
		}
		bCardPrev = (type << 4) | (num - 1);
	}
	else
	{
		switch (bCard)
		{
		case 0x32:
		case 0x33:
		case 0x34:
		case 0x42:
		case 0x43:
			bCardPrev = bCard - 1;
			break;
		case 0x31:
			bCardPrev = 0x43;
			break;
		case 0x41:
			bCardPrev = 0x34;
			break;
		default:
			assert(false);
			break;
		}
	}
	return bCardPrev;
}
_uint8 CGameLogic::NextCard3(_uint8 bCard)
{
	_uint8 bCardNext = INVALID_MJ_CARD;
	_uint8 type = CardType(bCard);
	_uint8 num = CardNum(bCard);
	if (type < 3)
	{
		if (num == 9)
			return bCardNext;
		bCardNext = (type << 4) | (num + 1);
	}	
	return bCardNext;
}
_uint8 CGameLogic::PrevCard3(_uint8 bCard)
{	
	_uint8 bCardPrev = INVALID_MJ_CARD;
	_uint8 type = CardType(bCard);
	_uint8 num = CardNum(bCard);
	if (type < 3)
	{
		if (num == 1)
			return bCardPrev;
		bCardPrev = (type << 4) | (num - 1);
	}	
	return bCardPrev;
}