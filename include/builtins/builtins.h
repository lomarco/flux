#pragma once

#include <context.h>
#include <core/eval.h>

#include <unistd.h>

typedef struct {
  Context* ctx;
  int argc;
  char** argv;
} BuiltinArgs;

typedef int (*builtin_func)(BuiltinArgs* args);

typedef struct {
  const char* name;
  builtin_func func;
} builtin_command;

int builtin_exit(BuiltinArgs* args);

int builtin_cd(BuiltinArgs* args);

extern builtin_command builtins[];

extern int num_builtins;
