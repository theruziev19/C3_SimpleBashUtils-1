#include <ctype.h>
#include <getopt.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct options {
  int b;
  int e;
  int n;
  int s;
  int t;
  int v;
} opt;

void parseOptions(int argc, char *argv[], opt *options);
void printSpecialCharacter(int ch, int *is_flag_v);
void printSqueezeLines(int ch, int *is_flag_s, int flag_n, int flag_b,
                       int flag_e, int *count);
void processFile(FILE *file, opt options);