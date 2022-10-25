#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "ch.h"
#include "hal.h"
#include "memory_protection.h"
#include <main.h>

#include "leds.h" // LED
#include "spi_comm.h"

#include "selector.h" //Selector

#include "epuck1x/uart/e_uart_char.h" //Bluetooth
#include "serial_comm.h"
#include "motors.h"

int main(void)
{

    halInit();
    chSysInit();
    mpu_init();

    clear_leds();
    spi_comm_start();
    motors_init();
    serial_start();

    /* Infinite loop. */
    while (1) {


        //send selector number to PC
        char str[100];
        int str_length;
        int value = get_selector();
        str_length = sprintf(str, "selector number is: %d \n", value);
        e_send_uart1_char(str,str_length);

    	
        if (get_selector()==0)
        {
            str_length = sprintf(str, "Go straight !\n");
            e_send_uart1_char(str, str_length);

            int left_motor_get_desired_speed(500);
            int right_motor_get_desired_speed(500);
        }
        
        else{//turning motion
            int value= get_selector();
            for ( i = 0; i < value; i++)
            {
                set_led(LED1,1);
                chThdSleepMilliseconds(1000);
                set_led(LED1,0);
                chThdSleepMilliseconds(1000);
            }
            
            str_length = sprintf(str, "Turning direction !\n");
            e_send_uart1_char(str, str_length);
            left_motor_set_speed(300);
            right_motor_set_speed(-300);
            chThdSleepMilliseconds(5000);
            left_motor_set_speed(300);
            right_motor_set_speed(-300);
            chThdSleepMilliseconds(5000);
            left_motor_set_speed(0);
            right_motor_set_speed(0);

        }

       
    }
}

#define STACK_CHK_GUARD 0xe2dee396
uintptr_t __stack_chk_guard = STACK_CHK_GUARD;

