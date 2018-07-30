#ifndef __LE_MAJIANG_H__
#define __LE_MAJIANG_H__
/*
   赖子 和非赖子。胡牌接口。 
   by xuke  !!

*/


#include <iostream>
#include <vector> 
#include <map>
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
private:
	std::vector<Card> elems;
};


static const int NROWS = 6;
static const int NCOLS = 10;

typedef Card Matrix[NROWS][NCOLS];



int kezicount(vector<Group> & groups);
bool complete(int row, bool group); 

int AtShunMid(int card); 
int GetShunziCount();

 int Can_hu(int *pArr, int len); 

 int is_all_kezi(); 
int    CanhuCount(vector<int> & all_hand_card , int );
int jilin_hu_if_add_can_be_jia_hu(vector<int> & all_card, bool laizi); 
int    GetCanHu_As_JiangMap(vector<int> & all_hand_card, int end_pos, int  hu_card, bool  &can_be_jiang);  

int  GetMaxKeCount(vector<int>  & all_han_card);


int hu_if_add(vector<int> & all_card_vec , int  end_  ); 


int get_all_ting_card_vec(vector<int> & Ting_Vec, vector<int> & all_card_vec); 
int hu_has_ke(vector<int> & all_han_card);


int le_check_hu( int *arr, int   len   );
int jiang_after_hu();







// 癞子的接口 。

int laizi_le_can_hu(int *arr, int len, int n_lazi);
int  laizi_le_can_hu_jiang258(int *arr, int len, int nlaizi); 
int jilin_check_hu(unsigned char *arr, unsigned char len);
 //int IsBianHu(int hucard);
int zhongfabai_count();
int jilin_hu_can_be_bianhu(vector<int> & all_card, bool laizi);
int Spe_hu(int *pArr, int len, int hupai);
int le_check_hu_jilin(unsigned char *arr, unsigned char len);
int hu_at_Middle(int c);
int hu_at_bianhu(int c);
#endif 