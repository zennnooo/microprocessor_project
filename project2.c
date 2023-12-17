#include <stdio.h>
#include <stdlib.h>
#include "S32K144.h"
#include "device_registers.h"
#include "clocks_and_modes.h"
#include "ADC.h"
#include "math.h"
#include "lcd1602A.h"

#define PTC13 13

unsigned int Dtime = 0; /* Delay Time Setting Variable*/
int lpit0_ch0_flag_counter = 0; /*< LPIT0 timeout counter */

//segment
unsigned int FND_DATA[11]={0x7E, 0x0C, 0xB6, 0x9E, 0xCC, 0xDA, 0xFA, 0x4E, 0xFE, 0xCE, 0x00, 0xEC}; // 0 ~ 9, nop, H
unsigned int FND_SEL[4]={0x0100, 0x0200, 0x0400, 0x0800}; //d1000, d100, d10, d1
unsigned int j=0; /*FND select pin index */
unsigned int num, d1, d10, d100, d1000 =0;
/*num is Counting value, num0 is '1', num2 is '10', num2 is '100', num3 is '1000'*/

//interrupt
unsigned int External_PIN=0; /* External_PIN:SW External input Assignment */
unsigned int page = 0; //인터럽트를 통해 PAGE인식
unsigned int press = 0; //인터럽트를 총해 game start 인식

//lcd
unsigned int i = 0;

//led
unsigned int led_data[8] = {0x1, 0x2, 0x4, 0x8, 0x4000, 0x8000, 0x10000, 0x20000};


