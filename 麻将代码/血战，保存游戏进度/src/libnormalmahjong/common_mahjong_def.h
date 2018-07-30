#ifndef __COMMON_MAHJONG_DEF_H__
#define __COMMON_MAHJONG_DEF_H__
#include "common.h"

struct MJConfCommon
{
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive & ar, const unsigned int /* file_version */) {
		printf("MJConfCommon::serialize\n");
		ar & m_bSupportChi;			//�Ƿ��ܳ�	
		ar & m_bZimoHu;             //�Ƿ�ֻ��������	
		ar & m_bYiPaoDuoXiang;      //�Ƿ�һ�ڶ���
		ar & m_bQiangGangHu;        //�Ƿ����ܺ�	
		ar & m_bQiangZhiGang;		//��ֱ��
		ar & m_bQiangAnGang;		//������
		ar & m_bGangShangPao;		//�Ƿ�����ڼӱ�
		ar & m_bHuangGang;			//�Ƿ��ׯ�ĸ�	
		ar & m_bZhuangXian;			//�Ƿ�ׯ�����
		ar & m_bForceHuQiangGangHu;	//ǿ�����ܺ�
		ar & m_bFourceHuMutilHu;	//���˲���ǿ�ƺ���
		ar & m_bAnGang_Card_TingInfo;	//���������˺�����ʾ��Ϣ
		ar & m_AnGangPoint;			//���ܷ���
		ar & m_WanGangPoint;			//��ܷ���
		ar & m_ZhiGangPoint;			//ֱ�ܷ���
	}

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
	bool m_bAnGang_Card_TingInfo;	//���������˺�����ʾ��Ϣ
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
		ar & m_bSupportBuyHorse;		//�Ƿ�����
		ar & m_HorseNum;				//������
		ar & m_HorseMaxNum;			//��ׯ����ⶥ
		ar & m_nBuyHorseIncNum;		//��ׯ����ÿ��������
		ar & m_buyHorseMojiJiangji;
	}
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
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive & ar, const unsigned int /* file_version */) {
		printf("MJConfZhuaniao::serialize\n");
		ar & m_bSupportZhuaniao;
		ar & m_bCalcAllNiao;		//��������
		ar & m_ZhuaniaoNum;
		ar & m_bJinNiao;
		ar & m_bMojiJiangji;		//��������	
	}
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
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive & ar, const unsigned int /* file_version */) {
		printf("MJHuType::serialize\n");
		ar & m_bHu7dui;
		ar & m_bHu13yao;
		ar & m_bNoSupport_Quanqiuren;//����ȫ����
		ar & m_bQiShou_14_zhang;
		ar & m_bCheckCardNum;
		ar & m_bMultiOpSpecail;
	}
	bool m_bHu7dui;
	bool m_bHu13yao;
	bool m_bNoSupport_Quanqiuren;//����ȫ����
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