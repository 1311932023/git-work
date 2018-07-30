#ifndef __COMMON_MAHJONG_LOGIC_H__
#define __COMMON_MAHJONG_LOGIC_H__
#include "mj_common2.h"
#include "mjcardtype.h"



/*
 * 手牌有 一样的牌，不计算碰杠
 * uiCounts = 3, 表示所有有3张得牌
 * uiCounts = 4, 表示所有有4张得牌
 */
int HandCardsHasMultiCard(HandCards *pHc, _uint8 uiCounts, MJ_cards_type_buffer& pCardsType);
/*
 * 手牌有pCardsType 包含的任意牌返回 true，不计算碰杠
 */
bool HandCardsHasAnyCard(HandCards* pHc, const MJ_cards_type_buffer& pCardsType, MJ_cards_type_buffer& pOut);
/*
 * 手牌有特定数字的牌， 不计算碰杠
 */
bool HandCardsHasSpecialNumber(HandCards* pHc, _uint8 uiNumber);
/*
 * 手牌每种花色有几张
 */
void HandCardsSuitCounts(HandCards* pHc, _uint8 uiSuitCount[5]);

#endif