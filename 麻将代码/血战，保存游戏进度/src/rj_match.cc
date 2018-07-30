#include "rj_match.h"
#include "redis_client.h"
#include "zjh.h"
#include "proto.h"
extern ZJH zjh;
extern Log log;

//BOOST_CLASS_EXPORT_GUID(RJMatch, "RJMatch")

RJMatch::RJMatch()
{
	m_bScoreDependOnSeat = true;
	m_fBaseScore = 1000.0f;
	m_bSameCard = true;
	m_cbInitCardPool = true;
	m_bCalcMpScore = true;

}


RJMatch::~RJMatch()
{    
    
}

void RJMatch::TurnReset()
{
    Parent::TurnReset();
    m_PlayersRank.clear();   
	m_TurnOverTables.clear();

}

void RJMatch::TurnStart()
{
    Parent::TurnStart();
    
	m_CurRound = 0;
	NextRound();
}



void RJMatch::InitPlayerTid()
{    
	int tmpTid = 1;
    for (std::map<int, Player*>::iterator it = all_players.begin(); it != all_players.end(); ++it, ++tmpTid) {
        Player *player = it->second;
		player->tid = tmpTid;        
		player->tmp_playerid = tmpTid;
        player->seatid = 0;
    }
}

void RJMatch::RoundOver(Table *table)
{    
    StatisticGameResult(table);	
	   
	m_TurnOverTableCnt++;
	if (m_CurRound == m_AllRound && m_CurTurn < m_AllTurn) 
	{		
		m_TurnOverTables.insert(table->tid);		
		for (std::set<int>::iterator it = m_TurnOverTables.begin(); it!= m_TurnOverTables.end(); ++it)
		{
			for (int i = 0; i < GAME_PLAYER; i++)
			{
				Player* tmp_player = all_tables[*it]->m_SeatPlayer[i].player;
				if (tmp_player != NULL && !tmp_player->m_isRobot)
				{
					HandlerTurnOverInfo(tmp_player);
					break;
				}
			}
		}
	}
	else if (m_CurRound == m_AllRound && m_CurTurn == m_AllTurn)
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
				break;
			}
		}		
	}

	Parent::RoundOver(table);	
	log_debug("matchid<%d>, m_TurnOverTableCnt<%d>, m_PlayerNum<%d> ", matchid, m_TurnOverTableCnt, m_PlayerNum);
	if (m_TurnOverTableCnt == m_PlayerNum) {
		if (m_CurRound == m_AllRound)
		{
			TurnOver();
		}
		else 
		{
			m_NextRoundTimer_running = true;
			ev_timer_again(zjh.loop, &m_NextRoundTimer);
		}
	}
}


void RJMatch::TurnOver()
{
    //将当前轮mp和rank存入redis并通知平台    
	Parent::TurnOver();	
	UploadMPInfo();
	UploadTurnRank();
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

void RJMatch::NextTurn()
{
    m_CurTurn++;
    m_AllRound = m_TurnRoundNum[m_CurTurn - 1];
    InitPlayerTid();
	m_fixed_seat_id = true;
    ResetTable();
    TurnStart();
}

void RJMatch::NextRound()
{
	Parent::NextRound();
	m_PlayersRank.clear();
}

void RJMatch::UploadRealTimeRank()
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
            tmp["curRole"] = 1;
            tmp["totalRank"] = (*it).totalRank;
            tmp["totalMPRankScore"] = (*it).meanMPRatio;
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
            tmp["curRound"] = CurRound();
            tmp["curRole"] = 1;
            tmp["totalMPRankScore"] = p->m_RankInfo.meanMPRatio;
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

/*反序列化用，用于重启*/
void RJMatch::restart()
{
	MatchExt::restart();
}
