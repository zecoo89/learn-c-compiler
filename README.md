This repository is to learn 8cc.

# compile codes
% make all

# how to test
% make test

# 9cc's usage
## compile to assembly code.
% ./9cc 'a=1;b=1;return a+b;' > tmp.s
## compile to binary.
% cc -o tmp tmp.s
## execute binary
% ./tmp
## echo a last return value.
% echo $?

# print_token's usage
% ./print_token 'a=1;' > token.dot
# compile dot file to png image.
% dot -Tpng -o token.png token.dot

# print_node_tree's usage
% ./print_node_tree 'a=1;b=1;return a+b;' > node_tree.dot
# compile dot file to png image
% dot -Tpng -o node_tree.png node_tree.dot
