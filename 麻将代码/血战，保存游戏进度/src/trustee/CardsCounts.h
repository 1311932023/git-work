#ifndef __CARDSCOUNTS_POOL__
#define __CARDSCOUNTS_POOL__
#include "common.h"

class CardsCounts
{
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive & ar, const unsigned int /* file_version */) {
		printf("CardsCounts::serialize\n");
		ar & m_Data;
		ar & _2inner;
		ar & _2outer;
	}
public:
	CardsCounts() 
	{ 
		memset(m_Data, 0, sizeof(m_Data)); 
	}
	char& operator[](_uint8 pos) {  return m_Data[_2inner[pos]]; }
	static _uint8 Convert(_uint8 pos) { return _2outer[pos]; }
private:
	char m_Data[44];
	static _uint8 _2inner[100];	
	static _uint8 _2outer[100];
};

#endif