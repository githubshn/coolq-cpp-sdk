#include<thread>
#include<iostream>
#include<chrono>
#include<ctime>
using namespace std;
class Timer{
	bool is_clear;
	clock_t start_time, end_time;
	public:
		Timer(){
			is_clear = false;
		}
		void setTimeout(auto func, int delay){
			is_clear = false;
			thread t([=] () {
				if (is_clear) return;
				std::this_thread::sleep_for(std::chrono::milliseconds(delay));
				if (is_clear) return;
				func();			
			});
			t.detach();
		}
		void setInterval(auto func, int interval, int times = -1){
			is_clear = false;
			thread t([=] () {
				int t_times = times;
				while (t_times==-1 || t_times>0) {
					if (is_clear) return;
					std::this_thread::sleep_for(std::chrono::milliseconds(interval));
					if (is_clear) return;
					func();
					if (t_times>0) t_times--;
				}
			});
			t.detach();
		}
		void reset(){
		}
		void clear(){
			is_clear = true;
		}
};

//void print(){
//	cout<<"interval\n";
//}
//
//int main(){
//	Timer timer;
//	timer.setInterval(print, 500, 10);
//	while(true){
//		
//	}
//}
