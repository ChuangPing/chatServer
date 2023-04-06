#include "chatServer.hpp"
#include "chatService.hpp" // 服务层

#include<functional>
#include "json.hpp"
using json = nlohmann::json;
using namespace std;
using namespace placeholders; // 函数参数占位符：std命名空间下

 // 初始化聊天服务器对象
ChatServer::ChatServer(EventLoop* loop, const InetAddress& listenAddr, const string& nameArg):_server(loop, listenAddr, nameArg), _loop(loop)
{
    // 注册链接回调、
    _server.setConnectionCallback(std::bind(&ChatServer::onConnection, this, _1));

    // 注册消息回调
    _server.setMessageCallback(std::bind(&ChatServer::onMessage, this, _1, _2, _3));

    // 设置线程数量
    _server.setThreadNum(4);
}

 // 启动服务
void ChatServer::start()
{
    _server.start();
}

// 上报链接相关信息的会调函数 断开/链接
void ChatServer::onConnection(const TcpConnectionPtr& conn)
{
    // 客户端连接断开
    if (!conn->connected())
    {
        // 处理可能客户端异常退出例如 ctr+c. 因为：我们设置了登录成功后会将状态设置为online，正常退出会设置为offline。如果客户端异常退出，服务器无法将用户状态进行设置为offline
        // 导致下次用户登录由于数据库状态为online，而无法登录。  --- 同样服务端的异常退出也需要处理
        ChatService::instance()->clientCloseException(conn);
        conn->shutdown(); // 客户端连接断开就关闭当前和客户端通信的套接字
    }

}

// 上报读写事件相关信息回调函数
void ChatServer::onMessage(const TcpConnectionPtr& conn, Buffer* buffer, Timestamp time)
{
    string buf = buffer->retrieveAllAsString();
    cout << "server:buf" << buf << endl;
    // 数据的反序列化
    json js = json::parse(buf);
    // 利用chatService与网络层解耦
    auto msgHandler = ChatService::instance()->getHandler(js["msgid"].get<int>()); // 根据消息id获得对应处理函数
    msgHandler(conn, js, time); // 调用对应消息处理函数
}