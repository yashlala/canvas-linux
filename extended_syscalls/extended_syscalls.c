#include <linux/extended_syscalls.h> 

SYSCALL_DEFINE0(set_cgroup_swap, int __user *, swap_info_struct_number)
{
    return 0; // TODO: Do the things
}

SYSCALL_DEFINE3(get_cgroup_swap, int __user *, swap_info_struct_number)
{
    return 0; // TODO: Do the things
}
