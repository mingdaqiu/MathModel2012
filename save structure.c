//
//  main.c
//  数模-结构1
//
//  Created by Lv Yiding on 12-5-21.
//  Copyright (c) 2012年 __MyCompanyName__. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>

struct doublearrow;
typedef struct doublearrow *parrow;
parrow initial(void);
parrow insert(parrow p,double n);

struct doublearrow{
    double *data;
    int length;
    int size;
};

parrow initial(void){//初始化一个结构
    parrow p;
    p->length=0;
    p->size=20;
    p->data=(double*)malloc(20*sizeof(double));
    memset(p->data,0,sizeof(double)*20);
    if (p->data!=NULL) {
        return p;
    }
    else return NULL;
}

parrow insert(parrow p,double n){//向结构中插入新的数值
    double *q;
    if (p->length<p->size) {
        p->data[p->length]=n;
        ++p->length;
    }
    else{
        q=(double*)realloc(p->data, (p->size+20)*sizeof(double));
        if (q!=NULL) {
            p->data=q;
            free(q);
        }
        else return NULL;
        p->size+=20;
        p->data[p->length]=n;
        ++p->length;
    }
    return p;
}
