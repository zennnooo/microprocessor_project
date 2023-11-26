#include <stdio.h>
#include "S32K144.h"
#include "device_registers.h"
#include "clocks_and_modes.h"
#include "ADC.h"
#include "math.h"
#include "lcd1602A.h"

unsigned int Dtime = 0; /* Delay Time Setting Variable*/
int lpit0_ch0_flag_counter = 0; /*< LPIT0 timeout counter */

//segment
unsigned int FND_DATA[10]={0x7E, 0x0C, 0xB6, 0x9E, 0xCC, 0xDA, 0xFA, 0x4E, 0xFE, 0xCE}; // 0 ~ 9
unsigned int FND_SEL[4]={0x0100, 0x0200, 0x0400, 0x0800}; //d1000, d100, d10, d1
unsigned int j=0; /*FND select pin index */
unsigned int num, d1, d10, d100, d1000 =0;
/*num is Counting value, num0 is '1', num2 is '10', num2 is '100', num3 is '1000'*/


void PORT_init(void) 
{
    //사용할 port에 따라서 각각 선언해주기

    /*=====================PORT-E 7-SEGMENT=====================*/
    PCC-> PCCn[PCC_PORTE_INDEX] = PCC_PCCn_CGC_MASK; /* Enable clock for PORT D */

    PTE->PDDR |= 1<<1| 1<<2| 1<<3| 1<<4| 1<<5| 1<<6| 1<<7;
	PORTE->PCR[1] = PORT_PCR_MUX(1); /* Port D1: MUX = GPIO */
	PORTE->PCR[2] = PORT_PCR_MUX(1); /* Port D2: MUX = GPIO */
	PORTE->PCR[3] = PORT_PCR_MUX(1); /* Port D3: MUX = GPIO */
	PORTE->PCR[4] = PORT_PCR_MUX(1); /* Port D4: MUX = GPIO */
	PORTE->PCR[5] = PORT_PCR_MUX(1); /* Port D5: MUX = GPIO */
	PORTE->PCR[6] = PORT_PCR_MUX(1); /* Port D6: MUX = GPIO */
    PORTE->PCR[7] = PORT_PCR_MUX(1); /* Port D7: MUX = GPIO */

    PTE->PDDR |= 1<<8|1<<9|1<<10|1<<11;
	PORTE->PCR[8] = PORT_PCR_MUX(1); /* Port D8: MUX = GPIO */
	PORTE->PCR[9] = PORT_PCR_MUX(1); /* Port D9: MUX = GPIO */
	PORTE->PCR[10] = PORT_PCR_MUX(1); /* Port D10: MUX = GPIO */
	PORTE->PCR[11] = PORT_PCR_MUX(1); /* Port D11: MUX = GPIO */

    /*=====================PORT-D LCD=====================*/
	/*
        port D 15, 14, 13 - RS, RW, ENABLE
	    port D 12, 11, 10, 9 - Transmission port(DB7, 6, 5, 4)
    */
    //PTD->PDDR |= 0xFE00;
    PTD->PDDR |= 1<<9 | 1<<10 | 1<<11 | 1<<12 | 1<<13 | 1<<14 | 1<<15;

	PCC->PCCn[PCC_PORTD_INDEX] &= ~PCC_PCCn_CGC_MASK;
	PCC->PCCn[PCC_PORTD_INDEX] |= PCC_PCCn_PCS(0x001);
    PCC->PCCn[PCC_PORTD_INDEX] |= PCC_PCCn_CGC_MASK;
    PCC->PCCn[PCC_FTM2_INDEX]  &= ~PCC_PCCn_CGC_MASK;
    PCC->PCCn[PCC_FTM2_INDEX]  |= (PCC_PCCn_PCS(1)| PCC_PCCn_CGC_MASK);		//Clock = 80MHz

    //Pin mux
    PORTD->PCR[9]= PORT_PCR_MUX(1);
    PORTD->PCR[10]= PORT_PCR_MUX(1);
    PORTD->PCR[11]= PORT_PCR_MUX(1);
    PORTD->PCR[12]= PORT_PCR_MUX(1);
    PORTD->PCR[13]= PORT_PCR_MUX(1);
    PORTD->PCR[14]= PORT_PCR_MUX(1);
    PORTD->PCR[15]= PORT_PCR_MUX(1);
    //Output set(set 4bit, 2line - 0b 0010 0101 000x xxxx)

    /*=====================PORT-C KEYPAD=====================*/
	PCC-> PCCn[PCC_PORTC_INDEX] = PCC_PCCn_CGC_MASK; /* Enable clock for PORT C */

	PTC->PDDR |= 1<<12|1<<14|1<<15;		/* Port C12,C14,C15:  Data Direction = output */
	PTC->PDDR &= ~(1<<0);   /* Port C0: Data Direction= input (default) */
	PTC->PDDR &= ~(1<<1);   /* Port C1: Data Direction= input (default) */
	PTC->PDDR &= ~(1<<2);   /* Port C2: Data Direction= input (default) */
	PTC->PDDR &= ~(1<<3);   /* Port C3: Data Direction= input (default) */

	PORTC->PCR[0] = PORT_PCR_MUX(1)|PORT_PCR_PFE_MASK|PORT_PCR_PE(1) | PORT_PCR_PS(0); /* Port C0: MUX = GPIO, input filter enabled */
	PORTC->PCR[1] = PORT_PCR_MUX(1)|PORT_PCR_PFE_MASK|PORT_PCR_PE(1) | PORT_PCR_PS(0); /* Port C1: MUX = GPIO, input filter enabled */
	PORTC->PCR[2] = PORT_PCR_MUX(1)|PORT_PCR_PFE_MASK|PORT_PCR_PE(1) | PORT_PCR_PS(0); /* Port C2: MUX = GPIO, input filter enabled */
	PORTC->PCR[3] = PORT_PCR_MUX(1)|PORT_PCR_PFE_MASK|PORT_PCR_PE(1) | PORT_PCR_PS(0); /* Port C3: MUX = GPIO, input filter enabled */

	PORTC->PCR[12]  = PORT_PCR_MUX(1);	/* Port C12: MUX = GPIO  */
	PORTC->PCR[14]  = PORT_PCR_MUX(1);	/* Port C14: MUX = GPIO  */
	PORTC->PCR[15]  = PORT_PCR_MUX(1);	/* Port C15: MUX = GPIO  */

    /*=====================PORT-B 일단아무거나=====================*/


}

