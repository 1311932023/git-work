#include "table.h"
#include "../location/table_pingxiangmj.h"


Table* Table::CreateLocationTable(std::string location)
{
	if(location == "pingxiangmajiang"){
		return (Table*)new Table_pingxiangmj();
	}
	return NULL;
}

