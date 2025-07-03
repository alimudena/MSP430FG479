/**
 * @test UART working
 * @brief Tests that makes sure that the UART is working properly.
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
#include "../functions/USCI.h"
#include "../functions/FLL.h"
//******************************************************************************
// UART Initialization *********************************************************
//******************************************************************************

#define SMCLK_115200     0
#define SMCLK_9600      1
#define ACLK_9600       2

#define UART_MODE       ACLK_9600//SMCLK_115200//

void SendUCA0Data(uint8_t data)
{
    while (!(IFG2&UCA0TXIFG));                // USCI_A0 TX buffer ready?
    UCA0TXBUF = data;
}
void initUART()
{
#if UART_MODE == SMCLK_115200
    UCA0CTL1 |= UCSSEL_2;                     // SMCLK
    UCA0BR0 = 69;                             // 8MHz 115200
    UCA0BR1 = 0;                              // UCBRx = 69
    UCA0MCTL = UCBRS_4;                       // Modulation UCBRSx = 4
    UCA0CTL1 &= ~UCSWRST;                     // **Initialize USCI state machine**
    IFG2 &= ~(UCA0RXIFG);
    IE2 |= UCA0RXIE;                          // Enable USCI_A0 RX interrupt
#elif UART_MODE == SMCLK_9600
    UCA0CTL1 |= UCSSEL_2;                     // SMCLK
    UCA0BR0 = 0x41;                           // 8MHz 9600
    UCA0BR1 = 0x03;                           // UCBRx = 833 = 0x0341
    UCA0MCTL = UCBRS_2;                       // Modulation UCBRSx = 2
    UCA0CTL1 &= ~UCSWRST;                     // **Initialize USCI state machine**
    IFG2 &= ~(UCA0RXIFG);
    IE2 |= UCA0RXIE;                          // Enable USCI_A0 RX interrupt
#elif UART_MODE == ACLK_9600
    UCA0CTL1 |= UCSSEL_1;                     // ACLK
    UCA0BR0 = 3;                              // 32768Hz 9600
    UCA0BR1 = 0;                              // UCBRx = 3
    UCA0MCTL = UCBRS_3;                       // Modulation UCBRSx = 3
    UCA0CTL1 &= ~UCSWRST;                     // **Initialize USCI state machine**
    IFG2 &= ~(UCA0RXIFG);
#else
    #error "Select UART Baud Rate of 115200 or 9600"
#endif
}

//******************************************************************************
// Device Initialization *******************************************************
//******************************************************************************


void initGPIO()
{
    P2SEL = BIT4 | BIT5;                      // P2.5 = RXD, P2.4=TXD
}

//******************************************************************************
// Main ************************************************************************
// Enters LPM0 if SMCLK is used and waits for UART interrupts. If ACLK is used *
// then the device will enter LPM3 mode instead. The UART RX interrupt handles *
// the received character and echoes it.                                       *
//******************************************************************************

void main()
{
    WDTCTL = WDTPW + WDTHOLD;                 // Stop WDT
    initClockTo8MHz();
    initGPIO();
    initUART();
    int i = 5;
    int j = 0;
    while(1){
         SendUCA0Data(i);
        i++;
      for(j = 0; j<8000; j++);
    }

}

