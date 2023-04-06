#include "chatService.hpp"
#include "public.hpp"
#include <muduo/base/Logging.h>
#include "user.hpp"
#include <vector>

// 获取单例对象的接口函数
ChatService* ChatService::instance()
{
    static ChatService service;
    return &service;
}

// 注册消息以及对应的Handler操作
ChatService::ChatService()
{
    // 绑定登录处理函数
    _msgHandlerMap.insert({LOGIN_MSG, std::bind(&ChatService::login, this, _1, _2, _3)});
    // 绑定注册处理函数
    _msgHandlerMap.insert({REG_MSG, std::bind(&ChatService::reg, this, _1, _2, _3)});
    // 绑定一对一聊天处理函数
    _msgHandlerMap.insert({ONE_CHAT_MSG, std::bind(&ChatService::oneChat, this, _1, _2, _3)});
    // 绑定添加好友处理函数
    _msgHandlerMap.insert({ADD_FRIEND_MSG, std::bind(&ChatService::addFriend, this, _1, _2, _3)});

    // 绑定创建群聊处理函数
    _msgHandlerMap.insert({CREATE_GROUP_MSG, std::bind(&ChatService::createGrop, this, _1, _2, _3)});
    // 绑定加入群聊组业务
    _msgHandlerMap.insert({ADD_GROUP_MSG, std::bind(&ChatService::addGroup, this, _1, _2, _3)});
    // 绑定群组聊天业务
    _msgHandlerMap.insert({GROUP_CHAT_MSG, std::bind(&ChatService::groupChat, this, _1, _2, _3)});
    
    // 绑定退出登录业务
    _msgHandlerMap.insert({LOGINOUT_MSG, std::bind(&ChatService::loginout, this, _1, _2, _3)});

    // 在服务层构造时连接redis
    if(_redis.connect())
    {
        // 设置上报消息的回调
        _redis.int_notify_handler(std::bind(&ChatService::handleRedisSubscribeMessage, this, _1, _2));
    }
}

 // 获取消息对应的处理函数
MsgHandler ChatService::getHandler(int msgid)
{
    // return this->_msgHandlerMap[msgid] 直接这样取，map的[]运算符重载函数可能有负重用（不存会插入）
    // 记录错误日志（直接使用muduo库中的日志模块），msgid没有对应的事件处理回调
    auto it = _msgHandlerMap.find(msgid);
    if (it == _msgHandlerMap.end())
    {
        // 返回一个默认的处理函数，空操作，提示没有该消息类型。可将服务器进行升级处理   -- 这里不可以抛出异常时服务器挂掉
        return [=] (const TcpConnectionPtr& conn, json& js, Timestamp time)
        {
            LOG_ERROR << "msgid" << msgid << "can not find handler!";
        };
    } 
    else
    {
        return _msgHandlerMap[msgid];
    }
}

