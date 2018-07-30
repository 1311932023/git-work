#ifndef _TABLE_H_
#define _TABLE_H_

#include <set>
#include <json/json.h>
#include <ev++.h>
#include <sstream> 
#include <iostream>

#include "log.h"
#include "base64.h"
#include "zlib.h"
#include "proto.h"
#include "game.h"
#include "../phz.h"
#include "../libphz/common.h"
#include "jpacket.h"
#include "ppacket.h"
#include "loginproto.pb.h"
#include "gameproto.pb.h"
#include "../libphz/game_logic.h"
#include "../table_cardpoor.h"
#include "../libphz/mjcardtype.h"


#define MaxVideoBufLen 18000
#define RANDOM(start, end, seed) (start + rand() % (end - start + 1))

extern PHZ phz;

//�淨����
typedef MJ_BUFFER<int, 30> PhzPlayID; 
//����淨�����Ƿ����
bool CheckPlayID(const Json::Value &val);
//��ȡ����id ��������
int GetPlaySubID(const Json::Value &val, std::string playID, PhzPlayID& SubID);
//��ѹ
int decompress_str(string &src, string &des);
//ѹ��
int compress_str(string &src, string &des);

//����ͳ����Ϣ
struct BaseSettleInfo
{
	_uint8 			m_HuCnt;		//���ƴ���
	_uint8 			m_DianPaoCnt;	//���ڴ���
	_uint8 			m_ZimoCnt; 		//��������
	_uint8 			m_WinCnt; 		//Ӯ����
	_uint8 			m_LoseCnt; 		//�����
	_tint32 		m_wBestScore;	//��ߵ÷�
	_tint32 		m_wTotalScore;	//�ܵ÷�
	_tint32 		m_wAllPiaoScore;//��Ʈ�÷�
	BaseSettleInfo()
	{
		m_HuCnt =0;m_DianPaoCnt=0;m_ZimoCnt=0;
		m_WinCnt =0;m_LoseCnt=0;
		m_wBestScore=0;m_wTotalScore=0;
		m_wAllPiaoScore=0;
	}
};
//ÿһ�ֵ÷ּ�¼
struct tagGain
{
	_tint32 Gain[GAME_PLAYER];
	tagGain()
	{
		memset(Gain, 0, sizeof(Gain));
	}
};
//----------------------------���ýṹ��--------------------------------
//��¼����������ȼ��͵Ĳ���
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


//���Ƶĺ�����Ϣ
struct tagHuInfos
{
	_uint8          bCard;              //������
	_uint8          bLeftCount;         //����ʣ����
	_uint8          bFanShu;            //���Ƶķ���
};

//������Ϣ
struct tagListenInfos
{
	_uint8          bOutCard;           //��Ҫ�������
	tagHuInfos      HuInfo[18];         //����ƺ���Ժ��Ƶ���Ϣ
	_uint8			bHuLen;
	tagListenInfos()
	{
		bOutCard = 0;
		memset(HuInfo, 0, sizeof(HuInfo));
		bHuLen = 0;
	}
};

