#include <stdio.h>
#include <gsl/gsl_errno.h>
#include <gsl/gsl_matrix.h>
#include <gsl/gsl_odeiv2.h>

int func(double t, const double y[], double f[], void * params)
{
	//double mu = *(double*)params;
	f[0] = y[0];
	return GSL_SUCCESS;
}

int jac(double t, const double y[], double * dfdy, double dfdt[], 
		void * params)
{
	//double mu = *(double *)params;
	gsl_matrix_view dfdy_mat = gsl_matrix_view_array(dfdy, 1, 1);
	gsl_matrix * m = &dfdy_mat.matrix;
	gsl_matrix_set (m,0,0,0.0);
	//dfdt[0] = y[0];
	dfdt[0] = 0;
	
	return GSL_SUCCESS;
}

int main(void)
{
	double mu = 10;
	gsl_odeiv2_system sys = {func, jac, 1, &mu};
	gsl_odeiv2_driver * d = gsl_odeiv2_driver_alloc_y_new
		(&sys, gsl_odeiv2_step_rk8pd, 1e-3,1e-3,0.0);
	
	int i;
	double t = 0.0;
	double t1 = 10.0;
	//double y[2] = {1.0, 0.0};
	double y[1] = {1.0};
	double ti;
	int status;

	for(i = 1; i <= 100; i ++)
	{
		ti = i * t1/100.0;
		status = gsl_odeiv2_driver_apply(d, &t, ti, y);
		if (status != GSL_SUCCESS)
		{
			fprintf(stderr, "error, return value = %d\n", status);
			break;
		}
		//printf("%.5e %.5e %.5e\n", t, y[0], y[1]);
		printf("%.5e %.5e\n", t, y[0]);
	}
	gsl_odeiv2_driver_free(d);
	return 0;
}
