#include <msp430.h>
#include <stdio.h>
#include <stdint.h>  // Necesario para uint16_t
#include <stdbool.h> // Necesario para boolean
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

    static const unsigned int analog_input = 0;
    
    // Setup and selection of the Analog input wanted
    setup_analog_input(analog_input);
    select_analog_input(analog_input);
    
    FLL_CTL0 |= XCAP14PF;                     // Configure load caps
    for (i = 0; i < 10000; i++);              // Delay for 32 kHz crystal to
    
    static const char v_ref = 'I';            // I: Internal (1.2V), O: Off-chip, E: External
    voltage_reference(v_ref);
    
    
    static const char clk_ref = 'S';
    static const unsigned int clk_div_1 = 1;
    static const unsigned int clk_div_2 = 1;
    clk_reference(clk_ref); // M: MCLK, S: SMCLK, A: ACLK, T:TACLK
    fM_dividers(clk_div_1, clk_div_2);
    

    bool const interruption = false;
    enable_interruption(interruption);

    //SUT
    static const unsigned int OSR = 1;
    config_OSR(OSR);
    //SUT end

    static const int gain = 1;
    gain_setup(gain);
    
    static const char conv_mode = 'S'; // C: Continuous  S: Single
    conversion_mode(conv_mode); 

    static const char polarity = 'B';       // B : Bipolar, U : unipolar
    static const char sign = 'O';           // O : Offset, C : 2's complement

    data_format(polarity, sign);

    

    for (i = 0; i < 0x3600; i++);             // Delay for 1.2V ref startup

    while(1){
        start_conversion();
        

        toggle_pin();

        switch (interruption) {
            case false:// if IFG polling is being used:
                result = IFG_polling();
                break;
            case true:// if interruption is being used:
                __bis_SR_register(LPM0_bits+GIE); // Enter LPM
                break;
            default:
                perror("Error: interruption enabling used wrong");
                break;
        }


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

  __bic_SR_register_on_exit(LPM0_bits);                   // Exit LPM0
}


