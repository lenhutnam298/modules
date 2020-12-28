#include <linux/init.h>
#include <linux/module.h>
#include <linux/syscalls.h>
#include <linux/kallsyms.h>
#include <linux/slab.h>
#include <linux/kern_levels.h>
#include <linux/gfp.h>
#include <asm/unistd.h>
#include <asm/paravirt.h>

unsigned long **SYS_CALL_TABLE;

void enable_page_writing(void){
	write_cr0(read_cr0() & (~0x10000));

} 
void disable_page_writing(void){
	write_cr0(read_cr0() | 0x10000);

} 

asmlinkage int (*original_read)(unsigned int, void __user*, size_t);
asmlinkage int  HookRead(unsigned int fd, void __user* buf, size_t count) {
	printk(KERN_INFO "READ HOOKED HERE! -- This is our function!"); 
	return (*original_read)(fd, buf, count);
}


static int __init init_hook(void) {
	// Gets Syscall Table **
 	SYS_CALL_TABLE = (unsigned long**)kallsyms_lookup_name("sys_call_table"); 
	printk(KERN_INFO "Hooks Will Be Set.\n");
	printk(KERN_INFO "System call table at %p\n", SYS_CALL_TABLE);
	enable_page_writing();

    	// Replaces Pointer Of Syscall_read on our syscall.
	original_read = (void*)SYS_CALL_TABLE[__NR_read];
	SYS_CALL_TABLE[__NR_read] = (unsigned long*)HookRead;
	disable_page_writing();
	return 0;
}


static void __exit clean_hook(void) {
	// Clean up our Hooks
	enable_page_writing();
	SYS_CALL_TABLE[__NR_read] = (unsigned long*)original_read;
	disable_page_writing();

	printk(KERN_INFO "Hooks cleaned");
}

module_init(init_hook);
module_exit(clean_hook);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("NhutNam-Le");
MODULE_DESCRIPTION("Simple Hooking");
MODULE_VERSION("1.0");
