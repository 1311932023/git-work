#include "common.h"
#include "gameLogic.h"
#include <iostream>

int main(int argc, char *argv[])
{
	_uint8 cbChiCard = 0x07;
	GameLogic m_GameLogic;
	ChiInfo m_ChiInfo;
	_uint8 cbCard[] = {0x04,0x05,0x06,0x06,0x06,0x16,0x08,0x09,0x0a};
	_uint8 m_cbCardIndex[MAX_INDEX] = {0};
	m_GameLogic.SwitchToCardIndex(cbCard, sizeof(cbCard), m_cbCardIndex);
	// m_GameLogic.GetChiCardInfo(m_cbCardIndex, cbChiCard, m_ChiInfo);
	// for(_uint8 i = 0; i < m_ChiInfo.size(); ++i){
	// 	fprintf(stdout, "m_ChiInfo[%d] chicard[[ ", i);
	// 	for(_uint8 j = 0; j < 3; ++j){
	// 		fprintf(stdout, "%d ", m_ChiInfo[i].chiCardInfo.cbCardData[j]);
	// 	}
	// 	fprintf(stdout, "]]\n");
	// 	for(_uint8 k = 0; k < m_ChiInfo[i].secondChiInfo.size(); ++k){
	// 		fprintf(stdout, "m_ChiInfo[%d] second chicard[[ ", i);
	// 		for(_uint8 j = 0; j < 3; ++j){
	// 			fprintf(stdout, "%d ", m_ChiInfo[i].secondChiInfo[k].chiCardInfo.cbCardData[j]);
	// 		}
	// 		fprintf(stdout, "]]\n");
	// 		for(_uint8 l = 0; l < m_ChiInfo[i].secondChiInfo[k].thirdChiInfo.size(); ++l){
	// 			fprintf(stdout, "m_ChiInfo[%d] third chicard[[ ", i);
	// 			for(_uint8 j = 0; j < 3; ++j){
	// 				fprintf(stdout, "%d ", m_ChiInfo[i].secondChiInfo[k].thirdChiInfo[l].cbCardData[j]);
	// 			}
	// 			fprintf(stdout, "]]\n");
	// 		}
	// 	}
	// }
	CAnalyseItemArray AnalyseItemArray;
	m_GameLogic.AnalyseCard(m_cbCardIndex, AnalyseItemArray);
	return 0;
}
