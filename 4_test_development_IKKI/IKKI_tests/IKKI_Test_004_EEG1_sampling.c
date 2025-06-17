/**
 * @test Sample the EEG 1 signal
 * @brief Test that confirms that the EEG in channel 1 is being sampled.
 * 
 * @details
 * After programming the platform, the pins associated to the EEG of channel 1 input should present the result of sampling one sample. 
 *             MSP430FG479
 *           -----------------
 *      /|\ |              XIN|---+
 *       |  |                 |   |
 *       ---|RST              |   32kHz
 *          |                 |   |
 *          |             XOUT|---+
 *          |             P1.1|--> MCLK = 8Mhz  --> 57 (referencia DCO)
 *          |             P1.4|--> SMCLK = 8MHz --> 54 (referencia DCO)
 *          |             P1.5|<------- A3+: ECG positive input --> 51
 *          |             P1.4|<------- A3-: ECG negative input --> 54
 *          |                 |
 *          |                 |
 * 
 * @expected
 * The pins number 51 and 54 should present signals of the EEG. 
 * WARNING: IT IS NOT POSSIBLE TO EVALUATE ACLK AT THE SAME TIME: P1.5 CONTAINS THE ACLK TOO
 */



/*

INCLUDES

*/
#include <msp430.h>
#include "../functions/system_config.h"
#include "../functions/general_functions.h"

#include "../functions/FLL.h"
#include "../functions/SD16_A.h"
#include "clk_config.h"
#include "SD16A_config.h"


unsigned int result;
#define   Num_of_Results   200
unsigned index = 0;
CLK_config_struct CLK_config;
unsigned counter;

float results[Num_of_Results];
float result_graphed;
volatile unsigned int i;                  // Use volatile to prevent removal

void general_setup(){

    //***************************************************************************** 
    /*GENERAL SETUP*/
    //*****************************************************************************

    stop_wd();
    CLK_config.CLK_debug = false;
    if (CLK_config.CLK_debug)
    {
        configure_PINS_for_clk_debug();
        //setup pin for led for toggle
        toggle_setup();
    }
}


void setup_CLK(){
    //***************************************************************************** 
    /*SETUP CLK*/
    /*For generating the 8MHz:
        CLK_config.operating_mode = 'A';
        CLK_config.LFXT1_wk_mode = 'L';
        CLK_config.DCO_range = 4;  
        CLK_config.DCOPLUS_on = true; //If D factor is wanted to be applied then -> True
        CLK_config.D_val = 2; //Max 8
        CLK_config.N_MCLK = 121; //Max 127
        CLK_config.ref_MCLK = 'D'; //  D: DCO, X: XT2, A: LFXT1
        CLK_config.ref_SMCLK = 'D'; // D: DCO, X: XT2, N: OFF
        CLK_config.divider_ACLK = 1; // 1, 2, 4, 8
        CLK_config.LFXT2_osc_on = false;
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
        CLK_config.operating_mode = 'A';
        select_operating_mode(CLK_config.operating_mode, 0);
    //Oscilador LFXT1
    /*
        L: Low Frequency Mode --> f auxiliar de 323kHz conectado
        H: High Frequency Mode
    */
        CLK_config.LFXT1_wk_mode = 'L';
    //Configura la capacidad interna del LFXT1
    /*0  --> XIN Cap = XOUT Cap = 0pf */
    /*10 --> XIN Cap = XOUT Cap = 10pf */
    /*14 --> XIN Cap = XOUT Cap = 14pf */
    /*18 --> XIN Cap = XOUT Cap = 18pf */
        CLK_config.LFXT1_int_cap = 18;
        LFXT1_working_mode(CLK_config.LFXT1_wk_mode);
        LFXT1_internal_cap_config(CLK_config.LFXT1_int_cap);
    //DCO
    //Rango de frecuencia de trabajo del DCO:
        /*2 --> fDCOCLK =   1.4-12MHz*/
        /*3 --> fDCOCLK =   2.2-17Mhz*/
        /*4 --> fDCOCLK =   3.2-25Mhz*/ //-> 8 MHz
        /*8 --> fDCOCLK =     5-40Mhz*/
        CLK_config.DCO_range = 4;  
        DCO_f_range(CLK_config.DCO_range);
    // Values for setting the frequency of the DCO+
    // DCO+ set so freq= xtal x D x N_MCLK+1 
    //XTAL --> 32767Hz
        CLK_config.DCOPLUS_on = true; //If D factor is wanted to be applied then -> True
        CLK_config.D_val = 2; //Max 8
        CLK_config.N_MCLK = 121; //Max 127
        configuring_DCO(CLK_config.DCOPLUS_on, CLK_config.D_val);
        configure_N_for_MCLK(CLK_config.N_MCLK);
    //MCLK
    //Reference selection for MCLK
        CLK_config.ref_MCLK = 'D'; //  D: DCO, X: XT2, A: LFXT1
        select_reference_MCLK(CLK_config.ref_MCLK);
    //SMCLK
    // Reference for SMCLK
        CLK_config.ref_SMCLK = 'D'; // D: DCO, X: XT2, N: OFF
        select_reference_SMCLK(CLK_config.ref_SMCLK);
    //ACLK
    //ACLK division for configuring ACLK/N
        CLK_config.divider_ACLK = 1; // 1, 2, 4, 8
        configure_ACLK_N(CLK_config.divider_ACLK);
    // LFXT2
    //Second oscillator ON OFF
        CLK_config.LFXT2_osc_on = false;
        LFXT2_disable(CLK_config.LFXT2_osc_on);

}



