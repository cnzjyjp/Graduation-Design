//myopencl.h
#ifndef myopencl_h
#define myopencl_h
#include <cstdio>
#include <iostream>
#include <string.h>
#include <string>
#include <algorithm>
#include <cstdlib>
#include <time.h> 
using namespace std;	//不支持多个 OpenCL 源程序同时引用 myopencl.h 联合编译
#define CL_SUCCESS 0
#define CL_CONTEXT_PLATFORM 1
#define CL_DEVICE_TYPE_ALL 0
#define CL_DEVICE_TYPE_CPU 1
#define CL_DEVICE_TYPE_GPU 2
#define CL_DEVICE_TYPE_ACCELERATOR 3
#define CL_DEVICE_TYPE_DEFAULT 4
#define CL_CONTEXT_DEVICES 1
#define CL_QUEUE_OUT_OF_ORDER_EXEC_MODE_ENABLE 1
#define CL_QUEUE_PROFILING_ENABLE 0 
#define CL_MEM_COPY_HOST_PTR 0
#define CL_MEM_READ_ONLY 0
#define CL_MEM_WRITE_ONLY 1
#define CL_MEM_READ_WRITE 2
#define CL_TRUE 1
#define CL_FALSE 0
#define ARG_KERNEL_TYPE_ADD &typeadd
#define ARG_KERNEL_TYPE_SUB &typesub
#define ARG_KERNEL_TYPE_MUL &typemul
#define ARG_KERNEL_TYPE_DIV &typediv
#define ARG_KERNEL_TYPE_ASSIGN &typeassign
double typeadd=1;
double typesub=1;
double typemul=10;
double typediv=41;
double typeassign=1;
typedef int* cl_context;
struct cq{
	int num;
	int property;//0有序1乱序 
};
typedef cq* cl_command_queue;
typedef int* cl_program;
typedef int cl_device_id;
typedef string* cl_kernel;
typedef int cl_int;
typedef int cl_uint;
typedef int cl_platform_id;
typedef int cl_context_properties;
typedef int cl_device_id;
typedef double cl_mem;
typedef int cl_event;

#define groupsize 2000
#define passsize 1000
#define epoch 10000
#define maxsize 20
#define maxarg 10

struct templist{
	string name;
	double arg[maxarg];
};
templist tlist[maxsize*2];

struct queueeventlist{
	int queuenum;
	int property;
	string list[maxsize];
};
queueeventlist qelist[maxsize];

struct eventlist{
	string name;
	double size;
	double time;
};
eventlist elist[maxsize];

struct nulllist{
	string name;
	double size;
	double time;
};
nulllist nlist[maxsize];

int eventgraph[maxsize][maxsize];
int eventgraph2[maxsize][maxsize];
int levelgraph[maxsize][maxsize];
int totaltemp=0;
int totalcq=0;
int totalnull=0;
int totallevel=0;

cl_event event[maxsize];

struct priority{
	int num;
	double weight;
};
struct generation{
	double maxt;
	int plan[maxsize];
};
generation group[groupsize];
generation nextg[groupsize];

int solution[maxsize]; 
double readytime[maxsize];
double endtime[maxsize];
double maxtime=0;
double device[maxsize];
int devicenum;

int sp=15;		//交换概率 (/100) 
int mp=1;		//突变概率 (/200)

int* clCreateContextFromType(int* a,int b,int* c,void* d,int* e){
	return NULL;
}

int clGetContextInfo(int* a,int b,size_t c,int* d,size_t* e){
	if(e!=NULL){
		*e=1;
	}
	return 0;
}

cq* clCreateCommandQueue(int* a,int b,int c,int* d){
	cq *temp=new cq;
	temp->num=totalcq;
	temp->property=c; 
	totalcq++;
	return temp;
}

int* clCreateProgramWithSource(int* a, int b,const char**c,const size_t* d, int* e){
	
	return NULL;
}

int clBuildProgram(int* a, int b, const int* c, const char* d, int* e, void* f){
	return 0;
}

double clCreateBuffer(int* a, int b, size_t c, void* d, int* e){
	return (double)c;
}

int clReleaseMemObject(double a){
	return 0;
}

int clReleaseCommandQueue(cq* a){
	return 0;
}

int clReleaseKernel(string* a){
	return 0;
}

