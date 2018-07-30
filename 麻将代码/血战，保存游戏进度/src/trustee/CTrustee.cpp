#include <algorithm>
#include "CTrustee.h"
#include "../zjh.h"
#include "../common/log.h"
#include "TrusteeFunc.h"
#include <string>

extern ZJH zjh;
extern Log log;
#define FIXED_DEEP (HAND_CARD_SIZE_MAX * 2)

CTrustee::CTrustee(Table* t) : table(t)
{
	m_GuiCard.clear();
	trustee_timer_running[0] = false;
	trustee_timer[0].data = this;
	trustee_timer_running[1] = false;
	trustee_timer[1].data = this;
	trustee_timer_running[2] = false;
	trustee_timer[2].data = this;
	trustee_timer_running[3] = false;
	trustee_timer[3].data = this;

	ev_timer_init(&trustee_timer[0], CTrustee::cbTimerOut0, m_Delay[0], m_Delay[0]);
	ev_timer_init(&trustee_timer[1], CTrustee::cbTimerOut1, m_Delay[1], m_Delay[1]);
	ev_timer_init(&trustee_timer[2], CTrustee::cbTimerOut2, m_Delay[2], m_Delay[2]);
	ev_timer_init(&trustee_timer[3], CTrustee::cbTimerOut3, m_Delay[3], m_Delay[3]);
}

CTrustee::CTrustee()
{
	m_GuiCard.clear();
	table = NULL;

	trustee_timer_running[0] = false;
	trustee_timer[0].data = this;
	trustee_timer_running[1] = false;
	trustee_timer[1].data = this;
	trustee_timer_running[2] = false;
	trustee_timer[2].data = this;
	trustee_timer_running[3] = false;
	trustee_timer[3].data = this;

	ev_timer_init(&trustee_timer[0], CTrustee::cbTimerOut0, m_Delay[0], m_Delay[0]);
	ev_timer_init(&trustee_timer[1], CTrustee::cbTimerOut1, m_Delay[1], m_Delay[1]);
	ev_timer_init(&trustee_timer[2], CTrustee::cbTimerOut2, m_Delay[2], m_Delay[2]);
	ev_timer_init(&trustee_timer[3], CTrustee::cbTimerOut3, m_Delay[3], m_Delay[3]);

	
}

CTrustee::~CTrustee()
{
	TIMER_STOP(trustee_timer[0]);
	TIMER_STOP(trustee_timer[1]);
	TIMER_STOP(trustee_timer[2]);
	TIMER_STOP(trustee_timer[3]);
	trustee_timer[0].data = NULL;
	trustee_timer[1].data = NULL;
	trustee_timer[2].data = NULL;
	trustee_timer[3].data = NULL;

}
void CTrustee::cbTimerOut0(struct ev_loop *loop, struct ev_timer *w, int revents)
{
	CTrustee* trustee = (CTrustee*)(w->data);
	trustee->trustee_timer_running[0] = false;
	TIMER_STOP(trustee->trustee_timer[0]);
	if (trustee != NULL) trustee->TimerOut(0);
}
void CTrustee::cbTimerOut1(struct ev_loop *loop, struct ev_timer *w, int revents)
{
	CTrustee* trustee = (CTrustee*)(w->data);
	trustee->trustee_timer_running[1] = false;
	TIMER_STOP(trustee->trustee_timer[1]);
	if (trustee != NULL) trustee->TimerOut(1);
}
void CTrustee::cbTimerOut2(struct ev_loop *loop, struct ev_timer *w, int revents)
{
	CTrustee* trustee = (CTrustee*)(w->data);
	trustee->trustee_timer_running[2] = false;
	TIMER_STOP(trustee->trustee_timer[2]);
	if (trustee != NULL) trustee->TimerOut(2);
}
void CTrustee::cbTimerOut3(struct ev_loop *loop, struct ev_timer *w, int revents)
{
	CTrustee* trustee = (CTrustee*)(w->data);
	trustee->trustee_timer_running[3] = false;
	TIMER_STOP(trustee->trustee_timer[3]);
	if (trustee != NULL) trustee->TimerOut(3);
}

