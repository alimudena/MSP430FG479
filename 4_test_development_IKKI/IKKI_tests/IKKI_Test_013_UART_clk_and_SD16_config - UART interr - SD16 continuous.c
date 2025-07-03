/**
 * @test UART, SD16 working
 * @brief Tests that makes sure that the UART is working properly and sends the values read from the SD16.
 *             MSP430FG479
 *           -----------------
 *      /|\ |              XIN|---+
 *       |  |                 |   |
 *       ---|RST              |   32kHz
 *          |                 |   |
 *          |             XOUT|---+
 *          |             P1.1|--> MCLK = 8Mhz  --> 57 (referencia DCO)
 *          |             P1.4|--> SMCLK = 8MHz --> 54 (referencia DCO)
 *          |             P1.5|--> ACLK = 32kHz --> 51
 *          |                 |
 *          |             P2.5|<------- Receive Data (UCA0RXD) --> 75
 *          |             P2.4|-------> Transmit Data (UCA0TXD) --> 76
 *          |                 |
 *          |             P1.5|<------- A3+: EEG1 positive input --> 51 
 *          |             P1.4|<------- A3-: EEG1 negative input --> 54
 *          |                 |
 *          |             P1.7|<------- A2+: EEG2 positive input --> 49 
 *          |             P1.6|<------- A2-: EEG2 negative input --> 50
 *          |                 | 
 *          |             P6.3|<------- A1+: EEG3 positive input --> 64 
 *          |             P6.4|<------- A1-: EEG3 negative input --> 63
 *          |                 |
 *          |             P6.0|<------- A0+: ECG positive input --> 67 
 *          |             P6.1|<------- A0-: ECG negative input --> 66
 *          |                 |
 *          |             P1.3|<------- A4+: BATT positive input --> 55 
 *          |             P1.2|<------- A4-: BATT negative input --> 56
 *          |                 |
 * @expected
 * The pins number 51 and 54 should present signals of the EEG. 
 * WARNING: IT IS NOT POSSIBLE TO EVALUATE ACLK AT THE SAME TIME: P1.5 CONTAINS THE ACLK TOO
 */
#include <msp430.h>
#include <stdint.h>

#include "../functions/general_functions.h"
#include "../functions/SD16_A.h"
#include "../functions/system_config.h"
#include "../functions/USCI.h"
#include "../functions/FLL.h"


unsigned int result;
bool CONFIG_SD16 = true;
uint16_t tx_val = 0;
uint8_t high_word = 0;
uint8_t low_word = 0;
uint16_t my_register = 0;
bool high_or_low = true;
bool state = 0;
int test = 0;

//******************************************************************************
// UART Initialization *********************************************************
//******************************************************************************


//******************************************************************************
// Main ************************************************************************
// Enters LPM0 if SMCLK is used and waits for UART interrupts. If ACLK is used *
// then the device will enter LPM3 mode instead. The UART RX interrupt handles *
// the received character and echoes it.                                       *
//******************************************************************************

//Character format configuration
    //True --> parity ON
    //False --> parity OFF            
//if parity enable:
    //parity_type:
        //E --> even (par)
        //O --> odd (impar)
//num_data_bit: quantity of data bits available (character length)
    //7 or 8
//num_stop_bit: stop bit select, one at least
    //1 or 2
//first_byte_sent: To choose between MSB or LSB 
    //M: MSB first
    //L: LSB first



