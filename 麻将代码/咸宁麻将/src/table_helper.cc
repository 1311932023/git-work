#include "table_helper.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <iostream>
#include <algorithm>

bool json_check_play_type_id(const Json::Value &val)
{
	return !val["dataMap"].isNull() && !val["dataMap"]["innerWayList"].isNull();
}


int json_get_play_type_id(const Json::Value &val, int iPlayID, MJ_SUB_ID& SubID)
{
	char tmp[0x1000];
	snprintf(tmp, sizeof(tmp), "%d", iPlayID);
	SubID.Clear();
	if (!val["dataMap"]["innerWayList"][tmp].isNull() && val["dataMap"]["innerWayList"][tmp].isString())
	{
		const char* str = val["dataMap"]["innerWayList"][tmp].asString().c_str();			
		const char* tmp_str = str;
		const char* tmp_str2 = NULL;
		int len = 0;
		while ((tmp_str2 = strchr(tmp_str, ',')) != NULL)
		{
			len = tmp_str2++ - tmp_str;
			strncpy(tmp, tmp_str, len);
			tmp[len] = '\0';
			SubID.Add(atoi(tmp));
			tmp_str = tmp_str2;
		}

		if ((len = strlen(tmp_str)) != 0)
		{
			strncpy(tmp, tmp_str, len);
			tmp[len] = '\0';
			SubID.Add(atoi(tmp));			
		}
		return SubID.Size();
	}
	return 0;
}

bool Is_ChiPengGang(int iOpType)
{
	if (iOpType == TYPE_PASS)
		return false;
	return iOpType & (TYPE_CHI | TYPE_GANG | TYPE_PENG);
}
bool Is_HuOperator(int iOpType)
{
	if (iOpType == TYPE_PASS)
		return false;
	return iOpType & TYPE_HU;
}

ForbidWin::ForbidWin() : m_LockPoints(0), m_Lock(false)
{
	
}

void ForbidWin::Lock(_uint8 card, _uint8 point)
{		
	if (!m_Lock)
	{		
		for (std::vector<_uint8>::const_iterator it = m_LockCard.begin(); it != m_LockCard.end(); it++)
		{
			if (*it == card)
			{
				goto __FUNC_END__;
			}
		}
		m_LockCard.push_back(card);
__FUNC_END__:
		m_LockPoints = MAX(m_LockPoints, point);
		m_Lock = true;
	}
	
}

bool ForbidWin::Evalute(_uint8 card, _uint8 point)const
{	
	if (m_Lock)
	{		
		if (point >= m_LockPoints)		
		{
			return true;
		}		
		for (std::vector<_uint8>::const_iterator it = m_LockCard.begin(); it != m_LockCard.end(); it++)
		{
			if (*it == FW_LOCK_ALL_CARD)
			{
				return false;
			}
		}				
		for (std::vector<_uint8>::const_iterator it = m_LockCard.begin(); it != m_LockCard.end(); it++)
		{
			if (*it == card)
			{
				return false;
			}
		}
		return true;
	}
	return false;
}
bool ForbidWin::IsLock(_uint8 card, _uint8 point)
{
	if (Evalute(card, point))
	{
		Unlock();
		return false;
	}
	return m_Lock;
}
void ForbidWin::Unlock()
{
	if (m_Lock)
	{
		//m_LockCard = 0;
		m_LockCard.clear();
		m_LockPoints = 0;
		m_Lock = false;
	}	
}

static _uint8 g_Card_Wan[] =
{
	0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09,//万	
};
static _uint8 g_Card_Tiao[] =
{
	0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19,//条	
};
static _uint8 g_Card_Tong[] =
{
	0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29,//筒	
};
static _uint8 g_Card_Feng[] =
{
	0x31, 0x32, 0x33, 0x34,	
};
static _uint8 g_Card_Jian[] =
{
	0x41, 0x42, 0x43,
};
static _uint8 g_Card_Zi[] =
{
	0x31, 0x32, 0x33, 0x34, 0x41, 0x42, 0x43,
};
static  _uint8 g_Card_Hua[] =
{
	0x51,0x52,0x53,0x54,0x55,0x56,0x57,0x58,
};
static  _uint8 g_Card_4_Hua[] =
{
	0x51, 0x52, 0x53, 0x54, 
};

