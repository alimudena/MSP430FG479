#include <msp430.h>

#define Num_of_Results 2

/* Arrays to store SD16_A conversion results */
unsigned int results_A0[Num_of_Results];
unsigned int m;
unsigned int results_A1[Num_of_Results];

volatile unsigned int current_channel = 0;  // 0 for A0, 1 for A1

int main(void)
{
  volatile unsigned int i;
  WDTCTL = WDTPW + WDTHOLD;                  // Stop WDT
  FLL_CTL0 |= XCAP14PF;                      // Configure load caps

  // Configuración de las entradas del SD16
  P6SEL |= BIT0+BIT1;                        // Config A0+/A0-
  
  for (i = 0; i < 10000; i++);               // Delay for 32 kHz crystal to stabilize

  // Reference clock for the SD16 and 
  SD16CTL |= SD16REFON + SD16SSEL0;          // 1.2V ref, SMCLK
  SD16INCTL0 |= SD16INTDLY_0;   // Interrupt on 4th sample  
  SD16CCTL0 |= SD16IE;                       // Multiple conv, enable interrupt
  
  for (i = 0; i < 0x3600; i++);              // Delay for 1.2V ref startup
 
  SD16AE = SD16AE0 + SD16AE1;                // Use A0+/- inputs and A1+/- inputs

  SD16CCTL0 &= ~SD16SC;                      // Stop conversion if any is in progress
  SD16INCTL0 &= ~(SD16INCH0 | SD16INCH1 | SD16INCH2);  // Clear channel selection bits
  SD16INCTL0 |= SD16INCH_0;                  // Select A0 (CH0)  
  SD16CCTL0 |= SD16SC;                       // Start conversion on A0
  __bis_SR_register(LPM0_bits+GIE);          // Enter LPM0 with interrupts enabled
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
  static unsigned int index_A0 = 0;
  static unsigned int index_A1 = 0;

  switch (SD16IV)
  {
    case 2:                                 // SD16MEM Overflow
      m = 2;
      break;
    case 4:                                 // SD16MEM0 IFG
      // Simulación de un desbordamiento (overflow):
      // En lugar de leer el registro SD16MEM0, inicia otra conversión inmediatamente,
      // lo que forzará el desbordamiento ya que el valor anterior no ha sido leído.
      SD16CCTL0 |= SD16SC;                  // Iniciar la siguiente conversión sin leer el registro
      break;
  }
}
