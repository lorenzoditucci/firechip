#include "rocc.h"
#include <stdio.h>

#define ITERS 32

#define SHARED_REG 1
#define GLOBAL_REG 2
#define SHARED_REG 0

#define NUM_GLOBAL_REGS 16
#define NUM_SHARED_REGS 20
#define NUM_SHARED_REGS 8

#define NUM_PES 150
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
	asm volatile ("fence");
}

static inline void ALSA_nop(){
	ROCC_INSTRUCTION_SS(0,0,0,1);
}


static inline void ALSA_load(void *ptr, unsigned char dest_pe, unsigned char dest_reg, unsigned char reg_type){
	uint64_t rs1 = 0;
	//printf("SIZEOF uint64_t = %d \n", sizeof(uint64_t));
	//printf("SIZEOF unsigned long int = %d \n", sizeof(unsigned long int));
	//printf("dest_pe %lu \n", ((uint64_t) dest_pe) << 40);
	
	rs1 = (uintptr_t)ptr;
	//printf("[DEBUG] RS1 VAL %lu \n", rs1);
	rs1 = rs1 & 1099511627775; //24 initial bits need to be zero
	rs1 |= (((uint64_t) dest_pe) << 40);
	//printf("[DEBUG] RS1 VAL %lu \n", rs1);
	rs1 |= (((uint64_t) dest_reg) << 48);
	//printf("[DEBUG] RS1 VAL %lu \n", rs1);	
	rs1 |= (((uint64_t) reg_type) << 53);
	//printf("[DEBUG] RS1 VAL %lu \n", rs1);

	//printf("[DEBUG] RS1 VAL %lu \n", rs1);
	asm volatile ("fence");
	ROCC_INSTRUCTION_SS(1, (uint64_t)rs1, (uintptr_t) ptr, 0);	
	//asm volatile ("fence");

}
unsigned long data = 0x3421L;

int main(void)
{
	unsigned long result;
	result = 134;
	//int a = 37 + 47;
	//printf("DEBUG sizeof(uintptr_t) = %d \n", sizeof(uintptr_t));
	
	unsigned int array[ITERS];
	for(int i = 0; i < ITERS; i++)
		array[i] = 73; //keep it constant for DEBUG

	//dest pe
	//dest reg
	ALSA_load(&array[0], 0, 7, SHARED_REG);
	ALSA_load(&array[1], 10, 7, SHARED_REG);
	ALSA_load(&array[2], 20, 7, SHARED_REG);
	ALSA_load(&array[0], 30, 7, SHARED_REG);
	ALSA_load(&array[0], 40, 7, SHARED_REG);
	ALSA_load(&array[0], 50, 7, SHARED_REG);
	ALSA_load(&array[0], 60, 7, SHARED_REG);
	ALSA_load(&array[0], 70, 7, SHARED_REG);
	ALSA_load(&array[0], 80, 7, SHARED_REG);
	ALSA_load(&array[0], 90, 7, SHARED_REG);
	ALSA_load(&array[0], 100, 7, SHARED_REG);
	ALSA_load(&array[0], 110, 7, SHARED_REG);
	ALSA_load(&array[0], 120, 7, SHARED_REG);
	ALSA_load(&array[0], 130, 7, SHARED_REG);
	ALSA_load(&array[0], 140, 7, SHARED_REG);
	ALSA_load(&array[0], 149, 7, SHARED_REG);

	
	//printf("37 = %d \n", a);

	//asm volatile ("fence");
	//ALSA_load(&result, 131, 7, 2);

	printf("Success\n");

	return 0;
}
