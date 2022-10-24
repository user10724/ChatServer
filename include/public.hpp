#ifndef PUBLIC_H
#define PUBLIC_H

enum EnMsgType
{
    LOGIN_MSG = 1,// 登录消息
    LOGIN_MSG_ACK,// 登录响应消息

    REG_MSG,
    REG_MSG_ACK, //注册响应消息
    ONE_CHAT_MSG,
    ADD_FRIEND_MSG,//添加好友消息

    CREATE_GROUP_MSG, //SET UP GROUP
    ADD_GROUP_MSG,    //ADD GROUP
    GROUP_CHAT_MSG,   // GROUP CHAT


    LOGINOUT_MSG,
};



#endif
