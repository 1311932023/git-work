#ifndef __MJ_COMMON4_H__
#define __MJ_COMMON4_H__
#include "common.h"

#define RD_CARD_BUFFER_MAX (32)

class MJ_Cardbuffer_rd
{
public:
	MJ_Cardbuffer_rd() : m_Begin(0), m_End(0), m_Size(0){}
	MJ_Cardbuffer_rd(const MJ_Cardbuffer_rd& other);

	void Init(){
		m_Begin = 0;
		m_End = 0;
		m_Size = 0;
	}
	void Add(_uint8 card);
	void Remove(_uint8 card);
	_uint8 operator[](int pos)		
	{		
		return m_Buffer[m_Begin + pos];
	}
	_uint8 Size(){ return m_Size; }
	const MJ_Cardbuffer_rd& operator=(const MJ_Cardbuffer_rd& other);
private:
	void LeftShift();
	void RightShift();	
private:
	_uint8 m_Begin;
	_uint8 m_End;
	_uint8 m_Size;
	union 
	{
		_uint8 m_Buffer[RD_CARD_BUFFER_MAX];
		_uint64 m_IntBuffer[4];
	};
	friend class Safe_Check_rd;
};

#endif