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

SYSCALL_DEFINE2(set_cgroup_swap, int, swap_info_struct_num, int, priority)
{
    pr_warn("set_cgroup_swap: enter syscall with si=%d, priority=%d.\n",
            swap_info_struct_num, priority);

    spin_lock(&swap_lock);
    cpuset_set_current_preferred_swap(swap_info[swap_info_struct_num], priority); // <-does it need lock? RCU?
    spin_unlock(&swap_lock);

    pr_warn("set_cgroup_swap: exit syscall.\n");
    return 0;
}

// This function almost certainly gets locking wrong.
SYSCALL_DEFINE1(get_cgroup_swap, int __user *, swap_info_struct_num)
{
	int ret;
	struct swap_info_struct *preferred;

	preferred = cpuset_get_current_preferred_swap();
	pr_warn("get_cgroup_swap: preferred=%px.\n", preferred);

	ret = get_swap_info_struct_num(preferred);
	if (ret >= 0) {
		pr_warn("get_cgroup_swap: returning w/ match %d\n", ret);
	} else {
		pr_warn("get_cgroup_swap: returning, no match\n");
	}
	return ret;
}
