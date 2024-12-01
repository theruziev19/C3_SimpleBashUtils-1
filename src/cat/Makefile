CC = gcc
CFLAGS = -Wall -Wextra -Werror -std=c11
CAT = s21_cat.c
CLANG_FORMAT = clang-format
FORMAT_FILES = $(CAT)

all: s21_cat

cat: $(CAT)
	$(CC) $(CFLAGS) $^ -o s21_cat

test:
	chmod 777 test.sh
	./test.sh

format:
	@$(CLANG_FORMAT) -i $(FORMAT_FILES)

clean:
	rm -rf s21_cat

rebuild: clean all

.PHONY: all test clean rebuild clang-format format
