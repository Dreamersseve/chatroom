#include "../httplib.h"
#include "../../json/json.h"
#include "../Server.h"
#include "../datamanage.h"
#include "../log.h"
using namespace std;
//����¼
string login_sucess(string name, string pwd ,manager::user& nowuser) {
    auto userptr = manager::FindUser(name);
    if (userptr == nullptr) return "USER_NOT_FOUND";

    //cout << nowuser.getpassword() << " " << pwd << endl;
    manager::user Rawuser = *userptr;

    if (Rawuser.getpassword() == pwd) {
        
        nowuser = *userptr;
        
        return "ACCEPT";
    }
    return "WRONG_PIASSWORD";
}
//�������
bool check_uid_same(string name) {
    if (manager::FindUser(name) == nullptr) return true;
    else return false;
}
//����ͻ��˵�¼����
auto login = [](const httplib::Request& req, httplib::Response& res, const Json::Value& jsonData) {

    res.set_header("Access-Control-Allow-Origin", "*"); // ����������Դ����
    res.set_header("Access-Control-Allow-Methods", "GET, POST, PUT, DELETE"); // ����� HTTP ����
    res.set_header("Access-Control-Allow-Headers", "Content-Type"); // �����ͷ���ֶ�


    std::string username = jsonData["username"].asString();
    std::string password = jsonData["hashedPassword"].asString();
    manager::user nowuser;
    std::string Flagtext = login_sucess(username, password, nowuser);
    //chk(username);
    Logger& logger = Logger::getInstance();


    if (Flagtext == "ACCEPT") {
        res.status = 200;
        res.set_content("Login successful", "text/plain");

        Server& server = Server::getInstance();
        server.setToken(res, to_string(nowuser.getuid()), password);
        //cout << password << endl;
        logger.logInfo("LoginSys", req.remote_addr + " " + username + "-" + password + " �ɹ���¼:");
    }
    else {
        res.status = 401;
        res.set_content(Flagtext, "text/plain");
        logger.logInfo("LoginSys", req.remote_addr + " " + username + "-" + password + " ����ͼ��¼ʱ �ͻ��˴���:" + Flagtext);
    }

};
//����ע������Ĭ��ǰ�˽ӿ����ݸ�ʽ���ԺϷ�
auto Register = [](const httplib::Request& req, httplib::Response& res, const Json::Value& jsonData) {

    res.set_header("Access-Control-Allow-Origin", "*"); // ����������Դ����
    res.set_header("Access-Control-Allow-Methods", "GET, POST, PUT, DELETE"); // ����� HTTP ����
    res.set_header("Access-Control-Allow-Headers", "Content-Type"); // �����ͷ���ֶ�


    std::string username = jsonData["username"].asString();
    std::string password = jsonData["password"].asString();
    manager::user newuser;

    //std::cout << username << " " << password << std::endl;

    Logger& logger = Logger::getInstance();
    if(check_uid_same(username) == true){
        if (!manager::AddUser(username, password, "", manager::Usuallabei)) {
            res.status = 500;//�������޷��������
            res.set_content("unkown error", "text/plain");
            logger.logError("LoginSys", req.remote_addr + " ����ͼ�� " + username + "ע��ʱ����δ֪����");
        }
        else {
            res.set_content("Register successful", "text/plain");
            
            logger.logInfo("LoginSys", req.remote_addr + " " + username + "-" + password + " �ɹ�ע��");
        }
    }
    else {
        res.status = 403;
        res.set_content("samed name", "text/plain");

        logger.logInfo("LoginSys", req.remote_addr + " ����ͼ�� " + username + "ע��ʱ�������û�������");
    }
    
};
//#include "../html/"
void start_loginSystem() {


        Server& server = Server::getInstance(HOST);


        server.serveFile("/login", "html/login.html");
        server.handlePostRequest("/register", Register);
        server.handlePostRequest("/login", login);
        
        //server.start();
 
}
/*
Server& server = Server::getInstance("127.0.0.1");

    server.serveFile("/login", "login.html");

    server.handlePostRequest("/login", login);

    server.handlePostRequest("/register", [](const httplib::Request& req, httplib::Response& res, const Json::Value& jsonData) {
        std::string username = jsonData["username"].asString();
        std::string password = jsonData["password"].asString();

        // ���������Խ���ע�����
        // ���磬���û���Ϣ�洢�����ݿ���
        res.set_content("Registration successful", "text/plain");
        });
    server.start();
*/