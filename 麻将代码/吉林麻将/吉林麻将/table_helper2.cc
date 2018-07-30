#include "table_helper.h"
#include "common/log.h"
#include <sys/time.h>
//湖南麻将-长沙麻将
//0x01 1万
//0x11 1条
//0x21 1同


//0x31 东
//0x41 中

void CCardPool::FixCard(int index)
{
	const _uint8 pool[][POOL_CARD_MAX] =
	{	
		{	//起手杠		
								
			0x24,0x25,0x26, 0x1,0x1,0x1, 0x2,0x2,0x2 ,  0x5 ,0x5    , 0x17,0x18 , 
			0x16,0x16,0x16, 0x1,0x2,0x3,  0x8 ,0x18,0x19,   0x21,0x22,0x23, 0x28 ,			
			0x11,0x11,0x11,0x12,0x12,0x12,0x13,0x13,0x13, 0x16,0x16,0x16, 0x16 , 			
			0x1    , 0x1, 0x1 , 0x1  , 0x1, 0x11, 0x21, 0x21, 0x21, 0x1, 0x21, 0x22, 0x13,     // 步步高 ---------------->  444 
			0x12, 0x12, 0x22, 0x22, 0x15, 0x11, 0x12, 0x13, 0x14, 0x15, 0x11, 0x12, 0x13,     // 金童玉女   ///===========8 									
			0x1,0x2,0x3,    0x4,0x5,0x6,  0x7,0x8, 0x15,   0x15,0x16 , 0x21 , 0x21 ,
			0x15  , 0x15 ,  0x15  ,0x15, 0x15, 0x15, 0x2, 0x15, 0x15, 0x15, 0x15, 0x15, 0x15 ,
			0x1, 0x1, 0x1, 0x2, 0x2, 0x2, 0x3, 0x4, 0x4, 0x4, 0x8, 0x16, 0x16,       //====== 4
				
			0x16, 0x16, 0x16, 0x16 , 0x19 ,  0x15, 0x15, 0x15, 0x16, 0x9, 0x18, 0x19, 0x11,    // 66 顺  						
				
			0x11, 0x12, 0x13, 0x14, 0x15, 0x11, 0x12,
		},
		{ 
														  	
			
			//0x31,0x32,0x33,0x34,0x41,0x42,0x43,  0x1,0x4, 0x9 ,  0x11,0x14, 0x19,  
			//0x1,0x2,0x3, 0x4, 0x5,0x6 , 0x7 , 0x8, 0x9  , 0x11,0x11,0x11,  0x19 ,
			0x11,0x11,0x11,0x11,   0x12,0x12,0x12,0x12,   0x13,0x13,0x13,0x13 ,  0x19 ,

			
			0x31,0x32,0x33,0x34,0x41,0x42,0x43 ,   0x1,0x4,0x9 ,0x11, 0x14,  0x19,  

			0x11,0x11,0x11,0x11,0x11 ,0x9 , 0x11,0x11,0x12,0x12, 0x13,0x13 , 0x19 ,			
			0x21  , 0x11,0x11,0x11, 0x12,0x12,0x12,0x12, 0x13,0x13,0x14,0x14 , 0x19 ,

			//0x15, 0x15 , 0x6 , 0x5 , 0x15 , 0x19 , 0x19   , 0x19   ,  0x19 , 0x15, 0x15, 0x15, 0x15 ,			

			0x21, 0x6 , 0x25,  0x25, 0x25,0x21 ,    0x24,0x24,0x24,     0x23,0x23,0x23 , 0x21 ,

			0x31, 0x31   , 0x31, 0x31  , 0x15, 0x16, 0x17, 0x18, 0x19, 0x11, 0x12, 0x13, 0x14,     //  666666666
			0x11, 0x12, 0x1, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x11, 0x12, 0x13, 0x14,
			0x11, 0x1, 0x1, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x11, 0x12, 0x13, 0x14,

			0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x11, 0x12, 0x13, 0x14,     ////  999999999
			0x19, 0x19, 0x19, 0x19, 0x19 , 0x19 , 0x19 , 0x19 , 0x19 , 0x19 , 0x19 , 0x19 , 0x19 ,

			0x19  , 0x19, 0x19, 0x19, 0x19 , 0x19 ,    //136 
			0x19 , 0x19, 0x19, 0x19  , 0x12, 0x13, 0x14,
			0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19
		},
		{  
			//起手杠						
			0x11, 0x11, 0x11, 0x12, 0x12, 0x12, 0x13, 0x13, 0x13, 0x14, 0x14, 0x14, 0x15,
			0x11, 0x11, 0x11, 0x12, 0x12, 0x12, 0x13, 0x13, 0x13, 0x14, 0x14, 0x14, 0x15,
			0x11, 0x15, 0x15, 0x12, 0x12, 0x12, 0x13, 0x13, 0x13, 0x14, 0x14, 0x14, 0x15,
			0x11, 0x15, 0x15, 0x12, 0x12, 0x12, 0x13, 0x13, 0x13, 0x14, 0x14, 0x14, 0x15,	

			0x11, 0x31  , 0x13, 0x14, 0x15, 0x11, 0x12, 0x13, 0x14, 0x15, 0x11, 0x12, 0x13,
			0x11, 0x12, 0x13, 0x14, 0x15, 0x11, 0x12, 0x13, 0x14, 0x15, 0x11, 0x12, 0x13,
			0x11, 0x12, 0x13, 0x14, 0x15, 0x11, 0x12, 0x13, 0x14, 0x15, 0x11, 0x12, 0x13,
			0x11, 0x12, 0x13, 0x14, 0x15, 0x11, 0x12, 0x13, 0x14, 0x15, 0x11, 0x12, 0x13,
			0x11, 0x12, 0x13, 0x14, 0x15, 0x11, 0x12, 0x13, 0x14, 0x15, 0x11, 0x12, 0x13,
			0x11, 0x12, 0x13, 0x14, 0x15, 0x11, 0x12, 0x13, 0x14, 0x15, 0x11, 0x12, 0x13,
			0x11, 0x12, 0x13, 0x14, 0x15, 0x11, 0x12, 0x13, 0x14, 0x15, 0x11, 0x12, 0x13,
			0x11, 0x12, 0x13, 0x14, 0x15, 0x11, 0x12,
		},
		{//起手杠			
			0x11, 0x11, 0x11, 0x12, 0x12, 0x12, 0x13, 0x13, 0x13, 0x14, 0x14, 0x14, 0x15,
			0x21, 0x21, 0x21, 0x22, 0x22, 0x22, 0x23, 0x23, 0x23, 0x24, 0x24, 0x24, 0x25,
			0x31, 0x31, 0x31, 0x32, 0x32, 0x32, 0x33, 0x33, 0x33, 0x34, 0x34, 0x34, 0x41,
			0x11, 0x11, 0x11, 0x12, 0x12, 0x12, 0x13, 0x13, 0x13, 0x14, 0x14, 0x14, 0x15,
			0x11, 0x31, 0x31, 0x32, 0x32, 0x32, 0x33, 0x33, 0x33, 0x34, 0x34, 0x34, 0x41,
			0x15, 0x15, 0x15, 0x15, 0x15, 0x15, 0x15, 0x15, 0x15, 0x15, 0x15, 0x15, 0x15,
			0x15, 0x15, 0x15, 0x15, 0x15, 0x15, 0x15, 0x15, 0x15, 0x15, 0x15, 0x15, 0x15,
			0x15, 0x15, 0x15, 0x15, 0x15, 0x15, 0x15, 0x15, 0x15, 0x15, 0x15, 0x15, 0x15,
			0x15, 0x15, 0x15, 0x15, 0x15, 0x15, 0x15, 0x15, 0x15, 0x15, 0x15, 0x15, 0x15,
			0x15, 0x15, 0x15, 0x15, 0x15, 0x15, 0x15, 0x15, 0x15, 0x15, 0x15, 0x15, 0x15,
			0x15, 0x15, 0x15, 0x15, 0x15, 0x15, 0x15, 0x15, 0x15, 0x15, 0x15, 0x15, 0x15,
			0x15, 0x15, 0x15, 0x15, 0x15, 0x15, 0x15,
		},		
	};
	
	static _uint8 bTypePool[] =
	{
		0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09,
		0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19,
		0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29,
		0x31, 0x32, 0x33, 0x34, 0x41, 0x42, 0x43,
	};

	m_CardTypePool.Clear();
	log_debug("GameID[%d]", GameId);
	if (GameId == NINGXIAG_MAJIAGN_GAME_ID)
	{
		m_CardTypePool.AddPool(bTypePool,  27);
		m_MaxCount = 108;
	}
	else
	{
		m_CardTypePool.AddPool(bTypePool, sizeof(bTypePool));
		m_MaxCount = 136;
	}	
	int iPoolCount = sizeof(pool) / POOL_CARD_MAX;
	memcpy(m_Pool, pool[index % iPoolCount], POOL_CARD_MAX); 

	m_Count = m_MaxCount;
	m_Current = 0;
	m_Last = m_MaxCount;
}