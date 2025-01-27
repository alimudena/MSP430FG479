//******************************************************************************
//   MSP430x47x Demo - USCI_A0, UART Echo received character
//                     (ACLK 9600/SMCLK 9600/SMCLK 115200)
//
//   Description: The device will wait in LPM0/LPM3 (based on clock source)
//   until a UART character is received.
//   Then the device will echo the received character.
//   The UART can operate using ACLK at 9600, SMCLK at 115200 or SMCLK at 9600.
//   To configure the UART mode, change the following line:
//
//      #define UART_MODE       SMCLK_115200
//      to any of:
//      #define UART_MODE       SMCLK_115200
//      #define UART_MODE       SMCLK_9600
//      #define UART_MODE       ACLK_9600
//
//   UART RX ISR is used to handle communication.
//   ACLK = 32.768kHz, MCLK = SMCLK = DCO 8MHz.
//
//
//                   MSP430FG479
//                 -----------------
//            /|\ |              XIN|---+
//             |  |                 |   |
//             ---|RST              |   32kHz
//                |                 |   |
//                |             XOUT|---+
//                |             P2.5|<------- Receive Data (UCA0RXD)
//                |             P2.4|-------> Transmit Data (UCA0TXD)
//                |                 |
//                |                 |
//
//   Nima Eskandari and Ryan Meredith
//   Texas Instruments Inc.
//   February 2018
//   Built with CCS V7.3



// ADDED A SELF DEVELOPED FUNCTION FOR SELECTING CHAR FORMAT


//******************************************************************************

#include <msp430.h>
#include <stdint.h>

#include "../functions/general_functions.h"
#include "../functions/SD16_A.h"
#include "../functions/USCI.h"

//******************************************************************************
// UART Initialization *********************************************************
//******************************************************************************

#define SMCLK_115200     0
#define SMCLK_9600      1
#define ACLK_9600       2

#define UART_MODE       ACLK_9600//SMCLK_115200//

char initUART()
{
#if UART_MODE == SMCLK_115200
    char USCI_clk_ref_sel = 'S';
    //Selection of the referemce ,pde
    USCI_clk_ref(USCI_clk_ref_sel);    
    int32_t BRCLK_freq = 8000000;
    int32_t baudrate = 115200;
    UART_baudrate_generation(BRCLK_freq, baudrate);
/*
    UCA0BR0 = 69;                             // 8MHz 115200
    UCA0BR1 = 0;                              // UCBRx = 69
    UCA0MCTL = UCBRS_4;                       // Modulation UCBRSx = 4
*/
    USCI_init();                  // **Initialize USCI state machine**
    IFG2 &= ~(UCA0RXIFG);
    IE2 |= UCA0RXIE;                          // Enable USCI_A0 RX interrupt
#elif UART_MODE == SMCLK_9600
    char USCI_clk_ref_sel = 'S';
    //Selection of the referemce ,pde
    USCI_clk_ref(USCI_clk_ref_sel);
    int32_t BRCLK_freq = 8000000;
    int32_t baudrate = 115200;
    UART_baudrate_generation(BRCLK_freq, baudrate);
    /*
    UCA0BR0 = 0x41;                           // 8MHz 9600
    UCA0BR1 = 0x03;                           // UCBRx = 833 = 0x0341
    UCA0MCTL = UCBRS_2;                       // Modulation UCBRSx = 2
    */
    USCI_init();                     // **Initialize USCI state machine**
    IFG2 &= ~(UCA0RXIFG);
    IE2 |= UCA0RXIE;                          // Enable USCI_A0 RX interrupt
#elif UART_MODE == ACLK_9600
    char USCI_clk_ref_sel = 'A';
    //Selection of the referemce ,pde
    USCI_clk_ref(USCI_clk_ref_sel);    
    int32_t BRCLK_freq = 32768;
    int32_t baudrate = 9600;
    UART_baudrate_generation(BRCLK_freq, baudrate);
    USCI_init();                     // **Initialize USCI state machine**
    IFG2 &= ~(UCA0RXIFG);
    IE2 |= UCA0RXIE;                          // Enable USCI_A0 RX interrupt
#else
    #error "Select UART Baud Rate of 115200 or 9600"
#endif
return USCI_clk_ref_sel;
}

//******************************************************************************
// Device Initialization *******************************************************
//******************************************************************************

void initClockTo8MHz()
{
    volatile unsigned int i;

    FLL_CTL0 |= DCOPLUS + XCAP14PF;           // DCO+ set, freq = xtal x D x N+1

    // Wait for xtal to stabilize
    do
    {
    IFG1 &= ~OFIFG;                           // Clear OSCFault flag
    for (i = 0x47FF; i > 0; i--);             // Time for flag to set
    }
    while ((IFG1 & OFIFG));                   // OSCFault flag still set?

    for(i=2100;i>0;i--);                      // Now with stable ACLK, wait for
                                              // DCO to stabilize.
    SCFI0 |= FN_4;                            // x2 DCO freq, 8MHz nominal DCO
    SCFQCTL = 121;                            // (121+1) x 32768 x 2 = 8 MHz
}


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
    
    //CLK configuration
    


    initClockTo8MHz();
    init_UART_GPIO();
    initUART();

    bool parity_enable = true;
    char parity_type = 'O';
    int num_data_bit = 7;
    int num_stop_bit = 2;
    char first_Byte_sent = 'L';
    character_format_sel(parity_enable, parity_type, num_data_bit, num_stop_bit, first_Byte_sent);

    //U --> Uart
    //I --> IDLE-LINE MULTIPROCESSOR MODE
    //D --> ADDRESS-BIT MULTIPROCESSOR MODE
    //A --> UART MODE WITH AUTOMATIC BAUD RATE DETECTION
    char USCI_mode = 'U';

    USCI_mode_sel(USCI_mode);



#if UART_MODE == ACLK_9600
    __bis_SR_register(LPM3_bits + GIE);       // Since ACLK is source, enter LPM3, interrupts enabled
#else
    __bis_SR_register(LPM0_bits + GIE);       // Since SMCLK is source, enter LPM0, interrupts enabled
#endif
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
