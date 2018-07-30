#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <assert.h>
#include "common.h"
#include "game_logic.h"
#include <unistd.h>
#include <sys/time.h>


static _uint8 x[] = { 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, };


int test02()
{
	HandCards hc;
	CGameLogic cgl;
	cgl.AddCard(&hc, 0x1);
	cgl.AddCard(&hc, 0x1);
	cgl.AddCard(&hc, 0x2);
	cgl.AddCard(&hc, 0x2);
	cgl.AddCard(&hc, 0x2);
	cgl.AddCard(&hc, 0x5);
	cgl.AddCard(&hc, 0x5);
	_uint8 CardDataNum[MJ_TYPE_COMMON][10] = { { 0 }, { 0 }, { 0 }, { 0 }, { 0 } };
	cgl.GetCardCount(&hc, CardDataNum);
	if (cgl.L_IsQiDui(&hc, INVALID_MJ_CARD, CardDataNum, 1))
	{
		printf("is qidui \n");
	}

	return 0;
}
int test03()
{
	HandCards phc;
	CGameLogic cgl;
	cgl.AddCard(&phc, 0x05);
	cgl.AddCard(&phc, 0x06);
	cgl.AddCard(&phc, 0x07);

	cgl.AddCard(&phc, 0x31);
	cgl.AddCard(&phc, 0x31);
	cgl.AddCard(&phc, 0x31);
	
	cgl.AddCard(&phc, 0x01);

	if (cgl.FormatCards_Laizi_3_Group(phc.ChangeableCards, phc.ChangeableCardsLen, 1))
	{
		return -1;
	}
	return 0;
}


void test_0()
{
	HandCards hc;
	CGameLogic cgl;
	hc.ChangeableCards[0] = 0x1;
	hc.ChangeableCards[1] = 0x1;
	hc.ChangeableCards[2] = 0x1;
	hc.ChangeableCards[3] = 0x2;
	hc.ChangeableCards[4] = 0x3;
	hc.ChangeableCards[5] = 0x4;
	hc.ChangeableCards[6] = 0x5;
	hc.ChangeableCards[7] = 0x6;
	hc.ChangeableCards[8] = 0x7;

	hc.ChangeableCards[9] = 0x8;
	hc.ChangeableCards[10] = 0x9;
	hc.ChangeableCards[11] = 0x9;
	hc.ChangeableCards[12] = 0x9;
	hc.ChangeableCardsLen = 13;
	hc.FixedCardsLen = 0;

	timeval begin, end;
	bool ok;
	gettimeofday(&begin, 0);
	MJ_win_pattern pat;
	for (int i = 0; i < 10000; i++)
	{
		for (_uint8 c = 0; c < sizeof(x); c++)
		{
			cgl.AddCard(&hc, x[c]);
			ok = cgl.FormatCards(&hc, pat);
			cgl.ChuPai(&hc, x[c]);
		}
	}
	gettimeofday(&end, 0);
	long t = (end.tv_sec - begin.tv_sec) * 1000000 + (end.tv_usec - begin.tv_usec);
	printf("%s\ttimer:%ld.%ld\n", ok ? "succ" : "failed", t / 1000000, t % 1000000);
}

void test_1(int lai_sum)
{
	HandCards hc;
	CGameLogic cgl;
	hc.ChangeableCards[0] = 0x1;
	hc.ChangeableCards[1] = 0x1;
	hc.ChangeableCards[2] = 0x1;
	hc.ChangeableCards[3] = 0x2;
	hc.ChangeableCards[4] = 0x3;
	hc.ChangeableCards[5] = 0x4;
	hc.ChangeableCards[6] = 0x5;
	hc.ChangeableCards[7] = 0x6;
	hc.ChangeableCards[8] = 0x7;

	hc.ChangeableCards[9] = 0x8;
	hc.ChangeableCards[10] = 0x9;
	hc.ChangeableCards[11] = 0x9;
	hc.ChangeableCards[12] = 0x9;
	hc.ChangeableCardsLen = 13;
	hc.FixedCardsLen = 0;
	
	for( int i= 0 ; i < lai_sum ; ++i  )
	{
		hc.ChangeableCardsLen--;
		hc.ChangeableCards[12 - 1 - i] = INVALID_MJ_CARD;
	}


	timeval begin, end;
	bool ok;
	gettimeofday(&begin, 0);
	for (int i = 0;	i < 10000 ; i++)
	{
		for (_uint8 c = 0; c < sizeof(x); c++)
		{
			cgl.AddCard(&hc, x[c]);
			MJ_win_pattern pat;
			ok = cgl.FormatCards_LaiZi(&hc, pat, lai_sum);
			cgl.ChuPai(&hc, x[c]);
		}
	}
	gettimeofday(&end, 0);
	long t = (end.tv_sec - begin.tv_sec) * 1000000 + (end.tv_usec - begin.tv_usec);
	printf("%s\ttimer:%ld.%ld\n", ok ? "succ" : "failed", t / 1000000, t % 1000000);

}



int main(int argc, char *argv[])
{	
	
// 	test_0();
//  	for ( int i = 0 ; i < 4 ; ++i  )
//  	{
//  		test_1(i);
//  	}

	HandCards hc;	
	CGameLogic cgl;
	hc.ChangeableCards[0] = 0x1;
	hc.ChangeableCards[1] = 0x1;	
	hc.ChangeableCards[2] = 0x1;
	hc.ChangeableCards[3] = 0x2;
	hc.ChangeableCards[4] = 0x3;
	hc.ChangeableCards[5] = 0x4;
	hc.ChangeableCards[6] = 0x5;
	hc.ChangeableCards[7] = 0x6;
	hc.ChangeableCards[8] = 0x7;

	hc.ChangeableCards[9] = 0x8;
	hc.ChangeableCards[10] = 0x9;
	hc.ChangeableCards[11] = 0x9;
	hc.ChangeableCards[12] = 0x9;
	hc.ChangeableCardsLen = 13;

	hc.FixedCardsLen = 0;

	bool ok;
	timeval begin, end;

	gettimeofday(&begin, 0);
	for (int i = 0;
#ifdef DEBUG		
		i < 1;
#else
		i < 10000;
#endif
		i++)
	{			
		for (_uint8 c = 0; c < sizeof(x); c++)
		{
			cgl.AddCard(&hc, x[c]);		
			ok = cgl.FormatCards_Laizi(hc.ChangeableCards, hc.ChangeableCardsLen, 1);	
			cgl.ChuPai(&hc, x[c]);
		}
	}
	gettimeofday(&end, 0);	
	long t = (end.tv_sec - begin.tv_sec) * 1000000 + (end.tv_usec - begin.tv_usec);
	printf("%s\ttimer:%ld.%ld\n", ok ? "succ" : "failed", t / 1000000, t % 1000000);
	return 0;
}

