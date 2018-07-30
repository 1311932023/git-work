#include <algorithm>
#include "match_ext.h"
#include "common/log.h"
#include "zjh.h"

extern ZJH zjh;
extern Log log;

typedef void(*cbFillRank)(Player*, int, int, float, int, int);

void Fill_PlayerRank(Player* player, int cur_round, int mpscore, float mpratio, int mprank, int rank)
{	
	SettleInfo& settle = player->m_SettleInfo[cur_round];	
	settle.sMPScore = mpscore;
	settle.sMPRatio = mpratio;
	settle.sMPRatioRank = mprank;
	settle.sCurRank = rank;
}
void CalcMpScore(mapPlayerRank& rank, int all_player_count, int cur_round,  cbFillRank fn)
{
	int less_count = 0;
	int mprank = 1;
	int currank = 1;
	for (mapPlayerRank::iterator it = rank.begin(); it != rank.end(); ++it)
	{		
		lstPlayer& tmp = it->second;
		for (lstPlayer::iterator it_player = tmp.begin(); it_player != tmp.end(); ++it_player)
		{
			int mpscore = less_count * 2 + tmp.size() - 1;
			fn(*it_player,
				cur_round,
				mpscore,
				mpscore / ((all_player_count - 1) * 2.0f),
				mprank,
				currank++);
		}		
		less_count += tmp.size();
		mprank++;		
	}
}

Player* GetNo1(mapPlayerRank& rank)
{
	mapPlayerRank::iterator it = rank.begin();
	if (it != rank.end())
	{
		lstPlayer& tmp = it->second;
		lstPlayer::iterator it_player = tmp.begin();
		if (it_player != tmp.end())
		{
			return *it_player;
		}
	}
	return NULL;
}

bool CompareUsedtime(const Player* lhs, const Player* rhs)
{
	return lhs->m_UseTime < rhs->m_UseTime;
}
void SortMpScore(mapPlayerRank& rank)
{
	for (mapPlayerRank::iterator it = rank.begin(); it != rank.end(); ++it)
	{
		it->second.sort(CompareUsedtime);		
	}
}

MatchExt::MatchExt()
{

}

void MatchExt::CalculateCurRank(Player *player, int curRound)
{
	log.info("%s:%d:%s  player->uid:%d curRound:%d\n", __FILE__, __LINE__, __FUNCTION__, player->uid, curRound);
	if (curRound != m_CurRound) {
		log.error("not current round settle current[%d] your[%d]\n", m_CurRound, curRound);
		return;
	}
	//非最后一轮若排名相同对比时间
	std::vector<int> scoreCnt;//得到该分值的人数
	std::vector<int> mpCnt;//对应mp值
	std::vector<int> rankCnt;//对应排名
	size_t curPlayersCnt = 0;
	for (std::map<int, std::multimap<float, Player*> >::iterator it = m_PlayersRank.begin(); it != m_PlayersRank.end(); ++it) {
		scoreCnt.push_back(it->second.size());
		mpCnt.push_back(0);
		rankCnt.push_back(0);
		curPlayersCnt += it->second.size();
	}
	for (int i = scoreCnt.size() - 1; i >= 0; --i) {
		//mpCnt.push_back(0);
		int lessCnt = 0;
		for (int j = i - 1; j >= 0; --j) {
			lessCnt += scoreCnt[j];
		}
		mpCnt[i] = lessCnt * 2 + scoreCnt[i] - 1;
	}
	for (size_t i = 0; i < scoreCnt.size(); ++i) {
		//rankCnt.push_back(0);
		int moreCnt = 0;
		for (size_t j = i + 1; j < scoreCnt.size(); ++j) {
			moreCnt += scoreCnt[j];
		}
		rankCnt[i] = moreCnt + 1;
	}
	size_t i = 0;
	std::map<int, std::multimap<float, Player*> >::iterator it_end = m_PlayersRank.end();
	it_end--;
	for (std::map<int, std::multimap<float, Player*> >::iterator it = m_PlayersRank.begin(); it != m_PlayersRank.end(); ++it) {
		SettleInfo settleInfo;
		settleInfo.clear();
		settleInfo.sMPScore = mpCnt[i];
		if (all_players.size() == 1)
			settleInfo.sMPRatio = 0.0f;
		else
			settleInfo.sMPRatio = (float)mpCnt[i] / (2.0f * (all_players.size() - 1));
		settleInfo.sMPRatioRank = rankCnt[i];
		size_t j = 0;
		for (std::map<float, Player*>::iterator it_p = it->second.begin(); it_p != it->second.end(); ++it_p) {
			Player *p = it_p->second;
			if (it == it_end && it_p == it->second.begin()) {
				m_TurnCardInfo[CurRound()].topScore[0] = p->m_CurMoney;
				m_TurnCardInfo[CurRound()].topUName[0] = p->name;
			}
			settleInfo.sCurRank = rankCnt[i] + j;			
			p->m_SettleInfo[CurRound()].sMPScore = settleInfo.sMPScore;
			p->m_SettleInfo[CurRound()].sMPRatio = settleInfo.sMPRatio;
			p->m_SettleInfo[CurRound()].sMPRatioRank = settleInfo.sMPRatioRank;
			p->m_SettleInfo[CurRound()].sCurRank = settleInfo.sCurRank;			
			++j;
		}
		++i;
	}
	log.info("%s:%d:%s player SettleInfo count:%d\n",__FILE__,__LINE__, __FUNCTION__, player->m_SettleInfo.size());	
}

