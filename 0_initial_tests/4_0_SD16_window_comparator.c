#include <msp430.h>

#define Num_of_Results 2
#define THRESHOLD 40000    // Definir el valor umbral para el ADC

/* Arrays to store SD16_A conversion results */
unsigned int results[Num_of_Results];
unsigned int m;

int main(void)
{
  volatile unsigned int i;                  // Use volatile to prevent removal
                                            // by compiler optimization

  WDTCTL = WDTPW + WDTHOLD;                 // Stop WDT
  
  P6SEL |= BIT0+BIT1;                       // Config SD16 inputs (A0+/A0-)
  
  FLL_CTL0 |= XCAP14PF;                     // Configure load caps
  
   P4DIR |= BIT6;                            // Set P4.6 to output direction
   P4OUT ^= BIT6;                          // Toggle P4.6 using exclusive-OR

  // Configurar el ADC para leer del canal 0 (A0)
  SD16INCTL0 |= SD16INCH_0;                 // Selección del canal A0 
  
  for (i = 0; i < 10000; i++);              // Delay para que el cristal de 32 kHz se estabilice

  SD16CTL = SD16REFON + SD16SSEL0;          // 1.2V ref, SMCLK
  SD16INCTL0 |= SD16INTDLY_0;               // Interrupción en la 4ta muestra  
  SD16CCTL0 |= SD16IE;                      // Habilitar interrupciones del ADC
  
  for (i = 0; i < 0x3600; i++);             // Delay para que la referencia de 1.2V arranque

  SD16CCTL0 |= SD16SC;                      // Iniciar conversión del ADC
  __bis_SR_register(LPM0_bits + GIE);       // Entrar en LPM0 (modo bajo consumo) con interrupciones habilitadas

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
  case 2:                                   // SD16MEM Overflow (desbordamiento de memoria del ADC)
    m = 2;
    break;
  case 4:                                   // SD16MEM0 IFG (interrupción por datos listos del ADC)
    results[index] = SD16MEM0;              // Guardar los resultados del canal 0 (A0) en el array
    
    if (results[index] > THRESHOLD)         // Comparar el resultado con el valor umbral
    {
      P4OUT |= BIT6;                        // Encender el LED si supera el umbral
    }
    else
    {
      P4OUT &= ~BIT6;                       // Apagar el LED si no supera el umbral
    }
    
    if (++index == Num_of_Results)
    {
      index = 0;                            // Resetear el índice si llega al límite de resultados
    }
    break;
  }
}
