//
//  main.c
//  数模-结构1
//
//  Created by Lv Yiding on 12-5-21.
//  Copyright (c) 2012年 __MyCompanyName__. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "double_array.h"


int main(){
    DoubleArray da;
    int n,k;
    DoubleArray_Initialize(&da);
    for (n=0; n<=34; ++n) {
        DoubleArray_Insert(&da,n);
    }
    for (k=0; k<=34; ++k) {
        printf("%f\n",(&da)->data[k]);
    }
    return 0;
}
