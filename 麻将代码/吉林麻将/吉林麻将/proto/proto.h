#ifndef __PROTO_H__
#define __PROTO_H__

enum system_command
{
	SYS_ECHO						= 0001,       /* echo */
	SYS_ONLINE						= 0002,       /* get online */
	SYS_SETUP						= 0003,       /* setup table */
    SYS_GET_USER_NUM				= 0004,      //��ȡ��������
	SERVER_REGISTER_REQ				= 7000, //���Ʒ�����ע��
	LINK_DB_CLIENT_BREAK			= 7001, //�������������ݿ�������Ͽ�����

	SERVER_CONFIGURE_REQ			= 7002, //�����ȡ���Ʒ�������ǰ����
	SERVER_CONFIGURE_RES			= 7003, //�������Ʒ�������ǰ����
	UPDATE_SERVER_CONFIGURE_REQ		= 7004, //����������Ʒ���������
	UPDATE_SERVER_CONFIGURE_RES		= 7005, //���ظ������Ʒ������������

	SERVER_UPDATE_MONEY				= 7006, //�����Ʒ������������ͬ����ҽ�ҵ����Ʒ�����
	SERVER_UPDATE_USER_INFO			= 7007, //���Ʒ������������������ݿ�������������������Ʒ������ڴ�����
	SERVER_UPDATE_DB_TASK_REQ       = 7008, //���Ʒ������������������ݿ���������
	SERVER_UPDATE_DB_TALLY_REQ      = 7009, //���Ʒ��������������ˮ��Ϣ�����ݿ�
	SERVER_SPEAK_INFO_WRITE_REQ     = 7010, //���Ʒ�����������´�������Ϣ�����ݿ�
	SERVER_USER_BROKE_WRITE_REQ     = 7011, //���Ʒ����������������Ʋ���Ϣ�����ݿ�
	SERVER_USER_ZID_INFO_WRITE_REQ  = 7012, //���Ʒ��������������ҵ�ǰ���ε�vid��zid��Ϣ
	SERVER_ADD_ROBOT_MONEY          = 7104, //���Ʒ���������ӻ����˽��

	SERVER_INFORM_USER_VID_TO_DATASV = 7105, //���Ʒ�����֪ͨdatasvr��ҳɹ�����ó�

	SERVER_SET_BANKER_TAG_TO_DB    = 7106,  //���Ʒ���������洢��ׯ��־
	SERVER_SAVE_POOL_TO_DB         = 7107, //���Ʒ���������洢��������
	SERVER_GET_POOL_FROM_DB        = 7108, //���Ʒ���������ǰ���ӽ������� 
	SERVER_GET_POOL_BACK           = 7109, //���ݿⷵ�ؽ������ݸ����Ʒ�����
	SERVER_SAVE_USER_OTHER_INFO    = 7110, //���Ʒ������洢���������ص�����
	SERVER_SAVE_GAME_INFO          = 7111, //���Ʒ������洢������Ϸ�������
	SERVER_GET_GAME_INFO           = 7112, //���Ʒ����������ȡ������Ϸ�������
	SERVER_GET_GAME_INFO_BACK      = 7113, //������Ϸ���ݸ����Ʒ�����
	SERVER_LAOHUJI_INFO            = 7114, //���Ʒ����������ϻ�����Ϣ����ˮDB������ȷ�����
	SERVER_INCR_USER_TIPS		   = 7115, //���Ʒ���������������������ʾ����
	
	SERVER_RECONNECT_REQ		   = 7201, //ƽ̨����ͳ�ƿͻ�������
	SERVER_DISBAND_ROOM			   = 7214, //��ɢ����������Ϸ���������͸���̨��������
	SERVER_DISBAND_ROOM_RESP	   = 7215, //��ɢ����ɹ�����	
	SERVER_ALL_PLAYER_DOWN		   = 7217, //���Ʒ�����֪ͨdatasvr ���������е�����Ѿ����߻����˳�
	SERVER_DELETE_ROOM_REQ  	   = 7219, //datasvr���͸����Ʒ���������ѯ�Ƿ����ɾ������
	SERVER_DELETE_ROOM_RESP		   = 7218, //���Ʒ��������͸�datasvr�� �ظ��Ƿ����ɾ������
	SERVER_LOGOUT_ROOM_REQ		   = 7220, // ���Ʒ����������˳�����
	SERVER_LOGOUT_ROOM_RESP	       = 7221, // �˳�����ɹ�����
	SERVER_FORCE_LOGOUT_ROOM_REQ	= 7222, //ǿ���˳�����
	SERVER_FORCE_LOGOUT_ROOM_RESP	= 7223,	//ǿ���˳����䷵��
};

