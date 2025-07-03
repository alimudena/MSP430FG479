/**
 * @test Generate a blinking led
 * @brief Tests that makes a led toggle.
 * 
 * @details
 * After programming the platform, the pin associated with the blinking led should blink. 
 * The proposed pin for blinking is the pin 12
 *          MSP430FG479
 *        -----------------
 *   /|\ |              XIN|---+
 *    |  |                 |   |
 *    ---|RST              |   32kHz
 *       |                 |   |
 *       |             XOUT|---+
 *       |             P4.6|-------> Toggle led --> 12
 *       |                 |
 *       |                 |
 * 
 * @expected
 * The pin number 12 should change from level VCC to 0 alternated.
 */

#include <msp430.h>
#include <stdio.h>
#include <stdint.h>  
#include "functions/general_functions.h"
#include "functions/SD16_A.h"

int main(void){
    // stop watch dog
    stop_wd();

    //setup pin for led for toggle
    toggle_setup();
    int i;
    while(1){
        toggle_pin();
        for(i = 0; i < 16000; i++);
    };
}

