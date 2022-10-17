/*
   muduo网络库给用户提供2个主要类
TCPserver :用于编写服务器程序
TcpClient: 用于编写客户端程序

epoll+线程池

好处：能够把网络I/O的代码和业务代码区分开
用户的连接和断开；用户的可读写事件；

*/
#include <functional>
#include <iostream>
#include <muduo/net/EventLoop.h>
#include <muduo/net/TcpServer.h>
#include <muduo/base/Logging.h>
using namespace muduo::net;
using namespace std;
using namespace placeholders;
using namespace muduo;
class ChatServer {
  public:
    ChatServer(EventLoop *loop,               //事件循环
               const InetAddress &listenAddr, // IP + Port
               const string &nameArg)         //服务器名字
        : _server(loop, listenAddr, nameArg), _loop(loop) {
        //给服务器注册用户连接的创建和断开回调
        _server.setConnectionCallback(
            bind(&ChatServer::onConnection, this, _1));
        //给服务器注册用户读写回调
        _server.setMessageCallback(
            bind(&ChatServer::onMessage, this, _1, _2, _3));
        _server.setThreadNum(4);
    }
    void start() { _server.start(); }

  private:
    TcpServer _server;
    EventLoop *_loop;
    void onConnection(const TcpConnectionPtr &conn) {
        if (conn->connected()) {
            cout << conn->peerAddress().toIpPort() << "->"
                 << conn->localAddress().toIpPort() << "state:online" << endl;
        } else {
            cout << conn->peerAddress().toIpPort() << "->"
                 << conn->localAddress().toIpPort() << "state:offline" << endl;
            conn->shutdown(); // close(fd)
                              // _loop->quit();
        }
    }
    void onMessage(const TcpConnectionPtr &conn, //连接
                   Buffer *buffer,               //缓冲区
                   muduo::Timestamp time) //接受到数据的时间信息
    {
        string buf = buffer->retrieveAllAsString();
        cout << "recv data:" << buf << "time:" << time.toString() << endl;
        LOG_ERROR<<"阿斯蒂芬贷记卡是否会";
        conn->send(buf);
    }
};

int main() {
    EventLoop loop;
    InetAddress addr("127.0.0.1", 6000);
    ChatServer server(&loop, addr, "ChatServer");
    server.start();
    loop.loop(); // epoll_wait
                 // 以阻塞的方式等待新用户连接，已连接用户的读写时间等
    
    return 0;
}

