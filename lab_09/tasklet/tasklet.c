#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/interrupt.h>
#include <linux/time.h>
#include <linux/proc_fs.h>
#include <linux/sched.h>
#include <linux/seq_file.h>

MODULE_LICENSE("GPL");

#define KEYBOARDIRQ 1
#define FORTUNE_NAME "queue_fort"

struct tasklet_struct my_tasklet;

// --------------- SEQ FILE
/* */
#define COOKIE_POT_SIZE PAGE_SIZE

static int my_proc_show(struct seq_file *m, void *v)
{
  seq_printf(m, " + PROC SHOW: state - %d, count - %d, data - %ld\n", my_tasklet.state, my_tasklet.count, my_tasklet.data);

  return 0;
}

static int my_proc_open(struct inode *inode, struct file *file)
{
  return single_open(file, my_proc_show, NULL);
}

static struct proc_ops fortune_proc_ops = {
    .proc_open = my_proc_open,
    .proc_release = single_release,
    .proc_read = seq_read,
};

// ------------- END SEQ FILE

void my_tasklet_function(unsigned long data)
{
  printk(KERN_INFO "Tasklet FUNCTION: state - %d, count - %d, data - %ld\n",
         my_tasklet.state, my_tasklet.count, my_tasklet.data);
}

irqreturn_t irq_handler(int irq, void *dev, struct pt_regs *regs)
{
  if (irq == KEYBOARDIRQ)
  {
    tasklet_schedule(&my_tasklet);
    printk(KERN_INFO "KEYBOARD tasklet was sheduled!\n");

    return IRQ_HANDLED;
  }
  else
    return IRQ_NONE;
}

static int __init my_module_init(void)
{
  printk(KERN_DEBUG "MODULE load initial!\n");

  int ret = request_irq(KEYBOARDIRQ, (irq_handler_t)irq_handler, IRQF_SHARED,
                        "my_irq_handler_tasklet", (void *)(irq_handler));

  if (ret != 0)
  {
    printk(KERN_ERR "KEYBOARD IRQ handler wasn't registered");
    return ret;
  }

  printk(KERN_INFO "KEYBOARD IRQ handler was registered successfully");

  tasklet_init(&my_tasklet, my_tasklet_function, (void *)(irq_handler));

  // INIT SEQFILE
  struct proc_dir_entry *entry;
  entry = proc_create(FORTUNE_NAME, S_IRUGO | S_IWUGO, NULL, &fortune_proc_ops); // S_IRUGO | S_IWUGO == 0666
  if (!entry)
  {
    printk(KERN_INFO "Error: can't create fortune file\n");
    return -ENOMEM;
  }
  // END INIT SEQFILE

  return ret;
}

static void __exit my_module_exit(void)
{
  free_irq(KEYBOARDIRQ, (void *)(irq_handler));

  tasklet_disable(&my_tasklet);
  tasklet_kill(&my_tasklet);

  remove_proc_entry(FORTUNE_NAME, NULL);

  printk(KERN_DEBUG "MODULE unloaded!\n");
}

module_init(my_module_init);
module_exit(my_module_exit);