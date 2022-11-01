#!/bin/bash

./assert.sh 0  ./test_assets/number.c
./assert.sh 3  ./test_assets/arithmetic_operations/add.c
./assert.sh 41 ./test_assets/arithmetic_operations/add_sub.c
./assert.sh 47 ./test_assets/arithmetic_operations/add_mul.c
./assert.sh 15 ./test_assets/arithmetic_operations/brackets.c
./assert.sh 4  ./test_assets/arithmetic_operations/div.c
./assert.sh 1  ./test_assets/minus_number.c
./assert.sh 20 ./test_assets/arithmetic_operations/sub_brackets_minus_number.c
./assert.sh 20 ./test_assets/arithmetic_operations/sub_minus_number.c
./assert.sh 1  ./test_assets/compare/equal_true.c
./assert.sh 0  ./test_assets/compare/equal_false.c
./assert.sh 1  ./test_assets/compare/not_equal_true.c
./assert.sh 0  ./test_assets/compare/not_equal_false.c
./assert.sh 1  ./test_assets/compare/less_true.c
./assert.sh 0  ./test_assets/compare/less_false.c
./assert.sh 1  ./test_assets/compare/greater_true.c
./assert.sh 0  ./test_assets/compare/greater_false.c
./assert.sh 1  ./test_assets/compare/less_equal_true.c
./assert.sh 0  ./test_assets/compare/less_equal_false.c
./assert.sh 1  ./test_assets/compare/greater_equal_true.c
./assert.sh 0  ./test_assets/compare/greater_equal_false.c
./assert.sh 10 ./test_assets/return.c
./assert.sh 10 ./test_assets/variable.c
./assert.sh 10 ./test_assets/long_variable_name.c
./assert.sh 2  ./test_assets/assign_to_var_from_var.c
./assert.sh 3  ./test_assets/add_var_and_var.c
./assert.sh 4  ./test_assets/if.c
./assert.sh 1  ./test_assets/if_else.c
./assert.sh 10 ./test_assets/while.c
./assert.sh 20 ./test_assets/for.c
./assert.sh 11 ./test_assets/block.c
./assert.sh 10 ./test_assets/func_def_call.c
./assert.sh 10 ./test_assets/func_def_call_return.c
./assert.sh 3  ./test_assets/func_def_call_with_arguments.c
./assert.sh 24 ./test_assets/factorial.c
./assert.sh 55 ./test_assets/fibonacci.c
./assert.sh 3 ./test_assets/address.c
./assert.sh 8 ./test_assets/pointer_plus.c ./alloc4.c
