#ifndef ACC_HEADER
#define ACC_HEADER

//import necessari
#include <linux/cdev.h>
#include <linux/semaphore.h>
#include <linux/ioport.h>



int acc_init_module(void);
void acc_cleanup_module(void);



#endif