void CTrustee::Init()
{
	CCardPool& pool = table->m_CardPool;

	for (int i = 0; i < pool.Count(); i++)
	{
		_uint8 card = pool[i];
		m_CardCounts[card]++;
	}
}
void CTrustee::OnUserGetCard(_uint8 ChairID, _uint8 bCard)
{
	m_CardCounts[bCard]--;
	m_CurChairID = ChairID;
	if (m_Robot[ChairID])
	{
		if (table->m_SeatPlayer[ChairID].player->m_isRobot)
		{//确保超时时间是机器人超时时间
			m_Delay[ChairID] = table->m_RobotDelayTime;
		}
		else
		{
			m_Delay[ChairID] = table->m_bDelayTime;
		}
		trustee_timer_running[ChairID] = true;
		TIMER_START(trustee_timer[ChairID], m_Delay[ChairID]);		
		log_info("CTrustee::%s tid:%d chair:%d  timer start\n", __FUNCTION__, table->tid, ChairID);
	}	
}
void CTrustee::OnUserOutCard(_uint8 ChairID, _uint8 bCard)
{	
	if (m_Robot[ChairID])
	{
		trustee_timer_running[ChairID] = false;
		TIMER_STOP(trustee_timer[ChairID]);
		log_info("CTrustee::%s tid:%d chair: %d timer stop\n", __FUNCTION__, table->tid, ChairID);
	}
}
void CTrustee::EstimateUserRespond(_uint8 ChairID)
{
	for (_uint8 c = table->NextChair(ChairID); c!=ChairID; c= table->NextChair(c))
	{
		if (m_Robot[c])
		{
			int iOpType = table->m_bUserOperate[c] & ~(TYPE_LISTEN);
			if (iOpType != TYPE_NULL)
			{
				trustee_timer_running[c] = true;
				TIMER_START(trustee_timer[c], m_Delay[c]);
				m_CurChairID = ChairID;
				log_info("CTrustee::%s %d timer start\n", __FUNCTION__, c);
			}
		}
	}
}
void CTrustee::GameEnd()
{
	trustee_timer_running[0] = false;
	TIMER_STOP(trustee_timer[0]);
	trustee_timer_running[1] = false;
	TIMER_STOP(trustee_timer[1]);
	trustee_timer_running[2] = false;
	TIMER_STOP(trustee_timer[2]);
	trustee_timer_running[3] = false;
	TIMER_STOP(trustee_timer[3]);
}
void CTrustee::DoSimpleOperate(_uint8 ChairID, int iOpType)
{	
	if (m_Robot[ChairID])
	{
		if (ChairID == table->m_bCurChairID && iOpType == TYPE_PASS)
		{
			trustee_timer_running[ChairID] = true;
			TIMER_START(trustee_timer[ChairID], m_Delay[ChairID]);			
			log_info("CTrustee::%s tid:%d chair:%d timer restart\n", __FUNCTION__, table->tid, ChairID);
		}
		else
		{
			trustee_timer_running[ChairID] = false;
			TIMER_STOP(trustee_timer[ChairID]);
			log_info("CTrustee::%s tid:%d chair:%d timer stop\n", __FUNCTION__, table->tid, ChairID);
		}
	}
	if (ChairID == table->m_bCurChairID && iOpType == TYPE_PASS)
		m_CurChairID = ChairID;
}
void CTrustee::DoMultiOperate(_uint8 ChairID, int iOpType)
{
	if (m_Robot[ChairID])
	{
		trustee_timer_running[ChairID] = false;
		TIMER_STOP(trustee_timer[ChairID]);
		log_info("CTrustee::%s tid:%d chair:%d timer stop\n", __FUNCTION__, table->tid, ChairID);
	}
}
void CTrustee::UserAfterOperate(_uint8 ChairID)
{
	m_CurChairID = ChairID;
	if (m_Robot[ChairID])
	{
		trustee_timer_running[ChairID] = true;
		TIMER_START(trustee_timer[ChairID], m_Delay[ChairID]);
		log_info("CTrustee::%s tid:%d chair:%d timer start\n", __FUNCTION__, table->tid, ChairID);
	}
}
void CTrustee::RobGangOperate(_uint8 ChairID)
{
	m_CurChairID = ChairID;
	if (m_Robot[ChairID])
	{
		trustee_timer_running[ChairID] = true;
		TIMER_START(trustee_timer[ChairID], m_Delay[ChairID]);		
		log_info("CTrustee::%s tid:%d chair:%d timer start\n", __FUNCTION__, table->tid, ChairID);
	}
}
void CTrustee::TimerOut(_uint8 ChairID)
{
	try
	{
		if (m_Robot[ChairID])
		{
			log_info("%s trustee tid:%d chairid:%d\n", __FUNCTION__, table->tid, ChairID);
			if (table->m_bGameState == GAME_FREE)
			{
				//table->User_Ready(ChairID, ACTION_READY);
			}
			else if (table->m_bRobWanGang)
			{
				RobotOutOperate(ChairID);
			}
			else if (table->m_bCurChairID == ChairID)
			{
				if ((table->m_bUserOperate[ChairID] & ~(TYPE_LISTEN)) != TYPE_NULL)
				{
					RobotGetOperate(ChairID);
				}
				else
				{
					RobotOutCard(ChairID);
				}
			}
			else
			{
				if (table->m_bMultiOpUserNum > 1)
				{
					if ((table->m_bUserOperate[ChairID] & ~(TYPE_LISTEN)) != TYPE_NULL)
					{
						RobotOutOperate(ChairID);
					}
				}
				else if ((table->m_bUserOperate[ChairID] & ~(TYPE_LISTEN)) != TYPE_NULL)
				{
					RobotOutOperate(ChairID);
				}
				else
				{
					RobotOutCard(ChairID);
				}
			}
		}
	}
	//try {}
	catch (...)
	{
		log_info("%s trustee chairid:%d force stop\n", __FUNCTION__, ChairID);
	}
}


