#include "json.hpp"
#include <iostream>
#include <thread>
#include <vector>
#include <chrono>
#include<ctime>
using namespace std;
using json = nlohmann::json;

#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <argp.h>

#include "group.hpp"
#include "user.hpp"
#include "public.hpp"


// 记录当前系统登录的用户信息
User g_currentUser;
// 记录当前登录用户的好友信息
vector<User> g_currentUserFirendList;
// 记录当前登录用户的群组信息列表
vector<Group> g_currentUserGroupList;
// 显示当前登录成功用户的基本信息
void showCurrentUserData();
// 控制主菜单页面程序变量
bool isMainMenuRunning = false;

// 接收线程  -- 接收用户输入
// void writeTaskHandler(int clientfd);
// 获取系统时间  -- 聊天时会加上系统时间
string getCurrentTime();
// 主聊天页面程序
void mainMenu(int);
//  接收线程
void readTaskHandler(int client);


// 聊天客户端程序实现， main线程用作发送线程，子线程用作接收线程（接收服务器返回的信息）
int main(int argc, char** argv)
{
    if (argc < 3)
    {
        cerr << "command invalid! example: ./ChatClient 127.0.0.1 6000" << endl;
        exit(-1);
    }

    // 解析通过命令传递的ip和port
    char* ip = argv[1];
    uint16_t port = atoi(argv[2]);

    // 创建client端的socket
    int clientfd = socket(AF_INET, SOCK_STREAM, 0); // socket类型为tcp
    if (clientfd == -1)
    {
        cerr << "socket create error" << endl;
        exit(-1);
    }

    // 填写clientfd幼链接的server信息
    sockaddr_in server;
    memset(&server, 0, sizeof(sockaddr_in)); // 将结构体清零

    server.sin_family = AF_INET;
    server.sin_port = htons(port);
    server.sin_addr.s_addr = inet_addr(ip);

    // client 和 server进行连接
    if (connect(clientfd, (sockaddr *)&server, sizeof(sockaddr)) == -1)
    {
        cerr << "connect server error" << endl;
        close(clientfd);
        exit(-1);
    }
    // main线程用户接收用户输入，负责发送数据
    for(;;)
    {
        // 显示首页菜单：登录、注册、退出
        cout << "=====================================" << endl;
        cout << "1. login" << endl;
        cout << "2. register" << endl;
        cout << "3. quit" << endl;
        cout << "=====================================" << endl;
        cout << "choice" << endl;
        int choice = 0; // 记录用户选择
        cin >> choice;
        cin.get(); // 读掉缓冲区的残留回车

        switch(choice)
        {
            case 1:  // Login业务
            {
                int id = 0; // 登录用户id
                char pwd[50] = {0}; // 用户密码
                cout << "userid: " << endl;
                cin >> id;
                cin.get(); // 读掉缓冲区的残留回车
                cout << "userpassword：" << endl;
                cin.getline(pwd, 50);

                json js;
                js["msgid"] = LOGIN_MSG;
                js["id"] = id;
                js["password"] = pwd;
                string request = js.dump(); // 将json序列化后发送

                int len = send(clientfd, request.c_str(), strlen(request.c_str()) + 1, 0);
                if (len == -1)
                {
                    cerr << "send login msg error" << request << endl;
                }
                else 
                {
                    char buffer[1024] = {0};
                    len = recv(clientfd, buffer, 1024, 0);
                    if (len == -1)
                    {
                        cerr << "recv login response err" << endl;
                    }
                    else
                    {
                        // 将接收的数据进行反序列化，判断返回的errno
                        json responsejs = json::parse(buffer);
                        if (responsejs["errno"].get<int>() != 0)
                        {
                            // 登录失败
                            cerr << responsejs["errmsg"] << endl;
                        }
                        else
                        {
                            // cout << "login success responsejson:" << responsejs << endl;
                            // 登录成功：记录当前用户的id和name
                            g_currentUser.setId(responsejs["id"].get<int>());
                            g_currentUser.setName(responsejs["name"]);

                            // 记录当前用户的好友列表
                            if (responsejs.contains("friends")) // 判断当前用户是否包含好友字段，当用户没有添加好友时就没有friend字段
                            {
                                // 初始化：防止登录两次后全局的g_currentUserFriendList保留上一次的信息
                                g_currentUserFirendList.clear();
                                vector<string> vec = responsejs["friends"]; // 需要对取friend字段进行如上面的保护
                                for(string& str : vec)
                                {
                                    json js = json::parse(str);
                                    User user;
                                    user.setId(js["id"].get<int>());
                                    user.setName(js["name"]);
                                    user.setState(js["state"]);
                                    g_currentUserFirendList.push_back(user); // 将好友信息存储在全局的friengdList容器中
                                }
                            }

                            // 记录当前用户的群组列表信息
                            if (responsejs.contains("groups"))
                            {
                                // 初始化
                                g_currentUserGroupList.clear();
                                vector<string> vec1 = responsejs["groups"];
                                for (string& groupStr : vec1)
                                {
                                    json groupjs = json::parse(groupStr);
                                    // 获取当前登录用户加入的群聊
                                    Group group;
                                    group.setId(groupjs["id"].get<int>());
                                    group.setName(groupjs["groupname"]);
                                    group.setDesc(groupjs["groupdesc"]);

                                    // 获取每个群聊的用户信息
                                    vector<string> vec2 = groupjs["users"];
                                    for(string & userStr : vec2)
                                    {
                                        GroupUser user;
                                        json js = json::parse(userStr);
                                        user.setId(js["id"].get<int>());
                                        user.setName(js["name"]);
                                        user.setState(js["state"]);
                                        user.setRole(js["role"]);
                                        group.getUsers().push_back(user); // 将当前群聊的用户信息添加group中的groupusers容器中
                                    }
                                    g_currentUserGroupList.push_back(group);
                                }
                            }

                            // 显示登录用户的基本信息
                            showCurrentUserData();

                            // 显示当前登录用户的离线消息：个人聊天的离线消息/群聊的离线消息
                            if (responsejs.contains("offlinemsg"))
                            {
                                cout << "offlien message: " << endl << endl ;
                                vector<string> vec = responsejs["offlinemsg"];
                                for (string& str : vec)
                                {
                                    json js = json::parse(str);
                                    // 判断当前离线消息是群聊的还是个人的离线消息 
                                    if(js["msgid"] == ONE_CHAT_MSG)
                                    {
                                        // time + [id] + "name" + "sid" + "xxxx"  --- 消息显示格式
                                        cout << "个人离线消息：" << js["time"] << " [ " << js["id"] << " ] " << js["name"] << " said: " << js["msg"] << endl;
                                    }
                                    if (js["msgid"] == GROUP_CHAT_MSG)
                                    {
                                         cout << " 群离线消息： [" << js["groupid"] << " ] " << js["time"].get<string>() << " [" << js["id"] << " ] " << js["name"].get<string>() << " said: " << js["msg"].get<string>() << endl;
                                    }
                                   
                                }
                                cout << endl;
                            }

                            // 登录成功，启动接收线程负责与服务端进行通信，接收数据
                            // 登录成功，启动接收线程负责接收数据，该线程只启动一次
                            static int threadnumber = 0;
                            if (threadnumber == 0)
                            {
                                std::thread readTask(readTaskHandler, clientfd);
                                readTask.detach(); // 与主进程分离
                                threadnumber ++;
                            }
                            
                            // 进入聊天页面
                            isMainMenuRunning = true;
                            mainMenu(clientfd);
                        }
                    }
                }
            }
            break;
            case 2: //用户注册
            {
                 // 接收用户输入
                char name[50] = {0};
                char pwd[50] = {0};
                cout << "username：" << endl;
                cin.getline(name, 50);
                cout << "password: " << endl;
                cin.getline(pwd, 50);

                // 组装json
                json js;
                js["msgid"] = REG_MSG;
                js["name"] = name;
                js["password"] = pwd;
                string request = js.dump();
                // 发送
                int len = send(clientfd, request.c_str(), strlen(request.c_str()) + 1, 0);
                if (len == -1 )
                {
                    cerr << "send reg msg error: " << request << endl;
                }
                else 
                {
                    // 发送成功，接收服务器的放回数据
                    char buffer[1024] = {0};
                    len = recv(clientfd, buffer, 1024, 0);
                    if (len == -1)
                    {
                        cerr << "recv req response error" << endl;
                    }else 
                    {
                        json responsejs = json::parse(buffer);
                        if (responsejs["errno"].get<int>() != 0) // 服务端的错误状态码不为0
                        {
                            cerr << name << "is already exist, register error" << endl; // 注册失败（只有名字重复，服务端插入数据库失败）
                        }
                        else
                        {
                            cout << name << "register success, userid is " << responsejs["id"] << ", do not forget it!" << endl;
                        }
                    }
                }
            }
            case 3: // quit退出
            {
                close(clientfd);
                exit(0);
            }
            default:
            {
                cerr << "invalid input!" << endl;
                break;
            }
        }
    }
    return 0;
}

