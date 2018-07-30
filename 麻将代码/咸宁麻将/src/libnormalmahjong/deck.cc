#include <algorithm>
#include "deck.h"

static int card_arr[] = {
0x01, 0x11, 0x21, 0x31,		//A 14 
0x02, 0x12, 0x22, 0x32,		//2 15
0x03, 0x13, 0x23, 0x33,		//3 3
0x04, 0x14, 0x24, 0x34,		//4 4
0x05, 0x15, 0x25, 0x35,		//5 5
0x06, 0x16, 0x26, 0x36,		//6 6
0x07, 0x17, 0x27, 0x37,		//7 7
0x08, 0x18, 0x28, 0x38,		//8 8
0x09, 0x19, 0x29, 0x39,		//9 9
0x0A, 0x1A, 0x2A, 0x3A,		//10 10
0x0B, 0x1B, 0x2B, 0x3B,		//J 11
0x0C, 0x1C, 0x2C, 0x3C,		//Q 12
0x0D, 0x1D, 0x2D, 0x3D,		//K 13
};

void Deck::fill()
{
	cards.clear();
	
	for (int i = 0; i < 52; i++) {
		Card c(card_arr[i]);
		push(c);
	}
}

void Deck::empty()
{
	cards.clear();
}

int Deck::count() const
{
	return cards.size();
}

bool Deck::push(Card card)
{
	cards.push_back(card);
	return true;
}

bool Deck::pop(Card &card)
{
	if (!count())
		return false;
	
	card = cards.back();
	cards.pop_back();
	return true;
}

bool Deck::shuffle(int seed)
{
	//srand(time(NULL) + seed);
	random_shuffle(cards.begin(), cards.end());
	return true;
}

void Deck::get_hole_cards(HoleCards &holecards)
{
	Card card;
	
	holecards.clear();
	for (int i = 0; i < 5; i++)
	{
		pop(card);
		holecards.add_card(card);
	}
}

void Deck::change_hole_cards(int pos, HoleCards &holecards)
{
	Card card;
	holecards.cards.erase(holecards.cards.begin() + pos);
	pop(card);
	holecards.cards.push_back(card);
	holecards.analysis();
}

void Deck::debug()
{
	Card::dump_cards(cards);
}

void Deck::make_cards( HoleCards &holecards, int card_type )
{
	switch( card_type )	
	{
		case  CARD_TYPE_WUXIAONIU:
			make_wuxiaoniu( holecards );
			break;
		case CARD_TYPE_JINNIU:
			make_wuhuaniu( holecards );
			break;
		case CARD_TYPE_ZHADAN:
			make_zhadan( holecards );
			break;
		default:
			get_hole_cards( holecards );
			return;
	}
	if( 5 != holecards.cards.size() )
	{
		for( std::vector<Card>::iterator iter = holecards.cards.begin(); iter != holecards.cards.end(); iter++ )
		{
			cards.push_back( *iter );
		}
		holecards.cards.clear();
		random_shuffle(cards.begin(), cards.end());
		get_hole_cards( holecards );
	}
	return;
}

void Deck::make_wuxiaoniu( HoleCards &holecards )
{
	int sum = 0;
	int card_face = 4;
	int face = 0;
	for( int i = 0; i < 5; i++ )
	{
		if( i < 4 )
		{
			if( 0 != i )
			{
				card_face = 10 - sum -( 5 - i);
			}
			face = ( rand() % card_face ) + 1;
		}
		else
		{
			face = 10 - sum;
		}
		std::vector<Card>::iterator iter = std::find_if( cards.begin(), cards.end(), Equal_Card_Face( face ) );
		if( iter != cards.end() )
		{
			holecards.add_card( *iter );
			cards.erase( iter );
			sum += face;
		}
	}
}

void Deck::make_wuhuaniu( HoleCards &holecards )
{
	for( int i = 0; i < 5; i++ )
	{
		int face = rand()%4 + 10;
		std::vector<Card>::iterator iter = std::find_if( cards.begin(), cards.end(), Equal_Card_Face( face ) );
		if( iter != cards.end() )
		{
			holecards.add_card( *iter );
			cards.erase( iter );
		}
	}
}

void Deck::make_zhadan( HoleCards &holecards )
{
	int count = 0;
	int face = 0;
	for( int i = 0; i < 13; i++ )
	{
		face = rand() % 13 + 1;
		count = std::count_if( cards.begin(), cards.end(), Equal_Card_Face( face ));
		if( 4 == count )
		{
			for( int j = 0; j < 4; j++ )
			{
				std::vector<Card>::iterator iter = std::find_if( cards.begin(), cards.end(), Equal_Card_Face( face ) );
				if( iter != cards.end() )
				{
					holecards.add_card( *iter );
					cards.erase( iter );
				}
			}
			Card card = cards.back();
			holecards.add_card( card );
			cards.pop_back();
			break;
		}
	}
}
