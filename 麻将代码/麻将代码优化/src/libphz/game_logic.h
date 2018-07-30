
#ifndef __GAME_LOGIC_H__
#define __GAME_LOGIC_H__
#include "common.h"
#include "mjcardtype.h"
#include <vector>


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

typedef MJ_BUFFER<OpState, 20> MJ_opinfo;

//-----------------------------麻将游戏逻辑-------------------------------
class CGameLogic
{
public:
	CGameLogic(void);
	~CGameLogic(void);
	
public:
//-----------------------------牌库操作-----牌类型操作--------------------------
	//洗牌
	void RandomCard(_uint8 cbCard[], _uint8 cbLength, int iTableID);
	//添加牌
	void AddCard(HandCards *pHc, _uint8 bCard);
    //出牌
	bool ChuPai(HandCards *pHc, _uint8 bCard);
    //杠操作
	void ExecuteGang(HandCards *pHc, _uint8 bOpCardChairID, _uint8 bOperateCard, int iOpType);
	//碰操作
	void ExecutePeng(HandCards *pHc, _uint8 bOpCardChairID, _uint8 bOperateCard);
	//左吃操作
	void ExecuteLeftChi(HandCards *handcard, _uint8 chairID, _uint8 cbOperateCard);
	//中吃操作
	void ExecuteCenterChi(HandCards *handcard, _uint8 chairID, _uint8 cbOperateCard);
	//右吃操作
	void ExecuteRightChi(HandCards *handcard, _uint8 chairID, _uint8 cbOperateCard);
	//获取杠信息
	int GetGangInfo(const HandCards*pHc, MJ_opinfo& ganginfo);
	int GetGangInfo(const HandCards*pHc, MJ_opinfo& ganginfo, MJ_gang &lou_gang, _uint8 laizi_card); 
	int GetGangInfo(const HandCards*pHc, MJ_opinfo& ganginfo, MJ_gang &lou_gang, MJ_BUFFER<_uint8, 14> showListenInfos[GAME_PLAYER], _uint8 ChairID);
	//获取碰杠信息
	int GetPengGang(HandCards *pHc, _uint8 bCard, MJ_opinfo& GangCardInfo);
	int GetPengGang(HandCards *pHc, _uint8 bCard, MJ_opinfo& GangCardInfo, _uint8 laizi_card);
	//获取弯杠	
	bool GetWanGang(const HandCards* pHc, MJ_opinfo& ganginfo, _uint8 bCard = INVALID_MJ_CARD);
	//获取自杠
	bool GetZhiGang(const HandCards* pHc, MJ_opinfo& ganginfo, _uint8 bCard = INVALID_MJ_CARD);
	//获取暗杠
	bool GetAnGang(const HandCards* pHc, MJ_opinfo& ganginfo, _uint8 bCard = INVALID_MJ_CARD);
	//获取吃信息
	int GetChi(const HandCards *p, _uint8 cbCard, MJ_opinfo& ChiInfo);
	//获取牌的逻辑下张
	_uint8 NextCard(_uint8 card);
	//获取牌的逻辑上张
	_uint8 PrevCard(_uint8 card);
	//检查是否为一组合
	bool checke_combine(_uint8 cbCard1, _uint8 cbCard2, _uint8 cbCard3);
	//检查是否为顺子组合
	bool checke_combine_shun(_uint8 cbCard1, _uint8 cbCard2, _uint8 cbCard3);
	//检查是否为对子组合
	bool checke_combine_dui_zi(_uint8 cbCard1, _uint8 cbCard2);
	//获取手牌每种牌的张数
	void GetCardCount(const HandCards* pHc, _uint8 CardDataNum[MJ_TYPE_COMMON][10]);
	//获取门前每种牌的张数
	void GetCardCountExt(const HandCards* pHc, _uint8 CardDataNum[MJ_TYPE_COMMON][10]);
	//获取手牌和门前指定牌的总张数
	int GetCardCountX(const HandCards* pHc, _uint8 card);
	//获取指定牌在门前中的张数
	int GetFixedCardCount(const HandCards* pHc, _uint8 bCard);
	//获取各个花色的牌的总张数
	bool GetCardTypeCount(const HandCards* pHc, _uint8 CardTypeCount[MJ_TYPE_COMMON]);
public:
	//获取色子
	Shai_Set GetShaizi(_uint8 iCount);
public:
//------------------------------------胡牌类型判断-------------------------------
	//判断手牌中牌是否4张
	bool Has4SameCards(HandCards& pHc, _uint8 bCard)const;
	//判断手牌及牌区是否4张
	bool Has4SameCardsX(HandCards& pHc, _uint8 bCard)const;
	//小三元
	bool IsXiaoSanYuan(const HandCards* p, const _uint8 CardsFormatBuf[HAND_CARD_KAN_MAX][4], const _uint8 Jiang);
	//大三元
	bool IsDaSanYuan(const HandCards* p, const _uint8 CardsFormatBuf[HAND_CARD_KAN_MAX][4], const _uint8 Jiang);
	//明四归
	bool IsMingSiGui(const HandCards* p, const _uint8 CardsFormatBuf[HAND_CARD_KAN_MAX][4], const _uint8 Jiang, _uint8 cbCard, bool bQuanPinDao = false);
	//暗四归
	bool IsAnSiGui(const HandCards* p, const _uint8 CardsFormatBuf[HAND_CARD_KAN_MAX][4], const _uint8 Jiang, _uint8 cbCard, bool bQuanPinDao = false);
	//卡五星
	bool IsKa5Xing(const HandCards* p, _uint8 cbCard);
	//清对
	bool IsQingDui(HandCards* p);
	//将将胡 全是258 的碰碰胡
	bool IsJiangJiangHu(const HandCards* p, const _uint8 buf[4][4], const _uint8 Jiang);
	//清一色
	bool IsQingYiSe(const HandCards* p);
	//带幺9
	bool IsDai19(HandCards* p, _uint8 buf[4][4], int length, _uint8 Jiang);
	//混一色19 加风牌	//不判断全风
	bool IsQing19(const HandCards* p, const _uint8 buf[4][4], _uint8 Jiang);
	//全19
	bool IsQuan19(const HandCards& p, const _uint8 buf[4][4], _uint8 Jiang);
	//全19
	bool IsQuan19Wind(const HandCards& p, const _uint8 buf[4][4], _uint8 Jiang);
	//混一色
	bool IsHunYiSe(const HandCards* p);									
	//字一色
	bool IsZiYiSe(const HandCards* p);
	//十八罗汉
	bool IsShiBaLuoHan(const HandCards* p);
	//大小三元
	int IsDaXiaoSanYuan(const HandCards* p, const _uint8 CardDataNum[MJ_TYPE_COMMON][10]);
	//大小四喜
	int IsDaXiaoSiXi(const HandCards* p, const _uint8 CardDataNum[MJ_TYPE_COMMON][10]);
	//全求人不算暗杠
	bool IsQuanqiuren(const HandCards* pHc);
	//全求人必须258做将
	bool IsQuanQiuRen(const HandCards* pHc);
	//碰碰胡
	bool IsPengPengHu(const HandCards* p, const _uint8 CardsFormatBuf[4][4], _uint8 Jiang);
	//13幺
	bool Is13Yao(const HandCards* p, _uint8 c, _uint8 CardsFormatBuf[4][4], _uint8 Jiang);
	//门清--暗杠不算
	bool IsMenQing(const HandCards* p);
	//门清--暗杠算
	bool IsMenQing_02(const HandCards* p);
	//一条龙
	bool IsYiTiaoLong(const HandCards* p);
	//一条龙_萍乡麻将
	bool IsYiTiaoLong_PX(const HandCards* p, const _uint8 CardsFormatBuf[HAND_CARD_KAN_MAX][4], const _uint8 Jiang);
//-----------------------------------非正规则型判断--------------------------------------
	//7对
	bool Is7Dui(const HandCards* p);
	//清7对
	bool IsQing7Dui(const HandCards* p);
	//龙7对												
	bool IsLong7Dui(const HandCards* p, _uint8 &cbGenNum);
	//清龙7对
	bool IsQingLong7Dui(HandCards* p);
	//单吊											
	bool IsDanDiao(const HandCards& p);
	//将一色
	bool IsJiangYiSe(const HandCards* p);			
	//13乱
	bool Is13Luan(const HandCards* p);
	//13风	
    bool Is13Feng(const HandCards* p);
	//乱将胡
	bool IsLuanJiangHu(const HandCards* p);
//-----------------------------------牌基本操作--------------------------------------------
	//牌排序
	void SortCard(_uint8 p[], _uint8 len);
	//获取将
	bool GetJiang(_uint8* p, int len, int &JiangIndex, _uint8 &jiang, _uint8 *pRestCardBuf);
	//已经碰了两种花色，第三种不能碰,不能碰则返回false
	bool EstimateFixCard(HandCards *pHC, _uint8 cbCardData);
	//普通胡牌组合判断--二版
	bool FormatCards(HandCards *hcs, MJ_win_pattern& pattern);
	//普通胡牌组合判断--二版
	bool FormatCards(std::vector<_uint8> vec, MJ_win_pattern& pattern);
	//普通胡牌组合判断--一版
	bool FormatCards(HandCards *hcs, _uint8 CardsFormatBuf[4][4], _uint8 &Jiang);	
	//移出牌
	void RemoveCard(HandCards *hcs, _uint8 c, int length);
	void RemoveCard(HandCards *pHC, _uint8 c);
	void RemoveCard(_uint8 sCard[], _uint8 cblen, _uint8 c);
	void RemoveCard(HandCards *hcs, _uint8 *c, int length);
	//判断牌是否存在
	bool HasCard(const HandCards *hc, _uint8 c);
	//将手牌分类
	void PyCard(const _uint8 handcard[14], int nLen, MJ_hand_buffer PaiCard[MJ_TYPE_JIAN]);
//-----------------------------------带癞子的胡牌判断------------------------------------
	//带癞子胡牌判断
	bool FormatCards_Laizi(_uint8 card[14], int nLen, int nLaiZiNum);
	//带癞子
	bool LaiZi_CanHu(MJ_hand_buffer stData, int LaiZiNum, bool bAllShunZi = false);
	//获取需要癞子的个数
	void get_need_hun_num(MJ_hand_buffer& stData, int nNeedNum, int &nNeedMinNum);
	//获取组成顺子需要癞子的个数
	void get_need_hun_num_by_shun(MJ_hand_buffer stData, int nNeedNum, int &nNeedMinNum);
	//针对每种花色 看是否符合
	bool FormatCards_Laizi_3_Group(const _uint8 card[14], int nLen, int nLaiZiNum);

