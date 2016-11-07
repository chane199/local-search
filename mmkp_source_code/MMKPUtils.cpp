
/***************************************************************************
                                 MMKPUtils.cpp
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

int GetNumGroups(char *fileName)
{
	int n;
	ifstream fread;
	fread.open(fileName);
	if ( fread.fail() )
	{
		printf("Unable to open input file for reading\n");
		exit(-1);
	}
	fread >> n;
	return n;
}

int GetNumItemsInGroup(char *fileName)
{
	int g;
	ifstream fread;
	fread.open(fileName);
	if ( fread.fail() )
	{
		printf("Unable to open input file for reading\n");
		exit(-1);
	}
	fread >> g;
	fread >> g;
	return g;
}

int GetNumConstraints(char *fileName)
{
	int m;
	ifstream fread;
	fread.open(fileName);
	if ( fread.fail() )
	{
		printf("Unable to open input file for reading\n");
		exit(-1);
	}
	fread >> m;
	fread >> m;
	fread >> m;
	return m;
}


int compare(const void *a, const void *b)
{
	effisInGroup *s1;
	effisInGroup *s2;

	s1 = (effisInGroup *)a;
	s2 = (effisInGroup *)b;

	if(s1->value > s2->value)
		return 1;
	else
		return -1;
}

int toAscend(const void *a, const void *b)
{
	effisInGroup *s1;
	effisInGroup *s2;

	s1 = (effisInGroup *)a;
	s2 = (effisInGroup *)b;

	if( s1->value > s2->value)
		return 1;
	else
		return -1;
}
