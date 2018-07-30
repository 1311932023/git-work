/********************************************************************
 *     �齫���Ʒ�������
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
 * ������ID����
 */
enum MJFanType
{	
	MJ_FAN_TYPE_NULL = 0,
	MJ_FAN_TYPE_JIULIANBAODENG = 1,				//��������
	MJ_FAN_TYPE_DASIXI = 2,						//����ϲ
	MJ_FAN_TYPE_XIAOSIXI = 3,					//С��ϲ
	MJ_FAN_TYPE_DASANYUAN = 4,					//����Ԫ 
	MJ_FAN_TYPE_SIANKE = 5,						//�İ���
	MJ_FAN_TYPE_SHIBALUOHAN = 6,				//ʮ���޺�
	MJ_FAN_TYPE_QUANYAOJIU = 7,					//ȫ�۾�	
	MJ_FAN_TYPE_ZIYISE = 8,						//��һɫ
	MJ_FAN_TYPE_XIAOSANYUAN = 9,				//С��Ԫ
	MJ_FAN_TYPE_QINGYAOJIU = 10,				//���۾�,���۾�
	MJ_FAN_TYPE_QINGDUI = 11,					//��� ��һɫ + ������	
	MJ_FAN_TYPE_QINGPENG = 12,					//������ ��һɫ + ������
	MJ_FAN_TYPE_HUNPENG = 13,					//����, ��һɫ + ������
	MJ_FAN_TYPE_QINGYISE = 14,					//��һɫ
	MJ_FAN_TYPE_HUNYISE = 15,					//��һɫ
	MJ_FAN_TYPE_PENGPENGHU = 16,				//������		
	MJ_FAN_TYPE_PINGHU = 17,					//ƽ��	
	MJ_FAN_TYPE_JIHU = 18,						//����	
	MJ_FAN_TYPE_TIANHU = 19,					//���
	MJ_FAN_TYPE_DIHU = 20,						//�غ�
	MJ_FAN_TYPE_RENHU = 21,						//�˺�
	MJ_FAN_TYPE_QIANGGANGHU = 22,				//���ܺ�
	MJ_FAN_TYPE_HAIDILAOYUE = 23,				//��������
	MJ_FAN_TYPE_GANGSHANGHUA = 24,				//���Ͽ���
	MJ_FAN_TYPE_QINGQIDUI = 25,					//���߶�	
	MJ_FAN_TYPE_QINGLONGQIDUI = 26,				//�����߶� 
	MJ_FAN_TYPE_LONGQIDUI = 27,					//���߶�
	MJ_FAN_TYPE_QUANQIUREN = 28,				//ȫ����
	MJ_FAN_TYPE_GANGHOUPAO = 29,				//������
	MJ_FAN_TYPE_MENQIANQING = 30,				//��ǰ��
	MJ_FAN_TYPE_FENGYISE = 31,					//��һɫ
	MJ_FAN_TYPE_MENQINGQINGYISE = 32,			//��ǰ�� + ��һɫ
	MJ_FAN_TYPE_QINGSIANKE = 33,				//��һɫ + �İ���
	MJ_FAN_TYPE_HAIDIPAO = 34,					//������
	MJ_FAN_TYPE_13YAO = 35,						//ʮ����	
	MJ_FAN_TYPE_QIDUI = 36,						//�߶�		
	MJ_FAN_TYPE_DAIYAOJIU = 37,					//���۾�	
	MJ_FAN_TYPE_JIANGDUI = 38,					//����, 258����
	MJ_FAN_TYPE_YITIAOLONG = 39,				//һ����
	MJ_FAN_TYPE_QINGYITIAOLONG = 40,			//��һ����
	MJ_FAN_TYPE_4JOKER = 41,					//4���
	MJ_FAN_TYPE_NOJOKER = 42,					//�޹�
	MJ_FAN_TYPE_QINGQUANQIUREN = 43,			//��ȫ����
	MJ_FAN_TYPE_DANDIAO = 44,					//����
	MJ_FAN_TYPE_MINGSIGUI = 45,					//���Ĺ�
	MJ_FAN_TYPE_ANSIGUI = 46,					//���Ĺ�
	MJ_FAN_TYPE_JIANGYISE = 47,					//��һɫ
	MJ_FAN_TYPE_ZIMO = 48,                      //����

	MJ_FAN_TYPE_MAX = 49,

