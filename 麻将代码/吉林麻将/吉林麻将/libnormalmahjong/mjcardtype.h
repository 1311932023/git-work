/********************************************************************
    created:	2013/5/02
    filename: 	MJCardType.h
    author:		
    
    purpose:	麻将类型定义
*********************************************************************/
#ifndef __MAHJONG_DEF_H__
#define __MAHJONG_DEF_H__
#include "common.h"
#include "mj_common2.h"

#define  BIAN_DAN	3
#define  DUI_ZI		2
#define  BIAN_KAN	1
#define  SINGLE		0
//游戏定时器
#define TIMER_WAIT_READY        10                  //等待准备定时器
#define TIMER_DICE              20                  //骰子时器
#define TIMER_LACKCARD          30                  //定缺牌定时器
#define TIMER_OUT_CARD          40                  //出牌定时器
#define TIMER_OPERATE           50                  //操作定时器
#define TIMER_DELAY_OUT_CARD    60                  //延迟出牌定时器
#define TIMER_GET_CARD          70                  //发第14张牌定时器
#define TIMER_MULTIOPERATE      80                  //多操作时钟
#define TIMER_DELAY_GET_CARD    90                  //自摸延迟定时器
#define TIMER_DELAY_WRITE_SCORE 100                 //延迟写分
#define TIMER_CHANGE_TIME       110                 //换牌时间
#define	TIMER_AUTO_OUT_CARD     120                 //系统自动出牌定时器

#define LOOPNUM   5


/**
 * 番类型ID定义
 */
enum MJFanType
{	
	MJ_FAN_TYPE_NULL = 0  ,
	MJ_FAN_TYPE_JIULIANBAODENG =1  ,				//九莲宝灯
	MJ_FAN_TYPE_DASIXI = 2,						//大四喜
	MJ_FAN_TYPE_XIAOSIXI =3,					//小四喜
	MJ_FAN_TYPE_DASANYUAN =4,					//大三元 
	MJ_FAN_TYPE_SIANKE =5 ,						//四暗刻
	MJ_FAN_TYPE_SHIBALUOHAN =6,				//十八罗汉
	MJ_FAN_TYPE_QUANYAOJIU =7 ,					//全幺九	
	MJ_FAN_TYPE_ZIYISE =8,						//字一色
	MJ_FAN_TYPE_XIAOSANYUAN =9 ,				//小三元
	MJ_FAN_TYPE_QINGYAOJIU =10,					//混幺九,清幺九
	MJ_FAN_TYPE_QINGDUI =11,					//清对 清一色 + 碰碰胡	
	MJ_FAN_TYPE_QINGPENG =12,					//清碰， 清一色 + 碰碰胡
	MJ_FAN_TYPE_HUNPENG =13 ,					//混碰, 混一色 + 碰碰胡
	MJ_FAN_TYPE_QINGYISE =14,					//清一色

	MJ_FAN_TYPE_HUNYISE =15,					//混一色
	MJ_FAN_TYPE_PENGPENGHU =16 ,					//碰碰胡		
	MJ_FAN_TYPE_PINGHU =17,						//平胡	
	MJ_FAN_TYPE_JIHU = 18,						//鸡胡	
	MJ_FAN_TYPE_ZIMO  = 18 ,               //= MJ_FAN_TYPE_JIHU =18 ,	//自摸
	MJ_FAN_TYPE_TIANHU =19 ,						//天胡
	MJ_FAN_TYPE_DIHU =20 ,						//地胡
	MJ_FAN_TYPE_RENHU = 21 ,						//人胡
	MJ_FAN_TYPE_QIANGGANGHU = 22 ,				//抢杠胡
	MJ_FAN_TYPE_HAIDILAOYUE =23,				//海底捞月
	MJ_FAN_TYPE_GANGSHANGHUA =24 ,				//杠上开花
	MJ_FAN_TYPE_QINGQIDUI =25 ,					//清七对	
	MJ_FAN_TYPE_QINGLONGQIDUI =26,				//清龙七对 
	MJ_FAN_TYPE_LONGQIDUI =27 ,					//龙七对

