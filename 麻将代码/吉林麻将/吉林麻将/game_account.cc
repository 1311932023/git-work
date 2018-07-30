#include "game_account.h"

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
using namespace std;
using namespace Json;

CGameAccout::CGameAccout()
{
	m_roomid = 0;
	m_round = -1;
	memset(m_score, 0, sizeof(m_score));
	memset(m_total_score, 0, sizeof(m_total_score));
	memset(m_paly_id, 0, sizeof(m_paly_id));
}

CGameAccout::~CGameAccout()
{
}


void CGameAccout::SetRound(int round)
{
	m_round = round;
}

void CGameAccout::SetRoomid(int id)
{
	m_roomid = id;
}


void CGameAccout::SetPlayerUid(int charid, int uid)
{
	assert(charid >= 0);
	assert(charid < GAME_PLAYER);
	m_paly_id[charid] = uid;
}

void CGameAccout::SetScore(int play_id, int score)
{
	assert(play_id < GAME_PLAYER);
	assert(play_id >= 0);

	m_score[play_id] = score;
}

void CGameAccout::SetTotalScore(int play_id, int total)
{
	assert(play_id < GAME_PLAYER);
	assert(play_id >= 0);

	m_total_score[play_id] = total;
}

void CGameAccout::Reset()
{
	m_roomid = 0;
	memset(m_score, 0, sizeof(m_score));
	memset(m_total_score, 0, sizeof(m_total_score));
}

bool CGameAccout::IsValid()
{
	assert(m_roomid > 0);
	assert(m_round >= 0);
	assert(m_round < 16);
	return true; 
}

void CGameAccout::GetJsonData(Json::Value &data)
{
	//不管有没有都清空
	if ( !data["player"].empty() )
	{
		data["player"].clear();
	}
	for (int i = 0; i < GAME_PLAYER; ++i)
	{
		data["player"].append(m_paly_id[i]);
	}


	if ( !data["total_score"].empty() )
	{
		data["total_score"].clear();
	}
	for ( int i = 0 ; i < GAME_PLAYER ; ++i  )
	{
		data["total_score"].append(m_total_score[i]);
	}
	string str_roundid = string("round_") + toString(m_round);

	Json::Value _round;
	for (int i = 0; i < GAME_PLAYER; ++i)
	{
		_round[str_roundid].append(m_score[i]);
	}
	data["game_round"].append(_round);
	data["update_time"] = (int)time(NULL);

}

std::string CGameAccout::toString(int index)
{
	char buff[1024];
	sprintf(buff , "%d", index);
	string str = buff;
	return str;
}

bool CGameAccout::GetPlayerScore(int uid, int &score)
{
	for ( int i = 0 ; i < GAME_PLAYER ; ++i  )
	{
		if ( m_paly_id[i] == uid )
		{
			score = m_total_score[i];
			return true;
		}
	}
	return false;
}

void CGameAccout::UpdateInfoFromJson(Json::Value root)
{
	memset(m_paly_id, 0, sizeof(m_paly_id));
	memset(m_total_score, 0, sizeof(m_total_score));
	if ( !root["player"].empty() )
	{
		Json::Value arryobj = root["player"];
		for (unsigned i = 0; i < arryobj.size() && i < 4; ++i)
		{
			m_paly_id[i] = arryobj[i].asInt();
		}
	}
	if (  !root["total_score"].empty()  )
	{
		Json::Value arryobj = root["total_score"];
		for (unsigned i = 0; i < arryobj.size() && i < 4; ++i)
		{
			m_total_score[i] = arryobj[i].asInt();
		}
	}
}

