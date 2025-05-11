#include <utils/tty.h>

#include <context.h>
#include <stdio.h>
#include <termio.h>
#include <unistd.h>

int disable_echoctl(Context* ctx) {
  struct termios term;

  if (tcgetattr(STDIN_FILENO, &term) == -1) {
    fprintf(stderr, "%s: error getting terminal attributes\n", ctx->argv[0]);
    return 1;
  }
  term.c_lflag &= (unsigned int)~ECHOCTL;
  if (tcsetattr(STDIN_FILENO, TCSANOW, &term) == -1) {
    fprintf(stderr, "%s: error setting terminal attributes\n", ctx->argv[0]);
    return 1;
  }
  return 0;
}
