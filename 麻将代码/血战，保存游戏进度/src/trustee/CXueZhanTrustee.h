#include "CTrustee.h"
#include <stdio.h>

class CXueZhanTrustee : public CTrustee
{
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive & ar, const unsigned int /* file_version */) {
		printf("CXueZhanTrustee::serialize\n");
		ar & boost::serialization::base_object<CTrustee>(*this);
		/*boost::serialization::void_cast_register<CXueZhanTrustee, CTrustee>(
			static_cast<CXueZhanTrustee *>(NULL),
			static_cast<CTrustee *>(NULL)
			);*/
	}
public:
	CXueZhanTrustee(Table* t);
	CXueZhanTrustee();
	virtual ~CXueZhanTrustee() {}
	virtual void RobotOutCard(_uint8 ChairID);
};