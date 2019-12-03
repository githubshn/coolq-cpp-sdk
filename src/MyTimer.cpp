#include<thread>
#include<iostream>
#include <iomanip>
#include<chrono>
#include<ctime>
#include<vector>
using namespace std;
class Timer{
	bool is_clear;
	clock_t start_time, end_time;
	public:
		Timer(){
			is_clear = false;
		}
		template<typename T>
		void setAlarmClock(T func, tm *ptm){
//			cout<<ptm<<endl;
			is_clear = false;
//			printf("in timer: %d/%d/%d/%d:%d:%d\n", 1900 + ptm->tm_year, 1 + ptm->tm_mon, ptm->tm_mday, ptm->tm_hour, ptm->tm_min, ptm->tm_sec);
			thread t([=] () {
				if (is_clear) return;
//				printf("in thread: %d/%d/%d/%d:%d:%d\n", 1900 + ptm->tm_year, 1 + ptm->tm_mon, ptm->tm_mday, ptm->tm_hour, ptm->tm_min, ptm->tm_sec);
				std::this_thread::sleep_until(chrono::system_clock::from_time_t (mktime(ptm)));
				if (is_clear) return;
				func();			
			});
			t.detach();
		}
		template<typename T>
		void setTimeout(T func, int delay){
			is_clear = false;
			thread t([=] () {
				if (is_clear) return;
				std::this_thread::sleep_for(std::chrono::milliseconds(delay));
				if (is_clear) return;
				func();			
			});
			t.detach();
		}
		template<typename T>
		void setInterval(T func, int interval, int times = -1){
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
class ClockDetail{
	public:
		time_t start_time;
		bool is_running;
		int interval;
		int times;
		char* details;
		ClockDetail(){
			is_running = true;
			interval = -1;
			times = 0;
			details = NULL;
		}
};
class AlarmClock{
	vector<Timer*> clocks;
	vector<ClockDetail*> details;
//	template<typename T>
//	void on_destroy(T func, int id){
//		func();
//		clear_clock(id);
//	}
	public:
		AlarmClock(){
		}
		int get_free_clock(){
			for(int i=0; i<clocks.size(); i++){
				if (clocks[i]==NULL) return i;
			}
			Timer *timer = NULL;
			clocks.push_back(timer);
			details.push_back(new ClockDetail());
			return clocks.size()-1;
		}
		int clear_clock(int id){
			if (id<clocks.size()-1){
				if (clocks[id]!=NULL){
					delete(clocks[id]);
					clocks[id] = NULL;
					return 0;
				} else {
					return -2;	// clock is already clear
				}
			} else {
				return -1;		// no such clock
			}
		}
		static void copy_tm(tm* p1, tm* p2){
			p2->tm_hour = p1->tm_hour;
			p2->tm_min  = p1->tm_min;
			p2->tm_sec  = p1->tm_sec;
			p2->tm_mday = p1->tm_mday;
			p2->tm_mon  = p1->tm_mon;
			p2->tm_year = p1->tm_year;
		}
		tm* set_time(int tm_sec, int tm_min, int tm_hour, int tm_mday = -1, int tm_month = -1, int tm_year = -1){
			time_t ptime;
			time(&ptime);
			tm *ptm_tmp = localtime(&ptime);
			tm* ptm = new tm();
			copy_tm(ptm_tmp, ptm); 
			
			ptm->tm_sec = tm_sec;
			ptm->tm_min = tm_min;
			ptm->tm_hour = tm_hour;
			if (tm_mday!=-1) ptm->tm_mday = tm_mday;
			if (tm_month!=-1) ptm->tm_mon = tm_month;
			if (tm_year!=-1) ptm->tm_year = tm_year;
			return ptm;
		}
		template<typename T>
		int setOnceClock(T func, tm* ptm){
			int id = get_free_clock();
			Timer *timer = clocks[id];
			timer = new Timer();
			timer->setAlarmClock(func, ptm);
			return id;
		}
		template<typename T>
		int setIntervalClock(T func, tm* ptm, int interval, int times = -1){
			int id = get_free_clock();
			Timer *timer = clocks[id];
			timer = new Timer();
			while(difftime(mktime(ptm), time(NULL))<0){
//				printf("%d/%d/%d/%d:%d:%d\n\n", 1900 + ptm->tm_year, 1 + ptm->tm_mon, ptm->tm_mday, ptm->tm_hour, ptm->tm_min, ptm->tm_sec);
				ptm->tm_sec += interval/1000;
			}
//			printf("%d/%d/%d/%d:%d:%d\n\n", 1900 + ptm->tm_year, 1 + ptm->tm_mon, ptm->tm_mday, ptm->tm_hour, ptm->tm_min, ptm->tm_sec);
			timer->setAlarmClock([=](){
				func();
				timer->setInterval(func, interval, times);
			}, ptm);
			return id;
		}
};

// void print(){
// 	cout<<"time_out\n";
// 	time_t ptime;
// 	time(&ptime);
// 	tm *ptm = localtime(&ptime);
// 	printf("%d/%d/%d/%d:%d:%d\n\n", 1900 + ptm->tm_year, 1 + ptm->tm_mon, ptm->tm_mday, ptm->tm_hour, ptm->tm_min, ptm->tm_sec);
// }
//
// int main(){
// 	AlarmClock ac;
// //	ac.setOnceClock(print, ac.set_time(0,47,18));
// 	ac.setIntervalClock(print, ac.set_time(0, 0, 19), 60*1000);
// //	cout<<difftime(time(NULL), mktime(ac.set_time(0,1,19)));
// 	while(true){
// //		time(&ptime);
// //		tm *ptm = localtime(&ptime);
// //		printf("%d/%d/%d/%d:%d:%d\n", 1900 + ptm->tm_year, 1 + ptm->tm_mon, ptm->tm_mday, ptm->tm_hour, ptm->tm_min, ptm->tm_sec);
// 	}
// //	time(&ptime);
// //	ptm = localtime(&ptime);
// //	printf("%d/%d/%d/%d:%d:%d\n", 1900 + ptm->tm_year, 1 + ptm->tm_mon, ptm->tm_mday, ptm->tm_hour, ptm->tm_min, ptm->tm_sec);
// }




