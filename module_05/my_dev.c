#include "functs.h"

int init_module(void){
	Major = register_chrdev(0, DEVICE_NAME, &fops);
	if (Major < 0){
    		printk(KERN_ALERT "I have failed to load!\n");
    		return Major;
	}

	printk(KERN_ALERT "I was assigned major number %d\n", Major);
	printk(KERN_ALERT "Please create device with name  \n mknod /dev/%s c %d 0 \n",DEVICE_NAME, Major);
	return 0;
}

int release_module(void){
	unregister_chrdev(Major, DEVICE_NAME);
    	printk(KERN_INFO "ROOTKITDEV_DEBUG : Driver Unloaded! \n");
	return 0;
}
