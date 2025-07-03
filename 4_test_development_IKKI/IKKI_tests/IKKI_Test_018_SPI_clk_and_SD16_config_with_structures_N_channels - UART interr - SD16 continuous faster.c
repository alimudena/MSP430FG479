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
//                |  ---  UART ---  |
//                |             P2.5|<------- Receive Data (UCA0RXD) --> 75
//                |             P2.4|-------> Transmit Data (UCA0TXD) --> 76
//                |                 |
//                |  ---  SPI  ---  |
//                |             P2.4|-> Data Out (UCA0SIMO) --> 76
//          LED <-|P4.6         P2.5|<- Data In (UCA0SOMI) --> 75
//                |             P3.0|-> Serial Clock Out (UCA0CLK) --> 41
//                |                 |
//                |  ---  SD16 ---  |

//                |             P1.5|<------- A3+: EEG1 positive input --> 51
//                azul oscuro |             P1.4|<------- A3-: EEG1 negative
//                input --> 54

//                |             P1.7|<------- A2+: EEG2 positive input --> 49
//                rosa |             P1.6|<------- A2-: EEG2 negative input -->
//                50

//                |             P6.3|<------- A1+: EEG3 positive input --> 64
//                gris |             P6.4|<------- A1-: EEG3 negative input -->
//                63

//                |             P6.0|<------- A0+: ECG positive input --> 67
//                blanco |             P6.1|<------- A0-: ECG negative input -->
//                66

//                |             P1.3|<------- A4+: BATT positive input --> 55
//                azul claro batería |             P1.2|<------- A4-: BATT
//                negative input --> 56

//                |                 |
//

#include "../functions/general_functions.h"
#include "../functions/system_config.h"
#include "msp430fg479.h"
#include <msp430.h>

#include "../functions/FLL.h"
#include "../functions/SD16_A.h"
#include "../functions/USCI.h"
#include "../functions/general_functions.h"
#include "../functions/system_config.h"

#include "IKKI_MAC.h"
#include "SD16A_config.h"
#include "SPI_config.h"
#include "UART_config.h"
#include "clk_config.h"

int state = 1;
uint8_t high_word = 0x00;
uint8_t low_word = 0xFF;
volatile uint16_t my_register = 0xabcd;
bool high_or_low = true;
#define SD16_ENABLE true
#define CLK_DEBUG true
int counter;
int i;

UART_config_struct UART_config;
SPI_config_struct SPI_config;
CLK_config_struct CLK_config;
SD16A_config_struct SD16A_configuration;

void general_setup() {

  //*****************************************************************************
  /*GENERAL SETUP*/
  //*****************************************************************************

  stop_wd();
  CLK_config.CLK_debug = false;
  if (CLK_config.CLK_debug) {
    configure_PINS_for_clk_debug();
    // setup pin for led for toggle
    toggle_setup();
  }
}

void setup_CLK() {
  //*****************************************************************************
  /*SETUP CLK*/
  /*For generating the 8MHz:
      CLK_config.operating_mode = 'A';
      CLK_config.LFXT1_wk_mode = 'L';
      CLK_config.DCO_range = 4;
      CLK_config.DCOPLUS_on = true; //If D factor is wanted to be applied then
     -> True CLK_config.D_val = 2; //Max 8 CLK_config.N_MCLK = 121; //Max 127
      CLK_config.ref_MCLK = 'D'; //  D: DCO, X: XT2, A: LFXT1
      CLK_config.ref_SMCLK = 'D'; // D: DCO, X: XT2, N: OFF
      CLK_config.divider_ACLK = 1; // 1, 2, 4, 8
      CLK_config.LFXT2_osc_on = false;
  */
  //*****************************************************************************

  // Modo de operación
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
  // Oscilador LFXT1
  /*
      L: Low Frequency Mode --> f auxiliar de 323kHz conectado
      H: High Frequency Mode
  */
  CLK_config.LFXT1_wk_mode = 'L';
  // Configura la capacidad interna del LFXT1
  /*0  --> XIN Cap = XOUT Cap = 0pf */
  /*10 --> XIN Cap = XOUT Cap = 10pf */
  /*14 --> XIN Cap = XOUT Cap = 14pf */
  /*18 --> XIN Cap = XOUT Cap = 18pf */
  CLK_config.LFXT1_int_cap = 18;
  LFXT1_working_mode(CLK_config.LFXT1_wk_mode);
  LFXT1_internal_cap_config(CLK_config.LFXT1_int_cap);
  // DCO
  // Rango de frecuencia de trabajo del DCO:
  /*2 --> fDCOCLK =   1.4-12MHz*/
  /*3 --> fDCOCLK =   2.2-17Mhz*/
  /*4 --> fDCOCLK =   3.2-25Mhz*/ //-> 8 MHz
  /*8 --> fDCOCLK =     5-40Mhz*/
  CLK_config.DCO_range = 4;
  DCO_f_range(CLK_config.DCO_range);
  // Values for setting the frequency of the DCO+
  // DCO+ set so freq= xtal x D x N_MCLK+1
  // XTAL --> 32767Hz
  CLK_config.DCOPLUS_on =
      true;                // If D factor is wanted to be applied then -> True
  CLK_config.D_val = 2;    // Max 8
  CLK_config.N_MCLK = 121; // Max 127
  configuring_DCO(CLK_config.DCOPLUS_on, CLK_config.D_val);
  configure_N_for_MCLK(CLK_config.N_MCLK);
  // MCLK
  // Reference selection for MCLK
  CLK_config.ref_MCLK = 'D'; //  D: DCO, X: XT2, A: LFXT1
  select_reference_MCLK(CLK_config.ref_MCLK);
  // SMCLK
  //  Reference for SMCLK
  CLK_config.ref_SMCLK = 'D'; // D: DCO, X: XT2, N: OFF
  select_reference_SMCLK(CLK_config.ref_SMCLK);
  // ACLK
  // ACLK division for configuring ACLK/N
  CLK_config.divider_ACLK = 1; // 1, 2, 4, 8
  configure_ACLK_N(CLK_config.divider_ACLK);
  // LFXT2
  // Second oscillator ON OFF
  CLK_config.LFXT2_osc_on = false;
  LFXT2_disable(CLK_config.LFXT2_osc_on);
}

