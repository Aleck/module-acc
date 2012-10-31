#include <stdio.h>
#include <sys/ioctl.h>
#include "acc.h"   //needed for the struct and the command



int main() {
	FILE *acc;
	int result = 0;
	acc = fopen("/dev/acc0", "r+");
	struct command_argument parametri;
	parametri.param1 = 500;
	parametri.param2 = 2;
	parametri.param3 = 4;
	parametri.return_value = 0;
	result = ioctl(fileno(acc), ACC_OPERATION, &parametri);
	
	
	if (!(result == 0)) {
		printf("APP: executed system call, and he returned %i (should be 0)\n", result);
	}
	
	if (!(parametri.return_value == parametri.param1 + parametri.param2 + parametri.param3)) {
		printf("APP: the return value is: %i\n", parametri.return_value);
	} else {
		printf("APP: It works!!!!!!!!!!!!!!\n", parametri.return_value);
	}
	fclose(acc);
	return 0;
}
