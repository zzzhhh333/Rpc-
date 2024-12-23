#include "test.pb.h"
#include <iostream>

int main()
{
    fixbug::FriendListResponse rsp;
    fixbug::ResultCode* result=rsp.mutable_result();
    result->set_errcode(0);
    result->set_errmsg("");

    fixbug::User* user1=rsp.add_friend_list();
    user1->set_name("zhang san");
    user1->set_age(11);
    user1->set_sex(fixbug::User::MAN);

    fixbug::User* user2=rsp.add_friend_list();
    user2->set_name("li si");
    user2->set_age(21);
    user2->set_sex(fixbug::User::MAN);

    std::cout<<rsp.friend_list_size()<<std::endl;
}



int main1()
{
    fixbug::LoginRequest req;
    req.set_name("zhang san");
    req.set_pwd("123456");


    std::string send_str;
    if(req.SerializeToString(&send_str))
    {
        std::cout<<send_str<<std::endl;
    }

    fixbug::LoginRequest req2;
    if(req2.ParseFromString(send_str))
    {
        std::cout<<req2.name()<<std::endl;
        std::cout<<req2.pwd()<<std::endl;
    }
    return 0;
}