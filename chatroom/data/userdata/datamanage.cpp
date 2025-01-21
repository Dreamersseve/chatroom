#include "../../include/tool.h"
#include "../../include/config.h"
#include "../../include/datamanage.h"
#include <map>
#include <mutex>

using namespace std;

namespace manager {

    std::mutex mtx;  // ���ڱ����������ݵĻ�����

    //���char�Ƿ�ȫ
    bool SafeWord(char word) {
        if (('0' <= word && word <= '9')
            || ('a' <= word && word <= 'z')
            || ('A' <= word && word <= 'Z')
            || word == '_') return true;

        return false;
    }

    //����û����Ƿ�Ϸ�
    bool CheckUserName(string name) {
        if (name.length() > 50) return false;
        for (int i = 0; i < name.length(); i++) {
            if (!SafeWord(name[i])) return false;
        }
        return true;
    }

    //�洢���ݵ��ļ���·��
    int usernum = 0;

    string user::getname() { return name; }
    string user::getcookie() { return cookie; }
    string user::getlabei() { return labei; }
    string user::getpassword() { return password; }
    void user::ban() {
        name = banedName;
        labei = BanedLabei;
    }

    bool user::setname(string str) {
        if (!CheckUserName(str)) {
            return false;
        }
        name = str;
        return true;
    }

    // �����ⷵ�ص���TM int!!! int!!! int!!!
    int user::getuid() { return uid; }

    bool user::operator <(user x) {
        return uid < x.uid;
    }

    user::user(string name_, string password_, string cookie_, string labei_) {
        name = name_, password = password_, cookie = cookie_, labei = labei_;
        uid = 0;
    }

    void user::setuid(int value) {
        if (value != -1) {
            uid = value;
            usernum = max(value, usernum);
        }
        else uid = ++usernum;
    }

    map<int, user> userList;  //ӳ��uid->�û�
    map<string, int> nameList; //ӳ���û���->uid
    //�������������û�
    bool AddUser(string name, string psw, string cookie, string labei, int uuid) {
        lock_guard<mutex> lock(mtx); // ����������������

        user newuser(name, psw, cookie, labei);
        if (nameList.find(name) != nameList.end()) return false; //����

        newuser.setuid(uuid);
        nameList[newuser.getname()] = newuser.getuid();
        userList[newuser.getuid()] = newuser;
        return true;
    }

    //ͨ��uid�����û�
    user* FindUser(int uid) {
        lock_guard<mutex> lock(mtx); // ����������������

        auto it = userList.find(uid);
        if (it != userList.end()) {
            return &(it->second);
        }
        return nullptr;
    }

    //ͨ��id�����û�
    user* FindUser(string name) {
        //lock_guard<mutex> lock(mtx); // ����������������

        auto it = nameList.find(name);
        //mtx.unlock();
        if (it != nameList.end()) {
            return FindUser(nameList[name]);
        }
        return nullptr;
    }

    //�Ƴ��û�
    bool RemoveUser(int uid) {
        lock_guard<mutex> lock(mtx); // ����������������

        user* duser = FindUser(uid);
        if (duser == nullptr) return false;
        duser->ban();
        return true;
    }

    //��ȡʱ�Ļ�����
    config DataFile;
    std::vector<item> list;

    //��ӡ�쳣
    void LogError(string path, string filename, int line) {
        Logger& logger = Logger::getInstance();
        logger.logError("config", "��ȡ�û���Ϣ" + path + "/" + filename + " ��" + to_string(line) + "ʱ��������");
    }

    //��ȡ�����ļ�
    void ReadUserData(string path, string filename) {
       

        // ��ȡ����
        DataFile.read(path, filename);
        list = DataFile.getlist(); //��ȡ����ȡ���������ļ��б�

        vector<user> users_to_add;  // ������ʱ�洢��Ҫ��ӵ��û���Ϣ

        for (int i = 0; i < list.size(); i++) {
            item now = list[i];
            if (now.gettype() == 2) { // �ҵ���
                string name, uuid, cookie, labei, password;
                auto son = now.getson();
                if (son.size() != 4) {
                    LogError(path, filename, i);
                    continue;
                }

                // ���������ļ�
                if (list[son[0]].gettype() != 1 || list[son[0]].getname() != "name") {
                    LogError(path, filename, i + 1);
                    continue;
                }
                name = list[son[0]].getval();

                if (list[son[1]].gettype() != 1 || list[son[1]].getname() != "password") {
                    LogError(path, filename, i + 2);
                    continue;
                }
                password = list[son[1]].getval();

                if (list[son[2]].gettype() != 1 || list[son[2]].getname() != "cookie") {
                    LogError(path, filename, i + 3);
                    continue;
                }
                cookie = list[son[2]].getval();

                if (list[son[3]].gettype() != 1 || list[son[3]].getname() != "labei") {
                    LogError(path, filename, i + 4);
                    continue;
                }
                labei = list[son[3]].getval();

                uuid = now.getname();
                int uid;
                if (str::safeatoi(uuid, uid) == 0) {
                    LogError(path, filename, i);
                    continue;
                }

                // �����û���Ϣ������ʱ����
                users_to_add.push_back(user(name, password, cookie, labei));
                users_to_add.back().setuid(uid);  // ����UID
            }
        }

        {
            // �����ټ���������û�
            lock_guard<mutex> lock(mtx); // ����������������

            // �������������
            nameList.clear();
            userList.clear();
        }

        
        for (auto& newuser : users_to_add) {
            AddUser(newuser.getname(), newuser.getpassword(), newuser.getcookie(), newuser.getlabei(), newuser.getuid());
        }
    }

    //���浱ǰ�û��б�
    void WriteUserData(string path, string filename) {
        lock_guard<mutex> lock(mtx); // ����������������

        DataFile.init(usernum * 5);
        for (int i = 0; i <= usernum; i++) { //ö��Ψһ��uuid
            if (userList.find(i) == userList.end()) continue; //����ȱʧ
            item now;
            user nowuser = userList[i]; //����Ŀ
            now.cur = (int)DataFile.list.size(), now.type = 2, now.val = "4";
            now.name = to_string(nowuser.getuid());
            DataFile.list.push_back(now);
            item son;

            son.cur = (int)DataFile.list.size(), son.type = 1, son.val = nowuser.getname(), son.name = "name";
            DataFile.list.push_back(son);
            now.addson(DataFile.list.back());
            //����ÿ������

            son.cur = (int)DataFile.list.size(), son.type = 1, son.val = nowuser.getpassword(), son.name = "password";
            DataFile.list.push_back(son);
            now.addson(DataFile.list.back());

            son.cur = (int)DataFile.list.size(), son.type = 1, son.val = nowuser.getcookie(), son.name = "cookie";
            DataFile.list.push_back(son);
            now.addson(DataFile.list.back());

            son.cur = (int)DataFile.list.size(), son.type = 1, son.val = nowuser.getlabei(), son.name = "labei";
            DataFile.list.push_back(son);
            now.addson(DataFile.list.back());

            DataFile.list[now.cur] = now; //���¸���
        }
        DataFile.write(path, filename); //����
    }
}
