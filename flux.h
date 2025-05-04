#pragma once

#include <stddef.h>

typedef struct {
  int argc;
  char** argv;
} Context;

typedef struct {
  Context* ctx;
  int argc;
  char** argv;
} BuiltinArgs;

extern const char* PROMPT;

// Signal handlers
void handler_sigint(int sig);
void handler_sigtstp(int sig);
void handler_sigterm(int sig);
void handler_sighup(int sig);
void handler_sigcont(int sig);

// Built-in commands
int builtin_exit(BuiltinArgs* args);
int builtin_cd(BuiltinArgs* args);

// Launch external commands
int launch_commands(Context* ctx, char** args);

// Execute a command (built-in or external)
int shell_execute(Context* ctx, int argc, char** args);

// Count arguments
int count_args(char** args);

// Lexical parsing of a line into arguments
char** lex_line(Context* ctx, char* line);

// Read a line from input
char* read_line(Context* ctx);

// Main shell command loop
void command_loop(Context* ctx);

// Create and free context
Context* create_context(int argc, char* argv[]);
void free_context(Context* ctx);

// Disable echoing of control characters in the terminal
void disable_echoctl(Context* ctx);

// Set a signal handler
int set_signal_handler(Context* ctx, int signum, void (*handler)(int));

// Setup all signal handlers
void setup_signal_handlers(Context* ctx);
