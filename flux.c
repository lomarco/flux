#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <termios.h>
#include <unistd.h>

#define MAX_LINE 1024
#define MAX_ARGS 64
#define RL_BUFSIZE 1024
#define LEX_BUFSIZE 64
#define LEX_DELIM " \t\n"

#ifdef DEBUG
#define DEBUG_PRINT(fmt, ...)                                          \
  do {                                                                 \
    fprintf(stderr, "DEBUG [%s:%d]: " fmt __VA_OPT__(, ) __VA_ARGS__); \
  } while (0)
#else
#define DEBUG_PRINT(fmt, ...) \
  do {                        \
  } while (0)
#endif

typedef struct {
  int argc;
  char** argv;
} Context;

const char* PROMPT = "> ";

void __sigint_handler(int sig) {
  write(STDOUT_FILENO, "\n", 1);
  write(STDOUT_FILENO, PROMPT, strlen(PROMPT));
}

void __sigtstp_handler(int sig) {
  write(STDOUT_FILENO, "\n", 1);
  write(STDOUT_FILENO, PROMPT, strlen(PROMPT));
}

void __sigquit_handler(int sig) {
  signal(SIGQUIT, SIG_DFL);
  raise(SIGQUIT);
}

void __sigterm_handler(int sig) {
  write(STDOUT_FILENO, "\nReceived SIGTERM, exiting...\n", 30);
  exit(0);
}

void __sighup_handler(int sig) {
  write(STDOUT_FILENO, "\nReceived SIGHUP, exiting...\n", 28);
  exit(0);
}

void __sigcont_handler(int sig) {
  write(STDOUT_FILENO, "\n", 1);
  write(STDOUT_FILENO, PROMPT, strlen(PROMPT));
}

int __launch_commands(Context* ctx, char** args) {
  pid_t pid;
  int status;

  pid = fork();
  if (pid == 0) {
    signal(SIGINT, SIG_DFL);

    if (execvp(args[0], args) == -1) {
      fprintf(stderr, "%s: command not found: %s\n", ctx->argv[0], args[0]);
    }
    exit(1);
  } else if (pid < 0) {
    fprintf(stderr, "%s: error pid\n", ctx->argv[0]);
  } else {
    do {
      waitpid(pid, &status, WUNTRACED);
    } while (!WIFEXITED(status) && !WIFSIGNALED(status));
  }
  return 1;
}

int _shell_execute(Context* ctx, char** args) {
  if (args[0] == NULL) {
    return 1;
  }
  // TODO binutils (cd, exit, q, ...)
  return __launch_commands(ctx, args);
}

char** _lex_line(Context* ctx, char* line) {
  int bufsize = LEX_BUFSIZE;
  int position = 0;
  char* token;
  char** tokens = (char**)malloc((size_t)bufsize * sizeof(char*));

  if (!tokens) {
    fprintf(stderr, "%s: error lex_line malloced\n", ctx->argv[0]);
    exit(1);
  }
  token = strtok(line, LEX_DELIM);
  while (token != NULL) {
    tokens[position] = token;
    ++position;

    if (position >= (int)bufsize) {
      bufsize += LEX_BUFSIZE;
      tokens = (char**)realloc(tokens, (size_t)bufsize * sizeof(char*));
      if (!tokens) {
        fprintf(stderr, "%s: error lex_line realloced\n", ctx->argv[0]);
        exit(1);
      }
    }
    token = strtok(NULL, LEX_DELIM);
  }
  tokens[position] = NULL;
  return tokens;
}

char* _read_line(Context* ctx) {
  char* buffer = (char*)malloc(sizeof(char) * RL_BUFSIZE);
  char c;
  int position;
  int bufsize = RL_BUFSIZE;

  if (!buffer) {
    fprintf(stderr, "%s: error read_line malloced\n", ctx->argv[0]);
    exit(1);
  }
  position = 0;
  while (1) {
    c = (char)getchar();
    if (c == '\n') {
      buffer[position] = '\0';
      return buffer;
    } else if (c == EOF) {
      exit(1);
    } else {
      buffer[position] = c;
    }
    position++;

    if (position >= bufsize) {
      bufsize += RL_BUFSIZE;
      buffer = (char*)realloc(buffer, (size_t)bufsize);
      if (!buffer) {
        fprintf(stderr, "%s: error read_line realloced\n", ctx->argv[0]);
        exit(1);
      }
    }
  }
}

void command_loop(Context* ctx) {
  char* line;
  char** args;
  int status;

  do {
    printf("%s", PROMPT);
    fflush(stdout);

    line = _read_line(ctx);
    args = _lex_line(ctx, line);
    status = _shell_execute(ctx, args);

    free(line);
    free(args);
  } while (status);
}

Context* _create_context(int argc, char* argv[]) {
  int i;

  Context* ctx = (Context*)malloc(sizeof(Context));
  ctx->argc = argc;
  ctx->argv = malloc((size_t)argc * sizeof(char*));
  for (i = 0; i < argc; ++i) {
    ctx->argv[i] = strdup(argv[i]);
  }
  return ctx;
}

void free_context(Context* ctx) {
  int i;
  for (i = 0; i < ctx->argc; ++i) {
    free(ctx->argv[i]);
  }
  free(ctx->argv);
  free(ctx);
}

void disable_echoctl(Context* ctx) {
  struct termios term;

  if (tcgetattr(STDIN_FILENO, &term) == -1) {
    fprintf(stderr, "%s: error getting terminal attributes\n", ctx->argv[0]);
    exit(1);
  }
  term.c_lflag &= (unsigned int)~ECHOCTL;
  if (tcsetattr(STDIN_FILENO, TCSANOW, &term) == -1) {
    fprintf(stderr, "%s: error setting terminal attributes\n", ctx->argv[0]);
    exit(1);
  }
}

int main(int argc, char* argv[]) {
  Context* ctx = _create_context(argc, argv);

  signal(SIGINT, __sigint_handler);
  signal(SIGTSTP, __sigtstp_handler);
  signal(SIGQUIT, __sigquit_handler);

  DEBUG_PRINT("Debug mode enabled\n", __FILE__, __LINE__);

  disable_echoctl(ctx);

  command_loop(ctx);

  free_context(ctx);

  return 0;
}
