#include "game_logic.h"
#include <set>

inline bool SpliteKezi(int pos, CardNode_set& h, CardNode_type_set& r)
{
	if (pos >= h.Size())
		return false;
	CardNode_count& c = h[pos];
	if (c.card == INVALID_MJ_CARD)
		return false;

	switch (c.count)
	{
	case 3:
		r.Add(CardNode_type(c.card, 0, E_CARDNODE_TYPE_KEZI));
		c.count = 0;
		return true;
	case 4:
		r.Add(CardNode_type(c.card, 0, E_CARDNODE_TYPE_KEZI));
		c.count = 1;
		return true;
	case 2:
		r.Add(CardNode_type(c.card, 1, E_CARDNODE_TYPE_KEZI));
		c.count = 0;
		return true;
	}
	return false;
}
inline bool SpliteShunzi(int pos, CardNode_set& h, CardNode_type_set& r)
{
	if (pos >= h.Size())
		return false;
	CardNode_count& c = h[pos];
	if (c.count == 0)
		return false;
	if (CardType(c.card) > MJ_TYPE_BING)
		return false;

	int next = pos + 1;
	int next2 = pos + 2;
	if (next2 < h.Size())
	{
		CardNode_count& c1 = h[next];
		CardNode_count& c2 = h[next2];
		if (c2.card - 2 == c.card)
		{			
			if (c1.count > 0 && c2.count > 0)
			{
				r.Add(CardNode_type(c.card, 0, E_CARDNODE_TYPE_SHUNZI));
				c.count--;
				c1.count--;
				c2.count--;
				return true;
			}			
		}
	}		
	return false;
}
inline bool SpliteShunzi2(int pos, CardNode_set& h, CardNode_type_set& r)
{
	if (pos >= h.Size())
		return false;
	CardNode_count& c = h[pos];
	if (c.count == 0)
		return false;
	if (CardType(c.card) > MJ_TYPE_BING)
		return false;

	int next = pos + 1;		
	if (next < h.Size())
	{
		bool bian = CardNum(c.card) == 1 || CardNum(c.card) == 8;
		CardNode_count& c1 = h[next];
		if (c1.card - 1 == c.card)
		{
			if (c1.count > 0)
			{
				r.Add(CardNode_type(c.card, 1, bian ? E_CARDNODE_TYPE_BIANZHANG : E_CARDNODE_TYPE_SHUNZI));
				c.count--;
				c1.count--;
				return true;
			}
		}
		else if (c1.card - 2 == c.card)
		{
			if (c1.count > 0)
			{
				r.Add(CardNode_type(c.card, 1, E_CARDNODE_TYPE_KAZHANG));
				c.count--;
				c1.count--;
				return true;
			}
		}
	}
	return false;
}
inline bool CheckDeep(int deep, const CardNode_type_set& r)
{
	for (int i =0; i < r.Size(); i++)
	{
		if ((deep -= r[i].res) < 0)
			return false;
	}
	return true;
}
inline bool CheckJiang(int deep, const CardNode_type_set& r)
{
	for (int i = 0; i < r.Size(); i++)
	{
		if (r[i].type == E_CARDNODE_TYPE_DAN)
			return true;
		else if (r[i].type == E_CARDNODE_TYPE_KEZI && r[i].res == 1)
			return true;
	}
	return deep > 1;	
}
inline bool SpliteDanzhang(const CardNode_set&h, CardNode_type_set& r)
{
	for (int i = 0; i < h.Size(); i++)
	{
		const CardNode_count& c = h[i];
		if (c.card == INVALID_MJ_CARD) continue;		
		if (c.count == 0) continue;
		if (c.count == 1)
			r.Add(CardNode_type(c.card, 2, E_CARDNODE_TYPE_DAN));
		else
			return false;
	}
	return true;
}
inline void Copy(const CardNode_set& org, CardNode_set& dst)
{
	dst.Clear();
	for (int i = 0; i < org.Size(); i++)
		if (org[i].count > 0)
			dst.Add(org[i]);
}
inline _uint8 GetPos(const CardNode_set& s, _uint8 card)
{
	for (int i = 0; i < s.Size(); i++)
		if (card == s[i].card)
			return i;
	throw("can't find card!");
}
bool SplitKan(int pos, int deep, const CardNode_set& h, CardNode_package& p)
{
	if (pos >= h.Size() - 1)
	{
	__END_SUCCESS__:
		CardNode_type_set * r = &p.Last();
		if (r->IsNull())
			return false;
		p.Add(*r);		
		if (!(SpliteDanzhang(h, *r) && CheckDeep(deep, *r) && CheckJiang(deep, *r)))
		{
			*r = p.Last();
			p.RemoveLast();
		}
		return true;
	}

	if (h[pos].count == 0)
		return SplitKan(pos + 1, deep, h, p);

	CardNode_set tmp;	
	Copy(h, tmp);		
	pos = GetPos(tmp, h[pos].card);

	CardNode_type_set* r = &p.Last();
	if (SpliteKezi(pos, tmp, *r))
	{
		if (SplitKan(pos + 1, deep, tmp, p))
		{
			r = &p.Last();
			r->RemoveLast();	
		}
		Copy(h, tmp);	
	}	
	if (SpliteShunzi(pos, tmp, *r))
	{			
		if (SplitKan(pos, deep, tmp, p))
		{
			r = &p.Last();		
			r->RemoveLast();			
		}
		Copy(h, tmp);
	}
	if (SpliteShunzi2(pos, tmp, *r))
	{
		if (SplitKan(pos, deep, tmp, p))
		{
			r = &p.Last();
			r->RemoveLast();
		}
		Copy(h, tmp);		
	}
	if (CardType(tmp[pos].card) != CardType(tmp[pos + 1].card))
	{
		if (p.Size() == 1) {}
		else
			goto __END_SUCCESS__;
	}
	return SplitKan(pos + 1, deep, tmp, p);
}

void CGameLogic::AnalysisCardCount(int deep, const CardNode_set& h, CardNode_package& package)
{
	try
	{
		CardNode_set s;
		Copy(h, s);
		s.Add(CardNode_count(INVALID_MJ_CARD, 1));

		package.Add(CardNode_type_set());
		_uint8 type = CardType(s[0].card);
		for (int i = 0; i < s.Size(); i++)
		{
			if (CardType(s[i].card) != type){
				if (package.Size() == 1)
					type = CardType(s[i].card);
				else
					break;
			}
			SplitKan(i, deep + 1, s, package);
		}
		package.RemoveLast();
	}
	catch (...)
	{		
	}	
}
int CGameLogic::CardTypeClone(const HandCards& src, HandCards& dst, _uint8 card)
{
	dst.ChangeableCardsLen = 0;
	dst.FixedCardsLen = 0;
	_uint8 type = CardType(card);
	for (int i = 0; i < src.ChangeableCardsLen; i++)
	{
		if (type == CardType(src.ChangeableCards[i]))
		{
			dst.ChangeableCards[dst.ChangeableCardsLen++] = src.ChangeableCards[i];
		}
	}
	return dst.ChangeableCardsLen;
}