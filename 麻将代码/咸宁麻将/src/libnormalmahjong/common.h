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

#define DEF_BUF_LEN (static_cast<int>(1024 * 128))

#define GAME_PLAYER					4				//游戏人数

#define INVALID_MJ_CARD 0xFF
#define NUM_MASK		0x0F
#define TYPE_MASK		0xF0
#define INVALID_FAN		-1

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
#define MJ_QI_TONG					(0x27)

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
#define TYPE_BAO_TING		0x4000								//报听
#define TYPE_PASS			0xFF								//过
#define TYPE_CHI_PENG		(TYPE_CHI | TYPE_PENG)
#define TYPE_CHI_PENG_GANG	(TYPE_CHI | TYPE_PENG | TYPE_GANG)
#define TYPE_PENG_GANG		(TYPE_PENG | TYPE_GANG)
#define TYPE_YX_YOUJINHU    0x8000



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
	READY = 2,        //准备状态
	PLAYING = 3,        //游戏状态
};

enum HU_MODE
{
	HU_INVALID,
	HU_ZIMO,
	HU_PAOHU,
	HU_DIANPAO,
	HU_BEIZIMO,
	HU_GANGSHANGHU,
};

enum E_BANKER_TYPE
{
	TURN_HU_BANKER,		//胡牌为庄
	TURN_SHUN_BANKER,	//轮流坐庄
	TURN_SHUN,			//顺庄
};
#endif //_COMMON_H_
