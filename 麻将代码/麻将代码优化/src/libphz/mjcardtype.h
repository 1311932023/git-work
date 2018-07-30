/********************************************************************
 *     麻将胡牌番数定义
*********************************************************************/
#ifndef __MAHJONG_DEF_H__
#define __MAHJONG_DEF_H__
#include "common.h"

#define  BIAN_DAN	3
#define  DUI_ZI		2
#define  BIAN_KAN	1
#define  SINGLE		0

#define LOOPNUM   5
#define MJ_CARDS_STYLE_LEN (34)	// 3 * 9 + 7


template<typename TYPE>
const TYPE& MAX(const TYPE& lhs, const TYPE& rhs)
{
	return lhs > rhs ? lhs : rhs;
}

template<typename TYPE>
const TYPE& MIN(const TYPE& lhs, const TYPE& rhs)
{
	return lhs < rhs ? lhs : rhs;
}

template<class TYPE, _uint32 SIZE = MJ_CARDS_STYLE_LEN>
class MJ_BUFFER
{
public:
	MJ_BUFFER() : len(0)
	{
	}
	void Add(const TYPE& uiCard)
	{
		if (len >= SIZE)
			throw("out of range!\n");
		cards[len++] = uiCard;
	}
	void Add(const TYPE* uiCard, _uint32 size)
	{
		if (len + size >= sizeof(cards) / sizeof(cards[0]))	
			throw("out of range!\n");
		
		for (_uint32 i = 0; i < size;i++)		
			Add(uiCard[i]);
		
	}
	bool Find(const TYPE& other)const
	{
		for (_uint32 i = 0; i < len; i++)
		{
			if (cards[i] == other)	return true;
		}
		return false;
	}
	const TYPE& operator[](_uint32 pos) const
	{
		if (pos >= len)		
			throw("out of range!\n");
		
		return cards[pos];
	}	
	TYPE& operator[](_uint32 pos)
	{
		if (pos >= len)
			throw("out of range!\n");

		return cards[pos];
	}
	int Size() const { return len; }
	bool IsNull()const{ return len == 0; }
	bool NotNull()const{ return len != 0; }
	const TYPE& Last()const
	{
		if (len == 0)
			throw("buffer is empty!\n");
		return cards[len - 1];
	}		
	void Clear() { len = 0; }
	bool RemoveLast() { if (len != 0) len--; return true; }
	bool RemoveLast(const TYPE& other)
	{
		if (IsNull())
		{
			return false;
		}
		if (Last() == other)
		{
			return RemoveLast();
		}
		if (len == 1)
		{
			return false;
		}			
		for (int i = len - 2; i >= 0; i--)
		{
			if (cards[i] == other)
			{
				memmove(&cards[i], &cards[i + 1], (--len - i) * sizeof(TYPE));
				return true;
			}
		}	
		return false;
	}
	bool Remove(const TYPE& other)
	{
		if (IsNull())
		{
			return false;
		}		
		_uint32 tmp = len - 1;
		for (int i = 0; i < (int)tmp; i++)
		{
			if (cards[i] == other)
			{
				memmove(&cards[i], &cards[i + 1], (tmp - i) * sizeof(TYPE));
				len--;
				return true;
			}
		}
		if (Last() == other)
		{
			return RemoveLast();
		}
		return false;
	}
	void SortAscend()
	{
		for (_uint32 i = 0; i < len; i++)
		{
			for (_uint32 j = i + 1; j < len; j++)
			{
				if (cards[i] > cards[j])
				{
					TYPE t = cards[i];
					cards[i] = cards[j];
					cards[j] = t;
				}
			}
		}		
	}
	void operator=(const MJ_BUFFER<TYPE, SIZE>& other)
	{
		len = other.len;
		for (int i = 0; i < (int)len; i ++)
		{
			cards[i] = other[i];
		}
	}
	void operator+=(const MJ_BUFFER<TYPE, SIZE>& other)
	{
		_uint32 iCount = MIN(SIZE - len, other.len);		
		for (_uint32 i = 0; i < iCount; i++)
			Add(other[i]);
	}
	int Counts(const TYPE& other)const
	{
		int iCount = 0;
		for (_uint32 i = 0; i < len; i++)
		{
			if (cards[i] == other)
			{
				iCount++;
			}
		}
		return iCount;
	}
private:
	_uint32 len;
	TYPE cards[SIZE];
};

