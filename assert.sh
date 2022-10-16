#!/bin/bash

expected="$1"
input="$2"

./9cc "$input" > tmp.s
cc -o tmp tmp.s
./tmp
actual="$?"

if [ "$actual" = "$expected" ]; then
  echo "expect => $expected"
  echo "actual => $actual"
  echo "[ok] $input"
else
  echo [error] $input
  echo "$expected expected, but got $actual"
  exit 1
fi
