This repository is to learn 9cc.

## compile codes
```sh
% make all
```

## how to test
```sh
% make test
```

## MacOS support
```sh
% make clean
% CFLAGS=-DMAC_FLAG make all
```

## 9cc's usage
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

## print_token's usage
```sh
% ./print_token.sh 'a=1;'
```

## print_node_tree's usage
```sh
% ./print_node_tree.sh 'a=1;b=1;return a+b;'
```
