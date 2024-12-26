/*

INCLUDES

*/
#include <msp430.h>
#include "../functions/FLL.h"
#include "../functions/system_config.h"
#include "functions/general_functions.h"

int main(void){

//***************************************************************************** 
/*SETUP CLK*/
//*****************************************************************************
stop_wd();
const bool debug_CLK_programm = false;
if debug_CLK_programm{
    configure_PINS_for_clk_debug();
}

/*
CONFIGURATION OF THE CLOCKS
*/

//VARIABLES
const char operating_mode = 'A';//A -- Active

//Rango de frecuencia de trabajo del DCO:
    /*2 --> fDCOCLK =   1.4-12MHz*/
    /*3 --> fDCOCLK =   2.2-17Mhz*/
    /*4 --> fDCOCLK =   3.2-25Mhz*/
    /*8 --> fDCOCLK =     5-40Mhz*/
const int DCO_range = 4;  

// Values for setting the frequency of the DCO+
// DCO+ set so freq= xtal x D x N+1 
const int N_MCLK = 127; //Max 127
const bool DCOPLUS_on = true; //If D factor is wanted to be applied then -> True
const int D_val = 2; //Max 8

//Reference selection for MCLK
/*
CLK references MCLK
    - D: DCO
    - X: XT2
    - A: LFXT1
*/
const char ref_MCLK = 'A';

/*
    L: Low Frequency Mode
    H: High Frequency Mode
*/
const char LFXT1_wk_mode = 'H';

//Configura la capacidad interna del LFXT1
/*0  --> XIN Cap = XOUT Cap = 0pf */
/*10 --> XIN Cap = XOUT Cap = 10pf */
/*14 --> XIN Cap = XOUT Cap = 14pf */
/*18 --> XIN Cap = XOUT Cap = 18pf */
const int LFXT1_int_cap = 18;

//Reference for SMCLK
/*
CLK references SMCLK
    - D: DCO
    - X: XT2
    - N: OFF
*/
const char ref_SMCLK = 'D';
    
//ACLK division for configuring ACLK/N
const int divider_ACLK = 2;


//Second oscillator ON OFF
const bool LFXT2_osc_on = false;

/*
ACLK division: 1 2 4 8 
*/
//FUNCTIONS
select_operating_mode(operating_mode, 0);

DCO_f_range(DCO_range);

configure_N_for_MCLK(N_MCLK);

select_reference_MCLK(ref_MCLK);

configuring_DCO(DCOPLUS_on, D_val);

LFXT1_working_mode(LFXT1_wk_mode);

LFXT1_internal_cap_config(LFXT1_int_cap);

select_reference_SMCLK(ref_SMCLK);

configure_ACLK_N(divider_ACLK);

LFXT2_disable(LFXT2_osc_on);


//***************************************************************************** 
/*SETUP SD16A*/
//*****************************************************************************



}

