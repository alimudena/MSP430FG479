#include "msp430fg479.h"
#include <msp430.h>
#include <stdio.h>
#include <msp430.h>

#include <assert.h>  // For using assert
#include <stdbool.h> 


#include "system_config.h"


//*****************************************************************************
/*OPERATING MODES*/
//*****************************************************************************

void select_operating_mode(char mode, int LPM_int){
    /*
    mode:
        - A: Active
        - L: Low Power
            - 0: LPM0
            - 1: LPM1
            - 2: LPM2 
            - 3: LPM3
    */
    switch (mode) {
        case 'A':
            break;

        case 'L':
            switch (LPM_int) {
                case 0:
                    break;
                case 1:
                    break;
                case 2:
                    break;
                case 3:
                    break;
            default:
                perror("Error: Selected LPM operating mode not available.");
                break;
            }

        default:
            perror("Error: Selected operating mode not available.");
            break;
    }

    return;
}

//*****************************************************************************
/*INTERRUPTIONS*/
//*****************************************************************************

void enable_interruptions(bool enable){
    //Testearla con un programa que funcione mediante interrupciones proporcionado por el MSP430
    if (enable) {
        __bis_SR_register(GIE);
    }
    return;
}