void MatchExt::CalculateTotalRank(std::map<int, Player*>& map_players, bool update_rank, bool single_turn)
{
	tmpRank.clear();
	log.info("%s:%d:%s mid:%d turn:%d round:%d\n", __FILE__,__LINE__,__FUNCTION__, matchid, m_CurTurn, m_CurRound);
	std::map<float, int> rankcount;	
	for (std::map<int, Player*>::iterator it = map_players.begin(); it != map_players.end(); ++it) {
		//没有完成任何局的玩家不计入总排名
		if (it->second->m_SettleInfo.empty()) {
			continue;
		}
		it->second->CalculateTotalScore();
		RankInfo& ri = it->second->m_RankInfo;		
		rankcount[ri.meanMPRatio] += 1;
	}
	std::map<float, int> ranknumber;
	int icount = map_players.size() + 1;
	for (std::map<float, int>::iterator it = rankcount.begin(); it != rankcount.end(); ++it)
	{
		icount -= it->second;
		ranknumber[it->first] = icount;
	}
	for (std::map<int, Player*>::iterator it = map_players.begin(); it != map_players.end(); ++it)
	{
		it->second->m_RankInfo.rankCount = rankcount[it->second->m_RankInfo.meanMPRatio];
		it->second->m_RankInfo.rankNumber = ranknumber[it->second->m_RankInfo.meanMPRatio];
		tmpRank.push_back(it->second->m_RankInfo);
	}
	sort(tmpRank.begin(), tmpRank.end(), RankInfo::LessCB);
	int i = 1;
	for (std::vector<RankInfo>::iterator it = tmpRank.begin(); it != tmpRank.end(); ++it) {
		int uid = it->uid;
		if (map_players.find(uid) != map_players.end()) {
			Player *player = map_players[uid];
			if (update_rank)
			{
				player->UpdateTotalRank(player->m_CurRound, i);

				player->UpdateLosePlayersMpRatio(single_turn);
			}
			it->totalRank = i;
			++i;
		}
	}
	log.info("%s:%d:%s mid:%d turn:%d round:%d\n", __FILE__, __LINE__, __FUNCTION__, matchid, m_CurTurn, m_CurRound);

}

