#include "mj_common4.h"
#include <assert.h>

class Safe_Check_rd
{
public:
	Safe_Check_rd(const MJ_Cardbuffer_rd& o) : m_Obj(o)
	{
		
	}
	~Safe_Check_rd()
	{
		//assert(m_Obj.m_Begin >= 0); 
		//assert(m_Obj.m_End <= RD_CARD_BUFFER_MAX);
	}
private:
	const MJ_Cardbuffer_rd& m_Obj;
};

MJ_Cardbuffer_rd::MJ_Cardbuffer_rd(const MJ_Cardbuffer_rd& other)
{	
	m_Begin = other.m_Begin;
	m_End = other.m_End;
	m_Size = other.m_Size;
	Safe_Check_rd(*this);
	if (other.m_Size == 0)
		return;
	for (int i = other.m_Begin / 8; i <= ((other.m_End - 1) / 8); i++)
	{
		//assert(i < 4);
		m_IntBuffer[i] = other.m_IntBuffer[i];
	}	
}

void MJ_Cardbuffer_rd::Add(_uint8 card)
{
	Safe_Check_rd(*this);
	if (m_Size == 0)
	{
		m_Begin = 0;
		m_End = 0; 
		m_Buffer[m_End++] = card;
		m_Size++;
		return;
	}
	//assert(m_Size < RD_CARD_BUFFER_MAX - sizeof(_uint64) - 1);
	if (card >= m_Buffer[m_End - 1])
	{
		if (m_End == RD_CARD_BUFFER_MAX)
		{	
			LeftShift();
		}
		m_Buffer[m_End++] = card;
		m_Size++;
		return;
		
	}
	if (card <= m_Buffer[m_Begin])
	{
		if (m_Begin == 0)
		{
			RightShift();		
		}
		m_Buffer[--m_Begin] = card;
		m_Size++;
		return;
	}
	if (m_Begin == 0)
	{
		RightShift();
	}
	for (int i = m_Begin; i < m_End; i++)
	{		
		if (card <= m_Buffer[i])
		{
			m_Buffer[i - 1] = card;
			m_Begin--;
			m_Size++;
			break;
		}
		else
		{
			m_Buffer[i - 1] = m_Buffer[i];
		}
	}
}

void MJ_Cardbuffer_rd::Remove(_uint8 card)
{
	Safe_Check_rd(*this);
	if (m_Size == 0)
	{
		m_Begin = 0;
		m_End = 0;
		return;
	}
	if (card == m_Buffer[m_Begin])
	{
		m_Begin++;
		m_Size--;
		if (m_Size == 0)
		{
			m_Begin = 0;
			m_End = 0;
			return;
		}
	}
	if (card == m_Buffer[m_End - 1])
	{
		m_End--;
		m_Size--;
		if (m_Size == 0)
		{
			m_Begin = 0;
			m_End = 0;
			return;
		}
	}	
	for (int i = m_Begin + 1; i < m_End; i++)
	{
		if (card == m_Buffer[i])
		{
			for (int j = i - 1; j >= m_Begin; j--)
			{
				m_Buffer[j + 1] = m_Buffer[j];
			}
			m_Begin++;
			m_Size--;
			break;
		}
	}	
}
const MJ_Cardbuffer_rd& MJ_Cardbuffer_rd::operator = (const MJ_Cardbuffer_rd& other)
{
	Safe_Check_rd(*this);
	m_Begin = other.m_Begin;
	m_End = other.m_End;
	m_Size = other.m_Size;
	if (other.m_Size == 0)
		return *this;
	for (int i = other.m_Begin / 8; i <= ((other.m_End - 1) / 8); i++)
	{
		m_IntBuffer[i] = other.m_IntBuffer[i];
	} 
	return *this;
}
void MJ_Cardbuffer_rd::RightShift()
{
	Safe_Check_rd(*this);
	//assert(m_Size < RD_CARD_BUFFER_MAX - sizeof(_uint64));
	m_IntBuffer[3] = m_IntBuffer[2];
	m_IntBuffer[2] = m_IntBuffer[1];
	m_IntBuffer[1] = m_IntBuffer[0];
	m_Begin += sizeof(_uint64);
	m_End += sizeof(_uint64);

}
void MJ_Cardbuffer_rd::LeftShift()
{	
	Safe_Check_rd(*this);
	//assert(m_Size < RD_CARD_BUFFER_MAX - sizeof(_uint64));
	m_IntBuffer[0] = m_IntBuffer[1];
	m_IntBuffer[1] = m_IntBuffer[2];
	m_IntBuffer[2] = m_IntBuffer[3];
	m_Begin -= sizeof(_uint64);
	m_End -= sizeof(_uint64);

}