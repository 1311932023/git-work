#ifndef _PLAYER_H_
#define _PLAYER_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <ev++.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <errno.h>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <json/json.h>
#include <ev.h>
#include "libnormalmahjong/common.h"
#include "common/client.h"

#define DEFINE_MONEY (100000)
class Client;
class Table;

//每局的结算信息保留内容
struct SettleInfo
{	
	int 				sSeatId;			//当局座位号
	std::vector<int>  		sRival;				//当局对抗牌手id 以,隔开	
	int sCurMoney;			//当前牌副得分
	union 
	{
		int sMPScore;			//当前牌副所得MP分值
		int curScore;
	};	
	int sMPRatioRank;		//MP得分率排名		
	float				sMPRatio;			//MP得分率
	float				sUseTime;			//打完此副牌用时
	int					sCurRank;			//当局排名
	int					sTotalRank;			//当局总排名	
	SettleInfo()
	{
		clear();
	}
	void clear()
	{
		sSeatId = 0;
		sMPScore = 0;
		sCurRank = 1;
		sMPRatioRank = 0;
		sTotalRank = 1;
		sMPRatio = 0.0f;
		sUseTime = 0.0f;
		sCurMoney = 0;		
		sRival.clear();		
	}
};

struct RankInfo
{
	int					uid;				//用户
	std::string 		name;				//昵称
	union 
	{
		float meanMPRatio;		//平均MP得分率值
		float totalScore;
	};
	//所胜对手平均得分率
	float meanMPRatioLosePlayers;
	float				totalUseTime;		//总用时
	int					totalRank;			//总排名
	int curRank;
	short rankCount;
	short rankNumber;	
	short no1Count;
	bool bGiveUp;
	RankInfo() : uid(0)/*, giveUp(false)*/ {}
	void clear()
	{
		meanMPRatio=0.0f;
		meanMPRatioLosePlayers = 0.0f;
		totalUseTime=0.0f;
		totalRank=1;
		curRank = 0;
		rankCount = 0;
		rankNumber = 0;		
		no1Count = 0;
		bGiveUp = false;
	}
	static bool LessCB(const RankInfo &a, const RankInfo &b)
	{
		//if (a.bGiveUp ^ b.bGiveUp) {
		//	return !a.bGiveUp;
		//}
		if (a.meanMPRatio > b.meanMPRatio) {
			return true;
		}
		if (a.meanMPRatio == b.meanMPRatio
			&& a.totalUseTime < b.totalUseTime) {
			return true;
		}
		return false;
	}
	static bool LessCB2(const RankInfo &a, const RankInfo &b)
	{
		if (a.meanMPRatio > b.meanMPRatio) 
			return true;
		if (a.meanMPRatio == b.meanMPRatio)
		{
			if (a.meanMPRatioLosePlayers > b.meanMPRatioLosePlayers)
				return true;
			if (a.meanMPRatioLosePlayers == b.meanMPRatioLosePlayers)
			{
				if (a.no1Count > b.no1Count)
					return true;
				if (a.no1Count == b.no1Count)
				{
					return a.totalUseTime < b.totalUseTime;
				}
			}
		}
		return false;
	}
	static bool total_rank_compare(const RankInfo &a, const RankInfo &b)
	{
		return a.totalRank < b.totalRank;
	}

};

class Player
{
public:	
	std::string				name;
	std::string				avatar;
	int						matchid;
	int 					tid;	
	int						uid;	
	int						seatid;
	int						tmp_playerid;
	int						sex;
	int						money;	
	int						status;
	bool  					m_GiveUp;//弃赛
	bool 					m_isRobot;	
	bool					m_isMatchOver;
	std::map<int, SettleInfo>	m_SettleInfo;	
	RankInfo				m_RankInfo;//总排名信息
	std::vector<RankInfo>		m_TurnScore;
	Client *client;
	Table 					*table;	
	int						m_CurMoney;//当局输赢分
	float 					m_UseTime;//用时
	int 					m_CurRound;
	int 					m_Role;//0 : player , 1 : robot
	int 					mRank;//用于比赛挂起后人人赛分桌	
	int DisBandCount;
	int m_BaseScore;
	int m_zhigang;
	int m_angang;
	int m_wangang;
	bool m_bPlayerMatchEnd; //人人赛的时候，玩家的比赛完成。并且计算排名完成。

public:
	Player();
	virtual ~Player();
	void SetClient(Client *c);
	int Init(Json::Value &val);
	void Reset();
	void ResetMoney();
		
	void update_info(long alter_money, int alter_total_board, int alter_total_win, int alter_exp);

	void UpdateTotalRank(int curRound, int i);
	void UpdateTotalRank2(int curRound, int i);
	
	//更新所胜对手平均得分率平均值
	void UpdateLosePlayersMpRatio(bool single_turn);
	void CalculateTotalScore();
	void CalculateCommonScore();
	void CalculateBaseScore();
	void IsGiveUp();
	template<class TYPE>
	int unicast(int iMsg, const TYPE& obj)
	{
		if (client != NULL)
			return client->unicast(iMsg, obj);
		return 0;
	}
};

#endif
