#include<iostream>
#include<vector>
#include<list>
#include<stdlib.h>
#define Default_Chances 10

using namespace std;

class Guess{
	public:
		char guess[10];
		int a,b;
		Guess(char* s_guess, int s_a, int s_b):a(s_a),b(s_a){
			for(int i=0; s_guess[i]; i++){
				guess[i] = s_guess[i];
			}
		};
		Guess() {
			a = 0;
			b = 0;
		}
};

class Number_Guess {
	private:
		int n,m;
		int guess_num;
		Guess guess[Default_Chances];
		list <int> poss;
		void guess_init(){
			guess_num = 0;
			poss.clear();
		}
		bool suit_rulers(int x) {
			int t = x;
			char s[10] = "000000000";
			int j = 0;
			while(t!=0){
				s[n-1-j] = char(t % 10 + 48);
				t /= 10;
				j++;
			}
			for(int i=0; i<n-1; i++){
				for(int j=i+1; j<n;j++){
					if(s[i]==s[j]) return false;
				}
			}
			for(int i=0; i<guess_num; i++){
				int a = 0, b = 0;
				for(int ii=0; ii<n; ii++){
					a += (s[ii]==guess[i].guess[ii]);
					for(int jj=0; jj<n; jj++){
						b += (s[ii]==guess[i].guess[jj] && ii!=jj);
					}
				}
				if (a!=guess[i].a || b!=guess[i].b) {
					return false;
				}
			}
			return true;
		}
		void add_poss() {
			for(int i=0; i<10000; i++){
				if (suit_rulers(i)) poss.push_back(i);
			}
		}
		bool removeable(const int& x){
			return suit_rulers(x);
		}
	public:
		Number_Guess(int s_n, int s_m):n(s_n),m(s_m) {
			guess_init();
		}
		Number_Guess() {
			n = 4;
			m = 10;
			guess_init();
		}
		void add_rulers(char* _guess, int _a, int _b) {
			for(int i=0; _guess[i]; i++){
				guess[guess_num].guess[i] = _guess[i];
			}
			guess[guess_num].a = _a;
			guess[guess_num].b = _b;
			guess_num++;
		}
		int get_suggestion(){
//			if(poss.size()==0){
//				add_poss();
//			}else{
//				poss.remove_if(Number_Guess::removeable);
//			}
			poss.clear();
			add_poss();
			// for(auto i=poss.begin(); i!=poss.end(); i++){
			// 	cout<<(*i)<<endl;
			// }
			if(poss.size()>0){
				return *(poss.begin());
			}else{
				return -1;
			}
		}
		void print_rulers(){
			for(int i=0; i<guess_num; i++){
				cout<<"guess: "<<guess[i].guess<<" "<<guess[i].a<<"A"<<guess[i].b<<"B\n";
			}
		}
		int get_times(){
			return guess_num;
		}
};

// int main(){
// 	int n,m;
// 	// cin>>n>>m;
// 	Number_Guess test;
// 	while(true){
// 		char t[4];
// 		int a,b;
// 		int x;
// 		cin>>t>>a>>b;
// 		// t[0] = x/1000;
// 		// t[1] = (x/100) % 10;
// 		// t[2] = (x/10) % 10;
// 		// t[3] = x % 10;
// 		if (a==-1) {
// 			test.print_rulers();
// 		}else {
// 			test.add_rulers(t, a, b);
// 			cout<<"suggestion: "<<test.get_suggestion();
// 		}
// 		system("pause");
// 	}
// 	return 0;
// } 
