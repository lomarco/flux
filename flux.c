#include "flux.h"
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

typedef int (*builtin_func)(Context* ctx, char** args);

builtin_command builtins[] = {
    {"cd", builtin_cd},
    {"exit", builtin_exit},
};

int num_builtins = sizeof(builtins) / sizeof(builtin_command);

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

const char* PROMPT = "> ";

void __sigint_handler(int sig) {
  (void)sig;  // Fix -Wconversion warn
  write(STDOUT_FILENO, "\n", 1);
  write(STDOUT_FILENO, PROMPT, strlen(PROMPT));
}

void __sigtstp_handler(int sig) {
  (void)sig;
  write(STDOUT_FILENO, "\n", 1);
  write(STDOUT_FILENO, PROMPT, strlen(PROMPT));
}

void __sigquit_handler(int sig) {
  (void)sig;
  signal(SIGQUIT, SIG_DFL);
  raise(SIGQUIT);
}

void __sigterm_handler(int sig) {
  (void)sig;
  write(STDOUT_FILENO, "\nReceived SIGTERM, exiting...\n", 30);
  exit(0);
}

void __sighup_handler(int sig) {
  (void)sig;
  write(STDOUT_FILENO, "\nReceived SIGHUP, exiting...\n", 28);
  exit(0);
}

void __sigcont_handler(int sig) {
  (void)sig;
  write(STDOUT_FILENO, "\n", 1);
  write(STDOUT_FILENO, PROMPT, strlen(PROMPT));
}

int builtin_exit(Context* ctx, char** args) {
  free_context(ctx);
  exit(0);
  return 0;
}

int builtin_cd(Context* ctx, char** args) {
  if (args[1] == NULL) {
    fprintf(stderr, "cd: expected argument\n");
    return 1;
  }
  if (chdir(args[1]) != 0) {
    fprintf(stderr, "cd");
    return 1;
  }
  return 1;
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
  for (int i = 0; i < num_builtins; ++i) {
    if (strcmp(args[0], builtins[i].name) == 0) {
      return builtins[i].func(ctx, args);
    }
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

int set_signal_handler(Context* ctx, int signum, void (*handler)(int)) {
  struct sigaction sa;
  memset(&sa, 0, sizeof(sa));
  sa.sa_handler = handler;
  sigemptyset(&sa.sa_mask);
  sa.sa_flags = SA_RESTART;  // Restart syscalls after signal

  if (sigaction(signum, &sa, NULL) == -1) {
    fprintf(stderr, "%s: error: sigaction", ctx->argv[0]);
    return -1;
  }
  return 0;
}

void setup_signal_handlers(Context* ctx) {
  size_t i;
  struct {
    int signum;
    void (*handler)(int);
  } signals[] = {{SIGINT, __sigint_handler},   {SIGTSTP, __sigtstp_handler},
                 {SIGQUIT, __sigquit_handler}, {SIGTERM, __sigterm_handler},
                 {SIGHUP, __sighup_handler},   {SIGCONT, __sigcont_handler}};

  size_t count = sizeof(signals) / sizeof(signals[0]);

  for (i = 0; i < count; ++i) {
    if (set_signal_handler(ctx, signals[i].signum, signals[i].handler) == -1) {
      fprintf(stderr, "Failed to set handler for signal %d\n",
              signals[i].signum);
    }
  }
}

int main(int argc, char* argv[]) {
  Context* ctx = _create_context(argc, argv);

  DEBUG_PRINT("Debug mode enabled\n", __FILE__, __LINE__);

  setup_signal_handlers(ctx);

  disable_echoctl(ctx);

  command_loop(ctx);

  free_context(ctx);

  return 0;
}
