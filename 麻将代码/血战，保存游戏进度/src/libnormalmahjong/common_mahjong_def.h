#ifndef __COMMON_MAHJONG_DEF_H__
#define __COMMON_MAHJONG_DEF_H__
#include "common.h"

struct MJConfCommon
{
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive & ar, const unsigned int /* file_version */) {
		printf("MJConfCommon::serialize\n");
		ar & m_bSupportChi;			//是否能吃	
		ar & m_bZimoHu;             //是否只能自摸胡	
		ar & m_bYiPaoDuoXiang;      //是否一炮多响
		ar & m_bQiangGangHu;        //是否抢杠胡	
		ar & m_bQiangZhiGang;		//抢直杠
		ar & m_bQiangAnGang;		//抢暗杠
		ar & m_bGangShangPao;		//是否杠上炮加倍
		ar & m_bHuangGang;			//是否荒庄荒杠	
		ar & m_bZhuangXian;			//是否庄闲算分
		ar & m_bForceHuQiangGangHu;	//强制抢杠胡
		ar & m_bFourceHuMutilHu;	//多人操作强制胡牌
		ar & m_bAnGang_Card_TingInfo;	//暗杠牌算人胡牌提示信息
		ar & m_AnGangPoint;			//暗杠分数
		ar & m_WanGangPoint;			//弯杠分数
		ar & m_ZhiGangPoint;			//直杠分数
	}

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
	bool m_bAnGang_Card_TingInfo;	//暗杠牌算人胡牌提示信息
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
		m_bFourceHuMutilHu = true;
		m_bAnGang_Card_TingInfo = false;
		m_AnGangPoint = 2;
		m_ZhiGangPoint = 3;
		m_WanGangPoint = 1;			
	}
};

struct MJConfPiao
{
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive & ar, const unsigned int /* file_version */) {
		printf("MJConfPiao::serialize\n");
		ar & m_bSupportPiao;
		ar & m_Piao;
	}
	bool m_bSupportPiao;
	_uint8 m_Piao[GAME_PLAYER];
	MJConfPiao()
	{
		m_bSupportPiao = false;
		memset(m_Piao, INVALID_MJ_CARD, sizeof(m_Piao));
	}
};

struct MJConfDingQue
{
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive & ar, const unsigned int /* file_version */) {
		printf("MJConfDingQue::serialize\n");
		ar & m_bSupportDingQue;
		ar & m_DingQueSe;
	}
	bool m_bSupportDingQue;
	_uint8 m_DingQueSe[GAME_PLAYER];
	MJConfDingQue()
	{
		m_bSupportDingQue = false;
		memset(m_DingQueSe, INVALID_MJ_CARD, sizeof(m_DingQueSe));
	}
};

struct MJConfBuyHorse
{
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive & ar, const unsigned int /* file_version */) {
		printf("MJConfBuyHorse::serialize\n");
		ar & m_bSupportBuyHorse;		//是否买马
		ar & m_HorseNum;				//买马数
		ar & m_HorseMaxNum;			//连庄买马封顶
		ar & m_nBuyHorseIncNum;		//连庄买马每次增加数
		ar & m_buyHorseMojiJiangji;
	}
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
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive & ar, const unsigned int /* file_version */) {
		printf("MJConfZhuaniao::serialize\n");
		ar & m_bSupportZhuaniao;
		ar & m_bCalcAllNiao;		//所有鸟都中
		ar & m_ZhuaniaoNum;
		ar & m_bJinNiao;
		ar & m_bMojiJiangji;		//摸几奖几	
	}
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
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive & ar, const unsigned int /* file_version */) {
		printf("MJHuType::serialize\n");
		ar & m_bHu7dui;
		ar & m_bHu13yao;
		ar & m_bNoSupport_Quanqiuren;//不胡全求人
		ar & m_bQiShou_14_zhang;
		ar & m_bCheckCardNum;
		ar & m_bMultiOpSpecail;
	}
	bool m_bHu7dui;
	bool m_bHu13yao;
	bool m_bNoSupport_Quanqiuren;//不胡全求人
	bool m_bQiShou_14_zhang;
	bool m_bCheckCardNum;
	bool m_bMultiOpSpecail;
	MJHuType()
	{
		m_bHu7dui = true;
		m_bHu13yao = true;
		m_bNoSupport_Quanqiuren = false;
		m_bQiShou_14_zhang = false;
		m_bCheckCardNum = true;
		m_bMultiOpSpecail = false;
	}
};
struct MJCustomizedConf: public MJConfCommon,	
	public MJConfPiao,
	public MJConfBuyHorse,
	public MJConfZhuaniao,
	public MJHuType,
	public MJConfDingQue
{
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive & ar, const unsigned int /* file_version */) {
		printf("MJCustomizedConf::serialize\n");
		ar & boost::serialization::base_object<MJConfCommon>(*this);
		ar & boost::serialization::base_object<MJConfPiao>(*this);
		ar & boost::serialization::base_object<MJConfBuyHorse>(*this);
		ar & boost::serialization::base_object<MJConfZhuaniao>(*this);
		ar & boost::serialization::base_object<MJHuType>(*this);
		ar & boost::serialization::base_object<MJConfDingQue>(*this);
		ar & m_bEndResCard;
	}

	bool m_bEndResCard;
	MJCustomizedConf() : m_bEndResCard(false) {}
};
#endif