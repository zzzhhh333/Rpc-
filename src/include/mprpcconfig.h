#pragma once

#include <string>
#include <unordered_map>

class MpRpcConfig
{
public:
    std::string Load(std::string key);
    void LoadConfigFile(const char* config_file);

private:
    std::unordered_map<std::string,std::string> m_configMap;
    void trim(std::string& buf);
};