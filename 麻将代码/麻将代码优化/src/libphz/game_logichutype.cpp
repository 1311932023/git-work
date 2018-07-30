#include "game_logic.h"



bool CGameLogic::IsQingYiSe(const HandCards* p)
{
	if(p == NULL) return false;

	_uint8 ct;
	if (p->ChangeableCardsLen == 0)
	{
		ct = CardType(p->FixedCards[0].CardData);
	}
	else
	{
		ct = CardType(p->ChangeableCards[0]);
	}
	if (ct == MJ_TYPE_FENG || ct == MJ_TYPE_JIAN)
		return false;

	for (int i=0; i<p->ChangeableCardsLen; i++)
		if (ct != CardType(p->ChangeableCards[i]))
			return false;

	if (p->FixedCards->state != TYPE_NULL)
	{
		for (int i=0; i<p->FixedCardsLen; i++)
		{
			if (ct != CardType(p->FixedCards[i].CardData))
				return false;
		}
	}

	return true;
}
bool CGameLogic::IsHunYiSe(const HandCards* p)
{
	if (p == NULL) return false;
	_uint8 CardDataNum[MJ_TYPE_COMMON] = { 0, 0, 0, 0, 0 };	

	for (int i = 0; i < p->ChangeableCardsLen; i++)
	{		
		CardDataNum[CardType(p->ChangeableCards[i])]++;
	}
	
	for (int i = 0; i < p->FixedCardsLen; i++)
	{
		CardDataNum[CardType(p->FixedCards[i].CardData)]++;
	}
	
	if (CardDataNum[MJ_TYPE_FENG] + CardDataNum[MJ_TYPE_JIAN] == 0)
		return false;

	int iCount = 0;
	for (int i = 0; i < MJ_TYPE_FENG; i++)
	{
		if (CardDataNum[i] != 0)
		{
			iCount++;
		}		
	}
	
	return (iCount == 1);	
}
bool CGameLogic::IsZiYiSe(const HandCards* p)
{
	if (p == NULL)
		return false;
	
	for (int i = 0; i < p->ChangeableCardsLen; i++)
	{
		if (!(CardType(p->ChangeableCards[i]) == MJ_TYPE_FENG || CardType(p->ChangeableCards[i]) == MJ_TYPE_JIAN))
			return false;		
	}
		

	for (int i = 0; i < p->FixedCardsLen; i++)
	{
		if (!(CardType(p->FixedCards[i].CardData) == MJ_TYPE_FENG || CardType(p->FixedCards[i].CardData) == MJ_TYPE_JIAN))
			return false;
	}
	return true;
}
bool CGameLogic::IsShiBaLuoHan(const HandCards* p)
{
	if (p->FixedCardsLen != HAND_CARD_KAN_MAX)
		return false;
	for (int i = 0; i < p->FixedCardsLen; i++)
	{
		if (!(p->FixedCards[i].state & TYPE_GANG))
			return false;
	}
	return true;
}
int CGameLogic::IsDaXiaoSanYuan(const HandCards* p, const _uint8 CardDataNum[MJ_TYPE_COMMON][10])
{	
	static _uint8 c[3] = { MJ_HONG_ZHONG, MJ_FA_CAI, MJ_BAI_BAN };
	int iCount = 0;
	for (int i = 0; i < (int)sizeof(c); i++)
	{
		if (CardDataNum[CardType(c[i])][CardNum(c[i])] >= 3)
		{
			iCount += 2;
		}
		else if (CardDataNum[CardType(c[1])][CardNum(c[i])] == 2)
		{
			iCount += 1;
		}
	}	
	return iCount == 6 ? MJ_FAN_TYPE_DASANYUAN : iCount == 5 ? MJ_FAN_TYPE_XIAOSANYUAN : MJ_FAN_TYPE_NULL;
}
int CGameLogic::IsDaXiaoSiXi(const HandCards* p, const _uint8 CardDataNum[MJ_TYPE_COMMON][10])
{	
	static _uint8 c[4] = { 0x31, 0x32, 0x33, 0x34 };
	int iCount = 0;
	for (int i = 0; i < (int)sizeof(c); i++)
	{
		if (CardDataNum[CardType(c[i])][CardNum(c[i])] >= 3)
		{
			iCount += 2;
		}
		else if (CardDataNum[CardType(c[1])][CardNum(c[i])] == 2)
		{
			iCount += 1;
		}
	}
	return iCount == 8 ? MJ_FAN_TYPE_DASIXI : iCount == 7 ? MJ_FAN_TYPE_XIAOSIXI : MJ_FAN_TYPE_NULL;
}
int CGameLogic::L_IsDaXiaoSanYuan(const HandCards* p, const _uint8 CardDataNum[MJ_TYPE_COMMON][10], int iLaiziNum)
{
	static _uint8 c[3] = { MJ_HONG_ZHONG, MJ_FA_CAI, MJ_BAI_BAN };
	if (iLaiziNum == 0)
		return IsDaXiaoSanYuan(p, CardDataNum);

	int iCounts = 0;
	for (int i = 0; i < (int)sizeof(c); i++)
	{
		switch (CardDataNum[CardType(c[i])][CardNum(c[i])])
		{
		case 0:
			iCounts += 3; break;
		case 1:
			iCounts += 2; break;
		case 2:
			iCounts += 1; break;			
		}
	}
	if (iCounts > iLaiziNum)
		return MJ_FAN_TYPE_NULL;

	HandCards tmp;
	for (int i = 0; i < p->ChangeableCardsLen; i++)
	{
		if (p->ChangeableCards[i] == c[0] || p->ChangeableCards[i] == c[1] || p->ChangeableCards[i] == c[2])
			continue;
		AddCard(&tmp, p->ChangeableCards[i]);
	}

	if (FormatCards_Laizi(tmp.ChangeableCards, tmp.ChangeableCardsLen, iLaiziNum - iCounts))
	{
		return MJ_FAN_TYPE_DASANYUAN;
	}
	return MJ_FAN_TYPE_XIAOSANYUAN;
}
int CGameLogic::L_IsDaXiaoSiXi(const HandCards* p, const _uint8 CardDataNum[MJ_TYPE_COMMON][10], int iLaiziNum)
{
	static _uint8 c[4] = { 0x31, 0x32, 0x33, 0x34 };
	if (iLaiziNum == 0)
		return IsDaXiaoSiXi(p, CardDataNum);

	int iCounts = 0;
	for (int i = 0; i < (int)sizeof(c); i++)
	{
		switch (CardDataNum[CardType(c[i])][CardNum(c[i])])
		{
		case 0:			
			iCounts += 3; break;
		case 1:
			iCounts += 2; break;
		case 2:
			iCounts += 1; break;
		}
	}
	if (iCounts > iLaiziNum)
		return MJ_FAN_TYPE_NULL;
	HandCards tmp;
	for (int i = 0; i < p->ChangeableCardsLen; i++)
	{
		if (p->ChangeableCards[i] == c[0] || p->ChangeableCards[i] == c[1] || p->ChangeableCards[i] == c[2] || p->ChangeableCards[i] == c[3])
			continue;
		AddCard(&tmp, p->ChangeableCards[i]);
	}
	if (FormatCards_Laizi(tmp.ChangeableCards, tmp.ChangeableCardsLen, iLaiziNum - iCounts))
	{
		return MJ_FAN_TYPE_DASIXI;
	}
	return MJ_FAN_TYPE_XIAOSIXI;
}
bool CGameLogic::IsDai19(HandCards *p, _uint8 buf[4][4], int length, _uint8 Jiang)
{
	if(p == NULL) return false;
	for (int i=0; i<p->FixedCardsLen; i++)
	{
		if (CardNum(p->FixedCards[i].CardData)!=1 && CardNum(p->FixedCards[i].CardData)!=9)
		{
			return false;
		}
	}
	if (CardNum(Jiang)!=1 && CardNum(Jiang)!=9)
	{
        return false;
	}
	bool bFind = false;
	for (int i=0; i<length; i++)
	{
		if (CardNum(buf[i][0])==CardNum(buf[i][2]))	//	如果组合是刻字
		{   
			if ((length-i) >= 3)
			{
					if (buf[i+2][0]-buf[i+1][0]==1 && buf[i+1][0]-buf[i][0]==1)
					{
                        if (CardNum(buf[i][0])==1 || CardNum(buf[i+2][0])==9)
						{
							bFind = true;
						}
					}	
					if (i == 0 && buf[3][0]-buf[i+2][0]==1 && buf[i+2][0]-buf[i][0]==1)
					{
						if (CardNum(buf[i][0])==1 || CardNum(buf[3][0])==9)
						{
							bFind = true;
						}
					}
					if ((i == 0) && (buf[3][0]-buf[i+1][0]==1 && buf[i+1][0]-buf[i][0]==1))
					{
						if (CardNum(buf[i][0])==1 || CardNum(buf[3][0])==9)
						{
							bFind = true;
						}	
					}
					
			}
			else if (CardNum(buf[i][0])!=1 && CardNum(buf[i][0])!=9 && !bFind)
			{
                return false;
			}		
		}
		else
		{
			if (CardNum(buf[i][0])!=1 && CardNum(buf[i][2])!=9)
			{
                return false;
			}
				
		}
	}
	return true;
}
bool CGameLogic::IsQuan19(const HandCards& p, const _uint8 buf[4][4], _uint8 Jiang)
{
	for (int i = 0; i < p.ChangeableCardsLen; i++)
	{
		if (!(CardNum(p.ChangeableCards[i]) == 1 || CardNum(p.ChangeableCards[i]) == 9))
		{
			return false;
		}
	}
	for (int i = 0; i < p.FixedCardsLen; i++)
	{
		if (p.FixedCards[i].state & TYPE_CHI)
		{
			return false;
		}
		if (!(CardNum(p.FixedCards[i].CardData) == 1 || CardNum(p.FixedCards[i].CardData) == 9))
		{
			return false;
		}		
	}
	return true;
}
bool CGameLogic::IsQing19(const HandCards* p, const _uint8 buf[4][4], _uint8 Jiang)
{//不判断全风
	static _uint8 c[MJ_TYPE_COMMON][10] = { { -1, 0, -1, -1, -1, -1, -1, -1, -1, 1, }, { -1, 2, -1, -1, -1, -1, -1, -1, -1, 3, }, { -1, 4, -1, -1, -1, -1, -1, -1, -1, 5, }, };
	char x[6] = { 0, 0, 0, 0, 0, 0 };
	for (int i = 0; i < p->ChangeableCardsLen; i++)
	{
		if (CardType(p->ChangeableCards[i]) == MJ_TYPE_FENG || CardType(p->ChangeableCards[i]) == MJ_TYPE_JIAN)
		{
			continue;
		}
		if (!(CardNum(p->ChangeableCards[i]) == 1 || CardNum(p->ChangeableCards[i]) == 9))
		{
			return false;
		}		
		x[c[CardType(p->ChangeableCards[i])][CardNum(p->ChangeableCards[i])]] = -1;
	}
	for (int i = 0; i < p->FixedCardsLen; i++)
	{
		if (p->FixedCards[i].state & TYPE_CHI)
		{
			return false;
		}
		if (CardType(p->FixedCards[i].CardData) == MJ_TYPE_FENG || CardType(p->FixedCards[i].CardData) == MJ_TYPE_JIAN)
		{
			continue;
		}
		if (!(CardNum(p->FixedCards[i].CardData) == 1 || CardNum(p->FixedCards[i].CardData) == 9))
		{
			return false;
		}		
		x[c[CardType(p->FixedCards[i].CardData)][CardNum(p->FixedCards[i].CardData)]] = -1;
	}
	if (x[0] ^ x[1])
		return false;
	if (x[2] ^ x[3])
		return false;
	if (x[4] ^ x[5])
		return false;		
	char r = x[0] + x[1] + x[2] + x[3] + x[4] + x[5];	
	return r == -2;
}
bool CGameLogic::IsQuan19Wind(const HandCards& p, const _uint8 buf[4][4], _uint8 Jiang)
{
	_uint8 CardDataNum[MJ_TYPE_COMMON] = { 0, 0, 0, 0, 0 };
	for (int i = 0; i < p.ChangeableCardsLen; i++)
	{
		if (!(CardNum(p.ChangeableCards[i]) == 1 || CardNum(p.ChangeableCards[i]) == 9
			|| CardType(p.ChangeableCards[i]) == MJ_TYPE_FENG || CardType(p.ChangeableCards[i]) == MJ_TYPE_JIAN))
		{
			return false;
		}
		else
		{
			CardDataNum[CardType(p.ChangeableCards[i])]++;
		}
	}
	for (int i = 0; i < p.FixedCardsLen; i++)
	{
		if (p.FixedCards[i].state & TYPE_CHI)
		{
			return false;
		}
		if (!(CardNum(p.FixedCards[i].CardData) == 1 || CardNum(p.FixedCards[i].CardData) == 9
			|| CardType(p.FixedCards[i].CardData) == MJ_TYPE_FENG || CardType(p.FixedCards[i].CardData) == MJ_TYPE_JIAN))
		{
			return false;
		}		
		else
		{
			CardDataNum[CardType(p.FixedCards[i].CardData)]++;
		}		
	}
	return (CardDataNum[MJ_TYPE_WAN] + CardDataNum[MJ_TYPE_TIAO] + CardDataNum[MJ_TYPE_TONG] != 0);	
}
bool CGameLogic::Is7Dui(const HandCards *p)
{
	if (p->ChangeableCardsLen != 14)
		return false;

	for (int i=0; i<7; i++)
	{
		if (p->ChangeableCards[i*2] != p->ChangeableCards[i*2+1])
			return false;
	}
	return true;
}

