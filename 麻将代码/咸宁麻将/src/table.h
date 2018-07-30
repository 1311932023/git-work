#ifndef _TABLE_H_
#define _TABLE_H_

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
#include <vector>
#include <list>

#include <json/json.h>

#include <map>
#include <set>

#include "table_helper.h"
#include "game.h"
#include "zjh.h"
#include "message_word.h"
#include "proto/proto.h"
#include "proto/gameproto.pb.h"
#include "common/jpacket.h"
#include "common/client.h"
#include "common/player.h"
#include "common/log.h"
#include "libnormalmahjong/common.h"
#include "libnormalmahjong/game_logic.h"
#include "libnormalmahjong/common_mahjong_logic.h"
#include "libnormalmahjong/common_mahjong_def.h"

#include "robot_manager.h"
#include "table_def.h"
#include "game_account.h"

#define TIMER_AGAIN(TIMER, STAMP)	do {ev_timer_stop(zjh.loop, &(TIMER));\
									ev_timer_set(&(TIMER), (STAMP), (STAMP));\
									ev_timer_again(zjh.loop, &(TIMER));}while(0)

#define TIMER_AGAIN_s(TIMER)			do{ev_timer_again(zjh.loop, &(TIMER));}while(0)

#define TIMER_STOP(TIMER)			do{ev_timer_stop(zjh.loop, &(TIMER));}while(0)

#define MaxVideoBufLen  (1024 * 200)

class Player;
class Client;


//游戏状态
typedef enum tagGameStatus
{
	GAME_FREE = 0,
	GAME_CHANGECARD = 1,	//选择换牌	
	GAME_CHOOSE_LACK = 2,//选择定缺色
	GAME_PLAYING = 3,
	//GAME_QUAN_FENG = 4,	     //圈风场景	
	//GAME_PIAO = 5,            //选票场景
	GAME_PIAO = 4,            //选票场景
}GAME_STATUS;

//#pragma pack(1)

class Seat
{
public:
	enum SeatState
	{
		E_Seat_Empty,
		E_Seat_Occupy,
	};
	SeatState state; // 该椅子是否可用, 0 : enable 1 : disable 
	int seatid; //
	int action; // 参考枚举 proto::game: ACTION	
	int uid;
	Player *player;	
    Seat()
	{
		state = E_Seat_Empty;
		seatid = 0;
		action = ACTION_SIDOWN;
		player = NULL;
		uid = 0;
	}
	void clear(void)
	{
		state = E_Seat_Empty;
		seatid = 0;
		action = ACTION_SIDOWN;
		player = NULL;
		uid = 0;
	}
	void leave()
	{
		seatid = INVALID_CHAIRID;
		action = ACTION_LEAVE;
		player = NULL;
	}
};


//记录多操作中优先级低的操作
typedef struct tagRecordLowLevelOperate
{
    int iOpType;
    _uint8 ChairID;
	_uint8 bCard;
    tagRecordLowLevelOperate()
    {
        iOpType = TYPE_NULL;
        ChairID = 0xFF;
		bCard = 0xFF;
    }
}LOWLEVELOPERATE;


//听牌的胡牌信息
struct tagHuInfos
{
	_uint8          bCard;              //胡的牌
	_uint8          bLeftCount;         //该牌剩余数
	_uint8          bFanShu;            //胡牌的番数
};

//听牌信息
struct tagListenInfos
{
	_uint8          bOutCard;           //需要打出的牌
	tagHuInfos      HuInfo[18];         //打出牌后可以胡牌的信息
	_uint8			bHuLen;
	tagListenInfos()
	{
		bOutCard = 0;
		memset(HuInfo, 0, sizeof(HuInfo));
		bHuLen = 0;
	}
};

//杠牌分数、杠谁的牌
class tagGangScore
{
public:	
	_uint8 Card;	
	int ChairID;		//杠牌赢谁的钱
	int iType;	
	tagGangScore()
	{
		Card = 0;		
		ChairID = INVALID_CHAIRID;
	}
	tagGangScore(_uint8 card, int id, int type) : Card(card), ChairID(id), iType(type)
	{
	}	
};

//杠牌信息 
class tagGangScoreInfo 
{
public:
	int iTotalScore;		//杠牌输赢总分	
	MJ_BUFFER<tagGangScore, 4> Gang; //杠牌信息
	tagGangScoreInfo()
	{
		iTotalScore = 0;	
	}
};

//记录胡牌信息
class tagHuRecord
{	
public:
	_uint8	bHuCard;		//胡的牌
	_uint8	bPasHuChairID;	//胡谁的牌	
	_uint8  bType;	
	int iFanNum;	
	tagHuRecord()
	{
		bHuCard = INVALID_MJ_CARD;
		bPasHuChairID = INVALID_CHAIRID;	
	}
	tagHuRecord(_uint8 id, _uint8 card, _uint8 type, int fannum) : bHuCard(card), bPasHuChairID(id), bType(type), iFanNum(fannum)
	{
	}
	
};

class tagHuInfo
{
public:
	_tint64	iTotalScore;	//胡牌输赢总分	
	MJ_BUFFER<tagHuRecord, 6> stHu;
	tagHuInfo()
	{
		iTotalScore = 0;		
	}
	
};

struct tagAddFanInfo     //另加番
{
	int nType;       //引起加番的类型
	int nFan;        //加的番数
	tagAddFanInfo()
	{
		nType = 0;
		nFan = 0;
	}
};

//结算，玩家输赢分数
struct tagUserScore
{
	int uid;
	int score;
	tagUserScore()
	{
		uid = 0;
		score = 0;
	}
};

