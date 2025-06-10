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
bool state = 0;
uint8_t high_word = 0x00;
uint8_t low_word = 0xFF;
volatile uint16_t my_register = 0;
bool high_or_low = true;
#define SD16_ENABLE true
#define CLK_DEBUG true
int main(void){

    stop_wd();
    init_MSP();
//************************** CLK configuration *****************************
#if CLK_DEBUG
    configure_PINS_for_clk_debug();
#endif    

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
    // XTAL --> 32767Hz
    const bool DCOPLUS_on = true; //If D factor is wanted to be applied then -> True
    const int D_val = 2; //Max 8
    const int N_MCLK = 122; //Max 127

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

//************************** LED configuration *****************************
  toggle_setup();								//Setup P4.6 for LED output



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

  USCI_init();                     // **Initialize USCI state machine**
  

  static const bool enable_USCI_interr_rx = false; 
  static const bool enable_USCI_interr_tx = false; 
  USCI_interrupt_enable(enable_USCI_interr_rx, enable_USCI_interr_tx); // Enable USCI_A0 RX interrupt
  

#if SD16_ENABLE

//************************** SD16 configuration *****************************
    // -- Entrada analógica
    static const unsigned int analog_input = 0; //0: A0, 1: A1, 2: A2, 3: A3, 4: A4
    setup_analog_input(analog_input);
    select_analog_input(analog_input);

    // -- Tensión de referencia
    static const char v_ref = 'I';            // I: Internal (1.2V), O: Off-chip, E: External
    voltage_reference(v_ref);

    // -- Reloj de referencia
    static const char clk_ref_SD16 = 'M';          // M: MCLK, S: SMCLK, A: ACLK, T: TACLK
    SD16_clk_reference(clk_ref_SD16); // M: MCLK, S: SMCLK, A: ACLK, T:TACLK

    // -- Divisor de frecuencia de referencia
    static const unsigned int clk_div_1 = 1;
    static const unsigned int clk_div_2 = 1;
    fM_dividers(clk_div_1, clk_div_2);

    // -- Método de lectura: Polling o Interrupciones
    bool const interruption_SD16A = true;
    enable_interruption_SD16A(interruption_SD16A);

    // -- Over Sampling Ratio
    static const unsigned int OSR = 512; //1, 32, 64, 128, 256, 512, 1024
    config_OSR(OSR);

    // -- Ganancia
    static const int gain = 1; //1, 2, 4, 8, 16 or 32
    gain_setup(gain);    

    // -- Método de conversión
    static const char conv_mode = 'C'; // C: Continuous  S: Single
    conversion_mode(conv_mode); 

    // -- Tipo de datos
    static const char polarity = 'B';       // B : Bipolar, U : unipolar
    static const char sign = 'O';           // O : Offset, C : 2's complement
    data_format(polarity, sign);
    
    start_conversion(); // While it is started, working in continuous mode will sample the channel A until it is stopped

#endif
  UCA0TXBUF = 0x45;                     // Transmit first character

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

    if (state == 1) {
        if (high_or_low) {
            UCA0TXBUF = high_word; // Envía el byte alto
        } else {
            UCA0TXBUF = low_word;  // Envía el byte bajo
            state = 0;  // Solo aquí finalizamos la transmisión completa
            IE2 &= ~(UCA0RXIE|UCA0TXIE); // Disabling UART interrupt until SD16 enables it again
//            USCI_interrupt_enable(false);
        }
        high_or_low = !high_or_low;  // Alterna entre alto y bajo
    }

/*
  while (!(IFG2 & UCA0TXIFG));              // USART1 TX buffer ready?
  volatile unsigned int i;

//  UCA0TXBUF = MST_Data;                     // Send next value
  UCA0TXBUF = 0x45;                     // Send next value

  for(i=30;i>0;i--);                        // Add time between transmissions to
*/
}   


//***************************************************************************** 
//Interrupción del SD16_A
//***************************************************************************** 

#if SD16_ENABLE
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
    //USCI_interrupt_enable(true);
    IE2 |= UCA0RXIE+UCA0TXIE;                 // Enabling UART interrupt

    if (state == 0){
        my_register = SD16MEM0;              // Save CH0 results (clears IFG)

        high_word = (my_register >> 8) & 0xFFFF; // 8 bits superiores (0x1234)
        low_word = my_register & 0xFFFF;          // 8 bits inferiores (0x5678)
        state = 1;
    }

    break;
  }
}
#endif
