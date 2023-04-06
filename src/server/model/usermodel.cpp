#include "usermodel.hpp"
#include "db.h"
#include <iostream>
using namespace std;

bool UserModel::insert(User& user)
{
    // 1、组装sql
    char sql[1024] = {0};
    sprintf(sql, "insert into user(name, password, state) values('%s', '%s', '%s')", user.getName().c_str(), user.getPwd().c_str(), user.getState().c_str()); // 不插入id，是因为它为主键由数据库进行维护并且插入后返回

    MySQL mysql;
    if(mysql.connect())
    {
        if(mysql.update(sql))
        {
            // 插入成功获取对应的主键
            user.setId(mysql_insert_id(mysql.getConnection()));  // 引用参数，id会设置到
            return true;
        }
    }
    return false;
}

// 根据用户id查询用户信息
 User UserModel::query(int id)  
 {
    char sql[1024] = {0};
    sprintf(sql, "select * from user where id = '%d'", id);

    MySQL mysql;
    if (mysql.connect())
    {
        MYSQL_RES* res = mysql.query(sql);
        if (res != nullptr)
        {
            MYSQL_ROW row = mysql_fetch_row(res);
            if(row != nullptr)
            {
                User user;
                user.setId(atoi(row[0]));
                user.setName(row[1]);
                user.setPwd(row[2]);
                user.setState(row[3]);
                mysql_free_result(res);
                return user;
            }
        }
    }
    return User();
 }

// 更新用户状态信息
bool UserModel::updateState(User user)  
{
    char sql[1024] = {0};
    sprintf(sql, "update user set state = '%s' where id = '%d'", user.getState().c_str(), user.getId());
    MySQL mysql;
    if (mysql.connect())
    {
        if (mysql.update(sql))
        {
            return true;
        }
    }
    return false;
}

 // 重置用户的状态
void UserModel::resetState()
{
    char sql[1024] = "update user set state = 'offline' where state = 'online'";

    MySQL mysql;
    if (mysql.connect())
    {
        mysql.update(sql);
    }
}