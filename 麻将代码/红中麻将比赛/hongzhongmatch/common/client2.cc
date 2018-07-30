#include "client.h"
#include "log.h"
#include <sys/types.h>
#include <sys/socket.h>

extern Log log;

bool CheckHeader_Json(JsonHeader* pHeader)
{
	if (pHeader->length <= 0 || pHeader->length >= DEF_BUF_LEN)
	{
		return false;
	}
	return true;
}

void Client::read_cb_proto(struct ev_loop *loop, struct ev_io *w, int revents)
{
	int iCount = 0;
	char useless[0x10] = "";
	char recv_buf[DEF_BUF_LEN];
	char useless0[0x10] = "";
	Client_Buffer* buffer= NULL;

	Client *self = (Client*)w->data;
	if (self == NULL)
	{
		return;
	}

	iCount = recv(self->fd, recv_buf, sizeof(recv_buf), 0);
	if (iCount < 0)
	{
		if (errno == EAGAIN || errno == EINPROGRESS || errno == EINTR)
		{
			log.error("%s read failed[%s]\n", __FUNCTION__, strerror(errno));
			return;
		}
		log.error("%s failed[%s]\n", __FUNCTION__, strerror(errno));
		Client::destroy(self);
		return;
	}
	if (iCount == 0 /*&& errno != 0*/)
	{
		//log.error("%s connection ip:%s close in read header:%d uid:%d error:%s\n", __FUNCTION__, self->string_ip.c_str(), self->fd, self->uid, strerror(errno));
		if (errno == EINTR || errno == 0)
		{
			if (self->m_ErrorCount++ > 3)
			{
				Client::destroy(self);
			}
			return;
		}
		Client::destroy(self);
		return;
	}
	if (iCount == sizeof(recv_buf))
	{
		log.error("%s recv buffer size is small\n", __FUNCTION__);
	}			
	char* tmp_buf = recv_buf;	
	while (iCount > 0)
	{		
		buffer = (Client_Buffer*)tmp_buf;
		if (!CheckHeader_Proto(&(buffer->proto_head)))
		{
			log.error("%s fd:%d uid:%d CheckHeader_Proto error len:%d package\n", __FUNCTION__, self->fd, self->uid, iCount);
			Client::destroy(self);
			return;
		}

		self->m_Buffer = buffer;
		time_t begin = time(NULL);
		//int ret = zjh.game->dispatch_client(self);
		int ret = dispatch_client(self);
		time_t end = time(NULL);
		time_t total = end - begin;
		if (total >= 1)
		{
			log.error("%s slow cmd:%d len:%d time:%d\n", __FUNCTION__, buffer->proto_head.cmdID, buffer->proto_head.length, total);
		}
		if (ret < 0)
		{
			log.error("%s parse buffer err cmd:%d len:%d\n", __FUNCTION__, buffer->proto_head.cmdID, buffer->proto_head.length);
			//pre_destroy(self);			
			Client::destroy(self);
			return ;
		}
		if (self->is_err) {
			log.error("%s client is err\n", __FUNCTION__);
			Client::destroy(self);
			return;
		}
		iCount -= buffer->proto_head.length + sizeof(buffer->proto_head);
		tmp_buf += buffer->proto_head.length + sizeof(buffer->proto_head);

		self->m_Buffer = NULL;
	}	
}


void Client::read_cb_json(struct ev_loop *loop, struct ev_io *w, int revents)
{
	int iCount = 0;
	char recv_buf[DEF_BUF_LEN];
	Client_Buffer* buffer = NULL;

	Client *self = (Client*)w->data;
	if (self == NULL)
	{
		return;
	}

	iCount = recv(self->fd, recv_buf, sizeof(recv_buf), 0);
	if (iCount < 0)
	{
		if (errno == EAGAIN || errno == EINPROGRESS || errno == EINTR)
		{
			log.error("%s read failed[%s]\n", __FUNCTION__, strerror(errno));
			return;
		}
		log.error("%s failed[%s]\n", __FUNCTION__, strerror(errno));
		Client::destroy(self);
		return;
	}
	if (iCount == 0 /*&& errno != 0*/)
	{
		log.error("%s connection close in read header[%d] error:%s\n", __FUNCTION__, self->fd, strerror(errno));
		if (errno == EINTR || errno == 0)
		{
			if (self->m_ErrorCount++ > 3)
			{
				Client::destroy(self);
			}
			return;
		}		
		Client::destroy(self);
		return;
	}
	if (iCount == sizeof(recv_buf))
	{
		log.error("%s recv buffer size is small\n", __FUNCTION__);
	}		
	char* tmp_buf = recv_buf;
	while (iCount > 0)
	{
		buffer = (Client_Buffer*)tmp_buf;		
		if (!CheckHeader_Json(&(buffer->json_head)))
		{
			log.error("%s fd[%d] recv an error len:%d package\n", __FUNCTION__, self->fd, iCount);
			Client::destroy(self);
			return;
		}		
		if (self->packet.parse(buffer->json_data, (int)buffer->json_head.length < iCount ? buffer->json_head.length : iCount) < 0)
		{
			log.error("%s parse err len:%d!!\n", __FUNCTION__, buffer->json_head.length);
			Client::destroy(self);
			return;
		}
		self->m_Buffer = buffer;
		time_t begin = time(NULL);		
		int ret = dispatch_server(self);
		time_t end = time(NULL);
		time_t total = end - begin;
		if (total >= 1)
		{
			log.error("%s slow cmd len:%d time:%d\n", __FUNCTION__, buffer->json_head.length, total);
		}
		if (ret < 0)
		{
			log.error("%s parse buffer err len:%d\n", __FUNCTION__, buffer->json_head.length);
			Client::destroy(self);
			return;
		}
		if (self->is_err)
		{
			log.error("%s client is err\n", __FUNCTION__);
			Client::destroy(self);
			return;
		}

		iCount -= buffer->json_head.length + sizeof(buffer->json_head);
		tmp_buf += buffer->json_head.length + sizeof(buffer->json_head);

		self->m_Buffer = NULL;
	}
}