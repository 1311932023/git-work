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

#include "library.h"
#include "log.h"

extern Log log;

Library::Library()
{
	cards.clear();
}

Library::~Library()
{
	cards.clear();
}

int Library::init(const char *lib_file)
{
	log_debug( "library init .\n" );
	std::ifstream in(lib_file, std::ifstream::binary);
	if (!in) 
	{
		log_error( "library init in failed .\n" );
		return -1;
	}

	Json::Value libcards;
	Json::Reader reader;
	bool ret = reader.parse(in, libcards);
	if (!ret) 
	{
		log_error( "library init parse failed .\n" );
		in.close();
		return -1;
	}
	log_debug( "library init parse succ .\n" );
	int library_size = libcards.size();
	log_debug( "library init library_size[%d] .\n", library_size );
	for( int i = 0; i < library_size; ++i )
	{
		int lib_element_size = libcards[i].size();
		if( lib_element_size != 5 )
		{
			log_error( "library init lib_element_size[%d] is not 5 .\n", lib_element_size );
			continue;
		}

		std::vector< std::vector<int> > lib_elements;
		lib_elements.clear();
		for( int j = 0; j < lib_element_size; ++j )
		{
			int element_size = libcards[i][j].size();
			if( element_size != 5 )
			{
				log_error( "library init element_size[%d] is not 5 .\n ", element_size );
				continue;
			}

			std::vector<int> elements;
			elements.clear();
			for( int k = 0; k < element_size; ++k )
			{
				elements.push_back( libcards[i][j][k].asInt() );
			}

			lib_elements.push_back( elements );
		}

		cards.push_back( lib_elements );
	}

	libcards.clear();
	in.close();
	return 0;
}
