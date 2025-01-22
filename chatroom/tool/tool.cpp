#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <filesystem>
#include "../include/tool.h"
namespace FILE_ {
    using namespace std;
    namespace fs = std::filesystem;

        // ��ָ��·���µ��ļ�������һ��д������
        bool file::appendToLastLine(const string& path, const string& filename, const string& text) {
            // ƴ��·��
            string fullPath = path + "/" + filename;

            // ��׷��ģʽ���ļ����������׷�ӵ��ļ�ĩβ��
            ofstream outfile(fullPath, ios::app);
            if (!outfile.is_open()) {
                return false;
            }

            // д�������ݵ��ļ�ĩβ
            outfile << text << endl;
            outfile.close();

            return true;
        }

        // ��ָ��·���µ��ļ���ָ����д�����ݣ�Ч�ʽϵͲ�����ʹ�ã�
        bool file::writeToLine(const string& path, const string& filename, int lineNumber, const string& text) {
            // ƴ��·��
            string fullPath = path + "/" + filename;

            ifstream infile(fullPath);
            if (!infile.is_open()) {
                return false;
            }

            vector<string> lines;
            string line;
            // ��ȡ�ļ���ÿһ��
            while (getline(infile, line)) {
                lines.push_back(line);
            }
            infile.close();

            // ���ָ�����кŴ����ļ���������������false
            if (lineNumber < 1 || lineNumber > lines.size()) {
                return false;
            }

            // ����ָ������
            lines[lineNumber - 1] = text;

            // ����д���ļ�
            ofstream outfile(fullPath, ofstream::trunc);
            if (!outfile.is_open()) {
                return false;
            }

            // ��������д���ļ�
            for (const auto& l : lines) {
                outfile << l << endl;
            }
            outfile.close();

            return true;
        }


        vector<string> cache;
        string nowpath = "@";
        //��һ���ļ�ȫ����ȡ������
        int file::readToCache(const string& path, const string& filename) {
            bool flag = false;
            // ƴ��·��
            string fullPath = path + "/" + filename;
            nowpath = fullPath;
            ifstream infile(fullPath);
            if (!infile.is_open()) {
                return 0;
            }
            
            string line;
            int linenum = 0;
            // ���ж�ȡ��ֱ���ҵ�ָ������
            while (getline(infile, line)) {
                cache.push_back(line);
                linenum++;
            }

            infile.close();
            return linenum;
        }

        // ��ȡָ��·�����ļ�ָ���е��ַ���
        string file::readFromLine(const string& path, const string& filename, int lineNumber) {
            bool flag = false;
            // ƴ��·��
            string fullPath = path + "/" + filename;

            if (fullPath == nowpath) flag = true;//����ڻ��棬ֱ���û���
            else cache.clear();
            nowpath = fullPath;

            if (flag) {
                if (lineNumber > (int)cache.size()) return "";
                return cache[lineNumber - 1];
            }

            ifstream infile(fullPath);
            if (!infile.is_open()) {
                return "";
            }

            string line;
            int currentLine = 1;

            // ���ж�ȡ��ֱ���ҵ�ָ������
            while (getline(infile, line)) {
                if (currentLine == lineNumber) {
                    infile.close();
                    return line;
                }
                currentLine++;
            }

            infile.close();
            return "";
        }

        // ��ָ��·�����½�һ���ļ�������ļ��Ѿ����ڣ��򷵻�false��
        bool file::createNewFile(const string& path, const string& filename) {
            // ƴ��·��
            string fullPath = path + "/" + filename;

            // ����ļ��Ƿ��Ѿ�����
            try {
                if (fs::exists(fullPath)) {
                    return true; // �ļ��Ѵ���
                }
            }
            catch (const fs::filesystem_error e_) {
                info::printerror("�ļ�����ʧ��:�Ҳ���ָ����·��"+fullPath);
                return false;
            }


            // �������ļ�
            ofstream outfile(fullPath);
            if (!outfile.is_open()) {
                return false;
            }

            // ���������ļ���д���ʼ���ݣ����߽��������ļ�
            outfile.close();

            return true;
        }
        //���ָ��·�����ļ�
        bool file::ClearFile(const string& path, const string& filename) {
            // ƴ��·��
            string fullPath = path + "/" + filename;

            // ���ļ�����дģʽ�򿪻�����ļ����ݣ�
            ofstream outfile(fullPath, ios::out | ios::trunc);
            if (!outfile.is_open()) {
                return false;
            }

            // �ļ��ѱ���գ��ر��ļ�
            outfile.close();

            return true;
        }
    
}

