// Processor initialiation routine //

#include "p30f6010A.h"

//configure oscillator
_FOSC(CSW_FSCM_OFF & HS3_PLL16); //26.667Mhz Fcy Tcy .0374995us
_FWDT(WDT_OFF); //watchdog off
_FBORPOR(PWMxH_ACT_HI & PWMxL_ACT_HI);

void init(void)
{
//configure digital outputs
TRISGbits.TRISG0 = 0; 
TRISGbits.TRISG1 = 0; 
TRISGbits.TRISG2 = 0; 
TRISGbits.TRISG3 = 0; 
TRISGbits.TRISG6 = 0; 

//configure PWM
PTCONbits.PTEN = 0; //pwm enable
PTCONbits.PTMOD = 3;

PTPERbits.PTPER = 1776; //7.5Khz Fpwm

PWMCON1bits.PEN1L = 1;
PWMCON1bits.PEN2L = 1;
PWMCON1bits.PEN3L = 1;

PWMCON1bits.PEN1H = 1;
PWMCON1bits.PEN2H = 1;
PWMCON1bits.PEN3H = 1;

PWMCON2bits.IUE = 0;

//configure dead time
DTCON1bits.DTAPS = 3;
DTCON1bits.DTA = 10; //DT 2.99us

//fault input configuration
FLTBCONbits.FBEN1 = 0; //enable now
FLTBCONbits.FBEN2 = 0;
FLTBCONbits.FBEN3 = 0;

//timer1 configuration for program tracking
PR1 = 267; //Period of 10.0124us 
T1CONbits.TON = 0; //timer off

//ADC configuration

ADCON1bits.SSRC = 7;
ADCON1bits.SIMSAM = 0;

ADCON2= 0; //CH0 only

ADCON3bits.SAMC = 2; //sample time 2*Tad
ADCON3bits.ADCS = 5; //4//Tconv = 112ns

ADPCFG = 0x8013;
ADCSSL = 0;

ADCON1bits.ADON = 0;

//configure interrupts
INTCON1bits.NSTDIS = 1; //disable interrupt nesting if set
IPC11bits.FLTBIP = 0x7;
IEC2bits.FLTBIE = 1; //enable fault interrupt
IEC0bits.T1IE = 1; //enable T1 interrupt
IPC0bits.T1IP = 0x5;
IEC2bits.PWMIE = 0; //enable pwm interrupt
IPC9bits.PWMIP = 0x6;

}