enum client_command
{
	CLIENT_LOGIN_REQ				= 1001,       /* join table */
	CLIENT_LOGOUT_REQ				= 1002,
	CLIENT_READY_REQ				= 1003,	      /* game ready */
	CLIENT_BET_REQ		 			= 1004,
	CLIENT_CHAT_REQ					= 1005,
	CLIENT_FACE_REQ					= 1006,
	CLIENT_CHANGE_REQ            = 1007,       /* change table */
	CLIENT_ROBOT_REQ             = 1008,
	CLIENT_INFO_REQ              = 1009,       /* update player info */
	CLIENT_TABLE_INFO_REQ        = 1010,      /* table info */
	CLIENT_EMOTION_REQ           = 1011,      /* interaction emotion */
	CLIENT_PROP_REQ              = 1012,      /* game prop */
	CLIENT_BOX_REQ               = 1013,
	CLIENT_ACTIVE_REQ            = 1014,
	CLIENT_BETTING_REQ           = 1015,
	CLIENT_UPTABLE_INFO_REQ      = 1016,
	CLIENT_UPTABLE_APPLY_REQ     = 1017,   //����
	CLIENT_DOWNTABLE_AHEAD_REQ   = 1018,
	CLIENT_DOWNTABLE_REQ         = 1019,   // ����
	CLIENT_RETURN_REQ            = 1020,
	CLIENT_DAEMONIZE_REQ         = 1021,
	CLINET_UPBANKER_REQ          = 1022,
	CLINET_UPBANKER_INFO_REQ     = 1023,
	CLINET_DOWN_BANKER_REQ       = 1024,
	CLINET_HEART_BEAT_REQ        = 1025,

	CLINET_TREND_REQ             = 1040,

	CLIENT_ALL_PLAYERS_REQ       = 1051,
	CLIENT_UP_GUEST_REQ          = 1052,
	CLIENT_DOWN_GUEST_REQ        = 1053,
	CLIENT_POOL_INFO_REQ         = 1054,
	CLINET_OTHER_SHOW_REQ        = 1056,
	CLIENT_CHOOSE_CARDS_REQ		 = 1057,
	CLIENT_TIP					 = 1059,	
	CLIENT_CHOSE_BETS_NUM			= 1060,
	CLIENT_DETAL_INFO_REQ        = 1070,

	CLIENT_RANK_REQ              = 1080,
	CLIENT_HISTORY_REQ           = 1090,

	//�齫Э��
	CLIENT_MJ_CHANGE_CARD		 = 1100, //������
	CLIENT_MJ_CHOOSE_LACKTYPE	 = 1101, //ѡ��ȱɫ
	CLIENT_MJ_OUT_CARD			 = 1102, //����
	CLIENT_MJ_OPERATE_CARD	 	 = 1103, //ѡ�����
	CLIENT_MJ_AUTOHU			 = 1104, //�Զ�����
	CLIENT_MJ_TRUSTEE			 = 1105, //�й�
	CLIENT_MJ_USER_ACTION		 = 1106, //���ѡ��׼�����뿪��������
	CLIENT_MJ_HOME_RETURN		 = 1107, //�ͻ��˰�home������
	CLIENT_MJ_CHOOSE_PIAO		 = 1108, //ѡ��Ʈ
	CLIENT_MJ_LISTEN_TYPE		 = 1109, //���ѡ������(���������ơ����ű���)

	CLINET_MJ_TIAN_TING			 = 1110, //�������
	CLINET_MJ_YOUJIN			 = 1111, //�ν�ť��Ϣ
	CLIENT_MJ_BAI_PAI_SEL        = 1112 ,  //  �ͻ��ˡ��������Ƶ�ѡ�� 
	CLIENT_MJ_JILIN_GAGN         = 1113  ,  // �����齫����  
	CLIENT_MJ_NINGXIANG_FENG_DONG  = 1114  , 


	//��ɢ����
	CLIENT_DISBAND_ROOM			 = 10000, //��������ɢ����
	CLIENT_DISBAND_CHOOSE		 = 10001, //����Ƿ��ɢ�����ѡ��
	/* ��ս */
	CLIENT_DZ_LOGIN_REQ			= 1200,
	