//���Ʒ�������˭����
class tagGangScore
{
public:	
	_uint8 Card;	
	int ChairID;		//����Ӯ˭��Ǯ
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

//������Ϣ 
class tagGangScoreInfo 
{
public:
	int iTotalScore;				  //������Ӯ�ܷ�	
	MJ_BUFFER<tagGangScore, 20> Gang; //������Ϣ
	tagGangScoreInfo()
	{
		iTotalScore = 0;	
	}
};

//��¼������Ϣ
class tagHuRecord
{	
public:
	_uint8	bHuCard;		//������
	_uint8	bPasHuChairID;	//��˭����	
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
	_tint64	iTotalScore;	//������Ӯ�ܷ�	
	MJ_BUFFER<tagHuRecord, 6> stHu;
	tagHuInfo()
	{
		iTotalScore = 0;		
	}
	
};

struct tagAddFanInfo     //��ӷ�
{
	int nType;       //����ӷ�������
	int nFan;        //�ӵķ���
	tagAddFanInfo()
	{
		nType = 0;
		nFan = 0;
	}
};

//���㣬�����Ӯ����
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

struct RoundScore
{
	int score[GAME_PLAYER];	
	RoundScore(){}
	RoundScore(int a, int b, int c, int d)
	{
		score[0] = a;		score[1] = b;		score[2] = c;		score[3] = d;
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
	void Clear()
	{
        m_oFan.Clear();
		m_Chi.Clear();
		m_GetGangInfo.Clear();
		m_GetGangInfo.Clear();
	}
};

//��Ϸ״̬����
typedef enum
{
	GAME_FREE = 0,
	GAME_CHANGECARD = 1,		//ѡ����	
	GAME_CHOOSE_LACK = 2,		//ѡ��ȱɫ
	GAME_PLAYING = 3,
	GAME_PIAO = 4,            	//ѡƮ����
	GAME_END = 5,				//����
} State;

class Table;
class Seat
{
public:
	_tint32 							m_Uid;
	_uint8 								m_SeatId;
	bool 								m_bReady;
	bool 								m_bOccupied;
	Player 					    		*m_Player;
	Table 								*m_Table; 					//��λ��������
	HandCards 							m_HandCard;					//����
	ShowCards 							m_bOutRecord;				//�������
	bool 								m_bResponse;				//�Ƿ�������������Ӧ
	int									m_bUserOperate;				//��Ҳ�����ʾ	
	_uint8 								m_OutCardCnt;				//���ƴ���
	int 								m_UserPiaoScore; 			//����Ʈ�ķ�
	BaseSettleInfo 						m_SettleInfo;  				//ͳ������
	//mj
	_uint8								m_bGetCardCount;			//���ƴ��� ��������
	int									m_bMultiUserDoneOp; 		//�û�ѡ��Ĳ���
	_uint8              				m_bMultiUserDoneOpCard;     //�û�ѡ��Ĳ�������
	Seat();
	virtual ~Seat();
	void Reset();
	void Clear();
	void ResetAction();
};

#pragma pack()

class Table
{
public:
//-------------------------------------������Ϣ--------------------------------
	std::string					tid;								//����Ψһ��ʶ
	int							base_money;							//��ʼ����
	std::map<int, Player*>		players;							//���Ϻ��������
	int							stand_money;						//�����������������
	int							m_TakeIn;							//�������
	_uint8						m_RoomStatus;						//����״̬
	_tint32						m_RoomUserId;						//����uid
	int 						m_PlayId;							//��Ϸid
	proto::game::AckTableInfo 	m_pTableInfo;						//�������������			

//-------------------------------------��ɢ��Ϣ--------------------------------
	std::map<int, Player*>		voters_players;						//��ɢ����ͶƱ��
	int							dissolve_applyer;					//��ɢ����������
	int							dissovle_state;						//�Ƿ��ڽ�ɢ���� 0--no 1--yes
	int							is_dissolved;						//�˷����ѱ���ɢ

//-------------------------------------������Ϣ--------------------------------
	State 						m_State;							//��Ϸ״̬
	_uint8 						m_GamePlayer;						//��������
	Shai_Set 					m_Start_Shai;						//ɫ��
	Seat						m_Seats[GAME_PLAYER];				//��λ
	bool 						m_bAlreadyResp;						//��Ϸ��ʼ���ر�־
	bool						m_bDispatch;						//�Ƿ����ƶշ�������
	bool 						m_bOutCard;							//�Ƿ�ó���
	bool 						m_bCanHu;							//�Ƿ���Ժ�
	bool 						m_bFirstCard; 						//�Ƿ�������
	bool 						m_bCardOperated;					//��ǰ���Ƿ񱻲���
	_uint8						m_bOverTimeState[GAME_PLAYER];		//������ҳ�ʱ��״̬��0��ʾû�г�ʱ��1��ʾ���Ƴ�ʱ��2��ʾ������ʱ	
	_uint8 						m_BankerSeat;						//ׯ����λ��
	_uint8 						m_BankerCard;						//ׯ��������
	_uint8 						m_CurSeat;							//��ǰ��λ��
	_uint8 						m_CurOutSeat;						//��ǰ������λ
	_uint8 						m_CurOutCard;						//��ǰ������
	_uint8 						m_CurCard;							//��ǰ������
	std::string					m_strInnings;						//�������Ψһ��ʶ
	std::string					m_strVideo;							//¼������
	_uint8 						m_CurTurn;							//��ǰ����
	_uint8 						m_TotalTurn;						//�ܾ���
	std::vector<tagGain>  		m_vecGains; 						//ÿ�ֵ÷�
	int							base_score;							//�׷�
	_uint8 						m_cbCurTimer;						//��ǰ��ʱ�����
	CGameLogic					m_GameLogic;                     	//�����߼�
	bool						m_bGangCalcImme;					//�ּܷ�ʱ���� ��־λ
	_uint8						m_last_Banker;						//��һ��ׯ��
	CCardPool					m_RepertoryCard;					//�ƿ�
	_uint8 						m_MaxCount;							//�׷�ÿ�˶�����
	//�����ʱ���ϵĲ���
	_uint8              		m_bMultiOpUserNum;             		//�����в�����ʾ������	
	_uint8              		m_bMultiDoneOpNum;               	//�Ѿ�ѡ������������			
	_uint8              		m_bMulitHuOpeCount;                 //��ʾ������ҵĲ���(�����������͹���)
	_uint8              		m_bMultiOpHuNum;                    //�����в���������Ҹ���
	int		            		m_bMulitFirstOperate;               //���ȼ���ߵ�����Ƿ�ִ�л��Ƿ���������Ϊfalse
	MJ_BUFFER<_uint8,8> 		m_GuiVector;						//������
	_uint8 						m_LaiziCard;						//������
	MJ_BUFFER<_uint8, GAME_PLAYER> m_PassHuPlayer;					//ѡ����������
	//����
	struct  
	{		
		int                 m_RobOperateNum;                           //���ܲ�����
		int                 m_RobHuNum;                                //���ܺ��������		
		int                 m_WangGangPlayer;                          //��������λ��
		int					m_WangGangPlayerOpType;					   //
		int					m_RobHuDoneOp[GAME_PLAYER];                //���ܺ����ѡ��Ĳ���		
		bool			    m_bAddFanFlag[GAME_PLAYER];			       //��ӷ������ϻ���������		
		_uint8				m_RobWanGangCard;						   //���ܺ�ʱ��ܵ���
		bool                m_bRobWanGang;                             //���ܺ���־��trueΪ����
		bool                m_bRobHu;                                  //������Ҳ�����־��true����
		bool				m_bRobGangHuJudge;						   //ֻ���ж��ǲ������ܺ���ʱ��ʹ��
	};
	//����
	struct  
	{
		bool				m_bIsHu[GAME_PLAYER];	    				  //����Ƿ��Ѿ�����0��ʾû��		
		int					m_HuFanNums[GAME_PLAYER];                     //���Ʒ���		
		tagGangScoreInfo	m_stGang[GAME_PLAYER];						  //������Ϣ		
		tagHuInfo 			m_stHuScore[GAME_PLAYER];				      //������Ӯ��			
		MJ_fan_buffer		m_oFan[GAME_PLAYER];						  //������Ϣ
	};


