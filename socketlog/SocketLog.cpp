//
// Created by liushuai on 2018/4/13.
//

#include <csignal>
#include "SocketLog.h"

//#define DEBUG_SOCKET_LOG
#ifdef DEBUG_SOCKET_LOG
    #if __ANDROID__
        #include <android/log.h>
        #define LOGD(...)   __android_log_print(ANDROID_LOG_DEBUG, "SocketLog", __VA_ARGS__)
    #else
        #include <cstdio>
        #define LOGD(...)   do{printf(__VA_ARGS__); printf("\n");} while(0)
    #endif
#else
    #define LOGD(...)       ((void)0)
#endif


SocketLog* SocketLog::getInstance() {
    static auto socketLog = new SocketLog();
    return socketLog;
}

void SocketLog::post(const void* buf, size_t len) {
    if (!inited)
        return;

    msgQueueMutex.lock();
    msgQueue.push(Msg(buf, len));
    msgQueueMutex.unlock();
    msgQueueCondition.notify_one();
}

void SocketLog::post(const char* str) {
    post(str, strlen(str));
}

void SocketLog::post(std::string str) {
    post(str.c_str());
}

void SocketLog::send(const void* buf, size_t len) {
    std::lock_guard<std::mutex> lockStream(streamMutex);
    LOGD("SocketLog::send: len=%ld", len);

    if (!inited)
        return;

    auto size = connectedStreams.size();
    if (size == 0) {
        LOGD("no stream connected now! will not send!");
    } else {
        LOGD("stream connected num=%ld", size);
    }

    for(auto iter = connectedStreams.cbegin(); iter != connectedStreams.end();) {
        auto stream = *iter;

        LOGD("try to send to stream:%p, len=%ld", stream, len);
        auto ret = stream->send(buf, len);
        if (ret == -1) {
            LOGD("send failed! delete(close) stream");
            delete stream;
            iter = connectedStreams.erase(iter);
        } else {
            LOGD("send success! send len=%ld", len);
            iter++;
        }
    }
}

void SocketLog::send(const char* str) {
    send(str, strlen(str));
}

void SocketLog::send(std::string str) {
    send(str.c_str());
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
    acceptor = new TCPAcceptor(port);

    if (acceptor->start() == 0) {
        LOGD("SocketLog start success! port:%d", port);
        startAcceptThread();
        startSendThread();
        inited = true;
    } else {
        LOGD("SocketLog start failed! check your port:%d", port);
        inited = false;
        delete acceptor;
        acceptor = nullptr;
    }
}

void SocketLog::startSendThread() {
    new std::thread([this]{
        LOGD("sendThread running...");
        Msg logMsg;

        while (true) {
            {
                std::unique_lock<std::mutex> lock(msgQueueMutex);
                LOGD("msgQueue.size=%ld, msgQueueCondition will %s", msgQueue.size(),
                     msgQueue.empty() ? "waiting..." : "not wait!");
                msgQueueCondition.wait(lock, [this] { return !msgQueue.empty(); });
                LOGD("msgQueueCondition wake! msgQueue.size=%ld", msgQueue.size());

                logMsg = std::move(msgQueue.front());
                msgQueue.pop();
            }

            LOGD("try to send data.len=%ld", logMsg.len);
            send(logMsg.data, logMsg.len);
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
                LOGD("accept failed! will stop acceptThread!");
                inited = false;
                delete acceptor;
                acceptor = nullptr;
                return;
            }
        }
    });
}
