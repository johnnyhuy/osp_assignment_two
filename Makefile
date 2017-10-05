MODULE = S3604367Device

ifneq ($(KERNELRELEASE),)
obj-m += $(MODULE).o
else
all:
	make -C /lib/modules/$(shell uname -r)/build M=$(shell pwd) modules_install
	$(CC) S3604367UserApplication.c -o S3604367UserApplication
clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(shell pwd) clean
load:
	sudo modprobe $(MODULE)
unload:
	sudo modprobe -r $(MODULE)
endif
