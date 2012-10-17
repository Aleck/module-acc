#include <stdio.h>
#include <sys/ioctl.h>
#include "acc.h"



int main() {


	printf("APP: start\n");

	FILE *acc;

	acc = fopen("/dev/acc0", "r+");
	printf("APP: inode opened\n");
	
	
	struct comand_argument parametri;
	parametri.param1 = 1;
	parametri.param2 = 2;
	parametri.return_value = 0;
	ioctl(fileno(acc), ACC_OPERATION, &parametri);
	printf("APP: executed system call\n");
	
	
	
	if 
	
	
	
	
	

	fclose(acc);
	printf("APP: inode closed\n");

	printf("APP: end\n");


	return 0;
}