void WDOG_disable (void)
{
	WDOG->CNT=0xD928C520;     /* Unlock watchdog */
	WDOG->TOVAL=0x0000FFFF;   /* Maximum timeout value */
	WDOG->CS = 0x00002100;    /* Disable watchdog */
}

void LPIT0_init (uint32_t delay)
{
   uint32_t timeout;

	/*!
	    * LPIT Clocking:
	    * ==============================
	    */
	  PCC->PCCn[PCC_LPIT_INDEX] = PCC_PCCn_PCS(6);    /* Clock Src = 6 (SPLL2_DIV2_CLK)*/
	  PCC->PCCn[PCC_LPIT_INDEX] |= PCC_PCCn_CGC_MASK; /* Enable clk to LPIT0 regs       */

	  /*!
	   * LPIT Initialization:
	   */
	  LPIT0->MCR |= LPIT_MCR_M_CEN_MASK;  /* DBG_EN-0: Timer chans stop in Debug mode */
	                                        /* DOZE_EN=0: Timer chans are stopped in DOZE mode */
	                                        /* SW_RST=0: SW reset does not reset timer chans, regs */
	                                        /* M_CEN=1: enable module clk (allows writing other LPIT0 regs) */

  timeout=delay* 40;
  LPIT0->TMR[0].TVAL = timeout;      /* Chan 0 Timeout period: 40M clocks */
  LPIT0->TMR[0].TCTRL |= LPIT_TMR_TCTRL_T_EN_MASK;
                                     /* T_EN=1: Timer channel is enabled */
                              /* CHAIN=0: channel chaining is disabled */
                              /* MODE=0: 32 periodic counter mode */
                              /* TSOT=0: Timer decrements immediately based on restart */
                              /* TSOI=0: Timer does not stop after timeout */
                              /* TROT=0 Timer will not reload on trigger */
                              /* TRG_SRC=0: External trigger soruce */
                              /* TRG_SEL=0: Timer chan 0 trigger source is selected*/
}

