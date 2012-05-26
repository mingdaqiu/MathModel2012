/**********************************************************************
  fitting.c
 **********************************************************************

  fitting - Test/example program for GAUL.
  Copyright ©2002-2004, Stewart Adcock <stewart@linux-domain.com>
  All rights reserved.

  The latest version of this program should be available at:
  http://gaul.sourceforge.net/

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.  Alternatively, if your project
  is incompatible with the GPL, I will probably agree to requests
  for permission to use the terms of any other license.

  This program is distributed in the hope that it will be useful, but
  WITHOUT ANY WARRANTY WHATSOEVER.

  A full copy of the GNU General Public License should be in the file
  "COPYING" provided with this distribution; if not, see:
  http://www.gnu.org/

 **********************************************************************

  Synopsis:	Test/example program for GAUL.

		This program aims to fit a function of the form
		y = Ax exp{Bx+C} + D
		through an input dataset.

  Last Updated:	19 Mar 2004 SAA	Custom mutation function since existing function produced mutations that were far too significant.
		21 Nov 2002 SAA	New seeding function.
		17 Nov 2002 SAA	Initial version.

 **********************************************************************/

#include "rlc_solver.h"
#include "double_array.h"

#include "gaul.h"

/*
 * Datastructure used to demonstrate attachment of data to specific
 * populations.
 * It is used to store the training data.
 */

typedef struct
{
	int size;
	double	*x;
	double	*y;
} fitting_data_t;

void debug_message(const char * message)
{
	fprintf(stderr, message);
}

#define E_GIVEN 10.0

#define ARGLEN	3
const double ARGSCALE[ARGLEN] = {1.0, 0.01, 0.01};

/**
 * params_c: R, L, C, untranlated
 * params_d: E, R, L, C in double
 *
 */
int chromo_translator(const char * params_c, double * params_d)
{
	int i;
	// R,L,C
	for(i = 0; i < ARGLEN; i++)
		params_d[i+1]= ((unsigned short)(params_c[i]) + 1) * ARGSCALE[i];

	params_d[0] = E_GIVEN;

	return 1; // success
}

int sort_double_func(const void * d1, const void * d2)
{
	return (*((double*)d1)) > (*((double*)d2));
}

/**
 * params_given: E
 * params_genome: R,L,C
 * double x: time
 * init_value: U_0, U'_0
 * return_value: U_t, U'_t
 */
double target_function(double t, double * params)
{
	int status;
	double E,C,L,R;
	double init_value[2];
	double return_value[2];

	E = params[0];
	R = params[1];
	L = params[2];
	C = params[3];

	init_value[0] = 0.0;
	init_value[1] = E/(R*C);

	status = rlc_solver(t, 0.0, init_value, return_value, params);

	return return_value[0];
}

boolean fitting_score(population *pop, entity *entity)
{
	int			i;		/* Loop variable over training points. */
	double		score=0.0;	/* Mean of squared deviations. */
	char *		params;
	double		params_d[4];	/* Fitting parameters. */
	fitting_data_t	*data;		/* Training data. */
	double E,R,L,C;

	entity->fitness = 0;

	data = (fitting_data_t *)pop->data;
	params = (char *)entity->chromosome[0];

	chromo_translator(params, params_d);
	E = params_d[0];
	R = params_d[1];
	L = params_d[2];
	C = params_d[3];

	for (i=0; i<data->size; i++)
	{
		if(score > 10000000) 
		{
			score = 10000000;
			break;
		}
		score += SQU(data->y[i]-(target_function(data->x[i], params_d)));
	}

	entity->fitness = -sqrt(score / data->size);
	return TRUE;
}


/**********************************************************************
  fitting_generation_callback()
  synopsis:	Generation callback
  parameters:
  return:
  updated:	17 Nov 2002
 **********************************************************************/

boolean fitting_generation_callback(int generation, population *pop)
{
	double params_d[4];
	chromo_translator((char*)(pop->entity_iarray[0]->chromosome[0]), params_d);

	fprintf(stdout, "%d: E=%8.2lf\tR=%8.2lf\tL=%4.2lf\tC=%4.2lf\t\tFitness: %lf\n",
            generation,
            params_d[0],
            params_d[1],
            params_d[2],
            params_d[3],
            pop->entity_iarray[0]->fitness );

	return TRUE;
}


