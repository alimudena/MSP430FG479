#include <msp430.h>
#include <stdio.h>
#include <stdint.h>  // Necesario para uint16_t
#include "../MSP430FG479_HAL/functions/general_functions.h"
#include "../MSP430FG479_HAL/functions/SD16_A.h"

int main(void){
    // stop watch dog
    stop_wd();

    //setup pin for led for toggle
    toggle_setup();

    while(1){
        toggle_pin();
    };

}

