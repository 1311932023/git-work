#include "table.h"
#include "zjh.h"
#include "common/log.h"
#include "proto/proto.h"
#include "mj_common2.h"

extern ZJH zjh;
extern Log log;
int Table::Disband_timer_remaintime(_uint8 ChairID)
{
	int remaintime = ev_timer_remaining(zjh.loop, &m_Disbandtable_Timer0);	
	return remaintime;	
}

// void Table::Start_Disband_timer(_uint8 bApplyChairID)
// {
// 	ev_timer_set(&m_Disbandtable_Timer0, m_Disbandtable_Timer_stamp, m_Disbandtable_Timer_stamp);
// 	ev_timer_again(zjh.loop, &m_Disbandtable_Timer0);
// }

void Table::Stop_Disband_timer(_uint8 ChairID)
{
	ev_timer_stop(zjh.loop, &m_Disbandtable_Timer0);	
}

void Table::Disband_table_Timer_cb0(struct ev_loop *loop, struct ev_timer *w, int revents)
{	
	Table *table = (Table*)w->data;
	if (table)
	{
		TIMER_STOP(table->m_Disbandtable_Timer0);
	
		for (int i = 0; i < table->m_GAME_PLAYER; i++)
		{
			if (table->m_DisbandChoose[i] == proto::game::DISBAND_WAIT)
			{
				log.info("%s tid:%d uid:%d\n", __FUNCTION__, table->tid, table->GetPlayerUid(i));
				table->RecvDisbandTableChoose(i, proto::game::DISBAND_YES);
			}
		}		
	}
}

void Table::Disband_table_Timer_Change(_uint8 ChairID, const ev_tstamp tstamp)
{	
	
	proto::game::AckDisbandApply stApply;
	for (int i = 0; i < m_GAME_PLAYER; i++)
	{
		log.info("%s uid:%d is %s \n", __FUNCTION__, GetPlayerUid(i), m_IsOffline[i] ? "offline" : "online");
		if (m_SeatPlayer[i].player == NULL)
		{
			continue;
		}
		stApply.add_onlinechairid(i);
	}
	stApply.set_applychairid(m_ApplyDisbandChairid);
	stApply.set_waittime(tstamp);
	broadcast(SERVER_APPLY_DISBAND_SUCC, stApply);	

	TIMER_AGAIN(m_Disbandtable_Timer0, tstamp);

}

//用户解散桌子请求

void Table::RecvDisbandTableApply(Player *player)
{
	log.info("%s disband tid:%d apply uid:%d\n", __FUNCTION__, tid, player->uid);
	if (m_bDeleteTable)
	{
		log.error("tid:%d is already delete \n", tid);
		return;
	}

	if (check_roomowner_disband_table(player->uid, m_room_status))
	{
		return;
	}

	proto::game::AckDisbandChooseFaild stAck;
	stAck.Clear();
	if (m_bIsDisbandTable)
	{
		log.info("table is disband now, so can not recv another disband apply \n");
		stAck.set_chairid(player->seatid);
		stAck.set_code(103);//有人正在申请解散房间

		unicast(SERVER_APPLY_DISBAND_FAILD, player, stAck);
		return;
	}
	_uint8 bChairID = player->seatid;
	stAck.set_chairid(bChairID);
	bool isPlaying = false;
	if (bChairID >= 0 && bChairID < m_GAME_PLAYER)
	{
		if (m_SeatPlayer[bChairID].player != NULL)
		{
			if (m_SeatPlayer[bChairID].player->uid == player->uid)
			{
				isPlaying = true;
			}
		}
	}
	if (!isPlaying)
	{
		log.info("uid:%d chairid:%d is not at seat, so can apply disband room \n", GetPlayerUid(player), bChairID);

		stAck.set_code(102);

		unicast(SERVER_APPLY_DISBAND_FAILD, player, stAck);
		return;
	}
	if (player->DisBandCount >= m_bDisbandCount)
	{
		stAck.set_code(101);

		unicast(SERVER_APPLY_DISBAND_FAILD, player, stAck);
		return;
	}

	m_ApplyDisbandChairid = bChairID;
	m_bIsDisbandTable = true;
	m_DisbandChoose[bChairID] = proto::game::DISBAND_YES;//发起者默认为同意解散
	Disband_table_Timer_Change(bChairID, m_Disbandtable_Timer_stamp);

	player->DisBandCount++;
	RecvDisbandTableChoose(bChairID, proto::game::DISBAND_YES);
}

