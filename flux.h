#pragma once

#include <stddef.h>  // для size_t

typedef struct {
  int argc;
  char** argv;
} Context;

typedef int (*builtin_func)(Context* ctx, char** args);

typedef struct {
  const char* name;
  builtin_func func;
} builtin_command;

int builtin_cd(Context* ctx, char** args);
int builtin_exit(Context* ctx, char** args);

int __launch_commands(Context* ctx, char** args);
int _shell_execute(Context* ctx, char** args);
char** _lex_line(Context* ctx, char* line);
char* _read_line(Context* ctx);
void command_loop(Context* ctx);
Context* _create_context(int argc, char* argv[]);
void free_context(Context* ctx);
void disable_echoctl(Context* ctx);
int set_signal_handler(Context* ctx, int signum, void (*handler)(int));
void setup_signal_handlers(Context* ctx);

void __sigint_handler(int sig);
void __sigtstp_handler(int sig);
void __sigquit_handler(int sig);
void __sigterm_handler(int sig);
void __sighup_handler(int sig);
void __sigcont_handler(int sig);
