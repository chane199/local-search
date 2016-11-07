
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
