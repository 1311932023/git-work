#include "card.h"
#include "card_type.h"
#include "hole_cards.h"

static const char *card_type_str[] = 
{
	(char *)"CARD_TYPE_NONNIU",
	(char *)"CARD_TYPE_NIUDING",
	(char *)"CARD_TYPE_NIUER",
	(char *)"CARD_TYPE_NIUSAN",
	(char *)"CARD_TYPE_NIUSI",
	(char *)"CARD_TYPE_NIUWU",
	(char *)"CARD_TYPE_NIULIU",
	(char *)"CARD_TYPE_NIUQI",
	(char *)"CARD_TYPE_NIUBA",
	(char *)"CARD_TYPE_NIUJIU",
	(char *)"CARD_TYPE_NIUNIU",
	(char *)"CARD_TYPE_JINNIU",
	(char *)"CARD_TYPE_ZHADAN",
	(char *)"CARD_TYPE_WUXIAONIU"
};

HoleCards::HoleCards()
{
	card_type = 0;
	card_ratio = 0;
}

void HoleCards::add_card(Card card)
{
	cards.push_back(card);
}

void HoleCards::sort()
{
	Card::sort_by_descending(cards);
}

void HoleCards::analysis()
{
	copy_cards(oldcards);
	Card::sort_by_descending(cards);
	kicker.set_value(cards[0].value);

	int wxn = cards[0].point + cards[1].point + cards[2].point + cards[3].point + cards[4].point;
	if (wxn <= 10 && cards[0].point < 5) {
		card_type = CARD_TYPE_WUXIAONIU;
		card_ratio = CARD_RATIO_WUXIAONIU;
		return;
	}

	if ((cards[0].face == cards[2].face) && (cards[1].face == cards[3].face)) {
		card_type = CARD_TYPE_ZHADAN;
		card_ratio = CARD_RATIO_ZHADAN;
		return;
	}

	if ((cards[1].face == cards[3].face) && (cards[2].face == cards[4].face)) {
		int temp = cards[0].value;
		cards[0].set_value(cards[1].value);
		cards[1].set_value(cards[2].value);
		cards[2].set_value(cards[3].value);
		cards[3].set_value(cards[4].value);
		cards[4].set_value(temp);
		card_type = CARD_TYPE_ZHADAN;
		card_ratio = CARD_RATIO_ZHADAN;
		return;
	}

	if (cards[4].face > Card::Ten) {
		card_type = CARD_TYPE_JINNIU;
		card_ratio = CARD_RATIO_JINNIU;
		return ;
	}

	bool havenui = false;
	for (unsigned a = 0; a < 3; a++) {
		for (unsigned b = a+1; b < 4; b++) {
			for (unsigned c = b+1; c < 5; c++) {
				if ((cards[a].point + cards[b].point + cards[c].point) % 10 == 0) {
					int t = 0;
					int temps[3] = {0,};
					for (unsigned i = 0; i < cards.size(); i++) {
						if (i != a && i != b && i != c) {
							temps[t] = cards[i].value;
							t++;
						}
					}
					cards[0].set_value(cards[a].value);
					cards[1].set_value(cards[b].value);
					cards[2].set_value(cards[c].value);
					cards[3].set_value(temps[0]);
					cards[4].set_value(temps[1]);
					havenui = true;
					break;
				}
			}
			if (havenui)
				break;
		}
		if (havenui)
			break;
	}

	if (!havenui) {
		card_type = CARD_TYPE_NONNIU;
		card_ratio = CARD_RATIO_LESS;
		return;
	}

	int points = cards[3].point + cards[4].point;
	if (points > 10)
		points = points - 10;
	switch (points) {
	case 1:
		card_type = CARD_TYPE_NIUDING;
		card_ratio = CARD_RATIO_LESS;
		break;
	case 2:
		card_type = CARD_TYPE_NIUER;
		card_ratio = CARD_RATIO_LESS;
		break;
	case 3:
		card_type = CARD_TYPE_NIUSAN;
		card_ratio = CARD_RATIO_LESS;
		break;
	case 4:
		card_type = CARD_TYPE_NIUSI;
		card_ratio = CARD_RATIO_LESS;
		break;
	case 5:
		card_type = CARD_TYPE_NIUWU;
		card_ratio = CARD_RATIO_LESS;
		break;
	case 6:
		card_type = CARD_TYPE_NIULIU;
		card_ratio = CARD_RATIO_LESS;
		break;
	case 7:
		card_type = CARD_TYPE_NIUQI;
		card_ratio = CARD_RATIO_LESS;
		break;
	case 8:
		card_type = CARD_TYPE_NIUBA;
		card_ratio = CARD_RATIO_NIUBA;
		break;
	case 9:
		card_type = CARD_TYPE_NIUJIU;
		card_ratio = CARD_RATIO_NIUJIU;
		break;
	case 10:
		card_type = CARD_TYPE_NIUNIU;
		card_ratio = CARD_RATIO_NIUNIU;
		break;
	default:
		card_type = CARD_TYPE_NONNIU;
		card_ratio = CARD_RATIO_LESS;
		break;
	}

	return;
}

int HoleCards::compare(HoleCards &hc)
{
	if (card_type > hc.card_type)
		return 1;
	else if (card_type < hc.card_type)
		return -1;

	if (kicker.face > hc.kicker.face)
		return 1;
	else if (kicker.face < hc.kicker.face)
		return -1;

	if (kicker.suit > hc.kicker.suit)
		return 1;
	else if (kicker.suit < hc.kicker.suit)
		return -1;

	return 0;
}

void HoleCards::set_card_type(int type)
{
	card_type = type;
}

int HoleCards::has_card(int value)
{
	for (unsigned int i = 0; i < cards.size(); i++){
		if (value == cards[i].value)
			return 0;
	}
	return -1;
}

int HoleCards::has_cow()
{
	if (oldcards.size() != 5)
		return -1;
	if ((oldcards[0].point + oldcards[1].point + oldcards[2].point) % 10 != 0)
		return -1;
	return 0;
}

void HoleCards::copy_cards(std::vector<Card> &v)
{
	v = cards;
}

void HoleCards::copy_cards(std::vector<int> &v)
{
	for (unsigned int i = 0; i < cards.size(); i++)
	{
		v.push_back(cards[i].value);
	}
}

void HoleCards::debug()
{
	analysis();
	Card::dump_cards(cards);
	printf("%s\n", card_type_str[card_type]);
}
