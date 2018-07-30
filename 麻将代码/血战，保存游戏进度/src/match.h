#ifndef _MATCH_H_
#define _MATCH_H_
#include <stdlib.h>
#include <ev++.h>
#include <json/json.h>
#include <vector>
#include <string>
#include <set>
#include "common/client.h"
#include "player.h"
#include "common/jpacket.h"
#include "table.h"

class Player;

typedef enum
{
    MATCH_JOINING = 0,  //报名阶段
    MATCH_PLAYING,      //正在比赛
    MATCH_PAUSE,        //赛事暂停
    MATCH_OVER,         //赛事结束
}MatchState;

struct CardPoolArgs
{
	_uint8* cards_pool;
	_uint8 cards_size;
	_uint8 banker;
	bool same_cards;
};
//每局牌信息
struct CardsInfo
{
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive & ar, const unsigned int /* file_version */) {
		printf("CardsInfo::serialize\n");
		ar & m_RepertoryCard;
		ar & topScore;
		ar & topUName;
	}

	MJ_BUFFER<_uint8, POOL_CARD_MAX> m_RepertoryCard;
    int                     topScore[GAME_PLAYER];              //每个方位最佳得分
    std::string             topUName[GAME_PLAYER];
    CardsInfo()
	{   
        memset(topScore, 0, sizeof(int)*GAME_PLAYER);
    }
};

