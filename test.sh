#!/bin/bash

assert() {
  expected="$1"
  input="$2"

  ./9cc "$input" > tmp.s
  cc -o tmp tmp.s
  ./tmp
  actual="$?"

  if [ "$actual" = "$expected" ]; then
    echo "[ok] $input"
  else
    echo [error] $input
    echo "$expected expected, but got $actual"
    exit 1
  fi
}

assert 0  ./test_assets/number.c
assert 3  ./test_assets/arithmetic_operations/add.c
assert 41 ./test_assets/arithmetic_operations/add_sub.c
assert 47 ./test_assets/arithmetic_operations/add_mul.c
assert 15 ./test_assets/arithmetic_operations/brackets.c
assert 4  ./test_assets/arithmetic_operations/div.c
assert 1  ./test_assets/minus_number.c
assert 20 ./test_assets/arithmetic_operations/sub_brackets_minus_number.c
assert 20 ./test_assets/arithmetic_operations/sub_minus_number.c
assert 1  ./test_assets/compare/equal_true.c
assert 0  ./test_assets/compare/equal_false.c
assert 1  ./test_assets/compare/not_equal_true.c
assert 0  ./test_assets/compare/not_equal_false.c
assert 1  ./test_assets/compare/less_true.c
assert 0  ./test_assets/compare/less_false.c
assert 1  ./test_assets/compare/greater_true.c
assert 0  ./test_assets/compare/greater_false.c
assert 1  ./test_assets/compare/less_equal_true.c
assert 0  ./test_assets/compare/less_equal_false.c
assert 1  ./test_assets/compare/greater_equal_true.c
assert 0  ./test_assets/compare/greater_equal_false.c
assert 10 ./test_assets/return.c
assert 10 ./test_assets/variable.c
assert 10 ./test_assets/long_variable_name.c
assert 2  ./test_assets/assign_to_var_from_var.c
assert 3  ./test_assets/add_var_and_var.c
assert 4  ./test_assets/if.c
assert 1  ./test_assets/if_else.c
assert 10 ./test_assets/while.c
assert 20 ./test_assets/for.c
assert 11 ./test_assets/block.c
assert 10 ./test_assets/func_call.c
assert 10 ./test_assets/func_call_return.c

echo OK
