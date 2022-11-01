#!/bin/bash

expected="$1"
input="$2"
extension="$3"

./9cc "$input" > tmp.s
cc -o tmp tmp.s $extension
./tmp
actual="$?"

if [ "$actual" = "$expected" ]; then
  echo "[ok] $input"
else
  echo [error] "expect: `printf "%2d" $expected`, actual: `printf "%2d" $actual` ($input)"
  exit 1
fi
