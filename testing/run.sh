#!/bin/bash

VYPE=$VYPE_DIR/vype
ASM=$MIPS_DIR/assembler2
LNK=$MIPS_DIR/linker
SIM=$MIPS_DIR/intersim2

print_test() {
	if [ $2 -eq 0 ]; then
		if [ -t 1 ]; then
			RESULT='\e[1;32mPASS\e[0m'
		else
			RESULT='PASS'
		fi
	else
		if [ -t 1 ]; then
			RESULT='\e[1;31mFAIL\e[0m'
		else
			RESULT='FAIL'
		fi
	fi

	printf '%-100s [ %b ]\n' "$1"... $RESULT
}

run_on() {
	"$VYPE" "$1".c tmp/"$1".asm 2>/dev/null >&2
	echo $? > "$1".ec && touch "$1".out && [[ `cat "$1".ec` -eq 0 ]] && \
	"$ASM" -i tmp/"$1".asm -o tmp/"$1".obj && \
	"$LNK" tmp/"$1".obj -o tmp/"$1".xexe && \
	if [ -r "$1".in ]; then
		"$SIM" -i tmp/"$1".xexe -x tmp/"$1".xml -n mips < "$1".in > tmp/"$1".out
	else
		"$SIM" -i tmp/"$1".xexe -x tmp/"$1".xml -n mips > tmp/"$1".out
	fi
	if [ -r tmp/"$1".out ]; then
		cat tmp/"$1".out | sed -r "/--- Simulation ended in [0-9]+ cycles ---$/q" \
			| sed -r "/^--- Simulation ended in [0-9]+ cycles ---/d" \
			| sed -r "s/--- Simulation ended in [0-9]+ cycles ---//g" > "$1".out
	fi
	diff -u "$1".ec.ref "$1".ec > tmp/"$1".ec.diff
	failed_ec=$?
	failed_out=0
	diff -u "$1".out.ref "$1".out > tmp/"$1".out.diff
	failed_out=$?
	failed=$(($failed_ec | $failed_out))
	print_test "$1".c $failed
	if [ $failed_out -ne 0 ]; then
		cat tmp/"$1".out.diff
	fi
	if [ $failed_ec -ne 0 ]; then
		cat tmp/"$1".ec.diff
	fi
}

if [ $# -eq 1 ]; then
	if [ "$1" = "clean" ]; then
		rm -rf tmp/ `find . -maxdepth 1 -type f | grep -E "\.(ec|out)$" | tr '\n' ' '`
		exit 0
	fi
fi

if [ ! -x "$VYPE" -o ! -x "$ASM" -o ! -x "$LNK" -o ! -x "$SIM" ]; then
	echo "Unable to locate VYPe compiler or MIPS assembler" >&2
	exit 1
fi

all_test_files=`find . -maxdepth 1 -type f | grep -E "\.c$" | sed -r "s%\.\/%%g"`

mkdir -p tmp
for test_file in $all_test_files; do
	run_on `echo "$test_file" | sed -r "s%\.c$%%g"`
done
#rm -rf tmp
