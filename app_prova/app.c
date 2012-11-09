#include <stdio.h>
#include <sys/ioctl.h>
#include "acc.h"   //needed for the struct and the command



int main() {
	FILE *acc;
	int result = 0;
	acc = fopen("/dev/acc0", "r+");
	struct command_argument parameters;
	parameters.param1 = 500;
	parameters.param2 = 2;
	parameters.param3 = 4;
	parameters.return_value = 0;
	result = ioctl(fileno(acc), ACC_OPERATION, &parameters);
	
	
	if (!(result == 0)) {
		printf("APP: executed system call, and he returned %i (should be 0)\n", result);
	}
	
	if (!(parameters.return_value == parameters.param1 + parameters.param2 + parameters.param3)) {
		printf("APP: the return value is: %i\n", parameters.return_value);
	} else {
		printf("APP: It works!!!!!!!!!!!!!!\n", parameters.return_value);
	}
	fclose(acc);
	return 0;
}