void SPI_configuration() {

  USCI_SPI_pin_setup();

  SPI_config.Master_Slave = 'M';
  SPI_mode_config(SPI_config.Master_Slave);

  SPI_config.inactive_state = 'H';     // clock polarity inactive high
  SPI_config.data_on_clock_edge = 'A'; // data cAptured on the first UCLK edge
                                       // and changed on the following edge
  SPI_clk_polarity_phase(SPI_config.inactive_state,
                         SPI_config.data_on_clock_edge);

  SPI_config.SPI_length = 8;
  SPI_config.first_Byte_sent = 'M';
  SPI_char_format(SPI_config.SPI_length,
                  SPI_config.first_Byte_sent); // 8-bit and MSB SPI

  /*clk_ref:
      U --> UCLK
      A --> ACLK
      S --> SMCLK
  */
  SPI_config.clk_ref_SPI = 'S';
  USCI_clk_ref(SPI_config.clk_ref_SPI); // CLK reference

  SPI_config.clk_div = 2;
  SPI_clk_division(SPI_config.clk_div);

  USCI_init(); // **Initialize USCI state machine**

  SPI_config.enable_USCI_interr_rx = false;
  SPI_config.enable_USCI_interr_tx = false;
  USCI_interrupt_enable(
      SPI_config.enable_USCI_interr_rx,
      SPI_config.enable_USCI_interr_tx); // Enable USCI_A0 RX interrupt
}

void setup_SD16A() {

  // -- Entrada analógica
  SD16A_configuration.analog_input_count = 5;
  SD16A_configuration.analog_input_being_sampled = 0;

  SD16A_configuration.analog_input[0] = EEG1; // 3: A3 - EEG1
  SD16A_configuration.analog_input[1] = EEG2; // 2: A2 - EEG2
  SD16A_configuration.analog_input[2] = EEG3; // 1: A1 - EEG3
  SD16A_configuration.analog_input[3] = ECG;  // 0: A0 - ECG
  SD16A_configuration.analog_input[4] = BATT; // 4: A4 - BATT

  SD16A_configuration.analog_input_ID[0] = 0x30; // 3: A3 - EEG1
  SD16A_configuration.analog_input_ID[1] = 0x31; // 2: A2 - EEG2
  SD16A_configuration.analog_input_ID[2] = 0x32; // 1: A1 - EEG3
  SD16A_configuration.analog_input_ID[3] = 0x33; // 0: A0 - ECG
  SD16A_configuration.analog_input_ID[4] = 0x34; // 4: A4 - BATT

  // -- Tensión de referencia
  SD16A_configuration.v_ref =
      'I'; // I: Internal (1.2V), O: Off-chip, E: External
           // -- Reloj de referencia
  SD16A_configuration.clk_ref = 'M'; // M: MCLK, S: SMCLK, A: AC1LK, T: TACLK
                                     // -- Divisor de frecuencia de referencia
  SD16A_configuration.clk_div_1 = 1;
  SD16A_configuration.clk_div_2 = 1;
  // -- Método de lectura: Polling o Interrupciones
  SD16A_configuration.interruption_SD16A = true;
  // -- Over Sampling Ratio
  SD16A_configuration.OSR = 512; // 1, 32, 64, 128, 256, 512, 1024
  // -- Ganancia
  SD16A_configuration.gain = 1; // 1, 2, 4, 8, 16 or 32
  // -- Método de conversión
  SD16A_configuration.conv_mode = 'C'; // C: Continuous  S: Single
                                       // -- Tipo de datos
  SD16A_configuration.polarity = 'U';  // B : Bipolar, U : unipolar
  SD16A_configuration.sign = 'O';      // O : Offset, C : 2's complement

  SD16A_configuration.sampled = false;

  for (counter = SD16A_configuration.analog_input_count; counter > 0;
       counter--) {
    setup_analog_input(SD16A_configuration.analog_input[counter - 1]);
  }
  select_analog_input(
      SD16A_configuration
          .analog_input[SD16A_configuration.analog_input_being_sampled]);
  FLL_CTL0 |= XCAP14PF; // Configure load caps
  for (i = 10000; i > 0; i--)
    ; // Delay for 32 kHz crystal to

  voltage_reference(SD16A_configuration.v_ref);
  SD16_clk_reference(
      SD16A_configuration.clk_ref); // M: MCLK, S: SMCLK, A: ACLK, T:TACLK
  fM_dividers(SD16A_configuration.clk_div_1, SD16A_configuration.clk_div_2);
  for (i = 10000; i > 0; i--)
    ; // Delay for 1.2V ref startup
  config_OSR(SD16A_configuration.OSR);
  gain_setup(SD16A_configuration.gain);
  conversion_mode(SD16A_configuration.conv_mode);
  data_format(SD16A_configuration.polarity, SD16A_configuration.sign);
}
int main(void) {

  general_setup();
  init_MSP();

  //************************** CLK configuration *****************************
  setup_CLK();

  //************************** LED configuration *****************************
  toggle_setup(); // Setup P4.6 for LED output

  //************************** SPI configuration *****************************
  SPI_configuration();

  //************************** SD16 configuration *****************************
  setup_SD16A();

  enable_interruption_SD16A(true);
  start_conversion(); // While it is started, working in continuous mode will
                      // sample the channel A until it is stopped

  UCA0TXBUF = 0x45; // Transmit first character

  enable_interruptions(true);
}

