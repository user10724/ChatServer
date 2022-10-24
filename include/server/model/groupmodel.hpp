#ifndef GROUPMODEL_H
#define GROUPMODEL_H

#include "group.hpp"
#include <vector>
#include <string>
using namespace std;

class GroupModel
{
public:

    bool createGroup(Group &group);

    //加入群组
    void addGroup(int userid,int groupid,string role);

    vector<Group> queryGroups(int userid);

    
    vector<int> queryGroupUsers(int userid,int groupid);
};

#endif