struct MJ_major
{
	MJ_fan_buffer m_oFan;
	MJ_opinfo m_Chi;
	MJ_opinfo m_GetPengGang;
	MJ_opinfo m_GetGangInfo;
	MJ_major()
	{
		m_oFan.Clear();
		m_Chi.Clear();
		m_GetGangInfo.Clear();
		m_GetGangInfo.Clear();
	}
};

//#pragma pack()

class Table
{
public:	
	int						    tid;//roomid
	int             			vid;
	int							zid;	
	int							base_money;
	int                         roomGold;
	int						    stand_money;
	int							lose_exp;
	int							win_exp;
	int						    m_BaseScore;					 //底分 200
	
	ev_tstamp                   preready_timer_stamp;
	
	ev_timer                    send_videodata_timer;
	ev_tstamp                   send_videodata_timer_stamp;

	ev_timer                    req_start_timer;
	ev_tstamp                   req_start_timer_stamp;
protected:
	MJCustomizedConf m_conf;		//麻将通用规则
	int			m_room_owner_uid;          //房主uid
	int			m_room_status;             //房间状态
protected:
	Table_TotalRecordInfo m_totalrecordinfo;
public:
	_uint8		m_GAME_PLAYER;
public://给用户广播掉线通知
	bool		m_Online_Beat[GAME_PLAYER];
	ev_timer	m_Offline_timer;
	ev_tstamp	m_Offline_timer_stamp;
	void Check_Offline_Beat();
	//接收到心跳包后，更新玩家在线状态
	void Online_Beat(Player* player);
public:
	Table();
	virtual ~Table();
	void init_member_variable();
	int init();
	void init(const Json::Value &val, const Json::Value &valCfg, Player *player);	
	bool HasSeat(const Player *player)const;
	_uint8 SeatPlayerCount()const;
	int Sit_Down(Player *player);
	bool Re_Sit_Down(Player* player);
	void Stand_Up(Player *player);
	void Player_Leave(Player *player);
	int del_player(Player *player);
	int handler_login(Player *player);

	//向用户发送登陆成功消息
	int handler_login_succ_uc(Player *player);
	int handler_login_succ_bc(Player *player);
	//向玩家发送其他玩家信息
	int handler_table_info(Player *player);
	//player玩家准备，
	int handler_ready(Player *player);
	//int handler_preready();
	int dz_login_error(const Player* player);
	bool CheckPlayerSkey(const Json::Value& val, const Client* client);

	//玩家自己请求自己的信息
	int handler_info(Player *player);
	void handler_all_players_info(Player *player);
	void handler_daemonize(Player *player);

	//用户退出当前游戏，消息处理
	int handler_logout(Player *player);
	//聊天
	int handler_chat(Client *client);
	//互动表情
	int handler_interaction_emotion(Client *client, Player *player);

	void update_user_info_to_datasvr_dz(tagUserScore userscore[GAME_PLAYER], int tally_type = 0, int cmd_type = SERVER_DZ_UPDATE_USER_INFO,int alter_type = 2);
	//向server请求游戏开始
	int game_start_req();
	//接受server 游戏开始 返回处理
	int game_start_res(Jpacket &packet);
	//用户上桌消息
	int handler_apply_uptable(Player *player);
	void uptable_error_uc(Player *player, int code);
	//用户上桌
	int handler_uptable(Player *player);
	//用户下桌
	int handler_downtable(Player *player);



public:
	//服务端接收客户端的协议
	/**
	 *  @fuction: RecvClientAction
	 *  @brief: 玩家准备、站起
	 *  @return: 
	**/
	//void recv_client_action(int action, Player *player);
	//玩家选择准备、离开、请求换桌 等消息处理
	void recv_client_action(Client *client, Player *player);

	void recv_client_outcard(Client *client, Player *player);
	/**
	 *  @fuction: RecvClientOpeateReuslt
	 *  @brief: 接收玩家操作
	 *  @return: 
	**/
	void recv_client_opeatereuslt(Client *client, Player *player);
	//收到自动胡牌协议
	void RecvClientAutoHu(Client *client, Player *player);
		
	//player登 出 当前房间
	int logout_room_req(Player *player);
	int force_logout_room_req(Player *player);
	
public:
	//ChairID玩家准备
	void User_Ready(_uint8 ChairID, int action);
	//玩家重新连接上来（可能断线重连）
	void Reconnect(Player *player);
	bool is_all_ready();
	//void check_robot_put_in();
   		
protected:
    
    //广播操作
    void BroadcastOperateResult(_uint8 bOpChairID, int iOpType, _uint8 bOpCard);
    
	void clean_table_user();
	

