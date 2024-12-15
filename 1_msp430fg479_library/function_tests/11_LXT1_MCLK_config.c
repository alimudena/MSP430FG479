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
    configure_PINS_for_clk_debug();

    static const bool interruptions_enabled = true;
    enable_interruptions(interruptions_enabled);
    
    static const char modo_operacion = 'A';
    static const int LPM_int = 3;
    select_operating_mode(modo_operacion, LPM_int);




    // ACLK can be seen directly in PIN-51 if not touched, its exacly fcristal LFXT1.

    //ACLK/N cannot be seen in any pin
    const int ACLK_N_div = 1;
    configure_ACLK_N(ACLK_N_div);

//*****************************************************************************
/*CONFIGURATION FOR LXT1*/
//*****************************************************************************
const bool osc_ext = false;
const int f_osc_ext = 0;
protection_cpu_required(osc_ext, f_osc_ext);

const char Low_High_frequency_mode = 'H';
LFXT1_working_mode(Low_High_frequency_mode);


/*0  --> XIN Cap = XOUT Cap = 0pf */
/*10 --> XIN Cap = XOUT Cap = 10pf */
/*14 --> XIN Cap = XOUT Cap = 14pf */
/*18 --> XIN Cap = XOUT Cap = 18pf */
const int LFXT1_int_cap = 18;
LFXT1_internal_cap_config(LFXT1_int_cap);



    //MCLK can be seen in PIN-57
    //*****************************************************************************
    /*CONFIGURATION FOR MCLK*/
    //*****************************************************************************
    /*
    CLK references for MCLK
        - D: DCO
        - X: XT2
        - A: LFXT1 (ACLK)
    */    
    const char clk_ref_MCLK = 'A';
    select_reference_MCLK(clk_ref_MCLK);

    
    



}