//  接收线程
void readTaskHandler(int clientfd)
{
   while(isMainMenuRunning)
   {
        char buffer[1024] = {0};
        int len = recv(clientfd, buffer, 1024, 0);
        if (len == -1 || len == 0)
        {
            close(clientfd);
            exit(-1);
        }

        // 解析服务端传递的信息
        json js = json::parse(buffer);
        if (js["msgid"].get<int>() == ONE_CHAT_MSG)
        {
            // 一对一聊天
            cout << "一对一聊天：" << js["time"].get<string>() << " [ " << js["id"] << " ] " << js["name"].get<string>() << " sid: " << js["msg"].get<string>() << endl;
            continue;
        }
        else if (js["msgid"].get<int>() == GROUP_CHAT_MSG)
        {
            cout << " 群消息： [" << js["groupid"] << " ] " << js["time"].get<string>() << " [" << js["id"] << " ] " << js["name"].get<string>() << " said: " << js["msg"].get<string>() << endl;
            continue;
        }
   }
}

// 显示当前登录成功用户的基本信息
void showCurrentUserData()
{
    cout << "============= login user infor =========" << endl << endl;
    cout <<  "current login user => id: " << g_currentUser.getId() << " name: " << g_currentUser.getName() << endl;
    cout << "login user friendlist: "<< endl << endl ;
    if (!g_currentUserFirendList.empty())
    {
        auto it = g_currentUserFirendList.begin();
        for (; it != g_currentUserFirendList.end(); ++it)
        {
            cout << "好友id: " << it->getId() << " 好友姓名：" << it->getName() << " 好友状态：" << it->getState() << endl;
        }
        cout << endl;
    }

    cout <<"gruop list: "<< endl << endl ;
    if (!g_currentUserGroupList.empty())
    {
        for(auto it = g_currentUserGroupList.begin(); it != g_currentUserGroupList.end(); ++it)
        {
            cout << "群组id：" << it->getId() << " 群组昵称：" << it->getName() << " 群组描述：" << it->getDesc() << endl; 
            // 遍历群组中的每一个成员
            cout << "群组成员信息：" << endl;
            for(GroupUser user : it->getUsers())
            {
                cout << "用户id：" << user.getId() << " 用户昵称：" << user.getName() <<  " 用户状态：" << user.getState() <<" 用户角色：" << user.getRole() << endl;
            }
        }
        cout << endl;
    }
    // cout << "=====================================" << endl << endl;
}

