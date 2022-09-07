# Canvas Linux

## ToC

- [Intro](#introduction)
- [Development Plan](#development-plan)
- [Interface](#swap-partition-control-interface)
- [Troubleshooting](#troubleshooting)
- [Kernel README](#kernel-readme)

## Introduction

In this repo, we modify the Linux kernel's swap subsystems with the goal of
merging these patches upstream. We plan to implement:

- [X] Per-cgroup control over active swap partitions
- [ ] Per-cgroup swap cache isolation

This work has positive implications for swap throughput; but more importantly,
it makes it easy to develop and manage frontswap-based remote memory systems.
All work is based on Canvas (NSDI '23).


## Development Plan

- [X] Phase 1: Prototype
  1. [X] Define per-cgroup preferred swap partition. Don't worry about
         consistency or proper cgroup interfaces.
  2. [X] Implement primitive syscall-based getters and setters for per-cgroup
         preferred swap partition.
  3. [X] Verify correctness
  4. [X] Test performance.

- [X] Phase 2: Interface
  1. [X] Set up per-cgroup priority lists to dispense with giant locks.
  2. [X] Set up proper interface for per-cgroup preferred swap partition.
  3. [X] Set up `swaps` vs `swaps.effective` distinction
  4. [X] Set up `swaps.lock_subtree` file
  5. [X] Reimplement cgroup plist propagation
  6. [X] Iface error recovery code; try remove after failed add (enomem)
  7. [X] cgroup fork, cgroup init, deletion, etc
  8. [X] Swapon, swapoff (actions too tightly bound to functionality, implement
         in stage 3).

- [.] Phase 3: Functionality
  1. [X] Reimplement actual plist functionality
  2. [ ] Implement swapon and swapoff
  3. [ ] Fix locks (remember `CONFIG_DEBUG_ATOMIC_SLEEP`!).
  4. [ ] Set up edge cases: shared pages, ifndef `CONFIG_CPUSETS`
  5. [ ] Move "`swap_info_struct` is full" to the `swap_info_struct` structure
	 itself. Right now, there's an "avail" list for non-full swap
	 partitions, and a "active" list for all swap partitions. Synchronizing
	 adds and removes to an "avail" list for each cgroup will be too
	 expensive! So move the "full or not" logic to the swap info struct
	 data itself. Our cgroups can check when they receive an `si`, and skip
	 the partition.
  6. [ ] Set up global fallback toggling API
  7. [ ] Set up the "+all" magic swapfile keyword
  8. [ ] Refactor swap accessor paths to use same `alloc_trial` path as other
         mask style functions.

- [ ] Phase 4: Polish
  1. [ ] Implement `swap_exclusive` iface file
  2. [ ] Clean up globals as much as we can
  3. [ ] Clean up code, add inline documentation
  4. [ ] Address commit f26411821d9b.
  5. [ ] Update documentation files
  6. [ ] Address all todos
  7. [ ] Send patchset to kernel mailing list

- [ ] Phase ?: Wishlist?
  1. [ ] Migrate already-swapped pages to per-cgroup swap partitions.
  2. [ ] Per-cgroup swap cache segregation.

We also have some thinking to do about interface details:

- [ ] What if the swap device name isn't even available in the filesystem
      namespace? Eg: swapfile isn't visible in a container.
- [ ] Should we separate swap file names by nulls instead of newlines when
      reading? More robust...but more annoying.
- [ ] What if we set `isolate_subtree` to 1, and a child sets it to zero?
      Update documentation and policy accordingly.

## Interface

Active swap partitions can be controlled via the `cpuset` cgroup controller.
This patchset exposes three new interface files for the cgroup v2 fs:

- `cpuset.swaps`
- `cpuset.swaps.effective`
- `cpuset.swaps.lock_subtree`

They are documented in `./Documentation/admin-guide/cgroup-v2.rst`.

## Troubleshooting

Ubuntu distributions prior to 22.04 don't use the cgroup v2 "unified"
hierarchy. Make sure you've mounted cgroup2 fs via `mount | grep cgroup2`.
`mount | grep -w 'type cgroup'` should print nothing.

After mounting cgroup2 fs, cd to its root.

Check to see if "cpuset" is in the output of "cgroup.controllers". If it isn't,
you've likely compiled it out (or cpuset controller might be attached to
a cgroup v1 hierarchy instead).

In the cgroup2 fs root, run `echo +cpuset >cgroup.subtree_control`.
This should enable the controller for child cgroups.

---

The remainder of this file is a copy of the upstream kernel's README.

---


Kernel Documentation
====================

There are several guides for kernel developers and users. These guides can
be rendered in a number of formats, like HTML and PDF. Please read
Documentation/admin-guide/README.rst first.

In order to build the documentation, use ``make htmldocs`` or
``make pdfdocs``.  The formatted documentation can also be read online at:

    https://www.kernel.org/doc/html/latest/

There are various text files in the Documentation/ subdirectory,
several of them using the Restructured Text markup notation.

Please read the Documentation/process/changes.rst file, as it contains the
requirements for building and running the kernel, and information about
the problems which may result by upgrading your kernel.
