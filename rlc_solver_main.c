#include <stdio.h>
#include <string.h>
#include <memory.h>
#include <assert.h>

#include "rlc_solver.h"

int main(int argc, char ** argv)
{
	double E,R,L,C;
	double E_GIVEN = 10;
	double t = 0;
	double y[2];
	double result[2];
	double params[4];
	int i,j;
	double te = 5;
	double ti;

	assert(argc >= 5);
	E = atof(argv[1]);
	R = atof(argv[2]);
	L = atof(argv[3]);
	C = atof(argv[4]);

	params[0] = E;
	params[1] = R;
	params[2] = L;
	params[3] = C;

	y[0] = 0.0;
	y[1] = E/(R*C);
	for(i = 0; i < 100; i++)
	{
		ti = (te - t)*i/100 + t;	
		rlc_solver(ti, t, y, result, params);

		fprintf(stdout, "%.5e %.5e %.5e\n", ti, result[0], result[1]);
	}
}
