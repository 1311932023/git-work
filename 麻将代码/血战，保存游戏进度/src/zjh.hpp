#ifndef __ZJH_HPP__
#define __ZJH_HPP__

#include "common/boostHeader.hpp"

template<class Archive>
void ZJH::serialize(Archive & ar, const unsigned int /* file_version */) {
	printf("ZJH::serialize\n");
	ar & game;
}


#endif
