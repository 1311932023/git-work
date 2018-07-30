/***********************************************************
 * File Name          :       common.h
 * Author             :       
 * Version            :       1.0
 * Date               :       2016-05-22 14:17
 * Description        :       公共接口
***********************************************************/

#ifndef _COMMON_H_
#define _COMMON_H_



#include <time.h>
#include <map>
#include <string.h>
#include <assert.h>
#include "to_char.h"
//数据类型定义
typedef          char        _tint8;     //有符号 1 字节
typedef unsigned char        _uint8;     //无符号 1 字节
typedef short                _tint16;    //有符号 2 字节
typedef unsigned short       _uint16;    //无符号 2 字节
typedef int                  _tint32;    //有符号 4 字节
typedef unsigned int         _uint32;    //无符号 4 字节
typedef long long            _tint64;    //有符号 8 字节
typedef unsigned long long   _uint64;    //无符号 8 字节

#define GPS_STATE_UNUPDATED         0       //GPS未更新
#define GPS_STATE_UPDATED           1       //GPS已更新

#define GAME_PLAYER					4				//游戏人数

#define INVALID_MJ_CARD 0xFF
#define NUM_MASK		0x0F
#define TYPE_MASK		0xF0
#define INVALID_FAN		-1

#define INVALID_CHAIR  				255	 								//无效座位号
#define INVALID_CARD				0xFF								//无效牌

#define INVALID_CHAIRID 0xFF
#define INVALID_MJ_CARD 0xFF
#define CARD_0			(0)
//常量定义
#define MAX_WEAVE					4				//最大组合
#define MAX_COUNT					14			    //最大数目
#define MAX_REPERTORY				108			    //最大库存
#define POOL_CARD_MAX				150				//最大牌数目
#define POOL_CARD_TYPE_MAX			42				//最大牌的种类
#define MAX_LAIZI_COUNT				(8)				//最大癞子数
#define MJ_HONG_ZHONG				(0x41)
#define MJ_FA_CAI					(0x42)
#define MJ_BAI_BAN					(0x43)

#ifdef MJ_HAND_CARD_17
#define HAND_CARD_SIZE_MAX			(17)
#define HAND_CARD_KAN_MAX			(5)
#else
#define HAND_CARD_SIZE_MAX			(14)
#define HAND_CARD_KAN_MAX			(4)
#endif

#define MJ_YAO_JI					((_uint8)0x11)
#define MJ_BA_TONG					((_uint8)0x28)
#define MJ_1_WAN					((_uint8)0x01)
#define MJ_1_TONG					((_uint8)0x21)
#define MJ_FENG					    ((_uint8)0x31)
#define MJ_JIAN					    ((_uint8)0x41)


#define TYPE_NULL			0x00								//没有类型
#define TYPE_PENG			0x01								//碰牌类型
#define TYPE_ZHIGANG		0x02								//直杠牌类型
#define TYPE_WANGANG        0x04                                //弯杠
#define TYPE_ANGANG         0x08                                //暗杠
#define TYPE_GANG			(TYPE_ZHIGANG | TYPE_WANGANG | TYPE_ANGANG)
#define TYPE_HU			    0x10								//吃胡类型
#define TYPE_LISTEN			0x20								//听牌类型
#define TYPE_LEFT_CHI	    0x40								//左吃类型
#define TYPE_CENTER_CHI		0x80								//中吃类型
#define TYPE_RIGHT_CHI		0x100								//右吃类型
#define TYPE_CHI			(TYPE_LEFT_CHI | TYPE_RIGHT_CHI | TYPE_CENTER_CHI)
#define TYPE_HUA_HU			0x200								//花胡
#define TYPE_BU_ZHANG		0x400								//补张
#define TYPE_GEN_ZHANG		0x400								//广东跟庄
#define TYPE_HAIDI_YAO		0x800								//海底牌（要）
#define TYPE_SUODING		0x1000								//锁定手牌
#define TYPE_NO_OPERATE		0x2000								//不做操作，必须要有个值

#define TYPE_LAIZI_GANG	 	0x10000								//赖子杠
#define TYPE_HONGZHONG_GANG	0x20000								//红中杠
#define TYPE_FACAI_GANG	 	0x40000								//发财杠
#define TYPE_LAIZIPI_GANG	0x80000								//赖子皮杠

