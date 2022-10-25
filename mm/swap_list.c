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

/*
 * add_to_swap_list - Add an entry to a swap list in the subtree
 * @si:    the swap partition to add
 * @list:  the swap list to consider
 * @new:  a preallocated swap_node
 */
void add_to_swap_list(struct swap_info_struct *si, struct plist_head *list,
		struct swap_node *new)
{
	new->si = si;
	plist_node_init(&new->plist, si->prio);
	plist_add(&new->plist, list);
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