	//--------------------------------癞子胡牌的第二种方法--------------------------------
	//癞子胡牌判断第二种方法
	bool FormatCards_LaiZi(HandCards *hcs, MJ_win_pattern& pattern, int lai_sum);
	bool FormatCards_LaiZi(std::vector<_uint8> vec, MJ_win_pattern& pattern, int lai_sum);
	//癞子判断将
	bool CheckJiang_LaiZi(std::vector<_uint8> vec, MJ_win_pattern& pattern, int lai_sum_remain);
	//癞子顺子三个
	bool CheckShunZi_LaiZi(std::vector<_uint8> vec, MJ_win_pattern& pattern, int lai_sum_remain);
	//查找相关组合 子序列 
	bool CheckSubVector_LaiZi(std::vector<_uint8> vec, std::vector<_uint8> &kan, int lai_sum_remain);
	//获取card成顺子 所需要的癞子数量
	_uint8 GetShunZi_LaiZi(const std::vector<_uint8> &vec, _uint8 card = INVALID_MJ_CARD);
	//获取组合中一坎的牌
	void GetKan(const std::vector<_uint8> &vec_kan, MJ_win_pattern& pattern);
	//检测子序列
	bool CheckSubVector(std::vector<_uint8> vec, std::vector<_uint8> &kan);
	// 获取将
	void GetJiang(std::vector<_uint8> &vec, std::vector<_uint8> &out_jiang);
	//删除指定 牌 多少个
	bool EraseCard(std::vector<_uint8> &vec, _uint8 card, _uint8 sum = 1);
	//检测顺子
	bool CheckShunZi(std::vector<_uint8> &vec, _uint8 card, _uint8 sum = 1);
	//获取没有癞子的牌
	int GetNoLaiZiHandCard(const HandCards &card_in, HandCards &card_out , _uint8 m_laizi);
	int GetNoLaiZiHandCard(const HandCards &card_in, HandCards &card_out,const std::vector<_uint8>& vec_laizi);
	//获取癞子的数量
	int GetLaiZiSum(const HandCards &phc, _uint8 m_laizi);
	int GetLaiZiSum(const HandCards &phc, const std::vector<_uint8>& vec_laizi);
//-----------------------------------带癞子特殊牌型--------------------------------------
	//带癞子--大小三元
	int L_IsDaXiaoSanYuan(const HandCards* p, const _uint8 CardDataNum[MJ_TYPE_COMMON][10], int iLaiziNum);
	//带癞子--大小四喜
	int L_IsDaXiaoSiXi(const HandCards* p, const _uint8 CardDataNum[MJ_TYPE_COMMON][10], int iLaiziNum);
	//带癞子--13幺
	bool L_Is13Yao(const HandCards* p, _uint8 c, _uint8 CardDataNum[MJ_TYPE_COMMON][10], int nHunNum);                  
	//带癞子--7对
	bool L_IsQiDui(const HandCards* p, _uint8 c, _uint8 CardDataNum[MJ_TYPE_COMMON][10], int nHunNum);                 
	//带癞子--获取组成7对需要癞子个数
	int L_IsQiDui_02(const HandCards* p, _uint8 c, const _uint8 CardDataNum[MJ_TYPE_COMMON][10], int nHunNum);
	//带癞子--碰碰胡
	bool L_IsPengPengHu(const HandCards* p, _uint8 c, _uint8 CardDataNum[MJ_TYPE_COMMON][10], int nHunNum);
	//带癞子--碰碰胡需要癞子的个数
	int L_IsPengPengHu_02(const HandCards* p, _uint8 c, _uint8 CardDataNum[MJ_TYPE_COMMON][10], int nHunNum);
private:
	MJ_hand_buffer m_PaiCard[MJ_TYPE_JIAN];        //各种花式的牌数据
//---------------------------------普通玩法第二种胡牌算法------------------------------------
	//常用胡牌算法
	//检测子序列是否能够全部成为坎
	bool CheckSubVectorPeng(HandCards &no_laizi_handcard, int lai_sum_remain);
	bool CheckSubVectorPeng(std::vector<_uint8> &vec, int lai_sum_remain);
	//先检测胡牌成功之后
	//检测清幺九
	bool CheckQing19(HandCards &no_laizi_handcard);
	//检测是否是七对
	bool Check7Dui(HandCards &no_laizi_handcard, int lai_sum);
	//检测子序列 是都能组成对子
	bool CheckSubVectorDui(std::vector<_uint8> vec, int lai_sum);
};

#endif
