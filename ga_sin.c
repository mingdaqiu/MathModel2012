/**
 * Genetic Algorithm for sine curve fitting
 * f(x) = Asin(Bx + C)
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>
#include <math.h>

#ifndef ushort
typedef unsigned short int ushort;
#define UINT_BITSIZE 8*sizeof(ushort)
#endif

// Problem specific arguments
#define ARGLEN		3 // three parameters
#define ARGACC		3 // accuracy
#define ARGMAX		0xff	
#define ARGMIN		-0xff
#define POP_SIZE	0x20	
#define ARGSCALE	0.01


#define ELITRATE	0.1
#define MUTATIONRATE 0.25
#define MUTATION	RAND_MAX * MUTATIONRATE

typedef struct tagGenome
{
	double		fitness;
	ushort *	genome;
	int			generation;
} Genome;

typedef struct tagDataPoints
{
	int * x;
	int * y;
	int size;
} DataPoints;

void debug_print_genome(FILE * fOut, ushort * genome)
{
	int i,k;	
	char map[3] = {'A', 'B', 'C'};
	for(k = 0;k < ARGLEN; k++)
	{
		fprintf(fOut, "%c: ", map[k]);
		for(i = 31; i >= 0; i--)
		{
			fprintf(fOut, "%d", (genome[k] >> i) & 0x01u);
		}
		fprintf(fOut, "\t%3.2lf\n", ((short)genome[k])*ARGSCALE);
	}
}



/**
 * function y = Asin(Bx + C)
 */
double target_function(double x, double A, double B, double C)
{
	//return A*sin(B*x + C);
	return A*x*x +B*x + C;
}

void genome_translator(Genome * genome, double * pA, double * pB, double * pC)
{
	short *p;
	p = (short*)(genome->genome);
	*pA = p[0]*ARGSCALE;
	*pB = p[1]*ARGSCALE;
	*pC = p[2]*ARGSCALE;
}

double square(double a)
{
	return a*a;
}

int sort_double_func(const void * d1, const void * d2)
{
	return (*((double*)d1)) > (*((double*)d2));
}


void cal_fitness(Genome * population, DataPoints * pdp) 
{
	double A,B,C;	
	int i,j,k;
	double * fitarr;
	fitarr = (double*)malloc(pdp->size * sizeof(double));

	for(i = 0; i < POP_SIZE; i++)
	{
		population[i].fitness = 0;
		genome_translator(population+i, &A, &B, &C);
		memset(fitarr, 0, pdp->size * sizeof(double));
		for(k = 0; k < pdp->size; k++)
		{
			fitarr[k] = square(pdp->y[k] - target_function(pdp->x[k], A,B,C));
			//fprintf(stderr, "A,B,C=%.3lf %.3lf %.3lf;x=%d; delta=%.3lf\n", A,B,C, pdp->x[k],
		//		fitarr[k]);
		}
		qsort(fitarr, pdp->size, sizeof(double), sort_double_func);
		//fprintf(stderr, "\nfitarr %.3lf %.3lf %.3lf: ", A, B, C);
		//for(j = 0; j < pdp->size; j++)
		//{
			//fprintf(stderr, "%lf ", fitarr[j]);
		//}
		//fprintf(stderr, "\n");
		population[i].fitness = fitarr[pdp->size-1];
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
	int * p;
	size_t alloc_size;
	alloc_size = (ARGLEN) * sizeof(ushort);
	for(i = 0; i < POP_SIZE; i++)
	{
		population[i].fitness = 0;
		population[i].genome = (ushort *)malloc(alloc_size);
		memset(population[i].genome, 0, alloc_size);

		p = (int*)(population[i].genome);
		for(k = 0; k < ARGLEN; k++ )
		{
			p[k] = rand() % (ARGMAX-ARGMIN+1) + ARGMIN;
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
	nom = rand() % 6;
	for(i = 0; i < nom; i++)
	{
		apos = rand()%UINT_BITSIZE;	
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
		pos = rand() % (UINT_BITSIZE);

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
		for(i = pos+1; i < UINT_BITSIZE; i++)
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

int main(int argc, char ** argv)
{
	double A,B,C;
	int i,k;
	int * p;
	int generation;

	int x[] = {0, 10, 20, 30};
	int y[] = {0, 10, 0, -10};
	DataPoints dp;
	dp.x = x;
	dp.y = y;
	dp.size = 4;
	Genome * ptmp;


	// set random seed
	srand(time(NULL));

	Genome * population;
	Genome * beta_population;
	population = (Genome *)malloc(sizeof(Genome) * POP_SIZE);
	beta_population = (Genome *)malloc(sizeof(Genome) * POP_SIZE);

	init_population(population,beta_population);

	for(generation = 0; generation < 10; generation++)
	{
		fprintf(stderr, "Generation %d\n", generation);
		cal_fitness(population, &dp);
		sort_by_fitness(population);

		// display
		i = 0;
		//for(i = 0; i < POP_SIZE; i++)
		{
			fprintf(stderr, "Geneation %d\tGenome %d\n", generation, i);

			genome_translator(population + i, &A, &B, &C);
			fprintf(stderr, "%02d: %.5lf, %.5lf, %.5lf", i, A,B,C);	
			//debug_print_genome(stderr, population[i].genome);
			fprintf(stderr, "\t\t...Fitness: %lf\n", 
				population[i].fitness);
		}

		mate(population, beta_population);
		/*
		i = 0;
		for(i = 0; i < POP_SIZE; i++)
		{
			fprintf(stderr, "beta population \t Genome %d\n",  i);
			debug_print_genome(stderr, beta_population[i].genome);
		}
		*/
		// Swap
		ptmp = population;
		population = beta_population;
		beta_population = ptmp;
		//memset(beta_population, 0, sizeof(Genome)*POP_SIZE);
		/*
		for(i = 0; i < POP_SIZE; i++)
		{
			memset(beta_population[i].genome, 0, sizeof(ushort)*ARGLEN);
			beta_population[i].fitness=0;
		}
		*/
	}

	return 0;
}
