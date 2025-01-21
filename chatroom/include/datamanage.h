#pragma once
#ifndef DATAMANAGE_H
#define DATAMANAGE_H

#include "tool.h"
#include "config.h"
#include<map>
namespace manager {
	const string banedName = "����û�";
	const string AluserType = "AllUser";
	const string GMlabei = "GM";//����Ա
	const string Usuallabei = "U";//��ͨ�û�
	const string BanedLabei = "BAN";//����û�
	//���char�Ƿ�ȫ
	bool SafeWord(char word);
	//����û����Ƿ�Ϸ�
	bool CheckUserName(string name);
	//�洢���ݵ��ļ���·��
	const string datafile = "data.txt";
	extern int usernum;

	class user {
		string name, password;
		string cookie, labei;
		int uid;
	public:
		string getname();
		string getcookie();
		string getlabei();
		string getpassword();
		void ban();

		bool setname(string str);

		int getuid();
		bool operator <(user x);
		user(string name_ = "NULL", string password_ = "", string cookie_ = "", string labei_ = "NULL");
		void setuid(int value = -1);
	};
	//�������������û�
	bool AddUser(string name, string psw, string cookie, string labei, int uuid = -1);
	//ͨ��uid�����û�
	user* FindUser(int uid);
	//ͨ��id�����û�
	user* FindUser(string name);
	//�Ƴ��û�
	bool RemoveUser(int uid);
	//��ȡʱ�Ļ�����
	extern config DataFile;
	extern std::vector<item> list;
	//��ӡ�쳣
	void LogError(string path, string filename, int line);
	//��ȡ�����ļ�
	void ReadUserData(string path, string filename);
	//���浱ǰ�û��б�
	void WriteUserData(string path, string filename);
}
#endif