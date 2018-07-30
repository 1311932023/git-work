#include <time.h>
#include "proto/proto.h"
#include "common/client.h"
#include "common/log.h"
#include "redis_client.h"
#include "player.h"
#include "match.h"
#include "zjh.h"
#include "game.h"

extern ZJH zjh;
extern Log log;
extern bool g_period_match;

Match::Match():
	m_TurnStartTStamp(3),
	m_NextTurnStamp(10),
	m_SendRankStamp(60),
	m_SendPromotionStamp(2),
	m_UploadRealRankStamp(5),
	m_NextRoundStamp(4.5f)
{
	matchid = 0;
	m_tmpTid = 0;
	m_MatchName = "";
	m_online = 0;
	m_RobotRatio = 0;
	m_State = MATCH_JOINING;
	m_TurnRoundNum.clear();
	m_PromotionNum.clear();
	m_DurationNum.clear();
	m_CompexsNum.clear();
	m_AllTurn = 0;
	m_CurTurn = 0;
	m_CurRound = 0;
	m_AllRound = 0;
	m_PlayerNum = 0;
	m_bPause = false;
	m_bInterval = false;
	m_lose_player_mp_ratio = false;
	m_fixed_seat_id = false;
	m_single_turn_split = false;
	all_players.clear();
	all_tables.clear();
	all_robots.clear();
	m_TotalRankInfo.clear();
	tmpRank.clear();
	m_FightMode = 0;
	m_SendLastRank = 0;
	m_TurnOverTableCnt = 0;
	m_TurnCardInfo.clear();
	m_TableConfig.clear();
	m_TurnOverTables.clear();
	m_roundinfo.clear();
	m_bCalcMpScore = true;
	m_bSameCard = false;
	m_bScoreDependOnSeat = false;
	m_cbInitCardPool = false;
	m_fBaseScore = 1.0f;

	m_TurnStartTimer_running = false;
	m_TurnStartTimer.data = this;
	m_SendRankTimer_running = false;
	m_SendRankTimer.data = this;
	m_NextTurnTimer_running = false;
	m_NextTurnTimer.data = this;
	m_SendPromotionTimer_running = false;
	m_SendPromotionTimer.data = this;
	m_UpLoadRealRankTimer_running = false;
	m_UpLoadRealRankTimer.data = this;
	m_NextRoundTimer_running = false;
	m_NextRoundTimer.data = this;
    ev_timer_init(&m_TurnStartTimer, Match::TurnStartCB, m_TurnStartTStamp, m_TurnStartTStamp);
    ev_timer_init(&m_NextTurnTimer, Match::NextTurnCB, m_NextTurnStamp, m_NextTurnStamp);
    ev_timer_init(&m_SendRankTimer, Match::MatchSendRankCB, m_SendRankStamp, m_SendRankStamp);
    ev_timer_init(&m_SendPromotionTimer, Match::SendPromotionCB, m_SendPromotionStamp, m_SendPromotionStamp);
    ev_timer_init(&m_UpLoadRealRankTimer, Match::UpLoadRealRankCB, m_UploadRealRankStamp, m_UploadRealRankStamp);
	ev_timer_init(&m_NextRoundTimer, Match::NextRoundCB, m_NextRoundStamp, m_NextRoundStamp);
}