//用户是否同意解散房间的选择
void Table::RecvDisbandTableChoose(_uint8 ChairID, int choosestype)
{

	if (choosestype != proto::game::DISBAND_YES && choosestype != proto::game::DISBAND_NO)
	{
		log.error("%s choosetype:%d is wrong type \n", __FUNCTION__, choosestype);
		return;
	}
	if (ChairID >= m_GAME_PLAYER)
	{
		log.error("%s chairid:%d is wrong \n", __FUNCTION__, ChairID);
		return;
	}
	if (m_DisbandChoose[ChairID] != proto::game::DISBAND_WAIT)
	{
		log.error("%s uid:%d chairid:%d has choosed\n", __FUNCTION__, GetPlayerUid(ChairID), ChairID);
	}

	m_DisbandChoose[ChairID] = choosestype;
	if (choosestype == proto::game::DISBAND_NO)
	{
		//广播玩家操作
		proto::game::AckDisbandChooseBc stAck;
		stAck.set_chairid(ChairID);
		stAck.set_choosestate(choosestype);
		broadcast(SERVER_DISBAND_CHOOSE, stAck);

		proto::game::AckDisbandResult stAckResult;
		stAckResult.set_result(0);
		broadcast(SERVER_DISBAND_RESULT, stAckResult);
		m_bIsDisbandTable = false;
		memset(m_DisbandChoose, proto::game::DISBAND_WAIT, sizeof(m_DisbandChoose));//初始化玩家选择

		Stop_Disband_timer(ChairID);
		log.info("%s tid:%d uid:%d chairid:%d choose no\n", __FUNCTION__, tid, GetPlayerUid(ChairID), ChairID);
		return;
	}

	//检查参与投票玩家的操作结果，在线玩家参与投票
	_uint8 bChooseNum = 0; //已经选择操作的人数
	_uint8 bChooseYesNum = 0; //已选择的同意的玩家
	for (int i = 0; i < m_GAME_PLAYER; i++)
	{
		if (m_DisbandChoose[i] != proto::game::DISBAND_WAIT)
		{
			bChooseNum++;
		}
		if (m_DisbandChoose[i] == proto::game::DISBAND_YES)
		{
			bChooseYesNum++;
		}
	}
	log.info("%s tid:%d player[%d,%d,%d,%d]choose[%d,%d,%d,%d]\n", __FUNCTION__, tid,
		GetPlayerUid(0), GetPlayerUid(1), GetPlayerUid(2), GetPlayerUid(3),
		m_DisbandChoose[0], m_DisbandChoose[1], m_DisbandChoose[2], m_DisbandChoose[3]);

	if (bChooseNum == 2)
	{//如果只有玩家申请了解散 其他人没有反应则再等2分钟			
		int remain = Disband_timer_remaintime(ChairID);
		if (remain > m_Disbandtable_Timer_stamp1)
		{
			Disband_table_Timer_Change(m_ApplyDisbandChairid, m_Disbandtable_Timer_stamp1);
		}
	}
	//广播玩家操作
	proto::game::AckDisbandChooseBc stAck;
	stAck.set_chairid(ChairID);
	stAck.set_choosestate(choosestype);
	broadcast(SERVER_DISBAND_CHOOSE, stAck);

	assert(m_Disband_ResUser < m_GAME_PLAYER);
	_uint8 bNeedNum = MIN(_uint8(m_GAME_PLAYER - m_Disband_ResUser), SeatPlayerCount());
	if (bNeedNum < 2)
	{
		bNeedNum = 2;
	}
	if (bChooseYesNum == bNeedNum)
	{
		Stop_Disband_timer(ChairID);
		proto::game::AckDisbandResult stAckResult;
		stAckResult.set_result(1);

		broadcast(SERVER_DISBAND_RESULT, stAckResult);

		GameBreakOff();
		send_video_to_svr();

		//使用计时器发送替换 直接发送消息避免平台无法处理	
		ev_tstamp tmp_tstamp = 1;
		TIMER_AGAIN(m_RecvDisbandTable_Timer, tmp_tstamp);
		//Send_DisbandTableMessage();

		m_bIsDisbandTable = false;
		memset(m_DisbandChoose, proto::game::DISBAND_WAIT, sizeof(m_DisbandChoose));//初始化玩家选择
	}

}



int Table::Disband_table_resp(Jpacket packet)
{
	TIMER_STOP(m_RecvDisbandTable_Timer);

	int code = packet.val["code"].asInt();
	proto::game::AckGameStartFail stGameStartFail;
	stGameStartFail.set_vid(vid);
	char strroomid[128] = { 0 };
	sprintf(strroomid, "%d", tid);
	stGameStartFail.set_roomid(strroomid);
	stGameStartFail.set_can_start(0);
	stGameStartFail.set_code(code);
	int ntime = (int)time(NULL);
	stGameStartFail.set_ts(ntime);

	broadcast(SERVER_DZ_GAME_START_RES, stGameStartFail);

	GameEndReset();
	return 0;
}

