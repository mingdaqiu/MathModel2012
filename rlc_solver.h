#ifndef _RLC_SOLVER_H_
#define _RLC_SOLVER_H_

#include <gsl/gsl_errno.h>
#include <gsl/gsl_matrix.h>
#include <gsl/gsl_odeiv2.h>

int rlc_solver(double ti, double t, const double y[], double result[],  double params[]);

#endif /*_RLC_SOLVER_H_*/