void PORT_init(void)
{
    //사용할 port에 따라서 각각 선언해주기

    /*=====================PORT-A LED=====================*/
    PCC-> PCCn[PCC_PORTA_INDEX] = PCC_PCCn_CGC_MASK;
    PTA->PDDR |= 1<<0| 1<<1| 1<<2| 1<<3| 1<<14| 1<<15| 1<<16| 1<<17;
    PORTA->PCR[0] = PORT_PCR_MUX(1); /* Port D0: MUX = GPIO */
    PORTA->PCR[1] = PORT_PCR_MUX(1); /* Port D1: MUX = GPIO */
	PORTA->PCR[2] = PORT_PCR_MUX(1); /* Port D2: MUX = GPIO */
	PORTA->PCR[3] = PORT_PCR_MUX(1); /* Port D3: MUX = GPIO */
	PORTA->PCR[14] = PORT_PCR_MUX(1); /* Port D4: MUX = GPIO */
	PORTA->PCR[15] = PORT_PCR_MUX(1); /* Port D5: MUX = GPIO */
	PORTA->PCR[16] = PORT_PCR_MUX(1); /* Port D6: MUX = GPIO */
    PORTA->PCR[17] = PORT_PCR_MUX(1); /* Port D7: MUX = GPIO */



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
    //1 - c13, 2 - c8, 3 - c12, 4 - c3, 5 - c17, 6 - c2, 7 - c9
    PCC-> PCCn[PCC_PORTC_INDEX] = PCC_PCCn_CGC_MASK; /* Enable clock for PORT C */

    PTC->PDDR |= 1<<12|1<<13|1<<17;		/* Port C12,C14,C15:  Data Direction = output */
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
    PORTC->PCR[17]  = PORT_PCR_MUX(1);	/* Port C15: MUX = GPIO  */ //3rd col

    /*=====================PORT-B 일단아무거나=====================*/
    //test를 위해 sw5(down - b12), sw4(up - b11)연결해서 up-down-counter interrupt 구현
    PCC->PCCn[PCC_PORTB_INDEX]|=PCC_PCCn_CGC_MASK;   /* Enable clock for PORTB */
    //PTB->PDDR &= ~(1<<9);		/* Port B11 Port Input set, value '0'*/
    //PTB->PDDR &= ~(1<<10);      /* Port B12 Port Input set, value '0'*/
    PTB->PDDR &= ~(1<<11);		/* Port B11 Port Input set, value '0'*/
    PTB->PDDR &= ~(1<<12);      /* Port B12 Port Input set, value '0'*/
    //PORTB->PCR[9] |= PORT_PCR_MUX(1); // Port B11 mux = GPIO
    //PORTB->PCR[9] |=(10<<16); // Port B11 IRQC : interrupt on Falling-edge
    //PORTB->PCR[10] |= PORT_PCR_MUX(1); // Port B12 mux = GPIO
    //PORTB->PCR[10] |=(10<<16); // Port B12 IRQC : interrupt on Falling-edge
    PORTB->PCR[11] |= PORT_PCR_MUX(1); // Port B11 mux = GPIO
	PORTB->PCR[11] |=(10<<16); // Port B11 IRQC : interrupt on Falling-edge
    PORTB->PCR[12] |= PORT_PCR_MUX(1); // Port B12 mux = GPIO
	PORTB->PCR[12] |=(10<<16); // Port B12 IRQC : interrupt on Falling-edge

	//interrupt용 스위치 game start
	PTB->PDDR &= (1<<14);
	PORTB->PCR[14] |= PORT_PCR_MUX(1); // Port B12 mux = GPIO
	PORTB->PCR[14] |=(10<<16); // Port B14 IRQC : interrupt on Falling-edge



	//buzer
/*	PTB->PDDR |= 1<<13;
	PORTB->PCR[13]  = PORT_PCR_MUX(1);*/

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
	else if((PORTB->ISFR & (1<<14)) != 0){
		External_PIN=3;
	}


	// External input Check Behavior Assignment
	switch (External_PIN){
		case 1:
			num = 1;
			page = 1;
			External_PIN=0;
			break;
		case 2:
			//num = 2;
			page = 2;
			External_PIN=0;
			break;
		case 3:
			press = 8;
			External_PIN=0;
			break;
		default:
			break;
	}


	PORTB->PCR[11] |= 0x01000000; // Port Control Register ISF bit '1' set
	PORTB->PCR[12] |= 0x01000000; // Port Control Register ISF bit '1' set
    PORTB->PCR[14] |= 0x01000000; // Port Control Register ISF bit '1' set
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
   if(PTC->PDIR & (1<<3))Kbuff=99;     //#
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

//game2 seg_out
void seg_out_game2(int num1, int num2) {
	Dtime = 1000;

    // 문제 출력 1
    PTE->PSOR = FND_SEL[0];
	PTE->PCOR =0x7f;
	PTE->PSOR = FND_DATA[num1];
    delay_us(Dtime);
	PTE->PCOR = 0xfff;


	//곱하기
	PTE->PSOR = FND_SEL[1];
	PTE->PCOR =0x7f;
	PTE->PSOR = FND_DATA[10];
    delay_us(Dtime);
	PTE->PCOR = 0xfff;


	// 문제 출력 2
	PTE->PSOR = FND_SEL[2];
	PTE->PCOR =0x7f;
	PTE->PSOR = FND_DATA[num2];
    delay_us(Dtime);
	PTE->PCOR = 0xfff;


	// 빈칸
	PTE->PSOR = FND_SEL[3];
	PTE->PCOR =0x7f;
	PTE->PSOR = FND_DATA[10];
    delay_us(Dtime);
	PTE->PCOR = 0xfff;

}

//특정 범위안에서 random key 생성
int crand(int min, int max)
{
	return min + rand() % (max - min + 1);
}

void game1(void) {
    int numbers[10];
    char prompt[25] = {0x4D, 0x65, 0x6D, 0x6F, 0x72, 0x69, 0x7A, 0x61, 0x74, 0x69, 0x6F, 0x6E, 0x3A, 0x20}; // "Memory: "
    char correct[10] = {0x43, 0x6F, 0x72, 0x72, 0x65, 0x63, 0x74, 0x21}; // "Correct!"
    char tryAgainMsg[12] = {0x54, 0x72, 0x79, 0x20, 0x61, 0x67, 0x61, 0x69, 0x6E, 0x21}; // "Try again!"

    // 랜덤한 10개의 숫자 선택
    //srand(time(NULL)); // 난수 시드 초기화
    for (int i = 0; i < 10; i++) {
        numbers[i] = crand(0, 9); // 0부터 9까지의 난수
    }

    // 숫자를 일정 간격으로 출력
    for (int i = 0; i < 10; i++) {
        /*lcdinput(0x01);
        delay_us(20000);
        lcdinput(0x80);
        delay_us(20000);

        lcdcharinput(prompt[i]);*/ // memory 게임을 진입할 때 이미 출력하므로 필요없을 것으로 보임
                                   // 필요있어도 for문 밖에 있어야할 거 같음.
        //lcdcharinput(numbers[i] + '0');
        seg_out(numbers[i]);
        delay_us(80000); //속도 보고 delay 조절
    }

    //lcdinput(0x01); // 화면 지우기

    // 사용자 입력 받기
    int key = 0, preKey = 100;

    lcdinput(0x80);
    delay_us(20000);
    int i = 0;
    while (prompt[i] != '\0') {
        lcdcharinput(prompt[i]);
        delay_us(80000);
        i++;
    }

    int userAnswer[10];
    /*for (int i = 0; i < 10; i++) {
        lcdcharinput(numbers[i] + '0');
        delay_us(80000);
    }

    lcdinput(0x01); // 화면 지우기
    lcdinput(0x80);
    delay_us(20000);*/

    // 사용자 입력 받기
    /*for (int i = 0; i < 10; i++) {
        key = KeyScan();
        if ((key < 10) && !(preKey == key)) // Key button push
        {
            userAnswer = userAnswer * 10 + key; // 사용자 입력 업데이트
            userAnswer %= 100;                   // 두 자리만 출력
        }
        preKey = key; // 반복 입력 방지를 위한
        //lcdcharinput(key + '0');
        seg_out(key);
        userAnswer[i] = key;
        delay_us(80000);
    }*/

    //사용자 입력 받기 while문
    int cnt = 0;
    while(1) {
        key = KeyScan();
        if ((key < 10) && !(preKey == key)) // Key button push
        {
            userAnswer[cnt] = userAnswer[cnt] * 10 + key; // 사용자 입력 업데이트
            userAnswer[cnt] %= 100;                   // 두 자리만 출력
            cnt++;
        }
        preKey = key; // 반복 입력 방지를 위한
        //lcdcharinput(key + '0');
        seg_out(key);
        userAnswer[cnt] = key;
        delay_us(80000);

        if (cnt == 10) break;
    }

    // 정답 체크 및 출력
    int a = 0;
    while(1)
    {
        if (userAnswer[i] != numbers[i]) {
            lcdinput(0x01);
            delay_us(20000);
            lcdinput(0x80);
            delay_us(20000);

            i = 0;
            while (tryAgainMsg[i] != '\0') {
                lcdcharinput(tryAgainMsg[i]);
                delay_us(80000);
                i++;
            }

            delay_us(2000000);
            break; //틀릴 경우 일정시간 이후 홈으로 돌아감
        }
        a++;
        if(a == 10) {
            lcdinput(0x01);
            delay_us(20000);
            lcdinput(0x80);
            delay_us(20000);

            i = 0;
            while (correct[i] != '\0') {
                lcdcharinput(correct[i]);
                delay_us(80000);
                i++;
            }

            delay_us(2000000);
            break; //게임이 종료되었으니 일정시간 지난 후 홈으로 돌아감
        }
    }
}


void game2(void) {
    int num1, num2, answer, playerAnswer;
    char congratulations[17] = {0x43, 0x6f, 0x6e, 0x67, 0x72, 0x61, 0x74, 0x75, 0x6c, 0x61, 0x74, 0x69, 0x6f, 0x6e, 0x73, 0x21};
    char incorrect[10] = {0x69, 0x6e, 0x63, 0x6f, 0x72, 0x72, 0x65, 0x63, 0x74};

    // 랜덤한 두 숫자 선택
    num1 = crand(2, 9);
    num2 = crand(2, 9);

    // 정답 계산
    answer = num1 * num2;

    int key = 0, pre_key = 100;
    int output_num = 0;

    while (1) {
        if (output_num == answer) {
            break;
        }

        Dtime = 1000;

            // 문제 출력 1
            PTE->PSOR = FND_SEL[0];
        	PTE->PCOR =0x7f;
        	PTE->PSOR = FND_DATA[num1];
            delay_us(Dtime);
        	PTE->PCOR = 0xfff;


        	//곱하기
        	PTE->PSOR = FND_SEL[1];
        	PTE->PCOR =0x7f;
        	PTE->PSOR = FND_DATA[10];
            delay_us(Dtime);
        	PTE->PCOR = 0xfff;


        	// 문제 출력 2
        	PTE->PSOR = FND_SEL[2];
        	PTE->PCOR =0x7f;
        	PTE->PSOR = FND_DATA[num2];
            delay_us(Dtime);
        	PTE->PCOR = 0xfff;


        	// 빈칸
        	PTE->PSOR = FND_SEL[3];
        	PTE->PCOR =0x7f;
        	PTE->PSOR = FND_DATA[10];
            delay_us(Dtime);
        	PTE->PCOR = 0xfff;

        key = KeyScan();
        if ((key < 10) & !(pre_key == key)) // Key button push
        {
            output_num = output_num * 10 + key; // output data update
            output_num %= 100;                // 2자리만 출력
        }
        pre_key = key; // 반복 입력 방지를 위한
    }

    // 정답 체크
    if (output_num == answer) {
        lcdinput(0x01);
        delay_us(20000);
        lcdinput(0x80);
        delay_us(20000);

        i = 0;
        while (congratulations[i] != '\0') {
            lcdcharinput(congratulations[i]);
            delay_us(80000);
            i++;
        }
    } else {
        lcdinput(0x01);
        delay_us(20000);
        lcdinput(0x80);
        delay_us(20000);

        i = 0;
        while (incorrect[i] != '\0') {
            lcdcharinput(incorrect[i]);
            delay_us(80000);
            i++;
        }
    }
}

int isPrime(int num) {
    if (num <= 1) {
        return 2;  // 0 and 1 are not prime
    }
    for (int i = 2; i * i <= num; ++i) {
        if (num % i == 0) {
            return 2;  // num is divisible by i, so not prime
        }
    }
    return 1;  // num is prime
}

void display_LED(int ledNumber) {
    PTA->PCOR = 1 << ledNumber; // Turn on the specified LED
}

void off_LED(int ledNumber) {
    PTA->PSOR = 1 << ledNumber; // Turn on the specified LED
}


void game3(void) {
    int led_arr[10][3]
    for (int l = 0; l < 10; l++) {
        int n1 = crand(0, 7);
    int n2, n3;

    do {
        n2 = crand(0, 7);
    } while (n2 == n1);

    do {
        n3 = crand(0, 7);
    } while (n3 == n1 || n3 == n2); // 중복 제거
        for (int a = 0; a < 3; a++) {

        }
    }

    int n1 = crand(0, 7);
    int n2, n3;

    do {
        n2 = crand(0, 7);
    } while (n2 == n1);

    do {
        n3 = crand(0, 7);
    } while (n3 == n1 || n3 == n2); // 중복 제거

    display_LED(n1);
    display_LED(n2);
    display_LED(n3);
    delay_us(500000);

    int key = 0, pre_key = 100;
    int output_num;
    int output_digits[3];

   /* seg_out(output_num);
            output_digits[0] = output_num / 100;
            output_digits[1] = (output_num %100) / 10;
            output_digits[2] = output_num % 10 ;*/

    while (1) {


    	output_digits[0] = output_num / 100;
    	            output_digits[1] = (output_num %100) / 10;
    	            output_digits[2] = output_num % 10 ;

    	/*seg_out(n1);
    	delay_us(500000);
    	seg_out(n2);
    	delay_us(500000);
    	seg_out(n3);
    	delay_us(500000);*/

    	//output_num % 10;
        if (output_digits[0] == n1 && output_digits[1] == n2 && output_digits[2] == n3)
            break;
        else if (output_digits[0] == n1 && output_digits[1] == n3 && output_digits[2] == n2)
            break;
        else if (output_digits[0] == n2 && output_digits[1] == n1 && output_digits[2] == n3)
            break;
        else if (output_digits[0] == n2 && output_digits[1] == n3 && output_digits[2] == n1)
            break;
        else if (output_digits[0] == n3 && output_digits[1] == n1 && output_digits[2] == n2)
            break;
        else if (output_digits[0] == n3 && output_digits[1] == n2 && output_digits[2] == n1)
            break;



        else {
            key = KeyScan();
            if ((key < 10) & !(pre_key == key)) // Key button push
            {
                output_num = output_num * 10 + key; // output data update
                output_num %= 1000;                 // 3자리만 출력
            }
            pre_key = key;
        }
    	//seg_out(output_num);
        seg_out(n1*100 + n2*10 + n3);
    	//delay_us(500000);

        /*off_LED(n1);
        off_LED(n2);
        off_LED(n3);*/


    }
    lcdinput(0x01);
            delay_us(20000);
            lcdinput(0x80);
        	delay_us(20000);

            char congratulations[17] = {0x43, 0x6f, 0x6e, 0x67, 0x72, 0x61, 0x74, 0x75, 0x6c, 0x61, 0x74, 0x69, 0x6f, 0x6e, 0x73, 0x21};

            int i = 0;
            while (congratulations[i] != '\0') {
                lcdcharinput(congratulations[i]);
                delay_us(80000);
                i++;
            }
}

void game4(void) {
    int randomNumber;
    char YESMessage[20] = {0x31, 0x20, 0x66, 0x6f, 0x72, 0x20, 0x59, 0x45, 0x53};
    char NoMessage[20] = {0x30, 0x20, 0x66, 0x6f, 0x72, 0x20, 0x4e, 0x4f, 0x00};
    char congratulations[16] = {0x43, 0x6f, 0x6e, 0x67, 0x72, 0x61, 0x74, 0x75, 0x6c, 0x61, 0x74, 0x69, 0x6f, 0x6e, 0x73, 0x21};
    char incorrect[9] = {0x69, 0x6e, 0x63, 0x6f, 0x72, 0x72, 0x65, 0x63, 0x74};

    //srand((unsigned int)time(NULL));

    lcdinput(0x01);
    delay_us(20000);
    lcdinput(0x80);
    delay_us(20000);
    int i = 0;
    while (YESMessage[i] != '\0') {
        lcdcharinput(YESMessage[i]); // 3(third) row text-char send to LCD module
        delay_us(80000);
        i++;
    }

    lcdinput(0x80 + 0x40); // second row
    delay_us(20000);
    i = 0;
    while (NoMessage[i] != '\0') {
        lcdcharinput(NoMessage[i]); // 4(fourth) row text-char send to LCD module
        delay_us(80000);
        i++;
    }

    randomNumber = crand(0, 99); // Generate a random number between 2 and 99

    int key = 0, pre_key = 100;
    int output_num = 0;

    while (1) {
        if (output_num == isPrime(randomNumber)) {
            break;
        }

        seg_out(randomNumber);
        key = KeyScan();
        if ((key < 10) & !(pre_key == key)) // Key button push
        {
            output_num = output_num * 10 + key; // output data update
            output_num %= 100;                    // 2자리만 출력
        }
        pre_key = key; // 반복 입력 방지를 위한
    }

    if (output_num == isPrime(randomNumber)) {
        lcdinput(0x01);
        delay_us(20000);
        lcdinput(0x80);
        delay_us(20000);

        i = 0;
        while (congratulations[i] != '\0') {
            lcdcharinput(congratulations[i]);
            delay_us(80000);
            i++;
        }
    } else {
        lcdinput(0x01);
        delay_us(20000);
        lcdinput(0x80);
        delay_us(20000);

        i = 0;
        while (incorrect[i] != '\0') {
            lcdcharinput(incorrect[i]);
            delay_us(80000);
            i++;
        }
    }
}

// main 함수
int main(void) {
    WDOG_disable(); /* Disable Watchdog in case it is not done in startup code */
    PORT_init();    /* Configure ports */
    SOSC_init_8MHz(); /* Initialize system oscillator for 8 MHz xtal */
    SPLL_init_160MHz(); /* Initialize SPLL to 160 MHz with 8 MHz SOSC */
    NormalRUNmode_80MHz(); /* Init clocks: 80 MHz sysclk & core, 40 MHz bus, 20 MHz flash */
    SystemCoreClockUpdate();
    ADC_init();
    delay_us(20000);

    // 여기부터
    char nothing[8] = {0x6E, 0x6F, 0x74, 0x68, 0x69, 0x6E, 0x67}; // nothing

    char msg_array1[11] = {0x57, 0x65, 0x6C, 0x63, 0x6F, 0x6D, 0x65, 0x20, 0x74, 0x6F}; // Welcome to
    char msg_array2[15] = {0x43, 0x6F, 0x75, 0x6E, 0x74, 0x69, 0x6E, 0x67, 0x20, 0x47, 0x61, 0x6D, 0x65, 0x21}; // Counting Game!
    char msg_array3[13] = {0x50, 0x72, 0x65, 0x73, 0x73, 0x20, 0x73, 0x77, 0x31, 0x20, 0x74, 0x6F}; // Press sw1 to
    char msg_array4[18] = {0x73, 0x65, 0x65, 0x20, 0x67, 0x61, 0x6D, 0x65, 0x20, 0x6C, 0x69, 0x73, 0x74}; // see game list
    char msg_array5[17] = {0x31, 0x2E, 0x6D, 0x65, 0x6D, 0x6F, 0x72, 0x79, 0x20, 0x32, 0x2E, 0x39, 0x78, 0x39}; // 1.memory 2.9x9
    char msg_array6[18] = {0x33, 0x2E, 0x31, 0x39, 0x78, 0x31, 0x39, 0x20, 0x34, 0x2E, 0x70, 0x72, 0x69, 0x6D, 0x65}; // 3.19x19 4.prime
    char msg_array7[11] = {0x44, 0x72, 0x69, 0x76, 0x65, 0x20, 0x6D, 0x6F, 0x64, 0x65}; // Drive mode

    char game1_arr[10] = {0x31, 0x2E, 0x20, 0x6D, 0x65, 0x6D, 0x6F, 0x72, 0x79}; // 1. memory
    char game2_arr[7] = {0x32, 0x2E, 0x20, 0x39, 0x78, 0x39}; // 2. 9x9
    char game3_arr[9] = {0x33, 0x2E, 0x20, 0x31, 0x39, 0x78, 0x31, 0x39}; // 3. 19x19
    char game4_arr[9] = {0x34, 0x2E, 0x20, 0x70, 0x72, 0x69, 0x6D, 0x65}; // 4. prime

    char game_start[21] = {0x70, 0x72, 0x65, 0x73, 0x73, 0x20, 0x73, 0x77, 0x31, 0x20, 0x73, 0x74, 0x61, 0x72, 0x74, 0x20, 0x67, 0x61, 0x6D, 0x65};
    //오버하면 뒤에 game 지우기
    lcdinit(); /* Initialize LCD1602A module */
    delay_us(20000);

    // text-char output
    int i = 0;
    while (msg_array1[i] != '\0') {
        lcdcharinput(msg_array1[i]); // 1(first) row text-char send to LCD module
        delay_us(80000);
        i++;
    }

    lcdinput(0x80 + 0x40); // second row
    delay_us(20000);
    i = 0;
    while (msg_array2[i] != '\0') {
        lcdcharinput(msg_array2[i]); // 2(second) row text-char send to LCD module
        delay_us(80000);
        i++;
    }
    delay_us(2000000);

    lcdinput(0x01);
    delay_us(20000);
    lcdinput(0x80);
    delay_us(20000);
    i = 0;
    while (msg_array3[i] != '\0') {
        lcdcharinput(msg_array3[i]); // 3(third) row text-char send to LCD module
        delay_us(80000);
        i++;
    }

    lcdinput(0x80 + 0x40); // second row
    delay_us(20000);
    i = 0;
    while (msg_array4[i] != '\0') {
        lcdcharinput(msg_array4[i]); // 4(fourth) row text-char send to LCD module
        delay_us(80000);
        i++;
    }

    //인터럽트 활성화
   NVIC_init_IRQs(); /* Interrupt Pending, Enable, Priority Set */

    page = 0;
    uint32_t select_game = 0;
    press = 0;

    while (1) {
        // page = num;

        if (page == 1) {
            lcdinput(0x01);
            delay_us(20000);
            lcdinput(0x80);
            delay_us(20000);

            i = 0;
            while(msg_array5[i] != '\0') {
                lcdcharinput(msg_array5[i]);
                delay_us(80000);
                i++;
            }

            lcdinput(0x80+0x40);
            delay_us(20000);

            i = 0;
            while(msg_array6[i] != '\0') {
                lcdcharinput(msg_array6[i]);
                delay_us(80000);
                i++;
            }

            page = 0;
            while (page == 0) {
                convertAdcChan(13);
                while(adc_complete()==0) {}
                select_game = read_adc_chx();

                seg_out(select_game);
                if (select_game >= 4000) {
                    lcdinput(0x01);
                    delay_us(20000);
                    lcdinput(0x80);
                    delay_us(20000);

                    i = 0;
                    while (game4_arr[i] != '\0') {
                        lcdcharinput(game4_arr[i]);
                        delay_us(80000);
                        i++;
                    }
                    //press sw1 to start game
                    lcdinput(0x80+0x40);
                    delay_us(20000);
                    i = 0;
                    while(game_start[i] != '\0') {
                        lcdcharinput(game_start[i]);
                        delay_us(80000);
                        i++;
                    }
                    //b인터럽트 sw1 press = 8
                    //PTB14

                    while (1) { //스위치 눌릴 때 까지 기다리기
                        //스위치 눌림 -> 게임구현
                        if (press == 8) {
                            for (int p = 0; p < 10; p++) {
                                game4();
                            }
                            press = 0;
                            break; //게임 10판 진행하고 while문 탈출
                        }
                        else if (page == 1 || page == 2) {
                            //page 바뀌면 탈출?
                            //없어도 되려나?
                            break;
                        }
                    }
                }
                else if (select_game >= 3000) {
                    lcdinput(0x01);
                    delay_us(20000);
                    lcdinput(0x80);
                    delay_us(20000);

                    i = 0;
                    while (game3_arr[i] != '\0') {
                        lcdcharinput(game3_arr[i]);
                        delay_us(80000);
                        i++;
                    }
                    //press sw1 to start game
                    lcdinput(0x80+0x40);
                    delay_us(20000);
                    i = 0;
                    while(game_start[i] != '\0') {
                        lcdcharinput(game_start[i]);
                        delay_us(80000);
                        i++;
                    }
                    //b인터럽트 sw1 press = 8
                    //PTB14

                    while (1) { //스위치 눌릴 때 까지 기다리기
                        //스위치 눌림 -> 게임구현
                        if (press == 8) {
                            game3();
                            press = 0;
                            break; //게임 10판 진행하고 while문 탈출
                        }
                        else if (page == 1 || page == 2) {
                            //page 바뀌면 탈출?
                            //없어도 되려나?
                            break;
                        }
                    }
                }
                else if (select_game >= 2000) {
                    lcdinput(0x01);
                    delay_us(20000);
                    lcdinput(0x80);
                    delay_us(20000);

                    i = 0;
                    while (game2_arr[i] != '\0') {
                        lcdcharinput(game2_arr[i]);
                        delay_us(80000);
                        i++;
                    }
                    //press sw1 to start game
                    lcdinput(0x80+0x40);
                    delay_us(20000);
                    i = 0;
                    while(game_start[i] != '\0') {
                        lcdcharinput(game_start[i]);
                        delay_us(80000);
                        i++;
                    }
                    //b인터럽트 sw1 press = 8
                    //PTB14

                    while (1) { //스위치 눌릴 때 까지 기다리기
                        //스위치 눌림 -> 게임구현
                        if (press == 8) {
                            for (int p = 1; p <= 10; p++) {
                                game2();
                            }
                            press = 0;
                            break; //게임 10판 진행하고 while문 탈출
                        }
                        else if (page == 1 || page == 2) {
                            //page 바뀌면 탈출?
                            //없어도 되려나?
                            break;
                        }
                    }
                }
                else if (select_game >= 1000) {
                    lcdinput(0x01);
                    delay_us(20000);
                    lcdinput(0x80);
                    delay_us(20000);

                    i = 0;
                    while (game1_arr[i] != '\0') {
                        lcdcharinput(game1_arr[i]);
                        delay_us(80000);
                        i++;
                    }
                    //press sw1 to start game
                    lcdinput(0x80+0x40);
                    delay_us(20000);
                    i = 0;
                    while(game_start[i] != '\0') {
                        lcdcharinput(game_start[i]);
                        delay_us(80000);
                        i++;
                    }
                    //b인터럽트 sw1 press = 8
                    //PTB14

                    while (1) { //스위치 눌릴 때 까지 기다리기
                        //스위치 눌림 -> 게임구현
                        if (press == 8) {
                            game1();
                            press = 0;
                            break; //게임 10판 진행하고 while문 탈출
                        }
                        else if (page == 1 || page == 2) {
                            //page 바뀌면 탈출?
                            //없어도 되려나?
                            break;
                        }
                    }
                }
                else {
                    //일단은 빈칸
                }
            }
        }
        else if (page == 2) {
            if (select_game >= 1000) {
                lcdinput(0x01);
                delay_us(20000);
                lcdinput(0x80);
                delay_us(20000);

                i = 0;
                while (nothing[i] != '\0') {
                    lcdcharinput(nothing[i]);
                    delay_us(80000);
                    i++;
                }
            }
            else {
                lcdinput(0x01);
                delay_us(20000);
                lcdinput(0x80);
                delay_us(20000);

                i = 0;
                while (msg_array7[i] != '\0') {
                    lcdcharinput(msg_array7[i]);
                    delay_us(80000);
                    i++;
                }
            }
        }
    }

    return 0;
}