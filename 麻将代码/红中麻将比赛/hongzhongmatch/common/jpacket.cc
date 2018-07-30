#include "jpacket.h"
#include "../proto/proto.h"
#include "log.h"

extern Log log;


void xorfunc(char* cString, int strLen)
{
	const int KEY = 13;
	for (int i = 0; i < strLen; i++)
	{
		cString[i] ^= KEY;
	}
}
Jpacket::Jpacket()
{
	useless = 0;
	protected_bound = 0;
}

Jpacket::~Jpacket()
{
}

std::string& Jpacket::tostring()
{
    return str;
}
const std::string& Jpacket::tostring()const
{
	return str;
}
Json::Value& Jpacket::tojson()
{
    return val;
}

void Jpacket::end()
{    
	char out[0x20000] = "";
	
	strncpy(out, val.toStyledString().c_str(), sizeof(out));
	out[sizeof(out) - 1] = 0;
	int strLen = strlen(out);
			
	JsonHeaderEx header;
	header.cmd = val["cmd"].asInt();
	header.d.length = strLen;
	
	//log.debug("SendDataStyled:%s", out);
	xorfunc(out, strLen);

    str.clear();
    str.append((const char *)&header, sizeof(struct JsonHeaderEx));
    str.append(out);
}


int Jpacket::parse(char* buf, int len)
{
	Json::Reader reader;
	
	xorfunc(buf, len);
	if (reader.parse(buf, val) < 0)
	{
		return -1;
	}
	//log.debug("RecvDataStyled:%s", val.toStyledString().c_str());
	return 0;
}

int Jpacket::safe_check()
{
	if (val.type() != Json::objectValue)
	{
		log.error("%s parse error!\n", __FUNCTION__);
		return -2;
	}
	if (val["cmd"].isNull())
	{
		log.error("%s cmd is null!\n", __FUNCTION__);
		return -2;
	}	

	if (!val["cmd"].isNumeric())
	{
		log.error("%s cmd is not number!\n", __FUNCTION__);
		return -2;
	}	
	int cmd = val["cmd"].asInt();
	return  cmd;
	switch (cmd)
	{
		
		case SYS_SETUP:
			if( val["password"].isString() && val["setup"].isNumeric() && val["type"].isNumeric()
						&& val["one"].isNumeric() && val["two"].isNumeric() && val["three"].isNumeric() 
						&& val["four"].isNumeric() && val["five"].isNumeric() && val["six"].isNumeric() )
			{
				return cmd;
			}
			return -1;
		case CLIENT_LOGIN_REQ: 
			if ( val["uid"].isNumeric() && val["skey"].isString() &&
						val["vid"].isNumeric() && val["zid"].isNumeric() )
			{
				return cmd;
			}
			return -1;		
		case CLIENT_BET_REQ:
			if( val["action"].isNumeric() )
			{
				return cmd;
			}
			return -1;		
		case CLIENT_FACE_REQ :
			if( val["faceid"].isNumeric() )
			{
				return cmd;
			}
			return -1;			
		case CLIENT_EMOTION_REQ:
			if( val["target_seatid"].isNumeric() && val["type"].isNumeric() )
			{
				return cmd;
			}
			return -1;		
		case CLIENT_BOX_REQ:
			if(  val["type"].isNumeric() && val["value"].isNumeric() );
			{
				return cmd;
			}
			return -1;
		case  CLIENT_ACTIVE_REQ:
			if( val["active"].isNumeric() )
			{
				return cmd;
			}
			return -1;
		case CLIENT_BETTING_REQ:
			if( val["seatid"].isNumeric() && val["my_bet"].isNumeric() )
			{
				return cmd;
			}
			return -1;	
		case CLIENT_RETURN_REQ:
			if( val["reward_money"].isNumeric() )
			{
				return cmd;
			}
			return -1;
		default:
			return cmd;
	}
	log.error("%s cmd is %d\n", __FUNCTION__, cmd);
	return -1;
}