int clReleaseProgram(int* a){
	return 0;
}

int clReleaseContext(int* a){
	return 0;
}

string* clCreateKernel(int* a,string s,int* b){
	string* temp=new string;
	*temp=s;
	tlist[totaltemp].name=s;
	totaltemp++;
	return temp;
}

int clSetKernelArg(string* a,int b,size_t c,const void* d){
	if(d==NULL) {cout<<"No such MemObject!"<<endl;return 1;}
	double* temp;
	temp=(double*)d;
	int i,j;
	for(i=0;i<maxsize*2;i++){
		if(tlist[i].name==*a){
			for(j=0;j<maxarg;j++){
				if(tlist[i].arg[j]==-1) {tlist[i].arg[j]=*temp;break;}
			}
			break;
		}
	}
	if(j==maxarg){cout<<*a<<":No more space for arg!"<<endl;return 1;}
	if(i==maxsize*2){cout<<*a<<":No such kernel created!"<<endl;return 2;}
	return 0;
}

int clEnqueueTask(cq* a, string* b, int c,const int* d,int* num){
	int index1,index2;
	if(num!=NULL){
		for(int i=0;i<maxsize;i++){
			if(qelist[i].queuenum==a->num){
				for(int j=0;i<maxsize;j++){
					if(qelist[i].list[j]==""){
						qelist[i].list[j]=*b;
						break;
					}
				}
				break;
			}
			else if(qelist[i].queuenum==-1){
				qelist[i].queuenum=a->num;
				qelist[i].property=a->property;
				qelist[i].list[0]=*b;
				break;
			}
		}
	}
	if(num==NULL) {
		nlist[totalnull].name=*b;
		int i,j;
		double ksize;
		for(i=0;i<maxsize*2;i++){
			if(tlist[i].name==*b) break;
		}
		for(j=0;j<maxarg;j++){
			if(tlist[i].arg[j]==-1) break;
		}
		if(j<2) {
			cout<<*b<<":未定义内核规模,请输入主要计算类型(ADD/SUB/MUL/DIV/ASSIGN)"<<endl;
			string type;
			cin>>type;
			transform(type.begin(), type.end(), type.begin(), ::toupper);
			double t;
			if (type.find("ADD")!=string::npos)	{t=1;}
			else if (type.find("SUB")!=string::npos) {t=1;}
			else if (type.find("MUL")!=string::npos) {t=10;}
			else if (type.find("DIV")!=string::npos) {t=41;}
			else if (type.find("ASSIGN")!=string::npos) {t=1;}
			cout<<"输入该计算的重复次数"<<endl;
			double repeat;
			scanf("%lf",&repeat);
			ksize=repeat*t;
			}
		else ksize=tlist[i].arg[j-1]*tlist[i].arg[j-2];
		nlist[totalnull].size=ksize;
		totalnull++;
	}
	else{
		index1=num-event;
		if(index1>=maxsize){printf("maxsize is too small!\n");return 1;}
		elist[index1].name=*b;
		int i,j;
		for(i=0;i<maxsize*2;i++){
			if(tlist[i].name==*b) break;
		}
		double ksize;
		for(j=0;j<maxarg;j++){
			if(tlist[i].arg[j]==-1) break;
		}
		if(j<2) {
			cout<<*b<<":未定义内核规模,请输入主要计算类型(ADD/SUB/MUL/DIV/ASSIGN)"<<endl;
			string type;
			cin>>type;
			transform(type.begin(), type.end(), type.begin(), ::toupper);
			double t;
			if (type.find("ADD")!=string::npos)	{t=1;}
			else if (type.find("SUB")!=string::npos) {t=1;}
			else if (type.find("MUL")!=string::npos) {t=10;}
			else if (type.find("DIV")!=string::npos) {t=41;}
			else if (type.find("ASSIGN")!=string::npos) {t=1;}
			cout<<"输入该计算的重复次数"<<endl;
			double repeat;
			scanf("%lf",&repeat);
			ksize=repeat*t;
			}
		else ksize=tlist[i].arg[j-1]*tlist[i].arg[j-2];
		elist[index1].size=ksize;
	}
	if(c!=0){
		index2=d-event;
		for(int i=0;i<c;i++){
			if(index2+i>=maxsize) break;
			if(elist[index2+i].name=="") {cout<<elist[index1].name<<":存在前驱结点未定义的情况,可能导致闭环!"<<endl;exit(1);}
			eventgraph[index2+i][index1]=1;
		}
	}
	return 0;
}

