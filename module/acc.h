#ifndef ACC_HEADER
#define ACC_HEADER


#include <linux/ioctl.h>




//accelerator parameters
#define BASE_ADDR 0xa0000000
#define SIZE_ADDR 0x00000014
#define DEVICE_NAME "acc"
#define DONE_STATUS_VALUE 2

//interrupt parameters
#define IRQ_LINE 5




// the struct that we exchange from user to the module
struct command_argument {
	int param1;
	int param2;
	int param3;
	int return_value;
};



/*     ****** WARNING! *********
this number should be an unique identifier
among the used devices, check if is true
in the file linux/Documentation/ioctl/ioctl-number.txt */
#define ACC_IOC_MAGIC 0xFA   


/* the operation supported by this device
   NB: the macro _IO* should change in
	-> _IOW( ... ) if return void
	-> _IOWR( ... ) otherwise

   NB2: the second argument start with 0 and is incresed
   as the number of devices grows */
#define ACC_OPERATION _IOWR(ACC_IOC_MAGIC, 0, struct command_argument)


//the total number of devices (starts from zero)
#define MAX_HW_OPERATION 0





#endif
