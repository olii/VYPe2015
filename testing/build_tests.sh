#!/bin/bash

build_gcc_file() {
	echo "#include <iostream>
#include <string>

using string = std::string;

template <typename T, typename... Args> void print(const T& data, const Args&... args) {
	std::cout << data;
	print(args...);
}

template <typename T> void print(const T& data) {
	std::cout << data;
}

char read_char(void) {
	return std::cin.get();
}

int read_int(void) {
	int value;
	std::cin >> value;
	return value;
}

string read_string(void) {
	string str;
	std::getline(std::cin, str);
	return str;
}

char get_at(const string& str, int pos) { return str[pos]; }

string set_at(string str, int pos, char ch) {
	str[pos] = ch;
	return str;
}

string strcat(const string& first, const string& second) { return first + second; }

" > tmp/"$1".c
	cat "$1".c >> tmp/"$1".c

	g++ -std=c++11 -o tmp/"$1".bin tmp/"$1".c
	if [ -r "$1".in ]; then
		if [ ! -r "$1".out.ref ]; then
			./tmp/"$1".bin 2>/dev/null >"$1".out.ref < "$1".in
		fi
	else
		if [ ! -r "$1".out.ref ]; then
			./tmp/"$1".bin 2>/dev/null >"$1".out.ref
		fi
	fi
}

prepare_test() {
	echo "$2" > "$1".ec.ref
	if [ $2 -eq 0 ]; then
		build_gcc_file "$1"
	else
		touch "$1".out.ref
	fi
}

mkdir -p tmp/

all_test_files=`find . -maxdepth 1 -type f | grep -E "\.c$" | sed -r "s%\.\/%%g"`
non_error_test_files=`echo "$all_test_files" | grep -Ev "err[1-9].*"`
error_test_files=`echo "$all_test_files" | grep -E "err[1-9].*"`

for test_file in $non_error_test_files; do
	prepare_test `echo "$test_file" | sed -r "s%\.c$%%g"` 0
done
for test_file in $error_test_files; do
	error_code=`echo "$test_file" | sed -r "s%^err([1-9]).*%\1%g"`
	prepare_test `echo "$test_file" | sed -r "s%\.c$%%g"` $error_code
done
#rm -rf tmp/
