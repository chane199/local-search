
/***************************************************************************
                                 MMKPH.h
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

#include<stdio.h>
#include<stdlib.h>
#include<ctime>
#include<math.h>
#include<fstream>
#include<iostream>
#include<iomanip>
#include<algorithm>
#include<string.h>
#include <sstream>
#include<cstring>
#include "ilcplex/ilocplex.h"
#include "ilcplex/cplex.h"

#include "MMKPUtils.h"
#include "MMKP.h"


#define EPSILON 0.000000001
#define MAX 2147483647
#define MMKP_STRING_SIZE 100

#define MMKPH_MIN(X,Y)  ((X) < (Y) ? (X) : (Y))
#define MMKPH_MAX(X,Y)  ((X) < (Y) ? (Y) : (X))
#define MMKPH_ABS(X)  (((X) < 0) ? -(X) : (X))

ILOSTLBEGIN
typedef IloArray<IloBoolVarArray> BoolVarMatrix;
using namespace std;