class Match
{
	friend class boost::serialization::access;
	BOOST_SERIALIZATION_SPLIT_MEMBER()
	template<class Archive>
	void save(Archive & ar, const unsigned int version) const
	{
		printf("Match::save\n");
		ar & matchid;
		ar & m_tmpTid;
		ar & m_MatchName;
		ar & m_online;
		ar & m_RobotRatio;
		ar & m_State;
		ar & m_TurnRoundNum;
		ar & m_PromotionNum;
		ar & m_DurationNum;
		ar & m_CompexsNum;
		ar & m_AllTurn;
		ar & m_CurTurn;
		ar & m_CurRound;
		ar & m_AllRound;
		ar & m_PlayerNum;
		ar & m_bPause;
		ar & m_bInterval;
		ar & m_lose_player_mp_ratio;
		ar & m_fixed_seat_id;
		ar & m_single_turn_split;
		ar & all_players;
		ar & all_tables;
		ar & all_robots;
		ar & m_TotalRankInfo;
		ar & tmpRank;
		ar & m_FightMode;
		ar & m_SendLastRank;
		ar & m_TurnOverTableCnt;
		ar & m_TurnCardInfo;
		std::string tableConfigStr = m_TableConfig.toStyledString();
		ar & tableConfigStr;
		ar & m_TurnOverTables;
		std::string roundinfoStr = m_roundinfo.toStyledString();
		ar & roundinfoStr;
		ar & m_bCalcMpScore;
		ar & m_bSameCard;
		ar & m_bScoreDependOnSeat;
		ar & m_cbInitCardPool;
		ar & m_fBaseScore;
		ar & m_TurnStartTimer_running;
		ar & m_NextTurnTimer_running;
		ar & m_SendRankTimer_running;
		ar & m_SendPromotionTimer_running;
		ar & m_UpLoadRealRankTimer_running;
		ar & m_NextRoundTimer_running;
	}
	template<class Archive>
	void load(Archive & ar, const unsigned int version)
	{
		printf("Match::load\n");
		ar & matchid;
		ar & m_tmpTid;
		ar & m_MatchName;
		ar & m_online;
		ar & m_RobotRatio;
		ar & m_State;
		ar & m_TurnRoundNum;
		ar & m_PromotionNum;
		ar & m_DurationNum;
		ar & m_CompexsNum;
		ar & m_AllTurn;
		ar & m_CurTurn;
		ar & m_CurRound;
		ar & m_AllRound;
		ar & m_PlayerNum;
		ar & m_bPause;
		ar & m_bInterval;
		ar & m_lose_player_mp_ratio;
		ar & m_fixed_seat_id;
		ar & m_single_turn_split;
		ar & all_players;
		ar & all_tables;
		ar & all_robots;
		ar & m_TotalRankInfo;
		ar & tmpRank;
		ar & m_FightMode;
		ar & m_SendLastRank;
		ar & m_TurnOverTableCnt;
		ar & m_TurnCardInfo;
		Json::Reader reader;
		std::string tableConfigStr;
		tableConfigStr.clear();
		ar & tableConfigStr;
		m_TableConfig.clear();
		reader.parse(tableConfigStr, m_TableConfig);
		ar & m_TurnOverTables;
		std::string roundinfoStr;
		roundinfoStr.clear();
		ar & roundinfoStr;
		m_roundinfo.clear();
		reader.parse(roundinfoStr, m_roundinfo);
		ar & m_bCalcMpScore;
		ar & m_bSameCard;
		ar & m_bScoreDependOnSeat;
		ar & m_cbInitCardPool;
		ar & m_fBaseScore;
		ar & m_TurnStartTimer_running;
		ar & m_NextTurnTimer_running;
		ar & m_SendRankTimer_running;
		ar & m_SendPromotionTimer_running;
		ar & m_UpLoadRealRankTimer_running;
		ar & m_NextRoundTimer_running;
	}

public:
	int matchid; //赛事编号
	int m_tmpTid;	//用于玩家分配房间号
    std::string m_MatchName;     //赛事名称
    int m_online;       //赛事类别 0|线下赛 1|线上赛
    int m_RobotRatio;    //机器人胜率
    MatchState m_State;         //赛事状态 0--未开赛 1--正在比赛 2--已结束
    std::vector<int> m_TurnRoundNum;  //每轮的局数
    std::vector<int> m_PromotionNum;  //每轮晋级人数第一轮为0
    std::vector<int> m_DurationNum;   //每轮间隔时间第一轮为0
    std::set<int> m_CompexsNum;    //比赛挂起的轮
    int m_AllTurn;       //赛事总轮数
    int m_CurTurn;       //赛事当前轮数
    int m_CurRound;      //赛事当前局数
    int m_AllRound;      //赛事总共局数 对应打几副牌
    int m_PlayerNum;     //参赛人数
    bool m_bPause;        //是否在下一轮之前暂停比赛
    bool m_bInterval;     //是否处于赛事间隔时间内	
	bool m_lose_player_mp_ratio;
	bool m_fixed_seat_id;
	bool m_single_turn_split;
    std::map<int, Player*> all_players;    //赛事所有参赛玩家<uid, player>
    std::map<int, Table*> all_tables;     //赛事所有比赛桌子<tid, table>
    std::set<Player*> all_robots;  //赛事机器人玩家
    std::vector<RankInfo> m_TotalRankInfo;     //最终总排名信息
    std::vector<RankInfo> tmpRank;            //临时排名信息用于玩家获取实时排行
    int m_FightMode;         //0|人机对战 1|人人对战
    int m_SendLastRank;      //是否发送最终大排名    	
    int m_TurnOverTableCnt;  //已完成当前轮的桌子数量	
	std::map<int, CardsInfo> m_TurnCardInfo;      //每轮的牌局信息
	Json::Value m_TableConfig;
	std::set<int> m_TurnOverTables;
	Json::Value m_roundinfo;
	struct
	{
		bool m_bCalcMpScore;
		bool m_bSameCard;
		bool m_bScoreDependOnSeat;
		bool m_cbInitCardPool;
		float m_fBaseScore;		
	};
	bool m_TurnStartTimer_running;
    ev_timer m_TurnStartTimer;    //比赛开始倒计时
    ev_tstamp m_TurnStartTStamp;

	bool m_NextTurnTimer_running;
    ev_timer m_NextTurnTimer;     //下一轮定时器
    ev_tstamp m_NextTurnStamp;

