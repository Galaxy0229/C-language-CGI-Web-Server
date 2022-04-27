all: simple die large server

simple: simple.o cgi.o
	gcc -Wall -g -o $@ $^

die: die.o
	gcc -Wall -g -o $@ $^

large: large.o cgi.o
	gcc -Wall -g -o $@ $^

server: server.o
	gcc -Wall -g -o $@ $^

%.o : %.c
	gcc -Wall -g -c $<

clean: 
	rm -f die large server simple *.o

test: all
	./server msg1 > msg1.test
	./server msg2 > msg2.test
	./server msg3 > msg3.test
	./server msg4 > msg4.test
	./server msg5 > msg5.test
	./server msg6 > msg6.test