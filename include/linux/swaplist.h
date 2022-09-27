/* SPDX-License-Identifier: GPL-2.0 */
#ifndef _LINUX_SWAPLIST_H
#define _LINUX_SWAPLIST_H

#include <linux/swap.h>
#include <linux/plist.h>

struct swap_node {
	struct swap_info_struct *si;
	struct plist_node plist;
};

bool in_swaplist(struct swap_info_struct *si, struct plist_head *list);
int add_to_swaplist(struct swap_info_struct *si, struct plist_head *list);
int __add_to_swaplist(struct swap_info_struct *si, struct plist_head *list);
void remove_from_swaplist(struct swap_info_struct *si,
		struct plist_head *list);
void decrement_subsequent_swap_prio(struct swap_info_struct *si,
		struct plist_head *list);

#endif