	//定时器回调  超时换牌、超时出牌、超时操作	
	static void UserOutCard_Timer_cb(struct ev_loop *loop, struct ev_timer *w, int revents);	
	static void UserChooseOperate_Timer_cb0(struct ev_loop *loop, struct ev_timer *w, int revents);	
	static void UserChooseOperate_Timer_cb1(struct ev_loop *loop, struct ev_timer *w, int revents);	
	static void UserChooseOperate_Timer_cb2(struct ev_loop *loop, struct ev_timer *w, int revents);	
	static void UserChooseOperate_Timer_cb3(struct ev_loop *loop, struct ev_timer *w, int revents);	    
	static void Disband_table_Timer_cb0(struct ev_loop *loop, struct ev_timer *w, int revents);	
    static void Send_Video_Data_Timer_cb(struct ev_loop *loop, struct ev_timer *w, int revents);
	static void Recv_DisbandTable_Timer_cb(struct ev_loop *loop, struct ev_timer *w, int revents);
	static void Check_Offline_Timer_cb(struct ev_loop *loop, struct ev_timer *w, int revents);
	static void RequestGameStart_Timer_cb(struct ev_loop *loop, struct ev_timer *w, int revents);
public:		
	char* CatHandCard(const HandCards& pHc, _uint8 ChairID);
		
protected:
	void UpdateUserInfoToUser(Player *player);
	//获取具体某张牌值张数
	int  Get_Card_Left_Num(_uint8 ChairID, _uint8 cbCard);
protected:
	//判断是否有抢杠胡
	bool RobGangHu(_uint8 chairID, _uint8 CardData, int iOpType = TYPE_WANGANG);
	//抢杠胡操作优先级
	void RobGangPriority(_uint8 ChairID, _uint8 CardData, int OperateType);
	void RobGangForceHu(_uint8 ChairID, _uint8 CardData);
	

protected:
	//设置操作时钟
	void set_operator_timer(_uint8 ChairID, bool bDelayTimer=false);
	//关闭操作时钟
	void stop_operator_timer(_uint8 ChairID);
	//设置出牌时钟
	void set_out_timer(_uint8 ChairID, bool bDelayTimer=false);
	//广播时钟剩余秒数
	void Broadcast_UpdateTimer(_tint8 ChairID);
	int get_operate_remain_time(int iOpType, bool bReconnnect);
	//玩家在线状态的改变
	void reset_disbandcount();

public:
	std::string  m_videostr;			//录视频时用的字符串
	void save_video_palyer_info();
	void save_game_scene_info();
	void save_video_gamestar();
	void save_video_firsthand_card();
	virtual void save_video_operate(int iOpType, _uint8 chairID, _uint8 paschairID, _uint8 cbCard, int score[GAME_PLAYER]);
	void save_user_get_card(_uint8 chairID, _uint8 cbCard, int nLeftCardNum);
	void save_user_out_card(_uint8 chairID, _uint8 cbCard);
	void save_video_hu(_uint8 chairID, _uint8 paschairID, int score[GAME_PLAYER]);
	void save_video_game_end(const proto::game::AckGameEnd& stGameEnd);
	void save_video_data(int cmd, const std::string& str, int nlen = 0);
	void send_video_to_svr();	
	
	template<typename TYPE>
	void SaveVideoData(int cmd, const TYPE&);
public:
	void User_Offline(Player * player, bool offline = true);
	
	//玩家在线状态的改变
	void UserOnlineState_bc(_uint8 Chairid);
	void RecvUserTrustee(Client *client);
	//用户解散桌子请求
	void RecvDisbandTableApply(Player *player);
	//用户是否同意解散房间的选择
	void RecvDisbandTableChoose(_uint8 ChairID, int choosestype);
	//玩家解散房间选择改变
	void Disband_table_Timer_Change(_uint8 ChairID, const ev_tstamp tstamp);
	//解散成功返回（房间后台服务器返回）
	int Disband_table_resp(Jpacket packet);
	//超时广播给其他用户
	void UserOvertime_State(_uint8 ChairID);
	//解散房间操作剩余秒数
	int Disband_timer_remaintime(_uint8 ChairID);
	//开启解散房间等待等待选择时钟
	//void Start_Disband_timer(_uint8 bApplyChairID);
	//关闭选择解散房间时钟
	void Stop_Disband_timer(_uint8 ChairID);
	void User_Home_Out(Player * player);
	
	//房主解散房间检测
	bool check_roomowner_disband_table(int uid, int roomstatus);	
	//检测是否所有玩家都掉线
	bool check_all_table_user_offline();
	//通知平台桌子没人或所有玩家都掉线
	void send_talbe_no_user();	
	void Check_All_Offline();
public:
	
public:
	//配置数据
	bool m_RandomSeat;	
	_uint16				m_bDelayTime;					 //延迟时间	
	std::map<int, Player*>		m_mapTable_Player;   // 桌子上的玩家,用uid为key	
	Seat				m_SeatPlayer[GAME_PLAYER];
protected:
	Shai_Set m_Start_Shai;
	_uint8						m_bCurPlayers;	//当前坐在该桌子上的玩家数
	GAME_STATUS					m_bGameState;	//游戏状态

	//出牌时钟
	ev_timer                    m_Outcard_Timer;
	ev_tstamp                   m_Outcard_Timer_stamp;
	//操作时钟
	ev_timer                    m_Operate_Timer0;
	ev_tstamp                   m_Operate_Timer_stamp0;	

	ev_timer                    m_Operate_Timer1;
	ev_tstamp                   m_Operate_Timer_stamp1;	

	ev_timer                    m_Operate_Timer2;
	ev_tstamp                   m_Operate_Timer_stamp2;	

	ev_timer                    m_Operate_Timer3;
	ev_tstamp                   m_Operate_Timer_stamp3;	
    	
	ev_timer					m_CurTimer;	//当前开启的定时器

	//等待玩家选择是否解散房间时钟
	ev_timer                    m_Disbandtable_Timer0;
	ev_tstamp                   m_Disbandtable_Timer_stamp;	
	ev_tstamp                   m_Disbandtable_Timer_stamp1;
	
	ev_timer					m_RecvDisbandTable_Timer;
	ev_tstamp					m_RecvDisbandTable_Timer_stamp;
	_uint8						m_Disband_ResUser;
public:
    CGameLogic			m_GameLogic;                     //牌型逻辑
	HandCards			m_HandCard[GAME_PLAYER];					 //玩家手牌
	bool				m_IsOffline[GAME_PLAYER];		 //各个玩家是否掉线情况      平台发送玩家已经掉线，记录一下
	bool				m_bIsTrustee[GAME_PLAYER];		 //玩家托管（自动胡牌）
	bool				m_bAutoHu[GAME_PLAYER];			 //自动胡牌，true为自动胡牌
	bool				m_bGangCalcImme;				//杠分即时结算 标志位

public:
    _uint8              m_bBanker;                       //庄家
	_uint8				m_last_Banker;
	CCardPool			m_CardPool;						//牌库	
	_uint8				m_bGetCardCount[GAME_PLAYER];	 //摸牌次数 包括吃碰

public:
	int					m_bUserOperate[GAME_PLAYER];               //各个玩家的操纵提示
	_uint8              m_bCurChairID;                   //当前出牌或摸牌的椅子ID
	_uint8				m_bOutCardChairID;			     //出牌椅子id
	_uint8				m_bCurOutCard;					 //当前打出的牌
	_uint8				m_bCurCard;						 //当前摸到或打出的牌	

