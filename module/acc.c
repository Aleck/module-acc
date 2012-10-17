#include <linux/module.h>
#include <linux/moduleparam.h>


#include <linux/kernel.h>	/* printk() */
#include <linux/fs.h>           /* almeno i numeri */
#include <linux/slab.h>		/* kmalloc() */
#include <linux/ioport.h>       /* device region */

#include <asm/uaccess.h>        /* for writing and reading */
#include <linux/ioctl.h>

#include "acc.h"





//******************************************** declarative part ************************************

// the major and minor number
int major = 0;    //dynamic allocation
int minor = 0;    //dynamic allocation

//static parameter
static long base_address = BASE_ADDR;  // the base device address
static long size_address = SIZE_ADDR;  // the size of address reserved
static char* name = DEVICE_NAME;


//the device's operation available
struct file_operations acc_fops = {
	.owner =    THIS_MODULE,
//	.read  =     acc_read,
//	.write =     acc_write,
	.open  =     acc_open,
        .release =   acc_release,
	.unlocked_ioctl =     acc_ioctl,
};




//****************************************************************************************************





//************************** module operation ****************************************


static int acc_open(struct inode *inode, struct file *filp) {
	printk(KERN_INFO "Opened the inode\n");
	return 0;
}


static int acc_release(struct inode *inode, struct file *filp) {
	printk(KERN_INFO "Closed the inode\n");	
	return 0;
}


//the function that the accelerator implement
static int acc_ioctl(struct inode *inode, struct file *filp,
                 unsigned int cmd, unsigned long arg) {

	//handle the command request
	switch(cmd) {
		case ACC_OPERATION_1:
		  printk(KERN_INFO "Comando eseguito, parametro=%i\n", arg);
		  break;
		


		default:
		  printk(KERN_ALERT "Unreacheble statement\n");
	}

	return 0;
}







//*************************************************************************************





//***************** module initialization and cleanup ***********************

int acc_init_module(void) {
	
	//declaration of the variable
	int result = 0;
	dev_t device_number = 0;  // for getting the major and minor number
	
//	if (! request_region(base_address, size_address, name)) {
//		printk(KERN_ALERT "%s: can't get I/O port address 0x%lx\n", name, base_address);
//		return -ENODEV;
//	}

	// Here we register our device - should not fail thereafter
	result = register_chrdev(major, name, &acc_fops);	
	if (result < 0) {
		printk(KERN_ALERT "%s: can't get major number\n", name);
		release_region(base_address,size_address);
		return result;
	}
	
	//set the minor and major number
	major = MAJOR(device_number);
	minor = MINOR(device_number);
	
	printk(KERN_INFO "%s: module init OK!\n", name);
	
	return 0;
}
 


void acc_cleanup_module(void)
{
	release_region(base_address,size_address);
	printk(KERN_INFO "%s: module cleanup OK!\n", name);
}





//****************************************************************************************************


module_init(acc_init_module);
module_exit(acc_cleanup_module);