int clEnqueueNDRangeKernel(cq* a, string* b, int c,const size_t* d,const size_t* e,const size_t* f,int g,const int* h,int* num){
	int index1,index2;
	if(num!=NULL){
		for(int i=0;i<maxsize;i++){
			if(qelist[i].queuenum==a->num){
				for(int j=0;i<maxsize;j++){
					if(qelist[i].list[j]==""){
						qelist[i].list[j]=*b;
						break;
					}
				}
				break;
			}
			else if(qelist[i].queuenum==-1){
				qelist[i].queuenum=a->num;
				qelist[i].property=a->property;
				qelist[i].list[0]=*b;
				break;
			}
		}
	}
	if(num==NULL) {
		nlist[totalnull].name=*b;
		int i,j;
		for(i=0;i<maxsize*2;i++){
			if(tlist[i].name==*b) break;
		}
		double ksize;
		for(j=0;j<maxarg;j++){
			if(tlist[i].arg[j]==-1) break;
		}
		if(j<2) {
			cout<<*b<<":未定义内核规模,请输入主要计算类型(ADD/SUB/MUL/DIV/ASSIGN)"<<endl;
			string type;
			cin>>type;
			transform(type.begin(), type.end(), type.begin(), ::toupper);
			double t;
			if (type.find("ADD")!=string::npos)	{t=1;}
			else if (type.find("SUB")!=string::npos) {t=1;}
			else if (type.find("MUL")!=string::npos) {t=10;}
			else if (type.find("DIV")!=string::npos) {t=41;}
			else if (type.find("ASSIGN")!=string::npos) {t=1;}
			cout<<"输入该计算的重复次数"<<endl;
			double repeat;
			scanf("%lf",&repeat);
			ksize=repeat*t;
			}
		else ksize=tlist[i].arg[j-1]*tlist[i].arg[j-2];
		nlist[totalnull].size=ksize;
		totalnull++;
	}
	else{
		index1=num-event;
		if(index1>=maxsize){printf("maxsize is too small!\n");return 1;}
		elist[index1].name=*b;
		int i,j;
		for(i=0;i<maxsize*2;i++){
			if(tlist[i].name==*b) break;
		}
		double ksize;
		for(j=0;j<maxarg;j++){
			if(tlist[i].arg[j]==-1) break;
		}
		if(j<2) {
			cout<<*b<<":未定义内核规模,请输入主要计算类型(ADD/SUB/MUL/DIV/ASSIGN)"<<endl;
			string type;
			cin>>type;
			transform(type.begin(), type.end(), type.begin(), ::toupper);
			double t;
			if (type.find("ADD")!=string::npos)	{t=1;}
			else if (type.find("SUB")!=string::npos) {t=1;}
			else if (type.find("MUL")!=string::npos) {t=10;}
			else if (type.find("DIV")!=string::npos) {t=41;}
			else if (type.find("ASSIGN")!=string::npos) {t=1;}
			cout<<"输入该计算的重复次数"<<endl;
			double repeat;
			scanf("%lf",&repeat);
			ksize=repeat*t;
			}
		else ksize=tlist[i].arg[j-1]*tlist[i].arg[j-2];
		elist[index1].size=ksize;
	}
	if(g!=0){
		index2=h-event;
		for(int i=0;i<g;i++){
			if(elist[index2+i].name=="") {cout<<elist[index1].name<<":存在前驱结点未定义的情况,可能导致闭环!"<<endl;exit(1);}
			if(index2+i>=maxsize) break;
			eventgraph[index2+i][index1]=1;
		}
	}
	return 0;
}

int clEnqueueBarrier(cq* a){
	for(int i=0;i<maxsize;i++){
		if(qelist[i].queuenum==a->num){
			if(qelist[i].property==0){break;}
			for(int j=0;i<maxsize;j++){
				if(qelist[i].list[j]==""&&j==0){break;}
				else if(qelist[i].list[j]==""){
					qelist[i].property=2;
					qelist[i].list[j]="commandqueuebarrier";
					break;	
				}
			}
			break;
		}
	}
	return 0;
}

