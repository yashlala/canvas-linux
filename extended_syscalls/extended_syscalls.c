#include <asm-generic/errno-base.h>
#include <linux/uaccess.h>
#include <linux/spinlock.h>
#include <linux/swap.h>
#include <linux/extended_syscalls.h>
#include <linux/cpuset.h>
#include <linux/swapfile.h>

extern atomic_t use_isolated_swap; // Defined in mm/swapfile.c
extern spinlock_t swap_lock; // You know this one
extern struct swap_info_struct *swap_info[MAX_SWAPFILES]; // mm/swapfile.c

SYSCALL_DEFINE1(isolate_swap, int, enable)
{
    pr_warn("Setting use_isolated_swap to %d.\n", enable);
    atomic_set(&use_isolated_swap, enable);
    return 0;
}

SYSCALL_DEFINE2(cgroup_add_swap, int, swap_info_struct_num, int, priority)
{
    pr_warn("add_cgroup_swap: enter syscall with si=%d, priority=%d.\n",
            swap_info_struct_num, priority);

    spin_lock(&swap_lock);
    cpuset_add_swap(current, swap_info[swap_info_struct_num]); // <-does it need lock? RCU?
    spin_unlock(&swap_lock);

    pr_warn("add_cgroup_swap: exit syscall.\n");
    return 0;
}

SYSCALL_DEFINE1(cgroup_remove_swap, int, swap_info_struct_num)
{
    pr_warn("remove_cgroup_swap: enter syscall with si=%d.\n", swap_info_struct_num);

    spin_lock(&swap_lock);
    cpuset_remove_swap(current, swap_info[swap_info_struct_num]); // <-does it need lock? RCU?
    spin_unlock(&swap_lock);

    pr_warn("remove_cgroup_swap: exit syscall.\n");
    return 0;
}

SYSCALL_DEFINE0(get_cgroup_swap)
{
	cpuset_get_preferred_swap(current);
	return 0;
}