	LOWLEVELOPERATE     m_bRecordMultiOp;		         				//��¼���ȵȼ��͵Ĳ���
	MJ_major            m_OperateRecord[GAME_PLAYER];                   //��Ҳ�������¼
	MJ_SUB_ID m_room_config;											//����������Ϣ
	MJCustomizedConf m_conf;											//�齫ͨ�ù���
	Fantype_to_FanNum* m_ft2fn;											//������Ӧ
//-------------------------------------��ʱ��-----------------------------------
	ev_timer                    start_timer;
	ev_tstamp                   start_timer_stamp;	

	ev_timer                    preready_timer;
	ev_tstamp                   preready_timer_stamp;	

	ev_timer                    dispatch_show_timer;								
	ev_tstamp                   dispatch_show_timer_stamp;

	ev_timer                    dispatch_delay_timer;								
	ev_tstamp                   dispatch_delay_timer_stamp;

	ev_timer					dissolve_room_timer;
	ev_tstamp					dissolve_room_tstamp;

	ev_timer                    send_videodata_timer;
	ev_tstamp                   send_videodata_timer_stamp; 

	ev_timer                    game_restart_timer;
	ev_tstamp                   game_restart_timer_stamp;

	//����ʱ��
	ev_timer                    m_Outcard_Timer;
	ev_tstamp                   m_Outcard_Timer_stamp;
	//����ʱ��
	ev_timer                    m_Operate_Timer;
	ev_tstamp                   m_Operate_Timer_stamp;

	ev_timer                    m_Operate_Timer0;
	ev_tstamp                   m_Operate_Timer_stamp0;	

	ev_timer                    m_Operate_Timer1;
	ev_tstamp                   m_Operate_Timer_stamp1;	

	ev_timer                    m_Operate_Timer2;
	ev_tstamp                   m_Operate_Timer_stamp2;	

	ev_timer                    m_Operate_Timer3;
	ev_tstamp                   m_Operate_Timer_stamp3;

	//�й��ӳ�ʱ��
	ev_timer                    m_Trusteeshit_Out_Delay_Timer;
	ev_tstamp                   m_Trusteeshit_Out_Delay_Timer_stamp;

	ev_timer                    m_Trusteeshit_Ope_Delay_Timer0;
	ev_tstamp                   m_Trusteeshit_Ope_Delay_Timer_stamp0;