bool CGameLogic::IsQingDui(HandCards* p)
{
	_uint8 bt = CardType(p->ChangeableCards[0]);
	for (int i=0; i<p->ChangeableCardsLen; i++)
	{
		if (bt != CardType(p->ChangeableCards[i]))
			return false;
	}

	for (int i=0; i<p->FixedCardsLen; i++)
	{
		if (bt != CardType(p->FixedCards[i].CardData))
			return false;
	}

	return true;
}

bool CGameLogic::IsJiangJiangHu(const HandCards* p, const _uint8 buf[4][4], const _uint8 Jiang)
{
	if(p == NULL) return false;
	for (int i=0; i<p->FixedCardsLen; i++)
	{
		if (CardNum(p->FixedCards[i].CardData)!=2 && CardNum(p->FixedCards[i].CardData)!=5 && CardNum(p->FixedCards[i].CardData)!=8)
			return false;
	}
	if (CardNum(Jiang)!=2 && CardNum(Jiang)!=5 && CardNum(Jiang)!=8)
		return false;

	//int x = 4 - p->FixedCardsLen;
	for (int i=0; i< 4; i++)
	{
		if (CardNum(buf[i][0])!=2 && CardNum(buf[i][0])!=5 && CardNum(buf[i][0])!=8)
			return false;
	}
	return true;
}

