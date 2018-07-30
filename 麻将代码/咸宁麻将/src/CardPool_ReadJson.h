#ifndef CARDPOOL_READJSON_H
#define CARDPOOL_READJSON_H

#include <vector>
#include <string>
#include "common.h"
#include "json/json.h"
#include "libnormalmahjong/mj_common2.h"

class CardPool_ReadJson
{
public:
	CardPool_ReadJson();
	virtual ~CardPool_ReadJson();
	
	void ReadJsonFile(std::string str_file_name , int  );

	_tint32 GetTestCard(_uint8 *pout, _uint32 out_size); //获取测试文件 

	_uint32 GetMaxCount();//获取最大值
	
private:



	void ParseJsonValue(Json::Value  &data  , int   );  //解析json 数组
	void ParseOneRecord(std::string rcord);//解析一条数据
	_uint32 Atoi(const std::string num, int base);

	std::string m_TestFileName;	//测试文件名称
	//std::vector< std::vector<_uint8> > m_TestCardData;	//测试数据
	MJ_BUFFER<_uint8, POOL_CARD_MAX> m_CardData;	//读取的牌数据
	_uint32 m_MaxCount;		//牌初始化数量
};

#endif