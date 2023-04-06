#ifndef OFFLINEMESSAGEMODEL_H
#define OFFLINEMESSAGEMODEL_H
#include<vector>
#include<string>
using namespace std;

// 提供离线消息表的操作方法
class OfflineMsgModel
{
public:
    // 存储用户的离线消息
    void insert(int uerid, string msg);

    // 删除用户的离线消息 --- 业务逻辑的正确性：当离线用户在线，查看到消息后就因该被删除。否则每次用户在线都会看到离线消息
    void remove(int userid);

    // 查询用户的离线消息
    vector<string> query(int userid);
};
#endif