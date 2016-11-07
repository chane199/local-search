/*
   The Implementation of MRLS by Chao Gao. 
   gaochao1990@gmail.com
*/
#include<stdio.h>
#include<iostream>
#include<stdlib.h>
#include"./cpu_time.c"
#include<limits.h>
using namespace std;

#define EPSILON (1e-5)
#define FABS(x) ((x)<0? -(x):(x))
#define MAX(a,b) ((a)>(b)? (a):(b))
#define MIN(a,b) ((a)<(b)? (a):(b))

int n, l, m;
typedef struct GroupST{
		int numOfItem;
		double *profit;
		int **consume;
}Group;

Group *group;
int *res;
int *cos;
int *x;
double LB;
double value=0;
int *violate;
int nVio;

int beFeasible(int, int, int);
void swap_one(int, int, int);

double *tabu_list; //tabu_list is a queue with length of tabu_len
int q_tail;
int q_head;
int q_size;
int tabu_len;
int step;
int no_improve=0;

void read_inst(char *file){
	int i,j,k;
	freopen(file, "r",stdin);
	scanf("%d%d%d", &n, &l, &m);
	group=new Group[n];
	res=new int[m];

	tabu_len=2*n;
	tabu_list=new double[tabu_len+11];
	q_head=0;
	q_tail=0;
	q_size=0;
	for(i=0;i<m;i++){
		scanf("%d",&res[i]);
	}
	for(i=0;i<n;i++){
		scanf("%d", &j);
		group[i].numOfItem=l;
		group[i].profit=new double[l];
		group[i].consume=new int*[l];

		for(j=0;j<l;j++){
			scanf("%lf",&group[i].profit[j]);
			group[i].consume[j]=new int[m];
			for(k=0;k<m;k++){
				scanf("%d",&group[i].consume[j][k]);
			}
		}
	}
	x=new int[n];
	LB=0;
	cos=new int[m];
	violate=new int[m];

	nVio=0;
	printf("read instance end\n");
}
int dotproduct(int a[], int b[], int len){
	int s=0;
	int i;
	for(i=0;i<len;i++) s+=a[i]*b[i];
	return s;
}
// cp(), construct a feasible initial solution
void construct(){
	int i,j,k;
	for(j=0;j<m;j++) cos[j]=0;
	value=0;
	for(i=0;i<n;i++){
		double u_max=-1;
		int k=-1;
		for(j=0;j<group[i].numOfItem;j++){
			double u=group[i].profit[j]/dotproduct(group[i].consume[j],res, m);
			if(u_max<u) {
				u_max=u;
				k=j;
			 }
		}
		x[i]=k;
		value+=group[i].profit[k];
		for(j=0;j<m;j++) cos[j]+= group[i].consume[k][j];
	}	
	for(i=0;i<m;i++){
		if(cos[i]>res[i]){
			violate[nVio]=i;
			nVio++;
		}
	}
	int ite=1, maxIte=1000;
	while(nVio>0 && ite++<maxIte){
		int maxVio=0,pos;
		for(j=0;j<nVio;j++){ 
			if(maxVio<cos[violate[j]]-res[violate[j]]){ 
				maxVio=cos[violate[j]]-res[violate[j]]; 
				pos=violate[j];
			} 
		} 
		if(maxVio>0){ 
			int maxG=-1, maxValue=-1; 
			for(j=0;j<n;j++){ 
				if(maxValue<group[j].consume[x[j]][pos]){
					maxValue=group[j].consume[x[j]][pos]; maxG=j;
				} 
			} 
			int flag=0;
			int minV=INT_MAX, minItemIndex; 
			i=maxG; 
			for(j=0;j<group[i].numOfItem;j++){
				if(j==x[i]) continue; 
				if(minV>group[i].consume[j][pos]){
					minV=group[i].consume[j][pos]; 
					minItemIndex=j; 
				} 
				if(beFeasible(i, x[i], j)){
					swap_one(i, x[i], j); 
					flag=1; break;
				} 
			} 
			if(flag) break; 
			else { 
				swap_one(i, x[i], minItemIndex); 
			} 
		} else break; 
	}
	if(nVio==0) LB=value;
	printf("construct LB %f\n",LB);
}
// check if this swap can guarantee feasibility
int beFeasible(int theGroup, int IRemove, int IAdd){
	int i, num=0;
	int g=theGroup;
	for(i=0;i<m;i++){
		if(cos[i]-group[g].consume[IRemove][i]+group[g].consume[IAdd][i]<=res[i]){
			num++;
		}
	}
	return num==m;
}

