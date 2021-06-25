all: out

out: 
	gcc -o secret -g -pedantic -Wall src/*.c

valgrind: 
	gcc -o secret -g -pedantic -Wall  -fsanitize=address src/*.c

clean:
	 rm out