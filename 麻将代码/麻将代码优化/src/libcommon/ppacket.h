#ifndef _PPACKET_H_
#define _PPACKET_H_

//�԰�ͷ���ж���
#pragma pack(push)
#pragma pack(1)
struct Pheader
{
	unsigned int cmd;
	short length;
	unsigned int uid;
	short svrid;
};
#pragma pack(pop)

class Ppacket
{
public:
	Ppacket();
	virtual ~Ppacket();
	//���
	void pack(unsigned int cmd = 0, unsigned int uid = 0, short svrid = 0);
	//���
	int save(Pheader *h, std::string b);
	int safe_check();


public:
	struct Pheader header;
public:
	std::string body;
	std::string data;

};


#endif