Match::~Match()
{
    ev_timer_stop(zjh.loop, &m_TurnStartTimer);
    ev_timer_stop(zjh.loop, &m_NextTurnTimer);
    ev_timer_stop(zjh.loop, &m_SendRankTimer);
    ev_timer_stop(zjh.loop, &m_SendPromotionTimer);
    ev_timer_stop(zjh.loop, &m_UpLoadRealRankTimer);
	ev_timer_stop(zjh.loop, &m_NextRoundTimer);
    log_info("~match mid[%d] destrutor\n",matchid);
}
int Match::Init(Json::Value &val)
{
	//初始化赛事
	matchid = val["minfo"]["mid"].asInt();
	m_PlayerNum = val["uinfo"].size();
	m_AllTurn = val["minfo"]["turns"].asInt();
	m_MatchName = val["minfo"]["name"].asString();
	m_online = val["minfo"]["online"].asInt();
	m_RobotRatio = val["minfo"]["robotRatio"].asInt();
	m_FightMode = val["minfo"]["fightMode"].asInt();
	m_SendLastRank = 1;
	if (!val["minfo"]["withLastRank"].isNull()) {
		m_SendLastRank = val["minfo"]["withLastRank"].asInt();
	}
	for (size_t i = 0; i < val["minfo"]["rounds"].size(); ++i) {
		m_TurnRoundNum.push_back(val["minfo"]["rounds"][i].asInt());
	}
	for (size_t i = 0; i < val["minfo"]["promotions"].size(); ++i) {
		m_PromotionNum.push_back(val["minfo"]["promotions"][i].asInt());
	}
	for (size_t i = 0; i < val["minfo"]["durations"].size(); ++i) {
		m_DurationNum.push_back(val["minfo"]["durations"][i].asInt());
	}
	if (!val["minfo"]["complexs"].isNull()) {
		for (size_t i = 0; i < val["minfo"]["complexs"].size(); ++i) {
			m_CompexsNum.insert(val["minfo"]["complexs"][i].asInt());
		}
	}
	m_single_turn_split = val["minfo"]["singleTurn"].asInt() != 0;
	//人人赛必须满足3的倍数
	if (m_PlayerNum == 0) {
		log_error("%s match:%d init error mFightMode:%d mPlayerNum:%d\n", __FUNCTION__, matchid, m_FightMode, m_PlayerNum);
		Jpacket packet_end;
		packet_end.val["cmd"] = SERVER_MATCH_EXCEPTION_REQ;
		packet_end.val["mid"] = matchid;
		packet_end.val["datetime"] = (int)time(NULL);
		packet_end.end();
		zjh.game->SendToDataSvr(packet_end.tostring());
		return -1;
	}
	all_players.clear();
	for (size_t i = 0; i < val["uinfo"].size(); ++i) {
		Player *player = new(std::nothrow)Player();
		player->Init(val["uinfo"][i]);
		player->matchid = matchid;
		all_players[player->uid] = player;
		log_info("%s init mid:%d player:%d succ\n", __FUNCTION__, matchid, player->uid);
	}
	
    m_CurTurn = 1;
    //若服务器指定轮数则从该轮开始
    if(!val["minfo"]["currentTurn"].isNull()){
        m_CurTurn = val["minfo"]["currentTurn"].asInt();
        if(m_CurTurn < 1){
            log_error("mid[%d] curTurn[%d] error\n", matchid, m_CurTurn);
            m_CurTurn = 1;
        }
    }
    m_CurRound = 1;
    m_AllRound = m_TurnRoundNum[m_CurTurn - 1];
	m_TurnStartTimer_running = true;
    ev_timer_again(zjh.loop, &m_TurnStartTimer);
    //线下赛开启实时排行
    if(m_online == 0){
        UploadRealTimeRank();
		m_UpLoadRealRankTimer_running = true;
        ev_timer_again(zjh.loop, &m_UpLoadRealRankTimer);
    }

	m_TableConfig = val["rinfo"]["match_config"];
	m_TableConfig["default"] = val["rinfo"]["plat_config"];
	

    InitPlayerTid();
    ResetTable();
    
	log_info("%s roundinfo round:%d/%d turn:%d/%d\n", __FUNCTION__, m_CurRound, m_AllRound, m_CurTurn, m_AllTurn);
    return 1;
}
int Match::Init2(Json::Value &val)
{
	//初始化赛事
	matchid = val["minfo"]["mid"].asInt();
	m_PlayerNum = val["uinfo"].size();
	m_AllTurn = val["minfo"]["turns"].asInt();
	m_MatchName = val["minfo"]["name"].asString();
	m_online = val["minfo"]["online"].asInt();
	m_RobotRatio = val["minfo"]["robotRatio"].asInt();
	m_FightMode = val["minfo"]["fightMode"].asInt();
	m_SendLastRank = 1;
	if (!val["minfo"]["withLastRank"].isNull()) {
		m_SendLastRank = val["minfo"]["withLastRank"].asInt();
	}
	for (size_t i = 0; i < val["minfo"]["rounds"].size(); ++i) {
		m_TurnRoundNum.push_back(val["minfo"]["rounds"][i].asInt());
	}
	for (size_t i = 0; i < val["minfo"]["promotions"].size(); ++i) {
		m_PromotionNum.push_back(val["minfo"]["promotions"][i].asInt());
	}
	for (size_t i = 0; i < val["minfo"]["durations"].size(); ++i) {
		m_DurationNum.push_back(val["minfo"]["durations"][i].asInt());
	}
	if (!val["minfo"]["complexs"].isNull()) {
		for (size_t i = 0; i < val["minfo"]["complexs"].size(); ++i) {
			m_CompexsNum.insert(val["minfo"]["complexs"][i].asInt());
		}
	}
	
	m_CurTurn = 1;
	//若服务器指定轮数则从该轮开始
	if (!val["minfo"]["currentTurn"].isNull()) {
		m_CurTurn = val["minfo"]["currentTurn"].asInt();
		if (m_CurTurn < 1) {
			log_error("mid[%d] curTurn[%d] error\n", matchid, m_CurTurn);
			m_CurTurn = 1;
		}
	}
	else
	{
		m_CurTurn = 1;
	}
	m_CurRound = 1;
	m_AllRound = m_TurnRoundNum[m_CurTurn - 1];	

	m_TableConfig = val["rinfo"]["match_config"];
	m_TableConfig["default"] = val["rinfo"]["plat_config"];

	TurnStart();
	//线下赛开启实时排行
	if (m_online == 0) {
		UploadRealTimeRank();
		m_UpLoadRealRankTimer_running = true;
		ev_timer_again(zjh.loop, &m_UpLoadRealRankTimer);
	}		
	return 1;
}
void Match::TurnStart()
{
	log_info("%s roundinfo round:%d/%d turn:%d/%d\n", __FUNCTION__, m_CurRound, m_AllRound, m_CurTurn, m_AllTurn);
    TurnReset();	
}

