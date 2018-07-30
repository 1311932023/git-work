#ifndef __TABLE_CAREDPPOR_H__
#define __TABLE_CAREDPPOR_H__
#include <vector>
#include <json/json.h>
#include <assert.h>
#include "../libphz/common.h"
#include "../libphz/game_logic.h"


typedef MJ_BUFFER<int, 40> MJ_SUB_ID;
bool json_check_play_type_id(const Json::Value &val);
int json_get_play_type_id(const Json::Value &val, int iPlayID, MJ_SUB_ID& SubID);

bool Is_ChiPengGang(int iOpType);
bool Is_HuOperator(int iOpType);

/*
 *	©������
 *	��¼�������ƣ� �Լ�����
 *	m_Available true����©������
 *	Lock ��ס���ܺ���
 *	Evalute �ж��Ƿ���Խ�������, true ���Խ���
 *	Unlock	�������Ժ���
 */
#define FW_LOCK_ALL_CARD	INVALID_MJ_CARD
#define FW_LOCK_ANY_POINT	(0xff)

class ForbidWin
{
	//_uint8 m_LockCard;
	std::vector<_uint8> m_LockCard;
	_uint8 m_LockPoints;
	bool m_Lock;		
	bool Evalute(_uint8 card, _uint8 point)const;
public:
	ForbidWin();
	void Lock(_uint8 card, _uint8 point);	
	bool IsLock(_uint8 card, _uint8 point = 0);	
	void Unlock();	
};


enum E_POOL_MODE
{
	CARD_POOL_ALL,			//������ �����˸���
	CARD_POOL_NO_FLOWER,	//��Ͳ�����
	CARD_POOL_NO_WIND_DRAGON_FLOWER,//��Ͳ��
	CARD_POOL_2_PEOPLE,
	CARD_POOL_3_PEOPLE,
	CARD_POOL_100CARDS,		//100��û����
	CARD_POOL_TTF,			//Ͳ����
	CARD_POOL_TTJ,			//Ͳ����
	CARD_POOL_TTFJNoDong,	//Ͳ�����������
	CARD_POOL_4_FLOWER,		//��Ͳ����� + �����ﶬ
	CARD_POOL_NO_WIND_4_FLOWER, //�޷���4��   ��Ͳ��+ �����ﶬ
	CARD_POOL_NO_WIND_8_FLOWER, //�޷���8��  ��Ͳ��+8��
	CARD_POOL_2_FLOWER,		//��Ͳ����� + ����
	CARD_POOL_6_FLOWER,		//��Ͳ����� + �����ﶬ÷��
	CARD_POOL_TTFJNoBai,	//ͬ�����û�аװ�
	CARD_POOL_72CARDS,		//72����
	CARD_POOL_WTTHONGZHONG, //����Ͳ�ͺ���
	CARD_POOL_WTT_HONGZHONG_8,//��Ͳ����8������
};

class CCardTypePool
{
public:
	CCardTypePool();
	~CCardTypePool();

	_uint8 operator[](_uint8 pos)const
	{
		assert(pos < m_MaxCount);
		return m_Pool[pos];
	}
	_uint8 Size()const { return m_MaxCount; }
	void Set(E_POOL_MODE eMode);
	void SetJoker(_uint8 card);
public:
	void Clear() { m_MaxCount = 0; }
	void AddPool(const _uint8* card, _uint8 size);
private:
	_uint8 m_JokerCard;
private:
	_uint8 m_Pool[POOL_CARD_TYPE_MAX];
	_uint8 m_MaxCount;
	E_POOL_MODE m_Mode;
};

class CCardPool
{
public:	
	CCardPool(E_POOL_MODE iMode, int iSeedID = 0);
	~CCardPool();

	void Set(E_POOL_MODE iMode, int iSeedID = 0);
	void SetJoker(_uint8 card, _uint8 count);
public:		
	_uint8 operator[](_uint8 pos)const;
	_uint8& operator[](_uint8 pos);	
	
	void InitPool();
	_uint8 Find(_uint8 card)const;
	//�ָ��������е����飨���Է�����
	int PushCardPool(_uint8 AllCard[], int len, int seedid,E_POOL_MODE mode,int maxcount,int count,int current,int last);
public:
	_uint8 GetOneCard();
	_uint8 GetLast();
	int GetCard(_uint8* pool, _uint8 size);	
	int GetLastCard(_uint8* pool, _uint8 size);

	_uint8 SeedID()const { return m_SeedID; }	
	E_POOL_MODE Mode()const { return m_Mode; }
	_uint8 Last()const { return m_Last; }		
	_uint8 MaxCount()const { return m_MaxCount; }		//�������
	_uint8 LastCard()const { return m_Pool[m_Last - 1];	}	//�鿴���һ����
	_uint8 CurCard()const { return m_Pool[m_Current]; }	//�鿴��һ����
	_uint8 Count()const { return m_Count; }				//��ǰʣ������
	bool IsEmpty()const { return m_Count == 0; }		//�Ƿ�û������
	bool IsLastCard()const { return m_Count == 1; }		//�Ƿ����һ����
	_uint8 Current()const { return m_Current; }			//��ǰ��λ��
	_uint8 GetAllCard(_uint8 pos)const 
	{ 
        assert(pos < m_MaxCount);
        return m_Pool[pos];

	};                        //������Ϊ����������ʹ��
	_uint8 GetCardCount_FromBack()const{ return m_MaxCount - m_Last; }
public:
	bool Exchange_Cur_Next(_uint8 pos = -1);
	void ChangeCard(int first, int second);
public:
	void FixCard(int index);//�����ù̶���
	void FixCard_ReadJson();
private:
	void AddPool(const _uint8* card, _uint8 size, _uint8 count = 4);
	void AddJoker();
private:
	void InitTTFJNoDong();
	void Init72Cards();
	void InitWTTHongZhong();
	void InitTTFJNoBaiCards();
	void InitTTJCards();	
	void InitTTFCards();	
	void InitNoFlower();	
	void InitNoWindDragon();
	void Init2People();
	void Init3People();
	void Init100Cards();
	void InitAllCard();
	void Init4FlowerCard();
	void InitNoWind4FlowerCard();
	void InitNoWind8FlowerCard();
	void Init2FlowerCard();
	void Init6FlowerCard();
	void InitWTTHONGZHONGCards8();
private:
	CCardTypePool m_CardTypePool;
public:
	const CCardTypePool& Type()const
	{
		return m_CardTypePool;
	}

private:
	int m_SeedID;
	E_POOL_MODE m_Mode;
	_uint8 m_Pool[POOL_CARD_MAX];
	_uint8 m_MaxCount;
	_uint8 m_Count;
	_uint8 m_Current;	//��ǰ��λ��
	_uint8 m_Last;		//���һ���Ƶ���һ��
	
private:
	struct
	{
		_uint8 m_JokerCard;
		_uint8 m_JokerCount;
	};
};


#endif
