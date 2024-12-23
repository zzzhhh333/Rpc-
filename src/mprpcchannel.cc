#include "mprpcapplication.h"
#include "zookeeperutil.h"
#include "logger.h"

#include <google/protobuf/descriptor.h>
#include <google/protobuf/message.h>
#include <string>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>



void MprpcChannel::CallMethod(const google::protobuf::MethodDescriptor* method,
                          google::protobuf::RpcController* controller,
                          const google::protobuf::Message* request,
                          google::protobuf::Message* response,
                          google::protobuf::Closure* done)
{
    const google::protobuf::ServiceDescriptor* service=method->service();
    std::string service_name=service->name();
    std::string method_name=method->name();

    std::string args_str;
    uint32_t args_size;
    if(request->SerializeToString(&args_str))
    {
        args_size=args_str.size();
    }
    else
    {
        controller->SetFailed("args serialize error");
        return;
    }
   
    uint32_t rpchead_size;
    
    mprpc::RpcHeader rpcheader;
    rpcheader.set_service_name(service_name);
    rpcheader.set_method_name(method_name);
    rpcheader.set_args_size(args_size);

    std::string rpchead_str;
    if(rpcheader.SerializeToString(&rpchead_str))
    {
        rpchead_size=rpchead_str.size();
    }
    else
    {
        controller->SetFailed("rpcheader serialize error");
        return;
    }

    std::string send_str;
    send_str.insert(0,std::string((char*)&rpchead_size,4));
    send_str+=rpchead_str;
    send_str+=args_str;

    std::cout<<"==================================="<<std::endl;
    std::cout<<"service_name:"<<service_name<<std::endl;
    std::cout<<"method_name:"<<method_name<<std::endl;
    std::cout<<"args_size:"<<args_size<<std::endl;
    std::cout<<"args:"<<args_str<<std::endl;
    std::cout<<"==================================="<<std::endl;

   

    //连接网络服务
    int fd=::socket(AF_INET,SOCK_STREAM,0);
    if(fd==-1)
    {
        controller->SetFailed("socket error");
        return;
    }
    
    ZookeeperUtil zk;
    zk.Start();
    char path[128]={0};
    sprintf(path,"/%s/%s",service_name.c_str(),method_name.c_str());
    std::string ipPort=zk.GetData(path);
    int idx=ipPort.find(":");
    if(idx==-1)
    {
        LOG_ERR("string ipPort.find error");
        return;
    }    
    std::string ip=ipPort.substr(0,idx);
    uint16_t port=atoi(ipPort.substr(idx+1).c_str());


    sockaddr_in addr;
    addr.sin_family=AF_INET;
    inet_pton(AF_INET,ip.c_str(),&addr.sin_addr.s_addr);
    addr.sin_port=htons(port);

    int ret=::connect(fd,(sockaddr*)&addr,sizeof(addr));
    if(ret == -1)
    {
        ::close(fd);
        controller->SetFailed("connect error");
        return;
    }

    int len=::send(fd,send_str.c_str(),send_str.size(),0);
    if(len ==-1)
    {
        ::close(fd);
        controller->SetFailed("send error");
        return;
    }

    char buff[1024]={0};
    int recv_len=::recv(fd,buff,1024,0);
    if(recv_len==-1)
    {
        ::close(fd);
        controller->SetFailed("recv error");
        return;
    }
    
    if(!response->ParseFromArray(buff,recv_len))
    {
        ::close(fd);
        controller->SetFailed("response parse error");
        return;
    }


    ::close(fd);
}