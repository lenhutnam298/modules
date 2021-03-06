#ifndef _DEV_H
#define _DEV_H

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/moduleparam.h>
#include <linux/stat.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/string.h>

#define DRIVER_AUTHOR "Nhut-Nam Le <lenam.fithcmus@gmail.com>"
#define DRIVER_DESC "A simple loadable kernel module"

int init_module(void);
int release_module(void);
int device_open(struct inode * inode, struct file *file);
int device_release(struct inode  * inode, struct file *file);
ssize_t device_read(struct file  * file,  char * buffer, size_t length, loff_t *offset);
ssize_t device_write(struct file * file , const char * buffer, size_t length, loff_t *offset);

#define SUCCESS 0 
#define FAILURE 1

#define DEVICE_NAME "nhutnam" 
#define BUF_LEN 80

extern int Major;

static struct file_operations fops = {
    .read    = device_read,
    .write   = device_write,
    .open    = device_open,
    .release = device_release
};

MODULE_LICENSE("GPL");
MODULE_AUTHOR(DRIVER_AUTHOR);
MODULE_DESCRIPTION(DRIVER_DESC);

#endif				/* _DEV_H */
