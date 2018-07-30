#include "rr_match.h"
#include "redis_client.h"
#include "zjh.h"
#include "proto.h"


extern ZJH zjh;
extern Log log;

RRMatch::RRMatch()
{
    m_TurnStartTimer.data = this;
    m_NextTurnTimer.data = this;
    m_SendRankTimer.data = this;
    m_SendPromotionTimer.data = this;
    m_UpLoadRealRankTimer.data = this;

	m_bScoreDependOnSeat = true;

	m_fBaseScore = 100.0f;
	m_cbInitCardPool = true;

	m_bCalcMpScore = true;

	m_lose_player_mp_ratio = true;

	m_bSameCard = false;
}

RRMatch::~RRMatch()
{
    
}

//初始化玩家信息
void RRMatch::InitPlayerTid()
{
    //如果初始轮非第一轮表示挂起的比赛继续 对于人人赛需要进行瑞士移位
    if(m_CurTurn > 1){
        UploadMoveInfo();
        SwitzerlandMove();
    }
    else{
        std::vector<int> tempUids;
		std::vector<int> tempRobUids;
        for(std::map<int, Player*>::iterator it = all_players.begin(); it != all_players.end(); ++it){
			//区分机器人与选手
			if (it->second->m_isRobot == false)
			{
				tempUids.push_back(it->first);
			}
			else
			{
				tempRobUids.push_back(it->first);
			}
        }
        srand(time(NULL));
        random_shuffle(tempUids.begin(), tempUids.end());
		random_shuffle(tempRobUids.begin(), tempRobUids.end());
		int totaltid = (all_players.size()) / GAME_PLAYER;

		if (all_players.size() % GAME_PLAYER != 0)
		{
			log.error("%s %d %s all_players.size:%d players num err", __FILE__, __LINE__, __FUNCTION__, all_players.size());
			MatchOver();
			return;
		}

		log.debug("%s:%d:%s InitPlayerTid all_players.size():%d  totaltid:%d\n",__FILE__,__LINE__, __FUNCTION__, all_players.size(), totaltid);

        int tid = 1;
		int playerid = 1;
        for(size_t i = 0; i < tempUids.size(); ++i, ++playerid,++tid){
			if (tid > totaltid)
			{
				tid = 1;
			}
            all_players[tempUids[i]]->tid = tid;	
			all_players[tempUids[i]]->tmp_playerid = playerid;
            all_players[tempUids[i]]->seatid = -1;
           // log.debug("%s mid:%d init player:%d tid:%d\n", __FUNCTION__, matchid, tempUids[i], tid);
        }

		for (size_t i = 0; i < tempRobUids.size() ; ++i, ++playerid,++ tid) {
			if (tid > totaltid)
			{
				tid = 1;
			}
			all_players[tempRobUids[i]]->tid = tid;
			all_players[tempRobUids[i]]->tmp_playerid = playerid;
			all_players[tempRobUids[i]]->seatid = -1;
			//log.debug("%s mid:%d init player:%d tid:%d\n", __FUNCTION__, matchid, tempRobUids[i], tid);
		}
    }

	for (std::map<int, Player*>::iterator it = all_players.begin(); it != all_players.end(); ++it) {

		log.debug(" file:%s %d %s matchid:%d m_isRobot:%d tid:%d tmp_playerid:%d seatid:%d\n",__FILE__,__LINE__, __FUNCTION__, matchid, all_players[it->first]->m_isRobot, all_players[it->first]->tid, all_players[it->first]->tmp_playerid, all_players[it->first]->seatid);

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
	log.info("%s:%d:%s\n", __FILE__, __LINE__, __FUNCTION__);
	if (m_single_turn_split)
	{
		m_bPause = false;
		m_bInterval = false;
		m_TurnOverTableCnt = 0;
		m_PlayerNum = all_players.size();		
	}
	else
	{
		Parent::TurnReset();
	}
	for (int i = 0; i < GAME_PLAYER ; i++)
	{
		m_playersInSeat[i].clear();
	}    
    m_TurnOverTables.clear();
}

//牌桌回合结束 按方位统计分数 分发下一局的扑克
void RRMatch::RoundOver(Table *table)
{
	log.info("%s:%d:%s matchid:%d m_CurRound:%d m_room_card_score:%d\n", __FILE__, __LINE__, __FUNCTION__,table->matchid, table->m_CurRound, m_room_card_score);

	if (!m_room_card_score)//房卡模式
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
			CalculateTotalRank(m_playersInSeat[i], false, m_single_turn_split);
		}
		CalculateTotalRank3(table->m_CurRound, m_single_turn_split);
	}
	else
	{
		CalculateCommonRank();

		StatisticCommonRank();
	}
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
        if(m_CurTurn < m_AllTurn){
            m_TurnOverTables.insert(table->tid);
            for(std::set<int>::iterator it = m_TurnOverTables.begin(); it != m_TurnOverTables.end(); ++it){
                int tmpTid = *it;
                for(int i = 0; i < GAME_PLAYER; ++i){
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
				tmp_player->m_isMatchOver = true;
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

	if (table->m_CurRound == AllRound() && m_CurTurn == m_AllTurn)
	{
		for (int i = 0; i < GAME_PLAYER; i++)
		{
			Player* tmp_player = table->m_SeatPlayer[i].player;
			tmp_player->m_bPlayerMatchEnd = true;
		}
	}

	Parent::RoundOver(table);
	//int player_size = all_players.size() + all_robots.size();
	log.info("%s:%d:%s matchid:%d m_CurRound:%d\n", __FILE__, __LINE__, __FUNCTION__, table->matchid, table->m_CurRound);
	int player_size = all_players.size();
    if (m_TurnOverTableCnt == ((player_size  / GAME_PLAYER ) + (player_size % GAME_PLAYER != 0))){
        TurnOver();
    }
	else
	{
		TIMER_AGAIN(table->m_NextRound_timer, table->m_NextRound_stamp);
	}
}


//优先将机器人淘汰 并重新排名

//轮次结束 计算排名 将机器人放最后
void RRMatch::TurnOver()
{
	log.info("%s:%d:%s matchid:%d\n", __FILE__, __LINE__, __FUNCTION__,matchid);

    Parent::TurnOver();
	if (isTurnOver())
	{
		UploadMPInfo();
		if (!m_room_card_score)
		{					
			CalculateTotalRank3(-1, m_single_turn_split);
		}
		EliminateRobot();
		UploadTurnRank();
	}

    //所有轮次完毕 赛事结束
    if (m_CurTurn == m_AllTurn){
        MatchOver();
    }
    else{
        m_bInterval = true;
        ev_timer_again(zjh.loop, &m_SendPromotionTimer);
    }
}

//进入下一轮进行瑞士移位
void RRMatch::NextTurn()
{
	log.info("%s:%d:%s \n", __FILE__, __LINE__, __FUNCTION__);

    m_CurTurn++;
    m_AllRound = m_TurnRoundNum[m_CurTurn - 1];
    UploadMoveInfo();
    SwitzerlandMove();
	m_fixed_seat_id = true;
    ResetTable();
    TurnStart();

}
//上传轮移位信息
int RRMatch::UploadMoveInfo()
{
	log.info("%s:%d:%s \n", __FILE__, __LINE__, __FUNCTION__);

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
		tmp["totaltime"] = _4float(player->m_RankInfo.totalUseTime);
		tmp["status"] = player->m_GiveUp? 1 : 0;
//		log.info("%s mid:%d uid:%d mean:%f(%f,%f,%f)\n", __FUNCTION__, matchid, player->uid,
//			player->m_RankInfo.meanMPRatio, 
//			player->m_SettleInfo[1].sMPRatio, player->m_SettleInfo[2].sMPRatio, player->m_SettleInfo[3].sMPRatio);
    }
//	log.info("%s mid:%d turn:%d data:%s\n", __FUNCTION__, matchid, CurTurn(), val.toStyledString().c_str());
    int ret = zjh.main_rc[0]->command("hset position:%d %d %s", matchid, CurTurn(), val.toStyledString().c_str());
    if (ret < 0){
        log.error("redis set mid[%d] rank error\n", matchid);
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
	log.info("%s:%d:%s \n", __FILE__, __LINE__, __FUNCTION__);

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
        log.error("redis set mid[%d] timeshow error\n", matchid);
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

void RRMatch::HandlerRankListReq2(Player *player)
{

	log.info("%s:%d:%s  player->uid:%d player->m_bPlayerMatchEnd:%d  player->m_CurRound:%d m_CurTurn:%d m_AllTurn:%d\n", __FILE__, __LINE__, __FUNCTION__, player->uid, player->m_bPlayerMatchEnd, player->m_CurRound, m_CurTurn, m_AllTurn);
	if (!player->m_bPlayerMatchEnd)
	{
		return;
	}
	int uid = player->uid;
	if (all_players.find(uid) == all_players.end()) {
		return;
	}
	if (player->client == NULL)
		return;
	proto::game::MatchRankReq pMRR;
	if (!player->client->parse_proto(pMRR))
	{
		log.error("ParseFromString MatchRankReq error\n");
		return;
	}
	std::vector<RankInfo> &tmp_rank = player->m_isMatchOver  && m_CurTurn != 1 ? m_TotalRankInfo : tmpRank;
	int page = pMRR.page();
	int beginPos = page == 1 ? 0 : (page - 1) * 10;
	int endPos = page * 10;
	if (endPos > (int)tmp_rank.size()) {
		endPos = tmp_rank.size();
	}
	proto::game::MatchRankAck pMRA;
	pMRA.set_page(page);
	for (int i = beginPos; i < endPos; i++) {
		RankInfo &rank_info = tmp_rank[i];
		if (i < tmpRank.size()) {
			rank_info = tmpRank[i];
		}
		proto::game::RankInfo *pRI = pMRA.add_ranklist();
		pRI->set_rank(rank_info.totalRank);
		pRI->set_name(rank_info.name);
		int tmp = CalcTotalScore(rank_info.meanMPRatio);
		pRI->set_totalmpscore(tmp);
		pRI->set_totalusetime(rank_info.totalUseTime);
		if (all_players.find(rank_info.uid) != all_players.end())
		{
			Player* p2 = all_players[rank_info.uid];
			if (p2 != NULL)
			{
				pRI->set_avatar(p2->avatar.c_str());
				pRI->set_uid(p2->uid);
			}
		}
	}
	player->unicast(SERVER_RANK_ACK_2, pMRA);

	log.info("%s:%d:%s player.uid:%d send pMRA:%s\n", __FILE__, __LINE__, __FUNCTION__, player->uid, pMRA.DebugString().c_str());
}
