#include <iostream>
#include "SocketLog.h"

using std::cout;
using std::endl;

int main() {
    cout<<"Send string every 1s..."<<endl;

    auto socketLog = SocketLog::getInstance();

    socketLog->setSendInterceptor([](const byte* buf, size_t len) {
        cout<<"SendInterceptor: "<<Msg(buf, len).toString()<<endl;
        return false;
    });

    char buf[1024];
    int count = 0;
    while (true) {
        sprintf(buf, "hello, count=%d\n", count++);
        cout<<"try to send:"<<buf;
        socketLog->post(buf);
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    return 0;
}