namespace time_ {
    using namespace std;
    //��ȡ��ǰ�ĸ�ʽ��ʱ��
    string getCurrentTime() {
        auto now = chrono::system_clock::now();
        auto in_time_t = chrono::system_clock::to_time_t(now);
        struct tm timeinfo;
        localtime_s(&timeinfo, &in_time_t);

        stringstream ss;
        ss << put_time(&timeinfo, "%Y-%m-%d %X");  // ��ʽ��ʱ��
        return ss.str();
    }
}
namespace info {
    using namespace std;
    void printinfo(string infotext) {
        cout << "[" + time_::getCurrentTime() + "]" << "[info][" << infotext << "]" << endl;
    }
    void printwarning(string infotext) {
        cout << "[" + time_::getCurrentTime() + "]" << "[warning][" << infotext << "]" << endl;
    }
    void printerror(string infotext) {
        cout << "[" + time_::getCurrentTime() + "]" << "[ERROR][" << infotext << "]" << endl;
    }
}

namespace str {
    using namespace std;
    //safeatoi
    bool safeatoi(const std::string& str, int& result) {
        std::stringstream ss(str);
        ss >> std::noskipws;
        int temp;

        // ���Դ��ַ������ж�ȡ����
        if (ss >> temp) {
            char c;
            if (!(ss >> c && ss.fail() && !ss.eof())) {
                result = temp;
                return true;
            }
        }
        result = 0; 
        return false;
    }

    

}

namespace SHA256 {
#include<openssl/sha.h>
#include <openssl/evp.h>
#include <openssl/err.h>
    std::string sha256(const std::string& str) {
        // ����һ�� EVP_MD_CTX ������
        EVP_MD_CTX* mdctx = EVP_MD_CTX_new();
        if (mdctx == nullptr) {
            std::cerr << "EVP_MD_CTX_new failed" << std::endl;
            ERR_print_errors_fp(stderr);
            exit(EXIT_FAILURE);
        }

        // ��ʼ�� SHA-256 ժҪ
        if (EVP_DigestInit_ex(mdctx, EVP_sha256(), nullptr) != 1) {
            std::cerr << "EVP_DigestInit_ex failed" << std::endl;
            ERR_print_errors_fp(stderr);
            EVP_MD_CTX_free(mdctx);
            exit(EXIT_FAILURE);
        }

        // ����ժҪ������������
        if (EVP_DigestUpdate(mdctx, str.c_str(), str.size()) != 1) {
            std::cerr << "EVP_DigestUpdate failed" << std::endl;
            ERR_print_errors_fp(stderr);
            EVP_MD_CTX_free(mdctx);
            exit(EXIT_FAILURE);
        }

        // ��ȡժҪ���
        unsigned char hash[EVP_MAX_MD_SIZE];
        unsigned int hash_len;
        if (EVP_DigestFinal_ex(mdctx, hash, &hash_len) != 1) {
            std::cerr << "EVP_DigestFinal_ex failed" << std::endl;
            ERR_print_errors_fp(stderr);
            EVP_MD_CTX_free(mdctx);
            exit(EXIT_FAILURE);
        }

        EVP_MD_CTX_free(mdctx); // �ͷ�������

        // ����ϣֵת��Ϊʮ�������ַ���
        std::string hex_string;
        hex_string.reserve(hash_len * 2); // Ԥ���㹻�Ŀռ�
        for (unsigned int i = 0; i < hash_len; ++i) {
            char buf[3]; // ÿ���ֽ���Ҫ����ʮ�������ַ���һ������ֹ�������������ǲ��ÿ���ֹ����
            snprintf(buf, sizeof(buf), "%02x", hash[i]);
            hex_string.append(buf, 2); // ֱ��׷�������ַ����ַ�����
        }

        return hex_string;
    }
}
namespace Base64 {
#include<algorithm>

#include <cassert>
    static const std::string alphabet_map = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    static const uint8_t reverse_map[] = {
        255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
        255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
        255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 62, 255, 255, 255, 63,
        52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 255, 255, 255, 255, 255, 255,
        255, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14,
        15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 255, 255, 255, 255, 255,
        255, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
        41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 255, 255, 255, 255, 255
    };

