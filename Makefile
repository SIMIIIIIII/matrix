CC=clang
CFLAGS=-Wall -Werror
LCUNIT=-lcunit
OBJECTS=objects
HELP=help
SRC=src

all: main

$(OBJECTS):
	mkdir -p objects

main: main.o matrix.o vector.o file.o
	$(CC) -o $@ $(OBJECTS)/main.o $(OBJECTS)/matrix.o $(OBJECTS)/vector.o $(OBJECTS)/file.o -lm

main.o: $(SRC)/main.c headers/vector_threads.h headers/matrix_threads.h headers/file.h
	$(CC) $(CFLAGS) -o $(OBJECTS)/$@ -c $<

vector.o: $(SRC)/vector_threads.c
	$(CC) $(CFLAGS) -o $(OBJECTS)/$@ -c $<

vector_base.o: $(SRC)/vector.c
	$(CC) $(CFLAGS) -o $(OBJECTS)/$@ -c $<

matrix.o: $(SRC)/matrix_threads.c
	$(CC) $(CFLAGS) -o $(OBJECTS)/$@ -c $<

matrix_base.o: $(SRC)/matrix.c
	$(CC) $(CFLAGS) -o $(OBJECTS)/$@ -c $<

file.o: $(SRC)/file.c
	$(CC) $(CFLAGS) -o $(OBJECTS)/$@ -c $<

file_seq.o: $(SRC)/file_seq.c
	$(CC) $(CFLAGS) -o $(OBJECTS)/$@ -c $<

test_base: tests/tests_sequentielle.c vector_base.o matrix_base.o file_seq.o
	$(CC) $(CFLAGS) -o test_base tests/tests_sequentielle.c $(OBJECTS)/vector_base.o $(OBJECTS)/matrix_base.o $(OBJECTS)/file_seq.o $(LCUNIT) -lm

test: tests/tests_threads.c vector.o matrix.o file.o
	$(CC) $(CFLAGS) -o test tests/tests_threads.c $(OBJECTS)/vector.o $(OBJECTS)/matrix.o $(OBJECTS)/file.o $(LCUNIT) -lm


.PHONY: all clean test

clean:
	rm -f $(OBJECTS)/*.o main test *.dat test_base