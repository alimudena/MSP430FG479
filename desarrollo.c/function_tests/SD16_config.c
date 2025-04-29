//***************************************************************************** 
/*SETUP SD16A*/


//Para muestrear a una frecuencia de 15kHz 
//fs = fm/OSR 

//tomando como referencia MCLK con frecuencia 8MHz -- fm = 8MHz 
//Ponemos OSR = 512 -- fs = 15.625 kHz

//*****************************************************************************
//****************** VARIABLES
        // -- Entrada analógica
    static const unsigned int analog_input = 0; //0: A0, 1: A1, 2: A2, 3: A3, 4: A4
        // -- Tensión de referencia
    static const char v_ref = 'I';            // I: Internal (1.2V), O: Off-chip, E: External
        // -- Reloj de referencia
    static const char clk_ref = 'M';          // M: MCLK, S: SMCLK, A: ACLK, T: TACLK
        // -- Divisor de frecuencia de referencia
    static const unsigned int clk_div_1 = 1;
    static const unsigned int clk_div_2 = 1;
        // -- Método de lectura: Polling o Interrupciones
    bool const interruption_SD16A = false;
        // -- Over Sampling Ratio
    static const unsigned int OSR = 512; //1, 32, 64, 128, 256, 512, 1024
        // -- Ganancia
    static const int gain = 1; //1, 2, 4, 8, 16 or 32
        // -- Método de conversión
    static const char conv_mode = 'C'; // C: Continuous  S: Single
        // -- Tipo de datos
    static const char polarity = 'B';       // B : Bipolar, U : unipolar
    static const char sign = 'O';           // O : Offset, C : 2's complement


  
 //****************** FUNCTIONS
    setup_analog_input(analog_input);
    select_analog_input(analog_input);
    
    FLL_CTL0 |= XCAP14PF;                     // Configure load caps

    voltage_reference(v_ref);
    clk_reference(clk_ref); // M: MCLK, S: SMCLK, A: ACLK, T:TACLK
    fM_dividers(clk_div_1, clk_div_2);
    enable_interruption_SD16A(interruption_SD16A);
    config_OSR(OSR);
    gain_setup(gain);    
    conversion_mode(conv_mode); 
    data_format(polarity, sign);
