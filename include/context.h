#pragma once

#include <stddef.h>

typedef struct {
  int argc;
  char** argv;
  char** env_vars;
  size_t env_size;
  int last_exit_code;
} Context;

Context* init_context(int argc, char* argv[]);

void free_context(Context* ctx);

int get_exit_code(Context* ctx);
