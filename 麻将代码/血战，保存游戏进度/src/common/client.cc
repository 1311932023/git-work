#include "client.h"
#include "log.h"
#include "../proto/proto.h"
#include "../player.h"

extern Log log;

GAME_DEL_CLIENT Client::del_client = NULL;
GAME_DISPATCH Client::dispatch_client = NULL;
GAME_DISPATCH Client::dispatch_server = NULL;
GAME_SEND_FILTER Client::game_send_filter = NULL;

struct ev_loop* Client::loop = NULL;

InitClient::InitClient(struct ev_loop* lp, GAME_DEL_CLIENT game_del_client, GAME_DISPATCH game_dispatch_client, GAME_DISPATCH game_dispatch_server, GAME_SEND_FILTER filter)
{
	Client::del_client = game_del_client;
	Client::dispatch_client = game_dispatch_client;
	Client::dispatch_server = game_dispatch_server;	
	Client::game_send_filter = filter;

	Client::loop = lp;
}

Client::Client(int fd_in, bool flag, int cliType) :
m_HeartBeat(false)
{

	is_close = false;
	fd = fd_in;
	is_err = false;
	uid = -1;
	player = NULL;
	position = false;
	clientType = cliType;

	_ev_read.data = this;
		
	if (1 == clientType)
	{
		ev_io_init(&_ev_read, Client::read_cb_json, fd, EV_READ);
	}
	else
	{
		ev_io_init(&_ev_read, Client::read_cb_proto, fd, EV_READ);
	}

	ev_io_start(loop, &_ev_read);

	ev_io_init(&_ev_write, Client::write_cb, fd, EV_WRITE);

	_ev_nodata_timer.data = this;
	_ev_nodata_tstamp = 600;
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

	log_debug("%s ctor %p\n", __FUNCTION__, this);
}

Client::~Client()
{
	if (useless != 0)
	{
		log_error("%s delete multi times uid:%d\n", __FUNCTION__, uid);
	}
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
	log_debug("%s dtor %p\n", __FUNCTION__, this);
}


void Client::destroy(Client *client, bool del_oldclient)
{   
	if (client == NULL)
	{
		log_error("destroy client error, client is NULL \n");
		return;
	}    
    del_client(client, del_oldclient);
}

void Client::pre_destroy(Client *client, bool del_oldclient)
{  
	if (client == NULL)
	{
		log_error("pre_destroy client error, client is NULL \n");
		return;
	}
	if (client->list_empty())
	{
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
			client->is_close = true;
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
		if (self->is_close) {
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
			log_warn("write failed[%s]\n", strerror(errno));
			return;
		}
		/* todo close this client */
		log_error("unknow err in written fd[%d] err:%s\n", self->fd, strerror(errno));
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
	//log_debug("self<%#x>", self);
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
	assert((clientType == 0 && shift == 0) || (clientType == 1 && shift == sizeof(int)));
	if (fd > 0) {
		if (list_empty()) {
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
				is_err = true;
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

void Client::set_positon(bool pos)
{
	position = pos;
}

void Client::list_push_back(Buffer* buf)
{	
	_write_q.push_back(buf);
}

bool Client::list_empty()
{
	bool bRet = false;	
	bRet = _write_q.empty();	
	return bRet;
}

Buffer* Client::list_front()
{
	Buffer* ptm = NULL;	
	ptm = _write_q.front();	
	return ptm;
}

void Client::list_pop_front()
{
	_write_q.pop_front();
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