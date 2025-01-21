#include "../include/Server.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include "../json/json.h"
#include "../log.h"
Server& Server::getInstance(const std::string& host, int port) {
    static Server instance(host, port);
    return instance;
}

Server::Server(const std::string& host, int port)
    : server_host(host), server_port(port) {
}

// �ṩ HTML ������Ӧ
void Server::serveHtml(const std::string& endpoint, const std::string& htmlContent) {
    server.Get(endpoint.c_str(), [htmlContent](const httplib::Request&, httplib::Response& res) {
        res.set_content(htmlContent, "text/html");
        });
}

// �ṩ�ļ���Ӧ
void Server::serveFile(const std::string& endpoint, const std::string& filePath) {
    server.Get(endpoint.c_str(), [filePath](const httplib::Request&, httplib::Response& res) {
        std::ifstream file(filePath, std::ios::binary);
        if (file) {
            std::ostringstream content;
            content << file.rdbuf();
            res.set_content(content.str(), detectMimeType(filePath));
        }
        else {
            res.status = 404;
            res.set_content("File not found", "text/plain");
        }
        });
}

// ���� GET ����
void Server::handleRequest(const std::string& endpoint, const std::function<void(const httplib::Request&, httplib::Response&)>& handler) {
    server.Get(endpoint.c_str(), handler);
}

// ����� JSON �� POST ����
void Server::handlePostRequest(const std::string& endpoint, const std::function<void(const httplib::Request&, httplib::Response&, const Json::Value&)>& jsonHandler) {
    server.Post(endpoint.c_str(), [jsonHandler](const httplib::Request& req, httplib::Response& res) {
        Json::Value root;
        Json::Reader reader;

        // ���Խ���������Ϊ JSON
        if (reader.parse(req.body, root)) {
            jsonHandler(req, res, root);  // �������Ч�� JSON������ JSON ������
        }
        else {
            res.status = 400;
            res.set_content("Invalid JSON", "text/plain");
        }
        });
}

// ������ļ��ϴ��� POST ����
void Server::handlePostRequest(const std::string& endpoint, const std::function<void(const httplib::Request&, httplib::Response&)>& fileHandler) {
    server.Post(endpoint.c_str(), [fileHandler](const httplib::Request& req, httplib::Response& res) {
        if (req.has_file("file")) {
            fileHandler(req, res);  // ��������а����ļ��������ļ�������
        }
        else {
            res.status = 400;
            res.set_content("No file uploaded", "text/plain");
        }
        });
}

// ����������
void Server::start() {

    Logger& logger = Logger::getInstance();
    logger.logInfo("Server", "��������" + server_host + "�ϼ������˿�Ϊ" + to_string(server_port));
    //std::cout << "Starting server at " << server_host << ":" << server_port << std::endl;
    
    server.Get("/", [](const httplib::Request&, httplib::Response& res) {
        res.set_redirect(ROOTURL);
        });
    

    server.listen(server_host.c_str(), server_port);
}

// ��ȡ MIME ����
std::string Server::detectMimeType(const std::string& filePath) {
    auto pos = filePath.rfind('.');
    if (pos != std::string::npos) {
        std::string extension = filePath.substr(pos);
        if (extension == ".html") return "text/html";
        if (extension == ".css") return "text/css";
        if (extension == ".js") return "application/javascript";
        if (extension == ".png") return "image/png";
        if (extension == ".jpg" || extension == ".jpeg") return "image/jpeg";
        if (extension == ".gif") return "image/gif";
        if (extension == ".txt") return "text/plain";
    }
    return "application/octet-stream";
}

// ������Ӧ�� Cookie
void Server::setCookie(httplib::Response& res, const std::string& cookieName, const std::string& cookieValue) {
    res.set_header("Set-Cookie", cookieName + "=" + cookieValue);
}

// ���� JSON ������Ϊ��Ӧ
void Server::sendJson(const Json::Value& jsonResponse, httplib::Response& res) {
    res.set_content(jsonResponse.toStyledString(), "application/json");
}

// ���ÿ�������� CORS ͷ��Ϣ
void Server::setCorsHeaders(httplib::Response& res) {
    res.set_header("Access-Control-Allow-Origin", "*");
    res.set_header("Access-Control-Allow-Methods", "GET, POST, OPTIONS");
    res.set_header("Access-Control-Allow-Headers", "Content-Type, X-Custom-Header");
}

void Server::setToken(httplib::Response& res, const std::string& uid, const std::string& clientid) {
    // �� uid �� clientid ����Ϊ���� Cookie
    res.set_header("Set-Cookie", "uid=" + uid + "; Path=/; HttpOnly");
    res.set_header("Set-Cookie", "clientid=" + clientid + "; Path=/; HttpOnly");
}