bool CGameLogic::IsLong7Dui(const HandCards* p, _uint8 &cbGenNum)
{
	if (p->ChangeableCardsLen != 14)
		return false;
	for (int i=0; i<6; i++)
	{
		if (p->ChangeableCards[i*2] != p->ChangeableCards[i*2+1])
			return false;
		if ((p->ChangeableCards[(i+1)*2] - p->ChangeableCards[i*2]) == 0)
			cbGenNum++;
	}
	if (cbGenNum >= 1)
		return(p->ChangeableCards[12] == p->ChangeableCards[13]);
	else
		return false;
}

bool CGameLogic::IsQing7Dui(const HandCards *p)
{
	if(p == NULL) return false;
	_uint8 ct = CardType(p->ChangeableCards[0]);
	for (int i=0; i<p->ChangeableCardsLen; i++)
		if (ct != CardType(p->ChangeableCards[i]))
			return false;

	return true;
}

// bool CGameLogic::IsQing19(HandCards* p)
// {
// 	if(p == NULL) return false;
// 	_uint8 bt = CardType(p->ChangeableCards[0]);
// 	for (int i=0; i<p->ChangeableCardsLen; i++)
// 	{
// 		if (bt != CardType(p->ChangeableCards[i]))
// 		{
// 			return false;
// 		}
// 	}
// 
// 	for (int i=0; i<p->FixedCardsLen; i++)
// 	{
// 		if (bt != CardType(p->FixedCards[i].CardData))
// 		{
// 			return false;
// 		}
// 	}
// 	
// 	return true;
// 
// }