void Match::InitPlayerTid()
{
	
}

void Match::RoundOver(Table *table)
{
    if(all_tables.find(table->tid) == all_tables.end()){
        log_error("mid[%d] tid[%d] RoundOver error\n", matchid, table->tid);
    }

	if (m_bCalcMpScore)
		table->CalcMPInfo(m_roundinfo);

	table->GameEndReset();
}

void Match::TurnOver()
{
	for (std::set<Player*>::iterator it = all_robots.begin();
		it != all_robots.end(); ++it)
	{
		Player* player = *it;
		if (player->table != NULL)
		{
			player->table->Stand_Up(player);		}		
		delete player;
		log_info("%s delete robot:%p\n", __FUNCTION__, player);
	}	
	all_robots.clear();
	log_info("%s roundinfo round:%d/%d turn:%d/%d\n", __FUNCTION__, m_CurRound, m_AllRound, m_CurTurn, m_AllTurn);


}

void Match::TurnReset()
{
    m_CurRound = 1;
    m_bPause = false;
    m_bInterval = false;
    for(std::map<int, Player*>::iterator it = all_players.begin(); it != all_players.end(); it++){
        it->second->m_SettleInfo.clear();
        it->second->m_RankInfo.clear();
		it->second->money = DEFINE_MONEY;
    }    
    m_TurnOverTableCnt = 0;
    m_PlayerNum = all_players.size();
    tmpRank.clear();
	log_info("%s roundinfo round:%d/%d turn:%d/%d\n", __FUNCTION__, m_CurRound, m_AllRound, m_CurTurn, m_AllTurn);
}

void Match::NextTurn()
{
	
}
void Match::NextRound()
{
	m_CurRound++;
	if (m_CurRound > m_AllRound) {
		return;
	}
	m_TurnOverTableCnt = 0;	
	
	for (std::map<int, Table*>::iterator it = all_tables.begin(); it != all_tables.end(); ++it) {
		if (it->second == NULL) continue;
		it->second->m_CurRound = m_CurRound;
		TableGameStart(it->second);
	}
	log_info("%s roundinfo round:%d/%d turn:%d/%d\n", __FUNCTION__, m_CurRound, m_AllRound, m_CurTurn, m_AllTurn);
}
void Match::UploadRealTimeRank()
{

}

int Match::CalcTotalScore(float ratio)
{	
	return int(ratio * m_fBaseScore);
}
float Match::ConvertTotalScore(int score)
{
	return score / m_fBaseScore;
}
void Match::HandlerRankInfoReq(Player *player)
{
    if (all_players.find(player->uid) == all_players.end()) {
        return;
    }
    proto::game::DetailAck pDA;
    for (std::map<int, SettleInfo>::iterator it = player->m_SettleInfo.begin(); it != player->m_SettleInfo.end(); ++it) {
        proto::game::DetailInfo *pDI = pDA.add_rankinfo();
        pDI->set_round(it->first);
        pDI->set_score(it->second.sCurMoney);
        pDI->set_roundrank(it->second.sCurRank);
        pDI->set_totalrank(it->second.sTotalRank);
		if (m_bScoreDependOnSeat)
		{
			pDI->set_bestname(m_TurnCardInfo[it->first].topUName[it->second.sSeatId]);
			pDI->set_bestscore(m_TurnCardInfo[it->first].topScore[it->second.sSeatId]);
		}
		else
		{
			pDI->set_bestname(m_TurnCardInfo[it->first].topUName[0]);
			pDI->set_bestscore(m_TurnCardInfo[it->first].topScore[0]);
		}
    }    
	if (g_period_match)
	{
		for (std::vector<RankInfo>::iterator it = 
			m_TotalRankInfo.begin(); it!=m_TotalRankInfo.end();++it)
		{
			if (it->uid == player->uid)
			{
				pDA.set_currank(player->m_RankInfo.totalRank);
				pDA.set_totalrank(player->m_TurnScore[0].totalRank);
			}
		}
	}
	player->unicast(SERVER_RANK_ACK, pDA);
}

