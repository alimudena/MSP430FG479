
//                 MSP430x47x
//             -----------------
//         /|\|              XIN|-
//          | |                 | 32kHz
//          --|RST          XOUT|-
//            |                 |
//            |             P1.1|--> MCLK = 8Mhz  --> 57 (referencia DCO)
//            |             P1.4|--> SMCLK = 8MHz --> 54 (referencia DCO)
//            |             P1.5|--> ACLK = 32kHz --> 51
//            |                 |



#include <msp430.h>
#include "../functions/system_config.h"
#include "../functions/general_functions.h"

#include "../functions/FLL.h"
#include "../functions/SD16_A.h"



int main(void){

stop_wd();
configure_PINS_for_clk_debug();

//setup pin for led for toggle
toggle_setup();


//***************************************************************************** 
/*SETUP CLK*/
/*For generating the 8MHz:

const char operating_mode = 'A';
const char LFXT1_wk_mode = 'L';
const int DCO_range = 4;  
const bool DCOPLUS_on = true; //If D factor is wanted to be applied then -> True
const int D_val = 2; //Max 8
const int N_MCLK = 121; //Max 127
const char ref_MCLK = 'D'; //  D: DCO, X: XT2, A: LFXT1
const char ref_SMCLK = 'D'; // D: DCO, X: XT2, N: OFF
const int divider_ACLK = 1; // 1, 2, 4, 8
const bool LFXT2_osc_on = false;

*/
//*****************************************************************************

//Modo de operación
/*    mode:
        - A: Active
        - L: Low Power
            - 0: LPM0
            - 1: LPM1
            - 2: LPM2 
            - 3: LPM3
*/
const char operating_mode = 'A';
select_operating_mode(operating_mode, 0);

//Oscilador LFXT1
/*
    L: Low Frequency Mode --> f auxiliar de 323kHz conectado
    H: High Frequency Mode
*/
const char LFXT1_wk_mode = 'L';

//Configura la capacidad interna del LFXT1
/*0  --> XIN Cap = XOUT Cap = 0pf */
/*10 --> XIN Cap = XOUT Cap = 10pf */
/*14 --> XIN Cap = XOUT Cap = 14pf */
/*18 --> XIN Cap = XOUT Cap = 18pf */
const int LFXT1_int_cap = 18;


LFXT1_working_mode(LFXT1_wk_mode);
LFXT1_internal_cap_config(LFXT1_int_cap);


//DCO
//Rango de frecuencia de trabajo del DCO:
    /*2 --> fDCOCLK =   1.4-12MHz*/
    /*3 --> fDCOCLK =   2.2-17Mhz*/
    /*4 --> fDCOCLK =   3.2-25Mhz*/ //-> 8 MHz
    /*8 --> fDCOCLK =     5-40Mhz*/
const int DCO_range = 4;  
DCO_f_range(DCO_range);


// Values for setting the frequency of the DCO+
// DCO+ set so freq= xtal x D x N_MCLK+1 
//XTAL --> 32767Hz
const bool DCOPLUS_on = true; //If D factor is wanted to be applied then -> True
const int D_val = 2; //Max 8
const int N_MCLK = 121; //Max 127

configuring_DCO(DCOPLUS_on, D_val);
configure_N_for_MCLK(N_MCLK);


//MCLK
//Reference selection for MCLK
const char ref_MCLK = 'D'; //  D: DCO, X: XT2, A: LFXT1
select_reference_MCLK(ref_MCLK);

//SMCLK
// Reference for SMCLK
const char ref_SMCLK = 'D'; // D: DCO, X: XT2, N: OFF
select_reference_SMCLK(ref_SMCLK);



//ACLK
//ACLK division for configuring ACLK/N
const int divider_ACLK = 1; // 1, 2, 4, 8
configure_ACLK_N(divider_ACLK);

// LFXT2
//Second oscillator ON OFF
const bool LFXT2_osc_on = false;
LFXT2_disable(LFXT2_osc_on);

}

