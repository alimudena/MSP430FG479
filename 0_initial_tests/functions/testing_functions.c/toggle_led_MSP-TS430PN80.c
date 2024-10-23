#include <stdint.h>
#include <msp430.h>
#include "../general_functions.h"


int main(void)
{
  WDTCTL = WDTPW + WDTHOLD;                 // Stop watchdog timer
  setup_led(); // Setup led
  for (;;)
  {
    volatile unsigned int i;

    toggle_led();                          // Toggle P4.6 using exclusive-OR

    i = 50000;                              // Delay
    do (i--);
    while (i != 0);
  }
}
