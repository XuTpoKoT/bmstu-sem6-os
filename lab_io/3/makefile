FLAGS = -O0
INC_DIR = ./inc

build:
	gcc $(FLAGS) -I$(INC_DIR) src/single.c -o single.out

build-m:
	gcc $(FLAGS) -I$(INC_DIR) src/multithread.c -o multithread.out

start:
	./single.out

start-m:
	./multithread.out

clear:
	rm -f *.o *.out
	rm -f output.txt
