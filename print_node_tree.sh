#!/bin/env bash

./print_node_tree $1 > node_tree.dot
dot -Tpng node_tree.dot -o node_tree.png
echo 'node_tree.png generated.'
