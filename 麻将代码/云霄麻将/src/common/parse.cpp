#include <arpa/inet.h>
#include <string.h>
#include "parse.h"
#include "../libnormalmahjong/common.h"

int PackHeader(char *pBuf, int cmd, int bodysize)
{
	ProtoHeader stHead = {0};	
 	stHead.cmdID = cmd;
 	stHead.length = bodysize;
	memcpy(pBuf, &stHead, sizeof(stHead));
	return sizeof(stHead);
}
/*
void UnPackHeader(ProtoHeader* pHeader, char* pBuf)
{
	char * buf = pBuf;
	memcpy(&(pHeader->cmdID), buf, sizeof(pHeader->cmdID));
	buf = buf + sizeof(pHeader->cmdID);
			 
	memcpy(&(pHeader->length), buf, sizeof(pHeader->length));	

}
*/

bool CheckHeader_Proto(ProtoHeader* pHeader)
{
	if (pHeader->cmdID <= 0 || pHeader->length <= 0 || pHeader->length >= DEF_BUF_LEN)
	{
     	return false;
	}
	return true;
}


int Reverse(int a)
{
	union {
		int i;
		char c[4];
	} u, r;

	u.i = a;
	r.c[0] = u.c[3];
	r.c[1] = u.c[2];
	r.c[2] = u.c[1];
	r.c[3] = u.c[0];

	return r.i;

}
