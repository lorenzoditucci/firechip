#include "rocc.h"
#include <stdio.h>
/*
static inline void accum_write(int idx, unsigned long data)
{
	ROCC_INSTRUCTION_SS(0, data, idx, 0);
}

static inline unsigned long accum_read(int idx)
{
	unsigned long value;
	ROCC_INSTRUCTION_DSS(0, value, 0, idx, 1);
	return value;
}

static inline void accum_load(int idx, void *ptr)
{
	asm volatile ("fence");
	ROCC_INSTRUCTION_SS(0, (uintptr_t) ptr, idx, 2);
}

static inline void accum_add(int idx, unsigned long addend)
{
	ROCC_INSTRUCTION_SS(0, addend, idx, 3);
}

static inline void accum_tuccio(int idx, unsigned long addend)
{
	ROCC_INSTRUCTION_SS(0, addend, idx, 4);
}
*/
static inline void fixed_operation(void *ptr, int idx)
{
	asm volatile ("fence");
	ROCC_INSTRUCTION_SS(0, (uintptr_t)ptr, idx, 0);
}

unsigned long data = 0x3421L;

int main(void)
{
	unsigned long result;

	//accum_tuccio(0, 12);
	printf("PRE-DATA = %d \n", data);
	int a = 37 + 47;
	for(int i = 0; i < 33; i++){
	asm("fence");
	fixed_operation(&data, 3);
	asm("fence");
	}
	printf("POST-DATA = %d \n", data);
	printf("POST-DATA = %d \n", data);
	printf("POST-DATA = %d \n", data);
	printf("37 = %d \n", a);

	printf("Success\n");

	return 0;
}
