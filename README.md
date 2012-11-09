module-acc
==========

#### File organization ####

In this repository we have structurated the source file in this way:

* module -> this folder contains the source files of the device driver
* app -> this folder contains the source files of the toy app that test the driver
* simulatore -> this folder contains the source files of the stub used to simulate the accelerator



#### Software needed ####

The OpenRISC toolchain is needed in order to build the device driver and the toy app.
The or1ksim is needed in order to build the stub.
Since the module is compiled outside the kernel tree, you must have a compiled kernel tree in order to build the device driver.

Usefull link:
The OpenRISC toolchain: http://openrisc.net/toolchain-build.html


#### How to compile ####

In order to correctly using the device driver is needed explain how the simulator works. Once you have build the simulator, it must be feeded with a configuration file that describe the SoC and an or32 executable file.
The default configuration file is located in linux/arch/openrisc and the executable file is the one you get from the kernel tree "vmlinux".
In that file is included even the fs, where the "/" path in the simulated os is generated from the folder linux/arch/openrisc/support/initramfs.
The device driver will be installed in the "/modules" directory.

- configure

In the firts line of the makefile is possible tune the parameters relative to the path

- build

Execute make in the wanted subdirectory

- install

Execute make install in the wanted subdirectory.
The module makefile handle the "/modules" directory by itself and add the module loader in the init script. In this way the device driver is loaded automatically when the os is started.



### How to use the simulator ###

Once you have installed the application and the module, build the kernel to commit the added files in the linux executable. The stub acts like the standalone simulator, except the fact that if no paramters is passed it assume that the config file and the linux exutable are in the working directory.

We use a modified configuration file, which is based on the generic file found in linux/arch/openrisc plus a geniric section that rapresent the physical device:

```
section generic
  enabled = 1
  baseaddr = 0xa0000000
  size = 0x00000014
  name = "acc"
  byte_enabled = 1
  hw_enabled = 1
  word_enabled = 1 
end
```

and we have set a xterm console for interacting with the simulated os in the uart section.

If you have copied the "vmlinux" exutables in the "simulatore" directory you can simulate everything by run "./simulatore", a xterm console should appear and from that you can launch the toy application and see if it works.


### INTRODUCTION TO THE DEVICE DRIVER ###

A character driver is suitable for most simple hardware device. Char devices are accessed through names in the filesystem. Those name are called special files or device files or simply nodes of the filesystem tree; they are conventionally located in the /dev directory. If yuo issue the ls -l command. you'll see two numbers in the device. These numbers are the major and the minor device number for the particular device. The major number identifies the driver associated with the device. The minor number is used by the kernel to determine exatcly wich device is being referred to. You can either get a direct pointer to your device from the kernel, or you can use the minor number yourself as an index into a local array of the device. Either way, the kernel itself knows almost nothing about minor number beyond the fact that they refer to devices implemented by your driver. 
You can get these number dynamically or you can specify them manually. We have choose to set them dynamically, becuase in this way the numbers are handled by the kernel itself.


### LOAD SCRIPT ###

When the simulated os is booted, its init script load the module executing the load_module_acc script. This simple script call the insmod command that perform the insert of the acc_module in the kernel by calling the module's init function (see the initializitaion and cleanup section). If it passed the initialization of the module the script look which major and minor number (see the initializitaion and cleanup section) are assigned by the kernel and create the module node in /dev/acc0. In this way every operations peformed over that file are allowed by the module.


### INITIALIZATION AND CLEANUP ###
We start talking about the function that perform the initialization of the module, called by the insmod command. The statement that actually register the device and then request the numbers is in the last part of that function, because, when we register the device, the kernel can use the module immediatly, so everything (like the space in the I/O memory or the request of the interrupt line...) must be correctly initialized.

As you can see in the acc.c file, in the init function you have to:

* Take care of the I/O memory region with the instruction:

	```
	my_device_region = request_mem_region(base_address, size_address, name);
	
	```
	
