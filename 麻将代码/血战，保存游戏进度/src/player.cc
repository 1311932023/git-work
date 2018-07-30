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
#include "common/log.h"
#include "proto/proto.h"
#include "common/client.h"
#include "player.h"
#include "game.h"
#include "match.h"

extern ZJH zjh;
extern Log log;

#define PROMOTION_RATE (0.75f)

Player::Player()
{
	client = NULL;
	matchid = 0;
	tid = 0;
	//gameid = 0;
	m_GiveUp = false;
	m_Role = 0;
	m_isRobot = 0;
	//mCurRound = 1;
	mRank = 1;
	//m_UserTimer.data = this;
	seatid = -1;
	m_CurRound = 1;
	DisBandCount = 0;
	//ev_timer_init(&m_UserTimer, Player::UseTimerCB, m_UserTStamp, m_UserTStamp);
	table = NULL;

	m_CurMoney = 0;
	m_UseTime = 0.0f;

	m_BaseScore = 0;
	m_zhigang = 0;
	m_angang = 0;
	m_wangang = 0;
	tmp_playerid = 0;

}

Player::~Player()
{
	//ev_timer_stop(zjh.loop, &m_UserTimer);
	if (client)
	  client->player = NULL;
}

// void Player::UseTimerCB(struct ev_loop *loop, struct ev_timer *w, int revents)
// {
// 	Player *p = (Player*)w->data;
// 	ev_timer_stop(zjh.loop, &p->m_UserTimer);
// }

void Player::SetClient(Client *c)
{
	client = c;
	client->player = this;
	client->uid = uid;
}

int Player::Init( Json::Value &val )
{
	Reset();
	m_isRobot = false;
	if (!val["type"].isNull())
	{
		int type = val["type"].asInt();
		if (type == 11)
			m_isRobot = true;
	}
	uid = val["uid"].asInt();
	name = val["uname"].asString();
	avatar = val["ulogo"].asString();
	sex = val["usex"].asInt();
	//type = val["type"].asInt();
	if(!val["rank"].isNull()){
		mRank = val["rank"].asInt();
	}
	m_SettleInfo.clear();
	m_RankInfo.clear();

	m_RankInfo.name = name;
	m_RankInfo.uid = uid;

	money = DEFINE_MONEY;
	return 0;
}

void Player::Reset()
{
	m_CurMoney = 0;	
	m_UseTime = 0;
	m_BaseScore = 0;	
}

void Player::ResetMoney()
{
	m_CurMoney = 0;	
	m_UseTime = 0;
	money = DEFINE_MONEY;
	m_BaseScore = 0;
	m_zhigang = 0;
	m_angang = 0;
	m_wangang = 0;
}
void Player::UpdateTotalRank(int curRound, int i)
{
	m_RankInfo.totalRank = i;
	if (m_SettleInfo.find(curRound) != m_SettleInfo.end()) {
		m_SettleInfo[curRound].sTotalRank = i;
	}
	proto::game::UpdateRank pUR;
	pUR.set_rank(i);	
	unicast(SERVER_UPDATE_RANK_UC, pUR);
	log_info("%s cur_round:%d player:%d update total rank:%d score:%d score:%f\n",__FUNCTION__, m_CurRound,
		uid, i, m_SettleInfo[curRound].sCurMoney, m_RankInfo.meanMPRatio);
}
void Player::UpdateTotalRank2(int i)
{
	m_RankInfo.totalRank = i;
}

int Calc_CurMoney(map<int, SettleInfo>& settle)
{
	int money = 0;
	for (std::map<int, SettleInfo>::iterator it = settle.begin(); it != settle.end(); ++it)
	{
		money += it->second.sCurMoney;
	}
	return money;
}
void Player::UpdateLosePlayersMpRatio()
{
	//Table* t = table;
	float total_score = 0;
	int total_count = 0;
	int curmoney = Calc_CurMoney(m_SettleInfo);
	for (_uint8 c = table->NextChair(seatid); c != seatid; c = table->NextChair(c))
	{
		Player* p = table->m_SeatPlayer[c].player;
		if (p != NULL)
		{	
			if (Calc_CurMoney(p->m_SettleInfo) < curmoney)
			{
				total_score += p->m_RankInfo.meanMPRatio;
				total_count ++;
			}
		}
	}
	if (total_count != 0)
		m_RankInfo.meanMPRatioLosePlayers = total_score / total_count;
	else
		m_RankInfo.meanMPRatioLosePlayers = 0.0f;

}

void Player::CalculateTotalScore()
{		
	float totalMPRatio = 0.0f;
	for (std::map<int, SettleInfo>::iterator it = m_SettleInfo.begin(); it != m_SettleInfo.end(); ++it) {
		totalMPRatio += it->second.sMPRatio;
		m_RankInfo.totalUseTime += it->second.sUseTime;
	}
	m_RankInfo.meanMPRatio = totalMPRatio / m_CurRound;
	log_info("%s matchid:%d uid:%d curmoney:%d money:%d MeanMPRatio:%f TotalUseTime:%f\n", __FUNCTION__, matchid, uid, m_CurMoney, money,
		m_RankInfo.meanMPRatio, m_RankInfo.totalUseTime);
}
void Player::CalculateCommonScore()
{	
	m_RankInfo.totalScore = 0.0f;
	m_RankInfo.totalUseTime = 0.0f;
	for (std::map<int, SettleInfo>::iterator it = m_SettleInfo.begin(); it != m_SettleInfo.end(); ++it) 
	{
		if (m_CurRound == 1 && !m_TurnScore.empty() )
		{			
			float last_rank = 0.0f;
			std::vector<RankInfo>::iterator last = m_TurnScore.end();
			last--;
			last_rank = PROMOTION_RATE * last->totalScore;
			m_RankInfo.totalScore += last_rank;
		}
		m_RankInfo.totalScore += (float)it->second.sCurMoney;
		m_RankInfo.totalUseTime += it->second.sUseTime;
	}	
	log_info("%s matchid:%d uid:%d curmoney:%d money:%d MeanMPRatio:%f TotalUseTime:%f\n", __FUNCTION__, matchid, uid, m_CurMoney, money,
		m_RankInfo.meanMPRatio, m_RankInfo.totalUseTime);
}
void Player::CalculateBaseScore()
{	
	m_RankInfo.totalScore = 0.0f;
	m_RankInfo.totalUseTime = 0.0f;
	for (std::map<int, SettleInfo>::iterator it = m_SettleInfo.begin(); it != m_SettleInfo.end(); ++it)
	{		
		m_RankInfo.totalScore += (float)it->second.sCurMoney;
		m_RankInfo.totalUseTime += it->second.sUseTime;
	}
	log_info("%s matchid:%d uid:%d curmoney:%d money:%d MeanMPRatio:%f TotalUseTime:%f\n", __FUNCTION__, matchid, uid, m_CurMoney, money,
		m_RankInfo.meanMPRatio, m_RankInfo.totalUseTime);
}
void Player::update_info(long alter_money, int alter_total_board, int alter_total_win, int alter_exp)
{	
	int old_money = money;
	money += alter_money;	
	alter_exp += alter_exp;
	m_CurMoney += alter_money;
	log_info("%s uid:%d old_money:%d alter_money:%d money:%d\n", __FUNCTION__, uid, old_money, alter_money, money);	
}
