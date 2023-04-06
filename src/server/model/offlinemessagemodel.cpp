#include "offlinemessagemodel.hpp"
#include "db.h"
#include <muduo/base/Logging.h>

 // 存储用户的离线消息
void OfflineMsgModel::insert(int userid, string msg)
{
    // 组装sql
    char sql[1024] = {0};
    sprintf(sql, "insert into offlinemessage value(%d, '%s')", userid, msg.c_str()); // 操作数据库中的offlineMessage
    MySQL mysql;
    if (mysql.connect())
    {
        mysql.update(sql);
    }
}

    // 删除用户的离线消息 --- 业务逻辑的正确性：当离线用户在线，查看到消息后就因该被删除。否则每次用户在线都会看到离线消息
void OfflineMsgModel::remove(int userid)
{
    char sql[1024] = {0};
    sprintf(sql, "delete from offlinemessage where userid = %d", userid);
    LOG_INFO << "remove info" << sql;
    MySQL mysql;
    if (mysql.connect())
    {
        mysql.update(sql);
    }
}

    // 查询用户的离线消息
vector<string> OfflineMsgModel::query(int userid)
{
    char sql[1024] = {0};
    sprintf(sql, "select message from offlinemessage where userid = %d", userid); 
    vector<string> vec;
    MySQL mysql;
    if (mysql.connect())
    {
        MYSQL_RES* res = mysql.query(sql);
        if (res != nullptr)
        {
            // 把useid用户的所有的离线消息放入到vec中
            MYSQL_ROW row;
            while((row = mysql_fetch_row(res)) != nullptr)
            {
                vec.push_back(row[0]);
            }
            mysql_free_result(res); // 释放资源
            return vec;
        }
    }
    return vec;
}