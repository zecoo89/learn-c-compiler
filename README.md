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
% ./9cc filepath > tmp.s
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

## Visualization
### token
```sh
% ./print_token.sh 'a=1;'
```

### node
```sh
% ./print_node_tree.sh 'a=1;b=1;return a+b;'
```
