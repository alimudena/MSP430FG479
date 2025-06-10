#include <msp430.h>
#include <stdio.h>
#include <stdint.h>  // Necesario para uint16_t
#include "../functions/general_functions.h"
#include "../functions/SD16_A.h"

unsigned int result;
int main(void){
    volatile unsigned int i;                  // Use volatile to prevent removal
                                            // by compiler optimization
    // stop watch dog
    stop_wd();

    //setup pin for led for toggle
    toggle_setup();

    volatile unsigned int analog_input = 0;
    
    // Setup and selection of the Analog input wanted
    setup_analog_input(analog_input);
    select_analog_input(analog_input);
    
    FLL_CTL0 |= XCAP14PF;                     // Configure load caps
    for (i = 0; i < 10000; i++);              // Delay for 32 kHz crystal to
    
    static const char v_ref = 'I';            // I: Internal (1.2V), O: Off-chip, E: External
    voltage_reference(v_ref);                   

    SD16CTL |= SD16SSEL0;                     // SMCLK
    SD16CCTL0 |= SD16IE;            //Enable interrupt

    volatile unsigned int gain = 1;
    gain_setup(gain);
    
    static const char conv_mode = 'S'; // C: Continuous  S: Single
    conversion_mode(conv_mode); 

    //SUT
    static const char polarity = 'B';       // B : Bipolar, U : unipolar
    static const char sign = 'O';           // O : Offset, C : 2's complement

    data_format(polarity, sign);
    //SUT end

    SD16INCTL0 |= SD16INTDLY_0;               // Interrupt on 4th sample  
    for (i = 0; i < 0x3600; i++);             // Delay for 1.2V ref startup

    while(1){
        start_conversion();
        
        //enter_LPM();
        toggle_pin();
       // fprintf(stdout, "Resultado de la conversión: %d\n", result);
       // __bis_SR_register(LPM0_bits+GIE);

    };    

}

#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=SD16A_VECTOR
__interrupt void SD16ISR(void)
#elif defined(__GNUC__)A
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
    result = SD16MEM0;                      // Save CH0 results (clears IFG)
    break;
  }

  //__bic_SR_register_on_exit(LPM0_bits);                   // Exit LPM0
}


