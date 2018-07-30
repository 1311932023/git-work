#include "jpacket.h"
#include "../proto/proto.h"
#include <sys/time.h>
#include "log.h"

//extern Log log;

void xorfunc(std::string &nString)
{
	const int KEY = 13;
	int strLen = (nString.length());
	char *cString = (char*)(nString.c_str());
	
	for (int i = 0; i < strLen; i++)
	{
		*(cString + i) = (*(cString + i) ^ KEY);
	}
}
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
	
	strncpy(out, val.toStyledString().c_str()  , sizeof(out));
	out[sizeof(out) - 1] = 0;
	int strLen = strlen(out);
			
	JsonHeaderEx header;
	header.cmd = val["cmd"].asInt();
	header.d.length = strLen;
	
	//log_debug("SendDataStyled:%s", out);
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
	if (len < 10240)
		log_debug("RecvDataStyled:%s", val.toStyledString().c_str());

	return 0;
}

int Jpacket::sefe_check()
{
	if (val.type() != Json::objectValue)
	{
		log_error("%s parse error!\n", __FUNCTION__);
		return -2;
	}
	if (val["cmd"].isNull())
	{
		log_error("%s cmd is null!\n", __FUNCTION__);
		return -2;
	}	

	if (!val["cmd"].isNumeric())
	{
		log_error("%s cmd is not number!\n", __FUNCTION__);
		return -2;
	}	
	int cmd = val["cmd"].asInt();
	return  cmd;
	switch (cmd)
	{
		case SYS_ECHO:
			return cmd;
		case SYS_ONLINE:
			return cmd;
		case SYS_GET_USER_NUM:
			return cmd;
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
		case CLIENT_READY_REQ:
			return cmd;
		case CLIENT_BET_REQ:
			if( val["action"].isNumeric() )
			{
				return cmd;
			}
			return -1;
		case CLIENT_CHAT_REQ:			
			return cmd;			
		case CLIENT_FACE_REQ :
			if( val["faceid"].isNumeric() )
			{
				return cmd;
			}
			return -1;
		case CLIENT_INFO_REQ:
			return cmd;
		case CLIENT_LOGOUT_REQ:
			return cmd;
		case CLIENT_CHANGE_REQ:
			return cmd;
		case CLIENT_TABLE_INFO_REQ:
			return cmd;
		case CLIENT_EMOTION_REQ:
			if( val["target_seatid"].isNumeric() && val["type"].isNumeric() )
			{
				return cmd;
			}
			return -1;
		case CLIENT_PROP_REQ:
			return cmd;
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
		case CLIENT_UPTABLE_INFO_REQ:
			return cmd;
		case CLIENT_UPTABLE_APPLY_REQ:
			return cmd;
		case  CLIENT_DOWNTABLE_AHEAD_REQ:
			return cmd;
		case CLIENT_DOWNTABLE_REQ:
			return cmd;
		case CLIENT_RETURN_REQ:
			if( val["reward_money"].isNumeric() )
			{
				return cmd;
			}
			return -1;
		case CLINET_UPBANKER_REQ:
			return cmd;
		case CLINET_UPBANKER_INFO_REQ:
			return cmd;
		case CLINET_DOWN_BANKER_REQ:
			return cmd;
		case CLIENT_DAEMONIZE_REQ:
			return cmd;
		case CLINET_HEART_BEAT_REQ:
			return cmd;
		default:
			return -1;
	}
	return -1;
}

