#include <iostream>
#include <string>

#include "user.pb.h"
#include "mprpcapplication.h"
#include "rpcprovider.h"
#include "logger.h"

class UserService :public fixbug::UserServiceRpc
{
public:
    bool Login(std::string name,std::string pwd)
    {
        std::cout<<"Login Service "<<std::endl;
        std::cout<<"name: "<<name<<" pwd: "<<pwd<<std::endl;
        return true;
    }

    void Login(::google::protobuf::RpcController* controller,
                       const ::fixbug::LoginRequest* request,
                       ::fixbug::LoginResponse* response,
                       ::google::protobuf::Closure* done)
    {
        std::string name=request->name();
        std::string pwd=request->pwd();

        bool success=Login(name,pwd);

        fixbug::ResultCode* rc=response->mutable_result();
        rc->set_errcode(0);
        rc->set_errmsg("");
        response->set_success(success);

        done->Run();
    }


private:
};



int main(int argc,char ** argv)
{
    
    MpRpcApplication::init(argc,argv);

    RpcProvider provider;
    provider.NotifyService(new UserService());

    provider.Run();

    return 0;
}