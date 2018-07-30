/***********************************************************
 * File Name          :       common.h
 * Author             :       
 * Version            :       1.0
 * Date               :       2016-05-22 14:17
 * Description        :       �����ӿ�
***********************************************************/

#ifndef _COMMON_H_
#define _COMMON_H_



#include <time.h>
#include <map>
#include <string.h>
#include <assert.h>
#include "to_char.h"
//�������Ͷ���
typedef          char        _tint8;     //�з��� 1 �ֽ�
typedef unsigned char        _uint8;     //�޷��� 1 �ֽ�
typedef short                _tint16;    //�з��� 2 �ֽ�
typedef unsigned short       _uint16;    //�޷��� 2 �ֽ�
typedef int                  _tint32;    //�з��� 4 �ֽ�
typedef unsigned int         _uint32;    //�޷��� 4 �ֽ�
typedef long long            _tint64;    //�з��� 8 �ֽ�
typedef unsigned long long   _uint64;    //�޷��� 8 �ֽ�

#define GPS_STATE_UNUPDATED         0       //GPSδ����
#define GPS_STATE_UPDATED           1       //GPS�Ѹ���

#define GAME_PLAYER					4				//��Ϸ����

#define INVALID_MJ_CARD 0xFF
#define NUM_MASK		0x0F
#define TYPE_MASK		0xF0
#define INVALID_FAN		-1

#define INVALID_CHAIR  				255	 								//��Ч��λ��
#define INVALID_CARD				0xFF								//��Ч��

#define INVALID_CHAIRID 0xFF
#define INVALID_MJ_CARD 0xFF
#define CARD_0			(0)
//��������
#define MAX_WEAVE					4				//������
#define MAX_COUNT					14			    //�����Ŀ
#define MAX_REPERTORY				108			    //�����
#define POOL_CARD_MAX				150				//�������Ŀ
#define POOL_CARD_TYPE_MAX			42				//����Ƶ�����
#define MAX_LAIZI_COUNT				(8)				//��������
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


#define TYPE_NULL			0x00								//û������
#define TYPE_PENG			0x01								//��������
#define TYPE_ZHIGANG		0x02								//ֱ��������
#define TYPE_WANGANG        0x04                                //���
#define TYPE_ANGANG         0x08                                //����
#define TYPE_GANG			(TYPE_ZHIGANG | TYPE_WANGANG | TYPE_ANGANG)
#define TYPE_HU			    0x10								//�Ժ�����
#define TYPE_LISTEN			0x20								//��������
#define TYPE_LEFT_CHI	    0x40								//�������
#define TYPE_CENTER_CHI		0x80								//�г�����
#define TYPE_RIGHT_CHI		0x100								//�ҳ�����
#define TYPE_CHI			(TYPE_LEFT_CHI | TYPE_RIGHT_CHI | TYPE_CENTER_CHI)
#define TYPE_HUA_HU			0x200								//����
#define TYPE_BU_ZHANG		0x400								//����
#define TYPE_GEN_ZHANG		0x400								//�㶫��ׯ
#define TYPE_HAIDI_YAO		0x800								//�����ƣ�Ҫ��
#define TYPE_SUODING		0x1000								//��������
#define TYPE_NO_OPERATE		0x2000								//��������������Ҫ�и�ֵ

#define TYPE_LAIZI_GANG	 	0x10000								//���Ӹ�
#define TYPE_HONGZHONG_GANG	0x20000								//���и�
#define TYPE_FACAI_GANG	 	0x40000								//���Ƹ�
#define TYPE_LAIZIPI_GANG	0x80000								//����Ƥ��

#define TYPE_PASS			0xFF								//��
#define TYPE_CHI_PENG		(TYPE_CHI | TYPE_PENG)
#define TYPE_CHI_PENG_GANG	(TYPE_CHI | TYPE_PENG | TYPE_GANG)
#define TYPE_PENG_GANG		(TYPE_PENG | TYPE_GANG)

//Ƽ����齫����ϲ�
#define PLAY_MODE_PINGXIANGZZ           0
#define PLAY_MODE_PINGXIANGHZ           1
#define PLAY_MODE_PINGXIANG258          2

#define ZHONG_NIAO_DOUBLE              3                //���񷭱�


