#include "json.hpp"
using json = nlohmann::json;

#include <iostream>
#include <vector>
#include <map>
#include <string>
using namespace std;

void func1(){
    json js;
    js["msg_type"]=2;
    js["from"] = "zhangsan";
    js["msg"]["zhang san"]="hello wirld";
    js["msg"]["liu shuo"]="hello china";
    js["msg"]={{"zhang san","dadsfh dsfasf"},
    {"liu shuo","231237 389123"}};

    string sendBuf = js.dump();
    cout<<sendBuf.c_str()<<endl;
    cout<<js<<endl;
}
string func3()
{
    json js;

    // 直接序列化一个vector容器
    vector<int> vec;
    vec.push_back(1);
    vec.push_back(2);
    vec.push_back(5);

    js["list"] = vec;

    // 直接序列化一个map容器
    map<int, string> m;
    m.insert({1, "挺好的?"});
    m.insert({2, "华山"});
    m.insert({3, "泰山"});

    js["path"] = m;

    string sendBuf = js.dump(); // json数据对象 =》序列化 json字符串
    //cout<<sendBuf<<endl;
    return sendBuf;
}

int main(){
    string recvBuf =func3();
    //sfds
    json jsbuf =json::parse(recvBuf);

    cout<< jsbuf["list"]<<endl;
    cout<< jsbuf["path"]<<endl;
    map<int,string> mymap = jsbuf["path"];
    for(auto &p:mymap)
    {
        cout<<p.first<<" "<<p.second<<endl;
    }
    return 0;                                                 
}
