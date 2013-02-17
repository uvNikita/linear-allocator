all: clean main.bin test.bin

main.bin:
		gcc -std=gnu99 -g -Wall allocator.c main.c -o main.bin

test.bin:
		gcc -std=gnu99 -g -Wall allocator.c test.c -o test.bin

clean:
		rm -f *.o *.bin