	CLIENT_MJ_VIDEO_DATA_REQ    = 1300,  //¼���������� 
	CLIENT_HOME_OUT             = 1400,   //��Ұ�home���뿪
	CLIENT_REQ_ROUND_SCORE		= 1401,	//ս��
	CLIENT_GAME_END_RECORD_REG  = 1600,   //��������ܽ�����Ϣ
	//CLIENT_MJ_OUT_CARD = 1102, //���� 
	CLIENT_XIAOHUA_OUT_CARD     =  1601 ,

};

enum client_action
{
	PLAYER_CALL           = 2001,       /* call */
	PLAYER_RAISE          = 2002,       /* raise */
	PLAYER_COMPARE        = 2003,       /* compare */
	PLAYER_SEE            = 2004,		  /* see */
	PLAYER_FOLD           = 2005,       /* fold */
	PLAYER_ALLIN          = 2006,     /* all in */
	PLAYER_ALLIN_COMPARE  = 2007,     /* all in compare */
};

enum prop_item
{
	CHANGE_CARD = 3001,  /* change card */
	FORBIDDEN_CARD = 3002, /* forbidden compare card */
	DOUBLE_CARD_FOUR = 3003,    /* compare four multiple bet card */
	DOUBLE_CARD_SIX = 3004,    /* compare six multiple bet card */
	DOUBLE_CARD_EIGHT = 3005,    /* compare eight multiple bet card */
	NOSEE_CARD = 3006,    /* cannot see */
};

enum server_command
{
	SERVER_LOGIN_SUCC_UC       		 = 4000,
	SERVER_LOGIN_SUCC_BC       		 = 4001,
	SERVER_LOGIN_ERR_UC         	 = 4002,
	SERVER_REBIND_UC				 = 4003,
	SERVER_LOGOUT_SUCC_BC			 = 4004,
	SERVER_LOGOUT_ERR_UC			 = 4005,
	SERVER_TABLE_INFO_UC			 = 4006,
	SERVER_READY_SUCC_BC			 = 4007,
	SERVER_READY_ERR_UC				 = 4008,
	SERVER_GAME_START_BC			 = 4009,
	SERVER_NEXT_BET_BC				 = 4010, //��һ��ִ�ж��������
	SERVER_BET_SUCC_BC				 = 4011, //��Ҳ�������Ĺ㲥
	SERVER_BET_SUCC_UC				 = 4012,
	SERVER_BET_ERR_UC                = 4013,
	SERVER_GAME_END_BC				 = 4014, //��Ϸ����
	SERVER_GAME_PREREADY_BC          = 4015,
	SERVER_CHAT_BC					 = 4016,
	SERVER_FACE_BC					 = 4017,
	SERVER_ROBOT_SER_UC              = 4018,
	SERVER_ROBOT_NEED_UC             = 4019,
	SERVER_UPDATE_INFO_BC			 = 4020,
	SERVER_EMOTION_BC				 = 4021,
	SERVER_PROP_SUCC_UC              = 4022,
	SERVER_PROP_SUCC_BC              = 4023,
	SERVER_PROP_ERR_UC               = 4024,
	SERVER_BOX_SUCC_BC				 = 4025,
	SERVER_ACTIVE_SUCC_UC            = 4026,
	SERVER_BETTING_SUCC_UC			 = 4027,//Ѻע�ɹ�
	SERVER_ACTIVE_UPDATE_BC			 = 4028,//�Թ�Ѻע����
	SERVER_UPTABLE_INFO_UC			 = 4029,//�����б���Ϣ
	SERVER_APPLY_UPTABLE_UC			 = 4030,//���������ɹ�����
	SERVER_UPTABLE_SUCC_BC			 = 4031,//���������
	SERVER_AHEAD_DOWNTABLE_UC		 = 4032,//
	SERVER_DOWNTABLE_SUCC_BC		 = 4033,//���������
	SERVER_START_BET_BC				 = 4034,//Ѻע��ʼ
	SERVER_STOP_BET_BC				 = 4035,//ֹͣ�Թ�Ѻע
	SERVER_WIN_UC					 = 4036,//Ѻע��������
	SERVER_BETTING_ERR_UC			 = 4037,//Ѻעʧ��
	SERVER_UPTABLE_ERR_UC			 = 4038,//����ʧ��
	SERVER_RETURN_UC                 = 4039,
	SERVER_REWARD_SUCC_UC            = 4040,
	SERVER_DAEMONIZE_SUCC_UC		 = 4041,
	SERVER_BANKER_CANDIDATE_UC       = 4042,
	SERVER_COMPARE_INFO_BC           = 4043,
	SERVER_DOWN_BANKER_UC            = 4044,
	SERVER_GIVE_UP_BANKER_BC         = 4045,	
	SERVER_GET_BANKER_BC             = 4046,
	SERVER_SHORT_UPTABLE_INFO_BC     = 4049,
	
