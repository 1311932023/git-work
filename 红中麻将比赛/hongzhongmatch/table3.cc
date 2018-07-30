#include "table.h"
#include "common/log.h"
#include "location/table_zz_laizi.h"
#include "location/table_hong_zhong.h"

extern Log log;

Table* Table::CreateTable_changsha(const Json::Value& table_val, const Json::Value& val)
{
	const string& loc = table_val["location"].asString();

	MJ_SUB_ID SubID;
	json_get_play_type_id(val, 2430, SubID);
	if (!SubID.Find(2430001))
	{
		log.info("%s local:%s table:%s \n", __FUNCTION__, loc.c_str(), "zz_laizi");
		return new Table_zz_laizi();
	}
	log.info("%s local:%s table:%s \n", __FUNCTION__, loc.c_str(), "zhuanzhuan");
	return new Table_cs_zhuanzhuan();
}




Table* Table::CreateTable(const Json::Value& table_val, const Json::Value& val)
{
	const string& loc = table_val["location"].asString();	

	if (json_check_play_type_id(val))
	{
		MJ_SUB_ID SubID;

		if (loc == "changsha")
		{
			json_get_play_type_id(val, 2441, SubID);
			if (SubID.Find(2441001))
			{
				return CreateTable_changsha(table_val, val);
			}
			json_get_play_type_id(val, 2442, SubID);
			if (SubID.Find(2442001))
			{
				return CreateTable_changsha(table_val, val);
			}

			json_get_play_type_id(val, 2451, SubID);
			if (SubID.Find(2451001))
			{
				log.info("%s local:%s table:%s\n", __FUNCTION__, loc.c_str(), "hongzhong");
				return new Table_hong_zhong();
			}
			json_get_play_type_id(val, 2452, SubID);
			if (SubID.Find(2452001))
			{
				log.info("%s local:%s table:%s\n", __FUNCTION__, loc.c_str(), "hongzhong3p");
				return new Table_hong_zhong();
			}
		}
	}
	log.info("%s local:%s table:%s\n", __FUNCTION__, loc.c_str(), "oragin");
	return new Table_cs_zhuanzhuan();	
}
