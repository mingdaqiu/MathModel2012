#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "double_array.h"

void DoubleArray_Initialize(DoubleArray * pda)
{
    DoubleArray * p = pda;
    p->length = 0;
    p->size = DOUBLEARRAY_DEF_INIT_SIZE;
    p->data = (double*)malloc(p->size * sizeof(double));
	assert(p->data != NULL);
    memset(p->data, 0, p->size * sizeof(double));
}

DoubleArray * DoubleArray_Insert(DoubleArray * p, double n)
{
    double *q;
    if (p->length<p->size) {
        p->data[p->length]=n;
        ++p->length;
    }
    else
	{
        q = (double*)realloc(p->data, 
			(p->size + DOUBLEARRAY_DEF_DELTA_SIZE)*sizeof(double));
        if (q != NULL) 
		{
            p->data = q;
            free(q);
        }
        else return NULL;
        p->size+=DOUBLEARRAY_DEF_DELTA_SIZE;
        p->data[p->length]=n;
        ++(p->length);
    }
    return p;
}

void DoubleArray_Release(DoubleArray * pda)
{
	free(pda->data);
}
