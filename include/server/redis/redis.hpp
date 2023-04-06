#ifndef REDIS_H
#define REDIS_H
#include <hiredis/hiredis.h>
#include <thread>
#include <functional>
using namespace std;

class Redis
{
public:
    Redis();
    ~Redis();

    // 连接redis服务器
    bool connect();

    // 向redis指定的通道channel发布消息
    bool publish(int channel, string message);

    // 向redis指定的通道subscribe订阅消息
    bool subscribel(int channel);

    // 向redis指定的通道subscribe取消订阅消息
    bool unsubscribel(int channel); // 当用户下线时需要向redis取消订阅消息

    // 子啊独立线程中接收订阅通道中的消息
    void observer_channel_message();

    // 初始化业务层上报通道中的消息
    void int_notify_handler(function<void(int, string)>fn);

private:
    // hiredis 同步上下文对象，负责publish消息
    redisContext* _publish_context;

    // hiredis 同步上下文对象，负责subscribe消息
    redisContext* _subscribe_context;

    // 回调操作，收到订阅的消息，给service层上报
    function<void(int, string)> _notify_message_handler;
};
#endif