	MJ_FAN_TYPE_QUANQIUREN =28 ,					//全求人
	MJ_FAN_TYPE_GANGHOUPAO =29 ,					//杠上炮
	MJ_FAN_TYPE_MENQIANQING =30,				//门前清
	MJ_FAN_TYPE_FANYISE = 31,					//风一色
	MJ_FAN_TYPE_MENQINGQINGYISE =32,			//门前清 + 清一色
	MJ_FAN_TYPE_QINGSIANKE =33,					//清一色 + 四暗刻
	MJ_FAN_TYPE_HAIDIPAO =34 ,					//海底炮
	MJ_FAN_TYPE_13YAO =35,						//十三幺	
	MJ_FAN_TYPE_QIDUI =36 ,						//七对		
	MJ_FAN_TYPE_DAIYAOJIU =37,					//带幺九	
	MJ_FAN_TYPE_JIANGDUI =38 ,					//将对, 258做将
	MJ_FAN_TYPE_YITIAOLONG =39,					//一条龙
	MJ_FAN_TYPE_QINGYITIAOLONG =40,				//清一条龙
	MJ_FAN_TYPE_4JOKER =41,						//4鬼胡
	MJ_FAN_TYPE_NOJOKER =42,					//无鬼
	MJ_FAN_TYPE_QINGQUANQIUREN =43,				//清全求人
	MJ_FAN_TYPE_ZHUANGXIAN = 44,
	MJ_FAN_TYPE_MAX = 45,

	MJ_FAN_TYPE_SPECIAL     = 512,
	MJ_FAN_CHANGSHA_SIXIHU =   512 , //MJ_FAN_TYPE_SPECIAL   =  512 ,	//长沙四喜胡
	MJ_FAN_CHANGSHA_BANBANHU   = 513 ,				//长沙板板胡
	MJ_FAN_CHANGSHA_QUEYIMEN   = 514 ,				//长沙缺一门
	MJ_FAN_CHANGSHA_LIULIUSHUN  = 515 ,				//长沙六六顺	
	MJ_FAN_CHANGSHA_JIANGJIANGHU = 516 ,			//长沙将将胡 全是258 的碰碰胡
	MJ_FAN_CHENZHOU_SIZHONGHU     = 517 ,	
	MJ_FAN_GUANGDONG_QIANGGANGQUANBAO = 518,
	MJ_FAN_GUANGDONG_GANGBAOQUANBAO  = 519 ,
	MJ_FAN_GUIYANG_RUANBAO = 520 ,
	MJ_FAN_GUIYANG_YINGBAO  = 521 ,
	MJ_FAN_GUIYANG_SHABAO_10 =522,
	MJ_FAN_GUIYANG_SHABAO_20 =523 ,
	MJ_FAN_HUA_DIAO = 524 ,				//花单钓
	MJ_FAN_HUA_DIAO_HUA =525 ,			//花钓花
	MJ_FAN_HUA_2_HUA_DIAO_HUA =526 ,		//双花钓花
	MJ_FAN_HUA_3_DIAO_HUA =527,			//三花钓花	

	MJ_FAN_QIDUI_SHUANGLONG =528 ,		//双龙七对
	MJ_FAN_QIDUI_SANLONG =529,			//三龙七对
	MJ_FAN_QIDUI_QINGSHUANGLONG =530 ,//不要了//清双龙七对
	MJ_FAN_QIDUI_QINGSANLONG =531,//不要了	//清三龙七对
	MJ_FAN_HUIZHOU_BAOJIUQUANBAO =532 ,		//惠州：报九全包
	MJ_FAN_YUANJIANG_BAOTING = 533 ,
	MJ_FAN_OTHER_NOJOKER =534 ,					//其余玩家无鬼
	MJ_FAN_13LAN =535 ,							//13烂
	MJ_FAN_7XING13LAN =536 ,						//7星13烂	

	MJ_FAN_ZUNYI_BIAN_ZHANG = 537 ,
	MJ_FAN_ZUNYI_KA_ZHANG =538 ,	
	MJ_FAN_ZUNYI_DA_KUAN_ZHANG =539 ,
	MJ_FAN_ZUNYI_YUAN_QUE   = 540 ,			//遵义原缺（缺一门）
	MJ_FAN_ZUNYI_No_QUEYIMEN =541 ,		//遵义缺一门（未听牌）
	MJ_FAN_YUANJIANG_YOUXI = 542 ,
	MJ_FAN_CHANGSHA_JIANGYIZHIHUA =543 ,
	MJ_FAN_CHANGSHA_DANSEYIZHIHUA = 544 ,    // 32+12 // 544  一枝花 
	MJ_FAN_CHANGSHA_YIZHINIAO = 545 ,
	MJ_FAN_CHANGSHA_SANTONG = 546 ,   // 三同
	MJ_FAN_CHANGSHA_ZHONGTU_SIXI =547 ,
	MJ_FAN_CHANGSHA_JIEJIEGAO  = 548 ,

