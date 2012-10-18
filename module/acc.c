#include <linux/module.h>
#include <linux/moduleparam.h>


#include <linux/kernel.h>	/* printk() */
#include <linux/fs.h>           /* almeno i numeri */
#include <linux/slab.h>		/* kmalloc() */
#include <linux/ioport.h>       /* device region */

#include <asm/uaccess.h>        /* for writing and reading */
#include <linux/ioctl.h>        /* for the ioctl function */
#include <linux/semaphore.h>    /* for the semaphore */


#include "acc.h"





//******************************************** declarative part ************************************

// the major and minor number
int major = 0;    //dynamic allocation
int minor = 0;    //dynamic allocation

//static parameter
static long base_address = BASE_ADDR;  // the base device address
static long size_address = SIZE_ADDR;  // the size of address reserved
static char* name = DEVICE_NAME;


//module operation's prototype
static int acc_open(struct inode *inode, struct file *filp);
static int acc_release(struct inode *inode, struct file *filp);
static long acc_ioctl(struct file *filp, unsigned int cmd, unsigned long arg);

//the device memory
struct region* my_device_region;

//the device's operation available
struct file_operations acc_fops = {
	.owner =    THIS_MODULE,
//	.read  =     acc_read,
//	.write =     acc_write,
	.open  =     acc_open,
        .release =   acc_release,
	.unlocked_ioctl =     acc_ioctl,
};

//set the semaphore concurrency
int sem_max_access = 1;    //actually i wont a mutex



//the kernel's command argument
struct comand_argument* kernel_argument;

//for handling concurrency
struct semaphore kernel_argument_semaphore;
struct semaphore device_argument_semaphore;



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
static long acc_ioctl(struct file *filp, unsigned int cmd, unsigned long arg) {

	int result = 0;


	/* these controls should always pass, but if the device magic number
	   isn't unique, they cuold save the day */

	//check the type and number bitfields
	if (_IOC_TYPE(cmd) != ACC_IOC_MAGIC) return -ENOTTY; // inappropriate ioctl
	if (_IOC_NR(cmd) > MAX_HW_OPERATION) return -ENOTTY;
	
	
	//check the whole operation
	if (cmd == ACC_OPERATION) {
	
	
		//take the exlusive control of the kernel_argument
		if (down_interruptible(&kernel_argument_semaphore)) {
    			return -ERESTARTSYS;
 		 }
		
		//get the data from the user space
		result = copy_from_user(kernel_argument, (int __user *)arg, sizeof(struct comand_argument));
		if (result > 0) {
			printk(KERN_ALERT "%i byte result unwritten from user to kernel, wrong command?\n", result);
			return -ENOMEM;
		}
		
		
		//do the computation, here polling or interrupt
		kernel_argument->return_value = kernel_argument->param1 + kernel_argument->param2;
		
		
		//write the result in the user space variable (if any return)
		result = copy_to_user((int __user *)arg, kernel_argument, sizeof(struct comand_argument));
		if (result > 0) {
			printk(KERN_ALERT "%i byte result unwritten from kernel to user, weird!\n", result);
			return -1;
		}
		
		//release the semaphore
		up(&kernel_argument_semaphore);
		
		
	} else {
		return -ENOTTY;
	}

	return 0;
}







//*************************************************************************************





//***************** module initialization and cleanup ***********************

int acc_init_module(void) {
	
	//declaration of the variable
	int result = 0;
	dev_t device_number = 0;  // for getting the major and minor number
	
	
	my_device_region = request_mem_region(base_address, size_address, name);
	if (my_device_region == NULL) {
		printk(KERN_ALERT "%s: can't get I/O mem address 0x%lx\n", name, base_address);
		return -ENODEV;
	}

	// Here we register our device - should not fail thereafter
	result = register_chrdev(major, name, &acc_fops);	
	if (result < 0) {
		printk(KERN_ALERT "%s: can't get major number\n", name);
		release_mem_region(base_address,size_address);
		return result;
	}
	
	//set the minor and major number
	major = MAJOR(device_number);
	minor = MINOR(device_number);
	
	
	//allocate the space for the device parameters
	kernel_argument = kmalloc(sizeof(struct comand_argument), GFP_KERNEL);
	if (!kernel_argument) {
		printk(KERN_ALERT "%s: can't allocate enough memory\n", name);
		release_mem_region(base_address,size_address);
		return -ENOMEM;
	}
	
	//initialize the resource semaphore
	sema_init(&kernel_argument_semaphore, sem_max_access);
	sema_init(&device_argument_semaphore, sem_max_access);
	
	
	printk(KERN_INFO "%s: module init OK!\n", name);
	
	return 0;
}
 


void acc_cleanup_module(void)
{
	release_mem_region(base_address,size_address);
	kfree(kernel_argument);
	printk(KERN_INFO "%s: module cleanup OK!\n", name);
}





//****************************************************************************************************


module_init(acc_init_module);
module_exit(acc_cleanup_module);





