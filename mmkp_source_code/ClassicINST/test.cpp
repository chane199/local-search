#include<iostream>
#include<stdlib.h>
#include<stdio.h>
int main(){
	int **p;
	p=new int*[3];
	for(int i=0;i<3;i++){
		p[i]=new int[4];
		for(int j=0;j<4;j++) p[i][j]=i*j;
	}
	for(int i=0;i<3;i++){
		delete []p[i];
	}
	delete p;
}