int clFinish(cq* a){
	for(int i=0;i<maxsize;i++){
		if(qelist[i].queuenum==a->num){
			if(qelist[i].property==0){break;}
			for(int j=0;i<maxsize;j++){
				if(qelist[i].list[j]==""&&j==0){break;}
				else if(qelist[i].list[j]==""){
					qelist[i].property=2;
					qelist[i].list[j]="commandqueuebarrier";
					break;	
				}
			}
			break;
		}
	}
	return 0;
}

int clFlush(cq* a){
	return 0;
}

int clEnqueueReadBuffer(cq* a, double b, int c, size_t d, size_t e, void* f,int g,const int* h,int* i){
	return 0;
}

int clGetPlatformIDs(int a,int* b,int* c){
	*b=1;
	*c=1;
	return 0;
}

bool cmp(generation a,generation b){
	return a.maxt<b.maxt;
}

bool cmp2(priority a,priority b){
	if(a.weight!=b.weight){return a.weight<b.weight;}
	else return elist[a.num].time<elist[b.num].time;
}

bool cmp3(queueeventlist a,queueeventlist b){
	return a.queuenum<b.queuenum;
}

void outputgraph(){
	int i,j;
	int flag;
	totallevel=0;
	int level[maxsize];
	int table[maxsize];
	fill(table,table+maxsize,1);
	for(i=0;i<maxsize;i++){
		if(elist[i].name==""){table[i]=0;}
	}
	for(i=0;i<maxsize;i++){
		for(j=0;j<maxsize;j++){
			eventgraph2[i][j]=eventgraph[i][j];
		}
	}
	while(1){
		fill(level,level+maxsize,0);
		for(i=0;i<maxsize;i++){
			flag=0;
			if(elist[i].name!=""&&table[i]==1){
				for(j=0;j<maxsize;j++){
					if(eventgraph[j][i]!=0){flag=1;}
				}
				if(flag==0) {level[i]=1;}
			}
		}		
		for(i=0;i<maxsize;i++){
			if(level[i]==1) {
				levelgraph[totallevel][i]=1;
				cout<<elist[i].name<<" ";
				table[i]=0;
			}
		}
		printf("\t");
		for(i=0;i<maxsize;i++){
			if(level[i]==1){
				for(j=0;j<maxsize;j++){
					if(eventgraph[i][j]==1){
						cout<<elist[i].name<<"->"<<elist[j].name<<" ";
						eventgraph[i][j]=0;
					}
				}
			}
		}
		printf("\n");
		totallevel++;
		flag=0;
		for(i=0;i<maxsize;i++){
			if(table[i]==1)	{flag=1;}
		}
		if(flag==0){break;}
	}
	return;
}

void outputcommandqueue(){
	int i=0,j=0;
	sort(qelist,qelist+maxsize,cmp3);
	for(i=0;i<maxsize;i++){
		if(qelist[i].queuenum==-1) continue;
		j=0;
		printf("commandqueue%d",qelist[i].queuenum);
		if(qelist[i].property==0){printf("(顺序队列):\n"); }
		else if(qelist[i].property==1||qelist[i].property==2){printf("(乱序队列):\n");}
		while(qelist[i].list[j]!=""){
			if(qelist[i].list[j]=="commandqueuebarrier"){printf("| ");j++;continue;}
			cout<<qelist[i].list[j]<<" ";
			j++;
		}
		cout<<endl;
	}
	cout<<endl;
	return ;
}

//计算评估函数值
void path(){
	int level=0;
	priority prior[maxsize][maxsize];
	memset(prior,0,sizeof(prior));
	fill(readytime,readytime+maxsize,0);
	while(level!=totallevel){
		if(level==0){
			for(int i=0;i<maxsize;i++){
				if(levelgraph[0][i]==1){
					prior[0][i].weight=elist[i].size/device[solution[i]];
					prior[0][i].num=i;
				}
			}
			sort(prior[0],prior[0]+maxsize,cmp2);
			for(int i=0;i<maxsize;i++){
				if(prior[0][i].weight>0){
					int k=prior[0][i].num;
					endtime[k]=readytime[solution[k]]+elist[k].time;
					readytime[solution[k]]=endtime[k];
				}
			}
		}
		else{
			for(int i=0;i<maxsize;i++){
				if(levelgraph[level][i]==1){
					prior[level][i].num=i;
					for(int j=0;j<maxsize;j++){
						if(levelgraph[level-1][j]==1){
							if(eventgraph2[j][i]==1){
								double foretime=(endtime[j]>readytime[solution[i]])?endtime[j]:readytime[solution[i]];
								prior[level][i].weight=foretime>prior[level][i].weight?foretime:prior[level][i].weight;
							}
						}
					}
				}
			}
			sort(prior[level],prior[level]+maxsize,cmp2);
			for(int i=0;i<maxsize;i++){
				if(prior[level][i].weight>0){
					int k=prior[level][i].num;
					double tmp=prior[level][i].weight>readytime[solution[k]]?prior[level][i].weight:readytime[solution[k]];
					endtime[k]=tmp+elist[k].time;
					readytime[solution[k]]=endtime[k];
				}
			}
		}
		level++;
	} 
	return;
}

