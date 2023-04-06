#ifndef CHATSERVER_H
#define CHATSERVER_H

#include<muduo/net/TcpServer.h>
#include<muduo/net/EventLoop.h>
#include <functional>
using namespace muduo;
using namespace muduo::net;
using namespace std;

class ChatServer{
public:
    // 初始化聊天服务器对象
    ChatServer(EventLoop* loop, const InetAddress& listenAddr, const string& nameArg);

    // 启动服务
    void start();
private:
    // 上报链接相关信息的会调函数 断开/链接
    void onConnection(const TcpConnectionPtr&);

    // 上报读写事件相关信息回调函数
    void onMessage(const TcpConnectionPtr&, Buffer*, Timestamp);
   
private:
    TcpServer _server; // 组合的muduo库，实现服务器功能的类对象
    EventLoop* _loop; // 指向事件循环对象的指针
};

#endif