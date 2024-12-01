#include "s21_cat.h"

static struct option long_options[] = {{"number-nonblank", no_argument, 0, 'b'},
                                       {"number", no_argument, 0, 'n'},
                                       {"squeeze-blank", no_argument, 0, 's'},
                                       {0, 0, 0, 0}};

void parseOptions(int argc, char *argv[], opt *options) {
  int currentFlags;
  while ((currentFlags =
              getopt_long(argc, argv, "+nbsetTvE", long_options, NULL)) != -1) {
    switch (currentFlags) {
      case 'n':
        options->n = 1;
        break;
      case 'b':
        options->b = 1;
        options->n = 0;
        break;
      case 's':
        options->s = 1;
        break;
      case 'e':
        options->e = 1;
        options->v = 1;
        break;
      case 't':
        options->t = 1;
        options->v = 1;
        break;
      case 'v':
        options->v = 1;
        break;
      case 'T':
        options->t = 1;
        break;
      case 'E':
        options->e = 1;
        break;
      default:
        printf("cat: wrong option!");
        break;
    }
  }
}

void printSpecialCharacter(int ch, int *is_flag_v) {
  if (ch >= 127 && ch < 160) {
    if (ch == 127) {
      printf("^?");
      *is_flag_v = 1;
    }
    if (ch > 127) {
      printf("M-^");
      putchar(ch - 64);
      *is_flag_v = 1;
    }
  }
  if (ch < 32 && ch != '\n' && ch != '\t') {
    printf("^");
    if (ch >= 0) {
      putchar(ch + 64);
    }
    if (ch < 0) {
      putchar(ch + 128);
    }
    *is_flag_v = 1;
  }
}

void processFile(FILE *file, opt options) {
  int ch;
  int new_line = 1;
  int count = 1;
  int is_flag_s = 0;
  while ((ch = fgetc(file)) != EOF) {
    int is_flag_v = 0;
    if (options.s == 1 && new_line && ch == '\n') {
      printSqueezeLines(ch, &is_flag_s, options.n, options.b, options.e,
                        &count);
    }

    if (options.s == 1 && new_line && ch != '\n') {
      is_flag_s = 0;
    }

    if (options.n == 1 && new_line && options.b != 1) {
      if (is_flag_s != 1) {
        printf("%6d\t", count++);
      }
    }

    if (options.b == 1 && new_line && ch != '\n') {
      printf("%6d\t", count++);
    }

    if (options.e == 1 && ch == '\n') {
      if (is_flag_s != 1) {
        printf("$");
      }
    }
    if (options.v == 1) {
      printSpecialCharacter(ch, &is_flag_v);
    }

    if (options.t == 1 && ch == '\t') {
      printf("^");
      ch = 'I';
    }

    if (is_flag_s != 1 && (is_flag_v == 0)) {
      printf("%c", ch);
    }

    new_line = (ch == '\n') ? 1 : 0;
  }
}

void printSqueezeLines(int ch, int *is_flag_s, int n, int b, int e,
                       int *count) {
  if (*is_flag_s == 0) {
    if (n == 1 && b == 0) {
      printf("%6d\t", (*count)++);
    }
    if (e == 1) {
      printf("$");
    }
    printf("%c", ch);
  }
  *is_flag_s = 1;
}

int main(int argc, char *argv[]) {
  opt options = {0};

  parseOptions(argc, argv, &options);

  // Если не передан файл в аргументе, показываем ошибку
  if (argc == 1) {
    printf("Usage: %s [OPTION]... [FILE]...\n", argv[0]);
    return 1;
  }

  while (optind < argc) {
    char *fileName = argv[optind];

    FILE *file = fopen(fileName, "r");

    if (file == NULL) {
      // Добавляем вывод ошибки, как у стандартного cat
      fprintf(stderr, "cat: %s: No such file or directory\n", fileName);
      return 1;
    } else {
      processFile(file, options);
      fclose(file);
    }

    optind++;
  }

  return 0;
}
