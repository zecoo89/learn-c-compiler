#!/bin/bash

expected="$1"
input="$2"

./9cc "$input" > tmp.s
cc -o tmp tmp.s
./tmp
actual="$?"

if [ "$actual" = "$expected" ]; then
  echo "[ok] expect: `printf "%2d" $expected`, actual: `printf "%2d" $actual` ($input)"
else
  echo [error] "expect: `printf "%2d" $expected`, actual: `printf "%2d" $actual` ($input)"
  exit 1
fi
