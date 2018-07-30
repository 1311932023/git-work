#ifndef __COMMON_MAHJONG_DEF_H__
#define __COMMON_MAHJONG_DEF_H__
#include "common.h"

struct MJConfCommon
{
	bool m_bSupportChi;			//�Ƿ��ܳ�	
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
		m_bSupportChi = true;		
		m_bZimoHu = false;		
		m_bYiPaoDuoXiang = false;
		m_bQiangGangHu = true;
		m_bQiangZhiGang = false;
		m_bQiangAnGang = false;
		m_bGangShangPao = false;		
		m_bHuangGang = false;
		m_bZhuangXian = false;		
		m_bForceHuQiangGangHu = true;
		m_bFourceHuMutilHu = false;
		m_AnGangPoint = 1;
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
	bool m_bSupportBuyHorse;		//�Ƿ�����
	_uint8 m_HorseNum;				//������
	_uint8 m_HorseMaxNum;			//��ׯ����ⶥ
	_uint8 m_nBuyHorseIncNum;		//��ׯ����ÿ��������
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
	bool m_bCalcAllNiao;		//��������
	_uint8 m_ZhuaniaoNum;
	bool m_bJinNiao;
	bool m_bMojiJiangji;		//��������	
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
	bool m_bQiShou_14_zhang;
	MJHuType()
	{
		m_bHu7dui = true;
		m_bHu13yao = true;
		m_bNoSupport_Quanqiuren = false;
		m_bQiShou_14_zhang = false;
	}
};
struct MJCustomizedConf: public MJConfCommon,	
	public MJConfPiao,
	public MJConfBuyHorse,
	public MJConfZhuaniao,
	public MJHuType
{
};
#endif