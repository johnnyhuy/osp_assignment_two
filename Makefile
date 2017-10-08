MODULE = S3604367Device
USER = johnny
GROUP = johnny

ifneq ($(KERNELRELEASE),)
obj-m += $(MODULE).o
else
all:
	make build
	make install
	$(CC) S3604367UserApplication.c -o S3604367UserApplication
build:
	make -C /lib/modules/$(shell uname -r)/build/ M=$(shell pwd) modules
install:
	make -C /lib/modules/$(shell uname -r)/build/ M=$(shell pwd) modules_install
clean:
	make -C /lib/modules/$(shell uname -r)/build/ M=$(shell pwd) clean
load:
	sudo modprobe $(MODULE)
	chmod 744 /dev/$(MODULE)
	chown -R $(USER):$(GROUP) /dev/$(MODULE)
unload:
	sudo modprobe -r $(MODULE)
restart:
	make unload
	make load
endif