bool CGameLogic::IsQingLong7Dui(HandCards* p)
{
	_uint8 ct = CardType(p->ChangeableCards[0]);

	for (int i=0; i<p->ChangeableCardsLen; i++)
		if (ct != CardType(p->ChangeableCards[i]))
			return false;

	return true;
}
bool CGameLogic::IsDanDiao(const HandCards& pHc)
{
	return (pHc.ChangeableCardsLen == 2) && (pHc.ChangeableCards[0] == pHc.ChangeableCards[1]);
}
bool CGameLogic::Has4SameCards(HandCards& pHc, _uint8 bCard)const
{
	int iCount = 0;
	for (int i = 0; i < pHc.ChangeableCardsLen; i++)
	{
		if (pHc.ChangeableCards[i] == bCard)
		{
			iCount++;
		}
	}
	return (iCount == 4);
}
bool CGameLogic::Has4SameCardsX(HandCards& pHc, _uint8 bCard)const
{
	int iCount = 0;
	for (int i = 0; i < pHc.ChangeableCardsLen; i++)
	{
		if (pHc.ChangeableCards[i] == bCard)
		{
			iCount++;
		}
	}
	for (int i = 0; i < pHc.FixedCardsLen; i++)
	{
		if (pHc.FixedCards[i].state & TYPE_CHI)
		{
			if (pHc.FixedCards[i].c[0] == bCard || 
				pHc.FixedCards[i].c[1] == bCard || 
				pHc.FixedCards[i].c[2] == bCard)
			{
				iCount++;
			}
		}
		else if (pHc.FixedCards[i].state & TYPE_PENG )
		{
			if (pHc.FixedCards[i].CardData == bCard)
			{
				iCount += 3;
			}
		}
		else if (pHc.FixedCards[i].state & TYPE_GANG)
		{
			if (pHc.FixedCards[i].CardData == bCard)
			{
				iCount += 4;
			}
		}
		
	}
	return (iCount == 4);
}

bool CGameLogic::IsXiaoSanYuan(const HandCards* p, const _uint8 CardsFormatBuf[HAND_CARD_KAN_MAX][4], const _uint8 Jiang)
{
    int jianCount = 0;
    for (int i=0; i<HAND_CARD_KAN_MAX; i++)
    {
        if (CardType(CardsFormatBuf[i][0]) == MJ_TYPE_JIAN)
		{
		    jianCount ++;
		}  
    }
    return (jianCount == 2) && (CardType(Jiang) == MJ_TYPE_JIAN);
}

bool CGameLogic::IsDaSanYuan(const HandCards* p, const _uint8 CardsFormatBuf[HAND_CARD_KAN_MAX][4], const _uint8 Jiang)
{
    int jianCount = 0;
    for (int i=0; i<HAND_CARD_KAN_MAX; i++)
    {
        if (CardType(CardsFormatBuf[i][0]) == MJ_TYPE_JIAN)
		{
            jianCount ++;
		}
    }
    return (jianCount == 3);
}

