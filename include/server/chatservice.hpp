#ifndef CHATSERVICE_H
#define CHATSERVICE_H

#include <unordered_map>
#include <functional>
#include <memory>
#include <mutex>
#include <muduo/net/TcpConnection.h>
#include <muduo/base/Timestamp.h>
#include "offlinemessage.hpp"
#include "groupmodel.hpp"
#include <friendmodel.hpp>
using namespace std;
using namespace muduo;
using namespace muduo::net;

#include "usermodel.hpp"
#include "json.hpp"
using json =nlohmann::json;

using MsgHandler = std::function<void(const TcpConnectionPtr &conn,json & js,Timestamp)>;
class ChatService
{
public:
    //获取单例对象的接口函数
    static ChatService* instance();
    void login(const TcpConnectionPtr &conn,json & js,Timestamp time);
    void reg(const TcpConnectionPtr &conn,json & js,Timestamp time);
    //一对一聊天业务
    void oneChat(const TcpConnectionPtr &conn,json & js,Timestamp time); 
    //添加好友业务
    void addFriend(const TcpConnectionPtr &conn,json & js,Timestamp time);
    // 创建群组业务
    void createGroup(const TcpConnectionPtr &conn, json &js, Timestamp time);
    // 加入群组业务
    void addGroup(const TcpConnectionPtr &conn, json &js, Timestamp time);
    // 群组聊天业务
    void groupChat(const TcpConnectionPtr &conn, json &js, Timestamp time);
    // 处理注销业务
    void loginout(const TcpConnectionPtr &conn, json &js, Timestamp time);
    // 处理客户端异常退出
    void clientCloseException(const TcpConnectionPtr &conn);
    //服务器异常 业务重置方法
    void reset();
    // 获取消息对应的处理器
    MsgHandler getHandler(int msgid);
    // 从redis消息队列中获取订阅的消息
    void handleRedisSubscribeMessage(int, string);



private:
    ChatService();

    std::unordered_map<int,MsgHandler> msgHandlerMap_;

    unordered_map<int,TcpConnectionPtr> _userConnMap;

    mutex _connMutex;

    UserModel _userModel;
    OfflineMsgModel _offlineMsgModel;

    FriendModel _friendModel;
    GroupModel _groupModel;
};

#endif
