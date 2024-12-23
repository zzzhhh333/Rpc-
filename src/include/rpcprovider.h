#pragma once

#include <google/protobuf/service.h>
#include <muduo/net/TcpServer.h>
#include <muduo/net/EventLoop.h>
#include <muduo/net/InetAddress.h>
#include <unordered_map>
#include <string>
#include <google/protobuf/descriptor.h>

class RpcProvider
{
public:
    void NotifyService(::google::protobuf::Service* service);

    void Run();
private:
    muduo::net::EventLoop m_loop;

    //存储Service的对象和相应方法的信息
    struct ServiceInfo
    {
        ::google::protobuf::Service* service;
        //存储service对象的方法的名称和对应的描述
        std::unordered_map<std::string,const ::google::protobuf::MethodDescriptor*> m_MethodMap;
    };

    //存储Service对象的名称和对应的描述
    std::unordered_map<std::string,ServiceInfo> m_ServiceMap;

    void onConnection(const muduo::net::TcpConnectionPtr& conn);
    void onMessage(const muduo::net::TcpConnectionPtr& conn,muduo::net::Buffer* buf,muduo::Timestamp time);

    //Closure的回调操作
    void SendRpcResponse(const muduo::net::TcpConnectionPtr& conn,google::protobuf::Message*);
};