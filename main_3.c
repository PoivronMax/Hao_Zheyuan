#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "ch.h"
#include "hal.h"
#include "memory_protection.h"
#include <main.h>

#include "leds.h" //Led
#include "spi_comm.h"

#include "selector.h" //Selector

#include "epuck1x/uart/e_uart_char.h" //Bluetooth
#include "serial_comm.h"
#include "stdio.h"

#include "sensors/proximity.h"
#include "sensors/VL53L0X/VL53L0X.h"

// 2022-10-19 : task3
messagebus_t bus;
MUTEX_DECL(bus_lock);
CONDVAR_DECL(bus_condvar);

int main(void)
{   messagebus_init(&bus, &bus_lock, &bus_condvar);
    //float widthBetweenWheels = ;
    //float wheelSize =;

    halInit();
    chSysInit();
    mpu_init();

    clear_leds();
    spi_comm_start();
    serial_start();

    proximity_start();
    calibrate_ir();
    VL53L0X_start();

    /* Infinite loop. */
    while (1) {
    	
        /// Send Selector number to PC
        char str[100];
        int str_length;
        for (int i = 0 ; i <8 ; i ++){
            int value = get_prox(i);
            str_length = sprintf(str, "prox_sensor %d : %d\n",i,value);
            e_send_uart1_char(str, str_length);    	
            int value2 = get_calibrated_prox(i);
            str_length = sprintf(str, "calibrated_sensor %d : %d\n",i,value2);
            e_send_uart1_char(str, str_length);
        }    
        int value = VL53L0X_get_dist_mm();
        str_length = sprintf(str, "Distance sensor :  %d\n",value);
        e_send_uart1_char(str, str_length);    	
        chThdSleepMilliseconds(1000);
        }
}

#define STACK_CHK_GUARD 0xe2dee396
uintptr_t __stack_chk_guard = STACK_CHK_GUARD;

void __stack_chk_fail(void)
{
    chSysHalt("Stack smashing detected");
}
