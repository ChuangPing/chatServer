#ifndef CHATSERVICE_H   // 防止头文件重复包含
#define CHATSERVICE_H

#include<muduo/net/TcpConnection.h>
#include<unordered_map>
#include<mutex>
#include<functional>

using namespace std;
using namespace muduo;
using namespace muduo::net;

#include "usermodel.hpp"
#include "offlinemessagemodel.hpp"
#include "groupmodel.hpp"
#include "friendmodel.hpp"
#include "json.hpp"
#include "redis.hpp"
using json = nlohmann::json;

// 服务层：与网络层相互解耦

// 表示处理消息的事件回调方法类型
using MsgHandler = std::function<void(const TcpConnectionPtr& conn, json& js, Timestamp time)>;

// 聊天服务类
class ChatService
{
public:
    // 获取单例对象接口（构造函数私有化）
    static ChatService* instance();
    // 处理登录业务
    void login(const TcpConnectionPtr& conn, json& js, Timestamp time);
    // 处理注册业务
    void reg(const TcpConnectionPtr& conn, json& js, Timestamp time);
    // 一对一聊天业务
    void oneChat(const TcpConnectionPtr& conn, json& js, Timestamp time);
    // 添加好友业务
    void addFriend(const TcpConnectionPtr& conn, json& js, Timestamp time);
    // 创建群聊业务
    void createGrop(const TcpConnectionPtr& conn, json& js, Timestamp time);
    // 加入群聊组业务
    void addGroup(const TcpConnectionPtr& conn, json& js, Timestamp time);
    // 群组聊天业务
    void groupChat(const TcpConnectionPtr& conn, json& js, Timestamp time);
    // 退出登录业务
    void loginout(const TcpConnectionPtr& conn, json& js, Timestamp time);
    // 获取消息对应的处理函数
    MsgHandler getHandler(int msgid);
    // 处理客户端异常退出
    void clientCloseException(const TcpConnectionPtr& conn);
    // 服务器异常退出，业务重置方法
    void reset();
    // 从redis消息队列中获取订阅的消息
    void handleRedisSubscribeMessage(int, string);
private:
    // 构造函数，一般服务层都只会初始化一次使用单例模式
    ChatService();  
    // 存储消息id和其对应的业务处理方法
    unordered_map<int, MsgHandler> _msgHandlerMap; 
    // 存储在线用户的连接信息：因为在聊天阶段，用户给在线用户发送的消息服务器需要通过传递的id转发给对应的用户，因此需要在用户登录成功后根据id存储当前连接对象
    unordered_map<int, TcpConnectionPtr> _userConnMap;
    // 定义互斥锁，保证_userConnMap的线程安全
    mutex _connMutex;
     // 数据库操作类对象
    UserModel _userModel; 
    OfflineMsgModel _offlineMsgModel;
    FriendModel _friendModel;
    GroupModel _groupModel;

    // redis操作对象
    Redis _redis;
};
#endif