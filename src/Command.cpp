#include<iostream>
#include<stdio.h>
#include<string>
#include<cstring>
#include<vector>

using namespace std;

class Command{
	// return: 
	// 	0 : 	Success
	// 	-1:		Quotation marks mismatch
	// 	-2:		Remain commands
	bool is_arg_letter(char x, bool in_quo = false){
		if (in_quo){
			return (x!='\"');
		}else{
			return (x!=' ');
		}
	}
	bool is_flag_letter(char x){
		if ((x>='a'&&x<='z')||(x>='A'&&x<='Z')||(x>='0'&&x<='9')) return true;
		return false;
	}
	public:
		char* command;
		vector<char*> flags;
		int flags_c;
		vector<char*> args;
		int args_c;
		Command(){
			clear();
		}
		Command(const char* text){
			clear();
			analyze(text);
		}
		void clear(){
			flags.clear();
			flags_c = 0;
			args.clear();
			args_c = 0;
		}
		int analyze(const char* text){
			int i=0;
			while(text[i] && text[i]!=' '){
				i++;
			}
			command = new char(i+1);
			for(int j=0; j<i; j++){
				command[j] = text[j];
			}
			command[i] = '\0';
			// cout<<"command: "<<command<<endl;
			while(text[i] && text[i]==' ') i++;

			if (text[i] && text[i]=='-'){
				// have flags
				while(text[i] && text[i]=='-'){
					i++;
					int ii = i;
					while(text[i] && is_flag_letter(text[i])) i++;
					char* n_flag = new char(i-ii+1);
					for(int j=ii; j<i; j++){
						n_flag[j-ii] = text[j];
					}
					n_flag[i-ii] = '\0';
					flags.push_back(n_flag);
					flags_c++;
					while(text[i] && text[i]==' ') i++;
				}
			}
//			cout<<"now i = "<<i<<endl;
			if (text[i] && is_arg_letter(text[i])) {
				// have args
				while(text[i] && is_arg_letter(text[i])){
//					cout<<"now i= "<<i<<"\""<<text[i]<<"\""<<endl;
					bool in_quo = false;
					if (text[i]=='\"'){
						in_quo = true;
						i++;
					}
					int ii = i;
					int jj;
					while(text[i] && is_arg_letter(text[i], in_quo)) i++;
					if (in_quo){
						if (text[i] && text[i]=='\"'){
							in_quo = false;
						}else{
							// cout<<"error: Quotation marks mismatch\n";
							return -1;
						}
						jj = i;
						i++;
					} else {
						jj = i;
					}
					char* n_arg = new char(jj-ii+1);
					for(int j=ii; j<jj; j++){
						n_arg[j-ii] = text[j];
					}
					n_arg[jj-ii] = '\0';
					args.push_back(n_arg);
					args_c++;
					while(text[i] && text[i]==' ') i++;
				}
			}
			if (text[i]){
				// cout<<"remain command:\n";
				while(text[i]){
					cout<<text[i];
					i++;
				}
				cout<<endl;
				return -2;
			}
			// for(auto i=flags.begin(); i!=flags.end(); i++){
			// 	cout<<"flags: "<<(*i)<<endl;
			// }
			// for(auto i=args.begin(); i!=args.end(); i++){
			// 	cout<<"args: "<<(*i)<<endl;
			// }
			// cout<<"analyzing finished successfully\n";
			return 0;
		}
};

// int main(){
// 	Command c;
// 	c.analyze(("t -m -pq -ls area wer \"awef uhuhwif -afew\" a \""));
// }
