# Assignment_2_comp521

# COMP 521/L — Advanced Operating Systems and Lab  
## Assignment #2 — Linux Kernel Module for Task Information

### **Objective**
Create a Linux kernel module that uses the `/proc` file system to display a task’s information based on its process identifier value (`pid`).

---

### **Specification**

- The program must be coded and tested within a **Linux Virtual Machine** (e.g., VirtualBox, VMware, etc.) to prevent any permanent changes to the host operating system.
- The program will consist of a module that displays a task's information based on its **process identifier value (pid)**.

---

### **Implementation Steps**

1. **Create the kernel source code file:**  
   `pidinfo.c`

   This file should include the following components:

   #### **Required Header Files**
   ```c
   #include <linux/module.h>
   #include <linux/kernel.h>
   #include <linux/proc_fs.h>
   #include <linux/seq_file.h>
   #include <linux/uaccess.h>
   #include <linux/sched/signal.h>
   #include <linux/version.h>
