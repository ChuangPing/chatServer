#ifndef GROUPUSER_H
#define GROUPUSER_H
#include "user.hpp"

// 群组用户ORM类：群组用户相对于普通user，多了一个role角色，因此从user类直接继承，复用user中的其它信息
class GroupUser : public User
{
public:
    void setRole(string role){ this->role = role;}
    string getRole() {return this->role;}

private:
    string role; // 群组用户角色
};
#endif