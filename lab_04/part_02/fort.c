#include<linux/module.h>
#include<linux/init.h>
#include<linux/proc_fs.h>
#include<linux/sched.h>
#include<linux/uaccess.h>
#include<linux/fs.h>
#include<linux/seq_file.h>
#include<linux/slab.h>
#include<linux/vmalloc.h>

MODULE_LICENSE("GPL");

static char *str = NULL;
unsigned int write_index;
unsigned int read_index;

#define COOKIE_POT_SIZE PAGE_SIZE

static int my_proc_show(struct seq_file *m, void *v)
{
	printk(KERN_INFO "+ FORT SEQ! call my_proc_show\n");

	int len = strlen(str + read_index);
	if (len)
		seq_printf(m, "Hello! Index is %u, message is: %s\n", read_index, str + read_index);
	else
		seq_printf(m, "Hello! Index is %u, message is EMPTY!!!\n", read_index);

	if (len)
		read_index += len + 1;
	return 0;
}


static int my_proc_open(struct inode *inode,struct file *file)
{
	printk(KERN_INFO "+ FORT SEQ! call my_proc_open\n");
	return single_open(file,my_proc_show,NULL);
}

static ssize_t my_proc_read(struct file* file, char *buffer,size_t count, loff_t *f_pos)
{
	printk(KERN_INFO "+ FORT SEQ! call my_proc_read\n");
	return seq_read(file,buffer,count, f_pos);
}

static int my_proc_release(struct inode *inode, struct file *file)
{
	printk(KERN_INFO "+ FORT SEQ! call my_proc_release\n");
	return single_release(inode, file);
}

static struct proc_ops fortune_proc_ops={
	//.proc_owner = THIS_MODULE,
	.proc_open = my_proc_open,
	.proc_release = my_proc_release,
	.proc_read = my_proc_read,
	.proc_lseek = seq_lseek,
	.proc_write = my_proc_write
};

static int __init fortune_init(void)
{
	printk(KERN_INFO "+ FORT SEQ! call fortune init\n");

	write_index = 0;
	read_index = 0;

	// выделить память для строки 
    str = vmalloc(COOKIE_POT_SIZE);
    if (!str)
    {
        printk(KERN_INFO "Error: can't malloc cookie buffer\n");
        return -ENOMEM;
    }
    memset(str, 0, COOKIE_POT_SIZE);

	struct proc_dir_entry *entry;
	entry = proc_create("fort", S_IRUGO | S_IWUGO, NULL, &fortune_proc_ops); // S_IRUGO | S_IWUGO == 0666
	if(!entry)
	{
		vfree(str);
		printk(KERN_INFO "Error: can't create fortune file\n");
        return -ENOMEM;
	}	

    // создать диалог в файловой системе /proc 
    struct proc_dir_entry *dir = proc_mkdir("cookie_dir", NULL);

    // создать символическую ссылку на "/proc/fortune"
    struct proc_dir_entry *symlink = proc_symlink("cookie_symlink", NULL, "/proc/cookie_dir");

	if ((dir == NULL) || (symlink == NULL))
    {
        vfree(str);
        printk(KERN_INFO "fortune: Couldn't create proc dir, symlink\n");
        return -ENOMEM;
    }

	printk(KERN_INFO "Fortune module loaded successfully\n");
	return 0;
}

static void __exit fortune_exit(void)
{
	printk(KERN_INFO "+ FORT SEQ! call fortune exit\n");

	remove_proc_entry("fort", NULL);
	remove_proc_entry("cookie_dir", NULL);
	remove_proc_entry("cookie_symlink", NULL);

    if (str)
        vfree(str);

    printk(KERN_INFO "Fortune module unloaded\n");
}

module_init(fortune_init);
module_exit(fortune_exit);