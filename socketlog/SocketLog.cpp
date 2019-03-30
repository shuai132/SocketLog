//
// Created by liushuai on 2018/4/13.
//

#include <csignal>
#include "SocketLog.h"
#include "log.h"

#define LOGE(fmt, ...)      LOG(fmt, ##__VA_ARGS__)

//#define DEBUG_THIS_FILE
#ifdef DEBUG_THIS_FILE
    #define LOGD(fmt, ...)  LOG(fmt, ##__VA_ARGS__)
#else
    #define LOGD(fmt, ...)  ((void)0)
#endif

SocketLog* SocketLog::getInstance() {
    static auto socketLog = new SocketLog();
    return socketLog;
}

void SocketLog::post(const std::string& msg) {
    if (!inited)
        return;

    msgQueueMutex.lock();
    msgQueue.push(msg);
    msgQueueMutex.unlock();
    msgQueueCondition.notify_one();
}

void SocketLog::flush() {
    std::lock_guard<std::mutex> lock(msgQueueMutex);

    while (!msgQueue.empty()) {
        send(msgQueue.front());
        msgQueue.pop();
    }
}

void SocketLog::send(const std::string& msg) {
    std::lock_guard<std::mutex> lockStream(streamMutex);
    LOGD("SocketLog::send: %s", msg.c_str());

    if (sendInterceptor) {
        if (sendInterceptor(msg)) return;
    }

    if (!inited)
        return;

    auto size = connectedStreams.size();
    if (size == 0) {
        LOGD("no stream connected now! will not send!");
    } else {
        LOGD("stream connected num=%ld", size);
    }

    for(auto iter = connectedStreams.cbegin(); iter != connectedStreams.cend();) {
        auto& stream = *iter;

        LOGD("try to send to stream:%p", stream);
        auto ret = stream->send(msg.data(), msg.length());
        if (ret == -1) {
            LOGE("send failed! delete(close) stream");
            delete stream;
            iter = connectedStreams.erase(iter);
        } else {
            LOGD("send success!");
            iter++;
        }
    }
}

void SocketLog::setSendInterceptor(Interceptor interceptor) {
    this->sendInterceptor = std::move(interceptor);
}

int SocketLog::getPort() {
    return this->port;
}

void SocketLog::disconnectAllStreams() {
    std::lock_guard<std::mutex> lockStream(streamMutex);

    for(auto stream:connectedStreams) {
        stream->send("SocketLog say Bye!\n");
        delete stream;
    }
    connectedStreams.clear();
}

SocketLog::SocketLog() {
    signal(SIGPIPE, SIG_IGN);

    START:
    acceptor = new TCPAcceptor(port);

    if (acceptor->start() == 0) {
        LOG("SocketLog start success! port:%d", port);
        startAcceptThread();
        startSendThread();
        inited = true;
    } else {
        LOGE("SocketLog start failed! check your port:%d", port);
        inited = false;
        delete acceptor;
        acceptor = nullptr;

        port++;
        goto START;
    }
}

void SocketLog::startSendThread() {
    new std::thread([this]{
        LOGD("sendThread running...");
        std::string msg;

        while (true) {
            {
                std::unique_lock<std::mutex> lock(msgQueueMutex);
                LOGD("msgQueue.size=%ld, msgQueueCondition will %s", msgQueue.size(),
                     msgQueue.empty() ? "waiting..." : "not wait!");
                msgQueueCondition.wait(lock, [this] { return !msgQueue.empty(); });
                LOGD("msgQueueCondition wake! msgQueue.size=%ld", msgQueue.size());

                msg = std::move(msgQueue.front());
                msgQueue.pop();
            }

            send(msg);
        }
    });
}

void SocketLog::startAcceptThread() {
    new std::thread([this]{
        LOGD("acceptThread running...");

        while (true) {
            LOGD("wait for accept...");
            TCPStream* stream = acceptor->accept();

            if (stream) {
                LOGD("catch an accept:%p", stream);
                stream->send("Welcome to SocketLog!\n");
                streamMutex.lock();
                connectedStreams.push_back(stream);
                streamMutex.unlock();
            } else {
                LOGE("accept failed! will stop acceptThread!");
                inited = false;
                delete acceptor;
                acceptor = nullptr;
                return;
            }
        }
    });
}
