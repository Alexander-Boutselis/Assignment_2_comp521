//pidinfo.c
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/uaccess.h>
#include <linux/sched/signal.h>
#include <linux/version.h>

#define MAX_PIDS 128
#define PROC_NAME "pidinfo"

/* ---- Global Variables ---- */
static int stored_pids[MAX_PIDS];
static int num_pids;
static struct proc_dir_entry *proc_entry;

/***************** Function Prototypes *****************/
static int pidinfo_show(struct seq_file *m, void *v);
static int proc_open(struct inode *inode, struct file *file);
static ssize_t proc_write(struct file *file, const char __user *usr_buf, size_t count, loff_t *pos);
static int __init pidinfo_init(void);
static void __exit pidinfo_exit(void);

/***************** seq_file Show *****************/
/* Called when user runs "cat /proc/pidinfo" */
static int pidinfo_show(struct seq_file *m, void *v)
{
    int i;

    if (num_pids == 0) {
        seq_puts(m, "No PIDs stored. echo a PID to /proc/" PROC_NAME " first.\n");
        return 0;
    }

    for (i = 0; i < num_pids; i++) {
        int pid = stored_pids[i];
        struct task_struct *task = pid_task(find_vpid(pid), PIDTYPE_PID);

        if (!task) {
            seq_printf(m, "PID %d: <not found>\n", pid);
            continue;
        }

        /* Print a few useful fields */
        seq_printf(m,
                   "command = [%s]\tpid = [%d]\tstate = [%u]\n",
                   task->comm,
                   task->pid,
                   READ_ONCE(task->__state));
    }

    return 0;
}

/***************** /proc Open *****************/
/* Called when /proc file is opened */
static int proc_open(struct inode *inode, struct file *file)
{
    return single_open(file, pidinfo_show, NULL);
}

/***************** /proc Write *****************/
/* Called when user runs "echo PID > /proc/pidinfo" */
static ssize_t proc_write(struct file *file, const char __user *usr_buf, size_t count, loff_t *pos){
    // Debug log to confirm the function runs
    pr_info("proc_write called, count=%zu\n", count);


    char k_mem[32];
    size_t n = min(count, sizeof(k_mem) - 1);
    int ret, pid;


    // Copy user data into kernel buffer
    if (copy_from_user(k_mem, usr_buf, n))
        return -EFAULT;
    k_mem[n] = '\0';


    // Trim trailing newline if present
    if (n > 0 && k_mem[n - 1] == '\n')
        k_mem[n - 1] = '\0';


    // Log what was written
    pr_info("user wrote: %s\n", k_mem);


    // Convert string to integer PID
    ret = kstrtoint(k_mem, 10, &pid);
    if (ret)
        return ret;


    // Store PID if array not full
    if (num_pids >= MAX_PIDS)
        return -ENOSPC;


    stored_pids[num_pids++] = pid;
    pr_info("storing PID %d at index %d\n", pid, num_pids - 1);
    pr_info("pidinfo: stored PID %d\n", pid);


    return count;
}

/***************** File Operations *****************/
static const struct proc_ops proc_ops = {
    .proc_open    = proc_open,
    .proc_read    = seq_read,
    .proc_write   = proc_write,
    .proc_lseek   = seq_lseek,
    .proc_release = single_release,
};

/***************** Module Initialization *****************/
static int __init pidinfo_init(void)
{
    num_pids = 0;
    proc_entry = proc_create(PROC_NAME, 0666, NULL, &proc_ops);
    if (!proc_entry)
        return -ENOMEM;
    pr_info("/proc/%s created\n", PROC_NAME);
    return 0;
}

/***************** Module Cleanup *****************/
static void __exit pidinfo_exit(void)
{
    if (proc_entry)
        proc_remove(proc_entry);
    pr_info("/proc/%s removed\n", PROC_NAME);
}

/***************** Module Declarations *****************/
module_init(pidinfo_init);
module_exit(pidinfo_exit);

// Module metadata
MODULE_LICENSE("pidinfo");
MODULE_DESCRIPTION("Assignment 2");
MODULE_AUTHOR("Alexander Boutselis");