const int MJ_TYPE_NUM = 3; //�齫���͹�����:��,��,Ͳ
enum MJ_TYPE
{
	MJ_TYPE_WAN = 0,		//��,0-8,��4�ţ���36��
	MJ_TYPE_TIAO = 1,		//��,0-8,��4�ţ���36��
	MJ_TYPE_SUO = 1,
	MJ_TYPE_TONG = 2,		//Ͳ,0-8,��4�ţ���36��
	MJ_TYPE_BING = 2,
	MJ_TYPE_FENG = 3,
	MJ_TYPE_JIAN = 4,	
	MJ_TYPE_FLOWER = 5,
	MJ_TYPE_SEASION = 5,
	MJ_TYPE_COMMON = 6,		//����Ͳ���
	MJ_TYPE_INVALID = -1,
};

#define CardNum(c) ((c) & NUM_MASK)
#define CardType(c) (((c) & TYPE_MASK)>>4)
#define CardNumX(card) ((card >= MJ_HONG_ZHONG && card <= MJ_BAI_BAN) ? (CardNum(card) + 4) : CardNum(card))
#define CardValue(t,n) ( (t<<4) | (n) )

struct FixedCard
{    
    int    state;			//��������
	_uint8    chairID;          //�������������ID(��Ҫ��Ϊ�˼�¼����ֱ��˭����)
	union 
	{
		_uint8 CardData; //���ܵ���
		_uint8 c[3];	//�Ե���
	};
	
	FixedCard()
	{		
		state = TYPE_NULL;
		chairID = INVALID_CHAIRID;
		CardData = INVALID_MJ_CARD;
	}
};

/**
 * ÿλ��������Ƽ�¼
 * 
 * @deprecated ChangeableCards �ɻ���
 *             ChangeableCardsLen �ɻ��Ƹ���
 *             FixedCards �̶���
 *             FixedCardsLen �̶�������
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
	ACTION_SIDOWN = 1,//����
	ACTION_READY = 2,//׼��
	ACTION_LEAVE = 3,//�뿪
	ACTION_CHANGE_TABLE = 4,//������
};
enum USER_STATUS
{
	STAND_UP = 0,        //վ��״̬
	SIT_DOWN = 1,        //����״̬
	READY = 2,        	//׼��״̬
	PLAYING = 3,        //��Ϸ״̬
};

enum OverTimeState
{
	OVER_TIME_NO = 0,					//û�г�ʱ
	OVER_TIME_OUTCARD = 1,				//���Ƴ�ʱ
	OVER_TIME_OPERATE = 2,				//������ʱ
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
	TURN_HU_BANKER,		//����Ϊׯ
	TURN_SHUN_BANKER,	//������ׯ
	TURN_SHUN,			//˳ׯ
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

//----------------------------����ѡ��-------------------------------------
struct MJConfCommon
{
	bool m_bSupportChi;			//�Ƿ��ܳ�
    bool m_bSupportWind;		//�Ƿ�������
    bool m_bSupportTing;		//�Ƿ�֧�ֱ���
    bool m_bZimoHu;             //�Ƿ�ֻ��������	
    bool m_bYiPaoDuoXiang;      //�Ƿ�һ�ڶ���	
    bool m_bQiangGangHu;        //�Ƿ����ܺ�	
    bool m_bQiangZhiGang;		//��ֱ��
    bool m_bQiangAnGang;		//������
    bool m_bGangShangPao;		//�Ƿ�����ڼӱ�
    bool m_bHuangGang;			//�Ƿ��ׯ�ĸ�	
    bool m_bZhuangXian;			//�Ƿ�ׯ�����
    bool m_bForceHuQiangGangHu;	//ǿ�����ܺ�
    bool m_bFourceHuMutilHu;	//���˲���ǿ�ƺ���
    int m_AnGangPoint;			//���ܷ���
    int m_WanGangPoint;			//��ܷ���
    int m_ZhiGangPoint;			//ֱ�ܷ���

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
	bool m_bNoSupport_Quanqiuren;//����ȫ����
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
//	int m_AnGangPoint;			//���ܷ���
//	int m_WanGangPoint;			//��ܷ���
//	int m_ZhiGangPoint;			//�Ըܷ���
//	bool m_bFourceHuMutilHu;	//�����־
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