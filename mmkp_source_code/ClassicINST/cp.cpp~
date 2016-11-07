#include<stdio.h>
#include<iostream>
#include<stdlib.h>
#include<limits.h>
#include<float.h>
#include"./cpu_time.c"

#define EPSILON (1e-6)
#define FABS(x) ((x)<0? -(x):(x))
#define MAX(a,b) ((a)>(b)? (a):(b))
#define MIN(a,b) ((a)<(b)? (a):(b))

int n, l, m;
typedef struct GroupST{
	int nItem;
	double *profit;
	int **consume;
}Group;

Group *group;
int *res;
int *cos;
int *rho;
double LB;
double value=0;
int *violate;
int nVio;
int *mult;
int *bestVec;

int *fix;
int **count;
int *fixRes;

void read_inst(char *file){
	int i,j,k;
	freopen(file, "r",stdin);
	scanf("%d%d%d", &n, &l, &m);
	group=new Group[n];
	res=new int[m];
	fix=new int [n];
	fixRes=new int[m];
	count=new int*[n];
	mult=new int[m]; //the penalty multiplier 
	for(i=0;i<m;i++){
		scanf("%d",&res[i]);
		mult[i]=1;
		fixRes[i]=0;
	}

	for(i=0;i<n;i++){
		scanf("%d", &j);
		group[i].nItem=l;
		group[i].profit=new double[l];
		group[i].consume=new int*[l];
		count[i]=new int[m];
		for(j=0;j<l;j++){
			scanf("%lf",&group[i].profit[j]);
			group[i].consume[j]=new int[m];
			for(k=0;k<m;k++){
				scanf("%d",&group[i].consume[j][k]);
			}
			count[i][j]=1;
		}
		fix[i]=-1;
	}
	rho=new int[n];
	bestVec=new int[n];
	LB=0;
	cos=new int[m];
	violate=new int[m];
	nVio=0;
}
long long dotproduct(int a[], int b[], int len){
	long long s=0;
	int i;
	for(i=0;i<len;i++){
		long long t=a[i]*b[i];
		s+=t;
	}
	return s;
}