void CTrustee::RobotOutCard(_uint8 ChairID)
{	
	tagListenInfos stListenInfo[14];
	_uint8 bListenLen = 0;
	if (table->GetListeningInfo(ChairID, stListenInfo, bListenLen) == TYPE_LISTEN)
	{
		CardNode_count res(0, 0);
		for (int i =0; i < bListenLen; i++)
		{
			const tagHuInfos* info = stListenInfo->HuInfo;			
			_uint8 left_total = 0;
			for (int j =0 ; j < stListenInfo->bHuLen; j++)
			{
				_uint8 card = info[j].bCard;
				left_total += m_CardCounts[card];
			}
			if (left_total > res.count)
			{
				res = CardNode_count(stListenInfo[i].bOutCard, left_total);
			}
		}
		if (table->m_GameLogic.GetCardCountH(&table->m_HandCard[ChairID], res.card) > 0)
		{
			table->recv_client_outcard(ChairID, res.card);
			return;
		}
	}

	CardNode_set h;
	table->m_GameLogic.GetCardCount(&table->m_HandCard[ChairID], h);

	for (int i =0;i < h.Size(); i++)
	{
		if (!m_GuiCard.empty())
		{
			if (h[i].card == m_GuiCard[0])
			{
				h[i].count = 0;
				break;
			}
		}
	}
	CardNode_package package;	
	table->m_GameLogic.AnalysisCardCount(FIXED_DEEP, h, package);	

	TrusteeScore score;
	if (table->m_GameLogic.GetCardCountH(&table->m_HandCard[ChairID], table->m_bCurCard) == 0) 
	{		
		for (int i = 0; i < h.Size(); i++)
		{
			if (h[i].count != 0)
			{
				score.OutCard = h[i].card;
			}
		}		
	}
	else
	{
		score.OutCard = table->m_bCurCard;
	}
	
		
	score.iOpType = TYPE_NULL;
	RobotOutCardScore(h, package, score);
	if (score.iOpType == TYPE_NULL)
	{
		table->recv_client_outcard(ChairID, score.OutCard);
	}
}
void CTrustee::RobotGetOperate(_uint8 ChairID)
{
	int iOpType = table->m_bUserOperate[ChairID];
	if (iOpType & TYPE_HU)
	{
		table->OnRecvUserOperate(ChairID, TYPE_HU, table->m_bCurCard);
	}
	else if (iOpType & TYPE_GANG_EXT)
	{
		_uint8 bCurCard = table->m_bCurCard;

		MJ_opinfo opinfo;
		table->GetGangInfo(ChairID, opinfo);
		if (opinfo.NotNull())
		{
			for (int i = 0; i < opinfo.Size(); i++)
			{
				if (bCurCard == opinfo[i].cbCard)
					goto __FIND_CARD__;
			}
			bCurCard = opinfo.Last().cbCard;
		}
	__FIND_CARD__:

		HandCards & tmp = table->m_HandCard[ChairID];

		CardNode_set h;
		table->m_GameLogic.GetCardCount(&tmp, h);

		CardNode_package package;
		table->m_GameLogic.AnalysisCardCount(FIXED_DEEP, h, package);

		TrusteeScore score;
		score.OutCard = bCurCard;
		score.iOpType = iOpType;
		RobotOutCardScore(h, package, score);

		if (score.iOpType == TYPE_PASS)
		{
			table->OnRecvUserOperate(ChairID, TYPE_PASS, table->m_bCurCard);
		}
		else if (score.iOpType != TYPE_NULL)
		{
			MJ_opinfo opinfo;
			table->GetGangInfo(ChairID, opinfo);
			if (opinfo.NotNull())
			{
				for (int i = 0; i < opinfo.Size(); i++)
				{
					if (opinfo[i].cbCard == score.OutCard)
					{
						table->OnRecvUserOperate(ChairID, opinfo[i].nState, opinfo[i].cbCard);
					}
				}
			}
		}
		else
		{
			table->OnRecvUserOperate(ChairID, TYPE_PASS, table->m_bCurCard);
		}
	}	
}
void CTrustee::RobotOutOperate(_uint8 ChairID)
{
	int iOpType = table->m_bUserOperate[ChairID];
	
	if (iOpType & TYPE_HU)
	{
		if (table->m_bRobWanGang)
		{
			if (!CheckWhetherPickUpGun(ChairID, table->m_RobWanGangCard))
			{
				table->OnRecvUserOperate(ChairID, TYPE_PASS, 0);
			}
			table->OnRecvUserOperate(ChairID, TYPE_HU, table->m_RobWanGangCard);
		}
		else
		{
			Player* p = table->m_SeatPlayer[ChairID].player;
			if (ChairID == table->m_bOutCardChairID)
			{
				if (!CheckWhetherPickUpGun(ChairID, table->m_bCurCard))
				{
					table->OnRecvUserOperate(ChairID, TYPE_PASS, 0);
				}
				table->OnRecvUserOperate(ChairID, TYPE_HU, table->m_bCurCard);
			}
			else
			{
				_uint8 cur = table->m_bOutCardChairID;
				Player* curPlayer = table->m_SeatPlayer[cur].player;
				if (!p->m_isRobot || curPlayer->m_isRobot)
				{
					table->OnRecvUserOperate(ChairID, TYPE_PASS, 0);
				}
				else
				{
					if (!CheckWhetherPickUpGun(ChairID, table->m_bCurCard))
					{
						table->OnRecvUserOperate(ChairID, TYPE_PASS, 0);
					}
					table->OnRecvUserOperate(ChairID, TYPE_HU, table->m_bCurCard);
				}
			}			
		}
	}
	else
	{
		HandCards& tmp = table->m_HandCard[ChairID];		

		CardNode_set h;
		table->m_GameLogic.GetCardCount(&tmp, h);

		CardNode_package package;
		table->m_GameLogic.AnalysisCardCount(FIXED_DEEP, h, package);

		TrusteeScore score;
		score.OutCard = table->m_bCurCard;
		score.iOpType = iOpType;
		RobotOutCardScore(h, package, score);

		iOpType = score.iOpType;
		if (iOpType == TYPE_PASS)
		{
			table->OnRecvUserOperate(ChairID, TYPE_PASS, table->m_bCurOutCard);
		}
		else if (iOpType & TYPE_GANG_EXT)
		{
			MJ_opinfo opinfo;
			table->GetPengGang(ChairID, table->m_bCurOutCard, opinfo);
			for (int i = 0; i < opinfo.Size(); i++)
			{
				if (opinfo[i].nState & TYPE_GANG)
				{
					table->OnRecvUserOperate(ChairID, opinfo[i].nState, opinfo[i].cbCard);
				}
			}
		}
		else if (iOpType & TYPE_PENG)
		{
			MJ_opinfo opinfo;
			table->GetPengGang(ChairID, table->m_bCurOutCard, opinfo);
			for (int i = 0; i < opinfo.Size(); i++)
			{
				if (opinfo[i].nState & TYPE_PENG)
				{
					table->OnRecvUserOperate(ChairID, opinfo[i].nState, opinfo[i].cbCard);
				}
			}
		}
		else if (iOpType & TYPE_CHI)
		{
			MJ_opinfo opinfo;
			table->GetChi(ChairID, table->m_bCurOutCard, opinfo);
			table->GetPengGang(ChairID, table->m_bCurOutCard, opinfo);
			for (int i = 0; i < opinfo.Size(); i++)
			{
				if (opinfo[i].nState & TYPE_CHI)
				{
					table->OnRecvUserOperate(ChairID, opinfo[i].nState, opinfo[i].cbCard);
				}
			}
		}
		else
		{
			table->OnRecvUserOperate(ChairID, TYPE_PASS, table->m_bCurOutCard);
		}
	}
}

