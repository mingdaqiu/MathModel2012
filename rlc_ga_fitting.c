#include <stdio.h>
#include <string.h>
#include <memory.h>
#include <math.h>
#include <assert.h>

#include "rlc_solver.h"

#include "double_array.h"


#ifndef ushort
typedef unsigned short int ushort;
//#define UINT_BITSIZE 8*sizeof(ushort)
#define BYTE_SIZE	4
#endif

// Problem specific arguments
#define ARGLEN		3 // three parameters
#define ARGMAX		0x0f	
#define ARGMIN		0x00
#define POP_SIZE	0x20	
double ARGSCALE[3] = {1, 0.01, 0.01};

#define ELITRATE	0.1
#define MUTATIONRATE 0.5
#define MUTATION	RAND_MAX * MUTATIONRATE

typedef struct tagGenome
{
	double		fitness;
	ushort *	genome;
	int			generation;
} Genome;

typedef struct tagDataPoints
{
	double * x;
	double * y;
	int size;
} DataPoints;

void debug_print_genome(FILE * fOut, ushort * genome)
{
	int i,k;	
	char map[3] = {'R', 'L', 'C'};
	for(k = 0;k < ARGLEN; k++)
	{
		fprintf(fOut, "%c: ", map[k]);
		for(i = BYTE_SIZE; i >= 0; i--)
		{
			fprintf(fOut, "%d", (genome[k] >> i) & 0x01u);
		}
		fprintf(fOut, "\t%3.2lf\n", (genome[k]*ARGSCALE[k]));
	}
}

/**
 * params_given: E
 * params_genome: R,L,C
 * double x: time
 * init_value: U_0, U'_0
 * return_value: U_t, U'_t
 */
double target_function(double t, double * params_given, double * params_genome)
{
	int status;
	double E,C,L,R;
	double init_value[2];
	double return_value[2];
	double params[4];

	E = params_given[0];
	R = params_genome[0];
	L = params_genome[1];
	C = params_genome[2];

	init_value[0] = 0;
	init_value[1] = E/(C*L);

	params[0] = E;
	params[1] = R;
	params[2] = L;
	params[3] = C;
	status = rlc_solver(t, 0.0, init_value, return_value, params);

	return return_value[0];
}

void genome_translator(Genome * genome, double * params_genome)
{
	int i;
	ushort * p;
	ushort pattern;
	p = (short*)(genome->genome);
	short trans[ARGLEN];

	/*
	pattern = 0;
	for(i = 0; i < BYTE_SIZE - 1; i++)
		pattern |= (1 << i);	
	for(i = 0; i < ARGLEN; i++)
	{
		params_genome[i] = (p[i] & pattern) * ARGSCALE[i];
		params_genome[i] *= (p[i] & (1<<(BYTE_SIZE-1))) ? (-1) : 1;
	}
	*/
	for(i = 0; i < ARGLEN; i++)
		params_genome[i] = p[i] * ARGSCALE[i];

}

double square(double a)
{
	return a*a;
}

int sort_double_func(const void * d1, const void * d2)
{
	return (*((double*)d1)) > (*((double*)d2));
}


void cal_fitness(Genome * population, DataPoints * pdp, double * params_given) 
{
	//double A,B,C;	
	double params_genome[ARGLEN];
	int i,j,k;
	double * fitarr;
	double sum = 0;
	fitarr = (double*)malloc(pdp->size * sizeof(double));

	for(i = 0; i < POP_SIZE; i++)
	{
		sum = 0;

		population[i].fitness = 0;
		memset(params_genome, 0, sizeof(double)*ARGLEN);
		genome_translator(population+i, params_genome);
		memset(fitarr, 0, pdp->size * sizeof(double));
		for(k = 0; k < pdp->size; k++)
		{
			//fitarr[k] = square(pdp->y[k] - target_function(pdp->x[k], params_given, params_genome));
			sum += square(pdp->y[k] - target_function(pdp->x[k], params_given, params_genome));
			//fprintf(stderr, "A,B,C=%.3lf %.3lf %.3lf;x=%d; delta=%.3lf\n", A,B,C, pdp->x[k],
		//		fitarr[k]);
		}
		//qsort(fitarr, pdp->size, sizeof(double), sort_double_func);
		//fprintf(stderr, "\nfitarr %.3lf %.3lf %.3lf: ", A, B, C);
		//for(j = 0; j < pdp->size; j++)
		//{
			//fprintf(stderr, "%lf ", fitarr[j]);
		//}
		//fprintf(stderr, "\n");
		//population[i].fitness = fitarr[pdp->size/2];
		population[i].fitness = sum;

		//population[i].fitness *= B*B + 1;
	}
	free(fitarr);
}

