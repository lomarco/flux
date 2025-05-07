#include "flux.h"
#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <termios.h>
#include <unistd.h>

#define RL_BUFSIZE 1024
#define LEX_BUFSIZE 64
#define LEX_DELIM " \t\n"

typedef enum {
  SHELL_EXIT = -1,
  SHELL_OK = 0,
  SHELL_ERROR = 1,
  SHELL_COMAND_NOT_FOUND = 127,
} ShellStatus;

typedef int (*builtin_func)(BuiltinArgs* args);

typedef struct {
  const char* name;
  builtin_func func;
} builtin_command;

builtin_command builtins[] = {
    {"cd", builtin_cd},
    {"exit", builtin_exit},
};

int num_builtins = sizeof(builtins) / sizeof(builtin_command);

static int signal_pipe[2];
extern char** environ;

#ifdef DEBUG
#define DEBUG_PRINT(fmt, ...)                                                  \
  do {                                                                         \
    fprintf(stderr, "DEBUG [%s:%d]: " fmt, __FILE__, __LINE__, ##__VA_ARGS__); \
  } while (0)
#else
#define DEBUG_PRINT(fmt, ...) \
  do {                        \
  } while (0)
#endif

const char* PROMPT = "> ";

void print_prompt(const Context* ctx) {
  char prompt[256];
  int len;

  len = snprintf(prompt, sizeof(prompt), "[%d] %s", get_exit_code(ctx), PROMPT);

  if (len > 0) {
    write(STDOUT_FILENO, prompt, (size_t)len);
  }
}

void handler_sigint(int sig) {
  int sig_code = sig;
  write(signal_pipe[1], &sig_code, 1);
}

void handler_sigtstp(int sig) {
  int sig_code = sig;
  write(signal_pipe[1], &sig_code, 1);
}

void handler_sigterm(int sig) {
  (void)sig;
  const char msg[] = "\nReceived SIGTERM, exiting...\n";
  write(STDOUT_FILENO, msg, sizeof(msg) - 1);
  exit(0);
}

void handler_sighup(int sig) {
  (void)sig;
  const char msg[] = "\nReceived SIGHUP, exiting...\n";
  write(STDOUT_FILENO, msg, sizeof(msg) - 1);
  exit(0);
}

void set_exit_code(Context* ctx, int code) {
  ctx->last_exit_code = code & 0xFF;  // Limit 8 bits (like bash)
}

int get_exit_code(const Context* ctx) {
  return ctx->last_exit_code;
}

void handler_sigcont(int sig) {
  int sig_code = sig;
  write(signal_pipe[1], &sig_code, 1);
}

void env_init(Context* ctx) {
  char** env;
  for (env = environ; *env; env++) {
    env_add(ctx, *env);
  }
}

void env_add(Context* ctx, const char* key_value) {  // FIXME: optimised realloc
  ctx->env_vars = realloc(ctx->env_vars, (ctx->env_size + 2) * sizeof(char*));
  ctx->env_vars[ctx->env_size++] = strdup(key_value);
  ctx->env_vars[ctx->env_size] = NULL;
}

char* env_get(Context* ctx, const char* key) {  // FIXME: Add validation
  size_t key_len = strlen(key);
  size_t i;

  for (i = 0; i < ctx->env_size; ++i) {
    if (strncmp(ctx->env_vars[i], key, key_len) == 0 &&
        ctx->env_vars[i][key_len] == '=') {
      return ctx->env_vars[i] + key_len + 1;
    }
  }
  return NULL;
}

void env_set(Context* ctx, const char* key_value) {  // TODO
  ;
}

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

int launch_commands(Context* ctx, char** args) {
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

int shell_execute(Context* ctx, int argc, char** argv) {
  int i, res_code;

  if (argc == 0 || argv[0] == NULL) {
    return SHELL_ERROR;
  }
  for (i = 0; i < num_builtins; ++i) {
    if (strcmp(argv[0], builtins[i].name) == 0) {
      BuiltinArgs args = {ctx, argc, argv};
      res_code = builtins[i].func(&args);
      return res_code;
    }
  }
  res_code = launch_commands(ctx, argv);
  return res_code;
}

int count_args(char** args) {
  int count;

  count = 0;
  while (args[count] != NULL) {
    ++count;
  }
  return count;
}

char** lex_line(Context* ctx, char* line) {
  int bufsize = LEX_BUFSIZE;
  int position = 0;
  char* token;
  char** tokens = (char**)malloc((size_t)bufsize * sizeof(char*));
  char** new_tokens;
  char* saveptr;

  if (!tokens) {
    fprintf(stderr, "%s: allocation error\n", ctx->argv[0]);
    exit(1);
  }
  token = strtok_r(line, LEX_DELIM, &saveptr);
  while (token != NULL) {
    tokens[position++] = token;
    if (position >= (int)bufsize) {
      bufsize *= 2;
      new_tokens = (char**)realloc(tokens, (size_t)bufsize * sizeof(char*));
      if (!new_tokens) {
        free(tokens);
        fprintf(stderr, "%s: reallocation error\n", ctx->argv[0]);
        exit(1);
      }
      tokens = new_tokens;
    }
    token = strtok_r(NULL, LEX_DELIM, &saveptr);
  }
  tokens[position] = NULL;
  return tokens;
}

void command_loop(Context* ctx) {
  char* line = NULL;
  size_t linecap = 0;
  ssize_t linelen;
  int status = 0;
  int ret;
  char sig_code;

  if (pipe(signal_pipe) == -1) {
    perror("pipe");
  }
  while (status != SHELL_EXIT) {
    print_prompt(ctx);

    // Preparation for input or signal
    fd_set readfds;
    FD_ZERO(&readfds);
    FD_SET(STDIN_FILENO, &readfds);
    FD_SET(signal_pipe[0], &readfds);

    int maxfd = (STDIN_FILENO > signal_pipe[0]) ? STDIN_FILENO : signal_pipe[0];

    ret = select(maxfd + 1, &readfds, NULL, NULL, NULL);
    if (ret < 0) {
      if (errno == EINTR) {
        // putchar('\n');  // FIXME: Rewrite me because i am continue iter
        // continue;
      } else {
        perror("select");
        break;
      }
    }

    if (FD_ISSET(signal_pipe[0], &readfds)) {
      sig_code = 0;
      while (read(signal_pipe[0], &sig_code, 1) == 1) {
        switch (sig_code) {
          case SIGINT:
            DEBUG_PRINT("INT\n");
            write(STDOUT_FILENO, "^C\n", 3);
            break;
          case SIGTSTP:
            DEBUG_PRINT("STP\n");
            write(STDOUT_FILENO, "\n", 1);
            break;
          default:
            break;
        }
        break;
      }
      continue;
    }

    // User input
    if (FD_ISSET(STDIN_FILENO, &readfds)) {
      linelen = getline(&line, &linecap, stdin);
      if (linelen == -1) {
        putchar('\n');
        break;
      }
      if (linelen > 0 && line[linelen - 1] == '\n') {
        line[linelen - 1] = '\0';
      }

      char** args = lex_line(ctx, line);
      int argsc = count_args(args);
      status = shell_execute(ctx, argsc, args);

      free(args);
    }
  }
  free(line);
}

Context* init_context(int argc, char* argv[]) {
  int i, j;

  Context* ctx = (Context*)malloc(sizeof(Context));
  if (!ctx) {
    fprintf(stderr, "%s: error allocating Context\n", argv[0]);
    return NULL;
  }

  ctx->env_size = 0;
  ctx->env_vars = NULL;
  ctx->last_exit_code = 0;
  ctx->argc = argc;

  ctx->argv = (char**)malloc((size_t)argc * sizeof(char*));
  if (!ctx->argv) {
    fprintf(stderr, "%s: error allocating argv array\n", argv[0]);
    free(ctx);
    return NULL;
  }
  for (i = 0; i < argc; ++i) {
    ctx->argv[i] = strdup(argv[i]);
    if (!ctx->argv[i]) {
      fprintf(stderr, "%s: error duplicating argv[%d]\n", argv[0], i);
      for (j = 0; j < i; ++j)
        free(ctx->argv[j]);
      free(ctx->argv);
      free(ctx);
      return NULL;
    }
  }

  ctx->argv[argc] = NULL;

  env_init(ctx);
  ctx->last_exit_code = SHELL_OK;
  return ctx;
}

void free_context(Context* ctx) {
  int i;

  if (!ctx)
    return;
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
    fprintf(stderr, "%s: error: sigaction\n", ctx->argv[0]);
    return -1;
  }
  return 0;
}

void setup_signal_handlers(Context* ctx) {
  size_t i;
  struct {
    int signum;
    void (*handler)(int);
  } signals[] = {{SIGINT, handler_sigint},
                 {SIGTSTP, handler_sigtstp},
                 {SIGTERM, handler_sigterm},
                 {SIGHUP, handler_sighup},
                 {SIGCONT, handler_sigcont}};

  size_t count = sizeof(signals) / sizeof(signals[0]);

  for (i = 0; i < count; ++i) {
    if (set_signal_handler(ctx, signals[i].signum, signals[i].handler) == -1) {
      fprintf(stderr, "Failed to set handler for signal %d\n",
              signals[i].signum);
    }
  }
}

int main(int argc, char* argv[]) {
  Context* ctx = init_context(argc, argv);
  if (!ctx) {
    return 1;
  }

  DEBUG_PRINT("Debug mode enabled\n\n");

  setup_signal_handlers(ctx);

  disable_echoctl(ctx);

  command_loop(ctx);

  free_context(ctx);

  return 0;
}