// 处理登录业务
void ChatService::login(const TcpConnectionPtr& conn, json& js, Timestamp time)
{
    LOG_INFO << "do login service" ;
    // 登录时使用id和密码
    int id = js["id"].get<int>(); // 根据传入的参数类型进行转换
    string pwd = js["password"]; 
    User user = _userModel.query(id); // 根据用户id查询数据库
    if(user.getId() != -1 && user.getPwd() == pwd)
    {
        // 登录成功 判断用户状态是否已经在线
        if(user.getState() == "online")
        {
            // 该用户已经登录，不允许重复登录
            json response;
            response["msgid"] = LOGIN_MSG_ACK;
            response["errno"] = 1; 
            response["errmsg"] = "该账号已经登录，请重新输入新账号";
            conn->send(response.dump()); 
            
        }else
        {
            // 登录成功: 1、更新用户状态信息 offline => online 2、向客户端发送消息 3、将当前用户连接信息进行存储 4、 查询当前登录用户是否有离线消息 5、查询当前用户好友信息并返回 6、查询用户群主信息并接收离线消息
            user.setState("online");
            // 更新用户状态为online
            _userModel.updateState(user);
            // 将用户连接信息进行存储.  这里需要考虑线程安全问题：因为可能有多个线程同时与不同的客户端建立连接，都会操作这个map。在c++中map不是线程安全的，因此需要进行加锁保护
            {
                // 细粒度加锁，这个代码块结束自动释放锁
                lock_guard<mutex> lock(_connMutex);  
                _userConnMap.insert({id, conn});
            }
            
            // 向redis中订阅自己id的消息
            _redis.subscribel(id);

            json response;
            response["msgid"] = LOGIN_MSG_ACK;
            response["errno"] = 0;
            response["id"] = user.getId();
            response["name"] = user.getName();

            // 查询用户是否有离线消息
            vector<string> vec = _offlineMsgModel.query(id);
            if (!vec.empty())
            {
                response["offlinemsg"] = vec; // 添加离线消息字段
                // 读取该用户离线消息后，把该用户的离线消息清空
                _offlineMsgModel.remove(id);
            }

            // 查询当前登录用户的好友信息
            vector<User> friendsVec = _friendModel.query(id);
            if (!friendsVec.empty())
            {
                vector<string> friendsInfo;
                for (auto it = friendsVec.begin(); it != friendsVec.end(); ++it)
                {
                    json js;
                    js["id"] = it->getId();
                    js["name"] = (*it).getName();
                    js["state"] = (*it).getState();
                    friendsInfo.push_back(js.dump());
                }
                response["friends"] = friendsInfo;
            }

            // 查询用户群组信息
            vector<Group> groupsv = _groupModel.queryGroups(id); // 根据登录用户id查询用户所在的所有群组的信息
            if(!groupsv.empty())
            {
                // group:[{groupid:[xxx, xxx, xxx, xxx]}]
                vector<string> groupv;
                for (Group& group : groupsv)
                {
                    json groupjs;
                    groupjs["id"] = group.getId();
                    groupjs["groupname"] = group.getName();
                    groupjs["groupdesc"] = group.getDesc();
                    vector<string> userv; // 每个群主中成员
                    for(GroupUser& user : group.getUsers())
                    {
                        json userjs;
                        userjs["id"] = user.getId();
                        userjs["name"] = user.getName();
                        userjs["state"] = user.getState();
                        userjs["role"] = user.getRole();
                        userv.push_back(userjs.dump());// 将js序列化string类型，存入userv中
                    }
                    groupjs["users"] = userv;
                    groupv.push_back(groupjs.dump()); // 将每一个群的信息序列化后放入groupv中
                }
                response["groups"] = groupv;
            }
            conn->send(response.dump());
        }
       
    }
    else
    {
        // 登录失败
        json response;
        response["msgid"] = LOGIN_MSG_ACK;
        response["errno"] = 2; 
        response["errmsg"] = "账号或密码错误，请检查";
        conn->send(response.dump()); 
    }
}

// 处理注册业务
void ChatService::reg(const TcpConnectionPtr& conn, json& js, Timestamp time)
{
    LOG_INFO << "do register service" ;
    // 获取client传递的注册用户信息
    string name = js["name"];
    string pwd = js["password"];

    User user;
    user.setName(name);
    user.setPwd(pwd);
    bool state = _userModel.insert(user); // 将注册用户插入数据库
    if (state)
    {
        // 注册成功，并向client提示信息
        json response;
        response["msgid"] = REG_MSG_ACK;
        response["errno"] = 0; // 0：表示没有错误
        response["id"] = user.getId(); 
        conn->send(response.dump()); // 将json字符串序列化后发送
    }
    else
    {
        // 注册失败，返回错误信息
        json response;
        response["msgid"] = REG_MSG_ACK;
        response["errno"] = 1; // 1：表示有错误
        conn->send(response.dump()); // 将json字符串序列化后发送
    }
}

// 处理客户端异常退出
void ChatService::clientCloseException(const TcpConnectionPtr& conn)
{
    User user;
    {
        // 确保_userConnMap线程安全
        lock_guard<mutex> lock(_connMutex);
        // 遍历connMap，判断当前异常断开的连接是否存在  -- 注这里由于客户端异常断开，无法拿到userid，不能根据key进行查找，只能遍历进行比较conn
        for(auto it = _userConnMap.begin(); it != _userConnMap.end(); ++it)
        {
            if (it->second == conn)
            {
                // 从map表中删除用户的连接信息
                user.setId(it->first); // 找到异常断开的用户id，通过id修改用户在线状态
                _userConnMap.erase(it);
                LOG_INFO << it->first << "user CloseException";
                break;
            }
        }
    }

    // 用户下线，在redis中取消发布订阅的通道
    _redis.unsubscribel(user.getId());
    
    // 更新用户在线状态信息
    if(user.getId() != -1)
    {
        user.setState("offline");
        _userModel.updateState(user);
    }
   
}

