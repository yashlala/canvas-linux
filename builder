#!/bin/bash -e

cd ~/linux-latest

function show_latest_commit () { 
	echo 'HEAD commit is:'
	git --no-pager log -1 --pretty=oneline
}

function synchronize_code () { 
	echo 'Pulling changes locally'
	git pull --rebase
	echo 'Pulling changes on zion-12'
	ssh -t zion-12 'cd ~/linux-latest && git pull --rebase'
}

function build_kernel () { 
	show_latest_commit | tee build.log
	./build_kernel.sh debugbuild 2>&1 | tee -a build.log
}

function install_kernel () { 
	show_latest_commit | tee install.log
	./build_kernel.sh install 2>&1 | tee -a install.log
}

function synchronize_debug () { 
	./scripts/clang-tools/gen_compile_commands.py
	scp compile_commands.json zion-12:~/linux-latest/compile_commands.json

	# Might need to copy in more of the GDB scripts as well. 
	scp "$(readlink vmlinux-gdb.py)" zion-12:~/linux-latest/vmlinux-gdb.py

	temp=$(mktemp)
	objcopy --only-keep-debug vmlinux "$temp"
	scp "$temp" zion-12:~/linux-latest/vmlinux
	rm "$temp"
}


echo 'Warning: Currently building debug kernels with "-ggdb3"'
echo '         Disable debug builds before testing performance.'
operation="$1"
if [ -z "$operation" ]; then
	echo 'Please select: show, sync, build, bgbuild, install, all, clean'
	printf '> '
	read -r operation
fi

case "$operation" in 
	'show') 
		show_latest_commit
		;; 
	'sync') 
		synchronize_code
		test -e ~/linux-latest/vmlinux && synchronize_debug
		;;
	'build') 
		synchronize_code
		build_kernel
		;;
	'install') 
		if ! [ -e ~/linux-latest/vmlinux ]; then 
			synchronize_code
			build_kernel
		fi
		synchronize_debug
		install_kernel
		;;
	'all') 
		synchronize_code
		build_kernel
		synchronize_debug
		install_kernel
		;;
	'clean')
		make clean 
		;;
	'bgbuild') 
		synchronize_code
		{ build_kernel && synchronize_debug; } >/dev/null 2>&1 & 
		disown %1
		;;
esac
