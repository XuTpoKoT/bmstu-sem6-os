FLAGS = -O0
INC_DIR = ./inc

build:
	gcc $(FLAGS) -I$(INC_DIR) src/client.c -o client.out
	gcc $(FLAGS) -I$(INC_DIR) src/server.c -o server.out

clear:
	rm -f *.o *.out
