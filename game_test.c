#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

void game1(void);
void game2(void);
void game4(void);

// lcd
unsigned int i = 0;
unsigned int num, page;

// 특정 범위 안에서 random key 생성
int crand(int min, int max)
{
    return min + rand() % (max - min + 1);
}

// 소수 판별
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

// main 함수
int main(void)
{
    // test를 위한 출력에서는 seg_out과 lcd 출력은 printf, keyscan은 scanf
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

    i = 0;
    while (msg_array1[i] != '\0')
    {
        printf("%c", msg_array1[i]);
        i++;
    }
    printf("\n");
    i = 0;
    while (msg_array2[i] != '\0')
    {
        printf("%c", msg_array2[i]);
        i++;
    }
    printf("\n");
    i = 0;
    while (msg_array3[i] != '\0')
    {
        printf("%c", msg_array3[i]);
        i++;
    }
    printf("\n");
    i = 0;
    while (msg_array4[i] != '\0')
    {
        printf("%c", msg_array4[i]);
        i++;
    }
    printf("\n");

    page = 0;
    int select_game = 0;
    int press = 0;

    // page select
    while (1)
    {
        scanf("%d", &page);

        // page = num;
        if (page == 1)
        {

            i = 0;
            while (msg_array5[i] != '\0')
            {
                printf("%c", msg_array5[i]);
                i++;
            }
            printf("\n");

            i = 0;
            while (msg_array6[i] != '\0')
            {
                printf("%c", msg_array6[i]);
                i++;
            }
            printf("\n");

            page = 0;
            while (page == 0)
            {
                printf("select_game : ");
                scanf("%d", &select_game); // adc

                if (select_game >= 4000)
                {
                    i = 0;
                    while (game1_arr[i] != '\0')
                    {
                        printf("%c", game1_arr[i]);
                        i++;
                    }
                    printf("\n");

                    printf("press sw1(대체 숫자 8) to start game : ");
                    scanf("%d", &press);

                    if (press == 8) {
                        game1();
                    }
                }
                else if (select_game >= 3000)
                {
                    i = 0;
                    while (game2_arr[i] != '\0')
                    {
                        printf("%c", game2_arr[i]);
                        i++;
                    }
                    printf("\n");

                    printf("press sw1(대체 숫자 8) to start game : ");
                    scanf("%d", &press);


                    if (press == 8) {
                        game2();
                    }
                }
                else if (select_game >= 2000)
                {
                    i = 0;
                    while (game3_arr[i] != '\0')
                    {
                        printf("%c", game3_arr[i]);
                        i++;
                    }
                    printf("\n");

                    printf("press sw1(대체 숫자 8) to start game : ");
                    scanf("%d", &press);

                    /*if (press == 8) {
                        //game3();
                    }*/
                }
                else if (select_game >= 1000)
                {
                    i = 0;
                    while (game4_arr[i] != '\0')
                    {
                        printf("%c", game4_arr[i]);
                        i++;
                    }
                    printf("\n");

                    printf("press sw1(대체 숫자 8) to start game : ");
                    scanf("%d", &press);

                    if (press == 8) {
                        for (int p = 1; p <= 10; p++) {
                            printf("problem %d : ", p);
                            game4();
                        }
                    }
                }
                else
                {
                    
                }
            }
        }
    }

    return 0;
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
    printf("problem : ");
    for (int i = 0; i < 10; i++) {
        printf("%d ", numbers[i]);
    }
    printf("\n");

    // 사용자 입력 받기
    int key = 0, preKey = 100;

    i = 0;
    while (prompt[i] != '\0') {
        printf("%c", prompt[i]);
        i++;
    }
    printf("\n");

    int userAnswer[10];

    // 사용자 입력 받기
    int cnt = 0;
    printf("input answer : \n");
    while (1) {
        scanf("%d", &key);
        userAnswer[cnt++] = key;

        if (cnt == 10) break;
    }

    for (int i = 0; i < 10; i++) {
        printf("%d ", userAnswer[i]);
    }
    printf("\n");

    // 정답 체크 및 출력
    int a = 0;
    while(1) {
        if (userAnswer[a] != numbers[a]) {
            i = 0;
            while (tryAgainMsg[i] != '\0') {
                printf("%c", tryAgainMsg[i]);
                i++;
            }
            printf("\n");
            break;
        } 
        a++;
        if (a == 10) {
            i = 0;
            while (correct[i] != '\0') {
                printf("%c", correct[i]);
                i++;
            }
            printf("\n");
            break;
        }
    }
}


void game2(void) 
{
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

        //test
        //printf("init key : %d\n", key);
        //printf("init outputnum : %d\n", output_num);

        //seg_out자리
        printf("%d * %d\n", num1, num2);
        // 플레이어에게 답 입력 받기
        //keyscan대신 scanf
        printf("input answer : ");
        scanf("%d", &key);
        output_num = key;

        //printf("after scan key : %d\n", key);
        //printf("after scan outputnum : %d\n", output_num);
    }

    // 정답 체크
    if (output_num == answer)
    {
        i = 0;
        while (congratulations[i] != '\0') {
            printf("%c", congratulations[i]);
            i++;
        }
        printf("\n");
    } 
    else 
    {
        i = 0;
        while (incorrect[i] != '\0') {
            printf("%c", incorrect[i]);
            i++;
        }
        printf("\n");
    }
}

void game4(void) {
    int randomNumber;
    char YESMessage[20] = {0x31, 0x20, 0x66, 0x6f, 0x72, 0x20, 0x59, 0x45, 0x53};
    char NoMessage[20] = {0x32, 0x20, 0x66, 0x6f, 0x72, 0x20, 0x4e, 0x4f, 0x00};
    char congratulations[17] = {0x43, 0x6f, 0x6e, 0x67, 0x72, 0x61, 0x74, 0x75, 0x6c, 0x61, 0x74, 0x69, 0x6f, 0x6e, 0x73, 0x21};
    char incorrect[10] = {0x69, 0x6e, 0x63, 0x6f, 0x72, 0x72, 0x65, 0x63, 0x74};

    i=0;
    while (YESMessage[i] != '\0') {
        printf("%c", YESMessage[i]);
        i++;
    }
    printf("\n");

    i=0;
    while (NoMessage[i] != '\0') {
        printf("%c", NoMessage[i]);
        i++;
    }
    printf("\n");

    //srand(time(NULL));
    randomNumber = crand(0, 99);  // Generate a random number between 2 and 99

    int key = 0;//, pre_key = 100;
    int output_num = 0;

    while (1) {
        if (output_num == isPrime(randomNumber)) {
            break;
        }

        printf("%d\n", randomNumber); //segout randomnumber
        printf("isPrime answer : %d\n", isPrime(randomNumber));
        printf("input answer : ");
        scanf("%d", &key); //keyscan

        output_num = key;
    }

    if (output_num == isPrime(randomNumber)) {
        i = 0;
        while (congratulations[i] != '\0') {
            printf("%c", congratulations[i]);
            i++;
        }
        printf("\n");
    } 
    else {
        i = 0;
        while (incorrect[i] != '\0') {
            printf("%c", incorrect[i]);
            i++;
        }
        printf("\n");
    }
}