void MatchExt::CalculateTotalRank4(std::map<int, Player*>& map_players, bool update_rank, bool single_turn)
{
	log.info("%s:%d:%s map_players.size:%d update_rank:%d single_turn:%d\n", __FILE__, __LINE__, __FUNCTION__, map_players.size(), update_rank, single_turn);
	//tmpRank.clear();
	tmpGiveUpRank.clear();
	//log.info("%s mid:%d turn:%d round:%d\n", __FUNCTION__, matchid, m_CurTurn, m_CurRound);
	for (std::map<int, Player*>::iterator it = map_players.begin(); it != map_players.end(); ++it) {

		it->second->IsGiveUp();
	}

	
	for (std::map<int, Player*>::iterator it = map_players.begin(); it != map_players.end(); ++it)
	{
	//	log.info("CalculateTotalRank4 it->second->m_RankInfo.bGiveUp = %d\n", it->second->m_RankInfo.bGiveUp);

		if (it->second->m_RankInfo.bGiveUp == true)
		{
			for (std::vector<RankInfo>::iterator itRank = tmpRank.begin(); itRank != tmpRank.end(); ++itRank)
			{
				if (it->second->m_RankInfo.uid == itRank->uid)
				{
					log.info("%s:%d:%s it->second->m_RankInfo.bGiveUp = %d it->second->m_RankInfo.uid:%d\n",__FILE__,__LINE__,__FUNCTION__, it->second->m_RankInfo.bGiveUp, it->second->m_RankInfo.uid);
					tmpGiveUpRank.push_back(it->second->m_RankInfo);
					tmpRank.erase(itRank);
					break;
				}
			}
		}
	}

	sort(tmpGiveUpRank.begin(), tmpGiveUpRank.end(), RankInfo::total_rank_compare);
	tmpRank.insert(tmpRank.end(), tmpGiveUpRank.begin(), tmpGiveUpRank.end());

	int i = 1;
	for (std::vector<RankInfo>::iterator it = tmpRank.begin(); it != tmpRank.end(); ++it) {
		int uid = it->uid;
		if (map_players.find(uid) != map_players.end()) {
			Player *player = map_players[uid];
			if (update_rank)
			{
				player->UpdateTotalRank(player->m_CurRound, i);

				player->UpdateLosePlayersMpRatio(single_turn);
			}
			it->totalRank = i;
			++i;
		}
	}

}

void MatchExt::CalculateTotalRank3(int curRound, bool single_turn)
{
	tmpRank.clear();
	log.info("%s:%d:%s mid:%d turn:%d round:%d\n", __FILE__,__LINE__,__FUNCTION__, matchid, m_CurTurn, m_CurRound);
	for (std::map<int, Player*>::iterator it = all_players.begin(); it != all_players.end(); ++it)
	{
		tmpRank.push_back(it->second->m_RankInfo);
	}
	sort(tmpRank.begin(), tmpRank.end(), RankInfo::LessCB2);
	int i = 1;
	for (std::vector<RankInfo>::iterator it = tmpRank.begin(); it != tmpRank.end(); ++it) {
		int uid = it->uid;
		if (all_players.find(uid) != all_players.end()) {
			Player *player = all_players[uid];			
			player->UpdateTotalRank2(curRound, i);
			if (curRound >= 0)
			{
				player->UpdateLosePlayersMpRatio(single_turn);
			}
			it->totalRank = i;
			++i;
		}
	}
	//log.info("%s mid:%d turn:%d round:%d\n", __FUNCTION__, matchid, m_CurTurn, m_CurRound);

}


