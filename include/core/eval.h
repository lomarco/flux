#pragma once

#include <context.h>

typedef enum {
  SHELL_EXIT = -1,
  SHELL_OK = 0,
  SHELL_ERROR = 1,
  SHELL_COMAND_NOT_FOUND = 127,
} ShellStatus;

void eval_loop(Context* ctx);
