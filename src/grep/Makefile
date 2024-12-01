CC = gcc
CFLAGS = -Wall -Wextra -Werror -std=c11
GREP = s21_grep.c
CLANG_FORMAT = clang-format
FORMAT_FILES = $(GREP)

all: s21_grep

grep: $(GREP)
	$(CC) $(CFLAGS) $^ -o s21_grep

test:
	chmod 777 test.sh
	./test.sh

format:
	@$(CLANG_FORMAT) -i $(FORMAT_FILES)

clean:
	rm -rf s21_grep

rebuild: clean all

.PHONY: all test clean rebuild