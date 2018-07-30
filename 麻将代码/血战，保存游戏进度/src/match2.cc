#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include <errno.h>

#include "zjh.h"
#include "game.h"
#include "proto/proto.h"
#include "common/log.h"
#include "common/client.h"
#include "player.h"
#include "table.h"
#include "match.h"
#include "redis_client.h"

extern ZJH zjh;
extern Log log;
extern Match* last_match;


int Match::HandlerJoinMatch(int matchid, Player *player, Json::Value& val)
{
	//玩家已经正在参赛
	Table* table;	
	if (m_TurnOverTables.find(player->tid) != m_TurnOverTables.end())
	{
		m_TurnOverTables.erase(player->tid);
		
		table = all_tables[player->tid];
		all_tables.erase(player->tid);

		delete table;
		goto __new_table__;
	}
	if (all_tables.find(player->tid) != all_tables.end())
	{	
		table= all_tables[player->tid];			
		table->PlayerLogin(player, true);
	}
	else
	{
__new_table__:
		table = Table::CreateTable(zjh.conf["tables"], val);
		table->init(zjh.conf["tables"], val, player);
		table->tid = player->tid;
		table->matchid = matchid;
		table->m_MatchName = m_MatchName;
		table->m_CurRound = InitRoundNumber();
		table->m_RobotRatio = m_RobotRatio;		
		table->match = this;
		table->m_cbInitCardPool = m_cbInitCardPool;		
		table->m_Type = 1002;
		all_tables[player->tid] = table;

		table->Sit_Down(player);
		table->PlayerLogin(player, true);	
		all_players[player->uid] = player;

		if (m_TurnCardInfo.empty())
		{
			int turn = m_TurnRoundNum[m_CurTurn - 1];
			for (int i = 0; i < turn; i++)
			{
				SpawnCards(i + 1, CurTurn());
			}
		}
		TableGameStart(table);
	}	
	player->matchid = matchid;
	player->table = table;		
	return 0;
}
Json::Value Match::GetTableConfigure(const Json::Value& remote)
{	
		
	std::ifstream in(zjh.conf["table_config"].asString().c_str(), std::ifstream::binary);
	Json::Value val;
	Json::Reader read;
	if (read.parse(in, val) <= 0)
	{
		log_error("%s parse file %s error\n", __FUNCTION__, zjh.conf["table_config"].asString().c_str());
	}
	if (zjh.game->qualifier_match)
		val["dataMap"]["innerWayList"] = remote;	
	return val;
}