bool CGameLogic::IsMingSiGui(const HandCards* p, const _uint8 CardsFormatBuf[HAND_CARD_KAN_MAX][4], const _uint8 Jiang, _uint8 cbCard, bool bQuanPinDao)
{
	if (p->FixedCardsLen > HAND_CARD_KAN_MAX) return false;

	for (int i = (HAND_CARD_KAN_MAX - p->FixedCardsLen); i < HAND_CARD_KAN_MAX; ++i)
	{
		if (CardsFormatBuf[i][0] == CardsFormatBuf[i][1])
		{
			_uint8 cbCardData = CardsFormatBuf[i][0];
			for (int j = 0; j < (HAND_CARD_KAN_MAX - p->FixedCardsLen); ++j)
			{
				if(CardsFormatBuf[j][0] == CardsFormatBuf[j][1])
					continue;
				
				for (int k = 0; k < 4; ++k)
				{
					if(CardsFormatBuf[j][k] == cbCardData)
					{
						if (bQuanPinDao)
						{
							return true;
						}
						else
						{
							if (cbCardData == cbCard)
							{
								return true;
							}
						}
					}
				}
			}
		}
	}
    return false;
}

bool CGameLogic::IsAnSiGui(const HandCards* p, const _uint8 CardsFormatBuf[HAND_CARD_KAN_MAX][4], const _uint8 Jiang, _uint8 cbCard, bool bQuanPinDao)
{
	if (p->FixedCardsLen > HAND_CARD_KAN_MAX) return false;

	_uint8 bCardCount[MJ_TYPE_COMMON][10];
	memset(bCardCount, 0, sizeof(bCardCount));

	bCardCount[CardType(Jiang)][CardNum(Jiang)] += 2;

	for (int i = 0; i < (HAND_CARD_KAN_MAX - p->FixedCardsLen); ++i)
	{
		if (bQuanPinDao)
		{
			for (int j = 0; j < 4; ++j)
			{
				if (CardsFormatBuf[i][j] == INVALID_MJ_CARD) continue;

				_uint8 iType = CardType(CardsFormatBuf[i][j]);
				_uint8 iNum = CardNum(CardsFormatBuf[i][j]);
				if (++bCardCount[iType][iNum] >= 4)
				{
					return true;
				}
			}
		}
		else
		{
			if (CardsFormatBuf[i][0] == CardsFormatBuf[i][1])
			{
				_uint8 cbCardData = CardsFormatBuf[i][0];
				for (int j = 0; j < (HAND_CARD_KAN_MAX - p->FixedCardsLen); ++j)
				{
					if(CardsFormatBuf[j][0] == CardsFormatBuf[j][1])
						continue;
					
					for (int k = 0; k < 4; ++k)
					{
						if(CardsFormatBuf[j][k] == cbCardData && cbCardData == cbCard)
						{
							return true;
						}
					}
				}
			}
		}
	}
	return false;
}
bool CGameLogic::IsKa5Xing(const HandCards* p, _uint8 cbCard)
{
	if (p->FixedCardsLen > HAND_CARD_KAN_MAX) return false;

	if (CardType(cbCard) >= MJ_TYPE_FENG || CardNum(cbCard) != 5) 
	{
        return false;
	}

    HandCards hcTemp = *p;
	if (!ChuPai(&hcTemp, cbCard - 1) || !ChuPai(&hcTemp, cbCard) || !ChuPai(&hcTemp, cbCard + 1))
	{
        return false;
	}

	return FormatCards_Laizi(hcTemp.ChangeableCards, hcTemp.ChangeableCardsLen, 0);
}

bool CGameLogic::IsJiangYiSe(const HandCards* p)
{
	if(p == NULL) return false;

	for (int i=0; i<p->FixedCardsLen; i++)
	{
		if (CardType(p->FixedCards[i].CardData) > MJ_TYPE_TONG)
			return false;

		if (p->FixedCards[i].state & TYPE_CHI)
		{
			if ((CardNum(p->FixedCards[i].c[0])!=2 && CardNum(p->FixedCards[i].c[0])!=5 && CardNum(p->FixedCards[i].c[0])!=8)
				|| (CardNum(p->FixedCards[i].c[1])!=2 && CardNum(p->FixedCards[i].c[1])!=5 && CardNum(p->FixedCards[i].c[1])!=8)
				|| (CardNum(p->FixedCards[i].c[2])!=2 && CardNum(p->FixedCards[i].c[2])!=5 && CardNum(p->FixedCards[i].c[2])!=8))
				return false;
		}
		else
		{
			if (CardNum(p->FixedCards[i].CardData)!=2 && CardNum(p->FixedCards[i].CardData)!=5 && CardNum(p->FixedCards[i].CardData)!=8)
				return false;
		}
	}

	for (int i = 0; i < p->ChangeableCardsLen; ++i)
	{
		if (CardType(p->ChangeableCards[i]) > MJ_TYPE_TONG)
			return false;

		if (CardNum(p->ChangeableCards[i])!=2 && CardNum(p->ChangeableCards[i])!=5 && CardNum(p->ChangeableCards[i])!=8)
			return false;
	}
	
	return true;
}

