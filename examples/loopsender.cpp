#include <iostream>
#include "SocketLog.h"

using std::cout;
using std::endl;

int main() {
    cout<<"Send string every 1s..."<<endl;

    auto socketLog = SocketLog::getInstance();

    char buf[1024];
    int count = 0;
    while (true) {
        sprintf(buf, "hello, count=%d\n", count++);
        cout<<"try to send:"<<buf;
        socketLog->send(buf);
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    return 0;
}
