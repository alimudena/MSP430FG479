//                   MSP430x47x
//                 -----------------
//             /|\|              XIN|-
//              | |                 |  32kHz xtal
//              --|RST          XOUT|-
//                |                 |
//                |  ---  CLK  ---  |
//                |             P1.1|--> MCLK = 8Mhz  --> 57 (referencia DCO)
//                |             P1.4|--> SMCLK = 8MHz --> 54 (referencia DCO)
//                |             P1.5|--> ACLK = 32kHz --> 51
//                |                 |
//                |  ---  SPI  ---  |
//                |             P2.4|-> Data Out (UCA0SIMO) --> 76
//          LED <-|P4.6         P2.5|<- Data In (UCA0SOMI) --> 75
//                |             P3.0|-> Serial Clock Out (UCA0CLK) --> 41
//                |                 |
//                |  ---  SD16 ---  |
//                |             P6.0|<- A0+ --> 67
//                |             P6.1|<- A0- --> 66
//                |                 |
//                |                 |
//

#include "msp430fg479.h"
#include <msp430.h>
#include "../functions/system_config.h"
#include "../functions/general_functions.h"

#include "../functions/FLL.h"
#include "../functions/SD16_A.h"
#include "../functions/USCI.h"


#include "clk_config.h"
#include "UART_config.h"
#include "SD16A_config.h"
#include "IKKI_MAC.h"

int  state = 0;

CLK_config_struct CLK_config;
SD16A_config_struct SD16A_configuration;
UART_config_struct UART_config;


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

    init_MSP();



//************************** SPI configuration *****************************


  USCI_SPI_pin_setup();
  
  static const char Master_Slave = 'M';
  SPI_mode_config(Master_Slave);

  static const char inactive_state = 'H'; // clock polarity inactive high
  static const char data_on_clock_edge = 'A'; //data cAptured on the first UCLK edge and changed on the following edge
  SPI_clk_polarity_phase(inactive_state, data_on_clock_edge);

  static const int SPI_length = 8;
  static const char first_Byte_sent = 'M';
  SPI_char_format(SPI_length, first_Byte_sent); //8-bit and MSB SPI 

/*clk_ref:
    U --> UCLK
    A --> ACLK
    S --> SMCLK        
*/
  static const char clk_ref_SPI = 'S';
  USCI_clk_ref(clk_ref_SPI);                    //CLK reference

  static const int clk_div = 2;
  SPI_clk_division(clk_div);

  static const bool enable_USCI_interr_rx = false; 
  static const bool enable_USCI_interr_tx = true; 
  USCI_interrupt_enable(enable_USCI_interr_rx, enable_USCI_interr_tx); // Enable USCI_A0 RX interrupt

  USCI_init();                     // **Initialize USCI state machine**


  enable_interruptions(true);



}

//***************************************************************************** 
//Interrupción del SPI
//***************************************************************************** 


#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=USCIAB0RX_VECTOR
__interrupt void USCIA0RX_ISR (void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(USCIAB0RX_VECTOR))) USCIA0RX_ISR (void)
#else
#error Compiler not supported!
#endif
{
  while (!(IFG2 & UCA0TXIFG));              // USART1 TX buffer ready?
  UCA0TXBUF = state;
  state++;

}   