void delay_us (volatile int us){
   LPIT0_init(us);           /* Initialize PIT0 for 1 second timeout  */
   while (0 == (LPIT0->MSR & LPIT_MSR_TIF0_MASK)) {} /* Wait for LPIT0 CH0 Flag */
               lpit0_ch0_flag_counter++;         /* Increment LPIT0 timeout counter */
               LPIT0->MSR |= LPIT_MSR_TIF0_MASK; /* Clear LPIT0 timer flag 0 */
}

//keypad
int KeyScan(void){
   Dtime = 1000;
   int Kbuff = 0;

   PTC->PSOR |=1<<12;
   delay_us(Dtime);
   if(PTC->PDIR &(1<<0))Kbuff=1;      //1
   if(PTC->PDIR &(1<<1))Kbuff=4;      //4
   if(PTC->PDIR &(1<<2))Kbuff=7;      //7
   if(PTC->PDIR &(1<<3))Kbuff=11;     //*
   PTC->PCOR |=1<<12;

   PTC->PSOR |=1<<14;
   delay_us(Dtime);
   if(PTC->PDIR & (1<<0))Kbuff=2;      //2
   if(PTC->PDIR & (1<<1))Kbuff=5;      //5
   if(PTC->PDIR & (1<<2))Kbuff=8;      //8
   if(PTC->PDIR & (1<<3))Kbuff=0;      //0
   PTC->PCOR |=1<<14;

   PTC->PSOR |=1<<15;
   delay_us(Dtime);
   if(PTC->PDIR & (1<<0))Kbuff=3;      //3
   if(PTC->PDIR & (1<<1))Kbuff=6;      //6
   if(PTC->PDIR & (1<<2))Kbuff=9;      //9
   if(PTC->PDIR & (1<<3))Kbuff=12;     //#
   PTC->PCOR |=1<<15;

   return Kbuff;
}

//7-segmnet
void seg_out(int number){

	Dtime = 1000;

	d1000 = (number/1000)%10;
	d100 = (number/100)%10;
	d10 = (number/10)%10;
	d1 = number%10;

	// 1000자리수 출력
	PTD->PSOR = FND_SEL[j];
	PTD->PCOR =0x7f;
	PTD->PSOR = FND_DATA[d1000];
    delay_us(Dtime);
	PTD->PCOR = 0xfff;
	j++;

	// 100자리수 출력
	PTD->PSOR = FND_SEL[j];
	PTD->PCOR =0x7f;
	PTD->PSOR = FND_DATA[d100];
    delay_us(Dtime);
	PTD->PCOR = 0xfff;
	j++;

	// 10자리수 출력
	PTD->PSOR = FND_SEL[j];
	PTD->PCOR =0x7f;
	PTD->PSOR = FND_DATA[d10];
    delay_us(Dtime);
    PTD->PCOR = 0xfff;
	j++;

	// 1자리수 출력
	PTD->PSOR = FND_SEL[j];
	PTD->PCOR =0x7f;
	PTD->PSOR = FND_DATA[d1];
    delay_us(Dtime);
	PTD->PCOR = 0xfff;
	j=0;
}

//인터럽트


int main(void)
{






    return 0;
}