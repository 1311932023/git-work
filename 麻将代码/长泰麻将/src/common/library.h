#ifndef _LIBRARY_H
#define _LIBRARY_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <ev++.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>

class Library
{
public:
	Library();
	~Library();

	int init(const char *lib_file);

public:
	std::vector< std::vector< std::vector<int> > > cards;
};

#endif
