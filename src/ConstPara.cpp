#include "./cqsdk/cqsdk.h"
#include <string>
#include <fstream>

using namespace std;

class Const_Para {
    public:
        unsigned long _SHN;
        unsigned long _SJJ;
        unsigned long _SELF;
        Const_Para() {
        }
        void update() {
            fstream file;
            try{
                file.open("d:\\coolq-data\\para.txt", ios::in);
                unsigned long t;
                char str[12];
                while (!file.eof()) {
                    file>>str>>t;
                    if (strcmp(str, "shn")==0) {
                        _SHN = t;
                    } else if (strcmp(str, "sjj")==0) {
                        _SJJ = t;
                    } else if (strcmp(str, "self")==0) {
                        _SELF = t;
                    }
                }
            } catch (const cq::exception::ApiError &) {
                cq::logging::warning(u8"CP_Update", u8"Const_Para_update_failed");
                _SHN = 2411393416;
                _SJJ = 1572264811;
                _SELF = 2467043105;
            }
            file.close();
        }
        void show(const cq::Target &t) {
            string msg = "";
            char num[40];
            ultoa(_SHN, num, 10);
            msg += "SHN: " + string(num) + '\n';
            ultoa(_SJJ, num, 10);
            msg += "SJJ: " + string(num) + '\n';
            ultoa(_SELF, num, 10);
            msg += "SELF: " + string(num) + '\n';
            cq::message::send(t, msg);
        }
};