void construct(){
	int i,j,k;
	for(j=0;j<m;j++) cos[j]=0;
	value=0;
	for(i=0;i<n;i++){
		double u_max=-1;
		int k=-1;
		for(j=0;j<group[i].nItem;j++){
			double u=group[i].profit[j]/dotproduct(group[i].consume[j],mult, m);
			if(u_max<u) {
				u_max=u;
				k=j;
			 }
		}
		rho[i]=k;
		value+=group[i].profit[k];
		for(j=0;j<m;j++) {
			cos[j]+= group[i].consume[k][j];
		}
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
		if(cos[i]>res[i]){
			violate[nVio]=i;
			nVio++;
		}
	}
	value=value-group[theG].profit[IRemove]+group[theG].profit[IAdd];
	rho[theG]=IAdd;
}
int check_feasibility(int g1, int iR1, int iA1, int g2, int iR2, int iA2){
	int i,j,k;
	k=0;
	for(i=0;i<m;i++){
		if(cos[i]-group[g1].consume[iR1][i] + group[g1].consume[iA1][i]- group[g2].consume[iR2][i]+group[g2].consume[iA2][i]<=res[i]) k++;
	}
	return k==m;
}
void two_flip(){
	int ii1, ii2, jj1, jj2;
	int flag=0;
	int i1,i2,j1,j2;
	int max_t=0;
	for(i1=0;i1<n;i1++){
		for(i2=i1+1;i2<n;i2++){
			for(j1=0;j1<l;j1++){
				for(j2=0;j2<l;j2++){
					if(rho[i1]==j1&&rho[i2]==j2) continue;
					double v_t=value-group[i1].profit[rho[i1]]+group[i1].profit[j1]-group[i2].profit[rho[i2]]+group[i2].profit[j2];
					if(v_t>LB){
						if(check_feasibility(i1,rho[i1],j1, i2, rho[i2], j2)){
							flag=1;
							if(max_t<v_t){
								max_t=v_t;
								ii1=i1; jj1=j1;
								ii2=i2; jj2=j2;
							}
						}
					}
				}
			}
		}
	}
	if(flag){
		swap_one(ii1, rho[ii1], jj1);
		swap_one(ii2, rho[ii2], jj2);
		LB=value;
		printf("sol %lf, time %lf\n", LB, cpu_time());
	}
}
int v_check(int i1, int j1, int i2, int j2, int i3, int j3){
	return 0;
}	
void three_flip(){
	int ii1, ii2, ii3, jj1, jj2, jj3;
	int i1,i2,i3, j1,j2, j3;
	for(i1=0;i1<n;i1++){
		for(i2=i1+1; i2<n;i2++){
			for(i3=i2+1; i3<n; i3++){
				for(j1=0;j1<l;j1++){
					for(j2=0; j2<l;j2++){
						for(j3=0;j3<l; j3++){
							if(rho[i1]==j1&&rho[i2]==j2&&rho[i3]==j3) continue;
							if(v_check(i1, j1, i2, j2, i3, j3)){
							}
						}
					}
				}
			}
		}
	}
}
void ccp(){
	int i, j;
	for(i=0;i<n;i++){
		double maxV=group[i].profit[rho[i]];
		int pos=-1;
		for(j=0;j<group[i].nItem;j++){
			if(j==rho[i]) continue;
			if(maxV<group[i].profit[j] && beFeasible(i, rho[i], j)){
				maxV=group[i].profit[j];
				pos=j;
			}
		}
		if(pos>-1){
			swap_one(i, rho[i], pos);
		}
	}
}
void fixing(){
	int t=0;
	int sum=0;
	int i,j,k;
	for(i=0;i<n;i++) fix[i]=-1;
	for(i=0;i<m;i++) {
		fixRes[i]=0;
		mult[i]=1;
	}
	for(i=0;i<n;i++){
		if(bestVec[i]!=rho[i]){
			swap_one(i, rho[i], bestVec[i]);
		}
	}
	k=n;
	while(t<n/4){
		sum=0;
		for(i=0;i<n;i++){
			if(fix[i]>=0) continue;
			j=bestVec[i];
		}
		int p=rand()%k;
		sum=0; j=0;
		for(i=0;i<n;i++){
			if(fix[i]>=0) continue;
			j++;
			if(p==j){
				break;
			}
		}
		fix[i]=bestVec[i];
		for(j=0;j<m;j++){
			fixRes[j]+=group[i].consume[bestVec[i]][j];
		}
		t++;
		k--;
	}
}
void local_search(int maxStep){
	int step=1;
	int i,j,k,cnt;
	int no_improve=0;
	while(step++<maxStep){
		while(nVio==0){
			if(value>LB) {
				LB=value;
				for(i=0;i<n;i++){
					//count[i][rho[i]]++;
					bestVec[i]=rho[i];
				}
				printf("sol %lf, time %lf, step %d\n ", LB, cpu_time(), step );
				for(i=0;i<m;i++) mult[i]=1;
				no_improve=0;
			}
			i=rand()%n;
			if(fix[i]>=0) continue;
			int theI=-1;
			double u=DBL_MAX;
			for(j=0;j<group[i].nItem; j++){
				if(j==rho[i]) continue;
				if(group[i].profit[j]<group[i].profit[rho[i]]) continue;
				double u1=dotproduct(group[i].consume[j],mult,m)/group[i].profit[j];
				if(u>u1){
					u=u1;
					theI=j;
				}
			}
			if(theI>-1) swap_one(i, rho[i],theI);
			//two_flip();
		//	ccp();
		}
		int dim=-1; // find a dimension
		int maxVio=-1;
		for(i=0;i<nVio;i++){
			if(maxVio<cos[violate[i]]-res[violate[i]]){
				maxVio=cos[violate[i]]-res[violate[i]];
				dim=violate[i];
			}
		}
		//dim=violate[rand()%nVio];
		while(1){
		int theG;
		int num=rand()%cos[dim], s=0; // randomly select a group
		for(i=0;i<n;i++){
			if(fix[i]>=0) continue;
			s=s+group[i].consume[rho[i]][dim];
			if(num<s) break;
		}
		theG=i;
		//theG=step%n;
		//theG=rand()%n;
		double u=DBL_MAX;
		int theI=-1; // find an item to swap in group theG 
		for(j=0;j<group[theG].nItem;j++){
			if(j==rho[theG]) continue;
			long long tProduct= dotproduct(group[theG].consume[j], mult, m);
			if(tProduct>INT_MAX/2) {
				printf("too big step %d\n", step);
				for(int ii=0;ii<m;ii++) mult[ii]=1;
				;}
			double u1=tProduct/(group[theG].profit[j]*count[theG][j]);
			if(u>u1+EPSILON && value-group[theG].profit[rho[theG]]+group[theG].profit[j]>LB){
				u=u1;
				theI=j;
			}
		}
		if(theI>-1){
			for(j=0;j<group[theG].nItem;j++){
				if(j==rho[theG]||j==theI) continue;
				//count[theG][j]++;
			}
			swap_one(theG, rho[theG], theI);
			for(i=0;i<nVio;i++){
				j=violate[i];
				mult[j]++;
			}
			break;
		} else{
		//	printf("abnormal\n");
		}
		}
		if(0&&no_improve++>=10000000){
			//printf("fixing\n");
			//fixing();
			two_flip();
			//no_improve=0;
		}
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
	cpu_time();
	double start=cpu_time();
	construct();
	printf("value:%lf, nVio:%d\n", value, nVio);
	local_search(maxStep);
	double end=cpu_time();
	printf("LB:%lf, seed :%d\n", LB, seed);
	printf("cup time: %f\n", end-start);
}
