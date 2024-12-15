#include <msp430.h>
#include <stdio.h>
#include <stdint.h>  // Necesario para uint16_t
#include <stdbool.h> // Necesario para boolean
#include "../functions/general_functions.h"
#include "../functions/SD16_A.h"
#include "../functions/FLL.h"
#include "../functions/system_config.h"


int main(void){
    volatile unsigned int i;                  // Use volatile to prevent removal
                                            // by compiler optimization
    // stop watch dog
    stop_wd();
    
    static const bool interruptions_enabled = true;
    enable_interruptions(interruptions_enabled);
    
    static const char modo_operacion = 'A';
    static const int LPM_int = 3;
    select_operating_mode(modo_operacion, LPM_int);

    configure_PINS_for_clk_debug();

//*****************************************************************************
/*CONFIGURATION FOR ACLK SIGNAL REFERENCE and ACLK/n */ 
//*****************************************************************************

// ACLK can be seen directly in PIN-51 if not touched, its exacly fcristal LFXT1.

//ACLK/N cannot be seen in any pin
const int ACLK_N_div = 1;
configure_ACLK_N(ACLK_N_div);

}

