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
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive & ar, const unsigned int /* file_version */) {
		printf("SettleInfo::serialize\n");
		ar & sSeatId;
		ar & sRival;
		ar & sCurMoney;
		ar & sMPScore;
		ar & sMPRatioRank;
		ar & sMPRatio;
		ar & sUseTime;
		ar & sCurRank;
		ar & sTotalRank;
	}

	int 				sSeatId;			//当局座位号
	std::string   		sRival;				//当局对抗牌手id 以,隔开	
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
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive & ar, const unsigned int /* file_version */) {
		printf("RankInfo::serialize\n");
		ar & uid;
		ar & name;
		ar & meanMPRatio;
		ar & meanMPRatioLosePlayers;
		ar & totalUseTime;
		ar & totalRank;
		ar & curRank;
		ar & rankCount;
		ar & rankNumber;
	}

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
	RankInfo() : uid(0)/*, giveUp(false)*/ 
	{
		name.clear();
		meanMPRatio = 0.0f;
		meanMPRatioLosePlayers = 0.0f;
		totalUseTime = 0.0f;
		totalRank = 1;
		curRank = 0;
		rankCount = 0;
		rankNumber = 0;
	}
	void clear()
	{
		meanMPRatio=0.0f;
		meanMPRatioLosePlayers = 0.0f;
		totalUseTime=0.0f;
		totalRank=1;
		curRank = 0;
		rankCount = 0;
		rankNumber = 0;		
	}
	static bool LessCB(const RankInfo &a, const RankInfo &b)
	{
		if (a.meanMPRatio > b.meanMPRatio) {
			return true;
		}
		if (a.meanMPRatio == b.meanMPRatio
			&& a.totalUseTime < b.totalUseTime) {
			return true;
		}
		return false;
	}
};

class Player
{
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive & ar, const unsigned int /* file_version */) {
		printf("Player::serialize\n");
		ar & name;
		ar & avatar;
		ar & matchid;
		ar & tid;
		ar & uid;
		ar & seatid;
		ar & tmp_playerid;
		ar & sex;
		ar & money;
		ar & status;
		ar & m_GiveUp;
		ar & m_isRobot;
		ar & m_SettleInfo;
		ar & m_RankInfo;
		ar & m_TurnScore;
		ar & table;
		ar & m_CurMoney;
		ar & m_UseTime;
		ar & m_CurRound;
		ar & m_Role;
		ar & mRank;
		ar & DisBandCount;
		ar & m_BaseScore;
		ar & m_zhigang;
		ar & m_angang;
		ar & m_wangang;
	}

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
public:
	Player();
	virtual ~Player();
	void SetClient(Client *c);
	int Init(Json::Value &val);
	void Reset();
	void ResetMoney();
		
	void update_info(long alter_money, int alter_total_board, int alter_total_win, int alter_exp);

	void UpdateTotalRank(int curRound, int i);//m_RankInfo	
	void UpdateTotalRank2(int i);//m_RankInfo	
	//更新所胜对手平均得分率平均值
	void UpdateLosePlayersMpRatio();
	void CalculateTotalScore();//m_RankInfo
	void CalculateCommonScore();//m_RankInfo
	void CalculateBaseScore();//m_RankInfo

	template<class TYPE>
	int unicast(int iMsg, const TYPE& obj)
	{
		if (client != NULL)
			return client->unicast(iMsg, obj);
		return 0;
	}
};

#endif
