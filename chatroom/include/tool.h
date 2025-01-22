#ifndef FILE_H
#define FILE_H

#include <string>
#include <vector>
namespace FILE_ {
    using namespace std;
    class file {
    private:
        vector<string> cache;
        string nowpath;
    public:
        // ��ָ��·���µ��ļ�������һ��д������
        bool appendToLastLine(const string& path, const string& filename, const string& text);

        // ��ָ��·���µ��ļ���ָ����д�����ݣ�Ч�ʽϵͲ�����ʹ�ã�
        bool writeToLine(const string& path, const string& filename, int lineNumber, const string& text);

        //��һ���ļ�ȫ����ȡ������
        int readToCache(const string& path, const string& filename);

        // ��ȡָ��·�����ļ�ָ���е��ַ���
        string readFromLine(const string& path, const string& filename, int lineNumber);

        // ��ָ��·�����½�һ���ļ�������ļ��Ѿ����ڣ��򷵻�false��
        bool createNewFile(const string& path, const string& filename);


        //���ָ��·�����ļ�
        bool ClearFile(const string& path, const string& filename);
    };
    
}
#include <chrono>
#include <sstream>
#include <iomanip>  // ��Ҫ���ͷ�ļ���ʹ�� put_time

namespace time_ {
    std::string getCurrentTime();
}
namespace info {
    
    void printinfo(std::string infotext);//��ӡ��Ϣ
    void printwarning(std::string infotext);//��ӡ����
    void printerror(std::string infotext);//��ӡ����
}
namespace str {
    using namespace std;
    bool safeatoi(const std::string& str, int& result);//��ȫ�ؽ�stirngתΪint

    //string DeleteSpical(string str);//ɾ��string�еĿո��ת���ַ�
}
#include "config.h"

namespace SHA256 {
//#include<openssl/sha.h>
    std::string sha256(const std::string& str);

}

namespace Base64 {
#include <string>

    // Base64 encode function
    std::string base64_encode(const std::string& text);

    // Base64 decode function
    std::string base64_decode(const std::string& code);
}
#include <unordered_set>
namespace Keyword {
    using namespace std;
    string process_string(const string& input);
}

#ifndef WORDCODE_H
#define WORDCODE_H

#include <string>

namespace WordCode {
    // Windows ƽ̨�µı���ת����������
#ifdef _WIN32
    std::string GbkToUtf8(const char* src_str);
    std::string Utf8ToGbk(const char* src_str);
#else
    // �� Windows ƽ̨�µı���ת����������
    int GbkToUtf8(char* str_str, size_t src_len, char* dst_str, size_t dst_len);
    int Utf8ToGbk(char* src_str, size_t src_len, char* dst_str, size_t dst_len);
#endif

}

#endif // WORDCODE_H

#endif // FILE_H
       