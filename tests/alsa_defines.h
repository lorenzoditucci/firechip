#ifdef ALSA_DEFINE_H
#define ALSA_DEFINE_H

#define LOAD_INSTR 0
#define COMPUTE_SW_INSTR 2

#define PRIVATE_REG 1
#define GLOBAL_REG 2
#define SHARED_REG 0

#define NUM_GLOBAL_REGS 16
#define NUM_PRIVATE_REGS 20
#define NUM_SHARED_REGS 8

#define NUM_PES 150

static inline void ALSA_load(void *ptr, unsigned char dest_pe, unsigned char dest_reg, unsigned char reg_type){
	uint64_t rs1 = 0;
	
	rs1 = (uintptr_t)ptr;
	rs1 = rs1 & 1099511627775; //24 initial bits need to be zero
	rs1 |= (((uint64_t) dest_pe) << 40);
	rs1 |= (((uint64_t) dest_reg) << 48);
	rs1 |= (((uint64_t) reg_type) << 53);
	asm volatile ("fence");
	ROCC_INSTRUCTION_SS(1, (uint64_t)rs1, (uintptr_t) ptr, 0);	
}

static inline void ALSA_computeSW(){
	
	asm volatile ("fence");
	ROCC_INSTRUCTION_SS(1, 0, 0, COMPUTE_SW_INSTR);	
}
#endif