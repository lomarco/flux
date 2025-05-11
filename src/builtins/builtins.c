#include <builtins/builtins.h>
#include <context.h>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

builtin_command builtins[] = {
    {"cd", builtin_cd},
    {"exit", builtin_exit},
};

int num_builtins =
    sizeof(builtins) /
    sizeof(builtin_command);  // Move this lines to cd.c, exit.c ...

int builtin_exit(BuiltinArgs* args) {
  long code = 0;

  if (args->argc >= 2) {
    char* endptr;
    code = strtol(args->argv[1], &endptr, 10);
    if (*endptr != '\0') {
      fprintf(stderr, "exit: numeric argument required\n");
      set_exit_code(args->ctx, SHELL_ERROR);
      return SHELL_ERROR;
    }
  }
  free_context(args->ctx);
  code = code & 0xFF;
  exit((int)code);
}

int builtin_cd(BuiltinArgs* args) {
  if (args->argv[1] == NULL) {
    fprintf(stderr, "cd: expected argument\n");  // <<<< $HOME getenv
    set_exit_code(args->ctx, SHELL_ERROR);
    return SHELL_ERROR;
  }
  if (chdir(args->argv[1]) != 0) {
    fprintf(stderr, "cd: no such file or directory: %s\n", args->argv[1]);
    set_exit_code(args->ctx, SHELL_ERROR);
    return SHELL_ERROR;
  }
  set_exit_code(args->ctx, SHELL_OK);
  return SHELL_OK;
}