// help  command handler  -- 帮助命令函数，提示系统可以支持的命令
void help(int fd = 0, string str = ""); // 提供无参调用
// addfriend command handler  -- 添加朋友命令对应的处理函数
void addfriend(int clientfd, string str);
// chat command handler -- 一对一聊天命令处理函数
void chat(int clientfd, string str);
// creategroup command handler  -- 创建群组命令处理函数
void creategroup(int clientfd, string str);
// addgroup command handler  --  添加群组处理函数
void addgroup(int clientfd, string str);
// groupchat command handler  --  群组聊天处理函数
void groupchat(int clientfd, string str);
// loginout command handler  -- 退出登录处理函数
void loginout(int clientfd, string str);

// 系统支持的客户端命令以及格式
unordered_map<string, string> commandMap = {
    {"help", "显示所有支持的命令, 格式help"},
    {"addfriend", "添加好友，格式addfriend:friendid"},
    {"chat", "一对一聊天，格式chat:friendid:message"},
    {"creategroup", "创建群组, 格式creategroup:groupname:groupdesc"},
    {"addgroup", "加入群组，格式addgroup:groupid"},
    {"groupchat", "群聊，格式groupchat:groupid:message" },
    {"loginout", "退出登录，格式:loginout"}};


// 注册系统支持的客户端命令处理 -- 将对应的命令和对应的函数利用函数对象function进行绑定（这样写有好处，下次再有别的命令，只需要写好函数，在这里添加即可
unordered_map<string, function<void(int, string)>> commandHandlerMap = { // 用户登录成功后可执行的命令
    {"help", help}, // help对应help函数
    {"chat", chat}, // 聊天命令
    {"addfriend", addfriend}, // 添加好友
    {"creategroup", creategroup}, // 创建群聊
    {"addgroup", addgroup}, // 添加群聊
    {"groupchat", groupchat}, // 进行群聊
    {"loginout", loginout} // 退出
};

