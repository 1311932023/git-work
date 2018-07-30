#include "client.h"
#include <sys/time.h>
#include  "log.h"
#include "../proto/proto.h"
#include "player.h"

//extern Log log;

char policy_cross_domain[] = "<cross-domain-policy>\
<allow-access-from domain=\"*\" to-ports=\"*\" /></cross-domain-policy>\0";
char policy_file[] = "<policy-file-request/>";

GAME_DEL_CLIENT Client::del_client = NULL;
GAME_DISPATCH Client::dispatch_client = NULL;
GAME_DISPATCH Client::dispatch_server = NULL;
GAME_SEND_FILTER Client::game_send_filter = NULL;

struct ev_loop* Client::loop = NULL;

InitClient::InitClient(struct ev_loop *zjh_loop, GAME_DEL_CLIENT game_del_client, GAME_DISPATCH game_dispatch_client, GAME_DISPATCH game_dispatch_server, GAME_SEND_FILTER filter)
{
	Client::del_client = game_del_client;
	Client::dispatch_client = game_dispatch_client;
	Client::dispatch_server = game_dispatch_server;
	Client::loop = zjh_loop;
	Client::game_send_filter = filter;
}

Client::Client(int fd_in, bool flag, int cliType) :
m_HeartBeat(false)
{

	is_close = 0;
	fd = fd_in;
	is_err = 0;
	uid = -1;
	player = NULL;
	position = POSITION_WAIT;
	clientType = cliType;

	zid = 0;
	vid = 0;

	_ev_read.data = this;

	//pthread_mutex_init(&list_mutex,NULL);
	//1是用jacksson
	if (1 == clientType)
	{
		ev_io_init(&_ev_read, Client::read_cb_json, fd, EV_READ);   // json 
	}
	else
	{
		ev_io_init(&_ev_read, Client::read_cb_proto, fd, EV_READ);   // pb .j
	}

	ev_io_start(loop, &_ev_read);

	ev_io_init(&_ev_write, Client::write_cb, fd, EV_WRITE);

	_ev_nodata_timer.data = this;
	_ev_nodata_tstamp = 180;
	ev_timer_init(&_ev_nodata_timer, Client::nodata_timer_cb,
		_ev_nodata_tstamp, _ev_nodata_tstamp);
	if (clientType == 0)
	{  
		//客户端需要
		ev_timer_start(loop, &_ev_nodata_timer);
	}

	m_ErrorCount = 0;
	useless = 0;
	useless0 = 0;
}

Client::~Client()
{
	log_debug("delete  client !!");
	_ev_read.data = NULL;
	_ev_write.data = NULL;
	_ev_nodata_timer.data = NULL;
	ev_io_stop(loop, &_ev_read);	
	ev_io_stop(loop, &_ev_write);	
	ev_timer_stop(loop, &_ev_nodata_timer);
	
	while (!list_empty()) 
	{
		delete (list_front());
		list_pop_front();
	}

	shutdown(fd, SHUT_RDWR);
	close(fd);
	useless = -1;
	useless0 = -1;
}


void Client::destroy(Client *client, bool del_oldclient)
{   
	if (client == NULL)
	{
		log_error("destroy client error, client is NULL ");
		return;
	}    
    del_client(client, del_oldclient);
}

void Client::pre_destroy(Client *client, bool del_oldclient)
{  
	if (client == NULL)
	{
		log_error("pre_destroy client error, client is NULL ");
		return;  
	}  
	if (client->list_empty() )
	{
		log_debug("into this  ");
        Client::destroy(client, del_oldclient);
	}
	else
	{		
		if (del_oldclient)
		{			
			Client::destroy(client, del_oldclient);
		}
		else
		{
			log_debug("isclose  !!");
			client->is_close = 1;
		}	
	}
			
}

