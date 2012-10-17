#ifndef ACC_HEADER
#define ACC_HEADER


#include <linux/ioctl.h>




//accelerator parameter
#define BASE_ADDR 0x30000000
#define SIZE_ADDR 0x01000000
#define DEVICE_NAME "acc"
#define IOCTL_ARG1 'q'   //our token



//the operation supported
#define ACC_OPERATION_1 0




int acc_init_module(void);
void acc_cleanup_module(void);


//module operation's function
static int acc_open(struct inode *inode, struct file *filp);
static int acc_release(struct inode *inode, struct file *filp);
static int acc_ioctl(struct inode *inode, struct file *filp, unsigned int cmd, unsigned long arg);



#endif
