#include "CardPool_ReadJson.h"

#include <fstream>
#include <iostream>
#include <stdlib.h>

#include "common/log.h" 


using namespace std;

CardPool_ReadJson::CardPool_ReadJson():m_MaxCount(0)
{
}


CardPool_ReadJson::~CardPool_ReadJson()
{
}


void CardPool_ReadJson::ReadJsonFile(std::string str_file_name , int  n_all  )
{
	m_TestFileName = str_file_name;
	ifstream ifs;
	ifs.open(m_TestFileName.data() , std::ios::binary);
	if ( !ifs )
	{		
		log_debug("error !!");
		abort();  
		return; 
	}
	Json::Reader json_read;
	Json::Value rootValue;
	if ( !json_read.parse(ifs , rootValue ) )
	{
		log_error("open error !!");
		abort();
		return;
	}
	//log_debug("sss");

	ParseJsonValue(rootValue ,  n_all  );
	ifs.close();
}

void CardPool_ReadJson::ParseJsonValue(Json::Value  &data  ,   int all_card  )
{


	m_MaxCount = all_card;
	 // log_debug("maxcount [%d]", m_MaxCount); 

	int size = data["card"].size();
	log_debug("max size  %d %d", m_MaxCount, size);
	m_CardData.Clear();

	for (int i = 0; i < size; i++)
	{
		int n = data["card"][i]["cards"].size(); 
		n = std::min(136,   all_card ); 
		for ( int j = 0; j < n; j++)
		{
			int ct = data["card"][i]["cards"][j].asInt();
			//	log_debug("cars[%d]  pai[%d] ", j, ct);
			if (ct > 10)
			{
				int i = ct / 10;
				int j = ct % 10;
				if (j == 0)
					abort();

				ct = i * 0x10 + j;
			} 
			if (ct > 0x43 || ct <= 0  )
				abort();

			m_CardData.Add( ct);  						
		}
	}		
}

void CardPool_ReadJson::ParseOneRecord(std::string record)
{
	log_debug("record  %s" , record.c_str()  );

	string str_tmp;
	while (record.length() )
	{
		size_t pos = record.find(',');
		str_tmp = record.substr(0, pos );

		while (str_tmp[0] == ' ' || str_tmp[0] == '\n' || str_tmp[0] == '\t' || str_tmp[0] == '\r')
		{
			str_tmp = str_tmp.substr(1);
		}
		str_tmp = str_tmp.substr(2);	
		if (m_CardData.Size() >= 136)
			return;
		m_CardData.Add( Atoi(str_tmp,16));
		if ( record.length() == 1 )
		{
			
			break;
		}
		else
		{
			record = record.substr(pos + 1);
		}
	}
}

_tint32 CardPool_ReadJson::GetTestCard(_uint8 *pout, _uint32 out_size)
{
	if ( m_CardData.Size() == 0 )
	{
		return 0;
	}

	if (_uint32( m_CardData.Size()) < out_size )
	{
		out_size = _uint32(m_CardData.Size());
	}
	for (_uint32 i = 0 ; i < out_size ; ++i  )
	{
		pout[i] = m_CardData[i];   
		//log_debug("pout i[%d]   [%x]"  , i ,  m_CardData[i] );
	}
	return out_size;
}

_uint32 CardPool_ReadJson::GetMaxCount()
{
	return m_MaxCount;
}

_uint32 CardPool_ReadJson::Atoi(const std::string num, int base)
{
	_uint32 ret = 0;
	_uint32 tbase = 1;
	_uint32 rmove = 0;
	switch (base)
	{
	case 2:
		rmove = 1;
		break;
	case 8:
		rmove = 3;
		break;
	case 16:
		rmove = 4;
		break;
	default:
		return 0;
	}
	for (int i = num.length() - 1; i >= 0; --i)
	{
		if (num[i] <= '9' && num[i] >= '0')
		{
			ret = ret + (num[i] - '0')* tbase;
			tbase = tbase << rmove;
		}
		else
		{
			return 0;
		}
	}
	return ret;
}

