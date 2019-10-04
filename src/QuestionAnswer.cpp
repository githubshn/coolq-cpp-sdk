#include<iostream>
#include<stdio.h>
#include<vector>
#include<cstring>
#include<fstream>
#include<algorithm>
#include<list>


#define Context_Size 100
using namespace std;

class QA_Node{
	public:
		char* context;
		list<QA_Node*> link;
		bool is_q;
		QA_Node(const char* _c, bool _q):is_q(_q){
			context = new char(strlen(_c)+1);
			for(int i=0; _c[i]; i++){
				context[i] = _c[i];
			}
			context[strlen(_c)] = '\0';
			link.clear();
		} 
};

class Question_Answer {
	char* filename;
	char* tmpfile;
	vector<QA_Node*> list;
	void loading(){
		fstream file;
		file.open(filename, ios::in);
		while(!file.eof()){
			char context[Context_Size];
			int t = -1;
			file.getline(context,Context_Size);
			file>>t;file.get();
//			cout<<context<<t<<endl;
			if (t==-1) break;
			QA_Node* node = find(context);
			if (!node) {
				node = new QA_Node(context, true);
				list.push_back(node);
				sort(list.begin(), list.end(), Question_Answer::QA_Lesser);
			}
			for(int i=0; i<t; i++){
				char answer[Context_Size];
				file.getline(answer, Context_Size);
//				cout<<"ans"<<answer<<endl;
				QA_Node* anode = new QA_Node(answer, false);
				node->link.push_back(anode);
			}
		}
		file.close();
	}
	void save(char* newfile = NULL) {
		char* savefile;
		if(newfile==NULL){
			savefile = filename;
		}else{
			savefile = newfile;
		}
		fstream file;
		file.open(savefile, ios::out);
		for (auto i=list.begin(); i!=list.end(); i++){
			file<<(*i)->context<<"\n";
			file<<(*i)->link.size()<<"\n";
			for (auto j=(*i)->link.begin(); j!=(*i)->link.end(); j++){
				file<<(*j)->context<<"\n";
			}
		}
		file.close();
	}
	QA_Node* find(const char* x){
		int t = 0, w = list.size();
		while(t<w){
			int m = (t+w)>>1;
			int cmpv = strcmp(list[m]->context, x);
			if (cmpv>0){
				w = m;
			}else if (cmpv<0){
				t = m+1;
			}else{
				return list[m];
			}
		}
		return NULL;
	}
	
	public:
		Question_Answer(char* _filename, char* _tmp):filename(_filename), tmpfile(_tmp){
		}
		void init(){
			clear_tmp();
			list.clear();
			loading();
			sort(list.begin(), list.end(), Question_Answer::QA_Lesser);
		}
		QA_Node* response(const char* question){
			QA_Node* q = find(question);
			if (q){
				// for(auto i = q->link.begin(); i!=q->link.end(); i++){
				// 	cout<<(*i)->context<<endl;
				// }
				return q;
			} else {
				return NULL;
			}
		}
		void print(){
			cout<<list.size()<<endl;
			for(auto i=list.begin(); i!=list.end(); i++){
				cout<<(*i)->context<<"\n";
				cout<<(*i)->link.size()<<"\n";
				for (auto j=(*i)->link.begin(); j!=(*i)->link.end(); j++){
					cout<<(*j)->context<<"\n";
				}
				cout<<endl;
			}
		}
		void data_copy(char* newfile = NULL){
			save(newfile);
		}
		void clear_tmp(){
			fstream file;
			file.open(tmpfile, ios::out);
			file.close();
		}
		void teach(const char* question, const char* answer){
			QA_Node* q = find(question);
			if (!q){
				q = new QA_Node(question, true);
				list.push_back(q);
				sort(list.begin(), list.end(), Question_Answer::QA_Lesser);
			}
			QA_Node* a = new QA_Node(answer, false);
			q->link.push_back(a);
			
			fstream file;
			file.open(filename, ios::out|ios::app);
			file<<question<<"\n1\n"<<answer<<"\n";
			file.close();
		}
		int remove(const char* question, const char* answer){
			QA_Node* q = find(question);
			if (!q) return 1;	// no such question
			for(auto i=q->link.begin(); i!=q->link.end(); i++){
				if (strcmp((*i)->context, answer)==0){
					q->link.remove(*i);
					return 0;
				}
			}
			return 2;		// no such answer
		}
		static bool QA_Lesser(const QA_Node* a, const QA_Node* b){
			return strcmp(a->context, b->context)<0;
		}
//		void merge(char* file1, char* file2){
//			fstream file1, file2;
//			file1.open(file1, ios::out|ios::app);
//			file2.open(file2, ioe::in);
//			while(!file2.eof()){
//				char context[Context_Size];
//				file2.getline(context, Context_Size);
//				if (strlen(context)>0){
//					file1<<context<<"\n";
//				}
//			}
//			file1.close();
//			file2.close();
//		}
}; 

// int main() {
// 	Question_Answer qa("D:\\coolq-data\\QA_data.txt", "D:\\coolq-data\\QA_data_tmp.txt");
// 	qa.init();
// 	qa.print();
// 	qa.response("b");
// 	qa.teach("b", "be careful");
// 	qa.teach("c", "call");
// 	qa.response("b");
// 	qa.response("qwr");
// 	qa.print();
// 	qa.data_copy("D:\\coolq-data\\new_QA_data.txt");
// //	Question_Answer::merge("D:\\coolq-data\\QA_data2.txt", "D:\\coolq-data\\QA_data_tmp.txt");
// } 
