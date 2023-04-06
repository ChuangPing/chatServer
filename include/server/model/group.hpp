#ifndef GROUP_H
#define GROUP_H
#include "groupuser.hpp"
#include <string>
#include <vector>
using namespace std;

// group表的ORM类
class Group
{
public:
    Group(int id = -1, string name = "", string desc = "")
    {
        this->id = id;
        this->name = name;
        this->desc = desc;
    }
    void setId(int id) {this->id = id;}
    void setName(string name) {this->name = name;}
    void setDesc(string desc) {this->desc = desc;}

    int getId() {return this->id;}
    string getName() {return this->name;}
    string getDesc() {return this->desc;}
    vector<GroupUser>& getUsers() {return this->users;}
private:
    int id; // 群聊id
    string name; // 群聊名称
    string desc; // 群聊描述
    vector<GroupUser> users; // 群聊中所有的成员信息
};
#endif