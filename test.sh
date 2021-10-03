#!/bin/bash

assert() {
  expected="$1"
  input="$2"

  ./9cc "$input" > tmp.s
  cc -o tmp tmp.s
  ./tmp
  actual="$?"

  if [ "$actual" = "$expected" ]; then
    echo "$input => $actual"
  else
    echo "$input => $expected expected, but got $actual"
    exit 1
  fi
}

assert 0 '0;'
assert 42 '42;'
assert 41 ' 12 + 34 - 5;'
assert 47 '5+6*7;'
assert 15 '5*(9-6);'
assert 4 '(3+5)/2;'
assert 10 '-10 + 20;'
assert 5 '20 - 15;'
assert 20 '10 - (-10);'
assert 20 '10 - -10;'
assert 1 '10 == 10;'
assert 0 '10 == 11;'
assert 1 '0 != 1;'
assert 0 '1 != 1;'
assert 1 '3 < 10;'
assert 0 '10 < 3;'
assert 1 '10 > 3;'
assert 0 '3 > 10;'
assert 1 '10 <= 10;'
assert 1 '9 <= 10;'
assert 0 '10 <= 9;'
assert 1 '10 >= 10;'
assert 1 '10 >= 9;'
assert 0 '9 >= 10;'
assert 2 'a=b=2;'
assert 10 'foo=bar=10;'
assert 2 'a=2;b=a;'
assert 3 'a=1;b=2;a+b;'
assert 11 'return 11;'
assert 3 'a=1;b=2;return a+b;'
assert 4 'a=1;b=2;if(a==1) { b=4 }; return b;'

echo OK
