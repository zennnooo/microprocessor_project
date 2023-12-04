#include <stdio.h>
#include <stdlib.h>
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

//interrupt
unsigned int External_PIN=0; /* External_PIN:SW External input Assignment */

//lcd
unsigned int i = 0;


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
    //1 - c13, 2 - c8, 3 - c12, 4 - c3, 5 - c15, 6 - c2, 7 - c9
    PCC-> PCCn[PCC_PORTC_INDEX] = PCC_PCCn_CGC_MASK; /* Enable clock for PORT C */

    PTC->PDDR |= 1<<12|1<<13|1<<15;		/* Port C12,C14,C15:  Data Direction = output */
    PTC->PDDR &= ~(1<<8);   /* Port C0: Data Direction= input (default) */ //1st row
    PTC->PDDR &= ~(1<<9);   /* Port C1: Data Direction= input (default) */ //2nd row
    PTC->PDDR &= ~(1<<2);   /* Port C2: Data Direction= input (default) */ //3rd row
    PTC->PDDR &= ~(1<<3);   /* Port C3: Data Direction= input (default) */ //4th row

    PORTC->PCR[8] = PORT_PCR_MUX(1)|PORT_PCR_PFE_MASK|PORT_PCR_PE(1) | PORT_PCR_PS(0); /* Port C0: MUX = GPIO, input filter enabled */
    PORTC->PCR[9] = PORT_PCR_MUX(1)|PORT_PCR_PFE_MASK|PORT_PCR_PE(1) | PORT_PCR_PS(0); /* Port C1: MUX = GPIO, input filter enabled */
    PORTC->PCR[2] = PORT_PCR_MUX(1)|PORT_PCR_PFE_MASK|PORT_PCR_PE(1) | PORT_PCR_PS(0); /* Port C2: MUX = GPIO, input filter enabled */
    PORTC->PCR[3] = PORT_PCR_MUX(1)|PORT_PCR_PFE_MASK|PORT_PCR_PE(1) | PORT_PCR_PS(0); /* Port C3: MUX = GPIO, input filter enabled */

    PORTC->PCR[12]  = PORT_PCR_MUX(1);	/* Port C12: MUX = GPIO  */ //1st col
    PORTC->PCR[13]  = PORT_PCR_MUX(1);	/* Port C14: MUX = GPIO  */ //2nd col
    PORTC->PCR[15]  = PORT_PCR_MUX(1);	/* Port C15: MUX = GPIO  */ //3rd col

    /*=====================PORT-B 일단아무거나=====================*/
    //test를 위해 sw5(down - b12), sw4(up - b11)연결해서 up-down-counter interrupt 구현
    PCC->PCCn[PCC_PORTB_INDEX]|=PCC_PCCn_CGC_MASK;   /* Enable clock for PORTB */
    PTB->PDDR &= ~(1<<11);		/* Port B11 Port Input set, value '0'*/
    PTB->PDDR &= ~(1<<12);      /* Port B12 Port Input set, value '0'*/
    PORTB->PCR[11] |= PORT_PCR_MUX(1); // Port B11 mux = GPIO
	PORTB->PCR[11] |=(10<<16); // Port B11 IRQC : interrupt on Falling-edge
    PORTB->PCR[12] |= PORT_PCR_MUX(1); // Port B12 mux = GPIO
	PORTB->PCR[12] |=(10<<16); // Port B12 IRQC : interrupt on Falling-edge

	//buzer
	PTB->PDDR |= 1<<13;
	PORTB->PCR[13]  = PORT_PCR_MUX(1);

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

//port B 인터럽트
void NVIC_init_IRQs(void){
	S32_NVIC->ICPR[1] |= 1<<(60%32); // Clear any pending IRQ60
	S32_NVIC->ISER[1] |= 1<<(60%32); // Enable IRQ60
	S32_NVIC->IP[60] =0xB; //Priority 11 of 15
}

void PORTB_IRQHandler(void){
	//PORTB_Interrupt State Flag Register Read
	if((PORTB->ISFR & (1<<11)) != 0){
		External_PIN=1;
	}
	else if((PORTB->ISFR & (1<<12)) != 0){
		External_PIN=2;
	}

	// External input Check Behavior Assignment
	switch (External_PIN){
		case 1:
			num += 1;
			External_PIN=0;
			break;
		case 2:
			num -= 1;
			External_PIN=0;
			break;
		default:
			break;
	}

	PORTB->PCR[11] |= 0x01000000; // Port Control Register ISF bit '1' set
	PORTB->PCR[12] |= 0x01000000; // Port Control Register ISF bit '1' set
}


