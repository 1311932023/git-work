#include <sys/time.h>
#include "../phz.h"
#include "game.h"
#include "client.h"
#include "proto.h"
#include "log.h"

extern PHZ phz;
extern Log log;

Client::Client(int fd_in, bool flag, int cli_type) :
_ev_nodata_tstamp(10)
{
	_state = PARSE_HEADER;
	_cur_head_len = 0;
	body_len = 0;
	_header_p = (struct Header*) _header;
	is_close = 0;
	fd = fd_in;
	is_err = 0;
	uid = -1;
	player = NULL;
	position = POSITION_WAIT;
	cmd_type = 0;
	client_type = cli_type;
	_ev_read.data = this;
	if (client_type == 1) {
		ev_io_init(&_ev_read, Client::read_cb_json, fd, EV_READ);
	}
	else if(client_type == 0){
		ev_io_init(&_ev_read, Client::read_cb_proto, fd, EV_READ);
	}
	ev_io_start(phz.loop, &_ev_read);
	ev_io_init(&_ev_write, Client::write_cb, fd, EV_WRITE);
	_ev_nodata_timer.data = this;
	ev_timer_init(&_ev_nodata_timer, Client::nodata_timer_cb, _ev_nodata_tstamp, _ev_nodata_tstamp);
	if(flag){
		ev_timer_start(phz.loop, &_ev_nodata_timer);
	}
	log.debug("client[%d] open\n", fd);
}

Client::~Client()
{
	ev_io_stop(phz.loop, &_ev_read);
	ev_io_stop(phz.loop, &_ev_write);
	ev_timer_stop(phz.loop, &_ev_nodata_timer);
	while (!_write_q.empty()) {
		delete (_write_q.front());
		_write_q.pop_front();
	}
	close(fd);
	if(player != NULL){
		player->client = NULL;
	}
	log.info("~client fd[%d] uid[%d] destrutor\n", fd, uid);
}

void Client::destroy(Client *client, bool del_oldclient)
{
	if (client == NULL){
		log.error("destroy client error, client is NULL \n");
		return;
	}
	log.info("client destroy fd[%d] uid[%d] destroy\n", client->fd, client->uid);
	phz.game->del_client(client, del_oldclient);
}

void Client::pre_destroy(Client *client, bool del_oldclient)
{
	if (client == NULL){
		log.error("pre destroy client is NULL \n");
		return;
	}
	if (client->_write_q.empty()){
		Client::destroy(client, del_oldclient);
	}
	else{
		log.info("pre destroy client[%d] uid[%d] list is not empty \n", client->fd, client->uid );
		if (del_oldclient){
			client->cmd_type = 1;
		}
		else{
			client->is_close = 1;
		}
	}
}

void Client::read_cb_json(struct ev_loop *loop, struct ev_io *w, int revents)
{
//    log.debug("%s: test, read called \n", __FUNCTION__);
	int ret;
	static char recv_buf[DEF_BUF_LEN];
	Client *self = (Client*) w->data;
	self->update_timer();
	if (self->_state == PARSE_HEADER) {
		ret = read(self->fd, &self->_header[self->_cur_head_len],
					sizeof(struct Header) - self->_cur_head_len);
		if (ret < 0) {
			if (errno == EAGAIN || errno == EINPROGRESS || errno == EINTR) {
				log.warn("read cb read header failed[%s]\n", strerror(errno));
				return;
			}
			log.error("read header failed[%s]\n", strerror(errno));
			Client::destroy(self);
			return;
		}
		if (ret == 0) {
			log.error("connection close in read header[%d]\n", self->fd);
			Client::destroy(self);
			return;
		}
		self->_cur_head_len += ret;
		//		cout<<"self->_header_p->length"<<self->_header_p->length<<endl;
		if (self->_cur_head_len == sizeof(self->_header)) {
			if (self->_header_p->length > MAX_BUF_LEN
						|| self->_header_p->length == 0) {
				log.error("fd[%d] recv an error len package[%d]\n", self->fd, self->_header_p->length);
				Client::destroy(self);
				return;
			}
			self->_state = PARSE_BODY;
			self->_cur_head_len = 0;
			self->body.clear();
		}
	} 
	else if (self->_state == PARSE_BODY){
		ret = read(self->fd, recv_buf, self->_header_p->length - self->body.length());
		if (ret < 0) {
			if (errno == EAGAIN || errno == EINPROGRESS || errno == EINTR) {
				log.debug("read body failed[%s]\n", strerror(errno));
				return;
			}
			log.error("read body failed[%s]\n", strerror(errno));
			Client::destroy(self);
			return;
		}
		if (ret == 0) {
			log.error("connection close in read body[%d]\n", self->fd);
			Client::destroy(self);
			return;
		}
		recv_buf[ret] = '\0';
		self->body.append(recv_buf);
		if (self->body.length() == self->_header_p->length) {
			self->_state = PARSE_HEADER;
			if (self->packet.parse(self->body) < 0) {
				log.error("parse err!!\n");
				Client::destroy(self);
				return;
			}
			time_t begin = time(NULL);
			int ret = phz.game->dispatch_json(self);
			time_t end = time(NULL);
			int total = end - begin;
			if (total >= 1) {
				log.error("slow cmd: [%d]\n", total);
			}
			if (ret < 0) {
				log.error("dispatch err\n");
				pre_destroy(self);
				return;
			}
			if (self->is_err == 1) {
				log.error("client is err\n");
				Client::destroy(self);
			}
		}
	}
}

