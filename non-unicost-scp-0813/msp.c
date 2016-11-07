#include<stdio.h>
#include<stdlib.h>
#include<vector>
#include<limits.h>
#include<float.h>
#include<memory.h>
#include<sys/times.h>
#include<unistd.h>
#include<set>
#define MAXC 50000
#define MAXR 500000
using namespace std;
typedef struct column{
	int config;
	int score;
	int time_stamp;
	int is_in_c;
	int cost;
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
double alpha[MAXC];

vector<int> uncov_r;
vector<int> best_sol;

set<int> tabu_list;
int tabu[20];
int c_size;
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
int fitness();
int best_value;
void build_instance2(char *file){
	int i,j,k,t;
	freopen(file,"r",stdin);
	scanf("%d%d",&r_num,&c_num);
	for(i=1;i<=c_num;i++){
		scanf("%d",&k);
		cs[i].cost=k;
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
		cs[i].score=0;
		cs[i].config=1;
		cs[i].time_stamp=0;
		cs[i].is_in_c=0;
		for(j=0;j<cs[i].vec_rs.size();j++){
			k=cs[i].vec_rs[j];
			cs[i].score+=rs[k].weight;
		}
		alpha[i]=cs[i].score*1.0/cs[i].cost;
	}
}
void init(){
	best_sol.clear();
	c_size=0;
	tabu[0]=tabu[1]=tabu[2]=0;
	best_value=INT_MAX;
}
void init_best(){
	int cnt;
	int i,j,k;
	double max;
	while(!uncov_r.empty()){
		max=-DBL_MAX;
		cnt=0;
		for(i=1;i<=c_num;i++){
			if(cs[i].is_in_c) continue;
			if(max<alpha[i]){
				max=alpha[i];
				best_array[0]=i;
				cnt=1;
			} else if(max==alpha[i]){
				best_array[cnt++]=i;
			}
		}
	//	printf("cnt==%d, max==%f, ",cnt, max);
		if(cnt>0){
			vector<int> bestA;
			int minCost=INT_MAX;
			k=0;
			for(j=0;j<cnt;j++){
				if(best_array[j] < minCost){
					minCost=best_array[j];
					bestA.clear();
					bestA.push_back(j);
				} else if( minCost==best_array[j]) {
					bestA.push_back(j);
				}
			}
			j=rand()%bestA.size();
			add(best_array[bestA[j]]);
		}
	}
	update_best_sol();
	printf("initial solution:%d\n",best_value);
}
void add(int c){
	cs[c].is_in_c=1;
	cs[c].score=-cs[c].score;
	alpha[c]=-alpha[c];
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
			cs[t].config=1;
		}
		if(cnt==0){ // c is the first one can cover this row 
			for(k=0;k<rs[j].vec_cs.size();k++){
				t=rs[j].vec_cs[k];
				if(t==c) continue;
				cs[t].score-=rs[j].weight;
				alpha[t]=cs[t].score*1.0/cs[t].cost;
			}
		} else if(cnt==1){// c is second one can cover this row
			cs[s].score+=rs[j].weight;
			alpha[s]=cs[s].score*1.0/cs[s].cost;
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
	cs[c].score=-cs[c].score;
	alpha[c]=-alpha[c];
	cs[c].config=0;
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
			cs[t].config=1;
		}
		if(cnt==0){
			uncov_r.push_back(j);
			for(k=0;k<rs[j].vec_cs.size();k++){
				t=rs[j].vec_cs[k];
				if(t==c) continue;
				cs[t].score+=rs[j].weight;
				alpha[t]=cs[t].score*1.0/cs[t].cost;
			}
		} else if(cnt==1){
			cs[s].score-=rs[j].weight;
			alpha[s]=cs[s].score*1.0/cs[s].cost;
		}
	}
	c_size--;
}
int in_tabu(int i){
	int j;
	int tabu_len=1;
	for(j=0;j<tabu_len;j++){
		if(i==tabu[j]) return 1;
	}
	return 0;
}
int find_best_in_c(int allowTabu){
	int i, maxc, cnt=0;
	double max=-DBL_MAX;
	for(i=1;i<=c_num;i++){
		if(!cs[i].is_in_c) continue;
		if(allowTabu&&in_tabu(i)) continue;
		if(max<alpha[i]){
			max=alpha[i];
			maxc=i;
			best_array[0]=i;
			cnt=1;
		} else if(max==alpha[i]){
			best_array[cnt++]=i;
			if(cs[maxc].time_stamp>cs[i].time_stamp){
				maxc=i;
			}
		}
	}
	//maxc=best_array[rand()%cnt];
//	printf("maxc:%d,,",maxc);
	return maxc;
}
void uncov_r_weight_inc(){
	int i,j,k,total=0,cnt,s;
	for(i=0;i<uncov_r.size();i++){
		rs[uncov_r[i]].weight++;
		for(j=0;j<rs[uncov_r[i]].vec_cs.size();j++){
			k=rs[uncov_r[i]].vec_cs[j];
			cs[k].score+=1;
			alpha[k]=cs[k].score*1.0/cs[k].cost;
		}
	}
}
void localsearch(){
	step=1;
	int b1=0;
	int i,j,k;
	int best_in_c;
	int maxc;
	double max=0;
	while(step<total_step){
		int fit=fitness();
		if(uncov_r.empty()){
			update_best_sol();
			if(best_value==BEST){
				printf("optimum found\n");
				break;
			}
			maxc=find_best_in_c(0);
			remove(maxc);
			continue;
		}
		//printf("fitness:%d.\n",fit);
		best_in_c=find_best_in_c(1);
		remove(best_in_c);
		//if(fit>=best_value){
		//}
		cs[best_in_c].time_stamp=step;
		while(!uncov_r.empty()){
			i=rand()%uncov_r.size();
			j=uncov_r[i];
		//	printf("uncov size:%d j==%d, rs[j]:%d.",uncov_r.size(),j,rs[j].vec_cs.size());
		    max=-DBL_MAX; 
			for(i=0; i<rs[j].vec_cs.size();i++){
				k=rs[j].vec_cs[i];
				if(!cs[k].config) continue;
				if(max<alpha[k]){
					max=alpha[k];
					maxc=k;
				} else if(max==alpha[k]&&cs[maxc].time_stamp>cs[k].time_stamp){
					maxc=k;
				}
			}
			if(cs[maxc].cost+fitness()>=best_value) break;
			add(maxc);
			tabu[0]=maxc;
			cs[maxc].time_stamp=step;
			uncov_r_weight_inc();
		}

		step++;
	}
}
int fitness(){
	int s=0;
	int i;
	for(i=1;i<=c_num;i++){
		if(cs[i].is_in_c){
			s+=cs[i].cost;
		}
	}
	return s;
}
void update_best_sol(){
	int i;
	int k=fitness();
	//printf("update: %d ",k);
	if(k<best_value){
		best_sol.clear();
		best_value=k;
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
void print_sol(){
	int i;
	for(i=0;i<best_sol.size();i++){
		printf("%d,",best_sol[i]);
	}
	printf("\n");
}
int main(int argc, char *argv[]){
	struct tms start, end;
	if(argc<5) return 0;
	build_instance2(argv[1]);
	BEST=atoi(argv[2]);
	seed=atoi(argv[3]);
	total_step=atoi(argv[4]);
	srand(seed);
	init();
	times(&start);
	init_best();
	localsearch();
	times(&end);
	printf("final best:%d\n",best_value);
	printf("search steps is %d, seed is %d\n",step,seed);
	if(!check()) printf("wrong answer \n");
	printf("time cost %f s\n", (end.tms_utime-start.tms_utime)*1.0/sysconf(_SC_CLK_TCK));
//	print_sol();
	return 0;
}
