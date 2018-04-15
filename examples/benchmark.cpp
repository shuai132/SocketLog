#include <iostream>
#include "../socketlog/SocketLog.h"

using std::cout;
using std::endl;

int main() {
    cout<<"SocketLog Test..."<<endl;

    auto threadFunc = [](int threadNum){
        cout<<"thread running..."<<threadNum<<endl;

        auto socketLog = SocketLog::getInstance();
        const int bufLen = 1024;
        char str[bufLen];
        const int msgNum = 100;
        for(int i=0; i<msgNum; i++) {
            snprintf(str, bufLen, "msg from thread:%d, msgNum=%d\n", threadNum, i);
            socketLog->post(str);
        }
    };

    auto socketLog = SocketLog::getInstance();
    cout<<"for receive message, please listen port:6666 in 10s"<<endl;
    cout<<"eg: nc localhost 6666"<<endl;
    std::this_thread::sleep_for(std::chrono::seconds(10));  // wait for client

    std::vector<std::thread*> myThreads;
    const int threadNum = 10;

    cout<<"create thread..."<<endl;
    for(int i=0; i<threadNum; i++) {
        myThreads.push_back(new std::thread(threadFunc, i));
    }

    for(auto t:myThreads) {
        t->join();
        delete t;
    }

    std::this_thread::sleep_for(std::chrono::seconds(10));  // make sure all msg has been send

    socketLog->disconnectAllStreams();
    cout<<"---end---"<<endl;

    return 0;
}
