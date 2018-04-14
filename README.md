# SocketLog

A log review tool using tcp socket, with thread safe, send queue, and multi reviewer support.  
It can be used in most platform include android/ios, and only C++11 are required.

## There is a test demo, usage are as below:
```
mkdir build && cd build
cmake ..
make
./socketlog
```

Finally, you should open an tcp test tool(eg:nc):
```
nc localhost 6666
```
