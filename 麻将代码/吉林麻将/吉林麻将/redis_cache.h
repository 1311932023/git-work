#ifndef CREDIS_CACHE_H
#define CREDIS_CACHE_H

#include "libnormalmahjong/common.h"
#include "redis_client.h"
#include "game_account.h"
#include <iostream>
#include <string>
#include <ev++.h>
#include <ev.h>
#include <stdio.h>

// 1.存储格式
// Hash表存储
// Key：	majhong02 + 游戏id
// Field : 房间id
// Value : json存储的游戏数据



//消息：7212	游戏结束，平台返回当前第几局等无效信息
//消息：7211	游戏开始，发送上面的相同的无用信息
// {
// "can_start" : 1,
// "cmd" : 7211,
// "code" : 200,
// "count" : 8,
// "currentCount" : 7,
// "datetime" : 1495087002,
// "roomid" : "161330",
// "status" : 200,
// "vid" : 2225
// }



class CRedisCache
{
public:
	static CRedisCache& GetInstance();

	void StartClearTime(int ex_time);

	static void clear_redis_time_cb(struct ev_loop *loop, struct ev_timer *w, int revents);


	~CRedisCache();


	
	//更新游戏结算信息
	bool UpdateGameEndScore(CGameAccout &game_account);

	//获取 最后一局结算信息
	bool GetGameScore(CGameAccout &game_account);

	




protected:

	//获取房间信息
	bool GetRoomidInfo(int roomid);

	//更新房间信息
	bool UpdateRoomidInfo(int roomid,const std::string &values);

	//将redis房间信息 ， 转换成 json::value格式
	bool TranslateInfoToJson(Json::Value &root);

	//清空超时
	void ClearRedisExTimeData();

	//一次检测 房间房间数组 是否超时
	void CheckExTimeFromRoomid(std::vector<std::string> &vec_roomid);

	//删除房间信息
	void DelRoomid(std::string roomid);


private:
	CRedisCache();
	ev_timer ev_time_clear_redis;

};

#endif

