#ifndef __JPACKET_H__
#define __JPACKET_H__

#include <json/json.h>

struct JsonHeader {    
	unsigned int    length;	
};
struct JsonHeaderEx
{
	int cmd;
	JsonHeader d;
};
class Jpacket
{
public:
    Jpacket();
    virtual ~Jpacket();

    std::string& tostring();
	const std::string& tostring()const;
    Json::Value& tojson();

    void end();
    //int parse(std::string&);	
	int parse(char* buf, int len);
    int sefe_check();

public:
    std::string str;
	long long useless;
    Json::Value val;  
	long long protected_bound;
};

#endif
