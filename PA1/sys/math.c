/* math.c */

#include<stdio.h>
#include<math.h>

#define RAND_MAX 32767

double pow( double x, int y)
{
	double ret=1;
	int i;
	for (i=1;i<=y;i++)
	{
		ret = ret * x;
	}
	return ret;
}

double log(double x)
{
	double ret = 0;
	x = x-1;
	int i;
	for (i=1;i<=20;i++) {
		ret = ret + (pow(-1,i+1))*((pow(x,i))/i);
	}
	return ret;
}

double expdev(double lamb)
{
	double ret;
	do
	{
		ret = (double) rand() / RAND_MAX;
	}while (ret == 0.0);
	return (-log(ret)/lamb);
}
