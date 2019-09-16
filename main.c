//**************************************************************************//
//         AC LAB KIT BOOST CONFIGURATION PROGRAM WITH PO MPPT              //
//**************************************************************************//

//include files
#include "p30f6010A.h"
#include "main.h"
#include "asmMATH.h"

//
int Vcount = 50; //voltage and MPPT sampling counter
int Vtick = 0;

int MPPcount = 32700; //.327sec //32700
int MPPtick = 0;

int VDC1 = 0;
int VDC2 = 0;
int VDC = 0;
int VDC_max = 920; //180V //max output voltage limit

//current control counters
int PID_Isample = 0; //current PID
int PID_I_count =  4; //25Khz //4

int Pnew = 0;
int Pold = 0;
int Ipv = 0;
int Vpv = 0;
int Vpv_old = 0;
int PVref = 0;
int Irtrip = 130; //6A // trip current setting
int offset = 0;

int IPreError = 0; //integral register

// PI gain parameters
int Vpgain = 36;
int Vigain = 9;

int Vpre_err = 0;

int Vpi_out = 0;

int PVref_max = 200; //40V //max PV mppt ref

int PWM_max =  14000; //14208
int PWM_min = -14000;

int SAT = 1776;

//


int main(void)
{

init(); //call processor initialisation routine

PWMenable = 0; //reset PWM control register
PWM_InterruptEnable = 0;

ADC_ON = 1; //enable adc
FAULT_ENABLE = 1; //0x000f; //reset fault register

PWM1 = 3552; //reset PWM registers
PWM2 = 3552;
PWM3 = 3552;

offset = adc(14); //read voltage offset//adc channel 14

Vpv  =  adc(5) - offset; //read PV voltage
if(Vpv <= 0) Vpv = 0; 

Ipv  =  offset - adc(8); //read PV current
if(Ipv <= 0) Ipv = 0;

asm("disi #0x3FFF"); //Vpv*Ipv
    {    
/*calculate Pnew*/
    asm("MOV _Vpv, W4"); // _Vpv
    asm("MOV _Ipv, W5"); // _Ipv
    asm("MPY W4*W5, A");
    asm("SFTAC A,#1");   
    asm("MOV ACCA, W0");
    asm("MOV W0, _Pnew"); //PV power
     } 
asm("disi #0x0000");

Pold = Pnew; //initialise values of MPPT
Vpv_old = Vpv;
PVref = 80; //80 //PV initial ref value

T1ON = 1;                //enable timers
PWMenable = 1;           //enable PWM
PWM_InterruptEnable = 1; //enable interrupt

    while(1)
    {

//DC link voltage protection
if(Vtick >= Vcount)
	    {

	VDC = (VDC1+VDC2)>>1;

	if(VDC >= VDC_max) fault_Flag = 1;

	Vtick = 0;
		     }

if(PID_Isample >= PID_I_count) //current PI control routine
             {

    IPreError = Vpre_err; //copy previous integrated error
    Vpi_out = asmPIcontroller(PVref,Vpv,Vpgain,Vigain); //set PI ref from MPPT
    Vpre_err = IPreError; //copy new integrated error

    if(Vpi_out >=  PWM_max) Vpi_out =  PWM_max; 
    if(Vpi_out <=  PWM_min) Vpi_out =  PWM_min;

    Vpi_out = Vpi_out>>3; //scale PI output
    Vpi_out = Vpi_out + SAT;

    asm("disi #0x3FFF");
    PWM1 =Vpi_out;      //program PWM duty cycle 1
    asm("disi #0x0000");   
 
    PID_Isample = 0;
                     } //current sample end

if(MPPtick >= MPPcount) //MPPT routine
                     {
/*calculate Pnew*/
asm("disi #0x3FFF");
           {
    asm("MOV _Vpv, W4"); // _Vpv
    asm("MOV _Ipv, W5"); // _Ipv
    asm("MPY W4*W5, A");
    asm("SFTAC A,#1");   
    asm("MOV ACCA, W0");
    asm("MOV W0, _Pnew"); //Pnew = Vpv*Ipv
           } 
asm("disi #0x0000");

//normal PO logic
     if(Pnew > Pold) //if power is increasing
           {
               if(Vpv > Vpv_old) PVref++; //if voltage is increasing increase ref
               if(Vpv < Vpv_old) PVref--; //if voltage is decreasing decrease ref           
                           
                     }
     if(Pnew < Pold) //if power is decreasing
            {
               if(Vpv > Vpv_old) PVref--; //if voltage is increasing decrease ref
               if(Vpv < Vpv_old) PVref++; //if voltage is decreasing increase ref           
                           
                      }

   if(PVref <= 0) PVref = 0;
   if(PVref >= PVref_max) PVref = PVref_max; //set max pv ref

   Pold = Pnew;   //copy old power
   Vpv_old = Vpv; //copy old voltage

             MPPtick = 0;
                              }//MPPT

                                  } //while end

                                      } //main end

