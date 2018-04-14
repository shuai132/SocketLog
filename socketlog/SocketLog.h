//
// Created by liushuai on 2018/4/13.
//

#ifndef SOCKETLOG_H
#define SOCKETLOG_H

#include <cstring>
#include <thread>
#include <mutex>
#include <vector>
#include <queue>
#include <condition_variable>
#include "lib/tcpacceptor.h"

struct LogMsg;
class SocketLog {
public:
    static SocketLog* getInstance();

    // post data to queue will be send automatic later
    void post(const char* buf, size_t len);
    void post(const char* str);
    void post(std::string str);

    // send data immediately with thread safe
    void send(const char* buf, size_t len);
    void send(const char* str);
    void send(std::string str);

    void disconnectAllStreams();

private:
    SocketLog();
    void startAcceptThread();
    void startSendThread();

    const int port = 6666;
    TCPAcceptor* acceptor = nullptr;
    bool inited = false;

    vector<TCPStream*> connectedStreams;
    std::mutex streamMutex;
    std::mutex sendMutex;

    std::queue<LogMsg> msgQueue;
    std::mutex msgQueueMutex;
    std::condition_variable msgQueueCondition;
};

struct LogMsg {
    explicit LogMsg(const char* data = nullptr, size_t len = 0) {
        this->len  = len;
        this->data = new char[len];
        memcpy(this->data, data, len);
    }

    LogMsg(const LogMsg& logMsg) {
        this->len  = len;
        this->data = new char[len];
        memcpy(this->data, data, len);
    }

    LogMsg(LogMsg&& logMsg) noexcept {
        this->len   = logMsg.len;
        this->data  = logMsg.data;
        logMsg.data = nullptr;
    }

    LogMsg& operator=(const LogMsg& logMsg) {
        if (&logMsg == this)
            return *this;

        this->len = logMsg.len;
        memcpy(data, logMsg.data, len);
        return *this;
    }

    LogMsg& operator=(LogMsg&& logMsg) noexcept {
        if (&logMsg == this)
            return *this;

        this->len   = logMsg.len;
        this->data  = logMsg.data;
        logMsg.data = nullptr;
        return *this;
    }

    ~LogMsg() {
        delete[] data;
    }

    size_t len;
    char* data;
};

#endif //SOCKETLOG_H