// 一对一聊天业务
void ChatService::oneChat(const TcpConnectionPtr& conn, json& js, Timestamp time)
{
    int  toid = js["toid"].get<int>(); // 接收方id
    {
        // 判断接收方是否在线
        lock_guard<mutex> lock(_connMutex);
        auto it = _userConnMap.find(toid);
        if (it != _userConnMap.end())
        {
            // toid用户在线  ，服务器转发给它，这就是用_userConnMap存储连接信息的目的
            it->second->send(js.dump());
            return;
        }
    }

    // 当前服务器中_userConnMap中没有userid信息，可能登录在别的服务器上
    User user = _userModel.query(toid);
    if (user.getState() == "online")
    {
        cout << "userid" << user.getId() << "dont hve this server so to redis" << js.dump() << endl;
        // 说明聊天对象在别的服务器，将消息内容发布到tod对应的通道，由redis发送到对应服务器
        _redis.publish(toid, js.dump());
        return ;
    }

    // toid 用户不在线，发送离线消息
    _offlineMsgModel.insert(toid, js.dump());

}

// 服务器异常退出，业务重置方法
void ChatService::reset()
{
    // 把online状态用户置为offline
    _userModel.resetState();
}

// 添加好友业务
void ChatService::addFriend(const TcpConnectionPtr& conn, json& js, Timestamp time)
{
    int userid = js["id"].get<int>();
    int friendid = js["friendid"].get<int>();

    // 存储好友信息
    _friendModel.insert(userid, friendid);
}

 // 创建群聊业务
void ChatService::createGrop(const TcpConnectionPtr& conn, json& js, Timestamp time)
{
    int userid = js["id"].get<int>();
    string groupname = js["groupname"];
    string groupdesc = js["groupdesc"];

    // 存储创建的群组信息
    Group group(-1, groupname, groupdesc); // 初始化群聊信息类，群聊id通过插入数据放回主键进行赋值
    if (_groupModel.createGroup(group))
    {
        // 存储创建人信息
        _groupModel.addGroup(userid, group.getId(), "creator"); // 创建人在群组里面角色为creator
    }
}
// 加入群聊组业务
void ChatService::addGroup(const TcpConnectionPtr& conn, json& js, Timestamp time)
{
    int userid = js["id"].get<int>();
    int groupid = js["groupid"].get<int>();
    _groupModel.addGroup(userid, groupid, "normal"); // 普通用户进入群聊角色为normal
}

// 群组聊天业务
void ChatService::groupChat(const TcpConnectionPtr& conn, json& js, Timestamp time)
{
    int userid = js["id"].get<int>();
    int groupid = js["groupid"].get<int>();
    vector<int> useridVec = _groupModel.queryGroupsUsers(userid, groupid); // 通过用户id和所在群的id，获取这个群里面所有的用户id（因为群聊需要将消息转发给所有人）
    lock_guard<mutex> lock(_connMutex);
    for(auto it = useridVec.begin(); it != useridVec.end(); ++it)
    {
        auto rit = _userConnMap.find(*it); // 判断当前群聊用户id是否在线
        if (rit != _userConnMap.end())
        {
            // 将消息转发给当前在线的群聊成员
            rit->second->send(js.dump());
        }
        else 
        {
            // 查询群聊用户是否在线
            User user = _userModel.query(*it);
            if (user.getState() == "online")
            {
                cout << "群聊用户在别的服务器登录: " << user.getId() << " " << *it << "msg:" << js.dump() <<  endl;
                // 当前群聊用户在别的服务器登录，将消息发布到user.getId()对应的通道
                _redis.publish(user.getId(), js.dump());
            }
            else
            {
                // 存储离线消息 -- 成员用户不在线，转发离线消息
                _offlineMsgModel.insert(*it, js.dump());
            }
        }
    }
}

// 退出登录业务
void ChatService::loginout(const TcpConnectionPtr& conn, json& js, Timestamp time)
{
    int userid = js["id"].get<int>();
    {
        lock_guard<mutex> lock(_connMutex); // 对_userConnMap的安全访问
        auto it = _userConnMap.find(userid);
        if  (it != _userConnMap.end())
        {
            _userConnMap.erase(it); // 删除当前需要退出用户的连接信息
        }
    }

    // 用户注销，相当于下线，在redis中取消订阅通道
    _redis.unsubscribel(userid);

    // 更新用户状态
    User user(userid, "", "", "offline");
    _userModel.updateState(user);
}

// 从redis消息队列中获取订阅消息
void ChatService::handleRedisSubscribeMessage(int userid, string msg)
{
    // json js = json::parse(msg.c_str());

    lock_guard<mutex>lock(_connMutex);
    auto it = _userConnMap.find(userid);
    if (it != _userConnMap.end())
    {
        cout << "userid have this server " << msg << endl;
        // 通过userid找到，将消息进行发送（跨服务器通信的，别的服务器需要给这台服务器上userid用户聊天
        it->second->send(msg);
        return;
    }

    // 刚好当前用户下线了，发送离线消息（将消息存储在离线消息表中）
    _offlineMsgModel.insert(userid, msg);

}