// 主聊天页面程序
void mainMenu(int clientfd)
{
    help(); // 提示用户支持的命令
    char buffer[1024] = {0};
    while(isMainMenuRunning)
    {
        cout << "请输入对应的命令:和对应的命令内容：" << endl;
        cin.getline(buffer, 1024); // 获取用户对应的输入 char类型
        string commandbuffer(buffer); // 使用char类型初始化string类型 comman
        string command ; // 存储命令
        int idx = commandbuffer.find(":"); // 将用户输入的命令和命令的内容分开
        if (idx == -1)
        {
            command = commandbuffer; // 只有命令没有内容
        }
        else
        {
            command = commandbuffer.substr(0, idx); // 截取的起始位置，和截取的长度
        }

        auto it = commandHandlerMap.find(command);
        if (it == commandHandlerMap.end())
        {
            cerr << "invalid input command!" << endl;
            continue;
        }

        // 调用相应命令的事件处理回调，mainMenu对修改关闭，添加新命令对应的处理函数，不需要修改这里的代码
        it->second(clientfd, commandbuffer.substr(idx + 1, commandbuffer.size() - idx )); // 调用对应命令的处理函数（并将除开命令的参数进行传递
    }
}

// help  command handler  -- 帮助命令函数，提示系统可以支持的命令
void help(int, string)
{
    cout << "show command list >>>" << endl;
    for (auto& p : commandMap)
    {
        cout << p.first <<  " : " << p.second << endl;
    }  
    cout << endl;
}

// addfriend command handler  -- 添加朋友命令对应的处理函数
void addfriend(int clientfd, string str)
{
    int friendid = atoi(str.c_str()); // 需要添加为朋友的id
    json js;
    js["msgid"] = ADD_FRIEND_MSG;
    js["id"] = g_currentUser.getId();
    js["friendid"] = friendid;
    string buffer = js.dump();

    int len = send(clientfd, buffer.c_str(), strlen(buffer.c_str()) + 1, 0);
    if (len == -1 )
    {
        cerr << "send addfriend msg error" <<  buffer << endl;
    }
}

