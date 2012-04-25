/**
 * Genetic Algorithm for sine curve fitting
 * f(x) = Asin(Bx + C)
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>
#include <math.h>

#ifndef uint
typedef unsigned int uint;
#endif

// Problem specific arguments
#define ARGLEN		3 // three parameters
#define ARGACC		3 // accuracy
#define ARGMAX		20
#define ARGMIN		-20
#define POP_SIZE	20	
#define ARGSCALE	0.1

#define ELITRATE	0.1
#define MUTATIONRATE 0.25
#define MUTATION	RAND_MAX * MUTATIONRATE

typedef struct tagGenome
{
	double	fitness;
	uint *	genome;
	int		generation;
} Genome;

typedef struct tagDataPoints
{
	int * x;
	int * y;
	int size;
} DataPoints;

/**
 * function y = Asin(Bx + C)
 */
double target_function(double x, double A, double B, double C)
{
	return A*sin(B*x + C);
}

void genome_translator(Genome * genome, double * pA, double * pB, double * pC)
{
	int *p;
	p = (int*)genome->genome;
	*pA = p[1]*ARGSCALE;
	*pB = p[2]*ARGSCALE;
	*pC = p[3]*ARGSCALE;
}

double square(double a)
{
	return a*a;
}

void cal_fitness(Genome * population, DataPoints * pdp) 
{
	double A,B,C;	
	int i,j,k;
	for(i = 0; i < POP_SIZE; i++)
	{
		population[i].fitness = 0;
		genome_translator(population+i, &A, &B, &C);
		for(k = 0; k < pdp->size; k++)
		{
			population[i].fitness += square(pdp->y[k] - target_function(pdp->x[k], A,B,C));
		}
	}
}

/**
 * Initialize new random population
 */
void init_population(Genome * population, Genome * beta_population)
{
	int i,k, tmp;
	int * p;
	size_t alloc_size;
	alloc_size = (ARGLEN) * sizeof(uint);
	for(i = 0; i < POP_SIZE; i++)
	{
		population[i].fitness = 0;
		population[i].genome = (uint *)malloc(alloc_size);
		memset(population[i].genome, 0, alloc_size);

		p = (int*)(population[i].genome);
		for(k = 0; k < ARGLEN; k++ )
		{
			p[k] = rand() % (ARGMAX-ARGMIN+1) + ARGMIN;
		}
		

		beta_population[i].fitness = 0;
		beta_population[i].genome = (uint *)malloc(alloc_size);
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
			sizeof(sizeof(uint)*ARGLEN));
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

	tsize = 8;
	nom = rand() % 6;
	for(i = 0; i < nom; i++)
	{
		apos = rand()%tsize;	
		k = rand()%3;
		member->genome[k] ^= 1 << apos;
	}
}

/**
 * mate randomly
 */


void mate(Genome * population, Genome * beta_population)
{
	int esize = POP_SIZE * ELITRATE;

	////
	elitism(population, beta_population, esize);
	int m, i, k, i1,i2,pos,tsize;
	uint tmp;
	m = esize;
	i1 = -1;
	i2 = -1;
	pos = -1;
	tsize = sizeof(uint);



	// crossover
	for(m = esize; m < POP_SIZE; m++)
	{
		i1 = rand() % POP_SIZE;
		i2 = rand() % POP_SIZE;
		k = rand()  % 3;
		pos = rand() % sizeof(uint);

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
		tmp = 0;
		for(i = 0; i <= k; i++)
		{
			beta_population[m].genome[k] |= 
				(population[i1].genome[k] & (0x01 << k));
		}
		for(i = k+1; i < tsize; i++)
		{
			beta_population[m].genome[k] |= 
				(population[i2].genome[k] & (0x01 << k));
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

int main(int argc, char ** argv)
{
	double A,B,C;
	int i,k;
	int * p;
	int generation;

	int x[] = {0, 1, 2};
	int y[] = {0, 1, 0};
	DataPoints dp;
	dp.x = x;
	dp.y = y;
	dp.size = 3;
	Genome * ptmp;


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
		cal_fitness(population, &dp);
		sort_by_fitness(population);

		// display
		//i = 0;
		for(i = 0; i < POP_SIZE; i ++)
		{
			//fprintf(stderr, "Geneation %d\tGenome %d\n", generation, i);

			genome_translator(population + i, &A, &B, &C);
			fprintf(stderr, "%02d: %.2lf, %.2lf, %.2lf", i, A,B,C);	
			fprintf(stderr, "\t\t...Fitness: %lf\n", 
				population[i].fitness);
		}
		mate(population, beta_population);
		// Swap
		ptmp = population;
		population = beta_population;
		beta_population = ptmp;
	}

	return 0;
}
