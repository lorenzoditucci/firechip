#include <stdint.h>
#include <stdio.h>

#include "mmio.h"
#include "nic.h"
#include "memblade.h"

uint64_t page_data[512];

static inline int send_rmem_request(
		void *src_addr, void *dst_addr,
		uint64_t dstmac, int opcode, uint64_t pageno)
{
	reg_write64(RMEM_CLIENT_SRC_ADDR, (unsigned long) src_addr);
	reg_write64(RMEM_CLIENT_DST_ADDR, (unsigned long) dst_addr);
	reg_write64(RMEM_CLIENT_DSTMAC,   dstmac);
	reg_write8 (RMEM_CLIENT_OPCODE,   opcode);
	reg_write64(RMEM_CLIENT_PAGENO,   pageno);

	while (reg_read32(RMEM_CLIENT_NREQ) == 0) {}
	asm volatile ("fence");
	return reg_read32(RMEM_CLIENT_REQ);
}

static inline void wait_response(int xact_id)
{
	for (;;) {
		while (reg_read32(RMEM_CLIENT_NRESP) == 0) {}

		if (reg_read32(RMEM_CLIENT_RESP) == xact_id)
			return;
	}
}

int main(void)
{
	int start = 0, len;
	uint64_t mymac;
	uint64_t extdata[8];
	uint64_t word_results[3];
	uint64_t exp_results[3] = {0xBE, 1, 0xDEADB00FL};
	uint64_t pageno = 4;
	int xact_ids[4];

	for (int i = 0; i < 512; i++)
		page_data[i] = i;

	mymac = nic_macaddr();

	printf("Sending write request\n");

	xact_ids[0] = send_rmem_request(
			page_data, NULL, mymac, MB_OC_PAGE_WRITE, pageno);

	printf("Receiving write response\n");

	wait_response(xact_ids[0]);

	start = 0;
	for (int i = 0; i < 512; i++)
		page_data[i] = 0;

	printf("Sending read request\n");

	xact_ids[0] = send_rmem_request(
			NULL, page_data, mymac, MB_OC_PAGE_READ, pageno);

	printf("Receiving read response\n");

	wait_response(xact_ids[0]);
	asm volatile ("fence");

	printf("Checking read response\n");

	for (int i = 0; i < 512; i++) {
		if (page_data[i] != i)
			printf("Page data at %d not correct: got %d\n",
					i, page_data[i]);
	}

	printf("Sending word-sized requests\n");

	pageno = 5;
	extdata[0] = memblade_make_exthead(8, 2);
	extdata[1] = 0xDEADBEEFL;
	xact_ids[0] = send_rmem_request(
			&extdata[0], NULL, mymac, MB_OC_WORD_WRITE, pageno);

	extdata[2] = memblade_make_exthead(9, 0);
	extdata[3] = 5;
	xact_ids[1] = send_rmem_request(
			&extdata[2], &word_results[0], mymac,
			MB_OC_ATOMIC_ADD, pageno);

	extdata[4] = memblade_make_exthead(8, 1);
	extdata[5] = 0xB00F;
	extdata[6] = 0xC3EF;
	xact_ids[2] = send_rmem_request(
			&extdata[4], &word_results[1], mymac,
			MB_OC_COMP_SWAP, pageno);

	extdata[7] = memblade_make_exthead(8, 2);
	xact_ids[3] = send_rmem_request(
			&extdata[7], &word_results[2], mymac,
			MB_OC_WORD_READ, pageno);

	printf("Receiving word-sized responses\n");

	for (int i = 0; i < 4; i++)
		wait_response(xact_ids[i]);

	for (int i = 0; i < 3; i++) {
		if (word_results[i] != exp_results[i])
			printf("Word result %d incorrect: got %lx\n",
					i, word_results[i]);
	}

	printf("All tests completed successfully\n");

	return 0;
}