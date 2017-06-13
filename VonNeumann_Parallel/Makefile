CC := gcc
CFLAGS := -Wall -Werror -O3 -fopenmp -pedantic -std=iso9899:2011


OBJECTS := VonNeumann_parallel.c


.Phony:
all : main

main : $(OBJECTS)
	$(CC) -o main $(OBJECTS) $(CFLAGS)

%.o : %.c
	$(CC) -o $@ -c $< $(CFLAGS)

clean:
	for file in main *.o; do if [ -f $${file} ]; then rm $${file}; fi; done

run: main
	./main
