#ifndef _HOLE_CARDS_H_
#define _HOLE_CARDS_H_

#include <vector>
#include <algorithm>

#include "card.h"

using namespace std;

class HoleCards
{
public:
	HoleCards();
	
	void add_card(Card c);
	
	void sort();
	
	void analysis();
	
	int compare(HoleCards &hc);
	
	void set_card_type(int type);

	void clear() { cards.clear(); oldcards.clear(); card_type=0; card_ratio=0;};
	
	int has_card(int value);

	int has_cow();

	void copy_cards(std::vector<Card> &v);

	void copy_cards(std::vector<int> &v);

	void debug();

	std::vector<Card> cards;
	Card kicker;
	std::vector<Card> oldcards;
	
	int card_type;
	int card_ratio;
};

#endif /* _HOLE_CARDS_H_ */
