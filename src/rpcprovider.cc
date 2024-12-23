#include "rpcprovider.h"
#include "mprpcapplication.h"
#include "rpcheader.pb.h"
#include "zookeeperutil.h"

void RpcProvider::NotifyService(::google::protobuf::Service *service)
{
    //获取service服务的描述信息
    const google::protobuf::ServiceDescriptor* serviceDec=service->GetDescriptor();
    //获取service服务对象的名称
    std::string service_name=serviceDec->name();
    //获取service服务对象所包含的方法的数量
    int methodNum=serviceDec->method_count();

    ServiceInfo m_service;

    std::cout<<"service_name:"<<service_name<<std::endl;
    for(int i=0;i<methodNum;i++)
    {
        const ::google::protobuf::MethodDescriptor* methodDec=serviceDec->method(i);
        std::string method_name=methodDec->name();
        m_service.m_MethodMap.insert({method_name,methodDec});
        std::cout<<"method_name:"<<method_name<<std::endl;
    }
    m_service.service=service;

    m_ServiceMap.insert({service_name,m_service});
    
    
}

void RpcProvider::Run()
{
    std::string ip=MpRpcApplication::GetInstance().GetConfig().Load("rpcserverip");
    uint16_t port=atoi(MpRpcApplication::GetInstance().GetConfig().Load("rpcserverport").c_str()); 
    muduo::net::InetAddress addr(ip,port);

    muduo::net::TcpServer server(&m_loop,addr,"RpcServer");

    server.setConnectionCallback(std::bind(&RpcProvider::onConnection,this,std::placeholders::_1));
    server.setMessageCallback(std::bind(&RpcProvider::onMessage,this,std::placeholders::_1,std::placeholders::_2,std::placeholders::_3));

    server.setThreadNum(3);

    ZookeeperUtil zp;
    zp.Start();
    std::string ipPort=ip+":"+std::to_string(port);
    for(auto&it:m_ServiceMap)
    {
        std::string service_name;
        std::string method_name;
        char service_path[128];
        service_name=it.first;
        sprintf(service_path,"/%s",service_name.c_str());
        zp.Create(service_path,nullptr,0);
        for(auto& itm:it.second.m_MethodMap)
        {
            method_name=itm.first;
            char method_path[128]={0};
            sprintf(method_path,"/%s/%s",service_name.c_str(),method_name.c_str());
            zp.Create(method_path,ipPort.c_str(),ipPort.size(),ZOO_EPHEMERAL);
        }
        
    }

    server.start();

    std::cout<<"RcpServer Start Service at ip:"<<ip<<" port:"<<port<<std::endl;

    m_loop.loop();

}

void RpcProvider::onConnection(const muduo::net::TcpConnectionPtr& conn)
{
    if(!conn->connected())
    {
        conn->shutdown();
    }
}

void RpcProvider::onMessage(const muduo::net::TcpConnectionPtr& conn,muduo::net::Buffer* buf,muduo::Timestamp time)
{
    std::string strmsg=buf->retrieveAllAsString();
    uint32_t headsize=0;
    strmsg.copy((char*)&headsize,4,0);
    
    std::string service_name;
    std::string method_name;
    uint32_t args_size;

    std::string rpc_header_str=strmsg.substr(4,headsize);
    mprpc::RpcHeader rpcheadr;
    if(rpcheadr.ParseFromString(rpc_header_str))
    {
        service_name=rpcheadr.service_name();
        method_name=rpcheadr.method_name();
        args_size=rpcheadr.args_size();
    }
    else
    {
        std::cout<<"rpcheadr parse error"<<std::endl;
    }

    std::string args;
    args=strmsg.substr(4+headsize,args_size);

    std::cout<<"==================================="<<std::endl;
    std::cout<<"service_name:"<<service_name<<std::endl;
    std::cout<<"method_name:"<<method_name<<std::endl;
    std::cout<<"args_size:"<<args_size<<std::endl;
    std::cout<<"args:"<<args<<std::endl;
    std::cout<<"==================================="<<std::endl;


    //获取service和method对象
    auto it=m_ServiceMap.find(service_name);
    if(it==m_ServiceMap.end())
    {
        std::cout<<service_name<<"is not exist"<<std::endl;
        return;
    }

    auto mit=it->second.m_MethodMap.find(method_name);
    if(mit==it->second.m_MethodMap.end())
    {
        std::cout<<service_name<<":"<<method_name<<"is not exist"<<std::endl;
        return;
    }

    google::protobuf::Service* service=it->second.service;
    const google::protobuf::MethodDescriptor* method=mit->second;

    google::protobuf::Message* Request=service->GetRequestPrototype(method).New();
    if(!Request->ParseFromString(args))
    {
        std::cout<<"request parse error!content:"<<args<<std::endl;
        return;
    }
    google::protobuf::Message* Response=service->GetResponsePrototype(method).New();

    google::protobuf::Closure* done=google::protobuf::NewCallback<RpcProvider,const muduo::net::TcpConnectionPtr&,google::protobuf::Message*>(this,&RpcProvider::SendRpcResponse,conn,Response);


    service->CallMethod(method,nullptr,Request,Response,done);

}

void RpcProvider::SendRpcResponse(const muduo::net::TcpConnectionPtr& conn,google::protobuf::Message* response)
{
    std::string send_str;
    if(response->SerializeToString(&send_str))
    {
        conn->send(send_str);
    }
    else
    {
        std::cout<<"response Serialize error"<<std::endl;
    }
    conn->shutdown();
}