int MatchExt::StatisticGameResult(Table *table)
{
	log.info("%s:%d:%s\n",__FILE__,__LINE__, __FUNCTION__);

	Player *player = table->m_SeatPlayer[0].player;
	int curRound = table->m_CurRound;
	if (all_players.find(player->uid) == all_players.end()) {
		log.error("%s player[%d] not in match[%d]\n", __FUNCTION__, player->uid, matchid);
		return -1;
	}
	if (curRound != CurRound()) {
		log.error("not current round settle current[%d] your[%d]\n", m_CurRound, curRound);
		return -1;
	}
	m_PlayersRank[player->m_CurMoney].insert(make_pair(player->m_UseTime ,player));
	log.info("%s mid:%d uid:%d round:%d OverTable:%d\n",__FUNCTION__,
		matchid, player->uid,  m_CurRound, m_TurnOverTableCnt);
	CalculateCurRank(player, curRound);

	CalculateTotalRank(all_players, true, false);


	return 0;
}

//按座位统计MP分 每轮进行清空
void MatchExt::StatisticSeatRank(int seatid, int cur_round)
{

	std::map<int, Player*>& tempPlayers = m_playersInSeat[seatid];

	std::map<int, std::multimap<float, Player*> > tempScore;
	for (std::map<int, Player*>::iterator it = tempPlayers.begin(); it != tempPlayers.end(); ++it) {
		Player *player = it->second;
		if (player->m_SettleInfo.find(cur_round) != player->m_SettleInfo.end()) {
			tempScore[player->m_SettleInfo[cur_round].sCurMoney].insert(make_pair(player->m_SettleInfo[cur_round].sUseTime, player));
		}
	}
	//当局还没有桌子完成比赛 说明后面的都没有
	if (tempScore.empty()) {
		return;
	}
	std::vector<int> scoreCnt;//得到该分值的人数
	std::vector<int> mpCnt;//对应mp值
	std::vector<int> rankCnt;//对应排名
	size_t curPlayersCnt = 0;
	for (std::map<int, std::multimap<float, Player*> >::iterator it = tempScore.begin(); it != tempScore.end(); it++) {
		scoreCnt.push_back(it->second.size());
		mpCnt.push_back(0);
		rankCnt.push_back(0);
		curPlayersCnt += it->second.size();
	}
	for (int i = scoreCnt.size() - 1; i >= 0; i--) {
		int lessCnt = 0;
		for (int j = i - 1; j >= 0; j--) {
			lessCnt += scoreCnt[j];
		}
		mpCnt[i] = lessCnt * 2 + scoreCnt[i] - 1;
	}
	for (size_t i = 0; i < scoreCnt.size(); i++) {
		int moreCnt = 0;
		for (size_t j = i + 1; j < scoreCnt.size(); j++) {
			moreCnt += scoreCnt[j];
		}
		rankCnt[i] = moreCnt + 1;
	}
	unsigned int i = 0;
	std::map<int, std::multimap<float, Player*> >::iterator it_end = tempScore.end();
	it_end--;
	for (std::map<int, std::multimap<float, Player*> >::iterator it = tempScore.begin(); it != tempScore.end(); ++it) {
		SettleInfo settleInfo;
		settleInfo.clear();
		settleInfo.sMPScore = mpCnt[i];
		if (tempPlayers.size() == 1)
			settleInfo.sMPRatio = 0.0f;
		else
			settleInfo.sMPRatio = (float)mpCnt[i] / (2.0f * (tempPlayers.size() - 1));
		settleInfo.sMPRatioRank = rankCnt[i];
		size_t j = 0;
		for (std::multimap<float, Player*>::iterator it_p = it->second.begin(); it_p != it->second.end(); ++it_p) {
			Player *p = it_p->second;
			p->m_SettleInfo[cur_round].sMPScore = settleInfo.sMPScore;
			p->m_SettleInfo[cur_round].sMPRatio = settleInfo.sMPRatio;
			p->m_SettleInfo[cur_round].sMPRatioRank = settleInfo.sMPRatioRank;
			p->m_SettleInfo[cur_round].sCurRank = rankCnt[i] + j;
			if (it == it_end && it_p == it->second.begin()) {
				m_TurnCardInfo[cur_round].topUName[seatid] = p->name;
				m_TurnCardInfo[cur_round].topScore[seatid] = p->m_SettleInfo[cur_round].sCurMoney;
			}
			++j;
			log.info("%s:%d:%s mid:%d uid:%d round:%d cur_rank:%d mpRank:%d score:%d ratio:%f\n",__FILE__,__LINE__, __FUNCTION__, matchid, p->uid, cur_round, p->m_SettleInfo[cur_round].sCurRank, p->m_SettleInfo[cur_round].sMPRatioRank, p->m_SettleInfo[cur_round].sMPScore, p->m_SettleInfo[cur_round].sMPRatio);
		}
		++i;
	}
}

