all:rlc_ga_main rlc_solver_main
	touch all

rlc_solver_main_dep=$(wildcard rlc_solver*.*)

rlc_solver_main:$(rlc_solver_main_dep)
	cc -lgsl -lgslcblas rlc_solver.c rlc_solver_main.c -o rlc_solver_main


rlc_fitting:double_array.c double_array.h rlc_fitting.c rlc_solver.c rlc_solver.h
	cc -lgaul -lgaul_util -lgsl -lgslcblas -Wl,--rpath -Wl,/usr/local/lib double_array.c rlc_fitting.c rlc_solver.c -o rlc_fitting

iw_fitting:double_array.c double_array.h iw_fitting.c
	cc -lgaul -lgaul_util -Wl,--rpath -Wl,/usr/local/lib double_array.c iw_fitting.c -o iw_fitting

