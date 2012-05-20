#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include "double_array.h"

void DoubleArray_Initialize(DoubleArray * pda)
{
    DoubleArray * p = pda;
    p->length = 0;
    p->size = DOUBLEARRAY_DEF_INIT_SIZE;
    p->data = (double*)malloc(p->size * sizeof(double));
	assert(p->data != NULL);
    memset((void*)(p->data), 0, p->size * sizeof(double));
}

DoubleArray * DoubleArray_Insert(DoubleArray * p, double n)
{
    double *q;
	// Longer
	while(p->length >= p->size)
	{
		q = (double*)malloc(
			(p->size + DOUBLEARRAY_DEF_DELTA_SIZE) 
				* sizeof(double));
        if (q != NULL) 
		{
			memcpy(q, p->data, p->size*sizeof(double));
			free(p->data);
            p->data = q;
        }
        else 
		{
			fprintf(stderr, "Run out of memory\n");
			return NULL;
		}

        p->size+=DOUBLEARRAY_DEF_DELTA_SIZE;
	}

	p->data[p->length]=n;
	++(p->length);

    return p;
}

void DoubleArray_Release(DoubleArray * pda)
{
	free(pda->data);
}
