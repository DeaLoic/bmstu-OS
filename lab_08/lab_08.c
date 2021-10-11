#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/time.h>
#include <linux/slab.h>
#include <linux/version.h>

#define VFS_MAGIC_NUMBER 0x1111111
#define SLABNAME "vfs_cache"

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("MYVFS");

static int size = 7;
static int number = 1;
static int sco = 0;

static void **line = NULL;

static void co(void* p)
{
    *(int*)p = (int)p;
    sco++;
}

struct kmem_cache *cache = NULL;

static struct vfs_inode
{
     int i_mode;
     unsigned long i_ino;
} vfs_inode;

static void vfs_put_super(struct super_block * sb)
{
    printk(KERN_DEBUG "+ MYVFS super block destroyed!\n");
}

static struct super_operations const vfs_super_ops = {
    .put_super = vfs_put_super,
    .statfs = simple_statfs,
    .drop_inode = generic_delete_inode,
};

static struct inode *vfs_make_inode(struct super_block *sb, int mode)
{
    struct inode *ret = new_inode(sb);

    if (ret)
    {
        inode_init_owner(ret, NULL, mode);
        ret->i_size = PAGE_SIZE;
        ret->i_atime = ret->i_mtime = ret->i_ctime = current_time(ret);
        ret->i_private = &vfs_inode;
    }

    return ret;
}

static int vfs_fill_sb(struct super_block *sb, void *data, int silent)
{
    sb->s_blocksize = PAGE_SIZE;
    sb->s_blocksize_bits = PAGE_SHIFT;
    sb->s_magic = VFS_MAGIC_NUMBER;
    sb->s_op = &vfs_super_ops;

    struct inode* root = NULL;
    root = vfs_make_inode(sb, S_IFDIR | 0755);
    if (!root)
    {
        printk (KERN_ERR "+ MYVFS Error in inode allocation\n");
        return -ENOMEM;
    }

    root->i_op  = &simple_dir_inode_operations;
    root->i_fop = &simple_dir_operations;

    sb->s_root = d_make_root(root);
    if (!sb->s_root)
    {
        printk(KERN_ERR "+ MYVFS root creation failed!\n");
        return -ENOMEM;
    }

    return 0;
}

static struct dentry* vfs_mount(struct file_system_type *type, int flags, char const *dev, void *data)
{
    struct dentry* const entry = mount_nodev(type, flags, data, vfs_fill_sb);

    if (IS_ERR(entry))
        printk(KERN_ERR "MYVFS mounting failed!\n");
    else
        printk(KERN_DEBUG "MYVFS mounted!\n");

    return entry;
}

static struct file_system_type vfs_type = {
    .owner = THIS_MODULE,           
    .name = "MYVFS",                
    .mount = vfs_mount,             
    .kill_sb = kill_litter_super,   
};

static int __init vfs_init(void)
{
    int i;

    line = kmalloc(sizeof(void*), GFP_KERNEL);

    if (line == NULL)
    {
        printk(KERN_ERR "MYVFS kmalloc error\n" );
        kfree(line);
        return -ENOMEM;
    }

        line[0] = NULL;

    cache = kmem_cache_create(SLABNAME, size, 0, SLAB_HWCACHE_ALIGN, co);

    if (cache == NULL)
    {
        printk(KERN_ERR "MYVFS kmem_cache_create error\n" );
        kmem_cache_destroy(cache);
        kfree(line);
        return -ENOMEM;
    }

    if (NULL == (line[0] = kmem_cache_alloc(cache, GFP_KERNEL))) 
    {
        printk(KERN_ERR "MYVFS kmem_cache_alloc error\n");

        kmem_cache_free(cache, line[0]);

        kmem_cache_destroy(cache);
        kfree(line);
        return -ENOMEM;
    }

    printk(KERN_INFO "MYVFS allocate %d objects into slab: %s\n", number, SLABNAME);
    printk(KERN_INFO "MYVFS object size %d bytes, full size %ld bytes\n", size, (long)size * number);
    printk(KERN_INFO "MYVFS constructor called %d times\n", sco);

    int ret = register_filesystem(&vfs_type);

    if (ret!= 0)
    {
        printk(KERN_ERR "MYVFS module cannot register filesystem!\n");
        return ret;
    }

    printk(KERN_DEBUG "MYVFS module loaded!\n");
    return 0;
}

static void __exit vfs_exit(void)
{
    kmem_cache_free(cache, line[0]);

    kmem_cache_destroy(cache);
    kfree(line);

    int ret = unregister_filesystem(&vfs_type);

    if (ret!= 0)
        printk(KERN_ERR "MYVFS module cannot unregister filesystem!\n");

    printk(KERN_DEBUG "MYVFS module unloaded! Get %d requested\n", sco);
}

module_init(vfs_init);
module_exit(vfs_exit);