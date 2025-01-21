#include "../Server.h"
#include <vector>
#include <string>
#include <deque>
#include <../../json/json.h>
#include "../datamanage.h"
#include "../tool.h"
using namespace std;
#include <filesystem>
namespace chatroom {
    using namespace std;
    const int MAXSIZE = 1000;
#include <filesystem>
    deque<Json::Value> chatMessages;
    Server& server = server.getInstance(HOST);

    void initializeChatRoom() {
        Json::Value initialMessage;
        initialMessage["user"] = "system";
        initialMessage["labei"] = "GM";
        initialMessage["message"] = Base64::base64_encode("wellcome!");
        chatMessages.push_back(initialMessage);
    }

    string transJsonMessage(Json::Value m) {
        string message = m["message"].asString();
        return "[" + m["user"].asString() + "][" + Base64::base64_decode(message) + "]";
    }

    void systemMessage(string message) {
        Json::Value initialMessage;
        initialMessage["user"] = "system";
        initialMessage["labei"] = "GM";
        initialMessage["message"] = Base64::base64_encode(message);


        chatMessages.push_back(initialMessage);


        Logger& logger = logger.getInstance();
        logger.logInfo("chatroom::message", transJsonMessage(initialMessage));
    }

    void getChatMessages(const httplib::Request& req, httplib::Response& res) {
        Json::Value response;
        for (const auto& msg : chatMessages) {
            response.append(msg);
        }

        // 设置响应头，支持跨域
        res.set_header("Access-Control-Allow-Origin", "*");
        res.set_header("Content-Type", "application/json");

        // 将聊天记录转为 JSON 并返回给前端
        res.set_content(response.toStyledString(), "application/json");
    }

    // 解析 Cookie
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

    std::mutex mtx;
    
    //路由发送消息请求
    void postChatMessage(const httplib::Request& req, httplib::Response& res, const Json::Value& root) {

        res.set_header("Access-Control-Allow-Origin", "*"); // 允许所有来源访问
        res.set_header("Access-Control-Allow-Methods", "GET, POST, PUT, DELETE"); // 允许的 HTTP 方法
        res.set_header("Access-Control-Allow-Headers", "Content-Type"); // 允许的头部字段


        Logger& logger = Logger::getInstance();
        std::string cookies = req.get_header_value("Cookie");
        std::string password, uid;
        transCookie(password, uid, cookies);

        if (!root.isMember("message") || !root.isMember("uid")) {
            res.status = 400;
            res.set_content("Invalid data format", "text/plain");
            logger.logInfo("ChatSys", req.remote_addr + "发送了一条不合法请求 ");
            return;
        }

        // 验证 uid 和密码
        int uid_;
        str::safeatoi(uid, uid_);
        if (manager::FindUser(uid_) == nullptr) {
            res.status = 400;
            res.set_content("Invalid Cookie", "text/plain");
            logger.logInfo("ChatSys", req.remote_addr + "的cookie无效 ");
            return;
        }
        manager::user nowuser = *manager::FindUser(uid_);
        if (nowuser.getpassword() != password) {
            res.status = 400;
            res.set_content("Invalid Cookie", "text/plain");
            logger.logInfo("ChatSys", req.remote_addr + "的cookie无效 ");
            return;
        }

        // 保存聊天消息
        Json::Value newMessage;
        newMessage["user"] = nowuser.getname();
        newMessage["labei"] = nowuser.getlabei();
        string msgSafe = Base64::base64_decode(root["message"].asString());
        string codedmsg = Base64::base64_encode(Keyword::process_string(msgSafe));

        newMessage["message"] = codedmsg;
        newMessage["imageUrl"] = root["imageUrl"];


        lock_guard<mutex> lock(mtx);
        //if (chatMessages.back() != newMessage) 
        chatMessages.push_back(newMessage);
        if (chatMessages.size() >= MAXSIZE) chatMessages.pop_front();

        //Logger& logger = logger.getInstance();
        logger.logInfo("chatroom::message", transJsonMessage(newMessage));


        // 响应 OK
        res.status = 200;
        res.set_content("Message received", "text/plain");
    }

    // 获取用户名接口
    void getUsername(const httplib::Request& req, httplib::Response& res) {

        res.set_header("Access-Control-Allow-Origin", "*"); // 允许所有来源访问
        res.set_header("Access-Control-Allow-Methods", "GET, POST, PUT, DELETE"); // 允许的 HTTP 方法
        res.set_header("Access-Control-Allow-Headers", "Content-Type"); // 允许的头部字段


        std::string cookies = req.get_header_value("Cookie");
        std::string uid;
        transCookie(uid, uid, cookies);

        // 获取用户名
        int uid_;
        str::safeatoi(uid, uid_);
        if (manager::FindUser(uid_) == nullptr) {
            res.status = 400;
            res.set_content("Invalid uid or user not found", "text/plain");
            return;
        }
        manager::user goaluser = *manager::FindUser(uid_);
        
        std::string username = goaluser.getname();

        if (username.empty()) {
            res.status = 400;
            res.set_content("Invalid uid or user not found", "text/plain");
        }
        else {
            Json::Value response;
            response["username"] = username;
            res.set_header("Access-Control-Allow-Origin", "*");
            res.set_header("Content-Type", "application/json");
            res.set_content(response.toStyledString(), "application/json");
        }
    }

