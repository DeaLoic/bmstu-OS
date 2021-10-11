#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/interrupt.h>
#include <linux/workqueue.h>
#include <linux/slab.h>
#include <linux/rtmutex.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>


MODULE_LICENSE("GPL");

#define KEYBOARDIRQ 1
#define FORTUNE_NAME "queue_fort"

char my_workqueue_data[] = "KEYBOARD IRQ";
// Очередь работ
static struct workqueue_struct *my_wq;

typedef struct
{
  struct work_struct work;
  int number;
} my_work_t;

static my_work_t *work1;
static my_work_t *work2;
   
static int irq_call_n = 0;

struct rt_mutex my_mutex;
struct lock_class_key key;

static int cur_work = 0;
static int cur_work_status = 0;
static int other_work_status = 0;

// --------------- SEQ FILE
/* */
#define COOKIE_POT_SIZE PAGE_SIZE

static int my_proc_show(struct seq_file *m, void *v)
{
  seq_printf(m, " + Cur work: - %d, cur work status: %d, other work status: - %d, irq_count: %d\n", cur_work, cur_work_status, other_work_status, irq_call_n);

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


// Bottom Half Function
void my_work_function(struct work_struct *foo)
{
  rt_mutex_lock(&my_mutex);
  my_work_t* w = (my_work_t*) foo; 

  printk(KERN_INFO "Worker%d: status %d\n", w->number, work_pending(&(w->work)));
  cur_work = w->number;
  cur_work_status = work_pending(&(w->work));
  if (w->number == 1)
  {
    printk(KERN_INFO "Worker2: status %d\n", work_pending(&(work2->work)));
    other_work_status = work_pending(&(work2->work));
  }
  else
  {
    printk(KERN_INFO "Worker1: status %d\n", work_pending(&(work1->work)));
    other_work_status = work_pending(&(work1->work));
  }


int i = 0;
int sum = 0;
  while (i < 300000000) {
    i++;
    sum += i;
  }
  printk(KERN_INFO "Sum %d\n", sum);
  printk(KERN_INFO "Workqueue: counter %d\n", ++irq_call_n);
  rt_mutex_unlock(&my_mutex);
}

irqreturn_t irq_handler(int irq, void *dev)
{
  if (irq == KEYBOARDIRQ)
  {
    if (work2)
    {
      queue_work(my_wq, (struct work_struct*)work2);
      queue_work(my_wq, (struct work_struct*)work1);

    }

    return IRQ_HANDLED;
  }
  else
    return IRQ_NONE;
}

static int __init my_module_init(void)
{
  printk(KERN_DEBUG "+MD MODULE loaded!\n");

  int ret = request_irq(KEYBOARDIRQ, irq_handler, IRQF_SHARED,
				"my_irq_handler_workqueue", irq_handler);

  if (ret != 0)
  {
    printk(KERN_ERR "+MD KEYBOARD IRQ handler wasn't registered");
    return -ENOMEM;
  }

  my_wq = create_workqueue("my_queue");

  work1 = (my_work_t*)kmalloc(sizeof(my_work_t), GFP_KERNEL);
  work2 = (my_work_t*)kmalloc(sizeof(my_work_t), GFP_KERNEL);

  INIT_WORK((struct work_struct*)work2, my_work_function);
  work2->number = 2;
  INIT_WORK((struct work_struct*)work1, my_work_function);
  work1->number = 1;

  if (my_wq && work2 && work1)
  {
    printk(KERN_INFO "+MD Workqueue was allocated successfully");
    __rt_mutex_init(&my_mutex, NULL, &key);

    INIT_WORK((struct work_struct*)work1, my_work_function);
    work1->number = 1;
    INIT_WORK((struct work_struct*)work2, my_work_function);
    work2->number = 2;
  }
  else
  {
    free_irq(KEYBOARDIRQ, irq_handler);
    printk(KERN_ERR "+MD Workqueue wasn't allocated");
    return -ENOMEM;
  }

  // INIT SEQFILE
	struct proc_dir_entry *entry;
	entry = proc_create(FORTUNE_NAME, S_IRUGO | S_IWUGO, NULL, &fortune_proc_ops); // S_IRUGO | S_IWUGO == 0666
	if(!entry)
	{
		printk(KERN_INFO "+MD Error: can't create fortune file\n");
        return -ENOMEM;
	}
  // END INIT SEQFILE

  printk(KERN_INFO "+MD KEYBOARD IRQ handler was registered successfully");
  return ret;
}


static void __exit my_module_exit(void)
{
  
  free_irq(KEYBOARDIRQ, irq_handler);
  
  flush_workqueue(my_wq);
  destroy_workqueue(my_wq);
  rt_mutex_destroy(&my_mutex);

  remove_proc_entry(FORTUNE_NAME, NULL);

  printk(KERN_DEBUG "+MD MODULE unloaded!\n");
}

module_init(my_module_init);
module_exit(my_module_exit);