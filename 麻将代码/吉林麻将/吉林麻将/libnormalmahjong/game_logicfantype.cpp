#include "game_logic.h"

#include <map>
#include "../common/log.h"

////////清一色

bool CGameLogic::qingyise_King(const HandCards  *p  , int  laizi_a  , int laizi_b )
{
	if (p == NULL) return  false; 
	int   ct = 9; 
	for(int i = 0; i < p->ChangeableCardsLen; i++)
	{
		if (p->ChangeableCards[i] == laizi_b ||  laizi_a == p->ChangeableCards[i]) 
			continue; 
		if (ct == 9 )
			ct = CardType(p->ChangeableCards[i]);  
		else
		{
			if (ct != CardType(p->ChangeableCards[i]))
				return false;
		}
		 
	}
	if (p->FixedCards->state != TYPE_NULL)
	{
		for (int i = 0; i < p->FixedCardsLen; i++)
		{
			if (ct == 9)
				ct = CardType(p->FixedCards[i].CardData); 
			else
			{
				if (ct != CardType(p->FixedCards[i].CardData))
					return false;
			}
			
		}
	}
	return  true;
}

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

	for (int i = 0; i < p->ChangeableCardsLen; i++)
	if (ct != CardType(p->ChangeableCards[i]))
		return false;

	if (p->FixedCards->state != TYPE_NULL)
	{
		for (int i = 0; i < p->FixedCardsLen; i++)
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
		if (CardType(p.ChangeableCards[i]) >= MJ_TYPE_FENG)
		{
			return false;
		}
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
		if (CardType(p.FixedCards[i].CardData) >= MJ_TYPE_FENG)
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

bool CGameLogic::is_qi_dui(const HandCards *p)
{
	if (p->ChangeableCardsLen != 14)
		return false;

	for (int i=0; i<7; i++)
	{
		if (p->ChangeableCards[i*2] != p->ChangeableCards[i*2+1])  // 0 1 |  2 3 |  4 5 
			return false;
	}
	return true;
}




bool   CGameLogic::qidui_laizi(const  HandCards  *p , int laize_a , int  laizi_b , int &haohua_qiduicount  )
{
	if (p->ChangeableCardsLen != 14)
		return false;
	int all = 0;
	haohua_qiduicount = 0;
	map<int, int> all_map; 
	for (int i = 0; i < p->ChangeableCardsLen; i++)
	{
		if (p->ChangeableCards[i] == laizi_b || laize_a == p->ChangeableCards[i])
			all++;
		else
			all_map[p->ChangeableCards[i]]++;
	}
	if (all == 0)
	{
		int ret =   is_qi_dui(p); 
		if (ret > 0)
		{
			haohua_qiduicount = hao_hua_qi_dui(p); 
			return 1;
		}
		return  0;
	}
		
	else
	{
		if (all_map.size() > 7)
			return 0;
		int dui = 0;
		int dui_count = 0;
		for (map<int, int>::iterator it = all_map.begin(); it != all_map.end(); ++it)
		{
			if (it->second == 2 || it->second == 4)
			{
				dui += (it->second / 2); 
				if (it->second == 2)
					dui_count++; 
				haohua_qiduicount += (it->second == 4 ); 
			}			
			else
			{
				if (all > 0)
				{
					dui += (it->second + 1) / 2;
					if (it->second + 1 == 2)
						dui_count++; 
					all--;
					haohua_qiduicount += (  (it->second +1)  == 4 );
				}
				
			}
		}
		if (all >= 2 )
		{
			dui += all / 2;
			while (dui_count && all>= 2 )
			{
				all -= 2;
				if (haohua_qiduicount<=2)
					haohua_qiduicount++;
				dui_count--;
			}
			//haohua_qiduicount += (all/ 4 );    
			//if (all%4>= 2 && haohua_qiduicount<=2 )
				//haohua_qiduicount++; 
 
		}

		return  dui >= 7;
	}
	return 0;

}

int CGameLogic::xiaohua_haohua_ting(const  HandCards *phc)
{
	if (phc->ChangeableCardsLen  == 10)
	{
		if (phc->FixedCardsLen != 1)
			return  0;
		if ((phc->FixedCards[0].state & TYPE_PENG) == 0)
		{			
			return  0;
		}				
		map<int, int>  map_; 
		for (int i = 0; i < phc->ChangeableCardsLen; i++)
		{
			map_[phc->ChangeableCards[i]]++;
		}
		int  dui_all = 0;
		for (map<int, int>::iterator it = map_.begin(); it != map_.end();++it)
		{
			if (it->second == 2)
				dui_all++;
			if (it->second == 4)
				dui_all += 2;						
		}		
		if ( dui_all !=5 )
		{			
			return 0;
		}			
		for (map<int, int>::iterator it = map_.begin(); it != map_.end();++it)
		{
			if (it->second != 2  && it->second != 4)
			{				
				return 0;
			}				
		}
		return 1;
		
	}
	return  0;
	
}


//先碰，然后手上有5对的情况下，摸到碰的那张，可以胡豪华七大对，20花（30）

int CGameLogic::xiaohua_haohua_qi_dui(const  HandCards *pHc ,   _uint8  hu_pai  )
{
	
	if (pHc->ChangeableCardsLen != 11 )
		return false;
	if (hu_pai == INVALID_MJ_CARD)
	{
		if (pHc->FixedCardsLen &&  (pHc->FixedCards[0].state == TYPE_PENG)  )
			hu_pai = pHc->FixedCards[0].CardData;
	}
	map<int, int> map_double; 
	for (int i = 0; i < pHc->ChangeableCardsLen; i++)
	{
		int ct = pHc->ChangeableCards[i];
		map_double[ct]++; 
	}
	int dui = 0;
	int signle_card = -1;
	map<int, int>::iterator  it = map_double.begin();
	for (; it != map_double.end(); ++it)
	{
		if (it->second == 2)
			dui++;
		else if (it->second == 4)
			dui += 2;
		else if (it->second == 1)
			signle_card = it->first;
		
	}
	//log_debug("dui [%d] singlecard [%x]  hand_len[%d]" , dui , signle_card , pHc->ChangeableCardsLen  );
	if (dui != 5)
		return false;
	//log_debug("sing card [%x]   hu_pai[%x ]", signle_card, hu_pai );
	for (int i = 0; i < pHc->FixedCardsLen; i++)
	{
		if ( (pHc->FixedCards[i].state & TYPE_PENG)  &&  (pHc->FixedCards[i].CardData == hu_pai) )
		{
			//log_debug("hupai[%x]  fix_card[%x]" , hu_pai , pHc->FixedCards[i].CardData );
			return true;
		}
		
	}
	return false;

}


int  CGameLogic::hao_hua_qi_dui(const  HandCards  *pHc    )
{

	if (pHc->ChangeableCardsLen != 14)
		return false;

	int all_4_card_count = 0;
	int count = 1;
	for (int i = 0; i < 14; i++)
	{
		_uint8  card_ = pHc->ChangeableCards[i];		
		count = 1;
		for (int j = i + 1; j <  14;j++)
		{
			_uint8  next_card = pHc->ChangeableCards[j];
			//log_debug("card1 [%x]  next_card[%x] count[%d]", card_, next_card, count);

			if (next_card == card_ )
				count++;
			if (count == 4)
			{
				all_4_card_count++;
				break;
			}
			
		}		
	}
	return all_4_card_count;
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

///// //判断是不是将将胡    手牌和吃碰刚牌。有一张不是 258 就不是将将胡 ！！

bool CGameLogic::is_jiang_jiang_hu(const  HandCards  *phand )
{
	if (phand == NULL)
		return false;
	
	for (int i = 0; i < phand->FixedCardsLen;i++)
	{
		_uint8  iTemp = phand->FixedCards[i].CardData;
		if (phand->FixedCards[i].state  & TYPE_CHI)
			return false;  // 将将胡，，修改。 
		if (CardNum(iTemp) != 2 && CardNum(iTemp) != 5 && CardNum(iTemp) != 8)
			return false;
	}
	for (int i = 0; i < phand->ChangeableCardsLen;i++)
	{
		_uint8 iTemp = phand->ChangeableCards[i];
		if (CardNum(iTemp) != 2 && CardNum(iTemp) != 5 && CardNum(iTemp) != 8)
			return false; 
	}
	return true;
	
	
}

//
//bool CGameLogic::IsJiangJiangHu(const HandCards* p, const _uint8 buf[4][4], const _uint8 Jiang)
//{
//	if(p == NULL) return false;
//	for (int i=0; i<p->FixedCardsLen; i++)
//	{
//		if (CardNum(p->FixedCards[i].CardData)!=2 && CardNum(p->FixedCards[i].CardData)!=5 && CardNum(p->FixedCards[i].CardData)!=8)
//			return false;
//	}
//	if (CardNum(Jiang)!=2 && CardNum(Jiang)!=5 && CardNum(Jiang)!=8)
//		return false;
//
//	//int x = 4 - p->FixedCardsLen;
//	for (int i=0; i< 4; i++)
//	{
//		if (CardNum(buf[i][0])!=2 && CardNum(buf[i][0])!=5 && CardNum(buf[i][0])!=8)
//			return false;
//	}
//	return true;
//}

bool CGameLogic::IsLong7Dui(const HandCards* p, _uint8 &cbGenNum)
{
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

//清一色

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

////花色还要一样的

bool CGameLogic::IsQingLong7Dui(HandCards* p)
{
	_uint8 ct = CardType(p->ChangeableCards[0]);

	for (int i=0; i<p->ChangeableCardsLen; i++)
		if (ct != CardType(p->ChangeableCards[i]))
			return false;

	return true;
}

//单调胡牌。。手牌只要一张了。。 这样胡牌的 

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

// 大决 
int  CGameLogic::dajue(const  HandCards  *pHc , int hupai  )
{
	for (int i = 0; i < pHc->FixedCardsLen;i++)
	{
		_uint8  cur_card = pHc->FixedCards[i].CardData;
		if (cur_card == hupai)
		{
			if (pHc->FixedCards[i].state   & TYPE_PENG)
			{
				//log_debug("card [%x]" , cur_card );
				return  1;
			}			
		}		
	}
	return  0;
}


//通天(同一个花色)

int CGameLogic::yi_tiao_long(const  HandCards  *pHc )
{
	int my_card_num[5][10];
	memset(my_card_num, 0, sizeof(my_card_num));
	for (int i = 0; i < pHc->ChangeableCardsLen; i++)
	{
		int suid = CardType(pHc->ChangeableCards[i]);
		int ct   = CardNum(pHc->ChangeableCards[i]);
		if (suid< 0 || suid> 4 || ct < 0 || ct > 9)
			return 0; 

		my_card_num[suid][ct] = 1;
	} 
	
	int all = 0;
	int i_color = -1;
	for (int i = 0; i < 3;i++)
	{
		all = 0; 
		for (int j = 1; j < 10; j++)
		{
			all += my_card_num[i][j];
			if (all == 9)
			{
				i_color = i;
				break;
			}
			
		}
		if (all==9)
			break;
		
	}		
	if (all == 9)
		return  i_color;
	return -1;

}


//对到胡   22 333  类型。 

int CGameLogic::DuiDaoHu( HandCards  *p )
{
	if (p->ChangeableCardsLen != 5)
	{
		return false;
	}
	map<int, int>  all_map;
	for (int i = 0; i < p->ChangeableCardsLen;i++)
	{
		all_map[p->ChangeableCards[i]] = 1;
	}
	return all_map.size() == 2;
	
}


