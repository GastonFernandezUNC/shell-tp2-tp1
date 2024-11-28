INC = -I ../include/ -I ../include/cjson/

all: normal

run: normal
	./main

normal:
	rm -f main
	# gcc ${INC} shell.c monitor.c handlers.c main.c cjson_handler.c cJSON.c -g -c
	gcc ${INC} *.c -g -c
#	gcc  ${INC} handlers.c monitor.c main.c -g -c
	gcc *.o -o main
	rm *.o

clean:
	rm *.o main a.out monitor
