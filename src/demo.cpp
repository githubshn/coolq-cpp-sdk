#include "cqsdk/cqsdk.h"
#include<string>
#include<cstring>
#include<ctime>
#include "NumberGuess.cpp"

// #define SJJ 204927320
#define SJJ 1572264811
#define SHN 2411393416

using namespace std;
void debug_print(string mess);
void debug_print(char* mess);
void debug_print(int mess);

// namespace cq::app 包含插件本身的生命周期事件和管理
// namespace cq::event 用于注册 QQ 相关的事件处理函数
// namespace cq::api 用于调用酷 Q 提供的接口
// namespace cq::logging 用于日志
// namespace cq::message 提供封装了的 Message 等类

class Robot{
    public:
        // state
        Number_Guess *Number_Guess_ing;
        bool En_Number_Guess;
        bool En_Catch_SJJ;
        Robot(){
        }
        void init(){
            En_Number_Guess = false;
            En_Catch_SJJ = false;
            Number_Guess_ing = NULL;
        }
        void response_to_shn(const cq::GroupMessageEvent &e) {
            cq::Message msg = e.message;
            string strmsg = to_string(msg);
            cq::logging::debug("shn", "next");
            if (strmsg.find("小鸭子，")==0) {
                cq::logging::debug("shn", "repeat");
                msg = strmsg.substr(strlen("小鸭子，"));
                cq::message::send(e.target, msg);
            }
            return;
        }
        void response_to_sjj(const cq::GroupMessageEvent &e) {
            cq::Message msg = e.message;
            string strmsg = to_string(msg);
            // cq::logging::debug("csz", strmsg);
            cq::logging::debug("sjj", "next");
            // cq::message::send(e.target, msg);
            if (En_Catch_SJJ) {
                if (strmsg.find("没人了？那我喵一声应该不会被发现")!=strmsg.npos) {
                    msg = "抓住一只野生的喵！";
                    cq::message::send(e.target, msg);
                }
            }
            if (En_Number_Guess) {
                if(strmsg.find("进制猜数字游戏开始")!=strmsg.npos){
                    Number_Guess_ing = new Number_Guess();
                    cq::logging::debug("csz", "new game");
                    msg = "猜数字 9087";
                    cq::message::send(e.target, msg);
                } else if (Number_Guess_ing!=NULL) {
                    if (strmsg.find("数字")!=strmsg.npos && strmsg.find("的匹配结果")!=strmsg.npos) {
                        cq::logging::debug("csz", "adding");
                        int a = int(strmsg[33])-48;
                        int b = int(strmsg[35])-48;
                        char adder[4];
                        for(int i=0; i<4; i++) {
                            adder[i] = int(strmsg[7+i]);
                        }
                        // debug_print(adder); debug_print(a); debug_print(b);
                        if (a==4 || Number_Guess_ing->get_times()==10) {
                            Number_Guess_ing = NULL;
                            En_Number_Guess = false;
                            cq::logging::debug("csz", "end game!");
                            return;
                        }
                        Number_Guess_ing->add_rulers(adder, a, b);
                        char suggestion[10];
                        itoa(Number_Guess_ing->get_suggestion(), suggestion, 10);
                        msg = "猜数字 " + string(suggestion);
                        cq::message::send(e.target, msg);
                    }
                }
            }
            return;
        }
};

inline string get_time_stamp() {
    char time_stamp[20];
    time_t t;
    time(&t);
    sprintf("%d", time_stamp, t);
    return time_stamp;
}

inline string get_time_string() {
    time_t t;
    struct tm *ptminfo;
    time(&t);
    ptminfo = localtime(&t);
    char time_str[50];
    sprintf("%02d-%02d-%02d %02d:%02d:%02d\n", time_str,
            ptminfo->tm_year + 1900, ptminfo->tm_mon + 1, ptminfo->tm_mday,
            ptminfo->tm_hour, ptminfo->tm_min, ptminfo->tm_sec);
    return time_str;
}

inline void debug_print(string mess) {
    cq::logging::debug("Debug", mess);
    // printf("%s:\n%s\n", get_time_string().c_str(), type, mess);
}
inline void debug_print(char* mess) {
    cq::logging::debug("Debug", string(mess));
}
inline void debug_print(int mess) {
    char str[10];
    itoa(mess, str, 10);
    cq::logging::debug("Debug", string(str));
}


Robot agent;

