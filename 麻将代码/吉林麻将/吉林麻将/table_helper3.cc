#include "table_helper.h"
#include "CardPool_ReadJson.h"
//湖南麻将-长沙麻将
//0x01 1万
//0x11 1条
//0x21 1同
//0x31 东
//0x41 中

void CCardPool::FixCard_ReadJson()
{

	static _uint8 bTypePool[] =
	{
		0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09,
		0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19,
		0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29,
		0x31, 0x32, 0x33, 0x34, 0x41, 0x42, 0x43,
	};

	m_CardTypePool.Clear();

	m_CardTypePool.AddPool(bTypePool, sizeof(bTypePool));


	CardPool_ReadJson testCard;
	
	int n = 136; 
	if (GameId == NINGXIAG_MAJIAGN_GAME_ID)
		n = 108;
	else
		n = 136; 

	
	testCard.ReadJsonFile("conf/xuke.json" ,   n  );

	testCard.GetTestCard(m_Pool,  n);

	m_MaxCount = testCard.GetMaxCount();


	m_Count = m_MaxCount;
	m_Current = 0;
	m_Last = m_MaxCount;
}