    std::string base64_encode(const std::string& text) {
        std::string encode;
        uint32_t i, j = 0;

        // ���ֽڱ����ַ���
        for (i = 0; i + 3 <= text.length(); i += 3) {
            encode.push_back(alphabet_map[(unsigned char)text[i] >> 2]);
            encode.push_back(alphabet_map[((unsigned char)text[i] << 4) & 0x30 | ((unsigned char)text[i + 1] >> 4)]);
            encode.push_back(alphabet_map[((unsigned char)text[i + 1] << 2) & 0x3C | ((unsigned char)text[i + 2] >> 6)]);
            encode.push_back(alphabet_map[(unsigned char)text[i + 2] & 0x3F]);
        }

        // ����ʣ����ֽڣ�����3���ֽڣ�
        if (i < text.length()) {
            uint32_t tail = (int)text.length() - i;
            if (tail == 1) {
                encode.push_back(alphabet_map[(unsigned char)text[i] >> 2]);
                encode.push_back(alphabet_map[((unsigned char)text[i] << 4) & 0x30]);
                encode.append("==");
            }
            else if (tail == 2) {
                encode.push_back(alphabet_map[(unsigned char)text[i] >> 2]);
                encode.push_back(alphabet_map[((unsigned char)text[i] << 4) & 0x30 | ((unsigned char)text[i + 1] >> 4)]);
                encode.push_back(alphabet_map[((unsigned char)text[i + 1] << 2) & 0x3C]);
                encode.push_back('=');
            }
        }

        return encode;
    }

    std::string base64_decode(const std::string& code) {
        assert(code.length() % 4 == 0);

        std::string plain;
        uint32_t i, j = 0;
        uint8_t quad[4];

        for (i = 0; i < code.length(); i += 4) {
            for (uint32_t k = 0; k < 4; k++) {
                quad[k] = reverse_map[code[i + k]];
            }

            assert(quad[0] < 64 && quad[1] < 64);

            plain.push_back((quad[0] << 2) | (quad[1] >> 4));

            if (quad[2] >= 64) break;
            else if (quad[3] >= 64) {
                plain.push_back((quad[1] << 4) | (quad[2] >> 2));
                break;
            }
            else {
                plain.push_back((quad[1] << 4) | (quad[2] >> 2));
                plain.push_back((quad[2] << 6) | quad[3]);
            }
        }

        return plain;
    }
}
#include <unordered_set>
#include <fstream>
#include <string>


namespace Keyword {
#include <unordered_set>

    // ���ڶ�ȡ�ļ������е����д�
    unordered_set<string> load_keywords(const string& filename) {
        unordered_set<string> keywords;
        ifstream file(filename);
        string line;
        while (getline(file, line)) {
            if (!line.empty()) {
                keywords.insert(line);
            }
        }
        return keywords;
    }

    // �滻�ַ����е����д�
    void replace_sensitive_words(string& input, const unordered_set<string>& keywords) {
        for (const auto& keyword : keywords) {
            size_t pos = 0;
            while ((pos = input.find(keyword, pos)) != string::npos) {
                input.replace(pos, keyword.length(), string(keyword.length(), '*'));
                pos += keyword.length();
            }
        }
    }

