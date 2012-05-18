all:
	touch all

scae:simple_c_genetic_algorithm_example.c
	cc simple_c_genetic_algorithm_example.c -o scae

ga_sin:ga_sin.c
	cc ga_sin.c -lm -o ga_sin

gsl_test:gsl_test.c
	cc -lgsl -lgslcblas gsl_test.c -Wl,--rpath -Wl,/usr/local/lib -o gsl_test

gsl_test_odeiv2:gsl_test_odeiv2.c
	cc -lgsl -lgslcblas gsl_test_odeiv2.c -Wl,--rpath -Wl,/usr/local/lib -o gsl_test_odeiv2
simple_ode:simple_ode.c
	cc -lgsl -lgslcblas simple_ode.c -Wl,--rpath -Wl,/usr/local/lib -o simple_ode
ele_test:ele_test.c
	cc -lgsl -lgslcblas ele_test.c -Wl,--rpath -Wl,/usr/local/lib -o ele_test
