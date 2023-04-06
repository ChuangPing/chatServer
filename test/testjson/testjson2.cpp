#include <fstream>
// #include <nlohmann/json.hpp>头文件路径的引入，这里将json.hpp文件放到linux系统中的/usr/local/include路径下，
// 这是系统默认头文件路径，在编译时系统会自动查找该路径。我们在/usr/local/include路径下创建/nlohmann/json.hpp，如下图所示
#include <nlohmann/json.hpp>
using json = nlohmann::json;
#include<iostream>
#include<vector>
#include<map>
using namespace std;


// json序列化示例1
string func1()
{
    json js;
    // 将js看成容器，直接键值放数据
    js["msg_type"] = 2;
    js["from"] = "zhang san";
    js["to"] = "li si";
    js["msg"] = "hello, what are you doing now?";

    // cout << js << endl;  // {"form":"zhang san","msg":"hello, what are you doing now?","msg_type":2,"to":"li si"}

    // 将json转换为字符串，dump：输出的意思
    string sendBuf = js.dump();
    // cout << sendBuf.c_str() << endl;
    return sendBuf;
}

// json反序列化示例2
string func2()
{
    json js;
    // 值为数组
    js["id"] = {1, 2, 3, 4, 5};
    js["name"] = "zhang san ";
    // 添加对象
    js["msg"]["zhang san"] = "hello word";
    js["msg"]["liu shuo"] = "hello china";
    // 上面等同于下面这句一次性添加数组对象
    js["msg"] = {{"zhang san", "hello word"}, {"liu shuo", "hello china"}};
    cout << js << endl; // {"id":[1,2,3,4,5],"msg":{"liu shuo":"hello china","zhang san":"hello word"},"name":"zhang san "}
    string sendBuf = js.dump();
    return sendBuf;
}

// json 反序列化3
string func3()
{
    json js;
    // 直接序列化一个vector容器
    vector<int> vec;
    vec.push_back(1);
    vec.push_back(2);
    vec.push_back(3);
    js["lis"] = vec;

    // 直接序列化一个map容器
    map<int, string> m;
    m.insert({1, "皇上"});
    m.insert({2, "娘娘"});
    m.insert({3, "阿哥"});

    js["path"] = m;

    cout << js << endl; // {"lis":[1,2,3],"path":[[1,"皇上"],[2,"娘娘"],[3,"阿哥"]]}
    string sendBuf = js.dump();
    return sendBuf;
}



int main()
{
    string recvBuf1 = func1();
    // 数据的反序列化  json字符串 => 反序列化 数据对象（看作容器，方便访问）
    json jsbuf1 = json::parse(recvBuf1);
    cout << jsbuf1["msg_type"] << endl;
    cout << jsbuf1["from"]  << endl;
    cout << jsbuf1["to"] << endl;
    cout << jsbuf1["msg"] << endl;

    string recvBuf2 = func2();
    json jsbuf2 = json::parse(recvBuf2);
    auto arr = jsbuf2["id"]; // arr为数组
    cout << arr[2] << endl; 
    auto msgjs = jsbuf2["msg"];
    cout << msgjs["zhang san"] << endl;
    cout << msgjs["liu shuo"] << endl;
    // func2();
    // func3();

    string recvBuf3 = func3();
    json jsbuf3 = json::parse(recvBuf3);
    vector<int> vec = jsbuf3["lis"];
    // 遍历容器
    for(int& v : vec)
    {
        cout << v << endl;
    }

    map<int, string> mymap = jsbuf3["path"];
    for(auto &p : mymap)
    {
        cout << p.first << " " << p.second << endl;
    }
    cout << endl;
    return 0;
}