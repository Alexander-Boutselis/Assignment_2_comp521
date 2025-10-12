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
   ```

   #### **Functions**
   - **`proc_show`**  
     Function for reading `/proc/pidinfo`.  
     Called whenever the command below is executed:
     ```bash
     cat /proc/pidinfo
     ```

   - **`pidinfo_write`**  
     Function for writing to `/proc/pidinfo`.  
     Called whenever the command below is executed:
     ```bash
     echo "{pid}" > /proc/pidinfo
     ```

   #### **Structures**
   - **`proc_ops`**  
     A file operations structure that contains the functions called when:
     - A process **reads** `pidinfo` → calls `proc_show`
     - A process **writes** to `pidinfo` → calls `pidinfo_write`

   #### **Initialization and Cleanup**
   - **`proc_init` function**  
     Calls `proc_create()` to register the module and create the `/proc` file.
   - **`proc_exit` function**  
     Calls `remove_proc_entry()` to remove the module from the `/proc` file system.

---

### **Compilation and Execution**

#### **1. Compile the Kernel Module**
Use the provided `Makefile` and compile the module with:
```bash
make
```
Fix any errors that appear during compilation.

---

#### **2. Insert the Kernel Module**
Load the module into the kernel:
```bash
sudo insmod pidinfo.ko
```

Check whether the module has loaded:
```bash
lsmod | head
```

---

#### **3. Test the Module**

- Check for valid process IDs:
  ```bash
  ps -gx
  ```

- Write a PID to the module:
  ```bash
  echo "{pid}" > /proc/pidinfo
  ```

- To store multiple PIDs, use:
  ```c
  static int stored_pids[MAX_PIDS];
  int num_pids;
  ```

- Read from the module:
  ```bash
  cat /proc/pidinfo
  ```

---

#### **4. Remove the Kernel Module**
Unload the module:
```bash
sudo rmmod pidinfo
```
> *Note:* The `.ko` suffix is unnecessary when using `rmmod`.

---

### **Summary of Commands**

| Action | Command |
|:--|:--|
| **Compile the module** | `make` |
| **Insert the module** | `sudo insmod pidinfo.ko` |
| **Check loaded modules** | `lsmod | head` |
| **List running processes** | `ps -gx` |
| **Write PID to module** | `echo "{pid}" > /proc/pidinfo` |
| **Read module output** | `cat /proc/pidinfo` |
| **Remove module** | `sudo rmmod pidinfo` |

---

### **Notes**

- Always work in a virtualized Linux environment to avoid affecting the host system.
- Use `dmesg` to view kernel log messages if debugging is needed.
- Ensure you have root privileges when inserting or removing kernel modules.

---
