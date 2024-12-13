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

    //Exit all Low Power Modes before choosing one
    __bic_SR_register_on_exit(LPM0_bits);
    __bic_SR_register_on_exit(LPM1_bits);
    __bic_SR_register_on_exit(LPM2_bits);
    __bic_SR_register_on_exit(LPM3_bits);
    __bic_SR_register_on_exit(LPM4_bits);
    switch (mode) {
        case 'A':
            break;

        case 'L':
            switch (LPM_int) {
                case 0:
                    __bis_SR_register(LPM0_bits)         /* Enter Low Power Mode 0 */
                    break;
                case 1:
                    __bis_SR_register(LPM1_bits)         /* Enter Low Power Mode 1 */
                    break;
                case 2:
                    __bis_SR_register(LPM2_bits)         /* Enter Low Power Mode 2 */
                    break;
                case 3:
                    __bis_SR_register(LPM3_bits)         /* Enter Low Power Mode 3 */
                    break;
                case 4:
                    __bis_SR_register(LPM4_bits)         /* Enter Low Power Mode 4 */
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
