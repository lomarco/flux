#pragma once

#include <stddef.h>

typedef struct {
  int argc;
  char** argv;
} Context;

extern const char* PROMPT;

// Signal handlers
void __sigint_handler(int sig);
void __sigtstp_handler(int sig);
void __sigterm_handler(int sig);
void __sighup_handler(int sig);
void __sigcont_handler(int sig);

// Built-in commands
int builtin_exit(Context* ctx, int argc, char** args);
int builtin_cd(Context* ctx, int argc, char** args);

// Launch external commands
int __launch_commands(Context* ctx, char** args);

// Execute a command (built-in or external)
int _shell_execute(Context* ctx, int argc, char** args);

// Count arguments
int count_args(Context* ctx, char** args);

// Lexical parsing of a line into arguments
char** _lex_line(Context* ctx, char* line);

// Read a line from input
char* _read_line(Context* ctx);

// Main shell command loop
void command_loop(Context* ctx);

// Create and free context
Context* _create_context(int argc, char* argv[]);
void free_context(Context* ctx);

// Disable echoing of control characters in the terminal
void disable_echoctl(Context* ctx);

// Set a signal handler
int set_signal_handler(Context* ctx, int signum, void (*handler)(int));

// Setup all signal handlers
void setup_signal_handlers(Context* ctx);
