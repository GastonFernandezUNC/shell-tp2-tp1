SRC = ../src/
INC = -I ../include -I ../include/cjson
LIB = -L ../lib -lunity

all:
	rm -f test
	gcc ${INC} ${SRC}shell.c unity.c ${SRC}/cJSON.c ${SRC}/handlers.c ${SRC}/cjson_handler.c  ${SRC}/monitor.c test.c -g -c
	gcc ${LIB} *.o -o test
	rm *.o

clean:
	rm *.o test a.out

