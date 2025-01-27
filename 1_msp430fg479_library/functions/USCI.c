#include "msp430fg479.h"
#include <msp430.h>
#include <stdio.h>
#include <stdint.h>  // Neccesary for  uint16_t
#include "SD16_A.h"
#include <assert.h>  // For using assert
#include <stdbool.h> 


//*****************************************************************************
/*USCI INITIALIZATION AND RESET FUNCTIONS*/
//*****************************************************************************

void USCI_reset(){
    //Set the bit UCSWRST in register USCI_A0 Control Register 1 
    UCA0CTL1 |= UCSWRST;

}

void USCI_init(){
    //Reset the bit UCSWRST in register USCI_A0 Control Register 1 
    UCA0CTL1 &= ~(UCSWRST);

}


void init_UART_GPIO()
{
    P2SEL = BIT4 | BIT5;                      // P2.5 = RXD, P2.4=TXD
}


void USCI_clk_ref(char clk_ref){
    UCA0CTL1 &= ~(UCSSEL_0|UCSSEL_1|UCSSEL_2|UCSSEL_3);
    switch (clk_ref) {
        case 'U':
            UCA0CTL1|=UCSSEL_0;
            break;
         
    }
}


//*****************************************************************************
/*UART RELATED FUNCTIONS*/
//*****************************************************************************

//Character format decision
void character_format_sel(bool parity_enable, char parity_type, int num_data_bit, int num_stop_bit, char first_Byte_sent){
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
    
    // first clear all bits
    UCA0CTL0 &= ~(UCPEN|UCPAR|UCMSB|UC7BIT|UCSPB);
    //parity enable
    if (parity_enable){
        UCA0CTL0 |= UCPEN;
        switch (parity_type) {
            case 'E':
                UCA0CTL0 |= UCPAR;
                break;
            case 'O':
                break;
            default:
                perror("Error: Not available parity in UART selection.");
                break;
        }
    }

    switch (num_data_bit) {
        case 7:
            UCA0CTL0 |= UC7BIT;
            break;
        case 8:
            break;
        default:
            perror("Error: Not available number of data bits in UART selection.");
            break;
    }
    
    switch (num_stop_bit) {
        case 1:
            break;
        case 2:
            UCA0CTL0 |= UCSPB;
            break;            
        default:
            perror("Error: Not available number of stop bits in UART selection.");
            break;
    }
    
    switch (first_Byte_sent){
        case 'L':
            break;
        case 'M':
            UCA0CTL0 |= UCMSB;        
            break;
        default:
            perror("Error: Not available first Byte  in UART selection.");
            break;
    }
}

void USCI_mode_sel(char USCI_mode){
    //U --> Uart
    //I --> IDLE-LINE MULTIPROCESSOR MODE
    //D --> ADDRESS-BIT MULTIPROCESSOR MODE
    //A --> UART MODE WITH AUTOMATIC BAUD RATE DETECTION
    UCA0CTL0 &= ~(UCMODE0|UCMODE1);
    switch (USCI_mode) {
        case 'U': //UART
            break;
        case 'I': //IDLE-LINE MULTIPROCESSOR MODE
            UCA0CTL0 |= UCMODE0;
            break;
        case 'D': //ADDRESS-BIT MULTIPROCESSOR MODE
            UCA0CTL0 |= UCMODE1;
            break;
        case 'A'://UART MODE WITH AUTOMATIC BAUD RATE DETECTION
            UCA0CTL0 |= UCMODE1+UCMODE0;
            break;
        default:
            perror("Error: Not available UART mode.");
            break;
    }
}


void data_to_transmit(uint8_t data){
    while (!(IFG2&UCA0TXIFG));                // The register for the data only can be written if the flag UCA0TXIFG is up
    UCA0TXBUF = data;
}

void UART_baudrate_generation(int ref_frec, int baudrate_wanted){
    
}

// IrDA encoding enable
void IrDA_enable(bool IrDA_enabled){
    UCA0IRTCTL &= ~(UCIREN);    
    if (IrDA_enabled){
        UCA0IRTCTL |= UCIREN;
    }
}

void IrDA_pulse_config(char IrDA_clk_ref, int IrDA_Transmit_Pulse_Length){
/* UCIRTXPL5 --> IRDA Transmit Pulse Length 5 */
/* UCIRTXPL4 --> IRDA Transmit Pulse Length 4 */
/* UCIRTXPL3 --> IRDA Transmit Pulse Length 3 */
/* UCIRTXPL2 --> IRDA Transmit Pulse Length 2 */
/* UCIRTXPL1 --> IRDA Transmit Pulse Length 1 */
/* UCIRTXPL0 --> IRDA Transmit Pulse Length 0 */
    UCA0IRTCTL &= ~(UCIRTXCLK|UCIRTXPL5|UCIRTXPL4|UCIRTXPL3|UCIRTXPL2|UCIRTXPL1|UCIRTXPL0);    
    switch (IrDA_clk_ref) {
        case 'I': // BITCLK16 
            UCA0IRTCTL |= UCIRTXCLK;
            break;
        case 'R': // BRCLK 
            break;
        default:
            perror("Error: Not available IrDA clk reference.");
            break; 
    }
    switch (IrDA_Transmit_Pulse_Length) {
        case 5:
            UCA0IRTCTL |= UCIRTXPL5;        
            break;
        case 4:
            UCA0IRTCTL |= UCIRTXPL4;        
            break;
        case 3:
            UCA0IRTCTL |= UCIRTXPL3;        
            break;
        case 2:
            UCA0IRTCTL |= UCIRTXPL2;        
            break;
        case 1:
            UCA0IRTCTL |= UCIRTXPL1;        
            break;
        case 0:
            UCA0IRTCTL |= UCIRTXPL0;        
            break;
        default:
            perror("Error: Not available IrDA clk divider proportion.");
            break; 
    }    

}

void IrDA_detect_value(int H_L_detect){
    UCA0IRRCTL &= ~(UCIRRXPL);
    if (H_L_detect==0){
        UCA0IRRCTL |= UCIRRXPL;
    }
}


void IrDA_decoding_filter(bool IrDA_dec_filter_enabled, int IrDA_Receive_Filter_Length){
/*UCIRRXFL5 -->  IRDA Receive Filter Length 5 */
/*UCIRRXFL4 -->  IRDA Receive Filter Length 4 */
/*UCIRRXFL3 -->  IRDA Receive Filter Length 3 */
/*UCIRRXFL2 -->  IRDA Receive Filter Length 2 */
/*UCIRRXFL1 -->  IRDA Receive Filter Length 1 */
/*UCIRRXFL0 -->  IRDA Receive Filter Length 0 */
    UCA0IRRCTL &= ~(UCIRRXFE|UCIRRXFL5|UCIRRXFL4|UCIRRXFL3|UCIRRXFL2|UCIRRXFL1|UCIRRXFL0);
    if (IrDA_dec_filter_enabled){
        UCA0IRRCTL |= UCIRRXFE;
    }
    switch (IrDA_Receive_Filter_Length) {
        case 5:
            UCA0IRRCTL |= UCIRRXFL5;
            break;
        case 4:
            UCA0IRRCTL |= UCIRRXFL4;
            break;
        case 3:
            UCA0IRRCTL |= UCIRRXFL3;
            break;
        case 2:
            UCA0IRRCTL |= UCIRRXFL2;
            break;
        case 1:
            UCA0IRRCTL |= UCIRRXFL1;
            break;
        case 0:
            UCA0IRRCTL |= UCIRRXFL0;
            break;
        default:
            perror("Error: Not available IrDA decoder digital filter configuration.");
            break; 
    }
}



