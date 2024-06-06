#include <usart.h>
#include <string.h>
 
#define MAX 5 //array size
#define SEVENFOLD(x) (x *7) //we pass a x and return x*7
 
 
void makeArray(int nums[]){
  for(int i=0; i<MAX; i++)
  {
     nums[i] = SEVENFOLD(i);
  }
}
 
void printArry(int nums[])
{
  printf("Contents of the array\n");
  for (int i = 0; i < MAX; i++)
  {
    printf("\tAddress: %p has value %d\n", &nums[i], nums[i]);
  }
}
 
int main(void)
{
  initUSART();
  int arr[MAX];
  for (int i = 0; i < MAX; i++  )
  {
    arr[i] = 0;
  }
 
  printArry(arr);

   makeArray(arr);
   printArry(arr);//and pass it 
 
  return 0;
}