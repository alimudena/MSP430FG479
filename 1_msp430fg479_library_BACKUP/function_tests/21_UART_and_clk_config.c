
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
//                |                 |

//******************************************************************************

#include <msp430.h>
#include <stdint.h>

#include "../functions/general_functions.h"
#include "../functions/SD16_A.h"
#include "../functions/system_config.h"
#include "../functions/USCI.h"
#include "../functions/FLL.h"

//******************************************************************************
// UART Initialization *********************************************************
//******************************************************************************

#define SMCLK_115200     0
#define SMCLK_9600      1
#define ACLK_9600       2

#define UART_MODE       ACLK_9600//SMCLK_115200//SMCLK_9600

//******************************************************************************
// Device Initialization *******************************************************
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
    static const int num_data_bit = 7;
    static const int num_stop_bit = 2;
    static const char first_Byte_sent = 'L';
    character_format_sel(parity_enable, parity_type, num_data_bit, num_stop_bit, first_Byte_sent);

    //U --> Uart
    //I --> IDLE-LINE MULTIPROCESSOR MODE
    //D --> ADDRESS-BIT MULTIPROCESSOR MODE
    //A --> UART MODE WITH AUTOMATIC BAUD RATE DETECTION
    static const char USCI_mode = 'U';

    USCI_mode_sel(USCI_mode);
    if (USCI_clk_ref_sel=='A'){
            __bis_SR_register(LPM3_bits + GIE);       // Since ACLK is source, enter LPM3, interrupts enabled
    }

}

//******************************************************************************
// UART RX Interrupt ***********************************************************
//******************************************************************************

#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=USCIAB0RX_VECTOR
__interrupt void USCI0RX_ISR(void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(USCIAB0RX_VECTOR))) USCI0RX_ISR (void)
#else
#error Compiler not supported!
#endif
{
    if (IFG2 & UCA0RXIFG)
    {
        uint8_t rx_val = UCA0RXBUF; //Must read UCxxRXBUF to clear the flag
        data_to_transmit(rx_val);
    }
}
