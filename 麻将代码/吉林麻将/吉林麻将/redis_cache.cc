#include "redis_cache.h"

#include "common/log.h"
#include "zjh.h"


using namespace std;


extern ZJH zjh;
//extern Log log;

const char *DATABASE_NAME_PRE = "mahjong02_";


CRedisCache::CRedisCache()
{
}

CRedisCache::~CRedisCache()
{
}

CRedisCache& CRedisCache::GetInstance()
{
	static CRedisCache dis;
	return dis;
}

void CRedisCache::StartClearTime(int ex_time)
{
	if ( ex_time <=0  )
	{
		log_error("%s:%d\t%s redis_call_ptime error set default 3600 \n",
			__FILE__,
			__LINE__,
			__FUNCTION__ 		
			);
		ex_time = 3600;
	}
	ev_time_clear_redis.data = this;
	ev_timer_init(&ev_time_clear_redis, CRedisCache::clear_redis_time_cb, ex_time, ex_time);
	ev_timer_start(zjh.loop, &ev_time_clear_redis);
}

void CRedisCache::clear_redis_time_cb(struct ev_loop *loop, struct ev_timer *w, int revents)
{
	CRedisCache* pthis =( CRedisCache* )w->data;
	pthis->ClearRedisExTimeData();
}


bool CRedisCache::UpdateGameEndScore(CGameAccout &game_account)
{
	Json::Value root;
	if ( GetRoomidInfo(game_account.GetRoomid())  && TranslateInfoToJson(root)  )
	{
		
	}
	else
	{
		root["roomid"] = game_account.GetRoomid();
	}

	game_account.GetJsonData(root);

	if ( !UpdateRoomidInfo(game_account.GetRoomid() , root.toStyledString()) )
	{		
		return false;
	}


	log_info("  UpdateRoomidInfo roomid:%d  data:%s ",
		
		game_account.GetRoomid(),
		root.toStyledString().c_str()
	);

	return true;
}



bool CRedisCache::GetGameScore(CGameAccout &game_account)
{
	Json::Value root;
	if (GetRoomidInfo(game_account.GetRoomid()) && TranslateInfoToJson(root))
	{
		log_info("%s:%d\t%s %s\n",
			__FILE__,
			__LINE__,
			__FUNCTION__,
			root.toStyledString().c_str()
			);
		game_account.UpdateInfoFromJson(root);
		return true;
	}
	return false;
}


bool CRedisCache::GetRoomidInfo(int roomid)
{
	if (zjh.main_rc[0]->command("hget %s%d %d", DATABASE_NAME_PRE, zjh.conf["tables"]["zid"].asInt(), roomid))
	{
		return false;
	}
	return true;
}

bool CRedisCache::UpdateRoomidInfo(int roomid,const std::string &values)
{
	if ( zjh.main_rc[0]->command("hset %s%d %d %s" , DATABASE_NAME_PRE , zjh.conf["tables"]["zid"].asInt(),roomid , values.c_str() ))
	{
		return false;
	}
	return true;
}


bool CRedisCache::TranslateInfoToJson(Json::Value &root)
{
	if ( !zjh.main_rc[0]->is_ret_string_ok() )
	{
		return false;
	}
	string str_conf = zjh.main_rc[0]->get_result_as_string();
	if ( str_conf.empty() )
	{
		return false;
	}

	Json::Reader _read;
	if ( _read.parse(str_conf , root) )
	{
		return true;
	}
	return false;
}


void CRedisCache::ClearRedisExTimeData()
{
	int zid = zjh.conf["tables"]["zid"].asInt();

	if ( zjh.main_rc[0]->command("hkeys %s%d" , DATABASE_NAME_PRE , zid)) 
	{
		return	;
	}

	if (!zjh.main_rc[0]->is_ret_array_ok())
	{
		return;
	}

	vector<string> vec;
	zjh.main_rc[0]->get_all_value_as_string_vec(vec);
	
	CheckExTimeFromRoomid(vec);

	vector<string>::const_iterator it = vec.begin();
	while ( it != vec.end() )
	{
		DelRoomid(*it);
		++it;
	}
}

void CRedisCache::CheckExTimeFromRoomid(std::vector<std::string> &vec_roomid)
{
	vector<string>::iterator it = vec_roomid.begin();
	
	int now_time = (int)time(NULL);

	int _exp_time = zjh.conf["timers"]["redis_exp_time"].asInt();
	if ( _exp_time == 0  )
	{
		log_error("%s:%d\t%s no redis_exp_time value ,set default 3600 \n",
			__FILE__,
			__LINE__,
			__FUNCTION__ 			
			);
		_exp_time = 3600;
	}

	while (it != vec_roomid.end())
	{
		Json::Value root;
		if (GetRoomidInfo( atoi(it->c_str()) ) && TranslateInfoToJson(root))
		{
			if ( root["update_time"].asInt() + _exp_time > now_time)
			{
				it = vec_roomid.erase(it);
				continue;
			}
		}
		++it;
	}
}

void CRedisCache::DelRoomid(std::string roomid)
{
	int zid = zjh.conf["tables"]["zid"].asInt();

	if (zjh.main_rc[0]->command("hdel %s%d %s",DATABASE_NAME_PRE , zid , roomid.c_str()) )
	{
		log_error("%s:%d\t%s roomid:%s \n",
			__FILE__,
			__LINE__,
			__FUNCTION__ , 			
			roomid.c_str()
			);
	}
	log_info("%s:%d\t%s roomid:%s \n",
		__FILE__,
		__LINE__,
		__FUNCTION__,
		roomid.c_str()
	);
}


