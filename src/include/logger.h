#pragma once

#include "lockqueue.h"

#include <string>

enum Loglevel
{
    INFO,
    ERROR,
};

class Logger
{
public:
    static Logger& GetInstance();

    void setLevel(int level);

    void Log(std::string msg);
private:
    int m_loglevel;
    LockQueue<std::string> m_lockqueue;

    Logger();
    Logger(const Logger&)=delete;
    Logger(Logger&&)=delete;

};


#define LOG_FIFO(logmsgformat,...)\
        do\
        {\
            Logger& log=Logger::GetInstance();\
            log.setLevel(INFO);\
            char buf[1024]={0};\
            snprintf(buf,1024,logmsgformat,##__VA_ARGS__);\
            log.Log(buf);\
        }while(0);

#define LOG_ERR(logmsgformat,...)\
        do\
        {\
            Logger& log=Logger::GetInstance();\
            log.setLevel(ERROR);\
            char buf[1024]={0};\
            snprintf(buf,1024,logmsgformat,##__VA_ARGS__);\
            log.Log(buf);\
        }while(0); 
        