	ev_timer                    m_Trusteeshit_Ope_Delay_Timer1;
	ev_tstamp                   m_Trusteeshit_Ope_Delay_Timer_stamp1;

	ev_timer                    m_Trusteeshit_Ope_Delay_Timer2;
	ev_tstamp                   m_Trusteeshit_Ope_Delay_Timer_stamp2;

	ev_timer                    m_Trusteeshit_Ope_Delay_Timer3;
	ev_tstamp                   m_Trusteeshit_Ope_Delay_Timer_stamp3; 

	//���ֵ�һ�����ƶ�ʱ��
	ev_timer                    first_getcard_timer;
	ev_tstamp                   first_getcard_timer_stamp;	
    	
	ev_timer					m_CurTimer;	//��ǰ����Ķ�ʱ��

//-------------------------------------��ʱ���ص�����--------------------------------
	static void start_timer_cb(struct ev_loop *loop, struct ev_timer *w, int revents);
	static void first_getcard_timer_cb(struct ev_loop *loop, struct ev_timer *w, int revents);	
	static void preready_timer_cb(struct ev_loop *loop, struct ev_timer *w, int revents);
	static void dispatch_show_timer_cb(struct ev_loop *loop, struct ev_timer *w, int revents);
	static void dispatch_delay_timer_cb(struct ev_loop *loop, struct ev_timer *w, int revents);
	static void dissolve_room_timer_cb(struct ev_loop *loop, struct ev_timer *w, int revents);
	static void Send_Video_Data_Timer_cb(struct ev_loop *loop, struct ev_timer *w, int revents);
	static void game_restart_timer_cb(struct ev_loop *loop, struct ev_timer *w, int revents);
	static void UserOutCard_Timer_cb(struct ev_loop *loop, struct ev_timer *w, int revents);
	static void UserChooseOperate_Timer_cb(struct ev_loop *loop, struct ev_timer *w, int revents);
	static void UserChooseOperate_Timer_cb0(struct ev_loop *loop, struct ev_timer *w, int revents);	
	static void UserChooseOperate_Timer_cb1(struct ev_loop *loop, struct ev_timer *w, int revents);	
	static void UserChooseOperate_Timer_cb2(struct ev_loop *loop, struct ev_timer *w, int revents);	
	static void UserChooseOperate_Timer_cb3(struct ev_loop *loop, struct ev_timer *w, int revents);	
	static void Trusteeshit_Out_Timer_cb(struct ev_loop *loop, struct ev_timer *w, int revents);	    
	static void Trusteeshit_Ope_Timer_cb0(struct ev_loop *loop, struct ev_timer *w, int revents);	   
	static void Trusteeshit_Ope_Timer_cb1(struct ev_loop *loop, struct ev_timer *w, int revents);	
	static void Trusteeshit_Ope_Timer_cb2(struct ev_loop *loop, struct ev_timer *w, int revents);	
	static void Trusteeshit_Ope_Timer_cb3(struct ev_loop *loop, struct ev_timer *w, int revents);	


//-------------------------------------���ò�����ʱ������---------------------------------
	//���ò���ʱ��
	void set_operator_timer(_uint8 ChairID, bool bDelayTimer=false);
	//�رղ���ʱ��
	void stop_operator_timer(_uint8 ChairID);
	//���ó���ʱ��
	void set_out_timer(_uint8 ChairID, bool bDelayTimer=false);
	//��ȡ����ʱʣ��ʱ��
	int get_operate_remain_time(int iOpType, bool bReconnnect);
	//�����йܲ���ʱ��
	void set_trusteeship_operator_timer(_uint8 ChairID);
	//�����йܳ���ʱ��
	void set_trusteeship_out_timer(_uint8 ChairID);


//-------------------------------------��������--------------------------------------
public:
	Table();
	virtual ~Table();
	//ʵ��������ӿ�
	static Table* CreateLocationTable(std::string location);
	int broadcast(Player *p, const std::string &packet);
	int unicast(Player *p, const std::string &packet);
	int sit_down(Player *player);
	void stand_up(Player *player);
	int del_player(Player *player);
	int handler_login(Player *player);
	int handler_login_succ_uc(Player *player);
	int handler_login_succ_bc(Player *player);
	int handler_logout(Player *player);
	int handler_chat(Player *player);
	int handler_face(Player *player);
	int handler_interaction_emotion(Player *player);
	void handler_offline(Player *player);
	int handler_dissolve_room(Player *player);
	int handler_dissolve_action(Player *player);
	void check_dissovle_result();
	int advance_game_end();
	void uptable_error_uc(Player *player, int code);
	int handler_apply_uptable(Player *player);
	int handler_uptable(Player *player);
	int handler_downtable(Player *player);
	void update_user_info_to_datasvr(int UserScore[GAME_PLAYER]);
	void UpdateUserInfoToUser(Player *player);
	int game_start_req();
	int handler_preready();
	void handler_ready(Player* player);
	void handler_update_gps(Player *player);
	void handler_get_gps(Player *player);	
	bool CheckReqValid(Player *player);
	int OnUserPiaoOperate(Player* player);
	void PiaoError(Player *player, int code);
	void StartPiao();

