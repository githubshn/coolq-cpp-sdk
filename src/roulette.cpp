#include<iostream>
#include<stdio.h>
#include<vector>
#include<ctime>
#include<cstring>
#include<stdlib.h>
using namespace std;

class Roulette{
	public:	
		vector<char*> reward;
		vector<double> poss;
		int roll_max;
		Roulette(){
			roll_max = 1000000;
			reward.clear();
			poss.clear();
		}
		int set_reward(int _n, vector<char*> _r, vector<double> _p){
			if (_r.size()<_n || _p.size()<_n) return -1;
			reward.clear();
			poss.clear();
			for(int i=0; i<_n; i++){
				char* tmp = new char(strlen(_r[i])+1);
				strcpy(tmp, _r[i]);
				tmp[strlen(_r[i])] = '\0';
				reward.push_back(tmp);
				poss.push_back(_p[i]);
			}
			return 0;
		}
		int change_poss(int _i, double _p){
			int size = reward.size();
			if (_i>=size) return -1;
			poss[_i] = _p;
			return 0;
		}
		int change_reward(int _i, char* _r){
			int size = reward.size();
			if (_i>=size) return -1;
			char* tmp = new char(strlen(_r)+1);
			tmp[strlen(_r)] = '\0';
			strcpy(tmp, _r);
			reward[_i] = tmp;
			return 0;
		}
		void default_init(){
			vector<char*> r;
			vector<double> p;
			int n = 5;
			r.push_back("N");
			p.push_back(40);
			r.push_back("R");
			p.push_back(30);
			r.push_back("SR");
			p.push_back(20);
			r.push_back("SSR");
			p.push_back(9);
			r.push_back("EX");
			p.push_back(1);
			set_reward(n, r, p);
		}
		void standardize(){
			double sum = 0;
			for(auto i=poss.begin(); i!=poss.end(); i++){
				sum += (*i);
			}
			for(auto i=poss.begin(); i!=poss.end(); i++){
				(*i)/=sum;
			}
		}
		void new_seed(){
			srand((unsigned)time(NULL));
		}
		double roll(){
			return rand() / (double)(RAND_MAX);
		}
		int get_reward(double p){
			int size = poss.size();
			for(int i=0; i<size; i++){
				p-=poss[i];
				if (p<=0){
					return i;
				}
			}
			return size-1;
		}
};

//int main(){
//	Roulette r;
//	r.default_init();
//	r.standardize();
////	cout<<r.roll();
//	for(int i=0; i<100; i++){
//		double t = r.roll();
//		cout<<r.reward[r.get_reward(t)]<<t<<endl;
//	}
//} 
