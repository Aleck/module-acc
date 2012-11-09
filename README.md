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

In order to correctly using the device driver is needed to explain how the simulator works. Once you have build the simulator, it must be feeded with a configuration file that describe the SoC and an or32 executable file.
The default configuration file is located in linux/arch/openrisc and the executable file is the one you get from the kernel tree "vmlinux".
In that file is included even the fs, where the "/" path in the simulated os is generated from the folder linux/arch/openrisc/support/initramfs.
The device driver will be installed in the "/modules" directory.

- configure

In the firts line of the makefile is possible tune the parameters relative to the path

- build

Execute make in the wanted subdirectory

- install

Execute make install in the wanted subdirectory.
The module makefile handles the "/modules" directory by itself and adds the module loader in the init script. In this way the device driver is loaded automatically when the os is started.



### How to use the simulator ###

Once you have installed the application and the module, build the kernel to commit the added files in the linux executable. The stub acts like the standalone simulator, except the fact that if no paramters is passed it assumes that the config file and the linux exutable are in the working directory.

We use a modified configuration file, which is based on the generic file found in linux/arch/openrisc plus a generic section that rappresents the physical device:

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

When the simulated os is booted, its init script load the module executing the load_module_acc script. This simple script calls the insmod command that performs the insert of the acc_module in the kernel by calling the module's init function (see the initializitaion and cleanup section). If it passed the initialization of the module the script looks which major and minor number (see the initializitaion and cleanup section) are assigned by the kernel and creates the module node in /dev/acc0. In this way every operations peformed over that file are allowed by the module.


### INITIALIZATION AND CLEANUP ###
We start talking about the function that performs the initialization of the module, called by the insmod command. The statement that actually registers the device and then requests the numbers is in the last part of that function, because, when we register the device, the kernel can use the module immediatly, so everything (like the space in the I/O memory or the request of the interrupt line...) must be correctly initialized.

As you can see in the acc.c file, in the init function you have to:

* Take care of the I/O memory region with the instruction:

	```
	my_device_region = request_mem_region(base_address, size_address, name);
	
	```
	
The function request_mem_region needs the physical address and the size of the device and its (arbitrary) name. This function reserves the I/O memory of the device. If the result of this function is NULL, the kernel can't get the I/O memory address, so you have to check this fact.
You can check if it worked by typing "cat /proc/iomem" in the simulated enviroment.
	   
* Then you have to remap the real physical address into a virtual one that can be used, with the instruction:


		device_virtual_address = ioremap(base_address, size_address);

		
this function returns a pointer of type __iomem*, which is a void* pointer but is stressed the fact is a pointer to an I/O memory. This should works fine but in our case, we have appended the following code 


		EXPORT_SYMBOL(__ioremap);
		EXPORT_SYMBOL(iounmap);

		
in the file or32_ksyms.c that is located at linux/arch/openrisc/kernel/or32_ksyms.c, because the token ioremap (and iounmap) isn't declared.
	   
* If there is an interrupt handler in your device you have to initialize here the interrupt with the function


		result = request_irq(irq, (irq_handler_t) acc_handler, IRQF_SHARED, name, (void*)(acc_handler));

		
The function request_irq, reserves the physical line specified by the variable irq to our device driver. The parameter 		   acc_handler is the function of the interrupt handler also define in this file. IRQF_SHARED is a flags that distinguishes the 		   type of the interrupt line.
You can check if it worked by typing "cat /proc/interrupts" in the simulated enviroment.
	   
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

* Open: this simple operation is called whenever the device node /dev/acc0 is open. There is no particular operation that must be done, because the device is an hardware device alredy powerd on and initializated. Here, if used an interrupt mechanism, there is the request of the interrupt line.

* Close: this function is simple the dual of the open operation. Here, if used an interrupt mechanism, there is the free of the interrupt line. In this way the module occupies the interrupt line only when used.

* Ioctl: the ioctl function is an advanced operation that rappresents a system call. An application that wants to do a system call, must sets a description file of the device node, the operation number and, optionally, other argument. In this case the description file of the device node is /dev/acc0 created with the load script. The operation number gives two information about a system call: the magic number and the operation number. The magic number is used to identify the module that must handle the operation and the other identify the operation itself. In the kernel documentation there is a list of used magic number. In this moment the magic number used by the module is free. The operation number is an incremental value starting from zero. If there are more than one device, the correspondent module can share the magic nuber, but different operation number.
If the operation that the device perform has a return value, the whole operation number must be obtained by the macro

		_IOWR(ACC_IOC_MAGIC, 0, struct command_argument)
		
otherwise use 

		_IOW(ACC_IOC_MAGIC, 0, struct command_argument)
		
the read and write are intended from the application point of view. This implies that an application which uses the system call must include the module header for the magic number and the struct command_argument, which is explained early.

	Warning: the old version of this macro, as third parameter wanted the size of the data passed from user space to kernel space.
		
In this module, this function fills the data needed by the device and return, once available, the result to the user.
The module uses a ioctl function to achieve this goal because it's possible for the module to sees the device's operation as an atomic one.
To tackle the concurrency problem the module uses a mutex to handle multiple system call.
This function consist of three part:

