/*
 *  mm/swap_list.c
 *
 *  Helper functions for managing plists of swap devices
 *
 *  Copyright (C) 2022 ???
 *  TODO: uuuh how does copyright work now
 *
 *  This file is subject to the terms and conditions of the GNU General Public
 *  License.  See the file COPYING in the main directory of the Linux
 *  distribution for more details.
 */

#include <linux/swap_list.h>

// Call with the lists locked
bool in_swap_list(struct swap_info_struct *si, struct plist_head *list)
{
	struct swap_node *pos;
	plist_for_each_entry(pos, list, plist) {
		if (pos->si == si)
			 return true;
	}
	return false;
}

int __add_to_swap_list(struct swap_info_struct *si,
		struct plist_head *list)
{
	struct swap_node *node;

	// TODO: We need more swap when memory is low. But low memory means
	// that GFP_NOWAIT will fail.
	// We need GFP_NOWAIT because this function is called under spinlock
	// (we don't know how many cgroups need node addition until we've
	// locked the cgroup tree). Think this through later, we need a better
	// way.
	if (!(node = kmalloc(sizeof(*node), GFP_NOWAIT)))
		 return -ENOMEM;

	node->si = si;
	plist_node_init(&node->plist, si->prio);
	plist_add(&node->plist, list);
	return 0;
}

int add_to_swap_list(struct swap_info_struct *si, struct plist_head *list)
{
	if (in_swap_list(si, list))
		 return 0;
	return __add_to_swap_list(si, list);
}

void remove_from_swap_list(struct swap_info_struct *si,
		struct plist_head *list)
{
	struct swap_node *node;
	plist_for_each_entry(node, list, plist) {
		if (node->si == si) {
			plist_del(&node->plist, list);
			kfree(node);
			return;
		}
	}
}

void decrement_subsequent_swap_prio(struct swap_info_struct *si,
		struct plist_head *list)
{
	struct swap_node *pos;
	bool found = false;

	plist_for_each_entry(pos, list, plist) {
		if (pos->si == si) {
			 found = true;
			 continue;
		}

		if (found)
			pos->plist.prio--;
	}
}
