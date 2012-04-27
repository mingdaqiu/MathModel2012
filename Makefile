all:
	touch all

scae:simple_c_genetic_algorithm_example.c
	cc simple_c_genetic_algorithm_example.c -o scae

ga_sin:ga_sin.c
	cc ga_sin.c -lm -o ga_sin