struct MJ_win_pattern
{
	_uint8 jiang;			//??
	_uint8 kan[HAND_CARD_KAN_MAX][4];		//??
};


typedef MJ_BUFFER<_uint8>				MJ_cards_type_buffer;
typedef MJ_BUFFER<_uint8, 18>			MJ_hand_buffer;
typedef MJ_BUFFER<_uint32, 40>			MJ_fan_buffer;
typedef MJ_BUFFER<_uint8, GAME_PLAYER>	MJ_user_buffer;
typedef MJ_BUFFER<_uint8, 80>			ShowCards;
typedef MJ_BUFFER<_uint8, 5>			MJ_peng;
typedef MJ_BUFFER<_uint8, 5>			MJ_gang;


/**
 * 番类型ID定义
 */
enum MJFanType
{	
	MJ_FAN_TYPE_NULL = 0,
	MJ_FAN_TYPE_JIULIANBAODENG = 1,				//九莲宝灯
	MJ_FAN_TYPE_DASIXI = 2,						//大四喜
	MJ_FAN_TYPE_XIAOSIXI = 3,					//小四喜
	MJ_FAN_TYPE_DASANYUAN = 4,					//大三元 
	MJ_FAN_TYPE_SIANKE = 5,						//四暗刻
	MJ_FAN_TYPE_SHIBALUOHAN = 6,				//十八罗汉
	MJ_FAN_TYPE_QUANYAOJIU = 7,					//全幺九	
	MJ_FAN_TYPE_ZIYISE = 8,						//字一色
	MJ_FAN_TYPE_XIAOSANYUAN = 9,				//小三元
	MJ_FAN_TYPE_QINGYAOJIU = 10,				//混幺九,清幺九
	MJ_FAN_TYPE_QINGDUI = 11,					//清对 清一色 + 碰碰胡	
	MJ_FAN_TYPE_QINGPENG = 12,					//清碰， 清一色 + 碰碰胡
	MJ_FAN_TYPE_HUNPENG = 13,					//混碰, 混一色 + 碰碰胡
	MJ_FAN_TYPE_QINGYISE = 14,					//清一色
	MJ_FAN_TYPE_HUNYISE = 15,					//混一色
	MJ_FAN_TYPE_PENGPENGHU = 16,				//碰碰胡		
	MJ_FAN_TYPE_PINGHU = 17,					//平胡	
	MJ_FAN_TYPE_JIHU = 18,						//鸡胡	
	MJ_FAN_TYPE_TIANHU = 19,					//天胡
	MJ_FAN_TYPE_DIHU = 20,						//地胡
	MJ_FAN_TYPE_RENHU = 21,						//人胡
	MJ_FAN_TYPE_QIANGGANGHU = 22,				//抢杠胡
	MJ_FAN_TYPE_HAIDILAOYUE = 23,				//海底捞月
	MJ_FAN_TYPE_GANGSHANGHUA = 24,				//杠上开花
	MJ_FAN_TYPE_QINGQIDUI = 25,					//清七对	
	MJ_FAN_TYPE_QINGLONGQIDUI = 26,				//清龙七对 
	MJ_FAN_TYPE_LONGQIDUI = 27,					//龙七对
	MJ_FAN_TYPE_QUANQIUREN = 28,				//全求人
	MJ_FAN_TYPE_GANGHOUPAO = 29,				//杠上炮
	MJ_FAN_TYPE_MENQIANQING = 30,				//门前清
	MJ_FAN_TYPE_FENGYISE = 31,					//风一色
	MJ_FAN_TYPE_MENQINGQINGYISE = 32,			//门前清 + 清一色
	MJ_FAN_TYPE_QINGSIANKE = 33,				//清一色 + 四暗刻
	MJ_FAN_TYPE_HAIDIPAO = 34,					//海底炮
	MJ_FAN_TYPE_13YAO = 35,						//十三幺	
	MJ_FAN_TYPE_QIDUI = 36,						//七对		
	MJ_FAN_TYPE_DAIYAOJIU = 37,					//带幺九	
	MJ_FAN_TYPE_JIANGDUI = 38,					//将对, 258做将
	MJ_FAN_TYPE_YITIAOLONG = 39,				//一条龙
	MJ_FAN_TYPE_QINGYITIAOLONG = 40,			//清一条龙
	MJ_FAN_TYPE_4JOKER = 41,					//4鬼胡
	MJ_FAN_TYPE_NOJOKER = 42,					//无鬼
	MJ_FAN_TYPE_QINGQUANQIUREN = 43,			//清全求人
	MJ_FAN_TYPE_DANDIAO = 44,					//单吊
	MJ_FAN_TYPE_MINGSIGUI = 45,					//明四归
	MJ_FAN_TYPE_ANSIGUI = 46,					//暗四归
	MJ_FAN_TYPE_JIANGYISE = 47,					//将一色
	MJ_FAN_TYPE_ZIMO = 48,                      //自摸

