#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "ch.h"
#include "hal.h"
#include "memory_protection.h"
#include <main.h>

#include "leds.h"
#include "spi_comm.h"

// 2022-10-19 : task1


int main(void)
{

    halInit();
    chSysInit();
    mpu_init();

    clear_leds();
    spi_comm_start();




    /* Infinite loop. */
    while (1) {
    	//waits 1 second
    	
        set_led(LED1,1);
        chThdSleepMilliseconds(1000);
        set_led(LED1,0);
        chThdSleepMilliseconds(1000);
    }
}

#define STACK_CHK_GUARD 0xe2dee396
uintptr_t __stack_chk_guard = STACK_CHK_GUARD;

void __stack_chk_fail(void)
{
    chSysHalt("Stack smashing detected");
}