void main()
{
    // stop watch dog
    stop_wd();

    /*
		- A: Active
		- L: Low Power
            - 0: LPM0
            - 1: LPM1
            - 2: LPM2 
            - 3: LPM3
    */
    const char operating_mode = 'A';
    select_operating_mode(operating_mode, 0);

    enable_interruptions(true);

    //CLK configuration
    initClockTo8MHz();
    if (CONFIG_SD16 == false){
        configure_PINS_for_clk_debug();        
    }

    //Init UART related peripherics
    init_UART_GPIO();

    //Config UART
    /*clk_ref:
        U --> UCLK
        A --> ACLK
        S --> SMCLK        
    */
    static const char USCI_clk_ref_sel = 'S';
    USCI_clk_ref(USCI_clk_ref_sel);    


    //Reference frequence chosen for UART operation
    static const int32_t BRCLK_freq = 8000000; //Source clock frequency
    /*Baudrate and BRCLK_freq combinations:

    BRCLK_freq      32768     1000000   1048576   4000000   8000000   12000000  16000000
    ------------------------------------------------------------------------------
                    1200      
                    2400      
                    4800      
                    9600       9600       9600      9600      9600      9600      9600
                     	       19200      19200     19200     19200     19200     19200
                               38400      38400     38400     38400     38400     38400
                               57600      57600     57600     57600     57600     57600
                               115200     115200    115200    115200    115200    115200
                                                    230400    230400    230400    230400
                                                              460800    460800    460800
    */ 
    static const int32_t baudrate = 115200;
    UART_baudrate_generation(BRCLK_freq, baudrate);
    USCI_init();                     // **Initialize USCI state machine**


    //parity enable:
        //True --> parity ON
        //False --> parity OFF            
    //if parity enable:
        //parity_type:
            //E --> even (par)
            //O --> odd (impar)
    //num_data_bit: quantity of data bits available (character length)
        //7 or 8
    //num_stop_bit: stop bit select, one at least
        //1 or 2
    //first_byte_sent: To choose between MSB or LSB 
        //M: MSB first
        //L: LSB first
    static const bool parity_enable = false;
    static const char parity_type = 'O';
    static const int num_data_bit = 8;
    static const int num_stop_bit = 1;
    static const char first_Byte_sent = 'L';
    character_format_sel(parity_enable, parity_type, num_data_bit, num_stop_bit, first_Byte_sent);

    //U --> Uart
    //I --> IDLE-LINE MULTIPROCESSOR MODE
    //D --> ADDRESS-BIT MULTIPROCESSOR MODE
    //A --> UART MODE WITH AUTOMATIC BAUD RATE DETECTION
    static const char USCI_mode = 'U';

    USCI_mode_sel(USCI_mode);



    if (CONFIG_SD16){
        
//***************************************************************************** 
/*SETUP SD16A*/


//Para muestrear a una frecuencia de 15kHz 
//fs = fm/OSR 

//tomando como referencia MCLK con frecuencia 8MHz -- fm = 8MHz 
//Ponemos OSR = 512 -- fs = 15.625 kHz

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
    bool const interruption_SD16A = true;
        // -- Over Sampling Ratio
    static const unsigned int OSR = 512; //1, 32, 64, 128, 256, 512, 1024
        // -- Ganancia
    static const int gain = 1; //1, 2, 4, 8, 16 or 32
        // -- Método de conversión
    static const char conv_mode = 'C'; // C: Continuous  S: Single
        // -- Tipo de datos
    static const char polarity = 'B';       // B : Bipolar, U : unipolar
    static const char sign = 'O';           // O : Offset, C : 2's complement


  
 //****************** FUNCTIONS
    setup_analog_input(analog_input);
    select_analog_input(analog_input);
    
    FLL_CTL0 |= XCAP14PF;                     // Configure load caps

    voltage_reference(v_ref);
    SD16_clk_reference(clk_ref); // M: MCLK, S: SMCLK, A: ACLK, T:TACLK
    fM_dividers(clk_div_1, clk_div_2);
    enable_interruption_SD16A(interruption_SD16A);
    config_OSR(OSR);
    gain_setup(gain);    
    conversion_mode(conv_mode); 
    data_format(polarity, sign);

    }
    

    start_conversion(); // While it is started, working in continuous mode will sample the channel A until it is stopped
    
    
    /*
    while (1) {
    

    while ((SD16CCTL0 & SD16IFG)==0);       // Poll interrupt flag
    my_register  = SD16MEM0;                // Save CH0 results (clears IFG)

    high_word = (my_register >> 8) & 0xFFFF; // 8 bits superiores (0x1234)
    low_word = my_register & 0xFFFF;          // 8 bits inferiores (0x5678)

 //   tx_val = SD16MEM0;                     
    
    //data_to_transmit(tx_val);
    //data_to_transmit(high_word);
    //data_to_transmit(low_word);
    
    }
    */

}



//***************************************************************************** 
//Interrupción de la UART
//***************************************************************************** 

//TX interrupt handler

#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=USCIAB0TX_VECTOR
__interrupt void USCI_A0_Tx (void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(USCIAB0TX_VECTOR))) USCI_A0_Tx (void)
#else
#error Compiler not supported!
#endif
/*
{
    //las siguientes lineas equivalen a utilizar la funcion data_to_transmit 
  if (state == 1){
  while(!(IFG2&UCA0TXIFG));
    if (high_or_low){
        UCA0TXBUF = high_word;                    // TX -> RXed character
    }else{
        UCA0TXBUF = low_word;                    // TX -> RXed character
        state = 0;
    }
    high_or_low = !high_or_low;
  }
}
*/
{
	/*
	UCA0TXBUF = test; // Envía el byte alto
     test++;
    */


    if (state == 1) {
        if (high_or_low) {
            UCA0TXBUF = high_word; // Envía el byte alto
        } else {
            UCA0TXBUF = low_word;  // Envía el byte bajo
            state = 0;  // Solo aquí finalizamos la transmisión completa
            IE2 &= ~(UCA0RXIE|UCA0TXIE); // Disabling UART interrupt until SD16 enables it again

        }
        high_or_low = !high_or_low;  // Alterna entre alto y bajo

    }

}



//***************************************************************************** 
//Interrupción del SD16_A
//***************************************************************************** 


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

