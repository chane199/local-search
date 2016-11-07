
/***************************************************************************
                                 MMKP.h
                             -------------------
    begin                : Jun 1 2013
    copyright            : (C) 2013 by Yuning Chen
    email                : yuning@info.univ-angers.fr
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/


class MMKP
{

private:

	double **GA;   /*coefficients of equations;*/
	double *GC;      /*coefficients of objective function*/
	double *Gb;      /*constances of constraints*/
	reduceProb *rp;

	int n;
	int gs;
	int m;
	int n_var;
	int n_ct;

	int nb_iter;
	int max_iter;//for CallCplex
	int nb_intGrps;
	double f_best;
	int *tempX;
	int *freeGroups;
	int free_len;
	double *primal_sol;
	double *dual_sol;
	double *reducedCosts;

	double up_bound,lp_value;

	double *primals;
	int **lpCuts;
	int ip_n_var;
	double curObj;
	double *ip_Gb;
	int *freeVars;
	int freeVarNum;

public:
	MMKP(int num_grp, int num_item, int num_cons)
	{
		n = num_grp;
		gs = num_item;
		m = num_cons;
		n_var = n*gs;

		GA = new double*[m];
		for (int i=0;i<m;i++)
			GA[i] = new double[gs*n];
		GC = new double[gs*n];
		Gb = new double[m];

		lpCuts = new int*[2];
		for(int i=0;i<2;i++)
		{
			lpCuts[i] = new int[n_var];
			memset(lpCuts[i],0,sizeof(int)*n_var);
		}
		primals = new double[n_var];
		sumOpt = new double[n_var];
		memset(sumOpt,0,sizeof(double)*n_var);

		ip_Gb = new double[m];
		tempX = new int[n];
		freeVars = new int[n_var];
		bestSolVec = new int[n];
		f_best = 0;
		rp = new reduceProb[n];
		for(int i=0;i<n;i++)
		{
			rp[i].posi = new int[gs];
		}

		n_ct = n+m;
		primal_sol= new double[n_var];
		dual_sol = new double[n_ct];
		reducedCosts = new double[n_var];
		freeGroups = new int[n];
	}

	int *bestSolVec;
	double *sumOpt;
	void callCplexLp(int iter,int istCutVal);
	void ipPrepare();
	void callCplexIP(int lftTime);
	double callCplexMip();
	double calMaxRdCost(int &curFracCount, bool &isCompute);
	double calMaxRdCost();
	void finalAssign();
	void getReduceProb(double selParam);
	void write_solution_file(char *fileName, int T_max);

	//MMKP solvers
	void PEGF_solve(int k_start, int k_step, int t_max, int T_max);
	void PERC_solve(int k0, int t_max, int T_max,int rd_step);

	~MMKP()
	{
		for(int i=0;i<2;i++)
			delete lpCuts[i];
		for (int i=0;i<m;i++)
			delete GA[i];
		delete[] GC;
		delete[] Gb;
		delete[] primals;
		delete[] sumOpt;
		delete[] ip_Gb;
		delete[] tempX;
		delete[] freeVars;
		delete[] bestSolVec;
		delete[] rp;
		delete[] primal_sol;
		delete[] dual_sol;
		delete[] reducedCosts;
		delete[] freeGroups;
	}

	void read_input_file(const char *fileName);

};

