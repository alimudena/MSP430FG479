
//                   MSP430FG479
//                 -----------------
//            /|\ |              XIN|---+
//             |  |                 |   |
//             ---|RST              |   32kHz
//                |                 |   |
//                |             XOUT|---+
//                |             P1.1|--> MCLK = 8Mhz  --> 57 (referencia DCO)
//                |             P1.4|--> SMCLK = 8MHz --> 54 (referencia DCO)
//                |             P1.5|--> ACLK = 32kHz --> 51
//                |             P2.5|<------- Receive Data (UCA0RXD) --> 75
//                |             P2.4|-------> Transmit Data (UCA0TXD) --> 76
//                |                 |
//        Vin+ -->|A1+              |
//        Vin- -->|A1-              |
//                |                 |

//******************************************************************************

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
    const char operating_mode = 'A';
    select_operating_mode(operating_mode, 0);
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
    static const char USCI_clk_ref_sel = 'A';
    USCI_clk_ref(USCI_clk_ref_sel);    


    //Reference frequence chosen for UART operation
    static const int32_t BRCLK_freq = 32768;
    /*Baudrate wanted to be performed:
        Options: 9600, 19200, 38400, 57600, 115200, 230400, 460800 
    */
    static const int32_t baudrate = 9600;
    UART_baudrate_generation(BRCLK_freq, baudrate);
    USCI_init();                     // **Initialize USCI state machine**

    static const bool parity_enable = false;
    static const char parity_type = 'O';
    static const int num_data_bit = 8;
    static const int num_stop_bit = 2;
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
  
 //****************** FUNCTIONS
    setup_analog_input(analog_input);
    select_analog_input(analog_input);
    
    FLL_CTL0 |= XCAP14PF;                     // Configure load caps

    voltage_reference(v_ref);
    clk_reference(clk_ref); // M: MCLK, S: SMCLK, A: ACLK, T:TACLK
    fM_dividers(clk_div_1, clk_div_2);
    enable_interruption_SD16A(interruption_SD16A);
    config_OSR(OSR);
    gain_setup(gain);    
    conversion_mode(conv_mode); 
    data_format(polarity, sign);

}
while (1) {
    

    start_conversion();
    while ((SD16CCTL0 & SD16IFG)==0);       // Poll interrupt flag
    my_register  = SD16MEM0;                // Save CH0 results (clears IFG)

    high_word = (my_register >> 8) & 0xFFFF; // 8 bits superiores (0x1234)
    low_word = my_register & 0xFFFF;          // 8 bits inferiores (0x5678)

 //   tx_val = SD16MEM0;                     
    
    //data_to_transmit(tx_val);
    data_to_transmit(high_word);
    data_to_transmit(low_word);


  }

}