	ShowCards           m_stUserOutCard[GAME_PLAYER];	 //各个玩家打出的牌    
    //多操作变量
	struct
	{
		_uint8              m_bMultiOpUserNum;               //引起有操作提示的人数	
		_uint8              m_bMultiDoneOpNum;               //已经选择操作的玩家数			
		_uint8              m_bMulitHuOpeCount;                //提示胡牌玩家的操作(包括胡操作和过牌)
		_uint8              m_bMultiOpHuNum;               //引起有操作胡牌玩家个数(一炮多响)
		int		            m_bMulitFirstOperate;            //优先级最高的玩家是否执行还是放弃，放弃为false
		int					m_bMultiUserDoneOp[GAME_PLAYER];           //用户选择的操作	
		_uint8              m_bMultiUserDoneOpCard[GAME_PLAYER];           //用户选择的操作的牌		
	};	
	MJ_BUFFER<_uint8, GAME_PLAYER> m_PassHuPlayer;

	struct  
	{		
		int                 m_RobOperateNum;                           //抢杠操作数
		int                 m_RobHuNum;                                //抢杠胡的玩家数		
		int                 m_WangGangPlayer;                          //弯杠玩家
		int					m_WangGangPlayerOpType;					   //弯杠玩家的操作类型
		int					m_RobHuDoneOp[GAME_PLAYER];                //抢杠胡玩家选择的操作		
		bool			    m_bAddFanFlag[GAME_PLAYER];			       //另加番，杠上花，杠上炮		
		_uint8				m_RobWanGangCard;						   //抢杠胡时弯杠的牌
		bool                m_bRobWanGang;                             //抢杠胡标志，true为开启
		bool                m_bRobHu;                                  //胡牌玩家操作标志，true胡牌
		bool				m_bRobGangHuJudge;						//只有判断是不是抢杠胡的时候使用
	};	
	
protected:
	LOWLEVELOPERATE     m_bRecordMultiOp;		         //记录优先等级低的操作
	void DoMultiForceHu(_uint8 ChairID, _uint8 bCard);
	void init_multi_data();
protected:
    //胡牌
	
	struct  
	{
		bool				m_bIsHu[GAME_PLAYER];	    				  //玩家是否已经胡，0表示没胡		
		int					m_HuFanNums[GAME_PLAYER];                     //胡牌番数		
		tagGangScoreInfo	m_stGang[GAME_PLAYER];						  //杠牌信息		
		tagHuInfo 			m_stHuScore[GAME_PLAYER];				      //胡牌输赢分			
		MJ_fan_buffer		m_oFan[GAME_PLAYER];						//胡番信息
	};	
	
protected:
	_uint8				m_ApplyDisbandChairid;						//发起申请解散房间的椅子id
	bool				m_bIsDisbandTable;							//当前是否正在解散桌子
	_uint8				m_DisbandChoose[GAME_PLAYER];				//玩家是否解散房间的选择
	_uint8				m_bDisbandCount;							//一局可以解散房间的次数	
	_uint8				m_bOverTimeState[GAME_PLAYER];				//各个玩家超时的状态，0表示没有超时，1表示出牌超时，2表示操作超时	
	int                 m_nPlayingTimes;                            //
	CGameAccout			m_account_info;								//游戏当局阶段信息

protected:
	ForbidWin m_louhu[GAME_PLAYER];									//漏胡相关的操作
public:
	bool				m_bDeleteTable;								//是否已经解散房间
	int                 m_nAllRound;
protected://
	MJ_SUB_ID m_room_config;		//房间配置信息
protected://鸟马信息
	MJ_BUFFER<_uint8, 12> m_niao;
	MJ_BUFFER<_uint8, 12> m_zhong_niao;
	_uint8 m_DaNiaoID;
protected:
	MJ_peng m_Peng[GAME_PLAYER];	//记录碰，防止直杠变弯杠

protected:
	Fantype_to_FanNum* m_ft2fn;
protected:
	//听牌	
	/**
	*  @fuction: IsListening
	*  @brief: 玩家胡后，如果有杠，则要判断杠后是否还能听牌
	*  ChairID: 玩家椅子ID
	*  bRemoveCard: 删除的手牌
	*  @return: 杠牌后是否还听牌，true为听牌
	**/	
	bool IsListening(_uint8 ChairID, tagListenInfos stListenInfo[14], _uint8 &bListenLen);
	
	//检查手中相关性的牌
	void GetHandCardRelativity(_uint8 bCard, MJ_BUFFER<_uint8, POOL_CARD_TYPE_MAX>& _relativity);

	void SendTingInfo(_uint8 ChairID);
	void SendTingInfo_Simple(_uint8 ChairID);
	void PackageTingInfo(_uint8 ChairID, tagListenInfos stListenInfo[14], _uint8 iPos, proto::game::tagListenInfo* listeninfo);
public:
	void Send_DisbandTableMessage();	
	void Send_GameRound_ScoreInfo(Player*);

public:
	virtual int dispatch_client(int cmd, Client *client, Player *player);
public:		
		
