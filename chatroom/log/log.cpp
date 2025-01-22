#include "../include/log.h"
#include <iostream>
#include <chrono>
#include <ctime>
#include <thread>
#include <mutex>
#include <queue>
#include <condition_variable>
#include "../include/tool.h" 

using namespace std;

// ��̬��Ա�����Ķ���
Logger* Logger::instance = nullptr;

// ����־����ת��Ϊ�ַ���
string Logger::logLevelToString(LogLevel level) {
    switch (level) {
    case LogLevel::INFO: return "INFO";
    case LogLevel::WARNING: return "WARNING";
    case LogLevel::ERROR_: return "ERROR_";
    case LogLevel::FATAL: return "FATAL";
    default: return "UNKNOWN";
    }
}

// ������־��Ϣ�ַ���
string Logger::createLogMessage(LogLevel level, const string& origin, const string& message) {
    return "[" + time_::getCurrentTime() + "] [" + logLevelToString(level) + "] [" + origin + "] " + message;
}

// ������־д�룬ȷ�����̰߳�ȫ
void Logger::writeLogToFile(const string& logMessage) {
    lock_guard<mutex> lock(mtx);  // �Զ���������֤�̰߳�ȫ

    // ����ļ������ڣ��ȴ����ļ�
    logfile.createNewFile(logPath, logFilename);

    // д����־���ļ�
    logfile.appendToLastLine(logPath, logFilename, logMessage);
    cout << logMessage << endl;
}

// ��־�̺߳���
void Logger::logThreadFunction() {
    while (true) {
        unique_lock<mutex> lock(mtx);
        cv.wait(lock, [this]() { return !logQueue.empty() || stopLogging; });

        // ���ֹͣ��־Ϊ���Ҷ���Ϊ�գ��˳��߳�
        if (stopLogging && logQueue.empty()) {
            break;
        }

        // ��ȡ�����е�һ����־��Ϣ
        string logMessage = logQueue.front();
        logQueue.pop();
        logCount--;  // ����һ����־�����ٴ����������
        lock.unlock();  // �������������̼߳�������

        // д���ļ�
        writeLogToFile(logMessage);
    }
}

// ˽�й��캯����ȷ���ⲿ����ֱ�Ӵ��� Logger ʵ��
Logger::Logger(const string& path, const string& filename) : logPath(path), logFilename(filename) {
    // ȷ���ļ�����
    logfile.createNewFile(logPath, logFilename);
}

// ��ȡȫ��Ψһ�� Logger ʵ��
Logger& Logger::getInstance() {
    if (instance == nullptr) {
        // Ĭ�ϵ�·�����ļ�������
        instance = new Logger(LOGPATH, LOGNAME);
        instance->start();  // ������־�߳�
    }
    return *instance;
}

// ������־�߳�
void Logger::start() {
    logThread = thread(&Logger::logThreadFunction, this);  // ������־�߳�
}

// ֹͣ��־�̣߳��ȴ�������־������Ϻ��˳�
void Logger::stop() {
    {
        lock_guard<mutex> lock(mtx);
        stopLogging = true;
    }
    cv.notify_all();  // ֪ͨ��־�߳��˳�

    // �ȴ�������־�������
    while (logCount > 0) {
        this_thread::sleep_for(chrono::milliseconds(10));  // ���߳���ʱ�䴦����־
    }

    if (logThread.joinable()) {
        logThread.join();  // �ȴ���־�߳̽���
    }
}

// �����µ���־·��
void Logger::setLogPath(const string& newPath) {
    lock_guard<mutex> lock(mtx);  // ȷ���̰߳�ȫ
    logPath = newPath;
    // ����ļ������ڣ�����һ���µ��ļ�
    logfile.createNewFile(logPath, logFilename);
}

// �����µ���־�ļ���
void Logger::setLogFilename(const string& newFilename) {
    lock_guard<mutex> lock(mtx);  // ȷ���̰߳�ȫ
    logFilename = newFilename;
    // ����ļ������ڣ�����һ���µ��ļ�
    logfile.createNewFile(logPath, logFilename);
}

// д����־����Ϣ����
void Logger::logInfo(const string& origin, const string& message) {
    string logMessage = createLogMessage(LogLevel::INFO, origin, message);
    {
        lock_guard<mutex> lock(mtx);
        logQueue.push(logMessage);
        logCount++;  // ���Ӵ��������־����
    }
    cv.notify_all();  // ֪ͨ��־�̴߳���
}

// д����־�����漶��
void Logger::logWarning(const string& origin, const string& message) {
    string logMessage = createLogMessage(LogLevel::WARNING, origin, message);
    {
        lock_guard<mutex> lock(mtx);
        logQueue.push(logMessage);
        logCount++;  // ���Ӵ��������־����
    }
    cv.notify_all();  // ֪ͨ��־�̴߳���
}

// д����־�����󼶱�
void Logger::logError(const string& origin, const string& message) {
    string logMessage = createLogMessage(LogLevel::ERROR_, origin, message);
    {
        lock_guard<mutex> lock(mtx);
        logQueue.push(logMessage);
        logCount++;  // ���Ӵ��������־����
    }
    cv.notify_all();  // ֪ͨ��־�̴߳���
}

// д����־���������󼶱�
void Logger::logFatal(const string& origin, const string& message) {
    string logMessage = createLogMessage(LogLevel::FATAL, origin, message);
    {
        lock_guard<mutex> lock(mtx);
        logQueue.push(logMessage);
        logCount++;  // ���Ӵ��������־����
    }
    cv.notify_all();  // ֪ͨ��־�̴߳���
}
