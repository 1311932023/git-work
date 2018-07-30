#include <assert.h>
#include <sys/time.h>
#include <algorithm>

#include "game_logic.h"

static bool _is19(_uint8 bCard)
{
	_uint8 type = CardType(bCard);
	_uint8 num = CardNum(bCard);

	if (type <= MJ_TYPE_BING)
	{
		return num == 1 || num == 9;
	}
	return  (type == MJ_TYPE_FENG || type == MJ_TYPE_JIAN);
}
//
//bool CGameLogic::Is13Yao(const HandCards* p, _uint8 c, _uint8 CardsFormatBuf[4][4], _uint8 Jiang)
//{
//	static _uint8 YaoPai[13] = { 0x01, 0x09, 0x11, 0x19, 0x21, 0x29, 0x31, 0x32, 0x33, 0x34, 0x41, 0x42, 0x43 };
//	HandCards thc;
//	int  DuiCount = 0;
//
//	thc = *p;
//	AddCard(&thc, c);
//
//	//必须门清
//	if (thc.ChangeableCardsLen != 14)
//	{
//		return false;
//	}
//	for (int i = 0, j = 0; i < MAX_COUNT; i++, j++)
//	{
//		if (thc.ChangeableCards[i] == YaoPai[j])
//		{
//			continue;
//		}
//		else if (thc.ChangeableCards[i] == thc.ChangeableCards[i - 1] && thc.ChangeableCards[i] == YaoPai[j - 1])
//		{
//			j--;
//			DuiCount++;
//		}
//		else
//		{
//			return false;
//		}
//	}
//	ChuPai(&thc, c);
//
//	return (DuiCount == 1);
//}
//peng

bool CGameLogic::IsQuanqiuren(const HandCards* pHc)
{

	if (pHc->ChangeableCardsLen ==  2 )
	{
		if (pHc->ChangeableCards[0] != pHc->ChangeableCards[1])
		{
			return false;
		}
		
		return true;
	}

	return false;
}

int CGameLogic::in_vector( std::vector<int> &all_Vec , int c )
{
	std::vector<int>::iterator  it = find(all_Vec.begin(), all_Vec.end(), c);
	if (it != all_Vec.end())
		return  1;
	return 0;

	
}

