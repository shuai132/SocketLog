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

class SocketLog {
    struct Msg {
        typedef unsigned char byte;

        size_t len = 0;
        byte* data = nullptr;

        inline void initMsg(const void* data = nullptr, size_t len = 0) {
            if (data == nullptr || len == 0) {
                this->len  = 0;
                this->data = nullptr;
                return;
            }

            this->len  = len;
            this->data = new byte[len];
            memcpy(this->data, data, len);
        }

        inline void moveMsg(Msg& msg) {
            this->len   = msg.len;
            this->data  = msg.data;
            msg.data = nullptr;
        }

        explicit Msg(const void* data = nullptr, size_t len = 0) {
            initMsg(data, len);
        }

        Msg(const Msg& msg) {
            initMsg(msg.data, msg.len);
        }

        Msg(Msg&& msg) noexcept {
            moveMsg(msg);
        }

        Msg& operator=(const Msg& msg) {
            if (&msg == this)
                return *this;

            delete[] this->data;
            initMsg(msg.data, msg.len);
            return *this;
        }

        Msg& operator=(Msg&& msg) noexcept {
            if (&msg == this)
                return *this;

            delete[] this->data;
            moveMsg(msg);
            return *this;
        }

        ~Msg() {
            delete[] data;
        }
    };

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

    std::queue<Msg> msgQueue;
    std::mutex msgQueueMutex;
    std::condition_variable msgQueueCondition;
};

#endif //SOCKETLOG_H