	MJ_FAN_TYPE_SPECIAL = 512,
	MJ_FAN_CHANGSHA_SIXIHU = 513,				//��ɳ��ϲ��
	MJ_FAN_CHANGSHA_BANBANHU = 514,				//��ɳ����
	MJ_FAN_CHANGSHA_QUEYIMEN = 515,				//��ɳȱһ��
	MJ_FAN_CHANGSHA_LIULIUSHUN = 516,			//��ɳ����˳	
	MJ_FAN_CHANGSHA_JIANGJIANGHU = 517,			//��ɳ������ ȫ��258 ��������
	MJ_FAN_CHENZHOU_SIZHONGHU = 518,	
	MJ_FAN_GUANGDONG_QIANGGANGQUANBAO = 519,
	MJ_FAN_GUANGDONG_GANGBAOQUANBAO = 520,
	MJ_FAN_GUIYANG_RUANBAO = 521,
	MJ_FAN_GUIYANG_YINGBAO = 522,
	MJ_FAN_GUIYANG_SHABAO_10 = 523,
	MJ_FAN_GUIYANG_SHABAO_20 = 524,
	MJ_FAN_HUA_DIAO = 525,				//������
	MJ_FAN_HUA_DIAO_HUA = 526,			//������
	MJ_FAN_HUA_2_HUA_DIAO_HUA = 527,	//˫������
	MJ_FAN_HUA_3_DIAO_HUA = 528,		//��������	

	MJ_FAN_QIDUI_SHUANGLONG = 529,		//˫���߶�(�������߶�)
	MJ_FAN_QIDUI_SANLONG = 530,			//�����߶ԣ��Ƿ��߶ԣ�
	MJ_FAN_QIDUI_QINGSHUANGLONG = 531,	//��˫���߶�
	MJ_FAN_QIDUI_QINGSANLONG = 532,		//�������߶�
	MJ_FAN_HUIZHOU_BAOJIUQUANBAO = 533,	//���ݣ�����ȫ��
	MJ_FAN_TYPE_KAWUXING = 534,			//����������
	MJ_FAN_TYPE_SHOWCARDS = 535,		//��������
	MJ_FAN_TYPE_LUANJIANGHU = 536,		//�ҽ���
	MJ_FAN_TYPE_13LUAN = 537,		    //13��
	MJ_FAN_TYPE_13FENG = 538,		    //13��
	MJ_FAN_TYPE_JIANGJIANGHU = 539,		//������
	MJ_FAN_TYPE_SPECIAL_MAX = 540,
};

//����״̬
enum OPERATE_STAUS
{ 
	OPERATE_NULL               = 0,                  
	OPERATE_CHANGE_CARD        = 1,                  //����״̬
	OPERATE_LACK_CARD          = 2,                  //��ȱ��״̬
	OPERATE_CARD               = 3,                  //���ƺͲ�����״̬
};

enum GANG_TYPE
{
	ZHI_GANG_INDEX = 0,
	WAN_GANG_INDEX = 1,
	AN_GANG_INDEX  = 2
};

struct GangScore
{  
	int         GangType;           //������
	_tint64		llScore;		      //�ܷ�
};
struct GangInfo
{
	int			chairID;	          //���ID
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

//���Ƽ�¼
struct GangRecord
{   
	int GangNum;                     //�ܸ���
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
	int chairID;                     //����ID
	int nFanType;                    //����
	int nFanNum;                     //����
	int nGenNum;                     //���ĸ���
};


//�Ǵ�������Ϣ
struct tagListen 
{
	_uint8  cbNum;					//����������
	info    Listen[4];				//�����ҵ���Ϣ
};

//���������Ϣ
struct HuInfo 
{  
	_uint8  cbZiMo;                     //�Ƿ�����
	_uint8	cbHuNum;					//��Һ��˼�����
	int	    ChairID[3];				    //��Һ���˭
	_uint8	cbFanType;					//���Ʒ���
	_uint8	cbFanNum;					//�ܷ���(����+����+����+��ӷ�)
	_uint8  cbCombType;                 //��Ϸ���
	int		nHuScore;					//����
	_uint8	cbGangNum;					//�ܵĸ���
	_uint8	cbGenNum;					//���ĸ���
	_uint8	cbAddFan;					//��ӷ�
	_uint8  cbPasChairID;               //����������(����ʱΪ�Լ���ID)
	_uint8  cbHuCard;                   //���������
	_uint8  HuType;                     //��������
	int     OutNum;                     //��������
	int     TaskDoubleTimes;            //�ƾ�������
};

//��������Ϣ��¼
struct HuRecord
{
	int	   ChairID;					//�������ID
	int	   nScore;						//����
};
//��������Ϣ
struct PassiveHuInfo
{
	_uint8		cbPasHuNum;					//��������Һ�
	HuRecord	sPassiveHu[3];				//���Ǹ���Һ�����Ϣ��¼
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
	_uint8 cbCardType;                //��ɫ����
	_uint8 cbTotalNum;                //���ֻ�ɫ������
	_uint8 cbSingleNum;               //���Ÿ���
	_uint8 cbDuiziNum;                //���Ӹ���
	_uint8 cbThreeNum;                //���Ÿ���
	_uint8 cbFourNum;                 //���Ÿ���
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
	int TaskType;             //��������
	int RewardType;           //��������
	int PropID;               //�����ĵ���ID
	int RewardNum;            //��������
	char TaskDesc[256];      //��������
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
