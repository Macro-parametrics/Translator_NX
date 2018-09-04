#include "stdafx.h"
#include "Definition.h"

#include <fstream>

int report_error(char *file, int line, char *call, int irc)
{
	if (irc)
	{
		char err[133];
		UF_get_fail_message(irc, err);
		fprintf(stderr, "\n%s\nerror %d at line %d in %s\n%s\n",
			err, irc, line, file, call);
	}
	return(irc);
}
/*
// ValRndPnt : small values in 3D Point to 0s.
void ValRndPnt(double C[3])
{
int size = 3;
for (int i = 0 ; i<size ; ++i) {
if ( (C[i]<0.000001)&&(C[i]>-0.000001) ) C[i] = 0;
}
}
// near zero -> 0
double ValRnd(double tmp)
{
if ( (tmp<0.000001)&&(tmp>-0.000001) ) return 0;
else return tmp;
}
*/

// ValRndPnt function make small value 3D Point zero
void ValRndPnt(double C[3])
{
	int size = 3;
	for (int i = 0; i<size; ++i) C[i] = ValRnd(C[i]);
}

// make a value 0 that is near to 0.
double ValRnd(double tmp)
{
	if (abs(tmp) < 0.000001) return 0.0;
	else if (tmp>0)
	{
		unsigned int temN = (unsigned int)tmp;
		if ((tmp - (double)temN) < 0.000001) return (double)temN;
		else if (((double)temN + 1.0 - tmp) < 0.000001) return (double)(temN + 1);
	}
	else if (tmp<0)
	{
		long int tmpN = (long int)tmp;
		if (((double)tmpN - tmp) < 0.000001) return (double)tmpN;
		else if ((tmp - (double)tmpN + 1.0) < 0.000001) return (double)(tmpN - 1);
	}
	return tmp;
}

// ValRndPnt function make small value 3D Point zero
void ValRndPnt6(double C[3])
{
	int size = 3;
	for (int i = 0; i<size; ++i) C[i] = ValRnd6(C[i]);
}
// make a value 0 that is near to 0.
double ValRnd6(double tmp)
{
	double under = tmp - (int)tmp;
	long ui = (long)(under * 1000000);
	tmp = double((int)tmp + ((double)ui)*0.000001);
	return tmp;
}

// 반올림 함수
extern double ValRnd9(double val)
{
	double rest;               // 나머지 부분
	double digit = pow(10.0, 9);   // 자릿 수
   
	val = val * digit;
      
	rest = val - floor(val);

	if(rest >= 0.5)   // 나머지가 0.5보다 크면 올림, 작으면 내림
		val = ceil(val);
	else
		val = floor(val);
      
	val /= digit;

	return val;
}

int WaitGetEnter(void)
{
	//char c;	scanf("%c", &c);
	return true;
}

// Translate a Point from WC to LC
void Map(const double A[12], double B[3])
{
	double C[3];
	double D[3];

	D[0] = A[0] * A[9] + A[1] * A[10] + A[2] * A[11];
	D[1] = A[3] * A[9] + A[4] * A[10] + A[5] * A[11];
	D[2] = A[6] * A[9] + A[7] * A[10] + A[8] * A[11];

	C[0] = A[0] * B[0] + A[1] * B[1] + A[2] * B[2] - D[0];
	C[1] = A[3] * B[0] + A[4] * B[1] + A[5] * B[2] - D[1];
	C[2] = A[6] * B[0] + A[7] * B[1] + A[8] * B[2] - D[2];

	ValRndPnt(C);

	B[0] = C[0];
	B[1] = C[1];
	B[2] = C[2];
}

// Translate a arc_center from WC to LC
void Cir_Map(const double A[12], double C[3])
{
	double D[3];

	D[0] = A[0] * A[9] + A[1] * A[10] + A[2] * A[11];
	D[1] = A[3] * A[9] + A[4] * A[10] + A[5] * A[11];
	D[2] = A[6] * A[9] + A[7] * A[10] + A[8] * A[11];

	C[0] = C[0] - D[0];
	C[1] = C[1] - D[1];
	C[2] = C[2] - D[2];

	ValRndPnt(C);
}

// Translate a Point from LC to WC : 2016. 7. 5 - YK
void Map2(const double A[12], double B[3])
{
	/** A[0~2]	: X direction of local coordinate
		A[3~5]	: Y direction of local coordinate
		A[6~8]	: Global origin
		A[9~11] : Sketch local origin **/

	double C[3];	// Cross product of X dir and Y dir
	
	C[0] = A[1] * A[5] - A[2] * A[4];
	C[1] = A[2] * A[3] - A[0] * A[5];
	C[2] = A[0] * A[4] - A[1] * A[3];

	/** Inverse matrix **/
	double D[9];

	double det;
	det = A[0] * (A[4] * C[2] - A[5] * C[1]) - A[1] * (A[3] * C[2] - A[5] * C[0]) + A[2] * (A[3] * C[1] - A[4] * C[0]);

	D[0] = (A[4] * C[2] - A[5] * C[1]) / det;
	D[1] = (A[2] * C[1] - A[1] * C[2]) / det;
	D[2] = (A[1] * A[5] - A[2] * A[4]) / det;
	D[3] = (A[5] * C[0] - A[3] * C[2]) / det;
	D[4] = (A[0] * C[2] - A[2] * C[0]) / det;
	D[5] = (A[2] * A[3] - A[0] * A[5]) / det;
	D[6] = (A[3] * C[1] - A[4] * C[0]) / det;
	D[7] = (A[1] * C[0] - A[0] * C[1]) / det;
	D[8] = (A[0] * A[4] - A[1] * A[3]) / det;

	/** LC to WC **/
	B[0] = (D[0] * A[9] + D[1] * A[10] + D[2] * A[11]) + A[6];
	B[1] = (D[3] * A[9] + D[4] * A[10] + D[5] * A[11]) + A[7];
	B[2] = (D[6] * A[9] + D[7] * A[10] + D[8] * A[11]) + A[8];	
}