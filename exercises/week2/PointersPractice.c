#include <avr/io.h>
#include <stdio.h>
#include <usart.h>
int main()
{		
	int var =  5;
	initUSART();
	//calculate power 2 -> write as a function pow2
	pow2(&var);//passing the address
	printf("Power 2 of 5 is %d", var);
	return 0;
}
void pow2(int* a){//we know it's a pointer
    (*a) = (*a) * (*a);
}