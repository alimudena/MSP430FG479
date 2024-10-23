// Definition of the includes
#include <stdint.h>
#include <msp430.h>
#include <math.h>
#include "general_functions.h"

/*----------------------------------------------------------------------------*/
/* LED RELATED FUNCTIONS                                                        */
/*----------------------------------------------------------------------------*/

void setup_led(void){
/*
Function that starts up the LED of the MSP-TS430PN80 dev kit
*/
    P4DIR |= BIT6;                            // Set P4.6 to output direction    
}

void toggle_led(void){
/*
Function that toggles the LED of the MSP-TS430PN80 dev kit
*/
    P4OUT ^= BIT6;                          // Toggle P4.6 using exclusive-OR

}


/*----------------------------------------------------------------------------*/
/* VOLTAGE RELATED FUNCTIONS                                                        */
/*----------------------------------------------------------------------------*/


//uint32_t V_fsr_calculation(uint32_t V_ref, uint32_t gain){
/*
Function that calculates the full scale range of the ADC 
considering selected reference voltage gain
*/
/*

    uint32_t V_fsr = (V_ref/2)/gain

}
*/ 
//void V_generated(uint32_t max_bits, uint32_t min_bits, uint32_t V_fsr, char type_conv){
/*
Function that returns the value of the voltage measured depending on the 
number of bits available, the Full Scale Range Voltage and the type of conversion 
*/
/*
switch (type_conv) {
    case 'OB': //Offset Binary from -Vsr to +Vfsr range 0000h to FFFFh
        uint32_t digital_out_max = pow(2, max_bits);
        uint32_t digital_out_min = pow(2, min_bits);
        uint32_t A = -V_fsr;


    case '2C': //2s complement from -Vfsr to +Vfsr range 8000h to 7FFFh

    case 'UO': //Unipolar output from 0 to +Vfsr range 0000h to FFFFh

    default: 
        return;
}

*/



