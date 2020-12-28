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



void EnablePageWriting(void){
    write_cr0(read_cr0() & (~0x10000));

} 
void DisablePageWriting(void){
    write_cr0(read_cr0() | 0x10000);

} 


//define our origional function. 
asmlinkage int ( *original_open ) (int dirfd, const char *pathname, int flags); 


//Create Our version of Open Function. 
asmlinkage int  HookOpen(int dirfd, const char *pathname, int flags){

char letter ;
int i = 0;

char directory[255];
char OurFile[14] = "breakpoints"; 


while (letter != 0 || i < 6){ // if (letter == 0x41 || letter < 0x7a) Maybe to prevent bad chars from entering string buffer
    //This macro copies a single simple variable from user space to kernel space. 
    //So this will copy pathname[i] to ch;
    get_user(letter, pathname+i);
    directory[i] = letter ;
    i++;
    }

    if (strcmp(OurFile , directory ) == 0 ){
        printk(KERN_INFO "File Accessed!!! %s", directory);
    }
    memset(directory, 0, 255);


    // Jump to origional OpenAt()
    return (*original_open)(dirfd, pathname, flags);
}

// Set up hooks.
static int __init SetHooks(void) {
    // Gets Syscall Table **
    SYS_CALL_TABLE = (unsigned long**)kallsyms_lookup_name("sys_call_table"); 

    printk(KERN_INFO "Hooks Will Be Set.\n");
    printk(KERN_INFO "System call table at %p\n", SYS_CALL_TABLE);

  // Opens the memory pages to be written
    EnablePageWriting();

  // Replaces Pointer Of Syscall_open on our syscall.
    original_open = (void*)SYS_CALL_TABLE[__NR_openat];
    SYS_CALL_TABLE[__NR_openat] = (unsigned long*)HookOpen;
    DisablePageWriting();

    return 0;
}

static void __exit HookCleanup(void) {

    // Clean up our Hooks
    EnablePageWriting();
    SYS_CALL_TABLE[__NR_openat] = (unsigned long*)original_open;
    DisablePageWriting();

    printk(KERN_INFO "HooksCleaned Up!");
}

module_init(SetHooks);
module_exit(HookCleanup);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("NhutNam-Le");
MODULE_DESCRIPTION("Simple Hooking");
MODULE_VERSION("1.0");
