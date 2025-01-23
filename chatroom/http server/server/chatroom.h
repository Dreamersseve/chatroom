#ifndef CHATROOM_H
#define CHATROOM_H

#include "../Server.h"
#include <vector>
#include <string>
#include <ctime>
#include <deque>
#include "../../json/json.h"
#include "../datamanage.h"
#include "../tool.h"
#include <mutex>
#include <fstream>
#include <sstream>
#include <filesystem>
#include "../httplib.h"  // 假设你使用的是这个库

using namespace std;
const int MAXROOM = 1000;
class chatroom {
public:
    vector<int> allowID;
private:
    const int MAXSIZE = 1000;
    deque<Json::Value> chatMessages;
    string chatTitle;
    int roomid;
    int type;
    

    // 初始化聊天室
    void initializeChatRoom();

    // 将 Json 消息转为字符串
    string transJsonMessage(Json::Value m);

    //检查请求是否在许可名单内
    bool checkAllowId(const int ID);
    bool checkAllowId(const string name);

    // 解析 Cookie
    void transCookie(std::string& cid, std::string& uid, std::string cookie);

    // 检查Cooie
    bool checkCookies(const httplib::Request& req);

    // 获取聊天消息
    void getChatMessages(const httplib::Request& req, httplib::Response& res);

    // 处理 POST 请求发送消息
    void postChatMessage(const httplib::Request& req, httplib::Response& res, const Json::Value& root);

    // 获取用户名
    void getUsername(const httplib::Request& req, httplib::Response& res);

    // 设置静态文件路由
    void setupStaticRoutes();

    // 判断是否是有效图片类型
    bool isValidImage(const std::string& filename);

    // 上传图片
    void uploadImage(const httplib::Request& req, httplib::Response& res);

    // 设置聊天相关路由
    void setupChatRoutes();

    // 用于同步的锁
    std::mutex mtx;

    const std::vector<std::string> allowedImageTypes = { ".jpg", ".jpeg", ".png", ".gif", ".bmp" ,"webp" };

public:
    

    // 构造函数和析构函数
    //chatroom(int id) : roomid(id) {}
    //~chatroom() {}

    // 系统消息
    void systemMessage(string message);

    //清空消息内容
    void clearMessage();

    // 启动聊天室
    bool start();

    //设置类型
    void settype(int type);

    string gettittle();

    void settittle(string tittle);

    chatroom(int id = 0);

    void init();
};

extern vector<chatroom> room;

extern bool used[MAXROOM];
int addroom();

void delroom(int x);

#endif // CHATROOM_H