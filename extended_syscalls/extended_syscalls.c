#include <linux/swap.h>
#include <linux/extended_syscalls.h> 
#include <linux/cpuset.h> 

SYSCALL_DEFINE1(isolate_swap, int, enable)
{
    pr_warn("isolate_swap syscall called.\n"); 
    return 0; 
}

SYSCALL_DEFINE1(set_cgroup_swap, int __user *, swap_info_struct_num)
{
    pr_warn("set_cgroup_swap syscall called.\n"); 
    return 0; 
}

SYSCALL_DEFINE1(get_cgroup_swap, int __user *, swap_info_struct_num)
{
    pr_warn("get_cgroup_swap syscall called.\n"); 
    return 0; 
}
