#include <msp430.h>
#include <stdio.h>
#include <stdint.h>  // Necesario para uint16_t
#include "../functions/general_functions.h"
#include "../functions/SD16_A.h"

#define   Num_of_Results   20

unsigned int results[Num_of_Results];
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
    
    FLL_CTL0 |= XCAP14PF;                     // Configure load caps TODO
    for (i = 0; i < 10000; i++);              // Delay for 32 kHz crystal to
    
    static const char v_ref = 'I';            // I: Internal (1.2V), O: Off-chip, E: External
    voltage_reference(v_ref);                   

    SD16CTL |= SD16SSEL0;                     // SMCLK
    SD16CCTL0 |= SD16IE;            //Enable interrupt
    SD16INCTL0 |= SD16INTDLY_0;               // Interrupt on 4th sample  

    volatile unsigned int gain = 1;
    gain_setup(gain);
 
    ////SUT
    conversion_mode('C');
   ////SUT end
    start_conversion();
    
    //enter_LPM();
    toggle_pin();
    __bis_SR_register(LPM0_bits+GIE);

}


#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=SD16A_VECTOR
__interrupt void SD16ISR(void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(SD16A_VECTOR))) SD16ISR (void)
#else
#error Compiler not supported!
#endif
{
  static unsigned int index = 0;

  switch (SD16IV)
  {
  case 2:                                   // SD16MEM Overflow
  case 4:                                   // SD16MEM0 IFG
    results[index] = SD16MEM0;              // Save CH0 results (clears IFG)
    if (++index == Num_of_Results)
    {
      index = 0;                            // SET BREAKPOINT HERE
    }
    break;
  }
}
