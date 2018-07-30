/*------------------------------------------
File Name		:   SCMahJongLogic.h
Author			:	chenyonghua
Version			:	1.0
Date			:	2016.05.23
Descripition	:	牌型判断逻辑
-------------------------------------------*/

#ifndef __GAME_LOGIC_H__
#define __GAME_LOGIC_H__
#include "common.h"
#include "mjcardtype.h"
#include "mj_common2.h"
#include "mj_common4.h"
#include <vector> 
//using namespace   std ;


//筛子
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

struct CardNode_count
{
	_uint8 card;
	_uint8 count;
	CardNode_count() {}
	CardNode_count(_uint8 c, _uint8 s) :card(c), count(s){}
};

typedef MJ_BUFFER<OpState, 20> MJ_opinfo;

//int power(int base, int times);

class CGameLogic
{
public:
	CGameLogic(void);
	~CGameLogic(void);
	
public:
    /**
     *  @function: RandomCard
     *  @brief: 打乱牌
     *  @cbCard: 要打乱的牌
     *  @cbLength: 牌的个数
     *  @return: 无
    **/
	void RandomCard(_uint8 cbCard[], _uint8 cbLength, int iTableID);
    /**
     *  @function: AddCard
     *  @brief: 把牌加到手牌种
     *  @pHc: 手牌
     *  @bCard: 要加入手牌的牌
     *  @return: 无
    **/
	int  AddCard(HandCards *pHc, _uint8 bCard);
    /**
     *  @function: ChuPai
     *  @brief: 出牌
     *  @pHc: 玩家手牌
     *  @bCard: 要打出的牌
     *  @return: 是否能成功打出该牌
    **/
	bool ChuPai(HandCards *pHc, _uint8 bCard);
    
	void ExecuteGang(HandCards *pHc, _uint8 bOpCardChairID, _uint8 bOperateCard, int iOpType);
    /**
     *  @function: ExecutePeng
     *  @brief: 玩家执行碰操作
     *  @bOpCardChairID: 引起操作的玩家椅子ID(纪录碰了谁的牌)
     *  @pHc: 玩家的手牌
     *  @bOperateCard: 操作的牌
     *  @return: 无
    **/
	void ExecutePeng(HandCards *pHc, _uint8 bOpCardChairID, _uint8 bOperateCard);
    /**
     *  @function: ChangeCard
     *  @brief: 传进四个玩家的牌，开始换牌
     *  @stUserCard: 四个玩家选择换的牌
     *  @return: 无
    **/
	void ExecuteLeftChi(HandCards *handcard, _uint8 chairID, _uint8 cbOperateCard);
	void ExecuteCenterChi(HandCards *handcard, _uint8 chairID, _uint8 cbOperateCard);
	void ExecuteRightChi(HandCards *handcard, _uint8 chairID, _uint8 cbOperateCard);
	int GetGangInfo(const HandCards*pHc, MJ_opinfo& GangCardInfo);
	int GetPengGang(HandCards *pHc, _uint8 bCard, MJ_opinfo& GangCardInfo);

	//_uint8 GetPengGang(HandCards *pHc, _uint8 bCard, bool bMo = false);
	//void ChangeCard(tagChangeCard stUserCard[4]);

	//_uint8 GetPeng(HandCards *pHc, _uint8 bCard);
	//_uint8 GetGang(HandCards *pHc, _uint8 bCard, bool bIsGetCard);	
	bool GetWanGang(const HandCards* pHc, MJ_opinfo& ganginfo, _uint8 bCard = INVALID_MJ_CARD);
	bool GetZhiGang(const HandCards* pHc, MJ_opinfo& ganginfo, _uint8 bCard = INVALID_MJ_CARD);
	bool GetAnGang(const HandCards* pHc, MJ_opinfo& ganginfo, _uint8 bCard = INVALID_MJ_CARD);

