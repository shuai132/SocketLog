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
public:
    typedef std::function<bool(const std::string& msg)> Interceptor;

public:
    static SocketLog* getInstance();

    // post message to queue will be send automatic later
    void post(const std::string& msg);

    // flush all posted messages
    void flush();

    // send message immediately with thread safe
    void send(const std::string& msg);

    // expanded function
    void setSendInterceptor(Interceptor interceptor);

    int getPort();

    void disconnectAllStreams();

private:
    SocketLog();
    void startAcceptThread();
    void startSendThread();

    int port = 6666;
    TCPAcceptor* acceptor = nullptr;
    bool inited = false;

    std::vector<TCPStream*> connectedStreams;
    std::mutex streamMutex;

    std::queue<std::string> msgQueue;
    std::mutex msgQueueMutex;
    std::condition_variable msgQueueCondition;

    Interceptor sendInterceptor = nullptr;
};

#endif //SOCKETLOG_H
