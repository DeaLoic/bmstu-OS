#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/proc_fs.h>
#include <linux/string.h>
#include <linux/vmalloc.h>
#include <linux/fs.h>
#include <linux/errno.h>
#include <asm/uaccess.h>

MODULE_LICENSE("GPL");

static struct proc_dir_entry *proc_entry;
static char* str;
int write_index, read_index; 
#define COOKIE_POT_SIZE PAGE_SIZE

ssize_t fortune_write(struct file *file, const char __user *buffer, size_t count, loff_t *offp)
{
    printk(KERN_INFO "+ FORT BUF: call fortune_write\n");
    if (copy_from_user(str + write_index, buffer, count))
        return -EFAULT;

    write_index += count;
    if (write_index >= COOKIE_POT_SIZE) {
        write_index = 0;
        str[write_index] = '\0';
    }
    return count;
}

ssize_t fortune_read(struct file *file, char __user *buffer, size_t count, loff_t *offp)
{
    printk(KERN_INFO "+ FORT BUF: call fortune_read\n");

    if (*offp > 0)
        return 0;
    
    copy_to_user(buffer, str + read_index, count);
    *offp += count;

    read_index = write_index;

    return count;
}

static int fortune_open(struct inode *inode,struct file *file)
{
	printk(KERN_INFO "+ FORT BUF: call fortune_open\n");
	return 0;
}

static int fortune_release(struct inode *inode, struct file *file)
{
	printk(KERN_INFO "+ FORT BUF: call fortune_release\n");
	return 0;
}

static struct proc_ops fort_proc_ops={
//	.proc_owner = THIS_MODULE,

	.proc_open = fortune_open,
	.proc_release = fortune_release,

	.proc_read = fortune_read,
	.proc_write = fortune_write
};

int fortune_module_init(void)
{
    str = (char *)vmalloc(COOKIE_POT_SIZE);
    if (!str)
    {
        printk(KERN_INFO "ERROR: No memory for create str\n");
        return -ENOMEM;
    }
    memset(str, 0, COOKIE_POT_SIZE);

    proc_entry = proc_create("fortbuf", 0666, NULL, &fort_proc_ops);
    if (proc_entry == NULL)
    {
        vfree(str);
        printk(KERN_INFO "ERROR: Couldn't create proc entry\n");
        return -ENOMEM;
    }

    struct proc_dir_entry *dir = proc_mkdir("fortbuf_dir", NULL);
    struct proc_dir_entry *symlink = proc_symlink("fortbuf_symlink", NULL, "/proc/fortbuf_dir");
    if ((dir == NULL) || (symlink == NULL))
    {
        vfree(str);
        printk(KERN_INFO "ERROR: Couldn't create proc dir, symlink\n");
        return -ENOMEM;
    }

    write_index = 0;
    read_index = 0;

    printk(KERN_INFO "+ FORT BUF: Module loaded successfully.\n");
    return 0;
}

void fortune_module_exit(void)
{
    remove_proc_entry("fortbuf", NULL);
    remove_proc_entry("fortbuf_symlink", NULL);
    remove_proc_entry("fortbuf_dir", NULL);
    
    if (str)
        vfree(str);

    printk(KERN_INFO "+ FORT BUF: Module unloaded.\n");
}

module_init(fortune_module_init);
module_exit(fortune_module_exit);