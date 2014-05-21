/*
 * libprussdrv and data acquisition test example
 *
 * GPL v3
 */

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#include <prussdrv.h>
#include <pruss_intc_mapping.h>

#include <sys/mman.h>

#include <time.h>

void *extmem, *pru0ram, *pru1ram;

#include "PRUTestFirmware_bin.h"

void load_firmware(char *name) {
	/* Look in normal directory first, else use */
}

int main(void) {

	int ret;
	uint32_t *ptr;

	tpruss_intc_initdata initdata = PRUSS_INTC_INITDATA;

	puts("PRU Application test\n"); /* print Hello, world */

	/* Set up the PRU */
	prussdrv_init();

	/* Set up EVTOUT0 and EVTOUT1 */
	if ((ret = prussdrv_open(PRU_EVTOUT_0)) != 0) {
		printf("prussdrv_open failed! Error code=%d", ret);
		return ret;
	}

	if ((ret = prussdrv_open(PRU_EVTOUT_1)) != 0) {
		printf("prussdrv_open failed! Error code=%d", ret);
		return ret;
	}

	/* Initialize INTC */
	prussdrv_pruintc_init(&initdata);

	/* Map external (DDR) memory to the PRU. We have already configured
	 * UIO driver with extram_pool_sz
	 */
	prussdrv_map_extmem(&extmem);
	prussdrv_map_prumem(PRUSS0_PRU0_DATARAM, &pru0ram);
	prussdrv_map_prumem(PRUSS0_PRU1_DATARAM, &pru1ram);

	if (prussdrv_exec_program(0, "./pru0fw.bin") == 0) {
		puts("Loaded PRU firmware for PRU0");
	}

	if (prussdrv_exec_program(1, "./pru1fw.bin") == 0) {
		puts("Loaded PRU firmware for PRU1.");
	}

	ret = prussdrv_pru_wait_event(PRU_EVTOUT_0);
	prussdrv_pru_clear_event(PRU_EVTOUT_0, PRU0_ARM_INTERRUPT);
	printf("PRU0 Received Interrupt from PRU1 \n");

	ret = prussdrv_pru_wait_event(PRU_EVTOUT_1);
	prussdrv_pru_clear_event(PRU_EVTOUT_1, PRU1_ARM_INTERRUPT);
	printf("Received Interrupt from PRU1 \n");
	//prussdrv_pru_clear_event(PRU_EVTOUT_1, PRU1_ARM_INTERRUPT);

	if (extmem != 0) {
		ptr = pru0ram;
		printf("Received in PRU0 ram: %d, \n", *ptr);

		ptr = ptr + 1;
		printf("Received in PRU1 ram: %d, \n", *ptr);

		ptr = ptr + 1;
		printf("Received in PRU1 ram: %d, \n", *ptr);
	}

	return EXIT_SUCCESS;
}
