/* SPDX-License-Identifier: GPL-2.0 */
#ifndef _LINUX_SWAP_LIST_H
#define _LINUX_SWAP_LIST_H

#include <linux/swap.h>
#include <linux/plist.h>

struct swap_node {
	struct swap_info_struct *si;
	struct plist_node plist;
};

bool in_swap_list(struct swap_info_struct *si, struct plist_head *list);
void add_to_swap_list(struct swap_info_struct *si, struct plist_head *list,
		struct swap_node *node);
void remove_from_swap_list(struct swap_info_struct *si,
		struct plist_head *list);
void decrement_subsequent_swap_prio(struct swap_info_struct *si,
		struct plist_head *list);

#endif
