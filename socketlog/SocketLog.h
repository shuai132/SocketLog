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
#include "msg.h"

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

    std::queue<Msg> msgQueue;
    std::mutex msgQueueMutex;
    std::condition_variable msgQueueCondition;
};

#endif //SOCKETLOG_H
