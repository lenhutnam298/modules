ccflags-y = -std=gnu99
obj-m += hooksyscall.o
hooksyscall-objs := main.o  

all:
	make -C  /lib/modules/$(shell uname -r)/build M=$(PWD) modules 

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean

