CC := gcc
CFLAGS := -Wall -Werror -O3 -pedantic -std=c99
OBJECTS := SmoothLifeL.c


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
