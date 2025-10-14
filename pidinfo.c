//pidinfo.c
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/uaccess.h>
#include <linux/sched/signal.h>
#include <linux/version.h>

#define MAX_TRACKED_PIDS 128
#define PROC_FILE_NAME "pidinfo"

static int tracked_pid_list[MAX_TRACKED_PIDS];
static int tracked_pid_count = 0;
static struct proc_dir_entry *proc_file_entry;

/* ---- Display process information when /proc file is read ---- */
static int display_tracked_processes(struct seq_file *output, void *v){
    int index;

    if (tracked_pid_count == 0) {
        seq_puts(output, "No PIDs currently tracked. Use 'echo <PID> > /proc/" PROC_FILE_NAME "' to add one.\n");
        return 0;
    }

    for (index = 0; index < tracked_pid_count; index++) {
        int current_pid = tracked_pid_list[index];
        struct task_struct *process_task = pid_task(find_vpid(current_pid), PIDTYPE_PID);

        if (!process_task) {
            seq_printf(output, "PID %d: <process not found>\n", current_pid);
            continue;
        }

        seq_printf(output,
                   "process_name = [%s]\tpid = [%d]\tstate = [%u]\n",
                   process_task->comm,
                   process_task->pid,
                   READ_ONCE(process_task->__state));
    }

    return 0;
}

/* ---- Open handler for /proc file ---- */
static int open_pidinfo_proc_file(struct inode *inode, struct file *file)
{
    return single_open(file, display_tracked_processes, NULL);
}

/* ---- Write handler for /proc file ---- */
static ssize_t write_pid_to_proc_file(struct file *file,
                                      const char __user *user_buffer,
                                      size_t buffer_length,
                                      loff_t *offset)
{
    char kernel_buffer[32];
    size_t copy_length = min(buffer_length, sizeof(kernel_buffer) - 1);
    int conversion_result;
    int input_pid;

    if (copy_from_user(kernel_buffer, user_buffer, copy_length))
        return -EFAULT;

    kernel_buffer[copy_length] = '\0';

    /* Remove trailing newline if present */
    if (copy_length > 0 && kernel_buffer[copy_length - 1] == '\n')
        kernel_buffer[copy_length - 1] = '\0';

    conversion_result = kstrtoint(kernel_buffer, 10, &input_pid);
    if (conversion_result)
        return conversion_result;

    if (tracked_pid_count >= MAX_TRACKED_PIDS)
        return -ENOSPC;

    tracked_pid_list[tracked_pid_count++] = input_pid;
    return buffer_length;
}

/* ---- File operations for the /proc entry ---- */
static const struct proc_ops pidinfo_proc_file_operations = {
    .proc_open    = open_pidinfo_proc_file,
    .proc_read    = seq_read,
    .proc_write   = write_pid_to_proc_file,
    .proc_lseek   = seq_lseek,
    .proc_release = single_release,
};

/* ---- Module initialization ---- */
static int __init initialize_pidinfo_module(void)
{
    tracked_pid_count = 0;

    proc_file_entry = proc_create(PROC_FILE_NAME, 0666, NULL, &pidinfo_proc_file_operations);
    if (!proc_file_entry)
        return -ENOMEM;

    pr_info("/proc/%s created successfully\n", PROC_FILE_NAME);
    return 0;
}

/* ---- Module cleanup ---- */
static void __exit cleanup_pidinfo_module(void)
{
    if (proc_file_entry)
        proc_remove(proc_file_entry);

    pr_info("/proc/%s removed successfully\n", PROC_FILE_NAME);
}

module_init(initialize_pidinfo_module);
module_exit(cleanup_pidinfo_module);

MODULE_AUTHOR("Alexander Boutsleis");
MODULE_DESCRIPTION("PID Information Tracking Module");
MODULE_LICENSE("GPL");