bool CTrustee::CheckWhetherPickUpGun(_uint8 ChairID, _uint8 huCard)
{
	tagListenInfos stListenInfo[14];
	_uint8 bListenLen = 0;
	table->GetListeningInfo(ChairID, stListenInfo, bListenLen);
	for (int i = 0; i < bListenLen; i++)
	{
		if (stListenInfo[i].bOutCard == huCard)
		{
			const tagHuInfos* info = stListenInfo->HuInfo;
			_uint8 cardCnt = 0;
			for (int j = 0; j < stListenInfo->bHuLen; j++)
			{
				_uint8 card = info[j].bCard;
				cardCnt += m_CardCounts[card];
			}
			if (cardCnt >= 3)
			{
				return false;
			}
			break;
		}
	}
	return true;
}

void CTrustee::TriggerTimer(_uint8 ChairID)
{
	log_info("%s mid:%d table:%d uid:%d\n", __FUNCTION__, table->matchid, table->tid, table->GetPlayerUid(ChairID));
	if (ChairID == INVALID_CHAIRID)
	{
		ChairID = table->m_bBanker;
	}
	for (int i = 0; i < table->m_GAME_PLAYER; i++)
	{
		Player * player = table->m_SeatPlayer[i].player;
		if (player != NULL && !player->m_isRobot)
		{
			m_Delay[i] = table->m_bDelayTime;
		}
		else
		{
			m_Delay[i] = table->m_RobotDelayTime;
		}
		m_Robot[i] = true;
	}
	if (m_Robot[ChairID] && ChairID == m_CurChairID)
	{
		trustee_timer_running[ChairID] = true;
		TIMER_START(trustee_timer[ChairID], m_Delay[ChairID]);		
	}	
}
static std::string g_scorestring;
void printScore(const ScoreNode::Score& s)
{
	char str[100];
	sprintf(str, "card:0x%x(%f)\t", s.card, s.score);
	g_scorestring += str;
}
void printScoreNode(int tid, const ScoreNode& node)
{
	log_info("%s tid:%d dan:%d bianka:%d kezi:%d pos:%d\n", __FUNCTION__, tid, node.dan, node.bianka, node.kezi, node.pos);	
	g_scorestring.clear();
	for_each(node.outcard.begin(),
		node.outcard.end(),
		printScore);
	log_info("%s tid:%d %s\n", __FUNCTION__, tid, g_scorestring.c_str());
	g_scorestring.clear();
}
void printPackage(int tid, const CardNode_package& package, int pos)
{	
	const CardNode_type_set& s = package[pos];
	const char* type_str[] = { "dan", "shun", "ka", "bian", "ke" };		
	for (int i = 0; i < s.Size() ; i++)
	{
		int type = s[i].type;
		log_info("%s tid:%d card:0x%x type:%s res:%d\n", __FUNCTION__, tid, s[i].card, type_str[type], s[i].res);
	}
}