	MJ_FAN_TYPE_MAX = 49,

	MJ_FAN_TYPE_SPECIAL = 512,
	MJ_FAN_CHANGSHA_SIXIHU = 513,				//长沙四喜胡
	MJ_FAN_CHANGSHA_BANBANHU = 514,				//长沙板板胡
	MJ_FAN_CHANGSHA_QUEYIMEN = 515,				//长沙缺一门
	MJ_FAN_CHANGSHA_LIULIUSHUN = 516,			//长沙六六顺	
	MJ_FAN_CHANGSHA_JIANGJIANGHU = 517,			//长沙将将胡 全是258 的碰碰胡
	MJ_FAN_CHENZHOU_SIZHONGHU = 518,	
	MJ_FAN_GUANGDONG_QIANGGANGQUANBAO = 519,
	MJ_FAN_GUANGDONG_GANGBAOQUANBAO = 520,
	MJ_FAN_GUIYANG_RUANBAO = 521,
	MJ_FAN_GUIYANG_YINGBAO = 522,
	MJ_FAN_GUIYANG_SHABAO_10 = 523,
	MJ_FAN_GUIYANG_SHABAO_20 = 524,
	MJ_FAN_HUA_DIAO = 525,				//花单钓
	MJ_FAN_HUA_DIAO_HUA = 526,			//花钓花
	MJ_FAN_HUA_2_HUA_DIAO_HUA = 527,	//双花钓花
	MJ_FAN_HUA_3_DIAO_HUA = 528,		//三花钓花	

	MJ_FAN_QIDUI_SHUANGLONG = 529,		//双龙七对(超豪华七对)
	MJ_FAN_QIDUI_SANLONG = 530,			//三龙七对（非凡七对）
	MJ_FAN_QIDUI_QINGSHUANGLONG = 531,	//清双龙七对
	MJ_FAN_QIDUI_QINGSANLONG = 532,		//清三龙七对
	MJ_FAN_HUIZHOU_BAOJIUQUANBAO = 533,	//惠州：报九全包
	MJ_FAN_TYPE_KAWUXING = 534,			//襄阳卡五星
	MJ_FAN_TYPE_SHOWCARDS = 535,		//襄阳亮牌
	MJ_FAN_TYPE_LUANJIANGHU = 536,		//乱将胡
	MJ_FAN_TYPE_13LUAN = 537,		    //13乱
	MJ_FAN_TYPE_13FENG = 538,		    //13风
	MJ_FAN_TYPE_JIANGJIANGHU = 539,		//将将胡
	MJ_FAN_TYPE_SPECIAL_MAX = 540,
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


//非大叫玩家信息
struct tagListen 
{
	_uint8  cbNum;					//大叫玩家人数
	info    Listen[4];				//大叫玩家的信息
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
/*
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
*/

class Fantype_to_FanNum
{
protected:
	_uint8 fan_num[MJ_FAN_TYPE_MAX];
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
	virtual bool FanFilter(const MJ_fan_buffer& oFan, MJ_fan_buffer& oOutFan)
	{
		return false;
	}
	virtual int operator[](int fan)const
	{
		return GetFan(fan);
	}
	virtual int GetFan(int fan)const
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
