
/***************************************************************************
                                 MMKP.cpp
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

#include "MMKPH.h"

void MMKP::read_input_file(const char *fileName)
{
	int i,j,k,grp;
	ifstream fread;
	fread.open(fileName);
	if ( fread.fail() )
    {
        printf("Unable to open input file for reading\n");
        exit(-1);
    }
	fread >> n;
	fread >> gs;
	fread >> m;

	//constants of the equations
	for(i=0;i<m;i++)
		fread>>Gb[i];

	for(i=0;i<n;i++)
	{
		fread>>grp;
		for(j=gs*i;j<gs*(i+1);j++)
		{
			fread>>GC[j];
			for(k=0;k<m;k++)
				fread>>GA[k][j];
		}
	}
	fread.close();
}


void MMKP::getReduceProb(double maxRdc)
{
	int i,g,id;
	for(i=0;i<free_len;i++)
	{
		g = freeGroups[i];
		rp[g].len = 0;
	}
	effisInGroup *rdCostSort = new effisInGroup[gs];
	for(id=0;id<free_len;id++)
	{
		g = freeGroups[id];
		for(i=0;i<gs;i++)
		{
			rdCostSort[i].posi = g*gs+i;
			rdCostSort[i].value = reducedCosts[g*gs+i];
		}

		qsort(rdCostSort,gs,sizeof(effisInGroup),toAscend);

		for(i=0;i<gs;i++)
		{
			if(rdCostSort[i].value <= maxRdc)
			{
				rp[g].posi[rp[g].len++] = rdCostSort[i].posi;
			}
			else
			{
				break;
			}
		}
	}
	delete[] rdCostSort;
}


void MMKP::ipPrepare()
{
	ip_n_var = free_len*gs;
	curObj = 0;
	memcpy(ip_Gb,Gb,sizeof(double)*m);
	for(int i=0;i<n;i++){
		if(tempX[i]!=0){
			curObj += GC[tempX[i]];
			for(int k=0;k<m;k++){
				ip_Gb[k] -= GA[k][tempX[i]];
			}
		}
	}
}

void MMKP::callCplexLp(int iter,int kk)
{
	int i,j,g;
	IloEnv env;
	try {
		IloModel model(env);

		//*********create a lp model************
		IloNumVarArray x(env,n_var,0,1);
		IloRangeArray con(env);
		//write objective function
		IloExpr obj(env);
		for(i=0;i<n_var;i++)
			obj += GC[i]*x[i];
		model.add(IloMaximize(env,obj));
		obj.end();
		//write constraints
		for(i=0;i<m;i++)
		{
			IloExpr expr(env);
			for(j=0;j<n_var;j++)
			{
				expr += GA[i][j]*x[j];
			}
			con.add(expr<=Gb[i]);
			expr.end();
		}
		for(g=0;g<n;g++)
		{
			IloExpr expr(env);
			for(i=0;i<gs;i++)
			{
				expr += x[g*gs+i];
			}
			con.add(expr==1);
			expr.end();
		}
		//add lpCuts
		for(i=0;i<iter;i++)
		{
			IloExpr expr(env);
			for(j=0;j<n_var;j++)
			{
				if(lpCuts[i][j]==1)
				{
					expr += (1-x[j]);
				}
				else if(lpCuts[i][j]==2)
				{
					expr += x[j];
				}
			}
			con.add(expr >= kk);
			expr.end();
		}

		model.add(con);


		//*******end create a lp model**********

		IloCplex cplex(env);
		cplex.extract(model);
		cplex.setOut(env.getNullStream());
		cplex.setParam(IloCplex::Threads,1);
		if (cplex.solve())
		{

			for(i=0;i<n_var;i++)
				primals[i] = (double)cplex.getValue(x[i]);

			if(iter==0)
			{
				lp_value = cplex.getObjValue();
				//By recording the first lp optimum in primal_sol, we only solve LP relaxation of the original model for one time.
				memcpy(primal_sol,primals,sizeof(double)*n_var);
				for(i=0;i<n_ct;i++)
					dual_sol[i] = (double)cplex.getDual(con[i]);
				for(i=0;i<n_var;i++)
					reducedCosts[i] = MMKPH_ABS((double)cplex.getReducedCost(x[i]));

				nb_intGrps=0;
				for(g=0;g<n;g++)
				{
					for(i=0;i<gs;i++)
					{
						if(primals[g*gs+i] > EPSILON && primals[g*gs+i] < 1 - EPSILON)
						{
							++nb_intGrps;
							break;
						}
					}
				}
				nb_intGrps = n-nb_intGrps;
			}

			for(i=0;i<n_var;i++)
				sumOpt[i] += primals[i];

			for(g=0;g<n;g++)
			{
				//For integer group, the value of each item is either 0 or 1 in the first lp optimum. In lpCuts, we use 2 to
				//record the item with a value of 0 in the first lp optimum, and 1 to record the item with a value of 1.
				for(i=0;i<gs;i++)
				{
					if(primals[g*gs+i] < EPSILON)
						lpCuts[iter][g*gs+i] = 2;
					else if(primals[g*gs+i] > 1 - EPSILON)
						lpCuts[iter][g*gs+i] = 1;
					else
					{
						break;
					}
				}
				//For fractional group, each value of lpCuts in this group is recorded as zero;
				if(i!=gs)
				{
					for(j=0;j<i;j++)
						lpCuts[iter][g*gs+j] = 0;
				}
			}
		}
		con.end();
		model.end();
		cplex.end();
	}
	catch (IloException& ex) {
      cerr << "Error: " << ex << endl;
	}
	catch (...) {
      cerr << "Error" << endl;
	}
	env.end();
}


void MMKP::callCplexIP(int lftTime)
{
	int i,j,k,g;
	ipPrepare();
	IloEnv env;
	try {
		IloModel model(env);
		IloIntVarArray x(env,ip_n_var,0,1);
		IloRangeArray con(env);
		//write objective function
		IloExpr obj(env);
		for(i=0;i<freeVarNum;i++){
			obj += GC[freeVars[i]]*x[i];
		}
		model.add(IloMaximize(env,obj));
		obj.end();
		for(k=0;k<m;k++)
		{
			IloExpr expr(env);
			for(int i=0;i<freeVarNum;i++){
				expr += GA[k][freeVars[i]]*x[i];
			}
			con.add(expr<=ip_Gb[k]);
			expr.end();
		}
		for(g=0;g<free_len;g++){
			IloExpr expr(env);
			for(i=0;i<gs;i++)
			{
				expr += x[g*gs+i];
			}
			con.add(expr==1);
			expr.end();
		}
		int *tmpFix = new int[n_var];
		memset(tmpFix,0,sizeof(int)*n_var);
		for(i=0;i<free_len;i++){
			g = freeGroups[i];
			for(j=0;j<rp[g].len;j++){
				tmpFix[rp[g].posi[j]] = 1;
			}
		}
		for(i=0;i<freeVarNum;i++){
			if(tmpFix[freeVars[i]]==0){
				IloExpr expr(env);
				expr += x[i];
				con.add(expr==0);
				expr.end();
			}
		}
		delete[] tmpFix;

		model.add(con);
		//*******end create a mip model**********
		IloCplex cplex(env);
		cplex.extract(model);
		cplex.setOut(env.getNullStream());
		cplex.setParam(IloCplex::TiLim, lftTime);
		cplex.setParam(IloCplex::Threads,1);
		if (cplex.solve())
		{
			double rs = cplex.getObjValue()+curObj;
			if(rs > f_best)
			{
				f_best = rs;
				for(i=0;i<freeVarNum;i++){
					if(cplex.getValue(x[i])==1){
						tempX[freeGroups[i/gs]] = freeVars[i];
					}
				}
				memcpy(bestSolVec,tempX,sizeof(int)*n);
			}
		}
		con.end();
		model.end();
		cplex.end();
	}
	catch (IloException& ex) {
      cerr << "Error: " << ex << endl;
	}
	catch (...) {
      cerr << "Error" << endl;
	}
	env.end();
}



double MMKP::callCplexMip()
{
	int i,j,g;
	double mipBest = MAX;
	IloEnv env;
	try {
		IloModel model(env);
		IloNumVarArray x(env);

		int *tmpFix = new int[n_var];
		memset(tmpFix,0,sizeof(int)*n_var);
		for(i=0;i<free_len;i++){
			g = freeGroups[i];
			for(j=0;j<rp[g].len;j++){
				tmpFix[rp[g].posi[j]] = 1;
			}
		}
		for(i=0;i<n_var;i++)
		{
			if(tmpFix[i]==1)
			{
				IloNumVar y(env,0,1,IloNumVar::Int);
				x.add(y);
			}

			if(tmpFix[i]==0)
			{
				IloNumVar y(env,0,1);
				x.add(y);
			}
		}
		delete[] tmpFix;

		IloRangeArray con(env);
		//write objective function
		IloExpr obj(env);
		for(i=0;i<n_var;i++)
			obj += GC[i]*x[i];
		model.add(IloMaximize(env,obj));
		obj.end();
		//write constraints
		for(i=0;i<m;i++)
		{
			IloExpr expr(env);
			for(j=0;j<n_var;j++)
			{
				expr += GA[i][j]*x[j];
			}
			con.add(expr<=Gb[i]);
			expr.end();
		}
		for(g=0;g<n;g++)
		{
			IloExpr expr(env);
			for(i=0;i<gs;i++)
			{
				expr += x[g*gs+i];
			}
			con.add(expr==1);
			expr.end();
		}
		model.add(con);


		//*******end create a lp model**********

		IloCplex cplex(env);
		cplex.extract(model);
		cplex.setOut(env.getNullStream());
		cplex.setParam(IloCplex::Threads, 1);
		//cplex.setParam(IloCplex::TiLim, 15);

		if (cplex.solve())
		{
			mipBest = cplex.getObjValue();
		}
		con.end();
		model.end();
		cplex.end();
	}
	catch (IloException& ex) {
      cerr << "Error: " << ex << endl;
	}
	catch (...) {
      cerr << "Error" << endl;
	}
	env.end();
	return mipBest;
}


void MMKP::finalAssign()
{
	int i,j,g,base,start,end;
	bool isFixed;
	memset(tempX,0,sizeof(int)*n);
	free_len = 0;
	for(g=0;g<n;g++)
	{
		isFixed = false;
		for(i=0;i<gs;i++)
		{
			if(sumOpt[g*gs+i]>2-EPSILON)
			{
				tempX[g] = g*gs+i;
				isFixed = true;
				break;
			}
		}
		if(!isFixed)
		{
			freeGroups[free_len++] = g;
		}
	}

	freeVarNum = free_len*gs;
	for(i=0;i<free_len;i++){
		base = freeGroups[i]*gs;
		start = i*gs;
		end = (i+1)*gs;
		for(j=start;j<end;j++){
			freeVars[j] = base++;
		}
	}
}


double MMKP::calMaxRdCost()
{
	double maxRdc = 0, curRdc;
	for(int i=0;i<n_var;i++)
	{
		if(sumOpt[i]>EPSILON && sumOpt[i]<2-EPSILON)
		{
			curRdc = MMKPH_ABS(reducedCosts[i]);
			maxRdc = MMKPH_MAX(curRdc,maxRdc);
		}
	}

	return maxRdc;
}

double MMKP::calMaxRdCost(int &curFracCount, bool &isCompute)
{
	double maxRdc = 0, curRdc;
	int itgCount = 0;
	for(int i=0;i<n_var;i++)
	{
		if(sumOpt[i]>EPSILON && sumOpt[i]<2-EPSILON)
		{
			curRdc = MMKPH_ABS(reducedCosts[i]);
			maxRdc = MMKPH_MAX(curRdc,maxRdc);
		}

		if(sumOpt[i]>2-EPSILON)
		{
			itgCount ++;
		}
	}

	if(n-itgCount > curFracCount)
	{
		curFracCount = n-itgCount;
		isCompute = true;
	}
	else
		isCompute = false;

	return maxRdc;
}


void MMKP::write_solution_file(char *fileName, int t_max)
{
	ostringstream sin;
	sin << t_max;
	const char *d = ".";
	char *p;
	p = strtok(fileName,d);
	string tempFileName(p);
	tempFileName = tempFileName + "." + sin.str() + ".sol";
	char *tf = new char[tempFileName.size()+1];
	tf[tempFileName.size()] = 0;
	memcpy(tf,tempFileName.c_str(),tempFileName.size());
	ofstream fout;
	fout.open(tf);
	fout<<"Best upper bound:"<<up_bound<<endl;
	fout<<"Best lower bound:"<<f_best<<endl;
	fout<<"Best solution vector:"<<endl;
	for(int i=0;i<n;++i)
	{
		fout<<this->bestSolVec[i]<<" ";
	}
	fout.close();
}