/**********************************************************************
  get_data()
  synopsis:	Read training data from standard input.
  parameters:
  return:
  updated:	17 Nov 2002
 **********************************************************************/

void get_data(FILE * fIn, fitting_data_t * data)
{
	int n;
	//int line_count = 0;
	DoubleArray da_x;
	DoubleArray da_y;
	double x,y;

	
	DoubleArray_Initialize(&da_x);
	DoubleArray_Initialize(&da_y);

	while(1)
	{
		n = fscanf(fIn, "%lf %lf\n", &x, &y);
		if(n < 2)
			break;
		DoubleArray_Insert(&da_x, x);
		DoubleArray_Insert(&da_y, y);
	}

	assert(da_x.length == da_y.length);
	//dp.size = da_x.length;
	data->size = da_x.length;
	data->x = (double*)malloc(sizeof(double)*data->size);
	data->y = (double*)malloc(sizeof(double)*data->size);
	memcpy(data->x, da_x.data, data->size * sizeof(double));
	memcpy(data->y, da_y.data, data->size * sizeof(double));
}

/**********************************************************************
  main()
  synopsis:	Main function.
  parameters:
  return:
  updated:	17 Nov 2002
 **********************************************************************/

int main(int argc, char **argv)
{
	population		*pop;			/* Population of solutions. */
	fitting_data_t	data;	/* Training data. */
	FILE * fData;

	random_seed(time(NULL));
	double params_d[4];
	double x;


	// fitting
	if(argc >= 3 && strcmp(argv[1], "fit") == 0)
	{

		pop = ga_genesis_char(
		   100,				/* const int              population_size */
		   1,				/* const int              num_chromo */
		   3,				/* const int              len_chromo */
		   fitting_generation_callback,	/* GAgeneration_hook      generation_hook */
		   NULL,				/* GAiteration_hook       iteration_hook */
		   NULL,				/* GAdata_destructor      data_destructor */
		   NULL,				/* GAdata_ref_incrementor data_ref_incrementor */
		   fitting_score,			/* GAevaluate             evaluate */
		   //fitting_seed,			/* GAseed                 seed */
		   ga_seed_char_random,
		   NULL,				/* GAadapt                adapt */
		   //ga_select_one_linearrank,	
		   ga_select_one_random,	/* GAselect_one           select_one */
		   ga_select_two_random,	/* GAselect_two           select_two */
		   ga_mutate_char_singlepoint_randomize,/* GAmutate               mutate */
		   ga_crossover_char_allele_mixing,/* GAcrossover            crossover */
		   NULL,				/* GAreplace              replace */
		   NULL				/* vpointer	User data */
				);

		ga_population_set_parameters(
		   pop,				/* population      *pop */
		   GA_SCHEME_DARWIN,		/* const ga_scheme_type     scheme */
		   GA_ELITISM_PARENTS_DIE,		/* const ga_elitism_type   elitism */
		   0.8,				/* double  crossover */
		   0.8,				/* double  mutation */
		   0.0      		        /* double  migration */
								  );

		fData = fopen("data", "r");

		debug_message("Begin Get Data\n");
		get_data(fData, &data);
		pop->data = &data;
		debug_message("Done Get Data\n");

		debug_message("Begin Evolution\n");
		ga_evolution(
			pop,				/* population	*pop */
			200				/* const int	max_generations */
		);
		debug_message("Done Evolution\n");

		ga_extinction(pop);
	}
	// plot
	else if(argc >= 5 && strcmp(argv[1], "plot")==0)
	{
		params_d[0] = E_GIVEN;			
		params_d[1] = atof(argv[2]);
		params_d[2] = atof(argv[3]);
		params_d[3] = atof(argv[4]);

		for(x = 0; x < 10; x += 0.01)
		{
			fprintf(stdout, "%lf %lf\n", x, target_function(x, params_d));	
		}
	}
	else
	{
		fprintf(stdout, "usage: %s [plot/fit] [params]\n", argv[0]);
	}
}


