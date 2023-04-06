#ifndef GROUPMODEL_H
#define GROUPMODEL_H

#include "group.hpp"
#include <string>
#include <vector>
using namespace std;

// 维护群组信息的操作接口方法
class GroupModel
{
public:
    // 创建群组
    bool createGroup(Group &group);
    // 加入群组
    void addGroup(int userid, int groupid, string role);
    // 查询用户所在的群组
    vector<Group> queryGroups(int userid); // 一个用户可以属于多个群组
    // 根据指定的groupid查询群组用户id列表，除了自己的userid，主要用于用户群聊业务给其它群成员发消息
    vector<int> queryGroupsUsers(int userid, int groupid);
private:
};
#endif