//

////////////////////////////////////////////////////////////////////////////////

//T1 interrupt for program tracking
void _ISRFAST __attribute__((interrupt, no_auto_psv)) _T1Interrupt(void)
  {  
     asm("disi #0x3FFF"); //update routine counters  
     Vtick++; 
     PID_Isample++;
     MPPtick++;
     asm("disi #0x0000");
   
     T1us_Flag = 0;
   } //T1 interupt end

///////////////////////////////////////////////////////////////////////

//PWM interrupt for MPPT
void _ISRFAST __attribute__((interrupt, no_auto_psv)) _PWMInterrupt(void)
  {
asm("disi #0x3FFF");  
 /*read DC link voltage*/  
	asm("MOV #0x0303, W0");       //select adc channel VDC1
	asm("MOV W0, ADCHS");    
	asm("BSET ADCON1,#1");        //start converstion
	asm("LOOP1:");
	asm("BTSS ADCON1,#0");        //wait for converstion
	asm("BRA LOOP1");
	asm("BTG ADCON1,#0"); 
	asm("MOV ADCBUF0, W0");   
	asm("MOV W0, _VDC1");         //get vdc1

    asm("MOV #0x0202, W0");       //select adc channel VDC2
	asm("MOV W0, ADCHS");    
	asm("BSET ADCON1,#1");        //start converstion
	asm("LOOP2:");
	asm("BTSS ADCON1,#0");        //wait for converstion
	asm("BRA LOOP2");
	asm("BTG ADCON1,#0"); 
	asm("MOV ADCBUF0, W0");   
	asm("MOV W0, _VDC2");         //get vdc2

/*read Vpv and Ipv*/
    asm("MOV _offset, W5");       //read 2.5V offset

	asm("MOV #0x0505, W0");       //select adc channel Vr
	asm("MOV W0, ADCHS");    
	asm("BSET ADCON1,#1");        //start converstion
	asm("LOOPa:");
	asm("BTSS ADCON1,#0");        //wait for converstion
	asm("BRA LOOPa");
	asm("BTG ADCON1,#0"); 
	asm("MOV ADCBUF0, W0");
    asm("SUB W0,W5,W0");
    asm("CLR W6");                //check less than zero
    asm("CPSGT W0, W6");
    asm("CLR W0");
	asm("MOV W0, _Vpv");          //get Pv voltage
	

	asm("MOV #0x0808, W0"); //0x0b0b       //select adc channel Ir
	asm("MOV W0, ADCHS");    
	asm("BSET ADCON1,#1");        //start converstion
	asm("LOOPb:");
	asm("BTSS ADCON1,#0");        //wait for converstion
	asm("BRA LOOPb");
	asm("BTG ADCON1,#0"); 
	asm("MOV ADCBUF0, W0");
    asm("SUB W5,W0,W0");
    asm("CLR W6");                //check greater than zero
    asm("CPSGT W0, W6");
    asm("CLR W0");
    asm("MOV _Irtrip, W6");       //read current max trip limit
    asm("CPSLT W0, W6");
    asm("BSET _IFS2+1,#4");       //set fault 
	asm("MOV W0, _Ipv");          //get Pv current
     
asm("disi #0x0000");
PWM_Flag = 0;

   } //PWM interupt end

///////////////////////////////////////////////////////////////////////

//fault interrupt
void _ISRFAST __attribute__((interrupt, no_auto_psv)) _FLTBInterrupt(void)
   {
     
     PWMenable = 0; //disable pwm if fault
     SET = 0;       //all switches off
     
ClrWdt();

fault_Flag = 0;
   }//fault end

///////////////////////////////////////////////////////////////////////
