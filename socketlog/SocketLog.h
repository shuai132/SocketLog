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

struct Msg;
class SocketLog {
public:
    static SocketLog* getInstance();

    // post data to queue will be send automatic later
    void post(const void* buf, size_t len);
    void post(const char* str);
    void post(std::string str);

    // send data immediately with thread safe
    void send(const void* buf, size_t len);
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

    std::queue<Msg> msgQueue;
    std::mutex msgQueueMutex;
    std::condition_variable msgQueueCondition;
};

struct Msg {
    typedef unsigned char byte;

    size_t len = 0;
    byte* data = nullptr;

    explicit Msg(const void* data = nullptr, size_t len = 0) {
        if (data == nullptr || len == 0) {
            this->len  = 0;
            this->data = nullptr;
            return;
        }

        this->len  = len;
        this->data = new byte[len];
        memcpy(this->data, data, len);
    }

    Msg(const Msg& logMsg) {
        this->len  = len;
        this->data = new byte[len];
        memcpy(this->data, data, len);
    }

    Msg(Msg&& logMsg) noexcept {
        this->len   = logMsg.len;
        this->data  = logMsg.data;
        logMsg.data = nullptr;
    }

    Msg& operator=(const Msg& logMsg) {
        if (&logMsg == this)
            return *this;

        delete[] this->data;
        this->len = logMsg.len;
        if (len == 0) {
            data = nullptr;
        } else {
            data = new byte[len];
            memcpy(data, logMsg.data, len);
        }
        return *this;
    }

    Msg& operator=(Msg&& logMsg) noexcept {
        if (&logMsg == this)
            return *this;

        delete[] this->data;
        this->len   = logMsg.len;
        this->data  = logMsg.data;
        logMsg.data = nullptr;
        return *this;
    }

    ~Msg() {
        delete[] data;
    }
};

#endif //SOCKETLOG_H
