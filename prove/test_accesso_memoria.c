#include <stdio.h>
#include <string.h>

#define ADDRESS 0x90000030



struct  testdev
{
  volatile unsigned char       byte;
  volatile unsigned short int  halfword;
  volatile unsigned long  int  fullword;
};




int main(int argc, char* argv[]) {
  
  printf("Test di accesso diretto alla memoria\n");
 
  struct testdev *dev = (struct testdev *)BASEADDR;

  unsigned char       byteRes;
  unsigned short int  halfwordRes;
  unsigned long int   fullwordRes;

  printf("Se non ha dato segmentation fault è ok");
  
  
  return 0;
}
