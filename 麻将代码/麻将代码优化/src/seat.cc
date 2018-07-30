
#include "table.h"

extern PHZ phz;
extern Log log;

Seat::Seat()
{
	m_Table = NULL;
	m_SeatId = INVALID_CHAIRID;   
	Clear();
}

Seat::~Seat()
{
	
}

void Seat::Clear()
{
	m_Uid = 0;
	m_Player = NULL;
	m_bOccupied = false;
	Reset();
}

void Seat::Reset()
{
	m_bReady = false;
	m_bResponse = false;
	m_bUserOperate = TYPE_NULL;
	m_bMultiUserDoneOp = TYPE_NULL;
	m_OutCardCnt = 0;
	m_UserPiaoScore = -1;
	m_HandCard.reset();
	m_bOutRecord.Clear();
	m_bGetCardCount = 0;
	m_bMultiUserDoneOpCard = 0;
}

void Seat::ResetAction()
{
	m_bUserOperate = TYPE_NULL;
	m_bMultiUserDoneOpCard = 0;
	m_bMultiUserDoneOp = TYPE_NULL;
	//m_OutCardCnt = 0;
}