#include "msp430fg479.h"
#include <msp430.h>
#include <stdio.h>
#include <stdint.h>  // Neccesary for  uint16_t
#include "SD16_A.h"
#include <assert.h>  // For using assert



//*****************************************************************************
/*SELECTING THE CHANNEL*/
//*****************************************************************************

void select_analog_input(int channel){
    switch (channel) {
        case 0: // A0
            P6SEL |= BIT0+BIT1;
            break;
        case 1: // A1
            P6SEL |= BIT3+BIT4;
            break;
        case 2: // A2
            P1SEL |= BIT6+BIT7;
            break;
        case 3: // A3
            P1SEL |= BIT4+BIT5;
            break;
        case 4: // A4
            P1SEL |= BIT2+BIT3;
            break;
        default:
            assert(!"Error: Not available analog input chosen.");
            break;
    }
}

int read_analog_input(){
    return SD16MEM0;
}



//*****************************************************************************
/*CONFIGURING THE REFERENCES*/
//*****************************************************************************

void voltage_reference(char v_reference){
    // I: Internal (1.2V)
    // O: Off-chip 
    // E: External
    // First, clean the bits so to not have a mess
    SD16CTL &= ~(SD16REFON | SD16VMIDON);
    switch (v_reference) {
        case 'I':
            SD16CTL |= SD16REFON;
            break;
        case 'E':
            SD16CTL |= SD16VMIDON;
            break;
        case 'O':
        //Already cleaned up the bits
            break;
        default:
            assert(!"Error: Voltage reference not contemplated.");
            break;
    
    }

}



//*****************************************************************************
/*ENTERING LOW POWER MODE*/
//*****************************************************************************

void enter_LPM(void){
    __bis_SR_register(LPM0_bits+GIE);
}
/*
void exit_LPM(void){
    __bic_SR_register_on_exit(LPM0_bits); 
}
*/