	//万条同：1-9循环   东南西北中发白循环   
	_uint8 NextCard(_uint8 card);
	//万条同：1-9循环   东南西北循环    中发白循环   
	_uint8 NextCard2(_uint8 card);
	_uint8 CheckNextCard(_uint8 bCard, _uint8 max_card);
	_uint8 PrevCard(_uint8 card);
public:
	Shai_Set GetShaizi(_uint8 iCount);
public:
	bool Has4SameCards(HandCards& pHc, _uint8 bCard)const;
	bool Has4SameCardsX(HandCards& pHc, _uint8 bCard)const;
	
	
	bool IsQingDui(HandCards* p);												//清对
	
	bool IsQingYiSe(const HandCards* p);												//清一色

	bool IsDai19(HandCards* p, _uint8 buf[4][4], int length, _uint8 Jiang);			//带幺9	
	bool IsQing19(const HandCards* p, const _uint8 buf[4][4], _uint8 Jiang);			//混一色19 加风牌	//不判断全风
	bool IsQuan19(const HandCards& p, const _uint8 buf[4][4], _uint8 Jiang);			//全19
	bool IsQuan19Wind(const HandCards& p, const _uint8 buf[4][4], _uint8 Jiang);			//全19
	bool IsHunYiSe(const HandCards* p);												//混一色
	bool IsZiYiSe(const HandCards* p);											//字一色
	bool IsShiBaLuoHan(const HandCards* p);		//十八罗汉
	int IsDaXiaoSanYuan(const HandCards* p, const _uint8 CardDataNum[MJ_TYPE_COMMON][10]);
	int IsDaXiaoSiXi(const HandCards* p, const _uint8 CardDataNum[MJ_TYPE_COMMON][10]);
	int L_IsDaXiaoSanYuan(const HandCards* p, const _uint8 CardDataNum[MJ_TYPE_COMMON][10], int iLaiziNum);
	int L_IsDaXiaoSiXi(const HandCards* p, const _uint8 CardDataNum[MJ_TYPE_COMMON][10], int iLaiziNum);
	//////////////////////////////////////////////////////////////////////////
	//非正规则型判断

	bool is_qi_dui(const HandCards* p);													//7对, 3番
	
	bool IsQing7Dui(const HandCards* p);												//清7对， 5番
	bool IsLong7Dui(const HandCards* p, _uint8 &cbGenNum);								//龙7对， 5番

	bool IsQingLong7Dui(HandCards* p);											//清龙7对,6番

	bool IsDanDiao(const HandCards& p);
public:
	void SortCard(_uint8 p[], _uint8 len);
	bool GetJiang(_uint8* p, int len, int &JiangIndex, _uint8 &jiang, _uint8 *pRestCardBuf);
	//已经碰了两种花色，第三种不能碰,不能碰则返回false

	bool EstimateFixCard(HandCards *pHC, _uint8 cbCardData);
	bool FormatCards(HandCards *hcs, _uint8 CardsFormatBuf[4][4], _uint8 &Jiang);	
	

	void RemoveCard(HandCards *hcs, _uint8 c, int length);
	void RemoveCard(HandCards *pHC, _uint8 c);
	void RemoveCard(_uint8 sCard[], _uint8 cblen, _uint8 c);
	void RemoveCard(HandCards *hcs, _uint8 *c, int length);	
	bool HasCard(const HandCards *hc, _uint8 c);	
	
public:
	bool IsQuanqiuren(const HandCards* pHc);
	int GetChi(const HandCards *p, _uint8 cbCard, MJ_opinfo& ChiInfo);
	bool IsPengPengHu(const HandCards* p, const _uint8 CardsFormatBuf[4][4], _uint8 Jiang);
	//bool Is13Yao(const HandCards* p, _uint8 c, _uint8 CardsFormatBuf[4][4], _uint8 Jiang);
	bool IsMenQing(const HandCards* p);		//算暗杠
	bool IsMenQing_02(const HandCards* p);	//不算暗杠
	//bool IsYiTiaoLong(const HandCards* p, const MJ_win_pattern&);

public:
	
