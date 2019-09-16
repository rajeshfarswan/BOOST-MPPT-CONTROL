//Processor variable definitions //

#include "p30f6010A.h"

#define RL1_ON PORTGbits.RG0
#define RL2_ON PORTGbits.RG1
#define RL3_ON PORTGbits.RG2
#define RL4_ON PORTGbits.RG3
#define RL5_ON PORTGbits.RG6

#define PWM1 PDC1
#define PWM2 PDC2
#define PWM3 PDC3

#define T1us_Flag IFS0bits.T1IF //every 1.5us flag


#define PWM_PERIOD PTPER 
#define Converstion_Done ADCON1bits.DONE
#define Start_Converstion ADCON1bits.SAMP

#define CH0_MULA ADCHSbits.CH0SA
#define CH0_MULB ADCHSbits.CH0SB

#define Ipeak_flag IFS2bits.PWMIF

#define Vr_ref 1000 //1count == .202volts //max output voltage ref 1023
#define Ir_ref 160 //40count == 1A //max current ref 160

#define softCount 3000 //4.5mS

#define IrTIMER_ON T2CONbits.TON //timer2 for peak current detect
#define IrCOUNT PR2 
#define T2_Flag IFS0bits.T2IF //for Ir peak detect

#define PWM_Flag IFS2bits.PWMIF

#define fault_Flag IFS2bits.FLTBIF

#define PWMenable PTCONbits.PTEN
#define PWM_InterruptEnable IEC2bits.PWMIE

#define T1ON T1CONbits.TON

#define FAULT_ENABLE FLTBCON

#define ADC_ON ADCON1bits.ADON

#define SET PWMCON1 

//********************************************************
//Iout = (2.5+-(0.625*Ip/6)V)*(1024/5) 

//VDCout = (Vdc*15/600)V)*(1024/5)
//VACout = (2.5+-(Vac*6.8/600)V)*(1024/5)
//********************************************************

//Iref max = 384//6A