int CGameLogic::Getchi_laizi(const  HandCards  *p  , _uint8 cbCard  , MJ_opinfo  &ChiInfo  ,  std::vector<int>  &all_laizi_vec)
{
	if (CardType(cbCard) >= MJ_TYPE_FENG  || p == NULL  )  
		return TYPE_NULL;

	int cbMask = TYPE_NULL;
	bool bFind[4] = { false, false, false, false };
	for (int i = 0; i < p->ChangeableCardsLen; i++)
	{
		
		if (p->ChangeableCards[i] + 2 == cbCard  &&   !in_vector(all_laizi_vec ,  cbCard-2 ) )
		{
			bFind[0] = true;
		}
		else if (p->ChangeableCards[i] + 1 == cbCard  &&  !in_vector(all_laizi_vec , cbCard-1))
		{
			bFind[1] = true;
		}
		else if (p->ChangeableCards[i] == cbCard + 1  &&  !in_vector(all_laizi_vec , cbCard+1))
		{
			bFind[2] = true;
		}
		else if (p->ChangeableCards[i] == cbCard + 2  && !in_vector(all_laizi_vec , cbCard+2 ) )
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

void CGameLogic::ExecuteLeftChi(HandCards *handcard, _uint8 chairID, _uint8 cbOperateCard)
{
	//assert(CardType(cbOperateCard) < MJ_TYPE_FENG);
	//删除扑克
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
	//assert(CardType(cbOperateCard) < MJ_TYPE_FENG);
	//删除扑克
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
	//assert( CardType(cbOperateCard) < MJ_TYPE_FENG);
	if (CardType(cbOperateCard) >= MJ_TYPE_FENG)
	{
		//log_error("cbOperateCard[%d]" , cbOperateCard );
		return;
	}
	
	//删除扑克
	_uint8 cbRemoveCard[] = { cbOperateCard - 2, cbOperateCard - 1 };
	RemoveCard(handcard, (_uint8*)cbRemoveCard, 2);

	handcard->FixedCards[handcard->FixedCardsLen].c[0] = cbOperateCard - 2;
	handcard->FixedCards[handcard->FixedCardsLen].c[1] = cbOperateCard - 1;
	handcard->FixedCards[handcard->FixedCardsLen].c[2] = cbOperateCard;
	handcard->FixedCards[handcard->FixedCardsLen].state = TYPE_RIGHT_CHI;
	handcard->FixedCards[handcard->FixedCardsLen].chairID = chairID;
	handcard->FixedCardsLen++;
}
bool CGameLogic::IsPengPengHu(const HandCards* p, const _uint8 CardsFormatBuf[4][4], _uint8 Jiang)
{	
	_uint8 CardDataNum[MJ_TYPE_COMMON][10] = { { 0 } };
	GetCardCount(p, CardDataNum);

	return L_IsPengPengHu(p, INVALID_MJ_CARD, CardDataNum, 0);	
}

bool CGameLogic::hai_di_lao_yue(_uint8  card_pool_isEmpty , _uint8 card_pool_last, bool zimo  ,  _uint8 jiang)
{
	if (zimo &&  card_pool_isEmpty )
	{		
		return true;
	}
	return  false;
}

bool CGameLogic::hai_di_pao(bool  card_pool_isEmpty, bool fanpao )
{
	if (fanpao  && card_pool_isEmpty)
	{		
		return true;
	}
	return false;
}

int   CGameLogic::HasOneAnGang( const  HandCards  *p)
{
	int gang = 0;
	for (int i = 0; i < p->FixedCardsLen; i++)
	{
		if (p->FixedCards[i].state == TYPE_ANGANG)
			gang++;
	}
	return  gang; 
}


bool CGameLogic::IsMenQing(const HandCards* p)
{
	//if (p->FixedCardsLen > 4)  // modify 
		//return false;
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

//
//bool CGameLogic::IsYiTiaoLong(const HandCards* p, const MJ_win_pattern& oPattern)
//{	
//	bool buf[MJ_TYPE_COMMON][10] ;
//	memset(buf, false, sizeof(buf));
//	for (int i = 0; i < HAND_CARD_KAN_MAX; i++)
//	{
//		if (oPattern.kan[i][0] == 0 || oPattern.kan[i][0] == INVALID_MJ_CARD)
//			break;
//		if (oPattern.kan[i][0] == oPattern.kan[i][1])
//			continue;
//		_uint8 type = CardType(oPattern.kan[i][0]);
//		_uint8 num = CardNum(oPattern.kan[i][0]);
//		buf[type][num] = true;
//	}
//	for (int i = 0; i < p->FixedCardsLen; i++)
//	{
//		if (p->FixedCards[i].state & TYPE_CHI)
//		{
//			_uint8 type = CardType(p->FixedCards[i].CardData);
//			_uint8 num = CardNum(p->FixedCards[i].CardData);
//			buf[type][num] = true;
//		}
//	}
//	return (buf[0][1] && buf[0][4] && buf[0][7]) || (buf[1][1] && buf[1][4] && buf[1][7]) || (buf[2][1] && buf[2][4] && buf[2][7]);
//}


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
	//memcpy(&tempData, &stData, sizeof(MjCardData));
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


//
//bool CGameLogic::L_Is13Yao(const HandCards* p, _uint8 c, _uint8 CardDataNum[MJ_TYPE_COMMON][10], int nHunNum)
//{
//	//static _uint8 YaoPai[13] = {0x01, 0x09, 0x11, 0x19, 0x21, 0x29,0x31,0x32,0x33,0x34,0x41,0x42,0x43};
//	HandCards thc;
//	thc = *p;
//	int OneNum = 0;
//	int TwoNum = 0;
//	//_uint8 CardDataNum[5][10] = {0};
//	//必须门清
//	if ((thc.ChangeableCardsLen + nHunNum) != 14)
//	{
//		return false;
//	}
//	for (int i = 1; i < 5; i++)
//	{
//		if (CardDataNum[3][i] == 1)
//		{
//			OneNum++;
//		}
//		if (CardDataNum[3][i] == 2)
//		{
//			TwoNum++;
//		}
//	}
//
//	for (int i = 1; i < 4; i++)
//	{
//		if (CardDataNum[4][i] == 1)
//		{
//			OneNum++;
//		}
//		if (CardDataNum[4][i] == 2)
//		{
//			TwoNum++;
//		}
//	}
//
//	for (int i = 0; i < 3; i++)
//	{
//		if (CardDataNum[i][1] == 1)
//		{
//			OneNum++;
//		}
//		if (CardDataNum[i][1] == 2)
//		{
//			TwoNum++;
//		}
//		if (CardDataNum[i][9] == 1)
//		{
//			OneNum++;
//		}
//		if (CardDataNum[i][9] == 2)
//		{
//			TwoNum++;
//		}
//	}
//	if (OneNum == (14 - nHunNum - TwoNum * 2))
//	{
//		if (TwoNum > 1)
//		{
//			return false;
//		}
//		return true;
//	}
//	return false;
//}


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
bool CGameLogic::L_IsQiDui_03(const HandCards* p, _uint8 c, _uint8 CardDataNum[MJ_TYPE_COMMON][10], int nHunNum, int& iLongQiDui)
{
	if (p->FixedCardsLen != 0)
	{
		return false;
	}
	int nNeedHun = 0;
	_uint8 last = INVALID_MJ_CARD;
	iLongQiDui = 0;
	for (int i = 0; i < p->ChangeableCardsLen; i++)
	{
		if (p->ChangeableCards[i] == last) continue;
		_uint8 m = CardType(p->ChangeableCards[i]);
		_uint8 n = CardNum(p->ChangeableCards[i]);
		if (CardDataNum[m][n] == 1)
		{
			nNeedHun++;
		}
		else if (CardDataNum[m][n] == 3)
		{
			nNeedHun++;
			iLongQiDui++;
		}
		else if (CardDataNum[m][n] == 4)
		{
			iLongQiDui++;
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

 // 第四个参数传的是 0  ；nHunNum = 0  ；


bool CGameLogic::L_IsPengPengHu(const HandCards* p, _uint8 c, _uint8 CardDataNum[MJ_TYPE_COMMON][10], int nHunNum)
{
	int nNeedHun = 0;
	const HandCards& pHc = *p;
	for (int i = 0; i < pHc.FixedCardsLen; i++)   // 碰碰胡。。不可以吃。。  
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
		if ( (CardDataNum[m][n] == 1 ) || (CardDataNum[m][n] == 4) )   //3*n + 2 模式 
		{
			nNeedHun += 2;
			return  false;
		}
		if (CardDataNum[m][n] == 2)
		{
			nNeedHun += 1;
		}
		last = pHc.ChangeableCards[i];
	}
	if (nNeedHun <= nHunNum + 1)   // 小于 1  
	{
		return true;
	}
	return false;
}

//
//int CGameLogic::L_IsPengPengHu_02(const HandCards* p, _uint8 c, _uint8 CardDataNum[MJ_TYPE_COMMON][10], int nHunNum)
//{
//	int nNeedHun = 0;
//	const HandCards& pHc = *p;
//	for (int i = 0; i < pHc.FixedCardsLen; i++)
//	{
//		if (pHc.FixedCards[i].state & TYPE_CHI)
//		{
//			return 1000;
//		}
//	}
//
//	_uint8 last = INVALID_MJ_CARD;
//	for (int i = 0; i < pHc.ChangeableCardsLen; i++)
//	{
//		if (pHc.ChangeableCards[i] == last)continue;
//		_uint8 m = CardType(pHc.ChangeableCards[i]);
//		_uint8 n = CardNum(pHc.ChangeableCards[i]);
//		if ((CardDataNum[m][n] == 1) || (CardDataNum[m][n] == 4))
//		{
//			nNeedHun += 2;
//		}
//		if (CardDataNum[m][n] == 2)
//		{
//				nNeedHun += 1;
//		}	
//		last = pHc.ChangeableCards[i];
//	}
//	return nNeedHun;	
//}



//每一张牌的张数。  函数 

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
int CGameLogic::GetCardCountH(const HandCards* pHc, _uint8 bCard)
{
	int iCounts = 0;
	for (int i = 0; i < pHc->ChangeableCardsLen; i++)
	{
		if (pHc->ChangeableCards[i] == bCard)
		{
			iCounts++;
		}
	}
	return iCounts;
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
int CGameLogic::GetCardCount(const HandCards* pHc, MJ_BUFFER<CardNode_count, HAND_CARD_SIZE_MAX>& h)
{
	_uint8 curCard = 0;
	for (int i = 0; i < pHc->ChangeableCardsLen; i++)
	{
		if (pHc->ChangeableCards[i] == curCard)
			continue;
		curCard = pHc->ChangeableCards[i];
		h.Add(CardNode_count(curCard, GetCardCountH(pHc, curCard)));
	}
	return h.Size();
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

bool CGameLogic::GetCardTypeCount(const HandCards* pHc, _uint8 CardTypeCount[MJ_TYPE_COMMON])
{
	memset(CardTypeCount, 0, sizeof(_uint8) * MJ_TYPE_COMMON);
	for (int i = 0; i < pHc->ChangeableCardsLen; i++)
	{
		_uint8 type = CardType(pHc->ChangeableCards[i]);
		if (type >= MJ_TYPE_COMMON)
			return false;
		CardTypeCount[type]++;
	}
	for (int i = 0; i < pHc->FixedCardsLen; i++)
	{
		if (pHc->FixedCards[i].state & TYPE_CHI)
		{			
			_uint8 type = CardType(pHc->FixedCards[i].CardData);
			CardTypeCount[type] += 3;
			
		}
		else if (pHc->FixedCards[i].state & TYPE_PENG)
		{
			_uint8 type = CardType(pHc->FixedCards[i].CardData);
			CardTypeCount[type] += 3;				
		}
		else if (pHc->FixedCards[i].state & TYPE_GANG)
		{
			_uint8 type = CardType(pHc->FixedCards[i].CardData);
			CardTypeCount[type] += 4;				
		}
	}
	return true;
}

bool CGameLogic::Is_BianZhang(const HandCards* pHc, _uint8 bHuCard, const MJ_win_pattern& oPattern)
{
	if (!(CardNum(bHuCard) == 3 || CardNum(bHuCard) == 7))
	{
		return false;
	}
	if (CardType(bHuCard) > MJ_TYPE_TONG)
	{
		return false;
	}
	bool bBianZhang = false;
	for (int i = 0; i < HAND_CARD_KAN_MAX; i++)
	{
		if (oPattern.kan[i][0] == 0)
		{
			break;
		}
		int iCount = 0;
		for (int j = 0; j < 3; j++)
		{
			if (oPattern.kan[i][j] == bHuCard)
				iCount++;
		}
		if (iCount == 0) continue;
		if (iCount > 1) continue;
		if (iCount == 1)
		{
			if (CardNum(bHuCard) == 3 )
			{				
				if (CardNum(oPattern.kan[i][0]) == 3  &&
					CardNum(oPattern.kan[i][1]) == 4 &&
					CardNum(oPattern.kan[i][2]) == 5)
					return false;					
				if (CardNum(oPattern.kan[i][0]) == 1 &&
					CardNum(oPattern.kan[i][1]) == 2 &&
					CardNum(oPattern.kan[i][2]) == 3)
					bBianZhang = true;
			}
			else if (CardNum(bHuCard) == 7)
			{
				if (CardNum(oPattern.kan[i][0]) == 5 &&
					CardNum(oPattern.kan[i][1]) == 6 &&
					CardNum(oPattern.kan[i][2]) == 7)
					return false;
				if (CardNum(oPattern.kan[i][0]) == 7 &&
					CardNum(oPattern.kan[i][1]) == 8 &&
					CardNum(oPattern.kan[i][2]) == 9)
					bBianZhang = true;
			}			
		}
	}	
	return bBianZhang;
}
bool CGameLogic::Is_KaZhang(const HandCards* pHc, _uint8 bHuCard, const MJ_win_pattern& oPattern)
{
	if ((CardNum(bHuCard) == 1 || CardNum(bHuCard) == 9))
	{
		return false;
	}
	if (CardType(bHuCard) > MJ_TYPE_TONG)
	{
		return false;
	}
	if (bHuCard == oPattern.jiang)
		return false;

	bool bKanZhang = false;
	for (int i = 0; i < HAND_CARD_KAN_MAX; i++)
	{
		if (oPattern.kan[i][0] == 0)
		{
			break;
		}
		int iCount = 0;
		for (int j = 0; j < 3; j++)
		{
			if (oPattern.kan[i][j] == bHuCard)
				iCount++;
		}
		if (iCount == 0) continue;
		if (iCount > 1) continue;
		if (iCount == 1)
		{
			if (oPattern.kan[i][0] == bHuCard &&
				oPattern.kan[i][1] == (bHuCard + 1) &&
				oPattern.kan[i][2] == (bHuCard + 2))
				return false;
			if (oPattern.kan[i][0] == bHuCard &&
				oPattern.kan[i][1] == (bHuCard - 1) &&
				oPattern.kan[i][2] == (bHuCard - 2))
				return false;
			if (oPattern.kan[i][0] == (bHuCard - 1) &&
				oPattern.kan[i][1] == (bHuCard) &&
				oPattern.kan[i][2] == (bHuCard + 1))
				bKanZhang = true;
		}
	}
	if (bKanZhang)
	{
		HandCards tmp = *pHc;
		ChuPai(&tmp, bHuCard);
		MJ_win_pattern pattern;
		AddCard(&tmp, bHuCard - 1);
		if (FormatCards(&tmp, pattern))
			return false;

		ChuPai(&tmp, bHuCard - 1);
		AddCard(&tmp, bHuCard + 1);
		if (FormatCards(&tmp, pattern))
			return false;		
		return true;
	}
	return false;
}
bool CGameLogic::Is_DaKuanZhang(const HandCards* pHc, _uint8 bHuCard, const MJ_win_pattern& oPattern)
{
	if (CardType(bHuCard) > MJ_TYPE_TONG)
	{
		return false;
	}
	static _uint8 dakuanzhang[10][2] = {
		{ 0, 0 },
		{ 4, 7 }, { 5, 8 }, { 6, 9 }, { 1, 7 }, { 2, 8 }, { 3, 9 }, { 1, 4 }, { 2, 5 }, { 3, 6 },
	};
	HandCards tmp = *pHc;
	ChuPai(&tmp, bHuCard);
	_uint8 num = CardNum(bHuCard);
	_uint8 zhang_0 = CardType(bHuCard) << 4 | dakuanzhang[num][0];
	AddCard(&tmp, zhang_0);

	MJ_win_pattern pattern;
	if (!FormatCards(&tmp, pattern))
		return false;

	_uint8 zhang_1 = CardType(bHuCard) << 4 | dakuanzhang[num][1];
	ChuPai(&tmp, zhang_0);
	AddCard(&tmp, zhang_1);
	if (!FormatCards(&tmp, pattern))
		return false;

	return true;
}

bool CGameLogic::Find_Jiang258(const HandCards* pHc, MJ_hand_buffer& hc)
{
	static bool c[10] = { false, false, true, false, false, true, false, false, true, false };
	hc.Clear();
	bool all = true;
	for (int i = 0; i < pHc->ChangeableCardsLen; i++)
	{
		_uint8 type = CardType(pHc->ChangeableCards[i]);
		_uint8 num = CardNum(pHc->ChangeableCards[i]);
		if (type < MJ_TYPE_FENG && c[num] )
		{
			hc.Add(pHc->ChangeableCards[i]);
		}		
		else 
			all = false;		
	}
	for (int i = 0; i < pHc->FixedCardsLen; i++)
	{
		if (pHc->FixedCards[i].state & TYPE_CHI)
		{
			for (int j = 0; j < 3; j++)
			{
				_uint8 type = CardType(pHc->FixedCards[i].c[j]);
				_uint8 num = CardNum(pHc->FixedCards[i].c[j]);
				if (type < MJ_TYPE_FENG && c[num])
				{
					hc.Add(pHc->FixedCards[i].c[j]);
				}
				else all = false;
			}
		}		
		else if (pHc->FixedCards[i].state & TYPE_PENG)
		{
			for (int j = 0; j < 4; j++)
			{
				_uint8 type = CardType(pHc->FixedCards[i].CardData);
				_uint8 num = CardNum(pHc->FixedCards[i].CardData);
				if (type < MJ_TYPE_FENG && c[num])
				{
					hc.Add(pHc->FixedCards[i].CardData);
				}
				else all = false;
			}
		}
		else if (pHc->FixedCards[i].state & TYPE_GANG)
		{
			for (int j = 0; j < 4; j++)
			{
				_uint8 type = CardType(pHc->FixedCards[i].CardData);
				_uint8 num = CardNum(pHc->FixedCards[i].CardData);
				if (type < MJ_TYPE_FENG && c[num])
				{
					hc.Add(pHc->FixedCards[i].CardData);
				}
				else all = false;
			}
		}
	}
	return all;
}

// 三同  1w 1条。1筒

bool CGameLogic::Find_SanTong(const HandCards* pHc, MJ_hand_buffer& hc)
{
	hc.Clear();
	MJ_hand_buffer tmp; 	
	for (int i = 0; i < pHc->ChangeableCardsLen; i++)
	{
		int iCounts = 0;
		for (int j = i + 1; j < pHc->ChangeableCardsLen; i = j++)
		{			
			if (pHc->ChangeableCards[i] == pHc->ChangeableCards[j])
			{
				iCounts++;				
				continue;
			}				
			break;				
		}
		if (iCounts > 0)
		{
			tmp.Add(pHc->ChangeableCards[i]);
		}
	}
	_uint8 c[10] = { 0 };
	for (int i = 0; i < tmp.Size(); i++)
	{
		c[CardNum(tmp[i])] ++;
	}
	for (int i = 1; i < 10; i++)
	{
		if (c[i] == 3)
		{
			hc.Add(i);
			hc.Add((1 << 4) + i);
			hc.Add((2 << 4) + i);
			return true;
		}			
	}
	return false;
}
bool CGameLogic::Find_JieJieGao(const HandCards* pHc, MJ_hand_buffer& hc)
{
	hc.Clear();
	MJ_hand_buffer tmp;
	for (int i = 0; i < pHc->ChangeableCardsLen; i++)
	{
		int iCounts = 0;
		for (int j = i + 1; j < pHc->ChangeableCardsLen; i = j++)
		{
			if (pHc->ChangeableCards[i] == pHc->ChangeableCards[j])
			{
				iCounts++;				
				continue;
			}
			break;
		}
		if (iCounts > 0)
		{
			tmp.Add(pHc->ChangeableCards[i]);
		}
	}
	for (int i = 0; i < tmp.Size(); i++)
	{
		int iCounts = 0;
		for (int j = i + 1; j < tmp.Size(); j++)
		{
			if (tmp[j] - tmp[j - 1] == 1)
			{
				iCounts++;
				continue;
			}
			break;
		}
		if (iCounts > 1)
		{
			hc.Add(tmp[i]);
			hc.Add(tmp[i] + 1);
			hc.Add(tmp[i] + 2);
			return true;
		}
	}	
	return false;
}


int CGameLogic::Get19Count(const HandCards* pHc)
{
	int iCount = 0;
	for (int i = 0; i < pHc->ChangeableCardsLen; i++)
	{
		if (_is19(pHc->ChangeableCards[i]))
			iCount++;		
	}
	for (int i = 0; i < pHc->FixedCardsLen; i++)
	{
		if (pHc->FixedCards[i].state & TYPE_CHI)
		{
			if (_is19(pHc->FixedCards[i].c[0])) iCount++;
			if (_is19(pHc->FixedCards[i].c[1])) iCount++;
			if (_is19(pHc->FixedCards[i].c[2])) iCount++;
		}
		else if (pHc->FixedCards[i].state & TYPE_PENG)
		{
			if (_is19(pHc->FixedCards[i].CardData)) iCount += 3;
		}
		else if (pHc->FixedCards[i].state & TYPE_GANG)
		{
			if (_is19(pHc->FixedCards[i].CardData)) iCount += 4;			
		}
	}
	return iCount;
}