	bool FormatCards_Laizi(_uint8 card[14], int nLen, int nLaiZiNum);	
	bool LaiZi_CanHu(MJ_hand_buffer stData, int LaiZiNum, bool bAllShunZi = false);
	void PyCard(const _uint8 handcard[14], int nLen, MJ_hand_buffer PaiCard[MJ_TYPE_JIAN]);
	void get_need_hun_num(MJ_hand_buffer& stData, int nNeedNum, int &nNeedMinNum);
	void get_need_hun_num_by_shun(MJ_hand_buffer stData, int nNeedNum, int &nNeedMinNum);
	bool checke_combine(_uint8 cbCard1, _uint8 cbCard2, _uint8 cbCard3);
	bool checke_combine_shun(_uint8 cbCard1, _uint8 cbCard2, _uint8 cbCard3);
	bool checke_combine_dui_zi(_uint8 cbCard1, _uint8 cbCard2);
	
public:
	//bool L_Is13Yao(const HandCards* p, _uint8 c, _uint8 CardDataNum[MJ_TYPE_COMMON][10], int nHunNum);                  //13幺

	bool L_IsQiDui(const HandCards* p, _uint8 c, _uint8 CardDataNum[MJ_TYPE_COMMON][10], int nHunNum);                  //七对
	bool L_IsQiDui_03(const HandCards* p, _uint8 c, _uint8 CardDataNum[MJ_TYPE_COMMON][10], int nHunNumint, int& iLongQiDui);                  //七对
	int L_IsQiDui_02(const HandCards* p, _uint8 c, const _uint8 CardDataNum[MJ_TYPE_COMMON][10], int nHunNum);                  //七对
	
	bool L_IsPengPengHu(const HandCards* p, _uint8 c, _uint8 CardDataNum[MJ_TYPE_COMMON][10], int nHunNum);             //碰碰胡	
	//int L_IsPengPengHu_02(const HandCards* p, _uint8 c, _uint8 CardDataNum[MJ_TYPE_COMMON][10], int nHunNum);
	
public:
	void GetCardCount(const HandCards* pHc, _uint8 CardDataNum[MJ_TYPE_COMMON][10]);
	void GetCardCountExt(const HandCards* pHc, _uint8 CardDataNum[MJ_TYPE_COMMON][10]);
	int GetCardCountH(const HandCards* pHc, _uint8 bCard);
	int GetCardCountX(const HandCards* pHc, _uint8 bCard);
	int GetFixedCardCount(const HandCards* pHc, _uint8 bCard);
	bool FormatCards_Laizi_3_Group(const _uint8 card[14], int nLen, int nLaiZiNum);
	bool GetCardTypeCount(const HandCards* pHc, _uint8 CardTypeCount[MJ_TYPE_COMMON]);

	int GetCardCount(const HandCards* pHc, MJ_BUFFER<CardNode_count, HAND_CARD_SIZE_MAX>& h);
private:
	MJ_hand_buffer m_PaiCard[MJ_TYPE_JIAN];        //各种花式的牌数据



public:
	//game_logic 移植过来的
	/*
	*	无赖子胡牌算
	*/
	bool FormatCards(HandCards *hcs, MJ_win_pattern& pattern);
	bool FormatCards(std::vector<_uint8> vec, MJ_win_pattern& pattern);
	/*
	*	癞子胡牌算
	*/
	bool FormatCards_LaiZi(HandCards *hcs, MJ_win_pattern& pattern, int lai_sum);
	bool FormatCards_LaiZi(std::vector<_uint8> vec, MJ_win_pattern& pattern, int lai_sum);
	//癞子混将
	bool CheckJiang_LaiZi(std::vector<_uint8> vec, MJ_win_pattern& pattern, int lai_sum_remain);
	//癞子混三个
	bool CheckShunZi_LaiZi(std::vector<_uint8> vec, MJ_win_pattern& pattern, int lai_sum_remain);
	//查找子序列 
	bool CheckSubVector_LaiZi(std::vector<_uint8> vec, std::vector<_uint8> &kan, int lai_sum_remain);
	//获取card成顺子 所需要的癞子数量
	_uint8 GetShunZi_LaiZi(const std::vector<_uint8> &vec, _uint8 card = INVALID_MJ_CARD);


	void GetKan(const std::vector<_uint8> &vec_kan, MJ_win_pattern& pattern);

