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
#include "../httplib.h"  // ������ʹ�õ��������

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
    

    // ��ʼ��������
    void initializeChatRoom();

    // �� Json ��ϢתΪ�ַ���
    string transJsonMessage(Json::Value m);

    //��������Ƿ������������
    bool checkAllowId(const int ID);
    bool checkAllowId(const string name);

    // ���� Cookie
    void transCookie(std::string& cid, std::string& uid, std::string cookie);

    // ���Cooie
    bool checkCookies(const httplib::Request& req);

    // ��ȡ������Ϣ
    void getChatMessages(const httplib::Request& req, httplib::Response& res);

    // ���� POST ��������Ϣ
    void postChatMessage(const httplib::Request& req, httplib::Response& res, const Json::Value& root);

    // ��ȡ�û���
    void getUsername(const httplib::Request& req, httplib::Response& res);

    // ���þ�̬�ļ�·��
    void setupStaticRoutes();

    // �ж��Ƿ�����ЧͼƬ����
    bool isValidImage(const std::string& filename);

    // �ϴ�ͼƬ
    void uploadImage(const httplib::Request& req, httplib::Response& res);

    // �����������·��
    void setupChatRoutes();

    // ����ͬ������
    std::mutex mtx;

    const std::vector<std::string> allowedImageTypes = { ".jpg", ".jpeg", ".png", ".gif", ".bmp" ,"webp" };

public:
    

    // ���캯������������
    //chatroom(int id) : roomid(id) {}
    //~chatroom() {}

    // ϵͳ��Ϣ
    void systemMessage(string message);

    //�����Ϣ����
    void clearMessage();

    // ����������
    bool start();

    //��������
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