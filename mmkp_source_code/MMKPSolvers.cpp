
/***************************************************************************
                                 MMKPSolver.cpp
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

void MMKP::PEGF_solve(int k_start, int k_step, int t_max, int T_max)
{
	time_t start, end, dif;
	int curFracCount = 0,kk;
	double maxRdc,curUb;
	bool isCompute;
	start = clock();
	//First linear relaxation
	callCplexLp(0,0);
	up_bound = lp_value;


	//Fist stopping criterion: k should be less than the number of integer groups;
	for(int k= k_start;k<=nb_intGrps;k+=k_step)
	{
		kk = 2*k;
		callCplexLp(1,kk);
		maxRdc = calMaxRdCost(curFracCount,isCompute);
		if(isCompute)
		{
			finalAssign();
			getReduceProb(maxRdc);

			curUb = callCplexMip();

			up_bound = MMKPH_MIN(curUb,up_bound);
			end = clock();
			dif = end - start;
			int leftTime = T_max-(int)(dif*1.0/CLOCKS_PER_SEC);
			//Second stopping criterion: time is not over;
			if(leftTime<=0)
				break;
			leftTime = MMKPH_MIN(leftTime,t_max);
			callCplexIP(leftTime);

			//Third stopping criterion: upper bound equals lower bound;
			if(MMKPH_ABS(up_bound-f_best) < 1)
				break;
		}
		memcpy(sumOpt,primal_sol,sizeof(double)*n_var);
	}
}


void MMKP::PERC_solve(int k0, int t_max, int T_max,int rd_step)
{
	time_t start, end, dif;
	start = clock();
	callCplexLp(0,0);

	up_bound = lp_value;
	k0 =  MMKPH_MIN(k0,nb_intGrps);
	int leftTime;
	int isCutVal = 2*k0;
	callCplexLp(1,isCutVal);
	double curUb;
	double maxRdc = calMaxRdCost();
	while(maxRdc<(up_bound-f_best))
	{
		finalAssign();
		getReduceProb(maxRdc);

		curUb = callCplexMip();

		up_bound = MMKPH_MIN(curUb,up_bound);

		end = clock();
		dif = end - start;
		leftTime = T_max-(int)(dif*1.0/CLOCKS_PER_SEC);
		if(leftTime<=0)
			break;
		leftTime = MMKPH_MIN(leftTime,t_max);
		callCplexIP(leftTime);

		maxRdc += rd_step;
	}
}

