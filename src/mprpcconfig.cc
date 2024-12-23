#include "mprpcconfig.h"

#include <iostream>
#include <string>

void MpRpcConfig::trim(std::string& buf)
{
    int idx = buf.find_first_not_of(" ");
    if (idx != -1)
    {
        buf=buf.substr(idx);
    }
    idx = buf.find_last_not_of(" ");
    if (idx != -1)
    {
        buf=buf.substr(0, idx+1);
    }


    if(buf[buf.size()-1]=='\n')
    {
        buf=buf.substr(0,buf.size()-1);
    }
}

std::string MpRpcConfig::Load(std::string key)
{
    auto it=m_configMap.find(key);
    if(it!=m_configMap.end())
    {
        return it->second;
    }
    return std::string();
}
void MpRpcConfig::LoadConfigFile(const char *config_file)
{
    FILE* file=fopen(config_file,"r");
    if(file==nullptr)
    {   
        std::cout<<config_file<<"is not exist"<<std::endl;
        exit(EXIT_FAILURE);
    }


    while(!feof(file))
    {
        char buff[128]={0};
        fgets(buff,sizeof(buff),file);

        //1.去空格
        std::string buf(buff);
        trim(buf);

        //2.去注释
        if(buf[0]=='#')
        {
            continue;
        }

        //3.格式判断
        int idxx=buf.find("=");
        if(idxx!=-1)
        {
            std::string key=buf.substr(0,idxx);
            trim(key);
            std::string value=buf.substr(idxx+1);
            trim(value);
            m_configMap.insert({key,value});
        }

    }
   

}