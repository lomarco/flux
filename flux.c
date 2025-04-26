#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>

#define MAX_LINE 1024
#define MAX_ARGS 64
#define RL_BUFSIZE 1024
#define LEX_BUFSIZE 64
#define LEX_DELIM " \t\n"

typedef struct
{
  int argc;
  char** argv;
} Context;

const char* PROMT = "> ";

void
__sigint_handler(int sig)
{
  write(STDOUT_FILENO, "\n", 1);
  write(STDOUT_FILENO, PROMT, strlen(PROMT));
}

void
__sigtstp_handler(int sig)
{
  printf("SIGTSTP");
  kill(0, SIGTSTP);
}

void
__sigquit_handler(int sig)
{
  printf("SIGQUIT");
  fflush(stdout);
}

char**
_lex_line(char* line)
{
  int bufsize = LEX_BUFSIZE;
  int position = 0;
  char* token;
  char** tokens = malloc(bufsize * sizeof(char*));

  if (!tokens) {
    fprintf(stderr, "flux: error lex_line malloced");
    exit(1);
  }
  token = strtok(line, LEX_DELIM);
  while (token != NULL) {
    tokens[position] = token;
    ++position;

    if (position >= bufsize) {
      bufsize += LEX_BUFSIZE;
      tokens = realloc(tokens, bufsize * sizeof(char*));
      if (!tokens) {
        fprintf(stderr, "flux: error lex_line realloced\n");
        exit(1);
      }
    }
    token = strtok(NULL, LEX_DELIM);
  }
  tokens[position] = NULL;
  return tokens;
}

char*
_read_line(void)
{
  char* buffer = malloc(sizeof(char) * RL_BUFSIZE);
  int c;
  int position;
  int bufsize = RL_BUFSIZE;

  if (!buffer) {
    fprintf(stderr, "lxe: error read_line malloced");
    exit(1);
  }
  position = 0;
  while (1) {
    if ((c = getchar()) == EOF || c == '\n') {
      buffer[position] = '\0';
      return buffer;
    } else {
      buffer[position] = c;
    }
    position++;

    if (position >= bufsize) {
      bufsize += RL_BUFSIZE;
      buffer = realloc(buffer, bufsize);
      if (!buffer) {
        fprintf(stderr, "lxe: error read_line realloced");
        exit(1);
      }
    }
  }
}

void
command_loop(void)
{
  char* line;
  char** args;
  int status;

  do {
    printf("%s", PROMT);
    fflush(stdout);

    line = _read_line();
    args = _lex_line(line);
    status = 1;

    free(line);
    free(args);
  } while (status);
}

Context*
_create_context(int argc, char* argv[])
{
  int i;

  Context* ctx = malloc(sizeof(Context));
  ctx->argc = argc;
  ctx->argv = malloc(argc * sizeof(char*));
  for (i = 0; i < argc; ++i) {
    ctx->argv[i] = strdup(argv[i]);
  }
  return ctx;
}

void
_free_context(Context* ctx)
{
  int i;
  for (i = 0; i < ctx->argc; ++i) {
    free(ctx->argv[i]);
  }
  free(ctx->argv);
  free(ctx);
}

void
_disable_echoctl(void)
{
  struct termios term;

  if (tcgetattr(STDIN_FILENO, &term) == -1) {
    fprintf(stderr, "flux: error getting terminal attributes");
    exit(1);
  }
  term.c_lflag &= ~ECHOCTL;
  if (tcsetattr(STDIN_FILENO, TCSANOW, &term) == -1) {
    fprintf(stderr, "flux: error setting terminal attributes");
    exit(1);
  }
}

int
main(int argc, char* argv[])
{
  Context* ctx = _create_context(argc, argv);

  signal(SIGINT, __sigint_handler);
  signal(SIGTSTP, __sigtstp_handler);
  signal(SIGQUIT, __sigquit_handler);

  _disable_echoctl();

  command_loop();

  _free_context(ctx);

  return 0;
}