/**
 * Initialize new random population
 */
void init_population(Genome * population, Genome * beta_population)
{
	int i,k, tmp;
	ushort * p;
	size_t alloc_size;
	alloc_size = (ARGLEN) * sizeof(ushort);
	for(i = 0; i < POP_SIZE; i++)
	{
		population[i].fitness = 0;
		population[i].genome = (ushort *)malloc(alloc_size);
		memset(population[i].genome, 0, alloc_size);

		p = population[i].genome;
		for(k = 0; k < ARGLEN; k++ )
		{
			p[k] = rand() % ARGMAX;
		}
		

		beta_population[i].fitness = 0;
		beta_population[i].genome = (ushort *)malloc(alloc_size);
		memset(beta_population[i].genome, 0, alloc_size);
	}
}


/**
 *
 * select elit element in top array after sorting
 *
 */
void elitism(Genome * population, Genome * beta_population, int esize)
{
	int i;
	for(i = 0; i < esize; i++)
	{
		memcpy(beta_population[i].genome, population[i].genome,
			sizeof(ushort) * (ARGLEN));
	}
}

/**
 * mutate
 */
void mutate(Genome * member)
{
	int tsize; // magic number
	int nom;

	int i,k;
	int apos;
	int * p;

	//tsize = 8;
	nom = rand() % 6 + 1;
	for(i = 0; i < nom; i++)
	{
		apos = rand()%BYTE_SIZE;	
		k = rand()%ARGLEN;
		member->genome[k] ^= (1 << apos);
	}
}

/**
 * mate randomly
 */


void mate(Genome * population, Genome * beta_population)
{

	////
	int m, i, k, i1,i2,pos;
	int esize;

	ushort tmp;
	//m = esize;
	i1 = -1;
	i2 = -1;
	pos = -1;
	esize = POP_SIZE * ELITRATE;

	//fprintf(stderr, "esize=%d\n", esize);
	elitism(population, beta_population, esize);


	// crossover
	for(m = esize; m < POP_SIZE; m++)
	{
		i1 = rand() % POP_SIZE;
		i2 = rand() % POP_SIZE;
		k = rand()  % ARGLEN;
		pos = rand() % (BYTE_SIZE);

		for(i = 0; i < k; i++)
		{
			beta_population[m].genome[i] =
				population[i1].genome[i];
		}
		for(i = k+1; i < ARGLEN; i++)
		{
			beta_population[m].genome[i] =
				population[i2].genome[i];
		}
		for(i = 0; i <= pos; i++)
		{
			beta_population[m].genome[k] |= 
				(population[i1].genome[k] & (0x01 << i));
		}
		for(i = pos+1; i < BYTE_SIZE; i++)
		{
			beta_population[m].genome[k] |= 
				(population[i2].genome[k] & (0x01 << i));
		}

		// mutation if necessary
		if(rand() < MUTATION)
		{
			mutate(&beta_population[m]);
		}
	}

}

//////////////////////
int sort_func(const void * e1, const void * e2)
{
	return (((Genome*)e1)->fitness > ((Genome*)e2)->fitness);
}


void sort_by_fitness(Genome * population)
{
	qsort(population, POP_SIZE, sizeof(Genome), sort_func);
}
///////////////////////

