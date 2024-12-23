#include <iostream>
#include <string>
#include <unistd.h>

#include "mprpcapplication.h"

MpRpcConfig MpRpcApplication::m_config;

void ShowArgsHelp()
{
    std::cout<<"format:command -i <config>"<<std::endl;
}

MpRpcApplication & MpRpcApplication::GetInstance()
{
    static MpRpcApplication mpapp;
    return mpapp;
}

MpRpcConfig& MpRpcApplication::GetConfig()
{
    return m_config;
}

void MpRpcApplication::init(int argc,char** argv)
{
    if(argc<2)
    {
        ShowArgsHelp();
        exit(EXIT_FAILURE);
    }

    int c=0;
    std::string config_file;
    while((c=getopt(argc,argv,"i:"))!=-1)
    {
        switch (c)
        {
        case 'i':
            config_file=optarg;
            break;
        case '?':
            ShowArgsHelp();
            exit(EXIT_FAILURE);
        case ':':
            ShowArgsHelp();
            exit(EXIT_FAILURE);
        default:
            break;
        }
    }

    //开始解析配置文件
    m_config.LoadConfigFile(config_file.c_str());

   /*  std::cout<<"rpcserverip:"<<m_config.Load("rpcserverip")<<std::endl;
    std::cout<<"rpcserverport:"<<m_config.Load("rpcserverport")<<std::endl;
    std::cout<<"zookeeperip:"<<m_config.Load("zookeeperip")<<std::endl;
    std::cout<<"zookeeperport:"<<m_config.Load("zookeeperport")<<std::endl; */
}