//keypad
int KeyScan(void){
   Dtime = 1000;
   int Kbuff = 100;

   PTC->PSOR |=1<<12;
   delay_us(Dtime);
   if(PTC->PDIR &(1<<8))Kbuff=1;      //1
   if(PTC->PDIR &(1<<9))Kbuff=4;      //4
   if(PTC->PDIR &(1<<2))Kbuff=7;      //7
   if(PTC->PDIR &(1<<3))Kbuff=11;     //*
   PTC->PCOR |=1<<12;

   PTC->PSOR |=1<<13;
   delay_us(Dtime);
   if(PTC->PDIR & (1<<8))Kbuff=2;      //2
   if(PTC->PDIR & (1<<9))Kbuff=5;      //5
   if(PTC->PDIR & (1<<2))Kbuff=8;      //8
   if(PTC->PDIR & (1<<3))Kbuff=0;      //0
   PTC->PCOR |=1<<13;

   PTC->PSOR |=1<<15;
   delay_us(Dtime);
   if(PTC->PDIR & (1<<8))Kbuff=3;      //3
   if(PTC->PDIR & (1<<9))Kbuff=6;      //6
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
	PTE->PSOR = FND_SEL[j];
	PTE->PCOR =0x7f;
	PTE->PSOR = FND_DATA[d1000];
    delay_us(Dtime);
	PTE->PCOR = 0xfff;
	j++;

	// 100자리수 출력
	PTE->PSOR = FND_SEL[j];
	PTE->PCOR =0x7f;
	PTE->PSOR = FND_DATA[d100];
    delay_us(Dtime);
	PTE->PCOR = 0xfff;
	j++;

	// 10자리수 출력
	PTE->PSOR = FND_SEL[j];
	PTE->PCOR =0x7f;
	PTE->PSOR = FND_DATA[d10];
    delay_us(Dtime);
    PTE->PCOR = 0xfff;
	j++;

	// 1자리수 출력
	PTE->PSOR = FND_SEL[j];
	PTE->PCOR =0x7f;
	PTE->PSOR = FND_DATA[d1];
    delay_us(Dtime);
	PTE->PCOR = 0xfff;
	j=0;
}

//특정 범위안에서 random key 생성
int crand(int min, int max)
{
	return min + rand() % (max - min + 1);
}


