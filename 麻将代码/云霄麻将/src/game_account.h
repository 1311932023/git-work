#ifndef GAME_ACCOUNT_H
#define GAME_ACCOUNT_H

#include "libnormalmahjong/common.h"
#include <iostream>
#include <string>
#include <json/json.h>


// redis 存储 游戏结算信息
class CGameAccout
{
public:
	CGameAccout();
	~CGameAccout();

	//设置第几局
	void SetRound(int round);
	int GetRound() { return m_round; }
	
	//设置房间id
	void SetRoomid(int id);
	int GetRoomid() { return m_roomid; }

	//设置玩家每个人的id
	void SetPlayerUid(int charid, int uid);


	//设置当前得分
	void SetScore(int play_id, int score);

	//设置总计得分
	void SetTotalScore(int play_id, int total);

	//获取uid玩家的 总分
	bool GetPlayerScore(int uid, int &score);

	//获取当前数据
	void UpdateInfoFromJson(Json::Value root);



	//数据清空
	void Reset();

	//有效数据
	bool IsValid(); 

	void GetJsonData(Json::Value &data);

	std::string toString(int);

private:

	int m_round;					//第几局
	int m_roomid;					//房间id
	int m_paly_id[GAME_PLAYER];		//玩家游戏id
	int m_score[GAME_PLAYER];		//每人当局得分
	int m_total_score[GAME_PLAYER];	//每人当前总分

};

#endif