#define TYPE_PASS			0xFF								//过
#define TYPE_CHI_PENG		(TYPE_CHI | TYPE_PENG)
#define TYPE_CHI_PENG_GANG	(TYPE_CHI | TYPE_PENG | TYPE_GANG)
#define TYPE_PENG_GANG		(TYPE_PENG | TYPE_GANG)

//萍乡多麻将种类合并
#define PLAY_MODE_PINGXIANGZZ           0
#define PLAY_MODE_PINGXIANGHZ           1
#define PLAY_MODE_PINGXIANG258          2

#define ZHONG_NIAO_DOUBLE              3                //中鸟翻倍


const int MJ_TYPE_NUM = 3; //麻将牌型共三种:万,条,筒
enum MJ_TYPE
{
	MJ_TYPE_WAN = 0,		//万,0-8,各4张，共36张
	MJ_TYPE_TIAO = 1,		//条,0-8,各4张，共36张
	MJ_TYPE_SUO = 1,
	MJ_TYPE_TONG = 2,		//筒,0-8,各4张，共36张
	MJ_TYPE_BING = 2,
	MJ_TYPE_FENG = 3,
	MJ_TYPE_JIAN = 4,	
	MJ_TYPE_FLOWER = 5,
	MJ_TYPE_SEASION = 5,
	MJ_TYPE_COMMON = 6,		//万条筒风箭
	MJ_TYPE_INVALID = -1,
};

#define CardNum(c) ((c) & NUM_MASK)
#define CardType(c) (((c) & TYPE_MASK)>>4)
#define CardNumX(card) ((card >= MJ_HONG_ZHONG && card <= MJ_BAI_BAN) ? (CardNum(card) + 4) : CardNum(card))
#define CardValue(t,n) ( (t<<4) | (n) )

struct FixedCard
{    
    int    state;			//碰杠类型
	_uint8    chairID;          //引起操作的椅子ID(主要是为了记录碰、直杠谁的牌)
	union 
	{
		_uint8 CardData; //碰杠的牌
		_uint8 c[3];	//吃的牌
	};
	
	FixedCard()
	{		
		state = TYPE_NULL;
		chairID = INVALID_CHAIRID;
		CardData = INVALID_MJ_CARD;
	}
};

/**
 * 每位玩家手中牌记录
 * 
 * @deprecated ChangeableCards 可换牌
 *             ChangeableCardsLen 可换牌个数
 *             FixedCards 固定牌
 *             FixedCardsLen 固定牌组数
 */
struct HandCards
{
	_uint8      ChangeableCardsLen;
    _uint8      ChangeableCards[HAND_CARD_SIZE_MAX];
    _uint8      FixedCardsLen;
	FixedCard   FixedCards[HAND_CARD_KAN_MAX];
	HandCards()
	{
		ChangeableCardsLen = 0;
		FixedCardsLen = 0;
		memset(ChangeableCards, INVALID_MJ_CARD, sizeof(ChangeableCards));
	}
	void reset(void)
	{
		ChangeableCardsLen = 0;
		FixedCardsLen = 0;
		memset(ChangeableCards, INVALID_MJ_CARD, sizeof(ChangeableCards));
	}
};

enum ACTION
{
	ACTION_SIDOWN = 1,//坐下
	ACTION_READY = 2,//准备
	ACTION_LEAVE = 3,//离开
	ACTION_CHANGE_TABLE = 4,//请求换桌
};
enum USER_STATUS
{
	STAND_UP = 0,        //站起状态
	SIT_DOWN = 1,        //坐下状态
	READY = 2,        	//准备状态
	PLAYING = 3,        //游戏状态
};

enum OverTimeState
{
	OVER_TIME_NO = 0,					//没有超时
	OVER_TIME_OUTCARD = 1,				//出牌超时
	OVER_TIME_OPERATE = 2,				//操作超时
};

enum HU_MODE
{
	HU_INVALID,
	HU_ZIMO = 1,
	HU_PAOHU = 2,
	HU_DIANPAO = 3,
	HU_BEIZIMO = 4,
	HU_QIANG_GANG = 5,
	HU_BEI_QIANG_GANG = 6,
	HU_YI_PAO_DUO_XIANG = 7,
	HU_GANG_SHANG_HUA = 8,
	HU_GANG_HOU_PAO = 9,
	HU_SHUA_KAI = 10,
};