bool CTrustee::RobotOutCardScore(CardNode_set& h, CardNode_package& package, TrusteeScore& score)
{
	if (score.iOpType == TYPE_NULL)
	{
		MJ_BUFFER<ScoreNode, CARDNODE_SIZE_MAX> node;
		
		ScoreNode_filter0(h, package, node);
		
		ScoreNode_filter1(h, package, node);

		ScoreNode_filter2(h, package, node);

		ScoreNode_filter3(h, package, node);

		if (node.NotNull())
		{		
			ScoreNode& first_node = node[0];
			//printScoreNode(table->tid, first_node);
			//printPackage(table->tid, package, first_node.pos);

			if (first_node.outcard.begin() != first_node.outcard.end())
			{
				 score.OutCard = first_node.outcard.begin()->card;
				 score.iOpType = TYPE_NULL;
				 return true;
			}
			else
			{
				if (first_node.dan == 0)
				{
					if (first_node.kezi == 0)
					{
						CardNode_type_set& s = package[first_node.pos];
						for (int i = 0; i < s.Size(); i++)
						{
							if (s[i].res > 0)
							{
								score.OutCard = s[i].card;
								score.iOpType = TYPE_NULL;
								return true;
							}
						}
					}
					else if (first_node.kezi == 1)
					{
						CardNode_type_set& s = package[first_node.pos];
						for (int i = 0; i < s.Size(); i++)
						{
							if (s[i].type == E_CARDNODE_TYPE_KEZI)
							{
								score.OutCard = s[i].card;
								score.iOpType = TYPE_NULL;
								return true;
							}
						}
					}
					else
					{
						CardNode_type_set& s = package[first_node.pos];
						for (int i = 0; i < s.Size(); i++)
						{
							if (s[i].res > 0)
							{
								score.OutCard = s[i].card;
								score.iOpType = TYPE_NULL;
								return true;
							}
						}
					}
				}
				else
				{
					CardNode_type_set& s = package[first_node.pos];
					for (int i =0; i < s.Size(); i++)
					{
						if (s[i].type == E_CARDNODE_TYPE_DAN)
						{
							score.OutCard = s[i].card;
							score.iOpType = TYPE_NULL;
							return true;
						}
					}
				}
			}
		}		
		//score.OutCard = table->m_bCurCard;
		score.iOpType = TYPE_NULL;
		return true;
	}
	else
	{
		int iOpType = score.iOpType;
		MJ_BUFFER<ScoreNode, CARDNODE_SIZE_MAX> node;
		ScoreNode_filter0(h, package, node);
		ScoreNode_filter1(h, package, node);
		ScoreNode_filter2(h, package, node);
		ScoreNode_filter3(h, package, node);

		
		_uint8 card = score.OutCard;
		if (iOpType & TYPE_ANGANG)
		{
			Add(h, card, -4);
			iOpType = TYPE_ANGANG;
		}
		else if (iOpType & TYPE_WANGANG)
		{
			Add(h, card, -1);
			iOpType = TYPE_WANGANG;
		}
		else if (iOpType & TYPE_ZHIGANG)
		{
			Add(h, card, -3);
			iOpType = TYPE_ZHIGANG;
		}
		else if (iOpType & TYPE_PENG)
		{
			Add(h, card, -2);
			iOpType = TYPE_PENG;
		}
		else if (iOpType & TYPE_CHI){}

		CardNode_package after_package;
		MJ_BUFFER<ScoreNode, CARDNODE_SIZE_MAX> after_node;
		table->m_GameLogic.AnalysisCardCount(FIXED_DEEP, h, after_package);
		ScoreNode_filter0(h, after_package, after_node);
		ScoreNode_filter1(h, after_package, after_node);
		ScoreNode_filter2(h, after_package, after_node);
		ScoreNode_filter3(h, after_package, after_node);

	__FUNC_AGAIN__:
		if (node.NotNull())
		{
			if (after_node.NotNull())
			{
				ScoreNode& first_node = node[0];
				ScoreNode& first_node1 = after_node[0];
				if (first_node.outcard.size() < first_node1.outcard.size())
				{
					goto __FUNC_END__;
				}
				score.iOpType = iOpType;
				return true;
			}
			else
			{
				score.iOpType = iOpType;
				return true;				
			}
		}

	__FUNC_END__:
		if (iOpType == TYPE_ZHIGANG)
		{
			Add(h, card, 1);
			iOpType = TYPE_PENG;
			after_package.Clear();
			after_node.Clear();
			table->m_GameLogic.AnalysisCardCount(FIXED_DEEP, h, after_package);
			ScoreNode_filter0(h, after_package, after_node);
			ScoreNode_filter1(h, after_package, after_node);
			ScoreNode_filter2(h, after_package, after_node);
			ScoreNode_filter3(h, after_package, after_node);
			goto __FUNC_AGAIN__;
		}
		//score.OutCard = score.OutCard;
		score.iOpType = TYPE_PASS;
		return true;
	}
	return false;
}

