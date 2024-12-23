#include <iostream>

#include "mprpcapplication.h"
#include "user.pb.h"
#include "mprpcchannel.h"
#include "logger.h"


int main(int argc,char** argv)
{
    MpRpcApplication::init(argc,argv);

    fixbug::UserServiceRpc_Stub stub(new MprpcChannel());

    fixbug::LoginRequest request;
    request.set_name("zhang san");
    request.set_pwd("123456");

    fixbug::LoginResponse response;
    MprpcController m_controller;
    stub.Login(&m_controller,&request,&response,nullptr);

    if(m_controller.Failed())
    {
        LOG_ERR("Login has a error:%s",m_controller.ErrorText().c_str());
    }
    else
    {
        if(response.result().errcode()==0)
        {
            LOG_FIFO("call Login success:%d",response.success());
        }
        else
        {
            LOG_ERR("call Login error");
        }
    }

    
    return 0;
}