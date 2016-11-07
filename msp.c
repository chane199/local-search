#include<stdio.h>
#include<stdlib.h>
#include<vector>
#include<limits.h>
#include<memory.h>
#define MAXC 50000
#define MAXR 500000
using namespace std;
typedef struct column{
	int confchange;
	int dscore;
	int time_stamp;
	int is_in_c;
	vector<int> vec_rs;// rows it can cover
}column;

typedef struct row{
	int weight;
	vector<int> vec_cs; // cols have can cover this row
}row;

int c_num;
int r_num;
column cs[MAXC];
row rs[MAXR];

vector<int> uncov_r;
vector<int> best_sol;

int tabu;
int c_size;
int ave_weight;
int total_weight;
float p_scale=0.3;
int threshold;
int seed;
int BEST;
int step;
int total_step;
int best_array[MAXC];

void init();
void init_best();
void localsearch();
void add(int);
void remove(int);
int find_best_in_c(int);
void uncov_r_weight_inc();
void update_best_sol();
/*
void build_instance(char *file){
	int i,j,k;
	char t1[20],t2[20];
	freopen(file,"r",stdin);
	scanf("%s%s%d%d",t1,t2,&r_num,&c_num);
	scanf("%s",t1);
	for(i=1;i<=c_num;i++){
		scanf("%s",t1);
		while((k=atoi(t1))){
			cs[i].vec_rs.push_back(k);
			if(scanf("%s",t1)==EOF) break;
		}
	}
	uncov_r.clear();
	for(i=1;i<=r_num;i++){
		rs[i].weight=1;
		uncov_r.push_back(i);
	}
	for(i=1;i<=c_num;i++){
		cs[i].confchange=1;
		cs[i].dscore=0;
		cs[i].time_stamp=0;
		cs[i].is_in_c=0;
		for(j=0;j<cs[i].vec_rs.size();j++){
			k=cs[i].vec_rs[j];
			cs[i].dscore+=rs[k].weight;
			rs[cs[i].vec_rs[j]].vec_cs.push_back(i);
		}
	}
	printf("read instance finish\n");
}
*/
void build_instance2(char *file){
	int i,j,k,t;
	freopen(file,"r",stdin);
	scanf("%d%d",&r_num,&c_num);
	for(i=1;i<=c_num;i++){
		scanf("%d",&k);
	}
	for(i=1;i<=r_num;i++){
		scanf("%d",&k);
		rs[i].weight=1;
		for(j=0;j<k;j++){
			scanf("%d",&t);
			rs[i].vec_cs.push_back(t);
			cs[t].vec_rs.push_back(i);
		}
		uncov_r.push_back(i);
	}
	for(i=1;i<=c_num;i++){
		cs[i].dscore=0;
		cs[i].confchange=1;
		cs[i].time_stamp=0;
		cs[i].is_in_c=0;
		for(j=0;j<cs[i].vec_rs.size();j++){
			k=cs[i].vec_rs[j];
			cs[i].dscore+=rs[k].weight;
		}
	}
}
void init(){
	best_sol.clear();
	c_size=0;
	ave_weight=1;
	total_weight=0;
	p_scale=0.3;
	threshold=(int)(5*c_num);
}
void init_best(){
	int cnt,max;
	int i,j,k;
	while(!uncov_r.empty()){
		max=INT_MIN;
		cnt=0;
		for(i=1;i<=c_num;i++){
			if(cs[i].is_in_c) continue;
			if(max<cs[i].dscore){
				max=cs[i].dscore;
				best_array[0]=i;
				cnt=1;
			} else if(max==cs[i].dscore){
				best_array[cnt]=i;
				cnt++;
			}
		}
		if(cnt>0){
			j=rand()%cnt;
			add(best_array[j]);
		}
	}
	update_best_sol();
	printf("inital solution size %d\n",best_sol.size());
}
void add(int c){
	cs[c].is_in_c=1;
	cs[c].dscore=-cs[c].dscore;
	int i,j,k,t,cnt,s;
	for(i=0;i<cs[c].vec_rs.size();i++){
		j=cs[c].vec_rs[i];
		cnt=0;
		for(k=0;k<rs[j].vec_cs.size();k++){
			t=rs[j].vec_cs[k];
			if(t==c) continue;
			if(cs[t].is_in_c){
				s=t;
				cnt++;
			}
			cs[t].confchange=1;
		}
		if(cnt==0){ // c is the first one can cover this row 
			for(k=0;k<rs[j].vec_cs.size();k++){
				t=rs[j].vec_cs[k];
				if(t==c) continue;
				cs[t].dscore-=rs[j].weight;
			}
		} else if(cnt==1){// c is second one can cover this row
			cs[s].dscore+=rs[j].weight;
		}
	}
	vector<int>::iterator it=uncov_r.begin();
	while(it!=uncov_r.end()){
		for(i=0;i<rs[*it].vec_cs.size();i++){
			if(rs[*it].vec_cs[i]==c){
				break;
			}
		}
		if(i<rs[*it].vec_cs.size()){
			it=uncov_r.erase(it);
		} else {
			it++;
		}
	}
	c_size++;
}
void remove(int c){
	cs[c].is_in_c=0;
	cs[c].dscore=-cs[c].dscore;
	cs[c].confchange=0;
	int i,j,k,t,cnt,s;
	for(i=0;i<cs[c].vec_rs.size();i++){
		j=cs[c].vec_rs[i];
		cnt=0;
		for(k=0;k<rs[j].vec_cs.size();k++){
			t=rs[j].vec_cs[k];
			if(t==c) continue;
			if(cs[t].is_in_c){
				cnt++;
				s=t;
			}
			cs[t].confchange=1;
		}
		if(cnt==0){
			uncov_r.push_back(j);
			for(k=0;k<rs[j].vec_cs.size();k++){
				t=rs[j].vec_cs[k];
				if(t==c) continue;
				cs[t].dscore+=rs[j].weight;
			}
		} else if(cnt==1){
			cs[s].dscore-=rs[j].weight;
		}
	}
	c_size--;
}
int find_best_in_c(int allowTabu){
	int i, max=INT_MIN, maxc;
	for(i=1;i<=c_num;i++){
		if(!cs[i].is_in_c) continue;
		if(allowTabu&&i==tabu) continue;
		if(max<cs[i].dscore){
			max=cs[i].dscore;
			maxc=i;
		} else if(max==cs[i].dscore){
			if(allowTabu&&cs[maxc].time_stamp>cs[i].time_stamp){
				maxc=i;
			}
		}
	}
	return maxc;
}
void uncov_r_weight_inc(){
	int i,j,k,total=0,cnt,s;
	for(i=0;i<uncov_r.size();i++){
		rs[uncov_r[i]].weight++;
		for(j=0;j<rs[uncov_r[i]].vec_cs.size();j++){
			k=rs[uncov_r[i]].vec_cs[j];
			cs[k].dscore++;
		}
	}
	total_weight+=uncov_r.size();
	if(total_weight>=r_num){
		ave_weight++;
		total_weight-=r_num;
	} 
	if(ave_weight>=threshold){
		total=0;
		for(i=1;i<=r_num;i++){
			rs[i].weight=(int)(rs[i].weight*p_scale);
			total+=rs[i].weight;
			cnt=0;
			for(j=0;j<rs[i].vec_cs.size();j++){
				k=rs[i].vec_cs[j];
				if(cs[k].is_in_c){
					s=k;
					cnt++;
				}
			}
			if(cnt==0){
				for(j=0;j<rs[i].vec_cs.size();j++){
					k=rs[i].vec_cs[j];
					cs[k].dscore+=rs[i].weight;
				}
			} else if(cnt==1){
				cs[s].dscore-=rs[i].weight;
			}
		}
		ave_weight=total/r_num;
	}
}
void localsearch(){
	step=1;
	int b1;
	int i,j,k;
	int best_in_c;
	int max,maxc,cnt;
	while(1){
		if(uncov_r.empty()){
			update_best_sol();
			if(best_sol.size()==BEST){
				printf("find optimal..\n");
				return;
			}
			b1=find_best_in_c(0);
			remove(b1);
			continue;
		}
		best_in_c=find_best_in_c(1);
		remove(best_in_c);
		i=rand()%uncov_r.size();
		j=uncov_r[i];
	    max=INT_MIN; 
		for(i=0;i<rs[j].vec_cs.size();i++){
			k=rs[j].vec_cs[i];
			if(cs[k].confchange==0) continue;
			if(max<cs[k].dscore){
				max=cs[k].dscore;
				maxc=k;
			} else if(max==cs[k].dscore){
				if(cs[maxc].time_stamp>cs[k].time_stamp){
					maxc=k;
				}
			}
		}
		add(maxc);
		tabu=maxc;
		uncov_r_weight_inc();
		cs[maxc].time_stamp=cs[best_in_c].time_stamp=step;
		step++;
		if(step>=total_step){
			break;
		}
	}
}
void update_best_sol(){
	int i;
	if(best_sol.size()==0||c_size<best_sol.size()){
		best_sol.clear();
		for(i=1;i<=c_num;i++){
			if(cs[i].is_in_c){
				best_sol.push_back(i);
			}
		}
	}
}
int check(){ // check if the solution is a correct cover
	int i,j,k;
	int t[MAXR];
	memset(t,0,MAXR*sizeof(int));
	for(i=0;i<best_sol.size();i++){
		j=best_sol[i];
		for(k=0;k<cs[j].vec_rs.size();k++){
			t[cs[j].vec_rs[k]]=1;
		}
	}
	for(i=1;i<=r_num;i++){
		if(!t[i]) return 0;
	}
	return 1;
}
int main(int argc, char *argv[]){
	if(argc<5) return 0;
	build_instance2(argv[1]);
	BEST=atoi(argv[2]);
	seed=atoi(argv[3]);
	total_step=atoi(argv[4]);
	srand(seed);
	init();
	init_best();
	localsearch();
	printf("final best:%d\n",best_sol.size());
	printf("search steps is %d\n",step);
	if(!check()) printf("wrong answer \n");
	return 0;
}
