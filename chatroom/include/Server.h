#ifndef SIMPLE_SERVER_H
#define SIMPLE_SERVER_H
//�������byte��ͻ�����һ��include˳��
#include "../include/httplib.h"
#include <string>
#include <functional>
#include "../json/json.h"  // ���� json ��
const std::string HOST = "0.0.0.0";
const int PORT = 80;
const std::string ROOTURL = "/chat";
class Server {
public:
    // ��ȡ Server ʵ����ʹ�õ���ģʽ
    static Server& getInstance(const std::string& host = HOST, int port = PORT);

    // Ϊָ���� endpoint �ṩ HTML ������Ӧ
    void serveHtml(const std::string& endpoint, const std::string& htmlContent);

    // Ϊָ���� endpoint �ṩ�ļ���Ӧ
    void serveFile(const std::string& endpoint, const std::string& filePath);

    // ���ô��� GET ����ķ���
    void handleRequest(const std::string& endpoint, const std::function<void(const httplib::Request&, httplib::Response&)>& handler);

    // ���ô��� POST ����ķ���
    void handlePostRequest(const std::string& endpoint, const std::function<void(const httplib::Request&, httplib::Response&, const Json::Value&)>& handler);
    void handlePostRequest(const std::string& endpoint, const std::function<void(const httplib::Request&, httplib::Response&)>& fileHandler);

    // ����������
    void start();

    // ������Ӧ Cookie
    void setCookie(httplib::Response& res, const std::string& cookieName, const std::string& cookieValue);

    // ���� JSON ������Ϊ��Ӧ
    void sendJson(const Json::Value& jsonResponse, httplib::Response& res);

    // ��ȡ�����ÿ�������Ĺ���
    void setCorsHeaders(httplib::Response& res);

    //�����û���¼ƾ��
    void setToken(httplib::Response& res, const std::string& uid, const std::string& clientid);

private:
    Server(const std::string& host, int port);
    ~Server() = default;

    Server(const Server&) = delete;
    Server& operator=(const Server&) = delete;

    std::string server_host;
    int server_port;
    httplib::Server server;

    // �����ļ���׺�Զ��Ʋ� MIME ����
    static std::string detectMimeType(const std::string& filePath);
};

#endif
