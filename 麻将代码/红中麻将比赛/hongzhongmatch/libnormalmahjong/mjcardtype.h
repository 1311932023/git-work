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
	MJ_FAN_TYPE_NULL,
	MJ_FAN_TYPE_JIULIANBAODENG,				//九莲宝灯
	MJ_FAN_TYPE_DASIXI,						//大四喜
	MJ_FAN_TYPE_XIAOSIXI,					//小四喜
	MJ_FAN_TYPE_DASANYUAN,					//大三元 
	MJ_FAN_TYPE_SIANKE,						//四暗刻
	MJ_FAN_TYPE_SHIBALUOHAN,				//十八罗汉
	MJ_FAN_TYPE_QUANYAOJIU,					//全幺九	
	MJ_FAN_TYPE_ZIYISE,						//字一色
	MJ_FAN_TYPE_XIAOSANYUAN,				//小三元
	MJ_FAN_TYPE_QINGYAOJIU,					//混幺九,清幺九
	MJ_FAN_TYPE_QINGDUI,					//清对 清一色 + 碰碰胡	
	MJ_FAN_TYPE_QINGPENG,					//清碰， 清一色 + 碰碰胡
	MJ_FAN_TYPE_HUNPENG,					//混碰, 混一色 + 碰碰胡
	MJ_FAN_TYPE_QINGYISE,					//清一色
	MJ_FAN_TYPE_HUNYISE,					//混一色
	MJ_FAN_TYPE_PENGPENGHU,					//碰碰胡		
	MJ_FAN_TYPE_PINGHU,						//平胡	
	MJ_FAN_TYPE_PAOHU = MJ_FAN_TYPE_PINGHU,	//炮胡
	MJ_FAN_TYPE_JIHU,						//鸡胡	
	MJ_FAN_TYPE_ZIMO = MJ_FAN_TYPE_JIHU,	//自摸
	MJ_FAN_TYPE_TIANHU,						//天胡
	MJ_FAN_TYPE_DIHU,						//地胡
	MJ_FAN_TYPE_RENHU,						//人胡
	MJ_FAN_TYPE_QIANGGANGHU,				//抢杠胡
	MJ_FAN_TYPE_HAIDILAOYUE,				//海底捞月
	MJ_FAN_TYPE_GANGSHANGHUA,				//杠上开花
	MJ_FAN_TYPE_QINGQIDUI,					//清七对	
	MJ_FAN_TYPE_QINGLONGQIDUI,				//清龙七对 
	MJ_FAN_TYPE_LONGQIDUI,					//龙七对
	MJ_FAN_TYPE_QUANQIUREN,					//全求人
	MJ_FAN_TYPE_GANGHOUPAO,					//杠上炮
	MJ_FAN_TYPE_MENQIANQING,				//门前清
	MJ_FAN_TYPE_FANYISE,					//风一色
	MJ_FAN_TYPE_MENQINGQINGYISE,			//门前清 + 清一色
	MJ_FAN_TYPE_QINGSIANKE,					//清一色 + 四暗刻
	MJ_FAN_TYPE_HAIDIPAO,					//海底炮
	MJ_FAN_TYPE_13YAO,						//十三幺	
	MJ_FAN_TYPE_QIDUI,						//七对		
	MJ_FAN_TYPE_DAIYAOJIU,					//带幺九	
	MJ_FAN_TYPE_JIANGDUI,					//将对, 258做将
	MJ_FAN_TYPE_YITIAOLONG,					//一条龙
	MJ_FAN_TYPE_QINGYITIAOLONG,				//清一条龙
	MJ_FAN_TYPE_4JOKER,						//4鬼胡
	MJ_FAN_TYPE_NOJOKER,					//无鬼
	MJ_FAN_TYPE_QINGQUANQIUREN,				//清全求人
	MJ_FAN_TYPE_ZHUANGXIAN,					//庄家
	MJ_FAN_TYPE_GANG,						//杠
	MJ_FAN_TYPE_MAX,

	MJ_FAN_TYPE_SPECIAL = 512,
	MJ_FAN_CHANGSHA_SIXIHU = MJ_FAN_TYPE_SPECIAL,	//长沙四喜胡
	MJ_FAN_CHANGSHA_BANBANHU,				//长沙板板胡
	MJ_FAN_CHANGSHA_QUEYIMEN,				//长沙缺一门
	MJ_FAN_CHANGSHA_LIULIUSHUN,				//长沙六六顺	
	MJ_FAN_CHANGSHA_JIANGJIANGHU,			//长沙将将胡 全是258 的碰碰胡
	MJ_FAN_CHENZHOU_SIZHONGHU,	
	MJ_FAN_YUANJIANG_MAMAHU = MJ_FAN_CHENZHOU_SIZHONGHU,
	MJ_FAN_GUANGDONG_QIANGGANGQUANBAO,
	MJ_FAN_GUANGDONG_GANGBAOQUANBAO,
	MJ_FAN_GUIYANG_RUANBAO,
	MJ_FAN_GUIYANG_YINGBAO,
	MJ_FAN_GUIYANG_SHABAO_10,
	MJ_FAN_GUIYANG_SHABAO_20,
	MJ_FAN_HUA_DIAO,				//花单钓
	MJ_FAN_HUA_DIAO_HUA,			//花钓花
	MJ_FAN_HUA_2_HUA_DIAO_HUA,		//双花钓花
	MJ_FAN_HUA_3_DIAO_HUA,			//三花钓花	

	MJ_FAN_QIDUI_SHUANGLONG,		//双龙七对
	MJ_FAN_QIDUI_SANLONG,			//三龙七对
	MJ_FAN_QIDUI_QINGSHUANGLONG,//不要了//清双龙七对
	MJ_FAN_QIDUI_QINGSANLONG,//不要了	//清三龙七对
	MJ_FAN_HUIZHOU_BAOJIUQUANBAO,		//惠州：报九全包
	MJ_FAN_YUANJIANG_BAOTING,
	MJ_FAN_OTHER_NOJOKER,					//其余玩家无鬼
	MJ_FAN_13LAN,							//13烂
	MJ_FAN_7XING13LAN,						//7星13烂	

	MJ_FAN_ZUNYI_BIAN_ZHANG,
	MJ_FAN_ZUNYI_KA_ZHANG,	
	MJ_FAN_ZUNYI_DA_KUAN_ZHANG,
	MJ_FAN_ZUNYI_YUAN_QUE,			//遵义原缺（缺一门）
	MJ_FAN_ZUNYI_No_QUEYIMEN,		//遵义缺一门（未听牌）
	MJ_FAN_YUANJIANG_YOUXI,
	MJ_FAN_CHANGSHA_JIANGYIZHIHUA,
	MJ_FAN_CHANGSHA_DANSEYIZHIHUA,
	MJ_FAN_CHANGSHA_YIZHINIAO,
	MJ_FAN_CHANGSHA_SANTONG,
	MJ_FAN_CHANGSHA_ZHONGTU_SIXI,
	MJ_FAN_CHANGSHA_JIEJIEGAO,
	MJ_FAN_CHANGSHA_ZHONGTU_66SHUN,			//中途66顺

	MJ_FAN_HENGYANG_SANPENGLIANGGANG_QUANBAO = MJ_FAN_CHANGSHA_ZHONGTU_66SHUN,

	MJ_FAN_BENXI_QIONG_XIAO,
	MJ_FAN_BENXI_QIANG_TOU_XIAO,
	MJ_FAN_BENXI_ZHA_QIANG_HU,
	MJ_FAN_BENXI_HUN,
	MJ_FAN_BENXI_QIANG,
	MJ_FAN_BENXI_QIONG_HU,	
	MJ_FAN_BENXI_JUE_TOU_HU,

	MJ_FAN_NINGXIANG_JINTONGYUNV,
	MJ_FAN_NINGXIANG_YIDIANHONG,
	MJ_FAN_NINGXIANG_TIANTING,

	MJ_FAN_KANGPING_BAOTING,
	MJ_FAN_KANGPING_MEN,
	MJ_FAN_KANGPING_3_JIA_MEN,
	MJ_FAN_KANGPING_DAN_DIAO_PENGPENG_HU,
	MJ_FAN_KANGPING_LIULEI,
	MJ_FAN_KANGPING_YIGEJIAO,
	MJ_FAN_KANGPING_SIGUIYI,
	MJ_FAN_KANGPING_ANJIAN,
	MJ_FAN_KANGPING_CAIGANG,

	//九江
	MJ_FAN_JIUJIANG_DANDIAO,
	MJ_FAN_JIUJIANG_HAOHUAQIDUI,
	MJ_FAN_JIUJIANG_SHUANGHAOHUAQIDUI,
	MJ_FAN_JIUJIANG_HAOHUAQIDUIDAIGUN1,
	MJ_FAN_JIUJIANG_HAOHUAQIDUIDAIGUN2,
	MJ_FAN_JIUJIANG_HAOHUAQIDUIDAIGUN3,
	MJ_FAN_JIUJIANG_XIAOSAQIDUI,
	MJ_FAN_JIUJIANG_SHUANGXIAOSAQIDUI,
	MJ_FAN_JIUJIANG_XIAOSAQIDUIDAIGUN1,
	MJ_FAN_JIUJIANG_XIAOSAQIDUIDAIGUN2,
	MJ_FAN_JIUJIANG_XIAOSAQIDUIDAIGUN3,
	MJ_FAN_JIUJIANG_QIXIANNV,
	MJ_FAN_JIUJIANG_HONGQIPIAOPIAO,	
	MJ_FAN_JIUJIANG_DAHU,

	//桃江
	MJ_FAN_TAOJIANG_TIANTIANHU_DAIPING,
	MJ_FAN_TAOJIANG_TIANHU_DAIPING,
	MJ_FAN_TAOJIANG_DIDIHU_DAIPING,
	MJ_FAN_TAOJIANG_DIHU_DAIPING,
	MJ_FAN_TAOJIANG_TIANTIANHU,
	MJ_FAN_TAOJIANG_DIDIHU,		
	MJ_FAN_TAOJIANG_TIANHU,

	//东乡麻将
	MJ_FAN_DX_QINGYISE_QIDUI,
	MJ_FAN_DX_QINGYISE_JIAHU,
	MJ_FAN_DX_QINGYISE_ZHENHU,
	MJ_FAN_DX_SHENYISE = MJ_FAN_DX_QINGYISE_ZHENHU,

	MJ_FAN_YIYANG_DAJIAO,


	MJ_FAN_DX_4GUI1,
	MJ_FAN_DX_8GUI1,
	
	MJ_FAN_SHANGHAI_WUHUA,

	//道州麻将
	MJ_FAN_DZ_MENQING,						//门清
	MJ_FAN_DZ_YOU1TAO,						//有一套
	MJ_FAN_DZ_YOU2TAO,						//有两套
	MJ_FAN_DZ_4GUI1,						//4归1
	MJ_FAN_DZ_1TIAOLONG,					//1条龙
	MJ_FAN_DZ_XIAODAO,						//小道
	MJ_FAN_DZ_DADAO,						//大道
	MJ_FAN_DZ_13LANG,						//13浪
	MJ_FAN_DZ_13LANG_WANGGUIWEI,			//13浪王归位
	MJ_FAN_DZ_13LANG_7FENG,					//13浪7风归位
	MJ_FAN_DZ_13LANG_7FENG_WANGGUIWEI,		//13浪7风到位王归位
	MJ_FAN_DZ_DANWANGZHUA,					//单王爪
	MJ_FAN_DZ_DANWANGZHUAWANG,				//单王爪王
	MJ_FAN_DZ_2WANGZHUA,					//双王爪
	MJ_FAN_DZ_2WANGZHUAWANG,				//双王爪王
	MJ_FAN_DZ_4WANGDAOWEI,					//4王到位
	MJ_FAN_DZ_DANWANGGUIWEI,				//单王归位
	MJ_FAN_DZ_WANGGUIWEI,					//王归位
	MJ_FAN_DZ_2WANGGUIWEI,					//双王归位

	//血战
	MJ_FAN_SZ_JIANGQIDUI,					//将7对
	MJ_FAN_SZ_ZHONGZHANG,					//中张
	MJ_FAN_SZ_GEN,							//根
	MJ_FAN_SZ_DANDIAO,						//金钩钓
	MJ_FAN_SZ_GEN2,							//根2
	MJ_FAN_SZ_GEN3,							//根3
	MJ_FAN_SZ_GEN4,							//根4

	//博慧庄
	MJ_FAN_BHZ_ZIMO,						//自摸
	MJ_FAN_BHZ_BEIZIMO,						//被自摸
	MJ_FAN_BHZ_JIHU,						//鸡胡
	MJ_FAN_BHZ_DIANPAO,						//点炮
	MJ_FAN_BHZ_ZHONGMA,						//中码
	MJ_FAN_BHZ_SHUMA,						//输马
	MJ_FAN_BHZ_ANGANG,						//暗杠
	MJ_FAN_BHZ_BEIANGANG,					//被暗杠
	MJ_FAN_BHZ_MINGGANG,					//明杠
	MJ_FAN_BHZ_BEIMINGGANG,					//被明杠
	MJ_FAN_BHZ_JIEGANG,						//接杠
	MJ_FAN_BHZ_FANGGANG,					//放杠
	MJ_FAN_BHZ_QIANGGANGHU,					//抢杠胡
	MJ_FAN_BHZ_BEIQIANGGANGHU,				//被抢杠胡
	MJ_FAN_BHZ_BUHUA,						//补花





	MJ_FAN_TYPE_SPECIAL_MAX,	
	
};
//cmdgame__scmahjong中已经定义
/*enum Hu_Type
{
    HU_NOMAL              = 0,                  //普通胡牌
	HU_ZIMO               = 1,                  //自摸
	HU_QIANG_GANG         = 2,                  //抢杠胡
	HU_GANG_SHANG_PAO     = 3,                  //杠上炮
	HU_GANG_SHANG_HUA     = 4,                  //杠上花
	HU_HAI_DI_NAO         = 5,                  //海底捞
	HU_HAI_DI_PAO         = 6,                  //海底炮
	HU_GANG_HU_HAI_DI_NAO = 7,                  //杠上花海底捞
	HU_HAI_DI_GANG_PAO    = 8,                  //海底杠上炮
};

enum Comb_Type
{
	COMB_NOMAL            = 0,                  //普通牌型
    COMB_TIANHU           = 1,                  //天胡
	COMB_DIHU             = 2,                  //地胡
	COMB_JIN_GOU_DIAO     = 3,                  //金钩钓

};*/

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
	_uint8 fan_num[MJ_FAN_TYPE_MAX];
private:
	_uint8 special_fan_num[MJ_FAN_TYPE_SPECIAL_MAX - MJ_FAN_TYPE_SPECIAL];
public:
	Fantype_to_FanNum()
	{
		memset(fan_num, 0, sizeof(fan_num));
		memset(special_fan_num, 0, sizeof(special_fan_num));
	}
	virtual ~Fantype_to_FanNum()
	{

	}
	virtual int operator[](int fan)const
	{
		return GetFan(fan);
	}
	int GetFan(int fan)const
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
	bool SetFan(int iFanType, _uint32 iFanNum)
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