//replace the old item in theG with a new one 
void swap_one(int theG, int IRemove, int IAdd){
	int i;
	nVio=0;
	for(i=0;i<m;i++){
		cos[i]=cos[i]-group[theG].consume[IRemove][i] + group[theG].consume[IAdd][i];
		if(cos[i]>res[i]){
			violate[nVio]=i;
			nVio++;
		}
	}
	value=value-group[theG].profit[IRemove]+group[theG].profit[IAdd];
	x[theG]=IAdd;
}

//check if this configuration is with a tabu value
int is_tabu(double v){
	int i=0;
	for(i=q_head;i<q_size;i++){
		i=i%tabu_len;
		if(FABS(tabu_list[i]-v)<EPSILON) return 1;
	}
	return 0;
}

//insert tabu, tabu list is a queue, length: tabu_len
void insert_tabu(double v){
	int i,j;
	for(i=0;i<q_size;i++){
		j=q_head+i;
		j=j%tabu_len;
		if(FABS(tabu_list[j]-v)<EPSILON) return;
	}

	tabu_list[q_tail]=v;
	q_tail++;
	q_tail=q_tail%tabu_len;
	if(q_tail==q_head){
		q_head++;
		q_head=q_head%tabu_len;
	} else {
		q_size++;
	}
}
//randomly select a tabu_len in the interval [2n, 2n+10]
void change_tabu(){
	int i,j,k;
	k=rand()%11;
	int len=2*n+k;
	q_size=q_size<len?q_size:len;
	q_tail=(q_head+q_size)%tabu_len;
}

//local swap search, CCPs()
void ccp(){
	int i, j,k=0;
	for(i=0;i<n;i++){
		int pos=-1;
		double maxPt=group[i].profit[x[i]];
		for(j=0;j<group[i].numOfItem;j++){
			if(j==x[i]) continue;
			if(maxPt < group[i].profit[j] && beFeasible(i, x[i], j)){
				if(is_tabu(value-group[i].profit[x[i]]+group[i].profit[j])) continue;
				maxPt=group[i].profit[j];
				pos=j;
				
			}
		}
		if(pos==-1) continue;
		swap_one(i, x[i], pos);
	}
}
// degrade p times
void degrade(int p){
	int i,j,k;
	for(int k=0;k<p;k++){
			i=rand()%n;	
			for(j=0;j<group[i].numOfItem;j++){
				if(j==x[i]) continue;
				if(beFeasible(i, x[i], j) && 
						!is_tabu(value-group[i].profit[x[i]]+group[i].profit[j])){
					swap_one(i, x[i], j);
					if(nVio==0&&LB<value){
						LB=value;
						printf("in degrade sol %lf, time %lf, step %d\n", LB, cpu_time(), step);
						no_improve=0;
					}
				}
			}
		}
}
//the main loop
void lsearch(int maxIter){
	step=0;
	int i,j,k;
	while(step++<maxIter){
		ccp();	
		/*update best solution */
		if(value>LB){
			LB=value;
			printf("sol %lf, time %lf, step %d\n", LB, cpu_time(), step);
			no_improve=0;
		} else no_improve++;

		insert_tabu(value); // insert into tabu list
		degrade(5); // degrad, p=5
		if(no_improve>=50){ // after 50 consective iterations no improvement 
			change_tabu();
			//q_head=q_head=q_size=0;
			//tabu_len=2*n+rand()%11;
		//	no_improve=0;
		}
	}
}

int main(int argc, char *argv[]){
	if(argc<4){
		printf("useage:./solve file maxStep seed\n");
		return 0;
	}
	int seed;
	read_inst(argv[1]);
	int maxStep=atoi(argv[2]);
	seed=atoi(argv[3]);
	srand(seed);
	double start=cpu_time();
	construct();
	printf("value:%f, nVio:%d\n", value, nVio);
	if(nVio>0){
		printf("construct infeasible\n");
		return 0;
	}
	lsearch(maxStep);
	double end=cpu_time();
	printf("LB:%f, seed :%d\n", LB, seed);
	printf("cup time: %f\n", end-start);
}
