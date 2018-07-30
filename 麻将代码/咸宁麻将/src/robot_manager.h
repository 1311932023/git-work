#ifndef _ROBOT_MANAGER_H__
#define _ROBOT_MANAGER_H__


#include <map>
#include "common/player.h"
#include "libnormalmahjong/common.h"


class RobotManager
{
public:
	RobotManager();
	~RobotManager();

	static RobotManager *GetInstance(){static RobotManager robot; return &robot;}
	//增加机器人到列表
	void AddRobotToList(Player *pPlayer);
	//从列表中删除指定机器人
	void DeleteRobot(_uint64 RobotUid);
	//获取一个机器人
	Player *GetOneRobot();
	//判断是否有此机器人
	bool find_robot(_uint64 robot_uid);
protected:
private:

protected:
	std::map<_uint64, Player *>		m_mapRobot_List;
};

#endif