bool CGameLogic::Is13Luan(const HandCards* p)
{
	if (p->FixedCardsLen > HAND_CARD_KAN_MAX) return false;

    _uint8 CardDataNum[MJ_TYPE_COMMON][10];
    GetCardCount(p, CardDataNum);

    for (int i = 0; i < MJ_TYPE_FENG; ++i)
    {   
        for (int j = 1; j < 10; ++j)
        {
            if (1 == CardDataNum[i][1])
            {
                if (1 == j || 4 == j || 7 == j)
                {
                    if (CardDataNum[i][j] != 1)
                    {
                        return false;
                    }
                }
                else
                {
                    if (CardDataNum[i][j] > 0)
                    {
                        return false;
                    }
                }
            }
            else if (1 == CardDataNum[i][2])
            {
                if (2 == j || 5 == j || 8 == j)
                {
                    if (CardDataNum[i][j] != 1)
                    {
                        return false;
                    }
                }
                else
                {
                    if (CardDataNum[i][j] > 0)
                    {
                        return false;
                    }
                }   
            }
            else if (1 == CardDataNum[i][3])
            {
                if (3 == j || 6 == j || 9 == j)
                {
                    if (CardDataNum[i][j] != 1)
                    {
                        return false;
                    }
                }
                else
                {
                    if (CardDataNum[i][j] > 0)
                    {
                        return false;
                    }
                } 
            }
            else
            {
                return false;
            }
        }
    }

    int iCount = 0;
    for (int i = 1; i < 5; ++i)
    {
        if (1 == CardDataNum[3][i])
        {
            iCount++;
        }
    }

    for (int i = 1; i < 4; ++i)
    {
        if (1 == CardDataNum[4][i])
        {
            iCount++;
        }
    }
    	
	return iCount == 5;
}

bool CGameLogic::Is13Feng(const HandCards* p)
{
    if(p == NULL) return false;

    for (int i = 0; i < p->ChangeableCardsLen; ++i)
    {
        int cardType = CardType(p->ChangeableCards[i]);
        if (cardType != MJ_TYPE_FENG && cardType != MJ_TYPE_JIAN)
        {
            return false;
        }
    }

    for (int i = 0; i < p->FixedCardsLen; ++i)
    {
        if (TYPE_PENG_GANG & p->FixedCards[i].state)
        {
            int cardType = CardType(p->FixedCards[i].CardData);
            if (cardType != MJ_TYPE_FENG && cardType != MJ_TYPE_JIAN)
            {
                return false;
            }
        }
        else
        {
            return false;
        }
    }
    
    return true;
}

bool CGameLogic::IsLuanJiangHu(const HandCards* p)
{
    if(p == NULL) return false;

    for (int i = 0; i < p->ChangeableCardsLen; ++i)
    {
        if (CardType(p->ChangeableCards[i]) >= MJ_TYPE_FENG 
            || (CardNum(p->ChangeableCards[i]) != 2 && CardNum(p->ChangeableCards[i]) != 5 && CardNum(p->ChangeableCards[i]) != 8))
        {
            return false;
        }
    }

    for (int i = 0; i < p->FixedCardsLen; ++i)
    {
        if (TYPE_PENG_GANG & p->FixedCards[i].state)
        {
            if (CardType(p->FixedCards[i].CardData) >= MJ_TYPE_FENG
                || (CardNum(p->FixedCards[i].CardData) != 2 && CardNum(p->FixedCards[i].CardData) != 5 && CardNum(p->FixedCards[i].CardData) != 8))
            {
                return false;
            }
        }
        else
        {
            return false;
        }
    }
    
    return true;
}

bool CGameLogic::IsQuanqiuren(const HandCards* pHc)
{

	if (pHc->ChangeableCardsLen - 2 == 0)
	{
		if (pHc->ChangeableCards[0] != pHc->ChangeableCards[1])
		{
			return false;
		}
		for (int i = 0; i < pHc->FixedCardsLen; i++)
		{
			if (pHc->FixedCards[i].state == TYPE_ANGANG)
			{
				return false;
			}
		}		
		return true;
	}

	return false;
}

bool CGameLogic::IsQuanQiuRen(const HandCards* pHc)
{
	if (pHc->ChangeableCardsLen == 2 && pHc->ChangeableCards[0] == pHc->ChangeableCards[1] 
		&& (CardNum(pHc->ChangeableCards[0]) == 2 || CardNum(pHc->ChangeableCards[0]) == 5 || CardNum(pHc->ChangeableCards[0]) == 8)) 
	{
		return true;
	}

	return false;
}