1) once we hold a semaphore we copy form the user space to the kernel space the parameter, than we write in the device memory. We use a 32bit write until we can because is the most efficient, then if there some bit left we do the 16 or 8 bit write.

2) once transferred all the parameters, we write in the " device state" the start command. The module doesn't check the "device state" because the module uses the semaphore.

3) now the module has to wait until the device performs the operation. If it's used polling, the module periodically reads the "device state". If it's used the interrupt, the module goes to sleep until the interrupt handler wakes up the module.

4) then the module reads the return value from the device memory, which in this module is an integer; copies the update structure from the kernel space to the user space.

5) releases the semaphore and complete the execution.


### INTERRUPT HANDLER ###
This function is called by the kernel, whenver there is an interrupt in the specified interrupt line (initialized when an application open the device node). This function wakes up the module and return a IRQ_HANDLED flag.


### THE STUB ###
Or1ksim can be used as a library. The simulator exposes a set of function to be used by the stub. The function that we use are:

		int or1ksim_init(int argc , char *argv , void *class_ptr , int (*upr )(void *class_ptr , unsigned long int addr , unsigned char mask [], unsigned char rdata [], int data_len ), int (*upw )(void *class_ptr , unsigned long int addr , unsigned char mask [], unsigned char wdata [], int data_len ))

this function initializes the or1ksim. The first two parmaters rappresent the argument taken by the standalone simulator. The third parameter is a pointer to a c++ class that can be passed back with the upcall. The fourth and fifth are the reading and writing upcall function. These two function are called by the library whenever the simulated application (linux in this case) reads or writes in an address defined in the custom section of the configuration file. These function have as parameter the class specified in the initialization (not used by the simulator itself), the object address, two vector rapresenting the used byte (32, 16 or 8 bit in reading/write) and the data transferred. The last parameter of the upcalling function is the length of the data array. 

		int or1ksim_run (double duration )
		
this function executes the simulator by duration specified, if set -1, it acts like the standalone simulator.

		void or1ksim_interrupt (int i )
		
this function generates an interrupt in the specified interrupt line.

In this case the stub emulates the behaviour of a simple device that executes a simple mathematical operation. Thus even the stub uses a simple state machine with 3 state:

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
		
Nowadays is updated at 2011/11/28 and all works. If you want to manage to build the toolchain by yourself check this site:

		http://openrisc.net/toolchain-build.html
		
However the interrupt don't work in the git version of linux.
These examples get the assumption that is used the virtual image, which is refered in this guide as "ubuntu".
Create a working folder in the home of ubuntu. Clone in this folder this repository. Also copy the linux three source that you can find in ~/soc-design/linux.
Now you should have in the working folder two other folders: linux and module-acc.
Before you can compile the module you must compile at last once the linux three, so cd to it and do:

		make ARCH=openrisc defconfig
		make
		make clean
		
Now you can compile the module, by cding in module-acc/module and:

		make
		make install
		
Note: in the version that we have used, in order to use the ioremap, we have appendende the following line:

		EXPORT_SYMBOL(__ioremap);
		EXPORT_SYMBOL(iounmap);
in the file or32_ksyms.c that is located at linux/arch/openrisc/kernel/or32_ksyms.c .
		
you need alsto the application to test the module, so in module-acc/app_prova do:
		
		make
		make install
		
Now you can create the linux executable like above. Is more suitable to copy the "vmlinux" exutable in folder module-acc/simulatore and do:

		make
		
Now in module-acc/simulatore you should have the "or1ksim.cfg" configuration file, the "vimlinux" and "simulatore" executable and you can start the simulation, by executing 

		./simulatore
		
Once the linux has booted, you can interact whit it by the x-term console. In the init script the module is automatically inserted, so you can test the module launching the application in the x-term console:

		./app_prova1
		
This application open the device node in /dev/acc0 and fill the struct command_argument with the real parameters. In this case the struct is composed by four integer. Three data number and the return value. The device perform the sum of this three data number. To achieve this goal the application call a system funciotion with:

		result = ioctl(fileno(acc), ACC_OPERATION, &parameters);
		
This function use the ioctl operation implemented by the device driver, so the device driver is called. The function, once get the semaphore and check if the operation is correct (using the magic number), copies the data from user space to kernel space. Send the data to the device address (like we explained early) and after write the start command at the state address. It's important the order of the parameters in the declaration of the struct command_argument, because it define the order of the sent data.
After the statement that write the start command in the device memory, the function must wait that the device copute the result. It can do this by polling or by interrupt.

* in the case of interrupt, the function (and so the app) go to sleep until che interrupt handler wake up the module.
* in the of polling, the module go in a loop where it continously read the device's state. When it read the "DONE" state, it breaks the loop.

Then read the ruslt value and copy it form kernel space to user space, finally release the semaphore and return. If it's used the interrupt approach, before return it must reset the flag used by the interrupt controller.

Once the ioctl function ha returned the app check the correctness of the operation.

