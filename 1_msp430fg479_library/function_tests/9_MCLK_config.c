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
    configure_PINS_for_clk_debug();

//*****************************************************************************
/*CONFIGURATION FOR LFXT1 OSCILLATOR*/
//*****************************************************************************
    static const int LFXT1_int_cap = 0;
    LFXT1_internal_cap_config(LFXT1_int_cap);    

    static const char Low_High_power_mode = 'H';
    LFXT1_working_mode(Low_High_power_mode);

    static const bool osc_ext = false;
    static const int f_osc_ext = 0;
    protection_cpu_required(osc_ext, f_osc_ext);
    
    

//*****************************************************************************
/*CONFIGURATION FOR LFXT2 OSCILLATOR*/
//*****************************************************************************        
    static const bool LFXT2_disabled = false;
    LFXT2_disable(LFXT2_disabled);

//*****************************************************************************
/*CONFIGURATION FOR DCO OSCILLATOR*/
//*****************************************************************************        


//    FLL_CTL0 |= DCOPLUS + XCAP18PF;           // DCO+ set so freq= xtal x D x N+1
    static const bool DCOPLUS_on = false;
    static const int D_val = 1;
    static const int N_val = 2;
    configuring_DCO(DCOPLUS_on, D_val, N_val);


    DCO_f_range(4);// x2 DCO freq, 8MHz nominal DCO
    
//*****************************************************************************
/*CONFIGURATION FOR MCLK*/
//*****************************************************************************
    /*
    CLK references for MCLK
        - D: DCO
        - X: XT2
        - A: LFXT1
    */    
    static const char clk_ref_MCLK = 'D';
    select_reference_MCLK(clk_ref_MCLK);
    
//    SCFQCTL = 121;                            // (121+1) x 32768 x 2 = 7.99 Mhz
    //static const int N_MCLK = 128;
    //configure_N_for_MCLK(N_MCLK);


}

