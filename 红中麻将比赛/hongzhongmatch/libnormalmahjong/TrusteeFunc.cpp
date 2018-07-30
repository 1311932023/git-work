#include "TrusteeFunc.h"

_uint8 NextCard(_uint8 bCard)
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
_uint8 PrevCard(_uint8 bCard)
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
_uint8 count(CardNode_set& hc, _uint8 card)
{
	if (card == INVALID_MJ_CARD)
		return 0;
	for (int i = 0; i < hc.Size(); i++)
		if (hc[i].card == card)
			return hc[i].count;
	return 0;
}
bool has_any_type(const CardNode_type_set& s, _uint8 type)
{
	for (int i =0 ; i < s.Size(); i++)
	{
		if (s[i].type == type)
			return true;
	}
	return false;
}
int ScoreNode_filter0(CardNode_set& hc, const CardNode_package& package, MJ_BUFFER<ScoreNode, CARDNODE_SIZE_MAX>& node)
{
// 	static const float dan_score = -1.0f; //res = 3
// 	static const float bian_ka_score = -0.5f;//res = 4
// 	static const float shun_score = 0.0f;
// 	static const float shun_res_score = -0.5f;//res = 8
// 	static const float ke_score = 0.0f;
// 	static const float ke_res_score = -0.5f;//res = 1
	MJ_BUFFER<ScoreNode, CARDNODE_SIZE_MAX> tmp;
	int res_count = 255;
	for (int pos = 0; pos < package.Size(); pos++)
	{
		const CardNode_type_set& s = package[pos];
		if (s.IsNull())
			continue;
				
		ScoreNode n(0,0,0,0,pos);		
		for (int i = 0; i < s.Size(); i++)
		{
			if (s[i].type == E_CARDNODE_TYPE_DAN)
			{
				n.dan++;
			}
			else if (s[i].type == E_CARDNODE_TYPE_KEZI)
			{
				n.kezi++;
			}
			else if (s[i].type == E_CARDNODE_TYPE_BIANZHANG)
			{
				n.bianka++;
			}
			else if (s[i].type == E_CARDNODE_TYPE_KAZHANG)
			{
				n.bianka++;
			}
			else if (s[i].type == E_CARDNODE_TYPE_SHUNZI)
			{

			}
			if (s[i].res != 0)
			{
				n.res += s[i].res;
			}
		}
		if (n.dan < res_count)
		{
			res_count = n.dan;
		}
		tmp.Add(n);
	}


	for (int i = 0; i < tmp.Size(); i++)
	{
		if (tmp[i].dan < (res_count + CARDNODE_FILTER_PARAM_1))
		{
			node.Add(tmp[i]);
		}
	}
	return 0;
}

