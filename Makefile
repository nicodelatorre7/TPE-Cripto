all: out

out: 
	gcc -o ss -g -pedantic -Wall src/*.c

valgrind: 
	gcc -o ss -g -pedantic -Wall  -fsanitize=address src/*.c

clean:
	rm -rf ss *.o *.dSYM