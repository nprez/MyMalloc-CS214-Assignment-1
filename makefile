ALL:mymalloc
mymalloc:
	gcc -Wall -g memgrind.c mymalloc.c -o memgrind.out
clean:
	rm -rf *.out