int ScoreNode_filter1(CardNode_set& hc, const CardNode_package& package, MJ_BUFFER<ScoreNode, CARDNODE_SIZE_MAX>& node)
{
	static float card_val[] = { 0.0f, 0.01f, 0.02f, 0.03f, 0.03f, 0.03f, 0.03f, 0.03f, 0.02f, 0.01f };
	for (int p = 0; p < node.Size(); p++)
	{
		ScoreNode& n = node[p];
		const CardNode_type_set& s = package[n.pos];
		if (s.NotNull())
		{
			for (int i = 0; i < s.Size(); i++)
			{
				if (s[i].type == E_CARDNODE_TYPE_DAN)
				{
					_uint8 card = s[i].card;
					_uint8 next = ::NextCard(card);
					_uint8 next2 = ::NextCard(next);
					_uint8 prev = ::PrevCard(card);
					_uint8 prev2 = ::PrevCard(prev);
					
					ScoreNode::Score last_score;
					last_score.card = s[i].card;
					last_score.score = count(hc, next) * 0.5f
						+ count(hc, next2) * 0.2f
						+ count(hc, prev) * 0.5f
						+ count(hc, prev2) * 0.2f
						+ card_val[CardNum(last_score.card)];
					if (!has_any_type(s, E_CARDNODE_TYPE_KEZI))
						last_score.score += 1.1f;
										
					n.outcard.insert(last_score);
				}				
				else if (s[i].type == E_CARDNODE_TYPE_BIANZHANG)
				{
					_uint8 card = s[i].card;
					if (CardNum(card) == 1)
					{
						_uint8 next = ::NextCard(card);
						_uint8 next2 = ::NextCard(next);
						ScoreNode::Score last_score;
						last_score.card = card;
						last_score.score = count(hc, card) * 0.49f
							+ count(hc, next) * 0.5f
							+ count(hc, next2) * 0.48f;
						n.outcard.insert(last_score);
						last_score.card = next;
						last_score.score = count(hc, card) * 0.5f
							+ count(hc, next) * 0.6f
							+ count(hc, next2) * 0.48f;
						n.outcard.insert(last_score);
					}
					else if (CardNum(card) == 8)
					{
						_uint8 next = ::NextCard(card);						
						_uint8 prev = ::PrevCard(card);
						ScoreNode::Score last_score;
						last_score.card = card;
						last_score.score = count(hc, card) * 0.5f
							+ count(hc, next)* 0.51f
							+ count(hc, prev) * 0.6f;
						n.outcard.insert(last_score);
						last_score.card = next;
						last_score.score = count(hc, card) * 0.5f
							+ count(hc, next) * 0.49f
							+ count(hc, prev) * 0.48f;
						n.outcard.insert(last_score);
					}
				}
				else if (s[i].type == E_CARDNODE_TYPE_KAZHANG)
				{
					_uint8 card = s[i].card;
					_uint8 next = ::NextCard(card);
					_uint8 next2 = ::NextCard(next);
					_uint8 prev = ::PrevCard(card);	
					_uint8 prev2 = ::PrevCard(prev);
					ScoreNode::Score last_score;
					last_score.card = card;
					last_score.score = count(hc, card) * 0.5f
						+ count(hc, next)  * 0.5f
						+ count(hc, next2) * 0.2f
						+ count(hc, prev) * 0.5f
						+ count(hc, prev2) * 0.2f
						+ card_val[CardNum(last_score.card)];
					n.outcard.insert(last_score);

					_uint8 next3 = ::NextCard(next2);
					_uint8 next4 = ::NextCard(next3);
					last_score.card = next2;
					last_score.score = count(hc, card) * 0.2f
						+ count(hc, next) * 0.5f
						+ count(hc, next2) * 0.5f
						+ count(hc, next3) * 0.5f
						+ count(hc, next4) * 0.2f
						+ card_val[CardNum(last_score.card)];
					n.outcard.insert(last_score);
				}
				else if (s[i].type == E_CARDNODE_TYPE_KEZI && s[i].res == 1)
				{
					if (n.kezi > 1)
					{
						_uint8 card = s[i].card;
						_uint8 next = ::NextCard(card);
						_uint8 next2 = ::NextCard(next);
						_uint8 prev = ::PrevCard(card);
						_uint8 prev2 = ::PrevCard(prev);

						ScoreNode::Score last_score;
						last_score.card = s[i].card;
						last_score.score = count(hc, next) * 1.0f
							+ count(hc, next2) * 0.5f
							+ count(hc, prev) * 1.0f
							+ count(hc, prev2) * 0.5f
							+ count(hc, card) * 1.0f
							+ card_val[CardNum(last_score.card)];

						n.outcard.insert(last_score);
					}
				}
			}
		}
	}
	return 0;
}
int ScoreNode_filter2(CardNode_set& hc, const CardNode_package& package, MJ_BUFFER<ScoreNode, CARDNODE_SIZE_MAX>& node)
{

	int res_min = 10000000;
	int bianka_min = 10000000;
	for (int i = 0; i < node.Size(); i++)
	{
		if (node[i].res < res_min)
			res_min = node[i].res;
		if (node[i].bianka < bianka_min)
			bianka_min = node[i].bianka;
	}

	MJ_BUFFER<ScoreNode, CARDNODE_SIZE_MAX> tmp;
	for (int i = 0; i < node.Size(); i++)
	{
		if (node[i].bianka == bianka_min && node[i].res == res_min)
			tmp.Add(node[i]);
	}

	for (int i = 0; i < node.Size(); i++)
	{
		if ((node[i].bianka == bianka_min + 1) || (node[i].res == res_min + 1))
			tmp.Add(node[i]);
	}
	if (tmp.IsNull())
	{
		for (int i = 0; i < node.Size(); i++)
		{
			if (node[i].res == res_min)
			{
				tmp.Add(node[i]);
			}
		}
	}
	node = tmp;
	return 0;
}
int ScoreNode_filter3(CardNode_set& hc, const CardNode_package& package, MJ_BUFFER<ScoreNode, CARDNODE_SIZE_MAX>& node)
{
	MJ_BUFFER<ScoreNode, CARDNODE_SIZE_MAX> tmp;
	float fscore = 999999;
	for (int i = 0; i < node.Size(); i++)
	{
		if (node[i].outcard.empty())
			continue;
		const ScoreNode::Score& score = *(node[i].outcard.begin());
		if (score.score < fscore)
		{
			tmp.Clear();
			tmp.Add(node[i]);
			fscore = score.score;
		}
	}
	if (tmp.NotNull())
	{
		node = tmp;
		return 0;
	}
	return 0;
}
void Add(CardNode_set& hc, _uint8 card, int count)
{
	for (int i = 0; i < hc.Size(); i++)
	{
		if (hc[i].card == card)
		{
			hc[i].count += count;
			break;
		}
	}
}