	MJ_FAN_HENGYANG_SANPENGLIANGGANG_QUANBAO =549 ,	

	MJ_FAN_BENXI_QIONG_XIAO =550,
	MJ_FAN_BENXI_QIANG_TOU_XIAO =551 ,
	MJ_FAN_BENXI_ZHA_QIANG_HU =552 ,
	MJ_FAN_BENXI_HUN = 553 ,
	MJ_FAN_BENXI_QIANG = 554 ,
	MJ_FAN_BENXI_QIONG_HU =555,	
	MJ_FAN_BENXI_JUE_TOU_HU = 556 ,

	MJ_FAN_NINGXIANG_JINTONGYUNV = 557 ,     // 金童玉女  557 
	MJ_FAN_NINGXIANG_YIDIANHONG  = 558 ,    //558  节节高 
	MJ_FAN_NINGXIANG_TIANTING  = 559 ,

	MJ_FAN_KANGPING_BAOTING = 560,   //
	MJ_FAN_KANGPING_3_JIA_MEN =561 ,
	MJ_FAN_KANGPING_DAN_DIAO_PENGPENG_HU = 562,

	MJ_FAN_HAO_HUA_QI_XIAO_DUI = 27 ,         //豪华旗下对
	MJ_FAN_HAO_HUA_DOUBLE_QI_XIAO_DUI = 528 ,  //豪华双七小队
	MJ_FAN_HAO_HUA_THREE_QI_XIAO_DUI =  529   ,  // 3豪华七小队
	
	MJ_FAN_BAOTING_ZIMO  = 566,   //报听自摸	
	MJ_FAN_BAOTING_JIE_PAO  =567 ,    //报听接炮 

	MJ_FAN_BAOTING_FANG_PAO  =  568 ,    // 报听。放炮
	MF_FAN_BAOTING_BEI_ZI_MO = 569,   // 报听被自摸。 

	MJ_FAN_YI_TIAO_LONG				= 570 ,  // 一条龙
	MJ_FAN_YA_ZI						=571   ,    // 8.丫子
	MJ_FAN_DUI_DAO_HU				=572  ,    // 9.对倒胡
	MJ_FAN_DA_JUE					= 573 ,  // 10.大绝
	MJ_FAN_WU_HUA_GUO				=574 ,   // 11.无花果
	MJ_FAN_SI_XI_XIAO_HUA_MAJIANG    =575 ,  // 12.四喜
	MJ_FAN_DUI_DUI_QING              = 576 ,   // 对对清 
	MJ_FAN_ZAOZAOHU                  = 577 ,   // 糟糟胡
	MJ_FAN_TUIDAO_HU                 = 578 ,   //推到胡 
	MJ_FAN_HUO_CHA                   = 579 ,   // 火叉 
	MJ_FAN_ALL_19                     =580 ,   //21.幺幺胡

	MJ_FAN_XI_LUAN                   = 581 ,   //   2.乱烂
	MJ_FAN_ZI_YI_SE                  =582 ,     // 字一色
	MJ_FAN_QI_XI_GUI_LIN             = 583 ,       // 七喜
	MJ_FAN_TYPE_QUE_YIMEN            = 584 ,       //缺一门 
	MJ_FAN_TYPE_BIAN_HU              = 585 ,    // 边胡。 
	MJ_FAN_TYPE_BI_MEN               = 586 ,    // 闭门  bao
	MJ_FAN_TYPE_MO_BAO               = 587 ,     //  摸宝  
	MJ_FAN_TYPE_TONG_BAO             = 588 ,    //  通宝  

	MJ_FAN_SAN_ZHANG_KING            = 589 ,      // 三正王
	MJ_FAN_SI_CHUN_kING              = 590 ,     // 四纯王 
	
	MJ_FAN_NO_KING_FAN              =  591   ,   //  无王番数。 
	

	MJ_FAN_TYPE_SPECIAL_MAX  =   600 ,

	
};
//cmdgame__scmahjong中已经定义


//换牌结构体
struct tagChangeCard
{
	_uint8 bCard[3];    //换的牌，每个人最多换三张
	_uint8 bLen;        //换牌张数
	tagChangeCard()
	{
		memset(bCard, 0, sizeof(bCard));
		bLen = 0;
	}
};

//操作状态
enum OPERATE_STAUS
{ 
	OPERATE_NULL               = 0,                  
	OPERATE_CHANGE_CARD        = 1,                  //换牌状态
	OPERATE_LACK_CARD          = 2,                  //定缺牌状态
	OPERATE_CARD               = 3,                  //出牌和操作牌状态
};