enum E_BANKER_TYPE
{
	TURN_HU_BANKER,		//胡牌为庄
	TURN_SHUN_BANKER,	//轮流坐庄
	TURN_SHUN,			//顺庄
};

//#define MAX_STYLE_LEN  80

//template<class T, _uint8 SIZE = MAX_STYLE_LEN>
//class MJ_BUFFER
//{
//private:
//	_uint8 len;
//	T buffers[SIZE];
//public:
//	MJ_BUFFER(): len(0)
//	{

//	}
//	void push(const T& other)
//	{
//		if(len >= SIZE) 
//			throw("out of range\n");
//		buffers[len++] = other;
//	}
//	void push(const T *other, _uint8 size)
//	{
//		if (len + size >= sizeof(buffers) / sizeof(T))	
//			throw("out of range!\n");
//		for (_uint8 i = 0; i < size; ++i)		
//			push(other[i]);
//	}
//	T& pop()
//	{
//		if(len == 0){
//			throw("arr is null\n");
//		}
//		return buffers[--len];
//	}
//	_uint8 find(const T &other)
//	{
//		for (_uint8 i = 0; i < len; i++)
//			if (buffers[i] == other){
//				return i;
//			}
//		return len;
//	}
//	const T& operator[](_uint8 pos) const
//	{
//		if (pos >= len)		
//			throw("out of range!\n");
//		return buffers[pos];
//	}	
//	T& operator[](_uint8 pos)
//	{
//		if (pos >= len)
//			throw("out of range!\n");
//		return buffers[pos];
//	}
//	void operator=(const MJ_BUFFER<T, SIZE>& other)
//	{
//		len = other.len;
//		for (_uint8 i = 0; i < len; ++i)
//			buffers[i] = other[i];
//	}
//	bool operator==(const MJ_BUFFER<T, SIZE>& other)
//	{
//		if(len != other.len){
//			return false;
//		}
//		return memcmp(buffers, other.buffers, len) == 0;
//	}
//	void operator+=(const MJ_BUFFER<T, SIZE>& other)
//	{
//		_uint8 iCount = MIN(_uint8(SIZE - len), other.len);
//		for (_uint8 i = 0; i < iCount; ++i)
//			push(other[i]);
//	}
//	_uint8 size() const { return len; }
//	bool empty() const{ return len == 0; }
//	void clear() { len = 0; }
//	_uint8 remove(const T& other)
//	{
//		if (empty()){
//			return -1;
//		}		
//		_uint8 tmp = len - 1;
//		for (_uint8 i = 0; i < tmp; ++i){
//			if (buffers[i] == other){
//				memmove(&buffers[i], &buffers[i + 1], (tmp - i) * sizeof(T));
//				len--;
//				return i;
//			}
//		}
//		if (buffers[len - 1] == other){
//			pop();
//			return len - 1;
//		}
//		return -1;
//	}
//	void sortByAscend()
//	{
//		for (_uint8 i = 0; i < len; i++){
//			for (_uint8 j = i + 1; j < len; j++){
//				if (buffers[i] > buffers[j]){
//					T t = buffers[i];
//					buffers[i] = buffers[j];
//					buffers[j] = t;
//				}
//			}
//		}		
//	}
//};

//----------------------------配置选项-------------------------------------
struct MJConfCommon
{
	bool m_bSupportChi;			//是否能吃
    bool m_bSupportWind;		//是否有字牌
    bool m_bSupportTing;		//是否支持报听
    bool m_bZimoHu;             //是否只能自摸胡	
    bool m_bYiPaoDuoXiang;      //是否一炮多响	
    bool m_bQiangGangHu;        //是否抢杠胡	
    bool m_bQiangZhiGang;		//抢直杠
    bool m_bQiangAnGang;		//抢暗杠
    bool m_bGangShangPao;		//是否杠上炮加倍
    bool m_bHuangGang;			//是否荒庄荒杠	
    bool m_bZhuangXian;			//是否庄闲算分
    bool m_bForceHuQiangGangHu;	//强制抢杠胡
    bool m_bFourceHuMutilHu;	//多人操作强制胡牌
    int m_AnGangPoint;			//暗杠分数
    int m_WanGangPoint;			//弯杠分数
    int m_ZhiGangPoint;			//直杠分数

