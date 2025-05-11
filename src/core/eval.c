#include <context.h>
#include <core/eval.h>
#include <core/exec.h>    // < shell_execute
#include <core/lexer.h>   // < break the text into tokens
#include <core/parser.h>  // TODO: Building ast to convey it to exec
#include <prompt.h>
#include <signals.h>

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <termios.h>
#include <unistd.h>

void eval_loop(Context* ctx) {
  char* line = NULL;
  size_t linecap = 0;
  ssize_t linelen;
  int shell_status = 0;
  int ret;
  char** tokens_array;

  if (signals_init(ctx) == -1) {
    fprintf(stderr, "%s: Failed to initialize signals\n", ctx->argv[0]);
    return;
  }

  while (shell_status != SHELL_EXIT) {
    print_prompt(ctx);

    fd_set readfds;
    FD_ZERO(&readfds);
    FD_SET(STDIN_FILENO, &readfds);

    int maxfd = STDIN_FILENO;

    if (sig_state.read_fd != -1) {
      FD_SET(sig_state.read_fd, &readfds);
      if (sig_state.read_fd > maxfd) {
        maxfd = sig_state.read_fd;
      }
    }

    ret = select(maxfd + 1, &readfds, NULL, NULL, NULL);
    if (ret < 0) {
      if (errno == EINTR) {
        signals_process_pending(ctx);
        continue;
      } else {
        perror("select");
        break;
      }
    }

    if (sig_state.read_fd != -1 && FD_ISSET(sig_state.read_fd, &readfds)) {
      signals_process_pending(ctx);
      continue;
    }

    if (FD_ISSET(STDIN_FILENO, &readfds)) {
      linelen = getline(&line, &linecap, stdin);
      if (linelen == -1) {
        putchar('\n');
        break;
      }
      if (linelen > 0 && line[linelen - 1] == '\n') {
        line[linelen - 1] = '\0';
      }

      tokens_array = lex_line(ctx, line);
      int argsc = count_args(tokens_array);
      shell_status = execute_command(ctx, argsc, tokens_array);

      free(tokens_array);
    }
  }

  free(line);
  signals_restore_defaults();
}
