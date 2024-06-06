#include <util/delay.h>
#include <avr/io.h>
#include <usart.h>
#include <display.h>

int main()
{
    initUSART();
    initDisplay();
    int a=1;
    int b=2;
    int c=3;
    int d=4;
    while ( 1 )
    {
        int number = ((d%10)*1000)+((c%10)*100)+((b%10)*10)+(a%10);
        for(int i=0;i<10000;i++){
        writeNumber( number );
        }
        printf("number %d \n",number);
        a++;
        b++;
        c++;
        d++;
    }
    return 0;
}