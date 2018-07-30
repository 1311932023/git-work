#ifndef __REDIS_CLIENT_H__
#define __REDIS_CLIENT_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <list>
#include <vector>
#include <queue>
#include <string>
#include "hiredis/hiredis.h"

class RedisClient {
public:
    redisReply          *reply;
    redisContext        *context;
public:
    RedisClient();
    virtual ~RedisClient();
	
    int init(std::string host, int port, int timeout, std::string pass);
	int connect_redis();
	void deinit();
    int command(const char *format, ...);
	int command_spec(const char *format, ...);


	//array存储 相关函数
	int is_array_return_ok();
    char* get_value_as_string(const char *key);
    int get_value_as_int(const char *key);
    long long get_value_as_int64(const char *key);
    float get_value_as_float(const char*key);

	void get_all_value_as_string_vec(std::vector<std::string> &vec);

	//hash存储  相关函数
	

	std::string get_result_as_string();


	//对接受到的结果进行判断
	bool is_ret_string_ok();
	bool is_ret_array_ok();
	bool is_ret_int_ok();
	bool is_ret_nil_ok();
	bool is_ret_status_ok();

protected:
	bool is_ret_ok(int ret_type);

private:
    std::string			_host;
    int                 _port;
	int					_timeout;
	std::string			_pass;
};

#endif
