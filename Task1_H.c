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
#include "motors.h"

#include "sensors/VL53L0X/VL53L0X.h" //distance
#include "sensors/proximity.h" //proximity
messagebus_t bus;
MUTEX_DECL(bus_lock);
CONDVAR_DECL(bus_condvar);
// 2022-10-19 : task1


int main(void)
{
    messagebus_init(&bus, &bus_lock, &bus_condvar);
    //float widthBetweenWheels = ;
    //float wheelSize =;

    halInit();
    chSysInit();
    mpu_init();

    clear_leds();
    spi_comm_start();
    serial_start();
    motors_init();

    proximity_start();
    calibrate_ir();
    VL53L0X_start();

    int Prox_Value[] = { 0,0,0,0,0,0,0,0 };
    led_name_t LED[] = { LED1,LED1,LED3,LED5,LED5,LED7,LED7,LED1 };
    rgb_led_name_t led[] = {LED2,LED2,LED4,LED4,LED6,LED6,LED8,LED8 };


    /* Infinite loop. */
    while (1) {

        /// Send Selector number to PC
        char str[100];
        int str_length;
        int Sel_value = get_selector();
        str_length = sprintf(str, "selector number is :  %d!\n", Sel_value);
        e_send_uart1_char(str, str_length);
        int distance_Value = VL53L0X_get_dist_mm();
        str_length = sprintf(str, "Distance sensor :  %d\n", distance_Value);
        e_send_uart1_char(str, str_length);

        for (int i = 0; i < 8; i++) //IR get sensor value
        {
            Prox_Value[i] = get_calibrated_prox(i);
            str_length = sprintf(str, "prox_sensor %d : %d\n", i, Prox_Value);
            e_send_uart1_char(str, str_length);
            int Cal_value = get_calibrated_prox(i);
            str_length = sprintf(str, "calibrated_sensor %d : %d\n", i, Cal_value);
            e_send_uart1_char(str, str_length);
            if (Prox_Value[i]>100)
            {
                set_led(LED[i], 1);
                set_rgb_led(led[i], 10, 0, 0);
            }
        }

        bool right_obstacle = Prox_Value[0] > 200 || Prox_Value[1] > 200 || Prox_Value[2] > 200; //right obstacle
        bool left_obstacle = Prox_Value[5] > 200 || Prox_Value[6] > 200 || Prox_Value[7] > 200;  // left obstacle

        if (left_obstacle)
        {
            left_motor_set_speed(500);
            right_motor_set_speed(-500);
            chThdSleepMilliseconds(3000);
        }
        else if (right_obstacle)
        {
            left_motor_set_speed(-500);
            right_motor_set_speed(500);
            chThdSleepMilliseconds(3000);
        }
        else if(left_obstacle && right_obstacle)
        {
            left_motor_set_speed(300);
            right_motor_set_speed(300);
            while (distance_Value < 30) 
            {
                left_motor_set_speed(500);
                right_motor_set_speed(-500);
                chThdSleepMilliseconds(1000);
            }
        }
        else
        {
            left_motor_set_speed(300);
            right_motor_set_speed(300);
        }

    }
}
//         double distance=VL53L0X_get_dist_mm();
//         e_send_uart1_char(str, str_length);


//         if(get_selector() == 0) { // Straight motion
//             str_length = sprintf(str, "Go straight !\n");
//             e_send_uart1_char(str, str_length);    	

//             left_motor_set_speed(-500);
//             right_motor_set_speed(500);

//         } 
//         else { //Turning motion

//             int value = get_selector();
//             for (int i =0 ; i <value ; i++ ){
//                 set_led(LED1,1);
//                 chThdSleepMilliseconds(1000);
//                 set_led(LED1,0);
//                 chThdSleepMilliseconds(1000);
//             }

//             // how do i know how long the motors spin?
//             left_motor_set_speed(350);
//             right_motor_set_speed(-350);
//             chThdSleepMilliseconds(5000);
//             left_motor_set_speed(-350);
//             right_motor_set_speed(350);
//             chThdSleepMilliseconds(5000);
//             left_motor_set_speed(0);
//             right_motor_set_speed(0);
//         }

//     }
// }

#define STACK_CHK_GUARD 0xe2dee396
uintptr_t __stack_chk_guard = STACK_CHK_GUARD;

void __stack_chk_fail(void)
{
    chSysHalt("Stack smashing detected");
}