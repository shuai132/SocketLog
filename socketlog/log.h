//
// Created by liushuai on 2018/11/30.
//

#ifndef LOG_H
#define LOG_H

#if __ANDROID__
    #include <android/log.h>
    #define LOG(...)        __android_log_print(ANDROID_LOG_DEBUG, "LOG", __VA_ARGS__)
#else
    #include <cstdio>
    #define LOG(fmt, ...)   do{printf(fmt, ##__VA_ARGS__); printf("\n");} while(0)
#endif

#endif //LOG_H
