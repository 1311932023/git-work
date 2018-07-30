
#ifndef __GAME_LOGIC_H__
#define __GAME_LOGIC_H__
#include "common.h"
#include "mjcardtype.h"
#include <vector>


//ɸ��
struct Shai_Set
{
	_uint8 Num;
	_uint8 Shai[7];	
	Shai_Set() 
	{		
		Num = 0;
		memset(Shai, 0, sizeof(Shai));
	}
};
struct OpState
{	
	_uint8 cbCard;
	int    nState;
	OpState()
	{
		cbCard = INVALID_MJ_CARD;
		nState = 0;
	}
	OpState(_uint8 card, int state) : cbCard(card), nState(state)
	{
	}
	bool operator==(const OpState& other)const
	{
		return cbCard == other.cbCard && nState == other.nState;
	}
};

typedef MJ_BUFFER<OpState, 20> MJ_opinfo;

//-----------------------------�齫��Ϸ�߼�-------------------------------
class CGameLogic
{
public:
	CGameLogic(void);
	~CGameLogic(void);
	
public:
//-----------------------------�ƿ����-----�����Ͳ���--------------------------
	//ϴ��
	void RandomCard(_uint8 cbCard[], _uint8 cbLength, int iTableID);
	//�����
	void AddCard(HandCards *pHc, _uint8 bCard);
    //����
	bool ChuPai(HandCards *pHc, _uint8 bCard);
    //�ܲ���
	void ExecuteGang(HandCards *pHc, _uint8 bOpCardChairID, _uint8 bOperateCard, int iOpType);
	//������
	void ExecutePeng(HandCards *pHc, _uint8 bOpCardChairID, _uint8 bOperateCard);
	//��Բ���
	void ExecuteLeftChi(HandCards *handcard, _uint8 chairID, _uint8 cbOperateCard);
	//�гԲ���
	void ExecuteCenterChi(HandCards *handcard, _uint8 chairID, _uint8 cbOperateCard);
	//�ҳԲ���
	void ExecuteRightChi(HandCards *handcard, _uint8 chairID, _uint8 cbOperateCard);
	//��ȡ����Ϣ
	int GetGangInfo(const HandCards*pHc, MJ_opinfo& ganginfo);
	int GetGangInfo(const HandCards*pHc, MJ_opinfo& ganginfo, MJ_gang &lou_gang, _uint8 laizi_card); 
	int GetGangInfo(const HandCards*pHc, MJ_opinfo& ganginfo, MJ_gang &lou_gang, MJ_BUFFER<_uint8, 14> showListenInfos[GAME_PLAYER], _uint8 ChairID);
	//��ȡ������Ϣ
	int GetPengGang(HandCards *pHc, _uint8 bCard, MJ_opinfo& GangCardInfo);
	int GetPengGang(HandCards *pHc, _uint8 bCard, MJ_opinfo& GangCardInfo, _uint8 laizi_card);
	//��ȡ���	
	bool GetWanGang(const HandCards* pHc, MJ_opinfo& ganginfo, _uint8 bCard = INVALID_MJ_CARD);
	//��ȡ�Ը�
	bool GetZhiGang(const HandCards* pHc, MJ_opinfo& ganginfo, _uint8 bCard = INVALID_MJ_CARD);
	//��ȡ����
	bool GetAnGang(const HandCards* pHc, MJ_opinfo& ganginfo, _uint8 bCard = INVALID_MJ_CARD);
	//��ȡ����Ϣ
	int GetChi(const HandCards *p, _uint8 cbCard, MJ_opinfo& ChiInfo);
	//��ȡ�Ƶ��߼�����
	_uint8 NextCard(_uint8 card);
	//��ȡ�Ƶ��߼�����
	_uint8 PrevCard(_uint8 card);
	//����Ƿ�Ϊһ���
	bool checke_combine(_uint8 cbCard1, _uint8 cbCard2, _uint8 cbCard3);
	//����Ƿ�Ϊ˳�����
	bool checke_combine_shun(_uint8 cbCard1, _uint8 cbCard2, _uint8 cbCard3);
	//����Ƿ�Ϊ�������
	bool checke_combine_dui_zi(_uint8 cbCard1, _uint8 cbCard2);
	//��ȡ����ÿ���Ƶ�����
	void GetCardCount(const HandCards* pHc, _uint8 CardDataNum[MJ_TYPE_COMMON][10]);
	//��ȡ��ǰÿ���Ƶ�����
	void GetCardCountExt(const HandCards* pHc, _uint8 CardDataNum[MJ_TYPE_COMMON][10]);
	//��ȡ���ƺ���ǰָ���Ƶ�������
	int GetCardCountX(const HandCards* pHc, _uint8 card);
	//��ȡָ��������ǰ�е�����
	int GetFixedCardCount(const HandCards* pHc, _uint8 bCard);
	//��ȡ������ɫ���Ƶ�������
	bool GetCardTypeCount(const HandCards* pHc, _uint8 CardTypeCount[MJ_TYPE_COMMON]);
public:
	//��ȡɫ��
	Shai_Set GetShaizi(_uint8 iCount);
public:
//------------------------------------���������ж�-------------------------------
	//�ж����������Ƿ�4��
	bool Has4SameCards(HandCards& pHc, _uint8 bCard)const;
	//�ж����Ƽ������Ƿ�4��
	bool Has4SameCardsX(HandCards& pHc, _uint8 bCard)const;
	//С��Ԫ
	bool IsXiaoSanYuan(const HandCards* p, const _uint8 CardsFormatBuf[HAND_CARD_KAN_MAX][4], const _uint8 Jiang);
	//����Ԫ
	bool IsDaSanYuan(const HandCards* p, const _uint8 CardsFormatBuf[HAND_CARD_KAN_MAX][4], const _uint8 Jiang);
	//���Ĺ�
	bool IsMingSiGui(const HandCards* p, const _uint8 CardsFormatBuf[HAND_CARD_KAN_MAX][4], const _uint8 Jiang, _uint8 cbCard, bool bQuanPinDao = false);
	//���Ĺ�
	bool IsAnSiGui(const HandCards* p, const _uint8 CardsFormatBuf[HAND_CARD_KAN_MAX][4], const _uint8 Jiang, _uint8 cbCard, bool bQuanPinDao = false);
	//������
	bool IsKa5Xing(const HandCards* p, _uint8 cbCard);
	//���
	bool IsQingDui(HandCards* p);
	//������ ȫ��258 ��������
	bool IsJiangJiangHu(const HandCards* p, const _uint8 buf[4][4], const _uint8 Jiang);
	//��һɫ
	bool IsQingYiSe(const HandCards* p);
	//����9
	bool IsDai19(HandCards* p, _uint8 buf[4][4], int length, _uint8 Jiang);
	//��һɫ19 �ӷ���	//���ж�ȫ��
	bool IsQing19(const HandCards* p, const _uint8 buf[4][4], _uint8 Jiang);
	//ȫ19
	bool IsQuan19(const HandCards& p, const _uint8 buf[4][4], _uint8 Jiang);
	//ȫ19
	bool IsQuan19Wind(const HandCards& p, const _uint8 buf[4][4], _uint8 Jiang);
	//��һɫ
	bool IsHunYiSe(const HandCards* p);									
	//��һɫ
	bool IsZiYiSe(const HandCards* p);
	//ʮ���޺�
	bool IsShiBaLuoHan(const HandCards* p);
	//��С��Ԫ
	int IsDaXiaoSanYuan(const HandCards* p, const _uint8 CardDataNum[MJ_TYPE_COMMON][10]);
	//��С��ϲ
	int IsDaXiaoSiXi(const HandCards* p, const _uint8 CardDataNum[MJ_TYPE_COMMON][10]);
	//ȫ���˲��㰵��
	bool IsQuanqiuren(const HandCards* pHc);
	//ȫ���˱���258����
	bool IsQuanQiuRen(const HandCards* pHc);
	//������
	bool IsPengPengHu(const HandCards* p, const _uint8 CardsFormatBuf[4][4], _uint8 Jiang);
	//13��
	bool Is13Yao(const HandCards* p, _uint8 c, _uint8 CardsFormatBuf[4][4], _uint8 Jiang);
	//����--���ܲ���
	bool IsMenQing(const HandCards* p);
	//����--������
	bool IsMenQing_02(const HandCards* p);
	//һ����
	bool IsYiTiaoLong(const HandCards* p);
	//һ����_Ƽ���齫
	bool IsYiTiaoLong_PX(const HandCards* p, const _uint8 CardsFormatBuf[HAND_CARD_KAN_MAX][4], const _uint8 Jiang);
//-----------------------------------�����������ж�--------------------------------------
	//7��
	bool Is7Dui(const HandCards* p);
	//��7��
	bool IsQing7Dui(const HandCards* p);
	//��7��												
	bool IsLong7Dui(const HandCards* p, _uint8 &cbGenNum);
	//����7��
	bool IsQingLong7Dui(HandCards* p);
	//����											
	bool IsDanDiao(const HandCards& p);
	//��һɫ
	bool IsJiangYiSe(const HandCards* p);			
	//13��
	bool Is13Luan(const HandCards* p);
	//13��	
    bool Is13Feng(const HandCards* p);
	//�ҽ���
	bool IsLuanJiangHu(const HandCards* p);
//-----------------------------------�ƻ�������--------------------------------------------
	//������
	void SortCard(_uint8 p[], _uint8 len);
	//��ȡ��
	bool GetJiang(_uint8* p, int len, int &JiangIndex, _uint8 &jiang, _uint8 *pRestCardBuf);
	//�Ѿ��������ֻ�ɫ�������ֲ�����,�������򷵻�false
	bool EstimateFixCard(HandCards *pHC, _uint8 cbCardData);
	//��ͨ��������ж�--����
	bool FormatCards(HandCards *hcs, MJ_win_pattern& pattern);
	//��ͨ��������ж�--����
	bool FormatCards(std::vector<_uint8> vec, MJ_win_pattern& pattern);
	//��ͨ��������ж�--һ��
	bool FormatCards(HandCards *hcs, _uint8 CardsFormatBuf[4][4], _uint8 &Jiang);	
	//�Ƴ���
	void RemoveCard(HandCards *hcs, _uint8 c, int length);
	void RemoveCard(HandCards *pHC, _uint8 c);
	void RemoveCard(_uint8 sCard[], _uint8 cblen, _uint8 c);
	void RemoveCard(HandCards *hcs, _uint8 *c, int length);
	//�ж����Ƿ����
	bool HasCard(const HandCards *hc, _uint8 c);
	//�����Ʒ���
	void PyCard(const _uint8 handcard[14], int nLen, MJ_hand_buffer PaiCard[MJ_TYPE_JIAN]);
//-----------------------------------����ӵĺ����ж�------------------------------------
	//����Ӻ����ж�
	bool FormatCards_Laizi(_uint8 card[14], int nLen, int nLaiZiNum);
	//�����
	bool LaiZi_CanHu(MJ_hand_buffer stData, int LaiZiNum, bool bAllShunZi = false);
	//��ȡ��Ҫ��ӵĸ���
	void get_need_hun_num(MJ_hand_buffer& stData, int nNeedNum, int &nNeedMinNum);
	//��ȡ���˳����Ҫ��ӵĸ���
	void get_need_hun_num_by_shun(MJ_hand_buffer stData, int nNeedNum, int &nNeedMinNum);
	//���ÿ�ֻ�ɫ ���Ƿ����
	bool FormatCards_Laizi_3_Group(const _uint8 card[14], int nLen, int nLaiZiNum);