void MatchExt::EliminateRobot()
{
	if (m_CurTurn >= m_AllTurn) {
		return;
	}
	int promotionNum = getPromotionNum(m_CurTurn);
	int robotgiveupNum = 0;
	unsigned int giveupNum = all_players.size() - promotionNum;
	if (giveupNum >= all_taotairobots.size()) {
		robotgiveupNum = all_taotairobots.size();
	}
	else {
		robotgiveupNum = all_players.size() - promotionNum;
	}
	//sort(all_robots.begin(), all_robots.end(), Player::LessCB);
	//将需要淘汰的机器人作弃赛处理
	log.info("%s:%d:%s mid:%d giveupNum:%d promotionNum:%d all_players.size():%d robotgiveupNum:%d\n", __FILE__,__LINE__,__FUNCTION__, matchid, giveupNum, promotionNum, all_players.size(), robotgiveupNum);

	while (robotgiveupNum > 0) {
		std::set<Player*>::iterator it = all_taotairobots.begin();
		(*it)->m_GiveUp = true;
		all_taotairobots.erase(it);
		robotgiveupNum--;
	}	
	CalculateTotalRank4(all_players, true, false);
}

void MatchExt::StatisticCommonRank()
{	
	log.info("%s:%d:%s matchid:%d\n ", __FILE__, __LINE__, __FUNCTION__, matchid);
	m_PlayersRank.clear();
	for (std::map<int, Player*>::iterator it = all_players.begin(); it != all_players.end(); ++it)
	{	
		Player* player = it->second;
		if (player != NULL && !player->m_isRobot)
		{
			m_PlayersRank[player->m_CurMoney].insert(make_pair(player->m_UseTime, player));
		}
	}
	//非最后一轮若排名相同对比时间	
	size_t i = m_PlayersRank.size();
	std::map<int, std::multimap<float, Player*> >::iterator it_end = m_PlayersRank.end();
	it_end--;
	for (std::map<int, std::multimap<float, Player*> >::iterator it = m_PlayersRank.begin(); it != m_PlayersRank.end(); ++it) {
		for (std::multimap<float, Player*>::iterator it_p = it->second.begin(); it_p != it->second.end(); ++it_p) {
			Player *p = it_p->second;
			if (it == it_end && it_p == it->second.begin()) {
				m_TurnCardInfo[CurRound()].topScore[0] = p->m_CurMoney;
				m_TurnCardInfo[CurRound()].topUName[0] = p->name;
			}			
			p->m_SettleInfo[CurRound()].sMPScore += p->m_SettleInfo[CurRound()].sCurMoney;
			p->m_SettleInfo[CurRound()].sCurRank = i;
		}
		i--;
	}	
}

