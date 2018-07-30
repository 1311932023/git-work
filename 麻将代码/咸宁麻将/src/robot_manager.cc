#include "robot_manager.h"
#include "common/log.h"
#include "table.h"

extern Log log;
RobotManager::RobotManager()
{

}

RobotManager::~RobotManager()
{

}

//增加机器人到列表
void RobotManager::AddRobotToList(Player *pPlayer)
{

	if (pPlayer == NULL)
	{
		log_error("AddRobotToList error, player is NULL \n");
		return;
	}
	_uint64 uid = pPlayer->uid;
	pPlayer->status = 0;
	if (m_mapRobot_List.find(uid) != m_mapRobot_List.end())
	{
		log_error("AddRobotToList failed, because of uid:%d is found in list \n", uid);
		return;
	}
	m_mapRobot_List.insert(pair<_uint64, Player *>(uid, pPlayer));
	log_info("AddRobotToList robot uid:%d\n, now robotlist totalnum:%d \n", uid, m_mapRobot_List.size());

}

//从列表中删除指定机器人
void RobotManager::DeleteRobot(_uint64 RobotUid)
{
	std::map<_uint64, Player *>::iterator iter = m_mapRobot_List.find(RobotUid);
	if (iter != m_mapRobot_List.end())
	{
		m_mapRobot_List.erase(iter);
		log_info("delete robot, uid:%d, now robotlist totalnum:%d \n", RobotUid, m_mapRobot_List.size());
		return;
	}
	log_error("cat not delete robot uid:%d, can not find in robotlist \n", RobotUid);
}

//获取一个机器人
Player *RobotManager::GetOneRobot()
{
	std::map<_uint64, Player *>::iterator iter = m_mapRobot_List.begin();
	if (iter != m_mapRobot_List.end())
	{
		Player *pPlayer = iter->second;
		m_mapRobot_List.erase(iter);
		log_info("get one robot, uid:%d, and delete from robotlist, now robotlist totalnum:%d \n", pPlayer->uid, m_mapRobot_List.size());
		return pPlayer;
	}
	log_error("can not get one robot \n");
	return NULL;
}

bool RobotManager::find_robot(_uint64 robot_uid)
{
	std::map<_uint64, Player *>::iterator iter = m_mapRobot_List.find(robot_uid);
	if (iter != m_mapRobot_List.end())
	{   
		return true;
	}
	return false;
}