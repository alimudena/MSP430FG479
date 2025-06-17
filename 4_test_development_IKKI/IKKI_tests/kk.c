int putchar(int c) {
    while (!(IFG2 & UCA0TXIFG));  // Espera a que el buffer esté libre
    UCA0TXBUF = c;                // Envía el carácter por UART
    return c;
}
void uart_init(void) {
    P3SEL |= BIT4 + BIT5;      // P3.4 = TXD, P3.5 = RXD
    UCA0CTL1 |= UCSWRST;       // Pone USCI en reset
    UCA0CTL1 |= UCSSEL_2;      // SMCLK
    UCA0BR0 = 104;             // 1MHz 9600
    UCA0BR1 = 0;
    UCA0MCTL = UCBRS_1;        // Modulación
    UCA0CTL1 &= ~UCSWRST;      // Sale de reset
}
int main(void) {
    WDTCTL = WDTPW | WDTHOLD;   // Detiene el watchdog
    uart_init();                // Inicializa UART

    int value = 42;
    printf("Value: %d\n", value);

    while (1);
}