int main(void){

    general_setup();
    setup_CLK();


    //***************************************************************************** 
    /*SETUP SD16A*/
    //*****************************************************************************

    SD16A_config_struct SD16A_configuration;

    // -- Entrada analógica
        SD16A_configuration.analog_input_count = 1;
        SD16A_configuration.analog_input[0] = 3; //0: A0, 1: A1, 2: A2, 3: A3, 4: A4
    // -- Tensión de referencia
        SD16A_configuration.v_ref = 'I';            // I: Internal (1.2V), O: Off-chip, E: External
    // -- Reloj de referencia
        SD16A_configuration.clk_ref = 'M';          // M: MCLK, S: SMCLK, A: ACLK, T: TACLK
    // -- Divisor de frecuencia de referencia
        SD16A_configuration.clk_div_1 = 1;
        SD16A_configuration.clk_div_2 = 1;
    // -- Método de lectura: Polling o Interrupciones
        SD16A_configuration.interruption_SD16A = true;
    // -- Over Sampling Ratio
        SD16A_configuration.OSR = 1; //1, 32, 64, 128, 256, 512, 1024
    // -- Ganancia
        SD16A_configuration.gain = 1; //1, 2, 4, 8, 16 or 32
    // -- Método de conversión
        SD16A_configuration.conv_mode = 'S'; // C: Continuous  S: Single
    // -- Tipo de datos
        SD16A_configuration.polarity = 'U';       // B : Bipolar, U : unipolar
        SD16A_configuration.sign = 'O';           // O : Offset, C : 2's complement

    if (CLK_config.CLK_debug == false){
    //****************** FUNCTIONS

        for (counter = SD16A_configuration.analog_input_count; counter > 0; counter--) {
            setup_analog_input(SD16A_configuration.analog_input[counter-1]);
        }
        select_analog_input(SD16A_configuration.analog_input[0]);
        
        FLL_CTL0 |= XCAP14PF;                     // Configure load caps
        for (i = 10000; i >0 ; i--);              // Delay for 32 kHz crystal to

        voltage_reference(SD16A_configuration.v_ref);
        SD16_clk_reference(SD16A_configuration.clk_ref); // M: MCLK, S: SMCLK, A: ACLK, T:TACLK
        fM_dividers(SD16A_configuration.clk_div_1, SD16A_configuration.clk_div_2);
        enable_interruption_SD16A(SD16A_configuration.interruption_SD16A);
        for (i = 10000; i >0 ; i--);             // Delay for 1.2V ref startup
        config_OSR(SD16A_configuration.OSR);
        gain_setup(SD16A_configuration.gain);    
        conversion_mode(SD16A_configuration.conv_mode); 
        data_format(SD16A_configuration.polarity, SD16A_configuration.sign);

        while(1){

            start_conversion();                    // SET BREAKPOINT HERE
            enable_interruptions(SD16A_configuration.interruption_SD16A);
        //__bis_SR_register(LPM0_bits);       // Enter LPM0
            
        }

    }


}

#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=SD16A_VECTOR
__interrupt void SD16ISR(void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(SD16A_VECTOR))) SD16ISR (void)
#else
#error Compiler not supported!
#endif
{
  switch (SD16IV)
  {
  case 2:                                   // SD16MEM Overflow
    break;
  case 4:                                   // SD16MEM0 IFG
    result = SD16MEM0;                      // Save CH0 results (clears IFG)
    result_graphed = result;

    results[index] = result;

    if (++index == Num_of_Results){
    	index = 0;                            
    }
    break;

  }

  //__bic_SR_register_on_exit(LPM0_bits);                   // Exit LPM0
}