	static Table* CreateTable(const Json::Value& table_val, const Json::Value& val);
	static Table* CreateTable_changsha(const Json::Value& table_val, const Json::Value& val);
	static Table* CreateTable_zhaoqing(const Json::Value& table_val, const Json::Value& val);
	static Table* CreateTable_gd_jihu(const Json::Value& table_val, const Json::Value& val);
	static Table* CreateTable_gy_zhuoji(const Json::Value& table_val, const Json::Value& val);
	static Table* CreateTable_gd_huizhouzhuang(const Json::Value& table_val, const Json::Value& val);
	static Table* CreateTable_JiangSu(const Json::Value& table_val, const Json::Value& val);
	static Table* CreateTable_JiangXi(const Json::Value& table_val, const Json::Value& val);
	/*
	 *	桌子配置初始化
	 *	平台远端发过来的配置
	 */
	virtual void ConfInit_Remote(const Json::Value& val);
	/*
	 *	游戏开始
	 */
	virtual void GameStart();
	/*
	 *	游戏开始 发送手牌
	 */
	virtual void Send_Hand_Card();
	/*
	 *	初始化庄家
	 */
	virtual void InitBanker();
	/*
	 *	服务端游戏逻辑
	 */
	virtual void InitData();
	/*
	 *	游戏结束
	 */
	virtual void GameEnd();
	/*
	 *	房间解散时发送游戏结束
	 */
	virtual void GameBreakOff();
	/*
	 *	游戏结束重置成员变量
	 */
	virtual void GameEndReset();
	/*
	 *	玩家摸牌
	 */
	virtual void OnUserGetCard(_uint8 ChairID, bool bGetLast = false);
	/*
	 *	玩家打牌
	 */
	virtual void OnUserOutCard(_uint8 bOutChairID, _uint8 bOutCard);
	virtual void OnUserOutCard_XianNing(_uint8 bOutChairID, _uint8 bOutCard) { return; };
	/*
	 *	统计信息
	 */
	virtual void CalcRecordInfo(const MJ_user_buffer& UserID, int iTotalScore[GAME_PLAYER]);
	/*
	*	redis 更新当前结算信息到redis数据库
	*/
	virtual void RedisUpdateScoreToServer(int iTotalScore[GAME_PLAYER]);

	/*
	*	redis 更新数据库信息到本地
	*/
	virtual void RedisUpdateScoreToLocal();
	/*
	*	游戏最开始的时候初始化用户
	*/
	virtual void InitPlayAccont();

	/*
	 *	桌面庄家轮庄
	 */
	virtual void TurnBanker();
	/**
	 *  @fuction: CheckUserOutCard
	 *  @brief: 检查玩家出牌
	 *  @return:
	 **/
	virtual bool CheckUserOutCard(_uint8 ChairID, _uint8 cbCard);
	/*
	 *	检查用户操作是否合法	
	 */
	virtual bool CheckUserOperator(_uint8 ChairID, int iOpType, _uint8 bCard);
	/*
	 *	玩家获得第一张牌
	 */
	virtual void UserGetFirstCard(_uint8 ChairID, int& iOpType, MJ_fan_buffer& oFan);
	/*
	 *	庄家打出的第一张牌
	 */
	virtual void UserFirstOutCard(_uint8 ChairID, int& iOpType, MJ_fan_buffer& oFan);
	/*
	 *	玩家获得最后一张牌
	 */
	virtual void UserGetLastCard(_uint8 ChairID, int& iOpType, MJ_fan_buffer& oFan);
	/*
	 *	玩家打出最后一张牌，其它玩家判断
	 */
	virtual void UserLastOutCard(_uint8 ChairID, int& iOpType, MJ_fan_buffer& oFan);
	/*
	 *	玩家杠上花
	 */
	virtual void UserGangShangHua(_uint8 ChairID, _uint8 bCard, MJ_fan_buffer& oFan);
	/*
	 *	玩家杠后炮
	 */
	virtual void UserGangHouPao(_uint8 ChairID, _uint8 bCard, MJ_fan_buffer& oFan);
	/*
	 *	玩家抢杠胡
	 */
	virtual void UserQiangGangHu(_uint8 ChairID, _uint8 bCard, MJ_fan_buffer& oFan);
	/*
	 *	玩家出牌后判断其他玩家是否能够吃碰胡
	 */
	virtual int OutCardEstimate(_uint8 bOutChairID, _uint8 bCard, MJ_major& major, int& fan_num);
	/*
	*	玩家摸牌后判断是否能够杠听胡
	*/
	virtual int GetCardEstimate(_uint8 ChairID, _uint8 bGetCard, MJ_major& major, int& fan_num);
	/*
	 *	检查牌有没有胡
	 */
	virtual int Check_Hu(_uint8 ChairID, HandCards& pHc, MJ_fan_buffer& oFan);
	/*
	 *	判断牌是不是7对
	 */
	virtual bool Is7Dui(_uint8 ChairID, HandCards& pHc, MJ_fan_buffer& oFan);
	/*
	 *	判断牌是不是13幺
	 */
	virtual bool Is13Yao(HandCards& pHc, MJ_fan_buffer& oFan, MJ_win_pattern& pattern);
	/*
	 *	判断全求人
	 */
	virtual bool IsQuanqiuren(const HandCards& pHc, MJ_fan_buffer& oFan, int& iOpType);
	/*
	 *	判断4坎1将牌型
	 */
	virtual bool FormatCards(HandCards *hcs, MJ_win_pattern& pattern);
	/*
	 *	检查牌有没有胡
	 */
	virtual int Check_Hu_Pattern(_uint8 ChairID, const HandCards& pHc, MJ_fan_buffer& oFan, const MJ_win_pattern& oPattern);
	/*
	*	胡牌类型是几番，返回胡牌类型的总番数
	*/
	virtual int Calc_Fan(_uint8 ChairID, MJ_fan_buffer& oFan);		
	/*
	 *	计算玩家胡牌操作的番
	 */
	virtual int Calc_Hu(_uint8 ChairID, _uint8 bCard, int Score[GAME_PLAYER]);
	/*
	 *	玩家执行吃操作	
	 */
	virtual void UserOperateChi(_uint8 ChairID, int iOpType, _uint8 bCard);
	/*
	 *	碰操作
	 */
	virtual void UserOperatePeng(_uint8 ChairID, _uint8 bCard);
	/*
	 *	胡操作
	 */	
	virtual void UserOperateHu(_uint8 ChairID, _uint8 bHuCard = 0xFF);
	/*
	 *	过操作
	 */
	virtual void UserOperatePass(_uint8 ChairID, int iOpType, _uint8 bCard);
	/*
	 *	玩家执行杠操作
	 */
	virtual void UserOperateGang(_uint8 ChairID, int iOpType, _uint8 bCard);
	/*
	 *	玩家吃
	 */
	virtual int GetChi(_uint8 ChairID, _uint8 bCard, MJ_opinfo& ChiInfo);
	/*
	 *	玩家弯杠、暗杠信息
	 */
	virtual int GetGangInfo(_uint8 ChairID, MJ_opinfo& GangCardInfo);
	/*
	 *	玩家直杠、碰信息
	 */
	virtual int GetPengGang(_uint8 ChairID, _uint8 cbCard, MJ_opinfo& GangCardInfo);
	/*
	 *	玩家执行吃碰杠操作以后
	 */
	virtual void UserAfterOperate(_uint8 ChairID, int iOpType, _uint8 bCard);
	/*
	 *	多个玩家有提示
	 */
	virtual void DoMultiOperate(_uint8 ChairID, int iOpType, _uint8 bCard);
	/*
	 *	只有一个玩家有提示
	 */
	virtual void DoSimpleOperate(_uint8 ChairID, int iOpType, _uint8 bCard);
	/*
	 *	多操作结束
	 */
	virtual void MultiOperateOver(_uint8 chairID, int iOpType, _uint8 bCard);
	
