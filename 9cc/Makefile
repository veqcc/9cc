CFLAGS=-Wall -std=c11
SRCS=$(wildcard *.c)
OBJS=$(SRCS:.c=.o)
CC=gcc

9cc: $(OBJS)
	$(CC) $(CFLAGS) -o 9cc $(OBJS)

$(OBJS): 9cc.h

test: 9cc
	./test.sh

clean:
	rm -f 9cc *.o *~