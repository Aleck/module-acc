#include <linux/module.h>
#include <linux/moduleparam.h>


#include <linux/kernel.h>	/* printk() */
#include <linux/fs.h>           /* almeno i numeri */
#include <linux/slab.h>		/* kmalloc() */
#include <linux/ioport.h>       /* device region */

#include <asm/uaccess.h>        /* for writing and reading */







//******************************************** declarative part ************************************
MODULE_AUTHOR("Alberto Lucchetti, Davide Gadioli");
MODULE_LICENSE("GPL");

// the major and minor number
int major = 0;    //dynamic allocation
int minor = 0;    //dynamic allocation

//static parameter
static long base_address = 0x30000000;  // the base device address
static long size_address = 0x01000000;  // the size of address reserved
static char* name = "acc";

//my device to be implemented
struct acc_dev *acc_device;

//the device's operation available
struct file_operations acc_fops = {
	.owner =    THIS_MODULE,
//	.read  =     acc_read,
//	.write =     acc_write,
//	.open  =     acc_open,
};




//****************************************************************************************************








//***************** module initialization and cleanup ***********************

int acc_init_module(void) {
	
	//declaration of the variable
	int result = 0;
	dev_t device_number = 0;  // for getting the major and minor number
	
	if (! request_region(base_address, size_address, name)) {
		printk(KERN_INFO "%s: can't get I/O port address 0x%lx\n", name, base_address);
		return -ENODEV;
	}

	// Here we register our device - should not fail thereafter
	result = register_chrdev(major, name, &acc_fops);	
	if (result < 0) {
		printk(KERN_INFO "%s: can't get major number\n", name);
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