//产牌
void Match::SpawnCards(int roundid, int turnid)
{
	if (all_tables.empty())
	{
		log_error("%s spawn cards no table\n", __FUNCTION__);
		return;
	}

	Table* table = all_tables.begin()->second;
	if (table == NULL)
	{
		log_error("%s spawn cards table is null\n", __FUNCTION__);
		return;
	}

    srand(time(NULL) + matchid);
	CardsInfo& rcard = m_TurnCardInfo[roundid];

	MJ_BUFFER<_uint8, POOL_CARD_MAX>& cpool = rcard.m_RepertoryCard;
	
	table->m_CardPool.InitPool();
	if (!m_FightMode)
	{
		std::vector<_uint8> robotPos;
		for (int i = 1; i < table->m_GAME_PLAYER; i++)
		{
			robotPos.push_back(i);
		}
		table->m_CardPool.InitPool(MJ_HONG_ZHONG, 1, robotPos);
	}

	_uint8 cards[150];
	int iSize = table->m_CardPool.GetCard(cards, 150);

	cpool.Clear();
	cpool.Add(cards, iSize);	
	
}
//发牌并开始游戏
void Match::TableGameStart(Table *table)
{
    if(table == NULL) return;
    if(all_tables.find(table->tid) == all_tables.end()){
        log_error("mid[%d] sendcard error table not in map\n");
        return;
    }
  
	Player* player = NULL;
	for (int i = 0; i < table->m_GAME_PLAYER; i++)
	{		
		if (table->m_SeatPlayer[i].player != NULL)
		{
			table->PlayerLogin(table->m_SeatPlayer[i].player, false);
			player = table->m_SeatPlayer[i].player;
		}
	}
	if (player == NULL)
		return;
	while (table->SeatPlayerCount() < table->m_GAME_PLAYER)
		AddRobot(table, player);
	
	if (table->m_bGameState != GAME_FREE)
	{
		log_error("%s mid:%d tid:%d game has start already\n", __FUNCTION__, matchid, table->tid);
		return;
	}
    table->GameStart();
	table->m_trustee.get()->Init();
	
	/*if (table->SeatPlayerCount() == table->m_GAME_PLAYER);
	{
		table->m_trustee.get()->TriggerTimer(INVALID_CHAIRID);
		log_info("%s roundinfo trigger timer\n", __FUNCTION__);
	}*/
}
const CardPoolArgs* Match::InitCardPool(int round, int turn)
{
	static CardPoolArgs r;
	CardsInfo& cardinfo = m_TurnCardInfo[round];
	MJ_BUFFER<_uint8, POOL_CARD_MAX>& cpool = cardinfo.m_RepertoryCard;
	r.cards_size = cpool.Size();
	r.cards_pool = cpool.Front();
	r.banker = (round - 1) % GAME_PLAYER;
	r.same_cards = m_bSameCard;
	return &r;
}
void Match::HandlerRankListReq(Player *player)
{
    int uid = player->uid;
    if (all_players.find(uid) == all_players.end()) {
        return;
    }
	if (player->client == NULL)
		return;
    proto::game::MatchRankReq pMRR;    
	if (!player->client->parse_proto(pMRR))
	{
        log_error("ParseFromString MatchRankReq error\n");
        return;
    }
    int page = pMRR.page();
    int beginPos = page == 1 ? 0 : (page - 1) * 10;
    int endPos = page * 10;
    if(endPos > (int)tmpRank.size()){
        endPos = tmpRank.size();
    }
    proto::game::MatchRankAck pMRA;
    pMRA.set_page(page);
    for(int i = beginPos; i < endPos; i++){
        proto::game::RankInfo *pRI = pMRA.add_ranklist();
        pRI->set_rank(tmpRank[i].totalRank);
        pRI->set_name(tmpRank[i].name);
		int tmp = CalcTotalScore(tmpRank[i].meanMPRatio);
        pRI->set_totalmpscore(tmp);
        pRI->set_totalusetime(tmpRank[i].totalUseTime);
    }    
	player->unicast(SERVER_RANKLIST_ACK, pMRA);
}

void Match::HandlerTurnOverInfo(Player *player, bool bOver)
{
    proto::game::MatchTurnOver pMatchTurnOver;    
    pMatchTurnOver.set_left_table(all_tables.size() - m_TurnOverTableCnt);
    proto::game::NextTurnInfo *pCurTurnInfo = pMatchTurnOver.mutable_cur_turn();
	pCurTurnInfo->set_turnid(CurTurn());
    pCurTurnInfo->set_join_num(all_players.size());
    pCurTurnInfo->set_promotion_num(getPromotionNum(m_CurTurn));
    proto::game::NextTurnInfo *pNextTurnInfo = pMatchTurnOver.mutable_next_turn();
    pNextTurnInfo->set_turnid(CurTurn() + 1);
    pNextTurnInfo->set_join_num(getPromotionNum(m_CurTurn));
    if(m_CurTurn + 1 == m_AllTurn){
        pNextTurnInfo->set_promotion_num(1);
    }
    else{
        pNextTurnInfo->set_promotion_num(getPromotionNum(m_CurTurn + 1));
    }
    pMatchTurnOver.set_cur_rank(player->m_RankInfo.totalRank);
    if(m_NextTurnTimer.active != 0){
        pMatchTurnOver.set_promotion(1);//淘汰玩家已被移除 所以此时玩家全部是晋级的
        pMatchTurnOver.set_left_time(ev_timer_remaining(zjh.loop, &m_NextTurnTimer));
    }
    else if(bOver){
        pMatchTurnOver.set_promotion(player->m_RankInfo.totalRank <= getPromotionNum(m_CurTurn) ? 1 : 2);
        pMatchTurnOver.set_left_time(m_DurationNum[m_CurTurn]);
    }  
	else
	{
		pMatchTurnOver.set_promotion(0);
		pMatchTurnOver.set_left_time(m_DurationNum[m_CurTurn]);
	}	
	pMatchTurnOver.set_score(CalcTotalScore(player));	
		
	log_info("%s mid:%d uid:%d score:%f\n", __FUNCTION__, matchid, player->uid, player->m_RankInfo.meanMPRatio);
	player->unicast(SERVER_TURN_OVER_BC, pMatchTurnOver);
}