enum GANG_TYPE
{
	ZHI_GANG_INDEX = 0,
	WAN_GANG_INDEX = 1,
	AN_GANG_INDEX  = 2
};

struct GangScore
{  
	int         GangType;           //杠类型
	_tint64		llScore;		      //总分
};
struct GangInfo
{
	int			chairID;	          //玩家ID
	GangScore   sGangScore[3];    
};
struct ScoreInfo
{
	int chairID;
	_tint64 llScore;
};
/**
*刮风
*/
struct WindInfo 
{
	_uint8			cbWindNum;	      //刮风了几个玩家
	GangInfo		sScore[3];	      //被刮风玩家扣分信息
};

struct RainInfo
{
	_uint8			cbRainNum;	      //下雨了几个玩家
	GangInfo		sScore[3];	      //被下雨玩家扣分信息
};

struct UserPasGangInfo
{
	int      chairID;
	int      GangType;
	_uint8   cbCardData;
	_tint64  llScore;
};

struct GangUserInfo
{  
	int GangUserNum;
	UserPasGangInfo userPasGang[3];
	GangUserInfo()
	{
		GangUserNum = 0;
		memset(userPasGang, 0, sizeof(userPasGang));
	}
};

//杠牌记录
struct GangRecord
{   
	int GangNum;                     //杠个数
	GangUserInfo sGangInfo[3];
	GangRecord()
	{
		GangNum = 0;
		memset(sGangInfo, 0, sizeof(sGangInfo));
	}
};

struct OpTimeRecord
{
	_uint64 ullTime;
	int     nOpTeype;
	OpTimeRecord()
	{
		ullTime = 0;
		nOpTeype = 0;
	}
};

struct info     
{ 
	int chairID;                     //椅子ID
	int nFanType;                    //番种
	int nFanNum;                     //番数
	int nGenNum;                     //根的个数
};
//非花猪玩家信息
struct tagFlower 
{
	_uint8  cbNum;			  //花猪的人数
	info    flower[4];	      //花猪信息
};

//非大叫玩家信息
struct tagListen 
{
	_uint8  cbNum;					//大叫玩家人数
	info    Listen[4];	//大叫玩家的信息
};

struct tagFLScore
{
	int chairID;
	_tint64 llScore;      //查花猪，查大叫赢分数
	_tint64 llBakcScore;  //刮风下雨归还分数
	int nFanType;         //番种
	int nFanNum;          //番数
	int nGenNum;          //根个数
};

struct tagUserFlower      //玩家查谁花猪信息
{   
	_uint8 cbNum;
	tagFLScore sScore[GAME_PLAYER + 1];  //被查玩家分数信息 
};

struct tagUserListen      //玩家查谁大叫信息
{
	_uint8 cbNum;      
	tagFLScore sScore[GAME_PLAYER + 1];  
};

//结算胡牌信息
struct HuInfo 
{  
	_uint8  cbZiMo;                     //是否自摸
	_uint8	cbHuNum;					//玩家胡了几个人
	int	    ChairID[3];				    //玩家胡了谁
	_uint8	cbFanType;					//胡牌番种
	_uint8	cbFanNum;					//总番数(番数+杠数+根数+另加番)
	_uint8  cbCombType;                 //组合翻型
	int		nHuScore;					//分数
	_uint8	cbGangNum;					//杠的个数
	_uint8	cbGenNum;					//根的个数
	_uint8	cbAddFan;					//另加番
	_uint8  cbPasChairID;               //引起胡的玩家(自摸时为自己的ID)
	_uint8  cbHuCard;                   //引起胡的牌
	_uint8  HuType;                     //胡牌类型
	int     OutNum;                     //出牌手数
	int     TaskDoubleTimes;            //牌局任务倍数
};

//被胡的信息记录
struct HuRecord
{
	int	   ChairID;					//玩家椅子ID
	int	   nScore;						//分数
};
//被胡牌信息
struct PassiveHuInfo
{
	_uint8		cbPasHuNum;					//被几个玩家胡
	HuRecord	sPassiveHu[3];				//被那个玩家胡的信息记录
};

struct LengthInfo 
{
	_uint8 cbLength;
};

struct ChanceInfo
{
	_uint8	cbNomal0;
	_uint8	cbPingHu1;
	_uint8	cbDuiDuihu2;
	_uint8	cbQingyise3;
	_uint8	cbJiangdui4;
	_uint8	cbLongQidui5;
	_uint8	cbQLongQidui6;
	_uint8	cbDiHu7;
	_uint8	cbTianHu8;
};