	//--------------------------------��Ӻ��Ƶĵڶ��ַ���--------------------------------
	//��Ӻ����жϵڶ��ַ���
	bool FormatCards_LaiZi(HandCards *hcs, MJ_win_pattern& pattern, int lai_sum);
	bool FormatCards_LaiZi(std::vector<_uint8> vec, MJ_win_pattern& pattern, int lai_sum);
	//����жϽ�
	bool CheckJiang_LaiZi(std::vector<_uint8> vec, MJ_win_pattern& pattern, int lai_sum_remain);
	//���˳������
	bool CheckShunZi_LaiZi(std::vector<_uint8> vec, MJ_win_pattern& pattern, int lai_sum_remain);
	//���������� ������ 
	bool CheckSubVector_LaiZi(std::vector<_uint8> vec, std::vector<_uint8> &kan, int lai_sum_remain);
	//��ȡcard��˳�� ����Ҫ���������
	_uint8 GetShunZi_LaiZi(const std::vector<_uint8> &vec, _uint8 card = INVALID_MJ_CARD);
	//��ȡ�����һ������
	void GetKan(const std::vector<_uint8> &vec_kan, MJ_win_pattern& pattern);
	//���������
	bool CheckSubVector(std::vector<_uint8> vec, std::vector<_uint8> &kan);
	// ��ȡ��
	void GetJiang(std::vector<_uint8> &vec, std::vector<_uint8> &out_jiang);
	//ɾ��ָ�� �� ���ٸ�
	bool EraseCard(std::vector<_uint8> &vec, _uint8 card, _uint8 sum = 1);
	//���˳��
	bool CheckShunZi(std::vector<_uint8> &vec, _uint8 card, _uint8 sum = 1);
	//��ȡû����ӵ���
	int GetNoLaiZiHandCard(const HandCards &card_in, HandCards &card_out , _uint8 m_laizi);
	int GetNoLaiZiHandCard(const HandCards &card_in, HandCards &card_out,const std::vector<_uint8>& vec_laizi);
	//��ȡ��ӵ�����
	int GetLaiZiSum(const HandCards &phc, _uint8 m_laizi);
	int GetLaiZiSum(const HandCards &phc, const std::vector<_uint8>& vec_laizi);
//-----------------------------------�������������--------------------------------------
	//�����--��С��Ԫ
	int L_IsDaXiaoSanYuan(const HandCards* p, const _uint8 CardDataNum[MJ_TYPE_COMMON][10], int iLaiziNum);
	//�����--��С��ϲ
	int L_IsDaXiaoSiXi(const HandCards* p, const _uint8 CardDataNum[MJ_TYPE_COMMON][10], int iLaiziNum);
	//�����--13��
	bool L_Is13Yao(const HandCards* p, _uint8 c, _uint8 CardDataNum[MJ_TYPE_COMMON][10], int nHunNum);                  
	//�����--7��
	bool L_IsQiDui(const HandCards* p, _uint8 c, _uint8 CardDataNum[MJ_TYPE_COMMON][10], int nHunNum);                 
	//�����--��ȡ���7����Ҫ��Ӹ���
	int L_IsQiDui_02(const HandCards* p, _uint8 c, const _uint8 CardDataNum[MJ_TYPE_COMMON][10], int nHunNum);
	//�����--������
	bool L_IsPengPengHu(const HandCards* p, _uint8 c, _uint8 CardDataNum[MJ_TYPE_COMMON][10], int nHunNum);
	//�����--��������Ҫ��ӵĸ���
	int L_IsPengPengHu_02(const HandCards* p, _uint8 c, _uint8 CardDataNum[MJ_TYPE_COMMON][10], int nHunNum);
private:
	MJ_hand_buffer m_PaiCard[MJ_TYPE_JIAN];        //���ֻ�ʽ��������
//---------------------------------��ͨ�淨�ڶ��ֺ����㷨------------------------------------
	//���ú����㷨
	//����������Ƿ��ܹ�ȫ����Ϊ��
	bool CheckSubVectorPeng(HandCards &no_laizi_handcard, int lai_sum_remain);
	bool CheckSubVectorPeng(std::vector<_uint8> &vec, int lai_sum_remain);
	//�ȼ����Ƴɹ�֮��
	//������۾�
	bool CheckQing19(HandCards &no_laizi_handcard);
	//����Ƿ����߶�
	bool Check7Dui(HandCards &no_laizi_handcard, int lai_sum);
	//��������� �Ƕ�����ɶ���
	bool CheckSubVectorDui(std::vector<_uint8> vec, int lai_sum);
};

#endif