	template<typename TYPE>
	const TYPE NextChair(const TYPE& ChairID);
	template<typename TYPE>
	const TYPE NextChair(const TYPE& BankerID, const TYPE& dis);	
	template<typename TYPE>
	const TYPE PreChair(const TYPE& ChairID);
	template<typename TYPE>
	void Calc_Score(TYPE base[GAME_PLAYER], int score[GAME_PLAYER]);
	char* CatHandCard(const HandCards& pHc, _uint8 ChairID);

//------------------------------------¼��------------------------------------
protected:
	//���������Ϣ
	virtual void SaveOutCardVideo(_uint8 wChairID, _uint8 cbCardData);
	//����¼����Ϣ
	virtual void save_video_data(_tint32 cmd, const std::string &str);
	//����¼����Ϣ
	virtual void send_video_to_svr();
	//����������Ϣ
	virtual void SaveGetCard(_uint8 chairID, _uint8 cbCard, int nLeftCardNum, bool bGetLast = false);
	//���������ʾ��Ϣ
	virtual void SaveOperateNotifyVideo();
	//�����һ����¼��(���������˵�����)
	virtual void save_video_firsthand_card();
	//���浥����¼��
	void save_operate_notify(const proto::game::AckOpNotify& stNotify);
	//���������Ϣ
	virtual void SaveOperateVideo(_uint8 wChairID, int cbOpCode, _uint8 cbCardData);
	//������Ʋ�����Ϣ
	virtual void SaveOperateVideo(int iOpType, _uint8 chairID, _uint8 paschairID, _uint8 cbCard, int score[GAME_PLAYER]);
	//������������Ϣ
	virtual void SaveOperateResVideo(_uint8 wChairID, int cbOpCode);
	//���濪����Ϣ
	virtual void SaveGameStartVideo();
	//�����ƾֿ�ʼʱ������Ϣ
	virtual void SaveTableInfoVideo();
	//����¼��
	virtual void save_video_hu(_uint8 chairID, _uint8 paschairID, int score[GAME_PLAYER]);

//------------------------------------���ݻָ�------------------------------------
public:
	//���治���������Ϣ
	virtual void InitProtoTable();
	//����䶯��������Ϣ
	virtual void SaveProtoTable();
	//���������Ϣ����λ��Ϣ
	virtual void SaveProtoPlayers();
	//���л���������
	virtual int SerializeTableData();
	//������������
	virtual int ParseTableData(std::string &data_str);
	virtual void ParseProtoTable();
	//�ָ�����������
	virtual void RecoverConfig();
	//����ʱ��
	virtual void AgainTimer();

//------------------------------------���̿���------------------------------------
public:
	//��������
	virtual void Reset();
	//������ʼ��
	virtual void Init(const Json::Value &val, const std::string &roomid);
	//��ʼ���߼�������
	virtual void InitLogic();
	//��ʼ����Ա����
	virtual void InitMember();
	//��ʼ�����淨��������
	virtual void InitBaseConf();
	//��ʼ����������
	virtual void InitConf(const Json::Value &val, const std::string &roomid);
	//��Ϸ����ʱ���̿���
	virtual int game_start_res(const Jpacket &packet);
	//��ȡ������Ϣ
	virtual int handler_table_info(Player *player);
	//�л����������Ϣ
	virtual void handler_switch_style(Player *player);
	//��ʼÿ����λ�Ĳ���
	void init_multi_data();
	//��Ϸ��ʼ
	virtual void GameStart();
	//��ׯ
	virtual void InitBanker();
	//��������
	virtual void SendGameStart();
	//����������Ϣ
	virtual void SendLaiziCard();
	//Э�鴦��
	virtual int HandlerUserOperate(int cmd, Player* player);
	//����
	virtual void OnUserGetCard(_uint8 ChairID, bool bGetLast = false);
	//�ж��ƿ�
	virtual bool CardPoolIsEmpty();
	//��һ�ִ���
	virtual void OnOperateStart();
	//��õ�һ����
	virtual void UserGetFirstCard(_uint8 ChairID, int& iOpType, MJ_fan_buffer& oFan);
	//ׯ�Ҵ����һ����
	virtual void UserFirstOutCard(_uint8 ChairID, int& iOpType, MJ_fan_buffer& oFan);
	//��һ�����һ����
	virtual void UserGetLastCard(_uint8 ChairID, int& iOpType, MJ_fan_buffer& oFan);
	//��ҳ���
	virtual bool OnUserOutCard(Player* player);
	//��Ҵ���ƺ�������ҵĲ�����ʾ�ж�
	virtual bool EstimateUserRespond(_uint8 bOutChairID, _uint8 bOutCard);
	//��Ҳ�����
	virtual bool OnUserOperateCard(Player* player);
	//�㲥����
	virtual void BroadcastOperateResult(_uint8 bOpChairID, int iOpType, _uint8 bOpCard);
	//�����
	virtual void DoMultiOperate(_uint8 ChairID, int iOpType, _uint8 bCard);
	//��ʱ�㲥�������û� Ԥ��
	void UserOvertime_State(_uint8 ChairID);
	//����û������Ƿ�Ϸ�
	virtual bool CheckUserOperator(_uint8 ChairID, int iOpType, _uint8 bCard);
	//���˿���ѡ���
	void DoMultiForceHu(_uint8 ChairID, _uint8 bCard);
	//���������
	virtual void MultiOperateOver(_uint8 chairID, int iOpType, _uint8 bCard);
	//������
	virtual void DoSimpleOperate(_uint8 ChairID, int iOpType, _uint8 bCard);
	//���ִ�г����ܲ����Ժ�
	virtual void UserAfterOperate(_uint8 ChairID, int iOpType, _uint8 bCard);
	//��Ҵ�����һ���ƣ���������ж�
	virtual void UserLastOutCard(_uint8 ChairID, int& iOpType, MJ_fan_buffer& oFan);
	//���̴����뷢��
	virtual void OperateError(_uint8 ChairID, int code);

//--------------------------------��Ϸ����������------------------------------------------
	//��Ϸ����
	virtual void GameEnd();
	//ͳ�ƺ�������
	virtual int MultiHu(MJ_user_buffer& id)const;
	//ȫ���������
	virtual void Fill_HandCard(_uint8 ChairID, proto::game::HandCards* pHandcard);
	//ȫ�����ϲ�����
	virtual void Fill_FixedCard(_uint8 ChairID, _uint8 Pos, proto::game::FixedCard* pstFix);
	//��Һ���
	virtual void Score_GameEndSucc(HuScoreSet& score, proto::game::AckGameEnd& stGameend);
	//����
	virtual void Score_GameEndDraw(HuScoreSet& score, proto::game::AckGameEnd& stGameend);
	//ʵʱ�ܷ�
	virtual void Update_Score(int iScore[GAME_PLAYER]);	
	//�����ּ���
	virtual void Calc_BaseScore(_uint8 ChairID, int iBaseScore[GAME_PLAYER]);
	//�ּܷ���
	virtual void Calc_HuScore(_uint8 ChairID, int iBaseScore[GAME_PLAYER], int iScore[GAME_PLAYER]);
	//��ׯ
	virtual void TurnBanker();
	//����������Ϣ
	virtual void GameEndReset();
	//�ܽ�������
	virtual void OnFinalEndReq(Player* player);
	//��ȡ���Ʒ���
	virtual int Calc_Fan(_uint8 ChairID, MJ_fan_buffer& oFan);
	//��ȡ���Ʒ�������
	virtual int Check_Hu_Pattern(_uint8 ChairID, const HandCards& pHc, MJ_fan_buffer& oFan, const MJ_win_pattern& oPattern);
	//��������
	virtual bool OnUserGainsReq(Player* player);
	//�жϺ�����
	virtual int Check_Hu(_uint8 ChairID, HandCards& pHc, MJ_fan_buffer& oFan);
	//ͳ����Ϣ
	virtual void CalcRecordInfo(const MJ_user_buffer& UserID, HuScoreSet& hu_score);
//--------------------------------��Ҳ���------------------------------------------
	//�Բ���
	virtual void UserOperateChi(_uint8 ChairID, int iOpType, _uint8 bCard);
	//������
	virtual void UserOperatePeng(_uint8 ChairID, _uint8 bCard);
	//������
	virtual void UserOperateHu(_uint8 ChairID, _uint8 bHuCard = 0xFF);
	//������
	virtual void UserOperatePass(_uint8 ChairID, int iOpType, _uint8 bCard);
	//�ܲ���
	virtual void UserOperateGang(_uint8 ChairID, int iOpType, _uint8 bCard);
	//���ܺ��������ȼ�
	virtual void RobGangPriority(_uint8 ChairID, _uint8 CardData, int OperateType);
	//�й��Զ�����
	virtual void OnAutoUserOutCard(_uint8 ChairID, _uint8 bCard);	
	//�й��Զ�����
    virtual void UserAutoOperate(_uint8 ChairID, _uint8 bCard);

//--------------------------------���߼��ж�----------------------------------------
	//�����ҳ���
	virtual bool CheckUserOutCard(_uint8 ChairID, _uint8 cbCard);
	//�������ܺ���־
	virtual void SetRobGangHu_Flag(_uint8 ChairID, int iOpType, proto::game::AckBcOpResult& stOpresult);
	//������ҵĺ���
	virtual int Calc_Hu(_uint8 ChairID, _uint8 bCard, int Score[GAME_PLAYER]);
	//�ж��Ƿ������ܺ�
    virtual	bool RobGangHu(_uint8 chairID, _uint8 CardData, int iOpType = TYPE_WANGANG);
	//����Ƿ�����
	virtual void SendTingInfo(_uint8 ChairID);
	//��ȡ������Ϣ(13������ʱ)
	virtual int GetListeningInfoNoOutCard(_uint8 ChairID, tagListenInfos stListenInfo[14], _uint8 &bListenLen);
	//��ȡ������Ϣ(14������ʱ)
	virtual int GetListeningInfo(_uint8 ChairID, tagListenInfos stListenInfo[14], _uint8& bListenLen);
	//���������Ϣ
	void Check_Listening(HandCards *pHc, _uint8 ChairID, _uint8 bCard, tagListenInfos stListenInfo[14], _uint8 &bListenLen);
	//��ȡ��������Ե���  ,����ӣ�ֱ�����ش˺���,���ƿ����棬�����������ɡ�
	virtual void GetAllHandCardRelativity(HandCards *pHc, MJ_BUFFER<_uint8, POOL_CARD_TYPE_MAX>& _relativity);
	//�����������Ե���
	void GetHandCardRelativity(_uint8 bCard, MJ_BUFFER<_uint8, POOL_CARD_TYPE_MAX>& _relativity);
	//��ȡ��ҳ�
	virtual int GetChi(_uint8 ChairID, _uint8 bCard, MJ_opinfo& ChiInfo);
	//��ȡ��ܡ�������Ϣ
	virtual int GetGangInfo(_uint8 ChairID, MJ_opinfo& GangCardInfo);
	virtual int GetGangInfo(_uint8 ChairID, MJ_opinfo& GangCardInfo, MJ_gang& lou_gang, _uint8 laizi_card);
	//��ҳ��ƺ��ж���������Ƿ��ܹ�������
	virtual int OutCardEstimate(_uint8 bOutChairID, _uint8 bCard, MJ_major& major, int& fan_num);
	//������ƺ��ж��Ƿ��ܹ�������
	virtual int GetCardEstimate(_uint8 ChairID, _uint8 bGetCard, MJ_major& major, int& fan_num);
	//���ֱ�ܡ�����Ϣ
	virtual int GetPengGang(_uint8 ChairID, _uint8 cbCard, MJ_opinfo& GangCardInfo);
	virtual int GetPengGang(_uint8 ChairID, _uint8 cbCard, MJ_opinfo& GangCardInfo, _uint8 laizi_card);
	//����Ƿ�����
	virtual bool IsListening(_uint8 ChairID, _uint8 bRemoveCard);
	//����Ƿ�����
	virtual bool IsListening(_uint8 ChairID, tagListenInfos stListenInfo[14], _uint8 &bListenLen);
	//��ȡ����ĳ����ֵ����
	virtual int  Get_Card_Left_Num(_uint8 ChairID, _uint8 cbCard);
	//�Ƴ��������
	virtual int removeAllGuiCard(HandCards *pHc, std::map<_uint8, _uint8>&guiCardCountMap, bool& isFourGui);
	//�����������
	virtual void addGuiCard(HandCards *pHc, std::map<_uint8, _uint8>&guiCardCountMap);
	//�ж��������
	virtual void Check_Listening_LaiZi(HandCards *pHc, _uint8 ChairID, _uint8 bCard, tagListenInfos stListenInfo[14], _uint8 &bListenLen);
	//�����������Ե����������
	virtual void GetHandCardRelativity_LaiZi(_uint8 bCard, MJ_BUFFER<_uint8, POOL_CARD_TYPE_MAX>& _relativity);
//----------------------------------���������ж����----------------------------------
	//���ϻ�
	virtual void UserGangShangHua(_uint8 ChairID, _uint8 bCard, MJ_fan_buffer& oFan);
	//�ܺ���
	virtual void UserGangHouPao(_uint8 ChairID, _uint8 bCard, MJ_fan_buffer& oFan);
	//������ܺ�
	virtual void UserQiangGangHu(_uint8 ChairID, _uint8 bCard, MJ_fan_buffer& oFan);
	//7��
	virtual bool Is7Dui(_uint8 ChairID, HandCards& pHc, MJ_fan_buffer& oFan);
	//13��
	virtual bool Is13Yao(HandCards& pHc, MJ_fan_buffer& oFan, MJ_win_pattern& pattern);
	//�ж�4��1������
	virtual bool FormatCards(HandCards *hcs, MJ_win_pattern& pattern);
	//ȫ����
	virtual bool IsQuanqiuren(const HandCards& pHc, MJ_fan_buffer& oFan, int& iOpType);
//---------------------------------��Ϸ����proto����----------------------------------
	//
	virtual void Proto_GameScene_lookon(_uint8 ChairID, Player* player, proto::game::AckGameScene& stGameScene);
	virtual void Proto_GameEndInfo(const MJ_user_buffer& UserID, proto::game::AckGameEnd& stGameend);	
	virtual void Proto_GameEndInfo_AddFan(const MJ_user_buffer& UserID, proto::game::AckGameEnd& stGameEnd);
	virtual void Proto_GameEndInfo_GangInfo(_uint8 ChairID, proto::game::AckGameEnd& stGameend);
	virtual void Proto_Game_OpNotify(_uint8 ChairID, _uint8 CurChairID, _uint8 bCard, proto::game::AckOpNotify& stNotify);
	virtual void Proto_Game_OpNotifyAfterPengChi(_uint8 ChairID, _uint8 CurChairID, const MJ_opinfo& gang, proto::game::AckOpNotify& stNotify);
	virtual void Proto_Game_UserGetCard(_uint8 ChairID, _uint8 CurChairID, _uint8 bCard, proto::game::AckUserGetCard& stUsergetcard);
	virtual void Proto_Game_OperateGang(_uint8 ChairID, _uint8 bCard, int iOpType, int nGangScore[GAME_PLAYER], proto::game::AckBcOpResult& stOpresult);
	virtual void Proto_Update_Gps(proto::game::UpdateGPSAck& stUpdateGPSAck);
	//���������Ϣ
	void PackageTingInfo(_uint8 ChairID, tagListenInfos stListenInfo[14], _uint8 iPos, proto::game::tagListenInfo* listeninfo);
	

//-------------------------------------��������--------------------------------------------
	//��������
	virtual void ResetAction();
	//��ȡ���UID
	int GetPlayerUid(int ChairID);

