GSL_OPTS=-lgsl -lgslcblas
GAUL_OPTS=-lgaul -lgaul_util

all:rlc_ga_main rlc_solver_main
	touch all

rlc_solver_main_dep=$(wildcard rlc_solver*.*)

rlc_solver_main:$(rlc_solver_main_dep)
	cc $(GSL_OPTS) rlc_solver.c rlc_solver_main.c -o rlc_solver_main


rlc_fitting:double_array.c double_array.h rlc_fitting.c rlc_solver.c rlc_solver.h
	cc $(GAUL_OPTS) $(GSL_OPTS) double_array.c rlc_fitting.c rlc_solver.c -o rlc_fitting

iw_fitting:double_array.c double_array.h iw_fitting.c
	cc $(GAUL_OPTS) double_array.c iw_fitting.c -o iw_fitting


ran_gen:ran_gen.c
	cc ran_gen.c -o ran_gen
