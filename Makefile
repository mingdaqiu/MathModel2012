all:rlc_ga_main rlc_solver_main
	touch all

rlc_solver_main_dep=$(wildcard rlc_solver*.*)

rlc_solver_main:$(rlc_solver_main_dep)
	cc -lgsl -lgslcblas rlc_solver.c rlc_solver_main.c -Wl,--rpath -Wl,/usr/local/lib -o rlc_solver_main


rlc_ga_main:rlc_ga_fitting.c rlc_solver.c double_array.c
	cc -lgsl -lgslcblas rlc_solver.c rlc_ga_fitting.c double_array.c -Wl,--rpath -Wl,/usr/local/lib -o rlc_ga_main

double_array_test:double_array_test.c double_array.c double_array.h
	cc double_array_test.c double_array.c -o double_array_test

fitting:double_array.c double_array.h fitting.c rlc_solver.c rlc_solver.h
	cc -lgaul -lgaul_util -lgsl -lgslcblas -Wl,--rpath -Wl,/usr/local/lib double_array.c fitting.c rlc_solver.c -o fitting

fitting_char:double_array.c double_array.h fitting_char.c rlc_solver.c rlc_solver.h
	cc -lgaul -lgaul_util -lgsl -lgslcblas -Wl,--rpath -Wl,/usr/local/lib double_array.c fitting_char.c rlc_solver.c -o fitting_char

iw_fitting:double_array.c double_array.h iw_fitting.c
	cc -lgaul -lgaul_util -Wl,--rpath -Wl,/usr/local/lib double_array.c iw_fitting.c -o iw_fitting
