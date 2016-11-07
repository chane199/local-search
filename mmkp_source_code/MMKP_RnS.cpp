
/***************************************************************************
                                 MMKP_RnS.cpp
 * Main funciton of the Executable for the "reduce and solve" approach for the Multiple-choice Multidimensional Knapsack Problem
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

int main(int argc,char *argv[])
{
	char in[MMKP_STRING_SIZE];

	int k_start,k_step, t_max, T_max,k0;

	double n_coef,m_coef,rd_step;
	int i;
	string algo_name;

	if(strncmp(argv[1],"PEGF",4)==0 && strcmp(argv[2],"-f")==0)
	{
		algo_name = "PEGF";
		k_start = 13;
		k_step = 3;
		t_max = 400;
		T_max = 3600;
		strncpy(in,argv[3],strlen(argv[3]));
		in[strlen(argv[3])]='\0';

		for(i=4;i<argc;++i)
		{
			if(strcmp(argv[i],"-t")==0)
			{
				t_max = atoi(argv[i+1]);
			}

			if(strcmp(argv[i],"-T")==0)
			{
				T_max = atoi(argv[i+1]);
			}

			if(strcmp(argv[i],"-ks")==0)
			{
				k_start = atoi(argv[i+1]);
			}

			if(strcmp(argv[i],"-kp")==0)
			{
				k_step = atoi(argv[i+1]);
			}
		}
	}
	else if(strncmp(argv[1],"PERC",4)==0 && strcmp(argv[2],"-f")==0)
	{
		algo_name = "PERC";
		k_start = 13;
		t_max = 900;
		T_max = 3600;
		n_coef = 1.2;
		m_coef = 0.5;
		rd_step = 1.0;
		strncpy(in,argv[3],strlen(argv[3]));
		in[strlen(argv[3])]='\0';

		for(i=4;i<argc;++i)
		{
			if(strcmp(argv[i],"-t")==0)
			{
				t_max = atoi(argv[i+1]);
			}

			if(strcmp(argv[i],"-T")==0)
			{
				T_max = atoi(argv[i+1]);
			}

			if(strcmp(argv[i],"-ks")==0)
			{
				k_start = atoi(argv[i+1]);
			}

			if(strcmp(argv[i],"-nc")==0)
			{
				n_coef = (double)(atof(argv[i+1]));
			}

			if(strcmp(argv[i],"-mc")==0)
			{
				m_coef = (double)(atof(argv[i+1]));
			}

			if(strcmp(argv[i],"-rs")==0)
			{
				rd_step = (double)(atof(argv[i+1]));
			}
		}
	}
	else
	{
		printf("Input error\n");
		printf("Usage:\n");
		printf("\t first parameter must be the name of the algorithm(either PEGF or PERC);\n");
		printf("\t second parameter must be the file name;\n");
	}
	


	if(algo_name == "PEGF")
	{
		int n = GetNumGroups(in);
		int gs = GetNumItemsInGroup(in);
		int m = GetNumConstraints(in);
		MMKP MM(n,gs,m);
		MM.read_input_file(in);
		MM.PEGF_solve(k_start, k_step, t_max, T_max);
		MM.write_solution_file(in,t_max);
	}

	if(algo_name == "PERC")
	{
		int n = GetNumGroups(in);
		int gs = GetNumItemsInGroup(in);
		int m = GetNumConstraints(in);
		MMKP MM(n,gs,m);
		MM.read_input_file(in);

		k0 = k_start + ((int)(log(pow(n_coef,(double)n))/log(10.0)+ m_coef*m) + 1);
		MM.PERC_solve(k0, t_max, T_max,rd_step);
		MM.write_solution_file(in,t_max);
	}
	
	return 0;
}

