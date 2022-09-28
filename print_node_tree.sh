#!/usr/bin/env bash

./print_node_tree "$1" > node_tree.dot

if [ $? == 0 ]; then
  echo 'print_token successed.'
  dot -Tpng node_tree.dot -o node_tree.png
else
  echo 'print_node_tree failed.'
  exit 1
fi

if [ $? == 0 ]; then
  echo 'node_tree.png generated.'
  rm ./node_tree.dot
else
  echo 'compiling dot file was failed.'
  exit 1
fi
