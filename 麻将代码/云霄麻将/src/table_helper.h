#ifndef __TABLE_HELPER_H__
#define __TABLE_HELPER_H__
#include <vector>
#include <json/json.h>
#include <assert.h>
#include "common.h"
#include "mj_common2.h"


typedef MJ_BUFFER<int, 40> MJ_SUB_ID;
bool json_check_play_type_id(const Json::Value &val);
int json_get_play_type_id(const Json::Value &val, int iPlayID, MJ_SUB_ID& SubID);

bool Is_ChiPengGang(int iOpType);
bool Is_HuOperator(int iOpType);

/*
 *	漏胡规则
 *	记录弃胡的牌， 以及番数
 *	m_Available true启用漏胡规则
 *	Lock 锁住不能胡牌
 *	Evalute 判断是否可以解锁胡牌, true 可以解锁
 *	Unlock	解锁可以胡牌
 */
#define FW_LOCK_ALL_CARD	INVALID_MJ_CARD
#define FW_LOCK_ANY_POINT	(0xff)

class ForbidWin
{
	//_uint8 m_LockCard;
	std::vector<_uint8> m_LockCard;
	_uint8 m_LockPoints;
	bool m_Lock;		
	bool Evalute(_uint8 card, _uint8 point)const;
public:
	ForbidWin();
	void Lock(_uint8 card, _uint8 point);	
	bool IsLock(_uint8 card, _uint8 point = 0);	
	void Unlock();	
};


class HuScore
{
public:
	int iTotleScore;
	int iHuScore;
	int iGangScore;
	int iAddtionScore;
	
};
class HuScoreSet
{
	HuScore set[GAME_PLAYER];
public:
	HuScoreSet()
	{
		memset(set, 0, sizeof(set));
	}
	HuScore& operator[](_uint32 pos)
	{
		assert(pos < GAME_PLAYER);
		return set[pos];
	}	
};

enum E_POOL_MODE
{
	CARD_POOL_ALL,			//所有牌 包括八个花
	CARD_POOL_NO_FLOWER,	//万筒条风箭
	CARD_POOL_NO_WIND_DRAGON_FLOWER,//万筒条
	CARD_POOL_2_PEOPLE,
	CARD_POOL_3_PEOPLE,
	CARD_POOL_100CARDS,		//100张没有万

	CARD_POOL_4_FLOWER,		//万筒条风箭 + 春夏秋冬
	CARD_POOL_NO_WIND_4_FLOWER, //无风牌4花   万筒条+ 春夏秋冬
	CARD_POOL_NO_WIND_8_FLOWER, //无风牌8花  万筒条+8花
	CARD_POOL_2_FLOWER,		//万筒条风箭 + 春夏
	CARD_POOL_6_FLOWER,		//万筒条风箭 + 春夏秋冬梅兰
	CARD_POOL_NO_WAN_4_FLOWER,
	CARD_POOL_NO_WAN_8_FLOWER,	
	CARD_POOL_NO_TIAO,		//牌库剔除条
};

class CCardTypePool
{
public:
	CCardTypePool();
	~CCardTypePool();

	_uint8 operator[](_uint8 pos)const
	{
		assert(pos < m_MaxCount);
		return m_Pool[pos];
	}
	_uint8 Size()const { return m_MaxCount; }
	void Set(E_POOL_MODE eMode);
	void SetJoker(_uint8 card);
public:
	void Clear() { m_MaxCount = 0; }
	void AddPool(const _uint8* card, _uint8 size);
private:
	_uint8 m_JokerCard;
private:
	_uint8 m_Pool[POOL_CARD_TYPE_MAX];
	_uint8 m_MaxCount;
	E_POOL_MODE m_Mode;
};

class CCardPool
{
public:	
	CCardPool(E_POOL_MODE iMode, int iSeedID = 0);
	~CCardPool();

	void Set(E_POOL_MODE iMode, int iSeedID = 0);
	void SetJoker(_uint8 card, _uint8 count);
public:		
	_uint8 operator[](_uint8 pos)const;
	_uint8& operator[](_uint8 pos);	
	
	void InitPool();
	_uint8 Find(_uint8 card)const;

public:
	_uint8 GetOneCard();
	_uint8 GetLast();
	int GetCard(_uint8* pool, _uint8 size);	
	int GetLastCard(_uint8* pool, _uint8 size);

	_uint8 MaxCount()const { return m_MaxCount; }		//最大牌数
	_uint8 LastCard()const { return m_Pool[m_Last - 1];	}	//查看最后一张牌
	_uint8 CurCard()const { return m_Pool[m_Current]; }	//查看第一张牌
	_uint8 Count()const { return m_Count; }				//当前剩余牌数
	bool IsEmpty()const { return m_Count == 0; }		//是否没有牌了
	bool IsLastCard()const { return m_Count == 1; }		//是否最后一张牌
	_uint8 GetCardCount_FromBack()const{ return m_MaxCount - m_Last; }
public:
	bool Exchange_Cur_Next(_uint8 pos = -1);
	void ChangeCard(int first, int second);
public:
	void FixCard(int );//测试用固定牌
	void FixCard_ReadJson();
private:
	void AddPool(const _uint8* card, _uint8 size, _uint8 count = 4);
	void AddJoker();
private:
	void InitNoFlower();	
	void InitNoWindDragon();
	void Init2People();
	void Init3People();
	void Init100Cards();
	void InitAllCard();
	void Init4FlowerCard();
	void InitNoWind4FlowerCard();
	void InitNoWind8FlowerCard();
	void Init2FlowerCard();
	void Init6FlowerCard();
	void InitCardPoolNoTong();
private:
	CCardTypePool m_CardTypePool;
public:
	const CCardTypePool& Type()const
	{
		return m_CardTypePool;
	}

private:
	int m_SeedID;
	E_POOL_MODE m_Mode;
	_uint8 m_Pool[POOL_CARD_MAX];		
	_uint8 m_MaxCount;
	_uint8 m_Count;
	_uint8 m_Current;	//当前牌位置
	_uint8 m_Last;		//最后一张牌的下一张
	
private:
	struct
	{
		_uint8 m_JokerCard;
		_uint8 m_JokerCount;
	};
};


#endif