struct TypeProbInfo
{
	int iProb;
	int extractIndex;
};
struct SuperChanceInfo
{
	TypeProbInfo	tagNomal0;
	TypeProbInfo	tagPingHu1;
	TypeProbInfo	tagDuiDuiHu2;
	TypeProbInfo	tagQingYiSe3;
	TypeProbInfo	tagDaiYaoJiu4;
	TypeProbInfo	tagQiDui5;
	TypeProbInfo	tagQingDaDui6;
	TypeProbInfo	tagJiangDui7;
	TypeProbInfo	tagLongQiDui8;
	TypeProbInfo	tagQingQiDui9;
	TypeProbInfo	tagQingYaoJiu10;
	TypeProbInfo	tagQLongQiDui11;
};

struct tagExtractInfo
{
	int iNum;
	int iProb;
	tagExtractInfo()
	{
		iNum = 0;
		iProb = 0;
	}
};

struct tagExtractList
{
	int index;
	std::map<_uint32,tagExtractInfo> Extract_list;
	tagExtractList()
	{
		index = 0;
	}
};
struct tagCardType
{
	_uint8 cbCardType;
	_uint8 cbNum;
	tagCardType()
	{
		cbCardType = 0;
		cbNum = 0;
	}
};

struct tagCardAnalysis
{
	_uint8 cbCardType;                //花色类型
	_uint8 cbTotalNum;                //本种花色总张数
	_uint8 cbSingleNum;               //单张个数
	_uint8 cbDuiziNum;                //对子个数
	_uint8 cbThreeNum;                //三张个数
	_uint8 cbFourNum;                 //四张个数
	_uint8 cbSingle[13];
	_uint8 cbDuizi[13];
	_uint8 cbThree[13];
	_uint8 cbFour[13];
	tagCardAnalysis()
	{
		cbCardType = 0;
		cbTotalNum = 0;
		cbSingleNum = 0;
		cbDuiziNum = 0;
		cbThreeNum = 0;
		cbFourNum = 0;
	}
};

struct tagTaskType
{   
	int TaskType;             //任务类型
	int RewardType;           //奖励类型
	int PropID;               //奖励的道具ID
	int RewardNum;            //奖励数量
	char TaskDesc[256];      //奖励描述
	tagTaskType()
	{  
		RewardType  = 0;
		RewardNum   = 0;
		PropID      = 0;
		memset(TaskDesc, 0, sizeof(TaskDesc));
	}
};

struct tagCardTask
{
	tagTaskType sTaskType[50];
	int nCount;
	tagCardTask()
	{
		nCount = 0;
		memset(sTaskType, 0, sizeof(sTaskType));
	};
};


class Fantype_to_FanNum
{
protected:
	_uint8 fan_num[MJ_FAN_TYPE_SPECIAL_MAX];   // debug ..  大问题。。 

	_uint8 special_fan_num[MJ_FAN_TYPE_SPECIAL_MAX ];
public:
	Fantype_to_FanNum()
	{
		memset(fan_num, 0, sizeof(fan_num));
		memset(special_fan_num,  8 , sizeof(special_fan_num));
	}
	virtual ~Fantype_to_FanNum()
	{

	}
	virtual  bool FanFilter_Guilin(const MJ_fan_buffer& oFan, MJ_fan_buffer& oOutFan, MJ_fan_buffer  &OtherFan)
	{
		return true;
	}
	virtual bool FanFilter(const MJ_fan_buffer& oFan, MJ_fan_buffer& oOutFan)
	{
		return false;
	}
	virtual int operator[](int fan)const
	{
		return GetFan(fan);
	}
	virtual int GetFan(int fan  )const
	{
		if (fan < MJ_FAN_TYPE_MAX)
		{
			return fan_num[fan];
		}
		else if (fan < MJ_FAN_TYPE_SPECIAL_MAX && fan >= MJ_FAN_TYPE_SPECIAL)
		{
			return special_fan_num[fan - MJ_FAN_TYPE_SPECIAL];
		}
		throw("out of range!\n");
		return 0;
	}
	virtual bool SetFan(int iFanType, _uint32 iFanNum)
	{		
		if (iFanType < MJ_FAN_TYPE_MAX)
		{
			fan_num[iFanType] = iFanNum;
			return true;
		}
		else if (iFanType >= MJ_FAN_TYPE_SPECIAL && iFanType < MJ_FAN_TYPE_SPECIAL_MAX)
		{
			special_fan_num[iFanType - MJ_FAN_TYPE_SPECIAL] = iFanNum;
			return true;
		}
		return false;
	}
};
#endif