//上传轮排名
int Match::UploadTurnRank()
{
    Json::Value val;
    size_t i = 0;
    m_TotalRankInfo.clear();
	for (std::map<int, Player*>::iterator it = all_players.begin(); it != all_players.end(); it++) {
		Json::Value& tmp = val["ranks"][i++];
		tmp["uid"] = it->first;
		tmp["rank"] = it->second->m_RankInfo.totalRank;
		tmp["withTurnUp"] = it->second->m_RankInfo.totalRank > getPromotionNum(m_CurTurn) ? 0 : 1;
		tmp["score"] = CalcTotalScore(it->second->m_RankInfo.totalScore);
		tmp["averageRatio"] = _4float(it->second->m_RankInfo.totalScore);
		tmp["totalMPRatioScore"] = _4float(it->second->m_RankInfo.totalScore);
		tmp["average_mp_ratio"] = _4float(it->second->m_RankInfo.totalScore);
		if (m_lose_player_mp_ratio)
		{
			tmp["rival_avg_mp_ratio"] = _4float(it->second->m_RankInfo.meanMPRatioLosePlayers);
		}

		if (it->second->m_RankInfo.rankCount > 1)
		{
			std::stringstream stream;
			std::string result;
			stream << it->second->m_RankInfo.rankNumber << "_" << it->second->m_RankInfo.rankCount;
			stream >> result;
			tmp["rank_count"] = result;
		}
		tmp["totalTime"] = _4float(it->second->m_RankInfo.totalUseTime);

		m_TotalRankInfo.push_back(it->second->m_RankInfo);
	}
	string tmp = val.toStyledString();	
    int ret = zjh.main_rc[0]->command("hset rank:%d %d %s", matchid, CurTurn(), tmp.c_str());
    if (ret < 0){
        log_error("redis set mid[%d] rank error\n", matchid);
        return -1;
    }
//	log_info("%s mid:%d rank %s\n", __FUNCTION__, matchid, tmp.c_str());

    Jpacket packet;
    packet.val["cmd"] = SERVER_TURN_RANK_INFO_REQ;
    packet.val["datetime"] = int(time(NULL));
    packet.val["mid"] = matchid;
    packet.val["turn"] = CurTurn();
    packet.end();
    zjh.game->SendToDataSvr(packet.tostring());
		
    //保存总排名
    Json::Value mval;
    i = 0;
    if(CurTurn() == 1){	
        for(std::vector<RankInfo>::iterator it_r = m_TotalRankInfo.begin(); it_r != m_TotalRankInfo.end(); it_r++){
			Json::Value& tmp = mval[i++];
			tmp["uid"] = it_r->uid;
			tmp["name"] = it_r->name;
			tmp["totalUseTime"] = _4float(it_r->totalUseTime);
			tmp["totalRank"] = it_r->totalRank;
			tmp["totalMPRatioScore"] = 	_4float(it_r->meanMPRatio);
			tmp["average_mp_ratio"] = _4float(it_r->meanMPRatio);
			tmp["averageRatio"] = _4float(it_r->meanMPRatio);
			if (m_lose_player_mp_ratio)
			{
				tmp["rival_avg_mp_ratio"] = _4float(it_r->meanMPRatioLosePlayers);
			}

			if (it_r->rankCount > 1)
			{
				std::stringstream stream;
				std::string result;
				stream << it_r->rankNumber << "_" << it_r->rankCount;
				stream >> result;
				tmp["rank_count"] = result;
			}            
        }
    }
    else{
        ret = zjh.main_rc[0]->command("get m_totalRank:%d", matchid);
        if(ret < 0){
            log_error("redis get mid[%d] totalRank error\n", matchid);
            return -1;
        }
        std::string strminfo = zjh.main_rc[0]->reply->str;
        Json::Reader reader;
        if(reader.parse(strminfo, mval) < 0){
            log_error("parse totalRank[%d] error\n", matchid);
            return -1;
        }
        //log_debug("get redis totalRank[%s]\n", mval.toStyledString().c_str());
        for(size_t j = 0; j < mval.size(); ++j){
            int p_uid = mval[j]["uid"].asInt();
            //将上一轮被淘汰的玩家信息加入总排名中
            std::map<int, Player*>::iterator it_f = all_players.find(p_uid);
            if(it_f == all_players.end()){
                RankInfo ri;
                ri.uid = p_uid;
                ri.name = mval[j]["name"].asString();
                ri.totalUseTime = mval[j]["totalUseTime"].asDouble();
                ri.totalRank = mval[j]["totalRank"].asInt();
				ri.meanMPRatio = //ConvertTotalScore(mval[j]["totalMPRatioScore"].asInt());
					mval[j]["totalMPRatioScore"].asDouble();
                m_TotalRankInfo.push_back(ri);
            }
            //将上一轮未淘汰的玩家信息进行更新
            else{
                mval[j]["totalUseTime"] = _4float(it_f->second->m_RankInfo.totalUseTime);
                mval[j]["totalRank"] = it_f->second->m_RankInfo.totalRank;
				mval[j]["totalMPRatioScore"] = _4float(it_f->second->m_RankInfo.meanMPRatio);
				mval[j]["average_mp_ratio"] = _4float(it_f->second->m_RankInfo.meanMPRatio);
				mval[j]["averageRatio"] = _4float(it_f->second->m_RankInfo.meanMPRatio);
				if (m_lose_player_mp_ratio)
				{
					mval[j]["rival_avg_mp_ratio"] = _4float(it_f->second->m_RankInfo.meanMPRatioLosePlayers);
				}
				if (it_f->second->m_RankInfo.rankCount > 1)
				{
					std::stringstream stream;
					std::string result;
					stream << it_f->second->m_RankInfo.rankNumber << "_" << it_f->second->m_RankInfo.rankCount;
					stream >> result;
					mval[j]["rank_count"] = result;
				}
            }
        }
    }
    ret = zjh.main_rc[0]->command("set m_totalRank:%d %s", matchid, mval.toStyledString().c_str());
    if(ret < 0){
        log_error("redis set mid[%d] totalRank error\n", matchid);
        return -1;
    }
//	log_info("%s mid:%d totalRank %s\n", __FUNCTION__, matchid, mval.toStyledString().c_str());
    return 0;
}

