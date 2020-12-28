#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/syscalls.h>
#include <linux/highmem.h>
#include <linux/fs.h>
#include <linux/sched.h>
#include <linux/moduleparam.h>
#include <linux/unistd.h>
#include <linux/fdtable.h>

#include <asm/pgtable_types.h>
#include <asm/cacheflush.h>
#include <asm/unistd.h>
#include <asm/cacheflush.h>

void **SYS_CALL_TABLE;		/*sys_call_table address*/

asmlinkage long (*original_syscallopen) (const char __user *, int, umode_t);
asmlinkage long (*original_syscallwrite) (unsigned int, const char __user *, size_t);

asmlinkage long hook_open(const char __user *filename, int flags, umode_t mode){
	printk(KERN_INFO "[HOOK] OPEN HOOKED HERE.\n");
	printk(KERN_INFO "[OPEN HOOK] File: %s\n", filename);
	return original_syscallopen(filename, flags, mode);
}


asmlinkage long hook_write(unsigned int fildes, const char __user *buffer, size_t count){
	printk(KERN_INFO "[HOOK] WRITE HOOKED HERE\n");
	printk(KERN_INFO "[WRITE HOOK] File descriptor : %u\n", fildes);
	printk(KERN_INFO "[WRITE HOOK] Bytes: %d\n", count);
	return original_syscallwrite(fildes, buffer, count);
}


// Make writeable 
int make_rw(unsigned long address) {
	unsigned int level;
	pte_t *pte = lookup_address(address, &level);
	if (pte->pte &~_PAGE_RW) {
		pte->pte |= _PAGE_RW;
	}
	return 0;
}

// Make write protected 
int make_ro(unsigned long address) {
	unsigned int level;
	pte_t *pte = lookup_address(address, &level);
	pte->pte = pte->pte &~_PAGE_RW;
	return 0;
}

static int __init entry_point(void){
	// Gets Syscall Table **
	printk(KERN_INFO "[HOOK] Captain Hook loaded successfully..\n");
 	SYS_CALL_TABLE = (void*)kallsyms_lookup_name("sys_call_table");
	printk(KERN_INFO "[HOOK] Hooks Will Be Set.\n");
	printk(KERN_INFO "[HOOK] System call table at %p\n", SYS_CALL_TABLE);

	/* Replace custom syscall with the correct system call name (write,open,etc) to hook*/
	original_syscallopen = SYS_CALL_TABLE[__NR_open];
	original_syscallwrite = SYS_CALL_TABLE[__NR_write];

	printk(KERN_INFO "[HOOK] Disable page protection success.\n");
	
	// Replaces Pointer Of Syscall_read on our syscall.
	make_rw((unsigned long)SYS_CALL_TABLE);
	SYS_CALL_TABLE[__NR_open] = hook_open;
	SYS_CALL_TABLE[__NR_write] = hook_write;

	
	printk(KERN_INFO "[HOOK] Overriding syscall open success.\n");
	printk(KERN_INFO "[HOOK] Overriding syscall write success.\n");
	printk(KERN_INFO "[HOOK] Overriding syscall read success.\n");

	return 0;
}

static void __exit exit_point(void){
	// Clean up our Hooks
	printk(KERN_INFO "[HOOK] Unloaded Captain Hook successfully.\n");

	/*Restore original system call */

	SYS_CALL_TABLE[__NR_open] = original_syscallopen;
	SYS_CALL_TABLE[__NR_write] = original_syscallwrite;

	make_ro((unsigned long)SYS_CALL_TABLE);

	printk(KERN_INFO "[HOOK] Restore syscall open success.\n");
	printk(KERN_INFO "[HOOK] Restore syscall write success.\n");
	printk(KERN_INFO "[HOOK] Restore syscall read success.\n");
	printk(KERN_INFO "[HOOK] Enable page protection success.\n");
}

module_init(entry_point);
module_exit(exit_point);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Nhut-Nam Le");
MODULE_DESCRIPTION("Operating System Project 03");
MODULE_VERSION("1.0.0");

