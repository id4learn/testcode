#include<stdio.h>
#include<stdint.h>
#include<stdlib.h>
#include<string.h>

void cyclicSort();

main()
{
  cyclicSort();

}

void cyclicSort()
{
  uint16_t  arr[] = {3,1,5,4,2};

  uint8_t i,j = 0;
  uint8_t len = (uint16_t)sizeof(arr)/sizeof(uint16_t);
  uint16_t  tmp = 0;
  

  for( i = 0 ; i < len ; i++) {
    if( i != arr[i]-1) {
      j = arr[i]-1;
      // swap value
      tmp = arr[i];
      arr[i] = arr[j];
      arr[j] = tmp;
    }
  }

  printf("\n CycliSort Arr:");
  for( i = 0 ; i < len ; i++) 
    printf("\t [%u]",arr[i]);

}

