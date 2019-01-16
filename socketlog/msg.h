//
// Created by liushuai on 2018/8/9.
//

#ifndef MSG_H
#define MSG_H

#include <cstddef>
#include <cstring>
#include <string>

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
        this->len  = msg.len;
        this->data = msg.data;
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

    std::string toString() {
        return std::string((char*)data, 0, len);
    }

    ~Msg() {
        delete[] data;
    }
};

#endif //MSG_H
