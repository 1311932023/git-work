#include "table.h"
#include "zjh.h"
#include "common/log.h"
#include "proto/proto.h"
#include "mj_common2.h"

extern ZJH zjh;


int Table::Disband_timer_remaintime(_uint8 ChairID)
{
	int remaintime = ev_timer_remaining(zjh.loop, &m_Disbandtable_Timer0);	
	return remaintime;	
}



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
				log_info("%s tid:%d uid:%d ", __FUNCTION__, table->tid, table->GetPlayerUid(i));
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
		log_info("%s uid:%d is %s  ", __FUNCTION__, GetPlayerUid(i), m_IsOffline[i] ? "offline" : "online");
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

//�û���ɢ��������
void Table::RecvDisbandTableApply(Player *player)
{
	log_info(" disband tid:%d apply uid:%d ", tid, player->uid);
	if (m_bDeleteTable)
	{
		log_error("tid:%d is already delete \n", tid);
		return;
	}

	if (check_roomowner_disband_table(player->uid, m_room_status))
	{
		log_error("uid[%d] room_status[%d]" , player->uid , m_room_status);
		return;
	}

	//char buf[64] = {0};
	proto::game::AckDisbandChooseFaild stAck;
	stAck.Clear();
	if (m_bIsDisbandTable)
	{
		log_info("table is disband now, so can not recv another disband apply ");
		stAck.set_chairid(player->seatid);
		stAck.set_code(103);//�������������ɢ����

		unicast(SERVER_APPLY_DISBAND_FAILD, player, stAck);
		return;
	}
	_uint8 bChairID = player->seatid;
	stAck.set_chairid(bChairID);
	bool isPlaying = false;
	if (bChairID >= 0 && bChairID < m_GAME_PLAYER)
	{
		if ( m_SeatPlayer[bChairID].player != NULL)
		{
			if ( m_SeatPlayer[bChairID].player->uid == player->uid)
			{
				isPlaying = true;
			}
		}
	}
	if (!isPlaying)
	{
		log_info("uid:%d chairid:%d is not at seat, so can apply disband room ", GetPlayerUid(player), bChairID);

		stAck.set_code(102);

		unicast(SERVER_APPLY_DISBAND_FAILD, player, stAck);
		return;
	}
	if (player->DisBandCount >= m_bDisbandCount )
	{
		log_debug("disbandcount [%d]  m_bDisbandCount[%d]", player->DisBandCount, m_bDisbandCount );
		stAck.set_code(101);
		unicast(SERVER_APPLY_DISBAND_FAILD, player, stAck);
		return;
	}

	m_ApplyDisbandChairid = bChairID;
	m_bIsDisbandTable = true;
	m_DisbandChoose[bChairID] = proto::game::DISBAND_YES;//������Ĭ��Ϊͬ���ɢ
	Disband_table_Timer_Change(bChairID, m_Disbandtable_Timer_stamp);

	player->DisBandCount++;

	RecvDisbandTableChoose(bChairID, proto::game::DISBAND_YES);

}

