CC = gcc
CFLAGS = -Wall -Wextra -Werror -std=c11 -O2 -Iinclude
LDFLAGS =

LIB_SRC = src/numeric.c src/preprocess.c src/records.c src/json.c src/parser.c
LIB_OBJ = $(LIB_SRC:src/%.c=build/%.o)

CLI_SRC = cli/main.c
CLI_OBJ = $(CLI_SRC:cli/%.c=build/cli_%.o)

TEST_BINS = build/tests/test_numeric build/tests/test_preprocess build/tests/test_json build/tests/test_samples

.PHONY: all clean test

all: build/libpsa.a build/psa-cli $(TEST_BINS)

build/%.o: src/%.c | build
	$(CC) $(CFLAGS) -c $< -o $@

build/cli_%.o: cli/%.c | build
	$(CC) $(CFLAGS) -c $< -o $@

build/tests/%.o: tests/%.c | build/tests
	$(CC) $(CFLAGS) -c $< -o $@

build/tests/%: build/tests/%.o build/libpsa.a
	$(CC) $(LDFLAGS) $< build/libpsa.a -o $@ -lm

build/libpsa.a: $(LIB_OBJ)
	ar rcs $@ $^

build/psa-cli: $(CLI_OBJ) build/libpsa.a
	$(CC) $(LDFLAGS) $^ -o $@ -lm

test: build/psa-cli $(TEST_BINS)
	@for t in $(TEST_BINS); do \
		echo "Running $$t..."; \
		"$$t" || exit 1; \
	done

build:
	mkdir -p build

build/tests: build
	mkdir -p build/tests

clean:
	rm -rf build
