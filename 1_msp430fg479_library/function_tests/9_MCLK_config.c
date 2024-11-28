#include <msp430.h>
#include <stdio.h>
#include <stdint.h>  // Necesario para uint16_t
#include <stdbool.h> // Necesario para boolean
#include "../functions/general_functions.h"
#include "../functions/SD16_A.h"
#include "../functions/FLL.h"


int main(void){
    volatile unsigned int i;                  // Use volatile to prevent removal
                                            // by compiler optimization
    // stop watch dog
    stop_wd();

    DCO_f_range(4);// x2 DCO freq, 8MHz nominal DCO

//    SCFQCTL = 121;                            // (121+1) x 32768 x 2 = 7.99 Mhz
//    configure_N_for_MCLK(128);
    FLL_CTL0 |= DCOPLUS + XCAP18PF;           // DCO+ set so freq= xtal x D x N+1

    configuring_DCO(1, 1, 1);
    configure_PINS_for_clk_debug();



}

