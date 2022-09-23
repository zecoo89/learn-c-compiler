This repository is to learn 8cc.

# compile codes
```sh
% make all
```

# how to test
```sh
% make test
```

# 9cc's usage
 compile to assembly code.
```sh
% ./9cc 'a=1;b=1;return a+b;' > tmp.s
```
 compile to binary.
```sh
% cc -o tmp tmp.s
```
 execute binary
```sh
% ./tmp
```
 echo a last return value.
```sh
% echo $?
```

# print_token's usage
```sh
% ./print_token 'a=1;' > token.dot
```
# compile dot file to png image.
```sh
% dot -Tpng -o token.png token.dot
```

# print_node_tree's usage
```sh
% ./print_node_tree 'a=1;b=1;return a+b;' > node_tree.dot
```
# compile dot file to png image
```sh
% dot -Tpng -o node_tree.png node_tree.dot
```
