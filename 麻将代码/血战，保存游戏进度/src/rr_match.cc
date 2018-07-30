#include "rr_match.h"
#include "redis_client.h"
#include "zjh.h"
#include "proto.h"


extern ZJH zjh;
extern Log log;

//BOOST_CLASS_EXPORT_GUID(RRMatch, "RRMatch")

RRMatch::RRMatch()
{
    m_TurnStartTimer.data = this;
    m_NextTurnTimer.data = this;
    m_SendRankTimer.data = this;
    m_SendPromotionTimer.data = this;
    m_UpLoadRealRankTimer.data = this;

	m_bScoreDependOnSeat = true;

	m_fBaseScore = 1000.0f;
	m_cbInitCardPool = true;

	m_bCalcMpScore = true;

	m_lose_player_mp_ratio = true;

	m_bSameCard = true;
}

RRMatch::~RRMatch()
{
    
}

//初始化玩家信息
void RRMatch::InitPlayerTid()
{
    //如果初始轮非第一轮表示挂起的比赛继续 对于人人赛需要进行瑞士移位
    if(m_CurTurn > 1){
        UpLoadMoveInfo();
        SwitzerlandMove();
    }
    else{
        std::vector<int> tempUids;
        for(std::map<int, Player*>::iterator it = all_players.begin(); it != all_players.end(); ++it){
            tempUids.push_back(it->first);
        }
        srand(time(NULL));
        random_shuffle(tempUids.begin(), tempUids.end());
        int tid = 1;
        for(size_t i = 0; i < tempUids.size(); ++i){
            if(i > 0 && i % GAME_PLAYER == 0){
                ++tid;
            }
            all_players[tempUids[i]]->tid = tid;	
			all_players[tempUids[i]]->tmp_playerid = i + 1;
            all_players[tempUids[i]]->seatid = -1;
            log_debug("%s mid:%d init player:%d tid:%d\n", __FUNCTION__, matchid, tempUids[i], tid);
        }
    }
}

//轮开始 统一发牌
void RRMatch::TurnStart()
{
    TurnReset();    
    for(std::map<int, Table*>::iterator it = all_tables.begin(); it != all_tables.end(); ++it){
        Table *t = it->second;
        TableGameStart(t);
    }
}

void RRMatch::TurnReset()
{
	if (m_single_turn_split)
	{
		m_bPause = false;
		m_bInterval = false;
		m_TurnOverTableCnt = 0;
		m_PlayerNum = all_players.size();
		//		log_info("%s roundinfo round:%d/%d turn:%d/%d\n", __FUNCTION__, m_CurRound, m_AllRound, m_CurTurn, m_AllTurn);
		return;
	}
	else
	{
		Parent::TurnReset();
	}
	for (int i = 0; i < GAME_PLAYER; i++)
	{
		m_playersInSeat[i].clear();
	}
	m_TurnOverTables.clear();

}

