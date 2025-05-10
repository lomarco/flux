#pragma once

#include <context.h>
#include <signal.h>

typedef struct {
  int read_fd;
  int write_fd;
  volatile sig_atomic_t pending;
} SignalState;

SignalState sig_state;

typedef void (*signal_handler_t)(int);

int signals_init(Context* ctx);

int signal_set_handler(int signum, signal_handler_t handler);

void signals_process_pending(Context* ctx);

void signals_restore_defaults(void);