/*
State machine:
1. Send Channel ID being sampled
2. Sample
3. Send sample (x2)

State changes:
1 - 2 : interruption SD16 ON
        interruption UART OFF
2 - 3 : interruption SD16 OFF
        interruption UART ON
3 - 1 : interruption SD16 OFF
        interruption UART ON

*/

//*****************************************************************************
// Interrupción del SPI
//*****************************************************************************

#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector = USCIAB0RX_VECTOR
__interrupt void USCIA0RX_ISR(void)
#elif defined(__GNUC__)
void __attribute__((interrupt(USCIAB0RX_VECTOR))) USCIA0RX_ISR(void)
#else
#error Compiler not supported!
#endif
{

  if (state == 1) {
    UCA0TXBUF = SD16A_configuration.analog_input_ID[SD16A_configuration.analog_input_being_sampled];
    state = 2;
    SD16CCTL0 |= SD16IE;         // Enabling SD16 interrupt
    IE2 &= ~(UCA0RXIE); // Disabling UART interrupt
  }
  if (state == 3) {
    if (high_or_low) {
      UCA0TXBUF = high_word;      // Envía el byte alto
      high_or_low = !high_or_low; // Alterna entre alto y bajo

    } else {
      UCA0TXBUF = low_word; // Envía el byte bajo
      state = 1; // Solo aquí finalizamos la transmisión completa de los bytes
      SD16CCTL0 &= ~(SD16IE);                   // Disabling SD16
      // interrupt
      IE2 |= UCA0RXIE;            // Enabling UART interrupt
      high_or_low = !high_or_low; // Alterna entre alto y bajo
    }
  }

}

//*****************************************************************************
// Interrupción del SD16_A
//*****************************************************************************

#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector = SD16A_VECTOR
__interrupt void SD16ISR(void)
#elif defined(__GNUC__)
void __attribute__((interrupt(SD16A_VECTOR))) SD16ISR(void)
#else
#error Compiler not supported!
#endif
{
  switch (SD16IV) {
  case 2: // SD16MEM Overflow
    state = 5;
    break;
  case 4: // SD16MEM0 IFG
    if (state == 2) {
      my_register = SD16MEM0; // Save CH0 results (clears IFG)
      SD16A_configuration.analog_input_being_sampled++;
      if (SD16A_configuration.analog_input_being_sampled == SD16A_configuration.analog_input_count) {
        SD16A_configuration.analog_input_being_sampled = 0;
      }
      // select_analog_input(SD16A_configuration.analog_input[SD16A_configuration.analog_input_being_sampled]);
      // First, clean the bits so to not have more than one channel reading
      SD16INCTL0 &=  ~(SD16INCH_0 | SD16INCH_1 | SD16INCH_2 | SD16INCH_3 | SD16INCH_4);
      SD16INCTL0 |= SD16A_configuration.analog_input[SD16A_configuration.analog_input_being_sampled];

      high_word = (my_register >> 8) & 0xFFFF; // 8 bits superiores (0x1234)
      low_word = my_register & 0xFFFF;         // 8 bits inferiores (0x5678)
      state = 3;
      IE2 |= UCA0RXIE; // Enabling UART interrupt
      SD16CCTL0 &= ~(SD16IE);                   // Disabling SD16
      //        interrupt
    } else {
      IE2 |= UCA0RXIE; // Enabling UART interrupt
      SD16CCTL0 &= ~(SD16IE);                   // Disabling SD16 interrupt
    }

    break;
  }
}