double init_data[45] = {
3.12500e-01, 4.74317e-02, -1.46722e+01,
6.25000e-01, -6.93778e-02, 1.07524e+01,
9.37500e-01, 7.60850e-02, -7.87174e+00,
1.25000e+00, -7.41426e-02, 5.75682e+00,
1.56250e+00, 6.77112e-02, -4.20572e+00,
1.87500e+00, -5.93520e-02, 3.06936e+00,
2.18750e+00, 5.05521e-02, -2.23765e+00,
2.50000e+00, -4.21671e-02, 1.62958e+00,
2.81250e+00, 3.46251e-02, -1.18543e+00,
3.12500e+00, -2.80604e-02, 8.61406e-01,
3.43750e+00, 2.25204e-02, -6.25036e-01,
3.75000e+00, -1.79155e-02, 4.53006e-01,
4.06250e+00, 1.41485e-02, -3.27924e-01,
4.37500e+00, -1.11043e-02, 2.37010e-01,
4.68750e+00, 8.65980e-03, -1.70935e-01,
};

int main(int argc, char ** argv)
{
	FILE * fData;
	int i,k;
	int * p;
	int generation;
	double params_given[1];
	double params_genome[3];
	// temp pointer for swap operation
	Genome * ptmp;
	int n;

	DataPoints dp;
	DoubleArray da_x;
	DoubleArray da_y;
	double x,y,z;

	double E, R, L, C;

	// Read Data into DataPoints
	if(argc > 1)
		fData  = fopen(argv[1], "r");
	else
		fData = fopen("data","r");

	assert(fData != NULL);

	DoubleArray_Initialize(&da_x);
	DoubleArray_Initialize(&da_y);
	fprintf(stderr, "Begin Reading Process\n");
	while(1)
	{
		n = fscanf(fData, "%lf %lf %lf\n", &x, &y, &z);
		if(n < 3)
			break;
		DoubleArray_Insert(&da_x, x);
		DoubleArray_Insert(&da_y, y);
	}

	assert(da_x.length == da_y.length);
	dp.size = da_x.length;
	dp.x = (double*)malloc(sizeof(double)*dp.size);
	dp.y = (double*)malloc(sizeof(double)*dp.size);
	memcpy(dp.x, da_x.data, dp.size * sizeof(double));
	memcpy(dp.y, da_y.data, dp.size * sizeof(double));


	
	//for(i = 0; i < dp.size;i++)
	//{
		//fprintf(stdout, "point: %lf %lf\n", dp.x[i], dp.y[i]);
	//}

	// init E
	E = 10.0;
	params_given[0] = E;

	// set random seed
	srand(time(NULL));

	Genome * population;
	Genome * beta_population;
	population = (Genome *)malloc(sizeof(Genome) * POP_SIZE);
	beta_population = (Genome *)malloc(sizeof(Genome) * POP_SIZE);

	init_population(population,beta_population);

	for(generation = 0; generation < 100; generation++)
	{
		fprintf(stderr, "Generation %d\n", generation);
		cal_fitness(population, &dp, params_given);
		//fprintf(stderr, "Done cal_fitness for generation %d\n", generation);
		sort_by_fitness(population);

		// display
		//i = 0;
		//for(i = 0; i < POP_SIZE; i++)
		for(i = 0; i < POP_SIZE/8; i++)
		{
			fprintf(stderr, "Geneation %d\tGenome %d\n", generation, i);

			//genome_translator(population + i, &A, &B, &C);
			genome_translator(population+i, params_genome);
			R = params_genome[0];
			L = params_genome[1];
			C = params_genome[2];
			//fprintf(stderr, "%02d: %.5lf, %.5lf, %.5lf", i, R,L,C);	
			debug_print_genome(stderr, population[i].genome);
			fprintf(stderr, "\t\t...Fitness: %lf\n", 
				population[i].fitness);
		}

		mate(population, beta_population);
		// Swap
		ptmp = population;
		population = beta_population;
		beta_population = ptmp;
		//memset(beta_population, 0, sizeof(Genome)*POP_SIZE);
	}

	return 0;
}
