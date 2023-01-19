#!/bin/zsh

function proclaim () {
  printf '\n#######################\n'
  printf '# %s\n' "$1"
  printf '#######################\n'
}

function echoeval () {
  echo "$*"
  eval $@
}

function show-file () {
  local cg_file=$1

  if ! [[ -e $cg_file ]]; then
    printf '- %s does not exist\n' $cg_file
    return
  fi

  printf '- %s: ' $cg_file
  < $cg_file tr '\n' ' '
  printf '\n'
}

function show-dir () {
  while [[ $# -ne 0 ]]; do
    pushd $1 >/dev/null
    local cg_dir="$1"

    printf 'BEGIN %s (lock_subtree=%d):\n' $cg_dir "$(cat cpuset.swaps.lock_subtree 2>/dev/null)"
    show-file cpuset.swaps
    show-file cpuset.swaps.effective
    printf 'END\n' $cg_dir

    popd >/dev/null
    shift
  done
}

function add () {
  pushd $1 >/dev/null
  printf '%s %s\n' $1 "$(echo +$1 | tee cpuset.swaps)"
  popd >/dev/null
}

function remove () {
  pushd $1 >/dev/null
  printf '%s %s\n' $1 "$(echo -$1 | tee cpuset.swaps)"
  popd >/dev/null
}

function enable-controllers () {
  for c in $(cat cgroup.controllers); do
    echo "+$c" | tee cgroup.subtree_control >/dev/null
  done
}

function brd-setup () {
  modprobe brd rd_nr=4 rd_size=2097152
  for i in /dev/ram*; do
    mkswap "$i"
  done
}

function brd-swapon () {
  for i in /dev/ram*; do
    echo swapon $i
    swapon $i >/dev/null
  done
}

function brd-swapoff () {
  for i in /dev/ram*; do
    echo swapoff $i
    swapoff $i >/dev/null
  done
}


cd /sys/fs/cgroup
cdirs=(test1 test2 test1/1 test1/2 test2/1 test2/2 test2/2/1)

# Enable controllers, etc

echo "please enable brd (don't swapon yet), then press enter"
(read foo)

proclaim setting up
enable-controllers
swapoff /swap.img &>/dev/null

mkdir -p $cdirs
for dir in $cdirs; do
  cd $dir
  enable-controllers
  cd /sys/fs/cgroup
done

echo isolating subtrees
(cd test2; echo 1 >cpuset.swaps.lock_subtree )
(cd test2/1; echo 0 >cpuset.swaps.lock_subtree )
(cd test2/2; echo 1 >cpuset.swaps.lock_subtree )

proclaim 'testing swapon (add recursive, respect lock_subtree)'
show-dir . test1 test2 test2/1 test2/2
echoeval swapon /dev/ram0
echoeval swapon /dev/ram1
show-dir . test1 test1/1 test2 test2/1 test2/2

proclaim 'testing swap removal'
echo 'removing swap device should:'
echo ' - propagate and ignore lock_subtree'
echo " - keep it in child's swaps"
echo " - but not child's effective_swaps"
show-dir test2 test2/1 test2/2
remove test2 /dev/ram1
show-dir test2 test2/1 test2/2
echo

proclaim 'testing swap addition'

echo 'add only to allowed if not in parent effective, dont propagate'
show-dir test2 test2/2 test2/2/1
add test2/2 /dev/ram0
show-dir test2 test2/2 test2/2/1
echo

echo 'add to children if parent is root, respect child isolate'
show-dir test2 test2/1 test2/2 test2/2/1
add test2 /dev/ram1
show-dir test2 test2/1 test2/2 test2/2/1
echo

echo 'TODO more testing here'
echo

proclaim 'testing cgroup creation'
show-dir . test2 test2/1
mkdir test3 test2/new test2/1/new
show-dir test3 test2/new test2/1/new

proclaim 'testing cgroup deletion'
rmdir test3 test2/new test2/1/new
ls

proclaim 'testing swapoff'
echo 'swapoff should remove from all, even if lock_subtree.'
swapoff /dev/ram1
show-dir . test1/1
echo TODO test swapoff TODO  >/dev/stderr


# BASIC TESTS
#
# Creation of new children:
# - Inherit all
#
# Add:
# - Add to nonroot cgroup should add
# - Add to nonroot cgroup idempotent
# - Add to child cgroups
# - Add to child allowed cgroups idempotent if already enabled (no double-add)
# - Add to
# -
#

# ADVANCED TESTS
# - Swap file not visible in child cgroup
# - set lock_subtree to 1, child sets it to zero? Should we allow?
# - child should not inherit parent's lock_subtree
