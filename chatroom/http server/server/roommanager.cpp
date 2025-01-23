#include "../Server.h"
#include <vector>
#include <string>
#include <ctime>
#include <deque>
#include <../../json/json.h>
#include "../datamanage.h"
#include "../tool.h"
using namespace std;
#include <filesystem>
#include "chatroom.h"

vector<chatroom> room(MAXROOM);

bool used[MAXROOM];
int addroom() {
    for (int i = 1; i < MAXROOM; i++) {
        if (used[i] == 0) {
            used[i] = 1;
            room[i].init();
            return i;
        }
    }
    return -1;
}

void delroom(int x) {
    used[x] = 0;
    room[x].init();
}


void transCookie(std::string& cid, std::string& uid, std::string cookie) {
    std::string::size_type pos1 = cookie.find("clientid=");
    if (pos1 != std::string::npos) {
        pos1 += 9; // Skip over "clientid="
        std::string::size_type pos2 = cookie.find(";", pos1);
        if (pos2 == std::string::npos) pos2 = cookie.length();
        cid = cookie.substr(pos1, pos2 - pos1);
    }

    std::string::size_type pos3 = cookie.find("uid=");
    if (pos3 != std::string::npos) {
        pos3 += 4; // Skip over "uid="
        std::string::size_type pos4 = cookie.find(";", pos3);
        if (pos4 == std::string::npos) pos4 = cookie.length();
        uid = cookie.substr(pos3, pos4 - pos3);
    }
}

string getRoomName(int roomid) {
    return room[roomid].gettittle();
}

void getRoomList(const httplib::Request& req, httplib::Response& res) {
    std::string cookies = req.get_header_value("Cookie");
    std::string password, uid;
    transCookie(password, uid, cookies);

    int uid_;
    str::safeatoi(uid, uid_);

    std::string List = manager::FindUser(uid_)->getcookie();  // 获取用户加入的聊天室 ID 列表
    std::vector<std::string> roomIds;
    std::stringstream ss(List);
    std::string roomId;

    // 分割 List 字符串，提取每个聊天室 ID
    while (std::getline(ss, roomId, '&')) {
        roomIds.push_back(roomId);
    }

    // 创建一个 JSON 响应对象
    Json::Value response(Json::arrayValue);

    // 遍历所有聊天室 ID，获取对应的聊天室名称
    for (const std::string& roomId : roomIds) {
        int room_id;
        str::safeatoi(roomId, room_id);
        std::string roomName = getRoomName(room_id);  // 获取聊天室名称

        // 构造每个聊天室的 JSON 对象
        Json::Value room;
        room["id"] = roomId;  // 添加聊天室的 ID
        room["name"] = roomName;  // 添加聊天室的名称

        response.append(room);  // 将聊天室对象添加到 JSON 数组中
    }

    // 设置响应头，支持跨域
    res.set_header("Access-Control-Allow-Origin", "*");
    res.set_header("Content-Type", "application/json");

    // 设置响应内容为 JSON 格式的字符串
    res.set_content(response.toStyledString(), "application/json");
}






void start() {

   Server& server = server.getInstance(HOST);
        // 提供聊天记录的 GET 请求
   server.getInstance().handleRequest("/list", getRoomList);

}