//上传总排名
int Match::UploadTotalRank()
{
    Json::Value val;
    unsigned int i = 0;
    for (std::vector<RankInfo>::iterator it = m_TotalRankInfo.begin(); it != m_TotalRankInfo.end(); it++) {	
		Json::Value& tmp = val["ranks"][i++];
        tmp["uid"] = (*it).uid;
        tmp["rank"] = (*it).totalRank;        
		tmp["totalUseTime"] = _4float((*it).totalUseTime);
		tmp["totalRank"] = (*it).totalRank;
		tmp["totalMPRatioScore"] = 	_4float((*it).meanMPRatio);
		tmp["average_mp_ratio"] = _4float((*it).meanMPRatio);
		tmp["averageRatio"] = _4float((*it).meanMPRatio);		
		if (m_lose_player_mp_ratio)
		{
			tmp["rival_avg_mp_ratio"] = _4float((*it).meanMPRatioLosePlayers);
		}

		if ((*it).rankCount > 1)
		{
			std::stringstream stream;
			std::string result;
			stream << (*it).rankNumber << "_" << (*it).rankCount;
			stream >> result;
			tmp["rank_count"] = result;
		}
    }
    zjh.main_rc[0]->command("del m_totalRank:%d", matchid);
//    log_info("%s mid:%d data:%s\n", __FUNCTION__, matchid, val.toStyledString().c_str());
    int ret = zjh.main_rc[0]->command("set end:%d %s", matchid, val.toStyledString().c_str());
    if (ret < 0){
        log_error("redis set mid[%d] rank error\n", matchid);
        return -1;
    }
	
    Jpacket packet;
    packet.val["cmd"] = SERVER_MATCH_RANK_INFO_REQ;
    packet.val["datetime"] = int(time(NULL));
    packet.val["mid"] = matchid;
    packet.end();
    zjh.game->SendToDataSvr(packet.tostring());
    return 0;
}

void Match::SendPromotion()
{
    if(m_CurTurn >= m_AllTurn){
        return;
    }
    //挂起优先级最高 比赛挂起 保存数据后回收赛事
    if(m_CompexsNum.find(m_CurTurn) != m_CompexsNum.end()){
        SaveMatchInfo();
        return;
    }
    for(std::map<int, Player*>::iterator it = all_players.begin(); it != all_players.end(); it++){
        HandlerTurnOverInfo(it->second, true);
    }	
    //淘汰玩家
    int promotionNum = getPromotionNum(m_CurTurn);
    std::vector<Player*> tempDelPlayer;
    for(std::map<int, Player*>::iterator it = all_players.begin(); it != all_players.end(); it++){
        Player *player = it->second;
        if (player->m_RankInfo.totalRank > promotionNum){
            tempDelPlayer.push_back(player);
        }
    }
    log_info("%s cur:%d playerNum:%d promotionNum:%d\n", __FUNCTION__,  m_CurTurn, all_players.size(), promotionNum);
    //回收已被淘汰的玩家
    for(std::vector<Player*>::iterator it = tempDelPlayer.begin(); it != tempDelPlayer.end(); it++){
        Player* player = *it;
        all_players.erase(player->uid);
        log_info("mid[%d] curturn[%d] delete player[%d]\n", matchid, m_CurTurn, player->uid);
        delete player;
        player = NULL;
    }
    if(!m_bPause){
		int dura = m_DurationNum[m_CurTurn];		
		if (dura == 0)
			dura = 3;
        ev_timer_set(&m_NextTurnTimer, dura, dura);
		m_NextTurnTimer_running = true;
        ev_timer_again(zjh.loop, &m_NextTurnTimer);
    }
    else{
        m_State = MATCH_PAUSE;        
		proto::game::tagUserState state;
		state.set_chairid(-1);
		state.set_state(0);
		broadcast(SERVER_MATCH_PAUSE_BC, state);

    }
}

