#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/types.h>
#include <linux/kdev_t.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Artyom Lazyan");
MODULE_DESCRIPTION("ReverseCharDriver");

static char *str = "Hello world";
module_param(str, charp, 0);
MODULE_PARM_DESC(str, "Input string you want to reverse: ");

static int major = 0;
module_param(major, int, 0);
MODULE_PARM_DESC(major, "Major number default = 0: ");

static dev_t ch_dev = 0;
static const int minor = 0;
static const char devname[] = "reversech";

static int __init reversech_init(void)
{
    int err = 0;

    pr_info("input string: %s\n", str);

    if (major != 0)
    {
        ch_dev = MKDEV(major, minor);
        err = register_chrdev_region(ch_dev, 1, devname);
    }
    else
    {
        err = alloc_chrdev_region(&ch_dev, minor, 1, devname);
    }

    if (err)
    {
        pr_err("Error registering device\n");
        return -1;
    }

    return 0;
}

static void __exit reversech_exit(void)
{
    pr_info("Cleaning up module.\n");
    unregister_chrdev_region(ch_dev, 1);
}

// file operations
static int ch_open(struct inode *inode, struct file *file)
{
    return 0;
}

static int ch_release(struct inode *inode, struct file *file)
{
    return 0;
}

static ssize_t ch_read(struct file *filp, char __user *buffer, size_t length, loff_t *offset)
{
    return 0;
}

static struct file_operations reversech_fops = {
    .owner = THIS_MODULE,
    .open = &ch_open,
    .release = &ch_release,
    .read = &ch_read,
};

module_init(reversech_init);
module_exit(reversech_exit);
