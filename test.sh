#!/bin/bash
assert() {
    expected="$1"
    input="$2"

    ./c/cc "$input" > tmp.s
    cc -o c/tmp tmp.s
    ./c/tmp
    actual="$?"

    if [ "$actual" = "$expected" ]; then
        echo "$input => $actual"
    else
        echo "$input => $expected expected, but got $actual"
        exit 1
    fi
}

assert 0 0
assert 42 42

echo OK