#include "../Server.h"
#include "../log.h"
#include "../datamanage.h"
#include <string>
#include <corecrt_wstring.h>
#include <locale>
#include <codecvt>
#include "server/chatroom.h" 
#include "server/roommanager.h"
//using namespace std;
std::string convertToUTF8(const std::string& input) {
	return input;
}
void start_loginSystem();
void start();



void command_runner(string command,int roomid) {
	command += "                                 ";

	Logger& logger = logger.getInstance();
	Server& server = server.getInstance(HOST);
	if (command.substr(0,13) == "userdata save") {
		manager::WriteUserData("./", manager::datafile);
		logger.logInfo("Control", "数据已保存" + command);
	}
	else if (command.substr(0,13) == "userdata load") {
		try {
			manager::ReadUserData("./", manager::datafile);
			logger.logInfo("Control", "数据已加载" + command);
		}
		catch (const std::exception& e) {
			logger.logFatal("ALL PROGRAM", e.what());
		}
		
	}
	else if (command.substr(0, 5) == "start") {

		try {
			{
				start();
				testroom.start();
				start_loginSystem();
				manager::ReadUserData("./", manager::datafile);
				logger.logInfo("Control", "数据已加载" + command);
				server.start();
				logger.logInfo("Control", "服务器已开启" + command);
			}
		}
		catch (const std::exception& e) {
			logger.logFatal("ALL PROGRAM", e.what());
		}
	}
	else if (command.substr(0,4) == "stop") {

		manager::WriteUserData("./", manager::datafile);
		exit(0);
	}
	else if (command.substr(0, 3) == "say") {
		string message;
		command.erase(0, 4);
		message = command;
		for (size_t i = message.size() - 1; message[i] == ' '; i--) message.pop_back();
		string UTF8msg = message;
		string GBKmsg = WordCode::Utf8ToGbk(UTF8msg.c_str());
		testroom.systemMessage(convertToUTF8(GBKmsg));
		logger.logInfo("Control", "消息已发送" + command);

	}
	else if (command.substr(0, 5) == "clear") {
		testroom.clearMessage();
		logger.logInfo("Control", "消息列表已清空" + command);
	}
	else {
		logger.logError("Control","不合法的指令 " + command);
	}
	return;
}
void command() {
	while (1) {
		string cmd;
		getline(cin,cmd);
		//cmd += '|';
		std::thread cmd_tread(command_runner, cmd);
		cmd_tread.detach();
	}
}
#include <thread>
void run() {

	testroom.allowID.push_back(0);
	testroom.allowID.push_back(1);

	std::thread maint(command);
	maint.join();
	Logger& logger = logger.getInstance();
	logger.logInfo("Control", "控制线程已结束");
	return;
}
