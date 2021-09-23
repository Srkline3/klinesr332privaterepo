#define pr_fmt(fmt) "[csse332]: " fmt

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/printk.h>
#include <linux/slab.h>
#include <linux/proc_fs.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Trey Kline");
MODULE_DESCRIPTION("Programming/Code");

int *myArray;
static struct proc_dir_entry *parent_entry;

static int
__init csse_init(void)
{
    pr_info("Module csse332 initializing...\n");
	parent_entry = kmalloc(sizeof(proc_dir_entry), GFP_KERNEL);
	parent_entry = proc_mkdir("csse332", NULL);

	if(!parent_entry){
		free(parent_entry);
	}

	myArray = kmalloc(10*sizeof(int), GFP_KERNEL);


	myArray[0] = 69;
	myArray[1] = 420;

	pr_info("Mallocated: %d, %d\n", myArray[0], myArray[1]);

    return 0;
}

static void
__exit csse_exit(void)
{
    pr_info("Freeing Array. Some values for your pleasure: %d, %d\n", myArray[0], myArray[1]);
	kfree(myArray);
	if(parent_entry){
		kfree(parent_entry);
	}
	pr_info("Module csse332 exiting...\n");
    
}

 module_init(csse_init);
 module_exit(csse_exit);