//评估函数框架
double judge(){
	int i;
	maxtime=0;
	fill(endtime,endtime+maxsize,0);
	for(i=0;i<maxsize;i++){
		if(elist[i].name=="") continue;
		else{elist[i].time=elist[i].size/device[solution[i]];}
	}
	path();
	for(i=0;i<maxsize;i++){
		if(endtime[i]!=0){maxtime=(endtime[i]>maxtime)?endtime[i]:maxtime;}
		else{continue;}
	}
	return maxtime;
}

int randomnum(){
	return rand()%devicenum; 
}

//遗传算法
void GA(){
	int i,j;
	for(i=0;i<groupsize;i++){
		for(j=0;j<maxsize;j++){
			if(elist[j].name!=""){group[i].plan[j]=randomnum();}
			else group[i].plan[j]=-1;
		}
	}
	double chance[passsize];
	int count=0;
	int phase=0;
	int totalchance=0;
	int reproduce=0;
	int select1,select2;
	int parent1[maxsize],parent2[maxsize];
	int best[maxsize];
	while(1){
		for(i=0;i<groupsize;i++){
			fill(solution,solution+maxsize,0);
			memcpy(solution,group[i].plan,maxsize*sizeof(int));
			group[i].maxt=judge();
		}
		sort(group,group+groupsize,cmp);
		if(++count==epoch){break;}	//初始为第一代，第十代即循环九次 
		else{
			if(count-phase>epoch/10){phase=count;mp++;sp--;}
			memcpy(best,group[0].plan,sizeof(int)*maxsize);
			fill(chance,chance+passsize,0);
			for(int i=0;i<passsize;i++){
				chance[i]=group[passsize-1].maxt*1.5-group[i].maxt;
			}
			for(int i=1;i<passsize;i++){
				chance[i]+=chance[i-1];
			}
			totalchance=ceil(chance[passsize - 1]);
			while(reproduce!=groupsize){
				select1=rand()%totalchance;			//选择
				select2=rand()%totalchance;
				int x=0;
				while(select1>chance[x]){	
					x++;
				}
				memcpy(parent1,group[x].plan,sizeof(int)*maxsize);
				x=0;
				while(select2>chance[x]){
					x++;
				}
				memcpy(parent2,group[x].plan,sizeof(int)*maxsize);
				int swapp;
				int temp;
				for(int i=0;i<maxsize;i++){			//交换
					swapp=rand()%100;
					if(swapp<sp){
						temp=parent1[i];
						parent1[i]=parent2[i];
						parent2[i]=temp;
					}
				}
				memcpy(nextg[reproduce].plan,parent1,maxsize*(sizeof(int)));
				int mutation;
				int m;
				for(int i=0;i<maxsize;i++){			//变异
					if(elist[i].name=="") continue;
					mutation=rand()%200;
					if(mutation<mp){
						do{
							m=randomnum(); 
						}while(nextg[reproduce].plan[i]!=m);
						nextg[reproduce].plan[i]=m;	
					}
				}
				reproduce++;
				if(reproduce==groupsize) break;
				else{
					memcpy(nextg[reproduce].plan,parent2,maxsize*(sizeof(int)));
					for(int i=0;i<maxsize;i++){
						if(elist[i].name=="") continue;
						mutation=rand()%200;
						if(mutation<mp){
							do{
								m=randomnum(); 
							}while(nextg[reproduce].plan[i]!=m);
							nextg[reproduce].plan[i]=m;	
						}
					}
					reproduce++;
				}
			}
			memcpy(nextg[groupsize-1].plan,best,sizeof(int)*maxsize);			
		}
		for(i=0;i<groupsize;i++){
			memcpy(group[i].plan,nextg[i].plan,sizeof(int)*maxsize);
		}
	}
	for(i=0;i<groupsize;i++){
		fill(solution,solution+maxsize,0);
		memcpy(solution,group[i].plan,maxsize*sizeof(int));
		group[i].maxt=judge();
	}
	sort(group,group+groupsize,cmp);
	return;	
}

