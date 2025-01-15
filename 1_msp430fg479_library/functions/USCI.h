#include <msp430.h>
#include <stdbool.h> 

//*****************************************************************************
/*USCI RELATED FUNCTIONS*/
//*****************************************************************************
//The configuration steps for the USCI initialization/re-configuration process is:
//First set UCSWRST 
//Initialize USCI registers with UCSWRST = 1
//Configure ports
//Clear UCSWRST
//Enable interrupts (optional)

void USCI_reset();

void USCI_init();

//*****************************************************************************
/*UART RELATED FUNCTIONS*/
//*****************************************************************************
// character format 
void character_format_sel(bool parity_enable, char parity_type, int num_data_bit, int num_stop_bit, char first_Byte_sent);

// USCI mode
void USCI_mode_sel(char USCI_mode);

// inicializacion del modo UART -> limpieza del registro UCSYNC 

// IrDA encoding(decoding) enable
void IrDA_enable(bool IrDA_enabled);
// Encoding
void IrDA_pulse_config(char IrDA_clk_ref, int IrDA_Transmit_Pulse_Length);

// Decoding
void IrDA_detect_value(int H_L_detect);
void IrDA_decoding_filter(bool IrDA_dec_filter_enabled, int IrDA_Receive_Filter_Length);

