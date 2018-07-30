#include "table.h"
#include "common/log.h"

#include "location/table_xianningmajiang.h"
extern Log log;


Table* Table::CreateTable(const Json::Value& table_val, const Json::Value& val)
{
	const string& loc = table_val["location"].asString();	

	if ( json_check_play_type_id(val))
	{
		MJ_SUB_ID SubID;
		
		if (loc == "xianningmajiang")
		{
			json_get_play_type_id(val, 5380, SubID);
			if (!SubID.IsNull())
			{
				switch (SubID[0])
				{
				case 5380001:
					log_info("%s local:%s table:%s \n", __FUNCTION__, loc.c_str(), "xianningmajiang");
					return new Table_xianningmajiang();
				}
			}
			return new Table_xianningmajiang();
		}
	
	}
	log_info("%s local:%s table:%s\n", __FUNCTION__, loc.c_str(), "oragin"); 
	return NULL  ;
}


