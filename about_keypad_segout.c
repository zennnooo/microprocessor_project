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
