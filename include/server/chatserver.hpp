#ifndef CHATSERVER_H
#define CHATSERVER_H

#include <muduo/net/TcpServer.h>
#include <muduo/net/EventLoop.h>

using namespace muduo;
using namespace muduo::net;


class ChatServer
{
public:
    ChatServer(EventLoop* loop,
               const InetAddress& listenAddr,
               const string& nameArg);
    void start();
private:
    TcpServer _server;
    EventLoop * _loop;


    void onConnection(const TcpConnectionPtr& );
    void onMessage(const TcpConnectionPtr& ,Buffer *,Timestamp);
    
};

#endif
