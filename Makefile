obj-m := acc.o

KDIR := /lib/modules/$(shell uname -r)/build
PWD := $(shell pwd)

ccflags-y := -I$(src)

default:
	$(MAKE) -C $(KDIR) SUBDIRS=$(PWD) modules


clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean