#include <stdio.h>
#include <string.h>
#include <memory.h>
#include <math.h>
#include <assert.h>

#include "rlc_solver.h"

/**
 * params: E, R,L,C
 * y[]: init_value at time 0
 * f[]: return value at time t
 */
int func(double t, const double y[], double f[], void * params)
{
	double E,R,L,C;
	double * p = (double *) params;
	E = p[0];
	R = p[1];
	L = p[2];
	C = p[3];
	
	f[0] = y[1];
	//f[1] = (U_MAX * U_FREQ * cos(U_FREQ * t) - y[0]/C)/L;
	f[1] = - 1.0/(R*C) * y[1] - y[0]/(L*C);
	
	return GSL_SUCCESS;
}

int jac(double t, const double y[], double * dfdy, double dfdt[], 
		void * params)
{
	double E,R,L,C;
	double * p = (double *) params;
	E = p[0];
	R = p[1];
	L = p[2];
	C = p[3];

	gsl_matrix_view dfdy_mat = gsl_matrix_view_array(dfdy, 2, 2);
	gsl_matrix * m = &dfdy_mat.matrix;
	gsl_matrix_set (m,0,0,0.0);
	gsl_matrix_set (m,0,1,1.0);
	gsl_matrix_set (m,1,0,-1.0/(C*L));
	gsl_matrix_set (m,1,1,-1.0/(R*C));

	// don't used 
	dfdt[0] = 0;
	dfdt[1] = 0;
	
	return GSL_SUCCESS;
}

///
/// t,y initial point
/// params contain C and L
///
/// after execution, y will be modified
//

/**
 * y[]: init value at time t
 * result[]: return value at time ti
 * params: E,R,L,C
 *
 */
int rlc_solver(double ti, double t, const double y[], double result[],  double params[])
{
	double C, L;
	int i;
	int status;

	gsl_odeiv2_system sys = {func, jac, 2, params};
	gsl_odeiv2_driver * d = gsl_odeiv2_driver_alloc_y_new
		(&sys, gsl_odeiv2_step_rk8pd, 1e-3,1e-3,0.0);

	memcpy(result, y, 2*sizeof(double));
	
	//params_translator(params, &C, &L);

	status = gsl_odeiv2_driver_apply(d, &t, ti, result);
	if (status != GSL_SUCCESS)
	{
		fprintf(stderr, "error, return value = %d\n", status);
		return 1;
	}
	//fprintf(stdout, "%.5e %.5e %.5e\n", t, result[0], result[1]);

	gsl_odeiv2_driver_free(d);
	return 0;
}
