#ifndef __MJ_COMMON2_H__
#define __MJ_COMMON2_H__
#include "common.h"
#include "../common/boostHeader.hpp"

#define MJ_CARDS_STYLE_LEN (34)	// 3 * 9 + 7


template<typename TYPE>
const TYPE& MAX(const TYPE& lhs, const TYPE& rhs)
{
	return lhs > rhs ? lhs : rhs;
}

template<typename TYPE>
const TYPE& MIN(const TYPE& lhs, const TYPE& rhs)
{
	return lhs < rhs ? lhs : rhs;
}

template<class TYPE, _uint32 SIZE = MJ_CARDS_STYLE_LEN>
class MJ_BUFFER
{
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive & ar, const unsigned int /* file_version */) {
		ar & len;
		ar & cards;
	}
public:
	MJ_BUFFER() : len(0)
	{
		memset(cards, 0, sizeof(cards));
	}	
	MJ_BUFFER(const MJ_BUFFER<TYPE, SIZE>& other) : len(other.len)
	{		
		for (int i = 0; i < (int)len; i++)
		{
			cards[i] = other[i];
		}
	}
	void Add(const TYPE& uiCard)
	{
		if (len >= SIZE)
			throw("out of range!\n");
		cards[len++] = uiCard;
	}
	void Add(const TYPE* uiCard, _uint32 size)
	{
		if (len + size >= sizeof(cards) / sizeof(cards[0]))	
			throw("out of range!\n");
		
		for (_uint32 i = 0; i < size;i++)		
			Add(uiCard[i]);
		
	}
	bool Find(const TYPE& other)const
	{
		for (_uint32 i = 0; i < len; i++)
		{
			if (cards[i] == other)	return true;
		}
		return false;
	}
	const TYPE& operator[](_uint32 pos) const
	{
		if (pos >= len)		
			throw("out of range!\n");
		
		return cards[pos];
	}	
	TYPE& operator[](_uint32 pos)
	{
		if (pos >= len)
			throw("out of range!\n");

		return cards[pos];
	}
	int Size() const { return len; }
	bool IsNull()const{ return len == 0; }
	bool NotNull()const{ return len != 0; }
	const TYPE& Last()const
	{
		if (len == 0)
			throw("buffer is empty!\n");
		return cards[len - 1];
	}		
	TYPE& Last()
	{
		if (len == 0)
			throw("buffer is empty!\n");
		return cards[len - 1];
	}
	void Clear() { len = 0; }
	bool RemoveLast() { if (len != 0) len--; return true; }
	bool RemoveLast(const TYPE& other)
	{
		if (IsNull())
		{
			return false;
		}
		if (Last() == other)
		{
			return RemoveLast();
		}
		if (len == 1)
		{
			return false;
		}			
		for (int i = len - 2; i >= 0; i--)
		{
			if (cards[i] == other)
			{
				memmove(&cards[i], &cards[i + 1], (--len - i) * sizeof(TYPE));
				return true;
			}
		}	
		return false;
	}
	bool Remove(const TYPE& other)
	{
		if (IsNull())
		{
			return false;
		}		
		_uint32 tmp = len - 1;
		for (int i = 0; i < (int)tmp; i++)
		{
			if (cards[i] == other)
			{
				memmove(&cards[i], &cards[i + 1], (tmp - i) * sizeof(TYPE));
				len--;
				return true;
			}
		}
		if (Last() == other)
		{
			return RemoveLast();
		}
		return false;
	}
	void SortAscend()
	{
		for (_uint32 i = 0; i < len; i++)
		{
			for (_uint32 j = i + 1; j < len; j++)
			{
				if (cards[i] > cards[j])
				{
					TYPE t = cards[i];
					cards[i] = cards[j];
					cards[j] = t;
				}
			}
		}		
	}
	void operator=(const MJ_BUFFER<TYPE, SIZE>& other)
	{
		len = other.len;
		for (int i = 0; i < (int)len; i ++)
		{
			cards[i] = other[i];
		}
	}
	void operator+=(const MJ_BUFFER<TYPE, SIZE>& other)
	{
		_uint32 iCount = MIN(SIZE - len, other.len);		
		for (_uint32 i = 0; i < iCount; i++)
			Add(other[i]);
	}
	int Counts(const TYPE& other)const
	{
		int iCount = 0;
		for (_uint32 i = 0; i < len; i++)
		{
			if (cards[i] == other)
			{
				iCount++;
			}
		}
		return iCount;
	}
	int Max()const { return len; }
	TYPE* Front() { return cards; }
private:
	_uint32 len;
	TYPE cards[SIZE];
};

struct MJ_win_pattern
{
	_uint8 jiang;			//½«
	_uint8 kan[HAND_CARD_KAN_MAX][4];		//¿²
};


typedef MJ_BUFFER<_uint8>				MJ_cards_type_buffer;
typedef MJ_BUFFER<_uint8, 18>			MJ_hand_buffer;
typedef MJ_BUFFER<_uint32, 40>			MJ_fan_buffer;
typedef MJ_BUFFER<_uint8, GAME_PLAYER>	MJ_user_buffer;
typedef MJ_BUFFER<_uint8, 80>			ShowCards;
typedef MJ_BUFFER<_uint8, 5>			MJ_peng;
typedef MJ_BUFFER<_uint32, 6>			MJ_qishou_fan;
#endif