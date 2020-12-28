#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/syscalls.h>
#include <linux/highmem.h>
#include <linux/fs.h>
#include <linux/sched.h>
#include <linux/moduleparam.h>
#include <linux/unistd.h>
#include <linux/kallsyms.h>
#include <linux/fcntl.h>
#include <linux/mm_types.h>
#include <linux/slab.h>
#include <linux/kern_levels.h>
#include <linux/gfp.h>

#include <asm/cacheflush.h>
#include <asm/unistd.h>
#include <asm/pgtable_types.h>

void **SYS_CALL_TABLE;

asmlinkage long(*original_syscallopen_hook) (const char __user *, int, int);
asmlinkage long(*original_syscallwrite_hook) (unsigned int, const char __user *, int);

asmlinkage long hook_write(unsigned int fildes, const char __user *buffer, size_t count){
	printk(KERN_INFO "[HOOK] WRITE HOOKED HERE \n");
	printk(KERN_INFO "[WRITE HOOK] File descriptor : %u\n", fildes);
	printk(KERN_INFO "[WRITE HOOK] Bytes: %d\n", count);
	return (*original_syscallwrite_hook)(fildes, buffer, count);
}

asmlinkage long hook_open(const char __user *filename, int flags, mode_t mode){
	printk(KERN_INFO "[HOOK] OPEN HOOKED HERE.\n");
	printk(KERN_INFO "[OPEN HOOK] File: %s\n", filename);
	return (*original_syscallopen_hook)(filename, flags, mode);
}

/*Make page writeable*/
int make_rw(unsigned long  address){
    unsigned int level;
    pte_t *pte = lookup_address(address, &level);
    if(pte->pte &~_PAGE_RW){
        pte->pte |=_PAGE_RW;
    }
    return 0;
}
/* Make the page write protected */
int make_ro(unsigned long address){
    unsigned int level;
    pte_t *pte = lookup_address(address, &level);
    pte->pte = pte->pte &~_PAGE_RW;
    return 0;
}

static int __init entry_point(void){
	// Gets Syscall Table **
	printk(KERN_INFO "[HOOK] Captain Hook loaded successfully..\n");
 	SYS_CALL_TABLE = (void**)kallsyms_lookup_name("sys_call_table"); 
	if (!SYS_CALL_TABLE) {
    		printk(KERN_INFO "[HOOK] Cannot find the system call address\n"); 
    		return -1;
  	}
	printk(KERN_INFO "[HOOK] Hooks Will Be Set.\n");
	printk(KERN_INFO "[HOOK] System call table at %p\n", SYS_CALL_TABLE);
	/* Replace custom syscall with the correct system call name (write,open,etc) to hook*/
	original_syscallopen_hook = (void*)SYS_CALL_TABLE[__NR_open];
	original_syscallwrite_hook = (void*)SYS_CALL_TABLE[__NR_write];

	/*Disable page protection*/
    	make_rw((unsigned long)SYS_CALL_TABLE);
	printk(KERN_INFO "[HOOK] Disable page protection success.\n");
	
	// Replaces Pointer Of Syscall_read on our syscall.
	SYS_CALL_TABLE[__NR_open] = (unsigned long*)hook_open;
	SYS_CALL_TABLE[__NR_write] = (unsigned long*)hook_write;
	printk(KERN_INFO "[HOOK] Overriding syscall open success.\n");
	printk(KERN_INFO "[HOOK] Overriding syscall write success.\n");
	return 0;
}

static void __exit exit_point(void){
	// Clean up our Hooks
	printk(KERN_INFO "[HOOK] Unloaded Captain Hook successfully\n");
	/*Restore original system call */
	SYS_CALL_TABLE[__NR_open] = (unsigned long*)original_syscallopen_hook;
	SYS_CALL_TABLE[__NR_write] = (unsigned long*)original_syscallwrite_hook;
	/*Renable page protection*/
    	make_ro((unsigned long)SYS_CALL_TABLE);
}

module_init(entry_point);
module_exit(exit_point);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("NhutNam-Le");
MODULE_DESCRIPTION("Simple Hooking");
MODULE_VERSION("1.0");

