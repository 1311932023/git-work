#ifndef _SCREENWORD_H
#define _SCREENWORD_H

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

class ScreenWord
{
public:
	ScreenWord();
	~ScreenWord();

	int init( const char *lib_file );
	void wordfilter( std::string &source_words); 
public:
	std::vector<std::string > masking_word;
};

#endif
