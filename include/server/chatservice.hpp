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

#include "../../include/server/db/usermodel.hpp"
#include "../json.hpp"
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
    
    MsgHandler getHandler(int msgid);
    //服务器异常 业务重置方法
    void reset();

    void clientCloseException(const TcpConnectionPtr &conn);
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
