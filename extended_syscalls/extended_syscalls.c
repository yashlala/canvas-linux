#include <asm-generic/errno-base.h>
#include <linux/uaccess.h>
#include <linux/spinlock.h>
#include <linux/swap.h>
#include <linux/extended_syscalls.h> 
#include <linux/cpuset.h> 

extern atomic_t use_isolated_swap; // Defined in mm/swapfile.c
extern spinlock_t swap_lock; // You know this one
extern struct swap_info_struct *swap_info[MAX_SWAPFILES]; // mm/swapfile.c

SYSCALL_DEFINE1(isolate_swap, int, enable)
{
    pr_warn("Setting use_isolated_swap to %d.\n", enable); 
    atomic_set(&use_isolated_swap, enable); 
    return 0; 
}

SYSCALL_DEFINE1(set_cgroup_swap, int, swap_info_struct_num)
{
    pr_warn("set_cgroup_swap: enter syscall with parameter %d.\n", 
            swap_info_struct_num); 

    spin_lock(&swap_lock); 
    cpuset_set_current_preferred_swap(swap_info[swap_info_struct_num]); // <-does it need lock? RCU? 
    spin_unlock(&swap_lock); 

    pr_warn("set_cgroup_swap: exit syscall.\n"); 
    return 0; 
}

// This function almost certainly gets locking wrong. 
SYSCALL_DEFINE1(get_cgroup_swap, int __user *, swap_info_struct_num)
{
    struct swap_info_struct *si; 
    unsigned long flags; 
    int ret; 

    pr_warn("get syscall called.\n"); 

    spin_lock(&swap_lock); 

    si = cpuset_get_current_preferred_swap(); // <-does it need lock? RCU? 
    if (si == NULL) { 
        pr_warn("Ran get_cgroup_swap, retrieved null pointer\n");
        goto err; 
    }

    spin_lock_irqsave(&si->lock, flags); 
    ret = si->type; 
    spin_unlock_irqrestore(&si->lock, flags); 

    spin_unlock(&swap_lock); 
    return ret; 

err: 
    spin_unlock(&swap_lock); 
    return -ENODATA; 
}
