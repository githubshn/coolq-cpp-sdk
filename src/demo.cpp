#include "cqsdk/cqsdk.h"
#include<string>
#include<cstring>
#include<ctime>
#include<fstream>
#include<time.h>
//  My Classes
#include "NumberGuess.cpp"
#include "ConstPara.cpp"
#include "Command.cpp"
#include "QuestionAnswer.cpp"
#include "MyTimer.cpp"
#include "roulette.cpp"

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
        // vector<Alarm_Clock*>  

        Const_Para *cp;
        Question_Answer *qa;
        Roulette * roulette;

        Robot(){
        }
        template<typename T>
        int my_switch(Command &c, const T &e){
            bool en = false;
            for(auto i=c.flags.begin(); i!=c.flags.end(); i++){
                // debug_print((*i));
                if (strcmp((*i), "enable")==0){
                    en = true;
                } else if (strcmp((*i), "disable")==0){
                    en = false;
                } else {
                    return -10001;  // unknown flags
                }
            }
            if (c.args_c<1){
                return -10002;
            }
            for(auto i=c.args.begin(); i!=c.args.end(); i++){
                if (strcmp((*i), "Catch_SJJ")==0){
                    if (en){
                        En_Catch_SJJ = true;
                        cq::message::send(e.target, "捕捉四季功能已打开");
                    } else {
                        En_Catch_SJJ = false;
                        cq::message::send(e.target, "捕捉四季功能已关闭");
                    }
                } else {
                    string msg(*i);
                    cq::message::send(e.target, "Unknown Switch: " + msg);
                }
            }
            return 0;
        }
        template<typename T>
        int set_alarm_clock(Command &c, const T &e){
            for(auto i=c.flags.begin(); i!=c.flags.end(); i++){
                return -10001;
            }
            if (c.args_c<1) return -10002;
            if (c.args_c>1) return -10003;
            
            return 0;
        }
        void init(Const_Para* _cp, Question_Answer* _qa, Roulette* _roulette){
            debug_print("agent init");
            cp = _cp;
            qa = _qa;
            roulette = _roulette;
            roulette->default_init();
            roulette->standardize();
            roulette->new_seed();
            En_Number_Guess = false;
            En_Catch_SJJ = true;
            Number_Guess_ing = NULL;
        }
        void qa_response(string strmsg, const cq::GroupMessageEvent &e){
            QA_Node* qa_response = qa->response(strmsg.c_str());
            if (qa_response){
                srand((unsigned)time(0));
                int tot = qa_response->link.size();
                debug_print(tot);
                if (tot==0) return;
                int ran = rand()%tot;
                debug_print(ran);
                auto i = qa_response->link.begin();
                for(; ran!=0 && i!=qa_response->link.end(); ran--,i++);
                if (i!=qa_response->link.end()){
                    cq::message::send(e.target, (*i)->context);
                }
            }
        }
        int qa_print_question(QA_Node* q, const cq::GroupMessageEvent &e){
            if (!q) return 1;
            string msg = "";   //  No such question
            int order = 1;
            for(auto i=q->link.begin(); i!=q->link.end(); i++, order++){
                msg += string((*i)->context) + "\n";
            }
            cq::message::send(e.target, msg);
            return 0;
        }
        int qa_teaching(Command &c, const cq::GroupMessageEvent &e){
            for (auto i=c.flags.begin(); i!=c.flags.end(); i++){
                if (strcmp((*i), "s")==0){
                    qa->data_copy();
                    cq::message::send(e.target, "已保存");
                    qa->clear_tmp();
                    return 0;
                } else if (strcmp((*i), "update")==0){
                    qa->init();
                    cq::message::send(e.target, "更新完成");
                    return 0;
                } else if (strcmp((*i), "d")==0){
                    if (c.args_c<2) {
                        return -10002;  // too few args
                    } else if (c.args_c>2) {
                        return -10003; // too many args
                    }
                    int ret = qa->remove(c.args[0], c.args[1]);
                    if (ret==1) return -10004;
                    if (ret==2) return -10005;
                    return 0;
                } else if (strcmp((*i), "fq")==0){
                    int ret = qa_print_question(qa->response(c.args[0]), e);
                    if(ret==1) return -10004;
                    return 0;
                }
                return -10001;  // unknown flags
            }
            if (c.args_c<2) {
                return -10002;  // too few args
            } else if (c.args_c>2) {
                return -10003; // too many args
            }
            qa->teach(c.args[0], c.args[1]);
            cq::message::send(e.target, "学会啦！");
            return 0;
        }
        int my_roulette(Command &c, const cq::GroupMessageEvent &e){
            int times = 1;
            bool debug = false;
            for (auto i=c.flags.begin(); i!=c.flags.end(); i++){
                if (strcmp((*i), "10")==0){
                    times = 10;
                } else if (strcmp((*i), "d")==0) {
                    debug = true;
                } else {
                    return -10001;
                }
            }
            if (c.args_c>0) {
                return -10003;
            }
            string msg = "";
            for(; times>0; times--){
                double r = roulette->roll();
                string strreward = roulette->reward[roulette->get_reward(r)];
                if (debug) {
                    debug_print((int)(r*10000));
                }
                msg += "获得了: " + strreward + "\n";
            }
            cq::message::send(e.target, msg);
            return 0;
        }
        int execute_command(Command &c, const cq::GroupMessageEvent &e){
            if (strcmp(c.command, "constpara")==0 && e.user_id==cp->_SHN){
                cp->show(e.target);
            } else if(strcmp(c.command, "teach")==0){
                return qa_teaching(c, e);
            }else if (strcmp(c.command, "repeat")==0){
                return 10000;
            }else if (strcmp(c.command, "switch")==0 && e.user_id==cp->_SHN){
                return my_switch(c, e);
            }else if (strcmp(c.command, "set_alarm_clock")==0 && e.user_id==cp->_SHN){
                return set_alarm_clock(c, e);
            }else if (strcmp(c.command, "掷骰子")==0){
                return my_roulette(c, e);
            }else {
                return 10000;//未知命令当复读
            }
        }
        void response_to_shn(const cq::GroupMessageEvent &e) {
            cq::Message msg = e.message;
            string strmsg = to_string(msg);
            cq::logging::debug("shn", "next");
            if (strmsg.find("小鸭子，")==0) {
                cq::logging::debug("shn", "repeat");
                strmsg = strmsg.substr(strlen("小鸭子，"));
                Command c(strmsg.c_str());
                int ret = execute_command(c, e);
                switch(ret){
                    case 0:
                        // success
                        break;
                    case -1:
                        cq::message::send(e.target, "未知命令");
                        break;
                    case -10001:
                        cq::message::send(e.target, "未知开关");
                        break;
                    case -10002:
                        cq::message::send(e.target, "缺少参数");
                        break;
                    case -10003:
                        cq::message::send(e.target, "过多参数");
                        break;
                    case -10004:
                        cq::message::send(e.target, "未找到问题");
                        break;
                    case -10005:
                        cq::message::send(e.target, "未找到回答");
                        break;
                    case 10000:
                        cq::message::send(e.target, strmsg);
                }
            } else {
                qa_response(strmsg, e);
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
                } else if (strmsg.find("CQ:image,file=106F2925C7AF2EF4330169B143A89EF1.png")!=strmsg.npos) {
                    msg = "戳！";
                    cq::message::send(e.target, msg);
                } else if (strmsg.find("嗯？怎么冷群了？")!=strmsg.npos) {
                    msg = "只剩下AI了，悲惨吧。。。";
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
Const_Para cp;
Question_Answer qa("D:\\coolq-data\\QA_data.txt", "D:\\coolq-data\\QA_data_tmp.txt");
Roulette rl;

// 插件入口，在静态成员初始化之后，app::on_initialize 事件发生之前被执行，用于配置 SDK 和注册事件回调
CQ_MAIN {
    cq::app::on_enable = [] {
        // cq::logging、cq::api、cq::dir 等命名空间下的函数只能在事件回调函数内部调用，而不能直接在 CQ_MAIN 中调用
        debug_print("The plug-in components is enabled!");
        // cq::logging::debug(u8"启用", u8"插件已启动");
        agent.init(&cp, &qa, &rl);
        cp.update();
        qa.init();
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
        if (e.user_id==cp._SJJ){
            cq::logging::debug("sjj", "response to SJJ");
            // cq::message::send(e.target, msg);
            agent.response_to_sjj(e);
            return;
        } else {//if (e.user_id==cp._SHN){
            agent.response_to_shn(e);
            return;
        }
        agent.qa_response(to_string(e.message), e);
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
