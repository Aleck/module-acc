################################## Parametrizzazione del makefile
NAME_MODULE=acc
ARCH=openrisc
CROSS_COMPILE=or32-linux-
KDIR := ~/progetti/linux
MODULE_FILE = acc.ko load_module_acc unload_module_acc
###################################




obj-m := $(NAME_MODULE).o

PWD := $(shell pwd)


default:
	$(MAKE) -C $(KDIR) SUBDIRS=$(PWD) modules


clean:
	@ make -C $(KDIR) M=$(PWD) clean
	@ $(foreach file, $(MODULE_FILE), rm $(KDIR)/arch/openrisc/support/initramfs/module/$(file);)
	@ rmdir --ignore-fail-on-non-empty $(KDIR)/arch/openrisc/support/initramfs/module
	@ echo "CLEAN module $(NAME_MODULE)"

install: $(NAME_MODULE).ko
	@ mkdir -p $(KDIR)/arch/openrisc/support/initramfs/module
	@ cp $(MODULE_FILE) $(KDIR)/arch/openrisc/support/initramfs/module
	@ echo "INS $(NAME_MODULE)"