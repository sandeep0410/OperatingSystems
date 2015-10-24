#myfirst make
LIB = -lrt
DEBUG = -g
server:	testserver2.o
	gcc ${DEBUG} -o server testserver2.o ${LIB}
testserver2.0:	testserver2.c
	gcc -c testserver2.c
