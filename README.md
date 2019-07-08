# SocketLog

[![Build Status](https://www.travis-ci.org/shuai132/SocketLog.svg?branch=master)](https://www.travis-ci.org/shuai132/SocketLog)

A log review tool using tcp socket, with thread safe, send queue, and multi reviewer support.  
It can be used in most platform include android/ios, and only C++11 are required.

## Usage:
```cpp
SocketLog::getInstance()->post("Hello World!");
```

### There are also some examples show how to use it:

First, you should build my examples:
```bash
mkdir build && cd build
cmake ..
make
```
Second, run a simple example:
```bash
./examples/loopsender
```
or more complicated:
```bash
./examples/benchmark
```

Finally, just open any tcp tools(eg:nc) to see what happen:
```bash
nc localhost 6666
```

## Notice:
default port is 6666, and will try open bigger port automatic on bind failed。
