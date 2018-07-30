#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <time.h>
#include <errno.h>

#include "log.h"

int					_fd = -1;
int					_console = 0;



#define COMMON_LOG() \
	static char buf[102400]; \
	time_t t;\
    struct tm gt;\
    struct tm* p;\
     int len=0;\
    int cnt;\
	time(&t);\
    p = localtime_r(&t, &gt);\
    if(!p)\
        p = &gt;\
	va_list argptr;\
	va_start(argptr, fmt);\
    cnt = vsprintf(buf + len, fmt, argptr);\
	va_end(argptr);\
    cnt += len;\

Log::Log() {
	_level = LEVEL_DEBUG;
	_fd = -1;
	_console = CONSOLE_OFF;
	_rotate = ROTATE_OFF; 
	_max_size = 0;
	_current_size = 0;
	_max_file = 0;

	time_t timep;
	time(&timep);
	_curr_day = localtime(&timep);
	_year = _curr_day->tm_year+1900;
	_month = _curr_day->tm_mon;
	_day = _curr_day->tm_mday;
#ifdef POSIX_PTHREAD
	pthread_mutex_init(&_mutex, NULL);
#endif
}

Log::~Log() {
#ifdef POSIX_PTHREAD
	pthread_mutex_destroy(&_mutex);
#endif
}



void Log::start(string log_file, int level, int console, int rotate,
		int64_t max_size = 1073741824, int max_file = 50) {

	// return;
	struct stat st;

	_log_file = log_file;
	_fd = open(_log_file.c_str(), O_RDWR | O_APPEND | O_CREAT, 0644);
	if (_fd < 0) {
		perror("LOG~open");
		//exit(1);
		abort();
	}

	if (fstat(_fd, &st) < 0) {
		perror("LOG~stat64");
		//exit(1);
		abort();
	}

	_level = level;
	//onsole = console;
	_rotate = rotate;
	if (max_size > 0)
		_max_size = max_size;
	_current_size = st.st_size;
	_max_file = max_file;
}

void Log::stop() {
	if (_fd != -1) {
		close(_fd);
		_fd = -1;
	}
}

void Log::output(char *buf, int cnt) 
{
	if (_console == CONSOLE_ON) {
		printf("%s", buf);
		return;
	}

#ifdef POSIX_PTHREAD	
	pthread_mutex_lock(&_mutex);
#endif
	printf("_fd  %d \n" , _fd );
	if (_fd < 0)
	{
		start("./log/zhajinhua.log", 5, 0, 999999, 10);
	}

	if (_fd < 0 || check_day_changed()) 
	{
		rotate_day();
	}
	if (_fd > 0) 
	{
//		if (_rotate == ROTATE_ON) {
//			if (_current_size > _max_size) {
//				rotate();
//			}
//		}
		int ret = write(_fd, buf, cnt);
		if (ret < 0) {
			// todo
			perror("Log~write");
		}
		_current_size += cnt;
	} else {
		printf("%s", buf);
	}
#ifdef POSIX_PTHREAD
	pthread_mutex_unlock(&_mutex);
#endif
}

void Log::rotate() {
	char src[1024];
	char dst[1024];

	close(_fd);
	_fd = -1;
	_current_size = 0;

	for (int i = _max_file; i > 0; i--) 
	{
		snprintf(dst, sizeof(dst), "%s.%d", _log_file.c_str(), i);
		snprintf(src, sizeof(src), "%s.%d", _log_file.c_str(), i - 1);
		rename(src, dst);
		// printf("rename %s %s\n",src ,dst);
	}

	snprintf(dst, sizeof(dst), "%s.%d", _log_file.c_str(), 0);
	snprintf(src, sizeof(src), "%s", _log_file.c_str());
	rename(src, dst);
	// printf("rename %s %s\n",src ,dst);

	_fd = open(_log_file.c_str(), O_RDWR | O_APPEND | O_CREAT, 0644);
	if (_fd < 0) {
		perror("LOG~open");
		return;
	}
}

void Log::rotate_day() {
	char src[1024];
	char dst[1024];

	close(_fd);
	_fd = -1;
	_current_size = 0;

	snprintf(src, sizeof(src), "%s", _log_file.c_str());
	snprintf(dst, sizeof(dst), "%s.%04d-%02d-%02d", _log_file.c_str(), _year, _month+1, _day);
	rename(src, dst);

	_fd = open(_log_file.c_str(), O_RDWR | O_APPEND | O_CREAT, 0644);
	if (_fd < 0) {
		perror("LOG~open");
		return;
	}

	_year = _curr_day->tm_year + 1900;
	_month = _curr_day->tm_mon;
	_day = _curr_day->tm_mday;
}

void Log::panic(char *fmt, ...) {
	return;
	if (_level < LEVEL_PANIC)
		return;
	//int log_type = PANIC_TYPE;
	COMMON_LOG()

	output(buf, cnt);

	//exit(1);
	abort();
}

void Log::fatal(char *fmt, ...) {
	return;
	if (_level < LEVEL_FATAL)
		return;
	//int log_type = FATAL_TYPE;
	COMMON_LOG()

	output(buf, cnt);
}

void Log::error(char *fmt, ...) {
	return;
	if (_level < LEVEL_ERROR)
		return;
	//int log_type = ERROR_TYPE;
	COMMON_LOG()

	output(buf, cnt);
}

void Log::warn(char *fmt, ...) {
	return;
	if (_level < LEVEL_WARN)
		return;
	//int log_type = WARN_TYPE;
	COMMON_LOG()

	output(buf, cnt);
}

void Log::info( char *fmt, ...) {
	return;
	if (_level < LEVEL_INFO)
		return;
	//int log_type = INFO_TYPE;
	COMMON_LOG()

	output(buf, cnt);
}

void Log::my_debug(char *fmt, ...) 
{	
	if (_level < LEVEL_DEBUG)
		return;
	//int log_type = DEBUG_TYPE;
	COMMON_LOG()

		output(buf, cnt);
}

  

void Log::debug(char *fmt, ...) {
	//return;
	if (_level < LEVEL_DEBUG)
		return;
	//int log_type = DEBUG_TYPE;
	COMMON_LOG()

	output(buf, cnt);
}

bool Log::check_day_changed() {
	time_t timep;
	time(&timep);
	localtime(&timep);

//	fprintf(stderr, "now:%04d-%02d-%02d, cur:%04d-%02d-%02d.\n", _curr_day->tm_year+1900,
//			_curr_day->tm_mon, _curr_day->tm_mday, _year, _month, _day);

	if (_year == (_curr_day->tm_year+1900)
			&& _month == _curr_day->tm_mon
			&& _day == _curr_day->tm_mday) {
		return false;
	}

	return true;
}

#ifdef LOG_DEBUG
int main()
{
	Log log;
	log_error("%s\n", "if is good.");
	//log_start("a.log", LEVEL_DEBUG, CONSOLE_OFF, ROTATE_ON, 100, 30);
	log_error("%s\n", "if is good.");
	log_error("%s\n", "if is good.");
	log_error("%s\n", "if is good.");
	log_info("%s\n", "if is good.");
	log_debug("%s\n", "if is good.");
	//log_panic("%s\n", "if is good.");
	//log_stop();

	return 0;
}
#endif