void Client::read_cb_proto(struct ev_loop *loop, struct ev_io *w, int revents)
{
//    log.debug("%s: test, read called \n", __FUNCTION__);
	int ret;
	static char recv_buf[DEF_BUF_LEN];
	Client *self = (Client*) w->data;
	self->update_timer();
	if (self->_state == PARSE_HEADER) {
		ret = read(self->fd, &self->p_header[self->_cur_head_len],
					sizeof(struct Pheader) - self->_cur_head_len);
		if (ret < 0) {
			if (errno == EAGAIN || errno == EINPROGRESS || errno == EINTR) {
				log.warn("read cb read header failed[%s]\n", strerror(errno));
				return;
			}
			log.error("read header failed[%s]\n", strerror(errno));
			Client::destroy(self);
			return;
		}
		if (ret == 0) {
			log.error("connection close in read header[%d]\n", self->fd);
			Client::destroy(self);
			return;
		}
		self->_cur_head_len += ret;
		if (self->_cur_head_len == sizeof(self->p_header)) {
			short m_body_len = ((struct Pheader*)(self->p_header))->length;

			if (m_body_len > MAX_BUF_LEN || m_body_len < 0) {
				log.error("fd[%d] recv an error len package[%d] cmd[%d]\n", self->fd, m_body_len, ((struct Pheader*)(self->p_header))->cmd);
				Client::destroy(self);
				return;
			}
			self->_state = PARSE_BODY;
			self->_cur_head_len = 0;
			self->body_len = m_body_len;
			self->body.clear();
			//�������Ϊ�� ֱ�ӽ���Э�鴦��
			if (m_body_len == 0) {
				goto dealer;
			}
		}
	} else if (self->_state == PARSE_BODY){
		//�������Ϊ��
		if (self->body_len != 0) {
			ret = read(self->fd, recv_buf, self->body_len - self->body.length());
			if (ret < 0) {
				if (errno == EAGAIN || errno == EINPROGRESS || errno == EINTR) {
					log.debug("read body failed[%s]\n", strerror(errno));
					return;
				}
				log.error("read body failed[%s]\n", strerror(errno));
				Client::destroy(self);
				return;
			}
			if (ret == 0) {
				log.error("connection close in read body[%d]\n", self->fd);
				Client::destroy(self);
				return;
			}
			self->body.append(recv_buf, ret);
		}
		if (self->body.length() == self->body_len) {
dealer:		self->_state = PARSE_HEADER;
			if (self->ppacket.save((struct Pheader*)(self->p_header), self->body) < 0) {
				log.error("client[%d] save buf err!!\n", self->fd);
				Client::destroy(self);
				return;
			}
			struct timeval begin, end;
			gettimeofday(&begin, NULL); 
			//time_t begin = time(NULL);
			int ret = phz.game->dispatch_proto(self);
			gettimeofday(&end, NULL);
			// time_t end = time(NULL);
			// int total = end - begin;
			// if (total >= 1) {
			// 	log.error("slow cmd: [%d]\n", total);
			// }
			int cmd = ((struct Pheader*)(self->p_header))->cmd;
			if(cmd != CLINET_HEART_BEAT_REQ){
				int usetime = 1000000 * (end.tv_sec - begin.tv_sec) + end.tv_usec - begin.tv_usec;
				log.debug("cmd:%d usetime:%dus\n", cmd, usetime);
				if(usetime >= 1000000){
					log.error("slow cmd:%d usetime:%dus\n", cmd, usetime);
				}
			}
			
			if (ret < 0) {
				log.error("dispatch err\n");
				pre_destroy(self);
				return;
			}
			if (self->is_err == 1) {
				log.error("client is err\n");
				Client::destroy(self);
			}
		}
	}
}

