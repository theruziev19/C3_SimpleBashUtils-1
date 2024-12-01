#ifndef S21_GREP_H
#define S21_GREP_H

#include <getopt.h>
#include <regex.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

typedef struct {
  int e, i, v, c, l, n, h, s, f, o;
} use_flags;

void parser_flags(int argc, char **argv, use_flags *flags, char *regexp);
void load_regex_from_file(const char *filename, char *regexp);
void compile_and_search(char *regexp, use_flags *flags, char *filename,
                        int count_files);
void process_file(char *filename, use_flags *flags, char *regexp,
                  int count_files);
void grep_file(FILE *file, use_flags *flags, regex_t *compiled, char *filename,
               int count_files);
void print_only_matches(use_flags *flags, regex_t *compiled, char *filename,
                        int count_files, int line_num, char *line);
void print_result(use_flags *flags, char *filename, int count_files,
                  int line_num, char *line);

#endif  // S21_GREP_H