	int GetPlayerUid(_uint8 ChairID);

	int GetPlayerUid(const Player* player);

	int pow(int n);


	void Calc_Score(int base[GAME_PLAYER], int score[GAME_PLAYER])
	{
		for (int i = 0; i < m_GamePlayer; i++)
		{
			base[i] += score[i];
		}
	}
	void Calc_Score(HuScoreSet& base, int score[GAME_PLAYER])
	{
		for (int i = 0; i < m_GamePlayer; i++)
		{
			base[i].iTotleScore += score[i];
		}
	}
	

//������
public:
	
//��Ա����
	bool 							m_bPiao;
	std::set<int> 					m_PiaoScore;					//��Ʈ�ķ�
		
};

template<typename TYPE>
const TYPE Table::NextChair(const TYPE& ChairID)
{
	return (ChairID + 1) % m_GamePlayer;
}

template<typename TYPE>
const TYPE Table::NextChair(const TYPE& ChairID, const TYPE& dis)
{
	return (ChairID + dis) % m_GamePlayer;
}
template<typename TYPE>
const TYPE Table::PreChair(const TYPE& ChairID)
{
	return (ChairID + m_GamePlayer - 1) % m_GamePlayer;
}

template<typename TYPE>
void Table::Calc_Score(TYPE base[GAME_PLAYER], int score[GAME_PLAYER])
{
	for (int i = 0; i < m_GamePlayer; i++)
	{
		base[i].iTotalScore += score[i];
	}
}

#endif