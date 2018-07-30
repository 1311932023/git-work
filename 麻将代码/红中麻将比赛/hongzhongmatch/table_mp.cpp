#include "table.h"
#include "trustee//CardsCounts.h"
#include "match.h"
#include "common/log.h"

extern Log log;

int Table::CalcMPInfo(Json::Value& mpinfo)
{	
	log.info("%s:%d:%s matchid:%d\n",__FILE__,__LINE__,__FUNCTION__, matchid);
	for (int ChairID = 0; ChairID < m_GAME_PLAYER; ChairID++)
	{
		//if (!m_SeatPlayer[ChairID].player->m_isRobot)
		if (m_SeatPlayer[ChairID].player->m_Role == 0)
		{
			Player* player = m_SeatPlayer[ChairID].player;
			Json::Value r;
			r["mid"] = matchid;		
			
			//rinfo
			Json::Value& init_card = r["rinfo"]["start_card_type"];
			int start_pos = ChairID * (HAND_CARD_SIZE_MAX - 1);
			int	end_pos = (ChairID + 1) * (HAND_CARD_SIZE_MAX - 1);
			for (; start_pos < end_pos; start_pos++)
			{
				init_card.append(
					CardsCounts::Convert(
						m_CardPool.GetPool()[start_pos]));
			}
			if (ChairID == m_bBanker)
			{
				init_card.append(
					CardsCounts::Convert(
						m_CardPool.GetPool()[
							(HAND_CARD_SIZE_MAX - 1) * m_GAME_PLAYER]));
			}
			Json::Value& res_card = r["rinfo"]["card_hole"];
			for (start_pos = (HAND_CARD_SIZE_MAX - 1) * m_GAME_PLAYER; start_pos < m_CardPool.MaxCount(); start_pos++)
			{
				res_card.append(
					CardsCounts::Convert(
						m_CardPool.GetPool()[
							start_pos]));
			}
			Json::Value& hand_card = r["rinfo"]["end_card_type"];
			for (int i = 0; i < m_HandCard[ChairID].ChangeableCardsLen; i++)
			{
				hand_card.append(
					CardsCounts::Convert(
						m_HandCard[ChairID].ChangeableCards[i]));
			}
			for (int i= 0; i < m_HandCard[ChairID].FixedCardsLen; i++)
			{
				if (m_HandCard[ChairID].FixedCards[i].state & TYPE_CHI)
				{
					_uint8 card = 
						CardsCounts::Convert(
							m_HandCard[ChairID].FixedCards[i].CardData);
					hand_card.append(card);
					hand_card.append(card + 1);
					hand_card.append(card + 2);
				}
				else if (m_HandCard[ChairID].FixedCards[i].state & TYPE_PENG)
				{
					_uint8 card =
						CardsCounts::Convert(
							m_HandCard[ChairID].FixedCards[i].CardData);
					hand_card.append(card);
					hand_card.append(card);
					hand_card.append(card);
				}
				else if (m_HandCard[ChairID].FixedCards[i].state & TYPE_GANG)
				{
					_uint8 card =
						CardsCounts::Convert(
							m_HandCard[ChairID].FixedCards[i].CardData);
					hand_card.append(card);
					hand_card.append(card);
					hand_card.append(card);
					hand_card.append(card);
				}
			}
			for (int i = 0; i < m_niao.Size(); i++)
			{
				r["rinfo"]["niao_card"].append(
					CardsCounts::Convert(
						m_niao[i]));
			}
			r["rinfo"]["banker_player_status"] = ChairID == m_bBanker ? 1 : 0;
			if (m_bIsHu[ChairID])
			{
				_uint8 tmpCard = INVALID_MJ_CARD;
				if (m_bRobHu)
				{
					tmpCard = m_RobWanGangCard;
				}
				else {
					tmpCard = m_bCurCard;
				}
				r["rinfo"]["hu_card_type"] = CardsCounts::Convert(tmpCard);
				if (m_stHuScore[ChairID].stHu[0].bType != HU_ZIMO)
				{
					hand_card.append(
						CardsCounts::Convert(tmpCard));
				}
			}
			MJ_user_buffer UserID;
			if (MultiHu(UserID) > 0)
			{
				if (m_bIsHu[ChairID])
				{
					r["rinfo"]["card_result"] = m_stHuScore[ChairID].stHu[0].bType == HU_ZIMO ? 4 : 5;
				}
				else if (ChairID == m_stHuScore[UserID[0]].stHu[0].bPasHuChairID)
				{
					r["rinfo"]["card_result"] = 2;
				}
				else
				{
					r["rinfo"]["card_result"] = 1;
				}
			}
			else
			{
				r["rinfo"]["card_result"] = 0;
			}
			r["rinfo"]["round_id"] = match->m_CurTurn - 1;
			r["rinfo"]["round"] = match->m_CurRound;
			r["rinfo"]["card_desk_id"] = player->tid;
			//uinfo
			r["uinfo"]["alterCoin"] = player->money - DEFINE_MONEY;
			r["uinfo"]["status"] = player->m_GiveUp ? 1 : 0;
			r["uinfo"]["tableNum"] = player->tid;
			r["uinfo"]["player_id"] = player->uid;
			r["uinfo"]["card_player_id"] = //player->seatid + 1;
				player->tmp_playerid;
			r["uinfo"]["card_numerical_order"] = m_CurRound;
			r["uinfo"]["player_position"] = player->seatid;
			r["uinfo"]["seat"] = player->seatid;

			for (_uint8 c = NextChair(ChairID); c != ChairID; c= NextChair(c))
			{
				Player* rival = m_SeatPlayer[c].player;
				if (rival != NULL && rival->m_Role == 0)
				{
					r["uinfo"]["card_rival"].append(rival->tmp_playerid);
					r["uinfo"]["rival"].append(rival->tmp_playerid);
				}				
			}

			r["mpinfo"]["base_score"] = player->m_BaseScore;
			r["mpinfo"]["zhigang_score"] = player->m_zhigang;
			r["mpinfo"]["wangang_score"] = player->m_wangang;
			r["mpinfo"]["angang_score"] = player->m_angang;
			r["mpinfo"]["final_score"] = player->m_CurMoney;
			r["mpinfo"]["card_score"] = player->m_CurMoney;
			r["mpinfo"]["mpScore"] = player->m_SettleInfo[m_CurRound].sMPScore;
			r["mpinfo"]["mpRatio"] = _4float(player->m_SettleInfo[m_CurRound].sMPRatio);
			r["mpinfo"]["mpRatioRank"] = player->m_SettleInfo[m_CurRound].sMPRatioRank;

			ExternMpInfo(r, player);

			mpinfo.append(r);
		}
	}	
	return mpinfo.size();
}

int Table::ExternMpInfo(Json::Value& mpinfo, Player* player)
{
	return 0;
}