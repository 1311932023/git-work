#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>
#include <json/json.h>
#include <string>
#include "jpacket.h"
#include "client_buffer.h"

#define SERVER_RESTART_PROC  2555   //重启程序 
 
int main(int argc, char *argv[])
{
	/*if(argc<3)
	{
	     printf("usage:%s ip port \n",argv[0]);
	     exit(1);          
	}*/
    
	/*步骤1：创建socket*/
	int sockfd=socket(AF_INET,SOCK_STREAM,0);
	if(sockfd<0)
	{
	     perror("socket error");
	     exit(1);
	}    
 
	struct sockaddr_in serveraddr;
	memset(&serveraddr,0,sizeof(serveraddr));
	serveraddr.sin_family=AF_INET;
	serveraddr.sin_port=htons(atoi("33301"));
 
	//主机字节序转换成网络字节序
	inet_pton(AF_INET, "127.0.0.1", &serveraddr.sin_addr.s_addr);
    
    /*步骤2:客户端调用connect函数连接到服务器*/
    if(connect(sockfd,(struct sockaddr*)&serveraddr,
              sizeof(serveraddr))<0)
    {
         perror("connect error");
         exit(1);
    }

	Jpacket packet;
	packet.val["cmd"] = SERVER_RESTART_PROC;
	packet.val["datetime"] = (int)time(NULL);
	packet.end();
	std::string str = packet.tostring();
	JsonHeaderEx t;
	Buffer* ptm = new Buffer(str.c_str(), str.length(), sizeof(t.cmd));

    if(write(sockfd, ptm->dpos(), ptm->nbytes()) != ptm->nbytes())
    {
         perror("write error");
    }
 }