	//检测子序列
	bool CheckSubVector(std::vector<_uint8> vec, std::vector<_uint8> &kan);

	// 获取将
	void GetJiang(std::vector<_uint8> &vec, std::vector<_uint8> &out_jiang);

	//删除制定 牌 多少个
	bool EraseCard(std::vector<_uint8> &vec, _uint8 card, _uint8 sum = 1);

	//检测顺子
	bool CheckShunZi(std::vector<_uint8> &vec, _uint8 card, _uint8 sum = 1);

	//获取没有癞子的牌
	int GetNoLaiZiHandCard(const HandCards &card_in, HandCards &card_out , _uint8 m_laizi);
	int GetNoLaiZiHandCard(const HandCards &card_in, HandCards &card_out,const std::vector<_uint8>& vec_laizi);

	//获取癞子的数量
	int GetLaiZiSum(const HandCards &phc, _uint8 m_laizi);
	int GetLaiZiSum(const HandCards &phc, const std::vector<_uint8>& vec_laizi);

	int RemoveLaiziCard(HandCards& pHc, const std::vector<_uint8>& gui, MJ_BUFFER<_uint8, 8>& joker);
	int AddLaiziCard(HandCards& pHc, const MJ_BUFFER<_uint8, 8>& joker);
	//常用胡牌算法
	//检测子序列是否能够全部成为坎
	bool CheckSubVectorPeng(HandCards &no_laizi_handcard, int lai_sum_remain);
	bool CheckSubVectorPeng(std::vector<_uint8> &vec, int lai_sum_remain);

	//下面判断需要之前判断已经胡牌
	//检测清幺九
	bool CheckQing19(HandCards &no_laizi_handcard);

	//检测是否是七对
	bool Check7Dui(HandCards &no_laizi_handcard, int lai_sum);
	//检测子序列 是都能组成对子
	bool CheckSubVectorDui(std::vector<_uint8> vec, int lai_sum);
	
	/*
	 *	边张和卡张需要在Check_Lisning，Check_DaJiao时保存修改m_bCurCard的值，否则会奔溃
	 */
	bool Is_BianZhang(const HandCards* pHc, _uint8 bHuCard, const MJ_win_pattern& oPattern);
	bool Is_KaZhang(const HandCards* pHc, _uint8 bHuCard, const MJ_win_pattern& oPattern);
	bool Is_DaKuanZhang(const HandCards* pHc, _uint8 bHuCard, const MJ_win_pattern& oPattern);
	bool Find_Jiang258(const HandCards* pHc, MJ_hand_buffer& hc);
	bool Find_SanTong(const HandCards* pHc, MJ_hand_buffer& hc);
	bool Find_JieJieGao(const HandCards* pHc, MJ_hand_buffer& hc);
	int Get19Count(const HandCards* pHc);
	bool is_jiang_jiang_hu(const HandCards *phand);
	
	bool hai_di_lao_yue(_uint8 , _uint8 , bool , _uint8 );
	
	bool hai_di_pao(bool card_pool_isEmpty, bool fanpao);
	
	int  hao_hua_qi_dui(const HandCards *pHc);
	
	int cur_card_count( const  HandCards *pHc, _uint8 c);
	
	int yi_tiao_long(const HandCards *pHc);
	
	int DuiDaoHu( HandCards *pHc);

	int dajue(const HandCards *pHc, int hupai);

	int HasOneAnGang(const HandCards *p);


	int xiaohua_haohua_qi_dui(const HandCards *pHc, _uint8 hu_pai = 0);
	int xiaohua_haohua_ting(const HandCards *phc);
	int in_vector( std::vector<int> &all_Vec, int c);

	int Getchi_laizi(const HandCards *phc, _uint8 cbCard, MJ_opinfo &ChiInfo,  std::vector<int> &all_laizi_vec);
	bool qidui_laizi(const HandCards *p, int laize_a, int laizi_b, int &haohua_qiduicount);
	bool qingyise_King(const HandCards *p, int laizi_a, int laizi_b);
	int GetGangInfo_King(const HandCards*pHc, MJ_opinfo& ganginfo, int King_a, int king_b);


};

#endif