	MJConfCommon()
	{
		m_bSupportChi = false;
		m_bSupportWind = false;
		m_bSupportTing = false;
		m_bZimoHu = false;		
		m_bYiPaoDuoXiang = false;
		m_bQiangGangHu = false;
		m_bQiangZhiGang = false;
		m_bQiangAnGang = false;
		m_bGangShangPao = false;		
		m_bHuangGang = false;
		m_bZhuangXian = false;		
		m_bForceHuQiangGangHu = false;
		m_bFourceHuMutilHu = false;
		m_AnGangPoint = 2;
		m_ZhiGangPoint = 1;
		m_WanGangPoint = 1;			
	}
};

struct MJConfPiao
{
	bool m_bSupportPiao;
	_uint8 m_Piao[GAME_PLAYER];
	MJConfPiao()
	{
		m_bSupportPiao = false;
		memset(m_Piao, INVALID_MJ_CARD, sizeof(m_Piao));
	}
};

struct MJConfBuyHorse
{
	bool m_bSupportBuyHorse;		//???????
	_uint8 m_HorseNum;				//??????
	_uint8 m_HorseMaxNum;			//????????
	_uint8 m_nBuyHorseIncNum;		//????????????????
	bool m_buyHorseMojiJiangji;	
	MJConfBuyHorse()
	{
		m_bSupportBuyHorse = false;
		m_HorseNum = 0;
		m_HorseMaxNum = 0;
		m_nBuyHorseIncNum = 0;
		m_buyHorseMojiJiangji = false;
	}
};
struct MJConfZhuaniao
{
	bool m_bSupportZhuaniao;
	bool m_bCalcAllNiao;		//????????
	_uint8 m_ZhuaniaoNum;
	bool m_bJinNiao;
	bool m_bMojiJiangji;		//????????	
	MJConfZhuaniao()
	{
		m_bSupportZhuaniao = false;
		m_bCalcAllNiao = false;
		m_ZhuaniaoNum = 0;
		m_bJinNiao = false;
		m_bMojiJiangji = false;

	}
};

struct MJHuType
{
	bool m_bHu7dui;
	bool m_bHu13yao;
	bool m_bNoSupport_Quanqiuren;//不胡全求人
	MJHuType()
	{
		m_bHu7dui = false;
		m_bHu13yao = false;
		m_bNoSupport_Quanqiuren = true;
	}
};
//struct MJCustomizedConf
//{
//	bool MJConfPiao;
//	bool MJConfBuyHorse;
//	bool MJConfZhuaniao;
//	bool MJHuType;
//	bool m_bQiangGangHu;
//	bool m_bQiangAnGang;
//	bool m_bQiangZhiGang;
//	bool m_bHuangGang;
//	bool m_bSupportChi;
//	bool m_bZimoHu;
//	bool m_bYiPaoDuoXiang;
//	bool m_bGangShangPao;
//	bool m_bHu7dui;
//	bool m_bSupportBuyHorse;
//	bool m_HorseMaxNum;
//	bool m_bHu13yao;
//	int m_AnGangPoint;			//暗杠分数
//	int m_WanGangPoint;			//弯杠分数
//	int m_ZhiGangPoint;			//自杠分数
//	bool m_bFourceHuMutilHu;	//多胡标志
//	bool m_bNoSupport_Quanqiuren;
//};
struct MJCustomizedConf: public MJConfCommon,	
	public MJConfPiao,
	public MJConfBuyHorse,
	public MJConfZhuaniao,
	public MJHuType
{
};

class HuScore
{
public:
	int iTotleScore;
	int iHuScore;
	int iGangScore;
	int iAddtionScore;
};
class HuScoreSet
{
	HuScore set[GAME_PLAYER];
public:
	HuScoreSet()
	{
		memset(set, 0, sizeof(set));
	}
	HuScore& operator[](_uint32 pos)
	{
		assert(pos < GAME_PLAYER);
		return set[pos];
	}	
};
#endif //_COMMON_H_