The function request_mem_region needs the physical address and the size of the device and its (arbitrary) name. This function reserves the I/O memory of the device. If the result of this function is NULL, the kernel can't get the I/O memory address, so you have to check this fact.
You can check if it worked by typing "cat /proc/iomem"
	   
* Then you have to remap the real physical address into a virtual one that can be used, with the instruction:


		device_virtual_address = ioremap(base_address, size_address);

		
this function return a pointer of type __iomem*, which is a void* pointer but is stressed the fact is a pointer to an I/O memory. This should works fine but in our case, we have appended the following code 


		EXPORT_SYMBOL(__ioremap);
		EXPORT_SYMBOL(iounmap);

		
in the file or32_ksyms.c that is located at linux/arch/openrisc/kernel, because the token ioremap (and iounmap) isn't declared.
	   
* If there is an interrupt handler in your device you have to initialize here the interrupt with the function


		result = request_irq(irq, (irq_handler_t) acc_handler, IRQF_SHARED, name, (void*)(acc_handler));

		
The function request_irq, reserve the physical line specified by the variable irq to our device driver. The parameter 		   acc_handler is the function of the interrupt handler also define in this file. IRQF_SHARED is a flags that distingues the 		   type of the interrupt line.
You can check if it worked by typing "cat /proc/interrupts"
	   
* Then you have to allocate the space for the device parameters with the function:


		kernel_argument = kmalloc(sizeof(struct command_argument), GFP_KERNEL);

		
where kernerl_argument is a global variable declared of type of the struct that you define as the parameter structure, that in our case is command_argumet. In this variable we store all the parameters that the device needs (even the return value).
	   
* If you need a samphore for the lock of the device, you need to initalize it here with the function:


		sema_init(&kernel_argument_semaphore, sem_max_access);

		
To takle the concurrency problem we use a global semaphore, wich actually is a mutex in our case. Further explanation on how it work is in the module operation section.
	   
* Finally you have to register your device with the instruction 


		device_number = register_chrdev(major, name, &acc_fops);

		
The variable major is the major number set to 0 (this means that we set the minor and the major number dynamically), name is 	   the device name and acc_fops is the file of the operation and is a global variable declared of type struct file_operations. 		   This struct countains all the operations of the device node that we will create with the load script.
From this point on the kernel can call one function that we have specifies in acc_fops.
* Then we have to get the minor and major number with the instruction:


		major = MAJOR(device_number);
		minor = MINOR(device_number);

		
The acc_cleanup_module function undone all the initializations that we have done.


### MODULE OPERATION ###

The operation allowed by the module are: open, close and ioctl.

* Open
This simple operation is called whenever the device node /dev/acc0 is open. There is no particular operation that must be done, because the device is an hardware device alredy powerd on and initializated. Here, if used an interrupt mechanism, there is the request of the interrupt line.

* Close
This function is simple the dual of the open operation. Here, if used an interrupt mechanism, there is the free of the interrupt line. In this way the module occupies the interrupt line only when used.

* Ioctl
The ioctl function is an advanced operation that rappresent a system call. An application that want to do a system call, must set a description file of the device node, the operation number and, optionally, other argument. In this case the description file of the device node is /dev/acc0 created with the load script. The operation number gives two information about a system call: the magic number and the operation number. The magic number is used to identify the module that must handle the operation and the other identify the operation itself. In the kernel documentation there is a list of used magic number. In this moment the magic number used by the module is free. The operation number is an incremental value starting from zero. If there are more than one device, the correspondent module can share the magic nuber, but different operation number.
If the operation that the device perform has a return value, the whole operation number must be obtained by the macro

		_IOWR(ACC_IOC_MAGIC, 0, struct command_argument)
		
otherwise use 

		_IOW(ACC_IOC_MAGIC, 0, struct command_argument)
		
the read and write are intended from the application point of view. This imply that an application which use the system call must include the module header for the magic number and the struct command_argument, which is explained early.

	Warning: the old version of this macro, as third parameter wanted the size of the data passed from user space to kernel space.
		