void Client::write_cb(struct ev_loop *loop, struct ev_io *w, int revents)
{
	Client *self = (Client*) w->data;

	if (self == NULL)
	{
		return;
	}

	if (self->list_empty()) {
		//log_debug("stop write event\n");
		ev_io_stop(EV_A_ w);
		if (self->is_close == 1) 
		{
			Client::destroy(self);
			return;
		}
		return;
	}

	game_send_filter(self, self->_write_q);
	if (self->_write_q.empty())
		return;

	Buffer* buffer = self->list_front();
	ssize_t written = write(self->fd, buffer->dpos(), buffer->nbytes());
	//ssize_t written = ::send(self->fd, buffer->dpos(), buffer->nbytes(), 0);
	if (written < 0) {
		if (errno == EAGAIN || errno == EINPROGRESS || errno == EINTR) {
			log_info("write failed[%s]\n", strerror(errno));
			return;
		}
		/* todo close this client */
		log_error("unknow err in written fd[%d] err:%s ", self->fd, strerror(errno));
		Client::destroy(self);
		return;
	}	
	buffer->nshift(written);
	if (buffer->nbytes() == 0) {
		self->list_pop_front();
		delete buffer;
	}		
}

void Client::nodata_timer_cb(struct ev_loop *loop, struct ev_timer *w, int revents)
{
    Client *self = (Client*)w->data;    
    if (self != NULL) 
	{		 
		if (!self->m_HeartBeat)
		{
			log_info("%s heart beat is missing, destroy client uid:%d!\n", __FUNCTION__, self->uid);
			self->m_HeartBeat = false;
			pre_destroy(self);			
		}
		else
		{
			self->m_HeartBeat = false;
		}		
    } 
	
}

int Client::send(const char *buf, unsigned int len, unsigned int shift)
{
	//assert((clientType == 0 && shift == 0) || (clientType == 1 && shift == sizeof(int)));
	if (fd > 0) 
	{
		if (list_empty()) 
		{
			_ev_write.data = this;
			ev_io_start(loop, &_ev_write);
			//log_debug("start write event\n");
		}
		Buffer* ptm = new Buffer(buf, len, shift);		
		list_push_back(ptm);		
		return 0;
	}
	return -1;
}


int Client::send_as_json(const std::string &res)
{
	static JsonHeaderEx t;	
	return send(res.c_str(), res.length(), sizeof(t.cmd));
	//return safe_writen(res.c_str(), res.length());
}

unsigned int Client::safe_writen(const char *buf, unsigned int len)
{
	size_t nleft;
	ssize_t nwritten;
	const char *ptr;

	ptr = (char*) buf;
	nleft = len;

	while (nleft > 0) {
		nwritten = write(fd, ptr, nleft);
		//nwritten = ::send(fd, ptr, nleft, 0);
		if (nwritten <= 0) {
			if (errno == EINTR)
				nwritten = 0;
			else {
				is_err = 1;
				return -1;
			}
		}
		nleft -= nwritten;
		ptr += nwritten;
	}

	return len;
}



void Client::join_table_failed(Client *client)
{
	Jpacket packet;
	packet.val["cmd"] = SERVER_LOGIN_ERR_UC;
	packet.val["code"] = 2;
	packet.val["msg"] = "no table";
	packet.end();
	client->send_as_json(packet.tostring());

	Client::pre_destroy(client);
}

void Client::set_positon(int pos)
{
	position = pos;
}

void Client::list_push_back(Buffer* buf)
{
	//pthread_mutex_lock(&list_mutex);
	_write_q.push_back(buf);
	//pthread_mutex_unlock(&list_mutex);
}

bool Client::list_empty()
{
	bool bRet = false;
	//pthread_mutex_lock(&list_mutex);
	bRet = _write_q.empty();
	//pthread_mutex_unlock(&list_mutex);
	return bRet;
}

Buffer* Client::list_front()
{
	Buffer* ptm = NULL;
	//pthread_mutex_lock(&list_mutex);
	ptm = _write_q.front();
	//pthread_mutex_unlock(&list_mutex);
	return ptm;
}

void Client::list_pop_front()
{
	//pthread_mutex_lock(&list_mutex);
	_write_q.pop_front();
	//pthread_mutex_unlock(&list_mutex);
}

void Client::Set_Beat_Time(int timer_tstamp)
{   
	ev_timer_stop(loop, &_ev_nodata_timer);
	ev_timer_set(&_ev_nodata_timer, timer_tstamp , timer_tstamp);
	ev_timer_again(loop, &_ev_nodata_timer);	
}

void Client::Heart_Beat()
{
	m_HeartBeat = true;
}