//牌桌回合结束 按方位统计分数 分发下一局的扑克
void RRMatch::RoundOver(Table *table)
{

	for (int i = 0; i < table->m_GAME_PLAYER; ++i)
	{
		Player *player = table->m_SeatPlayer[i].player;
		m_playersInSeat[player->seatid].insert(make_pair(player->uid, player));
	}
	for (int i = 0; i < table->m_GAME_PLAYER; ++i)
	{
		StatisticSeatRank(i, table->m_CurRound);
	}
	for (int i = 0; i < table->m_GAME_PLAYER; i++)
	{
		CalculateTotalRank(m_playersInSeat[i]);
	}
	CalculateTotalRank3();

	if (m_single_turn_split)
	{
		int tmp_all_round = table->m_CurRound - InitRoundNumber() + 1;
		if (tmp_all_round == m_AllRound)
		{
			m_TurnOverTableCnt++;
		}
		if (table->m_CurRound == AllRound())
		{
			m_TurnOverTables.insert(table->tid);
			for (std::set<int>::iterator it = m_TurnOverTables.begin(); it != m_TurnOverTables.end(); ++it) {
				int tmpTid = *it;
				for (int i = 0; i < GAME_PLAYER; ++i) {
					HandlerTurnOverInfo(all_tables[tmpTid]->m_SeatPlayer[i].player);
				}
			}
		}
	}
	else if (table->m_CurRound == AllRound())
	{
		m_TurnOverTableCnt++;
		if (m_CurTurn < m_AllTurn) {
			m_TurnOverTables.insert(table->tid);
			for (std::set<int>::iterator it = m_TurnOverTables.begin(); it != m_TurnOverTables.end(); ++it) {
				int tmpTid = *it;
				for (int i = 0; i < GAME_PLAYER; ++i) {
					HandlerTurnOverInfo(all_tables[tmpTid]->m_SeatPlayer[i].player);
				}
			}
		}
	}
	if (table->m_CurRound == AllRound() && m_CurTurn == m_AllTurn)
	{
		for (int i = 0; i < GAME_PLAYER; i++)
		{
			Player* tmp_player = table->m_SeatPlayer[i].player;
			if (tmp_player != NULL && !tmp_player->m_isRobot)
			{
				Jpacket packet;
				packet.val["cmd"] = SERVER_USER_END_REQ;
				packet.val["mid"] = matchid;
				packet.val["uid"] = tmp_player->uid;
				packet.val["datetime"] = (int)time(NULL);
				packet.end();
				zjh.game->SendToDataSvr(packet.tostring());
			}
		}
	}
	Parent::RoundOver(table);
	//int player_size = all_players.size() + all_robots.size();
	int player_size = all_players.size();
	if (m_TurnOverTableCnt == ((player_size / GAME_PLAYER) + (player_size % GAME_PLAYER != 0))) {
		TurnOver();
	}
	else
	{
		table->m_NextRound_timer_running = true;
		TIMER_AGAIN(table->m_NextRound_timer, table->m_NextRound_stamp);
	}
}

//优先将机器人淘汰 并重新排名

//轮次结束 计算排名 将机器人放最后
void RRMatch::TurnOver()
{
    Parent::TurnOver();
	if (isTurnOver())
	{
		//将当前轮mp和rank存入redis并通知平台
		UploadMPInfo();
		EliminateRobot();
		//CalculateTotalRank(all_players);

		CalculateTotalRank3();
		UploadTurnRank();
	}

    //所有轮次完毕 赛事结束
    if (m_CurTurn == m_AllTurn){
        MatchOver();
    }
    else{
        m_bInterval = true;
		m_SendPromotionTimer_running = true;
        ev_timer_again(zjh.loop, &m_SendPromotionTimer);
    }
}

//进入下一轮进行瑞士移位
void RRMatch::NextTurn()
{
    m_CurTurn++;
    m_AllRound = m_TurnRoundNum[m_CurTurn - 1];
    UpLoadMoveInfo();
    SwitzerlandMove();
	m_fixed_seat_id = true;
    ResetTable();
    TurnStart();
}
//上传轮移位信息
int RRMatch::UpLoadMoveInfo()
{
    Json::Value val;
    unsigned int i = 0;
    for (std::map<int, Player*>::iterator it = all_players.begin(); it != all_players.end(); it++) {
		Json::Value& tmp = val["positions"][i++];
		Player* player = it->second;
		tmp["round_id"] = m_CurTurn - 1;
		tmp["player_id"] = player->uid;
		tmp["uid"] = player->uid;
		tmp["card_rank"] = player->m_RankInfo.totalRank;
		tmp["position"] = player->m_RankInfo.totalRank;
		tmp["average_mp_ratio"] = _4float(player->m_RankInfo.meanMPRatio);
		tmp["averageRatio"] = _4float(player->m_RankInfo.meanMPRatio);
		if (m_lose_player_mp_ratio)
		{
			tmp["rival_avg_mp_ratio"] = _4float(player->m_RankInfo.meanMPRatioLosePlayers);
		}
		if (it->second->m_RankInfo.rankCount > 1)
		{
			std::stringstream stream;
			std::string result;
			stream << it->second->m_RankInfo.rankNumber << "_" << it->second->m_RankInfo.rankCount;
			stream >> result;
			tmp["rank_count"] = result;
		}
		tmp["totaltime"] = player->m_RankInfo.totalUseTime;
		tmp["status"] = player->m_GiveUp? 1 : 0;
    }
	//	log_info("%s mid:%d turn:%d data:%s\n", __FUNCTION__, matchid, CurTurn(), val.toStyledString().c_str());
	int ret = zjh.main_rc[0]->command("hset position:%d %d %s", matchid, CurTurn(), val.toStyledString().c_str());
    if (ret < 0){
        log_error("redis set mid[%d] rank error\n", matchid);
        return -1;
    }
    Jpacket packet;
    packet.val["cmd"] = SERVER_TURN_MOVE_INFO_REQ;
    packet.val["datetime"] = int(time(NULL));
    packet.val["mid"] = matchid;
    packet.val["turn"] = CurTurn();
    packet.end();
    zjh.game->SendToDataSvr(packet.tostring());
    return 0;
}

