#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <vector>
#include <assert.h>
#include <algorithm>

#include "game_logic.h"
#include <stdio.h>
using namespace std;


#define _CARD_TEST

CGameLogic::CGameLogic(void)
{

}

CGameLogic::~CGameLogic(void)
{

}

void CGameLogic::AddCard(HandCards *pHc, _uint8 bCard)
{
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

//	ASSERT(2 == hcs->ChangeableCardsLen%3);
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

int CGameLogic::GetPengGang(HandCards *pHc, _uint8 bCard, MJ_opinfo& GangCardInfo, _uint8 laizi_card)
{
    GangCardInfo.Clear();
	int count = 0;
	for (int i = 0; i < pHc->ChangeableCardsLen; i++)
	{
		if (bCard == pHc->ChangeableCards[i] && bCard != laizi_card)
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

int CGameLogic::GetGangInfo(const HandCards*pHc, MJ_opinfo& ganginfo, MJ_gang &lou_gang, _uint8 laizi_card)
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
		if (bCardCount[bType][bNum] == 4 && pHc->ChangeableCards[i] != laizi_card)
		{
			ganginfo.Add(OpState(pHc->ChangeableCards[i], TYPE_ANGANG));
			iOpType |= TYPE_ANGANG;
		}
		
	}
	for (int i = 0; i < pHc->FixedCardsLen; i++)
	{
		if (pHc->FixedCards[i].state == TYPE_PENG && pHc->FixedCards[i].CardData != laizi_card  
		    && !lou_gang.Find(pHc->FixedCards[i].CardData))
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

int CGameLogic::GetGangInfo(const HandCards*pHc, MJ_opinfo& ganginfo, MJ_gang &lou_gang, 
                                MJ_BUFFER<_uint8, 14> showListenInfos[GAME_PLAYER], _uint8 ChairID)
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
	//不能弯杠其他人亮牌后胡的牌
	MJ_BUFFER<_uint8, 56> showListenInfosAll;
    for (int i = 0; i < GAME_PLAYER; ++i)
    {
        if (i != ChairID)
        {
            for (int j = 0; j < showListenInfos[i].Size(); ++j)
            {
                showListenInfosAll.Add(showListenInfos[i][j]);
            }
        }
    }
	
	for (int i = 0; i < pHc->FixedCardsLen; i++)
	{
		if (pHc->FixedCards[i].state == TYPE_PENG && !lou_gang.Find(pHc->FixedCards[i].CardData) 
		    && !showListenInfosAll.Find(pHc->FixedCards[i].CardData))
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
		set.Shai[i] = rand() % 6 + 1;
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

void CGameLogic::ExecutePeng(HandCards *pHc, _uint8 bOpCardChairID, _uint8 cbOperateCard)
{
	RemoveCard(pHc, cbOperateCard, 2);
	pHc->FixedCards[pHc->FixedCardsLen].CardData = cbOperateCard;
	pHc->FixedCards[pHc->FixedCardsLen].state = TYPE_PENG;
	pHc->FixedCards[pHc->FixedCardsLen].chairID = bOpCardChairID;
	pHc->FixedCardsLen++;
}



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


//吃牌方法
void CGameLogic::ExecuteLeftChi(HandCards *handcard, _uint8 chairID, _uint8 cbOperateCard)
{
	assert(CardType(cbOperateCard) < MJ_TYPE_FENG);
	//??????
	_uint8 cbRemoveCard[] = { cbOperateCard + 1, cbOperateCard + 2 };
	RemoveCard(handcard, (_uint8*)cbRemoveCard, 2);

	handcard->FixedCards[handcard->FixedCardsLen].c[0] = cbOperateCard;
	handcard->FixedCards[handcard->FixedCardsLen].c[1] = cbOperateCard + 1;
	handcard->FixedCards[handcard->FixedCardsLen].c[2] = cbOperateCard + 2;
	handcard->FixedCards[handcard->FixedCardsLen].state = TYPE_LEFT_CHI;
	handcard->FixedCards[handcard->FixedCardsLen].chairID = chairID;
	handcard->FixedCardsLen++;
}

void CGameLogic::ExecuteCenterChi(HandCards *handcard, _uint8 chairID, _uint8 cbOperateCard)
{
	assert(CardType(cbOperateCard) < MJ_TYPE_FENG);
	//??????
	_uint8 cbRemoveCard[] = { cbOperateCard - 1, cbOperateCard + 1 };
	RemoveCard(handcard, (_uint8*)cbRemoveCard, 2);

	handcard->FixedCards[handcard->FixedCardsLen].c[0] = cbOperateCard - 1;
	handcard->FixedCards[handcard->FixedCardsLen].c[1] = cbOperateCard;
	handcard->FixedCards[handcard->FixedCardsLen].c[2] = cbOperateCard + 1;
	handcard->FixedCards[handcard->FixedCardsLen].state = TYPE_CENTER_CHI;
	handcard->FixedCards[handcard->FixedCardsLen].chairID = chairID;
	handcard->FixedCardsLen++;
}

void CGameLogic::ExecuteRightChi(HandCards *handcard, _uint8 chairID, _uint8 cbOperateCard)
{
	assert(CardType(cbOperateCard) < MJ_TYPE_FENG);
	//??????
	_uint8 cbRemoveCard[] = { cbOperateCard - 2, cbOperateCard - 1 };
	RemoveCard(handcard, (_uint8*)cbRemoveCard, 2);

	handcard->FixedCards[handcard->FixedCardsLen].c[0] = cbOperateCard - 2;
	handcard->FixedCards[handcard->FixedCardsLen].c[1] = cbOperateCard - 1;
	handcard->FixedCards[handcard->FixedCardsLen].c[2] = cbOperateCard;
	handcard->FixedCards[handcard->FixedCardsLen].state = TYPE_RIGHT_CHI;
	handcard->FixedCards[handcard->FixedCardsLen].chairID = chairID;
	handcard->FixedCardsLen++;
}

int CGameLogic::GetChi(const HandCards *p, _uint8 cbCard, MJ_opinfo& ChiInfo)
{

	if (CardType(cbCard) >= MJ_TYPE_FENG) return TYPE_NULL;

	int cbMask = TYPE_NULL;
	bool bFind[4] = { false, false, false, false };
	for (int i = 0; i < p->ChangeableCardsLen; i++)
	{
		if (p->ChangeableCards[i] + 2 == cbCard)
		{
			bFind[0] = true;
		}
		else if (p->ChangeableCards[i] + 1 == cbCard)
		{
			bFind[1] = true;
		}
		else if (p->ChangeableCards[i] == cbCard + 1)
		{
			bFind[2] = true;
		}
		else if (p->ChangeableCards[i] == cbCard + 2)
		{
			bFind[3] = true;
		}
	}
	if (bFind[0] && bFind[1])
	{
		cbMask |= TYPE_RIGHT_CHI;
		ChiInfo.Add(OpState(cbCard, TYPE_RIGHT_CHI));
	}
	if (bFind[1] && bFind[2])
	{
		cbMask |= TYPE_CENTER_CHI;
		ChiInfo.Add(OpState(cbCard, TYPE_CENTER_CHI));
	}
	if (bFind[2] && bFind[3])
	{
		cbMask |= TYPE_LEFT_CHI;
		ChiInfo.Add(OpState(cbCard, TYPE_LEFT_CHI));
	}
	return cbMask;
}


bool CGameLogic::LaiZi_CanHu(MJ_hand_buffer stData, int LaiZiNum, bool bAllShunZi)
{
	if (stData.Size() == 0)
	{
		if (LaiZiNum >= 2)
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	MJ_hand_buffer tempData;
	tempData = stData;
	for (int i = 0; i < tempData.Size(); i++)
	{
		if (i == tempData.Size() - 1)
		{
			if (LaiZiNum > 0)
			{
				LaiZiNum = LaiZiNum - 1;				
				stData.Remove(tempData[i]);
				int nNeedMinLaiZiNum = 8;
				if (bAllShunZi)
				{
					get_need_hun_num_by_shun(stData, 0, nNeedMinLaiZiNum);
				}
				else
				{
					get_need_hun_num(stData, 0, nNeedMinLaiZiNum);
				}

				if (nNeedMinLaiZiNum <= LaiZiNum)
				{
					return true;
				}
			}
		}
		else
		{
			if (((i + 2) == tempData.Size())
				|| tempData[i] != tempData[i + 2])
			{
				if (checke_combine_dui_zi(tempData[i], tempData[i + 1]))
				{					
					stData.Remove(tempData[i]);
					stData.Remove(tempData[i + 1]);
					int nNeedMinLaiZiNum = 8;
					if (bAllShunZi)
					{
						get_need_hun_num_by_shun(stData, 0, nNeedMinLaiZiNum);
					}
					else
					{
						get_need_hun_num(stData, 0, nNeedMinLaiZiNum);
					}
					if (nNeedMinLaiZiNum <= LaiZiNum)
					{
						return true;
					}					
					stData.Add(tempData[i]);
					stData.Add(tempData[i + 1]);
					stData.SortAscend();
				}
			}
			if (LaiZiNum > 0 && tempData[i] != tempData[i + 1])
			{
				LaiZiNum = LaiZiNum - 1;				
				stData.Remove(tempData[i]);
				int nNeedMinLaiZiNum = 8;
				if (bAllShunZi)
				{
					get_need_hun_num_by_shun(stData, 0, nNeedMinLaiZiNum);
				}
				else
				{
					get_need_hun_num(stData, 0, nNeedMinLaiZiNum);
				}
				if (nNeedMinLaiZiNum <= LaiZiNum)
				{
					return true;
				}
				LaiZiNum = LaiZiNum + 1;				
				stData.Add(tempData[i]);
				stData.SortAscend();
			}
		}
	}
	return false;
}

bool CGameLogic::FormatCards_Laizi(_uint8 card[14], int nLen, int nLaiZiNum)
{
	//MjCardData PaiCard[4];
	memset(m_PaiCard, 0, sizeof(m_PaiCard));
	PyCard(card, nLen, m_PaiCard);

	int needHunNum = 0;
	int needMinHunNum = 8;
	get_need_hun_num(m_PaiCard[0], 0, needMinHunNum);
	int wan_need = needMinHunNum;
	needMinHunNum = 8;
	get_need_hun_num(m_PaiCard[1], 0, needMinHunNum);
	int tiao_need = needMinHunNum;
	needMinHunNum = 8;
	get_need_hun_num(m_PaiCard[2], 0, needMinHunNum);
	int tong_need = needMinHunNum;
	needMinHunNum = 8;
	get_need_hun_num(m_PaiCard[3], 0, needMinHunNum);
	int feng_need = needMinHunNum;

	needHunNum = tiao_need + tong_need + feng_need;
	if (needHunNum <= nLaiZiNum)
	{
		int hasNum = nLaiZiNum - needHunNum;
		bool ishu = LaiZi_CanHu(m_PaiCard[0], hasNum);
		if (ishu)
		{
			return true;
		}
	}
	needHunNum = wan_need + tong_need + feng_need;
	if (needHunNum <= nLaiZiNum)
	{
		int hasNum = nLaiZiNum - needHunNum;
		bool ishu = LaiZi_CanHu(m_PaiCard[1], hasNum);
		if (ishu)
		{
			return true;
		}
	}
	needHunNum = wan_need + tiao_need + feng_need;
	if (needHunNum <= nLaiZiNum)
	{
		int hasNum = nLaiZiNum - needHunNum;
		bool ishu = LaiZi_CanHu(m_PaiCard[2], hasNum);
		if (ishu)
		{
			return true;
		}
	}
	needHunNum = wan_need + tiao_need + tong_need;
	if (needHunNum <= nLaiZiNum)
	{
		int hasNum = nLaiZiNum - needHunNum;
		bool ishu = LaiZi_CanHu(m_PaiCard[3], hasNum);
		if (ishu)
		{
			return true;
		}
	}
	return false;
}

void CGameLogic::PyCard(const _uint8 handcard[14], int nLen, MJ_hand_buffer PaiCard[4])
{
	for (int i = 0; i < nLen; i++)
	{
		if (CardType(handcard[i]) == MJ_TYPE_WAN)
		{			
			PaiCard[MJ_TYPE_WAN].Add(handcard[i]);
		}
		else if (CardType(handcard[i]) == MJ_TYPE_TIAO)
		{			
			PaiCard[MJ_TYPE_TIAO].Add(handcard[i]);
		}
		else if (CardType(handcard[i]) == MJ_TYPE_BING)
		{			
			PaiCard[MJ_TYPE_BING].Add(handcard[i]);
		}
		else if (CardType(handcard[i]) == MJ_TYPE_FENG || CardType(handcard[i]) == MJ_TYPE_JIAN)
		{			
			PaiCard[MJ_TYPE_FENG].Add(handcard[i]);
		}
	}
}

void CGameLogic::get_need_hun_num(MJ_hand_buffer& stData, int nNeedNum, int &nNeedMinNum)
{
	if (nNeedMinNum == 0)
	{
		return;
	}
	if (nNeedNum >= nNeedMinNum)
	{
		return;
	}

	if (stData.Size() == 0)
	{
		nNeedMinNum = MIN(nNeedNum, nNeedMinNum);;
		return;
	}
	else if (stData.Size() == 1)
	{
		nNeedMinNum = MIN(nNeedNum + 2, nNeedMinNum);
		return;
	}
	else if (stData.Size() == 2)
	{
		_uint8 cbCard1 = stData[0];
		_uint8 cbCard2 = stData[1];
		if (CardType(cbCard2) == MJ_TYPE_JIAN || CardType(cbCard2) == MJ_TYPE_FENG)
		{
			if (cbCard1 == cbCard2)
			{
				nNeedMinNum = MIN(nNeedNum + 1, nNeedMinNum);
				return;
			}
			else
			{
				stData.Remove(cbCard1);
				get_need_hun_num(stData, nNeedNum + 2, nNeedMinNum);
				stData.Add(cbCard1);
				stData.SortAscend();
			}
		}
		else if ((cbCard2 - cbCard1) < 3)
		{
			nNeedMinNum = MIN(nNeedNum + 1, nNeedMinNum);
		}
		return;
	}

	_uint8 bCard1 = stData[0];
	_uint8 bCard2 = stData[1];
	_uint8 bCard3 = stData[2];
	if ((nNeedNum + 2) < nNeedMinNum)
	{
		stData.Remove(bCard1);
		get_need_hun_num(stData, nNeedNum + 2, nNeedMinNum);
		stData.Add(bCard1);
		stData.SortAscend();
		
	}
	if (nNeedNum + 1 < nNeedMinNum)
	{
		if (CardType(bCard1) == MJ_TYPE_JIAN || CardType(bCard1) == MJ_TYPE_FENG)
		{
			if (bCard1 == bCard2)
			{
				stData.Remove(bCard1);
				stData.Remove(bCard2);
				get_need_hun_num(stData, nNeedNum + 1, nNeedMinNum);
				stData.Add(bCard1);
				stData.Add(bCard2);
				stData.SortAscend();
			}

		}
		else
		{
			for (int i = 1; i < stData.Size(); i++)
			{
				if ((nNeedNum + 1) >= nNeedMinNum)
				{
					break;;
				}
				bCard2 = stData[i];
				if ((i + 1) != stData.Size())
				{
					bCard3 = stData[i + 1];
					if (bCard3 == bCard2)
					{
						continue;
					}

				}
				if ((bCard2 - bCard1) < 3)
				{
					stData.Remove(bCard1);
					stData.Remove(bCard2);
					get_need_hun_num(stData, nNeedNum + 1, nNeedMinNum);
					stData.Add(bCard1);
					stData.Add(bCard2);
					stData.SortAscend();
				}
				else
				{
					break;
				}
			}
		}
	}

	for (int i = 1; i < stData.Size(); i++)
	{
		if (nNeedNum >= nNeedMinNum)
		{
			break;
		}
		bCard2 = stData[i];
		if ((i + 2) < stData.Size())
		{
			if (stData[i + 2] == bCard2)
			{
				continue;
			}
		}
		for (int j = i + 1; j < stData.Size(); j++)
		{
			if (nNeedNum >= nNeedMinNum)
			{
				break;;
			}
			bCard3 = stData[j];

			if ((j + 1) < stData.Size())
			{
				if (bCard3 == stData[j + 1])
				{
					continue;
				}
			}
			if (checke_combine(bCard1, bCard2, bCard3))
			{			
				stData.Remove(bCard1);
				stData.Remove(bCard2);
				stData.Remove(bCard3);
				get_need_hun_num(stData, nNeedNum, nNeedMinNum);
				stData.Add(bCard1);
				stData.Add(bCard2);
				stData.Add(bCard3);
				stData.SortAscend();
			}
		}
	}
}

void CGameLogic::get_need_hun_num_by_shun(MJ_hand_buffer stData, int nNeedNum, int &nNeedMinNum)
{
	if (nNeedMinNum == 0)
	{
		return;
	}
	if (nNeedNum >= nNeedMinNum)
	{
		return;
	}

	if (stData.Size() == 0)
	{
		nNeedMinNum = MIN(nNeedNum, nNeedMinNum);;
		return;
	}
	else if (stData.Size() == 1)
	{
		nNeedMinNum = MIN(nNeedNum + 2, nNeedMinNum);
		return;
	}
	else if (stData.Size() == 2)
	{
		_uint8 cbCard1 = stData[0];
		_uint8 cbCard2 = stData[1];
		if (CardType(cbCard2) == MJ_TYPE_JIAN || CardType(cbCard2) == MJ_TYPE_FENG)
		{
			if (cbCard1 == cbCard2)
			{
				nNeedMinNum = MIN(nNeedNum + 1, nNeedMinNum);
				return;
			}

		}
		else if ((cbCard2 - cbCard1) < 3)
		{
			nNeedMinNum = MIN(nNeedNum + 1, nNeedMinNum);
		}
		else
		{
			nNeedMinNum = 8;
		}
		return;
	}

	_uint8 bCard1 = stData[0];
	_uint8 bCard2 = stData[1];
	_uint8 bCard3 = stData[2];
	if ((nNeedNum + 2) < nNeedMinNum)
	{		
		stData.Remove(bCard1);
		get_need_hun_num(stData, nNeedNum + 2, nNeedMinNum);		
		stData.Add(bCard1);
		stData.SortAscend();
	}
	if (nNeedNum + 1 < nNeedMinNum)
	{
		if (CardType(bCard1) == MJ_TYPE_JIAN || CardType(bCard1) == MJ_TYPE_FENG)
		{
			if (bCard1 == bCard2)
			{				
				stData.Remove(bCard1);
				stData.Remove(bCard2);
				get_need_hun_num(stData, nNeedNum + 1, nNeedMinNum);				
				stData.Add(bCard1);
				stData.Add(bCard2);
				stData.SortAscend();
			}
		}
		else
		{
			for (int i = 1; i < stData.Size(); i++)
			{
				if ((nNeedNum + 1) >= nNeedMinNum)
				{
					break;;
				}
				bCard2 = stData[i];
				if ((i + 1) != stData.Size())
				{
					bCard3 = stData[i + 1];
					if (bCard3 == bCard2)
					{
						continue;
					}
				}
				if ((bCard2 - bCard1) < 3)
				{					
					stData.Remove(bCard1);
					stData.Remove(bCard2);
					get_need_hun_num(stData, nNeedNum + 1, nNeedMinNum);					
					stData.Add(bCard1);
					stData.Add(bCard2);
					stData.SortAscend();
				}
				else
				{
					break;
				}
			}
		}
	}

	for (int i = 1; i < stData.Size(); i++)
	{
		if (nNeedNum >= nNeedMinNum)
		{
			break;
		}
		bCard2 = stData[i];
		if ((i + 2) < stData.Size())
		{
			if (stData[i + 2] == bCard2)
			{
				continue;
			}
		}
		for (int j = i + 1; j < stData.Size(); j++)
		{
			if (nNeedNum >= nNeedMinNum)
			{
				break;;
			}
			bCard3 = stData[j];

			if ((j + 1) < stData.Size())
			{
				if (bCard3 == stData[j + 1])
				{
					continue;
				}
			}
			if (checke_combine_shun(bCard1, bCard2, bCard3))
			{				
				stData.Remove(bCard1);
				stData.Remove(bCard2);
				stData.Remove(bCard3);
				get_need_hun_num(stData, nNeedNum, nNeedMinNum);				
				stData.Add(bCard1);
				stData.Add(bCard2);
				stData.Add(bCard3);
				stData.SortAscend();
			}
		}
	}
}

bool CGameLogic::checke_combine(_uint8 cbCard1, _uint8 cbCard2, _uint8 cbCard3)
{
	bool bComBine = false;
	if ((cbCard1 == cbCard2) && (cbCard2 == cbCard3))
	{
		bComBine = true;
	}
	if (((cbCard1 + 1) == cbCard2) && ((cbCard2 + 1) == cbCard3))
	{
		if (CardType(cbCard1) != MJ_TYPE_JIAN && CardType(cbCard1) != MJ_TYPE_FENG)
		{
			bComBine = true;
		}
	}

	return bComBine;
}

bool CGameLogic::checke_combine_shun(_uint8 cbCard1, _uint8 cbCard2, _uint8 cbCard3)
{
	bool bComBine = false;
	if (((cbCard1 + 1) == cbCard2) && ((cbCard2 + 1) == cbCard3))
	{
		if (CardType(cbCard1) < MJ_TYPE_FENG)
		{
			bComBine = true;
		}
	}

	return bComBine;
}

bool CGameLogic::checke_combine_dui_zi(_uint8 cbCard1, _uint8 cbCard2)
{
	if (cbCard1 == cbCard2)
	{
		return true;
	}
	return false;
}

void CGameLogic::GetCardCount(const HandCards* pHc, _uint8 CardDataNum[MJ_TYPE_COMMON][10])
{
	memset(CardDataNum, 0, sizeof(_uint8) * MJ_TYPE_COMMON * 10);
	for (int i = 0; i < pHc->ChangeableCardsLen; i++)
	{
		int nCardType = CardType(pHc->ChangeableCards[i]);
		if (nCardType < MJ_TYPE_COMMON)
		{
			int nCardNum = CardNum(pHc->ChangeableCards[i]);
			if (nCardNum > 0 && nCardNum < 10)
			{
				CardDataNum[nCardType][nCardNum]++;
			}
		}
	}
}

void CGameLogic::GetCardCountExt(const HandCards* pHc, _uint8 CardDataNum[MJ_TYPE_COMMON][10])
{	
	for (int i = 0; i < pHc->FixedCardsLen; i++)
	{
		if (pHc->FixedCards[i].state & TYPE_CHI)
		{
			CardDataNum[CardType(pHc->FixedCards[i].c[0])][CardNum(pHc->FixedCards[i].c[0])]++;
			CardDataNum[CardType(pHc->FixedCards[i].c[1])][CardNum(pHc->FixedCards[i].c[1])]++;
			CardDataNum[CardType(pHc->FixedCards[i].c[2])][CardNum(pHc->FixedCards[i].c[2])]++;
		}
		else if (pHc->FixedCards[i].state & TYPE_PENG)
		{
			CardDataNum[CardType(pHc->FixedCards[i].c[0])][CardNum(pHc->FixedCards[i].c[0])] += 3;
		}
		else if (pHc->FixedCards[i].state & TYPE_GANG)
		{
			CardDataNum[CardType(pHc->FixedCards[i].c[0])][CardNum(pHc->FixedCards[i].c[0])] += 4;
		}
	}
}

int CGameLogic::GetCardCountX(const HandCards* pHc, _uint8 bCard)
{
	int iCounts = 0;
	for (int i = 0; i < pHc->ChangeableCardsLen; i++)
	{
		
		if (pHc->ChangeableCards[i] == bCard)
		{
			iCounts++;
		}
	}
	for (int i = 0; i < pHc->FixedCardsLen; i++)
	{
		if (pHc->FixedCards[i].state & TYPE_CHI)
		{
			if (pHc->FixedCards[i].c[0] == bCard ||
				pHc->FixedCards[i].c[1] == bCard ||
				pHc->FixedCards[i].c[2] == bCard)
			{
				iCounts++;
			}
		}
		else if (pHc->FixedCards[i].state & TYPE_PENG)
		{
			if (pHc->FixedCards[i].CardData == bCard)
			{
				iCounts += 3;
			}
		}
		else if (pHc->FixedCards[i].state & TYPE_GANG)
		{
			if (pHc->FixedCards[i].CardData == bCard)
			{
				iCounts += 4;
			}
		}
	}
	return iCounts;
}

int CGameLogic::GetFixedCardCount(const HandCards* pHc, _uint8 bCard)
{
	int iCounts = 0;
	for (int i = 0; i < pHc->FixedCardsLen; i++)
	{
		if (pHc->FixedCards[i].state & TYPE_CHI)
		{
			if (pHc->FixedCards[i].c[0] == bCard ||
				pHc->FixedCards[i].c[1] == bCard ||
				pHc->FixedCards[i].c[2] == bCard)
			{
				iCounts++;
			}
		}
		else if (pHc->FixedCards[i].state & TYPE_PENG)
		{
			if (pHc->FixedCards[i].CardData == bCard)
			{
				iCounts += 3;
			}
		}
		else if (pHc->FixedCards[i].state & TYPE_GANG)
		{
			if (pHc->FixedCards[i].CardData == bCard)
			{
				iCounts += 4;
			}
		}
	}
	return iCounts;
}

bool CGameLogic::FormatCards_Laizi_3_Group(const _uint8 card[14], int nLen, int nLaiZiNum)
{
	//MjCardData PaiCard[4];
	memset(m_PaiCard, 0, sizeof(m_PaiCard));
	PyCard(card, nLen, m_PaiCard);

	int needMinHunNum = 8;
	get_need_hun_num(m_PaiCard[0], 0, needMinHunNum);
	int wan_need = needMinHunNum;
	needMinHunNum = 8;
	get_need_hun_num(m_PaiCard[1], 0, needMinHunNum);
	int tiao_need = needMinHunNum;
	needMinHunNum = 8;
	get_need_hun_num(m_PaiCard[2], 0, needMinHunNum);
	int tong_need = needMinHunNum;
	needMinHunNum = 8;
	get_need_hun_num(m_PaiCard[3], 0, needMinHunNum);
	int feng_need = needMinHunNum;

	if ((wan_need + tiao_need + tong_need + feng_need) <= nLaiZiNum)
	{
		return true;
	}
	return false;
}
//---------------------------------------胡牌判断的第二版方法---------------------------------
bool CGameLogic::FormatCards(HandCards *hcs, MJ_win_pattern& pattern)
{
	return FormatCards(vector<_uint8>(&hcs->ChangeableCards[0], &hcs->ChangeableCards[hcs->ChangeableCardsLen]), pattern);
}

bool CGameLogic::FormatCards(std::vector<_uint8> vec, MJ_win_pattern& pattern)
{
	sort(vec.begin(), vec.end());
	
	vector<_uint8> vec_jiang;
	GetJiang(vec, vec_jiang);
	if (vec_jiang.empty())
	{
		return false;
	}

	vector<_uint8> vec_kan;
	vector<_uint8>::iterator it = vec_jiang.begin();
	while (it != vec_jiang.end())
	{
		vector<_uint8> tmp = vec;
		EraseCard(tmp, *it, 2);
		if (CheckSubVector(tmp , vec_kan))
		{
			GetKan(vec_kan, pattern);
			pattern.jiang = *it;
			return true;
		}
		++it;
	}
	return false;
}

bool CGameLogic::FormatCards_LaiZi(HandCards *hcs, MJ_win_pattern& pattern, int lai_sum)
{
	if ( (hcs->ChangeableCardsLen == 0 ) && lai_sum )
	{
		return true;
	}
	return FormatCards_LaiZi(vector<_uint8>(&hcs->ChangeableCards[0], &hcs->ChangeableCards[hcs->ChangeableCardsLen]) , pattern , lai_sum);
}

bool CGameLogic::FormatCards_LaiZi(std::vector<_uint8> vec, MJ_win_pattern& pattern, int lai_sum)
{
	sort(vec.begin(), vec.end());


	vector<_uint8> kan;
	if ( CheckJiang_LaiZi(vec , pattern, lai_sum) )
	{
		return true;
	}

	
	if ( CheckShunZi_LaiZi(vec , pattern, lai_sum) )
	{
		return true;
	}
	 
	return false;
}

bool CGameLogic::CheckJiang_LaiZi(std::vector<_uint8> vec, MJ_win_pattern& pattern, int lai_sum_remain)
{
	if ( lai_sum_remain < 1 )
	{
		return false; 
	}
	vector<_uint8> kan;
	_uint8 card = INVALID_MJ_CARD;
	for (vector<_uint8>::iterator it = vec.begin() ; it != vec.end() ; ++it )
	{
		if (card != *it)
		{
			card = *it;
			vector<_uint8> tmp = vec;			
			EraseCard(tmp, *it, 1);
			if (CheckSubVector_LaiZi(tmp, kan, lai_sum_remain - 1))
			{
				pattern.jiang = card;
				GetKan(kan, pattern);
				return true;
			}
		}
	}	
	
	return false;
}

bool CGameLogic::CheckShunZi_LaiZi(std::vector<_uint8> vec, MJ_win_pattern& pattern, int lai_sum_remain)
{
	vector<_uint8> kan;
	if ( lai_sum_remain < 0 )
	{
		return false;
	}

	vector<_uint8> vec_jiang;
	GetJiang(vec, vec_jiang);
	
	for ( vector<_uint8>::iterator it = vec_jiang.begin() ;  it != vec_jiang.end() ; it++ )
	{
		vector<_uint8> tmp = vec;
		EraseCard(tmp, *it, 2);
		if (CheckSubVector_LaiZi(tmp , kan , lai_sum_remain))
		{
			pattern.jiang = *it;
			GetKan(kan, pattern);
			return true;
		}
	}
	
	return false;
}

bool CGameLogic::CheckSubVector_LaiZi(std::vector<_uint8> vec, std::vector<_uint8> &kan , int lai_sum_remain)
{
	if ( vec.empty() )
	{
		return true;
	}

	if ( lai_sum_remain < 0  )
	{
		return false;
	}


	if ( lai_sum_remain == 0  )
	{
		return CheckSubVector(vec, kan);
	}
	
	_uint8 first_card = vec[0];
	_uint8 need_lai_sum = 0; 

	if ((CardType(first_card) == MJ_TYPE_WAN) || (CardType(first_card) == MJ_TYPE_TIAO) || (CardType(first_card) == MJ_TYPE_TONG))
	{
		need_lai_sum =  GetShunZi_LaiZi(vec, first_card);

		vector<_uint8> tmp = vec;
		
		EraseCard(tmp, first_card);
		EraseCard(tmp, first_card + 1);
		EraseCard(tmp, first_card + 2);
		if (CheckSubVector_LaiZi(tmp, kan, lai_sum_remain - need_lai_sum))
		{
			if (CardNum(first_card) == 8)
			{
				kan.push_back(first_card - 1);
				kan.push_back(first_card);
				kan.push_back(first_card + 1);
			}
			else if (CardNum(first_card) == 9)
			{
				kan.push_back(first_card - 2);
				kan.push_back(first_card - 1);
				kan.push_back(first_card);
			}
			else
			{
				kan.push_back(first_card);
				kan.push_back(first_card + 1);
				kan.push_back(first_card + 2);
			}
			return true;
		}
	}

	int cnt = count(vec.begin(), vec.end(), first_card);
	if ( cnt > 3 )
	{
		EraseCard(vec, first_card, 3);
		need_lai_sum = 0;
	}
	else
	{
		EraseCard(vec, first_card, cnt);
		need_lai_sum = 3 - cnt;
	}
	
	if ( CheckSubVector_LaiZi(vec , kan , lai_sum_remain - need_lai_sum  ))
	{
		kan.push_back(first_card);
		kan.push_back(first_card);
		kan.push_back(first_card);
		return true;
	}
	
	return false;
}

_uint8 CGameLogic::GetShunZi_LaiZi(const std::vector<_uint8> &vec, _uint8 card )
{
	if ( vec.empty() )
	{
		return 0;
	}

	if ( card == INVALID_MJ_CARD )
	{
		card = vec[0];
	}
	_uint8 single = 0;
	if ( find(vec.begin() , vec.end() , card + 1) == vec.end() )
	{
		single++;
	}
	if ( find(vec.begin() , vec.end() , card + 2 ) == vec.end() )
	{
		single++;
	}
	return single;
}

void CGameLogic::GetKan(const std::vector<_uint8> &vec_kan, MJ_win_pattern& pattern)
{

	memset(pattern.kan, 0, sizeof(pattern.kan));

	if ( (vec_kan.size() % 3 ) )
	{
		
		return ;
	}

	if ( vec_kan.size()/3  > HAND_CARD_KAN_MAX )
	{
	
		return;
	}	
	for ( size_t i = 0,j = 0  ; i < vec_kan.size() ; ++i  )
	{
		if (i != 0 && i % 3 == 0)
		{
			++j;
		}
		pattern.kan[j][i % 3] = vec_kan[i];

	}
}

bool CGameLogic::CheckSubVector(std::vector<_uint8> vec, std::vector<_uint8> &kan)
{
	if ( vec.empty() )
	{
		return true;
	}
	if ( vec.size() < 3 )
	{
		return false;
	}

	_uint8 first_card = vec[0];
	int sum = count(vec.begin(), vec.end(), first_card);

	if (  sum == 1 || sum == 2 )
	{
		if ( (CardType(first_card) == MJ_TYPE_FENG) || ( CardType(first_card) == MJ_TYPE_JIAN )   )
		{
			return false;
		}
		if ( CheckShunZi(vec , first_card , sum) )
		{
			EraseCard(vec, first_card, sum);
			EraseCard(vec, first_card + 1, sum);
			EraseCard(vec, first_card + 2, sum);
			if ( CheckSubVector(vec , kan) )
			{
				for ( int i = 0 ; i < sum ; ++i  )
				{
					kan.push_back(first_card);
					kan.push_back(first_card + 1);
					kan.push_back(first_card + 2);
				}
				return true;
			}
		}
	}
	else if ( sum == 3 )
	{
		vector<_uint8> tmp = vec;
		EraseCard(tmp, first_card, 3);
		if ( CheckSubVector(tmp, kan))
		{
			for (int i = 0; i < sum; ++i)
			{
				kan.push_back(first_card);
			}
			return true;
		}
		if ((CardType(first_card) == MJ_TYPE_FENG) || (CardType(first_card) == MJ_TYPE_JIAN))
		{
			return false;
		}
		if (  CheckShunZi(vec , first_card , 3))
		{
			EraseCard(vec, first_card, sum);
			EraseCard(vec, first_card + 1, sum);
			EraseCard(vec, first_card + 2, sum);
			if (CheckSubVector(vec, kan))
			{
				for (int i = 0; i < sum; ++i)
				{
					kan.push_back(first_card);
					kan.push_back(first_card + 1);
					kan.push_back(first_card + 2);
				}
				return true;
			}

		}
		return false;
	}
	else if ( sum == 4 )
	{
		if ((CardType(first_card) == MJ_TYPE_FENG) || (CardType(first_card) == MJ_TYPE_JIAN))
		{
			return false;
		}
		vector<_uint8> tmp = vec;
		EraseCard(tmp, first_card, 3);
		if ( CheckShunZi(tmp , first_card ) )
		{
			EraseCard(tmp, first_card);
			EraseCard(tmp, first_card + 1);
			EraseCard(tmp, first_card + 2);
		}
		if ( CheckSubVector(tmp , kan ) )
		{
			kan.push_back(first_card);
			kan.push_back(first_card);
			kan.push_back(first_card);

			kan.push_back(first_card);
			kan.push_back(first_card + 1);
			kan.push_back(first_card + 2);
			return true;
		}
		if ( CheckShunZi(vec , first_card , sum) )
		{
			EraseCard(vec, first_card, sum);
			EraseCard(vec, first_card + 1, sum);
			EraseCard(vec, first_card + 2, sum);
			if (CheckSubVector(vec, kan))
			{
				for (int i = 0; i < sum; ++i)
				{
					kan.push_back(first_card);
					kan.push_back(first_card + 1);
					kan.push_back(first_card + 2);
				}
				return true;
			}
		}
	}
	return false;
}

void CGameLogic::GetJiang(std::vector<_uint8> &vec, std::vector<_uint8> &out_jiang)
{
	map<_uint8, int> tmp;
	for (vector<_uint8>::iterator it = vec.begin() ; it != vec.end() ; ++it)
	{
		if ( tmp.find(*it) == tmp.end() )
		{
			tmp[*it] = 0;
		}
		else
		{
			tmp[*it]++;
		}
	}

	map<_uint8, int>::iterator it = tmp.begin();
	while ( it != tmp.end() )
	{
		if ( it->second >= 1 )
		{
			out_jiang.push_back(it->first);
		}
		++it;
	}
}

bool CGameLogic::EraseCard(std::vector<_uint8> &vec , _uint8 card, _uint8 sum)
{
	if ( count(vec.begin() , vec.end() , card) < sum )
	{
		return false;
	}

	for ( int i = 0 ; i < sum ; ++i  )
	{
		vector<_uint8>::iterator it = find(vec.begin(), vec.end(), card);
		vec.erase(it);
	}
	return true;

}

bool CGameLogic::CheckShunZi(std::vector<_uint8> &vec, _uint8 card, _uint8 sum)
{
	if ( vec.size() < sum*3 )
	{
		return false;
	}
	if ( count(vec.begin() , vec.end() , card) < sum )
	{
		return false;
	}
	if (count(vec.begin(), vec.end(), card+1) < sum)
	{
		return false;
	}
	if (count(vec.begin(), vec.end(), card + 2) < sum)
	{
		return false;
	}
	return true;
}

int CGameLogic::GetNoLaiZiHandCard(const HandCards &card_in, HandCards &card_out, _uint8 m_laizi)
{
	int _sum = 0;
	for ( _uint8 i = 0 , j = 0 ; i < card_in.ChangeableCardsLen ; ++i  )
	{
		if ( card_in.ChangeableCards[i] != m_laizi )
		{
			++_sum;
			card_out.ChangeableCards[j++] = card_in.ChangeableCards[i];
		}
	}
	card_out.ChangeableCardsLen = _sum;

	card_out.FixedCardsLen = card_in.FixedCardsLen;
	memcpy(card_out.FixedCards, card_in.FixedCards, sizeof(card_in.FixedCards));
	return card_in.ChangeableCardsLen - card_out.ChangeableCardsLen;
}

int CGameLogic::GetNoLaiZiHandCard(const HandCards &card_in, HandCards &card_out, const std::vector<_uint8>& vec_laizi)
{
	HandCards tmp = card_in;
	vector<_uint8>::const_iterator it = vec_laizi.begin();
	int iCounts = 0;
	while ( it != vec_laizi.end() )
	{
		iCounts += GetNoLaiZiHandCard(tmp, card_out , *it);
		tmp = card_out;
		it++;
	}
	return iCounts;
}


int CGameLogic::GetLaiZiSum(const HandCards &phc, _uint8 m_laizi)
{
	return (int)count(&phc.ChangeableCards[0], &phc.ChangeableCards[0] + phc.ChangeableCardsLen, m_laizi);
}

int CGameLogic::GetLaiZiSum(const HandCards &phc, const std::vector<_uint8>& vec_laizi)
{
	int single = 0; 
	for ( int i = 0 ; i < phc.ChangeableCardsLen ; ++i  )
	{
		vector<_uint8>::const_iterator it = find(vec_laizi.begin(), vec_laizi.end(), phc.ChangeableCards[i]);
		if ( it != vec_laizi.end() )
		{
			++single;
		}
	}
	return single;
}


bool CGameLogic::CheckSubVectorPeng(HandCards &card_in, int lai_sum_remain)
{
	vector<_uint8> vec(&card_in.ChangeableCards[0], &card_in.ChangeableCards[card_in.ChangeableCardsLen]);
	return CheckSubVectorPeng(vec, lai_sum_remain);
}

bool CGameLogic::CheckSubVectorPeng(std::vector<_uint8> &vec, int lai_sum_remain)
{
	if ( vec.empty() )
	{
		return true;
	}

	if ( lai_sum_remain < 0  )
	{
		return false;
	}
	_uint8 first_card = vec[0];
	_uint8 need_lai_sum = 0;

	int cnt = count(vec.begin(), vec.end(), first_card);
	if ( cnt > 3 )
	{
		EraseCard(vec, first_card, 3);
		need_lai_sum = 0;
	}
	else
	{
		EraseCard(vec, first_card, cnt);
		need_lai_sum = 3 - cnt;
	}

	return CheckSubVectorPeng(vec, lai_sum_remain - need_lai_sum);
}

bool CGameLogic::CheckQing19(HandCards &no_laizi_handcard)
{
	_uint8 _card_type = INVALID_MJ_CARD;
	for ( int i = 0 ; i < no_laizi_handcard.FixedCardsLen ; ++i  )
	{
		if (no_laizi_handcard.FixedCards[i].state & TYPE_CHI)
		{
			return false;
		}
		if (CardType(no_laizi_handcard.FixedCards[i].CardData) == MJ_TYPE_FENG || CardType(no_laizi_handcard.FixedCards[i].CardData) == MJ_TYPE_JIAN)
		{
			continue;
		}
		if (!(CardNum(no_laizi_handcard.FixedCards[i].CardData) == 1 || CardNum(no_laizi_handcard.FixedCards[i].CardData) == 9))
		{
			return false;
		}
		else
		{
			_uint8 types = CardType(no_laizi_handcard.FixedCards[i].CardData);
			if ( _card_type != INVALID_MJ_CARD )
			{
				_card_type = types;
			}
			else if ( _card_type != types )
			{
				return false;
			}			
		}
	}
	for ( int i = 0 ; i < no_laizi_handcard.ChangeableCardsLen ; ++i  )
	{

		_uint8 types = CardType(no_laizi_handcard.ChangeableCards[i]);
		if ( types == MJ_TYPE_FENG || types == MJ_TYPE_JIAN )
		{
			continue;
		}
		if ( ! ( CardNum(no_laizi_handcard.ChangeableCards[i])==1 || CardNum(no_laizi_handcard.ChangeableCards[i]) == 9  )  )
		{
			return false;
		}
		else
		{			
			if ( _card_type == INVALID_MJ_CARD )
			{
				_card_type = types;
			}
			else if ( _card_type != types )
			{
				return false;
			}
		}
	}
	return true;
}

bool CGameLogic::Check7Dui(HandCards &no_laizi_handcard, int lai_sum)
{
	if ( no_laizi_handcard.FixedCardsLen )
	{
		return false;
	}
	if ( no_laizi_handcard.ChangeableCardsLen == 0 )
	{
		return true;
	}
	return CheckSubVectorDui(vector<_uint8>(&no_laizi_handcard.ChangeableCards[0], &no_laizi_handcard.ChangeableCards[0] + no_laizi_handcard.ChangeableCardsLen), lai_sum);
}


bool CGameLogic::CheckSubVectorDui(std::vector<_uint8> vec, int lai_sum)
{
	sort(vec.begin(), vec.end());

	while (!vec.empty())
	{
		if (lai_sum < 0)
		{
			return false;
		}
		_uint8 first_card = vec[0];
		int sum = count(vec.begin(), vec.end(), first_card);

		if (sum % 2)
		{
			--lai_sum;
		}
		EraseCard(vec, first_card, sum);
	}
	return true;
}