bool CGameLogic::IsPengPengHu(const HandCards* p, const _uint8 CardsFormatBuf[4][4], _uint8 Jiang)
{	
	_uint8 CardDataNum[MJ_TYPE_COMMON][10] = { { 0 } };
	GetCardCount(p, CardDataNum);
	return L_IsPengPengHu(p, INVALID_MJ_CARD, CardDataNum, 0);	
}

bool CGameLogic::Is13Yao(const HandCards* p, _uint8 c, _uint8 CardsFormatBuf[4][4], _uint8 Jiang)
{
	static _uint8 YaoPai[13] = { 0x01, 0x09, 0x11, 0x19, 0x21, 0x29, 0x31, 0x32, 0x33, 0x34, 0x41, 0x42, 0x43 };
	HandCards thc;
	int  DuiCount = 0;

	thc = *p;
	AddCard(&thc, c);

	
	if (thc.ChangeableCardsLen != 14)
	{
		return false;
	}
	for (int i = 0, j = 0; i < MAX_COUNT; i++, j++)
	{
		if (thc.ChangeableCards[i] == YaoPai[j])
		{
			continue;
		}
		else if (thc.ChangeableCards[i] == thc.ChangeableCards[i - 1] && thc.ChangeableCards[i] == YaoPai[j - 1])
		{
			j--;
			DuiCount++;
		}
		else
		{
			return false;
		}
	}
	ChuPai(&thc, c);

	return (DuiCount == 1);
}

bool CGameLogic::IsMenQing(const HandCards* p)
{
	for (int i = 0; i < p->FixedCardsLen; i++)
	{
		if (p->FixedCards[i].state != TYPE_ANGANG)
			return false;
	}
	return true;
}

bool CGameLogic::IsMenQing_02(const HandCards* p)
{
	if (p->FixedCardsLen == 0)
	{
		return true;
	}
	return false;
}

bool CGameLogic::IsYiTiaoLong(const HandCards* p)
{
	_uint8 tmpBuf[MJ_TYPE_FENG][10];
	memset(tmpBuf, 0, sizeof(tmpBuf));
	for (int i = 0; i < p->ChangeableCardsLen; i++)
	{
		_uint8 type = CardType(p->ChangeableCards[i]);
		_uint8 num = CardNum(p->ChangeableCards[i]);
		if (type < MJ_TYPE_FENG)
		{
			tmpBuf[type][num]++;
		}		
	}
	for (int i = 0; i < p->FixedCardsLen; i++)
	{
		if (p->FixedCards[i].state & TYPE_CHI)
		{
			tmpBuf[CardType(p->FixedCards[i].c[0])][CardNum(p->FixedCards[i].c[0])]++;
			tmpBuf[CardType(p->FixedCards[i].c[1])][CardNum(p->FixedCards[i].c[1])]++;
			tmpBuf[CardType(p->FixedCards[i].c[2])][CardNum(p->FixedCards[i].c[2])]++;
		}
	}
	for (int i = 0; i < MJ_TYPE_FENG; i++)
	{
		int iCount = 0;
		for (int j = 1; j < 10; j++)
		{
			if (tmpBuf[i][j] >0)
			{
				iCount++;
			}
		}
		if (iCount == 9)
			return true;
	}
	return false;
}

bool CGameLogic::IsYiTiaoLong_PX(const HandCards* p, const _uint8 CardsFormatBuf[HAND_CARD_KAN_MAX][4], const _uint8 Jiang)
{
	if (p->FixedCardsLen > HAND_CARD_KAN_MAX) return false;

    bool bKan = false;
    _uint8 bShun[10] = {0};
	for (int i = 0; i < HAND_CARD_KAN_MAX; ++i)
	{
		if (CardsFormatBuf[i][0] == CardsFormatBuf[i][1])
		{
            bKan = true;
		}
		else
		{
            for (int j = 0; j < 3; ++j)
            {
                bShun[CardNum(CardsFormatBuf[i][j])]++;
            }
		}
	}

    for (int i = 1; i < 10; ++i)
    {
        if (bShun[i] != 1)
        {
            return false;
        }
    }
    	
	return bKan;
}

