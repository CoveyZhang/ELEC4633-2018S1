KDIR		:= /lib/modules/$(shell uname -r)/build
PWD		:= $(shell pwd)
EXTRA_CFLAGS	:= $(shell rtai-config --module-cflags) -I/usr/include/
obj-m		:= template.o # change this for other kernel file compilation

USER_CC		:= gcc
USER_CFLAGS	:= -Wall $(shell rtai-config --lxrt-cflags)

.PHONY:	default clean watch

default:
	$(MAKE) -C $(KDIR) M=$(PWD) modules

clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean
	rm -rf *.o *~

watch:
	watch -n 1 "dmesg | tail -n 20"

.c:
	$(USER_CC) $(USER_CFLAGS) $< -o $@