int Match::MatchPause(int type)
{
    log_debug("recieve svr match[%d] pause req type[%d]\n", matchid, type);
    //请求暂停比赛
    if(type == -1){
        m_bPause = true;
        if(m_NextTurnTimer.active == 0){
            log_debug("match[%d] will pause in turn[%d]\n", matchid, m_CurTurn);
        }
        else{
            m_State = MATCH_PAUSE;
			m_NextTurnTimer_running = false;
            ev_timer_stop(zjh.loop, &m_NextTurnTimer);
            
			proto::game::tagUserState state;
			state.set_chairid(-1);
			state.set_state(0);
			broadcast(SERVER_MATCH_PAUSE_BC, state);

        }
    }
    //请求继续比赛马上发牌
    else if(type == 1){
        m_bPause = false;
        if(m_State != MATCH_PAUSE){
            log_debug("match[%d] not in pause\n", matchid);
        }
        else{
            m_State = MATCH_PLAYING;
            ev_timer_set(&m_NextTurnTimer, 7, 7);
			m_NextTurnTimer_running = true;
            ev_timer_again(zjh.loop, &m_NextTurnTimer);

			proto::game::tagUserState state;
			state.set_chairid(-1);
			state.set_state(0);
			broadcast(SERVER_MATCH_CONTINUE_BC, state);
        }
    }
    //请求继续比赛继续倒计时仍然处于暂停中
    else if(type == 2){
        m_bPause = false;
        if(m_State != MATCH_PAUSE){
            log_debug("match[%d] not in pause\n", matchid);
        }
        else{
            m_State = MATCH_PLAYING;
			m_NextTurnTimer_running = true;
            ev_timer_start(zjh.loop, &m_NextTurnTimer);
        }
    }
    int ret = zjh.main_rc[0]->command("set mstatus:%d %d", matchid, m_bPause);
    if (ret < 0){
        log_error("redis set mid[%d] mstatus error\n", matchid);
        return -1;
    }
    return 1;

}

void Match::MatchOver()
{
    log_info("%s match %d over \n", __FUNCTION__, matchid);
    m_State = MATCH_OVER;
    if(m_SendLastRank == 1){
		m_SendRankTimer_running = true;
        ev_timer_again(zjh.loop, &m_SendRankTimer);
    }    
	proto::game::tagUserState state;
	state.set_chairid(-1);
	state.set_state(0);
	broadcast(SERVER_MATCH_END_BC, state);

	for (std::map<int, Table*>::iterator it = all_tables.begin(); it != all_tables.end(); ++it)
	{
		Table* table = it->second;
		table->m_bDeleteTable = true;		
	}
	log_info("%s roundinfo round:%d/%d turn:%d/%d\n", __FUNCTION__, m_CurRound, m_AllRound, m_CurTurn, m_AllTurn);
}

void Match::TurnStartCB(struct ev_loop *loop, struct ev_timer *w, int revents)
{
    Match *match = (Match*)w->data;
	match->m_TurnStartTimer_running = false;
    ev_timer_stop(zjh.loop, &match->m_TurnStartTimer);
    match->TurnStart();
}

void Match::NextTurnCB(struct ev_loop *loop, struct ev_timer *w, int revents)
{
    Match *match = (Match*)w->data;
	if (match != NULL)
	{
		match->m_NextTurnTimer_running = false;
		ev_timer_stop(zjh.loop, &match->m_NextTurnTimer);
		match->NextTurn();
//		log_info("%s mid:%d turn:%d\n", __FUNCTION__, match->matchid, match->m_CurTurn);
	}    
}


void Match::MatchSendRankCB(struct ev_loop *loop, struct ev_timer *w, int revents)
{
    Match *match = (Match*)w->data;
	match->m_SendRankTimer_running = false;
    ev_timer_stop(zjh.loop, &match->m_SendRankTimer);
    match->UploadTotalRank();
}


void Match::SendPromotionCB(struct ev_loop *loop, struct ev_timer *w, int revents)
{
    Match *match = (Match*)w->data;
	match->m_SendPromotionTimer_running = false;
    ev_timer_stop(zjh.loop, &match->m_SendPromotionTimer);
    match->SendPromotion();
}

