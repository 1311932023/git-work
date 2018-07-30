#ifndef PARSE_H__
#define PARSE_H__

#pragma pack(push)
#pragma pack(1)

struct ProtoHeader
{
	int cmdID;
	int length;	
};


int PackHeader(char *pBuf, int cmd, int bodysize);
//void UnPackHeader(ProtoHeader* pHeader, char* pBuf);
bool CheckHeader_Proto(ProtoHeader* pHeader);
int  Reverse(int a);


#pragma pack(pop)
#endif