#ifndef __ACP_H__
#define __ACP_H__
/********************************************************************
	author:		KEO
	
	purpose:	acp 
*********************************************************************/


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//接口封装//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#include <json/json.h>

 typedef void (*PFunction)(int); // 函数指针类型

int   acp_req(Json::Value  conf, PFunction gcall);


#endif//__ACP_H__