//�û��Ƿ�ͬ���ɢ�����ѡ��
void Table::RecvDisbandTableChoose(_uint8 ChairID, int choosestype)
{

	if (choosestype != proto::game::DISBAND_YES && choosestype != proto::game::DISBAND_NO)
	{
		log_error("%s choosetype:%d is wrong type ", __FUNCTION__, choosestype);
		return;
	}
	if (ChairID >= m_GAME_PLAYER)
	{
		log_error("%s chairid:%d is wrong ", __FUNCTION__, ChairID);
		return;
	}
	if (m_DisbandChoose[ChairID] != proto::game::DISBAND_WAIT)
	{
		log_error("%s uid:%d chairid:%d has choosed\n", __FUNCTION__, GetPlayerUid(ChairID), ChairID);
	}

	m_DisbandChoose[ChairID] = choosestype;
	if (choosestype == proto::game::DISBAND_NO)
	{
		//�㲥��Ҳ���
		proto::game::AckDisbandChooseBc stAck;
		stAck.set_chairid(ChairID);
		stAck.set_choosestate(choosestype);
		broadcast(SERVER_DISBAND_CHOOSE, stAck);

		proto::game::AckDisbandResult stAckResult;
		stAckResult.set_result(0);
		broadcast(SERVER_DISBAND_RESULT, stAckResult);
		m_bIsDisbandTable = false;
		memset(m_DisbandChoose, proto::game::DISBAND_WAIT, sizeof(m_DisbandChoose));//��ʼ�����ѡ��

		Stop_Disband_timer(ChairID);
		log_info(" tid:%d uid:%d chairid:%d choose no ",  tid, GetPlayerUid(ChairID), ChairID);
		return;
	}

	//������ͶƱ��ҵĲ��������������Ҳ���ͶƱ
	_uint8 bChooseNum = 0; //�Ѿ�ѡ�����������
	_uint8 bChooseYesNum = 0; //��ѡ���ͬ������
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
	log_info("%s tid:%d player[%d,%d,%d,%d]choose[%d,%d,%d,%d] ", __FUNCTION__, tid,
		GetPlayerUid(0), GetPlayerUid(1), GetPlayerUid(2), GetPlayerUid(3),
		m_DisbandChoose[0], m_DisbandChoose[1], m_DisbandChoose[2], m_DisbandChoose[3]);

	if (bChooseNum == 2)
	{	
		//���ֻ����������˽�ɢ ������û�з�Ӧ���ٵ�2����			
		int remain = Disband_timer_remaintime(ChairID);
		if (remain > m_Disbandtable_Timer_stamp1)
		{
			Disband_table_Timer_Change(m_ApplyDisbandChairid, m_Disbandtable_Timer_stamp1);
		}
	}
	//�㲥��Ҳ���
	proto::game::AckDisbandChooseBc stAck;
	stAck.set_chairid(ChairID);
	stAck.set_choosestate(choosestype);
	broadcast(SERVER_DISBAND_CHOOSE, stAck);

	//assert(m_Disband_ResUser < m_GAME_PLAYER);
	if (bChooseYesNum == MIN(_uint8(m_GAME_PLAYER - m_Disband_ResUser), SeatPlayerCount()))
	{
		log_info("into this !!");
		Stop_Disband_timer(ChairID);
		proto::game::AckDisbandResult stAckResult;
		stAckResult.set_result(1);

		broadcast(SERVER_DISBAND_RESULT, stAckResult);

		GameBreakOff();
		send_video_to_svr();

		//ʹ�ü�ʱ�������滻 ֱ�ӷ�����Ϣ����ƽ̨�޷�����	
		ev_tstamp tmp_tstamp = 1;
		TIMER_AGAIN(m_RecvDisbandTable_Timer, tmp_tstamp);
		//Send_DisbandTableMessage();

		m_bIsDisbandTable = false;
		memset(m_DisbandChoose, proto::game::DISBAND_WAIT, sizeof(m_DisbandChoose));//��ʼ�����ѡ��
	}

}

int Table::SendTableDisBandOff()
{
	//Stop_Disband_timer(ChairID);
	proto::game::AckDisbandResult stAckResult;
	stAckResult.set_result(1);

	broadcast(SERVER_DISBAND_RESULT, stAckResult);

	GameBreakOff();
	send_video_to_svr();

	//ʹ�ü�ʱ�������滻 ֱ�ӷ�����Ϣ����ƽ̨�޷�����	
	ev_tstamp tmp_tstamp = 1;
	TIMER_AGAIN(m_RecvDisbandTable_Timer, tmp_tstamp);
	//Send_DisbandTableMessage();

	m_bIsDisbandTable = false;
	memset(m_DisbandChoose, proto::game::DISBAND_WAIT, sizeof(m_DisbandChoose));//��ʼ�����ѡ��
	return 0; 
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