void MatchExt::CalculateCommonRank()
{
	tmpRank.clear();
	log.info("%s:%d:%s mid:%d turn:%d round:%d\n",__FILE__,__LINE__, __FUNCTION__, matchid, m_CurTurn, m_CurRound);
	std::map<float, int> rankcount;	
	for (std::map<int, Player*>::iterator it = all_players.begin(); it != all_players.end(); ++it) {
		//没有完成任何局的玩家不计入总排名
		if (it->second->m_SettleInfo.empty()) {
			continue;
		}
		it->second->CalculateCommonScore();
		RankInfo& ri = it->second->m_RankInfo;
		rankcount[ri.meanMPRatio] += 1;		
	}
	std::map<float, int> ranknumber;
	int icount = all_players.size() + 1;
	for (std::map<float, int>::iterator it = rankcount.begin(); it != rankcount.end(); ++it)
	{
		icount -= it->second;
		ranknumber[it->first] = icount;
	}
	for (std::map<int, Player*>::iterator it = all_players.begin(); it != all_players.end(); ++it)
	{
		it->second->m_RankInfo.rankCount = rankcount[it->second->m_RankInfo.meanMPRatio];
		it->second->m_RankInfo.rankNumber = ranknumber[it->second->m_RankInfo.meanMPRatio];
		tmpRank.push_back(it->second->m_RankInfo);
	}
	sort(tmpRank.begin(), tmpRank.end(), RankInfo::LessCB);
	int i = 1;
	for (std::vector<RankInfo>::iterator it = tmpRank.begin(); it != tmpRank.end(); ++it) {
		int uid = it->uid;
		if (all_players.find(uid) != all_players.end()) {
			Player *player = all_players[uid];
			player->UpdateTotalRank(player->m_CurRound, i);
			player->UpdateLosePlayersMpRatio(false);
			it->totalRank = i;
			++i;
		}
	}
}

int MatchExt::UploadMPInfo()
{

	for (int i =0; i < m_roundinfo.size(); )
	{
		Json::Value& tmp = m_roundinfo[i++];
		int uid = tmp["uinfo"]["player_id"].asInt();
		int roundid = tmp["uinfo"]["card_numerical_order"].asInt();
		if (all_players.find(uid) != all_players.end())
		{
			Player* p = all_players[uid];
			tmp["mpinfo"]["mpScore"] = p->m_SettleInfo[roundid].sMPScore;
			tmp["mpinfo"]["mpRatio"] = //CalcTotalScore(p->m_SettleInfo[roundid].sMPRatio);
				_4float(p->m_SettleInfo[roundid].sMPRatio);
			tmp["mpinfo"]["mpRatioRank"] = p->m_SettleInfo[roundid].sMPRatioRank;
			log.info("%s:%d:%s mid:%d uid:%d round:%d mpscore:%d mpratio:%f\n",__FILE__,__LINE__, __FUNCTION__, matchid, uid, roundid,
				p->m_SettleInfo[roundid].sMPScore, p->m_SettleInfo[roundid].sMPRatio);
		}
	}

	Json::Value& val = m_roundinfo;

	string tmp = val.toStyledString();
	int ret = zjh.main_rc[0]->command("hset mp:%d %d %s", matchid, CurTurn(), tmp.c_str());
	if (ret < 0) {
		log.error("redis set mid[%d] mp error\n", matchid);
		return -1;
	}
	log.info("%s:%d:%s mid:%d \n",__FILE__,__LINE__, __FUNCTION__, matchid);

	if (!isTurnOver())
		return 0;

	Jpacket packet;
	packet.val["cmd"] = SERVER_TURN_MP_INFO_REQ;
	packet.val["datetime"] = int(time(NULL));
	packet.val["mid"] = matchid;
	packet.val["turn"] = CurTurn();
	packet.end();
	zjh.game->SendToDataSvr(packet.tostring());

	m_roundinfo.clear();
	return 0;
}

