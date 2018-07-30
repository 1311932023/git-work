#ifndef _GAME_SERIALIZE_HPP_
#define _GAME_SERIALIZE_HPP_

#include "common/boostHeader.hpp"
#include "rr_match.h"
#include "sd_match.h"
#include "rr_waiwei_match.h"
#include "location/table_chengdumj.h"
#include "trustee/CTrustee.h"
#include "trustee/CXueZhanTrustee.h"
#include "location/table_changsha_zhuanzhuan.h"
#include "location/table_chengdumj.h"

template<class Archive>
void Game::save(Archive & ar, const unsigned int version) const
{
	printf("Game::save\n");
	ar.register_type(static_cast<MatchExt *>(NULL));
	ar.register_type(static_cast<RJMatch *>(NULL));
	ar.register_type(static_cast<RRMatch *>(NULL));
	ar.register_type(static_cast<RRWW_Match *>(NULL));
	ar.register_type(static_cast<SDMatch *>(NULL));
	ar.register_type(static_cast<Table_chengdumj *>(NULL));
	ar.register_type(static_cast<CTrustee *>(NULL));
	ar.register_type(static_cast<CXueZhanTrustee *>(NULL));
	ar.register_type(static_cast<CS_zhuanzhuan_Fantype_to_FanNum *>(NULL));
	ar.register_type(static_cast<chongqingmj_Fantype_to_FanNum *>(NULL));
	ar & all_match_map;
	ar & del_match_map;
	std::string prizeStr = prize_json.toStyledString();
	ar & prizeStr;
}

template<class Archive>
void Game::load(Archive & ar, const unsigned int version)
{
	printf("Game::load\n");
	ar.register_type(static_cast<MatchExt *>(NULL));
	ar.register_type(static_cast<RJMatch *>(NULL));
	ar.register_type(static_cast<RRMatch *>(NULL));
	ar.register_type(static_cast<RRWW_Match *>(NULL));
	ar.register_type(static_cast<SDMatch *>(NULL));
	ar.register_type(static_cast<Table_chengdumj *>(NULL));
	ar.register_type(static_cast<CTrustee *>(NULL));
	ar.register_type(static_cast<CXueZhanTrustee *>(NULL));
	ar.register_type(static_cast<CS_zhuanzhuan_Fantype_to_FanNum *>(NULL));
	ar.register_type(static_cast<chongqingmj_Fantype_to_FanNum *>(NULL));
	ar & all_match_map;
	ar & del_match_map;
	std::string prizeStr;
	ar & prizeStr;
	Json::Reader reader;
	reader.parse(prizeStr, prize_json);
}


#endif
