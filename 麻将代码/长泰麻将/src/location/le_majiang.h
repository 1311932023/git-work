#ifndef __LE_MAJIANG_H__
#define __LE_MAJIANG_H__
/*
   赖子 和非赖子。胡牌接口。 
   by xuke  !!

*/


#include <iostream>
#include <vector> 
#include <stdio.h>
#include <string.h>
using namespace std;


typedef  int  Card;


inline int get_suits(Card c)
{
	return c >> 4;
}

inline int get_face(Card c)
{
	return c & 0x0F;
}

inline Card make_card(int row, int col)
{
	return row * 0x10 + col;
}

class Group
{
public:
	Group(Card j1, Card j2)
	{
		elems.push_back(j1);
		elems.push_back(j2);
	}

	Group(Card c1, Card c2, Card c3)
	{
		elems.push_back(c1);
		elems.push_back(c2);
		elems.push_back(c3);
		//printf("c1 c2 c3 %x %x %x  \n ", c1, c2, c3);
	}

	size_t size() const
	{
		return elems.size();
	}

	Card first() const
	{
		return elems[0];
	}

	Card second() const
	{
		return elems[1]; 
	}

	Card third() const
	{
		return elems[2];
	}

	bool isKe() const
	{
		return elems.size() == 3 && elems.front() == elems.back();
	}

	bool isShun() const
	{
		return elems.size() == 3 && elems.front() != elems.back();
	}

	bool isJiang() const
	{
		return elems.size() == 2 && elems.front() == elems.back();
	}
public:
	std::vector<Card> elems;
};


static const int NROWS = 6;
static const int NCOLS = 10;

typedef Card Matrix[NROWS][NCOLS];




bool can_hu(bool group, int start_nol = 1);



// 癞子的接口 。


int laizi_chibi_check_hu( unsigned  char  *arr,  unsigned  char  len, int n_lazi);

#endif 