// chat command handler -- 一对一聊天命令处理函数
void chat(int clientfd, string str)
{
    int idx = str.find(":");
    if (idx == -1)
    {
        cerr << "chat command invalid" << endl;
        return ;
    }
    int friendid = atoi(str.substr(0, idx).c_str());
    string message = str.substr(idx + 1, str.size() - idx);

    json js;
    js["msgid"] = ONE_CHAT_MSG;
    js["id"] = g_currentUser.getId();
    js["name"] = g_currentUser.getName();
    js["toid"] = friendid;
    js["msg"] = message;
    js["time"] = getCurrentTime();
    string buffer = js.dump();

    int len = send(clientfd, buffer.c_str(), strlen(buffer.c_str()) + 1, 0);
    if (len == -1 )
    {
        cerr << "send chat msg err " << buffer << endl;
    }
}

// creategroup command handler  -- 创建群组命令处理函数
void creategroup(int clientfd, string str)
{
    int idx = str.find(":");
    if (idx == -1)
    {
        cerr << "creategroup command invalid" << endl;
        return ;
    }

    string groupname = str.substr(0, idx);
    string groupdesc = str.substr(idx+1, str.size() - idx);

    json js;
    js["msgid"] = CREATE_GROUP_MSG;
    js["id"] = g_currentUser.getId();
    js["groupname"] = groupname;
    js["groupdesc"] = groupdesc;
    string buffer = js.dump();

    int len = send(clientfd, buffer.c_str(), strlen(buffer.c_str()) + 1, 0);
    if (len == -1)
    {
        cerr << "send creategroup msg error" << buffer << endl;

    }
}

// addgroup command handler  --  添加群组处理函数
void addgroup(int clientfd, string str)
{
    int groupid = atoi(str.c_str());
    json js;
    js["msgid"] = ADD_GROUP_MSG;
    js["id"] = g_currentUser.getId();
    js["groupid"] = groupid;
    string buffer = js.dump();

    int len = send(clientfd, buffer.c_str(), strlen(buffer.c_str()) + 1, 0);
    if (len == -1)
    {
        cout << "send addgroup msg error" << buffer << endl;
    }
}

// groupchat command handler  --  群组聊天处理函数
void groupchat(int clientfd, string str)
{
    int idx = str.find(":");
    if (idx == -1)
    {
        cerr << "groupchat command invalid" << endl;
        return ;
    }

    int groupid = atoi(str.substr(0, idx).c_str());
    string message = str.substr(idx + 1, str.size() - idx);

    json js;
    js["msgid"] = GROUP_CHAT_MSG;
    js["id"] = g_currentUser.getId();
    js["name"] = g_currentUser.getName();
    js["groupid"] = groupid;
    js["msg"] = message;
    js["time"] = getCurrentTime();
    string buffer = js.dump();

    int len = send(clientfd, buffer.c_str(), strlen(buffer.c_str()) + 1, 0);
    if (len == -1)
    {
        cerr << "send groupchat msg error" << buffer << endl;
    }
}

// loginout command handler  -- 退出登录处理函数
void loginout(int clientfd, string str)
{
    json js;
    js["msgid"] = LOGINOUT_MSG;
    js["id"] = g_currentUser.getId();
    string buffer = js.dump();

    int len = send(clientfd, buffer.c_str(), strlen(buffer.c_str()) + 1, 0);
    if (len == -1)
    {
        cerr << "send loginout msg error" << buffer << endl;
    }else 
    {
        isMainMenuRunning = false; // 不在显示聊天主界面，而是显示登录注册页面
    }
}




// 获取系统时间  -- 聊天时会加上系统时间
string getCurrentTime()
{
    auto tt = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    struct tm* ptm = localtime(&tt);
    char date[60] = {0};
    sprintf(date, "%d-%02d-%02d %02d:%02d:%02d", (int)ptm->tm_year + 1900, (int)ptm->tm_mon + 1, (int)ptm->tm_mday, (int)ptm->tm_hour, (int)ptm->tm_min, (int)ptm->tm_sec);
    return std::string(date);
}