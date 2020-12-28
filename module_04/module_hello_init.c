#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>

#define DRIVER_AUTHOR "Nhut-Nam Le <lenam.fithcmus@gmail.com>"
#define DRIVER_DESC "A simple loadable kernel module"

static int __init init_my_module(void){
	printk(KERN_INFO "NHUTNAM_DEBUG: HELLO WORLD! \n");
	return 0; 
}

MODULE_LICENSE("GPL");
MODULE_AUTHOR(DRIVER_AUTHOR);
MODULE_DESCRIPTION(DRIVER_DESC);
module_init(init_my_module);