void Match::UpLoadRealRankCB(struct ev_loop *loop, struct ev_timer *w, int revents)
{
    Match *match = (Match*)w->data;
    match->UploadRealTimeRank();
}
void Match::NextRoundCB(struct ev_loop *loop, struct ev_timer *w, int revents)
{
	Match *match = (Match*)w->data;
	match->m_NextRoundTimer_running = false;
	ev_timer_stop(zjh.loop, &match->m_NextRoundTimer);
	match->NextRound();
}
//保存数据 通知平台 回收赛事
void Match::SaveMatchInfo()
{
    Json::Value val;
    size_t i = 0;
    for(std::map<int, Player*>::iterator it = all_players.begin(); it != all_players.end(); ++it){
        val[i]["uid"] = it->first;
        val[i]["rank"] = it->second->m_RankInfo.totalRank;
        val[i]["withTurnUp"] = it->second->m_RankInfo.totalRank <= getPromotionNum(m_CurTurn) ? 1 : 0;
        ++i;
    }
    int ret = zjh.main_rc[0]->command("hset turn_break:%d %d %s", matchid, CurTurn(), val.toStyledString().c_str());
    if (ret < 0){
        log_error("redis set mid[%d] turn_break error\n", matchid);
        return;
    }
    Jpacket packet;
    packet.val["cmd"] = SERVER_MATCH_SAVE_REQ;
    packet.val["mid"] = matchid;
    packet.val["turn"] = CurTurn();
    packet.end();
    zjh.game->SendToDataSvr(packet.tostring());
}

//重置桌子
void Match::ResetTable()
{
	//先把桌子全回收避免数据残留
	for (std::map<int, Table*>::iterator it = all_tables.begin(); it != all_tables.end(); ++it) {
		Table *table = it->second;
		if (table != NULL) {
			log_info("%s mid:%d recycle cur_turn:%d tid:%d succ\n", __FUNCTION__, matchid, m_CurTurn, it->second->tid);
			delete table;
			table = NULL;
		}
	}
	all_tables.clear();

	log_info("%s mid:%d init cur_turn:%d succ\n", __FUNCTION__, matchid, m_CurTurn);
	
	Json::Value val = GetTableConfigure(m_TableConfig);

	for (std::map<int, Player*>::iterator it = all_players.begin(); it != all_players.end(); ++it) {
		int tid = it->second->tid;
		Table* table;
		if (all_tables.find(tid) == all_tables.end()) {
			table = Table::CreateTable(zjh.conf["tables"], val);
			table->init(zjh.conf["tables"], val, it->second);
			table->tid = tid;
			table->matchid = matchid;
			table->m_CurRound = InitRoundNumber();
			table->m_MatchName = m_MatchName;			
			table->m_RobotRatio = m_RobotRatio;
			table->m_Type = m_FightMode == 0 ? 1001 : 1003;
			table->match = this;
			table->m_cbInitCardPool = m_cbInitCardPool;
			all_tables[tid] = table;
		}
		else
		{
			table = all_tables[tid];
		}
		table->handler_uptable(it->second, m_fixed_seat_id);
	}

	procSpawnCards();
}
void Match::procSpawnCards()
{
	int turn = m_TurnRoundNum[m_CurTurn - 1];
	for (int i = 0; i < turn; i++)
	{
		SpawnCards(i + 1, m_CurTurn);
	}
}

Player* Match::AddRobot(Table* table, Player* player)
{
	if (player == NULL)
		return NULL;

	static int robot_num = 100;
	Player* p = new (std::nothrow)Player;
	p->avatar = "default.jpg";
	p->tid = player->tid;	
	p->uid = robot_num;
	p->money = player->money;
	p->m_CurMoney = player->m_CurMoney;	
	p->name = (string)"机器人" + to_char(robot_num);
	p->m_isRobot = true;
	p->client = NULL;
	p->m_Role = 1;

	table->handler_uptable(p, false);

	table->PlayerLogin(p, true);
		
	all_robots.insert(p);

	robot_num++;
	robot_num = robot_num < 6000 ? robot_num : 100;
	return p;
}
bool Match::DelRobot(Player* player)
{
	if (player != NULL && player->m_isRobot)
	{
		delete player;
		all_robots.erase(player);
		return true;
	}
	return false;
}

/*反序列化用，用于重启*/
void Match::restart()
{
	for (std::map<int, Table*>::iterator it = all_tables.begin();
		it != all_tables.end(); it++)
	{
		it->second->restart();
	}
	if (m_TurnStartTimer_running)
	{
		ev_timer_again(zjh.loop, &m_TurnStartTimer);
	}
	if (m_SendRankTimer_running)
	{
		ev_timer_again(zjh.loop, &m_SendRankTimer);
	}
	if (m_NextTurnTimer_running)
	{
		ev_timer_again(zjh.loop, &m_NextTurnTimer);
	}
	if (m_SendPromotionTimer_running)
	{
		ev_timer_again(zjh.loop, &m_SendPromotionTimer);
	}
	if (m_UpLoadRealRankTimer_running)
	{
		ev_timer_again(zjh.loop, &m_UpLoadRealRankTimer);
	}
	if (m_NextRoundTimer_running)
	{
		ev_timer_again(zjh.loop, &m_NextRoundTimer);
	}
}