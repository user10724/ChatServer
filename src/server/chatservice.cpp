#include "chatservice.hpp"
//#include "chatservice.hpp"
#include "public.hpp"
//#include "public.hpp"


//#include <string>
#include <vector>
#include <map>
//#include <iostream>
using namespace std;
using namespace muduo;


ChatService *ChatService::instance()
{
    static ChatService service;
    return &service;
}

//注册消息以及相应的回调
ChatService::ChatService()
{
    msgHandlerMap_.insert({LOGIN_MSG,bind(&ChatService::login,this,_1,_2,_3)});
    msgHandlerMap_.insert({REG_MSG,bind(&ChatService::reg,this,_1,_2,_3)}); 
    msgHandlerMap_.insert({ONE_CHAT_MSG,bind(&ChatService::oneChat,this,_1,_2,_3)});  
    msgHandlerMap_.insert({ADD_FRIEND_MSG,bind(&ChatService::addFriend,this,_1,_2,_3)});
}
#include <muduo/base/Logging.h>
MsgHandler ChatService::getHandler(int msgid)
{
    auto it = msgHandlerMap_.find(msgid);
    if (it == msgHandlerMap_.end())
    {
        // 返回一个默认的处理器，空操作
        return [=](const TcpConnectionPtr &conn, json &js, Timestamp) {
            LOG_ERROR << "msgid:" << msgid << " can not find handler!";
        };
    }
    else
    {
        return msgHandlerMap_[msgid];
    }
}

void ChatService::login(const TcpConnectionPtr & conn,json &js,Timestamp time)
{
    int id =js["id"].get<int>();
    string pwd = js["password"];

    User user = _userModel.query(id);
    if(user.getId()== id&& user.getPwd() == pwd)
    {
        if(user.getState()=="online")
        {
            json response;
            response["msgid"]=LOGIN_MSG_ACK;
            response["errno"] = 2;
            response["errmsg"] = "该账户已经登录，请登录其他账号";

            conn->send(response.dump()); 
        }
        else
        {
            //登录成功，记录用户连接信息
            {
                lock_guard<mutex> lock(_connMutex);
                _userConnMap.insert({id,conn});
            }
            //登录成功，更新用户状态信息
            user.setState("online");
            _userModel.updateState(user);
            json response;
            response["msgid"]=LOGIN_MSG_ACK;
            response["errno"] = 0;
            response["id"] = user.getId();
            response["name"] = user.getName();
            //查询该用户是否有离线消息
            vector<string> vec = _offlineMsgModel.query(id);
            if(!vec.empty())
            {
                response["offlinemessage"]=vec;
                //读取该用户的离线消息后，把该用户的所有离线消息删除
                _offlineMsgModel.remove(id);
            }
            //查询该用户的好友信息并返回
            vector<User> userVec = _friendModel.query(id);
            if(!userVec.empty())
            {
                vector<string> vec2;
                for(User &user :userVec)
                {
                    json js;
                    js["id"] = user.getId();
                    js["name"] = user.getName();
                    js["state"] = user.getState();
                    vec2.push_back(js.dump());
                }
                response["friends"]=vec2;
            }
            conn->send(response.dump()); 
        }
    }
    else{

        json response;
        response["msgid"]=LOGIN_MSG_ACK;
        response["errno"] = 1;
        response["errmsg"] = "用户名或者密码错误";

        conn->send(response.dump());  
    }
}

void ChatService::reg(const TcpConnectionPtr &conn,json& js,Timestamp time)
{
    string name = js["name"];
    string pwd = js["password"];

    User user;
    user.setName(name);
    user.setPwd(pwd);
    bool state =_userModel.insert(user);
    if(state)
    {
        json response;
        response["msgid"]=REG_MSG_ACK;
        response["errno"] = 0;
        response["id"] = user.getId();
        conn->send(response.dump());

    }
    else
    {
        //注册失败
        json response;
        response["msgid"]=REG_MSG_ACK;
        response["errno"] = 1;
        conn->send(response.dump()); 
    }
}

//处理客户端异常退出
void ChatService::clientCloseException(const TcpConnectionPtr &conn)
{   
    User user;
    {
        lock_guard<mutex> lock(_connMutex);

        for(auto it=_userConnMap.begin();it!=_userConnMap.end();++it)
        {
            if(it->second ==conn)
            {
                //从map表删除用户的连接信息
                user.setId(it->first);
                _userConnMap.erase(it);
                break;
            }
        }
    }
    if(user.getId()!=-1)
    {
        user.setState("offline");
        _userModel.updateState(user);
    }
} 

 void ChatService::oneChat(const TcpConnectionPtr &conn,json & js,Timestamp time)
{
    int toid = js["to"].get<int>();

    
    {
        lock_guard<mutex> lock(_connMutex);
        auto it = _userConnMap.find(toid);
        if(it != _userConnMap.end())
        {
            //toid online,  
            it->second->send(js.dump());
            return;
            
        }
    }
 
        //toid offline
    _offlineMsgModel.insert(toid ,js.dump()); 
}  

void ChatService::reset()
{
    //把online用户设置为offline
    _userModel.resetState();
}

void ChatService::addFriend(const TcpConnectionPtr &conn, json &js, Timestamp time)
{
    int userid = js["id"].get<int>();
    int friendid = js["friendid"].get<int>();

    //存储好友信息
    _friendModel.insert(userid, friendid);
}


//创建群组业务
void ChatService::createGroup(const TcpConnectionPtr & conn,json &js,Timestamp time)
{
    int userid = js["id"].get<int>();
    string name = js["groupname"];
    string desc = js["groupdesc"];

        //存储新创建的群组信息
    Group group(-1,name,desc);
    if(_groupModel.createGroup(group))
    {
        
        //存储群组创建人信息
        _groupModel.addGroup(userid,group.getId(),"creator");
    }
}

//加入群组业务
void ChatService::addGroup(const TcpConnectionPtr &conn,json &js,Timestamp time)
{
    int userid =js["id"].get<int>();
    int groupid=js["groupid"].get<int>();
    _groupModel.addGroup(userid, groupid, "normal");
}


//群组聊天业务
void ChatService::groupChat(const TcpConnectionPtr & conn,json &js,Timestamp time)
{
    int userid =js["id"].get<int>();
    int groupid = js["groupid"].get<int>();
    vector<int> useridVec = _groupModel.queryGroupUsers(userid, groupid);
    lock_guard<mutex> lock(_connMutex); 
    for(int id:useridVec)
    {
       
        auto it = _userConnMap.find(id);
        if(it!=_userConnMap.end())
        {
            //转发群消息
            it->second->send(js.dump());
        }
        else
        {
            //存储离线群消息
            _offlineMsgModel.insert(id,js.dump());
        }
    }
}
