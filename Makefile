CC=clang
CFLAGS=-Wall -Werror
COVERAGE_FLAGS=-fprofile-instr-generate -fcoverage-mapping
LCUNIT=-lcunit
OBJECTS=objects
HELP=help
SRC=src

all: main

$(OBJECTS):
	mkdir -p objects

main: main.o matrix.o vector.o file.o
	$(CC) -o $@ $(OBJECTS)/main.o $(OBJECTS)/matrix.o $(OBJECTS)/vector.o $(OBJECTS)/file.o -lm

main.o: $(OBJECTS) $(SRC)/main.c headers/vector_threads.h headers/matrix_threads.h headers/file.h
	$(CC) $(CFLAGS) -o $(OBJECTS)/$@ -c $(SRC)/main.c

vector.o: $(OBJECTS) $(SRC)/vector_threads.c
	$(CC) $(CFLAGS) -o $(OBJECTS)/$@ -c $(SRC)/vector_threads.c

vector_base.o: $(OBJECTS) $(SRC)/vector.c
	$(CC) $(CFLAGS) -o $(OBJECTS)/$@ -c $(SRC)/vector.c

matrix.o: $(OBJECTS) $(SRC)/matrix_threads.c
	$(CC) $(CFLAGS) -o $(OBJECTS)/$@ -c $(SRC)/matrix_threads.c

matrix_base.o: $(OBJECTS) $(SRC)/matrix.c
	$(CC) $(CFLAGS) -o $(OBJECTS)/$@ -c $(SRC)/matrix.c

file.o: $(OBJECTS) $(SRC)/file.c
	$(CC) $(CFLAGS) -o $(OBJECTS)/$@ -c $(SRC)/file.c

file_seq.o: $(OBJECTS) $(SRC)/file_seq.c
	$(CC) $(CFLAGS) -o $(OBJECTS)/$@ -c $(SRC)/file_seq.c

test_base: tests/tests_sequentielle.c vector_base.o matrix_base.o file_seq.o
	$(CC) $(CFLAGS) -o test_base tests/tests_sequentielle.c $(OBJECTS)/vector_base.o $(OBJECTS)/matrix_base.o $(OBJECTS)/file_seq.o $(LCUNIT) -lm

test: tests/tests_threads.c vector.o matrix.o file.o
	$(CC) $(CFLAGS) -o test tests/tests_threads.c $(OBJECTS)/vector.o $(OBJECTS)/matrix.o $(OBJECTS)/file.o $(LCUNIT) -lm

# Coverage targets
vector_cov.o: $(OBJECTS) $(SRC)/vector_threads.c
	$(CC) $(CFLAGS) $(COVERAGE_FLAGS) -o $(OBJECTS)/$@ -c $(SRC)/vector_threads.c

matrix_cov.o: $(OBJECTS) $(SRC)/matrix_threads.c
	$(CC) $(CFLAGS) $(COVERAGE_FLAGS) -o $(OBJECTS)/$@ -c $(SRC)/matrix_threads.c

file_cov.o: $(OBJECTS) $(SRC)/file.c
	$(CC) $(CFLAGS) $(COVERAGE_FLAGS) -o $(OBJECTS)/$@ -c $(SRC)/file.c

vector_base_cov.o: $(OBJECTS) $(SRC)/vector.c
	$(CC) $(CFLAGS) $(COVERAGE_FLAGS) -o $(OBJECTS)/$@ -c $(SRC)/vector.c

matrix_base_cov.o: $(OBJECTS) $(SRC)/matrix.c
	$(CC) $(CFLAGS) $(COVERAGE_FLAGS) -o $(OBJECTS)/$@ -c $(SRC)/matrix.c

file_seq_cov.o: $(OBJECTS) $(SRC)/file_seq.c
	$(CC) $(CFLAGS) $(COVERAGE_FLAGS) -o $(OBJECTS)/$@ -c $(SRC)/file_seq.c

test_coverage: tests/tests_threads.c vector_cov.o matrix_cov.o file_cov.o
	$(CC) $(CFLAGS) $(COVERAGE_FLAGS) -o test_cov tests/tests_threads.c $(OBJECTS)/vector_cov.o $(OBJECTS)/matrix_cov.o $(OBJECTS)/file_cov.o $(LCUNIT) -lm

test_base_coverage: tests/tests_sequentielle.c vector_base_cov.o matrix_base_cov.o file_seq_cov.o
	$(CC) $(CFLAGS) $(COVERAGE_FLAGS) -o test_base_cov tests/tests_sequentielle.c $(OBJECTS)/vector_base_cov.o $(OBJECTS)/matrix_base_cov.o $(OBJECTS)/file_seq_cov.o $(LCUNIT) -lm

coverage: test_coverage test_base_coverage
	@echo "Running tests with coverage..."
	LLVM_PROFILE_FILE="test_cov.profraw" ./test_cov
	LLVM_PROFILE_FILE="test_base_cov.profraw" ./test_base_cov
	@echo "Merging coverage data..."
	llvm-profdata merge -sparse test_cov.profraw test_base_cov.profraw -o coverage.profdata
	@echo "Generating coverage report..."
	llvm-cov show ./test_cov -instr-profile=coverage.profdata $(SRC)/*.c > coverage.txt
	llvm-cov report ./test_cov -instr-profile=coverage.profdata $(SRC)/*.c
	@echo "Generating HTML coverage report..."
	llvm-cov show ./test_cov -instr-profile=coverage.profdata $(SRC)/*.c -format=html -output-dir=coverage_html
	@echo "Coverage report generated in coverage_html/index.html"
	@echo "Coverage summary saved to coverage.txt"


.PHONY: all clean test coverage

clean:
	rm -f $(OBJECTS)/*.o main test *.dat test_base test_cov test_base_cov *.gcda *.gcno *.info *.profraw *.profdata coverage.txt
	rm -rf coverage_html