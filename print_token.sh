#!/usr/bin/env bash

./print_token "$1" > token.dot

if [ $? == 0 ]; then
  echo 'print_token successed.'
  dot -Tpng token.dot -o token.png
else
  echo 'print_token failed.'
  exit 1
fi

if [ $? == 0 ]; then
  echo 'token.png generated.'
  rm ./token.dot
else
  echo 'compiling dot file was failed.'
  exit 1
fi
