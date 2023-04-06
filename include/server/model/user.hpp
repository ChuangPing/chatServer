#ifndef USER_H
#define USER_H
#include<iostream>
using namespace std;

class User // 数据库映射类 ORM类
{
public:
    User(int id = -1, string name = " ", string pwd="", string state="offline")  // 数据库中状态的默认值为offline
    {
        this->id = id;
        this->name = name;
        this->password = pwd;
        this->state = state;
    }
    void setId(int id) {this->id = id;} // 可以判断id的合法性，简便没做
    void setName(string name) {this->name = name;}
    void setPwd(string pwd) {this->password = pwd;}
    void setState(string state) {this->state = state;}

    int getId() {return this->id;}
    string getName() {return this->name;}
    string getPwd() {return this->password;}
    string getState() {return this->state;}
private:
    int id;
    string name;
    string password;
    string state;


};
#endif