    // ת���ַ����е�XMLע��ؼ���
    void escape_xml(std::string& input) {
        std::stringstream escaped;  // ʹ���ַ��������ۻ���������ַ�
        for (char c : input) {
            switch (c) {
            case '<':
                escaped << "&lt;";
                break;
            case '>':
                escaped << "&gt;";
                break;
            case '&':
                escaped << "&amp;";
                break;
            case '\'':
                escaped << "&apos;";
                break;
            case '\"':
                escaped << "&quot;";
                break;
            default:
                escaped << c;  // �����ַ����ֲ���
            }
        }
        input = escaped.str();  // �������Ľ������ԭ�ַ���
    }

    std::unordered_set<std::string> keywords = load_keywords("keyword.txt");
    // �����ַ������滻���дʲ�ת��XML�ؼ���
    string process_string(const string& input) {

        string result = input;

        // �滻���д�
        replace_sensitive_words(result, keywords);

        // ת��XMLע��ؼ���
        escape_xml(result);

        return result;
    }
}
namespace WordCode {
#include <iostream>
#include <string>
#include <string.h>
#include <stdlib.h>

    using namespace std;

#ifdef _WIN32
#include <windows.h>

    string GbkToUtf8(const char* src_str)
    {
        int len = MultiByteToWideChar(CP_ACP, 0, src_str, -1, NULL, 0);
        wchar_t* wstr = new wchar_t[len + 1];
        memset(wstr, 0, len + 1);
        MultiByteToWideChar(CP_ACP, 0, src_str, -1, wstr, len);
        len = WideCharToMultiByte(CP_UTF8, 0, wstr, -1, NULL, 0, NULL, NULL);
        char* str = new char[len + 1];
        memset(str, 0, len + 1);
        WideCharToMultiByte(CP_UTF8, 0, wstr, -1, str, len, NULL, NULL);
        string strTemp = str;
        if (wstr) delete[] wstr;
        if (str) delete[] str;
        return strTemp;
    }

    string Utf8ToGbk(const char* src_str)
    {
        int len = MultiByteToWideChar(CP_UTF8, 0, src_str, -1, NULL, 0);
        wchar_t* wszGBK = new wchar_t[len + 1];
        memset(wszGBK, 0, len * 2 + 2);
        MultiByteToWideChar(CP_UTF8, 0, src_str, -1, wszGBK, len);
        len = WideCharToMultiByte(CP_ACP, 0, wszGBK, -1, NULL, 0, NULL, NULL);
        char* szGBK = new char[len + 1];
        memset(szGBK, 0, len + 1);
        WideCharToMultiByte(CP_ACP, 0, wszGBK, -1, szGBK, len, NULL, NULL);
        string strTemp(szGBK);
        if (wszGBK) delete[] wszGBK;
        if (szGBK) delete[] szGBK;
        return strTemp;
    }
#else
#include <iconv.h>

    int GbkToUtf8(char* str_str, size_t src_len, char* dst_str, size_t dst_len)
    {
        iconv_t cd;
        char** pin = &str_str;
        char** pout = &dst_str;

        cd = iconv_open("utf8", "gbk");
        if (cd == 0)
            return -1;
        memset(dst_str, 0, dst_len);
        if (iconv(cd, pin, &src_len, pout, &dst_len) == -1)
            return -1;
        iconv_close(cd);
        *pout = '\0';

        return 0;
    }

    int Utf8ToGbk(char* src_str, size_t src_len, char* dst_str, size_t dst_len)
    {
        iconv_t cd;
        char** pin = &src_str;
        char** pout = &dst_str;

        cd = iconv_open("gbk", "utf8");
        if (cd == 0)
            return -1;
        memset(dst_str, 0, dst_len);
        if (iconv(cd, pin, &src_len, pout, &dst_len) == -1)
            return -1;
        iconv_close(cd);
        *pout = '\0';

        return 0;
    }


#endif
/*
    ��������������������������������

        ��Ȩ����������Ϊ����ԭ�����£���ѭ CC 4.0 BY - SA ��ȨЭ�飬ת���븽��ԭ�ĳ������Ӻͱ�������

        ԭ�����ӣ�https ://blog.csdn.net/u012234115/article/details/83186386
*/
}