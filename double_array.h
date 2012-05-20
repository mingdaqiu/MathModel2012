#ifndef _DOUBLE_ARRAY_H_
#define _DOUBLE_ARRAY_H_

#define DOUBLEARRAY_DEF_INIT_SIZE 32
#define DOUBLEARRAY_DEF_DELTA_SIZE 16

typedef struct tagDoubleArray{
    double * data;
    int length;
    int size;
} DoubleArray;

void DoubleArray_Initialize(DoubleArray * pda);
DoubleArray * DoubleArray_Insert(DoubleArray * p,double n);
void DoubleArray_Release(DoubleArray * pda);


#endif