//上传实时排行榜数据
void RRMatch::UploadRealTimeRank()
{
    Json::Value val;
    unsigned int i = 0;
    val["curTurn"] = CurTurn();
    val["allTurn"] = AllTurn();
    if(m_State == MATCH_OVER){
        for (std::vector<RankInfo>::iterator it = m_TotalRankInfo.begin(); it != m_TotalRankInfo.end(); it++) {
			Json::Value& tmp = val["players"][i++];
            tmp["uid"] = (*it).uid;
            tmp["name"] = (*it).name;
            tmp["totalRank"] = (*it).totalRank;
			tmp["totalMPRankScore"] = //CalcTotalScore((*it).meanMPRatio);
				_4float((*it).meanMPRatio);
            tmp["totalUseTime"] = (*it).totalUseTime;
            tmp["promotion"] = (*it).totalRank;            
        }
    }
    else{
        for (std::map<int, Player*>::iterator it = all_players.begin(); it != all_players.end(); it++) {
            Player *p = it->second;
			Json::Value& tmp = val["players"][i++];
            tmp["uid"] = p->uid;
            tmp["name"] = p->name;
            tmp["tid"] = p->tid;
            tmp["seatid"] = p->seatid;
            tmp["curRound"] = p->m_CurRound;
            tmp["curRole"] = p->m_Role;
			tmp["totalMPRankScore"] = _4float(p->m_RankInfo.meanMPRatio);
            tmp["totalUseTime"] = p->m_RankInfo.totalUseTime;
            tmp["totalRank"] = p->m_RankInfo.totalRank;
            if(!m_bInterval){
                if(m_State == MATCH_OVER){
                    tmp["promotion"] = p->m_RankInfo.totalRank;
                }
                else{
                    tmp["promotion"] = -1;
                }
            }
            else{
				tmp["promotion"] = p->m_RankInfo.totalRank <= getPromotionNum(m_CurTurn) ? -2 : -3;
            }            
        }
    }
    int ret = zjh.main_rc[0]->command("set timeshow:%d %s", matchid, val.toStyledString().c_str());
    if (ret < 0){
        log_error("redis set mid[%d] timeshow error\n", matchid);
    }
}

//瑞士移位
void RRMatch::SwitzerlandMove()
{
	static int next_chair[2][GAME_PLAYER] = { { 0,3,2,1,}, { 3,0,1,2,} };
	for (std::map<int, Player*>::iterator it = all_players.begin(); it != all_players.end(); it++) {
		Player *player = it->second;
		int playerRank = player->m_RankInfo.totalRank;
		int tableId = (playerRank + (GAME_PLAYER - 1)) / GAME_PLAYER;
		player->tid = tableId;
		player->seatid = next_chair[tableId % 2][playerRank % GAME_PLAYER];
	}
}
void RRMatch::procSpawnCards()
{
	if (m_single_turn_split)
	{
		if (m_CurTurn == 1)
		{
			int tmp = 1;
			for (int i = 0; i < m_AllTurn; i++)
			{
				int turn = m_TurnRoundNum[i];
				for (int j = 0; j < turn; j++)
				{
					SpawnCards(j + tmp, 1);
				}
				tmp += turn;
			}
		}
	}
	else
	{
		Parent::procSpawnCards();
	}
}

/*反序列化用，用于重启*/
void RRMatch::restart()
{
	MatchExt::restart();
}
