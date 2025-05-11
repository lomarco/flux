#include <builtins/builtins.h>
#include <context.h>
#include <core/eval.h>
#include <core/exec.h>

#include <stdio.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

static int launch_commands(Context* ctx, char** args) {
  pid_t pid;
  int status;
  int i;

  pid = fork();
  if (pid == 0) {
    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = SIG_DFL;
    for (i = 1; i < NSIG; ++i) {
      sigaction(i, &sa, NULL);
    }

    if (execve(args[0], args, ctx->env_vars) == -1) {
      fprintf(stderr, "%s: command not found: %s\n", ctx->argv[0], args[0]);
      set_exit_code(ctx, SHELL_ERROR);
    }
    _exit(SHELL_COMAND_NOT_FOUND);
  } else if (pid < 0) {
    fprintf(stderr, "%s: error pid\n", ctx->argv[0]);
  } else {
    do {
      waitpid(pid, &status, WUNTRACED);
    } while (!WIFEXITED(status) && !WIFSIGNALED(status));
    if (WIFEXITED(status)) {
      int exit_code = WEXITSTATUS(status);
      set_exit_code(ctx, exit_code);
    } else if (WIFSIGNALED(status)) {
      int signal_num = WTERMSIG(status);
      set_exit_code(ctx, 128 + signal_num);  // Error code by signal 128+signum
    } else {
      set_exit_code(ctx, SHELL_ERROR);
    }
  }
  return SHELL_OK;
}

int execute_command(Context* ctx, int args_count, char** args_arr) {
  int i, res_code;

  if (args_count == 0 || args_arr[0] == NULL) {
    return SHELL_ERROR;
  }
  for (i = 0; i < num_builtins; ++i) {
    if (strcmp(args_arr[0], builtins[i].name) == 0) {
      BuiltinArgs args = {ctx, args_count, args_arr};
      res_code = builtins[i].func(&args);
      return res_code;
    }
  }
  res_code = launch_commands(ctx, args_arr);
  return res_code;
}
