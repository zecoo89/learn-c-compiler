#!/usr/bin/env bash

./print_token $1 > token.dot
dot -Tpng token.dot -o token.png
echo 'token.png generated.'