//main 함수
/*
int main(void)
{






    return 0;
}
*/
//board 테스트용 코드 (결선확인 위한 코드) : 평소에는 주석처리
//test할 때만 기존 main 주석처리하고 아래의 main 사용
///*
int main(void)
{
    WDOG_disable();/* Disable Watchdog in case it is not done in startup code */
	PORT_init();            /* Configure ports */
	SOSC_init_8MHz();        /* Initialize system oscilator for 8 MHz xtal */
	SPLL_init_160MHz();     /* Initialize SPLL to 160 MHz with 8 MHz SOSC */
	NormalRUNmode_80MHz();  /* Init clocks: 80 MHz sysclk & core, 40 MHz bus, 20 MHz flash */
	NVIC_init_IRQs(); /*Interrupt Pending, Endable, Priority Set*/
    SystemCoreClockUpdate();
    delay_us(20000);

    //여기부터
    int page =0;
    char msg_array1[10] = {0x57, 0x65, 0x6C, 0x63, 0x6F, 0x6D, 0x65, 0x20, 0x74, 0x6F}; //Welcome to
    char msg_array2[14] = {0x43, 0x6F, 0x75, 0x6E, 0x74, 0x69, 0x6E, 0x67, 0x20, 0x47, 0x61, 0x6D, 0x65, 0x21}; //Counting Game!
    char msg_array3[12] = {0x50, 0x72, 0x65, 0x73, 0x73, 0x20, 0x73, 0x77, 0x31, 0x20, 0x74, 0x6F}; //Press sw1 to
    char msg_array4[17] = {0x73, 0x65, 0x65, 0x20, 0x74, 0x68, 0x65, 0x20, 0x67, 0x61, 0x6D, 0x65, 0x20, 0x6C, 0x69, 0x73, 0x74};//see game list
    char msg_array5[16] = {0x31, 0x2E, 0x20, 0x6D, 0x65, 0x6D, 0x6F, 0x72, 0x79, 0x20, 0x32, 0x2E, 0x20, 0x39, 0x78, 0x39};//1. memory 2. 9x9
    char msg_array6[17] = {0x33, 0x2E, 0x20, 0x31, 0x39, 0x78, 0x31, 0x39, 0x20, 0x34, 0x2E, 0x20, 0x70, 0x72, 0x69, 0x6D, 0x65};//3. 19x19 4. prime


    lcdinit();        /* Initialize LCD1602A module*/
	delay_us(20000);

	//text-char output
	while(msg_array1[i] != '\0')
    {
		lcdcharinput(msg_array1[i]); // 1(first) row text-char send to LCD module
		delay_us(80000);
		i++;
	}

	lcdinput(0x80+0x40);// second row
	delay_us(20000);
	i=0;
	while(msg_array2[i] != '\0')
    {
		lcdcharinput(msg_array2[i]);// 2(second) row text-char send to LCD module
		delay_us(80000);
		i++;
	}

    //Lcd off, LCD display clear
	delay_us(20000);
	lcdinput(0x08);	//lcd display off
	delay_us(4000);
	lcdinput(0x01);	//Clear display
	delay_us(200000);
	
    while(msg_array3[i] != '\0')
    {
		lcdcharinput(msg_array3[i]); // 3(third) row text-char send to LCD module
		delay_us(80000);
		i++;
	}

	lcdinput(0x80+0x40);// second row
	delay_us(20000);
	i=0;
	while(msg_array4[i] != '\0')
    {
		lcdcharinput(msg_array4[i]);// 4(fourth) row text-char send to LCD module
		delay_us(80000);
		i++;
	}
		
	
	if (PTB->PDIR & (1 << 9))
	{
	    page = page +1;
	}

	else if (PTB->PDIR & (1 << 10))
	{
	    page = page -1;
	}
	
    //의도가 애매
	if (page == 1)
	{
        delay_us(20000);
	    lcdinput(0x08);	//lcd display off
	    delay_us(4000);
	    lcdinput(0x01);	//Clear display
	    delay_us(20000); 

	    while(msg_array5[i] != '\0')
        {
		    lcdcharinput(msg_array5[i]);// 5(fifth) row text-char send to LCD module
		    delay_us(80000);
		    i++;
	    }
    }


	else if(page == 2)
	{
        delay_us(20000);
	    lcdinput(0x08);	//lcd display off
	    delay_us(4000);
	    lcdinput(0x01);	//Clear display
	    delay_us(20000);

	    while(msg_array6[i] != '\0')
        {
		    lcdcharinput(msg_array6[i]);// 5(sixth) row text-char send to LCD module
		    delay_us(80000);
		    i++;
	    }
    }

	//반복문 추가해야할듯

	//여기까지

    num = 0000;
    int key=0, pre_key = 100;
    int output_num = 0;

    //중복이 제거된 random key
	//난이도 조절을 위해선 crand의 범위 조절하기
	int rand_key = 0;
	int pre_rand = 100;
	int cnt = 0;

    int rand_arr[500];
    for (;;) 
    {
        if (cnt == 100) {
            break;
        }

        rand_key = crand(1,10);
        if (rand_key != pre_rand) {
            rand_arr[cnt++] = rand_key;
            pre_rand = rand_key;
        }
        else {
            continue;
        }
    }

    printf("rand_arr : \n");
    for (int n = 0; n < 100; n++) {
        if (n % 10 == 9) {
            printf("%d\n", rand_arr[n]);
        }
        else {
            printf("%d ", rand_arr[n]);
        }
    }

    int k = 0;

    while (1)
    {
        /*//up-down counter by interrupt
    	key=KeyScan();
        if ((key < 10) & !(pre_key == key)) // Key button push
        {
        	output_num = output_num * 10 + key; // output data update
        	output_num %= 10; // 1자리만 출력
        }
        pre_key = key; // 반복 입력 방지를 위한
    	if (num != output_num)
    	{
    		seg_out(num);
    		PTB-> PCOR |= 1<<13;
    	}
    	else
    	{
    		PTB-> PSOR |= 1<<13;
    		seg_out(output_num+1);

    	}*/


    	for (int m = 0; m < 10; m++){
    		for (int n = 0; n < 250; n++) {
    		    	seg_out(rand_arr[m]);
    		 }
    	}
    }

    //Lcd off, LCD display clear
	delay_us(20000);
	lcdinput(0x08);	//lcd display off
	delay_us(4000);
	lcdinput(0x01);	//Clear display
	delay_us(2000);

    return 0;
}
//*/
