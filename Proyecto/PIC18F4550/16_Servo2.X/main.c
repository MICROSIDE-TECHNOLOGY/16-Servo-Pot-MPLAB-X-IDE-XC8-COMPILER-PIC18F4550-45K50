/*
 * File:   main.c
 * Author: Microside Technology
 *
 * Created on 27 de noviembre de 2020, 02:01 AM
 */
// CONFIG1L
#pragma config PLLDIV = 1         // PLL Prescaler Selection bits (Divide by 5 (20 MHz oscillator input))
#pragma config CPUDIV = OSC1_PLL2 // System Clock Postscaler Selection bits ([Primary Oscillator Src: /1][96 MHz PLL Src: /2])
#pragma config USBDIV = 1         // USB Clock Selection bit (used in Full-Speed USB mode only; UCFG:FSEN = 1) (USB clock source comes directly from the primary oscillator block with no postscale)
// CONFIG1H
#pragma config FOSC = HSPLL_HS // Oscillator Selection bits (HS oscillator, PLL enabled (HSPLL))
#pragma config FCMEN = OFF     // Fail-Safe Clock Monitor Enable bit (Fail-Safe Clock Monitor disabled)
#pragma config IESO = OFF      // Internal/External Oscillator Switchover bit (Oscillator Switchover mode disabled)
// CONFIG2L
#pragma config PWRT = ON    // Power-up Timer Enable bit (PWRT enabled)
#pragma config BOR = OFF    // Brown-out Reset Enable bits (Brown-out Reset disabled in hardware and software)
#pragma config BORV = 3     // Brown-out Reset Voltage bits (Minimum setting 2.05V)
#pragma config VREGEN = OFF // USB Voltage Regulator Enable bit (USB voltage regulator disabled)
// CONFIG2H
#pragma config WDT = OFF     // Watchdog Timer Enable bit (WDT disabled (control is placed on the SWDTEN bit))
#pragma config WDTPS = 32768 // Watchdog Timer Postscale Select bits (1:32768)
// CONFIG3H
#pragma config CCP2MX = ON   // CCP2 MUX bit (CCP2 input/output is multiplexed with RC1)
#pragma config PBADEN = OFF  // PORTB A/D Enable bit (PORTB<4:0> pins are configured as digital I/O on Reset)
#pragma config LPT1OSC = OFF // Low-Power Timer 1 Oscillator Enable bit (Timer1 configured for higher power operation)
#pragma config MCLRE = ON    // MCLR Pin Enable bit (MCLR pin enabled; RE3 input pin disabled)
// CONFIG4L
#pragma config STVREN = ON // Stack Full/Underflow Reset Enable bit (Stack full/underflow will cause Reset)
#pragma config LVP = OFF   // Single-Supply ICSP Enable bit (Single-Supply ICSP enabled)
#pragma config ICPRT = OFF // Dedicated In-Circuit Debug/Programming Port (ICPORT) Enable bit (ICPORT disabled)
#pragma config XINST = OFF // Extended Instruction Set Enable bit (Instruction set extension and Indexed Addressing mode disabled (Legacy mode))
// CONFIG5L
#pragma config CP0 = OFF // Code Protection bit (Block 0 (000800-001FFFh) is not code-protected)
#pragma config CP1 = OFF // Code Protection bit (Block 1 (002000-003FFFh) is not code-protected)
#pragma config CP2 = OFF // Code Protection bit (Block 2 (004000-005FFFh) is not code-protected)
#pragma config CP3 = OFF // Code Protection bit (Block 3 (006000-007FFFh) is not code-protected)
// CONFIG5H
#pragma config CPB = OFF // Boot Block Code Protection bit (Boot block (000000-0007FFh) is not code-protected)
#pragma config CPD = OFF // Data EEPROM Code Protection bit (Data EEPROM is not code-protected)
// CONFIG6L
#pragma config WRT0 = OFF // Write Protection bit (Block 0 (000800-001FFFh) is not write-protected)
#pragma config WRT1 = OFF // Write Protection bit (Block 1 (002000-003FFFh) is not write-protected)
#pragma config WRT2 = OFF // Write Protection bit (Block 2 (004000-005FFFh) is not write-protected)
#pragma config WRT3 = OFF // Write Protection bit (Block 3 (006000-007FFFh) is not write-protected)
// CONFIG6H
#pragma config WRTC = OFF // Configuration Register Write Protection bit (Configuration registers (300000-3000FFh) are not write-protected)
#pragma config WRTB = OFF // Boot Block Write Protection bit (Boot block (000000-0007FFh) is not write-protected)
#pragma config WRTD = OFF // Data EEPROM Write Protection bit (Data EEPROM is not write-protected)
// CONFIG7L
#pragma config EBTR0 = OFF // Table Read Protection bit (Block 0 (000800-001FFFh) is not protected from table reads executed in other blocks)
#pragma config EBTR1 = OFF // Table Read Protection bit (Block 1 (002000-003FFFh) is not protected from table reads executed in other blocks)
#pragma config EBTR2 = OFF // Table Read Protection bit (Block 2 (004000-005FFFh) is not protected from table reads executed in other blocks)
#pragma config EBTR3 = OFF // Table Read Protection bit (Block 3 (006000-007FFFh) is not protected from table reads executed in other blocks)
// CONFIG7H
#pragma config EBTRB = OFF // Boot Block Table Read Protection bit (Boot block (000000-0007FFh) is not protected from table reads executed in other blocks)
// #pragma config statements should precede project file includes.
// Use project enums instead of #define for ON and OFF.

#define _XTAL_FREQ 48000000
#include <pic18f4550.h>
#include <stdbool.h>
#include <stdint.h>
#include <xc.h>

uint16_t ReadADC( void );
void ADCInit( void );
void Timer0_Init( void );
void InitPort( void );
void delay_us( uint16_t uS );
int SUBE = 1;
uint16_t DUTY;

void main( void )
{
    InitPort();
    Timer0_Init();
    ADCInit();
    while ( 1 ) {
        while ( LATCbits.LATC2 == 0 )
            ;
        DUTY = ReadADC();
        __delay_us( 400 );
        for ( int i = 0; i < DUTY; i++ ) {
            __delay_us( 1 );
        }
        LATCbits.LATC2 = 0;
    }
    return;
}

void InitPort( void )
{
    ADCON1bits.PCFG = 0x0B;
    LATC = 0;
    TRISCbits.RC2 = 0;
    LATA = 0;
}

void Timer0_Init( void )
{
    INTCONbits.GIE = 0;
    T0CON = 0x91;

    INTCONbits.TMR0IF = 0;
    INTCONbits.TMR0IE = 1;
    TMR0IP = 1;
    TMR0 = 5535;
    TMR0ON = 1;

    INTCONbits.PEIE = 1;
    INTCONbits.GIE = 1;
}

void __interrupt( high_priority ) ISR( void )
{
    if ( TMR0IF ) {
        LATCbits.LATC2 = 1;
        TMR0 = 5535;
        INTCONbits.TMR0IF = 0;
    }
}

void ADCInit( void )
{
    ADCON0bits.CHS = 0b0011;
    ADCON0bits.ADON = 1;
    ADCON2 = 0xBE;
}

uint16_t ReadADC( void )
{
    uint16_t result;
    ADCON0bits.GO_DONE = 1; //  Inicia la COnversió AD.
    while ( ADCON0bits.GO_DONE )
        ;                   //  Espera a que termine la conversión AD.
    result = ( ( ADRESH << 8 ) + ADRESL );
    return ( result );
}
