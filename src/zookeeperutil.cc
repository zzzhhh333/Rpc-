#include "zookeeperutil.h"
#include "mprpcapplication.h"

void global_watcher(zhandle_t* zh,int type,int state,const char* path,void* watcherCtx)
{
    if(type==ZOO_SESSION_EVENT)
    {
        if(state==ZOO_CONNECTED_STATE)
        {
            sem_t *sem=(sem_t*)zoo_get_context(zh);
            sem_post(sem);
        }
    }
}



ZookeeperUtil::ZookeeperUtil()
    :m_zhandle(nullptr)
{

}

ZookeeperUtil::~ZookeeperUtil()
{
    if(m_zhandle!=nullptr)
    {
        zookeeper_close(m_zhandle);
    }
}

void ZookeeperUtil::Start()
{
    std::string host=MpRpcApplication::GetInstance().GetConfig().Load("zookeeperip");
    std::string port=MpRpcApplication::GetInstance().GetConfig().Load("zookeeperport");
    std::string connstr=host+":"+port;

    m_zhandle=zookeeper_init(connstr.c_str(),global_watcher,30000,nullptr,nullptr,0);
    if(m_zhandle==nullptr)
    {
        std::cout<<"zookeeper_init error!"<<std::endl;
        exit(EXIT_FAILURE);
    }

    sem_t sem;
    sem_init(&sem,0,0);
    zoo_set_context(m_zhandle,&sem);

    sem_wait(&sem);

    std::cout<<"zookeeper_init success"<<std::endl;
}


void ZookeeperUtil::Create(const char* path,const char* data,int datalen,int state)
{
    char path_buff[128];
    int bufflen=sizeof(path_buff);
    int flag;
    flag=zoo_exists(m_zhandle,path,0,nullptr);
    if(flag==ZNONODE)
    {
        flag=zoo_create(m_zhandle,path,data,datalen,&ZOO_OPEN_ACL_UNSAFE,state,path_buff,bufflen);
        if(flag==ZOK)
        {
            std::cout<<"znode create success...path:"<<path<<std::endl;
        }
        else
        {
            std::cout<<"flag:"<<flag<<std::endl;
            std::cout<<"znode create error... path:"<<path<<std::endl;
            exit(EXIT_FAILURE);
        }
    }
}

std::string ZookeeperUtil::GetData(const char* path)
{
    char buffer[128]={0};
    int bufflen=sizeof(buffer);
    int flag=zoo_get(m_zhandle,path,0,buffer,&bufflen,nullptr);
    if(flag!=ZOK)
    {
        std::cout<<"get znode error... path:"<<path<<std::endl;
        return "";
    }
    else
    {
        return buffer;
    }
}