	SERVER_MULTI_BETS				 = 4060,//ѡ����
	SERVER_CHOSE_COMPLETE			 = 4065,//�������
	SERVER_COMPARE_CARDS_BC			 = 4066,//����
	SERVER_TIP						 = 4067,//û�õ�
	SERVER_SECOND_CARD				 = 4068,//�ڶ�������
	SERVER_THIRD_CARD				 = 4069,//����������
	SERVER_GAME_DARK_CARD			 = 4075,//��һ�ַ��İ���
	SERVER_START_COUNTDOWN			 = 4084,//��ͨ����
	SERVER_CHANGE_BANKER			 = 4085,//��ͨ����
	SERVER_BET_OVERSTEP_UC			 = 4086,
	SERVER_BETS_BC					 = 4087,

	SERVER_TREND_UC                  = 4100,
	SERVER_ALL_PLAYER_INFO_UC        = 4110,
	SERVER_UP_GUEST_SEAT_ERROR_UC    = 4111,
	SERVER_UP_GUEST_SEAT_SUC_BC      = 4112,
	//SERVER_DOWN_GUEST_SEAT_ERR_UC  = 4113,
	SERVER_DOWN_GUEST_SEAT_BC        = 4114,
	SERVER_POOL_INFO_UC              = 4115,

	SERVER_POOL_ALTER_BC             = 4116,
	SERVER_ROBOT_SER                 = 4117,
	SERVER_COMMUNITY_BC              = 4118,

	SERVER_TWO_COMMUNITY_UC          = 4119,
	SERVER_EMOTION_NOSEAT_BC         = 4120,
	SERVER_OTHER_SHOW_BC             = 4121,
	SERVER_BANNE_UC 			     = 4122,
	SERVER_GAME_RESULT_BC			 = 4123,
	SERVER_UPBANKER_ERR_UC			 = 4124,

	SERVER_DETAL_INFO_UC             = 4140,
	SERVER_DETAL_INFO_ERROR_UC       = 4141,

	SERVER_RANK_UC                   = 4160,
	SERVER_LOTTERY_SHOW_CARDS        = 4170,
	SERVER_HISTORY_UC                = 4171,
	SERVER_MULTI_OF_LOSE_BC          = 4210,
	SERVER_COPARE_RESULT_FORROBOT    = 4212,

