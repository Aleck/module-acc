#include <linux/module.h>
#include <linux/moduleparam.h>


#include <linux/kernel.h>	/* printk() */
#include <linux/fs.h>           /* almeno i numeri */
#include <linux/slab.h>		/* kmalloc() */
#include <linux/ioport.h>       /* device region */

#include <asm/uaccess.h>        /* for writing and reading */
#include <linux/ioctl.h>        /* for the ioctl function */
#include <linux/semaphore.h>    /* for the semaphore */
#include <asm/io.h>		/* for read and write in memory */

#include <linux/time.h>         /* for the polling */
#include <linux/sched.h>        /* for the polling */
#include <linux/delay.h>        /* for the polling */

#include <linux/interrupt.h>	/* for the interrupt */



#include "acc.h"





//******************************************** declarative part ************************************
MODULE_LICENSE("GPL");


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
struct resource* my_device_region;
void __iomem* device_virtual_address;


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
struct command_argument* kernel_argument;


// the start command, an int?
const int start_command = 0;
const int done_status = DONE_STATUS_VALUE;


//for handling concurrency
struct semaphore kernel_argument_semaphore;

//interrupt parameters
unsigned int irq = IRQ_LINE;

//****************************************************************************************************





//************************** module operation ****************************************


static int acc_open(struct inode *inode, struct file *filp) {
	return 0;
}


static int acc_release(struct inode *inode, struct file *filp) {
	return 0;
}


//the function that the accelerator implement
static long acc_ioctl(struct file *filp, unsigned int cmd, unsigned long arg) {

	int result = 0;
	unsigned int offset, offset_param = 0;
	size_t tx_byte_total, tx_byte_32, tx_byte_16, tx_byte_8 = 0;
	size_t tx_byte = 0;


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
		result = copy_from_user(kernel_argument, (int __user *)arg, sizeof(struct command_argument));
		if (result > 0) {
			up(&kernel_argument_semaphore);
			printk(KERN_ALERT "%i byte result unwritten from user to kernel, wrong command?\n", result);
			return -ENOMEM;
		}		
		
		
		//********* send the params to the device addr **********
		// the size of the status register, for now it is an int
		offset = sizeof(int);
		
		
		
		//check the size of the param
		tx_byte_total = sizeof(struct command_argument);
		tx_byte_32 = (tx_byte_total/4)*4;
		tx_byte_16 = ((tx_byte_total - tx_byte_32) / 2)*2;
		tx_byte_8 =  tx_byte_total - tx_byte_32 - tx_byte_16;
		
		
		// send the 32 bit struct part
		for(tx_byte = 0; tx_byte < tx_byte_32; tx_byte+=4) {
			iowrite32_rep(device_virtual_address + tx_byte + offset, ((void *)kernel_argument) + tx_byte, 1);
		}
		
		offset_param = tx_byte_32;
		
		
		// send the 16 bit struct part
		for(tx_byte = 0; tx_byte < tx_byte_16; tx_byte+=2) {
			iowrite16_rep(device_virtual_address + tx_byte + offset + offset_param, ((void *)kernel_argument) + tx_byte + offset_param, 1);
		}
		
		offset_param += tx_byte_16;
		
		// send the 8 bit struct part
		for(tx_byte = 0; tx_byte < tx_byte_8; tx_byte+=1) {
			iowrite8_rep(device_virtual_address + tx_byte + offset + offset_param, ((void *)kernel_argument) + tx_byte + offset_param, 1);
		}
		
		
		//********* send the start to the device addr **********
		iowrite32_rep(device_virtual_address, &start_command, 1);
		
		
		
		// this is a polling?
		while(1) {
		
			// read the state of the register
			int state = ioread32(device_virtual_address);
			
			// check if the computation is done
			if (state == done_status) {
				break;
			}
		
			// if not wait a millisecond
			msleep(1000);	
		}
		
		// ************ read the result ***********
		//for an int it's easy
		kernel_argument->return_value = ioread32(device_virtual_address + offset);
		
		//write the result in the user space variable (if any return)
		result = copy_to_user((int __user *)arg, kernel_argument, sizeof(struct command_argument));
		if (result > 0) {
			up(&kernel_argument_semaphore);
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



// the interupt handler
irqreturn_t acc_handler(int irq, void *dev_id, struct pt_regs *regs) {





	return IRQ_HANDLED;
}









//*************************************************************************************





//***************** module initialization and cleanup ***********************

int acc_init_module(void) {
	
	// declaration of the variable
	int result = 0;
	dev_t device_number = 0;  // for getting the major and minor number
	
	
	// take care of the device memory region
	my_device_region = request_mem_region(base_address, size_address, name);
	if (my_device_region == NULL) {
		printk(KERN_ALERT "%s: can't get I/O mem address 0x%lx\n", name, base_address);
		return -ENODEV;
	}
	
	
	// remap the physical address into a virtual
	device_virtual_address = ioremap(base_address, size_address);
	
	// initialize the interrupt handler
	result = request_irq(irq, acc_handler, IRQF_SHARED, name, NULL);
	if(result) {
		printk(KERN_ALERT "%s: can't register interface interrupt \n", name);
		free_irq(irq, NULL);
		return -EBUSY;
	}
	enable_irq(irq);
	
	
	//allocate the space for the device parameters
	kernel_argument = kmalloc(sizeof(struct command_argument), GFP_KERNEL);
	if (!kernel_argument) {
		printk(KERN_ALERT "%s: can't allocate enough memory\n", name);
		release_mem_region(base_address,size_address);
		iounmap(device_virtual_address);
		return -ENOMEM;
	}
	
	//initialize the resource semaphore
	sema_init(&kernel_argument_semaphore, sem_max_access);
	

	// Here we register our device
	// WARNING! from here any app can start to call our operation
	result = register_chrdev(major, name, &acc_fops);	
	if (result < 0) {
		printk(KERN_ALERT "%s: can't get major number\n", name);
		release_mem_region(base_address,size_address);
		kfree(kernel_argument);
		iounmap(device_virtual_address);
		return result;
	}
	
	//set the minor and major number
	major = MAJOR(device_number);
	minor = MINOR(device_number);
	
	printk(KERN_INFO "%s: the size of the param is: %i\n", name, sizeof(struct command_argument));
	printk(KERN_INFO "%s: module init OK!\n", name);
	
	return 0;
}
 


void acc_cleanup_module(void)
{
	release_mem_region(base_address,size_address);
	kfree(kernel_argument);
	iounmap(device_virtual_address);
	disable_irq(irq);
	free_irq(irq, NULL);
	printk(KERN_INFO "%s: module cleanup OK!\n", name);
}





//****************************************************************************************************


module_init(acc_init_module);
module_exit(acc_cleanup_module);





