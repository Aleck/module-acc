################################## Parametrizzazione del makefile
MODULE_NAME = acc
ARCH = openrisc
CROSS_COMPILE = or32-linux-
KDIR := ~/progetti/linux
MODULE_FILE = acc.ko load_module_acc unload_module_acc
MODULE_DIRECTORY = modules
NAME_LOADER = load_module_acc
###################################

################################## Derivazioni dai parametri
PATH_INITRAMFS = $(KDIR)/arch/openrisc/support
PATH_MODULE_DIR = $(PATH_INITRAMFS)/initramfs/$(MODULE_DIRECTORY)
PATH_INITD  = $(PATH_INITRAMFS)/initramfs/etc/init.d
######################################



obj-m := $(MODULE_NAME).o

PWD := $(shell pwd)


default:
	$(MAKE) -C $(KDIR) SUBDIRS=$(PWD) modules


clean:
	@ make -C $(KDIR) M=$(PWD) clean


install: $(MODULE_NAME).ko
	@ mkdir -p $(PATH_MODULE_DIR)
	@ cp $(MODULE_FILE) $(PATH_MODULE_DIR)
	@ echo "INSTALL $(MODULE_NAME)"
	@ sed -i '/dir \/modules 755 0 0/d' $(PATH_INITRAMFS)/initramfs.devnodes
	@ echo "dir /modules 755 0 0" >> $(PATH_INITRAMFS)/initramfs.devnodes
	@ echo "UPDATING initramfs.devnodes"
	@ sed -i "/#module $(MODULE_NAME) loader /d" $(PATH_INITD)/rcS
	@ sed -i '/\/$(MODULE_DIRECTORY)\/$(NAME_LOADER)/d' $(PATH_INITD)/rcS
	@ echo "#module $(MODULE_NAME) loader " >> $(PATH_INITD)/rcS
	@ echo "/$(MODULE_DIRECTORY)/$(NAME_LOADER)" >> $(PATH_INITD)/rcS
	@ echo "UPDATING rcS"


uninstall: 
	@ $(foreach file, $(MODULE_FILE), rm $(PATH_MODULE_DIR)/$(file);)
	@ rmdir --ignore-fail-on-non-empty $(PATH_MODULE_DIR)
	@ echo "UNINSTALL $(MODULE_NAME)"
	@ test -d $(PATH_MODULE_DIR) || sed -i '/dir \/modules 755 0 0/d' $(PATH_INITRAMFS)/initramfs.devnodes
	@ test -d $(PATH_MODULE_DIR) || echo "UNDO initramfs.devnodes"
	@ sed -i "/#module $(MODULE_NAME) loader /d" $(PATH_INITD)/rcS
	@ sed -i '/\/$(MODULE_DIRECTORY)\/$(NAME_LOADER)/d' $(PATH_INITD)/rcS
	@ echo "UNDO rcS"