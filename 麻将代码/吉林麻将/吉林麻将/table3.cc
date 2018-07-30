#include "table.h"
//#include "common/log.h"

 
#include "location/table_jilin.h"


Table* Table::CreateTable(const Json::Value& table_val, const Json::Value& val)
{
	 //const string& loc = table_val["location"].asString();	
	//std::string res = table_val.toStyledString().c_str();
	   //log_debug("json_str %s  ", res.c_str());

	//std::string  json_val = val.toStyledString().c_str();
	//log_debug("json_val [%s]" , json_val.c_str() );	

	return  new  Table_jilin(4);

	
	return NULL;

}

