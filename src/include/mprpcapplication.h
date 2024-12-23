#pragma once

#include "mprpcconfig.h"
#include "mprpcchannel.h"
#include "rpcheader.pb.h"
#include "mprpcconfig.h"
#include "mprpccontroller.h"

class MpRpcApplication
{
public:
    static MpRpcApplication& GetInstance();
    static void init(int argc,char** argv);

    static MpRpcConfig& GetConfig();
private:
    static MpRpcConfig m_config;

    MpRpcApplication()=default;
    MpRpcApplication(const MpRpcApplication&)=delete;
    MpRpcApplication(const MpRpcApplication&&)=delete;
};