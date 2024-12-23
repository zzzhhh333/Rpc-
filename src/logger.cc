#include "logger.h"

#include <time.h>
#include <iostream>

Logger& Logger::GetInstance()
{
    static Logger logger;
    return logger;
}

Logger::Logger()
{
    std::thread logWrite([&](){
        while(1)
        {
            time_t now=time(nullptr);
            tm* tnow=localtime(&now);
            char file_name[512]={0};
            sprintf(file_name,"%d-%d-%d-log.txt",tnow->tm_year+1900,tnow->tm_mon+1,tnow->tm_mday);
            FILE* fp=fopen(file_name,"a+");
            if(fp==nullptr)
            {
                std::cout<<"file:"<<file_name<<"open error!"<<std::endl;
                exit(EXIT_FAILURE);
            }

            std::string logmsg=m_lockqueue.Pop();

            char logbuf[512]={0};
            snprintf(logbuf,512,"%d:%d:%d[%s] => ",tnow->tm_hour,tnow->tm_min,tnow->tm_sec,(m_loglevel==INFO?"INFO":"ERROR"));
            logmsg.insert(0,logbuf);
            logmsg.append("\n");

            fputs(logmsg.c_str(),fp);
            fclose(fp);
        }
    });
    logWrite.detach();
}

void Logger::setLevel(int level)
{
    m_loglevel=level;
}

void Logger::Log(std::string msg)
{
    m_lockqueue.Push(msg);
}