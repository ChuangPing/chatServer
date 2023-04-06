#include "db.h"
#include<muduo/base/Logging.h>
using namespace std;
// 数据库配置信息
static string server = "127.0.0.1";
static string user = "root";
static string password = "root";
static string dbname = "chat";

MySQL::MySQL() // 初始化数据库连接
    {
        _conn = mysql_init(nullptr);  // 只是准备连接资源，不是真正的连接
    }

MySQL::~MySQL() // 释放连接资源
    {
        if(_conn != nullptr)
        {
            mysql_close(_conn);
        }
    }

    bool  MySQL::connect() // 连接数据库
    {
        MYSQL *p = mysql_real_connect(_conn, server.c_str(), user.c_str(), password.c_str(), dbname.c_str(), 3306, nullptr, 0);
        if (p != nullptr)
        {
            mysql_query(_conn, "set name gbk"); // 连接成功后使mysql支持中文 gbk
            LOG_INFO << "connect mysql success";
        }
        else
        {
            LOG_INFO << "connect mysql failed";
        }
        return p;
    }

    bool  MySQL::update(string sql) // 数据库更新操作
    {
        if (mysql_query(_conn, sql.c_str()))
        {
            LOG_INFO << __FILE__ << ": "<< __LINE__ << " : " << sql << "更新失败";
            return false;
        }
        return true;
    }

    MYSQL_RES*  MySQL::query(string sql) // 数据库查询操作
    {
        if(mysql_query(_conn, sql.c_str()))
        {
            LOG_INFO << __FILE__ << " : " << __LINE__ << " : " << sql << "查询失败" ;
            return nullptr;
        }
        return mysql_use_result(_conn);
    }
 MYSQL* MySQL::getConnection() // 获取当前连接对象
 {
    return _conn;
 } 