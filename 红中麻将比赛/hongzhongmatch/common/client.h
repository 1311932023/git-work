#ifndef __CLIENT_H__
#define __CLIENT_H__

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include <errno.h>
#include <list>
#include <assert.h>
#include <ev.h>
#include <json/json.h>
#include <string>
#include "parse.h"
#include "jpacket.h"
#include "../libnormalmahjong/common.h"
#include "client_buffer.h"

#define NODATA_TIMEOUT	(40)

class Client;
typedef void (*GAME_DEL_CLIENT)(Client *client, bool del_oldclient);
typedef int(*GAME_DISPATCH)(Client *client);
typedef int(*GAME_SEND_FILTER)(Client* client, std::list<Buffer*>& _write_q);

enum parse_state
{
    PARSE_HEADER    	= 0,
    PARSE_BODY      	= 1,
    PARSE_ERR       	= 2,
    PARSE_CROSS_DOMAIN	= 3,
};

class Player;

union Client_Buffer
{
	struct
	{		
		ProtoHeader proto_head;
		char proto_data[0];		
	};	
	struct
	{
		JsonHeader	json_head;
		char json_data[0];		
	};	
};
class InitClient
{
public:
	InitClient(struct ev_loop* lp, GAME_DEL_CLIENT game_del_client, GAME_DISPATCH game_dispatch_client, GAME_DISPATCH game_dispatch_server, GAME_SEND_FILTER filter);
};
enum E_CLIENT_TYPE
{
	E_PB_PROTO,
	E_PB_JSON,
};
class Client 
{
public:
	const Client_Buffer* m_Buffer;
	Player        *player;
	std::string   string_ip;		
	long long useless;
	Jpacket       packet;	
	long long useless0;
	int fd;
	int uid;
	bool is_err;	
	bool is_close;
	bool is_old_client;
	bool position;	
	unsigned char clientType;    
	bool m_HeartBeat;
	unsigned char m_ErrorCount;
	int	tid;
private:
	
	ev_io         _ev_write;
	ev_io         _ev_read;

	ev_timer      _ev_nodata_timer;
	ev_tstamp     _ev_nodata_tstamp;

public:	
	std::list<Buffer*> _write_q;
public:	
	void list_push_back(Buffer* buf);
	bool list_empty();
	Buffer* list_front();
	void list_pop_front();
	Client(int fd_in, bool flag,  int cliType = 0);
	virtual ~Client();	
	
	static void destroy(Client *client, bool del_oldclient=false);
	static void pre_destroy(Client *client, bool del_oldclient=false);
	static void read_cb_proto(struct ev_loop *loop, struct ev_io *w, int revents);
	static void read_cb_json(struct ev_loop *loop, struct ev_io *w, int revents);
	static void write_cb(struct ev_loop *loop, struct ev_io *w, int revents);
	static void nodata_timer_cb(struct ev_loop *loop, struct ev_timer *w, int revents);
	int send(const char *buf, unsigned int len, unsigned int shift = 0);	
	int send_as_json(const std::string &res);
	unsigned int safe_writen(const char *buf, unsigned int len);
	static void join_table_failed(Client *client);
	void set_positon(bool pos);
	void Set_Beat_Time(int timer_tstamp);		
	void Heart_Beat();

	template<typename TYPE>
	int unicast(int iMsg, const TYPE& p);

	template<class TYPE>
	bool parse_proto(TYPE& obj);
	
	static struct ev_loop *loop;
private:	
	static GAME_DEL_CLIENT del_client;
	static GAME_DISPATCH dispatch_client;
	static GAME_DISPATCH dispatch_server;
	static GAME_SEND_FILTER game_send_filter;	
	friend class InitClient;

public:	
	int send(const std::string& str) { return send(str.c_str(), str.size()); }

};

template<typename TYPE>
int Client::unicast(int iMsg, const TYPE& p)
{
	char buf[20480];
	int bodysize = p.ByteSize();
	int headsize = PackHeader(buf, iMsg, bodysize);
	assert((headsize + bodysize) < static_cast<int>(sizeof(buf)));
	p.SerializeToArray(buf + headsize, bodysize);
	send(buf, headsize + bodysize);	
	return 0;
}
template<class TYPE>
bool Client::parse_proto(TYPE& obj)
{
	return obj.ParseFromArray(m_Buffer->proto_data, m_Buffer->proto_head.length);
}

#endif // endif __CLIENT_H__
