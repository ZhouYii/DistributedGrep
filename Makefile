CC = gcc
FLAGS = -c -g -Wall -Werror 

all: main test

test : test.o LL.o
	$(CC) test.o LL.o -o test_me

main : main.o LL.o
	$(CC) main.o LL.o -o main

test.o : test.c
	$(CC) -c -g -Wall -Werror test.c
main.o : main.c
	$(CC) $(FLAGS) main.c

list.o : LL.c
	$(CC) $(FLAGS) LL.c

clean :
	rm ./*.o
