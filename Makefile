CFLAGS=-std=c11 -g -static
SRCS=$(wildcard *.c)
OBJS=$(SRCS:.c=.o)

9cc: $(OBJS)
	$(CC) -o 9cc $(filter-out func_call.o print_token.o print_node_tree.o, $(OBJS)) $(LDFLAGS)

print_token: $(OBJS)
	$(CC) -o print_token $(filter-out func_call.o main.o print_node_tree.o, $(OBJS)) $(LDFLAGS)

print_node_tree: $(OBJS)
	$(CC) -o print_node_tree $(filter-out func_call.o main.o print_token.o, $(OBJS)) $(LDFLAGS)

all: 9cc print_token print_node_tree

$(OBJS): 9cc.h

test: 9cc
	./test.sh

clean:
	rm -f 9cc *.o *~ tmp* *.dot *.png

.PHONY: test clean
