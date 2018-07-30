#ifndef __CLIENT_BUFFER_H__
#define __CLIENT_BUFFER_H__

class Buffer {
public:
	Buffer(const char *bytes, unsigned int nbytes, unsigned int sh = 0)
	{
		pos = 0;
		len = nbytes;
		shift = sh;
		data = new char[nbytes];
		memcpy(data, bytes, nbytes);

	}

	virtual ~Buffer()
	{
		delete[] data;
	}

	char *dpos()
	{
		return data + pos + shift;
	}

	int nbytes()
	{
		return len - pos - shift;
	}
	void nshift(unsigned int written)
	{
		pos += written;
	}
	char* pdata()const
	{
		return data;
	}
private:
	char	*data;
	unsigned int len;
	unsigned int pos;
	unsigned int shift;
};

#endif