void CTrustee::restart()
{
	printf("CTrustee::restart\n");
	//bool tuiChu = false;
	//for (_uint8 c = table->NextChair(m_CurChairID); c != m_CurChairID; c = table->NextChair(c))
	//{
	//	if (m_Robot[c])
	//	{
	//		int iOpType = table->m_bUserOperate[c] & ~(TYPE_LISTEN);
	//		if (iOpType != TYPE_NULL)
	//		{
	//			trustee_timer_running[c] = true;
	//			TIMER_START(trustee_timer[c], m_Delay[c]);
	//			tuiChu = true;
	//			log_info("CTrustee::%s %d timer start\n", __FUNCTION__, c);
	//		}
	//	}
	//}
	//if (tuiChu)
	//{
	//	return;
	//}
	//if (m_Robot[m_CurChairID])
	//{
	//	if (table->m_SeatPlayer[m_CurChairID].player->m_isRobot)
	//	{//确保超时时间是机器人超时时间
	//		m_Delay[m_CurChairID] = table->m_RobotDelayTime;
	//	}
	//	else
	//	{
	//		m_Delay[m_CurChairID] = table->m_bDelayTime;
	//	}
	//	trustee_timer_running[m_CurChairID] = true;
	//	TIMER_START(trustee_timer[m_CurChairID], m_Delay[m_CurChairID]);
	//	log_info("CTrustee::%s tid:%d chair:%d  timer start\n", __FUNCTION__, table->tid, m_CurChairID);
	//}


	for (int i = 0; i < GAME_PLAYER; i++)
	{
		if (trustee_timer_running[i])
		{
			if (table->m_SeatPlayer[i].player->m_isRobot)
			{//确保超时时间是机器人超时时间
				TIMER_START(trustee_timer[i], table->m_RobotDelayTime);
			}
			else
			{
				TIMER_START(trustee_timer[i], table->m_bDelayTime);
			}
		}
	}
}
