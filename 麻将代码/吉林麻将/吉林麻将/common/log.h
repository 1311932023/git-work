#ifndef _LOG_H_
#define _LOG_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <sys/time.h>

#ifdef POSIX_PTHREAD
	#include <pthread.h>
#endif

#include <iostream>
#include <fstream>
#include <string>

using namespace std;

							

enum LEVEL {
    LEVEL_PANIC = 0,
    LEVEL_FATAL,
    LEVEL_ERROR,
    LEVEL_WARN,
    LEVEL_INFO,
    LEVEL_DEBUG,
};

enum CONSOLE {
	CONSOLE_OFF = 0,
	CONSOLE_ON,
};

enum ROTATE {
	ROTATE_OFF = 0,
	ROTATE_ON,
};

class Log {
public:
    Log();
	static Log& Instance()
	{
		static Log instance_;
		return instance_;
	}

    virtual ~Log();
    void start(string log_file, int level, int console, int rotate, int64_t max_size, int max_file);
    void set_log_level(int level ) {    _level  = level  ; } 
	//start("./log/zhajinhua.log", 5, 0, 999999, 10); 


    void stop();
    void panic(char *fmt, ...);
    void fatal(char *fmt, ...);
    void error(char *fmt, ...);
    void warn(char *fmt, ...);
    void info(char *fmt, ...);
    void debug(char *fmt, ...);

	void my_debug(char *fmt ,...);
	
private:
    void output(char *buf, int cnt);
	void rotate();	
	bool check_day_changed();
	void rotate_day();

private:
	string				_log_file;
    int					_level;
	/*int					_fd;
	int					_console;*/
    int					_rotate;
    int64_t				_max_size;
    int64_t				_current_size;
	int					_max_file;
	struct tm       *_curr_day;
	int _year;
	int _month;
	int _day;
#ifdef POSIX_PTHREAD
	pthread_mutex_t		_mutex;
#endif
};

//#define PRE_STR "[%s:%s(%d)] "
//#define VAL_STR ,__FILE__,__FUNCTION__,__LINE__
//#else
//#define PRE_STR 
//#define VAL_STR 
//#endif

//#define sslog_trace(format, ...) slog_trace(PRE_STR format VAL_STR ,##__VA_ARGS__)


#define log_debug(format, ...) \
if (1)  \
{ \
	static  char _format[0x2000]; \
struct timeval _tv; \
	gettimeofday(&_tv, NULL); \
struct tm _tm; \
	localtime_r(&_tv.tv_sec, &_tm); \
	snprintf(_format, sizeof(_format)-1 , "|dbug|%d-%02d-%02d %02d:%02d:%02d.%06ld %s:%s:%d %s\n", 1900 + _tm.tm_year, 1 + _tm.tm_mon, _tm.tm_mday, _tm.tm_hour, \
	_tm.tm_min, _tm.tm_sec, _tv.tv_usec, __FILE__, __FUNCTION__, __LINE__, format); \
	Log::Instance().my_debug(_format, ##__VA_ARGS__); \
}

#define log_info(format, ...) \
if (1)  \
{ \
	static char info_format[0x2000]; \
struct timeval _tv; \
	gettimeofday(&_tv, NULL); \
struct tm _tm; \
	localtime_r(&_tv.tv_sec, &_tm); \
	snprintf(info_format, sizeof(info_format)-1, "|Info|%d-%02d-%02d %02d:%02d:%02d.%06ld %s:%s:%d %s\n", 1900 + _tm.tm_year, 1 + _tm.tm_mon, _tm.tm_mday, _tm.tm_hour, \
	_tm.tm_min, _tm.tm_sec, _tv.tv_usec, __FILE__, __FUNCTION__, __LINE__, format); \
	Log::Instance().my_debug(info_format, ##__VA_ARGS__); \
}

#define log_error(format, ...) \
if (1)  \
{ \
	static char error_format[0x2000]; \
struct timeval _tv; \
	gettimeofday(&_tv, NULL); \
struct tm _tm; \
	localtime_r(&_tv.tv_sec, &_tm); \
	snprintf(error_format, sizeof(error_format)-1  , "|err|%d-%02d-%02d %02d:%02d:%02d.%06ld %s:%s:%d %s\n", 1900 + _tm.tm_year, 1 + _tm.tm_mon, _tm.tm_mday, _tm.tm_hour, \
	_tm.tm_min, _tm.tm_sec, _tv.tv_usec, __FILE__, __FUNCTION__, __LINE__, format); \
	Log::Instance().my_debug(error_format, ##__VA_ARGS__); \
}


#endif
