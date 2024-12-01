#include "s21_grep.h"

int main(int argc, char **argv) {
  use_flags flags = {0};
  char regexp[4096] = {0};

  parser_flags(argc, argv, &flags, regexp);

  if (optind < argc) {
    int count_files = argc - optind;
    for (int i = optind; i < argc; i++) {
      compile_and_search(regexp, &flags, argv[i], count_files);
    }
  } else {
    fprintf(stderr, "Usage: %s [-eivclnhso] [-f fild nte] pattern file...\n",
            argv[0]);
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

void parser_flags(int argc, char **argv, use_flags *flags, char *regexp) {
  struct option long_options[] = {{"e", required_argument, NULL, 'e'},
                                  {"i", no_argument, NULL, 'i'},
                                  {"v", no_argument, NULL, 'v'},
                                  {"c", no_argument, NULL, 'c'},
                                  {"l", no_argument, NULL, 'l'},
                                  {"n", no_argument, NULL, 'n'},
                                  {"h", no_argument, NULL, 'h'},
                                  {"s", no_argument, NULL, 's'},
                                  {"f", required_argument, NULL, 'f'},
                                  {"o", no_argument, NULL, 'o'},
                                  {NULL, 0, NULL, 0}};

  int opt;
  while ((opt = getopt_long(argc, argv, "e:ivclnhsof:", long_options, NULL)) !=
         -1) {
    switch (opt) {
      case 'e':
        flags->e = 1;
        if (*regexp) strcat(regexp, "|");
        strcat(regexp, optarg);
        break;
      case 'i':
        flags->i = 1;
        break;
      case 'v':
        flags->v = 1;
        break;
      case 'c':
        flags->c = 1;
        break;
      case 'l':
        flags->l = 1;
        break;
      case 'n':
        flags->n = 1;
        break;
      case 'h':
        flags->h = 1;
        break;
      case 's':
        flags->s = 1;
        break;
      case 'f':
        flags->f = 1;
        load_regex_from_file(optarg, regexp);
        break;
      case 'o':
        flags->o = 1;
        break;
      default:
        fprintf(stderr, "Invalid option.\n");
        exit(EXIT_FAILURE);
    }
  }

  if (!flags->e && !flags->f && optind < argc) {
    strcpy(regexp, argv[optind++]);
  }

  if (!*regexp) {
    fprintf(stderr,
            "No pattern specified. Use -e, -f, or provide a pattern.\n");
    exit(EXIT_FAILURE);
  }
}

void load_regex_from_file(const char *filename, char *regexp) {
  FILE *file = fopen(filename, "r");
  if (!file) {
    perror(filename);
    exit(EXIT_FAILURE);
  }

  char line[256];
  while (fgets(line, sizeof(line), file)) {
    if (*regexp) strcat(regexp, "|");
    strcat(regexp, strtok(line, "\n"));  // Remove newline
  }
  fclose(file);
}

void compile_and_search(char *regexp, use_flags *flags, char *filename,
                        int count_files) {
  regex_t compiled;
  int comp_flags = REG_EXTENDED | (flags->i ? REG_ICASE : 0);

  if (regcomp(&compiled, regexp, comp_flags) != 0) {
    if (!flags->s) fprintf(stderr, "Could not compile regex: %s\n", regexp);
    exit(EXIT_FAILURE);
  }

  process_file(filename, flags, regexp, count_files);

  regfree(&compiled);
}

void process_file(char *filename, use_flags *flags, char *regexp,
                  int count_files) {
  FILE *file = fopen(filename, "r");
  if (!file) {
    if (!flags->s) perror(filename);
    return;
  }

  regex_t compiled;
  int comp_flags = REG_EXTENDED | (flags->i ? REG_ICASE : 0);
  if (regcomp(&compiled, regexp, comp_flags) != 0) {
    if (!flags->s)
      fprintf(stderr, "Could not compile regex for file %s\n", filename);
    fclose(file);
    return;
  }

  grep_file(file, flags, &compiled, filename, count_files);

  regfree(&compiled);
  fclose(file);
}

void grep_file(FILE *file, use_flags *flags, regex_t *compiled, char *filename,
               int count_files) {
  char line[4096];
  int line_num = 0, match_count = 0;

  while (fgets(line, sizeof(line), file)) {
    line_num++;
    regmatch_t matches[1];
    int match = (regexec(compiled, line, flags->o ? 1 : 0, matches, 0) == 0);
    if (flags->v) match = !match;

    if (match) {
      match_count++;
      if (!flags->c && !flags->l) {
        if (flags->o) {
          print_only_matches(flags, compiled, filename, count_files, line_num,
                             line);
        } else {
          print_result(flags, filename, count_files, line_num, line);
        }
      }
    }
  }

  if (flags->c) {
    if (count_files > 1 && !flags->h) {
      printf("%s:%d\n", filename, match_count);
    } else {
      printf("%d\n", match_count);
    }
  }

  if (flags->l && match_count > 0) {
    printf("%s\n", filename);
  }
}

void print_only_matches(use_flags *flags, regex_t *compiled, char *filename,
                        int count_files, int line_num, char *line) {
  regmatch_t match;
  char *ptr = line;

  while (regexec(compiled, ptr, 1, &match, 0) == 0) {
    if (!flags->h && count_files > 1) printf("%s:", filename);
    if (flags->n) printf("%d:", line_num);
    printf("%.*s\n", (int)(match.rm_eo - match.rm_so), ptr + match.rm_so);
    ptr += match.rm_eo;
  }
}

void print_result(use_flags *flags, char *filename, int count_files,
                  int line_num, char *line) {
  if (!flags->h && count_files > 1) printf("%s:", filename);
  if (flags->n) printf("%d:", line_num);
  printf("%s", line);
}