	//�齫��ϷЭ����
	SERVER_USER_READY			 = 4998, //�㲥׼��״̬
	SERVER_GAME_SCENE			 = 4999,//����Э��
	SERVER_GAME_START 			 = 5000,//��Ϸ��ʼ
	SERVER_FIRST_HANDCARD		 = 5001,//��һ����
	SERVER_CHANGE_CARD			 = 5009,//�㲥����
	SERVER_USER_LACK_TYPE 		 = 5002,//�㲥��ҵĶ�ȱɫ
	SERVER_USER_GETCARD			 = 5003,//����
SERVER_USER_OUTCARD = 5004,//����
SERVER_OPERATE_NOTIFY = 5005,//���ƺ����������ʾ
SERVER_OPERATE_RESULT = 5006,//�㲥�������
SERVER_LISTEN_NOTIFY = 5010,//������������ʾ
SERVER_USER_HU = 5007,//�û�����
SERVER_GAME_END = 5008,//��Ϸ����
SERVER_TRUSTEE = 5011,//�й�
SERVER_UPDATE_USERINFO = 5012,//���������Ϣ
SERVER_UPDATE_TIMER = 5013,//��ҳ�ʱ���㲥����ʱ��
SERVER_HEART_BEAT_RESP = 5014,//����������
SERVER_BUY_HORSE = 5015,//����������Ϣ
SERVER_VIDEO_FIRST_HANDCARD = 5016,//¼����Э��
SERVER_BEGIN_QUAN_FENG = 5017,//��ʼȦ��
SERVER_TURN_SEATID = 5018,  //��ʼת����λ���·�������ϢAckTableInfo��
SERVER_ENSURE_JOKER = 5019,  //��ʼ�����
SERVER_USER_OFFLINE_BC = 5020,//�㲥��ҵ���
SERVER_OVERTIME_USER = 5021,//�㲥��ʱû���Ƶ����
SERVER_BROAD_PIAO = 5022,//�㲥���ѡ���Ʈ
SERVER_BROAD_LISTENTYPE = 5023,//�㲥���ѡ����������ͣ����������ơ����ű�����
SERVER_START_PIAO = 5024,//��ʼѡ��Ʈ	
SERVER_CP_BANKER = 5025,  //��ׯ���
SERVER_FLOWER_HAND_CARD = 5026,  //����һ���ƺ󲹻�
SERVER_GET_HUA_CARD = 5027,  //��Ϸ��������������
SERVER_SEND_MESSAGE = 5028,//������ʾ��Ϣ
SERVER_HUBEI_LAIZIINFO = 5029,//���ͺ����齫�����Ϣ	
SERVER_GAME_END_JIANGMA = 5030,//��Ϸ�������� ���루ץ����ץ����Ϣ
SERVER_OPERATE_WAITE = 5031, //�ȴ�������Ϣ
SERVER_BAOPAI_REACH = 5032, //���ĵ��ƴ��
SERVER_VIDEO_CHANGE_FLOWER = 5033, //���ƺ󻻻�  
SERVER_ZIMOHU_HUANGHUANG = 5034, //�����λ��齫��������
SERVER_LAIZIGANG_HUANGHUANG = 5035, //�����λ��齫���·���Ӹܱ���
SERVER_FOURCARD_HUANGHUANG = 5036, //�����λ��齫���·����������	
/* �����齫 */
SERVER_GET_JI_PAI = 5040, //������		
SERVER_BAO_TING_SUCC = 5041, //�����ɹ�
/*�����齫*/
SERVER_DA_SHAIZI = 5042,//��ɸ��
SERVER_QISHOUHU = 5043,//���ֺ�������
SERVER_BANPAI_END = 5044,//���ֺ������ƽ�������������
SERVER_GANG_MO = 5055,//���ƺ�����
SERVER_GANG_CHU = 5056,//���ƺ����
//����2
SERVER_TIAN_TING = 5057,//��������
SERVER_JI_PAI_OP = 5058,//�㲥���Ʋ���
SERVER_LOCK_CARD = 5059,//��������
//�����齫
SERVER_FZ_CHANGE_HUA_CARD = 5060,//����֮�󻻻���
SERVER_FZ_GET_LAI_HUA_CARD = 5061,//��������ǻ���	
SERVER_FZ_GET_HUA_CARD = 5062,//����֮���ǻ���
//����3
SERVER_USER_CHOOSE_LACK = 5063,//�㲥�û�ѡ��ȱ
//����
SERVER_SANBI_FENG_HU = 5064,//�������ʷ��
//����
SERVER_LAIBIN_SANBI = 5065,//�����齫����
SERVER_LAIBIN_FENG_HU = 5066,//�����齫���
//��ƽ�齫
SERVER_NANPING_SENDZHONG = 5067,		//�������ƣ����͡��С�����
//�����齫
SERVER_LONGYAN_YOUJIN = 5068,		//�����齫 �ν�����
//����
SERVER_ZUNYI_YUANQUE = 5069,//����ԭȱ
//��
SERVER_ZHENJIANG_GENZHANG = 5070,   //������Ϣ
SERVER_ZHENJIANG_DASUM = 5071,   //��������
//�����齫�������ν𶯻�
SERVER_LONGYAN_ANIMATION = 5072,		//�����齫 �ν𶯻�
//�ϲ��齫����Э��
SERVER_NANCHANG_HUITOUXIAO = 5073,		//��ͷЦ
SERVER_NANCHANG_JINGZI = 5074,		//�վ���
SERVER_NANCHANG_SHOWJINGFEN = 5075,		//��Ϸ���㣬��ʾ����



SERVER_ACK_ROUND_SCORE = 6101,	//ս��


//��ɢ����
SERVER_APPLY_DISBAND_FAILD = 15000,//��������ɢ����ʧ��
SERVER_APPLY_DISBAND_SUCC = 15001,//�����ɢ�ɹ��󣬹㲥�ý�ɢ��������
SERVER_DISBAND_CHOOSE = 15002,//����Ƿ��ɢ�����ѡ��
SERVER_DISBAND_RESULT = 15003,//��ɢ����Ľ��
//����ͳ����Ϣ
SERVER_GAME_RECORD = 16000,//��Ϸ����ͳ����Ϣ 
/* ��ս */
SERVER_DZ_LOGIN_SUCC_RES = 4400, // ��սģʽ��¼�ɹ����� 
SERVER_DZ_LOGIN_ERR_RES = 4401, /* ��սģʽ��¼ʧ�ܷ��� */
SERVER_DZ_GAME_START_REQ = 7210, /* ��սģʽ���Ʒ�����������Ϸ��ʼ */
SERVER_DZ_GAME_START_RES = 7211, /* ��սģʽ���Ʒ�����������Ϸ��ʼ���� */
SERVER_DZ_UPDATE_USER_INFO = 7212, /* ��սģʽ���Ʒ�����������Ҹ�����Ϣ */
SERVER_DZ_UPDATE_USER_INFO_LITTLE = 7213, /*��սģʽ���Ʒ�����������Ҹ�����ϢС���� */


SERVER_VIDEO_DATA_SAVE = 10002,   //����¼������
SERVER_VIDEO_DATA_ERR_RESP = 10003,    //¼�����ݱ�����󷵻�
SERVER_ROOM_CONFIG_REQ = 10004,    //��������Ϣ
SERVER_ROOM_CONFIG_RES = 10005,    //������Ϣ����

SEVER_CMD_BAI_PAI_SEL = 10006,

SER_CMD_BAI_PAI_NOTIFY = 10007,   //  ����Ƿ��������߿ͻ��ˡ�������ѡ����ơ��� 
SER_CMD_QISHOU_HU_RESULT = 10008,   // ���ֺ����� 
SER_CMD_XIAO_HUA_OUT_CARD = 10009 ,
//

SER_CMD_NINGXIANG_HU_ = 10010 ,
SER_CMD_GUILIN_OP_GANG_ZHUI  = 10011 ,   // ���ָܡ���׷����Ʈ����Ϣ ��
SER_CMD_GUILIN_XING_         = 10012 ,   // �����齫������ 
SER_CMD_GUI_LIN_BAO          = 10013 ,    // �����齫�������� 


SER_CMD_OUT_CARD_ERR = 10014,  // ���ƴ��� 


SER_CMD_JILIN_GAGN_SUCCESS  = 10015 ,  //  ���ָܳɹ��ء� 
SER_CMD_JILIN_BU_SUCCESS   = 10016 , //  ���ֲ��ɹ� 
SER_CMD_JILIN_BAOPAI_MSG = 10017  ,     //   ���ֱ��ƣ� 

SET_CMD_NINGXIAGN_GAME_START_SHAI_LAIZI   =  10018,    // �����齫�����㲥��ӣ���������������
SER_CMD_NINGXIAGN_OUT_KING              = 10019 ,        // �����齫������������� 
SER_CMD_NINGXIAGN_FENG_DONG   = 10020 ,                   // �����齫���� �ⶳ�ġ� 

SER_CMD_JILING_GANG_INFO_AFTER_GANG  = 10021 ,   //  ��������ܺ󣬣�����Ϣ
SER_CMD_JILING_QIANG_GANG_MSG   = 10022 ,   //  ��������ܺ�  ��Ϣ��  

SER_CMD_JILIN_NOWING_BU          =10023 , 





};

//�¿�ܰ�ͷЭ��
enum newheadcmd
{
	CLIENT_MAIN_CMD = 100,     //�ͻ�����cmd
	SERVER_MAIN_CMD = 101      //�������cmd

};
enum newclinetheadsubcmd
{
    CLIENT_SUB_CMD = 101,      //�ͻ�����cdm
};
enum newserverheadsubcmd
{
	SERVER_GAME_TO_SERVER = 101,     //��Ϸ�� ���� ƽ̨��cmd
    SERVER_REGISTER_LOBBY_REQ = 150,       //ע�������cmd
    SERVER_REGISTER_LOBBY_ACK = 151,       //ע�᷵��
	CLIENT_BREAK_NOTIFY = 152,       //�ͻ��˶���֪ͨ
	GATE_BREAK_NOTIFY   = 156,       //���ض���
    DB_BREAK_NOTIFY     = 158,       //ƽ̨����֪ͨ
	SERVER_GET_ONLINE_USER_NUM = 162, //��ȡ��������
};

#endif