	virtual int GetListeningInfo(_uint8 ChairID, tagListenInfos stListenInfo[14], _uint8& bListenLen);

	//检查听牌
	virtual void Check_Listening(HandCards *pHc, _uint8 ChairID, _uint8 bCard, tagListenInfos stListenInfo[14], _uint8 &bListenLen);

	//获取所有相关性的牌  ,有癞子，直接重载此函数,从牌库里面，将癞子提出即可。
	virtual void GetAllHandCardRelativity(HandCards *pHc, MJ_BUFFER<_uint8, POOL_CARD_TYPE_MAX>& _relativity);

	virtual bool IsListening(_uint8 ChairID, _uint8 bRemoveCard);

	virtual void SetRobGangHu_Flag(_uint8 ChairID, int iOpType, proto::game::AckBcOpResult& stOpresult);

	virtual bool CardPoolIsEmpty();

	virtual int Check_DaJiao(_uint8 ChairID);
	/*
	 *	旁观玩家发送场景信息
	 */	
	void Send_lookon_GameScene(Player *player);	
	/*
	 *	发送正在进行的游戏场景
	 */
	void Send_Game_Scene(Player *player);

	/**
	 *  @fuction: EstimateUserRespond
	 *  @brief: 一玩家打出牌后其他玩家的操作提示判断
	 *  @return:
	 **/
	virtual bool EstimateUserRespond(_uint8 bOutChairID, _uint8 bOutCard);