void RandomCard(_uint8* pool, _uint8 size, int seed)
{
	static int Xseed = 0;
	srand(int(time(0)) + seed + Xseed++);

	for (int i = size - 1; i > 0; i--)
	{
		int index = rand() % i;
		if (i != index)
		{
			_uint8 tmp = pool[i];
			pool[i] = pool[index];
			pool[index] = tmp;
		}
	}
	for (int i = 0; i < size - 1; i++)
	{
		int index = rand() % (size - i) + i;
		if (i != index)
		{
			_uint8 tmp = pool[i];
			pool[i] = pool[index];
			pool[index] = tmp;
		}
	}
}
CCardPool::CCardPool(E_POOL_MODE iMode, int iSeedID)
{
	Set(iMode, iSeedID);
}
CCardPool::~CCardPool()
{

}
void CCardPool::Set(E_POOL_MODE iMode, int iSeedID)
{
	srand(int(time(0)));
	
	m_Mode = iMode;
	m_SeedID = iSeedID + rand();
	
	m_JokerCard = INVALID_MJ_CARD;
	m_JokerCount = 0;

	m_CardTypePool.Set(iMode);

	InitPool();
}
void CCardPool::SetJoker(_uint8 card, _uint8 count)
{
	m_JokerCard = card;
	m_JokerCount = count;

	m_CardTypePool.SetJoker(card);

}


void CCardPool::InitPool()
{
	m_MaxCount = 0;
	m_CardTypePool.Clear();

	switch (m_Mode)
	{	
	case CARD_POOL_NO_FLOWER:		
		InitNoFlower();
		break;	
	case CARD_POOL_2_PEOPLE:
		Init2People();		
		break;
	case CARD_POOL_NO_WIND_DRAGON_FLOWER:
		InitNoWindDragon();
		break;
	case CARD_POOL_NO_WIND_DRAGON_FLOWER_ADD_ZHONG:
		InitNoWindDragonAddZhong();
		break;
	case CARD_POOL_3_PEOPLE:
		Init3People();		
		break;			
	case CARD_POOL_100CARDS:
		Init100Cards();
		break;
	case CARD_POOL_ALL:
		InitAllCard();
		break;
	case CARD_POOL_4_FLOWER:
		Init4FlowerCard();
		break;
	case CARD_POOL_NO_WIND_4_FLOWER:
		InitNoWind4FlowerCard();
		break;
	case CARD_POOL_NO_WIND_8_FLOWER:
		InitNoWind8FlowerCard();
		break;
	case CARD_POOL_2_FLOWER:
		Init2FlowerCard();
		break;
	case CARD_POOL_6_FLOWER:
		Init6FlowerCard();
		break;
	case CARD_POOL_NO_WAN_4_FLOWER:
		AddPool(g_Card_Tong, sizeof(g_Card_Tong));
		AddPool(g_Card_Tiao, sizeof(g_Card_Tiao));
		AddPool(g_Card_Zi, sizeof(g_Card_Zi));
		AddPool(g_Card_4_Hua, sizeof(g_Card_4_Hua), 1);
		break;
	case CARD_POOL_NO_WAN_8_FLOWER:
		AddPool(g_Card_Tong, sizeof(g_Card_Tong));
		AddPool(g_Card_Tiao, sizeof(g_Card_Tiao));
		AddPool(g_Card_Zi, sizeof(g_Card_Zi));
		AddPool(g_Card_Hua, sizeof(g_Card_Hua), 1);
		break;
	case CARD_POOL_NO_TIAO:
	{
		InitCardPoolNoTong();
		break;
	}
	default:
		assert(false);
		break;
	}
	AddJoker();

	m_Count = m_MaxCount;
	m_Current = 0;
	m_Last = m_MaxCount;

	RandomCard(m_Pool, m_MaxCount, m_SeedID);

}

