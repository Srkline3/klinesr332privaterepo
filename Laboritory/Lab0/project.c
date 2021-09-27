#define pr_fmt(fmt) "[csse332]: " fmt

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/printk.h>
#include <linux/slab.h>
#include <linux/proc_fs.h>
#include <asm/uaccess.h>
#include <linux/uaccess.h>
#include <linux/string.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Trey Kline");
MODULE_DESCRIPTION("Programming/Code");

int *myArray;
static struct proc_dir_entry *parent_entry;

static ssize_t
csse332_read(struct file *filp, char __user *buff,
    size_t count, loff_t *offp)
{

		int bytes;
		char *myBuffer;

		if(*offp > 9){
			return 0;
		}

		// loff_t next; 
		myBuffer = kmalloc(sizeof(char) * (count + 1), GFP_KERNEL);
		bytes = sprintf(myBuffer, "%d", myArray[*offp]);
		myBuffer[bytes] = '\0';

	
		*offp = *offp + 1;

		// next = *offp + 1;
		// offp = &next;

		if (count >= strlen(myBuffer)){
			copy_to_user(buff, myBuffer, bytes + 1);
			kfree(myBuffer);
			return bytes + 1;
		}

		kfree(myBuffer);
        /* Your read code goes here ... */
        return -1; // please don't remove this line until you are ready to write your own!
}

static ssize_t
csse332_write(struct file *filp, const char __user *buff,
    size_t count, loff_t *offp)
{
		int address;
		int value;
		char* copied_buffer = kmalloc(sizeof(char) * (count + 1), GFP_KERNEL);

		pr_info("Doing !! !! !! !!! ");
		copy_from_user(copied_buffer, buff, count);

		if(copied_buffer[count - 1 ] == '\n'){
			copied_buffer[count - 1 ] = '\0';
		}
		
		copied_buffer[count] = '\0';

		pr_info("String: %s", copied_buffer);
		sscanf(copied_buffer, "%d %d", &address, &value);
		pr_info("Things: %d, %d", address, value);

		if(address > 9 || address < 0){
			kfree(copied_buffer);
			return -EINVAL;
		}else{
			myArray[address] = value;
			kfree(copied_buffer);
	
			pr_info("Wrote: %d, addr: %d, val: %d\n", myArray[address], address, value);
		}
        /* Your write code goes here ... */
        return count; // please don't remove this line until you are ready to write your own!
}

static const struct file_operations csse332_fops = {
        .owner = THIS_MODULE,
        .read  = csse332_read,
        .write = csse332_write,
};

static int
__init csse_init(void)
{
    pr_info("Module csse332 initializing...\n");
	parent_entry = proc_mkdir("csse332", NULL);

	proc_create("status", 0666, parent_entry, &csse332_fops);

	myArray = kmalloc(10*sizeof(int), GFP_KERNEL);

	// pr_info("Mallocated: %d, %d\n", myArray[0], myArray[1]);

    return 0;
}

static void
__exit csse_exit(void)
{
    pr_info("Freeing Array. Some values for your pleasure: %d, %d\n", myArray[0], myArray[1]);
	kfree(myArray);
	if(parent_entry){
		remove_proc_entry("csse332/status", NULL);
		remove_proc_entry("csse332", NULL);
	}
	pr_info("Module csse332 exiting...\n");
    
}

 module_init(csse_init);
 module_exit(csse_exit);