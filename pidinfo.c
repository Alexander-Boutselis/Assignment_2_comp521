//pidinfo.c
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/uaccess.h>
#include <linux/pid.h>
#include <linux/sched/signal.h>
#include <linux/version.h>

#define MAX_PIDS 128
#define PROC_NAME "pidinfo"

int stored_pids[MAX_PIDS];
int num_pids;



/***************** Prototypes *****************/
ssize_t proc_read(struct file *file, char __user *usr_buf, size_t count, loff_t *pos);

static struct file_operations proc_ops = {
	.owner = THIS_MODULE,
	.open = proc_open,
	.read = seq_read,
	.write = proc_write,
	.llseek = seq_lseek,
	.release = single_release,
};

// Initialization: create /proc/seconds and record start time
static int __init proc_init(void) {
    proc_create(PROC_NAME, 0666, NULL, &proc_ops);
    return 0;
}

// Cleanup: remove /proc/seconds
static void __exit proc_exit(void) {
    remove_proc_entry(PROC_NAME, NULL);
}

/***************** Write *****************/
ssize_t proc_write(struct file *file, const char __user *usr_buf, size_t count, loff_t *pos){
	char k_mem[32];

	k_mem = kmalloc(count, GFP_KERNEL);

	copy_from_user(k_mem, usr_buf, count);

	printfk(KERN_INFO "%s\n", k_mem);

	kfree(k_mem);

	return count;
}


// Read function for /proc/seconds
ssize_t proc_read(struct file *file, char __user *usr_buf, size_t count, loff_t *pos) {
    int rv = 0;
    char buffer[BUFFER_SIZE];
    static int completed = 0;

    if (completed) {
        completed = 0;
        return 0;
    }

    completed = 1;

    rv = sprintf(buffer, "Elapsed seconds: %ld\n", elapsed_seconds);

    if (copy_to_user(usr_buf, buffer, rv)) {
        return -1;
    }

    return rv;
}

/***************** Module declarations *****************/

// Register init and exit
module_init(proc_init);
module_exit(proc_exit);

// Module metadata
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Assignment 2");
MODULE_AUTHOR("Alexander Boutselis");