void CCardPool::AddPool(const _uint8* card, _uint8 size, _uint8 count)
{
	assert(m_MaxCount + size * count <= (int)sizeof(m_Pool));
	for (int i = 0; i < count; i++)
	{
		memcpy(m_Pool + m_MaxCount, card, size);
		m_MaxCount += size;
	}
	m_CardTypePool.AddPool(card, size);
}
void CCardPool::AddJoker()
{	
	switch (m_JokerCount)
	{
	case 4:
		assert(m_JokerCard != INVALID_MJ_CARD);
		AddPool(&m_JokerCard, 1);
		break;
	case 8:
		assert(m_JokerCard != INVALID_MJ_CARD);
		AddPool(&m_JokerCard, 1, 8);
		break;
	}	
}
void CCardPool::InitNoFlower()
{
	AddPool(g_Card_Wan, sizeof(g_Card_Wan));
	AddPool(g_Card_Tiao, sizeof(g_Card_Tiao));
	AddPool(g_Card_Tong, sizeof(g_Card_Tong));
	AddPool(g_Card_Zi, sizeof(g_Card_Zi));
}
void CCardPool::InitNoWindDragon()
{
	AddPool(g_Card_Wan, sizeof(g_Card_Wan));
	AddPool(g_Card_Tiao, sizeof(g_Card_Tiao));
	AddPool(g_Card_Tong, sizeof(g_Card_Tong));
}

void CCardPool::InitNoWindDragonAddZhong()
{
	AddPool(g_Card_Wan, sizeof(g_Card_Wan));
	AddPool(g_Card_Tiao, sizeof(g_Card_Tiao));
	AddPool(g_Card_Tong, sizeof(g_Card_Tong));
	AddPool(&g_Card_Jian[0], sizeof(_uint8));
}

void CCardPool::Init2People()
{	
	AddPool(g_Card_Wan, sizeof(g_Card_Wan));
	AddPool(g_Card_Zi, sizeof(g_Card_Zi));	
}
void CCardPool::Init3People()
{
	AddPool(g_Card_Wan, sizeof(g_Card_Wan));
	AddPool(g_Card_Tiao, sizeof(g_Card_Tiao));
	AddPool(g_Card_Tong, sizeof(g_Card_Tong));	
}
void CCardPool::Init100Cards()
{
	AddPool(g_Card_Tong, sizeof(g_Card_Tong));
	AddPool(g_Card_Tiao, sizeof(g_Card_Tiao));
	AddPool(g_Card_Zi, sizeof(g_Card_Zi));
}

void CCardPool::InitAllCard()
{
	InitNoFlower();
	AddPool(g_Card_Hua, sizeof(g_Card_Hua) , 1);
}

void CCardPool::Init4FlowerCard()
{
	InitNoFlower();
	AddPool(g_Card_4_Hua, sizeof(g_Card_4_Hua), 1);
}

void CCardPool::InitNoWind4FlowerCard()
{
	InitNoWindDragon();
	AddPool(g_Card_4_Hua, sizeof(g_Card_4_Hua), 1);
}

void CCardPool::InitNoWind8FlowerCard()
{
	InitNoWindDragon();
	AddPool(g_Card_Hua, sizeof(g_Card_Hua), 1);
}

void CCardPool::Init2FlowerCard()
{
	InitNoFlower();
	AddPool(g_Card_Hua, 2, 1);
}

void CCardPool::Init6FlowerCard()
{
	InitNoFlower();
	AddPool(g_Card_Hua, 6, 1);
}

void CCardPool::InitCardPoolNoTong()
{
	AddPool(g_Card_Wan, sizeof(g_Card_Wan));
	AddPool(g_Card_Tiao, sizeof(g_Card_Tiao));
	AddPool(g_Card_Zi, sizeof(g_Card_Zi));
	AddPool(g_Card_Hua, sizeof(g_Card_Hua), 1);
}

