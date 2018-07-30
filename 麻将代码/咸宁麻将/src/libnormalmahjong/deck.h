#ifndef _DECK_H_
#define _DECK_H_

#include <vector>

#include "card.h"
#include "hole_cards.h"

using namespace std;

struct Equal_Card_Face
{
	int face;
	Equal_Card_Face( int _face ): face( _face ){}
	bool operator()( const Card &card )
	{
		return card.face == face;
	}
};

class Deck
{
public:
	void fill();
	void empty();
	int count() const;
	
	bool push(Card card);
	bool pop(Card &card);
	bool shuffle(int seed);
	
	void get_hole_cards(HoleCards &holecards);
	void change_hole_cards(int pos, HoleCards &holecards);
	
	void debug();

	void make_cards( HoleCards &holecards, int card_type );
	void make_wuxiaoniu( HoleCards &holecards );
	void make_wuhuaniu( HoleCards &holecards );
	void make_zhadan( HoleCards &holecards );
	
public:
	vector<Card> cards;
};

#endif /* _DECK_H_ */
