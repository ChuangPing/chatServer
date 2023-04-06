#ifndef FRIENDMODEL_H
#define FRIENDMODEL_H

#include "user.hpp"
#include  <vector>
using namespace std;

// 维护好友信息的操作接口方法
class FriendModel{
public:
    // 添加好友关系
    void insert(int userid, int friendid);

    // 返回用户好友列表，用户登录上线后需要查询登录用户的好友信息。（需要使用这个函数）
    vector<User> query(int userid);
private:
};
#endif