	//接收玩家的操作
	virtual void OnRecvUserOperate(_uint8 ChairID, int iOpType, int bCard);
public:
	/*
	 *	游戏场景proto文件组装
	 */
	//游戏场景全部发送过去（可能从其他程序切换过来）
	virtual void Proto_GameScene(_uint8 ChairID, Player* player, proto::game::AckGameScene& stGameScene);
	//将用户可执行的操作（碰杠胡听牌信息）添加到stGameScene中
	virtual void Proto_GameScene_Operate(_uint8 ChairID, Player* player, proto::game::AckGameScene& stGameScene);
	//旁观者场景发送
	virtual void Proto_GameScene_lookon(_uint8 ChairID, Player* player, proto::game::AckGameScene& stGameScene);
	virtual void Proto_GameEndInfo(const MJ_user_buffer& UserID, proto::game::AckGameEnd& stGameend);	
	virtual void Proto_GameEndInfo_GangInfo(_uint8 ChairID, proto::game::AckGameEnd& stGameend);
	virtual void Proto_Game_OpNotify(_uint8 ChairID, _uint8 CurChairID, _uint8 bCard, proto::game::AckOpNotify& stNotify);
	virtual void Proto_Game_OpNotifyAfterPengChi(_uint8 ChairID, _uint8 CurChairID, const MJ_opinfo& gang, proto::game::AckOpNotify& stNotify);
	virtual void Proto_Game_UserGetCard(_uint8 ChairID, _uint8 CurChairID, _uint8 bCard, proto::game::AckUserGetCard& stUsergetcard);
	virtual void Proto_Game_UserOutCard(_uint8 ChairID, _uint8 bCard, proto::game::AckUserOutCard& stUserOutCard);
	virtual void Proto_Game_OperateGang(_uint8 ChairID, _uint8 bCard, int iOpType, int nGangScore[GAME_PLAYER], proto::game::AckBcOpResult& stOpresult);
		
public:
	//胡牌结分
	//一局有人胡牌
	virtual void Score_GameEndSucc(HuScoreSet& score, proto::game::AckGameEnd& stGameend);
	//流局
	virtual void Score_GameEndDraw(HuScoreSet& score, proto::game::AckGameEnd& stGameend);
	virtual void Calc_BaseScore(_uint8 ChairID, int iBaseScore[GAME_PLAYER]);
	virtual void Calc_HuScore(_uint8 ChairID, int iBaseScore[GAME_PLAYER], int iScore[GAME_PLAYER]);
	virtual void Calc_ZhongNiao(_uint8 ChairID, _uint8 ShaiNum, int iBaseScore[GAME_PLAYER], int iScore[GAME_PLAYER]);
	virtual void Calc_FengDing(_uint8 ChairID, int iScore[GAME_PLAYER]);
	virtual void Send_GameEnd_ScoreInfo(Player*, proto::game::hunanmj::GameEndRecord &score);
public:	
	int MultiHu(MJ_user_buffer& id)const;	
	void Fill_HandCard(_uint8 ChairID, proto::game::HandCards* pHandcard);
	void Fill_FixedCard(_uint8 ChairID, _uint8 Pos, proto::game::FixedCard* pstFix);			
	void Update_Score(int iScore[GAME_PLAYER]);	
	bool Is_GenZhuangMa4X(_uint8 ChairID, _uint8 niao);
	bool Is_GenZhuangByPlayerNum(_uint8 ChairID, _uint8 niao);
	bool Is_HuPaiMa4X(_uint8 HuId, _uint8 ChairID, _uint8 niao);
	void SendPlayerFirstCardOp(_uint8 ChairID);
	void GetFirstHandCard_NoOp(_uint8 ChairID);	
public:
	virtual _uint8 BankerScoreAdd(_uint8 ChairID)
	{//庄家加一番
		return (ChairID == m_bBanker && m_conf.m_bZhuangXian) ? 1 : 0;
	}
	int GetPlayerUid(int ChairID)
	{
		return GetPlayerUid((_uint8)ChairID);
	}
	int GetPlayerUid(_uint8 ChairID)
	{		
		if ((ChairID < m_GAME_PLAYER) && (m_SeatPlayer[ChairID].player != NULL))
		{
			return m_SeatPlayer[ChairID].player->uid;
		}				
		return 0;
	}
	int GetPlayerUid(const Player* player)
	{
		if (player != NULL)
		{
			return player->uid;
		}
		return 0;
	}
	void Calc_Score(int base[GAME_PLAYER], int score[GAME_PLAYER])
	{
		for (int i = 0; i < m_GAME_PLAYER; i++)
		{
			base[i] += score[i];
		}
	}
	void Calc_Score(HuScoreSet& base, int score[GAME_PLAYER])
	{
		for (int i = 0; i < m_GAME_PLAYER; i++)
		{
			base[i].iTotleScore += score[i];
		}
	}
	template<typename TYPE>
	int unicast(int iMsg, int ChairID, const TYPE& p);
	template<typename TYPE>
	int unicast(int iMsg, Player* player, const TYPE& p);
	template<typename TYPE>
	int broadcast(int iMsg, const TYPE& p);
	template<typename TYPE>
	void Proto_GameEndInfo_AddFan(const MJ_user_buffer& UserID, proto::game::AckGameEnd& stGameend, proto::game::hunanmj::tagHuNanGameEnd* end, const TYPE& ft2fn);
	template<typename TYPE>
	const TYPE NextChair(const TYPE& ChairID);
	template<typename TYPE>
	const TYPE NextChair(const TYPE& BankerID, const TYPE& dis);	
	template<typename TYPE>
	const TYPE PreChair(const TYPE& ChairID);
	template<typename TYPE>
	const TYPE DisChair(const TYPE& BankerID, const TYPE& ChairID);
	template<typename TYPE>
	const TYPE DisChair(const TYPE& ChairID);
	template<typename TYPE>
	void Calc_Score(TYPE base[GAME_PLAYER], int score[GAME_PLAYER]);
};

