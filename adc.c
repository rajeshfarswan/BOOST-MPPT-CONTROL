// ADC reading subroutine           //
// input argument is channel number //
// return value is ADC reading      //

#include "p30f6010A.h"
#include "main.h"

int adc(char ADC_NO)
{
asm("disi #0x3FFF");
    {	
CH0_MULA = (int)ADC_NO;
CH0_MULB = ADC_NO;
Start_Converstion = 1;
while(!Converstion_Done);
Converstion_Done = 0;
    } 
asm("disi #0x0000");
return (ADCBUF0);
     
} //adc end
