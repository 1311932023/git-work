#ifndef __COMMON_MAHJONG_DEF_H__
#define __COMMON_MAHJONG_DEF_H__
#include "common.h"

struct MJConfCommon
{
	bool m_bSupportChi;			//是否能吃	
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
	bool m_bSupportBuyHorse;		//是否买马
	_uint8 m_HorseNum;				//买马数
	_uint8 m_HorseMaxNum;			//连庄买马封顶
	_uint8 m_nBuyHorseIncNum;		//连庄买马每次增加数
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
	bool m_bCalcAllNiao;		//所有鸟都中
	_uint8 m_ZhuaniaoNum;
	bool m_bJinNiao;
	bool m_bMojiJiangji;		//摸几奖几	
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