// 插件入口，在静态成员初始化之后，app::on_initialize 事件发生之前被执行，用于配置 SDK 和注册事件回调
CQ_MAIN {
    cq::app::on_enable = [] {
        // cq::logging、cq::api、cq::dir 等命名空间下的函数只能在事件回调函数内部调用，而不能直接在 CQ_MAIN 中调用
        debug_print("The plug-in components is enabled!");
        // cq::logging::debug(u8"启用", u8"插件已启动");
        agent.init();
    };

    cq::app::on_disable = [] {
        debug_print("The plug-in components is disabled!");
        // fclose(stdout);
    };

    cq::event::on_private_msg = [](const cq::PrivateMessageEvent &e) {
        // cq::logging::debug(u8"消息", u8"收到私聊消息：" + e.message + u8"，发送者：" + std::to_string(e.user_id));
        debug_print("Private message reveived:" + e.message + " ,From：" + std::to_string(e.user_id));

        try {
            // cq::api::send_private_msg(e.user_id, e.message); // echo 回去
            // cq::api::send_msg(e.target, e.message); // 使用 e.target 指定发送目标

            // MessageSegment 类提供一些静态成员函数以快速构造消息段
            // cq::Message msg = cq::MessageSegment::contact(cq::MessageSegment::ContactType::GROUP, 201865589);
            // msg.send(e.target); // 使用 Message 类的 send 成员函数
            cq::Message msg = std::to_string(e.user_id);
            msg.send(e.target);
            // msg.send(cq::Target::user(649310342));
        } catch (const cq::exception::ApiError &err) {
            // API 调用失败
            cq::logging::debug(u8"API", u8"调用失败，错误码：" + std::to_string(err.code));
        }
        e.block(); // 阻止事件继续传递给其它插件
    };

    cq::event::on_group_msg = [](const auto &e /* 使用 C++ 的 auto 关键字 */) {
        const auto memlist = cq::api::get_group_member_list(e.group_id); // 获取数据接口
        cq::Message msg = "debug : group:" + std::to_string(e.group_id) + " from:" + std::to_string(e.user_id); 
        // msg.send(cq::Target::user(649310342));
        if (e.user_id==SJJ){
            cq::logging::debug("sjj", "response to SJJ");
            // cq::message::send(e.target, msg);
            agent.response_to_sjj(e);
            return;
        } else if (e.user_id==SHN){
            agent.response_to_shn(e);
        }
        // msg = "group:" + std::to_string(e.group_id) + " from:" + std::to_string(e.user_id); // Message 类可以进行加法运算
        // cq::message::send(e.target, msg); // 使用 message 命名空间的 send 函数
        // if (e.user_id==2411393416 && e.group_id!=250253824) cq::message::send(e.target, msg);
        // msg.send(cq::Target::user(649310342));
    };
}

// 添加菜单项，需要同时在 <appid>.json 文件的 menu 字段添加相应的条目，function 字段为 menu_demo_1
CQ_MENU(Number_Guess_Enable) {
    try{
        if (!agent.En_Number_Guess) {
            cq::logging::info(u8"菜单", u8"Number_Guess_Enable");
            agent.En_Number_Guess = true;
        } else {
            cq::logging::info(u8"菜单", u8"Number_Guess_Disable");
            agent.En_Number_Guess = false;
        }
    } catch (const cq::exception::ApiError &) {
        cq::logging::warning(u8"菜单", u8"Number_Guess_Fail");
    }
}
CQ_MENU(Catch_SJJ_Enable) {
    try{
        if (!agent.En_Catch_SJJ) {
            cq::logging::info(u8"菜单", u8"Catch_SJJ_Enable");
            agent.En_Catch_SJJ = true;
        } else {
            cq::logging::info(u8"菜单", u8"Catch_SJJ_Disable");
            agent.En_Catch_SJJ = false;
        }
    } catch (const cq::exception::ApiError &) {
        cq::logging::warning(u8"菜单", u8"Catch_SJJ_Fail");
    }
}

#include <Windows.h>

// 不像 CQ_MAIN，CQ_MENU 可以多次调用来添加多个菜单
CQ_MENU(menu_demo_2) {
    cq::logging::info(u8"菜单", u8"点击了示例菜单2");
    MessageBoxW(nullptr, L"这是一个提示", L"提示", MB_OK | MB_SETFOREGROUND | MB_TASKMODAL | MB_TOPMOST);
}
