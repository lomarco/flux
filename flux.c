#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>

#define MAX_LINE 1024
#define MAX_ARGS 64
#define RL_BUFSIZE 1024
#define TOK_BUFSIZE 64
#define TOK_DELIM " \t\n"

typedef struct
{
  int argc;
  char** argv;
} Context;

void
__sigint_handler(int sig)
{
  printf("SIGINT");
  fflush(stdout);
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
  // printf("SIGQUIT");
  // fflush(stdout);
}

char**
_lex_line(char* line)
{
  int bufsize = TOK_BUFSIZE;
  int position = 0;
  char* token;
  char** tokens = malloc(bufsize * sizeof(char*));

  if (!tokens) {
    fprintf(stderr, "TODO");
    exit(1);
  }
  token = strtok(line, TOK_DELIM);
  while (token != NULL) {
    tokens[position] = token;
    ++position;

    if (position >= bufsize) {
      bufsize += TOK_BUFSIZE;
      tokens = realloc(tokens, bufsize * sizeof(char*));
      if (!tokens) {
        fprintf(stderr, "TODO\n");
        exit(1);
      }
    }
    token = strtok(NULL, TOK_DELIM);
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
    fprintf(stderr, "lxe: error malloced");
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
        fprintf(stderr, "lxe: error malloced");
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
    printf("> ");
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
_disable_echoctl()
{
  struct termios term;

  if (tcgetattr(STDIN_FILENO, &term) == -1) {
    fprintf(stderr, "TODO");
    exit(1);
  }
  term.c_lflag &= ~ECHOCTL;
  if (tcsetattr(STDIN_FILENO, TCSANOW, &term) == -1) {
    fprintf(stderr, "TODO");
    exit(1);
  }
}

int
main(int argc, char* argv[])
{
  Context* ctx = _create_context(argc, argv);

  signal(SIGINT, __sigint_handler);
  signal(SIGINT, __sigtstp_handler);
  signal(SIGINT, __sigquit_handler);

  _disable_echoctl();

  command_loop();

  _free_context(ctx);

  return 0;
}
