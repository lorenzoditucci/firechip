#include "rocc.h"
#include <stdio.h>

#define ITERS 32

#define PRIVATE_REG 1
#define GLOBAL_REG 2
#define SHARED_REG 0

#define NUM_GLOBAL_REGS 16
#define NUM_PRIVATE_REGS 20
#define NUM_SHARED_REGS 8

#define NUM_PES 150

static inline void fixed_operation(void *ptr, int idx)
{
	asm volatile ("fence");
	ROCC_INSTRUCTION_SS(0, (uintptr_t)ptr, idx, 0);
	asm volatile ("fence");
}

static inline void ALSA_nop(){
	ROCC_INSTRUCTION_SS(0,0,0,1);
}

static inline void ALSA_computeSW(void *ptr){
	
	asm volatile ("fence");
	ROCC_INSTRUCTION_SS(1, (uintptr_t)ptr, 0, 2);	
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

	unsigned int query[12];
	unsigned int db[12];
	int scoringsystem[4];

	unsigned long int results[5] = {0};

	for(int i = 0; i < 12; i++){
		query[i] = i % 4;
		db[i] = i % 4;
		ALSA_load(&query[i], i, 3, PRIVATE_REG);
		ALSA_load(&db[i], i, 2, PRIVATE_REG);
	}

	scoringsystem[0] = 2; //match
	scoringsystem[1] = -1; //mismatch
	scoringsystem[2] = 1; //gapd
	scoringsystem[3] = 1; //gapi

	for(int i = 0; i < 4; i++){
		ALSA_load(&scoringsystem[i], 0, i, GLOBAL_REG);
	}

	//for(int i = 0; i < 10; i++)
	ALSA_computeSW(&results);
	asm volatile ("fence");

	/*Check correctnes..*/
	int val1;
	if(query[0] == db[0]){
		val1 = scoringsystem[0];
	}else{
		val1 = scoringsystem[1];
	}
	int val2 = scoringsystem[2];
	int val3 = scoringsystem[3];
	int dir = 0;
	if(val1 > 0 && val1 >= val2 && val1 >= val3){
		dir = 2;
	}else if(val2 > 0 && val2 >= val3){
		dir = 1;
	}else if(val3 > 0){
		dir = 3;
	}else{
		dir = 0;
	}

	printf("tutto = %lu \n", results[0]);
	printf("%u \n", (results[0]) & (unsigned long int)3);
	//for(int i =0; i < 12; i++){
	//	printf("%d \n", (results[0] >> i*2) & 3);
	//}
	if(dir != (unsigned)(results[0] & 3)){
		printf("Error dir = %d\n", dir);
	}else{
		printf("Success\n");
	}

	return 0;
}