int MatchExt::UpdateMPInfo2()
{
	Json::Value val;
	size_t k = 0;	
	for (std::map<int, Player*>::iterator it = all_players.begin(); it != all_players.end(); ++it)
	{
		for (int r = 1; r <= m_AllRound; ++r) 
		{
			Json::Value& tmp = val[k++];
			Player* player = it->second;
			SettleInfo& settleinfo = player->m_SettleInfo[r];
			tmp["rinfo"]["round"] = r;
			tmp["uinfo"]["uid"] = player->uid;			
			tmp["uinfo"]["status"] = //settleinfo.sCurMoney > 0 ? 2 : 1;
				player->m_GiveUp ? 1 : 0;
			tmp["uinfo"]["alterCoin"] = settleinfo.sCurMoney;
			tmp["uinfo"]["tableNum"] = player->tid;
			tmp["pinfo"]["cardTransferScore"] = settleinfo.curScore;
			
		}
	}
	int ret = zjh.main_rc[0]->command("hset point:%d 1 %s", matchid, val.toStyledString().c_str());
	if (ret < 0) {
		log.error("redis set mid[%d] mp error\n", matchid);
		return -1;
	}
	log.info("%s:%d:%s mid:%d \n", __FILE__,__LINE__,__FUNCTION__, matchid);
	Jpacket packet;
	packet.val["cmd"] = SERVER_GAME_SCORE_REQ;
	packet.val["mid"] = matchid;
	packet.val["turn"] = 1;
	packet.val["datetime"] = (int)time(NULL);
	packet.end();
	zjh.game->SendToDataSvr(packet.tostring());
	packet.val["cmd"] = SERVER_TURN_RANK_REQ;
	packet.end();
	zjh.game->SendToDataSvr(packet.tostring());
	ev_timer_again(zjh.loop, &m_SendRankTimer);
	return 0;
}

int MatchExt::CalculateTotalRank2(Player *player)
{
	log.info("%s:%d:%s mid:%d \n", __FILE__, __LINE__, __FUNCTION__, matchid);

	int uid = player->uid;	
	//玩家多次参赛先找到原先的排名
	for (size_t i = 0; i < m_TotalRankInfo.size(); ++i) {
		if (m_TotalRankInfo[i].uid == uid)
		{
			if (player->m_TurnScore.empty())
				player->m_TurnScore.push_back(player->m_RankInfo);
			m_TotalRankInfo[i] = player->m_TurnScore[0];
			goto __sort__;
		}
	}
	m_TotalRankInfo.push_back(player->m_TurnScore[0]);
	__sort__:
	std::sort(m_TotalRankInfo.begin(), m_TotalRankInfo.end(), RankInfo::LessCB);		
	for (size_t i = 0; i < m_TotalRankInfo.size(); ++i)
	{
		m_TotalRankInfo[i].totalRank = i + 1;
		int tmp_uid = m_TotalRankInfo[i].uid;
		if (all_players.find(tmp_uid) != all_players.end())
		{
			Player* tmp_player = all_players[m_TotalRankInfo[i].uid];
			tmp_player->m_TurnScore[0].totalRank = m_TotalRankInfo[i].totalRank;
		}
	}

	Jpacket packet;
	packet.val["cmd"] = SERVER_USER_END_REQ;
	packet.val["mid"] = matchid;
	packet.val["uid"] = uid;
	packet.val["datetime"] = (int)time(NULL);
	packet.end();
	zjh.game->SendToDataSvr(packet.tostring());	
	return 0;
}

void MatchExt::StatisticScoreRank(int round, Player *player)
{	
	m_PlayersRank.clear();
	for (std::map<int, Player*>::iterator it = all_players.begin(); it != all_players.end(); ++it)
	{
		Player* player = it->second;
		if (player != NULL && !player->m_isRobot)
		{
			if (player->m_SettleInfo.find(round) != player->m_SettleInfo.end())
			{
				SettleInfo& info = player->m_SettleInfo[round];
				info.curScore = info.sCurMoney;
				m_PlayersRank[info.sCurMoney].insert(make_pair(info.sUseTime, player));
			}			
		}
	}	
	int rank = all_players.size();
	for (std::map<int, std::multimap<float, Player*> >::iterator it = m_PlayersRank.begin(); it != m_PlayersRank.end(); ++it) {
		for (std::map<float, Player*>::iterator it_p = it->second.begin(); it_p != it->second.end(); ++it_p) {
			Player *p = it_p->second;					
			p->m_SettleInfo[round].sCurRank = rank--;		
		}		
	}	
	std::map<int, std::multimap<float, Player*> >::iterator it_end = m_PlayersRank.end();
	it_end--;
	if (it_end != m_PlayersRank.end())
	{
		std::multimap<float, Player*>::iterator it_p = it_end->second.begin();
		if (it_p != it_end->second.end())
		{
			Player*p = it_p->second;
			m_TurnCardInfo[round].topScore[0] = p->m_SettleInfo[round].curScore;
			m_TurnCardInfo[round].topUName[0] = p->name;
			log.info("%s round:%d  topuser uid:%d  name:%s score:%d\n", __FUNCTION__, round, p->uid, p->name.c_str(), p->m_SettleInfo[round].curScore);
		}
	}

	player->CalculateBaseScore();
	log.info("%s uid:%d round:%d rank:%d totalrank:%d\n",
		__FUNCTION__, player->uid, round, player->m_SettleInfo[round].sCurRank, player->m_SettleInfo[round].sTotalRank);
}

