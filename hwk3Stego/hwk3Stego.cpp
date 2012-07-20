// hwk3Stego.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <cmath>

void calculateDCTbasis(double (&basis)[8][8][8][8]);
void populatefourDimArray(double (&basis)[8][8][8][8]);
void calcDCT(unsigned char (&twoDArr)[8][8] );

int main(int argc, char* argv[])
{
	const int u = 8;
	const int v = 8;
	const int x = 8;
	const int y = 8;

	unsigned char charArr[][8] = 
	{
		84,  9,  5,  6, 14, 15, 27, 28,
		2,  4,  7, 13, 16, 26, 29, 42,
		3,  8, 12, 17, 25, 30, 41, 43,
		9, 11, 18, 24, 31, 40, 44, 53,
		10, 19, 23, 32, 39, 45, 52, 54,
		20, 21, 33, 38, 46, 51, 55, 60,
		21, 34, 37, 47, 50, 56, 59, 61,
		35, 36, 48, 49, 57, 58, 62, 63
	};

	double basis[u][v][x][y];
	populatefourDimArray(basis);

	calculateDCTbasis(basis);

	calcDCT(charArr);

	return 0;
}

void calculateDCTbasis(double (&basis)[8][8][8][8] ){
	
	const double PI = 3.1416;
	double dctSum;
	double cn;
	double result;

	double basisCpy[8][8][8][8];
	memcpy(basisCpy, basis, 4096*sizeof(double));

	
	//basisCpy = basis;
	
	for(int u = 0; u < 8; u++) // 
	{
		for(int v = 0; v < 8; v++)
		{
			for(int a = 0; a < 8; a++)
			{
				for(int b = 0; b < 8; b++)
				{
					result = 0; // reset summed results to 0
							double x = 1;
							double y = 1;
							if (a == 0)
							{
								x = 1/sqrt(2.0);
							} 
							if (b == 0)
							{
								y = 1/sqrt(2.0);
							} 

					for(int i = 0; i < 8; i++)
					{
						for(int j = 0; j < 8; j++)
						{
							

							result = result + ( basis[u][v][i][j] *
								cos(((PI*u)/(2*8))*(2*i + 1))*
								cos(((PI*v)/(2*8))*(2*j + 1))
								);
						}
					}
					basisCpy[u][v][a][b] = ((x*y)/4) * result;
				}
			}
		}
	}

	memcpy(basis, basisCpy, 4096*sizeof(double));
}

void populatefourDimArray(double (&basis)[8][8][8][8])
{
	for(int u = 0; u < 8; u++)
	{
		for(int v = 0; v < 8; v++)
		{
			for(int x = 0; x < 8; x++)
			{
				for(int y = 0; y < 8; y++)
				{
					basis[u][v][x][y] = 1;
				}
			}
		}
	}
}

void calcDCT(unsigned char (&twoDArr)[8][8] )
{
	double matrix4Dim[8][8][8][8];

	for(int x = 0; x < 8; x++)
	{
		for(int y = 0; y < 8; y++)
		{
			matrix4Dim[0][0][x][y] = twoDArr[x][y] - 128;
		}
	}

	calculateDCTbasis(matrix4Dim);

	for(int x = 0; x < 8; x++)
	{
		for(int y = 0; y < 8; y++)
		{
			printf("%f ", matrix4Dim[0][0][x][y]);
		}
		printf("\n ");
	}
}

