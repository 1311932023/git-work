#ifndef __MATCH_EXT_H__
#define __MATCH_EXT_H__
#include "match.h"
#include "common/client.h"
#include "player.h"

typedef std::list<Player*> lstPlayer;
typedef std::map<int, lstPlayer, std::greater<int> > mapPlayerRank;

class MatchExt : public Match
{
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive & ar, const unsigned int /* file_version */) {
		printf("MatchExt::serialize\n");
		ar & boost::serialization::base_object<Match>(*this);
		/*boost::serialization::void_cast_register<MatchExt, Match>(
			static_cast<MatchExt *>(NULL),
			static_cast<Match *>(NULL)
			);*/
	}

	typedef Match Parent;
public:
	MatchExt();
protected:
	std::map<int, std::multimap<float, Player*> >    m_PlayersRank;   //����������<money,<time, player>>		
	std::map<int, Player*>      m_playersInSeat[GAME_PLAYER];//����λ�����   	
public:
	int UploadMPInfo();
public:
	//�˻���
	void CalculateCurRank(Player *player, int curRound);

	int StatisticGameResult(Table *table);

	void CalculateTotalRank(std::map<int, Player*>& map_players);

	void CalculateTotalRank3();

public:
	//������
	void StatisticSeatRank(int,int);

	void EliminateRobot();

public:
	//�ܷ���
	void StatisticCommonRank();

	void CalculateCommonRank();

public:
	void StatisticScoreRank(int round, Player *player);
	//ʱ����
	int UpdateMPInfo2();

	int CalculateTotalRank2(Player *player);

	int UpdateRank(int cur_round, bool bEnd);

	int UpdateRank_total();

public:
	void MatchOver();

	/*�����л��ã���������*/
	virtual void restart();
};

BOOST_SERIALIZATION_ASSUME_ABSTRACT(MatchExt)

#endif