//初始化函数
int opencl_start(){
	int i,j;
	srand((unsigned)time(NULL));
	memset(eventgraph,0,sizeof(eventgraph));
	memset(eventgraph2,0,sizeof(eventgraph2));
	memset(levelgraph,0,sizeof(levelgraph));
	memset(group,0,sizeof(group));
	memset(nextg,0,sizeof(nextg));
	for(i=0;i<maxsize;i++){
		elist[i].name="";
		elist[i].size=-1;
		elist[i].time=0;
		nlist[i].name="";
		nlist[i].size=-1;
		nlist[i].time=0;
		qelist[i].queuenum=-1;
		qelist[i].property=-1;
		for(j=0;j<maxsize;j++){
			qelist[i].list[j]="";
		}
	}
	for(i=0;i<maxsize*2;i++){
		tlist[i].name="";
		for(j=0;j<maxarg;j++){
			tlist[i].arg[j]=-1;
		}
	}
	return 0;
}

void reorder(){
	int i,j,m,n1=-1,n2=-1,n3=-1,n4=-1;
	for(i=0;i<maxsize;i++){
		if(qelist[i].property==0){
			for(j=1;qelist[i].list[j]!=""&&j<maxsize;j++){
				n1=-1;n2=-1;
				for(m=0;m<maxsize;m++){
					if(elist[m].name==qelist[i].list[j-1]) {n1=m;}
					if(elist[m].name==qelist[i].list[j]) {n2=m;}
				}
				if(n1!=-1&&n2!=-1){
					eventgraph[n1][n2]=1;
				}
			}
		}
		else if(qelist[i].property==2){
			n1=0;
			for(j=0;qelist[i].list[j]!=""&&j<maxsize;j++){
				if(qelist[i].list[j]=="commandqueuebarrier"){
					n2=j;
					while(1){
						n3=-1;
						n4=-1;
						j++;
						if(qelist[i].list[j]==""||qelist[i].list[j]=="commandqueuebarrier"||j==maxsize){break;}
						for(int k=n1;k<n2;k++){
							for(int l=0;l<maxsize;l++){
								if(elist[l].name==qelist[i].list[k]) {n3=l;}
								if(elist[l].name==qelist[i].list[j]) {n4=l;}
							}
							if(n3!=-1&&n4!=-1){	
								eventgraph[n3][n4]=1;
							}	
						}
					}
					n1=n2+1;
					j--;
				}
			}
		}
	}
	return;
}

//在最后开始分析的控制流函数
int opencl_end(){
	int i,j;
	reorder();
	printf("关联内核层次：\n");
	outputgraph();
	printf("独立内核：\n"); 
	for(i=0;i<totalnull;i++){
		cout<<nlist[i].name<<" ";
	}
	printf("\n\n各队列中的内核任务(按执行顺序),顺序队列已经反映在任务图:\n");
	outputcommandqueue();
	printf("各关联内核规模:\n"); 
	for(i=0;i<maxsize;i++){
		if(elist[i].name=="")continue;
		cout<<elist[i].name<<":"<<elist[i].size<<endl;
	}
	printf("输入平台可用设备数:");
	scanf("%d",&devicenum);
	printf("按序输入设备性能：\n"); 
	for(i=0;i<devicenum;i++){
		scanf("%lf",&device[i]);
	}
	GA();
	printf("前三个方案:\n\n");
	for(i=0;i<3;i++){
		printf("方案%d:\n",i);
		for(j=0;j<maxsize;j++){
			if(elist[j].name!=""){
				cout<<elist[j].name<<":设备"<<group[i].plan[j]<<endl;	
			}
		} 
		printf("评估函数值:%.3lf\n",group[i].maxt);
		printf("\n"); 
	}
	return 0;
}

#endif
