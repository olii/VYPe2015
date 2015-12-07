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
	"$VYPE" "$1".c "$1".asm
	"$ASM" -i "$1".asm -o tmp/"$1".obj
	"$LNK" tmp/"$1".obj -o tmp/"$1".xexe
	"$SIM" -i tmp/"$1".xexe -x tmp/"$1".xml -n mips < "$1".in > "$1".out
	cat "$1".out | sed -r "/--- Simulation ended in [0-9]+ cycles ---$/q" | sed -r "s/--- Simulation ended in [0-9]+ cycles ---//g" > tmp/"$1".out
	diff -u "$1".ref tmp/"$1".out
	print_test "$1".c $?
}

if [ ! -x "$VYPE" -o ! -x "$ASM" -o ! -x "$LNK" -o ! -x "$SIM" ]; then
	echo "Unable to locate VYPe compiler or MIPS assembler" >&2
	exit 1
fi

mkdir tmp
#run_on "basic-empty-main"
rm -rf tmp