template<typename TYPE>
void Table::Proto_GameEndInfo_AddFan(const MJ_user_buffer& UserID, proto::game::AckGameEnd& stGameend, proto::game::hunanmj::tagHuNanGameEnd* end, const TYPE& ft2fn)
{
	if (end == NULL)	return;
	
	proto::game::hunanmj::FanInfo* info[GAME_PLAYER];
	for (_uint8 i = 0; i < m_GAME_PLAYER; i++)
	{
		info[i] = end->add_faninfo();
	}	

	proto::game::hunanmj::AddFanInfo* pfan = NULL;	
	if (UserID.Size() == 1)
	{//一个人胡
		_uint8 win_seat = UserID[0];
		//assert(m_stHuScore[win_seat].stHu.Size() != 0);
		if (m_stHuScore[win_seat].stHu[0].bType == HU_ZIMO)
		{//自摸胡
			info[win_seat]->set_chairid(win_seat);
			info[win_seat]->set_specialtype(HU_ZIMO);
			info[win_seat]->set_fannum(m_HuFanNums[win_seat]);

			for (int i = 0; i < m_oFan[win_seat].Size(); i++)
			{
				pfan = info[win_seat]->add_addfan();
				pfan->set_addtype(m_oFan[win_seat][i]);
				pfan->set_addnum(ft2fn[m_oFan[win_seat][i]]);
			}
			for (_uint8 c = NextChair(win_seat); c != win_seat; c = NextChair(c))
			{
				info[c]->set_chairid(c);
				info[c]->set_specialtype(HU_BEIZIMO);
				info[c]->set_fannum(0);
				//info[c]->set_fantype(0);				
			}
		}
		else if (m_stHuScore[win_seat].stHu[0].bType == HU_PAOHU)
		{//点炮
			info[win_seat]->set_chairid(win_seat);
			info[win_seat]->set_specialtype(HU_PAOHU);
			info[win_seat]->set_fannum(m_HuFanNums[win_seat]);
			//info[win_seat]->set_fantype(m_HuFanType[win_seat]);

			for (int i = 0; i < m_oFan[win_seat].Size(); i++)
			{
				pfan = info[win_seat]->add_addfan();
				pfan->set_addtype(m_oFan[win_seat][i]);
				pfan->set_addnum(ft2fn[m_oFan[win_seat][i]]);
			}

			for (_uint8 c = NextChair(win_seat); c != win_seat; c = NextChair(c))
			{
				info[c]->set_chairid(c);
				info[c]->set_specialtype(HU_INVALID);
				info[c]->set_fannum(0);
				//info[c]->set_fantype(0);

			}
			info[m_stHuScore[win_seat].stHu[0].bPasHuChairID]->set_specialtype(HU_DIANPAO);
		}
	}
	else if (UserID.Size() > 1)
	{//一炮多响
		for (_uint8 i = 0; i < m_GAME_PLAYER; i++)
		{
			if (UserID.Find(i))
			{

				info[i]->set_chairid(i);
				info[i]->set_specialtype(HU_PAOHU);
				info[i]->set_fannum(m_HuFanNums[i]);

				for (int j = 0; j < m_oFan[i].Size(); j++)
				{
					pfan = info[i]->add_addfan();
					pfan->set_addtype(m_oFan[i][j]);
					pfan->set_addnum(ft2fn[m_oFan[i][j]]);
				}
			}
			else
			{
				info[i]->set_chairid(i);
				info[i]->set_specialtype(HU_INVALID);
				info[i]->set_fannum(0);
				//info[i]->set_fantype(0);
			}

		}
		_uint8 dianpaoID = m_stHuScore[UserID[0]].stHu[0].bPasHuChairID;
		info[dianpaoID]->set_specialtype(HU_DIANPAO);
	}
	else
	{
		for (int i = 0; i < m_GAME_PLAYER; i++)
		{
			info[i]->set_chairid(i);
			info[i]->set_specialtype(HU_INVALID);
			info[i]->set_fannum(0);
		}
	}
}
template<typename TYPE>
int Table::unicast(int iMsg, int ChairID, const TYPE& p)
{
	char buf[0x1000] = { 0 };
	int bodysize = p.ByteSize();
	int headsize = PackHeader(buf, iMsg, bodysize);	
//	assert((headsize + bodysize) < static_cast<int>(sizeof(buf)));
	p.SerializeToArray(buf + headsize, bodysize);
	Player* player = m_SeatPlayer[ChairID].player;
	if (player != NULL && player->client != NULL)
	{
		player->client->send(buf, headsize + bodysize);		
		return 0;
	}
	return -1;
}
template<typename TYPE>
int Table::unicast(int iMsg, Player* player, const TYPE& p)
{
	char buf[0x1000] = { 0 };
	int bodysize = p.ByteSize();
	int headsize = PackHeader(buf, iMsg, bodysize);	
	//assert((headsize + bodysize) < static_cast<int>(sizeof(buf)));
	p.SerializeToArray(buf + headsize, bodysize);
	if ((player != NULL) && (player->client != NULL))
	{
		player->client->send(buf, headsize + bodysize);			
		return 0;
	}
	return -1;
	
}
template<typename TYPE>
int Table::broadcast(int iMsg, const TYPE& p)
{
	char buf[0x1000] = { 0 };
	int bodysize = p.ByteSize();
	int headsize = PackHeader(buf, iMsg, bodysize);
//	assert((headsize + bodysize) < static_cast<int>(sizeof(buf)));
	p.SerializeToArray(buf + headsize, bodysize);	
	//for (std::map<int, Player *>::iterator iter = m_mapTable_Player.begin(); iter != m_mapTable_Player.end(); iter++)	
	for (_uint8 i = 0; i < m_GAME_PLAYER; i++)
	{
		Player *player = m_SeatPlayer[i].player;
		if ((player != NULL) && (player->client != NULL))
		{
			player->client->send(buf, headsize + bodysize);			
		}		
	}
	return 0;
}
template<typename TYPE>
void Table::SaveVideoData(int cmd, const TYPE& p)
{
	char b[0x400] = "";
	sprintf(b, "%d:S:", cmd);

	m_videostr += b;
	m_videostr += p.SerializeAsString();
	m_videostr += ":E:";
}

template<typename TYPE>
const TYPE Table::NextChair(const TYPE& ChairID)
{
	return (ChairID + 1) % m_GAME_PLAYER;
}

template<typename TYPE>
const TYPE Table::NextChair(const TYPE& ChairID, const TYPE& dis)
{
	return (ChairID + dis) % m_GAME_PLAYER;
}
template<typename TYPE>
const TYPE Table::PreChair(const TYPE& ChairID)
{
	return (ChairID + m_GAME_PLAYER - 1) % m_GAME_PLAYER;
}

template<typename TYPE>
const TYPE Table::DisChair(const TYPE& BankerID, const TYPE& ChairID)
{
	const TYPE x = m_GAME_PLAYER * 10;
	return (x + ChairID - BankerID) % m_GAME_PLAYER;
}
template<typename TYPE>
const TYPE Table::DisChair(const TYPE& ChairID)
{
	const TYPE x = m_GAME_PLAYER * 10;
	return (x + ChairID - m_bBanker) % m_GAME_PLAYER;
}
template<typename TYPE>
void Table::Calc_Score(TYPE base[GAME_PLAYER], int score[GAME_PLAYER])
{
	for (int i = 0; i < m_GAME_PLAYER; i++)
	{
		base[i].iTotalScore += score[i];
	}
}
#endif
