CC := gcc
CFLAGS := -Wall -Werror -O3 -pedantic -std=c99
OBJECTS := LargerThanLifeColored.c
LIBRARIES := -lSDL2


.Phony:
all : main

main : $(OBJECTS)
	$(CC) -o main $(OBJECTS) $(CFLAGS) $(LIBRARIES)

%.o : %.c
	$(CC) -o $@ -c $< $(CFLAGS) $(LIBRARIES)

clean:
	for file in main *.o; do if [ -f $${file} ]; then rm $${file}; fi; done

run: main
	./main
