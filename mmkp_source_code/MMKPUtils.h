/***************************************************************************
                                 MMKPUtils.h
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

struct reduceProb
{
	int len;
	int curPosi;
	int *posi;
};
struct effisInGroup
{
	double value;
	int posi;
};

int GetNumGroups(char *fileName);
int GetNumItemsInGroup(char *fileName);
int GetNumConstraints(char *fileName);
int compare(const void *a, const void *b);
int toAscend(const void *a, const void *b);
