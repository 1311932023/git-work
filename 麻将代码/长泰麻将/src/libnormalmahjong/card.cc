#include "card.h"

static char face_symbols[] = {
	'A', '2', '3', '4', '5', '6', '7', '8', '9', 'T', 'J', 'Q', 'K'
};

static char suit_symbols[] = {
	'd', 'c', 'h', 's'
};

Card::Card()
{
	face = suit = value = point = 0;
}

Card::Card(int val)
{
//	printf("----val: %d\n", val);
	value = val;
//	printf("value: %d\n", value);
	
	face = value & 0xF;
	suit = value >> 4;
	if (face > 10)
		point = 10;
	else
		point = face;
//	printf("Face[%d] Suit[%d] Point[%d]\n", face, suit, point);
}

void Card::set_value(int val)
{
	value = val;
	
	face = value & 0xF;
	suit = value >> 4;
	if (face > 10)
		point = 10;
	else
		point = face;
//	printf("Face[%d] Suit[%d] Point[%d]\n", face, suit, point);
}

string Card::get_card()
{
	string card;
	
	/*
	char buf[32];
	snprintf(buf, sizeof(buf), "%d-%d", face, suit);
	card.append(buf);
	*/
	
	card.append(1, face_symbols[face - 1]);
	card.append(1, suit_symbols[suit]);
	
	return card;
}
