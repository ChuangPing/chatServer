/*
    muduo网络库给用户提供了两个主要的类
    TcpServer:用于编写服务器程序
    TcpClient:用于编写客户端程序的

    epoll + 线程池
    好处：能够把网络I/O的代码和业务代码区分开
    用户连接和断开  /   用户的读写事件
*/

/*
    基于muduo网络库开发服务程序
    1、组合TcpServer对象
    2、创建EventLoop事件循环对象指针
    3、明确TcpServer构造函数需要什么，输入chatServer的构造函数
    4、在当前服务器类的构造函数当中，注册处理连接的回调函数和处理读写事件的回调函数
    5、设置合适的服务端线程数量，muduo库会自己分配I/o线程和worker线程

*/

#include<muduo/net/TcpServer.h>
#include<muduo/net/EventLoop.h>
#include<iostream>
using namespace muduo::net;
using namespace std::placeholders; // 参数占位符
using namespace std;
using namespace muduo;

// 基于muduo网络库开发服务器程序
class ChatServer
{
public:
    ChatServer(EventLoop* loop, // 事件循环
        const InetAddress& listenAddr, // IP + port
        const string& nameArg) // 服务器名字
        :_server(loop, listenAddr, nameArg), _loop(loop)
        {
            // 给服务器注册用户连接的创建和断开回调
            _server.setConnectionCallback(std::bind(&ChatServer::onConnection, this, _1)); // 一个参数

            // 给服务器注册用户读写事件回调
            _server.setMessageCallback(std::bind(&ChatServer::onMessage, this, _1, _2, _3));

            // 设置服务器线程数量 1个I/o线程  3个worker线程
            _server.setThreadNum(4);
        }

        // 开启事件循环
        void start()
        {
            _server.start();
        }

private:
    // 专门处理用户的连接创建和断开  epoll、listenfd、accept
    void onConnection(const TcpConnectionPtr& conn)
    {
        if (conn->connected())
        {
            cout << conn->peerAddress().toIpPort() << "->" << conn->localAddress().toIpPort() << "state:online" << endl;
        }else
        {
            cout << conn->peerAddress().toIpPort() << " -> " << conn->localAddress().toIpPort() << "state:offine" << endl;
            conn->shutdown(); // close(fd)
        }

    }

    // 专门处理用户读写事件
    void onMessage(const TcpConnectionPtr& conn, Buffer* buffer, Timestamp time)  // 连接、缓冲区、接收到数据的时间信息
    {
        string buf = buffer->retrieveAllAsString(); // 读取缓冲区接收到的所有数据并转换为string
        cout << "recv data: " << buf << "time: " << time.toString() << endl;
        // conn->send(buf); // 响应给客户端相同的数据
    }
    TcpServer _server; // #1
    EventLoop* _loop; // #2 epoll
};

int main()
{
    EventLoop loop; // epoll
    InetAddress addr("127.0.0.1", 6000);
    ChatServer server(&loop, addr, "chatServer");

    server.start(); // listenfd epoll ctl => epoll
    loop.loop(); // epoll_wait以阻塞的方式等待新用户连接，已连接用户的读写事件等
    return 0;
}