int MatchExt::UpdateRank(int cur_round, bool bEnd)
{
	log.info("%s:%d:%s mid:%d \n", __FILE__, __LINE__, __FUNCTION__, matchid);

	tmpRank.clear();
	log.info("%s mid:%d turn:%d round:%d\n", __FUNCTION__, matchid, m_CurTurn, m_CurRound);
	for (std::map<int, Player*>::iterator it = all_players.begin(); it != all_players.end(); ++it) {
		//没有完成任何局的玩家不计入总排名		
		it->second->CalculateBaseScore();
		tmpRank.push_back(it->second->m_RankInfo);
	}
	sort(tmpRank.begin(), tmpRank.end(), RankInfo::LessCB);
	int i = 1;
	for (std::vector<RankInfo>::iterator it = tmpRank.begin(); it != tmpRank.end(); ++it) {
		int uid = it->uid;
		if (all_players.find(uid) != all_players.end()) {
			Player *player = all_players[uid];
			if (player->m_SettleInfo.find(cur_round) != player->m_SettleInfo.end())
			{
				player->UpdateTotalRank(cur_round, i);
				it->totalRank = i++;				
			}
		}
	}
	return 0;
}
int MatchExt::UpdateRank_total()
{
	log.info("%s:%d:%s mid:%d \n", __FILE__, __LINE__, __FUNCTION__, matchid);
	size_t index = 0;
	Json::Value val;	
	val["mid"] = matchid;
	for (std::vector<RankInfo>::iterator it = m_TotalRankInfo.begin(); it != m_TotalRankInfo.end(); ++it)
	{
		Json::Value& tmp = val["ranks"][index++];
		tmp["uid"] = it->uid;
		tmp["rank"] = it->totalRank;
		tmp["score"] = CalcTotalScore(it->totalScore);
		tmp["userName"] = it->name;
	}
	//int ret = zjh.main_rc[0]->command("set test_rank:%d %s", matchid, val.toStyledString().c_str());	
	int ret = zjh.main_rc[0]->command("hset rank:%d 1 %s", matchid, val.toStyledString().c_str());
	if (ret < 0)
		log.error("%s mid:%d hset error\n", __FUNCTION__, matchid);
// 	else
// 		log.info("%s mid:%d val:%s\n", __FUNCTION__, matchid, val.toStyledString().c_str());
	return 0;
}
void MatchExt::MatchOver()
{
	log.info("%s:%d:%s mid:%d \n", __FILE__, __LINE__, __FUNCTION__, matchid);
	for (std::map<int, Player*>::iterator it = all_players.begin(); it != all_players.end(); ++it)
	{
		Jpacket packet;
		packet.val["cmd"] = SERVER_USER_END_REQ;
		packet.val["mid"] = matchid;
		packet.val["uid"] = it->first;
		packet.val["datetime"] = (int)time(NULL);
		packet.end();
		zjh.game->SendToDataSvr(packet.tostring());
	}
	Parent::MatchOver();
}