In this module, this function fills the data needed by the device and return, once available, the result to the user.
The module use a ioctl function to achieve this goal because it's possible for the module to see the device's operation as an atomic one.
To tackle the concurrency problem the module use a mutex to handle multiple system call.
This function consist of three part:

1) once we hold a semaphore we copy form the user space to the kernel space the parameter, than we write in the device memory. We use a 32bit write until we can because is the most efficient, then if there some bit left we do the 16 or 8 bit write.

2) once transferred all the parameters, we write in the " device state" the start command. The module don't check the "device state" because the module uses the semaphore.

3) now the module has to wait until the device perform the operation. If it's used polling, the module periodically read the "device state". If it's used the interrupt, the module go to sleep until the interrupt handler wake up the module.

4) then the module reads the return value from the device memory, which in this module is an integer; copy the update structure from the kernel space to the user space.

5) release the semaphore and complete the execution.


### INTERRUPT HANDLER ###
This function is called by the kernel, whenver there is an interrupt in the specified interrupt line (initialized when an application open the device node). This function wake up the module and return a IRQ_HANDLED flag.


### THE STUB ###
Or1ksim can be used as a library. The simulator expose a set of function to be used by the stub. The function that we use are:

		int or1ksim_init(int argc , char *argv , void *class_ptr , int (*upr )(void *class_ptr , unsigned long int addr , unsigned char mask [], unsigned char rdata [], int data_len ), int (*upw )(void *class_ptr , unsigned long int addr , unsigned char mask [], unsigned char wdata [], int data_len ))

this function initialize the or1ksim. The first two parmaters rapprest the argument taken by the standalone simulator. The third parameter is a pointer to a c++ class that can be passed back with the upcall. The fourth and fifth are the reading and writing upcall function. These two function are called by the library whenever the simulated application (linux in this case) read or write in an address defined in the custom section of the configuration file. These function have as parameter the class specified in the initialization (not used by the simulator itself), the object address, two vector rapresenting the used byte (32, 16 or 8 bit in reading/write) and the data transferred. The last parameter of the upcalling function is the length of the data array. 

		int or1ksim_run (double duration )
		
this function execute the simulator by duration specified, if set -1, it act like the standalone simulator.

		void or1ksim_interrupt (int i )
		
this function generate an interrupt in the specified interrupt line.

In this case the stub enulate the behaviour of a simple device that execute a simple mathematical operation. Thus even the stub use a simple state machine with 3 state:

* READY: means that the device can be used.
* COMPUTING: means that the device is doing its work. This obviously fake computation delay stand for the real device one. Since we want to have a human-like delay is set in order of second.
* DONE: means that the device have generated a result (if any) that can be readed from the device memory.

To achieve the computation delay, we use a cycle that slot the time of the computation.

```

	//main simulator cycle
  	while(1){
  		or1ksim_run(UNIT_DELAY);
 
  		if (acc_param->state == COMPUTING) {
  			unit_count--;
  			if (unit_count <= 0) acc_function();
  				else std::cout << "STUB: Computation start in t -" << unit_count << std::endl;
  		}
  		
  	}
  	
```

acc_function perform the simple mathematical operation, with the interrupt signal if used..

In this application the parameters have a static address where they are stored (because they are four int). So whenever there is a reading or a writing in the device memory, the object address give us information about which paramater we are reading or writing; thus it's used a switch based on the base address offset. In the writng function, when some application in the simulated enviroment write the status address we start the computation. When in the reading function is read the return value the operation is done.


### SUMMARY OF THIS CASE STUDY ###

The easy way of getting all the prerequisites is to download a modified ubuntu virtual image by the openRISC team at this site:

		http://opencores.org/or1k/Ubuntu_VirtualBox-image_updates_and_information
		
Nowadays is updated at 2011 11 28 and all works. If you want to manage to build the toolchain by yourself check this site:

		http://openrisc.net/toolchain-build.html
		
However the interrupt don't work in the git version of linux.
These examples get the assumption that is used the virtual image, which is refered in this guide as "ubuntu".

