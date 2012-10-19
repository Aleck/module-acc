#include <stdio.h>
#include <sys/ioctl.h>
#include "acc.h"   //needed for the struct and the command



int main() {
	FILE *acc;
	int result = 0;
	acc = fopen("/dev/acc0", "r+");
	struct command_argument parametri;
	parametri.param1 = 1;
	parametri.param2 = 2;
	parametri.return_value = 0;
	result = ioctl(fileno(acc), ACC_OPERATION, &parametri);
	printf("APP: executed system call, and he returned %i (should be 0)\n", result);
	printf("APP: the return value is: %i\n", parametri.return_value);
	fclose(acc);
	return 0;
}
