#include <fstream>
// #include <nlohmann/json.hpp>头文件路径的引入，这里将json.hpp文件放到linux系统中的/usr/local/include路径下，
// 这是系统默认头文件路径，在编译时系统会自动查找该路径。我们在/usr/local/include路径下创建/nlohmann/json.hpp，如下图所示
#include <nlohmann/json.hpp>
using json = nlohmann::json;
#include<iostream>
#include<vector>
#include<map>
using namespace std;


// json反序列化示例1
void func1()
{
    json js;
    // 将js看成容器，直接键值放数据
    js["msg_type"] = 2;
    js["form"] = "zhang san";
    js["to"] = "li si";
    js["msg"] = "hello, what are you doing now?";

    cout << js << endl;  // {"form":"zhang san","msg":"hello, what are you doing now?","msg_type":2,"to":"li si"}

    // 将json转换为字符串，dump：输出的意思
    string sendBuf = js.dump();
    cout << sendBuf.c_str() << endl;
}

// json反序列化示例2
void func2()
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
}

// json 反序列化3
void func3()
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
}



int main()
{
    func1();
    func2();
    func3();
    return 0;
}