	bool m_SendRankTimer_running;
    ev_timer m_SendRankTimer;     //发送最终排名
    ev_tstamp m_SendRankStamp;

	bool m_SendPromotionTimer_running;
    ev_timer m_SendPromotionTimer;//发送轮次晋级
    ev_tstamp m_SendPromotionStamp;

	bool m_UpLoadRealRankTimer_running;
    ev_timer m_UpLoadRealRankTimer;//定时发送实时排名数据
    ev_tstamp m_UploadRealRankStamp;

	bool m_NextRoundTimer_running;
	ev_timer m_NextRoundTimer;//下一轮开赛
	ev_tstamp m_NextRoundStamp;
public:
    int Init(Json::Value &val);    

	int Init2(Json::Value &val);
    
    int UploadTurnRank();
    int UploadTotalRank();
    void SendPromotion();
    int MatchPause(int type);    
    void SaveMatchInfo();
	void SpawnCards(int roundid, int turnid);
    void TableGameStart(Table *table);
	const CardPoolArgs* InitCardPool(int round, int turn);

	void HandlerRankListReq(Player *player);
    void HandlerRankInfoReq(Player *player);
    void HandlerTurnOverInfo(Player *player, bool bOver = false);

	int HandlerJoinMatch(int matchid, Player *player, Json::Value& val);
	
	Json::Value GetTableConfigure(const Json::Value& remote);

	bool isTurnOver()const {
		return !m_single_turn_split || m_CurTurn == m_AllTurn;
	}
	int getPromotionNum(int turn)const {
		if (m_PromotionNum[turn] == 0)
			return m_PlayerNum;
		return m_PromotionNum[turn];
	}
	
    Match();
    virtual ~Match();
    virtual void InitPlayerTid();
    virtual void TurnReset();
    virtual void UploadRealTimeRank();
    virtual void TurnStart();
    virtual void TurnOver();
    virtual void RoundOver(Table *table);
    virtual void NextTurn();
	virtual void NextRound();
	virtual int CalcTotalScore(float ratio);
	virtual float ConvertTotalScore(int score);
	virtual void MatchOver();
	int CalcTotalScore(Player* player) { return CalcTotalScore(player->m_RankInfo.meanMPRatio); }
	virtual int CurRound()const { return m_CurRound; }
	virtual int AllRound()const { return m_AllRound; }
	virtual int CurTurn()const { return m_CurTurn; }
	virtual int AllTurn()const { return m_AllTurn; }	
	virtual void ResetTable();
	virtual void procSpawnCards();
	virtual int InitRoundNumber()const { return 1; }

    static void NextTurnCB(struct ev_loop *loop, struct ev_timer *w, int revents);
    static void TurnStartCB(struct ev_loop *loop, struct ev_timer *w, int revents);
    static void MatchSendRankCB(struct ev_loop *loop, struct ev_timer *w, int revents);
    static void SendPromotionCB(struct ev_loop *loop, struct ev_timer *w, int revents);
    static void UpLoadRealRankCB(struct ev_loop *loop, struct ev_timer *w, int revents);
	static void NextRoundCB(struct ev_loop *loop, struct ev_timer *w, int revents);

	Player* AddRobot(Table*, Player*);
	bool DelRobot(Player*);
	template<class TYPE>
	void broadcast(int iMsg, const TYPE& obj)
	{
		char buf[0x1000];
		int bodysize = obj.ByteSize();
		int headsize = PackHeader(buf, iMsg, bodysize);
		assert((headsize + bodysize) < static_cast<int>(sizeof(buf)));
		obj.SerializeToArray(buf + headsize, bodysize);
		for (std::map<int, Player*>::iterator it = all_players.begin(); it != all_players.end(); ++it)
			if (it->second != NULL && it->second->client != NULL)
				it->second->client->send(buf, headsize + bodysize); 
	}

	/*反序列化用，用于重启*/
	virtual void restart();
};

BOOST_SERIALIZATION_ASSUME_ABSTRACT(Match)

#endif