void Client::write_cb(struct ev_loop *loop, struct ev_io *w, int revents)
{
	Client *self = (Client*) w->data;
	if (self->_write_q.empty()) {
		ev_io_stop(EV_A_ w);
		if (self->is_close == 1) {
			Client::destroy(self);
			return;
		}
		if (self->cmd_type == 1) {
			Client::destroy(self, true);
			return;
		}
		return;
	}
	Buffer* buffer = self->_write_q.front();
	ssize_t written = write(self->fd, buffer->dpos(), buffer->nbytes());
	if (written < 0) {
		if (errno == EAGAIN || errno == EINPROGRESS || errno == EINTR) {
			log.warn("write failed[%s]\n", strerror(errno));
			return;
		}
		/* todo close this client */
		log.error("unknow err in written [%d]\n", self->fd);
		Client::destroy(self);
		return;
	}
	buffer->pos += written;
	if (buffer->nbytes() == 0) {
		self->_write_q.pop_front();
		delete buffer;
	}

}

void Client::nodata_timer_cb(struct ev_loop *loop, struct ev_timer *w, int revents)
{
	Client *self = (Client*)w->data;
	log.info("nodata_timer_cb client fd[%d] uid[%d] timeout\n", self->fd, self->uid);
	Client::pre_destroy(self);
}

int Client::send(const char *buf, unsigned int len)
{
	if (fd > 0) {
		if (_write_q.empty()) {
			_ev_write.data = this;
			ev_io_start(phz.loop, &_ev_write);
		}
		_write_q.push_back(new Buffer(buf, len));
		return 0;
	}
	return -1;
}

int Client::send(const std::string &res)
{
	if (client_type == 0) 
	{
		std::string header_str ;
		header_str.append((const char*)res.c_str(), sizeof(struct Pheader));
		int cmd = ((struct Pheader*)(header_str.c_str()))->cmd;
		if( cmd != CLINET_HEART_BEAT_REQ && cmd != SERVER_HEART_BEAT_RES ) //�ų�����
		{
			log.info("client[%d] uid[%d] sendData cmd[%d] \n",fd, uid, cmd);
		}
	}
	else
	{
		if (res.length() > 4) 
		{
			Jpacket pac;
			std::string body_str;
			body_str.append(res, 4, res.length());
			if( (pac.parse(body_str) >= 0) && !(pac.val["cmd"].isNull()) && pac.val["cmd"].isNumeric() )
			{
				int cmd = pac.val["cmd"].asInt();
				if( cmd != CLINET_HEART_BEAT_REQ && cmd != SERVER_HEART_BEAT_RES ) //�ų�����
				{
					log.info("client[%d] uid[%d] sendData cmd[%d] \n", fd, uid, cmd);
				}
			}
		}
	}
	return send(res.c_str(), res.length());
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

void Client::update_timer()
{
	if( (phz.game->link_data_client && fd == phz.game->link_data_client->fd)
		|| (phz.game->link_video_client && fd == phz.game->link_video_client->fd)){
		return;
	}
	ev_timer_again(phz.loop, &_ev_nodata_timer);
}

void Client::set_positon(int pos)
{
	position = pos;
}
