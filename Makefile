all: clean main.bin

main.bin:
		gcc -std=gnu99 -g -Wall allocator.c main.c -o main.bin

clean:
		rm -f *.o *.bin
