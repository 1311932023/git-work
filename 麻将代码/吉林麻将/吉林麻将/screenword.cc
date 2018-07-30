#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include <errno.h>
#include <json/json.h>

#include "screenword.h"
#include "common/log.h"

extern Log log;

ScreenWord::ScreenWord()
{
}

ScreenWord::~ScreenWord()
{
	masking_word.clear();
}

int ScreenWord::init(const char *lib_file)
{
	std::ifstream in(lib_file, std::ifstream::binary);
	if (!in) {
		return -1;
	}

	Json::Value masking_conf;
	Json::Reader reader;
	bool ret = reader.parse(in, masking_conf);
	if (!ret) {
		in.close();
		return -1;
	}

	int num = (int)masking_conf["masking-word"].size();
	log.debug("masking_word init size[%d].\n", num);
	for ( int i = 0; i < num; i++ ) 
	{
		std::string temp_word = masking_conf["masking-word"][i].asString();
		masking_word.push_back( temp_word );
		
	}
	masking_conf.clear();
	in.close();
	return 0;
}

void ScreenWord::wordfilter( std::string &source_words )
{
	for( std::vector<std::string>::iterator iter = masking_word.begin(); iter != masking_word.end(); iter++ )
	{
		unsigned int size = (*iter).size();
		if( 0 == size )
		{
			return;
		}
		std::string::size_type pos = 0;
		while( true )
		{
			pos = source_words.find( *iter, pos );
			if( pos != std::string::npos )
			{
				source_words.replace( pos, size, size, '*');
			}
			else
			{
				break;
			}
		}
	}
}
