#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "mmio.h"

#define N 4

uint64_t values[N];

int main(void)
{
        int a[N] = {0};

        for (int i = 0; i < N; i++){
        	printf("%d \n ", a[i] + 110);
        }

        return 0;
}