    void setupStaticRoutes() {
        // 提供图片文件 /logo.png
        server.getInstance().handleRequest("/logo.png", [](const httplib::Request& req, httplib::Response& res) {
            std::ifstream logoFile("html/logo.png", std::ios::binary);
            if (logoFile) {
                std::stringstream buffer;
                buffer << logoFile.rdbuf();
                res.set_content(buffer.str(), "image/png");
            }
            else {
                res.status = 404;
                res.set_content("Logo not found", "text/plain");
            }
            });

        // 提供 JS 文件 /chat/js
        server.getInstance().handleRequest("/chat/js", [](const httplib::Request& req, httplib::Response& res) {
            std::ifstream jsFile("html/chatroom.js", std::ios::binary);
            if (jsFile) {
                std::stringstream buffer;
                buffer << jsFile.rdbuf();
                res.set_content(buffer.str(), "application/javascript");
            }
            else {
                res.status = 404;
                res.set_content("chatroom.js not found", "text/plain");
            }
            });
        
        // 提供图片文件 /images/*，动态路由
        server.getInstance().handleRequest(R"(/images/([^/]+))", [](const httplib::Request& req, httplib::Response& res) {
            std::string imagePath = "html/images/" + req.matches[1].str();  // 获取图片文件名
            std::ifstream imageFile(imagePath, std::ios::binary);

            if (imageFile) {
                std::stringstream buffer;
                buffer << imageFile.rdbuf();

                // 自动推测图片的 MIME 类型
                std::string extension = imagePath.substr(imagePath.find_last_of('.') + 1);
                std::string mimeType = "images/" + extension;

                res.set_content(buffer.str(), mimeType);
            }
            else {
                res.status = 404;
                res.set_content("Image not found", "text/plain");
            }
            });
    }

    // 允许的图片类型
    const std::vector<std::string> allowedImageTypes = { ".jpg", ".jpeg", ".png", ".gif", ".bmp" };

    bool isValidImage(const std::string& filename) {
        // 获取文件扩展名
        std::string ext = filename.substr(filename.find_last_of("."));
        std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);

        // 检查文件扩展名是否在白名单中
        return std::find(allowedImageTypes.begin(), allowedImageTypes.end(), ext) != allowedImageTypes.end();
    }


    void uploadImage(const httplib::Request& req, httplib::Response& res) {
        res.set_header("Access-Control-Allow-Origin", "*");
        res.set_header("Access-Control-Allow-Methods", "GET, POST, PUT, DELETE");
        res.set_header("Access-Control-Allow-Headers", "Content-Type");

        if (req.has_file("file")) {
            auto file = req.get_file_value("file");

            // 检查文件类型是否合法
            if (!isValidImage(file.filename)) {
                res.status = 400;
                res.set_content("{\"error\": \"Invalid file type\"}", "application/json");
                return;
            }

            std::string uploadDir = "html/images/";
            filesystem::create_directories(uploadDir);

            std::string filePath = uploadDir + file.filename;

            std::ofstream outFile(filePath, std::ios::binary);
            if (outFile) {
                outFile.write(file.content.c_str(), file.content.size());
                outFile.close();

                std::string fileUrl = "/images/" + file.filename;

                Json::Value jsonResponse;
                jsonResponse["imageUrl"] = fileUrl;

                res.set_header("Content-Type", "application/json");
                res.status = 200;
                res.set_content(jsonResponse.toStyledString(), "application/json");
            }
            else {
                res.status = 500;
                res.set_content("{\"error\": \"Failed to save the file\"}", "application/json");
            }
        }
        else {
            res.status = 400;
            res.set_content("{\"error\": \"No file uploaded\"}", "application/json");
        }
    }


    void setupChatRoutes() {
        // 提供聊天记录的 GET 请求
        server.getInstance().handleRequest("/chat/messages", getChatMessages);

        // 处理 POST 请求，接收并保存新的聊天消息
        server.getInstance().handlePostRequest("/chat/messages", postChatMessage);

        // 获取用户名的 GET 请求
        server.getInstance().handleRequest("/user/username", getUsername);

        // 图片上传路由
        server.getInstance().handlePostRequest("/chat/upload", uploadImage);
        

        // 设置静态文件路由
        setupStaticRoutes();
    
        
    }

    
}

int start_chatroom() {
    chatroom::initializeChatRoom();
    chatroom::setupChatRoutes();

    Server& server = server.getInstance(HOST);
    server.serveFile("/chat", "html/index.html");

    return 0;
}
