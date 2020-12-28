obj-m += hello_world.o

hello_world-objs := module_hello_init.o module_hello_exit.o

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
