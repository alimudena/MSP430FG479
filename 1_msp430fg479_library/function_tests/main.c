//In debug mode

//                 MSP430x47x
//             -----------------
//         /|\|              XIN|-
//          | |                 | 32kHz
//          --|RST          XOUT|-
//            |                 |
//            |             P1.1|--> MCLK = 8Mhz --> 57
//            |                 |
//            |             P1.5|--> ACLK = 32kHz --> 51
//            |                 |


/*

INCLUDES

*/
#include <msp430.h>
#include "../functions/system_config.h"
#include "../functions/general_functions.h"

#include "../functions/FLL.h"
#include "../functions/SD16_A.h"

unsigned int result;
#define   Num_of_Results   20

unsigned int results[Num_of_Results];
int main(void){

//***************************************************************************** 
/*GENERAL SETUP*/
//*****************************************************************************
volatile unsigned int i;                  // Use volatile to prevent removal

stop_wd();
const bool debug_CLK_programm = true;
if (debug_CLK_programm)
{
    configure_PINS_for_clk_debug();
}

//setup pin for led for toggle
toggle_setup();


//***************************************************************************** 
/*SETUP CLK*/
/*For generating the 8MHz:

- Active
- DCO_range = 4
- N_MCLK = 

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

//MCLK
//Reference selection for MCLK
const char ref_MCLK = 'D'; //  D: DCO, X: XT2, A: LFXT1
select_reference_MCLK(ref_MCLK);

//SMCLK

// Reference for SMCLK
const char ref_SMCLK = 'D'; // D: DCO, X: XT2, N: OFF

select_reference_SMCLK(ref_SMCLK);

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
const bool DCOPLUS_on = true; //If D factor is wanted to be applied then -> True
const int D_val = 2; //Max 8
configuring_DCO(DCOPLUS_on, D_val);

const int N_MCLK = 121; //Max 127
configure_N_for_MCLK(N_MCLK);



//Oscilador LFXT1

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


LFXT1_working_mode(LFXT1_wk_mode);
LFXT1_internal_cap_config(LFXT1_int_cap);

//******************REHACER


    
//ACLK division for configuring ACLK/N
const int divider_ACLK = 1; // 1, 2, 4, 8

//Second oscillator ON OFF
const bool LFXT2_osc_on = false;

//******************FUNCTIONS

configure_ACLK_N(divider_ACLK);
LFXT2_disable(LFXT2_osc_on);


//***************************************************************************** 
/*SETUP SD16A*/
//*****************************************************************************
//****************** VARIABLES
        // -- Entrada analógica
    static const unsigned int analog_input = 0; //0: A0, 1: A1, 2: A2, 3: A3, 4: A4
        // -- Tensión de referencia
    static const char v_ref = 'I';            // I: Internal (1.2V), O: Off-chip, E: External
        // -- Reloj de referencia
    static const char clk_ref = 'M';          // M: MCLK, S: SMCLK, A: ACLK, T: TACLK
        // -- Divisor de frecuencia de referencia
    static const unsigned int clk_div_1 = 1;
    static const unsigned int clk_div_2 = 1;
        // -- Método de lectura: Polling o Interrupciones
    bool const interruption_SD16A = false;
        // -- Over Sampling Ratio
    static const unsigned int OSR = 1; //1, 32, 64, 128, 256, 512, 1024
        // -- Ganancia
    static const int gain = 1; //1, 2, 4, 8, 16 or 32
        // -- Método de conversión
    static const char conv_mode = 'S'; // C: Continuous  S: Single
        // -- Tipo de datos
    static const char polarity = 'B';       // B : Bipolar, U : unipolar
    static const char sign = 'O';           // O : Offset, C : 2's complement
  
  if (debug_CLK_programm == false){
 //****************** FUNCTIONS
    setup_analog_input(analog_input);
    select_analog_input(analog_input);
    
    FLL_CTL0 |= XCAP14PF;                     // Configure load caps
    for (i = 0; i < 10000; i++);              // Delay for 32 kHz crystal to

    voltage_reference(v_ref);
    clk_reference(clk_ref); // M: MCLK, S: SMCLK, A: ACLK, T:TACLK
    fM_dividers(clk_div_1, clk_div_2);
    enable_interruption_SD16A(interruption_SD16A);
    config_OSR(OSR);
    gain_setup(gain);    
    conversion_mode(conv_mode); 
    data_format(polarity, sign);
}

//***************************************************************************** 
/*SETUP UART*/




//*****************************************************************************


}




#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=SD16A_VECTOR
__interrupt void SD16ISR(void)
#elif defined(__GNUC__)A
void __attribute__ ((interrupt(SD16A_VECTOR))) SD16ISR (void)
#else
#error Compiler not supported!
#endif
{
  static unsigned int index = 0;
  switch (SD16IV)
  {
  case 2:                                   // SD16MEM Overflow
    break;
  case 4:                                   // SD16MEM0 IFG
    result = SD16MEM0;                      // Save CH0 results (clears IFG)
    results[index] = result;
    if (++index == Num_of_Results)
    {
      index = 0;                            // SET BREAKPOINT HERE
    }
    break;
  }

  __bic_SR_register_on_exit(LPM0_bits);                   // Exit LPM0
}

