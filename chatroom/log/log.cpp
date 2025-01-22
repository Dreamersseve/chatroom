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

// 静态成员变量的定义
Logger* Logger::instance = nullptr;

// 将日志级别转换为字符串
string Logger::logLevelToString(LogLevel level) {
    switch (level) {
    case LogLevel::INFO: return "INFO";
    case LogLevel::WARNING: return "WARNING";
    case LogLevel::ERROR_: return "ERROR_";
    case LogLevel::FATAL: return "FATAL";
    default: return "UNKNOWN";
    }
}

// 生成日志信息字符串
string Logger::createLogMessage(LogLevel level, const string& origin, const string& message) {
    return "[" + time_::getCurrentTime() + "] [" + logLevelToString(level) + "] [" + origin + "] " + message;
}

// 处理日志写入，确保多线程安全
void Logger::writeLogToFile(const string& logMessage) {
    lock_guard<mutex> lock(mtx);  // 自动加锁，保证线程安全

    // 如果文件不存在，先创建文件
    logfile.createNewFile(logPath, logFilename);

    // 写入日志到文件
    logfile.appendToLastLine(logPath, logFilename, logMessage);
    cout << logMessage << endl;
}

// 日志线程函数
void Logger::logThreadFunction() {
    while (true) {
        unique_lock<mutex> lock(mtx);
        cv.wait(lock, [this]() { return !logQueue.empty() || stopLogging; });

        // 如果停止标志为真且队列为空，退出线程
        if (stopLogging && logQueue.empty()) {
            break;
        }

        // 获取队列中的一条日志消息
        string logMessage = logQueue.front();
        logQueue.pop();
        logCount--;  // 处理一个日志，减少待处理的数量
        lock.unlock();  // 解锁，允许主线程继续操作

        // 写入文件
        writeLogToFile(logMessage);
    }
}

// 私有构造函数，确保外部不能直接创建 Logger 实例
Logger::Logger(const string& path, const string& filename) : logPath(path), logFilename(filename) {
    // 确保文件存在
    logfile.createNewFile(logPath, logFilename);
}

// 获取全局唯一的 Logger 实例
Logger& Logger::getInstance() {
    if (instance == nullptr) {
        // 默认的路径和文件名配置
        instance = new Logger(LOGPATH, LOGNAME);
        instance->start();  // 启动日志线程
    }
    return *instance;
}

// 启动日志线程
void Logger::start() {
    logThread = thread(&Logger::logThreadFunction, this);  // 启动日志线程
}

// 停止日志线程，等待所有日志处理完毕后退出
void Logger::stop() {
    {
        lock_guard<mutex> lock(mtx);
        stopLogging = true;
    }
    cv.notify_all();  // 通知日志线程退出

    // 等待所有日志处理完毕
    while (logCount > 0) {
        this_thread::sleep_for(chrono::milliseconds(10));  // 让线程有时间处理日志
    }

    if (logThread.joinable()) {
        logThread.join();  // 等待日志线程结束
    }
}

// 设置新的日志路径
void Logger::setLogPath(const string& newPath) {
    lock_guard<mutex> lock(mtx);  // 确保线程安全
    logPath = newPath;
    // 如果文件不存在，创建一个新的文件
    logfile.createNewFile(logPath, logFilename);
}

// 设置新的日志文件名
void Logger::setLogFilename(const string& newFilename) {
    lock_guard<mutex> lock(mtx);  // 确保线程安全
    logFilename = newFilename;
    // 如果文件不存在，创建一个新的文件
    logfile.createNewFile(logPath, logFilename);
}

// 写入日志（信息级别）
void Logger::logInfo(const string& origin, const string& message) {
    string logMessage = createLogMessage(LogLevel::INFO, origin, message);
    {
        lock_guard<mutex> lock(mtx);
        logQueue.push(logMessage);
        logCount++;  // 增加待处理的日志数量
    }
    cv.notify_all();  // 通知日志线程处理
}

// 写入日志（警告级别）
void Logger::logWarning(const string& origin, const string& message) {
    string logMessage = createLogMessage(LogLevel::WARNING, origin, message);
    {
        lock_guard<mutex> lock(mtx);
        logQueue.push(logMessage);
        logCount++;  // 增加待处理的日志数量
    }
    cv.notify_all();  // 通知日志线程处理
}

// 写入日志（错误级别）
void Logger::logError(const string& origin, const string& message) {
    string logMessage = createLogMessage(LogLevel::ERROR_, origin, message);
    {
        lock_guard<mutex> lock(mtx);
        logQueue.push(logMessage);
        logCount++;  // 增加待处理的日志数量
    }
    cv.notify_all();  // 通知日志线程处理
}

// 写入日志（致命错误级别）
void Logger::logFatal(const string& origin, const string& message) {
    string logMessage = createLogMessage(LogLevel::FATAL, origin, message);
    {
        lock_guard<mutex> lock(mtx);
        logQueue.push(logMessage);
        logCount++;  // 增加待处理的日志数量
    }
    cv.notify_all();  // 通知日志线程处理
}
