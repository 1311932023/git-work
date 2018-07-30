#ifndef __TRUSTEEFUNC_H__
#define __TRUSTEEFUNC_H__
#include "../libnormalmahjong/game_logic.h"
#include "../libnormalmahjong/mj_common2.h"
#include "CardsCounts.h"
#include "TrusteeBase.h"
#include <set>

#define  CARDNODE_FILTER_PARAM_1 (2)

struct ScoreNode
{
	_uint8 dan;
	_uint8 res;
	_uint8 kezi;
	_uint8 bianka;
	int pos;
	float score;
	ScoreNode() {}
	ScoreNode(int d, int r, int k, int bk, int p) : dan(d), res(r), kezi(k), bianka(bk), pos(p), score(0.0f) {}
	struct Score
	{
		Score() :card(0), score(0.0f) {}
		_uint8 card;
		float score;
		struct compare
		{
			bool operator()(const Score& lhs, const Score& rhs) const {
				return lhs.score < rhs.score ? true :
					lhs.score == rhs.score ?
					lhs.card < rhs.card : false;
			}
		};
	};

	std::set<Score, Score::compare> outcard;
};

int ScoreNode_filter0(CardNode_set& hc, const CardNode_package& package, MJ_BUFFER<ScoreNode, CARDNODE_SIZE_MAX>& node);
int ScoreNode_filter1(CardNode_set& hc, const CardNode_package& package, MJ_BUFFER<ScoreNode, CARDNODE_SIZE_MAX>& node);
int ScoreNode_filter2(CardNode_set& hc, const CardNode_package& package, MJ_BUFFER<ScoreNode, CARDNODE_SIZE_MAX>& node);
int ScoreNode_filter3(CardNode_set& hc, const CardNode_package& package, MJ_BUFFER<ScoreNode, CARDNODE_SIZE_MAX>& node);

void Add(CardNode_set& hc, _uint8 card, int count);

#endif