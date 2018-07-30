#ifndef _CARD_H_
#define _CARD_H_

#include <string>
#include <iostream>
#include <cstdio>
#include <vector>
#include <map>
#include <algorithm>

#include "card_type.h"

using namespace std;

/**
 * suit  0  方块  1 梅花   2  红桃    3黑桃
 * 	
0x01, 0x11, 0x21, 0x31, (1, 17, 33, 49)  //A 1
0x02, 0x12, 0x22, 0x32, (2, 18, 34, 50)  //2 2
0x03, 0x13, 0x23, 0x33, (3, 19, 35, 51)  //3 3
0x04, 0x14, 0x24, 0x34, (4, 20, 36, 52)  //4 4
0x05, 0x15, 0x25, 0x35, (5, 21, 37, 53)  //5 5
0x06, 0x16, 0x26, 0x36, (6, 22, 38, 54)  //6 6
0x07, 0x17, 0x27, 0x37, (7, 23, 39, 55)  //7 7
0x08, 0x18, 0x28, 0x38, (8, 24, 40, 56)  //8 8
0x09, 0x19, 0x29, 0x39, (9, 25, 41, 57)  //9 9
0x0A, 0x1A, 0x2A, 0x3A, (10, 26, 42, 58) //10 10
0x0B, 0x1B, 0x2B, 0x3B, (11, 27, 43, 59) //J 11
0x0C, 0x1C, 0x2C, 0x3C, (12, 28, 44, 60) //Q 12
0x0D, 0x1D, 0x2D, 0x3D, (13, 29, 45, 61) //K 13
 *
 */

class Card
{
public:
	typedef enum {
		Ace = 1,
		Two,
		Three,
		Four,
		Five,
		Six,
		Seven,
		Eight,
		Nine,
		Ten,
		Jack,
		Queen,
		King,

		FirstFace = Ace,
		LastFace = King
	} Face;
	
	typedef enum {
		Diamonds = 0,
		Clubs,
		Hearts,
		Spades,
		
		FirstSuit = Diamonds,
		LastSuit = Spades
	} Suit;
	
	int face;
	int suit;
	
	int value;
	int point;
	
	Card();
	Card(int val);
	void set_value(int val);
	
	std::string get_card();
	
	bool operator <  (const Card &c) const{ return (face < c.face); };
	bool operator >  (const Card &c) const { return (face > c.face); };
	bool operator == (const Card &c) const { return (face == c.face); };
	
	static int compare(const Card &a, const Card &b)
	{
		if (a.face > b.face)
		{
			return 1;
		}
		else if (a.face < b.face)
		{
			return -1;
		}
		else if (a.face == b.face)
		{
			if (a.suit > b.suit)
			{
				return 1;
			}
			else if (a.suit < b.suit)
			{
				return -1;
			}	
		}
	
		return 0;
	}

	static bool lesser_callback(const Card &a, const Card &b)
	{
		if (Card::compare(a, b) == -1)
			return true;
		else
			return false;
	}
	
	static bool greater_callback(const Card &a, const Card &b)
	{
		if (Card::compare(a, b) == 1)
			return true;
		else
			return false;
	}
	
	static void sort_by_ascending(std::vector<Card> &v)
	{
		sort(v.begin(), v.end(), Card::lesser_callback);
	}
	
	static void sort_by_descending(std::vector<Card> &v)
	{
		sort(v.begin(), v.end(), Card::greater_callback);
	}
	
	static void dump_cards(std::vector<Card> &v, string str = "cards")
	{
		fprintf(stdout, "[%s]: [[ ", str.c_str());
		for (std::vector<Card>::iterator it = v.begin(); it != v.end(); it++)
			fprintf(stdout, "%s ", it->get_card().c_str());
	
		fprintf(stdout, "]]\n");
	}
	
	static void dump_cards(std::map<int, Card> &m, string str = "cards")
	{
		fprintf(stdout, "[%s]: [[ ", str.c_str());
		for (std::map<int, Card>::iterator it = m.begin(); it != m.end(); it++)
			fprintf(stdout, "%s ", it->second.get_card().c_str());
	
		fprintf(stdout, "]]\n");
	}
};

#endif /* _CARD_H_ */
