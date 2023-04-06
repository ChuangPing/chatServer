#ifndef PUBLIC_H
#define PUBLIC_H
// server 和 client 的公共文件

// 消息id类型枚举  -- 不同的消息id对应不同的事件处理函数
enum EnMsgType
{
    LOGIN_MSG = 1, // 登录消息
    LOGIN_MSG_ACK, //登录响应消息
    REG_MSG,  // 注册消息
    REG_MSG_ACK,  //注册消息响应消息
    LOGINOUT_MSG, // 退出登录消息

    ONE_CHAT_MSG, // 一对一聊天消息
    ADD_FRIEND_MSG, // 添加朋友
    
    CREATE_GROUP_MSG, // 创建群组
    ADD_GROUP_MSG, // 加入群组
    GROUP_CHAT_MSG, // 群聊天
};
#endif