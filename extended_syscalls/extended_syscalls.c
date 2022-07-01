#include <linux/extended_syscalls.h> 
#include <linux/cpuset.h> 

SYSCALL_DEFINE1(isolate_swap, int, enable)
{
    // __isolate_swap(); 
    return 0; 
}

SYSCALL_DEFINE1(set_cgroup_swap, int __user *, swap_info_struct_num)
{
    // __set_cgroup_swap(); 
    return 0; 
}

SYSCALL_DEFINE1(get_cgroup_swap, int __user *, swap_info_struct_num)
{
    // __get_cgroup_swap(); 
    return 0; 
}
