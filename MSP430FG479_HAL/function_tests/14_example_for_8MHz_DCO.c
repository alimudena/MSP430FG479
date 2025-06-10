/* --COPYRIGHT--,BSD_EX
 * Copyright (c) 2012, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *******************************************************************************
 * 
 *                       MSP430 CODE EXAMPLE DISCLAIMER
 *
 * MSP430 code examples are self-contained low-level programs that typically
 * demonstrate a single peripheral function or device feature in a highly
 * concise manner. For this the code may rely on the device's power-on default
 * register values and settings such as the clock configuration and care must
 * be taken when combining code from several examples to avoid potential side
 * effects. Also see www.ti.com/grace for a GUI- and www.ti.com/msp430ware
 * for an API functional library-approach to peripheral configuration.
 *
 * --/COPYRIGHT--*/
//******************************************************************************
//  MSP430x47x Demo - FLL+, Runs Internal DCO at 8MHz
//
//  Description: This program demostrates setting the internal DCO to run at
//  8MHz with auto-calibration by the FLL+.
//  ACLK = LFXT1 = 32768Hz, MCLK = SMCLK = DCO = (121+1) x 2 x ACLK = 7995392Hz
//  //* An external watch crystal between XIN & XOUT is required for ACLK *//
//
//                 MSP430x47x
//             -----------------
//         /|\|              XIN|-
//          | |                 | 32kHz
//          --|RST          XOUT|-
//            |                 |
//            |             P1.1|--> MCLK = 8Mhz --> 57
//            |                 |
//            |             P1.5|--> ACLK = 32kHz --> 51
//            |                 |
//
//  M.Seamen/ P. Thanigai
//  Texas Instruments Inc.
//  September 2008
//  Built with IAR Embedded Workbench V4.11A and CCE V3.2
//******************************************************************************
#include <msp430.h>
#include "../functions/FLL.h"
#include "../functions/system_config.h"
#include "functions/general_functions.h"
int main(void)
{
    const char operating_mode = 'A';
    select_operating_mode(operating_mode, 0);
    stop_wd();
//    WDTCTL = WDTPW + WDTHOLD;                 // Stop watchdog timer


    //SCFI0 |= FN_4;                            // x2 DCO freq, 8MHz nominal DCO
    /*2 --> fDCOCLK =   1.4-12MHz*/
    /*3 --> fDCOCLK =   2.2-17Mhz*/
    /*4 --> fDCOCLK =   3.2-25Mhz*/
    /*8 --> fDCOCLK =     5-40Mhz*/
    const int DCO_range = 4;
    DCO_f_range(DCO_range);



    //    FLL_CTL0 |= DCOPLUS + XCAP18PF;           // DCO+ set so freq= xtal x D x N+1

    //SCFQCTL = 121;                            // (121+1) x 32768 x 2 = 7.99 Mhz    
    /*0 --> Disable*/
    /*2 --> fMCLK=2*fACLK          1+1 to 127+1 is possible */
    /*4 --> fMCLK=4*fACLK */
    /*8 --> fMCLK=8*fACLK */
    /*16 --> fMCLK=16*fACLK */
    /*32 --> fMCLK=32*fACLK */
    /*64 --> fMCLK=64*fACLK */
    /*128 --> fMCLK=128*fACLK */
    /*X --> fMCLK=X*fACLK */
    const int N_MCLK = 127;
    configure_N_for_MCLK(N_MCLK);

    /*
    CLK references MCLK
        - D: DCO
        - X: XT2
        - A: LFXT1
    */
    const char ref_MCLK = 'A';
    select_reference_MCLK(ref_MCLK);
    
    const bool DCOPLUS_on = true;
    const int D_val = 2;
    configuring_DCO(DCOPLUS_on, D_val);

    const char LFXT1_wk_mode = 'H';
    LFXT1_working_mode(LFXT1_wk_mode);

    const bool ext_osc = true;
    const int ext_osc_f = 32;
    protection_cpu_required(ext_osc, ext_osc_f);

    const int LFXT1_int_cap = 18;
    LFXT1_internal_cap_config(LFXT1_int_cap);

    /*
    CLK references SMCLK
        - D: DCO
        - X: XT2
        - N: OFF
    */
    const char ref_SMCLK = 'D';
    select_reference_SMCLK(ref_SMCLK);


    //  P1DIR = 0x22;                             // P1.1,5 to output direction
    //  P1SEL = 0x22;                             // P1.1,5 to output MCLK & ACLK
    //  P1SEL2 |= 0x02;
    configure_PINS_for_clk_debug();

    const int divider_ACLK = 2;
    configure_ACLK_N(divider_ACLK);

    const bool LFXT2_osc_on = false;
    LFXT2_disable(LFXT2_osc_on);
    while(1);                                 // Loop in place
}
