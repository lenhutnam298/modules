#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/moduleparam.h>
#include <linux/stat.h>

#define DRIVER_AUTHOR "Nhut-Nam Le <lenam.fithcmus@gmail.com>"
#define DRIVER_DESC "A simple loadable kernel module"

static char *my_msg = "";
module_param(my_msg, charp, 0000);
MODULE_PARM_DESC(my_msg, "Hello Linux, I'm a newbie here.");


static int __init init_my_module(void){
	printk(KERN_INFO "NHUTNAM_DEBUG: HELLO WORLD!, %s\n", my_msg);
	return 0; 
}
 

static void __exit exit_my_module(void){
	printk(KERN_INFO "NHUTNAM_DEBUG : GOODBYE WORLD \n");
}


module_init(init_my_module);
module_exit(exit_my_module);

MODULE_LICENSE("GPL");
MODULE_AUTHOR(DRIVER_AUTHOR);
MODULE_DESCRIPTION(DRIVER_DESC);
MODULE_SUPPORTED_DEVICE("testdevice");
