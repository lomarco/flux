#include <config.h>
#include <context.h>
#include <core/lexer.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char** lex_line(Context* ctx, char* line) {
  int bufsize = LEX_BUFSIZE;
  int position = 0;
  char* token;
  char** tokens = (char**)malloc((size_t)bufsize * sizeof(char*));
  char** new_tokens;
  char* saveptr;

  if (!tokens) {
    fprintf(stderr, "%s: allocation error\n", ctx->argv[0]);
    exit(1);
  }
  token = strtok_r(line, LEX_DELIM, &saveptr);
  while (token != NULL) {
    tokens[position++] = token;
    if (position >= bufsize) {
      bufsize *= 2;
      new_tokens =
          (char**)realloc((void*)tokens, (size_t)bufsize * sizeof(char*));
      if (!new_tokens) {
        free((void*)tokens);
        fprintf(stderr, "%s: reallocation error\n", ctx->argv[0]);
        exit(1);
      }
      tokens = new_tokens;
    }
    token = strtok_r(NULL, LEX_DELIM, &saveptr);
  }
  tokens[position] = NULL;
  return tokens;
}

int count_args(char** tokens_arr) {
  int count;

  count = 0;
  while (tokens_arr[count] != NULL) {
    ++count;
  }
  return count;
}