_uint8 CCardPool::Find(_uint8 card)const
{
	int iCount = 0;
	for (_uint8 i = m_Current; i < m_Last; i++)
	{
		if (m_Pool[i] == card)
			iCount++;
	}
	return iCount;
}
_uint8 CCardPool::operator[](_uint8 pos)const
{
	assert(pos + m_Current < m_Last);
	return m_Pool[pos + m_Current];
}
_uint8& CCardPool::operator[](_uint8 pos)
{
	assert(pos + m_Current < m_Last);
	return m_Pool[pos + m_Current];
}
_uint8 CCardPool::GetOneCard()
{
	if (m_Current < m_Last)
	{
		m_Count--;
		return m_Pool[m_Current++];
	}
	throw("Pool has no cards");
}
_uint8 CCardPool::GetLast()
{
	if (m_Current < m_Last)
	{
		m_Count--;
		return m_Pool[--m_Last];
	}
	throw("Pool has no cards");
}
int CCardPool::GetCard(_uint8* pool, _uint8 size)
{
	if (m_Count != 0 && size != 0)
	{
		_uint8 iCount = MIN(size, m_Count);
		memcpy(pool, &m_Pool[m_Current], iCount);
		m_Current += iCount;
		m_Count -= iCount;
		return iCount;
	}
	return 0;
}
int CCardPool::GetLastCard(_uint8* pool, _uint8 size)
{
	if (m_Count != 0 && size != 0)
	{
		_uint8 iCount = MIN(size, m_Count);
		m_Last -= iCount;
		memcpy(pool, &m_Pool[m_Last], iCount);
		m_Count -= iCount;
		return iCount;
	}
	return 0;
}
bool CCardPool::Exchange_Cur_Next(_uint8 pos)
{
	if (pos == (_uint8)-1)
	{
		if (m_Count >= 1)
		{
			_uint8 tmp = m_Pool[m_Current];
			m_Pool[m_Current] = m_Pool[m_Current + 1];
			m_Pool[m_Current + 1] = tmp;
			return true;
		}
	}
	else
	{
		if (m_Count > 0)
		{
			_uint8 tmp = m_Pool[m_Current];
			m_Pool[m_Current] = m_Pool[m_Current + pos];
			m_Pool[m_Current + pos] = tmp;
		}		
	}
	return false;	
}

void CCardPool::ChangeCard(int first, int second)
{
	assert(first < m_Count);
	assert(second < m_Count);
	_uint8 tmp = m_Pool[m_Current + first];
	m_Pool[m_Current + first] = m_Pool[m_Current + second];
	m_Pool[m_Current + second] = tmp;
}


CCardTypePool::CCardTypePool() : m_MaxCount(0)
{
	memset(m_Pool, 0, sizeof(m_Pool));
	m_JokerCard = 0;
}
CCardTypePool::~CCardTypePool()
{

}

const _uint8 pool_common[] = {
	0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09,//万			
	0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19,//条			
	0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29,//筒
	0x31, 0x32, 0x33, 0x34,//风
	0x41, 0x42, 0x43,//箭
};
const _uint8 pool_no_wind[] = {
	0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09,//万			
	0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19,//条			
	0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29,//筒
};
const _uint8 pool_with_zhong[] = {
	0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09,//万			
	0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19,//条			
	0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29,//筒
	0x41,
};
const _uint8 pool_all[] = {
	0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09,//万			
	0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19,//条			
	0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29,//筒
	0x31, 0x32, 0x33, 0x34,
	0x41, 0x42, 0x43,
	0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58,
};
const _uint8 pool_2_people[] = {
	0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09,//万			
	0x31, 0x32, 0x33, 0x34,
	0x41, 0x42, 0x43,
};
void CCardTypePool::Set(E_POOL_MODE eMode)
{
	m_MaxCount = 0;	
}
void CCardTypePool::SetJoker(_uint8 card)
{
	m_JokerCard = card;
}
void CCardTypePool::AddPool(const _uint8* card, _uint8 size)
{
	assert(m_MaxCount + size  <= sizeof(m_Pool));
	memcpy(m_Pool + m_MaxCount, card, size);
	m_MaxCount += size;
}