module-acc
==========

#### File organization ####

In this repository we have structurated the source file in this way:

 -module -> this folder contains the source files of the device driver
 -app -> this folder contains the source files of the toy app that test the driver
 -simulatore -> this folder contains the source files of the stub used to simulate the accelerator



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

Once you have installed the application and the module, build the kernel to commit the added files in the linux executable. The stub acts in the like the standalone simulator, except the fact that if no paramters is passed it assume that the config file and the linux exutable are in the working directory.

We use a modified configuration file, which is based on the generic file found in linux/arch/openrisc plus a geniric section that rapresent the physical device:

section generic
  enabled = 1
  baseaddr = 0xa0000000
  size = 0x00000014
  name = "acc"
  byte_enabled = 1
  hw_enabled = 1
  word_enabled = 1 
end

and we have set a xterm console for interacting with the simulated os in the uart section.

If you have copied the "vmlinux" exutables in the "simulatore" directory you can simulate everything by run "./simulatore", a xterm console should appear and from that you can launch the toy application and see if it works.



### INTRODUCTION TO THE DEVICE DRIVER ###

A character driver is suitable for most simple hardware device. Char devices are accessed through names in the filesystem. Those name are called special files or device files or simply nodes of the filesystem tree; they are conventionally located in the /dev directory. If yuo issue the ls -l command. you'll see two numbers in the device. These numbers are the major and the minor device number for the particular device. The major number identifies the driver associated with the device. The minor number is used by the kernel to determine exatcly wich device is being referred to. You can either get a direct pointer to your device from the kernel, or you can use the minor number yourself as an index into a local array of the device. Either way, the kernel itself knows almost nothing about minor number beyond the fact that they refer to devices implemented by your driver. 
You can get these number dynamically or you can specify them manually. We have choose to set them dynamically, becuase in this way the numbers are handled by the kernel itself.


### INITIALIZATION AND CLEANUP ###
We start talking about the function that perform the initialization of the module, called by the insmod command. The statement that actually register the device and then request the numbers is in the last part of that function, because, when we register the device, the kernel can use the module immediatly, so everything (like the space in the I/O memory or the request of the interrupt line...) must be correctly initialized.

As you can see in the acc.c file, in the init function you have to:

1) Take care of the I/O memory region with the instruction:
	my_device_region = request_mem_region(base_address, size_address, name);
The function request_mem_region needs the physical address and the size of the device and its (arbitrary) name. This function reserves the I/O memory of the device. If the result of this function is NULL, the kernel can't get the I/O memory address, so you have to check this fact.
You can check if it worked by typing "cat /proc/iomem"
	   
	2) Then you have to remap the real physical address into a virtual one that can be used, with the instruction:
		device_virtual_address = ioremap(base_address, size_address);
	   this function return a pointer of type __iomem*, which is a void* pointer but is stressed the fact is a pointer to an I/O 		   memory. This should works fine but in our case, we have appended the following code 
		EXPORT_SYMBOL(__ioremap);
		EXPORT_SYMBOL(iounmap);
	   in the file or32_ksyms.c that is located at linux/arch/openrisc/kernel, because the token ioremap (and iounmap) isn't 		   declared.
	   
	3) If there is an interrupt handler in your device you have to initialize here the interrupt with the function
		result = request_irq(irq, (irq_handler_t) acc_handler, IRQF_SHARED, name, (void*)(acc_handler));
	   The function request_irq, reserve the physical line specified by the variable irq to our device driver. The parameter 		   acc_handler is the function of the interrupt handler also define in this file. IRQF_SHARED is a flags that distingues the 		   type of the interrupt line.
	   You can check if it worked by typing "cat /proc/interrupts"
	   
	4) Then you have to allocate the space for the device parameters with the function:
		kernel_argument = kmalloc(sizeof(struct command_argument), GFP_KERNEL);
	   where kernerl_argument is a global variable declared of type of the struct that you define as the parameter structure, that 		   in our case is command_argumet. In this variable we store all the parameters that the device needs (even the return value)
	   
	5) If you need a samphore for the lock of the device, you need to initalize it here with the function:
		sema_init(&kernel_argument_semaphore, sem_max_access);
	   To takle the concurrency problem we use a global semaphore, wich actually is a mutex in our case. Further explanation on how it work is in the module operation section.
	   
	6) Finally you have to register your device with the instruction 
		device_number = register_chrdev(major, name, &acc_fops);
	   The variable major is the major number set to 0 (this means that we set the minor and the major number dynamically), name is 	   the device name and acc_fops is the file of the operation and is a global variable declared of type struct file_operations. 		   This struct countains all the operations of the device node that we will create with the load script.
	   From this point on the kernel can call one function that we have specifies in acc_fops.
	7) Then we have to get the minor and major number with the instruction:
		major = MAJOR(device_number);
		minor = MINOR(device_number);
The acc_cleanup_module function undone all the initializations that we have done.
