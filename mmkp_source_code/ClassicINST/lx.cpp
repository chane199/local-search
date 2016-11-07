#include<stdio.h>
#include<limits.h>
#include<iostream>
#include<stdlib.h>
#include"./cpu_time.c"

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
int **timestamp;
int *mult;
int thred=0;
Group *tabuConfig;

double *tabu_list;
int tabu_len;
int n_tabu_max;

int useTabu=1;
int *candidate;
void read_inst(char *file){
	int i,j,k;
	freopen(file, "r",stdin);
	scanf("%d%d%d", &n, &l, &m);
	group=new Group[n];
	res=new int[m];

	tabuConfig=new Group[n];
	mult=new int[m]; //the penalty multiplier 
	for(i=0;i<m;i++){
		scanf("%d",&res[i]);
		mult[i]=1;
	}

	n_tabu_max=n;
	tabu_list=new double[n_tabu_max];
	tabu_len=0;
	int maxNi=0;
	for(i=0;i<n;i++){
		scanf("%d", &j);
		group[i].numOfItem=l;
		group[i].profit=new double[l];
		group[i].consume=new int*[l];

		tabuConfig[i].numOfItem=l;
		tabuConfig[i].profit=new double[l];
		tabuConfig[i].consume=new int*[l];

		if(maxNi<group[i].numOfItem) maxNi=group[i].numOfItem;
		for(j=0;j<l;j++){
			scanf("%lf",&group[i].profit[j]);
			group[i].consume[j]=new int[m];
			tabuConfig[i].consume[j]=new int[m];
			for(k=0;k<m;k++){
				scanf("%d",&group[i].consume[j][k]);
				tabuConfig[i].consume[j][k]=0;
			}
		}
	}
	x=new int[n];
	LB=0;
	cos=new int[m];
	violate=new int[m];
	timestamp=new int*[n];
	candidate=new int[maxNi];
	thred=m;
	for(i=0;i<n;i++) {
		timestamp[i]=new int[group[i].numOfItem];
		for(j=0;j<group[i].numOfItem;j++){ 
			timestamp[i][j]=0;
		}
	}
	nVio=0;
	/*
	for(i=0;i<n;i++){
		printf("num of items in class %d is %d: \n", i+1, group[i].numOfItem);
		for(j=0;j<group[i].numOfItem;j++){
			printf("item:%d profit:%f, [",j+1, group[i].profit[j]);
			for(k=0;k<m;k++){
				printf("%d ", group[i].consume[j][k]);
			}
			printf("]\n");
		}
	}
	*/
}
int dotproduct(int a[], int b[], int len){
	int s=0;
	int i;
	for(i=0;i<len;i++) s+=a[i]*b[i];
	return s;
}
void construct(){
	int i,j,k;
	for(j=0;j<m;j++) cos[j]=0;
	value=0;
	for(i=0;i<n;i++){
		double u_max=-1;
		int k=-1;
		for(j=0;j<group[i].numOfItem;j++){
			double u=group[i].profit[j]/dotproduct(group[i].consume[j],mult, m);
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
}
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

void swap_one(int theG, int IRemove, int IAdd){
	int i;
	nVio=0;
	for(i=0;i<m;i++){
		cos[i]=cos[i]-group[theG].consume[IRemove][i] + group[theG].consume[IAdd][i];
		if(useTabu) tabuConfig[theG].consume[IRemove][i]=cos[i];
		if(cos[i]>res[i]){
			violate[nVio]=i;
			nVio++;
		}
	}
	if(useTabu) tabuConfig[theG].profit[IRemove]=value;
	value=value-group[theG].profit[IRemove]+group[theG].profit[IAdd];
	x[theG]=IAdd;
}
int isTabu(int theG, int IAdd){
	int i;
	return 0;
	if(FABS(tabuConfig[theG].profit[IAdd]-value)<EPSILON){
		return 1;
	}
	//return 0;
	for(i=0;i<m;i++){
		if(tabuConfig[theG].consume[IAdd][i]==cos[i]) continue;
		else break;
	}
	return i==m;
}
void ccp(){
	int i, j;
	useTabu=0;
	for(i=0;i<n;i++){
		int maxG=0, pos;
		for(j=0;j<group[i].numOfItem;j++){
			if(j==x[i]) continue;
			if(value-group[i].profit[x[i]]+group[i].profit[j]>LB&& beFeasible(i, x[i], j)){
				swap_one(i, x[i], j);
				//j=-1; 
				continue;
			}
		}
	}
	useTabu=1;
}
void local_search(int maxStep){
	int step=0;
	int i,j,k,cnt;
	int tabuRemove;
	int ccp_cnt=0;
	while(step<maxStep){
		while(nVio==0){
	//		printf("feasible\n");
			useTabu=0;
			if(value>LB) {
				ccp_cnt=0;
				LB=value;
				printf("sol %.2lf, time %.6lf, step %d\n ", LB, cpu_time(), step );
			}
			i=rand()%n;
			//i=rand()%n;
			cnt=0;
			for(j=0;j<group[i].numOfItem; j++){
				if(j==x[i]) continue;
				if(group[i].profit[j]>group[i].profit[x[i]]){
					candidate[cnt++]=j;
				//	swap_one(i,x[i],j);
				//	break;
				}
			} 
			if(cnt>0) swap_one(i,x[i], candidate[rand()%cnt]);
			ccp();
			useTabu=1;
		}
		int dim=-1; // find a dimension
		int maxVio=-1;
		for(i=0;i<nVio;i++){
			if(maxVio<cos[violate[i]]-res[violate[i]]){
				maxVio=cos[violate[i]]-res[violate[i]];
				dim=violate[i];
				candidate[0]=dim;
				cnt=1;
			} else if(maxVio==cos[violate[i]]-res[violate[i]]){
				candidate[cnt++]=violate[i];
			}
		}
		dim=candidate[rand()%cnt];
		//dim=violate[rand()%nVio];
		int theG;
		int num=rand()%(cos[dim]), s=0; // randomly select a group
		for(i=0;i<n;i++){
			s=s+group[i].consume[x[i]][dim];
			if(num<s) break;
		}
		theG=i;
		//theG=step%n;
		double u=-1;
		int minCos=INT_MAX;
		int theI=-1; // find an item to swap in group theG 
		for(j=0;j<group[theG].numOfItem;j++){
			if(j==x[theG]) continue;
			int tP=dotproduct(group[theG].consume[j], mult,m);
			double u1=group[theG].profit[j]/tP;
			if(u1>u && value-group[theG].profit[x[theG]]+group[theG].profit[j]>LB&& !isTabu(theG,j)){
				u=u1;
				theI=j;
			}
		}	
		if(theI>-1){
		//	printf("(%d,%d)\t",theG, theI);
			timestamp[theG][theI]=timestamp[theG][x[theG]]=step;
			swap_one(theG, x[theG], theI);
			ccp();
			for(i=0;i<nVio;i++){
				j=violate[i];
				mult[j]++;
				thred++;
			}
			if(0&&thred/m>res[0]) { 
				thred=0;
				for(i=0;i<m;i++){
					mult[i]=MAX(mult[i]*0.6,1);
					thred+=mult[i];
				}
			//	printf("forget\n");
			}
		} else{
		//	printf("abnormal\n");
		}
		ccp_cnt++;
		if(0&&ccp_cnt>1000000){
			ccp();
			ccp_cnt=0;
		}
		step++;
	}
}
int main(int argc, char *argv[]){
	if(argc<3){
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
	printf("value:%.2lf, nVio:%d\n", value, nVio);
	local_search(maxStep);
	double end=cpu_time();
	printf("LB:%f, seed :%d\n", LB, seed);
	printf("cup time: %f\n", end-start);
}
