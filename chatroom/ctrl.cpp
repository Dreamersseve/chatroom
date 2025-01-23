#include "../Server.h"
#include "../log.h"
#include "../datamanage.h"
#include <string>
#include <corecrt_wstring.h>
#include <locale>
#include <codecvt>
//using namespace std;

std::string convertToUTF8(const std::string& input) {
	return input;
}

void command_runner(string command) {
	command += "                                 ";

	Logger& logger = logger.getInstance();
	Server& server = server.getInstance(HOST);
	if (command.substr(0,13) == "userdata save") {
		manager::WriteUserData("./", manager::datafile);
		logger.logInfo("Control", "�����ѱ���" + command);
	}
	else if (command.substr(0,13) == "userdata load") {
		try {
			manager::ReadUserData("./", manager::datafile);
			logger.logInfo("Control", "�����Ѽ���" + command);
		}
		catch (const std::exception& e) {
			logger.logFatal("ALL PROGRAM", e.what());
		}
		
	}
	else if (command.substr(0, 5) == "start") {
		try {
			if (command.substr(7, 6) == "server") {
				//  start_chatroom();
				//  start_loginSystem();
				server.start();
				logger.logInfo("Control", "�������ѿ���" + command);
			}
			else if (command.substr(7, 5) == "login") {
				//  start_chatroom();
				start_loginSystem();

			}
			else if (command.substr(7, 4) == "room") {
				start_chatroom();
			}
			else {
				start_chatroom();
				start_loginSystem();
				manager::ReadUserData("./", manager::datafile);
				logger.logInfo("Control", "�����Ѽ���" + command);
				server.start();
				logger.logInfo("Control", "�������ѿ���" + command);
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
		chatroom::systemMessage(convertToUTF8(GBKmsg));
		logger.logInfo("Control", "��Ϣ�ѷ���" + command);

	}
	else if (command.substr(0, 5) == "clear") {
		chatroom::chatMessages.clear();
		logger.logInfo("Control", "��Ϣ�б������" + command);
	}
	else {
		logger.logError("Control","���Ϸ���ָ�� " + command);
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
	std::thread maint(command);
	maint.join();
	Logger& logger = logger.getInstance();
	logger.logInfo("Control", "�����߳��ѽ���");
	return;
}