bool CGameLogic::L_Is13Yao(const HandCards* p, _uint8 c, _uint8 CardDataNum[MJ_TYPE_COMMON][10], int nHunNum)
{
	//static _uint8 YaoPai[13] = {0x01, 0x09, 0x11, 0x19, 0x21, 0x29,0x31,0x32,0x33,0x34,0x41,0x42,0x43};
	HandCards thc;
	thc = *p;
	int OneNum = 0;
	int TwoNum = 0;
	//_uint8 CardDataNum[5][10] = {0};
	//????????
	if ((thc.ChangeableCardsLen + nHunNum) != 14)
	{
		return false;
	}
	for (int i = 1; i < 5; i++)
	{
		if (CardDataNum[3][i] == 1)
		{
			OneNum++;
		}
		if (CardDataNum[3][i] == 2)
		{
			TwoNum++;
		}
	}

	for (int i = 1; i < 4; i++)
	{
		if (CardDataNum[4][i] == 1)
		{
			OneNum++;
		}
		if (CardDataNum[4][i] == 2)
		{
			TwoNum++;
		}
	}

	for (int i = 0; i < 3; i++)
	{
		if (CardDataNum[i][1] == 1)
		{
			OneNum++;
		}
		if (CardDataNum[i][1] == 2)
		{
			TwoNum++;
		}
		if (CardDataNum[i][9] == 1)
		{
			OneNum++;
		}
		if (CardDataNum[i][9] == 2)
		{
			TwoNum++;
		}
	}
	if (OneNum == (14 - nHunNum - TwoNum * 2))
	{
		if (TwoNum > 1)
		{
			return false;
		}
		return true;
	}
	return false;
}

bool CGameLogic::L_IsQiDui(const HandCards* p, _uint8 c, _uint8 CardDataNum[MJ_TYPE_COMMON][10], int nHunNum)
{
	if (p->FixedCardsLen != 0)
	{
		return false;
	}
	int nNeedHun = 0;
	_uint8 last = INVALID_MJ_CARD;
	for (int i = 0; i < p->ChangeableCardsLen; i++)
	{
		if (p->ChangeableCards[i] == last) continue;
		_uint8 m = CardType(p->ChangeableCards[i]);
		_uint8 n = CardNum(p->ChangeableCards[i]);
		if (CardDataNum[m][n] == 1)
		{
			nNeedHun++;
		}
		if (CardDataNum[m][n] == 3)
		{
			nNeedHun++;
		}
		last = p->ChangeableCards[i];
	}
	if (nNeedHun <= nHunNum)
	{
		return true;
	}
	return false;
}

int CGameLogic::L_IsQiDui_02(const HandCards* p, _uint8 c, const _uint8 CardDataNum[MJ_TYPE_COMMON][10], int nHunNum)
{	
	if (p->FixedCardsLen != 0)
	{
		return false;
	}
	int nNeedHun = 0;
	_uint8 last = INVALID_MJ_CARD;
	for (int i = 0; i < p->ChangeableCardsLen; i++)
	{
		if (p->ChangeableCards[i] == last) continue;
		_uint8 m = CardType(p->ChangeableCards[i]);
		_uint8 n = CardNum(p->ChangeableCards[i]);
		if (CardDataNum[m][n] == 1)
		{
			nNeedHun++;
		}
		if (CardDataNum[m][n] == 3)
		{
			nNeedHun++;
		}
		last = p->ChangeableCards[i];
	}
	return nNeedHun;
}

bool CGameLogic::L_IsPengPengHu(const HandCards* p, _uint8 c, _uint8 CardDataNum[MJ_TYPE_COMMON][10], int nHunNum)
{
	int nNeedHun = 0;
	const HandCards& pHc = *p;
	for (int i = 0; i < pHc.FixedCardsLen; i++)
	{
		if (pHc.FixedCards[i].state & TYPE_CHI)			
		{
			return false;
		}
	}
	_uint8 last = INVALID_MJ_CARD;
	for (int i = 0; i < pHc.ChangeableCardsLen; i++)
	{
		if (pHc.ChangeableCards[i] == last)continue;
		_uint8 m = CardType(pHc.ChangeableCards[i]);
		_uint8 n = CardNum(pHc.ChangeableCards[i]);	
		if ( (CardDataNum[m][n] == 1 ) || (CardDataNum[m][n] == 4) )
		{
			nNeedHun += 2;
		}
		if (CardDataNum[m][n] == 2)
		{
			nNeedHun += 1;
		}
		last = pHc.ChangeableCards[i];
	}
	if (nNeedHun <= nHunNum + 1)
	{
		return true;
	}
	return false;
}

int CGameLogic::L_IsPengPengHu_02(const HandCards* p, _uint8 c, _uint8 CardDataNum[MJ_TYPE_COMMON][10], int nHunNum)
{
	int nNeedHun = 0;
	const HandCards& pHc = *p;
	for (int i = 0; i < pHc.FixedCardsLen; i++)
	{
		if (pHc.FixedCards[i].state & TYPE_CHI)
		{
			return 1000;
		}
	}

	_uint8 last = INVALID_MJ_CARD;
	for (int i = 0; i < pHc.ChangeableCardsLen; i++)
	{
		if (pHc.ChangeableCards[i] == last)continue;
		_uint8 m = CardType(pHc.ChangeableCards[i]);
		_uint8 n = CardNum(pHc.ChangeableCards[i]);
		if ((CardDataNum[m][n] == 1) || (CardDataNum[m][n] == 4))
		{
			nNeedHun += 2;
		}
		if (CardDataNum[m][n] == 2)
		{
				nNeedHun += 1;
		}	
		last = pHc.ChangeableCards[i];
	}
	return nNeedHun;	
}