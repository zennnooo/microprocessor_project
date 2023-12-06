
void game2(void) {
    int num1, num2, answer, playerAnswer;
	char congratulations[16] = {0x43, 0x6f, 0x6e, 0x67, 0x72, 0x61, 0x74, 0x75, 0x6c, 0x61, 0x74, 0x69, 0x6f, 0x6e, 0x73, 0x21};
	char incorrect[9] = {0x69, 0x6e, 0x63, 0x6f, 0x72, 0x72, 0x65, 0x63, 0x74};


    // 랜덤한 두 숫자 선택
    num1 = crand(2, 9);
    num2 = crand(2, 9);

    // 정답 계산
    answer = num1 * num2;

    int key=0, pre_key = 100;
       int output_num = 0;
while(1){


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

	// 플레이어에게 답 입력 받기
//	for(;;)
  //    {


         key=KeyScan();
         if ((key < 10) & !(pre_key == key)) // Key button push
         {
		output_num = output_num * 10 + key; // output data update
		output_num %= 100; // 2자리만 출력
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
			        while(congratulations[i] != '\0')
					{
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
			        while(incorrect[i] != '\0')
					{
						lcdcharinput(incorrect[i]);
						delay_us(80000);
						i++;
					}
			    }

}

int isPrime(int num) {
    if (num <= 1) {
        return 0;  // 0 and 1 are not prime
    }
    for (int i = 2; i * i <= num; ++i) {
        if (num % i == 0) {
            return 2;  // num is divisible by i, so not prime
        }
    }
    return 1;  // num is prime
}

void game4(void) {
	int randomNumber;
	char YESMessage[20] = {0x31, 0x20, 0x66, 0x6f, 0x72, 0x20, 0x59, 0x45, 0x53};
    char NoMessage[20] = {0x30, 0x20, 0x66, 0x6f, 0x72, 0x20, 0x4e, 0x4f, 0x00};
    char congratulations[16] = {0x43, 0x6f, 0x6e, 0x67, 0x72, 0x61, 0x74, 0x75, 0x6c, 0x61, 0x74, 0x69, 0x6f, 0x6e, 0x73, 0x21};
    char incorrect[9] = {0x69, 0x6e, 0x63, 0x6f, 0x72, 0x72, 0x65, 0x63, 0x74};

    srand((unsigned int)time(NULL));

    lcdinput(0x01);
	delay_us(20000);
	lcdinput(0x80);
	delay_us(20000);
	i=0;
    while(YESMessage[i] != '\0')
    {
		lcdcharinput(YESMessage[i]); // 3(third) row text-char send to LCD module
		delay_us(80000);
		i++;
	}

	lcdinput(0x80+0x40);// second row
	delay_us(20000);
	i=0;
	while(NoMessage[i] != '\0')
    {
		lcdcharinput(NoMessage[i]);// 4(fourth) row text-char send to LCD module
		delay_us(80000);
		i++;
	}


    //for (;;) {
        randomNumber = crand(2, 79);  // Generate a random number between 2 and 99
       // printf("Is %d a prime number? (1 for Yes, 0 for No, 2 to exit): ", randomNumber);


        // 플레이어에게 답 입력 받기
   int key=0, pre_key = 100;
   int output_num = 0;
	while(1)
      {
		if(output_num == isPrime(randomNumber)) {
		        	 break;
		         }

		seg_out(randomNumber);
         key=KeyScan();
         if ((key < 10) & !(pre_key == key)) // Key button push
         {
		output_num = output_num * 10 + key; // output data update
		output_num %= 100; // 2자리만 출력
         }
         pre_key = key; // 반복 입력 방지를 위한

         if(output_num == isPrime(randomNumber)) {
        	 break;
         }

     }
        if (output_num == isPrime(randomNumber)) {
                lcdinput(0x01);
        		delay_us(20000);
        		lcdinput(0x80);
        		delay_us(20000);

                i = 0;
                while(congratulations[i] != '\0')
        		{
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
                while(incorrect[i] != '\0')
        		{
        			lcdcharinput(incorrect[i]);
        			delay_us(80000);
        			i++;
        		}
    //}
            	}

}
